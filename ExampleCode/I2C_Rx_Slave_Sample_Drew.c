/*-------------------------------------------------------------------------------
 I2C Slave receive code
 Author: Drew Currie
 Date Created: 02/10/2024
 Last date editted: 02/17/204
 Purpose:
 Receive an ASCII value of either R, G, or B and change the output color of
 the RGB LED accordingly.
 -------------------------------------------------------------------------------*/
#include "msp430fr2310.h"
#include <msp430.h>

 volatile char DataIn = '0';
 int main(void) {
 WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

 // -- 1. Put eUSC_B0 into software reset
 UCB0CTLW0 |= UCSWRST; // SW reset

// -- 2. Configure eUSCI_B0
 /*
 Put EUSCI0 into I2C mode, put into slave mode, set I2C address, and
 enable own address.
 */
 UCB0CTLW0 |= UCMODE_3;
 UCB0CTLW0 &= ~UCMST;
 UCB0I2COA0 = 0x0048;
 UCB0I2COA0 |= UCOAEN;
 UCB0CTLW0 &= ~UCTR;
 UCB0CTLW1 &= ~UCASTP0;
 UCB0CTLW1 &= ~UCASTP1;

 // -- 3. Configure ports
 /*
 Set P1.3 as I2C Clock, set P1.2 as I2C Data
 */
 P1SEL1 &= ~BIT3;
 P1SEL0 |= BIT3;

 P1SEL1 &= ~BIT2;
 P1SEL0 |= BIT2;
 PM5CTL0 &= ~LOCKLPM5; // Turn on GPIO

 // -- 4. Take EUSCI_B0 out of software reset
 UCB0CTLW0 &= ~UCSWRST;

 // RGB LED pins
 // P2.0 -> Red, P1.7 -> Green, P1.6 -> Blue
 P1DIR |= BIT7;
 P1DIR |= BIT6;
 P2DIR |= BIT0;
 // -- 5. Enable local I2C receive interrupt and global interrupts


 UCB0IE |= UCRXIE0; // ENABLE I2C Rx0
 __enable_interrupt(); // enable maskable IRQs

 while (1) // Main loop
 {
 // Determine which ASCII character was received from the master
 switch (DataIn) {
 case 'R':
 P1OUT &= ~BIT7;
 P1OUT &= ~BIT6;
 P2OUT |= BIT0;
 break;
 case 'G':
 P2OUT &= ~BIT0;
 P1OUT &= ~BIT6;
 P1OUT |= BIT7;
 break;
 case 'B':
 P2OUT &= ~BIT0;
 P1OUT &= ~BIT7;
 P1OUT |= BIT6;
 break;
 default:
 break;
 }
 UCB0IE |= UCRXIE0; // Enable I2C Rx0
 }
 return 0;
 }

 /*
 Begin I2C Interrupt Service Routine
 -Receive data from master and save in the DataIn variable.
 */

 #pragma vector = EUSCI_B0_VECTOR // Triggers when RX buffer is ready for data,
 // after start and ack
 __interrupt void EUSCI_B0_I2C_ISR(void) {
 DataIn = UCB0RXBUF; // Store data in variable