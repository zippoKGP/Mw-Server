/*
**      XCDROM.CPP
**      CDROM Audio functions.
**
**      ZJian, 1999.05.25.
**          first version.
**      ZJian, 2000.8.4.
**          change function name for lower case strings.
**          make init_cdrom_music(),active_cdrom_music() and free_cdrom_music() 
**              for model install support.
**
*/
#include "rays.h"
#include "xsystem.h"
#include "xcdrom.h"
#include "xtimer.h"

#define MAX_CD_TRACKS           256
#define CDROM_TIMER_TPS         100             // ticks per second

#define CDROM_INIT              0x01
#define CDROM_START             0x02
#define CDROM_PAUSE             0x04

//cdrom_timer_tick in "xtimer.h"
extern  ULONG   cdrom_timer_tick;

static  MCIDEVICEID             mci_device_id;
static  MCI_OPEN_PARMS          mci_open_parms;
static  MCI_GENERIC_PARMS       mci_generic;
static  MCI_SET_PARMS           mci_set_parms;
static  MCI_STATUS_PARMS        mci_status_parms;
static  MCI_PLAY_PARMS          mci_play_parms;
static  ULONG       cdrom_flag=0;
static  MCIERROR    cdrom_result;

static  SLONG   cdrom_minute[MAX_CD_TRACKS];
static  SLONG   cdrom_second[MAX_CD_TRACKS];
static  SLONG   cdrom_frame [MAX_CD_TRACKS];
static  SLONG   cdrom_total  = 0;
static  SLONG   cdrom_track  = -1;
static  SLONG   play_minute;
static  SLONG   play_second;
static  SLONG   play_frame;
static  ULONG   play_tick;



EXPORT int FNBACK init_cdrom_music(void)
{
    DWORD data;
    SLONG i;

    cdrom_flag = 0;
    cdrom_total = 0;
    memset(&cdrom_minute[0],0x00,sizeof(SLONG)*MAX_CD_TRACKS);
    memset(&cdrom_second[0],0x00,sizeof(SLONG)*MAX_CD_TRACKS);
    memset(&cdrom_frame[0], 0x00,sizeof(SLONG)*MAX_CD_TRACKS);

    //(1) open cdrom device ------------------------------------------------------------
    mci_open_parms.dwCallback         = 0;
    mci_open_parms.wDeviceID          = 0;
    mci_open_parms.lpstrDeviceType    = "cdaudio";//MCI_DEVTYPE_CD_AUDIO
    mci_open_parms.lpstrElementName   = NULL;
    mci_open_parms.lpstrAlias         = NULL;
    // MCI_OPEN:    Initialize a device or file
    cdrom_result = mciSendCommand( 0,MCI_OPEN,MCI_OPEN_TYPE,(ULONG)(LPVOID)&mci_open_parms);
    if(cdrom_result)
        return(-1);
    mci_device_id = mci_open_parms.wDeviceID;

    //(2) set cdrom device time format ---------------------------------------------------
    mci_set_parms.dwCallback   = 0;
    mci_set_parms.dwTimeFormat = MCI_FORMAT_MSF;
    mci_set_parms.dwAudio      = 0;
    cdrom_result=mciSendCommand(mci_device_id,MCI_SET,MCI_SET_TIME_FORMAT,
        (ULONG)(LPVOID)&mci_set_parms);
    if(cdrom_result)
        return(-1);
    //(3) get total cdrom tracks ---------------------------------------------------------
    mci_status_parms.dwCallback    = 0;
    mci_status_parms.dwReturn      = 0;
    mci_status_parms.dwItem        = MCI_STATUS_NUMBER_OF_TRACKS;
    mci_status_parms.dwTrack       = 0;
    cdrom_result=mciSendCommand(mci_device_id,MCI_STATUS,MCI_STATUS_ITEM,
        (ULONG)(LPVOID)&mci_status_parms);
    if(cdrom_result)
        return(-1);
    cdrom_total=mci_status_parms.dwReturn;
    if(cdrom_total >= MAX_CD_TRACKS)
        return(-1);
    //(4) get track time length information ----------------------------------------------
    for(i=1;i<=cdrom_total;i++)
    {
        mci_status_parms.dwCallback = 0;
        mci_status_parms.dwReturn   = 0;
        mci_status_parms.dwItem     = MCI_STATUS_LENGTH;
        mci_status_parms.dwTrack    = i;
        cdrom_result=mciSendCommand(mci_device_id,MCI_STATUS,MCI_STATUS_ITEM|MCI_TRACK,
            (ULONG)(LPVOID)&mci_status_parms);
        if(cdrom_result)
            return(-1);
        data=mci_status_parms.dwReturn;
        cdrom_minute[i-1] = MCI_MSF_MINUTE(data);
        cdrom_second[i-1] = MCI_MSF_SECOND(data);
        cdrom_frame[i-1]  = MCI_MSF_FRAME (data);
    }
    cdrom_flag |= CDROM_INIT;
    return(0);
}



