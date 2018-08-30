/*
 * xels.cpp
 *
 *  Created on: 2018. 8. 24.
 *      Author: HanCheol Cho
 */




#include "xels.h"
#include "dxl/dxl.h"
#include "xel/xel_struct.hpp"





void xelsInit(void)
{
}

uint8_t xelsGetDataTypeLength(uint8_t data_type)
{
  uint8_t ret = 0;


  switch(data_type)
  {
    case XelNetwork::BOOLEAN:
    case XelNetwork::CHAR:
    case XelNetwork::INT8:
    case XelNetwork::UINT8:
      ret = 1;
      break;
    case XelNetwork::INT16:
    case XelNetwork::UINT16:
      ret = 2;
      break;
    case XelNetwork::INT32:
    case XelNetwork::UINT32:
    case XelNetwork::FLOAT32:
      ret = 4;
      break;
    case XelNetwork::INT64:
    case XelNetwork::UINT64:
    case XelNetwork::FLOAT64:
      ret = 8;
      break;
    case XelNetwork::IMU:
      ret = sizeof(Imu_t);
      break;
  }

  return ret;
}

