/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGCheckBox.h
*owner        : bob
*description  : Checkbox belong to  control class 
*modified     : 1. created on 2003/8/14 by bob
**************************************************************************
****/  

#if !defined(AFX_UGCHECKBOX_H__61B4A644_AA3E_4E99_BCB1_B34D0E46731F__INCLUDED_)
#define AFX_UGCHECKBOX_H__61B4A644_AA3E_4E99_BCB1_B34D0E46731F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "UGControl.h"   

#define  UG_CTRL_CHECKBOX   0x0000003      

enum CheckBox_StateFlag {CB_NORMAL, CB_LIGH, CB_GRAY};

class CUGCheckBox : public CUGControl    //记住保存复选框的状态
{
public:
    CUGCheckBox(CUGControl *pParent = NULL, int classID = UG_CTRL_CHECKBOX);

    virtual ~CUGCheckBox();

public:
    INT  m_nCheckFlag;

    bool  m_bMouse_Down;

public:
    virtual HRESULT	Create(POINT ptPos, CTRLIMGINFO *pInfo);

    virtual HRESULT Draw(LPDIRECT3DDEVICE pDevice);

    virtual CUGControl* LBtnDown(POINT ptCursor);

    virtual CUGControl* LBtnUp(POINT ptCursor);

    virtual CUGControl*  RBtnDown(POINT ptCursor) ;

    virtual HRESULT Update(POINT ptCursor);			//改动

	virtual HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return S_OK;
	}

	virtual HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return S_OK;
	}

	virtual HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return S_OK;
	}

	virtual HRESULT setTexture(LPDIRECT3DDEVICE8 pDevice, CCtrlRes *psc);

public:
    virtual HRESULT Load(FILE *fp);

    virtual HRESULT Save(FILE *fp);
};

#endif // !defined(AFX_UGCHECKBOX_H__61B4A644_AA3E_4E99_BCB1_B34D0E46731F__INCLUDED_)
