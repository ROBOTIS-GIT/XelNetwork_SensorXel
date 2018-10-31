/*
 * hmc5883l.h
 *
 *  Created on: 2017. 11. 3.
 *      Author: opus
 */

#ifndef HMC5883_H_
#define HMC5883_H_

#ifdef __cplusplus
  extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_HMC5883

#define HMC5883_I2CADDRESS           (0x1E)   //Default
#define HMC5883_REG_CONFIG_A         (0x00)   //RW
#define HMC5883_REG_CONFIG_B         (0x01)   //RW
#define HMC5883_REG_MODE             (0x02)   //RW
#define HMC5883_REG_OUT_X_M          (0x03)   //RO
#define HMC5883_REG_OUT_X_L          (0x04)   //RO
#define HMC5883_REG_OUT_Z_M          (0x05)   //RO
#define HMC5883_REG_OUT_Z_L          (0x06)   //RO
#define HMC5883_REG_OUT_Y_M          (0x07)   //RO
#define HMC5883_REG_OUT_Y_L          (0x08)   //RO
#define HMC5883_REG_STATUS           (0x09)   //RO
#define HMC5883_REG_IDENT_A          (0x0A)   //RO
#define HMC5883_REG_IDENT_B          (0x0B)   //RO
#define HMC5883_REG_IDENT_C          (0x0C)   //RO

typedef enum
{
    HMC5883_SAMPLES_8     = 0b11,
    HMC5883_SAMPLES_4     = 0b10,
    HMC5883_SAMPLES_2     = 0b01,
    HMC5883_SAMPLES_1     = 0b00
} hmc5883_samples_t;

typedef enum
{
    HMC5883_DATARATE_75HZ       = 0b110,
    HMC5883_DATARATE_30HZ       = 0b101,
    HMC5883_DATARATE_15HZ       = 0b100,
    HMC5883_DATARATE_7_5HZ      = 0b011,
    HMC5883_DATARATE_3HZ        = 0b010,
    HMC5883_DATARATE_1_5HZ      = 0b001,
    HMC5883_DATARATE_0_75_HZ    = 0b000
} hmc5883_dataRate_t;

typedef enum
{
    HMC5883_RANGE_8_1GA     = 0b111,
    HMC5883_RANGE_5_6GA     = 0b110,
    HMC5883_RANGE_4_7GA     = 0b101,
    HMC5883_RANGE_4GA       = 0b100,
    HMC5883_RANGE_2_5GA     = 0b011,
    HMC5883_RANGE_1_9GA     = 0b010,
    HMC5883_RANGE_1_3GA     = 0b001,
    HMC5883_RANGE_0_88GA    = 0b000
} hmc5883_range_t;

typedef enum
{
    HMC5883_IDLE          = 0b10,
    HMC5883_SINGLE        = 0b01,
    HMC5883_CONTINOUS     = 0b00
} hmc5883_mode_t;



typedef struct
{
  uint8_t ch;

  float mg_per_digit;
  int32_t x_offset, y_offset;

  float x, y, z;

  int32_t previous_deg, smooth_heading_deg;
  float heading_deg, fixed_heading_deg;

} hmc5883_t;


bool hmc5883Init(void);
bool hmc5883Begin(hmc5883_t *p_hmc5883, uint8_t i2c_ch);
void hmc5883Calibrate(hmc5883_t *p_hmc5883);


#endif


#ifdef __cplusplus
}
#endif

#endif /* HMC5883_H_ */
