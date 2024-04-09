#include <keypad.h>
#include "driverlib.h"
#include <stdint.h>

uint8_t Button; // tracker of last button pressed
uint8_t i;

void Init_Keypad() {
    RowInput();
    // Interrupt enable
    GPIO_enableInterrupt(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_enableInterrupt(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_enableInterrupt(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_enableInterrupt(KEYPAD_ROW4(0), KEYPAD_ROW4(1));
    // Rising edge sensitivity
    GPIO_selectInterruptEdge(KEYPAD_ROW1(0), KEYPAD_ROW1(1), GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_selectInterruptEdge(KEYPAD_ROW2(0), KEYPAD_ROW2(1), GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_selectInterruptEdge(KEYPAD_ROW3(0), KEYPAD_ROW3(1), GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_selectInterruptEdge(KEYPAD_ROW4(0), KEYPAD_ROW4(1), GPIO_LOW_TO_HIGH_TRANSITION);

}

//-CheckButton: Call functions to check which button was pressed, save it in last button and debounce.
void CheckButton() {
    CheckCol();
    CheckRow();
    KeyToChar();
    SwitchDebounce();
    SecondaryState &= ~KeyPressedFlag;
}//--END CheckButton-----------------------------------------------------------------------------------

//-ColumnInput: Sets row pins as OUTPUT---------------------------------------------------------------------
void ColumnInput(){

    //Configure outputs on Rows
    GPIO_setAsOutputPin(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_setAsOutputPin(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_setAsOutputPin(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_setAsOutputPin(KEYPAD_ROW4(0), KEYPAD_ROW4(1));

    //Configure inputs on Collumns
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_COL1(0), KEYPAD_COL1(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_COL2(0), KEYPAD_COL2(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_COL3(0), KEYPAD_COL3(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_COL4(0), KEYPAD_COL4(1));

    //Set High on ROWS
    GPIO_setOutputHighOnPin(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_setOutputHighOnPin(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_setOutputHighOnPin(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_setOutputHighOnPin(KEYPAD_ROW4(0), KEYPAD_ROW4(1));

}//--END ColumnInput----------------------------------------------------------------------------------------


//-CheckCol: Sets row pins as OUTPUT---------------------------------------------------------------------
void CheckCol() {
    // Set necessary pins as inputs and outputs for reading columns
    ColumnInput();

    // Clear button column values, then toggle bits corresponding to column
    Button &= 0x0F0;
    Button = ((P1IN & (KEYPAD_COL1(1))) == (KEYPAD_COL1(1))) ? (Button | BIT0) : (Button & ~BIT0);
    Button = ((P1IN & (KEYPAD_COL2(1))) == (KEYPAD_COL2(1))) ? (Button | BIT1) : (Button & ~BIT1);
    Button = ((P2IN & (KEYPAD_COL3(1))) == (KEYPAD_COL3(1))) ? (Button | BIT2) : (Button & ~BIT2);
    Button = ((P2IN & (KEYPAD_COL4(1))) == (KEYPAD_COL4(1))) ? (Button | BIT3) : (Button & ~BIT3);

}//--END CheckCol-------------------------------------------------------------------------------------


//-Collumns: Sets Collumn pins as OUTPUT-------------------------------------------------------------
void RowInput(){
    //Set P0-3 on keyboard as outputs
    GPIO_setAsOutputPin(KEYPAD_COL1(0), KEYPAD_COL1(1));
    GPIO_setAsOutputPin(KEYPAD_COL2(0), KEYPAD_COL2(1));
    GPIO_setAsOutputPin(KEYPAD_COL3(0), KEYPAD_COL3(1));
    GPIO_setAsOutputPin(KEYPAD_COL4(0), KEYPAD_COL4(1));

    //Set P4-7 on keyboard as inputs with pull down
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_setAsInputPinWithPullDownResistor(KEYPAD_ROW4(0), KEYPAD_ROW4(1));

    //Set P0-3 on keyboard as HIGH
    GPIO_setOutputHighOnPin(KEYPAD_COL1(0), KEYPAD_COL1(1));
    GPIO_setOutputHighOnPin(KEYPAD_COL2(0), KEYPAD_COL2(1));
    GPIO_setOutputHighOnPin(KEYPAD_COL3(0), KEYPAD_COL3(1));
    GPIO_setOutputHighOnPin(KEYPAD_COL4(0), KEYPAD_COL4(1));

}//--END RowInputs---------------------------------------------------------------------------------

//-CheckRow:  Clear row bits, then toggle bits based on input-----------------------------------------
void CheckRow() {
    // Set necessary pins as inputs and outputs for reading columns
    RowInput();

    // Clear row portion of button, then toggle bits corresponding to row
    Button &= 0x00F;
    Button = ((P2IN & (KEYPAD_ROW1(1))) == (KEYPAD_ROW1(1))) ? (Button | BIT4) : (Button & ~BIT4);
    Button = ((P2IN & (KEYPAD_ROW2(1))) == (KEYPAD_ROW2(1))) ? (Button | BIT5) : (Button & ~BIT5);
    Button = ((P4IN & (KEYPAD_ROW3(1))) == (KEYPAD_ROW3(1))) ? (Button | BIT6) : (Button & ~BIT6);
    Button = ((P4IN & (KEYPAD_ROW4(1))) == (KEYPAD_ROW4(1))) ? (Button | BIT7) : (Button & ~BIT7);
}//--END CheckRow-----------------------------------------------------------------------------------

//-KeyToChar: Updating last button to current button-----------------------------------------------
void KeyToChar() {
    // Update last button with identified button pressed
    switch(Button) {
        case KEY_0: LastButton = '0'; break;
        case KEY_1: LastButton = '1'; break;
        case KEY_2: LastButton = '2'; break;
        case KEY_3: LastButton = '3'; break;
        case KEY_4: LastButton = '4'; break;
        case KEY_5: LastButton = '5'; break;
        case KEY_6: LastButton = '6'; break;
        case KEY_7: LastButton = '7'; break;
        case KEY_8: LastButton = '8'; break;
        case KEY_9: LastButton = '9'; break;
        case KEY_AST: LastButton = '*'; break;
        case KEY_POUND: LastButton = '#'; break;
        case KEY_A: LastButton = 'A'; break;
        case KEY_B: LastButton = 'B'; break;
        case KEY_C: LastButton = 'C'; break;
        case KEY_D: LastButton = 'D'; break;
    }
}//--End KeyToChar


//-SwitchDebounce(): -------------------------------------------------------------------------
void SwitchDebounce(){
    for(i=0; i<10; i++){}
}//--END SwitchDebounce---------------------------------------------------------------


//-ISR Port 4 ----------------------------------------------------------------------------
#pragma vector = PORT1_VECTOR
#pragma vector = PORT2_VECTOR
#pragma vector = PORT3_VECTOR
#pragma vector = PORT4_VECTOR
__interrupt void ISR_Button_Pressed(void) {
    SecondaryState |= KeyPressedFlag;
    GPIO_clearInterrupt(KEYPAD_ROW1(0), KEYPAD_ROW1(1));
    GPIO_clearInterrupt(KEYPAD_ROW2(0), KEYPAD_ROW2(1));
    GPIO_clearInterrupt(KEYPAD_ROW3(0), KEYPAD_ROW3(1));
    GPIO_clearInterrupt(KEYPAD_ROW4(0), KEYPAD_ROW4(1));
}//-- End ISR_P5_Button_Pressed ----------------------------------------------------------

