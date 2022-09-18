/*
 * ap.cpp
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */




#include "ap.h"


static void cliThread(void *args);

static void updateSdCard(void);
static void updateFatfs(void);
static void updateLcd(void);
static void updateBat(void);
static void updateButton(void);





void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);

  if (xTaskCreate(cliThread, "cliThread", _HW_DEF_RTOS_THREAD_MEM_CLI, NULL, _HW_DEF_RTOS_THREAD_PRI_CLI, NULL) != pdPASS)
  {
    logPrintf("[NG] cliThread()\n");   
  }  

  delay(500);
}

void apMain(void)
{
  uint32_t pre_time;


  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }


    if (lcdDrawAvailable() == true)
    {
      lcdClearBuffer(black);

      updateSdCard();     
      updateFatfs();
      updateLcd();
      updateBat();
      updateButton();

      lcdRequestDraw();
    }
    delay(1);   
  }
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

void cliThread(void *args)
{
  while(1)
  {
    cliMain();
    delay(2);
  }
}