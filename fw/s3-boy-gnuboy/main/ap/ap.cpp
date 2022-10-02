/*
 * ap.cpp
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */




#include "ap.h"



extern "C" void gnuboyMain(void);


static void cliThread(void *args);
static void emulInit(void);
static void emulThread(void *args);



void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);

  if (xTaskCreate(cliThread, "cliThread", _HW_DEF_RTOS_THREAD_MEM_CLI, NULL, _HW_DEF_RTOS_THREAD_PRI_CLI, NULL) != pdPASS)
  {
    logPrintf("[NG] cliThread()\n");   
  }  

  delay(500);

  emulInit();
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
    delay(1);   
  }
}

void cliThread(void *args)
{
  while(1)
  {
    cliMain();
    delay(2);
  }
}

void emulInit(void)
{
  if (xTaskCreate(emulThread, "emulThread", 8*1024, NULL, 5, NULL) != pdPASS)
  {
    logPrintf("[NG] emulThreadThread()\n");   
  }
    
  while(1)
  {
    delay(100);
  }
}



void emulThread(void *args)
{
  logPrintf("emulThread..\n");
  while(1)
  {
    if (fatfsIsMounted() == true)
    {
      break;
    }
    delay(10);
  }
  logPrintf("emulThread Start\n");

  lcdClear(black);
  gnuboyMain();

  while(1)
  {
    delay(100);
  }
}