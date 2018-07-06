/*
 *  drv_flash.h
 *
 *  Created on: 2016. 5. 14.
 *      Author: Baram
 */

#ifndef DRV_FLASH_H
#define DRV_FLASH_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"


#define DRV_FLASH_PAGE_TOTAL 256 // 256*256 = 64KB


bool drvFlashInit(void);

err_code_t drvFlashWrite(uint32_t addr, uint8_t *p_data, uint32_t length);
err_code_t drvFlashRead(uint32_t addr, uint8_t *p_data, uint32_t length);
err_code_t drvFlashErase(uint32_t addr, uint32_t length);



#ifdef __cplusplus
}
#endif


#endif /* DRV_FLASH_H */
