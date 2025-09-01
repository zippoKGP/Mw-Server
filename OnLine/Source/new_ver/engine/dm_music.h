/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : dm_music.h
*owner        : Andy
*description  : 
*modified     : 2004/1/13
******************************************************************************/ 

#ifndef __DM_MUSIC_H__
#define __DM_MUSIC_H__

#define WM_GRAPHNOTIFY  (WM_USER+265)

HRESULT musicOpen(LPCTSTR strFileName);

void musicPlay(void);

void musicPause(void);

void musicStop(void);

void setMusicVolume(int volume);

HRESULT destoryDirectShow(void);

HRESULT handleMusicEvent(void);

#endif //#define __DM_MUSIC_H__