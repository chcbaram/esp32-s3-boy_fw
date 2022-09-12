/*
 * ap.cpp
 *
 *  Created on: 2021. 1. 9.
 *      Author: baram
 */




#include "ap.h"



LVGL_IMG_DEF(test_img);


void apInit(void)
{
  cliOpen(_DEF_UART2, 115200);

  image_t logo;
  
  logo = lcdCreateImage(&test_img, 0, 0, 0, 0);

  lcdClearBuffer(black);
  lcdDrawImage(&logo, 0, 0);
  //lcdUpdateDraw();  


  //static uint8_t *p_test;

  //p_test = (uint8_t *)heap_caps_malloc(LCD_WIDTH * LCD_HEIGHT * sizeof(uint16_t), MALLOC_CAP_SPIRAM);

  // if (p_test != NULL)
  // {
  //   for (int i=0; i<LCD_WIDTH*LCD_HEIGHT; i++)
  //   {
  //     p_test[i] = 0;
  //   }
  // }
  // else
  // {
  //   logPrintf("p_test NULL\n");
  // }

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

    cliMain();
    delay(1);   
  }
}
