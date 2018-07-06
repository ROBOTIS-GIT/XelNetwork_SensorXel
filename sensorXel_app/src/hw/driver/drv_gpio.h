/*
 * drv_gpio.h
 *
 *  Created on: 2017. 10. 27.
 *      Author: opus
 */

#ifndef DRV_GPIO_H_
#define DRV_GPIO_H_



#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

void drvGpioInit(void);
void drvGpioPinMode(uint8_t channel, uint8_t mode);
void drvGpioPinWrite(uint8_t channel, uint8_t bit);
int drvGpioPinRead(uint8_t channel);


#ifdef __cplusplus
 }
#endif

#endif /* DRV_GPIO_H_ */
