// GameServerDoc.h : interface of the CGameServerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMESERVERDOC_H__D1696CB6_6835_4F2B_8584_28718EFD32CC__INCLUDED_)
#define AFX_GAMESERVERDOC_H__D1696CB6_6835_4F2B_8584_28718EFD32CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DisplayView.h"
#include "InfoSheet.h"
#include "DisplayView.h"
#include "MainThread.h"

class CGameServerDoc : public CDocument
{
protected: // create from serialization only
	CGameServerDoc();
	DECLARE_DYNCREATE(CGameServerDoc)

// Attributes
public:
	CDisplayView	m_DisplayView;
	CInfoSheet		m_InfoSheet;

	CStatusBar*		m_pStatusBar;

	CWnd* m_pwndMainFrame;
	CWnd* m_pwndMainView;
	
// Operations
public:
	CGameServerDoc* GetDocument();

	void CreateDisplayView(CWnd* pWnd);
	void CreateSheetView(CWnd* pWnd);

private:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameServerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGameServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGameServerDoc)

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CGameServerDoc* g_pDoc;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMESERVERDOC_H__D1696CB6_6835_4F2B_8584_28718EFD32CC__INCLUDED_)
