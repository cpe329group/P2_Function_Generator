#include "waveforms.h"

/*
 * waveforms.c
 *
 *  Created on: May 2, 2019
 *      Author: Daniel Gutmann
 */

void square(int freq, int duty);
void sine(int freq);
void sawtooth(int freq);

void square(int freq, int duty)
{
    int fSys = 3000000;
    timerA.setVal(0, fSys/fIn * duty/10);
    timerA.setVal(1, fSys/fIn * (10-duty)/10);
    timerA.init(UP, 8);
}

void sine(int freq)
{
    int timeScaler = 1;
    int freqScaler = 1;

    timerA.setVal(0, freq*timeScaler);
    timerA.init(UP, 8);
}

void sawtooth(int freq)
{
    int timeScaler = 1;
    int freqScaler = 1;

    timerA.setVal(0, freq*timeScaler);
    timerA.init(UP, 8);
}

const struct waveformInterface wave =
{
     .square = square,
     .sine = sine,
     .sawtooth = sawtooth
};
