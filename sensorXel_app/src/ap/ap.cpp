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

//-- External Variables
//
//dxl_node_t dxl_node;

//-- Internal Functions

//-- External Functions

void apInit(void)
{
  cmdifBegin(_DEF_UART2, 57600);

//  dxl_node.use  = true;
//  dxl_node.ch   = _DEF_DXL1;
//  dxl_node.baud = 1000000;
//  dxlInit    (&dxl_node.node, DXL_PACKET_VER_2_0);
//  dxlOpenPort(&dxl_node.node, dxl_node.ch, dxl_node.baud);

  ledOn(_DEF_LED1);
}


void apMain(void)
{
  uint32_t pre_time = millis();

  testMain();

  while(1)
  {
    cmdifMain();

    if(millis() - pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
  }
}
