/*
 * button.h
 *
 *  Created on: 2020. 12. 23.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_BUTTON_H_
#define SRC_COMMON_HW_INCLUDE_BUTTON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_BUTTON

#define BUTTON_MAX_CH         HW_BUTTON_MAX_CH


bool buttonInit(void);
bool buttonGetPressed(uint8_t ch);


#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_BUTTON_H_ */
