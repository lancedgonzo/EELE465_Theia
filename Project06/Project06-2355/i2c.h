#ifndef I2C_H_
#define I2C_H_

#include <driverlib.h>

#define LED_Address 0x013
#define LCD_Address 0x046
#define ADC_Address 0x002


void Init_I2C();
void TransmitLCD();
void TransmitLED();
void TransmitRTC();


extern uint8_t TransmitState; // 0 LCD 1 LED 2 RTC 3 ADC, pending? 4-8?

// LCD Output
void LCDFormat();
char LCDMessage[32];
uint8_t LCDPointer;



#endif /* I2C_H_ */
