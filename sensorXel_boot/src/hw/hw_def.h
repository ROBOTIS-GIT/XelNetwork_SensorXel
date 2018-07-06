/*
 *  hw_def.h
 *
 *  Created on: 2016. 5. 14.
 *      Author: Baram
 */





#ifndef HW_DEF_H
#define HW_DEF_H

#include <stdint.h>
#include <stdbool.h>

#include "def.h"
#include "error_code.h"


#ifndef BOOL
#define BOOL uint8_t
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef bool
#define bool uint8_t
#endif

#ifndef true
#define true  1
#endif

#ifndef false
#define false 0
#endif


#define _USE_HW_LED
#define _USE_HW_UART
#define _USE_HW_DXLPORT
#define _USE_HW_RESET
#define _USE_HW_FLASH
#define _USE_HW_MUX




#define _HW_DEF_LED_CH_MAX                    1
#define _HW_DEF_BUTTON_CH_MAX                 1
#define _HW_DEF_UART_CH_MAX                   2
#define _HW_DEF_DXLPORT_CH_MAX                1
#define _HW_DEF_TIMER_CH_MAX                  1
#define _HW_DEF_MUX_CH_MAX					          3



#define _HW_DEF_FLASH_ADDR_APP_LENGTH         (44*1024)
#define _HW_DEF_FLASH_ADDR_APP_START          0x08005000
#define _HW_DEF_FLASH_ADDR_APP_END            (_HW_DEF_FLASH_ADDR_APP_START + _HW_DEF_FLASH_ADDR_APP_LENGTH)

#define _HW_DEF_CMD_MAX_DATA_LENGTH           1024










#endif

