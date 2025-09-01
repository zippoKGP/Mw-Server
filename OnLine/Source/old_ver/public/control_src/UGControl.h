/************************************************************************* 
*****
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGControl.h
*owner        : Andy
*description  : Control belong to  control class 
*modified     : 1. created on 2003/8/12 by Andy
*************************************************************************
*****/  

#if !defined(AFX_UGCONTROL_H__54A6C3FA_FA0D_4FB3_8F65_4C4B3F4F07BC__INCLUDED_)
#define AFX_UGCONTROL_H__54A6C3FA_FA0D_4FB3_8F65_4C4B3F4F07BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined UG_MAX_NAME
#define UG_MAX_NAME 128
#endif

#include "UtlVector.h"

class CUGImage;

class CUGControl                    //这是每一个控件的都应该具有的东西
{
protected:
    int m_nClassID;						//类别ID

	int m_nResID;						//资源ID

	RECT  m_rectPos;					//屏幕位置坐标

	DWORD m_dwFlag;						//标志位

    char m_szName[UG_MAX_NAME];		//名字
	
	CUtlVector<CUGImage*> m_pUGImage;		//用到的图片
	
    CUGControl *m_pParent;				//父控件指针

public:

    CUGControl(CUGControl *pParent, int nClassID, int nResID);

    virtual	~CUGControl();

public:
    virtual	CUGControl*  LBtnDown(POINT ptCursor);
    virtual	CUGControl*  LBtnUp(POINT ptCursor);
	virtual	CUGControl*  LBtnDblClk(POINT ptCursor);
    virtual	CUGControl*  RBtnDown(POINT ptCursor);
    virtual	CUGControl*  RBtnUp(POINT ptCursor);
	virtual HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual HRESULT Update(POINT ptCursor);

    virtual HRESULT Paint(void *bmp = NULL);

public:

    virtual	 HRESULT Load(FILE *fp);
    virtual	 HRESULT Save(FILE *fp);

public:
	
	inline int getClassID(void) const { return m_nClassID; }
	inline int getResourceID(void) const { return m_nResID; }
	inline void setResourceID(int nResID){m_nResID = nResID;}

	inline CUGControl* getParent() const { return m_pParent; }

    inline LPCTSTR getName(void) { return m_szName; }
	inline LPCTSTR setName(LPCTSTR strName) { return strcpy(m_szName, strName); }

	CUGImage *insertImage(CUGImage *pImage);
	CUGImage *getImage(int nIndex);

	BOOL getFlag(LONG lStyle);
	void setFlag(LONG lStyle,BOOL bFlag = TRUE);
	BOOL swtichFlag(LONG lStyle);
	
	virtual void setVisual(BOOL bVisual = TRUE);
	virtual BOOL getVisual(void);
    virtual void setEnable(BOOL bEnable = TRUE);
	virtual BOOL getEnable(void);
	virtual void setFocus(BOOL bFocus = TRUE);
	virtual BOOL getFocus();
	
	virtual CUGImage* ptInImageRect(POINT ptCursor);

	virtual void setPos(POINT pt);
	
	virtual void setPosRect(LPRECT rect) { Q_memcpy(&m_rectPos, rect, sizeof(*rect)); }

	virtual CUGImage* insertImage(LPCTSTR file_name, RECT rect_pos, RECT rect_clip);

protected:
	
	virtual HRESULT getRelativePosition(LPPOINT pPoint);
	
	virtual HRESULT drawBackGround(DWORD color);
};

#endif // !defined(AFX_UGCONTROL_H__54A6C3FA_FA0D_4FB3_8F65_4C4B3F4F07BC__INCLUDED_)
