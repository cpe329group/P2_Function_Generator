#include "utils.h"
#include "msp.h"
#include "spi.h"
#include <stdarg.h>

/**
 * main.c
 */

uint8_t squareLow = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	setDCO(CS_CTL0_DCORSEL_4); //24MHz
	spi.init();

	SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;               //Wake on exit from ISR.

	__enable_irq();                                     //Enable global interrupts
	__DSB();

	while(1)
	{
	    __sleep();
	}
}

void TA0_0_IRQHandler(void){
    if(TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
        squareLow ^= 0x1;       //Toggle squareLow
        if (squareLow)
        {
            spi.toDAC(2490);     //Send 2V at CCR[0] = 30000
        }
        else
        {
            spi.toDAC(0);            //Send 0V at CCR[0] = 30000
        }
    }
}

void TA0_N_IRQHandler(void){
    if(TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG){
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    }
}
