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

 #define LED_Address 0x048
 #define LCD_Address 0x046


// Defining row and column input as keys
#define KEY_1 0x011 // 17
#define KEY_2 0x012 // 18
#define KEY_3 0x014 // 20
#define KEY_4 0x021 // 33
#define KEY_5 0x022 // 34
#define KEY_6 0x024 // 36
#define KEY_7 0x041 // 65
#define KEY_8 0x042 // 66
#define KEY_9 0x044 // 68
#define KEY_0 0x082 // 130
#define KEY_POUND 0x084 // 132
#define KEY_AST 0x081 // 129
#define KEY_A 0x018 // 24
#define KEY_B 0x028 // 40
#define KEY_C 0x048 // 72
#define KEY_D 0x088 // 136

// Making ports and pins easier to reference
#define Port1 1
#define Port2 2
#define Port3 3
#define Port4 4
#define Port5 5

#define P0 (0x0001)
#define P1 (0x0002)
#define P2 (0x0004)
#define P3 (0x0008)
#define P4 (0x0010)
#define P5 (0x0020)
#define P6 (0x0040)
#define P7 (0x0080)

//Function Declarations ------------------------------------------------------
void ColInput();
void CheckCol();
void RowInput();
void CheckRow();
void CheckButton();
void ButtonResponse();
void TransmitButton();
void SwitchDebounce();
//--------------------------------------------------------------------------

//Vairable Declarations-----------------------------------------------------
uint8_t i;
uint8_t State = 0; // 0 - wait for key, 1-3 - correct button pressed for password,
uint8_t Button, LastButton = 0; // tracker of last button pressed
uint8_t const Passcode[] = {'1', '5', '3'};
bool CheckFlag = false;
bool TimerFlag = false;
bool QuestFlag = false;
int test; // arbitrary test register
//-----------------------------------------------------------------------

int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Initialize LED and keyboard
    RowInput();

    // Clear interrupt flag bits
    P2IFG &= ~BIT0;
    P2IFG &= ~BIT2;
    P4IFG &= ~BIT0;
    P3IFG &= ~BIT7;
    // Rising edge sensitivity
    P2IES &= ~BIT0;
    P2IES &= ~BIT2;
    P4IES &= ~BIT0;
    P3IES &= ~BIT7;

    UCB1CTLW0 |= UCSWRST; // Put UCB1CTLW0 into software reset
    UCB1CTLW0 |= UCSSEL_3; // Select mode 3
    UCB1BRW = 10; // Something useful

    UCB1CTLW0 |= UCMODE_3; // Mode 3
    UCB1CTLW0 |= UCMST; // Master
    UCB1CTLW0 |= UCTR; // Transmit mode

    UCB1CTLW1 |= UCASTP_2; // Autostop enabled
    // P1.2 SDA
    P1SEL1 &= ~(BIT2+BIT3);
//    P1SEL1 |= (BIT2+BIT3);
    P1SEL0 |= (BIT2+BIT3);
    //P1SEL0 &= ~(BIT2+BIT3);
    //----- P4.6 and P4.7 for I2C ---
    P4SEL1 &= ~BIT7;
    P4SEL0 |= BIT7;

    P4SEL1 &= ~BIT6;
    P4SEL0 |= BIT6;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
    UCB1CTLW0 &= ~UCSWRST; // Take out of Software Reset

    UCB1IE |= UCTXIE0; // Enable TX interrupt
    UCB1IE |= UCRXIE0; // Enable RX interrupt

    // Interrupt enable
    P2IE |= BIT0;
    P2IE |= BIT2;
    P4IE |= BIT0;
    P3IE |= BIT7;

    __enable_interrupt();

    while(1) {
        if (CheckFlag) {
            CheckButton();
            State++;
        }
        switch(State) {
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
                TransmitButton();

                // Go back to waiting for key press
                State--;
            break;

        }
    }
}

void TransmitButton() {

    switch(LastButton) {
        case '*':
            if (!TimerFlag) {
                TimerFlag = true;
                TB0CTL |= TBCLR + TBSSEL__ACLK + MC__UP + ID__1;
                TB0CCR1 = 32768;
                TB1R = 0;
                TB0CCTL1 |= CCIE;
                TB0CCTL1 &= ~CCIFG;
            } else {
                QuestFlag = true;
                TimerFlag = false;
            }
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case '#':
            UCB1TBCNT = 1;
            UCB1I2CSA = LED_Address; // Set the slave address in the module
            //...equal to the slave address
            UCB1CTLW0 |= UCTR; // Put into transmit mode
            UCB1CTLW0 |= UCTXSTT; // Generate the start condition
            for (i = 65000; i > 0; i--) {/* Delay */}
        default:
            UCB1TBCNT = 1;
            UCB1I2CSA = LCD_Address; // Set the slave address in the module
            //...equal to the slave address
            UCB1CTLW0 |= UCTR; // Put into transmit mode
            UCB1CTLW0 |= UCTXSTT; // Generate the start condition
            break;
    }

}

