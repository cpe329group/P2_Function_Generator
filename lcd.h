/*
 * lcd.h
 *
 *  Created on: Apr 10, 2019
 *      Author: Robin
 */

#ifndef LCD_H_
#define LCD_H_

#include "utils.h"


struct lcdInterface {
    void (*init)(void);
    void (*clear)(void);
    void (*home)(void);
    void (*putchar)(uint8_t c);
    void (*putstr)(uint8_t* str);
    void (*line_2)(void);
    void (*backlight)(uint8_t on);
    void (*setCursor)(uint8_t address);
    void (*command)(uint8_t data);
    void (*write)(uint8_t data);
    //Put any other functions in the lcd library here.
};

extern const struct lcdInterface lcd;
#endif /* LCD_H_ */
