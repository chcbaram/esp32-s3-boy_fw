#include "sd.h"
#include "cli.h"


#ifdef _USE_HW_SD
#include "sdmmc_cmd.h"
#include "driver/gpio.h"
#include "driver/sdmmc_types.h"
#include "driver/sdspi_host.h"
#include "driver/sdmmc_host.h"

#ifdef _USE_HW_CLI
static void cliSd(cli_args_t *args);
#endif

#define lock()      xSemaphoreTake(mutex_lock, portMAX_DELAY);
#define unLock()    xSemaphoreGive(mutex_lock);

#define SD_EVENT_FUNC_MAX    8


static void sdThread(void *args);


static bool is_init = false;
static bool is_init_slot = false;

static SemaphoreHandle_t mutex_lock;
static sdmmc_card_t card;
static sdmmc_host_t host = SDMMC_HOST_DEFAULT();
static sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
static sd_state_t sd_state = SDCARD_IDLE;
static uint8_t sd_event_index = 0;
static void (*sd_event_func[SD_EVENT_FUNC_MAX])(sd_event_t);




bool sdInit(void)
{
  esp_err_t err;

  mutex_lock = xSemaphoreCreateMutex();

  slot_config.width = 1;
  slot_config.cd  = GPIO_NUM_17;
  slot_config.clk = GPIO_NUM_8;
  slot_config.cmd = GPIO_NUM_9;
  slot_config.d0  = GPIO_NUM_18;


  err = host.init();
  logPrintf("[%s] sdmmc_host_init()\n", err == ESP_OK ? "OK":"NG");
  if (err != ESP_OK) return false;

  logPrintf("[%s] sdIsDetected()\n", sdIsDetected()== true ? "OK":"NG");
  if (sdIsDetected()== true)
  {
    err = sdmmc_host_init_slot(host.slot, &slot_config);
    logPrintf("[%s] sdmmc_host_init_slot()\n", err == ESP_OK ? "OK":"NG");
    if (err == ESP_OK)
    {
      is_init_slot = true;
    }
  }

  if (xTaskCreate(sdThread, "sdThread", _HW_DEF_RTOS_THREAD_MEM_SD, NULL, _HW_DEF_RTOS_THREAD_PRI_SD, NULL) != pdPASS)
  {
    logPrintf("[NG] sdThread()\n");   
  }

  is_init = false;


#ifdef _USE_HW_CLI
  cliAdd("sd", cliSd);
#endif

  return true;
}

bool sdAddEventFunc(void (*p_func)(sd_event_t))
{
  if (sd_event_index >= SD_EVENT_FUNC_MAX) return false;


  sd_event_func[sd_event_index] = p_func;
  sd_event_index++;

  return true;
}

bool sdSendEvent(sd_event_t *p_sd_event)
{
 
  for (int i=0; i<sd_event_index; i++)
  {
    sd_event_func[i](*p_sd_event);
  }

  return true;
}

void sdThread(void *args)
{
  uint32_t pre_time;
  uint8_t err_cnt = 0;
  sd_event_t sd_event;
  sd_state_t pre_state;


  pre_time = millis();
  while(1) 
  {
    pre_state = sd_state;

    switch(sd_state)
    {
      case SDCARD_IDLE:
        if (sdIsDetected() == true)
        {
          sd_state = SDCARD_CONNECTTING;
          logPrintf("[__] SDCARD_CONNECTING\n");
          pre_time = millis();
        }
        else
        {
          sd_state = SDCARD_DISCONNECTED;
          logPrintf("[__] SDCARD_DISCONNECTED\n");
          err_cnt = 0;
        }
        break;

      case SDCARD_CONNECTTING:
        if (millis()-pre_time >= 500)
        {
          is_init = sdReInit();
          if (is_init == true)
          {
            sd_state = SDCARD_CONNECTED;
            logPrintf("[__] SDCARD_CONNECTED\n");
          }
          else
          {
            sd_state = SDCARD_ERROR;
            logPrintf("[__] SDCARD_ERROR\n");        
            pre_time = millis();  
          }
        }
        if (sdIsDetected() == false)
        {
          sd_state = SDCARD_IDLE;
        }
        break;

      case SDCARD_CONNECTED:
        if (sdIsDetected() == false)
        {
          sd_state = SDCARD_IDLE;
        }      
        break;

      case SDCARD_DISCONNECTED:
        if (sdIsDetected() == true)
        {
          sd_state = SDCARD_IDLE;
        }
        break;

      case SDCARD_ERROR:
        if (millis()-pre_time >= 1000)
        {
          pre_time = millis();
          if (sdIsDetected() == true && err_cnt < 3)
          {
            host.deinit();
            host.init();
            sdmmc_host_init_slot(host.slot, &slot_config);
            sd_state = SDCARD_IDLE;
          }
          err_cnt++;
        }
        break;
    }

    if (pre_state != sd_state)
    {
      sd_event.sd_state = sd_state;
      sd_event.sd_arg = &card;
      sdSendEvent(&sd_event);
    }

    delay(10);
  }
}

const char *sdGetStateMsg(void)
{
  const char *ret[] = {"SDCARD_IDLE", 
                       "SDCARD_CONNECTTING",
                       "SDCARD_CONNECTED",
                       "SDCARD_DISCONNECTED",
                       "SDCARD_ERROR"};



  return ret[sd_state];
}

bool sdReInit(void)
{
  bool ret = false;
  esp_err_t err;


  if (is_init_slot == false)
  {
    err = sdmmc_host_init_slot(host.slot, &slot_config);
    logPrintf("[%s] sdmmc_host_init_slot()\n", err == ESP_OK ? "OK":"NG");
    if (err == ESP_OK)
    {
      is_init_slot = true;
    }
    else
    {
      is_init = false;
      return false;
    }
  }

  err = sdmmc_card_init(&host, &card);
  logPrintf("[%s] sdmmc_card_init()\n", err == ESP_OK ? "OK":"NG");
  if (err == ESP_OK)
  {
    ret = true;
  }

  is_init = ret;

  return ret;
}

bool sdIsInit(void)
{
  return is_init;
}

bool sdIsDetected(void)
{
  return gpio_get_level(GPIO_NUM_17) == 0 ? true : false;
}

sd_state_t sdGetState(void)
{
  return sd_state;
}



#ifdef _USE_HW_CLI
void cliSd(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    cliPrintf("sd init      : %d\n", is_init);
    cliPrintf("sd connected : %d\n", sdIsDetected());
    cliPrintf("sd host freq : %d KHz\n", host.max_freq_khz);
    cliPrintf("sd card freq : %d KHz\n", card.max_freq_khz);

    ret = true;
  }



  if (ret != true)
  {
    cliPrintf("sd info\n");
  }
}
#endif

#endif