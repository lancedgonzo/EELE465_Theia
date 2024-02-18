/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 03 - Feb 15 2024

	Summary:

	Version Summary:
        v01: Press button in column to toggle 0-3 binary on leds
        
    Ports Map: 
        P1.5, 1.6, 1.7, 3.6, 5.2, 4.5, 3.4, 1.3 - Keyboard P8-1

	Important Variables/Registers:
	    Button 0x76543210
            0-3 Col
            4-7 Row

    Keypad: (Pins L->R)
            P0   P1   P2   P3
        P4 |1|  |2|  |3|  |A| P1.4
        P5 |4|  |5|  |6|  |B| P5.3
        P6 |7|  |8|  |9|  |C| P5.1
        P7 |*|  |0|  |#|  |D| P5.0
          P3.1 P3.5 P1.1 P5.4
    
    MSP Errors:
        P1.3 Always High
        P4.5 Inconsistent Input?
	Todo:
		
-----------------------------------------------------------------------------------------------------------------------*/

#include "gpio.h"
#include "msp430fr2355.h"
#include <msp430.h> 
#include <driverlib.h>

int Col = 0, test;

int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN6);

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

    while(1) {
        CheckCol();
        switch(Col) {
            case BIT0: P6OUT &= ~BIT6; P1OUT &= ~BIT0; break;
            case BIT1: P6OUT |= BIT6; P1OUT &= ~BIT0; break;
            case BIT2: P6OUT &= ~BIT6; P1OUT |= BIT0; break;
            case BIT3: P6OUT |= BIT6; P1OUT |= BIT0; break;
        }


    }
}
void CheckCol() {
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN1);
    Col = 0;
    test = P5IN;
    test &= BIT4;
    if (test == BIT4)
        Col = BIT3;
    test = P1IN;
    test &= BIT1;
    if (test == BIT1)
        Col = BIT2;
    test = P3IN;
    test &= BIT5;
    if (test == BIT5)
        Col = BIT1;
    test = P3IN;
    test &= BIT1;
    if (test == BIT1)
        Col = BIT0;

}