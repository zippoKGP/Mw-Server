#if !defined(AFX_INFOSHEET_H__D565EBB6_B90B_4AFE_974F_57C85B044E01__INCLUDED_)
#define AFX_INFOSHEET_H__D565EBB6_B90B_4AFE_974F_57C85B044E01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoSheet.h : header file
//

#include "ServerTab.h"
#include "MapTab.h"
#include "TCPTab.h"
#include "ObjectTab.h"
#include "IPXTab.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoSheet

class CInfoSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CInfoSheet)

// Construction
public:
	enum {	TCP_TAB = 0,
			IPX_TAB = 1, 
			SERVER_TAB = 2,
			OBJECT_TAB = 3, 
			MAP_TAB = 4};

	CInfoSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CInfoSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CInfoSheet();
	void Construct(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CTCPTab m_tabTCP;
	CIPXTab m_tabIPX;
	CServerTab m_tabServer;
	CObjectTab m_tabObject;
	CMapTab m_tabMap;

// Operations
public:
	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInfoSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInfoSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFOSHEET_H__D565EBB6_B90B_4AFE_974F_57C85B044E01__INCLUDED_)
