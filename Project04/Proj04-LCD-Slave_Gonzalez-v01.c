/*-----------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez
	Project 04 - Mar 03 2024

	Summary:

	Version Summary:
        v01: I2C as slave setup

    Ports Map: 
        P1.2 - I2C SDA
        P1.3 - I2C SCL


	Important Variables/Registers:
	

    MSP Errors:

	Todo:
		
-----------------------------------------------------------------------------*/

//----- Library/Header Includes -----------------------------------------------
#include "msp430fr2355.h"
#include <driverlib.h>
//-----------------------------------------------------------------------------

//----- MACRO definitions -----------------------------------------------------

//-----------------------------------------------------------------------------

//----- Global Variables ------------------------------------------------------

//-----------------------------------------------------------------------------

int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Port Initialization

    I2C_slaveRx();              //I2C as slave, slave address, and more initialziation

    __enable_interrupt();       //Enable Global interrupts
    PMM_unlockLPM5();           //Unlock Low Power Mode for DIO enable

    while(1)
    {

        // Delay
        for(i=10000; i>0; i--);
    }
}

//----- Subroutines & Function Definitions ------------------------------------
//- I2C_slaveRx: Initalizes the controller as a slave with address 0x00
int I2C_slaveRx(void){
    UCB0CTL1 |= UCSWRST;        //eUSCI-B0 software reset
    UCB0CTLW0 |= UCMODE_3;      //I2C slave mode 
    UCB0I2COA0 = 0X0012;        //Slave address
    // P2SEL |= 0x03;              //configure I2C pins (device specific)
    UCB0CTL1 &= ^UCSWRST;       //eUSCI-B0 in operational state 
    UCB0IE |= UCTXIE + UCRXIE;  //Enable Tx and Rx interrupt 
}//--END I2C_slaveRx-----------------------------------------------------------

//-----------------------------------------------------------------------------

//----- Interrupt Service Routines --------------------------------------------
//- eUSCI_B0 ISR --------------------------------------------------------------
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
}//--END eUSCI_B0 ISR----------------------------------------------------------

//-----------------------------------------------------------------------------