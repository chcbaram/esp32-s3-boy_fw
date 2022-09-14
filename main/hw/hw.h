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
#include "files.h"


bool hwInit(void);


#ifdef __cplusplus
}
#endif

#endif /* MAIN_HW_HW_H_ */
