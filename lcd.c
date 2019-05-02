/*
 * lcd.c
 *
 *  To use this LCD Library, #define the pins and port you're connecting to.
 *  This library only supports using ONE port to interface with the LCD.
 *
 *  In main() run lcd.init() then everything should be ready to use.
 *  Recommend run lcd.clear() before printing anything to the screen.
 *
 *  Available commands:
 *
 *  init() - Initialize the display.
 *  clear() - Clears the screen.
 *  home() - Returns the cursor to home position, does not change display.
 *  putchar(c) - put a single character on the LCD screen
 *  putstr(str) - put a string (null terminated please) on the LCD screen.
 *  backlight(bool) - turn backlight on or off
 *  line_2() - Set cursor to the beginning of second line.
 *  setCursor(address) - Set cursor location.
 *
 *  Created on: Apr 10, 2019
 *      Author: Robin Morales
 */
#include "lcd.h"

#define RS_PIN BIT1
#define RW_PIN BIT2
#define EN_PIN BIT3
#define DB7 BIT7
#define DB6 BIT6
#define DB5 BIT5
#define DB4 BIT4
#define LCD_PORT P4


void command(uint8_t data);
void write(uint8_t data);
void sendData(void);
void LCD_init(void);
void RS(enum PinState state);
void RW(enum PinState state);
void EN(enum PinState state);
void clear(void);
void home(void);
void putchar(uint8_t c);
void putstr(uint8_t *str);
void line_2(void);
void setCursor(uint8_t address);
void primeData(uint8_t data);
void checkBusy(void);


/* CONSIDER THESE VALUES PRIVATE, DO NOT TOUCH */
static uint8_t dispMode = 0b00001000;
/* =========================================== */

void LCD_init(void){
    setGPIO_even(LCD_PORT, DB4, OUTPUT);
    setGPIO_even(LCD_PORT, DB5, OUTPUT);
    setGPIO_even(LCD_PORT, DB6, OUTPUT);
    setGPIO_even(LCD_PORT, DB7, OUTPUT);

    setGPIO_even(LCD_PORT, RS_PIN, OUTPUT);
    setGPIO_even(LCD_PORT, RW_PIN, OUTPUT);
    setGPIO_even(LCD_PORT, EN_PIN, OUTPUT);

    setGPIO_even(LCD_PORT, BIT0, OUTPUT); //Set back light pin to output.

    delay_us(15000);                    //Wait for LCD to turn on.
    primeData(0x03);                    //Put WAKEUP command on port
    delay_us(10000);                     //Wait 5ms, since busy flag is not available.
    sendData();                         //Send!

    delay_us(10000);                     //Wait for a while...
    sendData();                         //Send WAKEUP again.
    delay_us(10000);                     //Wait for a while...
    sendData();                         //Send WAKEUP again.
    delay_us(10000);                     //Wait for a while... busy flag now available.
    primeData(0x02);
    sendData();                         //Function set: 4-bit interface.

    command(0b00101000);                //Function set: 4-bit / 2-line.
    command(0b00010000);                //Set cursor (shift cursor left)
    command(0b00001111);                //Display ON; Blinking cursor.
    command(0b00000110);                //Entry mode set (cursor goes right, no display shift)
}

void primeData(uint8_t data){
    LCD_PORT->OUT &= 0b00001111;
    LCD_PORT->OUT |= 0b11110000 & data;
}

void checkBusy(void){
        uint8_t BUSY_FLAG = 0x00;
        setGPIO_even(LCD_PORT, DB7, INPUT); //Ready to read busy flag.
        int CNT = 0;
        RS(LOW);                            //Set RS LOW for command mode.
        RW(HIGH);                           //Set RW HIGH to read
        do {                                //When busy flag high, cannot send data...
            BUSY_FLAG = LCD_PORT->IN & DB7;
            CNT ++;
            delay_us(20);                   //Wait for busy flag to clear.
            if(CNT > 10){                   //Never get stuck in infinite loop
                break;
            }
        } while(BUSY_FLAG);
        setGPIO_even(LCD_PORT, DB7, OUTPUT);//Set DB7 back to output.
}

void sendData(void){
    EN(HIGH);
    delay_us(50);                           //Pulse width of EN must be >= 300 ns
    EN(LOW);                                //Clock on falling edge.
}

void RW(enum PinState state){
    if(state == HIGH){
        LCD_PORT->OUT |= RW_PIN;
    } else {
        LCD_PORT->OUT &= ~(RW_PIN);
    }
}

void RS(enum PinState state){
    if(state == HIGH){
        LCD_PORT->OUT |= RS_PIN;
    } else {
        LCD_PORT->OUT &= ~(RS_PIN);
    }
}

void EN(enum PinState state){
    if(state == HIGH){
        LCD_PORT->OUT |= EN_PIN;
    } else {
        LCD_PORT->OUT &= ~(EN_PIN);
    }
}

void clear(void){
    command(0x01);
    delay_us(20000);        //Wait for display to clear.
}

void home(void){
    command(0x02);
}

void putchar(uint8_t c){
    write(c);
}

void putstr(uint8_t *str){
    int i = 0;
    while(str[i]){
        write(str[i]);
        i++;
    }
}

void setCursor(uint8_t address){
    address |= 0x80;
    command(address);
}

void line_2(void){
    setCursor(0x40);
}

void backlight(uint8_t on){
    if(on){
            P4->OUT |= BIT0;
    } else {
        P4->OUT &= ~(BIT0);
    }
}

void command(uint8_t data){
    primeData(data);        //Put data on port.
    RS(LOW);                //D/I LOW to send instruction (RS).
    RW(LOW);                //R/W LOW to write.
    sendData();             //Send upper 4 bits.
    data = data<<4;         //Shift lower nibble to upper nibble.
    primeData(data);        //Put data on output port.
    sendData();             //Send lower 4 bits.
//    checkBusy();
}

void write(uint8_t data){
    primeData(data);        //Put data on port.
    RS(HIGH);               //D/I HIGH to send data.
    RW(LOW);                //R/W LOW to write.
    sendData();             //Send upper 4 bits.
    data = data<<4;         //Shift lower nibble to upper nibble.
    primeData(data);        //Put data on output port.
    sendData();             //Send lower 4 bits.
//    checkBusy();
}


const struct lcdInterface lcd = {
    .init = LCD_init,
    .clear = clear,
    .home = home,
    .putchar = putchar,
    .putstr = putstr,
    .line_2 = line_2,
    .backlight = backlight,
    .command = command,
    .write = write
};
