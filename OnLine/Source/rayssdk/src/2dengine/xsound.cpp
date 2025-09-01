/*
**      XSOUND.CPP
**
**      DirectSound support for load wave file and play it using static DirectSound
**      buffer and adjust its frequency,pan, and volume.
**      Copyright (c) 1999 Microsoft Corp. All rights reserved.
**      
**      ZJian,2000.10.17.
**          Re-Developped for my game engine by arraying MAX_SOUND_CHANNELS.
**      ZJian,2001.5.1.
**          Assigned sound channels to voice channel & music channel.
**          Added sound_cfg for our game system sound control, include volume control, pan control etc.
**
*/
#include "rays.h"
#include "winmain.h"
#include "xsystem.h"
#include "xsound.h"
#include "packfile.h"
#include "mp3file.h"

#pragma comment(lib,"dsound")


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define DS_INIT             0x01

#define SAFE_RELEASE(p)     { if(p) { (p)->Release(); (p)=NULL; } }

//-----------------------------------------------------------------------------
//数据流声音
typedef struct  tagSTREAMSOUND
{
	LPDIRECTSOUNDBUFFER lpDSB;	//Direct Sound Buffer
	LPDIRECTSOUNDNOTIFY lpDSN;	//Direct Sound Buffer Notify
	void  * music;              //
	LONG    pan;				//均衡
	LONG    volume;				//音量
	int     status;				//目前状态
	DSBPOSITIONNOTIFY dsbpn[2]; //事件通知
} STREAMSOUND, *LPSTREAMSOUND;

//
STREAMSOUND     StreamSound;
HANDLE          hStreamEvent[2];
HANDLE          hStreamThread = NULL;
DWORD           dwStreamThreadID;
//-----------------------------------------------------------------------------


SOUND_CFG       sound_cfg;

//-----------------------------------------------------------------------------
// Static values
//-----------------------------------------------------------------------------
static  LPDIRECTSOUND       g_pDS                                   = NULL;
static  LPDIRECTSOUNDBUFFER g_pDSBuffer[MAX_SOUND_CHANNELS]         = {NULL};
static  LPDIRECTSOUNDNOTIFY g_pDSNotify[MAX_SOUND_CHANNELS]         = {NULL};
static  SAMPLE*             g_pSample[MAX_SOUND_CHANNELS]           = {NULL};
static  DWORD               g_dwBufferBytes[MAX_SOUND_CHANNELS]     = {0};
static  DWORD               g_dwTimeLength[MAX_SOUND_CHANNELS]      = {0};
static  LONG                g_lFrequency[MAX_SOUND_CHANNELS];
static  LONG                g_lPan[MAX_SOUND_CHANNELS];
static  LONG                g_lVolume[MAX_SOUND_CHANNELS];
static  DWORD               dwDSFlag                                = 0;
//
static  LONG                lCtrlPan[MAX_SOUND_CHANNELS];
static  LONG                lCtrlVolume[MAX_SOUND_CHANNELS];

//-----------------------------------------------------------------------------
// Static function prototypes
//-----------------------------------------------------------------------------
static  HRESULT InitDirectSound( HWND handle );
static  HRESULT FreeDirectSound();
static  VOID    SetBufferFrequency( LONG lChannel, LONG lFrequency );
static  VOID    SetBufferPan( LONG lChannel, LONG lPan );
static  VOID    SetBufferVolume( LONG lChannel, LONG lVolume );
static  HRESULT LoadWaveFile( LONG lChannel, TCHAR* strFileName );
static  HRESULT LoadWaveBuffer( LONG lChannel, USTR *pBuffer, LONG lBufferSize );
static  HRESULT CreateStaticBuffer( LONG lChannel, TCHAR* strFileName );
static  HRESULT CreateStaticBufferFromBuffer( LONG lChannel, USTR *pBuffer, LONG lBufferSize );
static  HRESULT FillBuffer( LONG lChannel );
static  HRESULT PlayBuffer( LONG lChannel, BOOL bLooped );
static  VOID    StopBuffer( LONG lChannel, BOOL bResetPosition ); 
static  BOOL    IsSoundPlaying( LONG lChannel );
static  HRESULT RestoreBuffers( LONG lChannel );
static	LONG	GetFreeChannel( LONG lStart, LONG lEnd );
static  LONG    CompositeSoundVolume( LONG lMainVolume, LONG lChannelVolume);

//wave music functions
int     _init_wav_music(void);
void    _active_wav_music(int active);
void    _free_wav_music(void);
void    _play_wav_music(SLONG music_no, SLONG loop);
void    _stop_wav_music(void);
void    _set_wav_music_volume(SLONG volume);
void    _set_wav_music_pan(SLONG pan);
SLONG   _is_wav_music_playing(void);

//mp3 music functions
int     _init_mp3_music(void);
void    _active_mp3_music(int active);
void    _free_mp3_music(void);
void    _play_mp3_music(SLONG music_no, SLONG loop);
void    _stop_mp3_music(void);
void    _set_mp3_music_volume(SLONG volume);
void    _set_mp3_music_pan(SLONG pan);
SLONG   _is_mp3_music_playing(void);

//cdrom music functions
//...

//music play functions pointer
int     (*init_music)(void)                         =   _init_wav_music;
void    (*active_music)(int active)                 =   _active_wav_music;
void    (*free_music)(void)                         =   _free_wav_music;
void    (*play_music)(SLONG music_no, SLONG loop)   =   _play_wav_music;
void    (*stop_music)(void)                         =   _stop_wav_music;
void    (*set_music_volume)(SLONG volume)           =   _set_wav_music_volume;
void    (*set_music_pan)(SLONG pan)                 =   _set_wav_music_pan;
SLONG   (*is_music_playing)(void)                   =   _is_wav_music_playing;


