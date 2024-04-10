/*----------------------------------------------------------------------------------------------------------------------
    EELE465
    Written by: Lance Gonzalez, Grant Kirkland, Ken Vincent
    Working:
    Project 06 - Mar 31 2024

    Summary:

    Version Summary:
        v01:

    In Circuit Programming - 2310
        P3: SBWTCK
        P4: SBWTDIO
        P5: DVCC, 3v3
        P6: DVSS, GND

    Ports Map:
        P1.2 SDA  16
        P1.3 SCL  15

        P1.0 LED0 2
        P1.1 LED1 1
        P2.0 LED2 9
        P2.7 LED3 7
        P1.4 LED4 14
        P1.5 LED5 13
        P1.6 LED6 11
        P1.7 LED7 10

        P2.6 N/C  8


-----------------------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "msp430fr2310.h"
#include <driverlib.h>

#include <msp430.h>
#include <stdint.h>
#include "stdbool.h"
#include <gpio.h>

// Defining row and column input as keys

#define LED_Address 0x013

#define KEY_AST 0x081 // 129
#define KEY_A 0x018 // 24
#define KEY_B 0x028 // 40
#define KEY_C 0x048 // 72
#define KEY_D 0x088 // 136

#define P0 GPIO_PIN0
#define P1 GPIO_PIN1
#define P2 GPIO_PIN2
#define P3 GPIO_PIN3
#define P4 GPIO_PIN4
#define P5 GPIO_PIN5
#define P6 GPIO_PIN6
#define P7 GPIO_PIN7

void PatternOff();
void PatternHeat();
void PatternCool();
void PatternSetpointHeat();
void PatternSetpointCool();
void UpdateLED();

volatile uint8_t LED_Out = 0;
volatile int8_t PatternHeatCounter = 0;
volatile int8_t PatternCoolCounter = 0;
volatile int8_t PatternOffCounter = 0;
volatile int8_t PatternsetCoolCounter = 0;
volatile int8_t PatternsetHeatCounter = 0;
volatile int8_t state = 0;
volatile bool TimerFlag = false;
volatile bool Timer2Flag = false;
volatile bool HeatCool = false;

int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Setup I2C as slave
    UCB0CTLW0 |= UCSWRST; // SW reset
    UCB0CTLW0 |= UCMODE_3;
    UCB0CTLW0 &= ~UCMST;
    UCB0I2COA0 = LED_Address;
    UCB0I2COA0 |= UCOAEN;
    UCB0CTLW0 &= ~UCTR;
    UCB0CTLW1 &= ~UCASTP0;
    UCB0CTLW1 &= ~UCASTP1;
    UCB0CTLW0 &= ~UCSWRST;

    P1SEL0 = 0x00;
    P1SEL1 = 0x00;
    P2SEL0 = 0x00;
    P2SEL1 = 0x00;
    P1SEL1 &= ~BIT3;
    P1SEL0 |= BIT3;
    P1SEL1 &= ~BIT2;
    P1SEL0 |= BIT2;

    // Setup pins as output
    GPIO_setAsOutputPin(1, P0);
    GPIO_setAsOutputPin(1, P1);
    GPIO_setAsOutputPin(1, P4);
    GPIO_setAsOutputPin(1, P5);
    GPIO_setAsOutputPin(1, P6);
    GPIO_setAsOutputPin(1, P7);
    GPIO_setAsOutputPin(2, P0);
    GPIO_setAsOutputPin(2, P7);


    PM5CTL0 &= ~LOCKLPM5;
    UCB0IE |= UCRXIE0;
    __enable_interrupt();
    LED_Out = 0x000;

    // set clock for 3 Hz
    TB0CTL |= TBCLR + TBSSEL__ACLK + MC__UP + ID__1;
    TB0CCR0 = 5461;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;
    TB0R = 0;

    while (1) {
        if (TimerFlag) {
            TimerFlag = false;
            switch(state) {
                case 0: PatternOff(); break;
                case 1: PatternHeat(); break;
                case 2: PatternCool(); break;
                case 3:
                    if (HeatCool)
                        PatternSetpointHeat();
                    else
                        PatternSetpointCool();
                    break;
            }
        }
        UpdateLED();
    }
}

//-Update LED: -------------------------------------------------------------------------
void UpdateLED() {
    // Move LED_Out to led ports
    P1OUT = ((LED_Out & BIT0) == BIT0) ? (P1OUT | BIT0) : (P1OUT & ~BIT0);
    P1OUT = ((LED_Out & BIT1) == BIT1) ? (P1OUT | BIT1) : (P1OUT & ~BIT1);
    P2OUT = ((LED_Out & BIT2) == BIT2) ? (P2OUT | BIT7) : (P2OUT & ~BIT7);
    P2OUT = ((LED_Out & BIT3) == BIT3) ? (P2OUT | BIT0) : (P2OUT & ~BIT0);
    P1OUT = ((LED_Out & BIT4) == BIT4) ? (P1OUT | BIT4) : (P1OUT & ~BIT4);
    P1OUT = ((LED_Out & BIT5) == BIT5) ? (P1OUT | BIT5) : (P1OUT & ~BIT5);
    P1OUT = ((LED_Out & BIT6) == BIT6) ? (P1OUT | BIT6) : (P1OUT & ~BIT6);
    P1OUT = ((LED_Out & BIT7) == BIT7) ? (P1OUT | BIT7) : (P1OUT & ~BIT7);
}//End UpdateLED --------------------------------------------------------------------

//-PatternHeat: Scrolling up pattern------------------------------------------------------------
void PatternHeat() {
    switch(PatternHeatCounter) {
        case 0: LED_Out = 0x01; break;
        case 1: LED_Out = 0x03; break;
        case 2: LED_Out = 0x07; break;
        case 3: LED_Out = 0x0F; break;
        case 4: LED_Out = 0x1F; break;
        case 5: LED_Out = 0x3F; break;
        case 6: LED_Out = 0x7F; break;
        case 7: LED_Out = 0xFF; break;
        case 8: LED_Out = 0x00; PatternHeatCounter = -1; break;
    }
    PatternHeatCounter++;
}//End PatternHeat --------------------------------------------------------------------

void PatternCool() {
    switch(PatternCoolCounter) {
        case 0: LED_Out = 0b10000000; break;
        case 1: LED_Out = 0b11000000; break;
        case 2: LED_Out = 0b11100000; break;
        case 3: LED_Out = 0b11110000; break;
        case 4: LED_Out = 0b11111000; break;
        case 5: LED_Out = 0b11111100; break;
        case 6: LED_Out = 0b11111110; break;
        case 7: LED_Out = 0b11111111; break;
        case 8: LED_Out = 0b00000000; PatternCoolCounter = -1; break;
    }
    PatternCoolCounter++;
}

void PatternSetpointHeat() {
    switch(PatternsetHeatCounter) {
            case 0: LED_Out = 0b11111111; break;
            case 1: LED_Out = 0b11100111; break;
            case 2: LED_Out = 0b11000011; break;
            case 3: LED_Out = 0b10000001; break;
            case 4: LED_Out = 0b00000000; break;
            case 5: LED_Out = 0b10000001; break;
            case 6: LED_Out = 0b11000011; break;
            case 7: LED_Out = 0b11100111; break;
            case 8: LED_Out = 0b00000000; PatternsetHeatCounter = -1; break;
    }
    PatternsetHeatCounter++;
}

void PatternSetpointCool() {
    switch(PatternsetCoolCounter) {
            case 0: LED_Out = 0b11111111; break;
            case 1: LED_Out = 0b01111110; break;
            case 2: LED_Out = 0b00111100; break;
            case 3: LED_Out = 0b00011000; break;
            case 4: LED_Out = 0b00111100; break;
            case 5: LED_Out = 0b01111110; break;
            case 6: LED_Out = 0b00111100; break;
            case 7: LED_Out = 0b11111111; break;
            case 8: LED_Out = 0b00000000; PatternsetCoolCounter = -1; break;
    }
    PatternsetCoolCounter++;
}

void PatternOff() {
    switch(PatternOffCounter) {
            case 0: LED_Out = 0b10101010; break;
            case 1: LED_Out = 0b01010101; PatternOffCounter = -1; break;
    }
    PatternOffCounter++;
}

//-ISR Timer B---------------------------------------------------------------------------
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){
    TimerFlag = true;
    // Clear interrupt flag
    TB0CCTL0 &= ~CCIFG;
}//-- End Timer_B_ISR ------------------------------------------------------------------

// Triggers when RX buffer is ready for data, after start and ack
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void) {

    switch (UCB0RXBUF) {
        case 0:
            HeatCool = false;
            break;
        case 1:
            HeatCool = true;
            break;
        case 'A':
            state = 1;
            break;
        case 'B':
            state = 2;
            break;
        case 'C':
            state = 3;
            break;
        case 'D':
            state = 0;
            break;
    }
}
