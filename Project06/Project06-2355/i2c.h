#ifndef I2C_H_
#define I2C_H_

#include <driverlib.h>

#define LED_Address 0x013
#define LCD_Address 0x046
#define RTC_Address 0x024
#define ADC_Address 0x035

#define StartTxLCD 0b00010000
#define StartTxLED 0b00100000
#define StartTxRTC 0b01000000
#define StartTxADC 0b10000000
#define TxLCD 0b00000001;
#define TxLED 0b00000010;
#define TxRTC 0b00000100;
#define TxADC 0b00001000;

#define PendingBits 0b11110000;

void Init_I2C();
void TransmitStart();
void TransmitLCD();
void TransmitLED();
void TransmitRTC();
void ReceiveRTC();
void TransmitADC();
void ReceiveADC();

extern char LastButton;
extern bool HeatCool;
extern uint8_t TransmitState; // 0 LCD 1 LED 2 RTC 3 ADC, 4 pending LCD, 5 pending LED, 6 pending RTC 7 pending ADC

// LCD Output
void LCDFormat();
char LCDMessage[32];
uint8_t LCDPointer;



#endif /* I2C_H_ */
