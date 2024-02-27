/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 03 - Feb 15 2024

	Summary:

	Version Summary:
        v01: Press button in column to toggle 0-3 binary on leds
        v02: Working LED Output, defined keypad macros, working column readout
        v03: Working keypad password. Working Pattern B. 
        v04: Added lock button. Working Pattern D. Working Keypad reset

    Ports Map: 
        P3.1, 3.5, 1.1, 5.4, 1.4, 5.3, 5.1, 5.0 - Keyboard P1-8
        P3.0, 2.5, 4.4, 4.7, 4.6, 4.0, 2.2, 2.0 - LEDs 0-7

	Important Variables/Registers:
	    Button 0x76543210
            0-3 Col
            4-7 Row
        LED_Out
            1:1 LED

    Keypad: (Pins L->R)
            P0   P1   P2   P3
        P4 |1|  |2|  |3|  |A| P4.6
        P5 |4|  |5|  |6|  |B| P4.0
        P6 |7|  |8|  |9|  |C| P2.2
        P7 |*|  |0|  |#|  |D| P2.0
          P3.0 P2.5 P4.4 P4.7
    
    MSP Errors:
        P1.3 Always High
        Row P3.2-2.1 mostly not working?
        P4.5 Inconsistent Input?
	Todo:
		
-----------------------------------------------------------------------------------------------------------------------*/

#include "gpio.h"
#include "msp430fr2355.h"
#include "stdbool.h"
#include "sys/_stdint.h"
#include <msp430.h> 
#include <driverlib.h>
#include <stdint.h>

// Defining row and column input as keys
#define KEY_1 0x011
#define KEY_2 0x012
#define KEY_3 0x014
#define KEY_4 0x021
#define KEY_5 0x022
#define KEY_6 0x024
#define KEY_7 0x041
#define KEY_8 0x042
#define KEY_9 0x044
#define KEY_0 0x082
#define KEY_POUND 0x084
#define KEY_AST 0x081
#define KEY_A 0x018
#define KEY_B 0x028
#define KEY_C 0x048
#define KEY_D 0x088

// Making ports and pins easier to reference
#define Port1 GPIO_PORT_P1  
#define Port2 GPIO_PORT_P2
#define Port3 GPIO_PORT_P3  
#define Port4 GPIO_PORT_P4
#define Port5 GPIO_PORT_P5 

#define P0 GPIO_PIN0
#define P1 GPIO_PIN1
#define P2 GPIO_PIN2
#define P3 GPIO_PIN3
#define P4 GPIO_PIN4
#define P5 GPIO_PIN5
#define P6 GPIO_PIN6
#define P7 GPIO_PIN7

void CheckCol();
void CheckRow();
void PatternAUpdate();
void PatternBUpdate();
void PatternCUpdate();
void PatternDUpdate();
void PatternErrorUpdate();
void ButtonResponse();
void UpdateLED();

uint8_t Button = 0, LED_Out = 0, Pattern = 0;
uint8_t PatternBCounter = 0;
uint8_t PatternDCounter = 0;
uint8_t PatternECounter = 0;
uint8_t State = 0; // 0 - wait for key, 1-3 - correct button pressed for password, 
uint8_t LastButton = 0; // tracker of last button pressed
uint8_t const Passcode[] = {KEY_1, KEY_2, KEY_3};
bool CheckFlag = false;
bool TimerFlag = false;
int test; // arbitrary test register

