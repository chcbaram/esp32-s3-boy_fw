/*
 * filer.c
 *
 *  Created on: 2019. 11. 23.
 *      Author: Baram
 */


#include "hw/include/button.h"
#include "hw/include/lcd.h"
#include "hw/include/audio.h"



#define GNUBOY_ROOT_DIR  "/sdcard/gnuboy"


#define LOGO_BACKCOLOR blue

#define FILER_OFFSET_X    (0)
#define FILER_OFFSET_Y    (0)


#define FOLDER_FILE_MAX 50
#define LIST_ROWS 8
#define LIST_TOP  18
#define LIST_LEFT 4


int DispTop; // Top of the list
int SelIdx; // Cursor index
int FileCount; // Number of files in the folder

// file information
typedef struct tagLISTITEM{
    char filename[65];
    int  filename_len;
    bool isDir;
    long size;
} LISTITEM;

// folder file list
LISTITEM *flist = NULL;

char szRomFolder[256];
char szPrevFolder[256];
char szRomFilename[33];
char szRomPath[256];


extern uint32_t lcdGetStrWidth(const char *fmt, ...);


/*-------------------------------------------------------------------*/
/*  Get file extension                                               */
/*-------------------------------------------------------------------*/
char *get_ext(char *filename)
{
    int i;
    for (i = strlen(filename) - 1; i >= 0; --i) {
        if (filename[i] == '.') {
            return &filename[i + 1];
        }
    }
    return NULL;
}

/*-------------------------------------------------------------------*/
/*  Get file path                                                    */
/*-------------------------------------------------------------------*/
char *get_filepath(char *path, char *filename)
{
  static char filepath[256];

  if (strcmp(path, GNUBOY_ROOT_DIR) != 0)
  {
      filepath[0] = 0;
      return filepath;
  }

  sprintf(filepath, "%s/%s", path, filename);

  return filepath;
}

/*-------------------------------------------------------------------*/
/*  Get file list in the folder                                      */
/*-------------------------------------------------------------------*/
#if 1
int get_filelist(char *path)
{
  DIR *d;
  UINT i = 0;
  struct dirent * dir;
  char buffer[256];


  d = opendir(path);                       /* Open the directory */
  if (d != NULL)
  {
    for (;;)
    {
      dir = readdir(d);                   /* Read a directory item */

      if (dir == NULL) break;

      if (dir->d_type == DT_DIR)
      {                    /* It is a directory */
        continue;
      }
      else
      {                                       /* It is a file. */
        int len = strlen(dir->d_name);
        len = len > 64 ? 64 : len;
        memcpy(flist[i].filename, dir->d_name, len);
        flist[i].filename[len] = 0;
        flist[i].filename_len = len;
        flist[i].isDir = false;
        flist[i].size = 0;

        FILE *f;
        sprintf(buffer, GNUBOY_ROOT_DIR "/%s", dir->d_name);
        f = fopen(buffer, "r");
        if (f)
        {
          fseek(f, 0, SEEK_END);
          flist[i].size= ftell(f);
          fclose(f);
        }
        if (flist[i].filename[0] != '.')
        {
          ++i;
        }
      }
    }
    closedir(d);
  }

  return i;
}
#else
int get_filelist(char *path)
{
  FRESULT res;
  DIR dir;
  UINT i = 0;
  static FILINFO fno;

  res = f_opendir(&dir, path);                       /* Open the directory */
  if (res == FR_OK)
  {
    for (;;)
    {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */

      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      if (fno.fattrib & AM_DIR)
      {                    /* It is a directory */
        continue;
      }
      else
      {                                       /* It is a file. */
        int len = strlen(fno.fname);
        len = len > 32 ? 32 : len;
        memcpy(flist[i].filename, fno.fname, len);
        flist[i].filename[len] = 0;
        flist[i].isDir = false;
        flist[i].size = fno.fsize;
        ++i;
      }
    }
    f_closedir(&dir);
  }

  return i;
}
#endif

