/*
 * hw.c
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */




#include "hw.h"


static void bootMsg(void);




bool hwInit(void)
{
  bspInit();


  cliInit();
  logInit();

  ledInit();
  gpioInit();
  pwmInit();
  cdcInit();
  uartInit();
  uartOpen(_DEF_UART1, 115200);
  uartOpen(_DEF_UART2, 115200);

  logOpen(_DEF_UART1, 115200);

  bootMsg();
  logPrintf("\r\n[ Firmware Begin... ]\r\n");
  logPrintf("Booting..Name \t\t: %s\r\n", _DEF_BOARD_NAME);
  logPrintf("Booting..Ver  \t\t: %s\r\n", _DEF_FIRMWATRE_VERSION);  
  logPrintf("\n");

  nvsInit();
  adcInit();
  buttonInit();

  batteryInit();
  i2sInit();
  audioInit();
  buzzerInit();
  
  lcdInit();
  sdInit();
  fatfsInit();
  ymodemInit();


  esp_partition_iterator_t it;

  it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, "launcher");
  if (it != NULL)
  {
    const esp_partition_t *part = esp_partition_get(it);

    if (part != esp_ota_get_running_partition())
    {
      esp_ota_set_boot_partition(part);
    }
  }

  return true;
}

void bootMsg(void)
{
  logPrintf("\r\n[ ESP32-S3 Info ]\r\n");
  logPrintf("ESP32-S3-BOY !\n");

  /* Print chip information */
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  logPrintf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
          CONFIG_IDF_TARGET,
          chip_info.cores,
          (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
          (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

  logPrintf("silicon revision %d\n", chip_info.revision);

  uint32_t flash_size = 0;
  esp_flash_get_size(NULL, &flash_size);
  logPrintf("SPI FLASH : %uMB %s flash\n", flash_size / (1024 * 1024),
          (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");


  logPrintf("SPI PSRAM : %dMB\n", esp_psram_get_size()/1024/1024);

  logPrintf("Free heap : %ld KB\n", esp_get_free_heap_size()/1024);
  logPrintf("Free Heapi: %d KB\n", esp_get_free_internal_heap_size()/1024);
  logPrintf("CPU Freq  : %lu Mhz\n", bspGetCpuFreqMhz());
}
