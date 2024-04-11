/*----------------------------------------------------------------------------------------------------------------------
    EELE465
    Written by: Lance Gonzalez, Grant Kirkland, Ken Vincent
    Working:
    Project 06

    Summary:

    Version Summary:
        v01:

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



-----------------------------------------------------------------------------------------------------------------------*/


#include <stdint.h>
#include "msp430fr2355.h"
#include <driverlib.h>

#include <msp430.h>
#include <stdint.h>
#include "stdbool.h"
#include <gpio.h>

#include "keypad.h"
#include "analogdigital.h"
#include "i2c.h"


#define TempThreshold 1000.0

void LCDFormat();
void TempConversion();
void TransmitLCD();

//Vairable Declarations-----------------------------------------------------

// State Variables
uint8_t State = 0b00000000; // Peltier, Local and remote ADCs, RTC, Timer
    // 0-1: 0 Off, 1 Heat, 2 Cool, 3 Maintain                                   Peltier Device
    // 2-3: 0 Start ADC, 1 Wait for Sample, 2 Save+Avg, 3 Wait for timer        Local ADC
    // 4-5: 0 Request Temp, 1 Wait for Response, 2 Save+Avg, 3 Wait for timer   Remote ADC
    // 6-7: 0 Request Time, 1 Wait for Response, 2 save and respond, 3 wait     RTC


uint8_t SecondaryState = 0b00000000;
    // 0: Key pressed
    // 1-3: Timer stuff 1 sec loop
        // 000 RTC
        // 125 Local ADC
        // 250 External ADC
        // 375
        // 500 LCD
        // 625 Local ADC
        // 750 External ADC
        // 875
    // 4: Setpoint enter vs Window Averaging enter
    // 5:
    // Maybe peltier next state bits?

uint8_t TransmitState = 0b00000000; // 0 LCD 1 LED 2 RTC 3 ADC, 4 pending LCD, 5 pending LED, 6 pending RTC 7 pending ADC


// Keypad
void ButtonResponse();
char LastButton = 0;


// Peltier
void PeltierOff();
void PeltierCool();
void PeltierHeat();
void PeltierMaintain();

bool HeatCool = false;

// Temp
uint8_t Setpoint = 0;
uint8_t AveragingWindowValue = 3;
extern bool CheckTempThreshold();

//-----------------------------------------------------------------------

int main(void) {
    // Stop watchdog timer

    WDTCTL = WDTPW | WDTHOLD;

    // Initialization
    Init_Keypad();
    Init_I2C();
    Init_ADC();

    GPIO_setAsOutputPin(1, 0x04);
    P1OUT |= BIT3;
    GPIO_setAsOutputPin(1, 0x00);
    P1OUT &= ~BIT0;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Timer configured for 1/8th of a second
    //  ** TODO **
    TB0CTL |= TBSSEL__ACLK + MC__UP + ID__1;
    TB0CCR0 = 4260;
    TB0R = 0;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;


    __enable_interrupt();

    while(1) {
        if (SecondaryState & KeyPressedFlag) {
            CheckButton();
            ButtonResponse();
            continue;
        }
//        if (CheckTempThreshold()) {
//            continue;
//        }
//        // Peltier Device State
//        switch (0b00000011 & State) {
//            case 0: PeltierHeat(); break; // off
//            case 1: PeltierCool(); break; // heat
//            case 2: PeltierMaintain(); break; // cool
//            case 3: PeltierOff(); break; // maintain
//            default:
//                break;
//        }
//        // MSP ADC State
//        switch (0b00001100 & State) {
//            case 0: LocalADCStart(); State += 0b00000100;  break; // Start sample
//            case 4:  break; // wait
//            case 8:  break; // save and average
//            case 12: break; // wait
//            default:
//                break;
//        }
//        // LM92 State
        switch (0b00110000 & State) {
            case 0: TransmitState |= StartTxADC; State += 0b00010000;  break; // send message
            //case 16:  break; // wait
            case 32:  break; // save and average
            //case 48:  break; // wait
            default:
                break;
        }
//        // RTC State
//        switch (0b11000000 & State) {
//            case 0:   break; // send message
//            case 64:  break; // wait
//            case 128: break; // save time
//            case 192: break; // wait
//            default:
//                break;
//        }
        TransmitStart();
    }
}


void ButtonResponse() {
//    UCB1TBCNT = 1;
//    UCB1I2CSA = LCD_Address; // Set the slave address in the module
//    //...equal to the slave address
//    UCB1CTLW0 |= UCTR; // Put into transmit mode
//    UCB1CTLW0 |= UCTXSTT; // Generate the start condition

    switch(LastButton) {
        case '*':
            SecondaryState ^= KeypadModeToggle;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
            State &= 0b11111100;
            State |= LastButton - 'A';
            TransmitState |= StartTxLED;
            break;
        case '#':
            LocalADCDataReset();
            RemoteADCDataReset();
            TransmitState |= StartTxLCD + StartTxLED;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (SecondaryState & KeypadModeToggle) { // If in Averaging mode
                AveragingWindowValue = LastButton - 48;
            }
        case '0':
            if (SecondaryState & ~KeypadModeToggle) {
                if (Setpoint >= 10) {
                    Setpoint = 0;
                }
                Setpoint = Setpoint * 10 + (LastButton - 48);
            }
            break;
        default:
            break;
    }

}


bool CheckTempThreshold() { return false;}

void PeltierOff() {}
void PeltierCool() {}
void PeltierHeat() {}
void PeltierMaintain() {}


//-- Interrupt Service Routines -----------------------------------------------------------

//-ISR Timer B---------------------------------------------------------------------------
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){
    P1OUT ^= BIT0;
    switch (0b00001110 & SecondaryState) {
        case 10:        // Local ADC
        case 2:         // Local ADC
            //State &= 0b11110011;
            break;
        case 12:        // External ADC
        case 4:         // External ADC
            State &= 0b11001111;
            break;
        case 0:         // RTC
            //State &= 0b00111111;
            break;
        case 8:         // LCD
            //TransmitState |= StartTxLCD;
            break;
        case 14: break; // Wait
        case 6:  break; // Wait
        default:
            break;
    }
    if ((0b00001110 & SecondaryState) != 14)
        SecondaryState += 2;
    else
        SecondaryState -= 0b00001110;
    // Clear interrupt flag
    TB0CCTL0 &= ~CCIFG;
}//-- End Timer_B_ISR ------------------------------------------------------------------

//-ISR Timer B1 ------------------------------------------------------------------------
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer_B1_ISR(void){
    TB0CCTL1 &= ~CCIE;
    TB0CCTL1 &= ~CCIFG;
}//-- End Timer_B_ISR -----------------------------------------------------------------

