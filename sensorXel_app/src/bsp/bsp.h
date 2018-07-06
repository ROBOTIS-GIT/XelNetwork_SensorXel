/*
 * bsp.h
 *
 *  Created on: Feb 10, 2017
 *      Author: baram
 */

#ifndef BSP_H_
#define BSP_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>

#include "stm32l1xx.h"
#include "stm32l1xx_hal.h"

#include "hw_def.h"

#define _BSP_DEF_BOARD_NAME "SensorXel revA"

void bspInit();


#ifdef __cplusplus
 }
#endif
#endif /* BSP_H_ */
