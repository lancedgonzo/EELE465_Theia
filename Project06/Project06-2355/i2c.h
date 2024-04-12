#ifndef I2C_H_
#define I2C_H_

#include <driverlib.h>
#include "definitions.h"

#define LED_Address 0x013
#define LCD_Address 0x046
#define RTC_Address 0x068
#define ADC_Address 0x048

void Init_I2C();
void TransmitStart();
void TransmitLCD();
void TransmitLED();
void TransmitRTC();
void ReceiveRTC();
void TransmitADC();
void ReceiveADC();
void ADCToTemp();
void RTCResetInit();

extern uint8_t SecondaryState;
extern uint8_t Setpoint;

extern char LastButton;
extern bool HeatCool;
extern uint8_t TransmitState; // 0 LCD 1 LED 2 RTC 3 ADC, 4 pending LCD, 5 pending LED, 6 pending RTC 7 pending ADC
extern uint8_t State;
extern uint8_t LocalAveragedData;
extern uint8_t RemoteAveragedData;
extern uint8_t ADCRxData[2];
extern uint8_t AveragingWindowValue;

// LCD Output
void LCDFormat();
char LCDMessage[32];
uint8_t LCDPointer;



#endif /* I2C_H_ */
