/*
 * nvs.h
 *
 *  Created on: 2022. 9. 17.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_NVS_H_
#define SRC_COMMON_HW_INCLUDE_NVS_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_NVS


bool nvsInit(void);
bool nvsIsInit(void);


#endif


#ifdef __cplusplus
}
#endif

#endif 