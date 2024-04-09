#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "stdbool.h"

#define KEYPAD_COL1(r) (r == 0) ? 1 : 0x0040 // 1.6
#define KEYPAD_COL2(r) (r == 0) ? 1 : 0x0080 // 1.7
#define KEYPAD_COL3(r) (r == 0) ? 2 : 0x0001 // 2.0
#define KEYPAD_COL4(r) (r == 0) ? 2 : 0x0002 // 2.1
#define KEYPAD_ROW1(r) (r == 0) ? 2 : 0x0004 // 2.2
#define KEYPAD_ROW2(r) (r == 0) ? 2 : 0x0008 // 2.3
#define KEYPAD_ROW3(r) (r == 0) ? 4 : 0x0001 // 4.0
#define KEYPAD_ROW4(r) (r == 0) ? 4 : 0x0002 // 4.1

// Defining row and column input as keys
#define KEY_1 0x011     //  17
#define KEY_2 0x012     //  18
#define KEY_3 0x014     //  20
#define KEY_4 0x021     //  33
#define KEY_5 0x022     //  34
#define KEY_6 0x024     //  36
#define KEY_7 0x041     //  65
#define KEY_8 0x042     //  66
#define KEY_9 0x044     //  68
#define KEY_0 0x082     // 130
#define KEY_POUND 0x084 // 132
#define KEY_AST 0x081   // 129
#define KEY_A 0x018     //  24
#define KEY_B 0x028     //  40
#define KEY_C 0x048     //  72
#define KEY_D 0x088     // 136

extern bool KeyPressedFlag;
extern char LastButton;

//Function Declarations ------------------------------------------------------
void Init_Keypad(void);
void ColInput(void);
void CheckCol(void);
void RowInput(void);
void CheckRow(void);
void CheckButton(void);
void ButtonResponse(void);
void SwitchDebounce(void);
//--------------------------------------------------------------------------



#endif /* KEYPAD_H_ */
