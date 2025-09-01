/*
        SOUND.CPP

*/
#include "winway.h"
#include "mainfun.h"
#include "wvari.h"
#include "sound.h"
#include "wmain.h"

#define FailMsg         ErrorExit


#define WSSB_CAPS       DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME
#define DSSCL           DSSCL_PRIORITY  //|DSSCL_EXCLUSIVE   //sound cooperation level

#define STATICTIME      10
#define STREAMTIME      20
#define SBUFFERSIZE     ((((SOUNDBPS*2)>>3)*SOUNDSPS*STATICTIME/10)&0xfffffff8)
#define MBUFFERSIZE     ((SOUNDSPS*((SOUNDBPS*2)>>3)*STREAMTIME/10)&0xfffffff8)

typedef struct {
        LPDIRECTSOUNDBUFFER lpDSB;      // Direct Sound Buffer
        LPDIRECTSOUNDNOTIFY lpDSN;      // Direct Sound Buffer Notify
        SAMPLE *wave;                   // Sample wave pointer
        LONG    Pan;                    // pan
        LONG    Volume;                 // volume
        int     current;                // prepare play data offset
        int     Status;                 // play status
        DSBPOSITIONNOTIFY   dsbpn[2];   // event array
} STATICSOUND;

typedef struct {
        LPDIRECTSOUNDBUFFER lpDSB;      // Direct Sound Buffer
        LPDIRECTSOUNDNOTIFY lpDSN;      // Direct Sound Buffer Notify
        MUSIC  *wave;                   // Music wave pointer
        LONG    Pan;                    // pan
        LONG    Volume;                 // volume
        int     Status;                 // play status
        DSBPOSITIONNOTIFY   dsbpn[2];   // event array
} STREAMSOUND;

LPDIRECTSOUND       lpDS=NULL;
LPDIRECTSOUNDBUFFER lpDSB=NULL;
WAVEFORMATEX        wf;

STATICSOUND StaticSound[SAMPLEMAX];
STREAMSOUND StreamSound;

HANDLE  SoundEvent[SAMPLEMAX*2+2];
HANDLE  DS_thread=NULL;
DWORD   ThreadId=0;

int     sound_init=0,SOUND_ON=1,MUSIC_ON=1;

int     SoundNextStatic(STATICSOUND *ss,int point,int size);
int     SoundNextStream(int point,int size);
DWORD WINAPI    SoundThread(LPVOID p);
char    GetWaveData(PACKFILE * hfile,char * buffer,int blocksize);
char    GetWaveFormat(PACKFILE * hfile,int * blocksize,int * wavesize);

