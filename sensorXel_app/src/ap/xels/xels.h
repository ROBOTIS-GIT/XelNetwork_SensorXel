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

namespace XelNetwork
{
enum DataType
{
  BOOLEAN = 0,
  CHAR,
  INT8,
  UINT8,
  INT16,
  UINT16,
  INT32,
  UINT32,
  INT64,
  UINT64,
  FLOAT32,
  FLOAT64,
  VECTOR3,
  QUATERNION,
  POINT,
  TWIST,
  IMU
};
} //namespace XelNetwork

typedef struct
{
  uint8_t   data_type;
  uint32_t  data_get_interval_hz;
  char      data_name[32];
  uint8_t   msg_type;
  uint16_t  data_addr;
  uint8_t   data_lenght;
} __attribute__((packed)) xels_header_t;


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
  //VECTOR3,
  //QUATERNION,
  //POINT,
  //TWIST,
  //IMU
} xel_data_type_t;



void xelsInit(void);


#endif /* SRC_AP_XELS_XELS_H_ */