int main(void) {
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Initialize LED and keyboard
    LedLow();
    RowInput();

    // Clear interrupt flag bits
    P2IFG &= ~BIT0; 
    P2IFG &= ~BIT2;
    P4IFG &= ~BIT0;
    P4IFG &= ~BIT6;
    // Rising edge sensitivity
    P2IES &= ~BIT0;
    P2IES &= ~BIT2;
    P4IES &= ~BIT0;
    P4IES &= ~BIT6;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

    // Interrupt enable
    P2IE |= BIT0;
    P2IE |= BIT2;
    P4IE |= BIT0;
    P4IE |= BIT6;

    __enable_interrupt();

    while(1) {
        if (CheckFlag) {
            CheckCol();
            CheckRow();
            ButtonResponse();
            CheckFlag = false;
            State++;
        }
        switch(State) {
            default: // waiting for keypress / Run pattern based on current value 
                if (TimerFlag) {
                    switch (Pattern) { 
                        case 1: PatternA(); break;
                        case 2: PatternBUpdate(); break;
                        case 3: PatternC(); break;
                        case 4: PatternDUpdate(); break;
                        case 5: PatternErrorUpdate(); break;
                        default: break;
                    }
                    TimerFlag = false;
                }
                break;
            case 1: // respond to keypress
                TB0CTL |= TBSSEL__ACLK + MC__UP + ID__2;
                TB0CCR0 = 32768;
                TB0R = 0;
                TB0CCTL0 |= CCIE;
                TB0CCTL0 &= ~CCIFG;
                if (LastButton == Passcode[0]) {
                    State++;
                }
                else {
                    State=0;
                }
            break;
            case 3: // first key entered
                if (LastButton == Passcode[1]) {
                    State++;
                }
                else {
                    State=0;
                }
            break;
            case 5: // second key entered. If correct progress, otherwise reset
                if (LastButton == Passcode[2]) {
                    State++;
                }
                else {
                    State=0;
                }
            break;
            case 7: // password entered
                // switch pattern based on last button pressed
                switch (LastButton) { 
                    case KEY_A: Pattern = 1; break;
                    case KEY_B: // Reset pattern if already playing, otherwise switch to pattern
                        if (Pattern == 2)
                            PatternBCounter = 0;
                        Pattern = 2;  
                        break;
                    case KEY_C: Pattern = 3;  break;
                    case KEY_D: // Reset pattern if already playing, otherwise switch to pattern D
                        if (Pattern == 4)
                            PatternDCounter = 0;
                        Pattern = 4;  
                        break;
                    // Relock system if * is pressed
                    case KEY_AST: State = 0; Pattern = 0; break;
                    // Otherwise disable pattern
                    default: Pattern = 0; break;
                }
                // Tell LED to update
                TimerFlag = true;
                // Go back to waiting for key press
                State--;
            break;
            
        }
        UpdateLED();
    }
}

//-LED Init Low: Configures LED ports as output and LOW----------------------------------------------
void LedLow(){
    //LED Bar Output initalizing as low
    GPIO_setAsOutputPin(Port3, Pin1);
    GPIO_setOutputLowOnPin(Port3, Pin1);
    GPIO_setAsOutputPin(Port3, Pin5);
    GPIO_setOutputLowOnPin(Port3, Pin5);
    GPIO_setAsOutputPin(Port1, Pin1);
    GPIO_setOutputLowOnPin(Port1, Pin1);
    GPIO_setAsOutputPin(Port5, Pin4);
    GPIO_setOutputLowOnPin(Port5, Pin4);
    GPIO_setAsOutputPin(Port5, Pin0);
    GPIO_setOutputLowOnPin(Port5, Pin0);
    GPIO_setAsOutputPin(Port5, Pin1);
    GPIO_setOutputLowOnPin(Port5, Pin1);
    GPIO_setAsOutputPin(Port5, Pin3);
    GPIO_setOutputLowOnPin(Port5, Pin3);
    GPIO_setAsOutputPin(Port1, Pin4);
    GPIO_setOutputLowOnPin(Port1, Pin4);
}//--END LedLow-------------------------------------------------------------------------------------

//-Delay_init: Setup timer A for 1 second-----------------------------------------------------------
void delay_init(){
    // Configure Timer_A to generate interrupts every 1 second
    TB0CCTL0 = CCIE; 
    TB0CCR0 = 32767; 
    TB0CTL = TBSSEL_1 + MC_1 + TBCLR; 
}//--END Delay_init---------------------------------------------------------------------------------

