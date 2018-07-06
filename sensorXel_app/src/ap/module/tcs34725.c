/*
 * tcs34725.c
 *
 *  Created on: 2017. 10. 25.
 *      Author: baram
 */

#include <stdarg.h>
#include <stdbool.h>

#include "tcs34725.h"

#ifdef _USE_HW_TCS34725

#include "hw.h"
#include "i2c.h"
#include "math.h"






//-- Internal Variables
//



//-- External Variables
//


//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_TCS34725
void tcs34725CmdifInit(void);
int  tcs34725Cmdif(int argc, char **argv);
#endif

static void tcs34725SetIntegrationTime(tcs34725_t *p_tcs, tcs34725IntegrationTime_t it);
static void tcs34725SetGain(tcs34725_t *p_tcs, tcs34725Gain_t gain);

static void     writeReg(tcs34725_t *p_tcs, uint8_t reg, uint32_t value);
static uint8_t  readReg(tcs34725_t *p_tcs, uint8_t reg);
static uint16_t readRegWord(tcs34725_t *p_tcs, uint8_t reg);
float powf(const float x, const float y);


//-- External Functions
//






bool tcs34725Init(void)
{
#ifdef _USE_HW_CMDIF_TCS34725
  tcs34725CmdifInit();
#endif
  return true;
}

bool tcs34725Begin(tcs34725_t *p_tcs, uint8_t i2c_ch, tcs34725IntegrationTime_t it, tcs34725Gain_t gain)
{
  p_tcs->ch = i2c_ch;
  p_tcs->_tcs34725IntegrationTime = it;
  p_tcs->_tcs34725Gain = gain;


  /* Make sure we're actually connected */
  uint8_t x = readReg(p_tcs, TCS34725_ID);
  if ((x != 0x44) && (x != 0x10))
  {
    return false;
  }
  p_tcs->init = true;

  /* Set default integration time and gain */
  tcs34725SetIntegrationTime(p_tcs, p_tcs->_tcs34725IntegrationTime);
  tcs34725SetGain(p_tcs, p_tcs->_tcs34725Gain);

  /* Note: by default, the device is in power down mode on bootup */
  tcs34725Enable(p_tcs);


  return true;
}

void tcs34725GetRGBC(tcs34725_t *p_tcs)
{
  p_tcs->c_value = readRegWord(p_tcs, TCS34725_CDATAL);
  p_tcs->r_value = readRegWord(p_tcs, TCS34725_RDATAL);
  p_tcs->g_value = readRegWord(p_tcs, TCS34725_GDATAL);
  p_tcs->b_value = readRegWord(p_tcs, TCS34725_BDATAL);

  /* Set a delay for the integration time */
  switch (p_tcs->_tcs34725IntegrationTime)
  {
    case TCS34725_INTEGRATIONTIME_2_4MS:
      delay(3);
      break;
    case TCS34725_INTEGRATIONTIME_24MS:
      delay(24);
      break;
    case TCS34725_INTEGRATIONTIME_50MS:
      delay(50);
      break;
    case TCS34725_INTEGRATIONTIME_101MS:
      delay(101);
      break;
    case TCS34725_INTEGRATIONTIME_154MS:
      delay(154);
      break;
    case TCS34725_INTEGRATIONTIME_700MS:
      delay(700);
      break;
  }
}

uint16_t tcs34725CalculateColorTemperature(tcs34725_t *p_tcs)
{
  float X, Y, Z;      /* RGB to XYZ correlation      */
  float xc, yc;       /* Chromaticity co-ordinates   */
  float n;            /* McCamy's formula            */
  float cct;

  /* 1. Map RGB values to their XYZ counterparts.    */
  /* Based on 6500K fluorescent, 3000K fluorescent   */
  /* and 60W incandescent values for a wide range.   */
  /* Note: Y = Illuminance or lux                    */
  X = (-0.14282F * p_tcs->r_value) + (1.54924F * p_tcs->g_value) + (-0.95641F * p_tcs->b_value);
  Y = (-0.32466F * p_tcs->r_value) + (1.57837F * p_tcs->g_value) + (-0.73191F * p_tcs->b_value);
  Z = (-0.68202F * p_tcs->r_value) + (0.77073F * p_tcs->g_value) + ( 0.56332F * p_tcs->b_value);

  /* 2. Calculate the chromaticity co-ordinates      */
  xc = (X) / (X + Y + Z);
  yc = (Y) / (X + Y + Z);

  /* 3. Use McCamy's formula to determine the CCT    */
  n = (xc - 0.3320F) / (0.1858F - yc);

  /* Calculate the final CCT */
  cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;

  p_tcs->color_temp = (uint16_t)cct;

  /* Return the results in degrees Kelvin */
  return (uint16_t)cct;
}

