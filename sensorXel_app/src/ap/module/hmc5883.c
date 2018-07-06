/*
 * hmc5883l.c
 *
 *  Created on: 2017. 11. 3.
 *      Author: opus
 */

#include "hmc5883.h"

#ifdef _USE_HW_HMC5883

#include "hw.h"
#include "math.h"

#ifndef _USE_HW_I2C
#error "You must use I2C for this function"
#endif

#ifdef _USE_HW_CMDIF_MLX90614
void hmc5883CmdifInit(void);
int hmc5883Cmdif(int argc, char **argv);
#endif

bool hmc5883CheckDeviceID(hmc5883_t *p_hmc5883);

static void hmc5883Write8(hmc5883_t *p_hmc5883, uint8_t reg, uint8_t data);
static uint8_t hmc5883Read8(hmc5883_t *p_hmc5883, uint8_t reg);
static int16_t hmc5883Read16(hmc5883_t *p_hmc5883, uint8_t reg);
static int32_t hmc5883MapValue(int32_t x, int32_t in_min, int32_t in_max,
    int32_t out_min, int32_t out_max);

void hmc5883ReadNormalize(hmc5883_t *p_hmc5883);

void hmc5883SetOffset(hmc5883_t *p_hmc5883, int32_t x_off, int32_t y_off);
void hmc5883SetRange(hmc5883_t *p_hmc5883, hmc5883_range_t range);
hmc5883_range_t hmc5883GetRange(hmc5883_t *p_hmc5883);
void hmc5883SetMeasurementMode(hmc5883_t *p_hmc5883, hmc5883_mode_t user_mode);
hmc5883_mode_t hmc5883GetMeasurementMode(hmc5883_t *p_hmc5883);
void hmc5883SetDataRate(hmc5883_t *p_hmc5883, hmc5883_dataRate_t user_data_rate);
hmc5883_dataRate_t hmc5883GetDataRate(hmc5883_t *p_hmc5883);
void hmc5883SetSamples(hmc5883_t *p_hmc5883, hmc5883_samples_t user_samples);
hmc5883_samples_t hmc5883GetSamples(hmc5883_t *p_hmc5883);

bool hmc5883Init(void)
{

#ifdef _USE_HW_CMDIF_HMC5883
  hmc5883CmdifInit();
#endif

  return true;
}

bool hmc5883Begin(hmc5883_t *p_hmc5883, uint8_t i2c_ch)
{
  p_hmc5883->ch = i2c_ch;

  if (!hmc5883CheckDeviceID(p_hmc5883))
  {
    return false;
  }

  hmc5883SetRange(p_hmc5883, HMC5883_RANGE_1_3GA);
  hmc5883SetMeasurementMode(p_hmc5883, HMC5883_CONTINOUS);
  hmc5883SetDataRate(p_hmc5883, HMC5883_DATARATE_30HZ);
  hmc5883SetSamples(p_hmc5883, HMC5883_SAMPLES_8);

  hmc5883SetOffset(p_hmc5883, 0, 0);

  return true;
}

bool hmc5883CheckDeviceID(hmc5883_t *p_hmc5883)
{
  if ((hmc5883Read8(p_hmc5883, HMC5883_REG_IDENT_A) != 0x48)
      || (hmc5883Read8(p_hmc5883, HMC5883_REG_IDENT_B) != 0x34)
      || (hmc5883Read8(p_hmc5883, HMC5883_REG_IDENT_C) != 0x33))
  {
    return false;
  }

  return true;
}

