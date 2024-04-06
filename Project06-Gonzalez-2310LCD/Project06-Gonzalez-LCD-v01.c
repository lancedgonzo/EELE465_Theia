/*----------------------------------------------------------------------------------------------------------------------
    EELE465
    Written by: Lance Gonzalez, Grant Kirkland
    Working:
    Project 05

    Summary:

    Version Summary:
        v01: Working Project 04
        v02: Output 32 chars

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


-----------------------------------------------------------------------------------------------------------------------*/

#include "msp430fr2310.h"
#include <msp430.h>
#include "stdint.h"

#define EntryMode       0b00000110
#define DisplayClear    0b00000001
#define DisplayON       0b00001111
#define FunctionSet     0b00101100
#define HOME            0b00000010
#define Set_DDR_2       0b11000000

#define Command         0b11
#define Printing        0b10


// Global variables
char start[32] = {'E', 'n', 't', 'e', 'r', ' ', 'n', ':', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'T', ' ', '=', ' ', ' ', ' ', 'K', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0xDF, 'C'};
char data_in[32];
int s = 0;
int loop;
int input;
int printput;
int data_count = 0;

void INIT(void);
void LCD_WriteData(unsigned char data);
void Print_LCD(unsigned char data);
void new_sentence(void);
void start_sentence(void);
void latch(void);

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    INIT();
    PM5CTL0 &= ~LOCKLPM5;
    UCB0IE |= UCRXIE0;
    __enable_interrupt();

    while(1){
        for(loop = 0; loop < 1575; loop++){}     // 30 ms loop

        //Initialize the LCD screen
        LCD_WriteData(HOME);
        LCD_WriteData(FunctionSet);
        LCD_WriteData(DisplayON);
        start_sentence();

        while(1){}
    }

    return 0;
}

void INIT(void){
    // - setup i2c communication
    UCB0CTLW0 |= UCSWRST;


    UCB0CTLW0 |= UCMODE_3;
    UCB0CTLW0 &= ~UCMST;
    UCB0CTLW0 &= ~UCTR;
    UCB0I2COA0 = 0x046;
    UCB0I2COA0 |= UCOAEN;
    UCB0CTLW1 &= ~UCASTP0;
    UCB0CTLW1 &= ~UCASTP1;
    UCB0CTLW0 &= ~UCSWRST;


    UCB0CTLW1 |= UCASTP_2;

    // - configure ports
    P1SEL1 &= ~BIT3;       // P1.2 AS SDA
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;        //P1.3 AS SCL
    P1SEL0 |= BIT2;

    P1DIR |= BIT4;          // P1.4 AS DB4 and DB0
    P1OUT &= ~BIT4;

    P1DIR |= BIT5;          // P1.5 AS DB5 and DB1
    P1OUT &= ~BIT5;

    P1DIR |= BIT6;          // P1.6 AS DB6 and DB2
    P1OUT &= ~BIT6;

    P1DIR |= BIT7;          // P1.7 AS DB7 and DB3
    P1OUT &= ~BIT7;

    P2DIR |= BIT0;          //P2.0 AS E
    P2OUT &= ~BIT0;

    P2DIR |= BIT6;          //P2.6 AS RS
    P2OUT &= ~BIT6;

    P2DIR |= BIT7;          //P2.7 AS R/W
    P2OUT &= ~BIT7;
}

void LCD_WriteData(unsigned char data){

    //Set or clear RS & R/W
    P2OUT &= ~BIT6;
    P2OUT &= ~BIT7;

    //P1output to match upper nibble
    P1OUT = (data & 0xF0);

    //Latch
    latch();

    //P1output to match lower nibble
    P1OUT = (data & 0x0F) << 4;

    latch();

}

void Print_LCD(unsigned char data){
     //Set or clear RS & R/W
    P2OUT |= BIT6;
    P2OUT &= ~BIT7;

    //P1output to match upper nibble
    P1OUT = (data & 0xF0);

    //Latch
    latch();

    //P1output to match lower nibble
    P1OUT = (data & 0x0F) << 4;

    latch();
}

void new_sentence(){
    //display_clear();
    // home();
    LCD_WriteData(HOME);

    //display first sentence
    for(s = 0; s < 16; s++){
        printput = data_in[s];
        // print(printput);
        Print_LCD(printput);
    }

    //switch to second row
    LCD_WriteData(Set_DDR_2);

    //display second sentence
    for(s = 16; s < 32; s++){
        printput = data_in[s];
        // print(printput);
        Print_LCD(printput);
    }

}

void start_sentence(){
    //display_clear();
    // home();
    LCD_WriteData(HOME);

    //display first sentence
    for(s = 0; s < 16; s++){
        printput = start[s];
        // print(printput);
        Print_LCD(printput);
    }

    //switch to second row
    LCD_WriteData(Set_DDR_2);

    //display second sentence
    for(s = 16; s < 32; s++){
        printput = start[s];
        // print(printput);
        Print_LCD(printput);
    }
}

void latch(){
    // set enable
    P2OUT |= BIT0;
    for(loop = 0; loop < 52; loop++){} // 1 ms loop
    // clear enable
    P2OUT &= ~BIT0;
}

// ------------------------ interrupt service routines ------------------
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){

// collect data array one bit at a time
    if(data_count == 32){
        data_count = 0;
        data_in[30] = 0xDF;
        new_sentence();
    }
    else{
        data_in[data_count] = UCB0RXBUF;
        data_count++;
    }
}
