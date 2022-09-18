/*
 * i2s.c
 *
 *  Created on: 2020. 8. 7.
 *      Author: Baram
 */



#include "i2s.h"
#include "mixer.h"
#include "cli.h"
#include "buzzer.h"
#include <math.h>


#ifdef _USE_HW_I2S
#include "driver/i2s_std.h"


#define I2S_SAMPLERATE_HZ       16000
#define I2S_MAX_BUF_LEN         (16*4*8) // 32ms
#define I2S_MAX_FRAME_LEN       (16*4)   // 4ms

#define I2S_BCK_IO              (GPIO_NUM_40)
#define I2S_WS_IO               (GPIO_NUM_41)
#define I2S_DO_IO               (GPIO_NUM_39)


#ifdef _USE_HW_CLI
static void cliI2s(cli_args_t *args);
#endif
static void i2sThread(void *args);


static bool is_init = false;
static bool is_started = false;

static i2s_chan_handle_t i2s_chan;  // I2S tx channel handler
static i2s_chan_config_t i2s_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
static i2s_std_config_t  i2s_std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,    // some codecs may require mclk signal, this example doesn't need it
            .bclk = I2S_BCK_IO,
            .ws   = I2S_WS_IO,
            .dout = I2S_DO_IO,
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };





bool i2sInit(void)
{
  esp_err_t err;

  mixerInit();


  err = i2s_new_channel(&i2s_chan_cfg, &i2s_chan, NULL);
  logPrintf("[%s] i2s_new_channel()\n", err == ESP_OK ? "OK":"NG");
  if (err != ESP_OK) return false;

  err = i2s_channel_init_std_mode(i2s_chan, &i2s_std_cfg);
  logPrintf("[%s] i2s_channel_init_std_mode()\n", err == ESP_OK ? "OK":"NG");
  if (err != ESP_OK) return false;

  err = i2s_channel_enable(i2s_chan);
  logPrintf("[%s] i2s_channel_enable()\n", err == ESP_OK ? "OK":"NG");
  if (err != ESP_OK) return false;


  if (xTaskCreate(i2sThread, "i2sThread", _HW_DEF_RTOS_THREAD_MEM_I2S, NULL, _HW_DEF_RTOS_THREAD_PRI_I2S, NULL) != pdPASS)
  {
    logPrintf("[NG] i2sThread()\n");   
  }

  is_init = true;

#ifdef _USE_HW_CLI
  cliAdd("i2s", cliI2s);
#endif

  return true;
}

void i2sThread(void *args)
{
  int16_t *send_frame = (int16_t *)calloc(2, I2S_MAX_BUF_LEN);
  size_t w_bytes = 0;
  bool is_sent = false;

  while(1) 
  {
    if (mixerAvailable() >= I2S_MAX_FRAME_LEN)
    {
      mixerRead(send_frame, I2S_MAX_FRAME_LEN);

      if (i2s_channel_write(i2s_chan, send_frame, I2S_MAX_FRAME_LEN * 2, &w_bytes, 1000) != ESP_OK)
      {
        logPrintf("i2s_channel_write fail\n");
      } 
      is_sent = true;
    }
    else
    {
      if (is_sent == true)
      {
        is_sent = false;
        memset(send_frame, 0, I2S_MAX_FRAME_LEN * 2);
      }
      i2s_channel_write(i2s_chan, send_frame, I2S_MAX_FRAME_LEN * 2, &w_bytes, 1000);
    }
    delay(2);    
  }
}


bool i2sIsInit(void)
{
  return is_init;
}

bool i2sStart(void)
{
  is_started = true;
  return true;
}

bool i2sStop(void)
{
  is_started = false;
  return true;
}

uint32_t i2sAvailableForWrite(uint8_t ch)
{
  return mixerAvailableForWrite(ch);
}

bool i2sWrite(uint8_t ch, int16_t *p_data, uint32_t length)
{
  return mixerWrite(ch, p_data, length);
}

int8_t i2sGetEmptyChannel(void)
{
  return mixerGetEmptyChannel();
}

uint32_t i2sGetFrameSize(void)
{
  return I2S_MAX_FRAME_LEN;
}

