/*
 * drv_i2c.c
 *
 *  Created on: 2017. 4. 10.
 *      Author: D.ggavy
 */

#include <stdarg.h>
#include <stdbool.h>

#include "hw.h"
#include "drv_i2c.h"


//-- Internal Variables
//
static I2C_HandleTypeDef hI2C1;

//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//



bool drvI2CInit(void)
{
  bool ret = true;

  ret = drvI2CBegin(100000);

  return ret;
}

bool drvI2CBegin(uint32_t freq)
{
  uint32_t i2c_freq = 400000; //MAX 400kHz

  if(freq < i2c_freq)
  {
    i2c_freq = freq;
  }

  hI2C1.Instance             = I2C1;
  hI2C1.Init.ClockSpeed      = i2c_freq;
  hI2C1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
  hI2C1.Init.OwnAddress1     = 0x00;
  hI2C1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  hI2C1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hI2C1.Init.OwnAddress2     = 0x00;
  hI2C1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hI2C1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

  HAL_I2C_DeInit(&hI2C1);

  if(HAL_I2C_Init(&hI2C1) != HAL_OK)
  {
    return false;
  }

  return true;
}

err_code_t drvI2CWrites(uint8_t addr, uint8_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  err_code_t errcode = OK;
  HAL_StatusTypeDef ret;

  ret = HAL_I2C_Mem_Write(&hI2C1, (uint16_t)(addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, p_data, length, timeout);

  if( ret != HAL_OK )
  {
    errcode = ERR_I2C_WRITE;
  }


  return errcode;
}

err_code_t drvI2CReads(uint8_t addr, uint8_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  err_code_t errcode = OK;
  HAL_StatusTypeDef ret;


  ret = HAL_I2C_Mem_Read(&hI2C1, (uint16_t)(addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)p_data, length, timeout);

  if( ret != HAL_OK )
  {
    errcode = ERR_I2C_READ;
  }

  return errcode;
}



void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  if (hi2c->Instance == I2C1)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* SCL */
    GPIO_InitStruct.Pin       = GPIO_PIN_8;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SDA */
    GPIO_InitStruct.Pin       = GPIO_PIN_9;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{

  if (hi2c->Instance == I2C1)
  {
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
  }
}

