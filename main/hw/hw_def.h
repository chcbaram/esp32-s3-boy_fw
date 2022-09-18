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



#define _HW_DEF_RTOS_THREAD_PRI_CLI           5
#define _HW_DEF_RTOS_THREAD_PRI_SD            5
#define _HW_DEF_RTOS_THREAD_PRI_BATTERY       5
#define _HW_DEF_RTOS_THREAD_PRI_BUTTON        5
#define _HW_DEF_RTOS_THREAD_PRI_I2S           5

#define _HW_DEF_RTOS_THREAD_MEM_CLI           (4*1024)
#define _HW_DEF_RTOS_THREAD_MEM_SD            (4*1024)
#define _HW_DEF_RTOS_THREAD_MEM_BATTERY       (1*1024)
#define _HW_DEF_RTOS_THREAD_MEM_BUTTON        (1*1024)
#define _HW_DEF_RTOS_THREAD_MEM_I2S           (4*1024)


#define _USE_HW_RTOS
#define _USE_HW_CDC
#define _USE_HW_SD
#define _USE_HW_FATFS
#define _USE_HW_NVS
#define _USE_HW_BATTERY
#define _USE_HW_I2S
#define _USE_HW_BUZZER



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

#define _USE_HW_PWM
#define      HW_PWM_MAX_CH          1

#define _USE_HW_ADC                 
#define      HW_ADC_MAX_CH          1

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       11

#define _USE_HW_MIXER
#define      HW_MIXER_MAX_CH        8
#define      HW_MIXER_MAX_BUF_LEN   (16*4*8)



#define _PIN_GPIO_LCD_BLK           2
#define _PIN_GPIO_LCD_DC            1
#define _PIN_GPIO_LCD_CS           -1
#define _PIN_GPIO_LCD_RST           3

#define _BTN_LEFT                   0   
#define _BTN_RIGHT                  1   
#define _BTN_UP                     2   
#define _BTN_DOWN                   3   
#define _BTN_A                      4   
#define _BTN_B                      5   
#define _BTN_X                      6   
#define _BTN_Y                      7   
#define _BTN_START                  8   
#define _BTN_SELECT                 9   
#define _BTN_HOME                   10   


#endif /* MAIN_HW_HW_DEF_H_ */
