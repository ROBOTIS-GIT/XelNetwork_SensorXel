/*
 * neo_6m_gps.cpp
 *
 *  Created on: 2017. 11. 3.
 *      Author: opus
 */

#include "neo6m_gps.h"

#ifdef _USE_HW_NEO6M_GPS

#include "hw.h"
#include "gps/TinyGPS++.h"

void neo6mGpsCmdifInit(void);
int neo6mGpsCmdif(int argc, char **argv);


TinyGPSPlus neo6m_gps;

bool neo6mGpsInit(void){
  neo6mGpsCmdifInit();

  return true;
}

bool neo6mGpsBegin(neo6m_gps_t* p_neo6m_gps, uint8_t uart_ch)
{
  p_neo6m_gps->ch = uart_ch;

  uartOpen(p_neo6m_gps->ch, 9600);

  return true;
}

void neo6mGpsParseData(neo6m_gps_t* p_neo6m_gps){
  while(uartAvailable(p_neo6m_gps->ch)){
    neo6m_gps.encode(uartRead(p_neo6m_gps->ch));
  }
}

bool neo6mGpsGetLocation(neo6m_gps_t* p_neo6m_gps)
{
  if (neo6m_gps.location.isValid())
  {
    p_neo6m_gps->latitude = (float)(neo6m_gps.location.lat());
    p_neo6m_gps->longitude = (float)(neo6m_gps.location.lng());

    return true;
  }
  return false;
}



void neo6mGpsCmdifInit(void)
{
  cmdifAdd((char*)"neo6mGps", neo6mGpsCmdif);
}

int neo6mGpsCmdif(int argc, char **argv)
{
  bool ret = true;
  neo6m_gps_t neo6m_gps;

  if(argc == 2 && strcmp("read", argv[1]) == 0)
  {
    ret = neo6mGpsBegin(&neo6m_gps, NEO6M_GPS_UART_CH);
    if(ret == false) cmdifPrintf("neo6m_gps start fail\n");

    while(cmdifRxAvailable() == 0)
    {
      neo6mGpsParseData(&neo6m_gps);

      neo6mGpsGetLocation(&neo6m_gps);

      cmdifPrintf("GPS - latitude : %3.2f, longitude : %3.2f\n", neo6m_gps.latitude, neo6m_gps.longitude);
      delay(500);
    }
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf( "neo6mgps read\n");
  }

  return 0;
}

#endif
