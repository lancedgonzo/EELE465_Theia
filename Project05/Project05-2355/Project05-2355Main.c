/*----------------------------------------------------------------------------------------------------------------------
    EELE465
    Written by: Lance Gonzalez, Grant Kirkland
    Working:
    Project 05

    Summary:

    Version Summary:
        v01: Working Project 04

    MSP430FR2355:
         4 - SBWTCK - SBWTCK
         5 - SBWTDIO - SBWTDIO
         6 - DVCC - 3v3
         7 - DVSS - GND
        12 - P4.7 - SCL
        13 - P4.6 - SDA
        25 - P4.1 - P7 Row4
        26 - P4.0 - P6 Row3
        27 - P2.3 - P5 Row2
        28 - P2.2 - P4 Row1
        29 - P2.1 - P3 Col4
        30 - P2.0 - P2 Col3
        31 - P1.7 - P1 Col2
        32 - P1.6 - P0 Col1

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
        P4 |1|  |2|  |3|  |A| P3.7
        P5 |4|  |5|  |6|  |B| P4.0
        P6 |7|  |8|  |9|  |C| P2.2
        P7 |*|  |0|  |#|  |D| P2.0
          P3.0 P2.5 P4.4 P2.4

    MSP Errors:
        P1.3 Always High
        Row P3.2-2.1 mostly not working?
        P4.5 Inconsistent Input?
    Todo:

-----------------------------------------------------------------------------------------------------------------------*/


#include <stdint.h>
#include "msp430fr2355.h"
#include <driverlib.h>

#include <msp430.h>
#include <stdint.h>
#include "stdbool.h"
#include <gpio.h>

#include "keypad.h"
#define LED_Address 0x013
#define LCD_Address 0x046

void Init_I2C();
void LCDFormat();
void ADCToTemp();
void TempConversion();

//Vairable Declarations-----------------------------------------------------
uint8_t State = 99; // 0 - wait for key, 1-3 - correct button pressed for password,

// Keypad
char LastButton = 0;
bool CheckFlag = false;

// LCD Output
bool TransmitToLCD = true;
char LCDMessage[32] = "12345678901234567890123456789012";
uint8_t LCDPointer = 0;

// Temp
uint16_t Data[10];
uint8_t DataPointer;
uint8_t WindowValue = 3;
uint16_t AveragedTemp;


//-----------------------------------------------------------------------

int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Initialization
    Init_Keypad();
    Init_I2C();

 //   GPIO_setAsOutputPin(1, 0x04);
 //   P1OUT |= BIT3;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;


    __enable_interrupt();

    while(1) {
        switch (state) {
            case 99: // Test state for continuous transmit
                LCDFormat();
                TransmitButton();

                break;

        }

    }
}

void Init_I2C() {

    UCB1CTLW0 |= UCSWRST; // Put UCB1CTLW0 into software reset
    UCB1CTLW0 |= UCSSEL_3; // Select mode 3
    UCB1BRW = 10; // Something useful

    UCB1CTLW0 |= UCMODE_3; // Mode 3
    UCB1CTLW0 |= UCMST; // Master
    UCB1CTLW0 |= UCTR; // Transmit mode

    UCB1CTLW1 |= UCASTP_2; // Autostop enabled

    UCB1CTLW0 &= ~UCSWRST; // Take out of Software Reset

    UCB1IE |= UCTXIE0; // Enable TX interrupt
    UCB1IE |= UCRXIE0; // Enable RX interrupt

    //----- P4.6 and P4.7 for I2C ---
    P4SEL1 &= ~BIT7;
    P4SEL0 |= BIT7;

    P4SEL1 &= ~BIT6;
    P4SEL0 |= BIT6;


}


void TransmitButton() {
    LCDPointer = 0;
    UCB1TBCNT = 32;
    UCB1I2CSA = LED_Address; // Set the slave address in the module equal to the slave address
    UCB1CTLW0 |= UCTR; // Put into transmit mode
    UCB1CTLW0 |= UCTXSTT; // Generate the start condition
    __delay_cycles(5000);
}


void LCDFormat() {

}

void ADCToTemp() {

}

void TempConversion() {

}


//-- Interrupt Service Routines -----------------------------------------------------------
#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void) {
    UCB1TXBUF = LCDMessage[LCDPointer];
    LCDPointer++;
    if (LCDPointer == 33) {
        LCDPointer = 0;
    }
}

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
    TB0CCTL1 &= ~CCIE;
    TB0CCTL1 &= ~CCIFG;
}//-- End Timer_B_ISR -----------------------------------------------------------------

