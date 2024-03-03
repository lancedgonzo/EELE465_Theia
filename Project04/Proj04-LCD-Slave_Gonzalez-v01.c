/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 04 - Mar 02 2024

	Summary:

	Version Summary:
        v01: I2C as slave setup

    Ports Map: 



	Important Variables/Registers:
	

    MSP Errors:

	Todo:
		
-----------------------------------------------------------------------------------------------------------------------*/

#include <driverlib.h>

int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Port Initialization

    //I2C as slave, slave address, and more initialziation

    PMM_unlockLPM5();           //Unlock Low Power Mode for DIO enable

    while(1)
    {

        // Delay
        for(i=10000; i>0; i--);
    }
}
