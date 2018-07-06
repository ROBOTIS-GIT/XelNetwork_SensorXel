/*
 * dht22.c
 *
 *  Created on: Jan 18, 2018
 *      Author: Will Son
 *
 * Available GPIO Ports :
 * DAC #1, DAC #6, GPIO #8, GPIO #11,
 * SPI #8, SPI #9, CAN #1
 *
 */

#include <stdarg.h>
#include <stdbool.h>

#include "dht22.h"

#ifdef _USE_HW_DHT22
#include "hw.h"


//-- Internal Variables
//
#ifdef _USE_HW_CMDIF_DHT22
void dht22CmdifInit(void);
int dht22Cmdif(int argc, char **argv);
dht22_t p_dht22;
#endif

typedef struct
{
    uint8_t gpioCh;
    uint8_t extiCh;
} dht22_ch_t;

static dht22_ch_t dht22_ch_tbl[DHT22_CH_MAX] = {
    {16, _DEF_EXTI1},   //J0(_DEF_EXTI1),   GPIO6_1, DAC #1
    {17, _DEF_EXTI2},   //J1(_DEF_EXTI2),   GPIO6_2, DAC #6
    {7, _DEF_EXTI4},    //H3(_DEF_EXTI4),   GPIO1_8, GPIO #8
    {4, _DEF_EXTI12},   //G11(_DEF_EXTI12), GPIO1_5, GPIO #11
    {12, _DEF_EXTI14},  //H13(_DEF_EXTI14), GPIO4_1, SPI #8
    {13, _DEF_EXTI15},  //H14(_DEF_EXTI15), GPIO4_2, SPI #9
    {14, _DEF_EXTI16},  //H15(_DEF_EXTI16), GPIO5_1, CAN #1
};
//-- External Variables
//


//-- Internal Functions
//

bool dht22Init(void)
{
#ifdef _USE_HW_CMDIF_DHT22
  dht22CmdifInit();
#endif
  return true;
}


#ifdef _USE_HW_CMDIF_DHT11
void dht22CmdifInit(void)
{
  cmdifAdd("dht22", dht22Cmdif);
}

void dht22ExtiCallback(void *arg)
{
  p_dht22.pastEdgeTimeUs = p_dht22.currentEdgeTimeUs;
  p_dht22.currentEdgeTimeUs = micros();
  if((p_dht22.currentEdgeTimeUs - p_dht22.pastEdgeTimeUs) > RES_THRESHOLD_TIME_DHT22)
  {
    if(p_dht22.edgecount < 34)
    {
      if(p_dht22.edgecount > 1)
      {
        p_dht22.data |= (1 << (33 - p_dht22.edgecount));
      }
    }
    else
    {
      p_dht22.checksum |= (1 << (41 - p_dht22.edgecount));
    }
    p_dht22.edgecount++;
  }
  else
  {
    if(p_dht22.edgecount < 34)
    {
      if(p_dht22.edgecount > 1)
      {
        p_dht22.data |= (0 << (33 - p_dht22.edgecount));  //Save 0 by shifting bits(incoming signal)
      }
    }
    else
    {
      p_dht22.checksum |= (0 << (41 - p_dht22.edgecount));
    }
    p_dht22.edgecount++;
  }
}

void dht22ReadTempHumid(dht22_t *p_dht22)
{
  bool setInt = false;
  uint32_t timeout = 0;
  timeout = millis();

  //if new request is at least 2Hz
  if(timeout - p_dht22->prevRequestTimems > TIMEOUT_DHT22)
  {
    gpioPinMode(dht22_ch_tbl[p_dht22->ch].gpioCh, _DEF_OUTPUT);     //set GPIO pin as output
    gpioPinWrite(dht22_ch_tbl[p_dht22->ch].gpioCh, GPIO_PIN_RESET); //transmit request signal
    delayMillis(REQ_PD_TIME_DHT22);
    gpioPinWrite(dht22_ch_tbl[p_dht22->ch].gpioCh, GPIO_PIN_SET);   //transmit request signal

    setInt = extiAttachInterrupt(dht22_ch_tbl[p_dht22->ch].extiCh, _DEF_EXTI_FALLING, dht22ExtiCallback, NULL);

    while((millis() < (timeout + TIMEOUT_DHT22)) && (p_dht22->edgecount < 42))
    {
      //loop while until timeout occurs or receive all data(40bit)
    }

    if(setInt == true)
    {
      extiDetachInterrupt(dht22_ch_tbl[p_dht22->ch].extiCh);
      setInt = false;
    }

    p_dht22->pastEdgeTimeUs = 0;
    p_dht22->currentEdgeTimeUs = 0;
  }
  else
  {

  }
  p_dht22->prevRequestTimems = timeout;
}

float dht22Temp()
{
  float temp = 0;
  temp = (float)(int16_t)(p_dht22.data) / 10;
  return temp;
}

float dht22Humid()
{
  float humid = 0;
  humid = (float)(int16_t)(p_dht22.data >> 16) / 10;
  return humid;
}

int dht22Cmdif(int argc, char **argv)
{
  int ret = true;
  p_dht22.edgecount = 0;
  p_dht22.data = 0;
  p_dht22.checksum = 0;

  if(argc == 2)
  {
    p_dht22.ch = (uint8_t) strtoul((const char *)argv[1], (char **)NULL, (int)0);

    if ((p_dht22.ch < DHT22_CH_MAX) && (p_dht22.ch >= 0))
    {
      dht22ReadTempHumid(&p_dht22);
    }
    else
    {
      cmdifPrintf("channel range error \n");
      return -1;
    }

    //verify checksum
    if(p_dht22.checksum == (uint8_t)(p_dht22.data + (p_dht22.data >> 8) + (p_dht22.data >> 16) + (p_dht22.data >> 24)))
    {
      if(p_dht22.edgecount != 0)
      {
        cmdifPrintf("Temperature:%2.1f, Humidity:%2.1f\n", dht22Temp(), dht22Humid());
        ret = 1;
      }
      else
      {
        cmdifPrintf("Sensor is not detected!\n");
        ret = -1;
      }
    }
    else
    {
      cmdifPrintf("Corrupted Data\n");
      ret = -1;
    }
    delay(TIMEOUT_DHT22);  //Maximum operation frequency for DHT22 is 0.5Hz.
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf("dht22 channel[0~6]");
  }

  return ret;
}
#endif

#endif
