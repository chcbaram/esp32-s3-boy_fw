/*
 * audio.c
 *
 *  Created on: 2020. 8. 9.
 *      Author: Baram
 */




#include "audio.h"
#include "cli.h"



#ifdef _USE_HW_AUDIO
#include "i2s.h"
#include "button.h"
#include "buzzer.h"


#define AUDIO_CMD_MAX_CH        HW_AUDIO_CMD_MAX_CH


#define lock()      xSemaphoreTake(mutex_lock, portMAX_DELAY);
#define unLock()    xSemaphoreGive(mutex_lock);


typedef enum
{
  AUDIO_PLAY_FILE,
  AUDIO_PLAY_NOTE,
  AUDIO_PLAY_BEEP,
  AUDIO_NONE,
} AudioCmtTable;

typedef struct
{
  bool is_used;
  bool is_busy;
  bool request_stop;

  uint16_t  cmd;
  audio_t   *audio;
} audio_cmd_t;


#ifdef _USE_HW_CLI
static void cliAudio(cli_args_t *args);
#endif
static int8_t audioGetCmdChannel(void);
static void audioProcessPlayFile(audio_cmd_t *p_cmd);
static void audioProcessPlayNote(audio_cmd_t *p_cmd);
static void audioProcessPlayBeep(audio_cmd_t *p_cmd);
static void audioThread(void *argument);


static bool is_init = false;
static SemaphoreHandle_t mutex_lock;
static uint8_t note_volume = 50;

static QueueHandle_t msg_cmd_q[AUDIO_CMD_MAX_CH];



static audio_t     audio_note;
static audio_cmd_t audio_cmd[AUDIO_CMD_MAX_CH];





bool audioInit(void)
{
  bool ret = true;


  for (int i=0; i<AUDIO_CMD_MAX_CH; i++)
  {
    audio_cmd[i].is_used = false;
    audio_cmd[i].is_busy = false;
    audio_cmd[i].request_stop = false;
  }

  mutex_lock = xSemaphoreCreateMutex();

  audioOpen(&audio_note);


  for (int i=0; i<AUDIO_CMD_MAX_CH; i++)
  {
    msg_cmd_q[i] = xQueueCreate(3, sizeof(audio_cmd_t *));
    if (msg_cmd_q[i] == NULL) 
    {
      logPrintf("[NG] xQueueCreate(%d)\n", i);
      return false;
    }
    if (xTaskCreate(audioThread, "audioThread", _HW_DEF_RTOS_THREAD_MEM_AUDIO, (void * const)i, _HW_DEF_RTOS_THREAD_PRI_AUDIO, NULL) != pdPASS)
    {
      logPrintf("[NG] audioThread()\n");   
    }
  }

  is_init = ret;

#ifdef _USE_HW_CLI
  cliAdd("audio", cliAudio);
#endif  
  return true;
}

bool audioIsInit(void)
{
  return is_init;
}

void audioThread(void *argument)
{
  uint32_t ch = (uint32_t)argument;
  audio_cmd_t *p_cmd;


  logPrintf("[__] audioThread %d\n", ch);

  while(1)
  {
    p_cmd = NULL;
    if (xQueueReceive(msg_cmd_q[ch], &p_cmd, portMAX_DELAY) == pdTRUE)
    {
      if (p_cmd != NULL && p_cmd->is_used == true)
      {
        switch(p_cmd->cmd)
        {
          case AUDIO_PLAY_FILE:
            audioProcessPlayFile(p_cmd);
            break;

          case AUDIO_PLAY_NOTE:
            audioProcessPlayNote(p_cmd);
            break;

          case AUDIO_PLAY_BEEP:
            audioProcessPlayBeep(p_cmd);
            break;
        }
      }
    }
  }
}

int8_t audioGetCmdChannel(void)
{
  int8_t ret = -1;


  for (int i=0; i<AUDIO_CMD_MAX_CH; i++)
  {
    if (audio_cmd[i].is_used != true)
    {
      ret = i;
      break;
    }
  }

  return ret;
}