//-ColumnInput: Sets row pins as OUTPUT---------------------------------------------------------------------
void ColumnInput(){
  
  //Configure outputs on Rows
    GPIO_setAsOutputPin(Port2, GPIO_PIN0);
    GPIO_setAsOutputPin(Port2, GPIO_PIN2);
    GPIO_setAsOutputPin(Port4, GPIO_PIN0);
    GPIO_setAsOutputPin(Port4, GPIO_PIN6);

    //Configure inputs on Collumns
    GPIO_setAsInputPinWithPullDownResistor(Port4, P7);
    GPIO_setAsInputPinWithPullDownResistor(Port4, P4);
    GPIO_setAsInputPinWithPullDownResistor(Port2, P5);
    GPIO_setAsInputPinWithPullDownResistor(Port3, P0);

    //Set High on ROWS
    GPIO_setOutputHighOnPin(Port2, GPIO_PIN0);
    GPIO_setOutputHighOnPin(Port2, GPIO_PIN2);
    GPIO_setOutputHighOnPin(Port4, GPIO_PIN0);
    GPIO_setOutputHighOnPin(Port4, GPIO_PIN6);
}//--END ColumnInput----------------------------------------------------------------------------------------

//-CheckCol: Sets row pins as OUTPUT---------------------------------------------------------------------
void CheckCol() {
    // Set necessary pins as inputs and outputs for reading columns
    ColumnInput();

    // Clear button column values, then toggle bits corresponding to column
    Button &= 0x0F0;
    Button = ((P4IN & BIT7) == BIT7) ? (Button | BIT3) : (Button & ~BIT3);
    Button = ((P4IN & BIT4) == BIT4) ? (Button | BIT2) : (Button & ~BIT2);
    Button = ((P2IN & BIT5) == BIT5) ? (Button | BIT1) : (Button & ~BIT1);
    Button = ((P3IN & BIT0) == BIT0) ? (Button | BIT0) : (Button & ~BIT0);

}//--END CheckCol-------------------------------------------------------------------------------------

//-Collumns: Sets Collumn pins as OUTPUT-------------------------------------------------------------
void RowInput(){

    //Set P0-3 on keyboard as outputs
    GPIO_setAsOutputPin(Port4, P7);
    GPIO_setAsOutputPin(Port4, P4);
    GPIO_setAsOutputPin(Port2, P5);
    GPIO_setAsOutputPin(Port3, P0);

    //Set P4-7 on keyboard as inputs with pull down
    GPIO_setAsInputPinWithPullDownResistor(Port2, P0);
    GPIO_setAsInputPinWithPullDownResistor(Port2, P2);
    GPIO_setAsInputPinWithPullDownResistor(Port4, P0);
    GPIO_setAsInputPinWithPullDownResistor(Port4, P6);

    //Set P0-3 on keyboard as HIGH
    GPIO_setOutputHighOnPin(Port4, P7);
    GPIO_setOutputHighOnPin(Port4, P4);
    GPIO_setOutputHighOnPin(Port2, P5);
    GPIO_setOutputHighOnPin(Port3, P0);

}//--END RowInputs---------------------------------------------------------------------------------

//-CheckRow:  Clear row bits, then toggle bits based on input-----------------------------------------
void CheckRow() {
    // Set necessary pins as inputs and outputs for reading columns
    RowInput();

    // Clear row portion of button, then toggle bits corresponding to row
    Button &= 0x00F;
    Button = ((P2IN & BIT0) == BIT0) ? (Button | BIT7) : (Button & ~BIT7);
    Button = ((P2IN & BIT2) == BIT2) ? (Button | BIT6) : (Button & ~BIT6);
    Button = ((P4IN & BIT0) == BIT0) ? (Button | BIT5) : (Button & ~BIT5);
    Button = ((P4IN & BIT6) == BIT6) ? (Button | BIT4) : (Button & ~BIT4);
}//--END CheckRow-----------------------------------------------------------------------------------

