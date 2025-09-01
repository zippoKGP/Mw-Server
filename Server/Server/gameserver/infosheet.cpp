// InfoSheet.cpp : implementation file
//

#include "stdafx.h"
#include "ZoneServer.h"
#include "InfoSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoSheet

IMPLEMENT_DYNAMIC(CInfoSheet, CPropertySheet)

CInfoSheet::CInfoSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CInfoSheet::CInfoSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

CInfoSheet::CInfoSheet()
	:CPropertySheet()
{

}

void CInfoSheet::Construct(LPCTSTR pszCaption, CWnd* pParentWnd/* = NULL*/, UINT iSelectPage/* = 0*/)
{
	CPropertySheet::Construct(pszCaption, pParentWnd, iSelectPage);

	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage(&m_tabTCP);
	AddPage(&m_tabIPX);
	AddPage(&m_tabServer);
	AddPage(&m_tabObject);
	AddPage(&m_tabMap);
}

CInfoSheet::~CInfoSheet()
{
}

void CInfoSheet::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint/* = TRUE*/)
{
	CPropertySheet::MoveWindow(x, y, nWidth, nHeight);
}

BEGIN_MESSAGE_MAP(CInfoSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CInfoSheet)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoSheet message handlers

int CInfoSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	return 0;
}
