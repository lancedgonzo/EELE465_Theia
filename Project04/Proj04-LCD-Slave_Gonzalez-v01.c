/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 04 - Mar 02 2024

	Summary:

	Version Summary:
        v01:

    Ports Map: 
        I2C:
            P1.0 STE
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

#include <driverlib.h>

int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();

    while(1)
    {

        // Delay
        for(i=10000; i>0; i--);
    }
}
