/*
 * gpio.c
 *
 *  Created on: 2022. 9. 2.
 *      Author: baram
 */


#include "gpio.h"
#include "cli.h"


#ifdef _USE_HW_GPIO
#include "driver/gpio.h"



typedef struct
{
  uint32_t      pin;
  uint8_t       mode;
  bool          init_value;
} gpio_tbl_t;


static const gpio_tbl_t gpio_tbl[GPIO_MAX_CH] =
    {
      {GPIO_NUM_15, _DEF_OUTPUT, _DEF_HIGH}, // 0. LCD CS      
      {GPIO_NUM_16, _DEF_OUTPUT, _DEF_LOW }, // 1. LCD DC
      {GPIO_NUM_4 , _DEF_OUTPUT, _DEF_LOW }, // 2. LCD BLK      
      {GPIO_NUM_5 , _DEF_OUTPUT, _DEF_HIGH}, // 3. LCD RST
      {GPIO_NUM_38, _DEF_OUTPUT, _DEF_LOW }, // 4. SPK MUTE
      {GPIO_NUM_17, _DEF_INPUT , _DEF_HIGH}, // 5. SDCARD CD
    };

static uint8_t gpio_data[GPIO_MAX_CH];


#ifdef _USE_HW_CLI
static void cliGpio(cli_args_t *args);
#endif



bool gpioInit(void)
{
  bool ret = true;


  for (int i=0; i<GPIO_MAX_CH; i++)
  {
    gpioPinMode(i, gpio_tbl[i].mode);
    gpioPinWrite(i, gpio_tbl[i].init_value);
  }

#ifdef _USE_HW_CLI
  cliAdd("gpio", cliGpio);
#endif

  return ret;
}

bool gpioPinMode(uint8_t ch, uint8_t mode)
{
  bool ret = true;


  if (ch >= GPIO_MAX_CH)
  {
    return false;
  }

  gpio_reset_pin(gpio_tbl[ch].pin);
  
  switch(mode)
  {
    case _DEF_INPUT:
      gpio_pullup_dis(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_INPUT);
      break;

    case _DEF_INPUT_PULLUP:
      gpio_pullup_en(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_INPUT);
      break;

    case _DEF_INPUT_PULLDOWN:
      gpio_pulldown_en(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_INPUT);
      break;

    case _DEF_OUTPUT:
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_OUTPUT);
      break;

    case _DEF_OUTPUT_PULLUP:
      gpio_pullup_en(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_OUTPUT);
      break;

    case _DEF_OUTPUT_PULLDOWN:
      gpio_pulldown_en(gpio_tbl[ch].pin);
      gpio_set_direction(gpio_tbl[ch].pin, GPIO_MODE_OUTPUT);
      break;
  }

  return ret;
}

void gpioPinWrite(uint8_t ch, uint8_t value)
{
  if (ch >= GPIO_MAX_CH)
  {
    return;
  }

  gpio_data[ch] = value;
  gpio_set_level(gpio_tbl[ch].pin, value);
}

uint8_t gpioPinRead(uint8_t ch)
{
  uint8_t ret;

  if (ch >= GPIO_MAX_CH)
  {
    return false;
  }

  ret = gpio_get_level(gpio_tbl[ch].pin);
  return ret;
}

void gpioPinToggle(uint8_t ch)
{
  if (ch >= GPIO_MAX_CH)
  {
    return;
  }

  gpio_data[ch] = !gpio_data[ch];
  gpio_set_level(gpio_tbl[ch].pin, gpio_data[ch]);
}





#ifdef _USE_HW_CLI
void cliGpio(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "show") == true)
  {
    while(cliKeepLoop())
    {
      for (int i=0; i<GPIO_MAX_CH; i++)
      {
        cliPrintf("%d", gpioPinRead(i));
      }
      cliPrintf("\n");
      delay(100);
    }
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "read") == true)
  {
    uint8_t ch;

    ch = (uint8_t)args->getData(1);

    while(cliKeepLoop())
    {
      cliPrintf("gpio read %d : %d\n", ch, gpioPinRead(ch));
      delay(100);
    }

    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "write") == true)
  {
    uint8_t ch;
    uint8_t data;

    ch   = (uint8_t)args->getData(1);
    data = (uint8_t)args->getData(2);

    gpioPinWrite(ch, data);

    cliPrintf("gpio write %d : %d\n", ch, data);
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("gpio show\n");
    cliPrintf("gpio read ch[0~%d]\n", GPIO_MAX_CH-1);
    cliPrintf("gpio write ch[0~%d] 0:1\n", GPIO_MAX_CH-1);
  }
}
#endif


#endif