/*
 *  drv_eeprom.c
 *
 *  Created on: 2018. 2. 1.
 *      Author: OpusK
 */

#include "hw.h"
#include "drv_eeprom.h"


static bool IsInit = false;


/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS  ((uint32_t)0x08080000)
#define EEPROM_SIZE           4096
#define EEPROM_END_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + EEPROM_SIZE - 1))


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



bool drvEepromInit()
{
  IsInit = true;

  return IsInit;
}

uint8_t drvEepromReadByte(uint32_t index)
{
  uint8_t data;
  uint32_t addr;

  if( IsInit == false )
  {
    return 0;
  }

  addr = EEPROM_START_ADDRESS + index;

  if((addr < EEPROM_START_ADDRESS) || (addr > EEPROM_END_ADDRESS))
  {
    return 0;
  }

  data = *(volatile uint8_t*)(addr);

  return data;
}

err_code_t drvEepromReadBytes(uint32_t index, uint8_t* p_data_out, uint32_t length)
{
  uint8_t addr_cnt;
  uint32_t addr;

  if(IsInit == false)
  {
    return ERR_EEPROM_ERASE;
  }

  addr = EEPROM_START_ADDRESS + index;

  if((addr < EEPROM_START_ADDRESS) || (addr > EEPROM_END_ADDRESS))
  {
    return ERR_EEPROM_ERASE_ADDR;
  }

  if((addr + length - 1) > EEPROM_END_ADDRESS)
  {
    return ERR_EEPROM_ERASE_LENGTH;
  }

  for (addr_cnt=0; addr_cnt<length; addr_cnt++)
  {
    p_data_out[addr_cnt++] = *(volatile uint8_t*)(addr+addr_cnt);
  }

  return OK;
}

err_code_t drvEepromWriteByte(uint32_t index, uint8_t data_in)
{
  uint32_t addr;
  err_code_t err;

  if( IsInit == false ) return false;

  addr = EEPROM_START_ADDRESS + index;

  if((addr < EEPROM_START_ADDRESS) || (addr > EEPROM_END_ADDRESS))
  {
    return ERR_EEPROM_ERASE_ADDR;
  }

  HAL_FLASHEx_DATAEEPROM_Unlock();

  if(HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, addr, data_in) != HAL_OK)
  {
    err = ERR_EEPROM_WRITE;
  }

  HAL_FLASHEx_DATAEEPROM_Lock();

  return err;
}

err_code_t drvEepromWriteBytes(uint32_t index, uint8_t* p_data_in, uint32_t length)
{
  uint32_t i, addr;
  err_code_t err;

  if(IsInit == false)
  {
    return ERR_EEPROM_ERASE;
  }

  addr = EEPROM_START_ADDRESS + index;

  if((addr < EEPROM_START_ADDRESS) || (addr > EEPROM_END_ADDRESS))
  {
    return ERR_EEPROM_ERASE_ADDR;
  }

  if((addr + length - 1) > EEPROM_END_ADDRESS)
  {
    return ERR_EEPROM_ERASE_LENGTH;
  }

  HAL_FLASHEx_DATAEEPROM_Unlock();

  for(i = 0; i < length; i++)
  {
    if(HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, addr, p_data_in[i]) != HAL_OK)
    {
      err = ERR_EEPROM_WRITE;
      break;
    }
  }

  HAL_FLASHEx_DATAEEPROM_Lock();

  return err;
}

uint32_t drvEepromGetLength(void)
{
  if( IsInit == false ) return 0;

  return EEPROM_SIZE;
}

err_code_t drvEepromEraseBytes(uint32_t index, uint32_t length)
{
  uint32_t i, addr;
  err_code_t err;

  if(IsInit == false)
  {
    return ERR_EEPROM_ERASE;
  }

  addr = EEPROM_START_ADDRESS + index;

  if((addr < EEPROM_START_ADDRESS) || (addr > EEPROM_END_ADDRESS))
  {
    return ERR_EEPROM_ERASE_ADDR;
  }

  if((addr + length - 1) > EEPROM_END_ADDRESS)
  {
    return ERR_EEPROM_ERASE_LENGTH;
  }

  HAL_FLASHEx_DATAEEPROM_Unlock();

  for(i = 0; i < length; i++)
  {
    if(HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEERASEDATA_BYTE, addr+i) != HAL_OK)
    {
      err = ERR_EEPROM_ERASE;
      break;
    }
  }

  HAL_FLASHEx_DATAEEPROM_Lock();

  return err;
}


err_code_t drvEepromFormat(void)
{
  uint32_t i;
  err_code_t err;

  if(IsInit == false)
  {
    return ERR_EEPROM_ERASE;
  }

  HAL_FLASHEx_DATAEEPROM_Unlock();

  for(i = 0; i < EEPROM_SIZE; i+4)
  {
    if(HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEERASEDATA_WORD, EEPROM_START_ADDRESS + i) != HAL_OK)
    {
      err = ERR_EEPROM_ERASE;
      break;
    }
  }

  HAL_FLASHEx_DATAEEPROM_Lock();

  return err;
}
