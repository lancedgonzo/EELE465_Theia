/*----------------------------------------------------------------------------------------------------------------------
    EELE465
    Written by: Lance Gonzalez, Grant Kirkland
    Working:
    Project 05

    Summary:

    Version Summary:
        v01: Working Project 04
        v02: Output 32 chars

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

#define LED_Address 0x013
#define LCD_Address 0x046
#define ADC_Address 0x002
#define TempThreshold 1000.0
#define PELTIER_HEAT BIT1
#define PELTIER_COOL BIT0

void Init_I2C();
void LCDFormat();
void TempConversion();
void TransmitLCD();

//Vairable Declarations-----------------------------------------------------

// State Variables
uint8_t State = 0b00000000; // Peltier, Local and remote ADCs, RTC, Timer
    // 0-1: 0 Off, 1 Heat, 2 Cool, 3 Maintain
    // 2-3: 0 Start ADC, 1 Wait for Sample, 2 Save+Avg, 3 Wait for timer
    // 4-5: 0 Request Temp, 1 Wait for Response, 2 Save+Avg, 3 Wait for timer
    // 6: 0 Request Time, 1 Wait for Response
    // 7: Timer toggle 0 ADCs 1 RTC

uint8_t TransmitState = 0b00000000; // 0 LCD 1 LED 2 RTC 3 ADC, pending? 4-8?


// Keypad
void ButtonResponse();
char LastButton = 0;
bool KeyPressedFlag = false;

// Peltier
void PeltierOff();
void PeltierCool();
void PeltierHeat();
void PeltierMaintain();

// LCD Output
void LCDFormat();
char LCDMessage[32] = "12345678901234567890123456789012";
uint8_t LCDPointer = 0;

// Temp
uint8_t AveragingWindowValue = 3;
extern bool CheckTempThreshold();

// Timing
void delay_ms_(unsigned int);

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

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    TB0CTL |= TBSSEL__ACLK + MC__UP + ID__2;
    TB0CCR0 = 5460;
    TB0R = 0;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;


    __enable_interrupt();

    while(1) {
        if (KeyPressedFlag) {
            CheckButton();
            ButtonResponse();
            continue;
        }
        if (CheckTempThreshold()) {
            continue;
        }
        // Peltier Device State
        switch (0b00000011 & State) {
            case 0: PeltierOff(); break;
            case 1: PeltierHeat(); break;
            case 2: PeltierCool(); break;
            case 3: PeltierMaintain(); break;
            default:
                break;
        }
        // MSP ADC State
        switch (0b00001100 & State) {
            case 0:  break;
            case 4:  break;
            case 8:  break;
            case 12: break;
            default:
                break;
        }
        // LM92 State
        switch (0b00110000 & State) {
            case 0:  break;
            case 16:  break;
            case 32:  break;
            case 48:  break;
            default:
                break;
        }
        // RTC State
        switch (0b01000000 & State) {
            case 0:  break;
            case 64:  break;
            default:
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




void ButtonResponse() {
//    UCB1TBCNT = 1;
//    UCB1I2CSA = LCD_Address; // Set the slave address in the module
//    //...equal to the slave address
//    UCB1CTLW0 |= UCTR; // Put into transmit mode
//    UCB1CTLW0 |= UCTXSTT; // Generate the start condition

    switch(LastButton) {
        case '0':
            break;
        case 'D':
        case '*':
        case 'A':
        case 'B':
        case 'C':
            break;
        case '#':
            ADCDataReset();
            LCDFormat();
            TransmitLCD();
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
            AveragingWindowValue = LastButton - 48;
            break;
        default:
            break;
    }

}

void TransmitLCD() {
    LCDPointer = 0;
    UCB1TBCNT = 32;
    UCB1I2CSA = LCD_Address; // Set the slave address in the module equal to the slave address
    UCB1CTLW0 |= UCTR; // Put into transmit mode
    UCB1CTLW0 |= UCTXSTT; // Generate the start condition
}

bool CheckTempThreshold() { return false;}

//Delay tuned fairly close to 1ms * (passed int) for timing purposes. 
void delay_ms_(unsigned int ms){
    int i, j; 
    for(i=0; i<= ms; i++){
        __delay_cycles(1050);
    }
}

//Turns Off P2.0 & P2.1 on 2355, this turns switches off
void PeltierOff() {
    P2OUT &= ~PELTIER_HEAT; 
    P2OUT &= ~PELTIER_COOL;
}

//Turns on P2.1 after safety delay, this turns switch connected to heating on
void PeltierCool() {
    P2OUT &= ~PELTIER_HEAT; 
    delay_ms_(50);
    P2OUT |= PELTIER_COOL; 
}

//Turns on P2.0 after safety delay, this turns switch connected to cooling on
void PeltierHeat() {
    P2OUT &= ~PELTIER_COOL; 
    delay_ms_(50);
    P2OUT |= PELTIER_HEAT; 
}

//Will compare the current temperature to the set temperature, and turn on the appropriate switch
void PeltierMaintain() {
    //If the current temperature is less than the set temperature, turn on the heating switch. 
    if(AveragedTemp <= /*SELECTED temp reference (either LM19 or user select) - 1deg */){
        PeltierHeat(); 
    } else if (AveragedTemp >= /*SELECTED temp reference (either LM19 or user select) + 1deg */)
    {
        PeltierCool(); 
    } else {
        PeltierOff(); 
    }
    
    //If the current temperature is greater than the set temperature, turn on the cooling switch. 
    //If the current temperature is within 1 degree of the set temperature, turn off the switches. 

}


void LCDFormat() {}

//-- Interrupt Service Routines -----------------------------------------------------------
#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void) {
}

//-ISR Timer B---------------------------------------------------------------------------
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){
    switch (0b01000000 & State) {
        case 0:  break;
        case 128:  break;
        default:
            break;
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

