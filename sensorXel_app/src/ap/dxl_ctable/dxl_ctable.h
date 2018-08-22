/*
 * dxl_ctable.h
 *
 *  Created on: 2017. 7. 18.
 *      Author: baram
 */

#ifndef DXL_CTABLE_H_
#define DXL_CTABLE_H_



#include "hw.h"


#define DXL_MODEL_NUMBER                        450
#define DXL_INIT_ID                             200



#define EEP_ADDR_CHECK_A                        0
#define EEP_ADDR_CHECK_5                        1
#define EEP_ADDR_ID                             2
#define EEP_ADDR_BAUD                           3


#define P_CONST_MODEL_NUMBER                    0
#define P_CONST_MODEL_INFO                      2
#define P_CONST_FW_VERSION                      6
#define P_EEP_ID                                7
#define P_EEP_DXL_BAUDRATE                      8





void dxlCtableInit(void);
void dxlCtableUpdate(void);


#endif
