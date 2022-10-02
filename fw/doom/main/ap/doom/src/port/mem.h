/*
 * mem.h
 *
 *  Created on: 2019. 11. 2.
 *      Author: Baram
 */

#ifndef SRC_HW_CORE_MEM_H_
#define SRC_HW_CORE_MEM_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"




void  memInit(uint32_t addr, uint32_t length);
void *memMalloc(uint32_t size);
void  memFree(void *ptr);
void *memCalloc(size_t nmemb, size_t size);
void *memRealloc(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif



#endif /* SRC_HW_CORE_MEM_H_ */
