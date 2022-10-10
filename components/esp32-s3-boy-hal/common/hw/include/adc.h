/*
 * adc.h
 *
 *  Created on: 2022. 9. 18.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_ADC_H_
#define SRC_COMMON_HW_INCLUDE_ADC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_ADC


#define ADC_MAX_CH    HW_ADC_MAX_CH


bool    adcInit(void);
int32_t adcRead(uint8_t ch);
int32_t adcRead8(uint8_t ch);
int32_t adcRead10(uint8_t ch);
int32_t adcRead12(uint8_t ch);
int32_t adcRead16(uint8_t ch);
uint8_t adcGetRes(uint8_t ch);


#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_ADC_H_ */