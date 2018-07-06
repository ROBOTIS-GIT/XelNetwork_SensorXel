/*
 * dht11.c
 *
 *  Created on: Dec 14, 2017
 *      Author: Will Son
 *
 * Available GPIO Ports :
 * DAC #1, DAC #6, GPIO #8, GPIO #11,
 * SPI #8, SPI #9, CAN #1
 *
 *******/

#include <stdarg.h>
#include <stdbool.h>

#include "dht11.h"

#ifdef _USE_HW_DHT11
#include "hw.h"



//-- Internal Variables
//
#ifdef _USE_HW_CMDIF_DHT11
void dht11CmdifInit(void);
int  dht11Cmdif(int argc, char **argv);
dht11_t p_dht11;
#endif

typedef struct
{
    uint8_t gpioCh;
    uint8_t extiCh;

} dht11_ch_t;

static dht11_ch_t dht11_ch_tbl[DHT11_CH_MAX] = {
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

bool dht11Init(void)
{
#ifdef _USE_HW_CMDIF_DHT11
  dht11CmdifInit();
#endif
  return true;
}

void dht11ExtiCallback(void *arg)
{
  p_dht11.pastEdgeTimeUs = p_dht11.currentEdgeTimeUs;
  p_dht11.currentEdgeTimeUs = micros();
  if((p_dht11.currentEdgeTimeUs - p_dht11.pastEdgeTimeUs) > RES_THRESHOLD_TIME)
  {
    if(p_dht11.edgecount < 34)
    {
      if(p_dht11.edgecount > 1)    //The first value indicates the beginning of incoming data
      {
        p_dht11.data |= (1 << (33 - p_dht11.edgecount));  //Save 1 by shifting bits(incoming signal)
      }
    }
    else
    {
      p_dht11.checksum |= (1 << (41 - p_dht11.edgecount));
    }
  }
  else
  {
    if(p_dht11.edgecount < 34)
    {
      if(p_dht11.edgecount > 1)
      {
        p_dht11.data |= (0 << (33 - p_dht11.edgecount));  //Save 0 by shifting bits(incoming signal)
      }
    }
    else
    {
      p_dht11.checksum |= (0 << (41 - p_dht11.edgecount));
    }
  }
  p_dht11.edgecount++;
}

void dht11ReadTempHumid(dht11_t *p_dht11)
{
  bool setInt = false;
  uint32_t timeout = 0;
  timeout = millis();

  //if new request is at least 1Hz
  if(timeout - p_dht11->prevRequestTimems > TIMEOUT)
  {
    gpioPinMode(dht11_ch_tbl[p_dht11->ch].gpioCh, _DEF_OUTPUT);       //Set GPIO pin as Output
    gpioPinWrite(dht11_ch_tbl[p_dht11->ch].gpioCh, GPIO_PIN_RESET);   //Transmit Request Signal
    delayMillis(REQ_PD_TIME);                         //Low for 18ms
    gpioPinWrite(dht11_ch_tbl[p_dht11->ch].gpioCh, GPIO_PIN_SET);     //High for 20us

    setInt = extiAttachInterrupt(dht11_ch_tbl[p_dht11->ch].extiCh, _DEF_EXTI_FALLING, dht11ExtiCallback, NULL);   //Attach GPIO Interrupt for Falling edge detection

    while((millis() < (timeout + TIMEOUT)) && (p_dht11->edgecount < 42))
    {
      //Loop while until timeout or receive data (40 bit)
    }

    if(setInt == true)
    {
      extiDetachInterrupt(dht11_ch_tbl[p_dht11->ch].extiCh);  //NVIC must be disabled before resetting SYSCFG->EXTICR
      setInt = false;
    }

    p_dht11->pastEdgeTimeUs = 0;
    p_dht11->currentEdgeTimeUs = 0;
  }
  else
  {

  }
  p_dht11->prevRequestTimems = timeout;
}

#ifdef _USE_HW_CMDIF_DHT11
void dht11CmdifInit(void)
{
  cmdifAdd("dht11", dht11Cmdif);
}

int dht11Cmdif(int argc, char **argv)
{
  int ret = true;

  if(argc == 2)
  {
    p_dht11.ch = (uint8_t) strtoul((const char * ) argv[1], (char **)NULL, (int) 0);

    if ((p_dht11.ch < DHT11_CH_MAX) && (p_dht11.ch >= 0))
    {
      dht11ReadTempHumid(&p_dht11);
    }
    else
    {
      cmdifPrintf("channel range error \n");
      return -1;
    }

    //verify checksum
    if(p_dht11.checksum == (uint8_t)(p_dht11.data + (p_dht11.data >> 8) + (p_dht11.data >> 16) + (p_dht11.data >> 24)))
    {
      if(p_dht11.edgecount != 0)
      {
        cmdifPrintf("Temperature:%2d.%d, Humidity:%2d.%d\n", (uint8_t)(p_dht11.data >> 8), (uint8_t)(p_dht11.data), (uint8_t)(p_dht11.data >> 24), (uint8_t)(p_dht11.data >> 16));
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
    delay(TIMEOUT);  //Maximum operation frequency for DHT11 is 1Hz.
    p_dht11.edgecount = 0;
    p_dht11.data = 0;
    p_dht11.checksum = 0;
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf("dht11 channel[0~6]");
  }

  return ret;
}
#endif

#endif
