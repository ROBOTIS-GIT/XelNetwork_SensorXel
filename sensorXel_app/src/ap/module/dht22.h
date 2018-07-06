/*
 * dht22.h
 *
 *  Created on: Jan 18, 2018
 *      Author: Will Son
 */

#ifndef DHT22_H_
#define DHT22_H_

#ifdef __cplusplus
  extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_DHT22
#define _USE_HW_GPIO

#include "bsp.h"
#include "exti.h"

#define REQ_PD_TIME_DHT22         1  //minimum 1ms
#define REQ_PU_TIME_DHT22         20  //minimum 40us
#define RES_THRESHOLD_TIME_DHT22  100 //0 : 76~78us, 1 : 120us
#define TIMEOUT_DHT22             2000  //0.5Hz
#define DHT22_CH_MAX        7   //J0, J1, H3, G11, H13, H14, H15

typedef struct
{
  uint8_t  ch;
  uint32_t data;
  uint8_t  checksum;
  uint32_t currentEdgeTimeUs;
  uint32_t pastEdgeTimeUs;
  uint32_t prevRequestTimems;
  uint8_t  edgecount;

} dht22_t;

bool dht22Init(void);
void dht22ReadTempHumid(dht22_t *p_dht22);
void dht22ExtiCallback(void *arg);
float dht22Temp(void);
float dht22Humid(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* DHT22_H_ */
