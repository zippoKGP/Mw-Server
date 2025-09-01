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
    INT  m_nCurrState;		//��ǰ���к�

	INT  m_nStateNum;		//����(���һ�����к�)
	
	TCHAR m_cDate[STRING_LONG][STRING_LONG];	//�ַ���

	INT	 m_nWinTop;			//��������һ�����к�

	INT  m_nWinCurrState;	//����ڿ������ƶ�ʱ����λ�����ڵ��е����кţ���Թ�����ʱ�����к�

	INT  m_nCurrControl;	//�������ӿؼ�

	INT m_nWinStateNum;		//��ǰ���б�������ʾ������

	CUGSlider* m_pcSlider;

	CUGSpinBox* m_pcSpinBox;
	
	BOOL m_bScroll;			//�Ƿ���ʾ������

	/////////////////////////////////////////////////////////////

	RECT   m_rFontOffset;		//�ַ���λ��

	INT m_nFontSize;		//�����С

	TCHAR m_cFontStyle[64];	//������
	
	BOOL m_bFontFlag;

	/////////////////////////////////////////////////////////////

	LPSIZE m_lpFontSize;	//�ַ����߿�
	
	HFONT m_hfont;			//����

	LPD3DXFONT m_pFont;		//����

	HDC m_hdc;				
	
	HWND     m_hWnd;
	
public:
	CUGListBox(CUGControl *pParent = NULL, int classID = UG_CTRL_LISTBOX);

	virtual ~CUGListBox();

public:

    virtual HRESULT	Create(LPDIRECT3DDEVICE pDevice, POINT ptPos, CTRLIMGINFO *pinfo, CUGControl *pDialog);

	virtual HRESULT GetControl(CUGSlider *pSlider, CUGSpinBox *pSpinBox);	//�õ��ؼ�ָ��

	virtual HRESULT SetSelItems(int nIndex, TCHAR* pItems);	//�趨��Χ
	
	virtual TCHAR*	GetSelItems(int nIndex);	//�õ���Χ

	virtual HRESULT SetSelIndex(int nIndex); //�õ�����ֵ

	virtual INT		GetSelIndex(void);	//��õ�ǰֵ

	virtual INT		GetCount(void) ;

	virtual HRESULT	AddItems(TCHAR* pItems);

	virtual HRESULT	DelItems(int nIndex);

	virtual HRESULT SetFontOffset(RECT offset); //�����ַ�����ʾƫ��

	virtual HRESULT	GetFontOffset(RECT &offset);	//���������ʾƫ��

	virtual HRESULT SetFontFormat(int nFontSize, TCHAR* pFontStyle); //�����ַ����������С

	virtual HRESULT	GetFontFormat(int &nFontSize, TCHAR* pFontStyle);	//��������������С

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
	virtual void ListState(POINT pt);		//ȷ����������е����к�

	virtual HRESULT Draw_Cursor(LPDIRECT3DDEVICE pDevice);	//������

	virtual TCHAR* FontStyle(TCHAR* pStyle);		//�ж��ַ�����Ƿ���Ч
};

#endif // !defined(AFX_UGLISTBOX_H__6AE29DE2_CB39_436D_8EBD_8B804317E519__INCLUDED_)
