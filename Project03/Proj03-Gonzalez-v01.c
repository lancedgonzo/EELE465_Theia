/*----------------------------------------------------------------------------------------------------------------------
    EELE465
	Written by: Lance Gonzalez, Grant Kirkland
    Working: 
	Project 03 - Feb 21 2024

	Summary:

	Version Summary:
        v01: Tested LED output, Display Locked Pattern, Create checking for lock or unlock 


    Ports Map: 
        Keyboard P0-7 (P3.1, 3.5, 1.1, 5.4, 1.4, 5.3, 5.1, 5.0)
        LEDs 0-7 (P3.0, 2.5, 4.4, 4.7, 4.6, 4.0, 2.2, 2.0) 

	Important Variables/Registers:
	
	    
	Todo:
		
-----------------------------------------------------------------------------------------------------------------------*/

#include "gpio.h"
#include "msp430fr2355.h"

#include <msp430.h> 
#include <driverlib.h>
#include <stdint.h>


#define KEYPAD_PIN_COUNT 8
#define UNLOCK_CODE 1234
#define LOCKED_STATE 0
#define UNLOCKED_STATE 1    

volatile uint16_t delay_counter = 0;            //Used for delay loop

int state = LOCKED_STATE;                       //Initalize Locked

int i; 

//----- Function Declarations --------------------------------------------------------
void turn_off_leds();                   //Clear Output of LEDs
int read_keypad();                      //TODO: not done
void display_locked_pattern();          //TODO: LED's are not toggling
void display_pattern();                 //TODO: not done        
void delay_ms(int);                     //Maybe not working, will create delay of certain time

//----- END Declarations -------------------------------------------------------------

//----- 
void delay_init(){
    // Configure Timer_A to generate interrupts every 1 second
    TB0CCTL0 = CCIE; 
    TB0CCR0 = 32767; 
    TB0CTL = TBSSEL_1 + MC_1 + TBCLR; 
}

int main(void){

    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Port Configuration (LED1 P1.0, LED2 P6.6)
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN6);

    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);


    // Disable the GPIO power-on default high-impedance 
    PMM_unlockLPM5();

    //Initialize Timer_B for delay
    delay_init();

    while(1){
        if(state == LOCKED_STATE){
            display_locked_pattern(); 
            // int code = read_keypad(); 
            
            if(code == UNLOCK_CODE){
                state = UNLOCKED_STATE;
                turn_off_leds(); 
            } else{}
        } else if (state == UNLOCKED_STATE) {
            // int pattern = read_keypad(); 
            display_pattern(pattern);
        } else{}
    }
}


//------ Functions & Subroutines -----------------------------------------------------
//- turn_off_leds --------------------------------------------------------------------------
void turn_off_leds(){
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);

} //--END turn_off_leds --------------------------------------------------------------------

//- read_keypad ----------------------------------------------------------------------
int read_keypad(){

}//- END read_keypad -----------------------------------------------------------------

//- display_locked_pattern ----------------------------------------------------------------------
void display_locked_pattern(){
    GPIO_toggleOutputOnPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
    delay_ms(1000);     //Delay 1000mS


}//- END display_locked_pattern -----------------------------------------------------------------

//- display_pattern ----------------------------------------------------------------------
void display_pattern(){
    
}//- END display_pattern -----------------------------------------------------------------

void delay_ms(ms){
    delay_counter = 0;
    TB0CTL |= TBCLR; 
    TB0CTL |= MC_1; 

    while (delay_counter < ms){};

    TB0CTL &= ~MC_1; 
}


//---- Interrupt Service Routines -------------------------------------------------------------
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){
    delay_counter++;
    TB0CCTL0 &= ~CCIFG; 
}