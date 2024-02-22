/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 03 - Feb 15 2024

	Summary:

	Version Summary:
        v01: Press button in column to toggle 0-3 binary on leds
        v02: Working LED Output, defined keypad macros, working column readout

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
        P4 |1|  |2|  |3|  |A| P1.4
        P5 |4|  |5|  |6|  |B| P5.3
        P6 |7|  |8|  |9|  |C| P5.1
        P7 |*|  |0|  |#|  |D| P5.0
          P3.1 P3.5 P1.1 P5.4
    
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

#define Port1 GPIO_PORT_P1  
#define Port3 GPIO_PORT_P3  
#define Port5 GPIO_PORT_P5 

#define P0 GPIO_PIN0
#define P1 GPIO_PIN1
#define P2 GPIO_PIN2
#define P3 GPIO_PIN3
#define P4 GPIO_PIN4
#define P5 GPIO_PIN5

void CheckCol();
void CheckRow();
void PatternAUpdate();
void PatternBUpdate();
void PatternCUpdate();
void PatternDUpdate();
void ButtonResponse();
void UpdateLED();

uint8_t Button = 0, LED_Out = 0, Pattern = 0;
uint8_t PatternBCounter = 0;
uint8_t PatternDCounter = 0;
uint8_t State = 0; // 0 - wait for key, 1-3 - correct button pressed for password, 
uint8_t LastButton; // tracker of last button pressed
uint8_t Passcode[] = {KEY_1, KEY_2, KEY_3};
bool CheckFlag = false;
bool TimerFlag = false;
int test; // arbitrary test register

int main(void) {
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);
    //LED Bar Output initalizing as low
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);

    //Setting keypad ports as inputs with interupt enabled. todo decide which should be the input. rows vs columns. won't work as is
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN1);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN4);
    P5IES |= BIT0;
    P5IES |= BIT1;
    P5IES |= BIT3;
    P5IES |= BIT4;
    P1IES |= BIT4;
    P1IES |= BIT1;
    P3IES |= BIT5;
    P3IES |= BIT1;


    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

    while(1) {
        if (CheckFlag) {
            CheckCol();
            CheckRow();
        }
        switch(State) {
            case 0: // start waiting for password
                if (LastButton == Passcode[0])
                    State++;
            break;
            case 1: // first key entered
                if (LastButton == Passcode[1])
                    State++;
            break;
            case 2: // second key entered
                if (LastButton == Passcode[2])
                    State++;
            break;
            case 3: // password entered
                // switch pattern based on last button pressed
                switch (LastButton) { 
                    case KEY_A: Pattern = 1; break;
                    case KEY_B: Pattern = 2;  break;
                    case KEY_C: Pattern = 3;  break;
                    case KEY_D: Pattern = 4;  break;
                    default: Pattern = 0; break;
                }
                // Run pattern based on current value 
                switch (Pattern) { 
                    case 1: PatternAUpdate(); break;
                    case 2: PatternBUpdate();  break;
                    case 3: PatternCUpdate();  break;
                    case 4: PatternDUpdate();  break;
                    default: break;
                }
            break;
            
        }
        UpdateLED();
    }
}

void delay_init(){
    // Configure Timer_A to generate interrupts every 1 second
    TB0CCTL0 = CCIE; 
    TB0CCR0 = 32767; 
    TB0CTL = TBSSEL_1 + MC_1 + TBCLR; 
}

void CheckCol() {
    // Set necessary pins as inputs and outputs for reading columns
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN1);

    // Clear button column values
    Button &= 0x0F0;

    Button = ((P5IN & BIT4) == BIT4) ? (Button | BIT3) : (Button & ~BIT3);
    Button = ((P1IN & BIT1) == BIT1) ? (Button | BIT2) : (Button & ~BIT2);
    Button = ((P3IN & BIT5) == BIT5) ? (Button | BIT1) : (Button & ~BIT1);
    Button = ((P3IN & BIT1) == BIT1) ? (Button | BIT0) : (Button & ~BIT0);

    test = P3IN;
    test &= BIT1;
    if (test == BIT1)
        Button |= BIT0;

}

void CheckRow() {
    // Set necessary pins as inputs and outputs for reading columns
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN1);

}

void ButtonResponse() {
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
}

void PatternAUpdate() {
}

void PatternBUpdate() {
    // Count and reset on rollover
    PatternBCounter += 1;
    LED_Out = PatternBCounter;
}

void PatternCUpdate() {
}

void PatternDUpdate() {
    // Count and reset on rollover
    PatternDCounter += 1;
    if (PatternDCounter == 6)
        PatternDCounter = 0;
    switch(PatternDCounter) {
        case 0: LED_Out = 0x018; break;
        case 1: LED_Out = 0x024; break;
        case 2: LED_Out = 0x042; break;
        case 3: LED_Out = 0x018; break;
        case 4: LED_Out = 0x042; break;
        case 5: LED_Out = 0x024; break;
    }
}

void UpdateLED() {
    // Move LED_Out to led ports
    P3OUT = ((LED_Out & BIT0) == BIT0) ? (P3OUT | BIT0) : (P3OUT & ~BIT0);
    P2OUT = ((LED_Out & BIT1) == BIT1) ? (P2OUT | BIT5) : (P2OUT & ~BIT5);
    P4OUT = ((LED_Out & BIT2) == BIT2) ? (P4OUT | BIT4) : (P4OUT & ~BIT4);
    P4OUT = ((LED_Out & BIT3) == BIT3) ? (P4OUT | BIT7) : (P4OUT & ~BIT7);
    P4OUT = ((LED_Out & BIT4) == BIT4) ? (P4OUT | BIT6) : (P4OUT & ~BIT6);
    P4OUT = ((LED_Out & BIT5) == BIT5) ? (P4OUT | BIT0) : (P4OUT & ~BIT0);
    P2OUT = ((LED_Out & BIT6) == BIT6) ? (P2OUT | BIT2) : (P2OUT & ~BIT2);
    P2OUT = ((LED_Out & BIT7) == BIT7) ? (P2OUT | BIT0) : (P2OUT & ~BIT0);    
}

//-- Interrupt Service Routines --------------------------
#pragma vector = PORT1_VECTOR
__interrupt void ISR_P1_Button_Pressed(void) {
    CheckFlag = true;
    P1IV &= ~BIT4;  // Clear CCR0 Flag
}
//-- End ISR_P1_Button_Pressed ----------------

#pragma vector = PORT5_VECTOR
__interrupt void ISR_P5_Button_Pressed(void) {
    CheckFlag = true;
    P5IV &= ~BIT0; // Clear Flag
    P5IV &= ~BIT1;
    P5IV &= ~BIT3;
}
//-- End ISR_P5_Button_Pressed ----------------

#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){
    if (State < 3)
        State = 0;
    else 
        TimerFlag = true;  

    TB0CCTL0 &= ~CCIFG; 
}
//-- End Timer_B_ISR ----------------
