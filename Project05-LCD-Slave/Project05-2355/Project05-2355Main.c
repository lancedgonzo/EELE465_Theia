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
#include "driverlib/gpio.c"

#define LED_Address 0x013
#define LCD_Address 0x046

#define KEYPAD_COL1(r) (r == 0) ? 1 : 0x0040 // 1.6
#define KEYPAD_COL2(r) (r == 0) ? 1 : 0x0080 // 1.7
#define KEYPAD_COL3(r) (r == 0) ? 2 : 0x0001 // 2.0
#define KEYPAD_COL4(r) (r == 0) ? 2 : 0x0002 // 2.1
#define KEYPAD_ROW1(r) (r == 0) ? 2 : 0x0004 // 2.2
#define KEYPAD_ROW2(r) (r == 0) ? 2 : 0x0008 // 2.3
#define KEYPAD_ROW3(r) (r == 0) ? 4 : 0x0001 // 4.0
#define KEYPAD_ROW4(r) (r == 0) ? 4 : 0x0002 // 4.1

// Defining row and column input as keys
#define KEY_1 0x011 // 17
#define KEY_2 0x012 // 18
#define KEY_3 0x014 // 20
#define KEY_4 0x021 // 33
#define KEY_5 0x022 // 34
#define KEY_6 0x024 // 36
#define KEY_7 0x041 // 65
#define KEY_8 0x042 // 66
#define KEY_9 0x044 // 68
#define KEY_0 0x082 // 130
#define KEY_POUND 0x084 // 132
#define KEY_AST 0x081 // 129
#define KEY_A 0x018 // 24
#define KEY_B 0x028 // 40
#define KEY_C 0x048 // 72
#define KEY_D 0x088 // 136

//Function Declarations ------------------------------------------------------
void ColInput();
void CheckCol();
void RowInput();
void CheckRow();
void CheckButton();
void ButtonResponse();
void TransmitButton();
void SwitchDebounce();
//--------------------------------------------------------------------------

