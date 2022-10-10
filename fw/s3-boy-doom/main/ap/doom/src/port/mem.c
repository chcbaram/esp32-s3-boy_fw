/*
 * mem.c
 *
 *  Created on: 2019. 11. 2.
 *      Author: Baram
 */




#include "mem.h"





//-- Internal Variables
//


//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//



void memInit(uint32_t addr, uint32_t length)
{
}

void *memMalloc(uint32_t size)
{
  void *ret;
  
  ret = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_32BIT);
  
  return ret;
}

void memFree(void *ptr)
{
  heap_caps_free(ptr);
}

void *memCalloc(size_t nmemb, size_t size)
{
  void *ret;
  
  ret = heap_caps_calloc(size, 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_32BIT);
  
  return ret;
}

void *memRealloc(void *ptr, size_t size)
{
  void *ret;
  
  ret = heap_caps_realloc(ptr, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_32BIT);
  
  return ret;
}

