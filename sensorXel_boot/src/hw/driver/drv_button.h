/*
 * drv_button.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef DRV_BUTTON_H_
#define DRV_BUTTON_H_



#ifdef __cplusplus
 extern "C" {
#endif




bool     drvButtonInit(void);
uint8_t  drvButtonGetState(uint8_t ch);


#ifdef __cplusplus
}
#endif



#endif
