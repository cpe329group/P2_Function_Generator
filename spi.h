/*
 * spi.h
 *
 *  Created on: May 1, 2019
 *      Author: Daniel Gutmann
 */

#ifndef SPI_H_
#define SPI_H_

#include "msp.h"

#define DAC_CS  BIT7
#define GAIN    BITD
#define SHDN    BITC

struct spiInterface
{
    void (*init)(void);
    void (*tx)(uint8_t TxData);
    void (*toDAC)(uint16_t DACData);
};

extern const struct spiInterface spi;

#endif /* SPI_H_ */
