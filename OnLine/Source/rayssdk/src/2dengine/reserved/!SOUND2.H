/*
        SOUND.H
*/
#ifndef SOUND_H_INCLUDE
#define SOUND_H_INCLUDE         1
#include "pack.h"

#define SAMPLEMAX               16      // number of static sample
#define SOUNDSPS             22050      // samples per second
#define SOUNDBPS                16      // bits per sample

#define PLAYLOOP        0x00000001      // DSBPLAY_LOOPING
#define PANLEFT             -10000      // DSBPAN_LEFT
#define PANRIGHT             10000      // DSBPAN_RIGHT
#define VOLMIN              -10000      // DSBVOLUME_MIN


typedef struct  SAMPLE_STRUCT   SAMPLE,*LPSAMPLE;
typedef struct  MUSIC_STRUCT    MUSIC,*LPMUSIC;

struct  SAMPLE_STRUCT
{
        int bufferbyte; // sample data length
        int number;     // sample id
        char buffer[1]; // sample data buffer pointer
};

struct  MUSIC_STRUCT
{
        int bufferbyte; // data length
        long offset;    // data offset
        PACKFILE *f;    // packfile pointer
};

void    destroy_sample(SAMPLE * wd);
void    destroy_music(MUSIC * wd);
int     play_sample(SAMPLE *lpwave,int loop,int Pan,int Vol);
void    play_music(MUSIC *lpswave,int loop,long Pan,long Vol);
void    stop_sample(SAMPLE *lpwave);
void    stop_music(void);
void    adjust_vol(long Vol);
int     adjust_sample(SAMPLE *lpwave,long Pan,long Volume);
int     adjust_music(long Pan,long Volume);
SAMPLE* load_wav(char * name);
MUSIC*  load_music(char *name);
void    sound_on(void);
void    sound_off(void);
void    music_on(void);
void    music_off(void);


#endif  // SOUND_H_INCLUDE
