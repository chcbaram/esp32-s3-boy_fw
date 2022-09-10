/*
 * han.h
 *
 *  Created on: 2020. 12. 27.
 *      Author: baram
 */

#ifndef SRC_HW_DRIVER_HANGUL_HAN_H_
#define SRC_HW_DRIVER_HANGUL_HAN_H_


#include <stdint.h>
#include <string.h>


#define PHAN_NULL_CODE    0
#define PHAN_HANGUL_CODE  1
#define PHAN_ENG_CODE     2
#define PHAN_SPEC_CODE    3
#define PHAN_END_CODE     4





typedef struct
{
  uint16_t HanCode;
  uint16_t Size_Char;  // 글자 1개의 바이트수(한글:2 영문:1)
  uint16_t Code_Type;  // 한/영/특수 문자인지 감별

  uint8_t  FontBuffer[32];
} han_font_t;



uint16_t hanFontLoad(char *HanCode, han_font_t *FontPtr);


#endif /* SRC_HW_DRIVER_HANGUL_HAN_H_ */