//-ButtonResponse: Updating last button to current button-----------------------------------------------
void ButtonResponse() {
    // Update last button with identified button pressed
    switch(Button) {
        case KEY_0: LastButton = KEY_0; break;
        case KEY_1: LastButton = KEY_1; break;
        case KEY_2: LastButton = KEY_2; break;
        case KEY_3: LastButton = KEY_3; break;
        case KEY_4: LastButton = KEY_4; break;
        case KEY_5: LastButton = KEY_5; break;
        case KEY_6: LastButton = KEY_6; break;
        case KEY_7: LastButton = KEY_7; break;
        case KEY_8: LastButton = KEY_8; break;
        case KEY_9: LastButton = KEY_9; break;
        case KEY_AST: LastButton = KEY_AST; break;
        case KEY_POUND: LastButton = KEY_POUND; break;
        case KEY_A: LastButton = KEY_A; break;
        case KEY_B: LastButton = KEY_B; break;
        case KEY_C: LastButton = KEY_C; break;
        case KEY_D: LastButton = KEY_D; break;
    }
}//--End ButtonResponse

//-PatternA: Static pattern XOXOXOXO---------------------------------------------------------------
void PatternA(){
    //Set High on Collumns
    GPIO_setOutputHighOnPin(Port2, Pin2);
    GPIO_setOutputLowOnPin(Port4, Pin0);
    GPIO_setOutputHighOnPin(Port6, Pin3);
    GPIO_setOutputLowOnPin(Port4, Pin6);
    GPIO_setOutputHighOnPin(Port4, Pin7);
    GPIO_setOutputLowOnPin(Port4, Pin4);
    GPIO_setOutputHighOnPin(Port2, Pin5);
    GPIO_setOutputLowOnPin(Port3, Pin0);
}//--END PatternA--------------------------------------------------------------------------------

//-PatternB: Flashing and counts up--------------------------------------------------------------
void PatternBUpdate() {
    // set clock for 1 Hz
    TB0CTL |= TBCLR + TBSSEL__ACLK + MC__UP + ID__1;
    TB0CCR0 = 16384;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;
    TB0R = 0;
    // Count and reset on rollover
    PatternBCounter += 1;
    LED_Out = PatternBCounter;
}//--END PatternB

//-PatternC: Scrolling Pattern ---------------------------------------------------------------
void PatternC(){
    // Set timer for 2s
    TB0CTL |= TBCLR + TBSSEL__ACLK + MC__UP + ID__1;
    TB0CCR0 = 36864;
    TB0R = 0;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;

    // Count and reset on rollover
    switch(PatternDCounter) {
        case 0: LED_Out = 0x7F; break;
        case 1: LED_Out = 0xBF; break;
        case 2: LED_Out = 0xDF; break;
        case 3: LED_Out = 0xEF; break;
        case 4: LED_Out = 0xF7; break;
        case 5: LED_Out = 0xFB; break;
        case 6: LED_Out = 0xFD; break; 
        case 7: LED_Out = 0xFE; break;
    }
    PatternDCounter += 1;
    if (PatternDCounter == 7){
        PatternDCounter = 0;
    }
}//--END PatternC--------------------------------------------------------------------------------

//-PatternD: Scrolling double pattern------------------------------------------------------------
void PatternDUpdate() {
    // Set timer for 2s and 2.25s 
    TB0CTL |= TBCLR + TBSSEL__ACLK + MC__UP + ID__1;
    TB0CCR0 = 36864;
    TB0CCR1 = 32768;
    TB0R = 0;
    TB1R = 0;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;
    TB0CCTL1 |= CCIE;
    TB0CCTL1 &= ~CCIFG;

    // Count and reset on rollover
    switch(PatternDCounter) {
        case 0: LED_Out = 0x018; break;
        case 1: LED_Out = 0x024; break;
        case 2: LED_Out = 0x042; break;
        case 3: LED_Out = 0x081; break;
        case 4: LED_Out = 0x042; break;
        case 5: LED_Out = 0x024; break;
    }
    PatternDCounter += 1;
    if (PatternDCounter == 6)
        PatternDCounter = 0;
}//--END PatternD--------------------------------------------------------------------------------