int     init_sound(void)
{
        DSBUFFERDESC dsbdesc;
        STATICSOUND *ss;
        int i;
        HANDLE event;

        //  Create DirectSound
        if(FAILED(DirectSoundCreate(NULL,&lpDS,NULL)))
           {
           // DirectSound create fail
           sound_init=SOUND_ON=MUSIC_ON=0;
           return 0;
           }
        if(FAILED(IDirectSound_SetCooperativeLevel(lpDS,WinG.hWnd,DSSCL)))
           {
           IDirectSound_Release(lpDS);
           lpDS=NULL;
           sound_init=SOUND_ON=MUSIC_ON=0;
           return FailMsg("DirectSound cooperation level setup fail");
           }

        //  Primary Buffer Desc
        ZeroMemory(&dsbdesc,sizeof(dsbdesc));
        dsbdesc.dwSize=sizeof(dsbdesc);
        dsbdesc.dwFlags=DSBCAPS_PRIMARYBUFFER;

        //  Create Primary Buffer
        if(FAILED(IDirectSound_CreateSoundBuffer(lpDS,&dsbdesc,&lpDSB,NULL)))
           return FailMsg("Primary directsound buffer create fail");

        //  Set Primary Buffer Format
        //  if this format is not the same as statis or stream buffer's ,
        //  there may be noise !!!
        wf.wFormatTag=WAVE_FORMAT_PCM;
        wf.nChannels=2;
        wf.wBitsPerSample=SOUNDBPS;
        wf.nSamplesPerSec=SOUNDSPS;
        wf.nBlockAlign=(SOUNDBPS*2)>>3;
        wf.nAvgBytesPerSec=SOUNDSPS*wf.nBlockAlign;
        wf.cbSize=0;
        if(FAILED(IDirectSoundBuffer_SetFormat(lpDSB,&wf)))
           {
           IDirectSoundBuffer_Release(lpDSB);
           lpDSB=NULL;
           IDirectSound_Release(lpDS);
           lpDS=NULL;
           sound_init=SOUND_ON=MUSIC_ON=0;
           return FailMsg("Setup directsound wave format fail");
           }

        IDirectSoundBuffer_Play(lpDSB,0,0,DSBPLAY_LOOPING);

        //  create wave stream buffer
        dsbdesc.dwSize=sizeof(dsbdesc);
        dsbdesc.dwFlags=DSBCAPS_GETCURRENTPOSITION2|
                                                DSBCAPS_CTRLPOSITIONNOTIFY|WSSB_CAPS;
        dsbdesc.dwBufferBytes=MBUFFERSIZE;
        dsbdesc.lpwfxFormat=&wf;
        dsbdesc.dwReserved=0;
        if(FAILED(IDirectSound_CreateSoundBuffer(lpDS,&dsbdesc,&(StreamSound.lpDSB),NULL))){
                IDirectSoundBuffer_Release(lpDSB);
                lpDSB=NULL;
                IDirectSound_Release(lpDS);
                lpDS=NULL;
                return FailMsg("Create stream buffer fail");
        }
        if(FAILED(IDirectSoundNotify_QueryInterface(StreamSound.lpDSB,IID_IDirectSoundNotify,(void **)&StreamSound.lpDSN)))
           {
           IDirectSoundBuffer_Release(lpDSB);
           lpDSB=NULL;
           IDirectSound_Release(lpDS);
           lpDS=NULL;
           return FailMsg("Setup wave stream event fail");
           }
        event=CreateEvent(NULL,FALSE,FALSE,NULL);
        SoundEvent[SAMPLEMAX*2]=event;
        StreamSound.dsbpn[0].hEventNotify=event;
        StreamSound.dsbpn[0].dwOffset=0;
        event=CreateEvent(NULL,FALSE,FALSE,NULL);
        SoundEvent[SAMPLEMAX*2+1]=event;
        StreamSound.dsbpn[1].hEventNotify=event;
        StreamSound.dsbpn[1].dwOffset=MBUFFERSIZE>>1;
        StreamSound.Pan=0;
        StreamSound.Volume=0;
        if(FAILED(IDirectSoundNotify_SetNotificationPositions(StreamSound.lpDSN,2,StreamSound.dsbpn)))
           {
           IDirectSoundBuffer_Release(lpDSB);
           lpDSB=NULL;
           IDirectSound_Release(lpDS);
           lpDS=NULL;
           return FailMsg("Setup wave stream buffer event fail");
           }
        StreamSound.wave=NULL;

        //  setup buffer
        dsbdesc.dwSize=sizeof(dsbdesc);
        dsbdesc.dwFlags=DSBCAPS_GETCURRENTPOSITION2
                          | DSBCAPS_CTRLPOSITIONNOTIFY|WSSB_CAPS;
        dsbdesc.dwBufferBytes=SBUFFERSIZE;
        dsbdesc.lpwfxFormat=&wf;
        dsbdesc.dwReserved=0;

        ss=&StaticSound[0];
        for(i=0;i<SAMPLEMAX;i++,ss++)
           {
           // create static wave buffer
           if(FAILED(IDirectSound_CreateSoundBuffer(lpDS,&dsbdesc,&(ss->lpDSB),NULL)))
              {
              IDirectSoundBuffer_Release(lpDSB);
              lpDSB=NULL;
              IDirectSound_Release(lpDS);
              lpDS=NULL;
              sound_init=SOUND_ON=0;
              return FailMsg("Create static sound buffer fail");
              }
           if(FAILED(IDirectSoundBuffer_QueryInterface((ss->lpDSB),IID_IDirectSoundNotify,(void **)&ss->lpDSN)))
              {
              IDirectSoundBuffer_Release(lpDSB);
              lpDSB=NULL;
              IDirectSound_Release(lpDS);
              lpDS=NULL;
              sound_init=SOUND_ON=0;
              return FailMsg("Setup static sound buffer notify fail");
              }
           ss->Status=-1;
           event=CreateEvent(NULL,FALSE,FALSE,NULL);
           SoundEvent[i<<1]=event;
           ss->dsbpn[0].hEventNotify=event;
           ss->dsbpn[0].dwOffset=0;
           event=CreateEvent(NULL,FALSE,FALSE,NULL);
           SoundEvent[(i<<1)+1]=event;
           ss->dsbpn[1].hEventNotify=event;
           ss->dsbpn[1].dwOffset=SBUFFERSIZE>>1;
           ss->Pan=0;
           ss->Volume=0;
           if(FAILED(IDirectSoundNotify_SetNotificationPositions(ss->lpDSN,2,ss->dsbpn)))
              {
              IDirectSoundBuffer_Release(lpDSB);
              lpDSB=NULL;
              IDirectSound_Release(lpDS);
              lpDS=NULL;
              return FailMsg("Setup static sound buffer notify fail");
              }
           }

        //  create thread
        if(!(DS_thread=CreateThread(NULL,0,SoundThread,NULL,0,&ThreadId)))
           {
           IDirectSoundBuffer_Release(lpDSB);
           lpDSB=NULL;
           IDirectSound_Release(lpDS);
           lpDS=NULL;
           sound_init=SOUND_ON=0;
           return FailMsg("Create sound manager thread fail");
           }
        sound_init=SOUND_ON=1;
        return 0;
}


