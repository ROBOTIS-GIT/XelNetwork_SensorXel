/*
 * mlx90614.c
 *
 *  Created on: 2017. 4. 5.
 *      Author: KEI
 */

#include "mlx90614.h"

#ifdef _USE_HW_MLX90614

#include "hw.h"

#ifndef _USE_HW_I2C
#error "You must use I2C for this function"
#endif

//-- Internal Variables
//

//-- External Variables
//

//-- Internal Functions
//

bool mlx90614CheckDeviceID(mlx90614_t *p_mlx90614);

static uint16_t mlx90614Read(mlx90614_t *p_mlx90614, uint8_t reg);
/*
 static void mlx90614Write8(mlx90614_t *p_mlx90614, uint8_t reg, uint8_t data);
 static void mlx90614Write16(mlx90614_t *p_mlx90614, uint8_t reg, uint16_t data);
 */
static float mlx90614ReadTemp(mlx90614_t *p_mlx90614, uint8_t reg);

#ifdef _USE_HW_CMDIF_MLX90614
void mlx90614CmdifInit(void);
int mlx90614Cmdif(int argc, char **argv);
#endif
//-- External Functions
//

bool mlx90614Init(void)
{

  /* mlx90614 support SMbus & PWM
   * Therefore, you must set i2c speed less than 100Khz.
   */

#ifdef _USE_HW_CMDIF_MLX90614
  mlx90614CmdifInit();
#endif

  return true;
}

bool mlx90614Begin(mlx90614_t *p_mlx90614, uint8_t i2c_ch)
{
  p_mlx90614->ch = i2c_ch;

  if (!mlx90614CheckDeviceID(p_mlx90614))
  {
    return false;
  }

  return true;
}

bool mlx90614CheckDeviceID(mlx90614_t *p_mlx90614)
{
  if ((mlx90614Read(p_mlx90614, MLX90614_ID1) != 0x1404)
      || (mlx90614Read(p_mlx90614, MLX90614_ID2) != 0xEF1D)
      || (mlx90614Read(p_mlx90614, MLX90614_ID3) != 0x1123)
      || (mlx90614Read(p_mlx90614, MLX90614_ID4) != 0xB186))
  {
    return false;
  }

  return true;
}

static uint16_t mlx90614Read(mlx90614_t *p_mlx90614, uint8_t reg)
{
  uint16_t ret = 0;
  uint8_t data[3] = { 0, };

  i2cReadBytes(p_mlx90614->ch, MLX90614_I2CADDR, reg, data, 3);

  ret = (data[1] << 8) | data[0];

  return ret;
}

/*
 static void mlx90614Write8(mlx90614_t *p_mlx90614, uint8_t reg, uint8_t data)
 {
 i2cWriteBytes(p_mlx90614->ch, MLX90614_I2CADDR, reg, &data, 1);
 }

 static void mlx90614Write16(mlx90614_t *p_mlx90614, uint8_t reg, uint16_t data)
 {
 uint8_t temp_data[2];

 temp_data[1] = data|0xFF;
 temp_data[0] = (data >> 8)|0xFF;

 i2cWriteBytes(p_mlx90614->ch, MLX90614_I2CADDR, reg, temp_data, 2);
 }
 */

void mlx90614ReadObjectTempF(mlx90614_t *p_mlx90614)
{
  p_mlx90614->object_temp_f = mlx90614ReadTemp(p_mlx90614, MLX90614_TOBJ1) * 9 / 5 + 32;
}

void mlx90614ReadAmbientTempF(mlx90614_t *p_mlx90614)
{
  p_mlx90614->ambient_temp_f = (mlx90614ReadTemp(p_mlx90614, MLX90614_TA) * 9 / 5) + 32;
}

void mlx90614ReadObjectTempC(mlx90614_t *p_mlx90614)
{
  p_mlx90614->object_temp_c = mlx90614ReadTemp(p_mlx90614, MLX90614_TOBJ1);
}

void mlx90614ReadAmbientTempC(mlx90614_t *p_mlx90614)
{
  p_mlx90614->ambient_temp_c = mlx90614ReadTemp(p_mlx90614, MLX90614_TA);
}

static float mlx90614ReadTemp(mlx90614_t *p_mlx90614, uint8_t reg)
{
  uint16_t recv_data;
  float temp;

  recv_data = mlx90614Read(p_mlx90614, reg);
  temp = ((float) recv_data * 0.02) - 273.15;
  return temp;
}

#ifdef _USE_HW_CMDIF_MLX90614

void mlx90614CmdifInit(void)
{
  cmdifAdd("mlx90614", mlx90614Cmdif);
}

int mlx90614Cmdif(int argc, char **argv)
{
  bool ret = true;
  mlx90614_t mlx90614;

  if (argc == 3 && strcmp("read", argv[2]) == 0)
  {
    uint8_t ch = (uint8_t) strtoul((const char *) argv[1], (char **) NULL,
        (int) 0) - 1;

    if (ch < I2C_MAX_CH)
    {
      mlx90614Begin(&mlx90614, ch);
    }
    else
    {
      cmdifPrintf("channel range over \n");
      return -1;
    }
    while (cmdifRxAvailable() == 0)
    {
      mlx90614ReadObjectTempC(&mlx90614);
      mlx90614ReadAmbientTempC(&mlx90614);

      cmdifPrintf("Object Temp : %3.2f, Ambient Temp : %3.2f\n", mlx90614.object_temp_c,
          mlx90614.ambient_temp_c);
      delay(500);
    }
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf("mlx90614 channel[1~2] read\n");
  }

  return 0;
}

#endif

#endif