//-ColumnInput: Sets row pins as OUTPUT---------------------------------------------------------------------
void ColumnInput(){

  //Configure outputs on Rows
    P2DIR |= P0;
    P2DIR |= P2;
    P4DIR |= P0;
    P3DIR |= P7;

    //Configure inputs on Collumns
    P2DIR &= ~P4;
    P4DIR &= ~P4;
    P2DIR &= ~P5;
    P3DIR &= ~P0;
    P2REN |= P4;
    P4REN |= P4;
    P2REN |= P5;
    P3REN |= P0;
    P2OUT &= ~P4;
    P4OUT &= ~P4;
    P2OUT &= ~P5;
    P3OUT &= ~P0;

    //Set High on ROWS
    P2OUT |= P0;
    P2OUT |= P2;
    P4OUT |= P0;
    P3OUT |= P7;

}//--END ColumnInput----------------------------------------------------------------------------------------

//-CheckCol: Sets row pins as OUTPUT---------------------------------------------------------------------
void CheckCol() {
    // Set necessary pins as inputs and outputs for reading columns
    ColumnInput();

    // Clear button column values, then toggle bits corresponding to column
    Button &= 0x0F0;
    Button = ((P2IN & BIT4) == BIT4) ? (Button | BIT3) : (Button & ~BIT3);
    Button = ((P4IN & BIT4) == BIT4) ? (Button | BIT2) : (Button & ~BIT2);
    Button = ((P2IN & BIT5) == BIT5) ? (Button | BIT1) : (Button & ~BIT1);
    Button = ((P3IN & BIT0) == BIT0) ? (Button | BIT0) : (Button & ~BIT0);

}//--END CheckCol-------------------------------------------------------------------------------------

//-Collumns: Sets Collumn pins as OUTPUT-------------------------------------------------------------
void RowInput(){

    //Set P0-3 on keyboard as outputs
    P2DIR |= P4;
    P4DIR |= P4;
    P2DIR |= P5;
    P3DIR |= P0;

    //Set P4-7 on keyboard as inputs with pull down
    P2DIR &= ~P0;
    P2DIR &= ~P2;
    P4DIR &= ~P0;
    P3DIR &= ~P7;
    P2REN |= P0;
    P2REN |= P2;
    P4REN |= P0;
    P3REN |= P7;
    P2OUT &= ~P0;
    P2OUT &= ~P2;
    P4OUT &= ~P0;
    P3OUT &= ~P7;


    //Set P0-3 on keyboard as HIGH
    P2OUT |= P4;
    P4OUT |= P4;
    P2OUT |= P5;
    P3OUT |= P0;

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
    Button = ((P3IN & BIT7) == BIT7) ? (Button | BIT4) : (Button & ~BIT4);
}//--END CheckRow-----------------------------------------------------------------------------------

//-CheckButton: Call functions to check which button was pressed, save it in last button and debounce.
void CheckButton() {
    CheckCol();
    CheckRow();
    ButtonResponse();
    SwitchDebounce();
    CheckFlag = false;
}//--END CheckButton-----------------------------------------------------------------------------------

//-ButtonResponse: Updating last button to current button-----------------------------------------------
void ButtonResponse() {
    // Update last button with identified button pressed
    switch(Button) {
        case KEY_0: LastButton = '0'; break;
        case KEY_1: LastButton = '1'; break;
        case KEY_2: LastButton = '2'; break;
        case KEY_3: LastButton = '3'; break;
        case KEY_4: LastButton = '4'; break;
        case KEY_5: LastButton = '5'; break;
        case KEY_6: LastButton = '6'; break;
        case KEY_7: LastButton = '7'; break;
        case KEY_8: LastButton = '8'; break;
        case KEY_9: LastButton = '9'; break;
        case KEY_AST: LastButton = '*'; break;
        case KEY_POUND: LastButton = '#'; break;
        case KEY_A: LastButton = 'A'; break;
        case KEY_B: LastButton = 'B'; break;
        case KEY_C: LastButton = 'C'; break;
        case KEY_D: LastButton = 'D'; break;
    }
}//--End ButtonResponse

//-SwitchDebounce(): -------------------------------------------------------------------------
void SwitchDebounce(){
    for(i=0; i<10; i++){}
}//--END SwitchDebounce---------------------------------------------------------------

//-- Interrupt Service Routines -----------------------------------------------------------

#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void) {
    if (QuestFlag) {
        UCB1TXBUF = '?';
        QuestFlag = false;
        TimerFlag = false;
    } else
        UCB1TXBUF = LastButton;
}

//-ISR Port 4 ----------------------------------------------------------------------------
#pragma vector = PORT2_VECTOR
#pragma vector = PORT3_VECTOR
#pragma vector = PORT4_VECTOR
__interrupt void ISR_Button_Pressed(void) {
    CheckFlag = true; // Tell system to check which key was pressed
    P2IFG &= ~BIT0;  // Clear Interrupt Flags
    P2IFG &= ~BIT2;
    P4IFG &= ~BIT0;
    P3IFG &= ~BIT7;
}//-- End ISR_P5_Button_Pressed ----------------------------------------------------------

//-ISR Timer B---------------------------------------------------------------------------
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){
    // If password hasn't been fully entered, and timer triggered set pattern to 5 and reset to state 0
    if (State < 5) {
        State = 0;
    }
    // Clear interrupt flag
    TB0CCTL0 &= ~CCIFG;
}//-- End Timer_B_ISR ------------------------------------------------------------------

//-ISR Timer B1 ------------------------------------------------------------------------
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer_B1_ISR(void){
    TimerFlag = false;
    TB0CCTL1 &= ~CCIE;
    TB0CCTL1 &= ~CCIFG;
}//-- End Timer_B_ISR -----------------------------------------------------------------

