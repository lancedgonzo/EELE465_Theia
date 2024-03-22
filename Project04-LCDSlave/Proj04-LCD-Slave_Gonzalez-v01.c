/*-----------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez
	Project 04 - Mar 03 2024

	Summary:

	Version Summary:
        v01: I2C as slave setup
        v02: LCD communication

    Ports Map: 
        2310 LCD Slave
            P1.2 - I2C SDA  (PIN 16)
            P1.3 - I2C SCL  (PIN 15)

            P1.4 - DB0      (PIN 14)
            P1.5 - DB1      (PIN 13)
            P1.6 - DB2      (PIN 11)
            P1.7 - DB3      (PIN 10)

            P2.0 - E = 6    (PIN 9)
            P2.6 - RS = 4   (PIN 8)
            P2.7 - R/W = 5  (PIN 7) 


	Important Variables/Registers:
	

    MSP Errors:

	Todo:
		
-----------------------------------------------------------------------------*/

//----- Library/Header Includes -----------------------------------------------
#include <msp430.h>
// #include "msp430fr2355.h"
#include "msp430fr2310.h"

#include <stdint.h>
#include "_stdint.h"
// #include <msp430.h>
//-----------------------------------------------------------------------------

//----- MACRO definitions and Global Variables --------------------------------
//Port2
#define LCD_RS BIT6  // Register select pin 
#define LCD_EN BIT0  // Enable pin
#define LCD_RW BIT7  // Read/Write pin

//Port1
#define LCD_D4 BIT4  // Data line 4
#define LCD_D5 BIT5  // Data line 5
#define LCD_D6 BIT6  // Data line 6
#define LCD_D7 BIT7  // Data line 7

const clearDisplay =      0b00000001;
const returnHome =        0b00000010;
const entryMode =         0b00000110;
const displayCursorOn =   0b00001111; //cursor on and blink
const displayOn =         0b00001100; //cursor off and no blink
const displayOff =        0b00001000;
const fourBitMode  =      0b00101100; //function set NF
// char setDDRAM  =         0b10000000; // | this with whatever addy then driveChar

//-----------------------------------------------------------------------------

//----- Function Declarations -------------------------------------------------
void latch(); 
void LCDTx(); 

//-----------------------------------------------------------------------------

int main(void) {

    int i;

    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    LCD_init(); // Initialize LCD

    LCD_setCursor(0, 0); // Set cursor to first row, first column
    LCD_print("Hello, LCD!"); // Print message

    while (1){}

    return 0;
}


//----- Subroutines & Function Definitions ------------------------------------
// //- I2C_slaveRx: Initalizes the controller as a slave with address 0x00
// void I2C_slaveRx(void){
//     UCB0CTL1 |= UCSWRST;        //eUSCI-B0 software reset
//     UCB0CTLW0 |= UCMODE_3;      //I2C slave mode 
//     UCB0CTLW0 &= ~UCMST;           //again?
//     UCB0I2COA0 = 0X0012;        //Slave address
//     UCB0I2COA0 |= UCOAEN;       //Enables I2C own address
//     UCB0CTLW0 &= ~UCTR;         //clears transmit mode select bit
//     // P1SEL |= 0x06;              //configure I2C pins P1.2 P1.3
//     UCB0CTLW1 &= ~UCASTP1;      //clear auto stop bit
//     UCB0CTLW1 &= ~UCASTP0;

//     //Port Setup
//     P1SEL1 &= ~BIT3; 
//     P1SEL1 &= ~BIT2; 

//     P1SEL0 |= BIT3; 
//     P1SEL0 |= BIT2; 

//     // UCB0CTLW1 |= UCASTP_2;      //enable stop bit mode 2
//     UCB0CTL1 &= ~UCSWRST;       //eUSCI-B0 in operational state 

//     UCB0IE |= UCRXIE;  //Enable Tx and Rx interrupt 
// }//--END I2C_slaveRx-----------------------------------------------------------

void LCD_init() {
    //Port Initialization
    P2DIR |= (LCD_RS | LCD_EN | LCD_RW);  
    P1DIR |= (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7); // Set LCD control pins as output

    P2OUT &= ~(LCD_RS | LCD_EN | LCD_RW);
    P1OUT &= ~(LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7); // Initialize control pins to low

    delay(30);          // Wait >15ms after VDD rises to 4.5V
    LCD_command(0b00110011);
    LCD_command(0b00110010);

    LCD_command(fourBitMode); 
    LCD_command(displayOff); 
    LCD_command(clearDisplay);
    LCD_command(entryMode);
    LCD_command(displayOn); 

    LCD_data('H');

}

//-LCD Latch: -----------------------------------------------------------------
void latch(){
    P2OUT |= LCD_EN;      // set enable
    delay(50); 
    P2OUT &= ~LCD_EN;    // clear enable
    delay(50);
}//--END LCD latch ------------------------------------------------------------

void LCD_command(const command) {
    P2OUT &= ~LCD_RS; // Set RS low for command
    P1OUT = (command & 0xF0);
    latch();
    P1OUT = ((command & 0x0F) << 4);
    latch(); 
}

void LCD_data(char data) {
    P2OUT |= LCD_RS;  // Set RS high for data
    P1OUT = (data & 0xF0);
    latch(); 
    P1OUT = ((data & 0x0F) << 4);
    latch(); 
}


//-Delay: --------------------------------------------------------------------
void delay(unsigned int ms) {
    unsigned int i;
    for (i = 0; i < ms; i++){
        __delay_cycles(1000); // Assuming 1MHz clock
    }
}//--END LCD Transmit ---------------------------------------------------------


// //----- Interrupt Service Routines --------------------------------------------
// //- eUSCI_B0 ISR --------------------------------------------------------------
// #pragma vector = USCI_B0_VECTOR 
// __interrupt void USCI_B0_ISR(void) {
//     switch(__even_in_range(UCB0IV,0x1e))    {
//         case 0x00: break;   // No interrupts
//         case 0x02: break;   // Arbitration lost ALIFG
//         case 0x04: break;   // No Ack           NACKIFG
//         case 0x06: break;   // Start Recieved   STTIFG
//         case 0x08: break;   // Stop Recieved    STPIFG
//         case 0x0a: break;   // Slave 3 recieved RXIFG3
//         case 0x0c: break;   // Slave 3 transmit empty TXIFG3
//         case 0x0e: break;   // Slave 2 recieved RXIFG2
//         case 0x10: break;   // Slave 2 transmit empty TXIFG2
//         case 0x12: break;   // Slave 1 recieved RXIFG1
//         case 0x14: break;   // Slave 1 transmit empty TXIFG1
//         case 0x16: break;   // Data recieved RXIFG0
//         // case 0x18: txEmptyFlag = true; break;   // Transmit buffer empty TXIFG0
//         case 0x1a: break;   // byte counter zero BCNTIFG
//         case 0x1c: break;   // Vector 28: clock low time-out
//         case 0x1e: break;   // Vector 30: 9th bit
//         default:   break;
//     }
// }//--END eUSCI_B0 ISR----------------------------------------------------------

//-----------------------------------------------------------------------------
