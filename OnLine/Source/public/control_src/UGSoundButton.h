/************************************************************************* 
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : ugsoundbutton.h
*owner        : bob
*description  : interface for the CUGSoundButton class.
*modified     : 1. created on 2004/5/1 by bob
*modified     : 2. matrol
**************************************************************************/

#if !defined(__ug_sound_button_h_)
#define __ug_sound_button_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_NAME_LENGTH 256

#include "UGButton.h"
#include "UGCtrlID.h"

#include "utillib/sound.h"

class CUGSoundButton : public  CUGButton  
{
private:
	
	CSound  *m_pSndClkPly;			//Click to play!
	CSound  *m_pSndCurOnPly;		//Mouse On to play!
	CSound  *m_pSndDisClkPly;		//disable click
	
private:

	char* FindSoundFile(char* filepath,char* filename);
	
private:
	
	void PlaySoundCurOn();
	void PlaySoundLBtnClk();
	void PlaySoundDisLBtnClk();
	
public:
	
	CUGSoundButton(CUGControl *pParent, int res_id,	char *filepath, 
		char *HighLightFileName,char *ClickedFileName, char *DisclickFileName);
	
	virtual ~CUGSoundButton();
	
public:
	
	HRESULT Destory();
	
	HRESULT Update(POINT ptCursor);

	CUGControl*  LBtnDown(POINT ptCursor);	
};

#endif // !defined(__ug_sound_button_h_)
