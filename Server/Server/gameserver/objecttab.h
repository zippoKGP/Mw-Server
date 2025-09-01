#if !defined(AFX_OBJECTTAB_H__B3831C76_3F91_4197_B279_B2D80188E58B__INCLUDED_)
#define AFX_OBJECTTAB_H__B3831C76_3F91_4197_B279_B2D80188E58B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectTab dialog

class CObjectTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CObjectTab)

// Construction
public:
	CObjectTab();
	~CObjectTab();

	void UpdateTab();


// Dialog Data
	//{{AFX_DATA(CObjectTab)
	enum { IDD = IDD_DIALOG_OBJECTTAB };
	CButton	m_btAuto;
	CTreeCtrl	m_trObject;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CObjectTab)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CObjectTab)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonUpdate();
	afx_msg void OnButtonAuto();
	afx_msg void OnButtonSearch();
	afx_msg void OnButtonDestory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTTAB_H__B3831C76_3F91_4197_B279_B2D80188E58B__INCLUDED_)
