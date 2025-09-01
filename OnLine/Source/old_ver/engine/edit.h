/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2005 ALL RIGHTS RESERVED.

  *file name    : edit.h
  *owner  		: Andy
  *description  : 
  *modified     : 2005/1/29
******************************************************************************/ 

#ifndef WORLD_SERVER_EDIT_H
#define WORLD_SERVER_EDIT_H

#include "control_src/control_src.h"

bool UpdateEditKey(void);

CUGEditBox *getActiveEdit(void);

//////////////////////////////////////////////////////////////////////////
//
class CChatEdit : public CUGEditBox
{
public:
	CChatEdit(void);
	
	virtual ~CChatEdit(void);
	
private:
	virtual HRESULT OnEnter(void);

public:
	HRESULT Init(void);

	HRESULT Shutdown(void);

public:
	bool updateKey(void);

	HRESULT setFocus(void);

	HRESULT LBtnDown(int x, int y);	
	
	HRESULT OnEscape(void);
};

//////////////////////////////////////////////////////////////////////////
//
class CFriendEdit : public CUGEditBox
{
public:
	CFriendEdit(void);

	virtual ~CFriendEdit(void);

public:
	HRESULT Init(void);

	HRESULT Shutdown(void);

	bool updateKey(void);

	HRESULT Paint(int x, int y, void *bmp);

	HRESULT Send(void) { return OnEnter(); }

	HRESULT setFocus(void);

	HRESULT LBtnDown(int x, int y);

private:
	HRESULT OnEnter(void);
	
protected:
	RECT m_Rect;
};


//////////////////////////////////////////////////////////////////////////
//
class CNickNameBox : public CUGEditBox
{
public:
	CNickNameBox(void);

	virtual ~CNickNameBox(void);
	
public:
	HRESULT Init(void);

	HRESULT Shutdown(void);
	
public:
	bool updateKey(void);
};

//////////////////////////////////////////////////////////////////////////
//
class CFindNickNameBox : public CFriendEdit
{
public:
	CFindNickNameBox(void);

	virtual ~CFindNickNameBox(void);

public:
	HRESULT Init(void);

	HRESULT Shutdown(void);
		
	HRESULT OnTab(void);

	HRESULT OnEnter(void);
};

//////////////////////////////////////////////////////////////////////////
//
class CFindIDBox : public CFriendEdit
{
public:
	CFindIDBox(void);

	virtual ~CFindIDBox(void);

public:
	HRESULT Init(void);

	HRESULT Shutdown(void);
	
	HRESULT OnChar(UINT nChar, UINT nRepcnt, UINT nFlags);

	HRESULT OnTab(void);

	HRESULT OnEnter(void);
};

//////////////////////////////////////////////////////////////////////////
//
class CPetNameEdit : public CFriendEdit
{
public:
	CPetNameEdit(void);
	
	virtual ~CPetNameEdit(void);
	
public:
	HRESULT Init(void);
		
private:
	HRESULT OnEnter(void);
};

//////////////////////////////////////////////////////////////////////////
//
class CNumberEdit : public CUGEditBox
{
public:
	CNumberEdit(void);
	
	virtual ~CNumberEdit(void);

public:
	HRESULT Init(RECT& rect, int max_char);
	
	HRESULT Shutdown(void);
	
public:
	HRESULT OnChar(UINT nChar, UINT nRepcnt, UINT nFlags);
	
	HRESULT OnEnter(void);

	HRESULT Paint(int x, int y, void *bmp);
	
	bool updateKey(void);
	
	HRESULT setFocus(void);
	
	HRESULT LBtnDown(int x, int y);

public:
	int getNumber(void);

	void setNumber(int num);

	void setMaxNum(unsigned int max_num);

	bool CheckChar(int nChar);

private:
	RECT m_Rect;

	unsigned int m_nMaxNum;
};

#endif