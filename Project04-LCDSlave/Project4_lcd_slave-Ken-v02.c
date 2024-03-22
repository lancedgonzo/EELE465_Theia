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
int input;
int printput;

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
        //display_clear();
        home();
        functionset();
        display_on();
        input = 1;


        while(1){
            if(input == 1){
                printput = 0b00110001;
                print(printput);
                input = 2;
            }
            else if(input == 2){
                printput = 0b00110010;
                print(printput);
                input = 3;
            }
            else if(input == 3){
                printput = 0b00110011;
                print(printput);
                input = 4;
            }
            else if(input == 4){
                printput = 0b00110100;
                print(printput);
                input = 5;
            }
            else if(input == 5){
                printput = 0b00110101;
                print(printput);
                input = 6;
            }
            else if(input == 6){
                printput = 0b00110110;
                print(printput);
                input = 7;
            }
            else if(input == 7){
                printput = 0b00110111;
                print(printput);
                input = 8;
            }
            else if(input == 8){
                printput = 0b00111000;
                print(printput);
                input = 9;
            }
            else if(input == 9){
                printput = 0b00111001;
                print(printput);
                input = 10;
            }
            else if(input == 10){
                printput = 0b00110000;
                print(printput);
                input = 11;
            }
            else if(input == 11){
                printput = 0b01000001;
                print(printput);
                input = 12;
            }
            else if(input == 12){
                printput = 0b01000010;
                print(printput);
                input = 13;
            }
            else if(input == 13){
                printput = 0b01000011;
                print(printput);
                input = 14;
            }
            else if(input == 14){
                printput = 0b01000100;
                print(printput);
                input = 15;
            }
            else if(input == 15){
                printput = 0b00101010;
                print(printput);
                input = 16;
            }
            else if(input == 16){
                //display_clear();
                //home();
                printput = 0b00100011;
                print(printput);
                input = 0;
            }
            //input = 0;
        }
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

void print(int printput){
    entry_mode();

    //set RS and clear R/W
    P2OUT |= BIT6;
    P2OUT &= ~BIT7;

    //SETUP DB7 - DB4
    P1OUT = (printput & 0b11110000);

    latch();

    //SETUP DB3 - DB0
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



// ------------------------ interrupt service routines ------------------
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    data_in  = UCB0RXBUF;
    input = data_in;
}
