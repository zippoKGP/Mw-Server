/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGSpinBox.h
*owner        : bob
*description  : SpinBox belong to  control class 
*modified     : 1. created on 2003/8/8 by bob
*************************************************************************
*****/         

#if !defined(AFX_UGSPINBOX_H__02683DFF_475A_446B_B533_3AFA08626AF0__INCLUDED_)
#define AFX_UGSPINBOX_H__02683DFF_475A_446B_B533_3AFA08626AF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UGControl.h"
#include "UGButton.h"


#define UG_CTRL_SPINBOX 0x00000011
#define DEF_SPINBOX_RANGE_MAX  10
#define DEF_SPINBOX_RANGE_MIN  0

class CUGSpinBox:public CUGControl  
{
public:
    INT  m_nCurrState;  //��ǰ״̬��ʾ

	INT  m_nStateNum;	//״̬��

	INT  m_nMax;	

	INT	 m_nMin;

	BOOL m_bActive;

	CUGButton	*m_pcBtn1;

	CUGButton	*m_pcBtn2;

public:
	CUGSpinBox(CUGControl *pParent = NULL, int classID = UG_CTRL_SPINBOX);
	
	virtual ~CUGSpinBox();

public:
    virtual HRESULT	Create(POINT ptPos, CTRLIMGINFO *pInfo, CUGControl *pDialog);

	virtual HRESULT GetButton(CUGButton	*button1, CUGButton *button2);

	virtual HRESULT SetRange(int nMin, int nMax );	//�趨��Χ
	
	virtual HRESULT	GetRange(int& nMin, int& nMax  );	//�õ���Χ

	virtual HRESULT SetPos(int nPos); //�õ�����ֵ

	virtual INT		GetPos(void);	//��õ�ǰֵ

	virtual BOOL	GetActive(void);

	virtual HRESULT setTexture(LPDIRECT3DDEVICE pDevice, CCtrlRes *psc);
	
public:
	virtual HRESULT Draw(LPDIRECT3DDEVICE pDevice);
	
    virtual CUGControl* LBtnDown(POINT ptCursor);	

    virtual CUGControl* LBtnUp(POINT ptCursor);		

    virtual CUGControl*  RBtnDown(POINT ptCursor) ;

    virtual HRESULT Update(POINT ptCursor);	

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

public:
    virtual HRESULT Load(FILE *fp);	

    virtual HRESULT Save(FILE *fp);	

public:
	virtual void SpinBoxState(void);		//���������������Ĳ���
};

#endif // !defined(AFX_UGSPINBOX_H__02683DFF_475A_446B_B533_3AFA08626AF0__INCLUDED_)
