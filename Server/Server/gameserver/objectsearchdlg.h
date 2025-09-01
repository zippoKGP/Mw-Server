#if !defined(AFX_OBJECTSEARCHDLG_H__816FC11A_31CA_4641_B7CE_5D7DC27AB90E__INCLUDED_)
#define AFX_OBJECTSEARCHDLG_H__816FC11A_31CA_4641_B7CE_5D7DC27AB90E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectSearchDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectSearchDlg dialog

#include "GameObject.h"

class CObjectSearchDlg : public CDialog
{
// Construction
public:
	CObjectSearchDlg(CWnd* pParent = NULL);   // standard constructor

	_object_id m_ID;
	char m_szCharName[17];

// Dialog Data
	//{{AFX_DATA(CObjectSearchDlg)
	enum { IDD = IDD_DIALOG_SEARCH };
	CComboBox	m_cdObjectKind;
	CComboBox	m_cdObjectID;
	UINT	m_edIndex;
	CString	m_edStrCharName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectSearchDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectSearchDlg)
	virtual void OnOK();
	afx_msg void OnSelchangeCOMBOObjectKind();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTSEARCHDLG_H__816FC11A_31CA_4641_B7CE_5D7DC27AB90E__INCLUDED_)
