#if !defined(AFX_IPXTAB_H__3777CDCF_B098_4D1E_879C_8850E98FAD8E__INCLUDED_)
#define AFX_IPXTAB_H__3777CDCF_B098_4D1E_879C_8850E98FAD8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ipxtab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIPXTab dialog

class CIPXTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CIPXTab)

// Construction
public:
	CIPXTab();
	~CIPXTab();

	bool m_bUpdate;

	void UpdateTab();

// Dialog Data
	//{{AFX_DATA(CIPXTab)
	enum { IDD = IDD_DIALOG_IPXTAB };
	UINT	m_edBufList;
	UINT	m_edConnSocketNum;
	UINT	m_edMaxSocketNum;
	UINT	m_edOddMsg;
	UINT	m_edRecvErr;
	UINT	m_edSendBlock;
	UINT	m_edSendErr;
	UINT	m_edSendSocketNum;
	double	m_edRecvByte;
	double	m_edSendByte;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIPXTab)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIPXTab)
	afx_msg void OnButtonUpdatelistsize();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPXTAB_H__3777CDCF_B098_4D1E_879C_8850E98FAD8E__INCLUDED_)
