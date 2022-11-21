/*
 * fmsx_main.c
 *
 *  Created on: 2019. 11. 6.
 *      Author: HanCheol Cho
 */


#include "hw.h"
#include "Console.h"
#include "EMULib.h"
#include "MSX.h"
#include "fmsx.h"
#include <errno.h>
#include <sys/fcntl.h>

const char *dir_bios = "/sdcard/msx/bios";
const char *dir_roms = "/sdcard/msx/roms";


static char cur_path[1024];
static char cur_file[1024];


extern int StartMSX(int NewMode,int NewRAMPages,int NewVRAMPages);

void fmsxMain(void)
{
  ProgDir = dir_bios;


  chdir(ProgDir);

  UPeriod = 50;

  if (InitMachine())
  {
    Mode = (Mode&~MSX_VIDEO)|MSX_PAL;
    Mode = (Mode&~MSX_MODEL)|MSX_MSX2;
    Mode = (Mode&~MSX_JOYSTICKS)|MSX_JOY1|MSX_JOY2;


    RAMPages  = 2;
    VRAMPages = 2;

    if (!StartMSX(Mode,RAMPages,VRAMPages))
    {
      logPrintf("Start fMSX failed.\nMissing bios files?\n");
    }

    logPrintf("The emulation was shutted down\nYou can turn off your device\n");
  }
}


void fmsxChangeHome(void)
{
  chdir_(dir_roms);
}



int chdir_(const char *path)
{
  strcpy(cur_path, path);
  printf("chdir_ : %s\n", path);
  return 0;
}

#undef fopen

FILE *	fopen_ (const char *__restrict _name, const char *__restrict _type)
{
  sprintf(cur_file, "%s/%s", cur_path, _name);

  printf("fopen %s\n", cur_file);

  return fopen(cur_file, _type);
}

char *getcwd_(char *__buf, size_t __size )
{
  return cur_path;
}


/*
char *getcwd(char *__buf, size_t __size )
{
  char *p_buf;

  if (__buf != NULL)
  {
    p_buf = __buf;
  }
  else
  {
    p_buf = cur_path;
  }

  if (f_getcwd(p_buf, __size) == FR_OK)
  {
    printf("getcwd %s\n", p_buf);
  }
  else
  {
    printf("getcwd fail\n");
  }

  return p_buf;
}
*/