//Vairable Declarations-----------------------------------------------------
uint8_t i;
uint8_t samples = 3;
uint8_t currentCount = 0;
bool CheckFlag = false;
bool TimerFlag = false;
bool QuestFlag = false;
uint8_t Button, LastButton = 0; // tracker of last button pressed
//--------------------------------------------------------------------------


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// Initialize keyboard
    RowInput();
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

    // Interrupt enable
    GPIO_enableInterrupt(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_enableInterrupt(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_enableInterrupt(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_enableInterrupt(KEYPAD_ROW4(0), KEYPAD_ROW4(1));

    __enable_interrupt();
    P1DIR |= BIT0;                                            // Set P1.0/LED to output direction
    P1OUT &= ~BIT0;                                           // P1.0 LED off

    while(1)
    {
       ADCCTL0 |= ADCENC | ADCSC;                            // Sampling and conversion start
       __bis_SR_register(LPM0_bits | GIE);                   // LPM0, ADC_ISR will force exit
       if (ADC_Result < 0x155)                               // 0.5v = 1.5v* 0x155/0x3ff
           P1OUT &= ~BIT0;                                   // Clear P1.0 LED off
       else
           P1OUT |= BIT0;                                    // Set P1.0 LED on
       __delay_cycles(5000);
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


//-ColumnInput: Sets row pins as OUTPUT---------------------------------------------------------------------
void ColumnInput(){

    //Configure outputs on Rows
    GPIO_setAsOutputPin(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_setAsOutputPin(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_setAsOutputPin(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_setAsOutputPin(KEYPAD_ROW4(0), KEYPAD_ROW4(1));

    //Configure inputs on Collumns
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_COL1(0), KEYPAD_COL1(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_COL2(0), KEYPAD_COL2(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_COL3(0), KEYPAD_COL3(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_COL4(0), KEYPAD_COL4(1));

    //Set High on ROWS
    GPIO_setOutputHighOnPin(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_setOutputHighOnPin(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_setOutputHighOnPin(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_setOutputHighOnPin(KEYPAD_ROW4(0), KEYPAD_ROW4(1));

}//--END ColumnInput----------------------------------------------------------------------------------------

//-CheckCol: Sets row pins as OUTPUT---------------------------------------------------------------------
void CheckCol() {
    // Set necessary pins as inputs and outputs for reading columns
    ColumnInput();

    // Clear button column values, then toggle bits corresponding to column
    Button &= 0x0F0;
    Button = ((P1IN & (KEYPAD_COL1(1))) == (KEYPAD_COL1(1))) ? (Button | BIT0) : (Button & ~BIT0);
    Button = ((P1IN & (KEYPAD_COL2(1))) == (KEYPAD_COL2(1))) ? (Button | BIT1) : (Button & ~BIT1);
    Button = ((P2IN & (KEYPAD_COL3(1))) == (KEYPAD_COL3(1))) ? (Button | BIT2) : (Button & ~BIT2);
    Button = ((P2IN & (KEYPAD_COL4(1))) == (KEYPAD_COL4(1))) ? (Button | BIT3) : (Button & ~BIT3);

}//--END CheckCol-------------------------------------------------------------------------------------


//-Collumns: Sets Collumn pins as OUTPUT-------------------------------------------------------------
void RowInput(){
    //Set P0-3 on keyboard as outputs
    GPIO_setAsOutputPin(KEYPAD_COL1(0), KEYPAD_COL1(1));
    GPIO_setAsOutputPin(KEYPAD_COL2(0), KEYPAD_COL2(1));
    GPIO_setAsOutputPin(KEYPAD_COL3(0), KEYPAD_COL3(1));
    GPIO_setAsOutputPin(KEYPAD_COL4(0), KEYPAD_COL4(1));

    //Set P4-7 on keyboard as inputs with pull down
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_ROW4(0), KEYPAD_ROW4(1));

    //Set P0-3 on keyboard as HIGH
    GPIO_setOutputHighOnPin(KEYPAD_COL1(0), KEYPAD_COL1(1));
    GPIO_setOutputHighOnPin(KEYPAD_COL2(0), KEYPAD_COL2(1));
    GPIO_setOutputHighOnPin(KEYPAD_COL3(0), KEYPAD_COL3(1));
    GPIO_setOutputHighOnPin(KEYPAD_COL4(0), KEYPAD_COL4(1));

}//--END RowInputs---------------------------------------------------------------------------------

//-CheckRow:  Clear row bits, then toggle bits based on input-----------------------------------------
void CheckRow() {
    // Set necessary pins as inputs and outputs for reading columns
    RowInput();

    // Clear row portion of button, then toggle bits corresponding to row
    Button &= 0x00F;
    Button = ((P2IN & (KEYPAD_ROW1(1))) == (KEYPAD_ROW1(1))) ? (Button | BIT4) : (Button & ~BIT4);
    Button = ((P2IN & (KEYPAD_ROW2(1))) == (KEYPAD_ROW2(1))) ? (Button | BIT5) : (Button & ~BIT5);
    Button = ((P4IN & (KEYPAD_ROW3(1))) == (KEYPAD_ROW3(1))) ? (Button | BIT6) : (Button & ~BIT6);
    Button = ((P4IN & (KEYPAD_ROW4(1))) == (KEYPAD_ROW4(1))) ? (Button | BIT7) : (Button & ~BIT7);
}//--END CheckRow-----------------------------------------------------------------------------------

//-CheckButton: Call functions to check which button was pressed, save it in last button and debounce.
void CheckButton() {
    CheckCol();
    CheckRow();
    ButtonResponse();
    SwitchDebounce();
    CheckFlag = false;
}//--END CheckButton-----------------------------------------------------------------------------------

//-ButtonResponse: Updating last button to current button-----------------------------------------------
void ButtonResponse() {
    // Update last button with identified button pressed
    switch(Button) {
        case KEY_0: LastButton = '0'; break;
        case KEY_1: LastButton = '1'; break;
        case KEY_2: LastButton = '2'; break;
        case KEY_3: LastButton = '3'; break;
        case KEY_4: LastButton = '4'; break;
        case KEY_5: LastButton = '5'; break;
        case KEY_6: LastButton = '6'; break;
        case KEY_7: LastButton = '7'; break;
        case KEY_8: LastButton = '8'; break;
        case KEY_9: LastButton = '9'; break;
        case KEY_AST: LastButton = '*'; break;
        case KEY_POUND: LastButton = '#'; break;
        case KEY_A: LastButton = 'A'; break;
        case KEY_B: LastButton = 'B'; break;
        case KEY_C: LastButton = 'C'; break;
        case KEY_D: LastButton = 'D'; break;
    }
}//--End ButtonResponse

//-SwitchDebounce(): -------------------------------------------------------------------------
void SwitchDebounce(){
    for(i=0; i<10; i++){}
}//--END SwitchDebounce---------------------------------------------------------------

//-- Interrupt Service Routines -----------------------------------------------------------

#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void) {
    if (QuestFlag) {
        UCB1TXBUF = '?';
        QuestFlag = false;
        TimerFlag = false;
    } else
        UCB1TXBUF = LastButton;
}

//-ISR Port 4 ----------------------------------------------------------------------------
#pragma vector = PORT1_VECTOR
#pragma vector = PORT2_VECTOR
#pragma vector = PORT3_VECTOR
#pragma vector = PORT4_VECTOR
__interrupt void ISR_Button_Pressed(void) {
    CheckFlag = true; // Tell system to check which key was pressed
    GPIO_clearInterrupt(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_clearInterrupt(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_clearInterrupt(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_clearInterrupt(KEYPAD_ROW4(0), KEYPAD_ROW4(1));
}//-- End ISR_P5_Button_Pressed ----------------------------------------------------------

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
           ADC_Result = ADCMEM0;
           __bic_SR_register_on_exit(LPM0_bits);              // Clear CPUOFF bit from LPM0
           break;
       default:
           break;
   }
}
