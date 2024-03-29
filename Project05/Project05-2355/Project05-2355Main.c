/*----------------------------------------------------------------------------------------------------------------------
    EELE465
    Written by: Lance Gonzalez, Grant Kirkland
    Working:
    Project 04 - Feb 15 2024

    Summary:

    Version Summary:
        v01: Added previous project code for keypad and I2C
    MSP430FR2355:


-----------------------------------------------------------------------------------------------------------------------*/
#include <msp430.h> 
#include <stdint.h>
#include "stdbool.h"
#include <gpio.h>

#include "keypad.h"
//Vairable Declarations-----------------------------------------------------
uint8_t samples = 3;
uint8_t currentCount = 0;
bool TimerFlag = false;
bool QuestFlag = false;
char LastButton = 0;

//--------------------------------------------------------------------------

void I2CInit();


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	Init_Keypad();

    // Initialize I2C
    I2CInit();

    P1SEL0 |= BIT1;
    P1SEL1 |= BIT1;
    ADCCTL0 |= ADCSHT_2 | ADCON;    // ADC on, 16 clock cycles
    ADCCTL1 |= ADCSHP_1;            // Sample and hold
    ADCCTL2 |= ADCRES_2;            // 12 bit resolution
    ADCIE |= ADCIE0_1;              // ADC Interrupt enabled
    ADCMCTL0 |= ADCINCH_1 | ADCSREF_1;  // A1 ADC input select, voltage reference internal try voltage 2 and 3 also

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    PMMCTL0_H = PMMPW_H;
    PMMCTL2 |= REFVSEL_2 | INTREFEN_1;

    __delay_cycles(400);   // Delay for reference voltage to settle


    __enable_interrupt();
    P1DIR |= BIT0;                                            // Set P1.0/LED to output direction
    P1OUT &= ~BIT0;                                           // P1.0 LED off

    while(1)
    {
       ADCCTL0 |= ADCENC | ADCSC;                            // Sampling and conversion start
       __bis_SR_register(LPM0_bits | GIE);                   // LPM0, ADC_ISR will force exit
/*       if (ADC_Result < 0x155)                               // 0.5v = 1.5v* 0x155/0x3ff
           P1OUT &= ~BIT0;                                   // Clear P1.0 LED off
       else
           P1OUT |= BIT0;                                    // Set P1.0 LED on
       __delay_cycles(5000); */
    }
}

void I2CInit() {
    UCB1CTLW0 |= UCSWRST; // Put UCB1CTLW0 into software reset
    UCB1CTLW0 |= UCSSEL_3; // Select mode 3
    UCB1BRW = 10; // Something useful

    UCB1CTLW0 |= UCMODE_3; // Mode 3
    UCB1CTLW0 |= UCMST; // Master
    UCB1CTLW0 |= UCTR; // Transmit mode

    UCB1CTLW1 |= UCASTP_2; // Autostop enabled

    // Pin 4.7
    P4SEL1 &= ~BIT7;
    P4SEL0 |= BIT7;

    P4SEL1 &= ~BIT6;
    P4SEL0 |= BIT6;
    UCB1CTLW0 &= ~UCSWRST; // Take out of Software Reset

    UCB1IE |= UCTXIE0; // Enable TX interrupt
    UCB1IE |= UCRXIE0; // Enable RX interrupt

}


//-- Interrupt Service Routines -----------------------------------------------------------

#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void) {
    UCB1TXBUF = LastButton;
}

// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{
   switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
   {
       case ADCIV_NONE:
           break;
       case ADCIV_ADCOVIFG:
           break;
       case ADCIV_ADCTOVIFG:
           break;
       case ADCIV_ADCHIIFG:
           break;
       case ADCIV_ADCLOIFG:
           break;
       case ADCIV_ADCINIFG:
           break;
       case ADCIV_ADCIFG:
          // ADC_Result = ADCMEM0;
           __bic_SR_register_on_exit(LPM0_bits);              // Clear CPUOFF bit from LPM0
           break;
       default:
           break;
   }
}
