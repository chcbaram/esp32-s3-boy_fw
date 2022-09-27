/*
 * orocaboy.c
 *
 *  Created on: 2019. 11. 19.
 *      Author: HanCheol Cho
 */

#include "def.h"
#include "resize.h"
#include "hw/include/lcd.h"
#include "hw/include/button.h"
#include "hw/include/audio.h"


#include "defs.h"
#include "pcm.h"
#include "rc.h"
#include "fb.h"
#include "loader.h"


extern uint32_t micros(void);
extern uint32_t millis(void);
extern void lcdClear(uint32_t rgb_code);
extern uint16_t *lcdGetCurrentFrameBuffer(void);
extern uint16_t *lcdGetFrameBuffer(void);
extern bool lcdRequestDraw(void);
extern bool lcdDrawAvailable(void);
extern void resizeImageFast(resize_image_t *src, resize_image_t *dest);
extern void resizeImageFastOffset(resize_image_t *src, resize_image_t *dest);

struct pcm pcm;
struct fb fb;

static int stereo = 0;
static int samplerate = 11025;
static int sound = 1;

static int vid_mode = 1;
static audio_t audio;


EXT_RAM_BSS_ATTR uint16_t image_buf[160 * 144];


rcvar_t pcm_exports[] =
{
  RCV_BOOL("sound", &sound),
  RCV_INT("stereo", &stereo),
  RCV_INT("samplerate", &samplerate),
  RCV_END
};

rcvar_t vid_exports[] =
{
  RCV_END
};

rcvar_t joy_exports[] =
{
  RCV_END
};



static uint32_t pre_time;

void *sys_timer()
{
  pre_time = micros();

  return (void *)&pre_time;
}

int sys_elapsed(uint32_t *cl)
{
  uint32_t now;
  int usecs;

  now = micros();
  usecs = now - *cl;
  *cl = now;
  return usecs;
}

void sys_sleep(int us)
{
  uint32_t start;
  if (us <= 0) return;
  start = micros();
  while((micros()-start) < us);
}

void sys_checkdir(char *path, int wr)
{
}

void sys_initpath(char *exe)
{
}

void sys_sanitize(char *s)
{
}

void pcm_init()
{
  int n;


  pcm.stereo = 0;
  n = samplerate;
  pcm.hz = n;
  pcm.len = n / 60;
  pcm.buf = malloc(pcm.len);

  //speakerStart(samplerate);
  //delay(100);
  //speakerEnable();
  audioOpen(&audio);
  audioSetSampleRate(samplerate);
}

void pcm_close()
{
  if (pcm.buf) free(pcm.buf);
  memset(&pcm, 0, sizeof pcm);

  audioClose(&audio);
}

int pcm_submit()
{
  int16_t buf[pcm.len];

  while(audioAvailableForWrite(&audio) < pcm.pos)
  {
    delay(1);
  }

  if (pcm.buf)
  {
    for (int i=0; i<pcm.pos; i++)
    {
      //speakerPutch(pcm.buf[i]);
      buf[i] = pcm.buf[i];
      buf[i] -= 128;   
      buf[i] <<= 5;
    }
    audioWrite(&audio, buf, pcm.pos);
  }
  pcm.pos = 0;
  return 1;
}





void vid_init()
{
  fb.w        = 160;
  fb.h        = 144;
  fb.pelsize  = 2;
  fb.pitch    = 160 * 2;
  fb.indexed  = 0;

  fb.cc[0].r = 8 - 5;
  fb.cc[1].r = 8 - 6;
  fb.cc[2].r = 8 - 4;
  fb.cc[0].l = 11;
  fb.cc[1].l = 5;
  fb.cc[2].l = 0;


  fb.ptr = (byte *)image_buf;

  fb.dirty    = 0;
  fb.enabled  = 1;

  if (vid_mode >= 5)
  {
    vid_mode = 0;
  }
}

void vid_close()
{
  fb.enabled = 0;
}

void vid_preinit()
{

}

void vid_settitle(char *title)
{

}

void vid_begin()
{

}

void vid_change_mode(void)
{
  vid_mode = (vid_mode + 1) % 3;
}

void vid_end()
{
  static uint32_t pre_time;
  static uint32_t fps_count = 0;
  static int last_vid_mode = 0;
  static uint8_t skip_count = 0;
  static uint8_t frame_index = 0;


  fps_count++;
  if (millis()-pre_time >= 1000 )
  {
    pre_time = millis();
    printf("%d FPS\n", (int)fps_count);
    fps_count = 0;
  }

  frame_index++;
  if (frame_index%2 != 0)
  {
    return;
  }

  while(!lcdDrawAvailable())
  {
    delay(1);
  }

  resize_image_t src;
  resize_image_t dest;

  src.p_data = image_buf;
  src.x      = 0;
  src.y      = 0;
  src.w      = 160;
  src.h      = 144;
  src.stride = 160;

  switch(vid_mode)
  {
    case 0:
      dest.p_data = lcdGetFrameBuffer();
      dest.w      = 160;
      dest.h      = 144;
      dest.stride = lcdGetWidth();
      dest.x = (lcdGetWidth()-dest.w)/2;
      dest.y = (lcdGetHeight()-dest.h)/2;

      if (last_vid_mode != 0)
      {
        lcdClear(0);
        lcdClear(0);
      }
      resizeImageNearest(&src, &dest);
      lcdRequestDraw();
      break;

    case 1:
      dest.p_data = lcdGetFrameBuffer();
      //dest.w      = 267;
      //dest.h      = 240;
      dest.w      = 240;
      dest.h      = 216;
      dest.stride = lcdGetWidth();
      dest.x = (lcdGetWidth()-dest.w)/2;
      dest.y = (lcdGetHeight()-dest.h)/2;

      resizeImageNearest(&src, &dest);
      lcdRequestDraw();
      break;

    case 2:
      dest.p_data = lcdGetFrameBuffer();
      dest.w      = lcdGetWidth();
      dest.h      = lcdGetHeight();
      dest.stride = lcdGetWidth();
      dest.x = (lcdGetWidth()-dest.w)/2;
      dest.y = (lcdGetHeight()-dest.h)/2;

      resizeImageNearest(&src, &dest);
      lcdRequestDraw();
      break;
  }
  last_vid_mode = vid_mode;
  skip_count = (skip_count + 1) % 4;
}