void    adjust_vol(long Vol)
{
        if(lpDSB) IDirectSoundBuffer_SetVolume(lpDSB,Vol);
}


void    active_sound(int active)
{
        int i;
        DWORD status;

        if (!active) return;

        if (lpDS==NULL || lpDSB==NULL) sound_init=0;

        if(sound_init)
           {
           // restore primary sound buffer
           if(FAILED(IDirectSoundBuffer_Restore(lpDSB)))
              {
              IDirectSoundBuffer_Release(lpDSB);
              lpDSB=NULL;
              IDirectSound_Release(lpDS);
              lpDS=NULL;
              return;
              }
           // restore static sound buffer
           for(i=0;i<SAMPLEMAX;i++)
              {
              IDirectSoundBuffer_GetStatus(StaticSound[i].lpDSB,&status);
              if(status&DSBSTATUS_BUFFERLOST)
                 IDirectSoundBuffer_Restore(StaticSound[i].lpDSB);
              }
           // restore stream sound buffer
           IDirectSoundBuffer_GetStatus(StreamSound.lpDSB,&status);
           if(status&DSBSTATUS_BUFFERLOST)
              IDirectSoundBuffer_Restore(StreamSound.lpDSB);
           }
}

void    sound_off(void)
{
        int i;
        if (!sound_init || !SOUND_ON) return;

        for(i=0;i<SAMPLEMAX;i++)
           {
           if(StaticSound[i].lpDSB)
              {
              StaticSound[i].Status=-1;
              IDirectSoundBuffer_Stop(StaticSound[i].lpDSB);
              IDirectSoundBuffer_SetCurrentPosition(StaticSound[i].lpDSB,0);
              }
           }
        SOUND_ON=0;
}

void    sound_on(void)
{
        if (sound_init) SOUND_ON=1;
}

void    music_on(void)
{
        if (sound_init) MUSIC_ON=1;
}


void    music_off(void)
{
        if (!sound_init || !MUSIC_ON) return;
        if(StreamSound.lpDSB)
           {
           StreamSound.Status=-1;
           IDirectSoundBuffer_Stop(StreamSound.lpDSB);
           IDirectSoundBuffer_SetCurrentPosition(StreamSound.lpDSB,0);
           }
        MUSIC_ON=0;
}


void    free_sound()
{
        int i;

        if (sound_init)
           {
           sound_off();
           music_off();
           if(DS_thread)
              {
              TerminateThread(DS_thread,0);
              Sleep(200);
              }
           IDirectSoundBuffer_Release(StreamSound.lpDSN);
           IDirectSoundBuffer_Release(StreamSound.lpDSB);
           CloseHandle(SoundEvent[SAMPLEMAX*2]);
           CloseHandle(SoundEvent[SAMPLEMAX*2+1]);
           for(i=0;i<SAMPLEMAX;i++)
              {
              IDirectSoundNotify_Release(StaticSound[i].lpDSN);
              IDirectSoundBuffer_Release(StaticSound[i].lpDSB);
              CloseHandle(SoundEvent[i<<1]);
              CloseHandle(SoundEvent[(i<<1)+1]);
              }
           }
        if(lpDSB){ IDirectSoundBuffer_Release(lpDSB); lpDSB=NULL; }
        if(lpDS){ IDirectSound_Release(lpDS); lpDS=NULL; }
}