// https://m.blog.naver.com/PostView.nhn?blogId=hojoon108&logNo=80145019745&proxyReferer=https:%2F%2Fwww.google.com%2F
//
float i2sGetNoteHz(int8_t octave, int8_t note)
{
  float hz;
  float f_note;

  if (octave < 1) octave = 1;
  if (octave > 8) octave = 8;

  if (note <  1) note = 1;
  if (note > 12) note = 12;

  f_note = (float)(note-10)/12.0f;

  hz = pow(2, (octave-1)) * 55 * pow(2, f_note);

  return hz;
}

// https://gamedev.stackexchange.com/questions/4779/is-there-a-faster-sine-function
//
float i2sSin(float x)
{
  const float B = 4 / M_PI;
  const float C = -4 / (M_PI * M_PI);

  return -(B * x + C * x * ((x < 0) ? -x : x));
}

  

bool i2sPlayNote(int8_t octave, int8_t note, uint16_t volume, uint32_t time_ms)
{
  uint32_t pre_time;
  int32_t sample_rate = I2S_SAMPLERATE_HZ;
  int32_t num_samples = 4 * I2S_SAMPLERATE_HZ / 1000;
  float sample_point;
  int16_t sample[num_samples];
  int16_t sample_index = 0;
  float div_freq;
  int8_t mix_ch;
  int32_t volume_out;


  volume = constrain(volume, 0, 100);
  volume_out = (INT16_MAX/40) * volume / 100;

  mix_ch = i2sGetEmptyChannel();

  if (mix_ch < 0)
  {
    return false;
  }

  div_freq = (float)sample_rate/(float)i2sGetNoteHz(octave, note);

  pre_time = millis();
  while(millis()-pre_time <= time_ms)
  {
    if (i2sAvailableForWrite(mix_ch) >= num_samples)
    {
      for (int i=0; i<num_samples; i++)
      {
        sample_point = i2sSin(2 * M_PI * (float)(sample_index) / ((float)div_freq));
        sample[i] = (int16_t)(sample_point * volume_out);
        sample_index = (sample_index + 1) % (int)div_freq;
      }
      i2sWrite(mix_ch, sample, num_samples);
    }
    delay(2);
  }

  return true;
}

bool i2sPlayBeep(uint32_t freq_hz, uint16_t volume, uint32_t time_ms)
{
  uint32_t pre_time;
  int32_t sample_rate = I2S_SAMPLERATE_HZ;
  int32_t num_samples = 4 * I2S_SAMPLERATE_HZ / 1000;
  float sample_point;
  int16_t sample[num_samples];
  int16_t sample_index = 0;
  float div_freq;
  int8_t mix_ch;
  int32_t volume_out;


  volume = constrain(volume, 0, 100);
  volume_out = (INT16_MAX/40) * volume / 100;

  mix_ch = i2sGetEmptyChannel();

  if (mix_ch < 0)
  {
    return false;
  }

  div_freq = (float)sample_rate/(float)freq_hz;

  pre_time = millis();
  while(millis()-pre_time <= time_ms)
  {
    if (i2sAvailableForWrite(mix_ch) >= num_samples)
    {
      for (int i=0; i<num_samples; i++)
      {
        sample_point = i2sSin(2 * M_PI * (float)(sample_index) / ((float)div_freq));
        sample[i] = (int16_t)(sample_point * volume_out);
        sample_index = (sample_index + 1) % (int)div_freq;
      }
      i2sWrite(mix_ch, sample, num_samples);
    }
    delay(2);
  }

  return true;
}

#ifdef _USE_HW_CLI
void cliI2s(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "beep") == true)
  {
    uint32_t freq;
    uint32_t time_ms;

    freq = args->getData(1);
    time_ms = args->getData(2);
    
    i2sPlayBeep(freq, 100, time_ms);

    ret = true;
  }

  if (args->argc == 4 && args->isStr(0, "note") == true)
  {
    int8_t octave;
    int8_t note;
    uint32_t time_ms;

    octave = args->getData(1);
    note = args->getData(2);
    time_ms = args->getData(3);
    
    i2sPlayNote(octave, note, 100, time_ms);

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "melody"))
  {
    uint16_t melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
    int note_durations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

    for (int i=0; i<8; i++) 
    {
      int note_duration = 1000 / note_durations[i];

      i2sPlayBeep(melody[i], 100, note_duration);
      delay(note_duration * 0.3);    
    }
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("i2s info\n");
    cliPrintf("i2s beep freq time_ms\n");
    cliPrintf("i2s melody\n");
    cliPrintf("i2s note octave note time_ms\n");
  }
}
#endif
#endif