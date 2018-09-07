/*
 * xels.cpp
 *
 *  Created on: 2018. 8. 24.
 *      Author: HanCheol Cho
 */




#include "xels.h"
#include "dxl/dxl.h"
#include "xels/xels_db_access.h"



static XelNetwork::XelHeader_t xel_header = {
      XelNetwork::DataType::UINT32,
      5,
      "millis",
      XelNetwork::SEND,
      128,
      1
  };

static xel_data_type_t xel_data;



void xelsInitCallback(uint8_t ch);



void xelsInit(void)
{

  XelNetwork::XelHeader_t *p_xel_header;

  p_xel_header = &xel_header;

  p_xel_header->data_type            = XelNetwork::DataType::UINT32;
  p_xel_header->data_get_interval_hz = 5;
  p_xel_header->data_direction       = XelNetwork::SEND;
  p_xel_header->data_addr            = 128;
  p_xel_header->data_length          = xelsGetDataTypeLength(p_xel_header->data_type);

  strcpy(p_xel_header->data_name, "millis");


  xelsInitCallback(0);
}

XelNetwork::XelHeader_t * xelsGetHeader(uint8_t ch)
{
  return &xel_header;
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
    case XelNetwork::LED:
    case XelNetwork::GPIO0:
    case XelNetwork::GPIO1:
    case XelNetwork::GPIO2:
    case XelNetwork::GPIO3:
      ret = 1;
      break;
    case XelNetwork::INT16:
    case XelNetwork::UINT16:
    case XelNetwork::ANALOG0:
    case XelNetwork::ANALOG1:
    case XelNetwork::ANALOG2:
    case XelNetwork::ANALOG3:

      ret = 2;
      break;
    case XelNetwork::INT32:
    case XelNetwork::UINT32:
    case XelNetwork::FLOAT32:
    case XelNetwork::MILLIS:
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
    default:
      ret = 4;
      break;
  }

  return ret;
}

void xelsInitCallback(uint8_t ch)
{
  XelNetwork::XelHeader_t *p_xel_header;
  uint8_t data_type;

  p_xel_header = xelsGetHeader(ch);

  data_type = p_xel_header->data_type;
  switch(data_type)
  {
    case XelNetwork::IMU:
      break;
  }
}

void xelsReadCallback(uint8_t ch, uint16_t addr, uint8_t *p_data, uint16_t length)
{
  XelNetwork::XelHeader_t *p_xel_header;
  uint8_t data_type;

  p_xel_header = xelsGetHeader(ch);

  data_type = p_xel_header->data_type;
  switch(data_type)
  {
    case XelNetwork::MILLIS:
      xels_db_accessUpdateMillis(&xel_data);
      break;

    case XelNetwork::LED:
      xels_db_accessUpdateLed(&xel_data);
      break;

    case XelNetwork::ANALOG0:
      xels_db_accessUpdateAnalog(&xel_data, _DEF_ADC1);
      break;

    case XelNetwork::ANALOG1:
      xels_db_accessUpdateAnalog(&xel_data, _DEF_ADC2);
      break;

    case XelNetwork::ANALOG2:
      xels_db_accessUpdateAnalog(&xel_data, _DEF_ADC3);
      break;

    case XelNetwork::ANALOG3:
      xels_db_accessUpdateAnalog(&xel_data, _DEF_ADC4);
      break;

    case XelNetwork::IMU:
      xels_db_accessUpdateIMU(&xel_data);
      break;

    case XelNetwork::GPIO0:
    	xels_db_accessUpdateGPIO(&xel_data, _DEF_GPIO1);
    	break;

    case XelNetwork::GPIO1:
    	xels_db_accessUpdateGPIO(&xel_data, _DEF_GPIO2);
    	break;

    case XelNetwork::GPIO2:
    	xels_db_accessUpdateGPIO(&xel_data, _DEF_GPIO3);
    	break;

    case XelNetwork::GPIO3:
    	xels_db_accessUpdateGPIO(&xel_data, _DEF_GPIO4);
    	break;

  }

  memcpy(p_data, &xel_data.u8Data[addr], length);
}

void xelsWriteCallback(uint8_t ch, uint16_t addr, uint8_t *p_data, uint16_t length)
{
  XelNetwork::XelHeader_t *p_xel_header;
  uint8_t data_type;


  p_xel_header = xelsGetHeader(ch);
  memcpy(&xel_data.u8Data[addr], p_data, length);

  data_type = p_xel_header->data_type;
  switch(data_type)
  {
    case XelNetwork::LED:
      xels_db_accessActivateLED(&xel_data);
      break;

    case XelNetwork::GPIO0:
      xels_db_accessActivateGPIO(&xel_data, _DEF_GPIO1);
      break;

    case XelNetwork::GPIO1:
      xels_db_accessActivateGPIO(&xel_data, _DEF_GPIO2);
      break;

    case XelNetwork::GPIO2:
      xels_db_accessActivateGPIO(&xel_data, _DEF_GPIO3);
      break;

    case XelNetwork::GPIO3:
      xels_db_accessActivateGPIO(&xel_data, _DEF_GPIO4);
      break;
  }
}

