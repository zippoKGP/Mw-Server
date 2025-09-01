/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGListBox.h
*owner        : bob
*description  : ListBox belong to  control class 
*modified     : 1. created on 2003/8/12 by bob
*************************************************************************
*****/  

#if !defined(AFX_UGLISTBOX_H__6AE29DE2_CB39_436D_8EBD_8B804317E519__INCLUDED_)
#define AFX_UGLISTBOX_H__6AE29DE2_CB39_436D_8EBD_8B804317E519__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  UG_CTRL_LISTBOX 0x00000012
#define  STRING_LONG   260

#include "UGControl.h"

#include "UGSlider.h"
#include "UGSpinBox.h"

class CUGListBox:public CUGControl   
{
public:
    INT  m_nCurrState;		//当前序列号

	INT  m_nStateNum;		//行数(最后一个序列号)
	
	TCHAR m_cDate[STRING_LONG][STRING_LONG];	//字符串

	INT	 m_nWinTop;			//可显区第一个序列号

	INT  m_nWinCurrState;	//鼠标在可显区移动时，其位置所在的行的序列号，或对滚动条时的序列号

	INT  m_nCurrControl;	//操作的子控件

	INT m_nWinStateNum;		//当前的列表区能显示的行数

	CUGSlider* m_pcSlider;

	CUGSpinBox* m_pcSpinBox;
	
	BOOL m_bScroll;			//是否显示滚动条

	/////////////////////////////////////////////////////////////

	RECT   m_rFontOffset;		//字符串位移

	INT m_nFontSize;		//字体大小

	TCHAR m_cFontStyle[64];	//字体风格
	
	BOOL m_bFontFlag;

	/////////////////////////////////////////////////////////////

	LPSIZE m_lpFontSize;	//字符串高宽
	
	HFONT m_hfont;			//字体

	LPD3DXFONT m_pFont;		//字体

	HDC m_hdc;				
	
	HWND     m_hWnd;
	
public:
	CUGListBox(CUGControl *pParent = NULL, int classID = UG_CTRL_LISTBOX);

	virtual ~CUGListBox();

public:

    virtual HRESULT	Create(LPDIRECT3DDEVICE pDevice, POINT ptPos, CTRLIMGINFO *pinfo, CUGControl *pDialog);

	virtual HRESULT GetControl(CUGSlider *pSlider, CUGSpinBox *pSpinBox);	//得到控件指针

	virtual HRESULT SetSelItems(int nIndex, TCHAR* pItems);	//设定范围
	
	virtual TCHAR*	GetSelItems(int nIndex);	//得到范围

	virtual HRESULT SetSelIndex(int nIndex); //得到输入值

	virtual INT		GetSelIndex(void);	//获得当前值

	virtual INT		GetCount(void) ;

	virtual HRESULT	AddItems(TCHAR* pItems);

	virtual HRESULT	DelItems(int nIndex);

	virtual HRESULT SetFontOffset(RECT offset); //输入字符串显示偏移

	virtual HRESULT	GetFontOffset(RECT &offset);	//输出符串显示偏移

	virtual HRESULT SetFontFormat(int nFontSize, TCHAR* pFontStyle); //输入字符串字体风格大小

	virtual HRESULT	GetFontFormat(int &nFontSize, TCHAR* pFontStyle);	//输出符串字体风格大小

	virtual HRESULT setTexture(LPDIRECT3DDEVICE pDevice, CCtrlRes *psc);

public:
	
	virtual HRESULT Draw(LPDIRECT3DDEVICE pDevice);
	
    virtual CUGControl* LBtnDown(POINT ptCursor);	

    virtual CUGControl* LBtnUp(POINT ptCursor);		

    virtual CUGControl*  RBtnDown(POINT ptCursor) ;

    virtual HRESULT Update(POINT ptCursor);
	
	virtual HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	virtual HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return S_OK;
	}

	virtual HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return S_OK;
	}

public:
    virtual HRESULT Load(FILE *fp);	

    virtual HRESULT Save(FILE *fp);	

public:
	virtual void ListState(POINT pt);		//确定鼠标所在行的序列号

	virtual HRESULT Draw_Cursor(LPDIRECT3DDEVICE pDevice);	//画光条

	virtual TCHAR* FontStyle(TCHAR* pStyle);		//判断字符风格是否有效
};

#endif // !defined(AFX_UGLISTBOX_H__6AE29DE2_CB39_436D_8EBD_8B804317E519__INCLUDED_)
