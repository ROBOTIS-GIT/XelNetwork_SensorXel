/*
 * mlx90614.h
 *
 *  Created on: 2017. 4. 5.
 *      Author: KEI
 */

#ifndef MLX90614_H_
#define MLX90614_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "hw_def.h"

#ifdef _USE_HW_MLX90614

#define MLX90614_I2CADDR 0x5A //Default

/* RAM */
#define MLX90614_RAWIR1  0x04
#define MLX90614_RAWIR2  0x05
#define MLX90614_TA 	   0x06
#define MLX90614_TOBJ1 	 0x07
#define MLX90614_TOBJ2 	 0x08

/* EEPROM */
#define MLX90614_TOMAX   0x20
#define MLX90614_TOMIN   0x21
#define MLX90614_PWMCTRL 0x22
#define MLX90614_TARANGE 0x23
#define MLX90614_EMISS   0x24
#define MLX90614_CONFIG  0x25
#define MLX90614_ADDR    0x0E
#define MLX90614_ID1     0x3C
#define MLX90614_ID2     0x3D
#define MLX90614_ID3     0x3E
#define MLX90614_ID4     0x3F


typedef struct
{
  uint8_t ch;

  float   object_temp_c;
  float   object_temp_f;
  float   ambient_temp_c;
  float   ambient_temp_f;

} mlx90614_t;


bool mlx90614Init(void);
bool mlx90614Begin(mlx90614_t *p_mlx90614, uint8_t i2c_ch);
void mlx90614ReadObjectTempF(mlx90614_t *p_mlx90614);
void mlx90614ReadAmbientTempF(mlx90614_t *p_mlx90614);
void mlx90614ReadObjectTempC(mlx90614_t *p_mlx90614);
void mlx90614ReadAmbientTempC(mlx90614_t *p_mlx90614);


#endif

#endif /* MLX90614_H_ */
