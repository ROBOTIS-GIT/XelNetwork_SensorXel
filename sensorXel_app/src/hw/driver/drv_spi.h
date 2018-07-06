/*
 * drv_spi.h
 *
 *  Created on: 2017. 10. 19.
 *      Author: baram
 */

#ifndef DRV_SPI_H_
#define DRV_SPI_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"


#define SPI_CH_MAX            _HW_DEF_SPI_CH_MAX



#define SPI_CLOCK_DIV_1       0
#define SPI_CLOCK_DIV_2       SPI_BAUDRATEPRESCALER_2
#define SPI_CLOCK_DIV_4       SPI_BAUDRATEPRESCALER_4
#define SPI_CLOCK_DIV_8       SPI_BAUDRATEPRESCALER_8
#define SPI_CLOCK_DIV_16      SPI_BAUDRATEPRESCALER_16
#define SPI_CLOCK_DIV_32      SPI_BAUDRATEPRESCALER_32
#define SPI_CLOCK_DIV_64      SPI_BAUDRATEPRESCALER_64
#define SPI_CLOCK_DIV_128     SPI_BAUDRATEPRESCALER_128
#define SPI_CLOCK_DIV_256     SPI_BAUDRATEPRESCALER_256


#define SPI_MSB_FIRST         SPI_FIRSTBIT_MSB
#define SPI_LSB_FIRST         SPI_FIRSTBIT_LSB

#define SPI_MODE0             0
#define SPI_MODE1             1
#define SPI_MODE2             2
#define SPI_MODE3             3


bool      drvSpiInit(void);

void      drvSpiBegin(uint8_t spi_ch);
void      drvSpiSetBitOrder(uint8_t spi_ch, uint8_t bitOrder);
void      drvSpiSetClockDivider(uint8_t spi_ch, uint8_t clockDiv);
void      drvSpiSetDataMode(uint8_t spi_ch, uint8_t dataMode);

bool      drvSpiWrite(uint8_t spi_ch, uint8_t *p_data, uint32_t length);
bool      drvSpiWrite8(uint8_t spi_ch, uint8_t data);
bool      drvSpiWrite16(uint8_t spi_ch, uint16_t data);

bool      drvSpiRead(uint8_t spi_ch, uint8_t *p_data, uint32_t length);
bool      drvSpiRead8(uint8_t spi_ch, uint8_t *p_data);
bool      drvSpiRead16(uint8_t spi_ch, uint16_t *p_data);

bool      drvSpiTransfer(uint8_t spi_ch, uint8_t *p_tx_data, uint8_t *p_rx_data, uint32_t length);
uint8_t   drvSpiTransfer8(uint8_t spi_ch, uint8_t data);
uint16_t  drvSpiTransfer16(uint8_t spi_ch, uint16_t data);

void      drvSpiSetTimeout(uint8_t ch, uint32_t timeout);
uint32_t  drvSpiGetTimeout(uint8_t ch);

void      drvSpiSetCS(uint8_t spi_ch, uint8_t state);


#ifdef __cplusplus
}
#endif

#endif /* DRV_SPI_H_ */