//-PatternError: --------------------------------------------------------------------------
void PatternErrorUpdate() {
    // set timer for 0.25s
    TB0CTL |= TBCLR + TBSSEL__ACLK + MC__UP + ID__1;
    TB0CCR0 = 4096;
    TB0R = 0;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;
    // flash LEDs 3 times before turning off interrupt
    LED_Out = 0x000; 
    if (PatternECounter == 0 || PatternECounter == 2 || PatternECounter == 4) {
        LED_Out = 0x0FF;
    }
    PatternECounter += 1;
    if (PatternECounter == 6) {
        PatternECounter = 0;
        Pattern = 0;
        TB0CCTL0 &= ~CCIE;
    }
}//--End PatternError------------------------------------------------------------------------

//-Update LED: -------------------------------------------------------------------------
void UpdateLED() {
    // Move LED_Out to led ports
    P1OUT = ((LED_Out & BIT0) == BIT0) ? (P1OUT | BIT4) : (P1OUT & ~BIT4);
    P5OUT = ((LED_Out & BIT1) == BIT1) ? (P5OUT | BIT3) : (P5OUT & ~BIT3);
    P5OUT = ((LED_Out & BIT2) == BIT2) ? (P5OUT | BIT1) : (P5OUT & ~BIT1);
    P5OUT = ((LED_Out & BIT3) == BIT3) ? (P5OUT | BIT0) : (P5OUT & ~BIT0);
    P5OUT = ((LED_Out & BIT4) == BIT4) ? (P5OUT | BIT4) : (P5OUT & ~BIT4);
    P1OUT = ((LED_Out & BIT5) == BIT5) ? (P1OUT | BIT1) : (P1OUT & ~BIT1);
    P3OUT = ((LED_Out & BIT6) == BIT6) ? (P3OUT | BIT5) : (P3OUT & ~BIT5);
    P3OUT = ((LED_Out & BIT7) == BIT7) ? (P3OUT | BIT1) : (P3OUT & ~BIT1);    
}//End UpdateLED --------------------------------------------------------------------


//-- Interrupt Service Routines -----------------------------------------------------------
//-ISR Port 2---------------------------------------------------------------------------
#pragma vector = PORT2_VECTOR
__interrupt void ISR_P2_Button_Pressed(void) {
    CheckFlag = true; // Tell system to check which key was pressed
    P2IFG &= ~BIT0;  // Clear Interrupt Flags
    P2IFG &= ~BIT2; 
}//-- End ISR_P1_Button_Pressed ----------------------------------------------------------

//-ISR Port 4 ----------------------------------------------------------------------------
#pragma vector = PORT4_VECTOR
__interrupt void ISR_P4_Button_Pressed(void) {
    CheckFlag = true; // Tell system to check which key was pressed
    P4IV &= ~BIT0; // Clear Interrupt Flags
    P4IV &= ~BIT6;
}//-- End ISR_P5_Button_Pressed ----------------------------------------------------------

//-ISR Timer B---------------------------------------------------------------------------
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){
    // If password hasn't been fully entered, and timer triggered set pattern to 5 and reset to state 0
    if (State < 5) {
        State = 0;
        Pattern = 5;
        TimerFlag = true;
    } else // If password has been fully entered, then toggle LED update
        TimerFlag = true;  
    // Clear interrupt flag
    TB0CCTL0 &= ~CCIFG; 
}//-- End Timer_B_ISR ------------------------------------------------------------------

//-ISR Timer B1 ------------------------------------------------------------------------
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer_B1_ISR(void){
    LED_Out = 0x000; // turn off LEDs then disable interrupt and clear flag
    TB0CCTL1 &= ~CCIE;
    TB0CCTL1 &= ~CCIFG; 
}//-- End Timer_B_ISR -----------------------------------------------------------------

