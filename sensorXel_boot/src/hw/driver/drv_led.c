/*
 * drv_led.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */



#include <stdarg.h>
#include <stdbool.h>

#include "hw.h"
#include "drv_led.h"





//-- Internal Variables
//




//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//




typedef struct
{
  GPIO_TypeDef       *port;
  uint16_t            pin;
}drv_led_t;

drv_led_t drv_led_tbl[_HW_DEF_LED_CH_MAX] = {
    {GPIOA, GPIO_PIN_4},
};



bool drvLedInit(void)
{
  uint32_t i;
  GPIO_InitTypeDef  GPIO_InitStruct;

  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  for (i=0; i<_HW_DEF_LED_CH_MAX; i++)
  {
    GPIO_InitStruct.Pin   = drv_led_tbl[i].pin;
    HAL_GPIO_Init(drv_led_tbl[i].port, &GPIO_InitStruct);
  }

  return true;
}

bool drvLedGetState(uint8_t ch)
{
  GPIO_PinState pin_state = GPIO_PIN_RESET;
  bool ret = false;
  
  if (ch >= _HW_DEF_LED_CH_MAX)
  {
    return false;
  }

  pin_state = HAL_GPIO_ReadPin(drv_led_tbl[ch].port, drv_led_tbl[ch].pin);
  if (pin_state == GPIO_PIN_RESET) ret = true;

  return ret;
}

void drvLedSetState(uint8_t ch, bool led_state)
{
  GPIO_PinState pin_state;

  if (ch >= _HW_DEF_LED_CH_MAX) return;


  if (led_state == true )
  {
    pin_state = GPIO_PIN_RESET;
  }
  else
  {
    pin_state = GPIO_PIN_SET;
  }

  HAL_GPIO_WritePin(drv_led_tbl[ch].port, drv_led_tbl[ch].pin, pin_state);
}


