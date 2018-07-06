/*
 * hw.c
 *
 *  Created on: 2018. 2. 2.
 *      Author: opusk
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


void hwInit(void)
{
  cmdifInit();
  qbufferInit();

  swtimerInit();
  timerInit();

  gpioInit();
  ledInit();
  buttonInit();
  adcInit();

  uartInit();
  eepromInit();
  flashInit();
  resetInit();

  dxlportInit();
  dxlcmdInit();

  i2cInit();
  spiInit();
  pwmInit(50000);

  muxInit();
  muxSetChannel(MUX_ID_1, MUX_UART_RX);
  muxSetChannel(MUX_ID_2, MUX_UART_TX);
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
  return timerGetMicros();
}

