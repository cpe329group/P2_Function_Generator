/*
 * keypad.h
 *
 *  Created on: Apr 13, 2019
 *      Author: Robin
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "utils.h"

#define KPD_COL1 BIT4  //PIN3 (KEYPAD)
#define KPD_COL2 BIT5  //PIN1
#define KPD_COL3 BIT6  //PIN5

#define KPD_PORT P5
#define KPD_ROW1 BIT0  //PIN2
#define KPD_ROW2 BIT1  //PIN7
#define KPD_ROW3 BIT2  //PIN6
#define KPD_ROW4 BIT3  //PIN4

struct keypadInterface {
    void (*init)(void);
    char (*poll)(void);
    uint8_t (*pressed)(void);
    void (*release)(char key);
    void(*enableInt)(void);
};

extern const struct keypadInterface keypad;
#endif /* KEYPAD_H_ */
