#if !defined(AFX_TCPTAB_H__D513A03F_14E7_46D6_B1EA_D70999333B7C__INCLUDED_)
#define AFX_TCPTAB_H__D513A03F_14E7_46D6_B1EA_D70999333B7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TCPTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTCPTab dialog

class CTCPTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CTCPTab)

// Construction
public:
	CTCPTab();
	~CTCPTab();

	bool m_bUpdate;

	void UpdateTab();

// Dialog Data
	//{{AFX_DATA(CTCPTab)
	enum { IDD = IDD_DIALOG_TCPTAB };
	UINT	m_edConnSocketNum;
	UINT	m_edOddMsg;
	UINT	m_edRecvErr;
	UINT	m_edSendBlock;
	UINT	m_edSendErr;
	UINT	m_edRecvBlk;
	double	m_edRecvByte;
	double	m_edSendByte;
	int		m_edConnectUserNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTCPTab)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTCPTab)
	afx_msg void OnButtonUpdatelistsize();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCPTAB_H__D513A03F_14E7_46D6_B1EA_D70999333B7C__INCLUDED_)
