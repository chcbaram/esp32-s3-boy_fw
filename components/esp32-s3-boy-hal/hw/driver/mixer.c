/*
 * mixer.c
 *
 *  Created on: 2020. 8. 8.
 *      Author: Baram
 */




#include "mixer.h"



#ifdef _USE_HW_MIXER

typedef struct
{
  uint32_t length;
  uint32_t in;
  uint32_t out;

  int16_t  buf[MIXER_MAX_BUF_LEN];
} mixer_buf_t;


static  mixer_buf_t mixer_buf[MIXER_MAX_CH];



static uint32_t mixerBufAvailable(uint8_t ch);
static int16_t mixerBufRead(uint8_t ch);




bool mixerInit(void)
{
  uint32_t i;


  for (i=0; i<MIXER_MAX_CH; i++)
  {
    mixer_buf[i].in = 0;
    mixer_buf[i].out = 0;
    mixer_buf[i].length = MIXER_MAX_BUF_LEN;
  }

  return true;
}

bool mixerWrite(uint8_t ch, int16_t *p_data, uint32_t length)
{
  bool ret = true;
  uint32_t index;
  uint32_t next_index;
  uint32_t i;

  if (ch >= MIXER_MAX_CH) return false;

  for (i=0; i<length; i++)
  {
    index      = mixer_buf[ch].in;
    next_index = mixer_buf[ch].in + 1;

    if (next_index == mixer_buf[ch].length)
    {
      next_index = 0;;
    }

    if (next_index != mixer_buf[ch].out)
    {
      mixer_buf[ch].buf[index] = p_data[i];
      mixer_buf[ch].in         = next_index;
    }
    else
    {
      ret = false; // ERR_FULL
      break;
    }
  }

  return ret;
}


//  http://atastypixel.com/blog/how-to-mix-audio-samples-properly-on-ios/
//
int16_t mixerSamples(int16_t a, int16_t b)
{
 return
    // If both samples are negative, mixed signal must have an amplitude between the lesser of A and B, and the minimum permissible negative amplitude
    a < 0 && b < 0 ?
        ((int)a + (int)b) - (((int)a * (int)b)/INT16_MIN) :

    // If both samples are positive, mixed signal must have an amplitude between the greater of A and B, and the maximum permissible positive amplitude
    ( a > 0 && b > 0 ?
        ((int)a + (int)b) - (((int)a * (int)b)/INT16_MAX)

    // If samples are on opposite sides of the 0-crossing, mixed signal should reflect that samples cancel each other out somewhat
    :
        a + b);
}

bool mixerRead(int16_t *p_data, uint32_t length)
{
  uint32_t i;
  uint32_t ch;
  int16_t mixer_out;
  int16_t sample;

  for (i=0; i<length; i++)
  {

    mixer_out = mixerBufRead(0);
    for (ch=1; ch<MIXER_MAX_CH; ch++)
    {
      sample = mixerBufRead(ch);
      mixer_out = mixerSamples(mixer_out, sample);
    }

    p_data[i] = mixer_out;
  }

  return true;
}

uint32_t mixerBufAvailable(uint8_t ch)
{
  uint32_t ret = 0;

  if (ch >= MIXER_MAX_CH) return 0;

  ret = (mixer_buf[ch].length + mixer_buf[ch].in - mixer_buf[ch].out) % mixer_buf[ch].length;

  return ret;
}

int16_t mixerBufRead(uint8_t ch)
{
  int16_t ret = 0;
  uint32_t index;
  uint32_t next_index;

  if (ch >= MIXER_MAX_CH) return 0;


  index      = mixer_buf[ch].out;
  next_index = mixer_buf[ch].out + 1;

  if (next_index == mixer_buf[ch].length)
  {
    next_index = 0;
  }

  if (index != mixer_buf[ch].in)
  {
    ret = mixer_buf[ch].buf[index];
    mixer_buf[ch].out = next_index;
  }

  return ret;
}

uint32_t mixerAvailable(void)
{
  uint32_t ret = 0;
  uint32_t i;
  uint32_t buf_len;

  for (i=0; i<MIXER_MAX_CH; i++)
  {
    buf_len = mixerBufAvailable(i);
    if (buf_len > ret)
    {
      ret = buf_len;
    }
  }


  return ret;
}

uint32_t mixerAvailableForWrite(uint8_t ch)
{
  uint32_t rx_len;
  uint32_t wr_len;

  if (ch >= MIXER_MAX_CH) return 0;

  rx_len = mixerBufAvailable(ch);
  wr_len = (mixer_buf[ch].length - 1) - rx_len;

  return wr_len;
}

bool mixerIsEmpty(uint8_t ch)
{
  if (mixerBufAvailable(ch) > 0) return false;
  else                           return true;
}

int8_t mixerGetEmptyChannel(void)
{
  int8_t ret = -1;
  uint32_t i;

  for (i=0; i<MIXER_MAX_CH; i++)
  {
    if (mixerBufAvailable(i) == 0)
    {
      ret = i;
      break;
    }
  }

  return ret;
}

int8_t mixerGetValidChannel(uint32_t length)
{
  int8_t ret = -1;
  uint32_t i;

  for (i=0; i<MIXER_MAX_CH; i++)
  {
    if (mixerAvailableForWrite(i) >= length)
    {
      ret = i;
    }
  }

  return ret;
}


#endif