#include "nvs.h"


#ifdef _USE_HW_NVS
#include "nvs_flash.h"
#include "nvs.h"



static bool is_init = false;



bool nvsInit(void)
{
  bool ret = true;
  esp_err_t err;
  
  err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) 
  {
    logPrintf("[__] nvs_flash_erase()\n");

    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }

  if (err == ESP_OK)
  {
    logPrintf("[OK] nvs_flash_init()\n");
  }
  else
  {
    ret = false;
    logPrintf("[NG] nvs_flash_init()\n");
  }

  is_init = ret;
  
  return ret;
}

bool nvsIsInit(void)
{
  return is_init;
}

#endif