/*-------------------------------------------------------------------*/
/*  Get file information string                                      */
/*-------------------------------------------------------------------*/
char* filedata(int idx)
{
    static char disptext[41];

    if (flist[idx].isDir) {
        sprintf(disptext, "%-16s  <DIR>", flist[idx].filename);
    } else {
        sprintf(disptext, "%-16s", flist[idx].filename);
    }

    disptext[25] = 0;

    return disptext;
}

/*-------------------------------------------------------------------*/
/*  Draw file list                                                   */
/*-------------------------------------------------------------------*/
void draw_list()
{
  int i,y;

  for (y = 0, i = DispTop; y < LIST_ROWS && i < FileCount; ++y, ++i)
  {
    lcdPrintf( LIST_LEFT, LIST_TOP + y * 16, white, "%s", filedata(i));
  }

  if (y < LIST_ROWS)
  {
    //tft_boxfill( 0, LIST_TOP + y * 8, 319, LIST_TOP + LIST_ROWS * 8 - 1, TFT_BLACK);
    //lcdDrawFillRect( 0, LIST_TOP + y * 16, 319, LIST_ROWS * 16 - 1, black);
  }
}

/*-------------------------------------------------------------------*/
/*  Draw cursor                                                      */
/*-------------------------------------------------------------------*/
void draw_cursor(bool clear)
{
  int y = SelIdx - DispTop;
  if (clear)
  {
    lcdDrawFillRect( LIST_LEFT - 1, LIST_TOP + y * 16 - 0, HW_LCD_WIDTH-LIST_LEFT*2, 15, black);
    lcdPrintf(LIST_LEFT, LIST_TOP + y * 16, white, "%s", filedata(SelIdx));
  } else
  {
    lcdDrawFillRect( LIST_LEFT - 1, LIST_TOP + y * 16 - 0, HW_LCD_WIDTH-LIST_LEFT*2, 15, yellow);
    lcdPrintf( LIST_LEFT, LIST_TOP + y * 16, black, "%s", filedata(SelIdx));
  }
}

/*-------------------------------------------------------------------*/
/*  Draw back ground                                                 */
/*-------------------------------------------------------------------*/
void draw_frame()
{
  //lcdClear(black);


  lcdDrawFillRect(0, 0, HW_LCD_WIDTH, 16, LOGO_BACKCOLOR);
  lcdDrawFillRect(0, HW_LCD_HEIGHT-31, HW_LCD_WIDTH, 16, LOGO_BACKCOLOR);

  lcdPrintf((HW_LCD_WIDTH - 6*8)/2, 0, yellow, "GNUBOY");
}

/*-------------------------------------------------------------------*/
/*  Dialog window                                                    */
/*-------------------------------------------------------------------*/
void dialog(char *msg)
{
  if (msg)
  {
    lcdDrawFillRect(0, (lcdGetHeight()-50)/2, lcdGetWidth(), 50, darkgray);

    uint16_t w = 8, h = 16;
    w = lcdGetStrWidth(msg);
    lcdPrintf((lcdGetWidth()-w)/2, (lcdGetHeight()-50)/2 + (50-h)/2, red, "%s", msg);
  }
  else
  {
    lcdDrawFillRect(0, (lcdGetHeight()-50)/2, lcdGetWidth(), 50, black);
  }
}

