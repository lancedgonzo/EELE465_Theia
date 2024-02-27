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

//Macro Definitions for ease of coding -----------------------------------------------------------------
#define Port1 GPIO_PORT_P1  
#define Port2 GPIO_PORT_P2
#define Port3 GPIO_PORT_P3  
#define Port4 GPIO_PORT_P4
#define Port5 GPIO_PORT_P5 

#define Pin0 GPIO_PIN0
#define Pin1 GPIO_PIN1
#define Pin2 GPIO_PIN2
#define Pin3 GPIO_PIN3
#define Pin4 GPIO_PIN4
#define Pin5 GPIO_PIN5
#define Pin6 GPIO_PIN6
#define Pin7 GPIO_PIN7

//Function Declarations ----------------------------------------------------------------------------------------
void SetCollumn();
void SetRows(); 


int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Disable the GPIO power-on default high-impedance 
    PMM_unlockLPM5();

    SetRows(); 
    while(1){}

}

//-----Subroutines-----------------------------------------------------------------------------------
//-Collumns: Sets Collumn pins as OUTPUT-------------------------------------------------------------
void SetCollumn(){

    //Set P0-3 on keyboard as outputs
    GPIO_setAsOutputPin(Port3, Pin6);
    GPIO_setAsOutputPin(Port4, Pin5);
    GPIO_setAsOutputPin(Port1, Pin1);
    GPIO_setAsOutputPin(Port3, Pin4);

    //Set P4-7 on keyboard as inputs with pull down
    GPIO_setAsInputPinWithPullDownResistor(Port3, Pin5);
    GPIO_setAsInputPinWithPullDownResistor(Port1, Pin3);
    GPIO_setAsInputPinWithPullDownResistor(Port3, Pin1);
    GPIO_setAsInputPinWithPullDownResistor(Port1, Pin2);

    //Set P0-3 on keyboard as HIGH
    GPIO_setOutputHighOnPin(Port3, Pin6);
    GPIO_setOutputHighOnPin(Port4, Pin5);
    GPIO_setOutputHighOnPin(Port1, Pin1);
    GPIO_setOutputHighOnPin(Port3, Pin4);

}//--END SetCollumns-------------------------------------------------------------------------------------

//-Rows: Sets row pins as OUTPUT---------------------------------------------------------------------
  void SetRows(){
  
  //Configure outputs on Rows
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN2);

    //Configure inputs on Collumns
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN4);

    //Set High on ROWS
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN2);
}//--END Rows----------------------------------------------------------------------------------------


