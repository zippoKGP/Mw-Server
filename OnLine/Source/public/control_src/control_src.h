#if !defined(CONTROL_SRC_H__INCLUDED_)
#define CONTROL_SRC_H__INCLUDED_


#include <objbase.h>
#include <basetsd.h>
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <string.h>
#include <process.h>
#include <direct.h>
#include <stdlib.h>
#include <ctype.h>
#include <io.h>
#include <time.h>
#include <fcntl.h>
#include <malloc.h>
#include <dos.h>
#include <stdio.h>
#include <stdarg.h>
#include <lmcons.h>
#include <mmsystem.h>

//#define _3D_RENDER //


//////////////////////////////////////////////////////////////////////////
//
#ifdef _3D_RENDER

class IMatSystemSurface;
class IMaterialSystem;
class ITextureMgr;

#define control_mtl_name "control_mtl"

namespace ugcontrol
{
	HRESULT Init(IMaterialSystem* pMatSystem, IMatSystemSurface* pSurface, ITextureMgr *pTexMgr);
	
	void Shutdown(void);
};

#else

typedef unsigned long HFont;

#endif

#include "UGCtrlID.H"
#include "UGControl.h"
#include "UGButton.h"
#include "UGDialog.h"
#include "UGEditBox.h"
/*
#include "UGSoundButton.h"
#include "UGHelpButton.h"
#include "UGSlider.h"
#include "UGHSButton.h"
#include "UGMenu.h"
#include "ugtextdlg.h"
*/
//#include "UGSpinBox.h"
//#include "UGListBox.h"
//#include "UGCombo.h"
//#include "UGCheckBox.h" 
//#include "UGRadioBox.h"

#endif //#if !defined(CONTROL_SRC_H__INCLUDED_)