uint16_t tcs34725CalculateLux(tcs34725_t *p_tcs)
{
  float illuminance;

  /* This only uses RGB ... how can we integrate clear or calculate lux */
  /* based exclusively on clear since this might be more reliable?      */
  illuminance = (-0.32466F * p_tcs->r_value) + (1.57837F * p_tcs->g_value) + (-0.73191F * p_tcs->b_value);

  p_tcs->lux = (uint16_t)illuminance;

  return (uint16_t)illuminance;
}

void tcs34725SetIntegrationTime(tcs34725_t *p_tcs, tcs34725IntegrationTime_t it)
{
  /* Update the timing register */
  writeReg(p_tcs, TCS34725_ATIME, it);

  /* Update value placeholders */
  p_tcs->_tcs34725IntegrationTime = it;
}

void tcs34725SetGain(tcs34725_t *p_tcs, tcs34725Gain_t gain)
{
  /* Update the timing register */
  writeReg(p_tcs, TCS34725_CONTROL, gain);

  /* Update value placeholders */
  p_tcs->_tcs34725Gain = gain;
}


void tcs34725Enable(tcs34725_t *p_tcs)
{
  writeReg(p_tcs, TCS34725_ENABLE, TCS34725_ENABLE_PON);
  delay(3);
  writeReg(p_tcs, TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
}

void tcs34725Disable(tcs34725_t *p_tcs)
{
  uint8_t reg = 0;
  reg = readReg(p_tcs, TCS34725_ENABLE);
  writeReg(p_tcs, TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

void tcs34725Lock(tcs34725_t *p_tcs)
{
  uint8_t r = readReg(p_tcs, TCS34725_ENABLE);
  r |= TCS34725_ENABLE_AIEN;
  writeReg(p_tcs, TCS34725_ENABLE, r);
}

void tcs34725Unlock(tcs34725_t *p_tcs)
{
  uint8_t r = readReg(p_tcs, TCS34725_ENABLE);
  r &= ~TCS34725_ENABLE_AIEN;
  writeReg(p_tcs, TCS34725_ENABLE, r);
}


float powf(const float x, const float y)
{
  return (float)(pow((double)x, (double)y));
}


void writeReg(tcs34725_t *p_tcs, uint8_t reg, uint32_t value)
{
  i2cWriteBytes(p_tcs->ch, TCS34725_ADDRESS, TCS34725_COMMAND_BIT | reg, (uint8_t *)&value, 1);
}


uint8_t readReg(tcs34725_t *p_tcs, uint8_t reg)
{
  uint8_t ret = 0;

  i2cReadBytes(p_tcs->ch, TCS34725_ADDRESS, TCS34725_COMMAND_BIT | reg, &ret, 1);

  return ret;
}


uint16_t readRegWord(tcs34725_t *p_tcs, uint8_t reg)
{
  uint16_t ret = 0;
  uint8_t data[2];

  i2cReadBytes(p_tcs->ch, TCS34725_ADDRESS, TCS34725_COMMAND_BIT | reg, data, 2);

  ret = data[1]<<8 | data[0]<<0;

  return ret;
}



#ifdef _USE_HW_CMDIF_TCS34725
void tcs34725CmdifInit(void)
{
  cmdifAdd("tcs34725", tcs34725Cmdif);
}

int tcs34725Cmdif(int argc, char **argv)
{
  bool ret = true;
  tcs34725_t tcs34725;


  if(argc == 3 && strcmp("read", argv[2]) == 0)
  {
    uint8_t ch = (uint8_t) strtoul((const char * ) argv[1], (char **)NULL, (int) 0) - 1;

    if ((ch) < I2C_MAX_CH)
    {
      ret = tcs34725Begin(&tcs34725, ch, TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
      if (ret == false)
      {
        cmdifPrintf("fail : tcs34725Begin\n");
        return 0;
      }
    }
    else
    {
      cmdifPrintf("channel range over \n");
      return -1;
    }
    while(cmdifRxAvailable() == 0)
    {
      tcs34725GetRGBC(&tcs34725);

      cmdifPrintf("R:%d\t G:%d\t B:%d\r", tcs34725.r_value, tcs34725.g_value, tcs34725.b_value);
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
