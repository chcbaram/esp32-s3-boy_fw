/*
 * mixer.h
 *
 *  Created on: 2020. 8. 8.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_MIXER_H_
#define SRC_COMMON_HW_INCLUDE_MIXER_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_MIXER


#define MIXER_MAX_CH        HW_MIXER_MAX_CH
#define MIXER_MAX_BUF_LEN   HW_MIXER_MAX_BUF_LEN


bool mixerInit(void);
bool mixerWrite(uint8_t ch, int16_t *p_data, uint32_t length);
bool mixerRead(int16_t *p_data, uint32_t length);
bool mixerIsEmpty(uint8_t ch);

uint32_t mixerAvailable();
uint32_t mixerAvailableForWrite(uint8_t ch);
int8_t   mixerGetEmptyChannel(void);
int8_t   mixerGetValidChannel(uint32_t length);

int16_t mixerSamples(int16_t a, int16_t b);


#ifdef __cplusplus
}
#endif


#endif



#endif /* SRC_COMMON_HW_INCLUDE_MIXER_H_ */