bool audioOpen(audio_t *p_audio)
{
  int8_t ch;
  bool ret = false;


  p_audio->is_open = false;

  ch = audioGetCmdChannel();
  if (ch >= 0)
  {
    p_audio->ch = ch;
    p_audio->is_open = true;
    audio_cmd[ch].is_used = true;
    ret = true;
  }

  return ret;
}

bool audioClose(audio_t *p_audio)
{
  bool ret = true;

  if (p_audio->is_open == true)
  {
    audioStopFile(p_audio);
    p_audio->is_open = false;
    audio_cmd[p_audio->ch].is_used = false;
  }

  return ret;
}

void audioSetNoteVolume(uint8_t volume)
{
  note_volume = constrain(volume, 0, 100);
}

uint8_t audioGetNoteVolume(void)
{
  return note_volume;
}

bool audioPlayFile(audio_t *p_audio, const char *p_name, bool wait)
{
  bool ret = true;
  uint8_t ch;


  if (p_audio == NULL)
  {
    return false;
  }
  if (p_audio->is_open != true)
  {
    return false;
  }

  ch = p_audio->ch;

  if (audio_cmd[ch].is_used)
  {
    if (audio_cmd[ch].is_busy)
    {
      audioStopFile(p_audio);
    }

    audio_cmd_t *p_cmd;

    p_cmd = &audio_cmd[ch];

    p_cmd->cmd = AUDIO_PLAY_FILE;
    p_cmd->audio = p_audio;
    p_cmd->is_busy = true;
    p_cmd->request_stop = false;
    p_cmd->audio->p_file_name = p_name;

    if (xQueueSend(msg_cmd_q[ch], &p_cmd, 0) != pdTRUE) 
    {
      logPrintf("[NG] xQueueSend\n");
    }

    if (wait == true)
    {
      while(1)
      {
        if (p_cmd->is_busy != true)
        {
          break;
        }
        delay(5);
      }
    }
  }

  return ret;
}

bool audioStopFile(audio_t *p_audio)
{
  bool ret = true;

  if (p_audio == NULL)
  {
    return false;
  }
  if (p_audio->is_open != true)
  {
    return false;
  }

  if (audio_cmd[p_audio->ch].is_used == true)
  {
    audio_cmd[p_audio->ch].request_stop = true;

    while(1)
    {
      if (audio_cmd[p_audio->ch].is_busy == false)
      {
        break;
      }
      delay(1);
    }
    audio_cmd[p_audio->ch].request_stop = false;
  }

  return ret;
}

bool audioIsPlaying(audio_t *p_audio)
{
  bool ret = false;

  if (p_audio == NULL)
  {
    return false;
  }
  if (p_audio->is_open != true)
  {
    return false;
  }

  if (audio_cmd[p_audio->ch].is_used == true)
  {
    ret = audio_cmd[p_audio->ch].is_busy;
  }

  return ret;
}

uint32_t audioAvailableForWrite(audio_t *p_audio)
{
  uint32_t ret;

  ret = i2sAvailableForWrite(p_audio->ch);

  return ret;
}

bool audioWrite(audio_t *p_audio, int16_t *p_wav_data, uint32_t wav_len)
{
  bool ret;

  ret = i2sWrite(p_audio->ch, p_wav_data, wav_len);

  return ret;
}

void audioSetSampleRate(uint32_t sample_rate)
{
  i2sSetSampleRate(sample_rate);
}

bool audioPlayNote(int8_t octave, int8_t note, uint32_t time_ms)
{
  uint8_t ch;
  audio_t *p_audio;


  p_audio = &audio_note;


  ch = p_audio->ch;

  if (audio_cmd[ch].is_used)
  {
    if (audio_cmd[ch].is_busy)
    {
      audioStopFile(p_audio);
    }

    audio_cmd_t *p_cmd;

    p_cmd = &audio_cmd[ch];

    p_cmd->cmd = AUDIO_PLAY_NOTE;
    p_cmd->audio = p_audio;
    p_cmd->is_busy = true;
    p_cmd->request_stop = false;
    p_cmd->audio->p_file_name = NULL;
    p_cmd->audio->octave = octave;
    p_cmd->audio->note = note;
    p_cmd->audio->note_time = time_ms;

    if (xQueueSend(msg_cmd_q[ch], &p_cmd, 0) != pdTRUE) 
    {
      logPrintf("[NG] xQueueSend\n");
    }
  }

  return true;
}

