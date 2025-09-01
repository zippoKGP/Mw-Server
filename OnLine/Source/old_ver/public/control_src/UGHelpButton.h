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

#define INTERVAL 3  //文字与边框间隔的像素
#define MOUSELEN 32  //鼠标在y方向上的长度

#define SCREEN_WIDTH       800
#define SCREEN_HEIGHT      600

#define TIME_DELAY_MIN    1000
#define TIME_DELAY_MAX    4200

class CUGHelpButton : public CUGButton  
{
private:
	
	char       m_szHelp[MAX_WORDS_LENGTH];  //解释性文字的内容
	
	float      m_fTimeDelay;			    //鼠标放在按钮上后所经过的时间
	
	float      m_fTimeBeginning;		    //用来记录每一次调用DrawHelp的起始时间
	
	POINT      m_ptRect;				    //记录光标的实际坐标
	
	RECT       m_rtRect;				    //帮助文字显示矩形坐标
	
	RECT       m_rtLine[4];				    //帮助文字框
	
	RECT       m_rtHelpText;				//帮助文字
	
	HFont     m_hHelpFont;					//帮助文字字体
	
	SIZE       m_size;						//帮助长度
	
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
