EXPORT void FNBACK free_cdrom_music(void)
{
    if(!(cdrom_flag & CDROM_INIT)) return;
    //(1) stop cdrom device ---------------------------------------------------
    mci_generic.dwCallback=0;
    cdrom_result=mciSendCommand(mci_device_id,MCI_STOP,0,(ULONG)(LPVOID)&mci_generic);
    if(cdrom_result)
        return;
    //(2) close cdrom device --------------------------------------------------
    mci_generic.dwCallback=0;
    cdrom_result=mciSendCommand(mci_device_id,MCI_CLOSE,0,(ULONG)(LPVOID)&mci_generic);
    if(cdrom_result)  // if not successful
        return;
    cdrom_flag = 0;
    play_tick=play_minute=play_second=play_frame=0;
}



EXPORT void FNBACK active_cdrom_music(int active)
{
    if(!(cdrom_flag & CDROM_INIT)) return;
    if(active)
    {
        // active cdrom music now
    }
    else
    {
        // pause cdrom music ??
    }
}



EXPORT void FNBACK pause_cdrom_music(void)
{
    if(!(cdrom_flag & CDROM_INIT)) return;
    if(!(cdrom_flag & CDROM_START)) return;
    if(cdrom_flag & CDROM_PAUSE) return;
    
    play_tick=cdrom_timer_tick;
    play_frame=(play_tick%CDROM_TIMER_TPS)*100/CDROM_TIMER_TPS; // 1/100 second
    play_second=(play_tick/CDROM_TIMER_TPS)%60; // second
    play_minute=(play_tick/CDROM_TIMER_TPS)/60; // minutes
    
    mci_generic.dwCallback=0;
    //MCI_PAUSE     Pause the current action (for audio cd,just like MCI_STOP)
    cdrom_result=mciSendCommand(mci_device_id,MCI_STOP,0,(ULONG)(LPVOID)&mci_generic);
    // cdrom_result=mciSendCommand(mci_device_id,MCI_PAUSE,0,(ULONG)(LPVOID)&mci_generic);
    if(cdrom_result) // not successful
        return;
    cdrom_flag |= CDROM_PAUSE;
}



EXPORT void FNBACK resume_cdrom_music(void)
{
    if(!(cdrom_flag & CDROM_INIT)) return;
    if(!(cdrom_flag & CDROM_START)) return;
    if(!(cdrom_flag & CDROM_PAUSE)) return;
    if(cdrom_track<0) return;
    
    mci_set_parms.dwCallback   = 0;
    mci_set_parms.dwTimeFormat = MCI_FORMAT_TMSF;
    mci_set_parms.dwAudio      = 0;
    cdrom_result=mciSendCommand(mci_device_id,MCI_SET,MCI_SET_TIME_FORMAT,
        (ULONG)(LPVOID)&mci_set_parms);
    if(cdrom_result)  // not successful
        return;
    
    cdrom_timer_tick=play_tick;
    mci_play_parms.dwCallback = 0;
    mci_play_parms.dwFrom     = MCI_MAKE_TMSF(cdrom_track+1,play_minute,
        play_second,play_frame);
    mci_play_parms.dwTo       = MCI_MAKE_TMSF(cdrom_track+1,cdrom_minute[cdrom_track],
        cdrom_second[cdrom_track],cdrom_frame[cdrom_track]);
    
    cdrom_result=mciSendCommand(mci_device_id,MCI_PLAY,MCI_FROM|MCI_TO|MCI_NOTIFY,
        (ULONG)(LPVOID)&mci_play_parms);
    if(cdrom_result)  // not successful
        return;
    cdrom_flag &= ~CDROM_PAUSE;
}



