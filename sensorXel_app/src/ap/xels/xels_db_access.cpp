/*
 * xels_db_access.cpp
 *
 *  Created on: 2018. 9. 4.
 *      Author: D.ggavy
 */


#include "hw.h"
#include "ap_def.h"
#include "xels/xels_struct.hpp"
#include "xels.h"
#include "mux/mux_init.h"
#include "imu/imu.h"

void xels_db_accessUpdateMillis(xel_data_type_t* xel_data)
{
	xel_data->MILLIS = millis();
}

void xels_db_accessUpdateLed(xel_data_type_t* xel_data)
{
	xel_data->BOOLEAN = ledState(_DEF_LED1);
}

void xels_db_accessUpdateAnalog(xel_data_type_t* xel_data, uint8_t adc_ch)
{
	adcInit();
	adcSetup(adc_ch);

	xel_data->UINT16 = adcRead(adc_ch);
}

void xels_db_accessUpdateIMU(xel_data_type_t* xel_data)
{
	bool ret;
	imu_node_t imu_node;

	mux_initSet(MUX_SPI);
	imuInit();
	ret = imuBegin();
	imuUpdate(&imu_node);

	xel_data->IMU.quat_x = (double)imu_node.quat[0];
	xel_data->IMU.quat_y = (double)imu_node.quat[1];
	xel_data->IMU.quat_z = (double)imu_node.quat[2];
	xel_data->IMU.quat_w = (double)imu_node.quat[3];

	xel_data->IMU.acc_x = (double)imu_node.ax;
	xel_data->IMU.acc_y = (double)imu_node.ay;
	xel_data->IMU.acc_z = (double)imu_node.az;

	xel_data->IMU.gyro_x = (double)imu_node.gx;
	xel_data->IMU.gyro_y = (double)imu_node.gy;
	xel_data->IMU.gyro_z = (double)imu_node.gz;
}

void xels_db_accessUpdateGPIO(xel_data_type_t* xel_data, uint8_t gpio_ch)
{
    gpioPinMode(gpio_ch, _DEF_INPUT_PULLUP);
    xel_data->BOOLEAN = gpioPinRead(gpio_ch);
}

void xels_db_accessActivateLED(xel_data_type_t* xel_data)
{
  if (xel_data->BOOLEAN == 0)
  {
    ledOff(_DEF_LED1);
  }
  else
  {
    ledOn(_DEF_LED1);
  }
}

void xels_db_accessActivateGPIO(xel_data_type_t* xel_data, uint8_t gpio_ch)
{
    gpioPinMode(gpio_ch, _DEF_OUTPUT_PULLUP);
    gpioPinWrite(gpio_ch, xel_data->BOOLEAN);
}





