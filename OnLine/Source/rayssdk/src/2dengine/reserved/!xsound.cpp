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
#include "xwavread.h"
#include "packfile.h"

#pragma comment(lib,"dsound")




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define DS_INIT             0x01

#define SAFE_DELETE(p)      { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p)     { if(p) { (p)->Release(); (p)=NULL; } }


SOUND_CFG   sound_cfg;


//-----------------------------------------------------------------------------
// Static values
//-----------------------------------------------------------------------------
static  LPDIRECTSOUND       g_pDS                                   = NULL;
static  LPDIRECTSOUNDBUFFER g_pDSBuffer[MAX_SOUND_CHANNELS]         = {NULL};
static  LPDIRECTSOUNDNOTIFY g_pDSNotify[MAX_SOUND_CHANNELS]         = {NULL};
static  CWaveSoundRead*     g_pWaveSoundRead[MAX_SOUND_CHANNELS]    = {NULL};
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
static  HRESULT CreateStaticBuffer( LONG lChannel, TCHAR* strFileName );
static  HRESULT FillBuffer( LONG lChannel );
static  HRESULT PlayBuffer( LONG lChannel, BOOL bLooped );
static  VOID    StopBuffer( LONG lChannel, BOOL bResetPosition ); 
static  BOOL    IsSoundPlaying( LONG lChannel );
static  HRESULT RestoreBuffers( LONG lChannel );
static	LONG	GetFreeChannel( LONG lStart, LONG lEnd );

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

    // Set primary buffer format to 22kHz and 16-bit output.
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

    sprintf((char *)print_rec,"      Wave format : %s", "PCM Wave" );
    log_error(1,print_rec);
    sprintf((char *)print_rec,"         Channels : %d", wfx.nChannels );
    log_error(1,print_rec);
    sprintf((char *)print_rec,"  Samples per sec : %d", wfx.nSamplesPerSec );
    log_error(1,print_rec);
    sprintf((char *)print_rec,"  Bits per sample : %d", wfx.wBitsPerSample );
    log_error(1,print_rec);


    SAFE_RELEASE( pDSBPrimary );

    dwDSFlag |= DS_INIT;

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
        SAFE_DELETE( g_pWaveSoundRead[i] );

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




