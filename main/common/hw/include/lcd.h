/*
 * lcd.h
 *
 *  Created on: 2020. 12. 27.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_LCD_H_
#define SRC_COMMON_HW_INCLUDE_LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_LCD

#define LCD_WIDTH         HW_LCD_WIDTH
#define LCD_HEIGHT        HW_LCD_HEIGHT


#define GETR(c) (((uint16_t)(c)) >> 11)
#define GETG(c) (((c) & 0x07E0)>>5)
#define GETB(c) ((c) & 0x1F)
#define RGB2COLOR(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))


enum class_color
{
 white     = 0xFFFF,
 gray      = 0x8410,
 darkgray  = 0xAD55,
 black     = 0x0000,
 purple    = 0x8010,
 pink      = 0xFE19,
 red       = 0xF800,
 orange    = 0xFD20,
 brown     = 0xA145,
 beige     = 0xF7BB,
 yellow    = 0xFFE0,
 lightgreen= 0x9772,
 green     = 0x07E0,
 darkblue  = 0x0011,
 blue      = 0x001F,
 lightblue = 0xAEDC,
};


typedef enum
{
  LCD_FONT_07x10,
  LCD_FONT_11x18,
  LCD_FONT_16x26,
  LCD_FONT_HAN,
  LCD_FONT_MAX
} LcdFont;


typedef struct lcd_driver_t_ lcd_driver_t;

typedef struct lcd_driver_t_
{
  bool     (*init)(void);
  bool     (*reset)(void);
  void     (*setWindow)(int32_t x, int32_t y, int32_t w, int32_t h);
  uint16_t (*getWidth)(void);
  uint16_t (*getHeight)(void);
  bool     (*setCallBack)(void (*p_func)(void));
  bool     (*sendBuffer)(uint8_t *p_data, uint32_t length, uint32_t timeout_ms);

} lcd_driver_t;



bool lcdInit(void);
bool lcdIsInit(void);
void lcdReset(void);

uint8_t lcdGetBackLight(void);
void    lcdSetBackLight(uint8_t value);

uint32_t lcdReadPixel(uint16_t x_pos, uint16_t y_pos);
void lcdClear(uint32_t rgb_code);
void lcdClearBuffer(uint32_t rgb_code);

bool lcdDrawAvailable(void);
bool lcdRequestDraw(void);
void lcdUpdateDraw(void);
void lcdSetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void lcdSendBuffer(uint8_t *p_data, uint32_t length, uint32_t timeout_ms);
void lcdDisplayOff(void);
void lcdDisplayOn(void);

uint32_t lcdGetFps(void);
uint32_t lcdGetFpsTime(void);
uint32_t lcdGetDrawTime(void);

int32_t lcdGetWidth(void);
int32_t lcdGetHeight(void);

uint16_t *lcdGetFrameBuffer(void);
uint16_t *lcdGetCurrentFrameBuffer(void);
void lcdSetDoubleBuffer(bool enable);

void lcdDrawPixel(uint16_t x_pos, uint16_t y_pos, uint32_t rgb_code);
void lcdDrawPixelMix(uint16_t x_pos, uint16_t y_pos, uint32_t rgb_code, uint8_t mix);
void lcdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);
void lcdDrawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void lcdDrawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void lcdDrawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void lcdDrawFillScreen(uint16_t color);
void lcdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void lcdDrawFillCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color);
void lcdDrawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
void lcdDrawFillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
void lcdDrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
void lcdDrawFillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color);
void lcdDrawString(int x, int y, uint16_t color, const char *str);
void lcdPrintf(int x, int y, uint16_t color,  const char *fmt, ...);
void lcdSetFont(LcdFont font);
LcdFont lcdGetFont(void);


#endif /* _USE_HW_LCD */

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_LCD_H_ */
