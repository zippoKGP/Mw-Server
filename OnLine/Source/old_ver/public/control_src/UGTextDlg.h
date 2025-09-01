// UGTextDlg.h: interface for the CUGTextDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UGTEXTDLG_H__3EDA0BEA_6F74_4C40_8C00_0CD798AAAB41__INCLUDED_)
#define AFX_UGTEXTDLG_H__3EDA0BEA_6F74_4C40_8C00_0CD798AAAB41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ugdialog.h"
#include <vector>
using namespace std;

class CUGTextDlg : public CUGDialog
{
private:
	struct text_line
	{
		string text;
		
		RECT bound;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	vector<text_line> m_pTextList;

	HFont m_hFont;
		
	DWORD m_dwFontColor;

	DWORD m_dwBGColor;

	BOOL m_bLightBar;

	RECT m_rectLightBar;

	int m_tallHightBar;

	DWORD m_dwColorLightBar;

	POINT m_ptOffsetLightBar;

	int m_dwBarIndex;

	//////////////////////////////////////////////////////////////////////////
	//
	typedef CUGDialog BaseClass;

public:
	CUGTextDlg();

	virtual ~CUGTextDlg();

public:
	HRESULT Init(void);

	void Shutdown(void);

	//////////////////////////////////////////////////////////////////////////
	//
	void setColor(DWORD color) { m_dwBGColor = color; }

	void setFont(HFont font) { m_hFont = font; }

	void insertText(LPCTSTR text, POINT pt);

	void setLightBarEnable(BOOL enable) { m_bLightBar = enable; }

	void setLightBarRect(RECT rect) { m_rectLightBar = rect; }

	void setLightBarOffset(int x, int y) { m_ptOffsetLightBar.x = x; m_ptOffsetLightBar.y = y; }

	void setLightBarHeight(int tall) { m_tallHightBar = tall; }

	void setFontColor(DWORD color) { m_dwFontColor = color; }

	int getLineNum(void) { return m_pTextList.size(); }

	int getBarIndex(void) { return m_dwBarIndex; }

	void clearLine(void) { m_pTextList.clear(); }

public:
	virtual HRESULT Update(POINT ptCursor);

	virtual HRESULT Paint(void);

	virtual CUGControl* LBtnDown(POINT ptCursor);

private:
	HRESULT drawTextList(void);

	HRESULT drawTextLine(text_line& line);

	HRESULT drawLightBar(void);

	HRESULT updateLightBar(POINT ptCursor);
};

#endif // !defined(AFX_UGTEXTDLG_H__3EDA0BEA_6F74_4C40_8C00_0CD798AAAB41__INCLUDED_)
