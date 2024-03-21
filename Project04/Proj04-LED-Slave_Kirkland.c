/*----------------------------------------------------------------------------------------------------------------------
    EELE465
    Written by: Lance Gonzalez, Grant Kirkland
    Working:
    Project 04 - Mar 02 2024

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


    Important Variables/Registers:
        Button 0x76543210
            0-3 Col
            4-7 Row
        LED_Out
            1:1 LED

    MSP Errors:
        P4.5 Inconsistent Input?
    Todo:

-----------------------------------------------------------------------------------------------------------------------*/
#include "gpio.h"
#include "msp430fr2310.h"
#include <msp430.h>
#include <stdint.h>

// Defining row and column input as keys

#define KEY_AST 0x081 // 129
#define KEY_A 0x018 // 24
#define KEY_B 0x028 // 40
#define KEY_C 0x048 // 72
#define KEY_D 0x088 // 136
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

void PatternAUpdate();
void PatternBUpdate();
void PatternCUpdate();
void PatternDUpdate();
void UpdateLED();

uint8_t LED_Out = 0;
volatile uint8_t PatternCounter = 0;
volatile uint8_t state = 0b00000000;

int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    UCB0CTLW0 |= UCSWRST; // SW reset
    UCB0CTLW0 |= UCMODE_3;
    UCB0CTLW0 &= ~UCMST;
    UCB0I2COA0 = 0x0048;
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

    GPIO_setAsOutputPin(Port1, P0);
    GPIO_setAsOutputPin(Port1, P1);
    GPIO_setAsOutputPin(Port1, P4);
    GPIO_setAsOutputPin(Port1, P5);
    GPIO_setAsOutputPin(Port1, P6);
    GPIO_setAsOutputPin(Port1, P7);
    GPIO_setAsOutputPin(Port2, P0);
    GPIO_setAsOutputPin(Port2, P7);


    PM5CTL0 &= ~LOCKLPM5;
    UCB0IE |= UCRXIE0;
    __enable_interrupt();
    LED_Out = 0x000;
    //UpdateLED();
   // LED_Out = 0x0AA;


    while (1) {
        if (state & 0b00001000) {
            state &= ~0b00001000;
            switch(state & 0b00000111) {
                case 0b00000000: LED_Out = 0x00; state += 0b00000100; break;
                case 0b00000001: LED_Out = 0x00; state += 0b00000100; break;
                case 0b00000010: LED_Out = 0x00; state += 0b00000100; break;
                case 0b00000011: LED_Out = 0x0F; state += 0b00000100; break;
                case 0b00000100: PatternAUpdate(); break;
                case 0b00000101: PatternBUpdate(); break;
                case 0b00000110: PatternCUpdate(); break;
                case 0b00000111: PatternDUpdate(); break;
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


//-PatternA: Static pattern XOXOXOXO---------------------------------------------------------------
void PatternAUpdate(){
    //Set High on Collumns
    LED_Out = 0x0AA;
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
    PatternCounter += 1;
    LED_Out = PatternCounter;
}//--END PatternB--------------------------------------------------------------------------------

//-PatternC: Scrolling Pattern ---------------------------------------------------------------
void PatternCUpdate(){
    // Set timer for 2s
    TB0CTL |= TBCLR + TBSSEL__ACLK + MC__UP + ID__1;
    TB0CCR0 = 32768;
    TB0R = 0;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;

    // Count and reset on rollover
    switch(PatternCounter) {
        case 0: LED_Out = 0x7F; break;
        case 1: LED_Out = 0xBF; break;
        case 2: LED_Out = 0xDF; break;
        case 3: LED_Out = 0xEF; break;
        case 4: LED_Out = 0xF7; break;
        case 5: LED_Out = 0xFB; break;
        case 6: LED_Out = 0xFD; break;
        case 7: LED_Out = 0xFE; break;
    }
    PatternCounter += 1;
    if (PatternCounter >= 8){
        PatternCounter = 0;
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
    switch(state & 0b01110000) {
        case 0b00000000:
            LED_Out = 0x018;
            state += 0b00010000;
            break;
        case 0b00010000: LED_Out = 0x024; state += 0b00010000; break;
        case 0b00100000: LED_Out = 0x042; state += 0b00010000; break;
        case 0b00110000: LED_Out = 0x081; state += 0b00010000; break;
        case 0b01000000: LED_Out = 0x042; state += 0b00010000; break;
        case 0b01010000: LED_Out = 0x024; state -= 0b01010000; break;
    }
}//--END PatternD--------------------------------------------------------------------------------

//-ISR Timer B---------------------------------------------------------------------------
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){
    state |= 0b00001000;
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


#pragma vector = EUSCI_B0_VECTOR // Triggers when RX buffer is ready for data,
// after start and ack
__interrupt void EUSCI_B0_I2C_ISR(void) {
    LED_Out = UCB0RXBUF;

    TB0CTL |= TBCLR + TBSSEL__ACLK + MC__UP + ID__1;
    TB0CCR0 = 16384;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;
    switch (LED_Out) {
        case KEY_A:
            state &= 0b11111000; state |= 0b00000000; break;
        case KEY_B: PatternCounter = state == KEY_B ? 0:PatternCounter;
            state &= 0b11111000; state |= 0b00000001; break;
        case KEY_C: PatternCounter = state == KEY_C ? 0:PatternCounter;
            state &= 0b11111000; state |= 0b00000010; break;
        case KEY_D:
            state &= 0b11111000; state |= 0b00000011; break;
    }
}
