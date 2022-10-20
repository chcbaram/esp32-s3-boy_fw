#ifndef LAUNCHER_H_
#define LAUNCHER_H_


#include "ap_def.h"


namespace launcher
{

void main(void);
void drawBackground(const char *title_str);
void drawMsgBox(const char *str, uint16_t txt_color, uint32_t timeout);
bool runFile(const char *file_name);

}


#endif