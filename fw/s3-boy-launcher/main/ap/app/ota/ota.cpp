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

bool otaRunByName(const char *p_name)
{
  bool ret = false;

  esp_app_desc_t app_desc;

  
  for (int i=0; i<ota_part_count; i++)
  {
    if (esp_ota_get_partition_description(ota_part_tbl[i], &app_desc) != ESP_OK)
    {
      app_desc.project_name[0] = 0;
    }      
    if (strcmp(app_desc.project_name, p_name) == 0)
    {
      lcdClear(black);
      esp_ota_set_boot_partition(ota_part_tbl[i]);
      esp_restart();
    }
  }

  return ret;
}

void cliOta(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    esp_app_desc_t app_desc;


    cliRunStr("launcher disable\n");
    cliPrintf("\n");

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

    cliRunStr("launcher enable\n");
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "ymodem"))
  {
    uint8_t ota_index;
    bool is_done = false;
    bool is_started = false;
    bool is_error = false;
    esp_err_t esp_ret;
    ymodem_t ymodem;
    esp_ota_handle_t handle = 0;
    

    ota_index = args->getData(1);
    cliRunStr("launcher disable\n");


    ymodemOpen(&ymodem, cliGetPort());


    buttonClear();
    while(buttonGetPressedEvent(_BTN_HOME) == false)
    {
      if (lcdDrawAvailable())
      {
        lcdClearBuffer(black);

        lcdPrintfRect(0, 0, LCD_WIDTH, 32, white, 32, LCD_ALIGN_H_CENTER|LCD_ALIGN_V_CENTER, 
          "YMODEM");

        if (is_started == true)
        {
          lcdPrintf(0, 16*3, white, "START..\n");
          lcdPrintf(0, 16*4, white, "File : %s\n", ymodem.file_name);
          lcdPrintf(0, 16*5, white, "Size : %dKB\n", ymodem.file_length/1024);

          int percent;
          percent = ymodem.file_received*100 / ymodem.file_length;

          lcdPrintf(0, 16*7, white, "%d%%", percent);
          lcdDrawRect(0, 16*8, LCD_WIDTH, 32, white);
          lcdDrawFillRect(0, 16*8, LCD_WIDTH * percent / 100, 32, green);     

          if (is_done == true)
          {
            switch(ymodem.type)
            {
              case YMODEM_TYPE_END:
                lcdPrintf(0, 16*10, white, "DONE - OK");
                break;

              case YMODEM_TYPE_CANCEL:
                lcdPrintf(0, 16*10, white, "DONE - CANCEL");
                break;

              case YMODEM_TYPE_ERROR:
                lcdPrintf(0, 16*10, white, "DONE - ERROR");
                break;

              default:
                break;
            }
          }     
        }
        else
        {
          if (is_error == false)
          {
            lcdPrintf(0, 16*3, white, "WAIT...\n");
          }
          else
          {
            lcdPrintf(0, 16*3, white, "ERROR...\n");
          }
        }


        lcdRequestDraw();    
      }

      if (ymodemReceive(&ymodem) == true)
      {
        switch(ymodem.type)
        {
          case YMODEM_TYPE_START:
            is_started = true;
            is_done = false;
            is_error = false;

            esp_ret = esp_ota_begin(ota_part_tbl[ota_index], ymodem.file_length, &handle);
            if (esp_ret != ESP_OK)
            {
              is_error = true;
            }
            break;

          case YMODEM_TYPE_DATA:
            esp_ret = esp_ota_write_with_offset(handle, ymodem.file_buf, ymodem.file_buf_length, ymodem.file_addr);
            if (esp_ret != ESP_OK)
            {
              is_error = true;
            }
            break;

          case YMODEM_TYPE_END:
            is_done = true;
            esp_ota_end(handle);          
            break;

          case YMODEM_TYPE_CANCEL:
            is_done = true;
            break;

          case YMODEM_TYPE_ERROR:
            is_done = true;
            break;
        }
        ymodemAck(&ymodem);
      }

      delay(1);    
    }


    cliRunStr("launcher enable\n");
  }

  if (ret == false)
  {
    cliPrintf("ota info\n");
    cliPrintf("ota ymodem 0~%d[slot]\n", ota_part_count - 1);
  }
}