int     SoundNextStream(int point,int size)
{
        char * buffer;
        int bufferbyte,s;

        if (StreamSound.Status<0)
           {
           // stop
           IDirectSoundBuffer_Stop(StreamSound.lpDSB);
           IDirectSoundBuffer_SetCurrentPosition(StreamSound.lpDSB,0);
           pack_fseek(StreamSound.wave->f,StreamSound.wave->offset,SEEK_SET);
           StreamSound.wave=NULL;
           return 0;
           }

        if(FAILED(IDirectSoundBuffer_Lock(StreamSound.lpDSB,point,size,(void **)&buffer,(DWORD *)&bufferbyte,NULL,NULL,0)))
           return FailMsg("Lock sound stream fail");
        if((s=pack_fread(buffer,bufferbyte,StreamSound.wave->f))<bufferbyte)
           {
           if (StreamSound.Status==PLAYLOOP)
              {
              pack_fseek(StreamSound.wave->f,StreamSound.wave->offset,SEEK_SET);
              pack_fread(buffer,bufferbyte,StreamSound.wave->f);
              }
           else
              {
              FillMemory(buffer+s,bufferbyte-s,0);
              StreamSound.Status=-1;
              }
           }
        IDirectSoundBuffer_Unlock(StreamSound.lpDSB,buffer,bufferbyte,NULL,0);
        return 0;
}

int     adjust_music(long Pan,long Volume)
{
        if (StreamSound.Pan!=Pan)
           {
           if(FAILED(IDirectSoundBuffer_SetPan(StreamSound.lpDSB,Pan)))
              return -1;
           StreamSound.Pan=Pan;
           }
        if (StreamSound.Volume!=Volume)
           {
           if(FAILED(IDirectSoundBuffer_SetVolume(StreamSound.lpDSB,Volume)))
              return -1;
           StreamSound.Volume=Volume;
           }
        return 0;
}

void    play_music(MUSIC *lpswave,int loop,long Pan,long Vol)
{
        if (!MUSIC_ON || !sound_init) return;
        if (StreamSound.wave) stop_music();
        StreamSound.wave=lpswave;
        StreamSound.Status=loop;

        if(SoundNextStream(0,MBUFFERSIZE>>1)) return;

        IDirectSoundBuffer_SetCurrentPosition(StreamSound.lpDSB,0);
        IDirectSoundBuffer_SetPan(StreamSound.lpDSB,Pan);
        IDirectSoundBuffer_SetVolume(StreamSound.lpDSB,Vol);
        IDirectSoundBuffer_Play(StreamSound.lpDSB,0,0,DSBPLAY_LOOPING);
}


int     adjust_sample(SAMPLE *lpwave,long Pan,long Volume)
{
        STATICSOUND * ss;
        int i=lpwave->number;
        if (i<0) return 0;
        ss=StaticSound+i;
        if(!ss->lpDSB) return -1;
        if (ss->Pan!=Pan)
           {
           if(FAILED(IDirectSoundBuffer_SetPan(ss->lpDSB,Pan)))
              return -1;
           ss->Pan=Pan;
           }
        if (ss->Volume!=Volume)
           {
           if(FAILED(IDirectSoundBuffer_SetVolume(ss->lpDSB,Volume)))
              return -1;
           ss->Volume=Volume;
           }
        return 0;
}

void    stop_sample(SAMPLE *lpwave)
{
        STATICSOUND * ss;
        int i=lpwave->number;
        if (i<0) return;
        ss=&StaticSound[i];
        IDirectSoundBuffer_Stop(ss->lpDSB);
        IDirectSoundBuffer_SetCurrentPosition(ss->lpDSB,0);
        ss->Status=-1;
        ss->wave=NULL;
}

void    stop_music()
{
        if (StreamSound.wave==NULL) return;
        IDirectSoundBuffer_Stop(StreamSound.lpDSB);
        IDirectSoundBuffer_SetCurrentPosition(StreamSound.lpDSB,0);
        StreamSound.Status=-1;
        pack_fseek(StreamSound.wave->f,StreamSound.wave->offset,SEEK_SET);
        StreamSound.wave=NULL;
}