//-----------------------------------------------------------------------------
// Name: CreateStaticBuffer()
// Desc: Creates a wave file, sound buffer and notification events 
//-----------------------------------------------------------------------------
HRESULT CreateStaticBuffer( LONG lChannel, TCHAR* strFileName )
{
    HRESULT hr; 

    if( ! ( dwDSFlag & DS_INIT ) )
        return S_OK;

    // Free any previous globals 
    SAFE_DELETE( g_pWaveSoundRead[lChannel] );
    SAFE_RELEASE( g_pDSBuffer[lChannel] );

    // Create a new wave file class
    g_pWaveSoundRead[lChannel] = new CWaveSoundRead();

    // Load the wave file
    if( FAILED( hr = g_pWaveSoundRead[lChannel]->Open( strFileName ) ) )
    {
        //sprintf((char *)print_rec,"channel=%d : bad wave file %s",lChannel,strFileName);
        //log_error(1,print_rec);
        return hr;
    }

    // Set up the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_STATIC | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = g_pWaveSoundRead[lChannel]->m_ckIn.cksize;
    dsbd.lpwfxFormat   = g_pWaveSoundRead[lChannel]->m_pwfx;

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
    BYTE*   pbWavData; // Pointer to actual wav data 
    UINT    cbWavSize; // Size of data
    VOID*   pbData  = NULL;
    VOID*   pbData2 = NULL;
    DWORD   dwLength;
    DWORD   dwLength2;

    if( ! ( dwDSFlag & DS_INIT ) )
        return S_OK;

    // The size of wave data is in pWaveFileSound->m_ckIn
    INT nWaveFileSize = g_pWaveSoundRead[lChannel]->m_ckIn.cksize;

    // Allocate that buffer.
    pbWavData = new BYTE[ nWaveFileSize ];
    if( NULL == pbWavData )
        return E_OUTOFMEMORY;

    if( FAILED( hr = g_pWaveSoundRead[lChannel]->Read( nWaveFileSize, 
                                           pbWavData, 
                                           &cbWavSize ) ) )           
        return hr;

    // Reset the file to the beginning 
    g_pWaveSoundRead[lChannel]->Reset();

    // Lock the buffer down
    if( FAILED( hr = g_pDSBuffer[lChannel]->Lock( 0, g_dwBufferBytes[lChannel], &pbData, &dwLength, 
                                   &pbData2, &dwLength2, 0L ) ) )
        return hr;

    // Copy the memory to it.
    memcpy( pbData, pbWavData, g_dwBufferBytes[lChannel] );

    // Unlock the buffer, we don't need it anymore.
    g_pDSBuffer[lChannel]->Unlock( pbData, g_dwBufferBytes[lChannel], NULL, 0 );
    pbData = NULL;

    // We dont need the wav file data buffer anymore, so delete it 
    SAFE_DELETE( pbWavData );

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
EXPORT  int     FNBACK  init_sound(void)
{
    HRESULT hr;

    sprintf((char *)print_rec,"SOUND module initialize starting...");
    log_error(1,print_rec);

    init_sound_cfg();

    hr = InitDirectSound( g_hDDWnd );
    if(S_OK != hr)
        return(-1);
    
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
    for(i=0;i<MAX_SOUND_CHANNELS;++i)
    {
        RestoreBuffers( i );
    }
}


EXPORT  void    FNBACK  free_sound(void)
{
    HRESULT hr;

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
    //HRESULT hr;

    if(! sound_cfg.voice_flag)
        return;

    if(channel < VOICE_CHANNEL_MIN || channel > VOICE_CHANNEL_MAX)
        return;

    /*
    if(FAILED( hr = LoadWaveFile( (LONG)channel, (TCHAR*)filename ) ) )
    {
        sprintf((char*)print_rec, "load voice [%d]%s error", channel, filename);
        log_error(1, print_rec);
        return;
    }

    if(FAILED( hr = PlayBuffer( (LONG)channel, loop) ) )
    {
        sprintf((char *)print_rec, "play voice [%d]%s error", channel, filename);
        log_error(1, print_rec);
        return;
    }
    */

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


EXPORT  void    FNBACK  play_music(SLONG music_no, SLONG loop)
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

    if(FAILED( hr = PlayBuffer( (LONG)MUSIC_CHANNEL, loop) ) )
    {
        sprintf((char *)print_rec, "play music [%d]%s error", channel, filename);
        log_error(1, print_rec);
        return;
    }

    //Play music OK.
    return;
}



EXPORT  void    FNBACK  stop_music(void)
{
    if(! sound_cfg.music_flag)
        return;

    StopBuffer( MUSIC_CHANNEL, 1);
    sound_cfg.music_no = -1;
}


EXPORT  void    FNBACK  set_music_volume(SLONG volume)
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


EXPORT  void    FNBACK  set_music_pan(SLONG pan)
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


EXPORT  SLONG   FNBACK  is_music_playing(void)
{
    if(! sound_cfg.music_flag)
        return FALSE;

    return IsSoundPlaying((LONG)MUSIC_CHANNEL);
}


EXPORT  SLONG   FNBACK  get_wavfile_information(USTR *filename, ULONG *channels, ULONG *sample_rate, 
                                                ULONG *bps, ULONG *size, ULONG *play_time)
{
    CWaveSoundRead* wsr = NULL;
    DWORD   dwTotalBytes;
    DWORD   dwAvgBytesPerSec;
    

    HRESULT hr;

    wsr = new CWaveSoundRead();
    // Load the wave file
    if( FAILED( hr = wsr->Open( (char *) filename ) ) )
    {
        sprintf((char *)print_rec,"bad wave file %s", filename);
        log_error(1,print_rec);
        return TTN_ERROR;
    }

    if(channels) *channels = wsr->m_pwfx->nChannels;
    if(sample_rate) *sample_rate = wsr->m_pwfx->nSamplesPerSec;
    if(bps) *bps = wsr->m_pwfx->wBitsPerSample;

    dwTotalBytes = wsr->m_ckIn.cksize;
    //dwAvgBytesPerSec = wsr->m_pwfx->nAvgBytesPerSec;
    dwAvgBytesPerSec = wsr->m_pwfx->nChannels * wsr->m_pwfx->nSamplesPerSec * wsr->m_pwfx->wBitsPerSample / 8;

    if(size) *size = dwTotalBytes;
    if(play_time) *play_time = dwTotalBytes * 1000 / dwAvgBytesPerSec ;

    wsr->Close();
    delete wsr;

    return TTN_OK;
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
                    return FALSE;
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
        if((pack_fread(data += j,1, blockhead.size, hfile)) != blockhead.size)
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

        //if( (pack_fread(&riffhead,1, sizeof(riffhead), hfile)) != sizeof(riffhead) )
            //return TTN_ERROR;
        memcpy(&riffhead, &buffer[offset], sizeof(riffhead));
        offset += sizeof(riffhead);

        if(riffhead.id != 'FFIR')
        {
            //pack_fseek(hfile,riffhead.size,SEEK_CUR);
            offset += riffhead.size;

            continue;
        }
        //if( (pack_fread(&waveid,1, sizeof(waveid), hfile)) != sizeof(waveid) )
          //  return TTN_ERROR;
        memcpy(&waveid, &buffer[offset], sizeof(waveid));
        offset += sizeof(waveid);

        i += sizeof(waveid);
        if(waveid!='EVAW')
        {
            //pack_fseek(hfile, riffhead.size-i, SEEK_CUR);
            offset += ((SLONG)riffhead.size - i);

            continue;
        }
        while(1)
        {
            if(i >= (SLONG)riffhead.size) break;

            //if((pack_fread(&blockhead,1, sizeof(blockhead), hfile)) != sizeof(blockhead))
              //  return TTN_ERROR;
            memcpy(&blockhead, &buffer[offset], sizeof(blockhead));
            offset += sizeof(blockhead);

            i += sizeof(blockhead);
            if(blockhead.id!=' tmf')
            {
                //pack_fseek(hfile, blockhead.size, SEEK_CUR);
                offset += blockhead.size;

                i += blockhead.size;
                continue;
            }
            //if((pack_fread(wf,1, blockhead.size, hfile)) != blockhead.size)
              //  return TTN_ERROR;
            memcpy(wf, &buffer[offset], blockhead.size);
            offset += blockhead.size;

            i += blockhead.size;
            wf->cbSize=0;
            *block_size=(SLONG)riffhead.size-i;
            
            //point = pack_ftell(hfile);
            point = offset;

            while(1)
            {
                if(i >= (int)riffhead.size) break;
                //if((pack_fread(&blockhead, 1, sizeof(blockhead), hfile)) != sizeof(blockhead))
                  //  return FALSE;
                memcpy(&blockhead, &buffer[offset], sizeof(blockhead));
                offset += sizeof(blockhead);

                i += sizeof(blockhead);
                if(blockhead.id == 'atad') *wave_size += blockhead.size;
                //pack_fseek(hfile, blockhead.size, SEEK_CUR);
                offset += blockhead.size;

                i += blockhead.size;
            }
            //pack_fseek(hfile,point,SEEK_SET);
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
        //if((pack_fread(&blockhead,1, sizeof(blockhead), hfile)) != sizeof(blockhead))
          //  return TTN_ERROR;
        memcpy(&blockhead, &buffer[offset], sizeof(blockhead));
        offset += sizeof(blockhead);

        i += sizeof(blockhead);
        if(blockhead.id != 'atad')
        {
            //pack_fseek(hfile, blockhead.size, SEEK_CUR);
            offset += blockhead.size;

            i += blockhead.size;
            continue;
        }
        //if((pack_fread(data += j,blockhead.size, hfile)) != blockhead.size)
          //  return TTN_ERROR;
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


EXPORT  SLONG   FNBACK  get_wavfile_information2(USTR *filename, ULONG *channels, ULONG *sample_rate, 
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

