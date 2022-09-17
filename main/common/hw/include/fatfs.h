/*
 * fatfs.h
 *
 *  Created on: 2022. 9. 17.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_FATFS_H_
#define SRC_COMMON_HW_INCLUDE_FATFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_FATFS


bool fatfsInit(void);
bool fatfsIsInit(void);
bool fatfsIsMounted(void);


#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_FATFS_H_ */
