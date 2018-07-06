/*
 * drv_gpio.c
 *
 *  Created on: 2017. 10. 27.
 *      Author: opus
 */

#include "hw.h"
#include "drv_gpio.h"

#define GPIO_INPUT            _DEF_INPUT
#define GPIO_INPUT_PULLUP     _DEF_INPUT_PULLUP
#define GPIO_INPUT_PULLDOWN   _DEF_INPUT_PULLDOWN
#define GPIO_OUTPUT           _DEF_OUTPUT
#define GPIO_OUTPUT_PULLUP    _DEF_OUTPUT_PULLUP
#define GPIO_OUTPUT_PULLDOWN  _DEF_OUTPUT_PULLDOWN

typedef struct
{
  GPIO_TypeDef       *port;
  uint16_t            pin;
} drv_gpio_t;

const drv_gpio_t g_drv_gpio_map_t[_HW_DEF_GPIO_CH_MAX] = {
  { GPIOA, GPIO_PIN_6 },   //0
  { GPIOA, GPIO_PIN_7 },   //1
  { GPIOB, GPIO_PIN_0 },   //2
  { GPIOB, GPIO_PIN_1 },   //3
};

void drvGpioInit(void)
{

}

void drvGpioPinMode(uint8_t channel, uint8_t mode)
{
  if(channel > _HW_DEF_GPIO_CH_MAX)
  {
    return;
  }

  GPIO_InitTypeDef GPIO_InitStruct;
  const drv_gpio_t* p_pindef = &g_drv_gpio_map_t[channel];

  GPIO_InitStruct.Pin = p_pindef->pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  switch (mode)
  {
    case GPIO_INPUT:
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      break;

    case GPIO_INPUT_PULLUP:
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      break;

    case GPIO_INPUT_PULLDOWN:
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLDOWN;
      break;

    case GPIO_OUTPUT:
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      break;

    case GPIO_OUTPUT_PULLUP:
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      break;

    case GPIO_OUTPUT_PULLDOWN:
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_PULLDOWN;
      break;
  }
  HAL_GPIO_Init(p_pindef->port, &GPIO_InitStruct);
}

void drvGpioPinWrite(uint8_t channel, uint8_t bit)
{
  if(channel > _HW_DEF_GPIO_CH_MAX)
  {
    return;
  }

  const drv_gpio_t* p_pindef = &g_drv_gpio_map_t[channel];

  switch (bit)
  {
    case _DEF_LOW:
      HAL_GPIO_WritePin(p_pindef->port, p_pindef->pin, GPIO_PIN_RESET);
      break;
    case _DEF_HIGH:
      HAL_GPIO_WritePin(p_pindef->port, p_pindef->pin, GPIO_PIN_SET);
      break;
    default:
      break;
  }
}

int drvGpioPinRead(uint8_t channel)
{
  if(channel > _HW_DEF_GPIO_CH_MAX)
  {
    return _DEF_LOW;
  }

  const drv_gpio_t* p_pindef = &g_drv_gpio_map_t[channel];

  if (HAL_GPIO_ReadPin(p_pindef->port, p_pindef->pin) == GPIO_PIN_RESET)
  {
    return _DEF_LOW;
  }
  return _DEF_HIGH;
}

