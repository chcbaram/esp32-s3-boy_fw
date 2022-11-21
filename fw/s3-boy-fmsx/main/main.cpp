#include "main.h"



extern "C" void app_main(void);


void app_main(void)
{
  hwInit();
  apInit();
  apMain();
}
