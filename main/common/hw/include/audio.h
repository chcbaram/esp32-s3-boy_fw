/*
 * audio.h
 *
 *  Created on: 2020. 8. 9.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_AUDIO_H_
#define SRC_COMMON_HW_INCLUDE_AUDIO_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_AUDIO




typedef struct
{
  bool    is_open;
  uint8_t ch;
  const char *p_file_name;

  int8_t   octave;
  int8_t   note;
  uint32_t note_time;
  uint8_t  volume;
  uint32_t freq;
} audio_t;


bool audioInit(void);
bool audioIsInit(void);

bool audioOpen(audio_t *p_audio);
bool audioClose(audio_t *p_audio);

bool audioPlayFile(audio_t *p_audio, const char *p_name, bool wait);
bool audioStopFile(audio_t *p_audio);
bool audioIsPlaying(audio_t *p_audio);

uint32_t audioAvailableForWrite(audio_t *p_audio);
bool     audioWrite(audio_t *p_audio, int16_t *p_wav_data, uint32_t wav_len);

bool audioPlayBeep(uint32_t freq, uint8_t volume, uint32_t time_ms);

bool audioPlayNote(int8_t octave, int8_t note, uint32_t time_ms);
void audioSetNoteVolume(uint8_t volume);
uint8_t audioGetNoteVolume(void);
void audioSetSampleRate(uint32_t sample_rate);
#endif


#ifdef __cplusplus
 }
#endif


#endif /* SRC_COMMON_HW_INCLUDE_AUDIO_H_ */
