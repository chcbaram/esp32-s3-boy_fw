#include "ota.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"



static void cliOta(cli_args_t *args);


static bool is_init = false;
static bool is_busy = false;

static const esp_partition_t *ota_part_tbl[16];
static uint8_t ota_part_count = 0;





void otaInit(void)
{
  ota_part_count = 0;

  for (int i=0; i<16; i++)
  {
    ota_part_tbl[i] = esp_partition_find_first(ESP_PARTITION_TYPE_APP, (esp_partition_subtype_t)((int)ESP_PARTITION_SUBTYPE_APP_OTA_MIN + i), NULL);
    if (ota_part_tbl[i] != NULL)
    {
      ota_part_count = i + 1;
    }
  }

  is_init = true;
  cliAdd("ota", cliOta);
}

bool otaIsBusy(void)
{
  return is_busy;
}

void cliOta(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    esp_app_desc_t app_desc;

    cliPrintf("ota part cnt : %d\n", ota_part_count);

    cliPrintf("Num |Part   |Project         |Address    |Size |\n");
    cliPrintf("------------------------------------------------\n");
    
    for (int i=0; i<ota_part_count; i++)
    {
      if (esp_ota_get_partition_description(ota_part_tbl[i], &app_desc) != ESP_OK)
      {
        app_desc.project_name[0] = 0;
      }      
      cliPrintf(" %d   %-7s %-16s 0x%06X %7dKB\n", i, ota_part_tbl[i]->label, app_desc.project_name, ota_part_tbl[i]->address, ota_part_tbl[i]->size/1024);
    }

    ret = true;
  }


  if (ret == false)
  {
    cliPrintf("ota info\n");
  }
}
