#ifndef UTILS_H_

#include "msp.h"

#define UTILS_H_
#define freq1_5Mhz CS_CTL0_DCORSEL_0    //666.6 ns period
#define freq3Mhz CS_CTL0_DCORSEL_1      //333.3 ns period
#define freq6Mhz CS_CTL0_DCORSEL_2      //166.6 ns period
#define freq12Mhz CS_CTL0_DCORSEL_3     //83.33 ns period
#define freq24Mhz CS_CTL0_DCORSEL_4     //41.66 ns period
#define freq48Mhz CS_CTL0_DCORSEL_5     //20.83 ns period

enum Direction {OUTPUT, INPUT, INPUT_PU, INPUT_PD};
enum PinState {LOW, HIGH};

void delay_us(int us);
void setDCO(int freq);

void setGPIO_odd(DIO_PORT_Odd_Interruptable_Type* PORT, uint8_t pin, enum Direction dir);
void setGPIO_even(DIO_PORT_Even_Interruptable_Type* PORT, uint8_t pin, enum Direction dir);

#endif /* UTILS_H_ */
