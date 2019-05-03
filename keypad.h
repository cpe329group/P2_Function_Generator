/*
 * keypad.h
 *
 *  Created on: Apr 13, 2019
 *      Author: Robin
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "utils.h"

struct keypadInterface {
    void (*init)(void);
    char (*poll)(void);
    uint8_t (*pressed)(void);
    void (*release)(char key);
    void(*enableInt)(void);
    void(*disableInt)(void);
};

extern const struct keypadInterface keypad;
#endif /* KEYPAD_H_ */
