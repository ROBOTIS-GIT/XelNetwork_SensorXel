/*
 * drv_adc.h
 *
 *  Created on: 2018. 2. 10.
 *      Author: kei
 */

#ifndef DRV_ADC_H_
#define DRV_ADC_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"


bool     drvAdcInit(void);
bool     drvAdcSetup(uint8_t ch);

uint16_t drvAdcRead(uint8_t ch);
uint16_t drvAdcRead8(uint8_t ch);
uint16_t drvAdcRead10(uint8_t ch);
uint16_t drvAdcRead12(uint8_t ch);
uint16_t drvAdcRead16(uint8_t ch);
uint16_t drvAdcReadVoltage(uint8_t ch);
uint16_t drvAdcConvVoltage(uint8_t ch, uint32_t adc_value);

uint8_t  drvAdcGetRes(uint8_t ch);


#ifdef __cplusplus
}
#endif



#endif /* DRV_ADC_H_ */
