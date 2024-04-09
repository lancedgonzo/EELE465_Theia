/*----------------------------------------------------------------------------------------------------------------------
    EELE465
    Written by: Lance Gonzalez, Grant Kirkland
    Working:
    Project 05

    Summary:

    Version Summary:
        v01: Working Project 04
        v02: Output 32 chars

    MSP430FR2355:
         4 - SBWTCK - SBWTCK
         5 - SBWTDIO - SBWTDIO
         6 - DVCC - 3v3
         7 - DVSS - GND
        12 - P4.7 - SCL
        13 - P4.6 - SDA
        25 - P4.1 - P7 Row4
        26 - P4.0 - P6 Row3
        27 - P2.3 - P5 Row2
        28 - P2.2 - P4 Row1
        29 - P2.1 - P3 Col4
        30 - P2.0 - P2 Col3
        31 - P1.7 - P1 Col2
        32 - P1.6 - P0 Col1


    Important Variables/Registers:
        Button 0x76543210
            0-3 Col
            4-7 Row
        LED_Out
            1:1 LED

    Keypad: (Pins L->R)
            P0   P1   P2   P3
        P4 |1|  |2|  |3|  |A| P3.7
        P5 |4|  |5|  |6|  |B| P4.0
        P6 |7|  |8|  |9|  |C| P2.2
        P7 |*|  |0|  |#|  |D| P2.0
          P3.0 P2.5 P4.4 P2.4



-----------------------------------------------------------------------------------------------------------------------*/


#include <stdint.h>
#include "msp430fr2355.h"
#include <driverlib.h>

#include <msp430.h>
#include <stdint.h>
#include "stdbool.h"
#include <gpio.h>

#include "keypad.h"
#define LED_Address 0x013
#define LCD_Address 0x046
#define TempThreshold 1000.0

void Init_ADC();
void Init_I2C();
void LCDFormat();
void ADCToTemp();
void TempConversion();
void ADCSave();
void ADCAverage();
void ADCDataReset();
void TransmitButton();
void TransmitLCD();

//Vairable Declarations-----------------------------------------------------
uint8_t State = 0; // 0 - wait for key, 1-3 - correct button pressed for password,

uint8_t TransmitMode = 0;
bool TimerFlag = false;
bool LEDModeD = false; // If LED Mode is set to D
bool LevelFlag = false; // If ADC was last under level
 uint8_t j = 0;

// Keypad
char LastButton = 0;
bool CheckFlag = false;

// LCD Output
volatile bool TransmitToLCD = true;
volatile char LCDMessage[32] = "12345678901234567890123456789012";
volatile uint8_t LCDPointer = 0;
volatile uint8_t LCDCounter = 0;

// Temp
volatile uint16_t ADCResult = 0;
uint16_t Data[10];
volatile uint8_t DataPointer = 0;
volatile uint8_t WindowValue = 3;
float AveragedTemp = 0;
volatile float Celsius;
volatile int Kelvin;


//-----------------------------------------------------------------------

int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Initialization
    Init_Keypad();
    Init_I2C();
    Init_ADC();

    GPIO_setAsOutputPin(1, 0x04);
    P1OUT |= BIT3;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    TimerFlag = true;
    TB0CTL |= TBSSEL__ACLK + MC__UP + ID__2;
    TB0CCR0 = 5460;
    TB0R = 0;
    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;

    __enable_interrupt();
    LCDFormat();
    TransmitLCD();
    __delay_cycles(5000);

    while(1) {
        if (CheckFlag) {
            LEDModeD = false;
            CheckButton();
            TransmitButton();
            continue;
        }
        if (LEDModeD && AveragedTemp != 0) {

            if ((!LevelFlag && (AveragedTemp > TempThreshold)) || (LevelFlag && (AveragedTemp <= TempThreshold))) {
                TransmitMode++;
                TransmitButton();
            }

        }
        switch (State) {
            case 0:
                ADCCTL0 |= ADCENC | ADCSC;                           // Sampling and conversion start
                __bis_SR_register(LPM0_bits | GIE);                  // LPM0, ADC_ISR will force exit
                __no_operation();                                    // For debug only
                State++;
                break;
            case 2:
                ADCSave();
                ADCAverage();
                if (LCDCounter == 6) {
                    LCDFormat();
                    TransmitLCD();
                    LCDCounter = 0;
                } else
                    LCDCounter++;
                State++;
                break;
            case 99: // Test state for continuous transmit
                LCDFormat();
                TransmitLCD();
                __delay_cycles(5000);
                break;
            default:
                break;
        }

    }
}

void Init_I2C() {

    UCB1CTLW0 |= UCSWRST; // Put UCB1CTLW0 into software reset
    UCB1CTLW0 |= UCSSEL_3; // Select mode 3
    UCB1BRW = 10; // Something useful

    UCB1CTLW0 |= UCMODE_3; // Mode 3
    UCB1CTLW0 |= UCMST; // Master
    UCB1CTLW0 |= UCTR; // Transmit mode

    UCB1CTLW1 |= UCASTP_2; // Autostop enabled

    UCB1CTLW0 &= ~UCSWRST; // Take out of Software Reset

    UCB1IE |= UCTXIE0; // Enable TX interrupt
    UCB1IE |= UCRXIE0; // Enable RX interrupt

    //----- P4.6 and P4.7 for I2C ---
    P4SEL1 &= ~BIT7;
    P4SEL0 |= BIT7;

    P4SEL1 &= ~BIT6;
    P4SEL0 |= BIT6;
}

