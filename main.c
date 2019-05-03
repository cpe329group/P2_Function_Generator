#include "utils.h"
#include "msp.h"
#include "spi.h"
#include "timerA.h"
#include "waveforms.h"
#include "keypad.h"
#include "lcd.h"
#include <stdarg.h>

/**
 * main.c
 */
#define freq24Mhz CS_CTL0_DCORSEL_4     //41.66 ns period

enum StateMachine {WAIT, POLL, SQUARE, SINE, SAWTOOTH};
enum StateMachine STATE = SQUARE;
enum StateMachine waveOut = SQUARE; //current waveform displayed on DAC

/*
 * Look Up Tables for WaveForms
 */
uint16_t squareLUT[] = {0, 4096};
uint16_t sineLUT[];
uint16_t sawtoothLUT[];
/*
 * End Look Up Tables
 */

int LUTInc = 1;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//Initialization Stuffs
	setDCO(freq24Mhz); //24MHz
	spi.init();
	keypad.enableInt();
	SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;               //Wake on exit from ISR.
	__enable_irq();                                     //Enable global interrupts
	__DSB();

    STATE = SQUARE;  //Initialize waveform to square
    int fIn = 100;  //initialize frequency at 100Hz
    int duty = 5;   //initialize duty to 50%

	while(1)
	{
	   char key = ' ';
	   while(STATE == WAIT)  //display waveforms until button is pressed
	   {
	       __sleep();
	   }
	   switch(STATE)
	   {
	   case POLL:
	       /*
           * Keypad Interrupt Disable Code
           */

          key = keypad.poll();

          switch(key)
          {
          case '9':
              waveOut = SAWTOOTH;
              break;
          case '8':
              waveOut = SINE;
              break;
          case '7':
              waveOut = SQUARE;
              break;
          case '6':
              break;
          case '5':
              fIn = 500;
              break;
          case '4':
              fIn = 400;
              break;
          case '3':
              fIn = 300;
              break;
          case '2':
              fIn = 200;
              break;
          case '1':
              fIn = 100;
              break;
          case '0':
              duty = 5;
              break;
          case '*':
              if(duty>10)
              {
                  duty-=1;
              }
              break;
          case '#':
              if(duty<9)
              {
                  duty+=9;
              }
              break;
          }
          keypad.release(key); //wait until button is released to avoid triggering another interrupt

          /*
           * Keypad Interrupt Enable Code
           */

          LUTInc = 0;
          STATE = waveOut; //Change state according to button
          break;

       case SQUARE:
          wave.square(fIn,duty);  //initialize timers for frequency
          waveOut = SQUARE; //update waveform
          STATE = WAIT; //move to wait state
          break;

       case SINE:
           wave.sine(fIn);
           waveOut = SINE;
           STATE = WAIT;
           break;
       case SAWTOOTH:
          wave.sawtooth(fIn);
          waveOut = SAWTOOTH;
          STATE = WAIT;
          break;
	   }
	}
}

void TA0_0_IRQHandler(void)
{
    if(TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
       static uint16_t dacVal = 0;
       TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
       switch(waveOut)
       {
       case SQUARE:
           dacVal = 4096;
           break;
       case SAWTOOTH:
           dacVal = sawtoothLUT[LUTInc];
           break;
       case SINE:
           dacVal = sineLUT[LUTInc];
           break;
       }

       LUTInc += 1;
       spi.toDAC(dacVal);
    }
}

void TA0_N_IRQHandler(void)
{
    if(TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG & waveOut == SQUARE)
    {
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
        spi.toDAC(0);
    }
}

/*
 * Key Press Interrupt Handler Code
 */