bool audioPlayBeep(uint32_t freq, uint8_t volume, uint32_t time_ms)
{
  uint8_t ch;
  audio_t *p_audio;


  p_audio = &audio_note;


  ch = p_audio->ch;

  if (audio_cmd[ch].is_used)
  {
    if (audio_cmd[ch].is_busy)
    {
      audioStopFile(p_audio);
    }

    audio_cmd_t *p_cmd;

    p_cmd = &audio_cmd[ch];

    p_cmd->cmd = AUDIO_PLAY_BEEP;
    p_cmd->audio = p_audio;
    p_cmd->is_busy = true;
    p_cmd->request_stop = false;
    p_cmd->audio->p_file_name = NULL;
    p_cmd->audio->freq = freq;
    p_cmd->audio->volume = constrain(volume, 0, 100);
    p_cmd->audio->note_time = time_ms;

    if (xQueueSend(msg_cmd_q[ch], &p_cmd, 0) != pdTRUE) 
    {
      logPrintf("[NG] xQueueSend\n");
    }
  }

  return true;
}

void audioProcessPlayFile(audio_cmd_t *p_cmd)
{
  uint8_t ch;
  FILE *fp;
  uint32_t r_len;
  int16_t buf_frame[32*8];

  
  while(1)
  {
    fp = fopen(p_cmd->audio->p_file_name, "r");
    if (fp == NULL)
    {
      logPrintf("[NG] fopen fail\n");
      break;
    }
    fseek(fp, 44*2, SEEK_SET);


    r_len = 32*8;
    ch = p_cmd->audio->ch;
    while(p_cmd->request_stop == false)
    {
      int len;

      if (i2sAvailableForWrite(ch) >= r_len)
      {
        len = fread(buf_frame, 1, r_len*2, fp);

        if (len != r_len*2)
        {
          break;
        }
        i2sWrite(ch, (int16_t *)buf_frame, r_len);
      }
      delay(1);
    }
    memset(buf_frame, 0, r_len*2);
    i2sWrite(ch, (int16_t *)buf_frame, r_len);


    fclose(fp);
    break;
  }

  p_cmd->request_stop = false;
  p_cmd->is_busy = false;
}

void audioProcessPlayNote(audio_cmd_t *p_cmd)
{
  i2sPlayNote(p_cmd->audio->octave, p_cmd->audio->note, note_volume, p_cmd->audio->note_time);
  p_cmd->request_stop = false;
  p_cmd->is_busy = false;
}

void audioProcessPlayBeep(audio_cmd_t *p_cmd)
{
  i2sPlayBeep(p_cmd->audio->freq, p_cmd->audio->volume, p_cmd->audio->note_time);
  p_cmd->request_stop = false;
  p_cmd->is_busy = false;
}

#ifdef _USE_HW_CLI
void cliAudio(cli_args_t *args)
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

    cliPrintf("beep %d hz, %d ms\n", freq, time_ms);
    audioPlayBeep(freq, 100, time_ms);

    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "file") == true)
  {
    const char *file_name;
    char file_path[128];
    audio_t audio;

    file_name = args->getStr(1);

    sprintf(file_path, "/sdcard/%s", file_name);

    audioOpen(&audio);
    

    cliPrintf("play file %s\n", file_path);
    audioPlayFile(&audio, file_path, false);

    while(audioIsPlaying(&audio))
    {
      if (buttonGetPressedEvent(_BTN_A) == true)
      {
        buzzerBeep(100);
      }
      delay(1);
    }
    
    audioClose(&audio);
    ret = true;
  }

  
  if (ret != true)
  {
    cliPrintf("audio info\n");
    cliPrintf("audio beep freq time_ms\n");
    cliPrintf("audio file filename\n");
  }
}
#endif
#endif