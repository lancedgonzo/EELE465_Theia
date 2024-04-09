#ifndef ANALOGDIGITAL_H_
#define ANALOGDIGITAL_H_

#include "driverlib.h"
#include "stdbool.h"


uint16_t ADCResult = 0;
uint16_t ADCData[10];
uint8_t DataPointer = 0;
uint8_t WindowValue = 3;
float AveragedTemp = 0;
uint8_t j = 0;

extern uint8_t State;

/* ADC Result */
extern unsigned int functiongenerator_ADC_Result;


void Init_ADC();
void ADCStart();
void ADCDataReset();
void ADCSave();
void ADCAverage();
bool CheckTempThreshold();


#endif /* ANALOGDIGITAL_H_ */