void hmc5883Calibrate(hmc5883_t *p_hmc5883)
{
  hmc5883_t *p = p_hmc5883;

  hmc5883ReadNormalize(p);

  // Calculate heading
  float heading = atan2(p->y, p->x);

  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Seoul / Seoul declination angle is -8'20W (negative)
  // Formula: (deg + (min / 60.0)) / (180 / M_PI);
  // Template : float declination_angle = (4.0 + (26.0 / 60.0)) / (180 / M_PI);
  float declination_angle = (-8.0 + (20.0 / 60.0)) / (180 / PI); //for Seoul
  heading += declination_angle;

  // Correct for heading < 0deg and heading > 360deg
  if (heading < 0)
  {
    heading += 2 * PI;
  }

  if (heading > 2 * PI)
  {
    heading -= 2 * PI;
  }

  // Convert to degrees
  p->heading_deg = heading * 180 / M_PI;

  if (p->heading_deg >= 1 && p->heading_deg < 240)
  {
    p->fixed_heading_deg = hmc5883MapValue(p->heading_deg, 0, 239, 0, 179);
  }
  else if (p->heading_deg >= 240)
  {
    p->fixed_heading_deg = hmc5883MapValue(p->heading_deg, 240, 360, 180, 360);
  }

  // Smooth angles rotation for +/- 3deg
  p->smooth_heading_deg = round(p->fixed_heading_deg);

  if (p->smooth_heading_deg < (p->previous_deg + 3)
      && p->smooth_heading_deg > (p->previous_deg - 3))
  {
    p->smooth_heading_deg = p->previous_deg;
  }

  p->previous_deg = p->smooth_heading_deg;

  delay(30);
}

void hmc5883ReadNormalize(hmc5883_t *p_hmc5883)
{
  hmc5883_t * p = p_hmc5883;

  p->x = ((float) hmc5883Read16(p, HMC5883_REG_OUT_X_M) - p->x_offset)
      * p->mg_per_digit;
  delay(1);
  p->y = ((float) hmc5883Read16(p, HMC5883_REG_OUT_Y_M) - p->y_offset)
      * p->mg_per_digit;
  delay(1);
  p->z = (float) hmc5883Read16(p, HMC5883_REG_OUT_Z_M) * p->mg_per_digit;
}

void hmc5883SetOffset(hmc5883_t *p_hmc5883, int32_t x_off, int32_t y_off)
{
  p_hmc5883->x_offset = x_off;
  p_hmc5883->y_offset = y_off;
}

void hmc5883SetRange(hmc5883_t *p_hmc5883, hmc5883_range_t range)
{
  switch (range)
  {
    case HMC5883_RANGE_0_88GA:
      p_hmc5883->mg_per_digit = 0.073f;
      break;

    case HMC5883_RANGE_1_3GA:
      p_hmc5883->mg_per_digit = 0.92f;
      break;

    case HMC5883_RANGE_1_9GA:
      p_hmc5883->mg_per_digit = 1.22f;
      break;

    case HMC5883_RANGE_2_5GA:
      p_hmc5883->mg_per_digit = 1.52f;
      break;

    case HMC5883_RANGE_4GA:
      p_hmc5883->mg_per_digit = 2.27f;
      break;

    case HMC5883_RANGE_4_7GA:
      p_hmc5883->mg_per_digit = 2.56f;
      break;

    case HMC5883_RANGE_5_6GA:
      p_hmc5883->mg_per_digit = 3.03f;
      break;

    case HMC5883_RANGE_8_1GA:
      p_hmc5883->mg_per_digit = 4.35f;
      break;

    default:
      break;
  }

  hmc5883Write8(p_hmc5883, HMC5883_REG_CONFIG_B, range << 5);
}

hmc5883_range_t hmc5883GetRange(hmc5883_t *p_hmc5883)
{
  return (hmc5883_range_t) (hmc5883Read8(p_hmc5883, HMC5883_REG_CONFIG_B) >> 5);
}

void hmc5883SetMeasurementMode(hmc5883_t *p_hmc5883, hmc5883_mode_t user_mode)
{
  uint8_t mode;

  mode = hmc5883Read8(p_hmc5883, HMC5883_REG_MODE);
  mode &= 0b11111100;
  mode |= user_mode;

  hmc5883Write8(p_hmc5883, HMC5883_REG_MODE, mode);
}

hmc5883_mode_t hmc5883GetMeasurementMode(hmc5883_t *p_hmc5883)
{
  uint8_t ret_mode;

  ret_mode = hmc5883Read8(p_hmc5883, HMC5883_REG_MODE);
  ret_mode &= 0b00000011;

  return (hmc5883_mode_t) ret_mode;
}

