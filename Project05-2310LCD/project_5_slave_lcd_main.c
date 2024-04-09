#include <msp430.h> 

/**
 *      Created on: Mar 5, 2024
 *      Credit: Drew Currie
 *      Credit: datasheets for LCD
 *      Credit: data sheet for NHD-0216HZ-FSW-FBW-33V3C
 *      Credit: datasheet for 2310
 *      Author: Kenneth
 */


#include "msp430fr2310.h"
#include <msp430.h>
char start[32] = {'E', 'n', 't', 'e', 'r', ' ', 'n', ':', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'T', ' ', '=', ' ', ' ', ' ', ' ', 'K', ' ', ' ', ' ', ' ', ' ', ' ', 0xDF, 'C'};
int s = 0;
char data_in[32];
int key = 0;
int loop;
int input;
int printput;
int data_count = 0;
/**
 * main.c
 */
int main(void){
WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

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


PM5CTL0 &= ~LOCKLPM5;



UCB0IE |= UCRXIE0;
__enable_interrupt();


    while(1){
        for(loop = 0; loop < 1575; loop++){} // 30 ms loop

        //intialization start
        //display_clear();
        home();
        functionset();
        display_on();

        start_sentance();

        while(1){}
    }
    return 0;
}


void set_ddr_2nd_row(){
    //clear RS and R/W
    P2OUT &= ~BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT |= BIT7;
    P1OUT |= BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    latch();
}


void new_sentance(){
    //display_clear();
    home();

    //display first sentence
    for(s = 0; s < 16; s++){
        printput = data_in[s];
        print(printput);
    }

    //switch to second row
    set_ddr_2nd_row();

    //display second sentence
    for(s = 16; s < 32; s++){
        printput = data_in[s];
        print(printput);
    }

}

void start_sentance(){
    //display_clear();
    home();

    //display first sentence
    for(s = 0; s < 16; s++){
        printput = start[s];
        print(printput);
    }

    //switch to second row
    set_ddr_2nd_row();

    //display second sentence
    for(s = 16; s < 32; s++){
        printput = start[s];
        print(printput);
    }

}


void print(int printput){
    entry_mode();

    //set RS and clear R/W
    P2OUT |= BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    //mask upper niddle
    P1OUT = (printput & 0b11110000);

    latch();

    //SETUP DB3 - DB0
    //mask lower nibble
    P1OUT = (printput & 0b00001111) << 4;

    latch();
}



void home(){
    //clear RS and R/W
    P2OUT &= ~BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT |= BIT5;
    P1OUT &= ~BIT4;

    latch();
}



void latch(){
    // set enable
    P2OUT |= BIT0;
    for(loop = 0; loop < 52; loop++){} // 1 ms loop
    // clear enable
    P2OUT &= ~BIT0;
}

void functionset(){
    //clear RS and R/W
    P2OUT &= ~BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT |= BIT5;
    P1OUT &= ~BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT |= BIT7;
    P1OUT |= BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    latch();
}

void entry_mode(){
    //clear RS and R/W
    P2OUT &= ~BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT &= ~BIT7;
    P1OUT |= BIT6;
    P1OUT |= BIT5;
    P1OUT &= ~BIT4;

    latch();
}

void display_on(){
    //clear RS and R/W
    P2OUT &= ~BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT |= BIT7;
    P1OUT |= BIT6;
    P1OUT |= BIT5;
    P1OUT |= BIT4;

    latch();
}

void display_clear(){
    //clear RS and R/W
    P2OUT &= ~BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT |= BIT4;

    latch();

    P1OUT = 0;
}



// ------------------------ interrupt service routines ------------------
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){


// collect data array one bit at a time
    if(data_count == 32){
        data_count = 0;
        data_in[30] = 0xDF;
        new_sentance();
    }
    else{
        data_in[data_count] = UCB0RXBUF;
        data_count++;

    }

}
