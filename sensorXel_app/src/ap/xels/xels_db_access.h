/*
 * xels_db_access.h
 *
 *  Created on: 2018. 9. 4.
 *      Author: D.ggavy
 */

#ifndef SRC_AP_XELS_XELS_DB_ACCESS_H_
#define SRC_AP_XELS_XELS_DB_ACCESS_H_



#include "hw.h"
#include "ap_def.h"
#include "xel/xel_struct.hpp"
#include "xels.h"
#include "mux/mux_init.h"
#include "imu.h"

void xel_db_accessUpdateMillis(xel_data_type_t* xel_data)
{
	xel_data->MILLIS = millis();
}

void xel_db_accessUpdateLed(xel_data_type_t* xel_data)
{
	xel_data->LED = 0;
}

void xel_db_accessUpdateAnalog(xel_data_type_t* xel_data)
{
	adcInit();

}

void xel_db_accessUpdateIMU(xel_data_type_t* xel_data)
{
	bool ret;

	mux_initSet(MUX_SPI);
	imuInit();
	ret = imuBegin();
	imuUpdate(xel_data->IMU);

}








#endif /* SRC_AP_XELS_XELS_DB_ACCESS_H_ */
