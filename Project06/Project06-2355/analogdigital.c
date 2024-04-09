#include <analogdigital.h>
#include <stdio.h>
#include <math.h>

#include "driverlib.h"


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

void ADCStart() {
    ADCCTL0 |= ADCENC | ADCSC;                           // Sampling and conversion start
    __bis_SR_register(LPM0_bits | GIE);                  // LPM0, ADC_ISR will force exit
    __no_operation();                                    // For debug only
}

void ADCDataReset() {
    DataPointer = 0;
    ADCResult = 0;
    ADCData[0]=0;
    ADCData[1]=0;
    ADCData[2]=0;
    ADCData[3]=0;
    ADCData[4]=0;
    ADCData[5]=0;
    ADCData[6]=0;
    ADCData[7]=0;
    ADCData[8]=0;
    ADCData[9]=0;
}

void ADCSave() {
    ADCData[DataPointer] = ADCResult;
    DataPointer++;
    if (DataPointer == WindowValue) {
        DataPointer = 0;
    }
}

void ADCAverage() {
    AveragedTemp = 0;
    for (j = 0; j < WindowValue; j++) {
        if (ADCData[j] == 0) {
            AveragedTemp = 0;
            return;
        }
        AveragedTemp += (float) ADCData[j];
    }
    AveragedTemp = AveragedTemp / (float) WindowValue;
}

// ADC interrupt service routine
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void) {
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

