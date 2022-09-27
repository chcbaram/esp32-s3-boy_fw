/*
 * battery.c
 *
 *  Created on: 2022. 9. 18.
 *      Author: baram
 */


#include "battery.h"
#include "adc.h"
#include "cli.h"


#ifdef _USE_HW_BATTERY

#define BAT_ADC_MAX_COUNT     10

#define lock()      xSemaphoreTake(mutex_lock, portMAX_DELAY);
#define unLock()    xSemaphoreGive(mutex_lock);


typedef struct
{
  int32_t percent;
  float   voltage;
  int32_t adc_raw;
} battery_info_t;


#ifdef _USE_HW_CLI
static void cliBattery(cli_args_t *args);
#endif
static void batteryThread(void *args);


static SemaphoreHandle_t mutex_lock;
static bool is_init = false;

static float bat_max = 4.1f;
static float bat_min = 3.1f;
static battery_info_t bat_info;
static uint32_t adc_data[BAT_ADC_MAX_COUNT];
static uint8_t  adc_ch = 0;
static uint16_t buf_index = 0;




bool batteryInit(void)
{
  bool ret = true;

  
  mutex_lock = xSemaphoreCreateMutex();

  for (int i=0; i<BAT_ADC_MAX_COUNT; i++)
  {
    adc_data[i] = adcRead12(adc_ch);
  }

  if (xTaskCreate(batteryThread, "batteryThread", _HW_DEF_RTOS_THREAD_MEM_BATTERY, NULL, _HW_DEF_RTOS_THREAD_PRI_BATTERY, NULL) != pdPASS)
  {
    logPrintf("[NG] batteryThread()\n");   
  }

  is_init = true;


#ifdef _USE_HW_CLI
  cliAdd("battery", cliBattery);
#endif

  return ret;
}

bool batteryIsInit(void)
{
  return is_init;
}

bool batteryIsCharging(void)
{
  return true;
}

int32_t batteryGetPercent(void)
{
  int32_t ret;

  lock();
  ret = bat_info.percent;
  unLock();

  return ret;
}

float batteryGetVoltage(void)
{
  float ret;

  lock();
  ret = bat_info.voltage;
  unLock();

  return ret;
}

void batteryThread(void *args)
{
  int32_t adc_value;
  float adc_vol;
  float bat_vol;
  uint8_t dif_cnt = 0;
  uint8_t state = 0;


  while(1) 
  {
    adc_data[buf_index] = adcRead12(adc_ch);
    buf_index = (buf_index + 1) % BAT_ADC_MAX_COUNT;

    int32_t sum = 0;
    int32_t percent;
   
    for (int i=0; i<BAT_ADC_MAX_COUNT; i++)
    {
      sum += adc_data[i];
    }
    adc_value = sum/BAT_ADC_MAX_COUNT;
    bat_vol = (float)adc_value * 3.3f * 2.0f / 4095.f; 
    adc_vol = constrain(bat_vol, bat_min, bat_max);

    percent = (int32_t)cmap(adc_vol, bat_min, bat_max, 0, 100);


    switch(state)
    {
      case 0:
        lock();
        bat_info.adc_raw = adc_value;
        bat_info.voltage = bat_vol;
        bat_info.percent = percent;
        unLock();
        state = 1;
        break;

      case 1:
        if (bat_info.percent != percent)
        {
          dif_cnt = 0;
          state = 2;
        }
        break;
      
      case 2:
        dif_cnt++;
        if (dif_cnt >= 100)
        {
          state = 3;
        }
        if (bat_info.percent == percent)
        {
          state = 1;
        }
        break;

      case 3:
        lock();
        bat_info.percent = percent;
        unLock();
        state = 1;
        break;
    }

    lock();
    bat_info.adc_raw = adc_value;
    bat_info.voltage = bat_vol;
    unLock();

    delay(10);
  }
}


#ifdef _USE_HW_CLI
void cliBattery(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    cliPrintf("battery init : %d\n", is_init);
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "show") == true)
  {
    while(cliKeepLoop())
    {
      cliPrintf("%03d%% %1.2fV %04d\n", batteryGetPercent(), batteryGetVoltage(), bat_info.adc_raw);
      delay(100);
    }
    
    ret = true;
  }


  if (ret != true)
  {
    cliPrintf("battery info\n");
    cliPrintf("battery show\n");
  }
}
#endif

#endif