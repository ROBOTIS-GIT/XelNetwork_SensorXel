/*
 * xels_db_access.h
 *
 *  Created on: 2018. 9. 4.
 *      Author: D.ggavy
 */

#ifndef SRC_AP_XELS_XELS_DB_ACCESS_H_
#define SRC_AP_XELS_XELS_DB_ACCESS_H_

#include "xels.h"
#include "dxl/dxl.h"

void xels_db_accessUpdateMillis(xel_data_type_t*);
void xels_db_accessUpdateLed(xel_data_type_t*);
void xels_db_accessUpdateAnalog(xel_data_type_t*, uint8_t);
void xels_db_accessUpdateIMU(xel_data_type_t*);
void xels_db_accessUpdateGPIO(xel_data_type_t*, uint8_t);

void xels_db_accessActivateLED(xel_data_type_t*);
void xels_db_accessActivateGPIO(xel_data_type_t*, uint8_t);





#endif /* SRC_AP_XELS_XELS_DB_ACCESS_H_ */
