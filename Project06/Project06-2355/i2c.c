#include <i2c.h>

#include <stdio.h>
#include "driverlib.h"

uint8_t TransmitCounter = 0;
// LCD Output
void LCDFormat();
char LCDMessage[32];
uint8_t LCDPointer;

//Plant temp LM92
volatile float RCelsius;
volatile int RCelsius_int;
char RCe[3];

//Area temp LM19
volatile float LCelsius;
volatile int LCelsius_int;
char LCe[3];


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

// TransmitState 0 LCD 1 LED 2 RTC 3 ADC, 4 pending LCD, 5 pending LED, 6 pending RTC 7 pending ADC
void TransmitStart() {
    if ((TransmitState & ~PendingBits) == 0) { // if nothing is transmitting
        if (TransmitState & StartTxLCD) // if LCD pending
            TransmitLCD();
        else if (TransmitState & StartTxLED) // if LED pending
            TransmitLED();
        else if (TransmitState & StartTxRTC) // if RTC pending
            TransmitRTC();
        else if (TransmitState & StartTxADC) // if ADC Pending
            /*TransmitADC();*/ReceiveADC();

    }
}

void TransmitLCD() {
    LCDFormat();

    TransmitState &= ~StartTxLCD;
    TransmitState |= TxLCD;
    TransmitCounter = 32;
    UCB1TBCNT = 32;
    UCB1I2CSA = LCD_Address; // Set the slave address in the module equal to the slave address
    UCB1CTLW0 |= UCTR; // Put into transmit mode
    UCB1CTLW0 |= UCTXSTT; // Generate the start condition
}

void TransmitLED() {
    TransmitState &= ~StartTxLED;
    TransmitState |= TxLED;
    TransmitCounter = 2;
    UCB1TBCNT = 2;
    UCB1I2CSA = LED_Address; // Set the slave address in the module equal to the slave address
    UCB1CTLW0 |= UCTR; // Put into transmit mode
    UCB1CTLW0 |= UCTXSTT; // Generate the start condition
}

void TransmitRTC() {
    TransmitState &= ~StartTxRTC;
    TransmitState |= TxRTC;
}

void ReceiveRTC() {

}

void TransmitADC() {
    TransmitState &= ~StartTxADC;
    TransmitState |= TxADC;
    TransmitCounter = 1;
    UCB1TBCNT = 1;
    UCB1I2CSA = ADC_Address; // Set the slave address in the module equal to the slave address
    UCB1CTLW0 |= UCTR; // Put into transmit mode
    UCB1CTLW0 |= UCTXSTT; // Generate the start condition
}

void ReceiveADC() {
    TransmitState &= ~StartTxADC;
    TransmitState |= TxADC;
    TransmitCounter = 2;
    UCB1TBCNT = 2;
    UCB1I2CSA = ADC_Address; // Set the slave address in the module equal to the slave address
    UCB1CTLW0 &= ~UCTR; // Put into Receive mode
    UCB1CTLW0 |= UCTXSTT; // Generate the start condition
}


//temp calibration
void ADCToTemp() {
    LCelsius = ((LocalAveragedData)/3100)*100;
    LCelsius_int = LCelsius*10;
    sprintf(LCe,"%d", LCelsius_int);
    if (LCe[1] == 0) {
        LCe[1] = LCe[0];
        LCe[0] = '0';
    }
    if (LCe[2] == 0) {
        LCe[2] = LCe[1];
        LCe[1] = '0';
    }

    RCelsius = 35.4; // a place holder until the ADC for the plant is made;
    RCelsius_int = RCelsius*10;
    sprintf(RCe,"%d", RCelsius_int);
    if (RCe[1] == 0) {
        RCe[1] = RCe[0];
        RCe[0] = '0';
    }
    if (RCe[2] == 0) {
        RCe[2] = RCe[1];
        RCe[1] = '0';
    }

}

void LCDFormat() {
    // clear LCDmessage[32]

    ADCToTemp();
    sprintf(LCDMessage, "Res=N   A:%c%c.%c CM:   s  P:%c%c.%c C", 'A' + (State & 0b00000011), LCe[0], LCe[1], LCe[2], RCe[0], RCe[1], RCe[2]);


}


#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void) {
    switch(TransmitState & ~PendingBits) {
        case 1: // LCD
            UCB1TXBUF = LCDMessage[32 - TransmitCounter];
            TransmitCounter--;
            break;
        case 2: // LED
            if (TransmitCounter == 2) {
                UCB1TXBUF = HeatCool;
            } else {
                UCB1TXBUF = LastButton;
            }
            TransmitCounter--;
            break;
        case 4: // RTC
        case 8: // ADC
            if (TransmitCounter == 2) {
                ADCRxData[0] = UCB1RXBUF;
            } else {
                ADCRxData[1] = UCB1RXBUF;
                State += RemoteADCIncrement;
            }
            //UCB1TXBUF = 0x00;
            TransmitCounter--;
            break;

    }
    if (TransmitCounter == 0) {
        TransmitState &= PendingBits;
    }
}
