/*
 * button.c
 *
 *  Created on: 2022. 9. 18.
 *      Author: Baram
 */




#include "button.h"
#include "cli.h"



#ifdef _USE_HW_BUTTON

typedef struct
{
  uint32_t      pin;
  uint8_t       on_state;
  uint8_t       off_state;
} button_tbl_t;




button_tbl_t button_port_tbl[BUTTON_MAX_CH] =
{
  {GPIO_NUM_47, _DEF_LOW, _DEF_HIGH}, // 0. BTN_LEFT
  {GPIO_NUM_2 , _DEF_LOW, _DEF_HIGH}, // 1. BTN_RIGHT
  {GPIO_NUM_48, _DEF_LOW, _DEF_HIGH}, // 2. BTN_UP
  {GPIO_NUM_42, _DEF_LOW, _DEF_HIGH}, // 3. BTN_DOWN  
  {GPIO_NUM_13, _DEF_LOW, _DEF_HIGH}, // 4. BTN_A
  {GPIO_NUM_11, _DEF_LOW, _DEF_HIGH}, // 5. BTN_B
  {GPIO_NUM_14, _DEF_LOW, _DEF_HIGH}, // 6. BTN_X
  {GPIO_NUM_12, _DEF_LOW, _DEF_HIGH}, // 7. BTN_Y
  {GPIO_NUM_45, _DEF_LOW, _DEF_HIGH}, // 8. BTN_START
  {GPIO_NUM_21, _DEF_LOW, _DEF_HIGH}, // 9. BTN_SELECT
  {GPIO_NUM_10, _DEF_LOW, _DEF_HIGH}, // 10.BTN_HOME
};

static const char *button_name[BUTTON_MAX_CH] = 
{
  "_BTN_LEFT",   
  "_BTN_RIGHT",
  "_BTN_UP",
  "_BTN_DOWN",
  "_BTN_A",
  "_BTN_B",
  "_BTN_X",    
  "_BTN_Y",
  "_BTN_START",
  "_BTN_SELECT",
  "_BTN_HOME",
};

typedef struct
{
  bool        pressed;
  bool        pressed_event;
  uint16_t    pressed_cnt;
  uint32_t    pressed_start_time;
  uint32_t    pressed_end_time;

  bool        released;
  bool        released_event;
  uint32_t    released_start_time;
  uint32_t    released_end_time;

  bool        repeat_update;
  uint32_t    repeat_cnt;
  uint32_t    repeat_time_detect;
  uint32_t    repeat_time_delay;
  uint32_t    repeat_time;
} button_t;


#ifdef _USE_HW_CLI
static void cliButton(cli_args_t *args);
#endif
static void buttonThread(void *args);

static bool is_enable = true;
static button_t button_tbl[BUTTON_MAX_CH];






bool buttonInit(void)
{
  uint32_t i;


  for (i=0; i<BUTTON_MAX_CH; i++)
  {
    gpio_pullup_en(button_port_tbl[i].pin);
    gpio_pulldown_dis(button_port_tbl[i].pin);
    gpio_set_direction(button_port_tbl[i].pin, GPIO_MODE_INPUT);   
  }

  for (i=0; i<BUTTON_MAX_CH; i++)
  {
    button_tbl[i].pressed_cnt    = 0;
    button_tbl[i].pressed        = 0;
    button_tbl[i].released       = 0;
    button_tbl[i].released_event = 0;

    button_tbl[i].repeat_cnt     = 0;
    button_tbl[i].repeat_time_detect = 60;
    button_tbl[i].repeat_time_delay  = 250;
    button_tbl[i].repeat_time        = 200;

    button_tbl[i].repeat_update = false;
  }


  if (xTaskCreate(buttonThread, "buttonThread", _HW_DEF_RTOS_THREAD_MEM_BUTTON, NULL, _HW_DEF_RTOS_THREAD_PRI_BUTTON, NULL) != pdPASS)
  {
    logPrintf("[NG] buttonThread()\n");   
  }

#ifdef _USE_HW_CLI
  cliAdd("button", cliButton);
#endif

  return true;
}

void buttonClear(void)
{
  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    button_tbl[i].pressed_start_time    = 0;
    button_tbl[i].pressed_end_time      = 0;
    button_tbl[i].released_start_time   = 0;
    button_tbl[i].released_end_time     = 0;

    button_tbl[i].pressed_event = false;
    button_tbl[i].released_event = false;
  }
}

