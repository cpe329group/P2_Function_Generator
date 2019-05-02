/*
 * timer.c
 *
 *  Created on: May 1, 2019
 *      Author: Daniel Gutmann
 */

#include "timerA.h"

void init(enum Mode, uint16_t clkDiv);
void setVal(int timerReg, int Val);

void init(enum Mode selMode, uint16_t clkDiv)
{

    switch (clkDiv)
    {
    case 1:
        clkDiv = TIMER_A_CTL_ID__1;
        break;
    case 2:
        clkDiv = TIMER_A_CTL_ID__2;
        break;
    case 4:
        clkDiv = TIMER_A_CTL_ID__4;
        break;
    case 8:
        clkDiv = TIMER_A_CTL_ID__8;
        break;
    default:
        clkDiv = TIMER_A_CTL_ID__1;
    }

    uint8_t countMode;
    switch(selMode)
    {
    case STOP:
        countMode = TIMER_A_CTL_MC__STOP;
        break;
    case UP:
        countMode = TIMER_A_CTL_MC__UP;
        break;
    case UPDOWN:
        countMode = TIMER_A_CTL_MC__UPDOWN;
        break;
    case CONTINUOUS:
        countMode = TIMER_A_CTL_MC__CONTINUOUS;
        break;
    default:
        countMode = TIMER_A_CTL_MC__STOP;
    }

    //SET CTL BTIS FOR TIMER
    TIMER_A0->CTL |= 0X0000;

    TIMER_A0->CTL = TACLR |
                    countMode |                   //Count Mode
                    clkDiv |                      //Divide Clock
                    TIMER_A_CTL_SSEL__SMCLK;      //Use SMCLK as input clock

}

void setVal(int timerReg, int Val)
{
    init(STOP, 1);                                  //Stop timer if changing value
    TIMER_A0->CCTL[timerReg] = TIMER_A_CCTLN_CCIE;  //enables interrupts for timers whose value is set
    TIMER_A0->CCR[timerReg] = Val;                  //set values for necessary timers

    if (timerReg == 0)
    {
        NVIC->ISER[0] |= 1 << ((TA0_0_IRQn) & 31);
    }
    else
    {
        NVIC->ISER[0] |= 1 << ((TA0_N_IRQn) & 31);
    }
}

const struct timerAInterface timerA =
{
     .init = init,
     .setVal = setVal
};
