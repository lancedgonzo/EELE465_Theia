/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 04 - Mar 02 2024

	Summary:

	Version Summary:
        v01:

    Ports Map: 
        I2C:
            P1.0 STE
            P1.1 CLK
            P1.2 SDA
            P1.3 SCL


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
#include <driverlib.h>
#include "gpio.h"
#include "msp430fr2355.h"

    // Data to be sent
    bool txEmptyFlag = true;
    uint8_t data = 0;
    uint8_t i = 0;
    void i2c_init();

int main(void) {
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    UCB0IE |= UCTXIE0;               // enable TX-interrupt
    GIE;                            // general interrupt enable


    i2c_init(); // Initialize I2C communication
    
    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();
    
    while (1) {
        i2c_init();
        UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start condition
        for (i = 0; i < 7; i++) {
            UCB0TXBUF = data;   // Send byte
            while (~txEmptyFlag) {}
            data += 3; // Increment data by 3 for next transmission
        }
        UCB0CTL1 |= UCTXSTP; // I2C stop condition

    }
}


void i2c_init() {
    UCB0CTL1 |= UCSWRST;            // put eUSCI_B in reset state
    UCB0CTLW0 |= UCMODE_3 + UCMST;  // I2C master mode
    UCB0BRW = 0x0008;               // baud rate = SMCLK / 8
    UCB0CTLW1 = UCASTP_2;           // automatic STOP assertion
    UCB0TBCNT = 0x07;               // TX 7 bytes of data
    UCB0I2CSA = 0x0012;             // address slave is 12hex
    P1SEL1 |= 0x00C;                 // configure I2C pins (device specific)
    UCB0CTL1 &= ~UCSWRST;           // eUSCI_B in operational state
}

#pragma vector = USCI_B0_VECTOR 
__interrupt void USCI_B0_ISR(void) {
    switch(__even_in_range(UCB0IV,0x1e))    {
        case 0x00: break;   // No interrupts
        case 0x02: break;   // Arbitration lost ALIFG
        case 0x04: break;   // No Ack           NACKIFG
        case 0x06: break;   // Start Recieved   STTIFG
        case 0x08: break;   // Stop Recieved    STPIFG
        case 0x0a: break;   // Slave 3 recieved RXIFG3
        case 0x0c: break;   // Slave 3 transmit empty TXIFG3
        case 0x0e: break;   // Slave 2 recieved RXIFG2
        case 0x10: break;   // Slave 2 transmit empty TXIFG2
        case 0x12: break;   // Slave 1 recieved RXIFG1
        case 0x14: break;   // Slave 1 transmit empty TXIFG1
        case 0x16: break;   // Data recieved RXIFG0
        case 0x18: txEmptyFlag = true; break;   // Transmit buffer empty TXIFG0
        case 0x1a: break;   // byte counter zero BCNTIFG
        case 0x1c: break;   // Vector 28: clock low time-out
        case 0x1e: break;   // Vector 30: 9th bit
        default:   break;
    }
}