char    GetWaveFormat(PACKFILE * hfile,int * blocksize,int * wavesize)
{
        int i,point;
        DWORD waveid;

        struct BLOCKHEAD{
                DWORD id;
                DWORD size;
        }blockhead,riffhead;


        *wavesize=0;
        while(1)
           {
           i=0;
           if((pack_fread(&riffhead,sizeof(blockhead),hfile))!=sizeof(blockhead))
              return -1;
           if(riffhead.id!='FFIR')
              {
              pack_fseek(hfile,riffhead.size,SEEK_CUR);
              continue;
              }
           if((pack_fread(&waveid,sizeof(waveid),hfile))!=sizeof(waveid))
              return -1;
           i+=sizeof(waveid);
           if(waveid!='EVAW')
              {
              pack_fseek(hfile,riffhead.size-i,SEEK_CUR);
              continue;
              }
           while(1)
              {
              if(i>=(int)riffhead.size) break;
              if((pack_fread(&blockhead,sizeof(blockhead),hfile))!=sizeof(blockhead))
                 return -1;
              i+=sizeof(blockhead);
              if(blockhead.id!=' tmf')
                 {
                 pack_fseek(hfile,blockhead.size,SEEK_CUR);
                 i+=blockhead.size;
                 continue;
                 }
//            if((pack_fread(wf,sizeof(WAVEFORMAT)+2,hfile))!=(sizeof(WAVEFORMAT)+2))
//               return FALSE;
//            i+=sizeof(WAVEFORMAT)+2;
              if((pack_fread(&wf,blockhead.size,hfile))!=blockhead.size)
                 return FALSE;
              i+=blockhead.size;
              wf.cbSize=0;
              *blocksize=(int)riffhead.size-i;

              point=pack_ftell(hfile);

              while(1)
                 {
                 if(i>=(int)riffhead.size) break;
                 if((pack_fread(&blockhead,sizeof(blockhead),hfile))!=sizeof(blockhead))
                     return FALSE;
                 i+=sizeof(blockhead);
                 if(blockhead.id=='atad') *wavesize+=blockhead.size;
                 pack_fseek(hfile,blockhead.size,SEEK_CUR);
                 i+=blockhead.size;
                 }
              pack_fseek(hfile,point,SEEK_SET);
              return 0;
              }
           }
}


char    GetWaveData(PACKFILE * hfile,char * buffer,int blocksize)
{
        struct {
                DWORD id;
                DWORD size;
        } blockhead;

        int i=0,j=0;

        while(1)
           {
           if(i>=blocksize) break;
           if((pack_fread(&blockhead,sizeof(blockhead),hfile))!=sizeof(blockhead))
              return -1;
           i+=sizeof(blockhead);
           if(blockhead.id!='atad')
              {
              pack_fseek(hfile,blockhead.size,SEEK_CUR);
              i+=blockhead.size;
              continue;
              }
           if((pack_fread(buffer+=j,blockhead.size,hfile))!=blockhead.size)
              return -1;
           j+=blockhead.size;
           i+=blockhead.size;
           }
        return 0;
}

SAMPLE* load_wav(char * name)
{
        PACKFILE * f;
        int blocksize,bufferbyte;
        SAMPLE * wd;

        if((f=pack_fopen(name))!=NULL)
           {
           if (GetWaveFormat(f,&blocksize,&bufferbyte))
              {
              pack_fclose(f);
              return NULL;
              }
           wd=(SAMPLE *)malloc(sizeof(SAMPLE)+bufferbyte-1);
           wd->bufferbyte=bufferbyte;
           wd->number=-1;
           if(GetWaveData(f,wd->buffer,blocksize))
              {
              free(wd);
              pack_fclose(f);
              return NULL;
              }
           pack_fclose(f);
           return wd;
           }
        return NULL;
}

MUSIC*  load_music(char * name)
{
        PACKFILE * f;
        int blocksize,bufferbyte;
        MUSIC * wd;

        if((f=pack_fopen(name))!=NULL)
           {
           if (GetWaveFormat(f,&blocksize,&bufferbyte))
              {
              pack_fclose(f);
              return NULL;
              }
           wd=(MUSIC *)malloc(sizeof(MUSIC));
           wd->f=f;
           wd->offset=pack_ftell(f);
           return wd;
           }
        return NULL;
}

void    destroy_sample(SAMPLE * wd)
{
        stop_sample(wd);
        free(wd);
}

void    destroy_music(MUSIC * wd)
{
        if (StreamSound.wave==wd)
           stop_music();
        pack_fclose(wd->f);
        free(wd);
}

