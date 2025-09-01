/****************************************************************************************
*****
*@Copyright Unigium Technology(shang hai) Co,LTD 2002
ALL RIGHTS RESERVED
*file name		: UGEditBox.h
*author			: Jason
*description	: the head file of UGEditBox.h
*modified		: Create on 2003/8/11 by Jason
*modified       : matrol
*****************************************************************************************
****/
#if !defined(AFX_UGEDITBOX_H__0CA287DC_51D6_4445_9977_3A350B67519B__INCLUDED_)
#define AFX_UGEDITBOX_H__0CA287DC_51D6_4445_9977_3A350B67519B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UGControl.h"
#include "UGCtrlID.h"
#include "UtlVector.h"

#define KEYDOWN(vk_code) ((::GetAsyncKeyState(vk_code) & 0x8000) ? 1:0)

struct ugedit_word_t                                
{
	int width, height;

	int char_num;
	
	wchar_t word;
};

class CUGEditBox : public CUGControl
{
public:
	char m_string[16];

	int m_nwidth;								//字符个数

	CUtlVector<ugedit_word_t> m_wordList;		//字符列表

	int m_nCursorIndex;							//当前光标位置

	int m_nSelectStart;							//选择区起始点
	int m_nSelectEnd;							//选择区结束点

	int m_nDrawStart;							//显示区起始点
//	int m_nDrawEnd;								//显示区结束点

	RECT m_rectText;							//字符显示区坐标矩形

	RECT m_rectCursor;							//光标显示坐标矩形

	RECT m_rectSelect;							//选择区坐标矩形

	int m_nMaxChar;								//最大字符数

	HFont m_hFont;							//显示所用字体

	BOOL m_bIsCut;								//用于剪切
	
	BOOL m_bCursorShow;

	double m_fCursorTime;
public:

    CUGEditBox(CUGControl *pParent, int nResID = 0);

    virtual ~CUGEditBox();

public:

    virtual HRESULT	Create(LPCTSTR strFileName, LPRECT pPos, LPRECT pClip, int frame);

	virtual HRESULT setText(LPCTSTR lpstrText);
	
	virtual LPCTSTR getText(LPTSTR lpstrbuf);

	virtual HRESULT Erase(void);
	
	inline HFont setFont(HFont font) { return m_hFont = font; }

	HRESULT setCursorRect(LPRECT rect);
	HRESULT setTextOutRect(LPRECT rect);
		
	int getCharNum(void);
	
protected:
	HRESULT drawSelectRect(void *bmp = NULL, DWORD dwColor = 0x80ffffff);

	virtual HRESULT drawText(void *bmp = NULL, DWORD dwColor = 0xffffffff);

	HRESULT drawCursor(void *bmp = NULL, DWORD dwColor = 0xffffffff);

	HRESULT setSelectRect(void);

	int getCursorIndex(POINT ptCursor);

	virtual HRESULT moveCursor(UINT nChar, UINT nFlags);

	int getDrawEnd(int begin);
	int getDrawStart(int end);

	virtual HRESULT insertChar(UINT nChar);
	virtual HRESULT deleteChar(UINT nChar);

	HRESULT wstringTOstring(WCHAR* wstr,char* str);
		
protected:
	virtual HRESULT OnBackSpace(void);
	virtual HRESULT	OnCut(void);
	virtual HRESULT	OnCopy(void);
	virtual HRESULT	OnPaste(void);
	virtual HRESULT	OnHome(void);
	virtual HRESULT	OnEnd(void);
	virtual HRESULT OnDelete(void);
	virtual HRESULT OnEnter(void);
	virtual HRESULT OnBackup(void);
	virtual HRESULT OnTab(void);
	virtual HRESULT OnEscape(void);

public:
	
    virtual CUGControl*  LBtnDown(POINT ptCursor);
    virtual CUGControl*  LBtnUp(POINT ptCursor) ;
	virtual CUGControl*	 LBtnDblClk(POINT ptCursor);
    virtual CUGControl*  RBtnDown(POINT ptCursor) ;
	virtual CUGControl*	 RBtnUp(POINT ptCursor);
	virtual HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual HRESULT Paint(void *bmp = NULL); 
	virtual HRESULT Update(POINT ptCursor);
	
public:
	
    virtual HRESULT Load(FILE *fp);
    virtual HRESULT Save(FILE *fp);
};

#endif // !defined(AFX_UGEDITBOX_H__0CA287DC_51D6_4445_9977_3A350B67519B__INCLUDED_)



