#include "spi.h"

/*
 * spi.c
 *
 *  Created on: May 1, 2019
 *      Author: Daniel Gutmann
 */

void SPI_init(void);
void SPI_tx(uint8_t TxData);
void DAC_send(uint16_t DACData);


void SPI_init(void)
{
    P1->SEL0 |= BIT5|BIT6;                  //Set P1.5 and P1.6 as SPI pins functionality
    P1->SEL1 &= ~(BIT5|BIT6);
    P1->SEL0 &= ~DAC_CS;
    P1->SEL1 &= ~DAC_CS;
    P1->DIR|= DAC_CS;                         //set CS as output (P1.7)
    P1->OUT |= DAC_CS;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;             //Put eUSCI state machine in reset
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST |            //Remain in reset
                        EUSCI_B_CTLW0_MST |                 //Set SPI Master
                        EUSCI_B_CTLW0_SYNC |                //Set in synchronous mode
                        EUSCI_B_CTLW0_CKPL |                //Set clock polarity high
                        EUSCI_B_CTLW0_MSB;                  //MSB First

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;        //Use ACLK for clock source.
    EUSCI_B0->BRW = 0x02;                               //Baud rate prescaler of 2.
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;            //Start state machine.
}

void SPI_tx(uint8_t TXData)
{
    EUSCI_B0->TXBUF = TXData;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));        //Wait for TXBUF to be empty before sending data
}

void DAC_send(uint16_t DACData)
{

    uint8_t loByte, hiByte;

    P1->OUT &= ~DAC_CS; //Select DAC

    DACData = (DACData & 0x0FFF) |GAIN | SHDN;

    hiByte = DACData >> 8;             //Send upper byte
    loByte = DACData & 0x00FF;         //Send lower byte

    SPI_tx(hiByte);
    SPI_tx(loByte);

    P1->OUT |= DAC_CS; //deselect DAC
}

const struct spiInterface spi = {
    .init = SPI_init,
    .tx = SPI_tx,
    .toDAC = DAC_send
};