void buttonThread(void *args)
{
  uint8_t i;
  uint32_t repeat_time;


  while(1)
  {
    for (i=0; i<BUTTON_MAX_CH; i++)
    {
      if (buttonGetPin(i) == true)
      {
        if (button_tbl[i].pressed == false)
        {
          button_tbl[i].pressed_event = true;
          button_tbl[i].pressed_start_time = millis();
        }

        button_tbl[i].pressed = true;
        button_tbl[i].pressed_cnt++;

        if (button_tbl[i].repeat_cnt == 0)
        {
          repeat_time = button_tbl[i].repeat_time_detect;
        }
        else if (button_tbl[i].repeat_cnt == 1)
        {
          repeat_time = button_tbl[i].repeat_time_delay;
        }
        else
        {
          repeat_time = button_tbl[i].repeat_time;
        }
        if (button_tbl[i].pressed_cnt >= repeat_time)
        {
          button_tbl[i].pressed_cnt = 0;
          button_tbl[i].repeat_cnt++;
          button_tbl[i].repeat_update = true;
        }

        button_tbl[i].pressed_end_time = millis();

        button_tbl[i].released = false;
      }
      else
      {
        if (button_tbl[i].pressed == true)
        {
          button_tbl[i].released_event = true;
          button_tbl[i].released_start_time = millis();
        }

        button_tbl[i].pressed  = false;
        button_tbl[i].released = true;
        button_tbl[i].repeat_cnt = 0;
        button_tbl[i].repeat_update = false;

        button_tbl[i].released_end_time = millis();
      }
    }
    delay(10);
  }
}

const char *buttonGetName(uint8_t ch)
{
  ch = constrain(ch, 0, BUTTON_MAX_CH-1);

  return button_name[ch];
}

bool buttonGetPin(uint8_t ch)
{
  if (ch >= BUTTON_MAX_CH)
  {
    return false;
  }

  if (gpio_get_level(button_port_tbl[ch].pin) == button_port_tbl[ch].on_state)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void buttonEnable(bool enable)
{
  is_enable = enable;
}

bool buttonGetPressed(uint8_t ch)
{
  if (ch >= BUTTON_MAX_CH || is_enable == false)
  {
    return false;
  }

  return button_tbl[ch].pressed;
}

uint8_t  buttonGetPressedCount(void)
{
  uint32_t i;
  uint8_t ret = 0;

  for (i=0; i<BUTTON_MAX_CH; i++)
  {
    if (buttonGetPressed(i) == true)
    {
      ret++;
    }
  }

  return ret;
}

void buttonSetRepeatTime(uint8_t ch, uint32_t detect_ms, uint32_t repeat_delay_ms, uint32_t repeat_ms)
{
  if (ch >= BUTTON_MAX_CH || is_enable == false) return;

  button_tbl[ch].repeat_update = false;
  button_tbl[ch].repeat_cnt = 0;
  button_tbl[ch].pressed_cnt = 0;

  button_tbl[ch].repeat_time_detect = detect_ms;
  button_tbl[ch].repeat_time_delay  = repeat_delay_ms;
  button_tbl[ch].repeat_time        = repeat_ms;
}

uint32_t buttonGetRepeatEvent(uint8_t ch)
{
  volatile uint32_t ret = 0;

  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;

  if (button_tbl[ch].repeat_update)
  {
    button_tbl[ch].repeat_update = false;
    ret = button_tbl[ch].repeat_cnt;
  }

  return ret;
}

uint32_t buttonGetRepeatCount(uint8_t ch)
{
  volatile uint32_t ret = 0;

  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;

  ret = button_tbl[ch].repeat_cnt;

  return ret;
}

bool buttonGetPressedEvent(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].pressed_event;

  button_tbl[ch].pressed_event = 0;

  return ret;
}

uint32_t buttonGetPressedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;


  ret = button_tbl[ch].pressed_end_time - button_tbl[ch].pressed_start_time;

  return ret;
}


bool buttonGetReleased(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].released;

  return ret;
}

bool buttonGetReleasedEvent(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].released_event;

  button_tbl[ch].released_event = 0;

  return ret;
}

uint32_t buttonGetReleasedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;


  ret = button_tbl[ch].released_end_time - button_tbl[ch].released_start_time;

  return ret;
}



#ifdef _USE_HW_CLI
void cliButton(cli_args_t *args)
{
  bool ret = false;
  uint8_t ch;
  uint32_t i;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    for (i=0; i<BUTTON_MAX_CH; i++)
    {
      cliPrintf("%-12s pin %d\n", buttonGetName(i), button_port_tbl[i].pin);
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "show"))
  {
    while(cliKeepLoop())
    {
      for (i=0; i<BUTTON_MAX_CH; i++)
      {
        cliPrintf("%d", buttonGetPressed(i));
      }
      delay(50);
      cliPrintf("\r");
    }
    ret = true;
  }
  
  if (args->argc == 1 && args->isStr(0, "time"))
  {
    ch = (uint8_t)args->getData(1);
    ch = constrain(ch, 0, BUTTON_MAX_CH-1);

    while(cliKeepLoop())
    {
      for (int i=0; i<BUTTON_MAX_CH; i++)
      {
        if(buttonGetPressed(i))
        {
          cliPrintf("%-12s, Time :  %d ms\n", buttonGetName(i), buttonGetPressedTime(i));
        }
      }
      delay(10);
    }
    ret = true;
  }


  if (ret == false)
  {
    cliPrintf("button info\n");
    cliPrintf("button show\n");
    cliPrintf("button time\n", BUTTON_MAX_CH);
  }
}
#endif

#endif