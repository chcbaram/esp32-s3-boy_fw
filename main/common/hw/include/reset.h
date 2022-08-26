/*
 * reset.h
 *
 *  Created on: 2020. 12. 9.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_RESET_H_
#define SRC_COMMON_HW_INCLUDE_RESET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_RESET


bool resetInit(void);

uint32_t resetGetCount(void);


#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_RESET_H_ */
