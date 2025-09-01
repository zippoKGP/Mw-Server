// GameServerView.h : interface of the CGameServerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMESERVERVIEW_H__17ADEEDA_6CB9_4420_AE9B_393744753DC6__INCLUDED_)
#define AFX_GAMESERVERVIEW_H__17ADEEDA_6CB9_4420_AE9B_393744753DC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZoneServerDoc.h"

class CGameServerView : public CFormView
{
protected: // create from serialization only
	CGameServerView();
	DECLARE_DYNCREATE(CGameServerView)
 
public:
	//{{AFX_DATA(CGameServerView)
	enum { IDD = IDD_GAMESERVER_FORM };
	//}}AFX_DATA

// Attributes
public:
	CGameServerDoc* GetDocument();

// Operations
public:
	void ChangeDisplayMode() { OnButtonDisplaymode(); } 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameServerView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGameServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
//	static UINT RuleThread(void* pv);

// Generated message map functions
//protected:
public:
	//{{AFX_MSG(CGameServerView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonDisplaymode();
	afx_msg void OnButtonLogfile();
	afx_msg void OnButtonMonster();
	afx_msg void OnButtonDummy();
	afx_msg void OnButtonOffplayer();
	afx_msg void OnButtonCollline();
	afx_msg void OnButtonDisplayall();
	afx_msg void OnBUTTONPreClose();
	afx_msg void OnBUTTONServerClose();
	afx_msg void OnBUTTONWorldConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GameServerView.cpp
inline CGameServerDoc* CGameServerView::GetDocument()
   { return (CGameServerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMESERVERVIEW_H__17ADEEDA_6CB9_4420_AE9B_393744753DC6__INCLUDED_)
