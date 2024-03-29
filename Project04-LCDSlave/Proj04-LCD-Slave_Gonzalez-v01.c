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
#include "msp430fr2310.h"

// #include <stdint.h>
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

const char clearDisplay =      0b00000001;
const char returnHome =        0b00000010;
const char entryMode =         0b00000110;
const char displayCursorOn =   0b00001111; //cursor on and blink
const char displayOn =         0b00001100; //cursor off and no blink
const char displayOff =        0b00001000;
const char fourBitMode  =      0b00101100; //function set NF
const char setDDRAM  =         0b10000000; // | this with whatever addy then driveChar

//-----------------------------------------------------------------------------

//----- Function Declarations -------------------------------------------------
void latch(); 
void LCD_init();
void LCD_command(char); 
void LCD_data(char);  
void delay(); 
void I2C_slaveRx(); 

//-----------------------------------------------------------------------------

int main(void) {

    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // I2C_slaveRx(); //Initialize I2C
    LCD_init(); // Initialize LCD

    while (1){}

    return 0;
}


//----- Subroutines & Function Definitions ------------------------------------
//- I2C_slaveRx: Initalizes the controller as a slave with address 0x00
void I2C_slaveRx(void){
    // UCB0CTLW0 |= UCSWRST;        //eUSCI-B0 software reset
    // UCB0CTLW0 |= UCMODE_3;      //I2C slave mode 
    // UCB0CTLW0 &= ~UCMST;           //again?
    // UCB0CTLW0 &= ~UCTR;         //clears transmit mode select bit
    // UCB0I2COA0 = 0X046;        //Slave address
    // UCB0I2COA0 |= UCOAEN;       //Enables I2C own address
    // UCB0CTLW1 &= ~UCASTP1;      //clear auto stop bit
    // UCB0CTLW1 &= ~UCASTP0;

    // //Port Setup
    // P1SEL1 &= ~BIT3; 
    // P1SEL1 &= ~BIT2; 

    // P1SEL0 |= BIT3; 
    // P1SEL0 |= BIT2; 

    // UCB0CTLW0 |= UCASTP_2;      //enable stop bit mode 2
    // UCB0CTLW0 &= ~UCSWRST;       //eUSCI-B0 in operational state 

    // UCB0IE |= UCRXIE0;  //Enable Tx and Rx interrupt 
    __enable_interrupt(); 
}//--END I2C_slaveRx-----------------------------------------------------------

void LCD_init(){
    //Port Initialization
    P2DIR |= (LCD_RS | LCD_EN | LCD_RW);  
    P1DIR |= (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7); // Set LCD control pins as output

    P2OUT &= ~(LCD_RS | LCD_EN | LCD_RW);
    P1OUT &= ~(LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7); // Initialize control pins to low
    PM5CTL0 &= ~LOCKLPM5;

    delay(30);          // Wait 

    LCD_command(0x00 | setDDRAM);

    LCD_command(fourBitMode); 
    LCD_command(displayOff); 
    LCD_command(clearDisplay);
    LCD_command(entryMode);
    LCD_command(displayOn); 
    LCD_command(0x00 | setDDRAM);

    LCD_data('H');
    // LCD_data('Y');
    LCD_command(0x40 | setDDRAM);
}

//-LCD Latch: -----------------------------------------------------------------
void latch(){
    P2OUT |= LCD_EN;      // set enable
    delay(50); 
    P2OUT &= ~LCD_EN;    // clear enable
    delay(50);
}//--END LCD latch ------------------------------------------------------------

void LCD_command(char command){
    P2OUT &= ~LCD_RS; // Set RS low for command
    P1OUT = (command & 0xF0);
    latch();
    P1OUT = ((command & 0x0F) << 4);
    latch(); 
}

void LCD_data(char data){
    LCD_command(entryMode);

    P2OUT |= LCD_RS;  // Set RS high for data
    P1OUT = (data & 0xF0);
    latch(); 
    P1OUT = ((data & 0x0F) << 4);
    latch(); 
}


//-Delay: --------------------------------------------------------------------
void delay(unsigned int ms){
    unsigned int i;
    for (i = 0; i < ms; i++){
        __delay_cycles(1000); // Assuming 1MHz clock
    }
}//--END Delay --------------------------------------------------------------


// Interrupt service routine for I2C
// #pragma vector = EUSCI_B0_VECTOR 
// __interrupt void EUSCI_B0_I2C_ISR(void) {
//     UCB0IFG &= ~UCRXIFG; // Clear I2C Rx interrupt flag
// }
//-----------------------------------------------------------------------------

