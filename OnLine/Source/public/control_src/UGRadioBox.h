/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGRadioBox.h
*owner        : bob
*description  : RadioBox belong to  control class 
*modified     : 1. created on 2003/8/7 by bob
*************************************************************************
****/         

#if !defined(AFX_UGRADIOBOX_H__719D5B4E_4F93_47D2_B41E_F259CDA94FD3__INCLUDED_)
#define AFX_UGRADIOBOX_H__719D5B4E_4F93_47D2_B41E_F259CDA94FD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UGControl.h"


#define  UG_CTRL_RADIOBOX   0x0000004    

class CUGRadioBox : public CUGControl
{
public:
    //ccc

    BOOL m_bMouse_Down;

	BOOL m_bCheckFlag;

	int m_nSelect;

public:
	CUGRadioBox(CUGControl *pParent = NULL, int classID = UG_CTRL_RADIOBOX);

	virtual ~CUGRadioBox();

public:
	void SetInitFlag(int nSelect);

	int GetInitFlag(void);

	BOOL GetCompareFlag(int nFlag);

public:

    HRESULT	Create(LPDIRECT3DDEVICE pDevice, POINT ptPos, CTRLIMGINFO *pInfo);

	HRESULT Draw(LPDIRECT3DDEVICE pDevice);

	CUGControl* LBtnDown(POINT ptCursor);

	CUGControl* LBtnUp(POINT ptCursor);

    CUGControl*  RBtnDown(POINT ptCursor) ;

	HRESULT  Update(POINT ptCursor);

	HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return S_OK;
	}

	HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return S_OK;
	}

	HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return S_OK;
	}

	HRESULT setTexture(LPDIRECT3DDEVICE pDevice, CCtrlRes *psc);
	
public:
	HRESULT Load(FILE *fp);

	HRESULT Save(FILE *fp);
};

#endif // !defined(AFX_UGRADIOBOX_H__719D5B4E_4F93_47D2_B41E_F259CDA94FD3__INCLUDED_)
