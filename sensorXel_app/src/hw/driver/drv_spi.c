/*
 * drv_uart.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */



#include <stdarg.h>
#include <stdbool.h>

#include "drv_spi.h"
#include "hw.h"

typedef struct
{
  bool tx_done;
  uint8_t *p_tx_buf;
  uint8_t *p_tx_buf_next;
  uint32_t tx_length_next;
} spi_dma_buf_t;

typedef struct
{
  bool               is_open;
  bool               is_dma_init;
  bool               is_refresh;
  SPI_HandleTypeDef  h_spi;
  DMA_HandleTypeDef  hdma_tx;
  spi_dma_buf_t      dma_tx_buf;
  uint32_t           timeout;
  void              (*func_tx)(void);
} spi_t;




//-- Internal Variables
//
static spi_t spi_tbl[SPI_CH_MAX];


//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//


bool drvSpiInit(void)
{
  uint8_t i;

  for(i=0; i<SPI_CH_MAX; i++)
  {
    spi_tbl[i].is_open     = false;
    spi_tbl[i].is_dma_init = false;
    spi_tbl[i].is_refresh  = false;
    spi_tbl[i].func_tx     = NULL;
    spi_tbl[i].timeout     = 100;

    spi_tbl[i].dma_tx_buf.p_tx_buf         = NULL;
    spi_tbl[i].dma_tx_buf.p_tx_buf_next    = NULL;
    spi_tbl[i].dma_tx_buf.tx_done          = false;
    spi_tbl[i].dma_tx_buf.tx_length_next   = 0;

    drvSpiSetCS(i, _DEF_HIGH);
  }

  return true;
}


void drvSpiBegin(uint8_t spi_ch)
{
  spi_t  *p_spi;
  GPIO_InitTypeDef  GPIO_InitStruct;

  switch(spi_ch)
  {
    case _DEF_SPI1:
    default :

      GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull  = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      GPIO_InitStruct.Pin   = GPIO_PIN_1;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      p_spi = &spi_tbl[spi_ch];

      p_spi->h_spi.Instance               = SPI1;
      p_spi->h_spi.Init.Mode              = SPI_MODE_MASTER;
      p_spi->h_spi.Init.Direction         = SPI_DIRECTION_2LINES;
      p_spi->h_spi.Init.DataSize          = SPI_DATASIZE_8BIT;
      p_spi->h_spi.Init.CLKPolarity       = SPI_POLARITY_LOW;
      p_spi->h_spi.Init.CLKPhase          = SPI_PHASE_1EDGE;
      p_spi->h_spi.Init.NSS               = SPI_NSS_SOFT;
      p_spi->h_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; // 4 Mbit  (32M/2/8)
      p_spi->h_spi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
      p_spi->h_spi.Init.TIMode            = SPI_TIMODE_DISABLE;
      p_spi->h_spi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
      p_spi->h_spi.Init.CRCPolynomial     = 7;

      HAL_SPI_DeInit(&p_spi->h_spi);
      HAL_SPI_Init(&p_spi->h_spi);

      p_spi->is_open = true;
      break;
  }
}


bool drvSpiTransfer(uint8_t spi_ch, uint8_t *p_tx_data, uint8_t *p_rx_data, uint32_t length)
{
  bool ret = false;
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return false;

  if (p_rx_data == NULL)
  {
    if (HAL_SPI_Transmit(&p_spi->h_spi, p_tx_data, length, p_spi->timeout) == HAL_OK)
    {
      ret = true;
    }
  }
  else if (p_tx_data == NULL)
  {
    if (HAL_SPI_Receive(&p_spi->h_spi, p_rx_data, length, p_spi->timeout) == HAL_OK)
    {
      ret = true;
    }
  }
  else
  {
    if (HAL_SPI_TransmitReceive(&p_spi->h_spi, p_tx_data, p_rx_data, length, p_spi->timeout) == HAL_OK)
    {
      ret = true;
    }
  }

  return ret;
}


uint8_t drvSpiTransfer8(uint8_t spi_ch, uint8_t data)
{
  uint8_t ret;
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return 0;


  drvSpiTransfer(spi_ch, &data, &ret, 1);

  return ret;
}


uint16_t drvSpiTransfer16(uint8_t spi_ch, uint16_t data)
{
  uint8_t tBuf[2];
  uint8_t rBuf[2];
  uint16_t ret;
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return 0;


  tBuf[1] = (uint8_t)data;
  tBuf[0] = (uint8_t)(data>>8);
  drvSpiTransfer(spi_ch, (uint8_t *)&tBuf, (uint8_t *)&rBuf, 2);

  ret = rBuf[0];
  ret <<= 8;
  ret += rBuf[1];

  return ret;
}

bool drvSpiWrite(uint8_t spi_ch, uint8_t *p_data, uint32_t length)
{
  return drvSpiTransfer(spi_ch, p_data, NULL, length);
}

bool drvSpiWrite8(uint8_t spi_ch, uint8_t data)
{
  return drvSpiTransfer(spi_ch, &data, NULL, 1);
}

bool drvSpiWrite16(uint8_t spi_ch, uint16_t data)
{
  return drvSpiTransfer(spi_ch, (uint8_t *)&data, NULL, 2);
}

bool drvSpiRead(uint8_t spi_ch, uint8_t *p_data, uint32_t length)
{
  return drvSpiTransfer(spi_ch, NULL, p_data, length);
}

bool  drvSpiRead8(uint8_t spi_ch, uint8_t *p_data)
{
  return drvSpiTransfer(spi_ch, NULL, p_data, 1);
}

bool  drvSpiRead16(uint8_t spi_ch, uint16_t *p_data)
{
  return drvSpiTransfer(spi_ch, NULL, (uint8_t *)p_data, 2);
}

void drvSpiSetBitOrder(uint8_t spi_ch, uint8_t bitOrder)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;

  p_spi->h_spi.Init.FirstBit = bitOrder;
  HAL_SPI_Init(&p_spi->h_spi);
}


void drvSpiSetClockDivider(uint8_t spi_ch, uint8_t clockDiv)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;

  p_spi->h_spi.Init.BaudRatePrescaler = clockDiv;
  HAL_SPI_Init(&p_spi->h_spi);
}


void drvSpiSetDataMode(uint8_t spi_ch, uint8_t dataMode)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;


  switch( dataMode )
  {
    // CPOL=0, CPHA=0
    case SPI_MODE0:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;

    // CPOL=0, CPHA=1
    case SPI_MODE1:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;

    // CPOL=1, CPHA=0
    case SPI_MODE2:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;

    // CPOL=1, CPHA=1
    case SPI_MODE3:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;
  }
}

void drvSpiSetTimeout(uint8_t spi_ch, uint32_t timeout)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;

  p_spi->timeout = timeout;
}

uint32_t drvSpiGetTimeout(uint8_t spi_ch)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return 0;

  return p_spi->timeout;
}

void drvSpiSetCS(uint8_t spi_ch, uint8_t state)
{
  switch(spi_ch)
  {
    case _DEF_SPI1:
      if (state == _DEF_HIGH)
      {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
      }
      else
      {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
      }
      break;
  }
}



void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;



  if (hspi->Instance == spi_tbl[_DEF_SPI1].h_spi.Instance)
  {

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable SPI clock */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_4;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_5;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}


void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{


  if (hspi->Instance == spi_tbl[_DEF_SPI1].h_spi.Instance)
  {
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_SPI1_FORCE_RESET();
    __HAL_RCC_SPI1_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);
    /* Configure SPI MISO as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5);
  }
}


