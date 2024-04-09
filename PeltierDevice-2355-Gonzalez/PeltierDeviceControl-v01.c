#include "driverlib.h"

int main(void) {

    WDT_A_hold(WDT_A_BASE);

    return (0);
}


//-ISR Timer B1 ------------------------------------------------------------------------
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer_B1_ISR(void){
    // TB0CCTL1 &= ~CCIE;
    TB0CCTL1 &= ~CCIFG;
}//-- End Timer_B_ISR -----------------------------------------------------------------