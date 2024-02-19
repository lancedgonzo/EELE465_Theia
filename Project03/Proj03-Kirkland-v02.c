/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 03 - Feb 15 2024

	Summary:

	Version Summary:
        v01: Press button in column to toggle 0-3 binary on leds
        v02: Working LED Output, defined keypad macros, working column readout

    Ports Map: 
        P3.1, 3.5, 1.1, 5.4, 1.4, 5.3, 5.1, 5.0 - Keyboard P1-8
        P3.0, 2.5, 4.4, 4.7, 4.6, 4.0, 2.2, 2.0 - LEDs 0-7

	Important Variables/Registers:
	    Button 0x76543210
            0-3 Col
            4-7 Row
        LED_Out
            1:1 LED

    Keypad: (Pins L->R)
            P0   P1   P2   P3
        P4 |1|  |2|  |3|  |A| P1.4
        P5 |4|  |5|  |6|  |B| P5.3
        P6 |7|  |8|  |9|  |C| P5.1
        P7 |*|  |0|  |#|  |D| P5.0
          P3.1 P3.5 P1.1 P5.4
    
    MSP Errors:
        P1.3 Always High
        Row P3.2-2.1 mostly not working?
        P4.5 Inconsistent Input?
	Todo:
		
-----------------------------------------------------------------------------------------------------------------------*/

#include "gpio.h"
#include "msp430fr2355.h"
#include <msp430.h> 
#include <driverlib.h>
#include <stdint.h>

#define KEY_1 0x011
#define KEY_2 0x012
#define KEY_3 0x014
#define KEY_4 0x021
#define KEY_5 0x022
#define KEY_6 0x024
#define KEY_7 0x041
#define KEY_8 0x042
#define KEY_9 0x044
#define KEY_0 0x082
#define KEY_POUND 0x084
#define KEY_AST 0x081
#define KEY_A 0x018
#define KEY_B 0x028
#define KEY_C 0x048
#define KEY_D 0x088

int Button = 0, LED_Out = 0, Pattern = 0;
int test; // arbitrary test register

int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN6);
    //LED Bar Output
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);


    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

    while(1) {
        CheckCol();
        switch(Button) {
            case BIT0: Pattern = BIT0; break;
            case BIT1: Pattern = BIT1; break;
            case BIT2: Pattern = BIT2; break;
            case BIT3: Pattern = BIT3; break;
            case KEY_0: break;
            case KEY_1: break;
            case KEY_2: break;
            case KEY_3: break;
            case KEY_4: break;
            case KEY_5: break;
            case KEY_6: break;
            case KEY_7: break;
            case KEY_8: break;
            case KEY_9: break;
            case KEY_AST: break;
            case KEY_POUND: break;
            case KEY_A: break;
            case KEY_B: break;
            case KEY_C: break;
            case KEY_D: break;
        }
        if (Pattern == BIT0) {
            PatternBUpdate();
        }
        UpdateLED();
    }
}

void CheckCol() {
    // Set necessary pins as inputs and outputs for reading columns
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
    // Clear button column values
    Button &= 0x0F0;

    test = P5IN;
    test &= BIT4;
    if (test == BIT4)
        Button |= BIT3;
    test = P1IN;
    test &= BIT1;
    if (test == BIT1)
        Button |= BIT2;
    test = P3IN;
    test &= BIT5;
    if (test == BIT5)
        Button |= BIT1;
    test = P3IN;
    test &= BIT1;
    if (test == BIT1)
        Button |= BIT0;

}

void PatternBUpdate() {
    // Count and reset on rollover
    LED_Out += 1;
    if (LED_Out == 256)
        LED_Out = 0;
}

void UpdateLED() {
    // Move LED_Out to led ports
    P3OUT = ((LED_Out & BIT0) == BIT0) ? (P3OUT | BIT0) : (P3OUT & ~BIT0);
    P2OUT = ((LED_Out & BIT1) == BIT1) ? (P2OUT | BIT5) : (P2OUT & ~BIT5);
    P4OUT = ((LED_Out & BIT2) == BIT2) ? (P4OUT | BIT4) : (P4OUT & ~BIT4);
    P4OUT = ((LED_Out & BIT3) == BIT3) ? (P4OUT | BIT7) : (P4OUT & ~BIT7);
    P4OUT = ((LED_Out & BIT4) == BIT4) ? (P4OUT | BIT6) : (P4OUT & ~BIT6);
    P4OUT = ((LED_Out & BIT5) == BIT5) ? (P4OUT | BIT0) : (P4OUT & ~BIT0);
    P2OUT = ((LED_Out & BIT6) == BIT6) ? (P2OUT | BIT2) : (P2OUT & ~BIT2);
    P2OUT = ((LED_Out & BIT7) == BIT7) ? (P2OUT | BIT0) : (P2OUT & ~BIT0);    
}