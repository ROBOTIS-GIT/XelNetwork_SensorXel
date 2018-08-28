/*
 * ap.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef AP_H_
#define AP_H_



#include "hw.h"


#include "qbuffer.h"
#include "dxl/dxl.h"
#include "ctable/ctable.h"
#include "dxl_ctable/dxl_ctable.h"
#include "dxl_slave/dxl_slave.h"



typedef struct
{
  err_code_t      err_code;


  uint16_t        model_number;
  uint8_t         firmware_version;


  dxl_node_t      dxl_slave;
  dxl_node_t      *p_dxl_motor;

  ctable_t        ctable;
} ap_t;





extern ap_t *p_ap;


void apInit(void);
void apMain(void);





#endif /* AP_H_ */
