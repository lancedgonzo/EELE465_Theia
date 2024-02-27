/*------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 03 - Feb 26 2024

	Summary:

	Version Summary:
        v01: Polled Keyboard Input, verified LED Output
        v02: Pattern A and Pattern C 



    Ports Map: 
        Keypad:  
                P0   P1   P2   P3
            P4 |1|  |2|  |3|  |A| P3.5
            P5 |4|  |5|  |6|  |B| P1.3
            P6 |7|  |8|  |9|  |C| P3.1
            P7 |*|  |0|  |#|  |D| P1.2
               P3.6 P4.5 P1.1 P3.4

        LED Bar:
            LED0 - P
	Important Variables/Registers:
	   
	Todo:
		
--------------------------------------------------------------------------------------------------*/

#include "gpio.h"
#include "msp430fr2355.h"
// #include "sys/_stdint.h"
#include <msp430.h> 
#include <driverlib.h>
// #include <stdint.h>

//Macro Definitions for ease of coding -------------------------------------------------------------
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

uint8_t PatternDCounter = 0;
uint8_t Button = 0, LED_Out = 0, Pattern = 0;

//Function Declarations ----------------------------------------------------------------------------
void Init(); 
void RowInput();
void CollumnInput(); 
void LedLow(); 
void LedLocked();
void PatternA();
void PatternC(); 
void UpdateLED();


int main(void) {

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Disable the GPIO power-on default high-impedance 
    PMM_unlockLPM5();

    __enable_interrupt();

    Init();

    PatternC(); 
    while(1){
        // PatternA(); 
        PatternC(); 
    }

}

//-----Subroutines-----------------------------------------------------------------------------------
//-Initalization: Main Init for startup----------------------------------------------------
void Init(){
    LedLocked(); 
    RowInput(); 
}//--END Init-----------------------------------------------------------------------------

//-Collumns: Sets Collumn pins as OUTPUT-------------------------------------------------------------
void RowInput(){

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

}//--END RowInputs---------------------------------------------------------------------------------

//-Rows: Sets row pins as OUTPUT---------------------------------------------------------------------
void CollumnInput(){
  
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

//-LED Init Low: Configures LED ports as output and LOW----------------------------------------------
void LedLow(){
    //LED Bar Output initalizing as low
    GPIO_setAsOutputPin(Port3, Pin1);
    GPIO_setOutputLowOnPin(Port3, Pin1);
    GPIO_setAsOutputPin(Port3, Pin5);
    GPIO_setOutputLowOnPin(Port3, Pin5);
    GPIO_setAsOutputPin(Port1, Pin1);
    GPIO_setOutputLowOnPin(Port1, Pin1);
    GPIO_setAsOutputPin(Port5, Pin4);
    GPIO_setOutputLowOnPin(Port5, Pin4);
    GPIO_setAsOutputPin(Port5, Pin0);
    GPIO_setOutputLowOnPin(Port5, Pin0);
    GPIO_setAsOutputPin(Port5, Pin1);
    GPIO_setOutputLowOnPin(Port5, Pin1);
    GPIO_setAsOutputPin(Port5, Pin3);
    GPIO_setOutputLowOnPin(Port5, Pin3);
    GPIO_setAsOutputPin(Port1, Pin4);
    GPIO_setOutputLowOnPin(Port1, Pin4);
}//--END LedLow-------------------------------------------------------------------------------------

//-LED Locked: Turns on all LEDs indicating lockout-------------------------------------------------
void LedLocked(){
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
}//--END LedLocked---------------------------------------------------------------------------------

//-PatternA: Static pattern XOXOXOXO---------------------------------------------------------------
void PatternA(){
    //Set High on Collumns
    GPIO_setOutputHighOnPin(Port2, Pin2);
    GPIO_setOutputLowOnPin(Port4, Pin0);
    GPIO_setOutputHighOnPin(Port6, Pin3);
    GPIO_setOutputLowOnPin(Port4, Pin6);
    GPIO_setOutputHighOnPin(Port4, Pin7);
    GPIO_setOutputLowOnPin(Port4, Pin4);
    GPIO_setOutputHighOnPin(Port2, Pin5);
    GPIO_setOutputLowOnPin(Port3, Pin0);
}//--END PatternA--------------------------------------------------------------------------------

//-PatternC: Scrolling Pattern ---------------------------------------------------------------
void PatternC(){
    // Set timer for 2s
    TB0CTL |= TBCLR + TBSSEL__ACLK + MC__UP + ID__1;
    TB0CCR0 = 36864;
    TB0R = 0;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;

    // Count and reset on rollover
    switch(PatternDCounter) {
        case 0: LED_Out = 0x7F; break;
        case 1: LED_Out = 0xBF; break;
        case 2: LED_Out = 0xDF; break;
        case 3: LED_Out = 0xEF; break;
        case 4: LED_Out = 0xF7; break;
        case 5: LED_Out = 0xFB; break;
        case 6: LED_Out = 0xFD; break; 
        case 7: LED_Out = 0xFE; break;
    }
    PatternDCounter += 1;
    if (PatternDCounter == 7){
        PatternDCounter = 0;
    }
}//--END PatternC--------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------

//-----Interrupt Service Routines------------------------------------------------------------------
//-TimerB0 ISR-------------------------------------------------------------------------------------
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){

    TB0CCTL0 &= ~CCIFG; 
}
//-- End Timer_B_ISR ------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
