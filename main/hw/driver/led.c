/*
 * led.c
 *
 *  Created on: 2021. 6. 14.
 *      Author: baram
 */


#include "led.h"

#ifdef _USE_HW_LED
#include "driver/gpio.h"


typedef struct
{
  uint32_t    pin;
  uint8_t     on_state;
  uint8_t     off_state;
  uint8_t     data;
} led_tbl_t;



static led_tbl_t led_tbl[LED_MAX_CH] =
    {
        {GPIO_NUM_46, _DEF_LOW, _DEF_HIGH, 0},
    };





bool ledInit(void)
{

  for (int i=0; i<LED_MAX_CH; i++)
  {
    gpio_reset_pin(led_tbl[i].pin);
    gpio_set_direction(led_tbl[i].pin, GPIO_MODE_OUTPUT);
    ledOff(i);
  }

  return true;
}

void ledOn(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;

  led_tbl[ch].data = led_tbl[ch].on_state;
  gpio_set_level(led_tbl[ch].pin, led_tbl[ch].data);
}

void ledOff(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;

  led_tbl[ch].data = led_tbl[ch].off_state;
  gpio_set_level(led_tbl[ch].pin, led_tbl[ch].data);
}

void ledToggle(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;

  led_tbl[ch].data = !led_tbl[ch].data;
  gpio_set_level(led_tbl[ch].pin, led_tbl[ch].data);
}

#endif