/*-------------------------------------------------------------------*/
/*  Rom selection                                                    */
/*-------------------------------------------------------------------*/
//  return code:
//   0 : File selected
//   1 : Return to Emu
//   2 : Reset
//   -1: Error
int gnuboyFiler()
{
  bool loadFolder;
  int keyrepeat = 1;
  uint32_t dwPad1 = 0;
  uint32_t prev_dwPad1 = 0;


  strcpy(szRomFolder, GNUBOY_ROOT_DIR);

  lcdClearBuffer(black);
  lcdUpdateDraw();

  //lcdClear(black);
  //lcdClear(black);

  //lcdSetDoubleBuffer(false);
  //lcdClear(red);
  //lcdUpdateDraw();
  //while(1);

  //lcdSetWindow(0, 0, lcdGetWidth(), lcdGetHeight());

  // init
  if (flist == NULL)
  {
    flist = (LISTITEM *)malloc(sizeof(LISTITEM) * FOLDER_FILE_MAX);
    if (flist == NULL)
    {
        return -1;
    }
  }


  loadFolder = true;
  int ret;


  while(1)
  {
    while(lcdDrawAvailable() != true)
    {
      delay(1);
    }

    // Draw back ground
    draw_frame();

    if (loadFolder)
    {
      // Drawing of the files in the folder
      FileCount = get_filelist(szRomFolder);

      if (FileCount > 0)
      {
        if (strcmp(szRomFolder, szPrevFolder) != 0)
        {
          DispTop = 0;
          SelIdx = 0;
          strcpy( szPrevFolder, szRomFolder);
        }
        draw_list();
        draw_cursor(false);
      }
      loadFolder = false;
    }

    dwPad1 = 0;
    for (int i=0; i<BUTTON_MAX_CH; i++)
    {
      if (buttonGetPressed(i))
      {
        dwPad1 |= (1<<i);
      }
    }


    if (dwPad1 != prev_dwPad1)
    {
        keyrepeat = 0;
    }

    if (keyrepeat == 0 || keyrepeat >= 10)
    {
      // Down
      if (dwPad1 & (1<<_BTN_DOWN))
      {
        if (SelIdx < FileCount - 1)
        {
          // clear cursor
          draw_cursor(true);

          if (SelIdx - DispTop == LIST_ROWS - 1)
          {
            DispTop++;
            draw_list();
          }

          // draw new cursor
          SelIdx++;
          draw_cursor(false);
        }
      }

      // Up
      if (dwPad1 & (1<<_BTN_UP))
      {
        //audioPlayNote(5, 1, 30);

        if (SelIdx > 0)
        {
          // clear cursor
          draw_cursor(true);

          if (SelIdx - DispTop == 0)
          {
            DispTop--;
            draw_list();
          }

          // draw new cursor
          SelIdx--;
          draw_cursor(false);
        }
      }

      // Select
      if (dwPad1 & (1<<_BTN_A))
      {
        // clear cursor
        draw_cursor(true);


        if (flist[SelIdx].isDir)
        {
          // folder
          if (strcmp(flist[SelIdx].filename, "..") != 0)
          {
            sprintf(szRomFolder, "%s/%s", szRomFolder, flist[SelIdx].filename);
          }
          else
          {
            // upper folder
            char *upper = strrchr(szRomFolder, '/');
            if (upper)
            {
              *upper = 0;
            }
          }

          loadFolder = true;
        }
        else
        {
          char *ext = get_ext(flist[SelIdx].filename);

          if (ext != NULL && strcasecmp(ext, "gb") == 0)
          {
            strcpy(szRomFilename, flist[SelIdx].filename);
            strcpy(szRomPath, get_filepath(szRomFolder, szRomFilename));
            ret = 0;
            break;
          }
          else if (ext != NULL && strcasecmp(ext, "gbc") == 0)
          {
            strcpy(szRomFilename, flist[SelIdx].filename);
            strcpy(szRomPath, get_filepath(szRomFolder, szRomFilename));
            ret = 0;
            break;
          }
          else
          {
            dialog((char *)"Not a rom file!!");
            lcdRequestDraw();
            delay(1000);
            dialog(NULL);
            lcdRequestDraw();
            draw_list();
            draw_cursor(false);
          }
        }
      }

      /*
      // Return to Emu
      if (dwSystem & 0x01) {
          if ( szRomFilename[0] != 0 ) {
              do {
                  wait(0.08);
                  pNesX_PadState( &dwPad1, &dwPad2, &dwSystem );

              } while (dwSystem & 0x01);

              ret = 1;
              break;
          }
      }

      // Reset
      if (dwSystem & 0x02) {
          if ( szRomFilename[0] != 0 ) {
              do {
                  wait(0.08);
                  pNesX_PadState( &dwPad1, &dwPad2, &dwSystem );
              } while (dwSystem & 0x02);

              ret = 2;
              break;
          }
      }
      */
    }

    keyrepeat++;

    prev_dwPad1 = dwPad1;

    delay(20);
    lcdRequestDraw();
  }

  //release memory
  //free(flist);

  lcdClear(black);
  buttonClear();
  lcdUpdateDraw();
  return ret;
}

