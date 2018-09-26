/*
 *  hw_def.h
 *
 *  Created on: 2016. 5. 14.
 *      Author: Baram
 */





#ifndef HW_DEF_H
#define HW_DEF_H

#include <stdint.h>
#include <stdarg.h>
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


//#define _USE_HW_MLX90614
#define _USE_HW_IMU
#define _USE_HW_ULTRA_SONIC


#define _USE_HW_LED
#define _USE_HW_BUTTON
#define _USE_HW_GPIO
#define _USE_HW_ADC
#define _USE_HW_TIMER
#define _USE_HW_UART
#define _USE_HW_I2C
#define _USE_HW_SPI
#define _USE_HW_RESET
#define _USE_HW_TIMER
#define _USE_HW_SW_TIMER
#define _USE_HW_EEPROM
#define _USE_HW_FLASH
#define _USE_HW_PWM
#define _USE_HW_CTABLE

#define _USE_HW_DXLPORT
#define _USE_HW_DXLCMD
#define _USE_HW_DXL
#define _USE_HW_MUX

//#define _USE_HW_CTABLE

//#define _USE_HW_CMDIF_GPIO
//#define _USE_HW_CMDIF_BUTTON
//#define _USE_HW_CMDIF_LED
//#define _USE_HW_CMDIF_ADC
//#define _USE_HW_CMDIF_PWM
//#define _USE_HW_CMDIF_I2C
//#define _USE_HW_CMDIF_DXLCMD


#define _HW_DEF_CMDIF_LIST_MAX                32


#define _HW_DEF_DXLCMD_MAX_NODE               32
#define _HW_DEF_DXLCMD_MAX_NODE_BUFFER_SIZE   32
#define _HW_DEF_DXLCMD_BUFFER_MAX             1024
#define _HW_DEF_DXL_MAX_BUFFER                (1024+10)


#define _HW_DEF_LED_CH_MAX                    1
#define _HW_DEF_BUTTON_CH_MAX                 1
#define _HW_DEF_ADC_CH_MAX                    4
#define _HW_DEF_GPIO_CH_MAX                   4
#define _HW_DEF_UART_CH_MAX                   2
#define _HW_DEF_I2C_CH_MAX                    1
#define _HW_DEF_SPI_CH_MAX                    1
#define _HW_DEF_DXLPORT_CH_MAX                1
#define _HW_DEF_TIMER_CH_MAX                  1
#define _HW_DEF_PWM_CH_MAX                    4
#define _HW_DEF_MUX_CH_MAX                    3


typedef enum {
  MUX_ID_0 = 0,
  MUX_ID_1 = 1,
  MUX_ID_2 = 2,
} mux_id_t;

typedef enum {
  MUX_UART_RX  = 0,
  MUX_UART_TX  = 1,
  MUX_I2C_SCL  = 2,
  MUX_I2C_SDA  = 3,
  MUX_SPI_MISO = 4,
  MUX_SPI_MOSI = 5,
  MUX_SPI_SCK  = 6,
  MUX_GND      = 7
} mux_channel_t;

typedef enum {
  MUX_UART = 0,
  MUX_I2C  = 1,
  MUX_SPI  = 2,

} mux_default_set_t;


#endif

