/************************************************************************* 
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : ugshbutton.h
*owner        : Matrol
*description  : interface for the CUGHSButton class.
*modified     : 1. created on 2004/5/26 by Matrol
**************************************************************************/
#if !defined (UG_SOUND_HELP_BUTTON_H)
#define UG_SOUND_HELP_BUTTON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_NAME_LENGTH 256

#include "UGHelpButton.h"
#include "UGCtrlID.h"
#include "utillib/sound.h"

class CUGHSButton : public CUGHelpButton  
{
private:
	
	CSound  *m_pSndClkPly;		//Click to play!
	
	CSound  *m_pSndCurOnPly;	// Mouse On to play!
	
	CSound  *m_pSndDisClkPly;	//disable click
	
private:
	
	LPCSTR FindSoundFile(char* filepath,char* filename);
	
private:
	
	void PlaySoundCurOn();
	
	void PlaySoundLBtnClk();
	
	void PlaySoundDisLBtnClk();
	
public:
	
	CUGHSButton(CUGControl *pParent, int res_id);
	
	virtual ~CUGHSButton();
	
public:
	
	HRESULT Destory();
	
    virtual HRESULT Update(POINT ptCursor);
	
	virtual CUGControl*  LBtnDown(POINT ptCursor);	

	void setSoundFile(char *filepath,char *HighLightFileName,char *ClickedFileName,char *DisclickFileName);
};

#endif //#if !defined (UG_SOUND_HELP_BUTTON_H)