//-----------------------------------------------------------------------------
// Name: InitDirectSound()
// Desc: Initilizes DirectSound
//-----------------------------------------------------------------------------
HRESULT InitDirectSound( HWND handle )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    dwDSFlag = 0;

    // Initialize COM
    if( FAILED (hr = CoInitialize( NULL ) ) )
        return hr;

    sprintf((char *)print_rec,"       Sound Play : %s", "DirectSound" );
    log_error(1,print_rec);

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate( NULL, &g_pDS, NULL ) ) )
        return hr;

    // Set coop level to DSSCL_PRIORITY
    if( FAILED( hr = g_pDS->SetCooperativeLevel( handle, DSSCL_PRIORITY ) ) )
        return hr;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return hr;

    // Set primary buffer format to 22kHz,16-bit, stero output.
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = 2; 
    wfx.nSamplesPerSec  = 22050; 
    wfx.wBitsPerSample  = 16; 
    wfx.nBlockAlign     = (WORD)(wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return hr;

    SAFE_RELEASE( pDSBPrimary );
    dwDSFlag |= DS_INIT;

    // Init samples[].
    SLONG   i;
    for(i=0; i<MAX_SOUND_CHANNELS; i++)
    {
        g_pSample[i] = NULL;
        lCtrlPan[i] = 0; //0 = MUSIC_PAN_CENTER = VOICE_PAN_CENTER
        lCtrlVolume[i] = 0; //0 = MUSIC_VOLUME_MAX = VOICE_VOLUME_MAX
    }


    sprintf((char *)print_rec,"      Wave format : %s", "PCM Wave" );
    log_error(1,print_rec);
    sprintf((char *)print_rec,"         Channels : %d", wfx.nChannels );
    log_error(1,print_rec);
    sprintf((char *)print_rec,"  Samples per sec : %d", wfx.nSamplesPerSec );
    log_error(1,print_rec);
    sprintf((char *)print_rec,"  Bits per sample : %d", wfx.wBitsPerSample );
    log_error(1,print_rec);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FreeDirectSound()
// Desc: Releases DirectSound 
//-----------------------------------------------------------------------------
HRESULT FreeDirectSound()
{
    LONG i;

    for(i=0;i<MAX_SOUND_CHANNELS;++i)
    {
        if(g_pSample[i])
        {
            destroy_sample(&g_pSample[i]);
        }
    }

    // Release DirectSound interfaces
    for(i=0;i<MAX_SOUND_CHANNELS;++i)
        SAFE_RELEASE( g_pDSBuffer[i] );
    SAFE_RELEASE( g_pDS ); 

    // Release COM
    CoUninitialize();

    dwDSFlag = 0;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SetBufferFrequency()
// Desc: Sets the DirectSound buffer frequency
//-----------------------------------------------------------------------------
VOID    SetBufferFrequency( LONG lChannel, LONG lFrequency )
{
    if( ! ( dwDSFlag & DS_INIT ) )
        return;

    //#define DSBFREQUENCY_MIN            100
    //#define DSBFREQUENCY_MAX            100000
    //#define DSBFREQUENCY_ORIGINAL       0
    if( g_pDSBuffer[lChannel] )
    {
        g_pDSBuffer[lChannel]->SetFrequency( lFrequency );
        g_lFrequency[lChannel] = lFrequency;
    }
}



//-----------------------------------------------------------------------------
// Name: SetBufferPan()
// Desc: Sets the DirectSound buffer pan
//-----------------------------------------------------------------------------
VOID    SetBufferPan( LONG lChannel, LONG lPan )
{
    if( ! ( dwDSFlag & DS_INIT ) )
        return;

    //#define DSBPAN_LEFT                 -10000
    //#define DSBPAN_CENTER               0
    //#define DSBPAN_RIGHT                10000
    if( g_pDSBuffer[lChannel] )
    {
        if( lPan < DSBPAN_LEFT ) lPan = DSBPAN_LEFT;
        if( lPan > DSBPAN_RIGHT ) lPan = DSBPAN_RIGHT;
        g_pDSBuffer[lChannel]->SetPan( lPan );
        g_lPan[lChannel] = lPan;
    }
}



//-----------------------------------------------------------------------------
// Name: SetBufferVolume()
// Desc: Sets the DirectSound buffer volume
//-----------------------------------------------------------------------------
VOID    SetBufferVolume( LONG lChannel, LONG lVolume )
{
    if( ! ( dwDSFlag & DS_INIT ) )
        return;

    //#define DSBVOLUME_MIN               -10000
    //#define DSBVOLUME_MAX               0
    if( g_pDSBuffer[lChannel] )
    {
        if( lVolume < DSBVOLUME_MIN ) lVolume = DSBVOLUME_MIN;
        if( lVolume > DSBVOLUME_MAX ) lVolume = DSBVOLUME_MAX;
        g_pDSBuffer[lChannel]->SetVolume( lVolume );
        g_lVolume[lChannel] = lVolume;
    }
}



//-----------------------------------------------------------------------------
// Name: LoadWaveFile()
// Desc: Loads the wave file into a secondary static DirectSound buffer
//-----------------------------------------------------------------------------
HRESULT LoadWaveFile( LONG lChannel, TCHAR* strFileName )
{
    HRESULT hr;

    if( ! ( dwDSFlag & DS_INIT ) )
        return S_OK;

    // Create the sound buffer object from the wave file data
    if( FAILED( hr = CreateStaticBuffer( lChannel, strFileName ) ) )
    {
        //sprintf((char *)print_rec,"couldn't create sound buffer.");
        //log_error(1,print_rec);
        return  hr;
    }

    // Fill the buffer with wav data
    if( FAILED( hr = FillBuffer( lChannel ) ) )
    {
        //sprintf((char *)print_rec, "fill sound buffer error");
        //log_error(1, print_rec);
        return  hr;
    }
    return  S_OK;
}




HRESULT LoadWaveBuffer( LONG lChannel, USTR *pBuffer, LONG lBufferSize )
{
    HRESULT hr;

    if( ! ( dwDSFlag & DS_INIT ) )
        return S_OK;

    // Create the sound buffer object from the wave file data
    if( FAILED( hr = CreateStaticBufferFromBuffer( lChannel, pBuffer, lBufferSize ) ) )
    {
        //sprintf((char *)print_rec,"couldn't create sound buffer.");
        //log_error(1,print_rec);
        return  hr;
    }

    // Fill the buffer with wav data
    if( FAILED( hr = FillBuffer( lChannel ) ) )
    {
        //sprintf((char *)print_rec, "fill sound buffer error");
        //log_error(1, print_rec);
        return  hr;
    }
    return  S_OK;
}



//-----------------------------------------------------------------------------
// Name: CreateStaticBuffer()
// Desc: Creates a wave file, sound buffer and notification events 
//-----------------------------------------------------------------------------
HRESULT CreateStaticBuffer( LONG lChannel, TCHAR* strFileName )
{
    HRESULT hr;

    if( ! ( dwDSFlag & DS_INIT ) )
        return S_OK;

    if(g_pSample[lChannel])
    {
        destroy_sample(&g_pSample[lChannel]);
    }

    SAFE_RELEASE( g_pDSBuffer[lChannel] );

    if(NULL == (g_pSample[lChannel] = load_wav_from_file((USTR*)strFileName)))
    {
        return  -1;
    }


    // Set up the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_STATIC 
        | DSBCAPS_CTRLPAN 
        | DSBCAPS_CTRLVOLUME 
        ;
    dsbd.dwBufferBytes = g_pSample[lChannel]->buffer_bytes;
    dsbd.lpwfxFormat   = &g_pSample[lChannel]->wf;

    // Create the static DirectSound buffer 
    hr = g_pDS->CreateSoundBuffer( &dsbd, &g_pDSBuffer[lChannel], NULL );
    if(FAILED(hr))  return hr;

    // Remember how big the buffer is
    g_dwBufferBytes[lChannel] = dsbd.dwBufferBytes;

    // Calculate channel's time length(ms)
    g_dwTimeLength[lChannel] = dsbd.dwBufferBytes * 1000/ max(1, dsbd.lpwfxFormat->nAvgBytesPerSec);

    return S_OK;
}


HRESULT CreateStaticBufferFromBuffer( LONG lChannel, USTR *pBuffer, LONG lBufferSize )
{
    HRESULT hr;

    if( ! ( dwDSFlag & DS_INIT ) )
        return S_OK;

    if(g_pSample[lChannel])
    {
        destroy_sample(&g_pSample[lChannel]);
    }

    SAFE_RELEASE( g_pDSBuffer[lChannel] );

    if(NULL == (g_pSample[lChannel] = load_wav_from_buffer((USTR*)pBuffer, lBufferSize)))
    {
        return  -1;
    }


    // Set up the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_STATIC | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = g_pSample[lChannel]->buffer_bytes;
    dsbd.lpwfxFormat   = &g_pSample[lChannel]->wf;

    // Create the static DirectSound buffer 
    if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, &g_pDSBuffer[lChannel], NULL ) ) )
        return hr;

    // Remember how big the buffer is
    g_dwBufferBytes[lChannel] = dsbd.dwBufferBytes;

    // Calculate channel's time length(ms)
    g_dwTimeLength[lChannel] = dsbd.dwBufferBytes * 1000/ max(1, dsbd.lpwfxFormat->nAvgBytesPerSec);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FillBuffer()
