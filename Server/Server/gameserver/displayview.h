#if !defined(AFX_DISPLAYVIEW_H__21B7B17A_D547_4016_9E30_34A5FADD6C9F__INCLUDED_)
#define AFX_DISPLAYVIEW_H__21B7B17A_D547_4016_9E30_34A5FADD6C9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisplayView.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDisplayView dialog

class CDisplayView : public CDialog
{
// Construction
public:
	CDisplayView(CWnd* pParent = NULL);   // standard constructor
	CPoint m_ptLBtn;

	void SetExtendMode(BOOL bExtend);
	void GetDrawableRect(CRect* rcOut);
	
// Dialog Data
	//{{AFX_DATA(CDisplayView)
	enum { IDD = IDD_DIALOG_DISPLAY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nVerRange;
	int m_nHorzRange;
	int m_nVertPos;
	int m_nHorzPos;
	int m_nVerPage;
	int m_nHorzPage;

	// Generated message map functions
	//{{AFX_MSG(CDisplayView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYVIEW_H__21B7B17A_D547_4016_9E30_34A5FADD6C9F__INCLUDED_)
