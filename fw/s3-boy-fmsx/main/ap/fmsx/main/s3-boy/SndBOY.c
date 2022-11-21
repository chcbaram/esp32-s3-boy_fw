/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                        SndUnix.c                        **/
/**                                                         **/
/** This file contains Unix-dependent sound implementation  **/
/** for the emulation library.                              **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996-2018                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#include "EMULib.h"
#include "Sound.h"

#include "hw.h"




static sample *playData;
unsigned int   playLength;
uint16_t       toPlayLength = 0;
///static int     volLevel;
static char    stop = 0;
static audio_t audio;

int  ARGC;
char **ARGV;

/** InitAudio() **********************************************/
/** Initialize sound. Returns rate (Hz) on success, else 0. **/
/** Rate=0 to skip initialization (will be silent).         **/
/*************************************************************/
unsigned int InitAudio(unsigned int Rate,unsigned int Latency)
{
  printf("InitAudio Rate %d %d\n", Rate, Latency);

  stop = 0;

  audioOpen(&audio);
  audioSetSampleRate(Rate);

  return(Rate);
}

/** TrashAudio() *********************************************/
/** Free resources allocated by InitAudio().                **/
/*************************************************************/
void TrashAudio(void)
{
  printf("TrashAudio\n");
}

/** PauseAudio() *********************************************/
/** Pause/resume audio playback. Returns current playback   **/
/** state.                                                  **/
/*************************************************************/
int PauseAudio(int Switch)
{
  printf("PauseAudio\n");

  stop=1;

  return(stop);
}

/** GetFreeAudio() *******************************************/
/** Get the amount of free samples in the audio buffer.     **/
/*************************************************************/
unsigned int GetFreeAudio(void)
{
  return audioAvailableForWrite(&audio);
}

/** WriteAudio() *********************************************/
/** Write up to a given number of samples to audio buffer.  **/
/** Returns the number of samples written.                  **/
/*************************************************************/
unsigned int WriteAudio(sample *Data,unsigned int Length)
{
  //printf("WriteAudio %d\n", Length);

  uint32_t i;
  uint32_t written = 0;

  written = audioAvailableForWrite(&audio);
  if (written >= Length)
    written = Length;

  if (written > 0)
    audioWrite(&audio, Data, written);

  return written;
}
#if 0
/** PlayAllSound() *******************************************/
/** Render and play given number of microseconds of sound.  **/
/************************************ TO BE WRITTEN BY USER **/
void PlayAllSound(int uSec) {

  //void* tempPtr = (void*)0x1234;
  //toPlayLength = 2*uSec*AUDIO_SAMPLE_RATE/1000000;
  //xQueueSend(audioQueue, &tempPtr, portMAX_DELAY);

  printf("PlayAllSound\n");
}
#endif
