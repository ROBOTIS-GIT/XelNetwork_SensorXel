/*
 * test_vcp.c
 *
 *  Created on: 2018. 2. 13.
 *      Author: kei
 */

#include "ap.h"

#include "test_def.h"




//-- Internal Variables
//


//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//


#if _TEST_ENABLE_SPI
#include "imu/imu.h"
static void testSpiMpu9250(void);
#endif

#if _TEST_ENABLE_I2C
#include "module/mlx90614.h"
static void testI2cMxl90614(void);
#endif


void testMain(void)
{
#if _TEST_ENABLE_SPI
  testSpiMpu9250();
#endif

#if _TEST_ENABLE_I2C
  testI2cMxl90614();
#endif
}


#if _TEST_ENABLE_SPI
static void testSpiMpu9250(void)
{
  uint32_t pre_time;
  imu_node_t imu_node;
  int roll, pitch, yaw;
  bool ret;

  /* Set MUX */
  muxSetChannel(MUX_ID_1, MUX_SPI_SCK);
  muxSetChannel(MUX_ID_2, MUX_SPI_MOSI);
  muxSetChannel(MUX_ID_3, MUX_SPI_MISO);

  imuInit();

  dxlportOpen(_DEF_DXL1, 115200);

  ret = imuBegin();
  pre_time = millis();

  while (1)
  {
    imuUpdate(&imu_node);

    if(millis() - pre_time >= 500)
    {
      pre_time = millis();

      ledToggle(0);

      roll  = (int)imu_node.roll;
      pitch = (int)imu_node.pitch;
      yaw   = (int)imu_node.yaw;

      if(ret == false)
      {
        dxlportPrintf(_DEF_DXL1, "IMU communication Fail...\r\n");

        ret = imuBegin();
      }
      else
      {
        dxlportPrintf(_DEF_DXL1, "IMU(R/P/Y) : %d\t %d\t %d\r\n", roll, pitch, yaw);
      }
    }
  }
}
#endif


#if _TEST_ENABLE_I2C
static void testI2cMxl90614(void)
{
  mlx90614_t mlx90614;

  /* Set MUX */
  muxSetChannel(MUX_ID_1, MUX_I2C_SCL);
  muxSetChannel(MUX_ID_2, MUX_I2C_SDA);

  mlx90614Init();

  mlx90614Begin(&mlx90614, _DEF_I2C1);

  //dxlportOpen(_DEF_DXL1, 115200);

  while (1)
  {
    mlx90614ReadObjectTempC(&mlx90614);
    mlx90614ReadAmbientTempC(&mlx90614);

    ledToggle(0);

    delay(500);

    //dxlportPrintf(_DEF_DXL1, "Object Temp : %d, Ambient Temp : %d\r\n", (int)mlx90614.object_temp_c, (int)mlx90614.ambient_temp_c);
  }
}

#endif /* _TEST_ENABLE_I2C */
