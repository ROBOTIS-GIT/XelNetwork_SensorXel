/*
 *  drv_flash.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram
 */



#include "hw.h"
#include "drv_flash.h"



typedef struct
{
  uint32_t addr;
  uint32_t length;
} flash_page_attr_t;


uint32_t            flash_page_total = DRV_FLASH_PAGE_TOTAL;
flash_page_attr_t   flash_page_attr[DRV_FLASH_PAGE_TOTAL];


static err_code_t drvFlashErasePage(uint32_t page_addr);
static err_code_t drvFlashErasePages(uint32_t start_page_addr, uint32_t page_cnt );

bool drvFlashInit(void)
{
  uint32_t i;

  for(i = 0; i < DRV_FLASH_PAGE_TOTAL; i++)
  {
	  flash_page_attr[i].addr   = 0x08000000 + (256*i);
	  flash_page_attr[i].length = 256;
  }

  return true;
}

err_code_t drvFlashWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  err_code_t err_code = OK;
  HAL_StatusTypeDef HAL_FLASHStatus = HAL_OK;
  uint32_t start_addr = addr;
  uint32_t write_cnt;
  uint32_t i;
  uint32_t *p_buf = (uint32_t *)p_data;

  write_cnt = length / 128; // halfpage (256/2)

  if( (length % 128) > 0 ) write_cnt++;

  HAL_FLASH_Unlock();
  for( i = 0; i < write_cnt; i++ )
  {
    HAL_FLASHStatus = HAL_FLASHEx_HalfPageProgram(start_addr+(i*128), &p_buf[i*32] );

    if( HAL_FLASHStatus != HAL_OK )
    {
      HAL_FLASHStatus = HAL_FLASHEx_HalfPageProgram(start_addr+(i*128), &p_buf[i*32] );
    }
  }
  HAL_FLASH_Lock();

  return err_code;
}

err_code_t drvFlashRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  err_code_t err_code = OK;
  uint32_t Dataindex;
  uint32_t addr_cnt;


  Dataindex = 0;
  for (addr_cnt=0;addr_cnt<length;addr_cnt++)
  {
    p_data[Dataindex++] = *(volatile uint8_t*)(addr+addr_cnt);
  }

  return err_code;
}

err_code_t drvFlashErase(uint32_t addr, uint32_t length)
{
  err_code_t err_code = OK;
  uint32_t addr_begin;
  uint32_t addr_end;
  uint32_t target_addr_begin;
  uint32_t target_addr_end;

  uint32_t i;


  target_addr_begin = addr;
  target_addr_end   = addr + length;


  for (i=0; i<flash_page_total; i++)
  {
    addr_begin = flash_page_attr[i].addr;
    addr_end   = flash_page_attr[i].addr + flash_page_attr[i].length - 1;

    if ((addr_begin >= target_addr_begin) && (addr_begin <= target_addr_end))
    {
      err_code = drvFlashErasePage(addr_begin);
    }
    else if((addr_end >= target_addr_begin) && (addr_end <= target_addr_end) )
    {
      err_code = drvFlashErasePage(addr_begin);
    }
  }


  return err_code;
}

static err_code_t drvFlashErasePage(uint32_t page_addr)
{
  err_code_t err_code = OK;
  HAL_StatusTypeDef HAL_FLASHStatus = HAL_OK;
  FLASH_EraseInitTypeDef pEraseInit;
  uint32_t page_err;

  pEraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
  pEraseInit.PageAddress = page_addr;
  pEraseInit.NbPages     = 1;

  HAL_FLASH_Unlock();

  HAL_FLASHStatus = HAL_FLASHEx_Erase(&pEraseInit, &page_err);
  if(HAL_FLASHStatus != HAL_OK)
  {
    err_code = ERR_FLASH_ERASE;
  }

  HAL_FLASH_Lock();

  return err_code;
}

err_code_t drvFlashErasePages(uint32_t start_page_addr, uint32_t page_cnt )
{

  err_code_t err_code = OK;
  HAL_StatusTypeDef HAL_FLASHStatus = HAL_OK;
  FLASH_EraseInitTypeDef pEraseInit;
  uint32_t page_err;

  pEraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
  pEraseInit.PageAddress = start_page_addr;
  pEraseInit.NbPages     = page_cnt;

  HAL_FLASH_Unlock();

  HAL_FLASHStatus = HAL_FLASHEx_Erase(&pEraseInit, &page_err);
  if(HAL_FLASHStatus != HAL_OK)
  {
    err_code = ERR_FLASH_ERASE;
  }

  HAL_FLASH_Lock();

  return err_code;
}

