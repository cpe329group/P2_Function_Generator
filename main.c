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
enum StateMachine STATE;
enum StateMachine waveOut; //current waveform displayed on DAC

/*
 * Look Up Tables for WaveForms
 */
uint16_t squareLUT[] = {0, 4096};
uint16_t sineLUT[];
uint16_t sawtoothLUT[];
/*
 * End Look Up Tables
 */

int g_LUTInc = 1;
uint16_t g_dacVal; //value sent to DAC

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	setGPIO_even(P2, BIT7, OUTPUT);
	enum StateMachine State_Debug = WAIT;
	//Initialization Stuffs
	setDCO(freq24Mhz); //24MHz

	lcd.init();
	lcd.backlight(1);
	lcd.clear();
	spi.init();

	keypad.enableInt();
	timerA.init(STOP,0);
	SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;               //Wake on exit from ISR.
	__enable_irq();                                     //Enable global interrupts
	__DSB();

    STATE = SQUARE;  //Initialize waveform to square
    State_Debug = STATE;
    int fIn = 100;  //initialize frequency at 100Hz
    int duty = 5;   //initialize duty to 50%

	while(1)
	{
	   char key = ' ';
	   char* integerConvert[];
	   switch(STATE)
	   {
	   case POLL:

          key = keypad.poll();

          /*
           * Key Press Handle
           */
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
          /*
           * End Keypad Handle
           */

          keypad.release(key); //wait until button is released to avoid triggering another interrupt

          g_LUTInc = 0; //set to 0 to prevent over polling
          STATE = waveOut; //Change state according to button
          break;

       case SQUARE:
          iota(freq,integerConvert,16);
          lcd.clear();
          lcd.putstr(integerConvert);
          lcd.putstr("Hz Square Wave");
          lcd.line_2();
          lcd.putstr("")
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
	    while(STATE == WAIT)  //display waveforms until button is pressed
        {
           spi.toDAC(g_dacVal);
        }
	}
}

void TA0_0_IRQHandler(void)
{
    if(TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
       switch(waveOut)
       {
       case SQUARE:
           P2->OUT = BIT7;
           g_dacVal = 4095;
           break;
       case SAWTOOTH:
           g_dacVal = sawtoothLUT[g_LUTInc];
           break;
       case SINE:
           g_dacVal = sineLUT[g_LUTInc];
           break;
       }

       g_LUTInc += 1;
       TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    }
}

void TA0_N_IRQHandler(void)
{
    if(TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG & waveOut == SQUARE)
    {
        P2->OUT = 0;
        g_dacVal = 0;
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    }
}

void PORT_5_IRQHandler(void)
{
    if(P5->IFG)
    {
        P5->IFG = 0;
        STATE = POLL;
    }
}
/*
 * Key Press Interrupt Handler Code
 */
