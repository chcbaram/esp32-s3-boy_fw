/*
 * hw.h
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */

#ifndef MAIN_HW_HW_H_
#define MAIN_HW_HW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#include "led.h"
#include "uart.h"
#include "gpio.h"
#include "cli.h"
#include "log.h"
#include "cdc.h"
#include "lcd.h"
#include "sd.h"
#include "fatfs.h"
#include "pwm.h"
#include "nvs.h"
#include "adc.h"
#include "battery.h"
#include "button.h"
#include "mixer.h"
#include "i2s.h"


bool hwInit(void);


#ifdef __cplusplus
}
#endif

#endif /* MAIN_HW_HW_H_ */
