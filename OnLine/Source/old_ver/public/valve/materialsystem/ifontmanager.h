/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

*file name    : ifontmanager.h
*owner        : Andy
*description  : 
*modified     : 2004/09/14
******************************************************************************/ 

#ifndef IFONTMANAGER_H
#define IFONTMANAGER_H

#include <vgui/VGUI.h>

using vgui::HFont;

#define FONT_MANAGER_INTERFACE_VERSION "font_manager_001"

class IFontManager
{
public:
	virtual HRESULT Init(void) = 0;

	virtual HRESULT Shutdown(void) = 0;

	virtual HFont GetFont(const char *name, const char *font_name, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange) = 0;
	
	virtual HFont GetFontByName(const char *name) = 0;
	
	virtual void SetFontName(HFont font, const char *name) = 0;
	
	virtual void GetCharABCwide(HFont font, int ch, int &a, int &b, int &c) = 0;
	
	virtual int GetFontTall(HFont font) = 0;
	
	virtual int GetCharacterWidth(HFont font, int ch) = 0;
	
	virtual void GetTextSize(HFont font, const char *text, int &wide, int &tall) = 0;

	virtual void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall) = 0;
	
	virtual int DrawString(HFont font, const char *text, RECT* rect, DWORD color, DWORD flag) = 0;

	virtual int DrawString(HFont font, const wchar_t *text, RECT* rect, DWORD color, DWORD flag) = 0;
};

#endif