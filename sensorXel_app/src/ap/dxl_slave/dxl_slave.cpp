/*
 * dxl_slave.cpp
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */




#include "ap.h"






//-- Internal Variables
//


//-- External Variables
//


//-- Internal Functions
//
static uint8_t processPing(uint8_t *p_data, uint16_t *p_length);
static uint8_t processReboot(void);
static uint8_t processFactoryReset(uint8_t mode);
static uint8_t processRead(uint16_t addr, uint8_t *p_data, uint16_t length);
static uint8_t processWrite(uint16_t addr, uint8_t *p_data, uint16_t length);


//-- External Functions
//





void dxlSlaveInit(void)
{
  dxlInit    (&p_ap->dxl_slave.node, DXL_PACKET_VER_2_0);
  dxlSetId   (&p_ap->dxl_slave.node, p_ap->dxl_slave.id);
  dxlOpenPort(&p_ap->dxl_slave.node, p_ap->dxl_slave.ch, p_ap->dxl_slave.baud);

  dxlAddProcessPingFunc        (&p_ap->dxl_slave.node, processPing);
  dxlAddProcessRebootFunc      (&p_ap->dxl_slave.node, processReboot);
  dxlAddProcessFactoryResetFunc(&p_ap->dxl_slave.node, processFactoryReset);
  dxlAddProcessReadFunc        (&p_ap->dxl_slave.node, processRead);
  dxlAddProcessWriteFunc       (&p_ap->dxl_slave.node, processWrite);
}

bool dxlSlaveLoop(void)
{
  uint8_t ret = false;


  if (p_ap->p_dxl_motor->use == true)
  {
    if (dxlProcessPacket(&p_ap->p_dxl_motor->node) != DXL_PROCESS_INST)
    {
      ret = true;
    }
  }

  return ret;
}

uint8_t processPing(uint8_t *p_data, uint16_t *p_length)
{
  uint8_t ret = DXL_ERR_NONE;


  p_data[0] = p_ap->model_number>>0;
  p_data[1] = p_ap->model_number>>8;
  p_data[2] = p_ap->firmware_version;

  *p_length = 3;

  return ret;
}

uint8_t processReboot(void)
{
  uint8_t ret = DXL_ERR_NONE;


  resetRunSoftReset();

  return ret;
}

uint8_t processFactoryReset(uint8_t mode)
{
  uint8_t ret = DXL_ERR_NONE;


  if (mode == 0x02)
  {
    ctableReset(&p_ap->ctable);
  }
  if (mode == 0xFF)
  {
    ctableReset(&p_ap->ctable);
  }
  return ret;
}

uint8_t processRead(uint16_t addr, uint8_t *p_data, uint16_t length)
{
  uint8_t ret = DXL_ERR_NONE;

  ret = ctableRead(&p_ap->ctable, addr, p_data, length);

  return ret;
}

uint8_t processWrite(uint16_t addr, uint8_t *p_data, uint16_t length)
{
  uint8_t ret = DXL_ERR_NONE;


  ret = ctableWrite(&p_ap->ctable, addr, p_data, length);

  return ret;
}


