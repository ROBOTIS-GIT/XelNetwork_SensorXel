/*
 * drv_pwm.h
 *
 *  Created on: 2017. 11. 10.
 *      Author: opus
 */

#ifndef DRV_PWM_H_
#define DRV_PWM_H_

#include "hw_def.h"


bool drvPwmInit(uint32_t freq);
bool drvPwmSetup(uint8_t channel);

bool drvPwmSetDuty(uint8_t channel, uint8_t duty);
bool drvPwmSetPulse(uint8_t channel, uint32_t res, uint32_t pulse);
uint8_t drvPwmGetDuty(uint8_t channel);
uint32_t drvPwmGetPulse(uint8_t channel);

#endif /* DRV_PWM_H_ */
