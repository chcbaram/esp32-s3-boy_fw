/*
 * ap.cpp
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */




#include "ap.h"


static void cliThread(void *args);





void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);

  if (xTaskCreate(cliThread, "cliThread", 4096, NULL, 5, NULL) != pdPASS)
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

      lcdPrintf(0,16*0, white, "SD : %s", sdIsDetected() == true ? "Present" : "Empty");
      lcdPrintf(0,16*1, white, "     %s", sdGetStateMsg());
     
      lcdRequestDraw();
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