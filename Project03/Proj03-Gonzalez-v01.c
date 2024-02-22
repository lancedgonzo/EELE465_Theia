/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 03 - Feb 21 2024

	Summary:

	Version Summary:
        v01: Tested LED Bar Output

    Ports Map: 
        

	Important Variables/Registers:
	
	    

	Todo:
		
-----------------------------------------------------------------------------------------------------------------------*/


#include "gpio.h"
#include "msp430fr2355.h"
#include "sys/_stdint.h"
#include <msp430.h> 
#include <driverlib.h>
#include <stdint.h>



int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Disable the GPIO power-on default high-impedance 
    PMM_unlockLPM5();

    while(1){
        GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0);
        P3OUT |= BIT0; 
    }

    
}
