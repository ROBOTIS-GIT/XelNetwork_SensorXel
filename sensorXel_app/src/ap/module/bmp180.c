/*
 * bmp180.c
 *
 *  Created on: 2017. 10. 24.
 *      Author: baram
 */

#include <stdarg.h>
#include <stdbool.h>

#include "bmp180.h"

#ifdef _USE_HW_BMP180

#ifndef _USE_HW_I2C
#error "You must use I2C for this function"
#endif

#include "hw.h"
#include "i2c.h"
#include "math.h"






//-- Internal Variables
//



//-- External Variables
//


//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_BMP180
void bmp180CmdifInit(void);
int  bmp180Cmdif(int argc, char **argv);
#endif

static uint8_t  bmp180Read8(bmp180_t *p_bmp180, uint8_t addr);
static uint16_t bmp180Read16(bmp180_t *p_bmp180, uint8_t addr);
static void     bmp180Write8(bmp180_t *p_bmp180, uint8_t addr, uint8_t data);


//-- External Functions
//






bool bmp180Init(void)
{
#ifdef _USE_HW_CMDIF_BMP180
  bmp180CmdifInit();
#endif
  return true;
}

bool bmp180Begin(bmp180_t *p_bmp180, uint8_t i2c_ch, uint8_t mode)
{
  p_bmp180->ch = i2c_ch;
  p_bmp180->oversampling = mode;
  p_bmp180->altitude_meters = 0;
  p_bmp180->sealevelPressure = 101325;


  if (bmp180Read8(p_bmp180, 0xD0) != 0x55)
  {
    return false;
  }

  /* read calibration data */
  p_bmp180->ac1 = bmp180Read16(p_bmp180, BMP085_CAL_AC1);
  p_bmp180->ac2 = bmp180Read16(p_bmp180, BMP085_CAL_AC2);
  p_bmp180->ac3 = bmp180Read16(p_bmp180, BMP085_CAL_AC3);
  p_bmp180->ac4 = bmp180Read16(p_bmp180, BMP085_CAL_AC4);
  p_bmp180->ac5 = bmp180Read16(p_bmp180, BMP085_CAL_AC5);
  p_bmp180->ac6 = bmp180Read16(p_bmp180, BMP085_CAL_AC6);

  p_bmp180->b1 = bmp180Read16(p_bmp180, BMP085_CAL_B1);
  p_bmp180->b2 = bmp180Read16(p_bmp180, BMP085_CAL_B2);

  p_bmp180->mb = bmp180Read16(p_bmp180, BMP085_CAL_MB);
  p_bmp180->mc = bmp180Read16(p_bmp180, BMP085_CAL_MC);
  p_bmp180->md = bmp180Read16(p_bmp180, BMP085_CAL_MD);

  return true;
}

int32_t bmp180ComputeB5(bmp180_t *p_bmp180, int32_t UT)
{
  int32_t X1 = (UT - (int32_t)p_bmp180->ac6) * ((int32_t)p_bmp180->ac5) >> 15;
  int32_t X2 = ((int32_t)p_bmp180->mc << 11) / (X1+(int32_t)p_bmp180->md);

  return X1 + X2;
}

uint16_t bmp180ReadRawTemperature(bmp180_t *p_bmp180)
{
  bmp180Write8(p_bmp180, BMP085_CONTROL, BMP085_READTEMPCMD);
  delay(5);

  return bmp180Read16(p_bmp180, BMP085_TEMPDATA);
}

uint32_t bmp180ReadRawPressure(bmp180_t *p_bmp180)
{
  uint32_t raw;

  bmp180Write8(p_bmp180, BMP085_CONTROL, BMP085_READPRESSURECMD + (p_bmp180->oversampling << 6));

  if (p_bmp180->oversampling == BMP085_ULTRALOWPOWER)
    delay(5);
  else if (p_bmp180->oversampling == BMP085_STANDARD)
    delay(8);
  else if (p_bmp180->oversampling == BMP085_HIGHRES)
    delay(14);
  else
    delay(26);

  raw = bmp180Read16(p_bmp180, BMP085_PRESSUREDATA);

  raw <<= 8;
  raw |= bmp180Read8(p_bmp180, BMP085_PRESSUREDATA+2);
  raw >>= (8 - p_bmp180->oversampling);

  return raw;
}

