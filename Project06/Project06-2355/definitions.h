#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

// State
#define StateInit           0b00000011
#define PeltierBits         0b00000011
#define PeltierStateA       0b00000000
#define PeltierStateB       0b00000001
#define PeltierStateC       0b00000010
#define PeltierStateD       0b00000011
#define LocalADCBits        0b00001100
#define LocalADCIncrement   0b00000100
#define RemoteADCBits       0b00110000
#define RemoteADCIncrement  0b00010000
#define RemoteValueValid    0b01000000
#define LocalValueValid     0b10000000


// SecondaryState
#define SecondaryStateInit  0b00000000
#define KeyPressedFlag      0b00000001
#define TimerBits           0b00001110
#define KeypadModeToggle    0b00010000
#define RTCBits             0b11100000
#define RTCIncrement        0b00100000
#define RTCTxWait           0b00100000
#define RTCRxWait           0b01100000
#define RTCResetStart       0b10100000
#define RTCReset            0b11000000
#define RTCTxRxWait         0b01100000

// TransmitStatus
#define TransmitInit        0b00000000
#define StartTxLCD          0b00010000
#define StartTxLED          0b00100000
#define StartTxRTC          0b01000000
#define StartTxADC          0b10000000
#define TxLCD               0b00000001
#define TxLED               0b00000010
#define TxRTC               0b00000100
#define TxADC               0b00001000
#define PendingBits         0b11110000

#endif /* DEFINITIONS_H_ */
