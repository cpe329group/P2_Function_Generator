#include "msp.h"
#include <stdio.h>
#include <utils.h>

void setGPIO_odd(DIO_PORT_Odd_Interruptable_Type* PORT, uint8_t pin, enum Direction dir){
    PORT->SEL0 &= ~(pin);         //Select function GPIO
    PORT->SEL1 &= ~(pin);         //Select function GPIO
    if(dir == OUTPUT){
        PORT->DIR |= (pin);       //Set direction bit; set OUTPUT
    } else if(dir == INPUT_PD){
        PORT->DIR &= ~(pin);      //Set clear direction bit; set INPUT
        PORT->REN |= (pin);       //Enable resistor
        PORT->OUT &= ~(pin);      //Set pulldown
    } else if(dir == INPUT_PU){
        PORT->DIR &= ~(pin);      //Set clear direction bit; set INPUT
        PORT->REN |= (pin);       //Enable resistor
        PORT->OUT |= (pin);       //Set pullup
    } else {
        PORT->DIR &= ~(pin);      //Set clear direction bit; set INPUT
        PORT->REN &= ~(pin);      //Disable resistor
    }
}

void setGPIO_even(DIO_PORT_Even_Interruptable_Type* PORT, uint8_t pin, enum Direction dir){
    PORT->SEL0 &= ~(pin);         //Select function GPIO
    PORT->SEL1 &= ~(pin);         //Select function GPIO
    if(dir == OUTPUT){
        PORT->DIR |= (pin);       //Set direction bit; set OUTPUT
    } else if(dir == INPUT_PD){
        PORT->DIR &= ~(pin);      //Set clear direction bit; set INPUT
        PORT->REN |= (pin);       //Enable resistor
        PORT->OUT &= ~(pin);      //Set pulldown
    } else if(dir == INPUT_PU){
        PORT->DIR &= ~(pin);      //Set clear direction bit; set INPUT
        PORT->REN |= (pin);       //Enable resistor
        PORT->OUT |= (pin);       //Set pullup
    } else {
        PORT->DIR &= ~(pin);      //Set clear direction bit; set INPUT
        PORT->REN &= ~(pin);      //Disable resistor
    }
}

void setDCO(int freq){
    CS->KEY = CS_KEY_VAL;                           //Unlock CS module.
    CS->CTL0 = 0;                                   //Clear CTL0
    // ACLK = REFO, SMCLK = MCLK = DCO
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;



    if(freq == freq48Mhz){
        /* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
         PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));

        /* Configure Flash wait-state to 1 for both banks 0 & 1 */
        FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
        FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;
        CS->CTL0 = freq48Mhz;
    } else {
        CS->CTL0 = freq;                            //Set frequency.
    }

    CS->KEY = 0;                                    //Lock CS module.
}

void delay_us(int us){
    int currFreq = (CS->CTL0 & CS_CTL0_DCORSEL_MASK);   //Get current DCO Frequency
    int i;

    /*Change us to ns, on every cycle, subtract time in ns until 0.*/
    if (us > 1000){
        switch (currFreq){
        case freq1_5Mhz:
            for(i = us * 100; i > 0; i -= 666);
            break;
        case freq3Mhz:
            for(i = us * 100; i > 0; i -= 333);
            break;
        case freq6Mhz:
            for(i = us * 100; i > 0; i -= 167);
            break;
        case freq12Mhz:
            for(i = us * 100; i > 0; i -= 83);
            break;
        case freq24Mhz:
            for(i = us * 100; i > 0; i -= 42);
            break;
        case freq48Mhz:
            for(i = us * 100; i > 0; i -= 23);
            break;
        default:
            for (i = 20000; i > 0; i--);
            break;
        }
    } else {        //If the delay is less than 1 ms, use these for loops instead.
        switch (currFreq){
        case freq1_5Mhz:
            for(i = us * 1000; i > 0; i -= 12050);
            break;
        case freq3Mhz:
            for(i = us * 1000; i > 0; i -= 6669);
            break;
        case freq6Mhz:
            for(i = us * 1000; i > 0; i -= 2299);
            break;
        case freq12Mhz:
            for(i = us * 1000; i > 0; i -= 950);
            break;
        case freq24Mhz:
            for(i = us * 1000; i > 0; i -= 440);
            break;
        case freq48Mhz:
            for(i = us * 1000; i > 0; i -= 239);
            break;
        default:
            for (i = 20000; i > 0; i--);
            break;
        }
    }
}
