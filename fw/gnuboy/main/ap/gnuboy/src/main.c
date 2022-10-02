#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "gnuboy.h"
#include "input.h"
#include "rc.h"
#include "loader.h"
#include "hw.h"
#include "Version"
#include "hw_def.h"
#include "button.h"
#include "hw/include/lcd.h"

extern char szRomPath[256];

extern int gnuboyFiler();



void doevents()
{
  ev_poll();

  pad_set(PAD_UP,    buttonGetPressed(_BTN_UP));
  pad_set(PAD_RIGHT, buttonGetPressed(_BTN_RIGHT));
  pad_set(PAD_DOWN,  buttonGetPressed(_BTN_DOWN));
  pad_set(PAD_LEFT,  buttonGetPressed(_BTN_LEFT));

  pad_set(PAD_SELECT, buttonGetPressed(_BTN_SELECT));
  pad_set(PAD_START,  buttonGetPressed(_BTN_START));

  pad_set(PAD_A, buttonGetPressed(_BTN_A));
  pad_set(PAD_B, buttonGetPressed(_BTN_B));
}




static void shutdown()
{
	vid_close();
	pcm_close();
}

void die(char *fmt, ...)
{
	va_list ap;

	printf("die\n");

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

#if 0



void gnuboyMain(char *file_name)
{
  strcpy(szRomPath, file_name);

  while(1)
  {
    vid_preinit();

    init_exports();


    vid_init();
    pcm_init();


    rc_command("set rcpath /gnuboy");
    rc_command("set savedir /gnuboy/saves");


    loader_init(szRomPath);
    emu_reset();
    emu_run();
    loader_unload();

    vid_close();
    pcm_close();
  }
}


#else
void gnuboyMain(void)
{
	vid_preinit();

	init_exports();


	vid_init();
	pcm_init();

	
  rc_command("set rcpath /sdcard/gnuboy");
  rc_command("set savedir /sdcard/gnuboy/saves");

  mkdir("/sdcard/gnuboy/saves", 0755);


  while(1)
  {
    switch (gnuboyFiler())
    {
      case 0:  // Selected a file
        loader_init(szRomPath);
        emu_reset();
        emu_run();

        loader_unload();
        break;

      case 1:  // Return to Emu
          break;

      case 2:  // Reset
        break;

      case -1:  // Error
        printf("Filer Error\r\n");
        while(1);
    }
  }

  vid_close();
  pcm_close();
}
#endif