void Init_ADC() {
    // Configure ADC A1 pin
    P1SEL0 |= BIT1;
    P1SEL1 |= BIT1;
    // Configure ADC12
    ADCCTL0 |= ADCSHT_2 | ADCON;                             // ADCON, S&H=16 ADC clks
    ADCCTL1 |= ADCSHP;                                       // ADCCLK = MODOSC; sampling timer
    ADCCTL2 &= ~ADCRES;                                      // clear ADCRES in ADCCTL
    ADCCTL2 |= ADCRES_2;                                     // 12-bit conversion results
    ADCMCTL0 |= ADCINCH_1;                                   // A1 ADC input select; Vref=AVCC
    ADCIE |= ADCIE0;                                         // Enable ADC conv complete interrupt


}

void ADCSave() {
    Data[DataPointer] = ADCResult;
    DataPointer++;
    if (DataPointer == WindowValue) {
        DataPointer = 0;
    }
}

void ADCAverage() {
    AveragedTemp = 0;
    for (j = 0; j < WindowValue; j++) {
        if (Data[j] == 0) {
            AveragedTemp = 0;
            return;
        }
        AveragedTemp += (float) Data[j];
    }
    AveragedTemp = AveragedTemp / (float) WindowValue;
}
void ADCDataReset() {
    ADCCTL0 &= ~(ADCENC | ADCSC);                           // Sampling and conversion start
    DataPointer = 0;
    ADCResult = 0;
    AveragedTemp = 0;
    LCDPointer = 0;
//    Celsius = 0;
    Kelvin = 0;
    State=2;
    TB0R = 0;
    Data[0]=0;
    Data[1]=0;
    Data[2]=0;
    Data[3]=0;
    Data[4]=0;
    Data[5]=0;
    Data[6]=0;
    Data[7]=0;
    Data[8]=0;
    Data[9]=0;
}

void TransmitButton() {
//    UCB1TBCNT = 1;
//    UCB1I2CSA = LCD_Address; // Set the slave address in the module
//    //...equal to the slave address
//    UCB1CTLW0 |= UCTR; // Put into transmit mode
//    UCB1CTLW0 |= UCTXSTT; // Generate the start condition

    switch(LastButton) {
        case '0':
            break;
        case 'D':
            LEDModeD = true;
        case '*':
        case 'A':
        case 'B':
        case 'C':
            TransmitMode++;
            UCB1TBCNT = TransmitMode;
            UCB1I2CSA = LED_Address;
            UCB1CTLW0 |= UCTR;
            UCB1CTLW0 |= UCTXSTT;
            break;
        case '#':
            ADCDataReset();
            LCDFormat();
            TransmitLCD();
            __delay_cycles(5000);
            LCDCounter = 0;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
//            TransmitMode++;
            while (LCDPointer != 0) {}
            ADCDataReset();
            LCDFormat();
            TransmitLCD();
            __delay_cycles(5000);
            WindowValue = LastButton - 48;
            LCDCounter = 0;
//            LCDFormat();
 //           TransmitLCD();
            break;
        default:
            break;
    }

}

void TransmitLCD() {
    LCDPointer = 0;
    UCB1TBCNT = 32;
    UCB1I2CSA = LCD_Address; // Set the slave address in the module equal to the slave address
    UCB1CTLW0 |= UCTR; // Put into transmit mode
    UCB1CTLW0 |= UCTXSTT; // Generate the start condition
}

void LCDFormat() {

}

void ADCToTemp() {

}

void TempConversion() {

}


//-- Interrupt Service Routines -----------------------------------------------------------
#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void) {
    switch (TransmitMode) {
        case 0:
            UCB1TXBUF = LCDMessage[LCDPointer];
            LCDPointer++;
            if (LCDPointer == 32) {
                LCDPointer = 0;
            }
            break;
        case 1:
            UCB1TXBUF = LastButton;
            TransmitMode--;
        case 2:
            LevelFlag = AveragedTemp > TempThreshold ? true : false;
            UCB1TXBUF = LevelFlag;
            TransmitMode--;
        default:
            TransmitMode = 0;
    }
}

//-ISR Timer B---------------------------------------------------------------------------
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B_ISR(void){
    State = 0;

    // Clear interrupt flag
    TB0CCTL0 &= ~CCIFG;
}//-- End Timer_B_ISR ------------------------------------------------------------------

//-ISR Timer B1 ------------------------------------------------------------------------
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer_B1_ISR(void){
    TB0CCTL1 &= ~CCIE;
    TB0CCTL1 &= ~CCIFG;
}//-- End Timer_B_ISR -----------------------------------------------------------------

// ADC interrupt service routine
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
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
            ADCResult = ADCMEM0;
            __bic_SR_register_on_exit(LPM0_bits);            // Clear CPUOFF bit from LPM0
            State++;
            break;
        default:
            break;
    }
}