void ev_poll()
{
  if (buttonGetPressedTime(_BTN_X) > 50 && buttonGetPressedEvent(_BTN_X) == true)
  {
    vid_change_mode();
  }
}

void vid_setpal(int i, int r, int g, int b)
{
  printf("setpal %d, %d %d %d\n", i, r, g, b);
}


void joy_init()
{
}

void joy_close()
{
}

void joy_poll()
{
}


uint32_t osdWaitKey(bool wait)
{
  uint32_t key = 0;
  uint32_t prev_key = 0;
  uint32_t key_repeat = 1;

  while(1)
  {
    delay(10);

    key = 0;
    for (int i=0; i<BUTTON_MAX_CH; i++)
    {
      if (buttonGetPressed(i) == true)
      {
        key |= (1<<i);
      }
    }

    if (wait != true)
    {
      break;
    }

    if (key != prev_key)
    {
      key_repeat = 0;
    }
    key_repeat++;

    prev_key = key;

    if (key > 0)
    {
      if (key_repeat == 0 || key_repeat >= 10)
      {
        break;
      }
    }
  }

  return key;
}

void osdMessage(char *msg)
{
  uint16_t bg_color = white;

  if (msg)
  {
    //lcdDrawFillRect(0, (lcdGetWidth()-50)/2, lcdGetHeight(), 50, bg_color);

    uint16_t w = 8, h = 16;
    w = lcdGetStrWidth(msg);
    lcdPrintf((lcdGetWidth()-w)/2, (lcdGetHeight()-50)/2 + (50-h)/2, black, "%s", msg);
  }
  else
  {
    lcdDrawFillRect(0, (lcdGetHeight()-50)/2, lcdGetWidth(), 50, bg_color);
  }

  lcdUpdateDraw();
}


bool osd_menu(void)
{
  uint32_t cursor = 0;
  uint32_t cursor_last = 0;
  uint32_t cursor_max = 4;
  uint32_t key;
  uint16_t bg_color = orange;
  bool ret_exit = false;


  if (buttonGetPressedEvent(_BTN_HOME) == false)
  {
    return false;
  }

  lcdClear(black);
  buttonClear();

  while(1)
  {

    lcdClearBuffer(bg_color);
    lcdDrawFillRect(0, 0, lcdGetWidth(), 20, blue);

    //lcdSetBgColor(blue);
    lcdPrintf((lcdGetWidth()-lcdGetStrWidth("MENU"))/2, 2, white, "MENU");


    //lcdSetBgColor(bg_color);
    lcdPrintf(0, 46 + 20*0, white, " SAVE");
    lcdPrintf(0, 46 + 20*1, white, " LOAD");
    lcdPrintf(0, 46 + 20*2, white, " EXIT");
    lcdPrintf(0, 46 + 20*3, white, " OK");

    lcdDrawRect(2, 44 + 20*cursor, lcdGetWidth()-4, 19, blue);
    if (cursor != cursor_last)
    {
      lcdDrawRect(2, 44 + 20*cursor_last, lcdGetWidth()-4, 19, bg_color);
    }

    lcdUpdateDraw();

    //key = osdWaitKey(true);

    //if (key & (1<<_BTN_UP))
    if (buttonGetPressedEvent(_BTN_UP) == true)
    {
      cursor_last = cursor;

      if (cursor == 0) cursor = cursor_max - 1;
      else             cursor = (cursor - 1) % cursor_max;
    }
    //if (key & (1<<_BTN_DOWN))
    if (buttonGetPressedEvent(_BTN_DOWN) == true)
    {
      cursor_last = cursor;
      cursor = (cursor + 1) % cursor_max;
    }

    //if (key & (1<<_BTN_A))
    if (buttonGetPressedEvent(_BTN_A) == true)
    {
      if (cursor == 0) // SAVE
      {
        state_save(0);
        osdMessage("Saving...");
        delay(1000);
      }
      if (cursor == 1) // LOAD
      {
        state_load(0);
        osdMessage("Loading...");
        delay(1000);
      }
      if (cursor == 2) // EXIT
      {
        ret_exit = true;
      }
      break;
    }
    if (buttonGetPressedEvent(_BTN_HOME) == true)
    {
      break;
    }
  }

  lcdClear(black);

  while(buttonGetPressedCount() > 0)
  {
    delay(1);
  }
  buttonClear();

  return ret_exit;
}