int32_t bmp180ReadPressure(bmp180_t *p_bmp180)
{
  int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
  uint32_t B4, B7;

  UT = bmp180ReadRawTemperature(p_bmp180);
  UP = bmp180ReadRawPressure(p_bmp180);

  B5 = bmp180ComputeB5(p_bmp180, UT);


  // do pressure calcs
  B6 = B5 - 4000;
  X1 = ((int32_t)p_bmp180->b2 * ( (B6 * B6)>>12 )) >> 11;
  X2 = ((int32_t)p_bmp180->ac2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((int32_t)p_bmp180->ac1*4 + X3) << p_bmp180->oversampling) + 2) / 4;


  X1 = ((int32_t)p_bmp180->ac3 * B6) >> 13;
  X2 = ((int32_t)p_bmp180->b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = ((uint32_t)p_bmp180->ac4 * (uint32_t)(X3 + 32768)) >> 15;
  B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL >> p_bmp180->oversampling );


  if (B7 < 0x80000000) {
    p = (B7 * 2) / B4;
  } else {
    p = (B7 / B4) * 2;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;


  p = p + ((X1 + X2 + (int32_t)3791)>>4);

  p_bmp180->pressure = p;

  return p;
}

int32_t bmp180ReadSealevelPressure(bmp180_t *p_bmp180)
{
  float pressure = bmp180ReadPressure(p_bmp180);

  return (int32_t)(pressure / pow(1.0-p_bmp180->altitude_meters/44330, 5.255));
}

float bmp180ReadTemperature(bmp180_t *p_bmp180)
{
  int32_t UT, B5;     // following ds convention
  float temp;

  UT = bmp180ReadRawTemperature(p_bmp180);

  B5 = bmp180ComputeB5(p_bmp180, UT);
  temp = (B5+8) >> 4;
  temp /= 10;

  p_bmp180->temp = temp;
  return temp;
}

float bmp180ReadAltitude(bmp180_t *p_bmp180)
{
  float altitude;

  float pressure = bmp180ReadPressure(p_bmp180);

  altitude = 44330 * (1.0 - pow(pressure /p_bmp180->sealevelPressure,0.1903));

  p_bmp180->altitude = altitude;

  return altitude;
}




uint8_t bmp180Read8(bmp180_t *p_bmp180, uint8_t addr)
{
  uint8_t ret = 0;

  i2cReadBytes(p_bmp180->ch, BMP085_I2CADDR, addr, &ret, 1);

  return ret;
}

uint16_t bmp180Read16(bmp180_t *p_bmp180, uint8_t addr)
{
  uint16_t ret = 0;
  uint8_t data[2];

  i2cReadBytes(p_bmp180->ch, BMP085_I2CADDR, addr, data, 2);

  ret = data[0]<<8 | data[1]<<0;
  return ret;
}

void bmp180Write8(bmp180_t *p_bmp180, uint8_t addr, uint8_t data)
{
  i2cWriteBytes(p_bmp180->ch, BMP085_I2CADDR, addr, &data, 1);
}




#ifdef _USE_HW_CMDIF_BMP180
void bmp180CmdifInit(void)
{
  cmdifAdd("bmp180", bmp180Cmdif);
}

int bmp180Cmdif(int argc, char **argv)
{
  bool ret = true;
  bmp180_t bmp180;


  if(argc == 3 && strcmp("read", argv[2]) == 0)
  {
    uint8_t ch = (uint8_t) strtoul((const char * ) argv[1], (char **)NULL, (int) 0) - 1;

    if ((ch) < I2C_MAX_CH)
    {
      bmp180Begin(&bmp180, ch, BMP085_STANDARD);
    }
    else
    {
      cmdifPrintf("channel range over \n");
      return -1;
    }
    while(cmdifRxAvailable() == 0)
    {
      float   temp;
      int32_t pressure;
      float   altitude;

      temp     = bmp180ReadTemperature(&bmp180);
      pressure = bmp180ReadPressure(&bmp180);
      altitude = bmp180ReadAltitude(&bmp180);

      cmdifPrintf("Temp:%2d.%02d Pressure:%d, Altitude:%2d.%02d\r", (int)temp, abs(((int)(temp*100)) % 100), pressure, (int)altitude, abs(((int)(altitude*100)) % 100));
      delay(100);
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cmdifPrintf( "bmp180 channel[1~2] read\n");
  }

  return 0;
}
#endif

#endif
