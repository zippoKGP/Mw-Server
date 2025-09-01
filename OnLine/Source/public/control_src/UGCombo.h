/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGCombox.h
*owner        : bob
*description  : Combo belong to  control class 
*modified     : 1. created on 2003/8/14 by bob
*************************************************************************
*****/  

#if !defined(AFX_UGCOMBO_H__64E26122_F9DF_41CD_8614_FC3D99B91F60__INCLUDED_)
#define AFX_UGCOMBO_H__64E26122_F9DF_41CD_8614_FC3D99B91F60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UG_CTRL_COMBO	0x00000013
#define STRING_LONG  260  

#include "UGControl.h"
#include "UGListBox.h"
#include "UGButton.h"
#include "UGEditBox.h"

class CUGCombo : public CUGControl  
{
public:
	
    INT  m_nCurrState;		//当前序列号

	TCHAR m_cDate[STRING_LONG];	//字符串

	BOOL m_bListView;			//是否显示列表框

	BOOL m_bEditEnable;			//编辑框是否可写，可操作

	CUGEditBox* m_pcEdit;	

	CUGListBox* m_pcListBox;
	
	CUGButton* m_pcButton;

public:
	CUGCombo(CUGControl *pParent = NULL, int classID = UG_CTRL_COMBO);
	
	virtual ~CUGCombo();

public:
    virtual HRESULT	Create(POINT ptPos, CTRLIMGINFO *pinfo, CUGControl *pDialog);
	
	virtual HRESULT GetControl(CUGEditBox *pEdit, CUGListBox *pListBox , CUGButton *pButton);	//得到控件指针

	virtual HRESULT SetSelItems(int nIndex, TCHAR* pItems);	//设定范围
	
	virtual TCHAR*	GetSelItems(int nIndex);	//得到范围

	virtual HRESULT SetSelIndex(int nIndex); //得到输入值

	virtual INT		GetSelIndex(void);	//获得当前值

	virtual int		GetCount(void) ;

	virtual HRESULT	AddItems(TCHAR* pItems);

	virtual HRESULT	DelItems(int nIndex);

	virtual HRESULT	SetEditEnable(BOOL flag);	//设置编辑框是否可写，可操作

	virtual BOOL	GetEditEnable(void);

	virtual HRESULT SetListFontOffset(RECT offset); //输入字符串显示偏移

	virtual HRESULT	GetListFontOffset(RECT &offset);	//输出符串显示偏移

	virtual HRESULT SetListFontFormat(int nFontSize, TCHAR* pFontStyle); //输入字符串字体风格大小

	virtual HRESULT	GetListFontFormat(int &nFontSize, TCHAR* pFontStyle);	//输出符串字体风格大小

	virtual HRESULT setTexture(LPDIRECT3DDEVICE pDevice, CCtrlRes *psc);

public:
	
	virtual HRESULT Draw(LPDIRECT3DDEVICE pDevice);
	
    virtual CUGControl* LBtnDown(POINT ptCursor);	

    virtual CUGControl* LBtnUp(POINT ptCursor);		

    virtual CUGControl*  RBtnDown(POINT ptCursor) ;

	virtual CUGControl*	 LBtnDblClk(POINT ptCursor);

    virtual HRESULT Update(POINT ptCursor);
	
	virtual HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	virtual HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return S_OK;
	}

public:
    virtual HRESULT Load(FILE *fp);	

    virtual HRESULT Save(FILE *fp);
};

#endif // !defined(AFX_UGCOMBO_H__64E26122_F9DF_41CD_8614_FC3D99B91F60__INCLUDED_)
