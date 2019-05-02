/*
 * timer.h
 *
 *  Created on: May 1, 2019
 *      Author: Daniel Gutmann
 */
#ifndef TIMERA_H_
#define TIMERA_H_

#include "msp.h"

enum Mode {STOP,UP, CONTINUOUS, UPDOWN};

struct timerAInterface
{
    void(*init)(enum Mode, uint16_t clkDiv);
    void(*setVal)(int timerReg, int val);
};

extern const struct timerAInterface timerA;
#endif /* TIMERA_H_ */
