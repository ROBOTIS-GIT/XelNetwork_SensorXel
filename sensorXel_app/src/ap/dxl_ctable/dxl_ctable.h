/*
 * dxl_ctable.h
 *
 *  Created on: 2017. 7. 18.
 *      Author: baram
 */

#ifndef DXL_CTABLE_H_
#define DXL_CTABLE_H_



#include "hw.h"


#define DXL_MODEL_NUMBER                        460
#define DXL_INIT_ID                             1
#define DXL_INIT_BAUD                           3    // 1Mbps



#define EEP_ADDR_CHECK_AA                       0
#define EEP_ADDR_CHECK_55                       1
#define EEP_ADDR_ID                             2
#define EEP_ADDR_BAUD                           3

#define EEP_ADDR_XEL_HEADER_1_CHECK_AA          4
#define EEP_ADDR_XEL_HEADER_1_CHECK_55          5
#define EEP_ADDR_XEL_HEADER_1                   6    // len = 38
#define EEP_ADDR_XEL_HEADER_1_CHECKSUM          44



#define P_CONST_MODEL_NUMBER                    0
#define P_CONST_MODEL_INFO                      2
#define P_CONST_FW_VERSION                      6
#define P_EEP_ID                                7
#define P_EEP_DXL_BAUDRATE                      8

#define P_MILLIS                                12


#define P_XEL_HEADER_DATA_TYPE                  32   // 1
#define P_XEL_HEADER_DATA_INTERVAL              33   // 4
#define P_XEL_HEADER_DATA_NAME                  37   // 32
#define P_XEL_HEADER_DATA_DIRECTION             69   // 1
#define P_XEL_HEADER_DATA_ADDR                  70   // 2
#define P_XEL_HEADER_DATA_LENGTH                72   // 1
#define P_XEL_DATA                              128  // 128



void dxlCtableInit(void);
void dxlCtableUpdate(void);


#endif
