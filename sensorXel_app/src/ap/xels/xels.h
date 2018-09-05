/*
 * xels.h
 *
 *  Created on: 2018. 8. 24.
 *      Author: HanCheol Cho
 */

#ifndef SRC_AP_XELS_XELS_H_
#define SRC_AP_XELS_XELS_H_


#include "hw.h"
#include "ap_def.h"
#include "xels/xels_struct.hpp"



namespace ros2 {
enum MessagePrefix{
  TOPICS_PUBLISH = 0,
  TOPICS_SUBSCRIBE,
  SERVICE_REQUEST,
  SERVICE_RESPONSE,
  SERVICE,
  PARAMETER,
  ACTION
};
}

typedef union
{
  uint8_t  u8Data[128];
  uint8_t  BOOLEAN;
  char     CHAR;
  int8_t   INT8;
  uint8_t  UINT8;
  int16_t  INT16;
  uint16_t UINT16;
  int32_t  INT32;
  uint32_t UINT32;
  int64_t  INT64;
  uint64_t UINT64;
  float    FLOAT32;
  double   FLOAT64;
  Imu_t    IMU;
  uint32_t MILLIS;
  bool     LED;
  uint16_t ANALOG;
} xel_data_type_t;


void xelsInit(void);
XelNetwork::XelHeader_t * xelsGetHeader(uint8_t ch);
uint8_t xelsGetDataTypeLength(uint8_t data_type);
void xelsReadCallback(uint8_t ch, uint16_t addr, uint8_t *p_data, uint16_t length);
void xelsWriteCallback(uint8_t ch, uint16_t addr, uint8_t *p_data, uint16_t length);

#endif /* SRC_AP_XELS_XELS_H_ */
