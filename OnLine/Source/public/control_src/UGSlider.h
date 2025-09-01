/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGSlider.h
*owner        : bob
*description  : slider belong to  control class 
*modified     : 1. created on 2003/8/7 by bob
*************************************************************************
*****/         

#if !defined(AFX_UGSLIDER_H__03BC11DE_E212_4D49_AFB9_5EB586DF2B98__INCLUDED_)
#define AFX_UGSLIDER_H__03BC11DE_E212_4D49_AFB9_5EB586DF2B98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UGControl.h"
#include "UGButton.h"
#include "UGCtrlID.h"

#define DEF_SLIDER_RANGE_MAX  10
#define DEF_SLIDER_RANGE_MIN  0

typedef struct {
	RECT  rtUp;
	RECT  rtDown;
	RECT  rtBkground;
	RECT  rtSplider;
	BOOL  bVertical;
}SPLIDER_INFO,*LPSPLIDER_INFO;

class CUGSlider:public CUGControl  
{
protected:

	INT  m_nMax;							//����λ�÷�Χ���ֵ	
	INT	 m_nMin;							//����λ�÷�Χ��Сֵ

	INT  m_nPosNum;							//״̬��

	INT  m_nPtNum;							//�������ƶ�����������

    INT  m_nCurrPos;						//��ǰPos

	RECT m_rtTrack;							//ʹ�������϶������������

	CUtlVector<CUGControl*> m_pCtrlList;		//���������ÿؼ��б�

public:

	CUGSlider(CUGControl *pParent = NULL , int nResID = 0);

	virtual ~CUGSlider();

public:

    virtual HRESULT	Create(LPCTSTR szFileName, LPRECT pPos,LPSPLIDER_INFO pInfo);

	virtual HRESULT setRange(int nMin, int nMax );	//�趨��Χ
	virtual HRESULT	getRange(int& nMin, int& nMax );	//�õ���Χ

	virtual HRESULT setSliderPos(int nPos); //�趨��ǰֵ
	virtual INT		getSliderPos(void); //��õ�ǰֵ

    virtual HRESULT InsertControl(CUGControl *pCtrl);

	virtual void setVisual(BOOL bVisual = TRUE);
	virtual BOOL getVisual(void);
    virtual void setEnable(BOOL bEnable = TRUE);
	virtual BOOL getEnable(void);

protected:
	
	virtual void sliderDrag(POINT pt);		//�϶�����
	
	void adjustSlider();
	
public:


    virtual CUGControl* LBtnDown(POINT ptCursor);
    virtual CUGControl* LBtnUp(POINT ptCursor);
    virtual CUGControl*  RBtnDown(POINT ptCursor) ;
	virtual HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return E_FAIL;
	}
	virtual HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return E_FAIL;
	}
	virtual HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		return E_FAIL;
	}
    virtual HRESULT Update(POINT ptCursor);
	virtual HRESULT Draw(void *bmp);

public:
	
    virtual HRESULT Load(FILE *fp);
    virtual HRESULT Save(FILE *fp);
};

#endif // !defined(AFX_UGSLIDER_H__03BC11DE_E212_4D49_AFB9_5EB586DF2B98__INCLUDED_)
