/*
 * neo_6m_gps.h
 *
 *  Created on: 2017. 11. 3.
 *      Author: opus
 */

#ifndef NEO6M_GPS_H_
#define NEO6M_GPS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "hw_def.h"

#ifdef _USE_HW_NEO6M_GPS

#define NEO6M_GPS_UART_CH _DEF_UART5

typedef struct
{
    uint8_t ch;

    float latitude;
    float longitude;
} neo6m_gps_t;

bool neo6mGpsInit(void);
bool neo6mGpsBegin(neo6m_gps_t *p_neo6m_gps, uint8_t uart_ch);
void neo6mGpsParseData(neo6m_gps_t* p_neo6m_gps);
bool neo6mGpsGetLocation(neo6m_gps_t *p_neo6m_gps);

#endif

#ifdef __cplusplus
}
#endif

#endif /* NEO_6M_GPS_H_ */
