/*
 * dht11.h
 *
 *  Created on: Dec 14, 2017
 *      Author: Will Son
 */

#ifndef DHT11_H_
#define DHT11_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_DHT11
#define _USE_HW_GPIO

#include "bsp.h"
#include "exti.h"

#define REQ_PD_TIME         18  //minimum 18ms
#define REQ_PU_TIME         20  //minimum 40us
#define RES_PD_TIME         60  //minimum 54us
#define RES_PU_TIME         90  //minimum 80us
#define RES_THRESHOLD_TIME  100 //0 : 76~78us, 1 : 120~124us
#define TIMEOUT             1000  //minimum 5.2ms
#define DHT11_CH_MAX        7   //J0, J1, H3, G11, H13, H14, H15

typedef struct
{
  uint8_t  ch;
  uint32_t data;
  uint8_t  checksum;
  uint32_t currentEdgeTimeUs;
  uint32_t pastEdgeTimeUs;
  uint32_t prevRequestTimems;
  uint8_t  edgecount;

} dht11_t;

bool dht11Init(void);
void dht11ReadTempHumid(dht11_t *p_dht11);
void dht11ExtiCallback(void *arg);

#endif

#ifdef __cplusplus
}
#endif

#endif /* DHT11_H_ */
