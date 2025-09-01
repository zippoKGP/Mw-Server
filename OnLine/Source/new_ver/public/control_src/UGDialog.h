/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGDialog.h
*owner        : bob
*description  : Dialog belong to  control class 
*modified     : 1. created on 2003/8/12 by bob
*************************************************************************
*****/  

#if !defined(AFX_UGDIALOG_H__4CCB8B42_C706_4EC3_81AD_326732CF28B4__INCLUDED_)
#define AFX_UGDIALOG_H__4CCB8B42_C706_4EC3_81AD_326732CF28B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UGControl.h"
#include "UGCtrlID.H"

class CUGDialog : public CUGControl
{
public:

    CUGControl *m_pActiveCtrl;						//当前控件,暂未用

	CUtlVector<CUGControl*> m_pCtrlList;				//对话框中控件列表

public:

    CUGDialog(CUGControl *pParent = NULL, int classID = UG_CTRL_DIALOG);

    virtual ~CUGDialog();

public:

    virtual HRESULT	Create(LPCTSTR strFileName, POINT ptPos, RECT *rectClip);
    virtual HRESULT Create(RECT rtPos);

    virtual HRESULT InsertControl(CUGControl *pCtrl);
    virtual HRESULT DeleteControl(CUGControl *pCtrl);

	virtual CUGControl* GetCtrl(int idx);
	virtual CUGControl* getControl(int res);

	virtual int GetCtrlCount(void);

public:

    virtual CUGControl* LBtnDown(POINT ptCursor);
    virtual CUGControl* LBtnUp(POINT ptCursor);
	virtual CUGControl* LBtnDblClk(POINT ptCursor);
    virtual CUGControl* RBtnDown(POINT ptCursor);
	virtual CUGControl* RBtnUp(POINT ptCursor);
	virtual HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual HRESULT Update(POINT ptCursor);
	virtual HRESULT Paint(void* bmp);

public:

    virtual HRESULT Load(FILE *fp);
    virtual HRESULT Save(FILE *fp);
};

#endif // !defined(AFX_UGDIALOG_H__4CCB8B42_C706_4EC3_81AD_326732CF28B4__INCLUDED_)
