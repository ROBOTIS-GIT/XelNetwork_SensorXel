/*
 * nfc.c
 *
 *  Created on: 2017. 4. 4.
 *      Author: JHSeon
 */



#include <stdarg.h>
#include <stdbool.h>

#include "imu.h"

#ifdef _USE_HW_IMU

#include "hw.h"

#ifndef _USE_HW_SPI
#error "You must use SPI for this function"
#endif

#include "./mpu9250/MPU9250.h"
#include "./mpu9250/MadgwickAHRS.h"




//-- Internal Variables
//
static cMPU9250 SEN;
static imu_node_t imu_node;
Madgwick filter;
uint32_t update_hz = 200;
uint32_t update_ms = 1000 / 200;

static bool is_init = false;
static bool bConnected;

//-- External Variables
//


//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_IMU
void imuCmdifInit(void);
int  imuCmdif(int argc, char **argv);
#endif

void imuComputeIMU( void );

//-- External Functions
//




bool imuInit(void)
{
  bool ret = true;


#ifdef _USE_HW_CMDIF_IMU
  imuCmdifInit();
#endif


  bConnected = false;

  imu_node.aRes =          8.0 / 32768.0; // 8g
  imu_node.gRes =       2000.0 / 32768.0; // 2000dps
  imu_node.mRes = 10. * 4912.0 / 32760.0; // 16BIT


  is_init = imuBegin();

  return ret;
}

bool imuIsInit(void)
{
  return is_init;
}

bool imuBegin(void)
{
  bool ret = true;
  uint32_t i;
  uint32_t pre_time;


  bConnected = SEN.begin();

  if( bConnected == true )
  {
    filter.begin(update_hz);

    for (i=0; i<32; i++)
    {
      imuUpdate(&imu_node);
    }

    pre_time = millis();
    while(!SEN.gyro_cali_get_done())
    {
      if (imuUpdate(&imu_node) == true)
      {
      }

      if (millis()-pre_time > 5000)
      {
        break;
      }
    }



    /*
    while(1)
    {
      imuUpdate(&imu_node);
      vcpPrintf("%d %d %d\n", (int)imu_node.roll, (int)imu_node.pitch, (int)imu_node.yaw);
    }
    */
  }

  ret = bConnected;

  return ret;
}



bool imuUpdate(imu_node_t *p_node)
{
  bool ret = false;
  static uint32_t tTime;

  if( (millis()-tTime) >= update_ms )
  {
    tTime = millis();

    imuComputeIMU();

    *p_node = imu_node;

    ret = true;
  }

/*
  if ( imu.fifoAvailable() )
  {
    // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
    if ( imu.dmpUpdateFifo() == INV_SUCCESS)
    {
      imu.computeEulerAngles();

      p_node->roll  = imu.roll;
      p_node->pitch = imu.pitch;
      p_node->yaw   = imu.yaw;


      p_node->ax = imu.calcAccel(imu.ax);
      p_node->ay = imu.calcAccel(imu.ay)*1000;
      p_node->az = imu.calcAccel(imu.az)*1000;

      p_node->gx = imu.calcGyro(imu.gx);
      p_node->gy = imu.calcGyro(imu.gy);
      p_node->gz = imu.calcGyro(imu.gz);
      ret = true;
    }
  }
*/
  return ret;
}


#define FILTER_NUM    3

/*---------------------------------------------------------------------------
     TITLE   : compute
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void imuComputeIMU( void )
{
  //static uint32_t prev_process_time = micros();
  //static uint32_t cur_process_time = 0;
  //static uint32_t process_time = 0;
  uint32_t i;
  static int32_t gyroADC[3][FILTER_NUM] = {0,};
  int32_t gyroAdcSum;

  uint32_t axis;

  SEN.acc_get_adc();
  SEN.gyro_get_adc();
  SEN.mag_get_adc();



  for (axis = 0; axis < 3; axis++)
  {
    gyroADC[axis][0] = SEN.gyroADC[axis];


    gyroAdcSum = 0;
    for (i=0; i<FILTER_NUM; i++)
    {
      gyroAdcSum += gyroADC[axis][i];
    }
    SEN.gyroADC[axis] = gyroAdcSum/FILTER_NUM;
    for (i=FILTER_NUM-1; i>0; i--)
    {
      gyroADC[axis][i] = gyroADC[axis][i-1];
    }

    if (abs(SEN.gyroADC[axis]) <= 3)
    {
      SEN.gyroADC[axis] = 0;
    }
  }


  imu_node.ax = (float)SEN.accADC[0]*imu_node.aRes;
  imu_node.ay = (float)SEN.accADC[1]*imu_node.aRes;
  imu_node.az = (float)SEN.accADC[2]*imu_node.aRes;

  imu_node.gx = (float)SEN.gyroADC[0]*imu_node.gRes;
  imu_node.gy = (float)SEN.gyroADC[1]*imu_node.gRes;
  imu_node.gz = (float)SEN.gyroADC[2]*imu_node.gRes;

  imu_node.mx = (float)SEN.magADC[0]*imu_node.mRes;
  imu_node.my = (float)SEN.magADC[1]*imu_node.mRes;
  imu_node.mz = (float)SEN.magADC[2]*imu_node.mRes;


  //cur_process_time  = micros();
  //process_time      = cur_process_time-prev_process_time;
  //prev_process_time = cur_process_time;

  if (SEN.calibratingG == 0 && SEN.calibratingA == 0)
  {
    //filter.invSampleFreq = (float)process_time/1000000.0f;
    filter.updateIMU(imu_node.gx, imu_node.gy, imu_node.gz, imu_node.ax, imu_node.ay, imu_node.az);
  }


  imu_node.roll  = filter.getRoll();
  imu_node.pitch = filter.getPitch();
  imu_node.yaw   = filter.getYaw()-180.;

  imu_node.quat[0] = filter.q0;
  imu_node.quat[1] = filter.q1;
  imu_node.quat[2] = filter.q2;
  imu_node.quat[3] = filter.q3;
}


#ifdef _USE_HW_CMDIF_IMU
void imuCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd((char *)"imu", imuCmdif);
}


int imuCmdif(int argc, char **argv)
{
  bool ret = true;


  if (argc < 2)
  {
    ret = false;
  }
  else
  {

    if(strcmp("info", argv[1]) == 0)
    {
      cmdifPrintf("imu init : %d\n", is_init);
    }
    else if(strcmp("begin", argv[1]) == 0)
    {
      uint32_t data_count = 0;
      uint32_t data_rate = 0;
      uint32_t pre_count = 0;
      uint32_t pre_time = millis();

      pre_time = millis();

      SEN.acc_cali_start();
      while(!SEN.acc_cali_get_done())
      {
        if (imuUpdate(&imu_node) == true)
        {
        }
        if (millis()-pre_time > 5000)
        {
          break;
        }
      }

      while(cmdifRxAvailable() == 0)
      {
        if (imuUpdate(&imu_node))
        {
            int r, p, y;

            r = imu_node.roll  * 1;
            p = imu_node.pitch * 1;
            y = imu_node.yaw   * 1;

            data_count++;

            cmdifPrintf("%d\t %d\t %d\t %d\n ", data_rate, r, p, y);
        }

        if (millis()-pre_time >= 1000)
        {
          pre_time = millis();
          data_rate = data_count - pre_count;
          pre_count = data_count;
        }
      }
    }
    else
    {
      ret = false;
    }
  }


  if (ret == false)
  {
    cmdifPrintf( "imu info\n");
    cmdifPrintf( "imu begin\n");
  }

  return 0;
}

#endif





#endif
