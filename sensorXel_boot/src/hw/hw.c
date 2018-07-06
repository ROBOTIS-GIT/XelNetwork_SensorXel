/*
 * hw.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */



#include "hw.h"





//-- Internal Variables
//


//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//

static uint32_t microsGetMicros(void);



void hwInit(void)
{
  qbufferInit();
  ledInit();

  muxInit();
  muxSetChannel(0, 0);
  muxSetChannel(1, 1);
  uartInit();
  dxlportInit();
  flashInit();
  resetInit();
}

void delay(uint32_t delay_ms)
{
  HAL_Delay(delay_ms);
}

void delayMillis(uint32_t delay_ms)
{
  HAL_Delay(delay_ms);
}

void delayMicros(uint32_t delay_us)
{
  uint32_t tickstart = 0;

  tickstart = micros();
  while((micros() - tickstart) < delay_us)
  {
  }
}

void delaySeconds(uint32_t delay_sec)
{

}

uint32_t millis(void)
{
  return HAL_GetTick();
}


uint32_t micros(void)
{
  return microsGetMicros();
}


static TIM_HandleTypeDef    TimHandle;

void microsInit(void)
{
  uint32_t uwPrescalerValue = 0;

  __HAL_RCC_TIM2_CLK_ENABLE();

  // Compute the prescaler value to have TIMx counter clock equal to 1Mh
  uwPrescalerValue = (uint32_t)((SystemCoreClock / 1) / 1000000) - 1;

  TimHandle.Instance = TIM2;
  TimHandle.Init.Period            = 0xFFFFFFFF;
  TimHandle.Init.Prescaler         = uwPrescalerValue;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;

  HAL_TIM_Base_Init(&TimHandle);
  HAL_TIM_Base_Start(&TimHandle);
}

static uint32_t microsGetMicros(void)
{
  return TimHandle.Instance->CNT;
}


