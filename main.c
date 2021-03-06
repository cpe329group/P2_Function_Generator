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

//STATE MACHINE INITIALIZATION
enum StateMachine {WAIT, POLL, SQUARE, SINE, SAWTOOTH};
enum StateMachine STATE;
enum StateMachine waveOut;

/*
 * Look Up Tables for WaveForms
 */
uint16_t sineLUT[] =    {0x800,0x824,0x849,0x86e,0x892,0x8b7,0x8dc,0x900,0x925,0x949,0x96d,0x991,0x9b5,0x9d9,0x9fd,0xa20,0xa44,0xa67,0xa8a,0xaac,
                          0xacf,0xaf1,0xb13,0xb35,0xb57,0xb78,0xb99,0xbba,0xbda,0xbfa,0xc1a,0xc39,0xc58,0xc77,0xc95,0xcb3,0xcd1,0xcee,0xd0a,0xd27,
                          0xd43,0xd5e,0xd79,0xd94,0xdae,0xdc7,0xde1,0xdf9,0xe11,0xe29,0xe40,0xe57,0xe6d,0xe83,0xe98,0xeac,0xec0,0xed4,0xee7,0xef9,
                          0xf0b,0xf1c,0xf2c,0xf3c,0xf4b,0xf5a,0xf68,0xf76,0xf83,0xf8f,0xf9b,0xfa6,0xfb0,0xfba,0xfc3,0xfcc,0xfd4,0xfdb,0xfe1,0xfe7,
                          0xfec,0xff1,0xff5,0xff8,0xffb,0xffd,0xffe,0xfff,0xfff,0xffe,0xffd,0xffb,0xff8,0xff5,0xff1,0xfec,0xfe7,0xfe1,0xfdb,0xfd4,
                          0xfcc,0xfc3,0xfba,0xfb0,0xfa6,0xf9b,0xf8f,0xf83,0xf76,0xf68,0xf5a,0xf4b,0xf3c,0xf2c,0xf1c,0xf0b,0xef9,0xee7,0xed4,0xec0,
                          0xeac,0xe98,0xe83,0xe6d,0xe57,0xe40,0xe29,0xe11,0xdf9,0xde1,0xdc7,0xdae,0xd94,0xd79,0xd5e,0xd43,0xd27,0xd0a,0xcee,0xcd1,
                          0xcb3,0xc95,0xc77,0xc58,0xc39,0xc1a,0xbfa,0xbda,0xbba,0xb99,0xb78,0xb57,0xb35,0xb13,0xaf1,0xacf,0xaac,0xa8a,0xa67,0xa44,
                          0xa20,0x9fd,0x9d9,0x9b5,0x991,0x96d,0x949,0x925,0x900,0x8dc,0x8b7,0x892,0x86e,0x849,0x824,0x800,0x7db,0x7b6,0x791,0x76d,
                          0x748,0x723,0x6ff,0x6da,0x6b6,0x692,0x66e,0x64a,0x626,0x602,0x5df,0x5bb,0x598,0x575,0x553,0x530,0x50e,0x4ec,0x4ca,0x4a8,
                          0x487,0x466,0x445,0x425,0x405,0x3e5,0x3c6,0x3a7,0x388,0x36a,0x34c,0x32e,0x311,0x2f5,0x2d8,0x2bc,0x2a1,0x286,0x26b,0x251,
                          0x238,0x21e,0x206,0x1ee,0x1d6,0x1bf,0x1a8,0x192,0x17c,0x167,0x153,0x13f,0x12b,0x118,0x106,0xf4,0xe3,0xd3,0xc3,0xb4,
                          0xa5,0x97,0x89,0x7c,0x70,0x64,0x59,0x4f,0x45,0x3c,0x33,0x2b,0x24,0x1e,0x18,0x13,0xe,0xa,0x7,0x4,
                          0x2,0x1,0x0,0x0,0x1,0x2,0x4,0x7,0xa,0xe,0x13,0x18,0x1e,0x24,0x2b,0x33,0x3c,0x45,0x4f,0x59,
                          0x64,0x70,0x7c,0x89,0x97,0xa5,0xb4,0xc3,0xd3,0xe3,0xf4,0x106,0x118,0x12b,0x13f,0x153,0x167,0x17c,0x192,0x1a8,
                          0x1bf,0x1d6,0x1ee,0x206,0x21e,0x238,0x251,0x26b,0x286,0x2a1,0x2bc,0x2d8,0x2f5,0x311,0x32e,0x34c,0x36a,0x388,0x3a7,0x3c6,
                          0x3e5,0x405,0x425,0x445,0x466,0x487,0x4a8,0x4ca,0x4ec,0x50e,0x530,0x553,0x575,0x598,0x5bb,0x5df,0x602,0x626,0x64a,0x66e,
                          0x692,0x6b6,0x6da,0x6ff,0x723,0x748,0x76d,0x791,0x7b6,0x7db,0x800};

