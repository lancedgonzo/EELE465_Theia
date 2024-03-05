#include <msp430.h>
 #include <stdint.h>

 #define Slave_Address 0x048

 const char I2C_Message[] = {'R', 'G', 'B'};
 volatile unsigned int ColorIndex;
int main(void) {

 volatile uint32_t i;


 // Stop watchdog timer
 WDTCTL = WDTPW | WDTHOLD;

 /*
 Configure I2C for master transmit mode
 */
 UCB1CTLW0 |= UCSWRST; // Put UCB1CTLW0 into software reset
 UCB1CTLW0 |= UCSSEL_3; // Select mode 3
 UCB1BRW = 10; // Something useful

 UCB1CTLW0 |= UCMODE_3; // Mode 3
 UCB1CTLW0 |= UCMST; // Master
 UCB1CTLW0 |= UCTR; // Transmit mode

 UCB1CTLW1 |= UCASTP_2; // Autostop enabled

 //----- P4.6 and P4.7 for I2C ---
 P4SEL1 &= ~BIT7;
 P4SEL0 |= BIT7;

 P4SEL1 &= ~BIT6;
 P4SEL0 |= BIT6;
 //--------------------------
 PM5CTL0 &= ~LOCKLPM5; // Take out of low power mode

 UCB1CTLW0 &= ~UCSWRST; // Take out of Software Reset

 UCB1IE |= UCTXIE0; // Enable TX interrupt
 UCB1IE |= UCRXIE0; // Enable RX interrupt

 __enable_interrupt();
 /*
 End I2C Setup
 */
 while (1) {

 /*Transmit Slave address and one byte of data*/
 UCB1TBCNT = 1;
 UCB1I2CSA = Slave_Address; // Set the slave address in the module
 //...equal to the slave address
 UCB1CTLW0 |= UCTR; // Put into transmit mode
 UCB1CTLW0 |= UCTXSTT; // Generate the start condition
 for (i = 65000; i > 0; i--) {
 // Delay
 }
 for (i = 65000; i > 0; i--) {
 // Delay
 }
 for (i = 65000; i > 0; i--) {
 // Delay
 }
 for (i = 65000; i > 0; i--) {
 // Delay
 }
 for (i = 65000; i > 0; i--) {
 // Delay
 }

 for (i = 65000; i > 0; i--) {
 // Delay
 }
 ColorIndex++;
 if (ColorIndex > 2) {
 ColorIndex = 0;
 }
 }
 }

 #pragma vector = EUSCI_B1_VECTOR
 __interrupt void EUSCI_B1_I2C_ISR(void) {
 UCB1TXBUF = I2C_Message[ColorIndex]; // Send the next byte in the
 // I2C_Message_Global string
 }
