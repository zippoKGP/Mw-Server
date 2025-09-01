#if !defined(AFX_MAPTAB_H__3ADFFDC2_0973_4BCC_835B_9C5E463F53D6__INCLUDED_)
#define AFX_MAPTAB_H__3ADFFDC2_0973_4BCC_835B_9C5E463F53D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapTab dialog

class CMapTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CMapTab)

// Construction
public:
	CMapTab();
	~CMapTab();

	HTREEITEM m_hMap[32];
	
	void UpdateTab();

// Dialog Data
	//{{AFX_DATA(CMapTab)
	enum { IDD = IDD_DIALOG_MAPTAB };
	CTreeCtrl	m_trMap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMapTab)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMapTab)
	afx_msg void OnButtonMapchange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPTAB_H__3ADFFDC2_0973_4BCC_835B_9C5E463F53D6__INCLUDED_)
