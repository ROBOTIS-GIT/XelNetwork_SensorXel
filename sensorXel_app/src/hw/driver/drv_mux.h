/*
 *  drv_mux
 *  Created on: 2018. 2. 27.
 *      Author: D.ggavy
 */

#ifndef SRC_HW_DRIVER_DRV_MUX_C_
#define SRC_HW_DRIVER_DRV_MUX_C_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"
#include "bsp.h"
#define I2C__CH      _HW_DEF_I2C_MAX_CH





bool    drvMuxInit(void);
void    drvMuxEnableId(uint8_t id, bool state);

bool    drvMuxSetChannel(uint8_t id, uint8_t ch);
uint8_t drvMUxGetChannel(uint8_t id);


#ifdef __cplusplus
 }
#endif

#endif
