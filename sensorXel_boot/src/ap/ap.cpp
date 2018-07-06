/*
 * ap.cpp
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#include "ap.h"
#include "hw.h"
#include "ap_def.h"



//-- External Variables
//

//-- Internal Functions

//-- External Functions

void apInit(void)
{
  bootInit();
}


void apMain(void)
{
  while(1)
  {
    bootProcess();
  }
}

