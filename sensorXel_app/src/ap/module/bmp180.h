/*
 * bmp180.h
 *
 *  Created on: 2017. 10. 24.
 *      Author: baram
 */

#ifndef BMP180_H_
#define BMP180_H_



#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_BMP180

#include "bsp.h"



#define BMP085_I2CADDR            0x77

#define BMP085_ULTRALOWPOWER      0
#define BMP085_STANDARD           1
#define BMP085_HIGHRES            2
#define BMP085_ULTRAHIGHRES       3
#define BMP085_CAL_AC1            0xAA  // R   Calibration data (16 bits)
#define BMP085_CAL_AC2            0xAC  // R   Calibration data (16 bits)
#define BMP085_CAL_AC3            0xAE  // R   Calibration data (16 bits)
#define BMP085_CAL_AC4            0xB0  // R   Calibration data (16 bits)
#define BMP085_CAL_AC5            0xB2  // R   Calibration data (16 bits)
#define BMP085_CAL_AC6            0xB4  // R   Calibration data (16 bits)
#define BMP085_CAL_B1             0xB6  // R   Calibration data (16 bits)
#define BMP085_CAL_B2             0xB8  // R   Calibration data (16 bits)
#define BMP085_CAL_MB             0xBA  // R   Calibration data (16 bits)
#define BMP085_CAL_MC             0xBC  // R   Calibration data (16 bits)
#define BMP085_CAL_MD             0xBE  // R   Calibration data (16 bits)

#define BMP085_CONTROL            0xF4
#define BMP085_TEMPDATA           0xF6
#define BMP085_PRESSUREDATA       0xF6
#define BMP085_READTEMPCMD        0x2E
#define BMP085_READPRESSURECMD    0x34



typedef struct
{
  uint8_t ch;

  float altitude_meters;
  float sealevelPressure;

  uint8_t oversampling;


  int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
  uint16_t ac4, ac5, ac6;

  float   temp;
  int32_t pressure;
  float   altitude;
} bmp180_t;




bool bmp180Init(void);
bool bmp180Begin(bmp180_t *p_bmp180, uint8_t i2c_ch, uint8_t mode);

float   bmp180ReadTemperature(bmp180_t *p_bmp180);
int32_t bmp180ReadPressure(bmp180_t *p_bmp180);
int32_t bmp180ReadSealevelPressure(bmp180_t *p_bmp180);
float   bmp180ReadAltitude(bmp180_t *p_bmp180);

uint16_t bmp180ReadRawTemperature(bmp180_t *p_bmp180);
uint32_t bmp180ReadRawPressure(bmp180_t *p_bmp180);

#endif

#ifdef __cplusplus
}
#endif



#endif /* BUTTON_H_ */
