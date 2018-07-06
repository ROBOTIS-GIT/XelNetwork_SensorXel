/*
 * vcp.c
 *
 *  Created on: 2018. 2. 2.
 *      Author: OpusK
 */



#include <stdarg.h>
#include <stdbool.h>

#include "hw.h"
#include "uart.h"
#include "printf.h"





//-- Internal Variables
//


//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//





int __io_putchar(int ch)
{
  uartPutch(_DEF_UART2, ch);

  return 1;
}

int __io_getchar(void)
{
  return uartGetch(_DEF_UART2);
}



