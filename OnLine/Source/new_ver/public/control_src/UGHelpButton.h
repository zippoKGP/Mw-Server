/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGHelpButton.h
*owner        : 
*description  : Button belong to  control class 
*modified     : 1. created by Robert (CInterpretationButton.h)
*modified     : 2. modified by Matrol
*************************************************************************
****/  
#if !defined(UGHELPBUTTON_H)
#define UGHELPBUTTON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ugctrlid.h"
#include "UGButton.h"

#define MAX_WORDS_LENGTH 50

#define INTERVAL 3  //������߿���������
#define MOUSELEN 32  //�����y�����ϵĳ���

#define SCREEN_WIDTH       800
#define SCREEN_HEIGHT      600

#define TIME_DELAY_MIN    1000
#define TIME_DELAY_MAX    4200

class CUGHelpButton : public CUGButton  
{
private:
	
	char       m_szHelp[MAX_WORDS_LENGTH];  //���������ֵ�����
	
	float      m_fTimeDelay;			    //�����ڰ�ť�Ϻ���������ʱ��
	
	float      m_fTimeBeginning;		    //������¼ÿһ�ε���DrawHelp����ʼʱ��
	
	POINT      m_ptRect;				    //��¼����ʵ������
	
	RECT       m_rtRect;				    //����������ʾ��������
	
	RECT       m_rtLine[4];				    //�������ֿ�
	
	RECT       m_rtHelpText;				//��������
	
	HFont     m_hHelpFont;					//������������
	
	SIZE       m_size;						//��������
	
public:
	
	CUGHelpButton(CUGControl *pParent, int res_id);
	
	virtual ~CUGHelpButton();
	
public:
	
    virtual HRESULT Update(POINT ptCursor);
	
	virtual HRESULT Draw();		
	
	virtual CUGControl*  LBtnDown(POINT ptCursor);

public:

    virtual HRESULT	Create(LPCTSTR strFileName, LPRECT pPos, LPRECT pClip);
	
	HRESULT      InitFont(void);

	HRESULT      SetHelpWords(char *szHelp = NULL);
	
	void         setDraw(BOOL bDraw = TRUE){setFlag(HELPBTN_STYLE_DRAWHELP,bDraw);};

	BOOL         getDraw(){return getFlag(HELPBTN_STYLE_DRAWHELP);   };
	
	HRESULT      DrawHelp();
	
	int  CalTime(POINT ptCursor);

	void CalRect(POINT ptCursor);

	void ResetTime();
};

#endif // !defined(UGHELPBUTTON_H)
















