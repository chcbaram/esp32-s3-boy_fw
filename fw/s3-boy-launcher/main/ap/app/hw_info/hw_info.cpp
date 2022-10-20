#include "hw_info.h"
#include "launcher/launcher.h"

namespace hw_info
{


static void updateSdCard(void);
static void updateFatfs(void);
static void updateLcd(void);
static void updateBat(void);
static void updateButton(void);
static void updateAudio(void);
static void updatePartition(void);
static void cliHwInfo(cli_args_t *args);

static audio_t audio;
static bool is_enable = true;
static bool is_req_enable = false;
static bool is_req_value = false; 



void main(void)
{
  uint32_t pre_time;
  static bool is_init = false;


  if (is_init == false)
  {
    is_init = true;
    cliAdd("hw_info", cliHwInfo);
  }

  audioOpen(&audio);

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }

    if (is_req_enable == true)
    {
      is_enable = is_req_value;
      is_req_enable = false;
      buttonClear();
    }

    if (buttonGetPressedEvent(_BTN_HOME))
    {
      buttonClear();
      break;
    }

    if (is_enable == true && lcdDrawAvailable() == true)
    {
      lcdClearBuffer(black);

      updateSdCard();     
      updateFatfs();
      updateLcd();
      updateBat();
      updateButton();
      updateAudio();
      updatePartition();

      lcdRequestDraw();
    }
    delay(1);   
  }

  audioClose(&audio);
}

void updateSdCard(void)
{
  lcdPrintf(0,16*0, white, "SD : %s", sdIsDetected() == true ? "Present" : "Empty");
  lcdPrintf(0,16*1, white, "     %s", sdGetStateMsg());
}

void updateFatfs(void)
{
  DIR * d = opendir("/sdcard"); 
  struct dirent * dir;
  char buffer[300];
  uint8_t file_cnt = 0;
  int16_t x_o = 8*5;
  int16_t y_o = 16*2;


  if (fatfsIsMounted() == false)
  {
    lcdPrintf(0, y_o, white, "FAT: Empty");
    return;
  }
  lcdPrintf(0, y_o, white, "FAT:");

  if (d != NULL)
  {
    fatfsLock();
    while ((dir = readdir(d)) != NULL) 
    {
      if (dir-> d_type != DT_DIR) 
      {
        FILE *f;
        int file_len = 0;

        sprintf(buffer, "/sdcard/%s", dir->d_name);
        f = fopen(buffer, "r");
        if (f)
        {
          fseek(f, 0, SEEK_END);
          file_len = ftell(f);
          fclose(f);
        }
        if (strlen(dir->d_name) >= 8)
        {
          dir->d_name[8] = '.';
          dir->d_name[9] = '.';
          dir->d_name[10] = '.';
          dir->d_name[11] = 0;
        }
        lcdPrintf(x_o, y_o + 16*file_cnt, green, "%-12s %dKB\n", dir->d_name, file_len/1024);
        file_cnt++;
        if (file_cnt >= 3)
          break;
      }
    }
    closedir(d);

    fatfsUnLock();
  }
}

void updateLcd(void)
{
  lcdPrintf(0,16*5, white, "BKL: %d%%", lcdGetBackLight());
}

void updateBat(void)
{
  lcdPrintf(0,16*6, white, "BAT: %-3d%% %1.2fV", batteryGetPercent(), batteryGetVoltage());
}

void updateButton(void)
{
  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    if (buttonGetPressed(i) == true)
    {
      lcdPrintf(0,16*7, white, "BTN: %-12s %dms", buttonGetName(i), buttonGetPressedTime(i));
      break;
    }
  }
  lcdPrintf(0,16*7, white, "BTN:");
}

void updateAudio(void)
{
  lcdPrintf(0,16*8, white, "AUD: test.wav");

  if (audioIsPlaying(&audio) == true)
    lcdPrintf(0,16*9, white, "     Playing..");
  else
    lcdPrintf(0,16*9, white, "     Stop..");
  
  if (buttonGetPressedEvent(_BTN_SELECT) == true)
  {
    if (audioIsPlaying(&audio) == true)
      audioStopFile(&audio);
    else
      audioPlayFile(&audio, "/sdcard/test.wav", false);
  }
}

void updatePartition(void)
{
  esp_partition_iterator_t it;
  uint32_t index = 0;
  static uint32_t cur_index = 0;
  bool draw_cursor = false;
  const esp_partition_t *cur_part = NULL;
  esp_app_desc_t app_desc;

  it = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);

  lcdPrintf(0,16*10, white, "PAT: ");

  for (; it != NULL; it = esp_partition_next(it)) 
  {
    const esp_partition_t *part = esp_partition_get(it);

    if (esp_ota_get_partition_description(part, &app_desc) != ESP_OK)
    {
      strcpy(app_desc.project_name, "empty");
    }

    draw_cursor = false;
    if (index > 0 && (index-1) == cur_index)
    {
      draw_cursor = true;
      cur_part = part;
    }

    if (draw_cursor == true)
    {
      lcdDrawFillRect(40, 16*10+16*index, LCD_WIDTH-40, 16, yellow);
      lcdPrintf(40,16*10+16*index, black, "%-16s 0x%06X", app_desc.project_name, part->address);
    }
    else
    {
      lcdPrintf(40,16*10+16*index, white, "%-16s 0x%06X", part->label, part->address);
    }
    index++;         
  }  

  if (buttonGetPressedEvent(_BTN_UP))
  {
    if (cur_index == 0)
      cur_index = index - 2;
    else
      cur_index--;
  }
  if (buttonGetPressedEvent(_BTN_DOWN))
  {
    cur_index++;
    if (cur_index >= (index-1)) 
      cur_index = 0;
  }

  if (buttonGetPressedEvent(_BTN_A) == true && cur_part != NULL)
  {
    lcdClear(black);

    buzzerBeep(100);
    delay(500);
    esp_ota_set_boot_partition(cur_part);
    esp_restart();
  } 
}

void cliHwInfo(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "enable"))
  {
    is_req_value = true;
    is_req_enable = true;

    while(is_req_enable == true)
    {
      delay(1);
    }
    cliPrintf("[OK] hw_info Enable\n");
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "disable"))
  {
    is_req_value = false;
    is_req_enable = true;

    while(is_req_enable == true)
    {
      delay(1);
    }
    cliPrintf("[OK] hw_info Disable\n");
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("hw_info enable\n");
    cliPrintf("hw_info disable\n");
  }
}

}