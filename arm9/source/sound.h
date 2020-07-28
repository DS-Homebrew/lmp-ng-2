#ifndef SOUND_H
#define SOUND_H

#include "file.h"

#define SOUND_MAX_SAMPLES_TRANSFER 1024
#define SOUND_BUFFER_SAMPLES 16384

enum state { PLAYING, PAUSED, STOPPED, SEEKING, FINISHING, WAITING, ERROR };

#ifndef SOUNDC
extern
#endif
char *pcmL, *pcmR;

#ifndef SOUNDC
extern
#endif
char *pcmbufL, *pcmbufR;

#ifndef SOUNDC
extern
#endif
int buffer_samples, buffer_end, buffer_pos, buffer_size, buffer_lowest;

#ifndef SOUNDC
extern
#endif
unsigned sound_channels, sound_samplerate, sound_bps;

#ifndef SOUNDC
extern
#endif
enum format format;

#ifndef SOUNDC
extern
#endif
FILE *sound_playfile;

#ifndef SOUNDC
extern
#endif
enum state state;

#ifndef SOUNDC
extern
#endif
u32 sound_elapsed, sound_time;

void sound_start(FILE *fp_arg, enum format format_arg);
void sound_stop(void);
void sound_rewind(void);
void sound_forward(void);
void sound_finishing(void);
void sound_playpause(void);
void sound_init(void);
u32 sound_position(void);
u32 sound_size(void);
void sound_flush(void);

#endif
