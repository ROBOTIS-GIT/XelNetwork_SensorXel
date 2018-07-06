/*
 * drv_i2c.c
 *
 *  Created on: 2017. 4. 10.
 *      Author: D.ggavy
 */

#ifndef DRV_I2C_C_
#define DRV_I2C_C_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"


bool drvI2CInit(void);
bool drvI2CBegin(uint32_t freq);
err_code_t drvI2CWrites(uint8_t addr, uint8_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
err_code_t drvI2CReads(uint8_t addr, uint8_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);


#ifdef __cplusplus
 }
#endif

#endif /* DRV_I2C_C_ */
