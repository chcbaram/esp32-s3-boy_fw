/*
 * hw_def.h
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */

#ifndef MAIN_HW_HW_DEF_H_
#define MAIN_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"



#define _DEF_FIRMWATRE_VERSION    "V220917R1"
#define _DEF_BOARD_NAME           "ESP32-S3-BOY_FW"



#define _HW_DEF_RTOS_THREAD_PRI_SD            5

#define _HW_DEF_RTOS_THREAD_MEM_SD            (4*1024)



#define _USE_HW_RTOS
#define _USE_HW_CDC
#define _USE_HW_FATFS


#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         2

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    16
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    4
#define      HW_CLI_LINE_BUF_MAX    64

#define _USE_HW_LOG
#define      HW_LOG_CH              _DEF_UART1
#define      HW_LOG_BOOT_BUF_MAX    1024
#define      HW_LOG_LIST_BUF_MAX    1024

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         6

#define _USE_HW_LCD
#define      HW_LCD_SWAP_RGB        1
#define      HW_LCD_LVGL            1
#define      HW_LCD_LOGO            1
#define _USE_HW_ST7789
#define      HW_LCD_WIDTH           240
#define      HW_LCD_HEIGHT          240

#define _USE_HW_SD



#define _PIN_GPIO_LCD_BLK           2
#define _PIN_GPIO_LCD_DC            1
#define _PIN_GPIO_LCD_CS           -1
#define _PIN_GPIO_LCD_RST           3

#endif /* MAIN_HW_HW_DEF_H_ */