EXPORT void FNBACK stop_cdrom_music(void)
{
    if(!(cdrom_flag & CDROM_INIT)) return;
    if(!(cdrom_flag & CDROM_START)) return;
    play_tick=play_minute=play_second=play_frame=0;
    mci_generic.dwCallback=0;
    //MCI_STOP     Stop all play and record sequences...
    cdrom_result=mciSendCommand(mci_device_id,MCI_STOP,0,(ULONG)(LPVOID)&mci_generic);
    if(cdrom_result)
        return;
    cdrom_flag &= ~CDROM_START;
    cdrom_flag &= ~CDROM_PAUSE;
}



EXPORT void FNBACK play_cdrom_music(SLONG track)
{
    if(!(cdrom_flag & CDROM_INIT)) return;
    if(cdrom_flag & CDROM_START) return;
    
    //MCI_SET   Set device information
    mci_set_parms.dwCallback   = 0;
    mci_set_parms.dwTimeFormat = MCI_FORMAT_TMSF;
    mci_set_parms.dwAudio      = 0;
    cdrom_result=mciSendCommand(mci_device_id,MCI_SET,MCI_SET_TIME_FORMAT,
        (ULONG)(LPVOID)&mci_set_parms);
    if(cdrom_result) 
        return;
    //MCI_PLAY  Signals the device to begin transmitting output data
    mci_play_parms.dwCallback = 0;
    mci_play_parms.dwFrom     = MCI_MAKE_TMSF(track+1,0,0,0);
    mci_play_parms.dwTo       = MCI_MAKE_TMSF(track+1,cdrom_minute[track],
        cdrom_second[track],cdrom_frame[track]);
    cdrom_result=mciSendCommand(mci_device_id,MCI_PLAY,MCI_FROM|MCI_TO|MCI_NOTIFY,
        (ULONG)(LPVOID)&mci_play_parms);
    if(cdrom_result)
        return;
    cdrom_track=track;
    play_minute=play_second=play_frame=play_tick=0;
    cdrom_timer_tick=play_tick;
    cdrom_flag |= CDROM_START;
    cdrom_flag &= ~CDROM_PAUSE;
}



EXPORT SLONG FNBACK status_cdrom_music(void)
{
    ULONG    data;
    
    if(!(cdrom_flag & CDROM_INIT)) return(-1);
    //MCI_STATUS    Retrives infomation about an MCI device
    mci_status_parms.dwCallback    = 0;
    mci_status_parms.dwReturn      = 0;
    mci_status_parms.dwItem        = MCI_STATUS_MODE;
    mci_status_parms.dwTrack       = 0;
    cdrom_result=mciSendCommand(mci_device_id,MCI_STATUS,MCI_WAIT|MCI_STATUS_ITEM,
        (ULONG)(LPVOID)&mci_status_parms);
    if(cdrom_result)
        return(-1);
    data=mci_status_parms.dwReturn;
    if(data==MCI_MODE_STOP)
        return(0);
    return(-1);
}



EXPORT void FNBACK loop_cdrom_music(void)
{
    SLONG   track;
    if(!(cdrom_flag & CDROM_INIT)) return;
    if((status_cdrom_music()==0)&&(cdrom_track != -1))
    {
        track=cdrom_track;
        stop_cdrom_music();
        play_cdrom_music(track);
    }
}



EXPORT SLONG FNBACK total_cdrom_music(void)
{
    return cdrom_total;
}
