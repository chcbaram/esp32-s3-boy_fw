#ifndef OTA_H_
#define OTA_H_


#include "ap_def.h"


void otaInit(void);
bool otaIsBusy(void);
bool otaRunByName(const char *p_name);

#endif