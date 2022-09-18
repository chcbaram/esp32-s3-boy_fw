/*
 * adc.c
 *
 *  Created on: 2022. 9. 18.
 *      Author: baram
 */


#include "adc.h"
#include "cli.h"


#ifdef _USE_HW_ADC
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define lock()      xSemaphoreTake(mutex_lock, portMAX_DELAY);
#define unLock()    xSemaphoreGive(mutex_lock);


#ifdef _USE_HW_CLI
static void cliAdc(cli_args_t *args);
#endif

static bool adcCaliInit(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle);


static SemaphoreHandle_t mutex_lock;
static bool is_init = false;
static bool is_cali = false;
static adc_oneshot_unit_handle_t adc_h = NULL;
static adc_cali_handle_t adc_cali_h = NULL;





bool adcInit(void)
{
  esp_err_t err;

  adc_oneshot_unit_init_cfg_t init_config = {
    .unit_id = ADC_UNIT_1,
  };
  adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_12,
    .atten = ADC_ATTEN_DB_11,
  };  

  mutex_lock = xSemaphoreCreateMutex();

  err = adc_oneshot_new_unit(&init_config, &adc_h);
  logPrintf("[%s] adc_oneshot_new_unit()\n", err == ESP_OK ? "OK" : "NG");
  if (err != ESP_OK) return false;

  err = adc_oneshot_config_channel(adc_h, ADC_CHANNEL_0, &config);
  logPrintf("[%s] adc_oneshot_config_channel()\n", err == ESP_OK ? "OK" : "NG");
  if (err != ESP_OK) return false;

  is_cali = adcCaliInit(ADC_UNIT_1, ADC_ATTEN_DB_11, &adc_cali_h);


  is_init = true;

#ifdef _USE_HW_CLI
  cliAdd("adc", cliAdc);
#endif
  return true;
}

int32_t adcRead(uint8_t ch)
{
  esp_err_t err;
  int adc_raw = 0;
  int adc_vol = 0;
  int32_t adc_ret = 0;

  if (is_init == false) return 0;


  lock();
  err = adc_oneshot_read(adc_h, ADC_CHANNEL_0, &adc_raw);
  if (err == ESP_OK)
  {
    if (is_cali) 
    {
      adc_cali_raw_to_voltage(adc_cali_h, adc_raw, &adc_vol);      
      adc_raw = (adc_vol * 4095)/3300;
    }

    adc_ret = adc_raw;
  }
  unLock();

  return adc_ret;
}

int32_t adcRead8(uint8_t ch)
{
  return adcRead(ch)>>4;
}

int32_t adcRead10(uint8_t ch)
{
  return adcRead(ch)>>2;
}

int32_t adcRead12(uint8_t ch)
{
  return adcRead(ch)>>0;
}

int32_t adcRead16(uint8_t ch)
{
  return adcRead(ch)<<4;  
}

uint8_t adcGetRes(uint8_t ch)
{
  return 12;
}

bool adcCaliInit(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
  adc_cali_handle_t handle = NULL;
  esp_err_t ret = ESP_FAIL;
  bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
  if (!calibrated) 
  {
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
        calibrated = true;
    }
  }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

  *out_handle = handle;
  if (ret == ESP_OK) 
  {
    logPrintf("[OK] adc calibration\n");
  } 
  else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) 
  {
    logPrintf("[__] eFuse not burnt, skip software calibration\n");
  } 
  else 
  {
    logPrintf("[NG] Invalid arg or no memory");
  }

  return calibrated;
}




#ifdef _USE_HW_CLI
void cliAdc(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    cliPrintf("adc init : %d\n", is_init);
    cliPrintf("adc res  : %d\n", adcGetRes(0));
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "show") == true)
  {
    while(cliKeepLoop())
    {
      for (int i=0; i<ADC_MAX_CH; i++)
      {
        cliPrintf("%04d ", adcRead(i));
      }
      cliPrintf("\n");
      delay(50);
    }
    
    ret = true;
  }


  if (ret != true)
  {
    cliPrintf("adc info\n");
    cliPrintf("adc show\n");
  }
}
#endif

#endif