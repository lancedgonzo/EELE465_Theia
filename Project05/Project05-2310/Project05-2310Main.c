#include "gpio.c"
#include "msp430fr2310.h"
#include <msp430.h> 
#include <stdint.h>


#define LED_Address 0x013

#define P0 GPIO_PIN0
#define P1 GPIO_PIN1
#define P2 GPIO_PIN2
#define P3 GPIO_PIN3
#define P4 GPIO_PIN4
#define P5 GPIO_PIN5
#define P6 GPIO_PIN6
#define P7 GPIO_PIN7

int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Initialize slave with
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
    while (1) {

    }
}

#pragma vector = EUSCI_B0_VECTOR // Triggers when RX buffer is ready for data,
// after start and ack
__interrupt void EUSCI_B0_I2C_ISR(void) {
}
