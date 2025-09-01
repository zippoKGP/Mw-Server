/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : sound.h
*owner        : Andy
*description  : 
*modified     : 2004/04/14
******************************************************************************/ 

#ifndef __SOUND_H__
#define __SOUND_H__

#include "tier0/platform.h"
#include <windows.h>

class CSound;

class ISoundMgr
{
public:
	virtual HRESULT Init(HWND hWnd, DWORD dwCoopLevel) = 0;

	virtual HRESULT Shutdown(void) = 0;
	
	virtual CSound *getSound(LPCTSTR strFileName) = 0; 

	virtual void deleteSound(CSound *pSound) = 0;
	
	virtual void playSound(CSound *pSound, BOOL bLoop) = 0;
	
	virtual void stopSound(CSound *pSound) = 0;
	
	virtual void setSoundEnable(BOOL bEnable) = 0;
	
	virtual void setSoundVolume(DWORD dwVolume) = 0;
	
	virtual int getSoundVolume(CSound *pSound) = 0;
	
	virtual void setSoundVolumeEx(CSound *pSound, DWORD dwVolume) = 0;
};

UTILLIB_INTERFACE ISoundMgr *g_pSoundMgr;

#endif // #ifndef __SOUND_H__