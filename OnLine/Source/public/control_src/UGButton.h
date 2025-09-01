/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGButton.h
*owner        : bob
*description  : Button belong to  control class 
*modified     : 1. created on 2003/8/14 by bob
*************************************************************************
****/  

#if !defined(AFX_UGBUTTON_H__48A6BFE4_B098_4B81_BBC4_FB8F65E35936__INCLUDED_)
#define AFX_UGBUTTON_H__48A6BFE4_B098_4B81_BBC4_FB8F65E35936__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UGControl.h"

class CUGButton : public CUGControl
{
public:
    CUGButton(CUGControl *pParent = NULL, int nResID = 0);

    virtual	~CUGButton();
	
public:
	HRESULT	Init(LPCTSTR strFileName, LPRECT pPos, LPRECT pClip);

	HRESULT Init(LPCTSTR med_name, int frame);

public:	
    virtual CUGControl* LBtnDown(POINT ptCursor);
    
	virtual CUGControl* LBtnUp(POINT ptCursor);

    virtual HRESULT Update(POINT ptCursor);
};

#endif // !defined(AFX_UGBUTTON_H__48A6BFE4_B098_4B81_BBC4_FB8F65E35936__INCLUDED_)
