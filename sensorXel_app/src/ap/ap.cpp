/*
 * ap.cpp
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#include "ap.h"
#include "hw.h"
#include "ap_def.h"

#include "test.h"


static ap_t ap_log;

ap_t *p_ap = &ap_log;



//-- External Variables

//-- Internal Functions

//-- External Functions

void apInit(void)
{
  cmdifBegin(_DEF_UART2, 57600);

  p_ap->model_number = DXL_MODEL_NUMBER;
  p_ap->firmware_version = 1;



  p_ap->dxl_slave.use  = true;
  p_ap->dxl_slave.ch   = _DEF_DXL1;
  p_ap->dxl_slave.id   = DXL_INIT_ID;
  p_ap->dxl_slave.baud = 57600;

  p_ap->p_dxl_motor  = &p_ap->dxl_slave;


  ledOn(_DEF_LED1);
}


void apMain(void)
{
  uint32_t pre_time = millis();


  dxlSlaveInit();
  dxlCtableInit();



  while(1)
  {
    dxlSlaveLoop();

    if(millis() - pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
  }
}
