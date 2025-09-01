#if !defined(AFX_SERVERTAB_H__B442BFBA_2598_4A59_9D47_EA1F7419525D__INCLUDED_)
#define AFX_SERVERTAB_H__B442BFBA_2598_4A59_9D47_EA1F7419525D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServerTab dialog

class CServerTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CServerTab)

// Construction
public:
	CServerTab();
	~CServerTab();

	BOOL OnSetActive();

	void UpdateServerTab();


// Dialog Data
	//{{AFX_DATA(CServerTab)
	enum { IDD = IDD_DIALOG_SERVERTAB };
	CTreeCtrl	m_trServer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CServerTab)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CServerTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonUpdateserver();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERTAB_H__B442BFBA_2598_4A59_9D47_EA1F7419525D__INCLUDED_)
