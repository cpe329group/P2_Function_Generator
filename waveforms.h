/*
 * waveforms.h
 *
 *  Created on: May 2, 2019
 *      Author: Daniel Gutmann
 */

#ifndef WAVEFORMS_H_
#define WAVEFORMS_H_

#include "msp.h"
#include "timerA.h"
#include "spi.h"

struct waveformInterface
{
    void(*square)(int fIn, int duty);
    void(*sine)(int fIn);
    void(*sawtooth)(int fIn);
};

extern const struct waveformInterface wave;

#endif /* WAVEFORMS_H_ */
