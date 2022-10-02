/*
 * rtc.h
 *
 *  Created on: 2020. 12. 9.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_RTC_H_
#define SRC_COMMON_HW_INCLUDE_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_RTC


bool rtcInit(void);

uint32_t rtcBackupRegRead(uint32_t index);
void     rtcBackupRegWrite(uint32_t index, uint32_t data);


#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_RTC_H_ */
