#ifndef ANALOGDIGITAL_H_
#define ANALOGDIGITAL_H_

#include "driverlib.h"
#include "stdbool.h"
#include "definitions.h"

// State Variables
extern uint8_t State;
extern uint8_t SecondaryState;

// Local ADC Variables

// Remote ADC Variables

extern uint8_t AveragingWindowValue;


void Init_ADC();
void LocalADCStart();
void LocalADCDataReset();
void LocalADCSave();
void LocalADCAverage();
void RemoteADCDataReset();
void RemoteADCSave();
void RemoteADCAverage();



#endif /* ANALOGDIGITAL_H_ */
