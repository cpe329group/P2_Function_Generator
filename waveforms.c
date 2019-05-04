#include "waveforms.h"

/*
 * waveforms.c
 *
 *  Created on: May 2, 2019
 *      Author: Daniel Gutmann
 */

void square(int fIn, int duty);
void sine(int fIn);
void sawtooth(int fIn);

void square(int fIn, int duty)
{
    int fSys = 3000000;
    timerA.setVal(0, fSys/fIn);
    timerA.setVal(1, fSys/fIn*duty/10);
    timerA.init(UP, 8);
}

void sine(int fIn)
{
    int lenLUT = 100;
    int fSys = 3000000;
    timerA.setVal(0, fSys/fIn/lenLUT/7*2);
    timerA.init(UP, 8);
}

void sawtooth(int fIn)
{
    int lenLUT = 100;
    int fSys = 3000000;
    timerA.setVal(0, fSys/fIn/lenLUT/7*2);
    timerA.init(UP, 8);
}

const struct waveformInterface wave =
{
     .square = square,
     .sine = sine,
     .sawtooth = sawtooth
};
