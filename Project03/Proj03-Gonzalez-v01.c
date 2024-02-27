/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
	Project 03 - Feb 21 2024

	Summary:

	Version Summary:
        v01: Polled Keyboard Input, Verified LED Output
         


    Ports Map: 
        Keyboard   
                P0   P1   P2   P3
            P4 |1|  |2|  |3|  |A| P3.5
            P5 |4|  |5|  |6|  |B| P1.3
            P6 |7|  |8|  |9|  |C| P3.1
            P7 |*|  |0|  |#|  |D| P1.2
               P3.6 P4.5 P1.1 P3.4

	Important Variables/Registers:
	
	    
	Todo:
		
-----------------------------------------------------------------------------------------------------------------------*/

#include <msp430.h> 
#include <driverlib.h>
#include "msp430fr2355.h"
#include "gpio.h"

#define Port1 GPIO_PORT_P1  
#define Port2 GPIO_PORT_P2
#define Port3 GPIO_PORT_P3  
#define Port4 GPIO_PORT_P4
#define Port5 GPIO_PORT_P5 
#define Port6 GPIO_PORT_P6

#define Pin0 GPIO_PIN0
#define Pin1 GPIO_PIN1
#define Pin2 GPIO_PIN2
#define Pin3 GPIO_PIN3
#define Pin4 GPIO_PIN4
#define Pin5 GPIO_PIN5
#define Pin6 GPIO_PIN6
#define Pin7 GPIO_PIN7

int main(void){

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Unlock Low Power Mode 
    PMM_unlockLPM5();

    // //Configure outputs on Rows
    // GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN5);
    // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN3);
    // GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN1);
    // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN2);

    // //Configure inputs on Collumns
    // GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN6);
    // GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P4, GPIO_PIN5);
    // GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN1);
    // GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN4);

    // //Set High on ROWS
    // GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5);
    // GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3);
    // GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN1);
    // GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN2);

    // //Configure outputs on Collumns
    // GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);
    // GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);
    // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN1);
    // GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN4);

    // //Configure inputs on Rows
    // GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN5);
    // GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN3);
    // GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN1);
    // GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN2);

    // //Set High on Collumns
    // GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
    // GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
    // GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
    // GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN4);


    //Configure outputs on LED PINS
    GPIO_setAsOutputPin(Port2, Pin2);
    GPIO_setAsOutputPin(Port4, Pin0);
    GPIO_setAsOutputPin(Port6, Pin3);
    GPIO_setAsOutputPin(Port4, Pin6);
    GPIO_setAsOutputPin(Port4, Pin7);
    GPIO_setAsOutputPin(Port4, Pin4);
    GPIO_setAsOutputPin(Port2, Pin5);
    GPIO_setAsOutputPin(Port3, Pin0);

    //Set High on Collumns
    GPIO_setOutputHighOnPin(Port2, Pin2);
    GPIO_setOutputHighOnPin(Port4, Pin0);
    GPIO_setOutputHighOnPin(Port6, Pin3);
    GPIO_setOutputHighOnPin(Port4, Pin6);
    GPIO_setOutputHighOnPin(Port4, Pin7);
    GPIO_setOutputHighOnPin(Port4, Pin4);
    GPIO_setOutputHighOnPin(Port2, Pin5);
    GPIO_setOutputHighOnPin(Port3, Pin0);

    while(1){}

}