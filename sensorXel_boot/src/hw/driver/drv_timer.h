/*
 * drv_timer.h
 *
 *  Created on: 2017. 4. 7.
 *      Author: baram
 */

#ifndef DRV_TIMER_H_
#define DRV_TIMER_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"


bool drvTimerInit(void);

void drvTimerStop(uint8_t channel);
void drvTimerSetPeriod(uint8_t channel, uint32_t period_data);
void drvTimerAttachInterrupt(uint8_t channel, voidFuncPtr handler);
void drvTimerDetachInterrupt(uint8_t channel);
void drvTimerStart(uint8_t channel);


#ifdef __cplusplus
}
#endif

#endif /* DRV_TIMER_H_ */
