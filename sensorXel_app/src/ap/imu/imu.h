/*
 * nfc.h
 *
 *  Created on: 2017. 3. 27.
 *      Author: JHSeon
 */

#ifndef IMU_H_
#define IMU_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_IMU

#include "bsp.h"


typedef struct
{
  float quat[4];
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;
  float heading;
  float pitch, roll, yaw;
  float aRes;
  float gRes;
  float mRes;

} imu_node_t;

bool imuInit(void);
bool imuIsInit(void);
bool imuBegin(void);
bool imuUpdate(imu_node_t *p_node);


#endif


#ifdef __cplusplus
}
#endif


#endif