int     play_sample(SAMPLE *lpwave,int loop,int Pan,int Vol)
{
        int i=0;
        STATICSOUND *ss;

        if (!SOUND_ON || !sound_init) return -1;

        for (i=0;i<SAMPLEMAX;i++)
           {
           ss=&StaticSound[i];
           if (ss->Status<0) break;
           }
        if (i==SAMPLEMAX) return -1;
        ss->current=0;
        ss->wave=lpwave;
        ss->Status=loop;
        lpwave->number=i;
        if(SoundNextStatic(ss,0,SBUFFERSIZE>>1)) return -1;
        IDirectSoundBuffer_SetCurrentPosition(ss->lpDSB,0);
        IDirectSoundBuffer_SetPan(ss->lpDSB,Pan);
        IDirectSoundBuffer_SetVolume(ss->lpDSB,Vol);
        if(FAILED(IDirectSoundBuffer_Play(ss->lpDSB,0,0,DSBPLAY_LOOPING)))
           return -1;
        return i;
}


int     SoundNextStatic(STATICSOUND *ss,int point,int size)
{
        char * buffer;
        int bufferbyte,s=0;

        if (ss->Status<0)       // stop
           {
           IDirectSoundBuffer_Stop(ss->lpDSB);
           IDirectSoundBuffer_SetCurrentPosition(ss->lpDSB,0);
           ss->wave=NULL;
           return 0;
           }

        if((point+size)>SBUFFERSIZE) return -1;
        if(FAILED(IDirectSoundBuffer_Lock(ss->lpDSB,point,size,(void **)&buffer,(DWORD *)&bufferbyte,NULL,NULL,0)))
           return FailMsg("Lock static sound buffer fail");
        if((ss->current+bufferbyte)>(ss->wave->bufferbyte))
           {
           s=(ss->wave->bufferbyte)-(ss->current);
           if(ss->Status&PLAYLOOP)
              {
              memcpy(buffer,(ss->wave->buffer)+(ss->current),s);
              memcpy(buffer+s,ss->wave->buffer,bufferbyte-s);
              ss->current=bufferbyte-s;
              }
           else
              {
              memcpy(buffer,(ss->wave->buffer)+(ss->current),s);
              if(wf.wBitsPerSample==16) FillMemory(buffer+s,bufferbyte-s,0);
              else FillMemory(buffer+s,bufferbyte-s,0x80);
              ss->current=ss->wave->bufferbyte;
              if (ss->Status==PLAYLOOP) ss->current=0;
              else ss->Status=-1;   // END
              }
           }
        else
           {
           memcpy(buffer,(ss->wave->buffer)+(ss->current),bufferbyte);
           ss->current+=bufferbyte;
           }
        IDirectSoundBuffer_Unlock(ss->lpDSB,buffer,bufferbyte,NULL,0);
        return 0;
}


DWORD WINAPI    SoundThread(LPVOID p)
{
        DWORD i;
        for (;;)
           {
           i=MsgWaitForMultipleObjects(SAMPLEMAX*2+2,SoundEvent,FALSE,INFINITE,0);
           if (i==SAMPLEMAX*2)
              {
              if (SoundNextStream(MBUFFERSIZE>>1,MBUFFERSIZE>>1))
                 {
                 IDirectSoundBuffer_Stop(StreamSound.lpDSB);
                 IDirectSoundBuffer_SetCurrentPosition(StreamSound.lpDSB,0);
                 StreamSound.wave=NULL;
                 StreamSound.Status=-1;
                 }
              }
           else if (i==SAMPLEMAX*2+1)
              {
              if (SoundNextStream(0,MBUFFERSIZE>>1))
                 {
                 IDirectSoundBuffer_Stop(StreamSound.lpDSB);
                 IDirectSoundBuffer_SetCurrentPosition(StreamSound.lpDSB,0);
                 StreamSound.wave=NULL;
                 StreamSound.Status=-1;
                 }
              }
           else if (SoundNextStatic(&StaticSound[i/2],(i&1)?0:(SBUFFERSIZE>>1),SBUFFERSIZE>>1))
              {
              IDirectSoundBuffer_Stop(StaticSound[i/2].lpDSB);
              IDirectSoundBuffer_SetCurrentPosition(StaticSound[i/2].lpDSB,0);
              StaticSound[i/2].wave=NULL;
              StaticSound[i/2].Status=-1;
              }

           }
        return 0;
}