void hmc5883SetDataRate(hmc5883_t *p_hmc5883, hmc5883_dataRate_t user_data_rate)
{
  uint8_t data_rate;

  data_rate = hmc5883Read8(p_hmc5883, HMC5883_REG_CONFIG_A);
  data_rate &= 0b11100011;
  data_rate |= (user_data_rate << 2);

  hmc5883Write8(p_hmc5883, HMC5883_REG_CONFIG_A, data_rate);
}

hmc5883_dataRate_t hmc5883GetDataRate(hmc5883_t *p_hmc5883)
{
  uint8_t data_rate;

  data_rate = hmc5883Read8(p_hmc5883, HMC5883_REG_CONFIG_A);
  data_rate &= 0b00011100;
  data_rate >>= 2;

  return (hmc5883_dataRate_t) data_rate;
}

void hmc5883SetSamples(hmc5883_t *p_hmc5883, hmc5883_samples_t user_samples)
{
  uint8_t samples;

  samples = hmc5883Read8(p_hmc5883, HMC5883_REG_CONFIG_A);
  samples &= 0b10011111;
  samples |= (user_samples << 5);

  hmc5883Write8(p_hmc5883, HMC5883_REG_CONFIG_A, samples);
}

hmc5883_samples_t hmc5883GetSamples(hmc5883_t *p_hmc5883)
{
  uint8_t data_rate;

  data_rate = hmc5883Read8(p_hmc5883, HMC5883_REG_CONFIG_A);
  data_rate &= 0b01100000;
  data_rate >>= 5;

  return (hmc5883_samples_t) data_rate;
}

static uint8_t hmc5883Read8(hmc5883_t *p_hmc5883, uint8_t reg)
{
  uint8_t data;

  i2cReadByte(p_hmc5883->ch, HMC5883_I2CADDRESS, reg, &data);

  return data;
}

static int16_t hmc5883Read16(hmc5883_t *p_hmc5883, uint8_t reg)
{
  int16_t ret = 0;
  uint8_t data[2] = { 0, };

  i2cReadBytes(p_hmc5883->ch, HMC5883_I2CADDRESS, reg, data, 2);

  ret = (data[0] << 8) | data[1];

  return ret;
}

static void hmc5883Write8(hmc5883_t *p_hmc5883, uint8_t reg, uint8_t data)
{
  i2cWriteBytes(p_hmc5883->ch, HMC5883_I2CADDRESS, reg, &data, 1);
}

static int32_t hmc5883MapValue(int32_t x, int32_t in_min, int32_t in_max,
    int32_t out_min, int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#ifdef _USE_HW_CMDIF_HMC5883

void hmc5883CmdifInit(void)
{
  cmdifAdd("hmc5883", hmc5883Cmdif);
}

int hmc5883Cmdif(int argc, char **argv)
{
  bool ret = true;
  hmc5883_t hmc5883;

  if (argc == 3 && strcmp("read", argv[2]) == 0)
  {
    uint8_t ch = (uint8_t) strtoul((const char *) argv[1], (char **) NULL,
        (int) 0) - 1;

    if (ch < I2C_MAX_CH)
    {
      ret = hmc5883Begin(&hmc5883, ch);
      if (ret == false)
        cmdifPrintf("hmc5883 start fail\n");
    }
    else
    {
      cmdifPrintf("channel range over \n");
      return -1;
    }
    while (cmdifRxAvailable() == 0)
    {
      hmc5883Calibrate(&hmc5883);

      cmdifPrintf("Compass - x : %3.2f, y : %3.2f, z : %3.2f\n", hmc5883.x,
          hmc5883.y, hmc5883.z);
      cmdifPrintf("Compass - heading : %3.2f, fixed : %3.2f, smooth : %d\n\n",
          hmc5883.heading_deg, hmc5883.fixed_heading_deg,
          hmc5883.smooth_heading_deg);
      delay(500);
    }
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf("hmc5883 channel[1~2] read\n");
  }

  return 0;
}

#endif

#endif
