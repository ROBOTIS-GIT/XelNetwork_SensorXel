/*
 * boot.c
 *
 *  Created on: 2017. 11. 23.
 *      Author: opus
 */

#include "boot.h"

#include "ap.h"
#include "hw.h"

#define BOOT_MODE_BUTTON  _DEF_BUTTON1
#define BOOT_LED          _DEF_LED1

//-- Internal Variables
//
const volatile __attribute__((section(".version_str"))) uint8_t fw_version_str[256] = _BOOT_VER_STR;
const volatile __attribute__((section(".version_num"))) uint8_t fw_version_num[256] = _BOOT_VER_NUM;

typedef struct {
    cmd_t   *cmd;
    uint8_t  uart_ch;
    uint32_t baud;
} boot_t;

cmd_t cmd_boot_ch1;
cmd_t cmd_boot_ch2;

boot_t boot_tbl[] =
{
    {&cmd_boot_ch1, _DEF_UART2, 57600},
    {&cmd_boot_ch2, _DEF_UART1, 57600},
    {NULL, 0, 0}
};

static boot_mode_t boot_mode = BOOT_JUMP_APP;

static void bootLoaderSetup(void);
static void bootJumpProcess(void);
static void bootLoaderProcess(void);

void bootInit(void)
{
  ledOff(BOOT_LED);

  boot_mode = bootCheckMode();

  switch(boot_mode)
  {
    case BOOT_JUMP_APP :
      break;

    case BOOT_LOADER :
      bootLoaderSetup();
      break;
  }
}

void bootProcess(void)
{
  switch(boot_mode)
  {
    case BOOT_JUMP_APP :
      bootJumpProcess();
      break;

    case BOOT_LOADER :
      bootLoaderProcess();
      break;
  }
}

#include "driver/drv_button.h"

boot_mode_t bootCheckMode(void)
{
  boot_mode_t boot_mode = BOOT_JUMP_APP;

  if(resetGetStatus() == _DEF_RESET_WDG)
  {
    resetClearFlag();
    boot_mode = BOOT_LOADER;
  }
  else if (drvButtonGetState(BOOT_MODE_BUTTON) == 1)
  {
    boot_mode = BOOT_LOADER;
  }

  return boot_mode;
}

static void bootJumpProcess(void)
{
  err_code_t err;
  fw_tag_type_a_t *p_tag;

  err = checkFw(FW_TAG_TYPE_A, _HW_DEF_FLASH_ADDR_APP_START);

  if(err == OK)
  {
    p_tag = (fw_tag_type_a_t*) (_HW_DEF_FLASH_ADDR_APP_START);
    bspJumpToAddress(p_tag->address);
  }
  else
  {
    bootLoaderSetup();

    boot_mode = BOOT_LOADER;
  }
}

static void bootLoaderSetup(void)
{
  uint8_t boot_ch = 0;

  while(boot_tbl[boot_ch].cmd != NULL)
  {
    cmdInit(boot_tbl[boot_ch].cmd);
    cmdBegin(boot_tbl[boot_ch].cmd, boot_tbl[boot_ch].uart_ch, boot_tbl[boot_ch].baud);
    if(boot_tbl[boot_ch].uart_ch == _DEF_UART2)
    {
      dxlportOpen(_DEF_DXL1, boot_tbl[boot_ch].baud);
    }
    boot_ch++;
  }

  ledOn(BOOT_LED);
}

static void bootLoaderProcess(void)
{
  uint8_t boot_ch = 0;

  while(boot_tbl[boot_ch].cmd != NULL)
  {
    if(cmdReceivePacket(boot_tbl[boot_ch].cmd))
    {
      if(boot_tbl[boot_ch].cmd->packet_err == OK)
      {
        if(boot_tbl[boot_ch].uart_ch == _DEF_UART2)
        {
          dxlportTxEnable(_DEF_DXL1);
        }

        bootCmdProcess(boot_tbl[boot_ch].cmd);

        if(boot_tbl[boot_ch].uart_ch == _DEF_UART2)
        {
          dxlportTxDisable(_DEF_DXL1);
        }
      }
      else
      {
        cmdFlush(boot_tbl[boot_ch].cmd);

        if(boot_tbl[boot_ch].uart_ch == _DEF_UART2)
        {
          dxlportTxEnable(_DEF_DXL1);
        }

        cmdSendResp(boot_tbl[boot_ch].cmd, boot_tbl[boot_ch].cmd->packet_err, NULL, OK);

        if(boot_tbl[boot_ch].uart_ch == _DEF_UART2)
        {
          dxlportTxDisable(_DEF_DXL1);
        }
      }
    }
    boot_ch++;
  }
}
