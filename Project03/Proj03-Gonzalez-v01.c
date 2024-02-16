/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 03 - Feb 15 2024

	Summary:


	Version Summary:
        v01:
        v02:
	    v03: Transmits byte of data with ack, sends another address
 	    v04: Switches to Compare interrupt for clock, finished I2C transmission
        v05: Merged lances and zachs code
        v05: Working stop transmit and start code

    Ports Map: 

	Important Variables/Registers:
	    R4	SDA
	    R5	Clock Delay Loop
	    R6	Remaining transmit bits


	Todo:
		
-----------------------------------------------------------------------------------------------------------------------*/


#include <driverlib.h>

int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Set P1.0 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

}