// Desc: Fill the DirectSound buffer with data from the wav file
//-----------------------------------------------------------------------------
HRESULT FillBuffer( LONG lChannel )
{
    HRESULT hr; 
    VOID*   pbData  = NULL;
    DWORD   dwLength;

    if( ! ( dwDSFlag & DS_INIT ) )
        return S_OK;

    hr = g_pDSBuffer[lChannel]->Lock( 0, g_dwBufferBytes[lChannel], &pbData, &dwLength, NULL, NULL, 0L );
    if(FAILED(hr))  return hr;

    memcpy( pbData, g_pSample[lChannel]->buffer, g_dwBufferBytes[lChannel] );
    g_pDSBuffer[lChannel]->Unlock( pbData, g_dwBufferBytes[lChannel], NULL, 0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: PlayBuffer()
// Desc: User hit the "Play" button, so play the DirectSound buffer
//-----------------------------------------------------------------------------
HRESULT PlayBuffer( LONG lChannel, BOOL bLooped )
{
    HRESULT hr;

    if( ! ( dwDSFlag & DS_INIT ) )
        return S_OK;

    if( NULL == g_pDSBuffer[lChannel] )
        return E_FAIL;

    // Restore the buffers if they are lost
    if( FAILED( hr = RestoreBuffers(lChannel) ) )
        return hr;

    // Play buffer 
    DWORD dwLooped = bLooped ? DSBPLAY_LOOPING : 0L;
    if( FAILED( hr = g_pDSBuffer[lChannel]->Play( 0, 0, dwLooped ) ) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: StopBuffer()
// Desc: Stop the DirectSound buffer from playing 
//-----------------------------------------------------------------------------
VOID    StopBuffer( LONG lChannel, BOOL bResetPosition ) 
{
    if( ! ( dwDSFlag & DS_INIT ) )
        return;

    if( NULL == g_pDSBuffer[lChannel] )
        return;

    g_pDSBuffer[lChannel]->Stop();

    if( bResetPosition )
        g_pDSBuffer[lChannel]->SetCurrentPosition( 0L );    
}




//-----------------------------------------------------------------------------
// Name: IsSoundPlaying()
// Desc: Checks to see if a sound is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
BOOL IsSoundPlaying( LONG lChannel )
{
    if( ! ( dwDSFlag & DS_INIT ) )
        return FALSE;

    if( g_pDSBuffer[lChannel] )
    {  
        DWORD dwStatus = 0;
        g_pDSBuffer[lChannel]->GetStatus( &dwStatus );
        return( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
    }
    else
    {
        return FALSE;
    }
}




//-----------------------------------------------------------------------------
// Name: RestoreBuffers()
// Desc: Restore lost buffers and fill them up with sound if possible
//-----------------------------------------------------------------------------
HRESULT RestoreBuffers( LONG lChannel )
{
    HRESULT hr;

    if( ! ( dwDSFlag & DS_INIT ) )
        return S_OK;

    if( NULL == g_pDSBuffer[lChannel] )
        return S_OK;

    DWORD dwStatus;
    if( FAILED( hr = g_pDSBuffer[lChannel]->GetStatus( &dwStatus ) ) )
        return hr;

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = g_pDSBuffer[lChannel]->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        } while( FAILED ( hr = g_pDSBuffer[lChannel]->Restore() ) );

        if( FAILED( hr = FillBuffer(lChannel) ) )
            return hr;
    }

    return S_OK;
}


static	LONG	GetFreeChannel( LONG lStart, LONG lEnd )
{
	LONG	i;
	DWORD	dwStatus = 0;

    if( ! ( dwDSFlag & DS_INIT ) )
        return FALSE;

	for(i = lStart; i <= lEnd; i ++)
	{
		if( g_pDSBuffer[i] )
		{  
			dwStatus = 0;
			g_pDSBuffer[i]->GetStatus( &dwStatus );
			if(! (dwStatus & DSBSTATUS_PLAYING ) )
			{
				return	i;
			}
		}
	}
	if(lStart >= lEnd)
	{
		return	lStart;
	}
	else
	{
		return	rand() % (lEnd - lStart + 1);
	}
}


static  LONG    CompositeSoundVolume( LONG lMainVolume, LONG lChannelVolume)
{
    LONG   lVolume;
    double  km, kn;
    
    km = (lMainVolume - VOICE_VOLUME_MIN) * 1.0 / (VOICE_VOLUME_MAX - VOICE_VOLUME_MIN);
    kn = (lChannelVolume - VOICE_VOLUME_MIN) * 1.0 / (VOICE_VOLUME_MAX - VOICE_VOLUME_MIN);
    lVolume = VOICE_VOLUME_MIN + (SLONG)( km * kn * (VOICE_VOLUME_MAX - VOICE_VOLUME_MIN) );

    return  lVolume;
}

//=====================================================================================================
//  SOUND CONFIG FUNCTIONS
//=====================================================================================================
EXPORT  void    FNBACK  init_sound_cfg(void)
{
    sound_cfg.music_flag = 0;
    sound_cfg.music_no = -1;
    sound_cfg.music_pan = MUSIC_PAN_CENTER;
    sound_cfg.music_total = 0;
    sound_cfg.music_volume = MUSIC_VOLUME_MAX;
    sound_cfg.voice_flag = 0;
    sound_cfg.voice_pan = VOICE_PAN_CENTER;
    sound_cfg.voice_volume = VOICE_VOLUME_MAX;
}


EXPORT  void    FNBACK  notify_changed_sound_cfg(SLONG changed_flags)
{
    SLONG   channel;

    if(changed_flags & CHANGED_MUSIC_FLAG)
    {
    }
    if(changed_flags & CHANGED_MUSIC_TOTAL)
    {
    }
    if(changed_flags & CHANGED_MUSIC_VOLUME)
    {
        set_music_volume(lCtrlVolume[MUSIC_CHANNEL]);
    }
    if(changed_flags & CHANGED_MUSIC_PAN)
    {
        set_music_pan(lCtrlPan[MUSIC_CHANNEL]);
    }
    if(changed_flags & CHANGED_VOICE_FLAG)
    {
    }
    if(changed_flags & CHANGED_VOICE_VOLUME)
    {
        for(channel=VOICE_CHANNEL_0; channel<=VOICE_CHANNEL_MAX; channel++)
        {
            set_voice_volume(channel, lCtrlVolume[channel]);
        }
    }
    if(changed_flags & CHANGED_VOICE_PAN)
    {
        for(channel=VOICE_CHANNEL_0; channel<=VOICE_CHANNEL_MAX; channel++)
        {
            set_voice_pan(channel, lCtrlPan[channel]);
        }
    }
}
//=====================================================================================================
// SOUND : VOICE & MUSIC FUNCTIONS
//=====================================================================================================
EXPORT  void    FNBACK  set_music_type(SLONG type)
{
    switch(type)
    {
    case MUSIC_TYPE_WAVE:
        init_music = _init_wav_music;
        active_music = _active_wav_music;
        free_music = _free_wav_music;
        play_music = _play_wav_music;
        stop_music = _stop_wav_music;
        set_music_volume = _set_wav_music_volume;
        set_music_pan = _set_wav_music_pan;
        is_music_playing = _is_wav_music_playing;
        break;
    case MUSIC_TYPE_MP3:
        init_music = _init_mp3_music;
        active_music = _active_mp3_music;
        free_music = _free_mp3_music;
        play_music = _play_mp3_music;
        stop_music = _stop_mp3_music;
        set_music_volume = _set_mp3_music_volume;
        set_music_pan = _set_mp3_music_pan;
        is_music_playing = _is_mp3_music_playing;
        break;
    case MUSIC_TYPE_CDROM:
        break;
    }
}


EXPORT  int     FNBACK  init_sound(void)
{
    HRESULT hr;

    sprintf((char *)print_rec,"SOUND module initialize starting...");
    log_error(1,print_rec);

    init_sound_cfg();

    hr = InitDirectSound( g_hDDWnd );
    if(S_OK != hr)
        return(-1);


    if(0 != init_music())
        return  -1;
    
    sound_cfg.music_flag = 1;
    sound_cfg.voice_flag = 1;
    
    sprintf((char *)print_rec,"SOUND module initialized OKay.");
    log_error(1,print_rec);
    sprintf((char *)print_rec,"\n");
    log_error(1,print_rec);

    return  hr;
}


EXPORT  void    FNBACK  active_sound(int active)
{
    LONG i;

    if( ! active ) return;

    active_music(active);

    for(i=0;i<MAX_SOUND_CHANNELS;++i)
    {
        RestoreBuffers( i );
    }
}


EXPORT  void    FNBACK  free_sound(void)
{
    HRESULT hr;

    free_music();

    if(FAILED( hr = FreeDirectSound() ) )
        return;


    sound_cfg.music_flag = 0;
    sound_cfg.voice_flag = 0;
    
    sprintf((char *)print_rec,"SOUND module free OKay.");
    log_error(1,print_rec);
}


EXPORT  void    FNBACK  play_voice(SLONG channel,SLONG loop,USTR *filename)
{
    HRESULT hr;

    if(! sound_cfg.voice_flag)
        return;

    if(channel < VOICE_CHANNEL_MIN || channel > VOICE_CHANNEL_MAX)
        return;

    if(FAILED( hr = LoadWaveFile( (LONG)channel, (TCHAR*)filename ) ) )
    {
        sprintf((char*)print_rec, "load voice [%d]%s error", channel, filename);
        log_error(1, print_rec);
        return;
    }

    // Jack,  [09:11,10/10/2002]
    {
        LONG   volume;

        volume = CompositeSoundVolume(sound_cfg.voice_volume, VOICE_VOLUME_MAX);
        SetBufferVolume( (LONG)channel, volume );
    }

    if(FAILED( hr = PlayBuffer( (LONG)channel, loop) ) )
    {
        sprintf((char *)print_rec, "play voice [%d]%s error", channel, filename);
        log_error(1, print_rec);
        return;
    }

    //Play voice OK.
    return;
}


EXPORT  void    FNBACK  play_buffer_voice(SLONG channel,SLONG loop,USTR *buffer, SLONG buffer_size)
{
    HRESULT hr;

    if(! sound_cfg.voice_flag)
        return;

    if(channel < VOICE_CHANNEL_MIN || channel > VOICE_CHANNEL_MAX)
        return;

    if(FAILED( hr = LoadWaveBuffer( (LONG)channel, (USTR*)buffer, buffer_size ) ) )
    {
        sprintf((char*)print_rec, "load wave buffer [%d](size = %d) error", channel, buffer_size);
        log_error(1, print_rec);
        return;
    }

    // Jack,  [09:11,10/10/2002]
    {
        LONG   volume;

        volume = CompositeSoundVolume(sound_cfg.voice_volume, VOICE_VOLUME_MAX);
        SetBufferVolume( (LONG)channel, volume );
    }

    if(FAILED( hr = PlayBuffer( (LONG)channel, loop) ) )
    {
        sprintf((char *)print_rec, "play buffer voice [%d](size = %d) error", channel, buffer_size);
        log_error(1, print_rec);
        return;
    }

    //Play voice OK.
    return;
}


EXPORT  void    FNBACK  stop_voice(SLONG channel)
{
    if(! sound_cfg.voice_flag )
        return;

    if(channel < VOICE_CHANNEL_MIN || channel > VOICE_CHANNEL_MAX )
        return;

    StopBuffer( (LONG)channel,1);
}


EXPORT	void	FNBACK	direct_play_voice(SLONG start_channel, SLONG end_channel, SLONG loop, USTR *filename)
{
    HRESULT hr;
	SLONG	channel;

    if(! sound_cfg.voice_flag)
        return;

	channel = GetFreeChannel(start_channel, end_channel);
    if(channel < VOICE_CHANNEL_MIN || channel > VOICE_CHANNEL_MAX)
        return;

    if(FAILED( hr = LoadWaveFile( (LONG)channel, (TCHAR*)filename ) ) )
    {
        sprintf((char*)print_rec, "load voice [%d]%s error", channel, filename);
        log_error(1, print_rec);
        return;
    }

    // Jack,  [09:11,10/10/2002]
    {
        LONG   volume;

        volume = CompositeSoundVolume(sound_cfg.voice_volume, VOICE_VOLUME_MAX);
        SetBufferVolume( (LONG)channel, volume );
    }

    if(FAILED( hr = PlayBuffer( (LONG)channel, loop) ) )
    {
        sprintf((char *)print_rec, "play voice [%d]%s error", channel, filename);
        log_error(1, print_rec);
        return;
    }

    //Play voice OK.
    return;
}



EXPORT  void    FNBACK  set_voice_volume(SLONG channel,SLONG volume)
{
    SLONG   valid_volume;
    double  kn,kc;

    if(! sound_cfg.voice_flag)
        return;

    if(channel < VOICE_CHANNEL_MIN || channel > VOICE_CHANNEL_MAX )
        return;

    {
        kn = (volume - VOICE_VOLUME_MIN) * 1.0 / (VOICE_VOLUME_MAX - VOICE_VOLUME_MIN);
        kc = (sound_cfg.voice_volume - VOICE_VOLUME_MIN) * 1.0 / (VOICE_VOLUME_MAX - VOICE_VOLUME_MIN);
        
        valid_volume = VOICE_VOLUME_MIN + (SLONG)( kn * kc * (VOICE_VOLUME_MAX - VOICE_VOLUME_MIN) );
        
        SetBufferVolume( (LONG)channel,(LONG)valid_volume );
        
        lCtrlVolume[channel] = volume;
    }
}



EXPORT  void    FNBACK  set_voice_pan(SLONG channel,SLONG pan)
{
    SLONG   valid_pan;

    if(! sound_cfg.voice_flag)
        return;

    if(channel < VOICE_CHANNEL_MIN || channel > VOICE_CHANNEL_MAX )
        return;

    {
        valid_pan = pan + sound_cfg.voice_pan;
        if(valid_pan < VOICE_PAN_LEFT)
            valid_pan = VOICE_PAN_LEFT;
        if(valid_pan > VOICE_PAN_RIGHT)
            valid_pan = VOICE_PAN_RIGHT;

        SetBufferPan( (LONG)channel,(LONG)valid_pan );

        lCtrlPan[channel] = pan;
    }
}


EXPORT  SLONG   FNBACK  is_voice_playing(SLONG channel)
{
    if(! sound_cfg.voice_flag)
        return FALSE;

    if(channel >= VOICE_CHANNEL_MIN && channel <= VOICE_CHANNEL_MAX )
        return IsSoundPlaying((LONG)channel);
    return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WAVE MUSIC FUNCTIONS                                                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int     _init_wav_music(void)
{
    return  0;
}

void    _active_wav_music(int active)
{
}

void    _free_wav_music(void)
{
}


void    _play_wav_music(SLONG music_no, SLONG loop)
{
    SLONG   channel = MUSIC_CHANNEL;
    USTR    filename[_MAX_FNAME];
    SLONG   old_music_no;
    HRESULT hr;

    old_music_no = sound_cfg.music_no;
    sound_cfg.music_no = music_no;
    if(! sound_cfg.music_flag)
        return;
    if(old_music_no == music_no)
        return;

    sprintf((char *)filename,"MUSIC\\MUS%03d.WAV", music_no);
    if(FAILED( hr = LoadWaveFile( (LONG)MUSIC_CHANNEL, (TCHAR*)filename ) ) )
    {
        sprintf((char *)print_rec, "load music [%d]%s error", channel, filename);
        log_error(1, print_rec);
        return;
    }

    // Jack,  [09:12,10/10/2002]
    {
        LONG   volume;

        volume = CompositeSoundVolume(sound_cfg.music_volume, MUSIC_VOLUME_MAX);
        SetBufferVolume( (LONG)channel, volume );
    }

    if(FAILED( hr = PlayBuffer( (LONG)MUSIC_CHANNEL, loop) ) )
    {
        sprintf((char *)print_rec, "play music [%d]%s error", channel, filename);
        log_error(1, print_rec);
        return;
    }

    //Play music OK.
    return;
}



void    _stop_wav_music(void)
{
    if(! sound_cfg.music_flag)
        return;

    StopBuffer( MUSIC_CHANNEL, 1);
    sound_cfg.music_no = -1;
}


void    _set_wav_music_volume(SLONG volume)
{
    SLONG   valid_volume;
    double  kn, kc;

    if(! sound_cfg.music_flag)
        return;

    kn = (volume - MUSIC_VOLUME_MIN) * 1.0 / (MUSIC_VOLUME_MAX - MUSIC_VOLUME_MIN);
    kc = (sound_cfg.music_volume - MUSIC_VOLUME_MIN) * 1.0 / (MUSIC_VOLUME_MAX - MUSIC_VOLUME_MIN);
    
    valid_volume = MUSIC_VOLUME_MIN + (SLONG)( kn * kc * (MUSIC_VOLUME_MAX - MUSIC_VOLUME_MIN) );

    SetBufferVolume( (LONG)MUSIC_CHANNEL, (LONG)valid_volume );

    lCtrlVolume[MUSIC_CHANNEL] = volume;
}


void    _set_wav_music_pan(SLONG pan)
{
    SLONG   valid_pan;

    if(! sound_cfg.music_flag)
        return;

    valid_pan = pan + sound_cfg.music_pan;
    if(valid_pan < MUSIC_PAN_LEFT)
        valid_pan = MUSIC_PAN_LEFT;
    if(valid_pan > MUSIC_PAN_RIGHT)
        valid_pan = MUSIC_PAN_RIGHT;

    SetBufferPan( MUSIC_CHANNEL,(LONG)pan );

    lCtrlPan[MUSIC_CHANNEL] = pan;
}


SLONG   _is_wav_music_playing(void)
{
    if(! sound_cfg.music_flag)
        return FALSE;

    return IsSoundPlaying((LONG)MUSIC_CHANNEL);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// W A V E    F I L E     F U N C T I O N S                                                          //
///////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  SLONG   FNBACK  get_wav_format(PACK_FILE *hfile, WAVEFORMATEX *wf, SLONG *block_size, SLONG *wave_size)
{
    SLONG   i, point;
    DWORD   waveid;
    struct BLOCKHEAD
    {
        DWORD id;
        DWORD size;
    } blockhead,riffhead;
    
    
    *wave_size=0;
    while(1)
    {
        i = 0;
        if( (pack_fread(&riffhead,1, sizeof(riffhead), hfile)) != sizeof(riffhead) )
            return TTN_ERROR;
        if(riffhead.id != 'FFIR')
        {
            pack_fseek(hfile,riffhead.size,SEEK_CUR);
            continue;
        }
        if( (pack_fread(&waveid,1, sizeof(waveid), hfile)) != sizeof(waveid) )
            return TTN_ERROR;

        i += sizeof(waveid);
        if(waveid!='EVAW')
        {
            pack_fseek(hfile, riffhead.size-i, SEEK_CUR);
            continue;
        }
        while(1)
        {
            if(i >= (SLONG)riffhead.size) break;
            if((pack_fread(&blockhead,1, sizeof(blockhead), hfile)) != sizeof(blockhead))
                return TTN_ERROR;

            i += sizeof(blockhead);
            if(blockhead.id!=' tmf')
            {
                pack_fseek(hfile, blockhead.size, SEEK_CUR);
                i+=blockhead.size;
                continue;
            }
            if((pack_fread(wf,1, blockhead.size, hfile)) != blockhead.size)
                return TTN_ERROR;

            i += blockhead.size;
            wf->cbSize=0;
            *block_size=(SLONG)riffhead.size-i;
            
            point = pack_ftell(hfile);
            while(1)
            {
                if(i >= (int)riffhead.size) break;
                if((pack_fread(&blockhead, 1, sizeof(blockhead), hfile)) != sizeof(blockhead))
                    return TTN_ERROR;
                i += sizeof(blockhead);
                if(blockhead.id == 'atad') *wave_size += blockhead.size;
					pack_fseek(hfile, blockhead.size, SEEK_CUR);
                i += blockhead.size;
            }
            pack_fseek(hfile,point,SEEK_SET);
            return TTN_OK;
        }
    }
}


EXPORT  SLONG   FNBACK  get_wav_data(PACK_FILE *hfile, USTR *data, SLONG blocksize)
{
    SLONG   i = 0, j = 0;
    struct 
    {
        DWORD id;
        DWORD size;
    } blockhead;
    
    while(1)
    {
        if(i >= blocksize) break;
        if((pack_fread(&blockhead,1, sizeof(blockhead), hfile)) != sizeof(blockhead))
            return TTN_ERROR;

        i += sizeof(blockhead);
        if(blockhead.id != 'atad')
        {
            pack_fseek(hfile, blockhead.size, SEEK_CUR);
            i += blockhead.size;
            continue;
        }
        if((pack_fread(&data[j], 1, blockhead.size, hfile)) != blockhead.size)
            return TTN_ERROR;

        j += blockhead.size;
        i += blockhead.size;
    }
    return TTN_OK;
}


EXPORT  SLONG   FNBACK  get_buffer_wav_format(USTR *buffer, SLONG *buffer_offset, WAVEFORMATEX *wf, SLONG *block_size, SLONG *wave_size)
{
    SLONG   i, point, offset;
    DWORD   waveid;
    struct BLOCKHEAD
    {
        DWORD id;
        DWORD size;
    } blockhead,riffhead;
    
    offset = 0;
    *buffer_offset = 0;
    *wave_size=0;
    while(1)
    {
        i = 0;

        memcpy(&riffhead, &buffer[offset], sizeof(riffhead));
        offset += sizeof(riffhead);

        if(riffhead.id != 'FFIR')
        {
            offset += riffhead.size;

            continue;
        }
        memcpy(&waveid, &buffer[offset], sizeof(waveid));
        offset += sizeof(waveid);

        i += sizeof(waveid);
        if(waveid!='EVAW')
        {
            offset += ((SLONG)riffhead.size - i);
            continue;
        }
        while(1)
        {
            if(i >= (SLONG)riffhead.size) break;

            memcpy(&blockhead, &buffer[offset], sizeof(blockhead));
            offset += sizeof(blockhead);

            i += sizeof(blockhead);
            if(blockhead.id!=' tmf')
            {
                offset += blockhead.size;

                i += blockhead.size;
                continue;
            }
            memcpy(wf, &buffer[offset], blockhead.size);
            offset += blockhead.size;

            i += blockhead.size;
            wf->cbSize=0;
            *block_size=(SLONG)riffhead.size-i;
            
            point = offset;
            while(1)
            {
                if(i >= (int)riffhead.size) break;
                memcpy(&blockhead, &buffer[offset], sizeof(blockhead));
                offset += sizeof(blockhead);

                i += sizeof(blockhead);
                if(blockhead.id == 'atad') *wave_size += blockhead.size;
                offset += blockhead.size;

                i += blockhead.size;
            }
            *buffer_offset = point;

            return TTN_OK;
        }
    }
}


EXPORT  SLONG   FNBACK  get_buffer_wav_data(USTR *buffer, SLONG *buffer_index, USTR *data, SLONG blocksize)
{
    SLONG   i, j, offset, index;
    struct 
    {
        DWORD id;
        DWORD size;
    } blockhead;
    
    offset = *buffer_index;
    index = 0;
    i = j = 0;
    while(1)
    {
        if(i >= blocksize) break;
        memcpy(&blockhead, &buffer[offset], sizeof(blockhead));
        offset += sizeof(blockhead);

        i += sizeof(blockhead);
        if(blockhead.id != 'atad')
        {
            offset += blockhead.size;

            i += blockhead.size;
            continue;
        }
        memcpy(&data[index], &buffer[offset], blockhead.size);
        index += j;
        offset += blockhead.size;

        j += blockhead.size;
        i += blockhead.size;
    }
    return TTN_OK;
}



EXPORT  SAMPLE* FNBACK  load_wav_from_file(USTR *filename)
{
    WAVEFORMATEX wf;
    PACK_FILE *hfile = NULL;
    SLONG   block_size,buffer_bytes;
    SAMPLE  * sample = NULL;
    
    if( NULL == (hfile = pack_fopen((const char *)filename, "rb")) )
    {
        log_error(1, "open file %s error", filename);
        goto _some_error;
    }
    if(TTN_OK != get_wav_format(hfile, &wf, &block_size, &buffer_bytes))
    {
        log_error(1, "get wav %s format error", filename);
        goto _some_error;
    }
    if(NULL == (sample = (SAMPLE *)malloc(sizeof(SAMPLE) + buffer_bytes - sizeof(USTR))))
    {
        log_error(1, "sample memory alloc error");
        goto _some_error;
    }

    memcpy(&sample->wf, &wf, sizeof(WAVEFORMATEX));
    sample->buffer_bytes = buffer_bytes;
    sample->number = -1;

    if(TTN_OK != get_wav_data(hfile, sample->buffer, block_size))
    {
        log_error(1, "get wav %s data error", filename);
        goto _some_error;
    }
    pack_fclose(hfile);
    return sample;

_some_error:
    if(hfile) { pack_fclose(hfile); hfile = NULL; }
    if(sample) { free(sample); sample = NULL; }
    return NULL;
}


EXPORT  SAMPLE* FNBACK  load_wav_from_buffer(USTR *buffer, SLONG buffer_size)
{
    WAVEFORMATEX wf;
    SLONG   block_size,buffer_bytes;
    SLONG   buffer_offset;
    SAMPLE  * sample = NULL;
    
    buffer_offset = 0;
    if(TTN_OK != get_buffer_wav_format(buffer, &buffer_offset, &wf, &block_size, &buffer_bytes))
    {
        log_error(1, "get buffer wav format error");
        goto _some_error;
    }
    if(NULL == (sample = (SAMPLE *)malloc(sizeof(SAMPLE) + buffer_bytes - sizeof(USTR))))
    {
        log_error(1, "sample memory alloc error");
        goto _some_error;
    }

    memcpy(&sample->wf, &wf, sizeof(WAVEFORMATEX));
    sample->buffer_bytes = buffer_bytes;
    sample->number = -1;

    if(TTN_OK != get_buffer_wav_data(buffer, &buffer_offset, sample->buffer, block_size))
    {
        log_error(1, "get buffer wav data error");
        goto _some_error;
    }
    return sample;

_some_error:
    if(sample) { free(sample); sample = NULL; }
    return NULL;
}


EXPORT  SLONG   FNBACK  get_wavfile_information(USTR *filename, ULONG *channels, ULONG *sample_rate, 
                                                ULONG *bps, ULONG *size, ULONG *play_time)
{
    WAVEFORMATEX wf;
    PACK_FILE *hfile = NULL;
    SLONG   block_size, buffer_bytes;
    
    if( NULL == (hfile = pack_fopen((const char *)filename, "rb")) )
    {
        log_error(1, "open file %s error", filename);
        goto _some_error;
    }
    if(TTN_OK != get_wav_format(hfile, &wf, &block_size, &buffer_bytes))
    {
        log_error(1, "get wav %s format error", filename);
        goto _some_error;
    }
    pack_fclose(hfile);

    if(channels) *channels = wf.nChannels;
    if(sample_rate) *sample_rate = wf.nSamplesPerSec;
    if(bps) *bps = wf.wBitsPerSample;
    if(size) *size = buffer_bytes;
    if(play_time) *play_time = buffer_bytes * 1000 / wf.nAvgBytesPerSec;

    return TTN_OK;

_some_error:
    if(hfile) { pack_fclose(hfile); hfile = NULL; }
    return TTN_NOT_OK;
}


EXPORT  void    FNBACK  destroy_sample(SAMPLE **sample)
{
    if(*sample)
    {
        free(*sample);
        *sample = NULL;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MP3 MUSIC FUNCTIONS                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define	MP3_INIT			0x01
#define	MP3_PLAY			0x02

#define MBUFFERSIZE         ((44100 * ((16*2)>>3) * 2) & 0xfffffff8)    //44kHz,16bit, stero, 2 sec.
#define PLAYLOOP            0x00000001

static	DWORD   dwMp3Flag = 0;
static  char    mp3_filename[_MAX_FNAME];
static  void    *mp3file = NULL;
static  DWORD WINAPI    _mp3_thread(LPVOID p);
static  int             _mp3_next_stream(int point,int size);


int     _init_mp3_music(void)
{
	if(dwMp3Flag & MP3_INIT)	// have already initialized
		return 0;

    StreamSound.lpDSB = NULL;
    StreamSound.lpDSN = NULL;
    StreamSound.music = NULL;
    StreamSound.pan = 0;
    StreamSound.volume = 0;
    StreamSound.status = -1;

	dwMp3Flag = 0;
	dwMp3Flag |= MP3_INIT;
	return 0;
}

void    _active_mp3_music(int active)
{
    DWORD   status;
//    LPDIRECTSOUNDBUFFER lpDSB;

    if(! active) return;

    if(! StreamSound.lpDSB)
        return;

    StreamSound.lpDSB->GetStatus(&status);
    if(status & DSBSTATUS_BUFFERLOST)
    {
        StreamSound.lpDSB->Restore();
    }
}


void    _free_mp3_music(void)
{
	if( !(dwMp3Flag & MP3_INIT) )
		return;

	_stop_mp3_music();

	dwMp3Flag = 0;
}


void    _play_mp3_music(SLONG music_no, SLONG loop)
{
    char    filename[_MAX_FNAME];
    WAVEFORMATEX *wfx;
    HANDLE  event;

	if( !(dwMp3Flag & MP3_INIT ) )	// have not init
		return;
	if( dwMp3Flag & MP3_PLAY )	// have been play
		return;

    sprintf((char *)filename, "music\\mus%03d.mp3", music_no);
    strcpy((char*)mp3_filename, (const char *)filename);

    if(StreamSound.music)
        _stop_mp3_music();
    StreamSound.status = loop;


    //打开mp3文件, 并且取得mp3的声音格式
    mp3file = mp3_open((char *)filename);
    if(NULL == mp3file)
    {
        log_error(1, "open mp3 file %s error", filename);
        return;
    }
    wfx = (WAVEFORMATEX *)mp3_waveformatex(mp3file);

    //创建数据流的声音缓冲, 缓冲标志为可以获得当前播放位置, 播放位置通知, 可控制均衡, 可控制音量.
	DSBUFFERDESC dsbdesc;
	dsbdesc.dwSize = sizeof(dsbdesc);
    dsbdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 
        | DSBCAPS_CTRLPOSITIONNOTIFY 
        | DSBCAPS_CTRLPAN 
        | DSBCAPS_CTRLVOLUME;
	dsbdesc.dwBufferBytes = MBUFFERSIZE;
	dsbdesc.lpwfxFormat = wfx;
	dsbdesc.dwReserved=0;
	if(FAILED(g_pDS->CreateSoundBuffer(&dsbdesc, &(StreamSound.lpDSB),NULL)))
    {
        return;
	}

    //获得位置通知接口.
	if(FAILED(StreamSound.lpDSB->QueryInterface(IID_IDirectSoundNotify,(void **)&StreamSound.lpDSN)))
    {
        return;
	}

    //创建两个通知事件, 分别对应声音缓冲的起始位置和一半大小的位置.
	event = CreateEvent(NULL, FALSE, FALSE, NULL);
    hStreamEvent[0] = event;
	StreamSound.dsbpn[0].hEventNotify = event;
	StreamSound.dsbpn[0].dwOffset = 0;
	event = CreateEvent(NULL, FALSE, FALSE, NULL);
	hStreamEvent[1] = event;
	StreamSound.dsbpn[1].hEventNotify = event;
	StreamSound.dsbpn[1].dwOffset = MBUFFERSIZE >> 1;

    //设置播放位置事件通知.
	StreamSound.pan = 0;
	StreamSound.volume = 0;
	if(FAILED(StreamSound.lpDSN->SetNotificationPositions(2, StreamSound.dsbpn)))
    {
        return;
	}
    //Jack, 2002.6.5. todo:
	StreamSound.music = (MUSIC*)mp3file;
    
    //创建一个单独的线程, 用来处理数据流声音的位置通知事件.
	if(!(hStreamThread = CreateThread(NULL, 0, _mp3_thread, NULL, 0, &dwStreamThreadID)))
	{
        return;
	}

    //读入一半缓冲大小的流数据.
	if(_mp3_next_stream(0, MBUFFERSIZE>>1) ) return;

    //设置均衡, 音量, 开始播放.
	StreamSound.lpDSB->SetCurrentPosition(0);
	StreamSound.lpDSB->SetPan(StreamSound.pan);
	StreamSound.lpDSB->SetVolume(StreamSound.volume);
	StreamSound.lpDSB->Play(0, 0, DSBPLAY_LOOPING);

    dwMp3Flag |= MP3_PLAY;
}


void    _stop_mp3_music(void)
{
	if( !( dwMp3Flag & MP3_INIT) )	// not init
		return;
	if( !( dwMp3Flag & MP3_PLAY) )	// not play
		return;

    if(! StreamSound.lpDSB)
        return;

    //如果没有声音数据流源, 马上返回.
    if(! StreamSound.music)
        return;

    //停止播放, 并将播放位置重置.
    StreamSound.lpDSB->Stop();
    StreamSound.lpDSB->SetCurrentPosition(0);
	StreamSound.status = -1;
	StreamSound.music = NULL;

    //中止播放线程.
	if(hStreamThread)
    {
		TerminateThread(hStreamThread, 0);
		Sleep(200);
	}

    //释放播放位置事件通知.
    StreamSound.lpDSN->Release();
    StreamSound.lpDSN = NULL;

    //释放数据流声音缓冲.
	StreamSound.lpDSB->Release();
    StreamSound.lpDSB = NULL;

    //关闭通知事件.
	CloseHandle(hStreamEvent[0]);
	CloseHandle(hStreamEvent[1]);

	dwMp3Flag &= ~MP3_PLAY;
}

void    _set_mp3_music_volume(SLONG volume)
{
	if( !(dwMp3Flag & MP3_INIT ) )
		return;
    if(! StreamSound.lpDSB)
        return;

    if( StreamSound.volume != volume )
    {
        StreamSound.lpDSB->SetVolume(volume);
        StreamSound.volume = volume;
    }
}

void    _set_mp3_music_pan(SLONG pan)
{
	if( !(dwMp3Flag & MP3_INIT ) )
		return;
    if(! StreamSound.lpDSB)
        return;
    if( StreamSound.pan != pan )
    {
        StreamSound.lpDSB->SetPan(pan);
        StreamSound.pan = pan;
    }
}


SLONG   _is_mp3_music_playing(void)
{
    DWORD dwStatus = 0;

	if( !(dwMp3Flag & MP3_INIT ) )
		return FALSE;
	if( !(dwMp3Flag & MP3_PLAY) )
		return FALSE;

    if( ! StreamSound.lpDSB)
        return  FALSE;

    StreamSound.lpDSB->GetStatus( &dwStatus );
    return( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
}


int     _mp3_next_stream(int point,int size)
{
	char * buffer;
	int bufferbyte,s;

	if (StreamSound.status < 0) 
    {
		StreamSound.lpDSB->Stop();
        StreamSound.lpDSB->SetCurrentPosition(0);

        mp3_close(mp3file);
        mp3file = mp3_open((char *)mp3_filename);

		StreamSound.music = NULL;
		return 0;
	}

	if(FAILED(StreamSound.lpDSB->Lock(point, size, (void **)&buffer,(DWORD *)&bufferbyte,NULL,NULL,0)))
		return FailMsg("Lock stream sound buffer failed !");

	if((s = mp3_read(mp3file, buffer,bufferbyte)) < bufferbyte)
    {
        if (StreamSound.status == PLAYLOOP)
        {
            if(s <= 0)
            {
                mp3_close(mp3file);
                mp3file = mp3_open((char *)mp3_filename);
                mp3_read(mp3file, buffer, bufferbyte);
            }
            else
            {
                memset(buffer+s, bufferbyte-s, 0);
            }
        }
        else
        {
            memset(buffer, bufferbyte, 0);
			StreamSound.status = -1;
        }
	}
    log_error(1, "read mp3 len = %d", s);
	StreamSound.lpDSB->Unlock(buffer, bufferbyte, NULL, 0);

	return 0;
}



DWORD WINAPI _mp3_thread(LPVOID p)
{
    DWORD i;

    for (;;) 
    {
        i = MsgWaitForMultipleObjects(2, hStreamEvent, FALSE, INFINITE, 0);
        switch(i)
        {
        case 0: //当前播放指标在起始位置, 所以开始读入数据到后一半缓冲区.
            if ( _mp3_next_stream(MBUFFERSIZE>>1, MBUFFERSIZE>>1) )
            {
                StreamSound.lpDSB->Stop();
                StreamSound.lpDSB->SetCurrentPosition(0);
                StreamSound.music = NULL;
                StreamSound.status = -1;
            }
            break;

        case 1: //当前播放指标在缓冲区一半的位置, 所以开始读入数据到前一半缓冲区.
            if ( _mp3_next_stream(0, MBUFFERSIZE>>1) ) 
            {
                StreamSound.lpDSB->Stop();
                StreamSound.lpDSB->SetCurrentPosition(0);
                StreamSound.music = NULL;
                StreamSound.status = -1;
            }
            break;
        }
    }
    return 0;
}