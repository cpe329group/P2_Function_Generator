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
    void(*square)(int freq, int duty);
    void(*sine)(int freq);
    void(*sawtooth)(int freq);
};

extern const struct waveformInterface wave;

#endif /* WAVEFORMS_H_ */
