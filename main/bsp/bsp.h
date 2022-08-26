/*
 * bsp.h
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */

#ifndef MAIN_BSP_BSP_H_
#define MAIN_BSP_BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "spi_flash_mmap.h"
#include "esp_task_wdt.h"
#include "esp_chip_info.h"
#include "soc/rtc.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_psram.h"
#include "esp_flash.h"

#if 0
#define _USE_LOG_PRINT    1

#if _USE_LOG_PRINT
#define logPrintf(fmt, ...)     printf(fmt, ##__VA_ARGS__)
#else
#define logPrintf(fmt, ...)
#endif
#endif

void logPrintf(const char *fmt, ...);


void bspInit(void);
uint32_t bspGetCpuFreqMhz(void);

void delay(uint32_t ms);
uint32_t millis(void);
uint32_t micros(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_BSP_BSP_H_ */
