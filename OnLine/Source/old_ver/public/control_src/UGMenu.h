/************************************************************************* 
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.
*file name    : UGMenu.h
*owner        : matrol
*description  : Menu control and MenuItem control
*modified     : 1. created on 2004/6/23 by matrol
**************************************************************************/
#ifndef UGCONTROL_H
#define UGCONTROL_H

#include "UGControl.h"
#include "UGCtrlID.H"

#define MAX_TEXT_LENGTH				128

class CUGMenu;

//////////////////////////////////////////////////////////////////////////
//Class CUGMenuItem define
//////////////////////////////////////////////////////////////////////////

class CUGMenuItem : public CUGControl
{
protected:
	
	RECT m_rectText;							//菜单项文字显示范围矩形

	RECT m_rtTextFont;							//菜单项文字实际显示范围
	
	char m_szText[MAX_TEXT_LENGTH];				//菜单项文字
	
	DWORD dwTextColor;						//菜单项文字颜色
	
	HFont m_hFont;							//显示所用字体
	
	CUGMenu* m_pCascadingMenu;					//级联菜单
	
	int m_nWidth;								//菜单项宽度
	
	int m_nHeight;								//菜单项高度
	
public:
	
	CUGMenuItem(CUGControl *pParent = NULL,int nResID = 0);

	virtual ~CUGMenuItem();

public:

	virtual void Create(int nWidth,int nHeight, LPCTSTR pszText);
	
	//设置或取得菜单项的级联菜单
	virtual void setCacscadingMenu(CUGMenu* pMenu);
	virtual CUGMenu* getCacscadingMenu();

	virtual void setText(const char* szText);
	
	virtual void setFont(HFont font);

	virtual void setPos(POINT pt);
		
	inline const LPRECT	getRectPos()  { return &m_rectPos; }

	virtual CUGMenuItem* getMenuItem(int nResID);

	inline virtual void setItemWidth(int nWidth) { m_nWidth = nWidth; }
	inline virtual void setItemHeight(int nHeight) { m_nHeight = nHeight; }
	inline virtual int  getItemWidth() { return m_nWidth; }
	inline virtual int  getItemHeight() { return m_nHeight; }
	
    virtual void		setEnable(BOOL bEnable = TRUE);
	inline virtual BOOL getEnable() { return getFlag(UG_STYLE_ENABLE); }
	
		
protected:
	
	//create cscading menu to show
	HRESULT createCscMenu();
	void TextOutput();

public:
	
    virtual CUGControl* LBtnDown(POINT ptCursor);
    virtual CUGControl* LBtnUp(POINT ptCursor);
	virtual CUGControl* RBtnUp(POINT ptCursor);
	virtual HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual HRESULT Update(POINT ptCursor);
	virtual HRESULT Draw(void);
};

//////////////////////////////////////////////////////////////////////////
//Class CUGMenu define
//////////////////////////////////////////////////////////////////////////

class CUGMenu : public CUGControl
{
protected:
	
	CUtlVector<CUGMenuItem*> m_pItemList;				//菜单项列表
	
	CUGMenuItem* m_pActiveItem;						//当前选择菜单项
	
	CUGMenuItem* m_pcascdingItem;					//casading menu existed
	
	HFont m_hFont;							//显示所用字体

	IBaseTexture *pTex;								//背景图片
	
	int m_nMaxWidth;								//此菜单宽度
	int m_nMaxHeight;								//此菜单高度
	
	DWORD m_dwBkgroudColor;						//菜单背景色
	DWORD m_dwBorderColor;						//菜单前景色
	DWORD m_dwForeColor;							//菜单边框色
	
	int m_nRangeX;									//屏幕最右端
	int m_nRangeY;									//屏幕最下端

public:
	
	CUGMenu(CUGControl *pParent = NULL,int nResID = 0);
	
	virtual ~CUGMenu();

public:

	virtual void Create(LPPOINT pPt);

	virtual int addMenuItem(int nWidth,int nHeight,LPCTSTR pszText,int nResID);
	virtual void			deleteMenuItem(int nResID);
	virtual void			deleteAllMenuItem();
	virtual CUGMenuItem*	getMenuItem(int nResID);
	virtual int				getItemCount();

	//return max width/height of items in menu
	virtual int getMaxWidth()  { return m_nMaxWidth; }
	virtual int getMaxHeight() { return m_nMaxHeight; }

	//set text font of all items in menu
	virtual void setFont(HFont hFont);
	virtual HFont getFont() {return m_hFont;}
	
	virtual void setFgColor(DWORD color) { m_dwForeColor = color; }	
	virtual void setBgColor(DWORD color) { m_dwBkgroudColor = color; }

	virtual void setBgImage(LPCTSTR strFileName,LPRECT pClip);

	//set screen size for menu to adjust
	virtual void setMenuRange(int nX,int nY);

protected:
	
	virtual HRESULT initPos(LPPOINT pPt);
	
	virtual void cascadingClose();
	
public:

    virtual CUGControl* LBtnDown(POINT ptCursor);
    virtual CUGControl* LBtnUp(POINT ptCursor);
	virtual CUGControl* RBtnUp(POINT ptCursor);
	virtual HRESULT OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual HRESULT OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual HRESULT Update(POINT ptCursor);
	virtual HRESULT Draw(void);
};

#endif //#ifndef UGMENUITEM_H