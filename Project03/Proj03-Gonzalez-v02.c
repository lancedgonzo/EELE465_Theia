/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 03 - Feb 21 2024

	Summary:

	Version Summary:
        v01: 

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
	    

	Todo:
		
-----------------------------------------------------------------------------------------------------------------------*/

#include "gpio.h"
#include "msp430fr2355.h"
#include "sys/_stdint.h"
#include <msp430.h> 
#include <driverlib.h>
#include <stdint.h>


#define  GPIO_PORT_P1 Port1 
#define  GPIO_PORT_P3 Port3 
#define  GPIO_PORT_P5 Port5

#define GPIO_PIN_PIN0 Pin0 
#define GPIO_PIN_PIN1 Pin1
#define GPIO_PIN_PIN2 Pin2
#define GPIO_PIN_PIN3 Pin3   
#define GPIO_PIN_PIN4 Pin4
#define GPIO_PIN_PIN5 Pin5 

uint8_t pinConfig_G1[][2] = {
    {Port3, Pin1}, //P3.1
    {Port3, Pin5}, //P3.5
    {Port1, Pin1}, //P1.1
    {Port5, Pin4}, //P5.4 
};

uint8_t pinConfig_G2[][2] = {
    {Port1, Pin4}, //P1.4
    {Port5, Pin3}, //P5.3
    {Port5, Pin1}, //P5.1
    (Port5, Pin0)  //P5.0
};

int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Disable the GPIO power-on default high-impedance 
    PMM_unlockLPM5();



}

//-----Subroutines-----------------------------------------------------------------------------------
//-Collumns: Sets Collumn pins as OUTPUT-------------------------------------------------------------
void Collumns(){
    for (int i = 0; i < sizeof(pinConfig_G1) / sizeof(pinConfig_G1[0]); i++){
        pinMode(pinConfig_G1[i][0], OUTPUT)
    }
}//--END Collumns-------------------------------------------------------------------------------------

//-Rows: Sets row pins as OUTPUT---------------------------------------------------------------------
void Rows(){
    for (int i = 0; i < sizeof(pinConfig_G2) / sizeof(pinConfig_G2[0]); i++){
        pinMode(pinConfig_G2[i][0], OUTPUT)
        pinMode(pinConfig_G1[i][0], INPUT)
    }
}//--END Rows----------------------------------------------------------------------------------------


}