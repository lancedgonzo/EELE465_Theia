/*----------------------------------------------------------------------------------------------------------------------
    EELE465
    Written by: Lance Gonzalez, Grant Kirkland
    Working:
    Project 04 - Mar 02 2024

    Summary:

    Version Summary:
        v01:

	In Circuit Programming
		P3: SBWTCK
		P4: SBWTDIO
		P5: DVCC, 3v3
		P6: DVSS, GND
		
    Ports Map:
    	P1.0-1.7 LED
        I2C:
            

            P1.1 CLK
            P1.2 SDA
            P1.3 SCL


    Important Variables/Registers:
        Button 0x76543210
            0-3 Col
            4-7 Row
        LED_Out
            1:1 LED

    MSP Errors:
        P4.5 Inconsistent Input?
    Todo:

-----------------------------------------------------------------------------------------------------------------------*/
#include "gpio.h"
#include <msp430.h>
#include <stdint.h>

// Making ports and pins easier to reference
#define Port1 GPIO_PORT_P1
#define Port2 GPIO_PORT_P2
#define Port3 GPIO_PORT_P3
#define Port4 GPIO_PORT_P4
#define Port5 GPIO_PORT_P5

#define P0 GPIO_PIN0
#define P1 GPIO_PIN1
#define P2 GPIO_PIN2
#define P3 GPIO_PIN3
#define P4 GPIO_PIN4
#define P5 GPIO_PIN5
#define P6 GPIO_PIN6
#define P7 GPIO_PIN7


int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
    GPIO_setAsOutputPin(Port1, P0);
    GPIO_setAsOutputPin(Port1, P1);
    GPIO_setAsOutputPin(Port1, P2);
    GPIO_setAsOutputPin(Port1, P3);
    GPIO_setAsOutputPin(Port1, P4);
    GPIO_setAsOutputPin(Port1, P5);
    GPIO_setAsOutputPin(Port1, P6);
    GPIO_setAsOutputPin(Port1, P7);
    GPIO_setOutputHighOnPin(Port1, P0);
    GPIO_setOutputHighOnPin(Port1, P1);
    GPIO_setOutputHighOnPin(Port1, P2);
    GPIO_setOutputHighOnPin(Port1, P3);

	while (1) {
	}
}
