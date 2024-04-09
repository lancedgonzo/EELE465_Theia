#include <i2c.h>

#include "driverlib.h"

uint8_t TransmitCounter = 0;

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

void TransmitLCD() {
    TransmitCounter = 32;
    UCB1TBCNT = 32;
    UCB1I2CSA = LCD_Address; // Set the slave address in the module equal to the slave address
    UCB1CTLW0 |= UCTR; // Put into transmit mode
    UCB1CTLW0 |= UCTXSTT; // Generate the start condition
}

void TransmitLED() {
    TransmitCounter = 2;
    UCB1TBCNT = 2;
    UCB1I2CSA = LED_Address; // Set the slave address in the module equal to the slave address
    UCB1CTLW0 |= UCTR; // Put into transmit mode
    UCB1CTLW0 |= UCTXSTT; // Generate the start condition
}

void TransmitRTC() {

}

void ReceiveRTC() {

}

void LCDFormat() {}

#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void) {
    switch(TransmitState & 0b00001111) {
        case 1: // LCD
        case 2: // LED
            if (TransmitCounter == 2) {
                UCB1TXBUF = HeatCool;
            } else {
                UCB1TXBUF = LastButton;
            }
            TransmitCounter--;
            break;
        case 4: // RTC
        case 8: // ADC

    }
    if (TransmitCounter == 0) {
        TransmitState &= 0b11110000;
    }
}
