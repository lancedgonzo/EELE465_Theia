#ifndef ANALOGDIGITAL_H_
#define ANALOGDIGITAL_H_

#include "driverlib.h"
#include "stdbool.h"


extern uint8_t AveragingWindowValue;

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