uint16_t sawtoothLUT[] = {0xc,0x17,0x23,0x2f,0x3b,0x46,0x52,0x5e,0x69,0x75,0x81,0x8c,0x98,0xa4,0xb0,0xbb,0xc7,0xd3,0xde,0xea,
                          0xf6,0x101,0x10d,0x119,0x125,0x130,0x13c,0x148,0x153,0x15f,0x16b,0x176,0x182,0x18e,0x19a,0x1a5,0x1b1,0x1bd,0x1c8,0x1d4,
                          0x1e0,0x1eb,0x1f7,0x203,0x20f,0x21a,0x226,0x232,0x23d,0x249,0x255,0x260,0x26c,0x278,0x284,0x28f,0x29b,0x2a7,0x2b2,0x2be,
                          0x2ca,0x2d5,0x2e1,0x2ed,0x2f9,0x304,0x310,0x31c,0x327,0x333,0x33f,0x34a,0x356,0x362,0x36e,0x379,0x385,0x391,0x39c,0x3a8,
                          0x3b4,0x3bf,0x3cb,0x3d7,0x3e3,0x3ee,0x3fa,0x406,0x411,0x41d,0x429,0x434,0x440,0x44c,0x458,0x463,0x46f,0x47b,0x486,0x492,
                          0x49e,0x4a9,0x4b5,0x4c1,0x4cd,0x4d8,0x4e4,0x4f0,0x4fb,0x507,0x513,0x51e,0x52a,0x536,0x542,0x54d,0x559,0x565,0x570,0x57c,
                          0x588,0x593,0x59f,0x5ab,0x5b7,0x5c2,0x5ce,0x5da,0x5e5,0x5f1,0x5fd,0x608,0x614,0x620,0x62c,0x637,0x643,0x64f,0x65a,0x666,
                          0x672,0x67d,0x689,0x695,0x6a1,0x6ac,0x6b8,0x6c4,0x6cf,0x6db,0x6e7,0x6f2,0x6fe,0x70a,0x716,0x721,0x72d,0x739,0x744,0x750,
                          0x75c,0x767,0x773,0x77f,0x78b,0x796,0x7a2,0x7ae,0x7b9,0x7c5,0x7d1,0x7dc,0x7e8,0x7f4,0x800,0x80b,0x817,0x823,0x82e,0x83a,
                          0x846,0x851,0x85d,0x869,0x875,0x880,0x88c,0x898,0x8a3,0x8af,0x8bb,0x8c6,0x8d2,0x8de,0x8ea,0x8f5,0x901,0x90d,0x918,0x924,
                          0x930,0x93b,0x947,0x953,0x95f,0x96a,0x976,0x982,0x98d,0x999,0x9a5,0x9b0,0x9bc,0x9c8,0x9d4,0x9df,0x9eb,0x9f7,0xa02,0xa0e,
                          0xa1a,0xa25,0xa31,0xa3d,0xa49,0xa54,0xa60,0xa6c,0xa77,0xa83,0xa8f,0xa9a,0xaa6,0xab2,0xabe,0xac9,0xad5,0xae1,0xaec,0xaf8,
                          0xb04,0xb0f,0xb1b,0xb27,0xb32,0xb3e,0xb4a,0xb56,0xb61,0xb6d,0xb79,0xb84,0xb90,0xb9c,0xba7,0xbb3,0xbbf,0xbcb,0xbd6,0xbe2,
                          0xbee,0xbf9,0xc05,0xc11,0xc1c,0xc28,0xc34,0xc40,0xc4b,0xc57,0xc63,0xc6e,0xc7a,0xc86,0xc91,0xc9d,0xca9,0xcb5,0xcc0,0xccc,
                          0xcd8,0xce3,0xcef,0xcfb,0xd06,0xd12,0xd1e,0xd2a,0xd35,0xd41,0xd4d,0xd58,0xd64,0xd70,0xd7b,0xd87,0xd93,0xd9f,0xdaa,0xdb6,
                          0xdc2,0xdcd,0xdd9,0xde5,0xdf0,0xdfc,0xe08,0xe14,0xe1f,0xe2b,0xe37,0xe42,0xe4e,0xe5a,0xe65,0xe71,0xe7d,0xe89,0xe94,0xea0,
                          0xeac,0xeb7,0xec3,0xecf,0xeda,0xee6,0xef2,0xefe,0xf09,0xf15,0xf21,0xf2c,0xf38,0xf44,0xf4f,0xf5b,0xf67,0xf73,0xf7e,0xf8a,
                          0xf96,0xfa1,0xfad,0xfb9,0xfc4,0xfd0,0xfdc,0xfe8,0xff3,0xfff};
/*
 * End Look Up Tables
 */

int g_LUTInc = 1;
uint16_t g_dacVal; //value sent to DAC

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	setGPIO_even(P2, BIT7, OUTPUT);
	//Initialization Stuffs
	setDCO(freq24Mhz); //24MHz

	lcd.init();
	lcd.backlight(1);
	lcd.clear();
	spi.init();

	keypad.init();
	keypad.enableInt();
	timerA.init(STOP,0);

	__enable_irq();                                     //Enable global interrupts

    STATE = SQUARE;  //Initialize waveform to square
    int fIn = 100;  //initialize frequency at 100Hz
    int duty = 5;   //initialize duty to 50%

	while(1)
	{
	   char key = ' ';
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
              if(duty>1)
                  duty = duty - 1;
              break;
          case '#':
              if(duty<9)
              {
                  duty+=1;
              }
              break;
          }
          /*
           * End Keypad Handle
           */

          keypad.enableInt();
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
       if(g_LUTInc == 350)
       {g_LUTInc = 0;}
       TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    }
}

void TA0_N_IRQHandler(void)
{
    if(TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG & waveOut == SQUARE)
    {
        g_dacVal = 0;
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    }
}

void PORT5_IRQHandler(void)
{
    if(P5->IFG & (BIT4|BIT5|BIT6))
    {
        P5->IFG = 0;
        STATE = POLL;
    }
}
