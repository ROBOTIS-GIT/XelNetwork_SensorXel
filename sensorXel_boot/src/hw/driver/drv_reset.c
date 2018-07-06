/*
 *  drv_reset.c
 *
 *  Created on: 2017. 6. 9.
 *      Author: Baram
 */

#include "hw.h"
#include "drv_reset.h"


//-- Internal Variables
//
static uint8_t reset_status;



//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//





bool drvResetInit()
{
  uint8_t ret = 0;


  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
  {
    ret |= (1<<_DEF_RESET_PIN);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
  {
    ret |= (1<<_DEF_RESET_POWER);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
  {
    ret |= (1<<_DEF_RESET_WDG);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
  {
    ret |= (1<<_DEF_RESET_WDG);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
  {
    ret |= (1<<_DEF_RESET_SOFT);
  }

  __HAL_RCC_CLEAR_RESET_FLAGS();



  if (ret & (1<<_DEF_RESET_WDG))
  {
    reset_status = _DEF_RESET_WDG;
  }
  else if (ret == (1<<_DEF_RESET_PIN))
  {
    reset_status = _DEF_RESET_PIN;
  }
  else if (ret == (1<<_DEF_RESET_SOFT))
  {
    reset_status = _DEF_RESET_SOFT;
  }
  else
  {
    reset_status = _DEF_RESET_POWER;
  }

  return true;
}

uint8_t drvResetGetStatus(void)
{
  return reset_status;
}

void drvResetClearFlag(void)
{
  __HAL_RCC_CLEAR_RESET_FLAGS();
}
