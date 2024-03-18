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

int data_in;
int loop;
int key = 0;
int c;

/**
 * main.c
 */
int main(void){
WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

UCB0CTLW0 |= UCSWRST;


UCB0CTLW0 |= UCMODE_3;
UCB0CTLW0 &= ~UCMST;
UCB0CTLW0 &= ~UCTR;
UCB0I2COA0 = 0X0048;
UCB0I2COA0 |= UCOAEN;
UCB0CTLW1 &= ~UCASTP0;
UCB0CTLW1 &= ~UCASTP1;
GIE;


UCB0CTLW1 |= UCASTP_2;

// - configure ports
P1SEL1 &= ~BIT3;       // P1.2 AS SCL
P1SEL0 |= BIT3;

P1SEL1 &= ~BIT2;        //P1.3 AS SDA
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

UCB0CTLW0 &= ~UCSWRST;

UCB0IE |= UCRXIE0;
__enable_interrupt();


    while(1){
        for(loop = 0; loop < 1575; loop++){} // 30 ms loop

        //intialization start
        home();
        functionset();
        display_on();
        //busy();
        c = 1;

        while(key < 1){
            switch (c) {
              case 1:
                  print1();
                  busy();
                break;
              case 2:
                  print2();
                  busy();
                break;
              case 3:
                  key = 1;
              default:
                //busy();
            }
            c = 0;
            busy();
        }
        //print();
        //busy();
        //entry_mode();
        //curser_shift();
        //CGR_set();
        //DDR_set();

        /*
        while(1){
            curser_shift();
        }
        */

    }
    return 0;
}

void print1(){
    entry_mode();

    //set RS and clear R/W
    P2OUT |= BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT |= BIT5;
    P1OUT |= BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT |= BIT4;

    latch();

    //set RS and R/W
    P2OUT |= BIT6;
    P2OUT |= BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT |= BIT5;
    P1OUT |= BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT |= BIT4;

    latch();

    curser_shift();
}

void print2(){
    entry_mode();

    //set RS and clear R/W
    P2OUT |= BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT |= BIT5;
    P1OUT |= BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT |= BIT5;
    P1OUT &= ~BIT4;

    latch();

    //set RS and R/W
    P2OUT |= BIT6;
    P2OUT |= BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT |= BIT5;
    P1OUT |= BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT |= BIT5;
    P1OUT &= ~BIT4;

    latch();

    curser_shift();
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

void busy(){
    //clear RS and R/W
    P2OUT &= ~BIT6;
    P2OUT |= BIT7;

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
}

void display_off(){
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
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    latch();
}

void curser_shift(){
    //clear RS and R/W
    P2OUT &= ~BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT |= BIT4;

    latch();

    //SETUP DB3 - DB0
    P1OUT |= BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    latch();
}



// ------------------------ interrupt service routines ------------------
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    data_in  = UCB0RXBUF;
}
