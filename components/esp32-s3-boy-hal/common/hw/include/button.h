/*
 * button.h
 *
 *  Created on: 2022. 9. 18.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_BUTTON_H_
#define SRC_COMMON_HW_INCLUDE_BUTTON_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_BUTTON

#define BUTTON_MAX_CH       HW_BUTTON_MAX_CH



bool     buttonInit(void);
void     buttonEnable(bool enable);
void     buttonClear(void);
bool     buttonGetPin(uint8_t ch);
bool     buttonGetPressed(uint8_t ch);
bool     buttonGetPressedEvent(uint8_t ch);
uint32_t buttonGetPressedTime(uint8_t ch);


bool     buttonGetReleased(uint8_t ch);
bool     buttonGetReleasedEvent(uint8_t ch);
uint32_t buttonGetReleasedTime(uint8_t ch);

uint8_t  buttonGetPressedCount(void);

void     buttonSetRepeatTime(uint8_t ch, uint32_t detect_ms, uint32_t repeat_delay_ms, uint32_t repeat_ms);
uint32_t buttonGetRepeatEvent(uint8_t ch);
uint32_t buttonGetRepeatCount(uint8_t ch);

const char *buttonGetName(uint8_t ch);

#endif

#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_BUTTON_H_ */