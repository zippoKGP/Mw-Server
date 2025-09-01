// GameServerDoc.cpp : implementation of the CGameServerDoc class
//

#include "stdafx.h"
#include "ZoneServer.h"
#include "MyUtil.h"
#include "ZoneServerDoc.h"
#include "ZoneServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGameServerDoc* g_pDoc;

/////////////////////////////////////////////////////////////////////////////
// CGameServerDoc

IMPLEMENT_DYNCREATE(CGameServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CGameServerDoc, CDocument)
	//{{AFX_MSG_MAP(CGameServerDoc)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameServerDoc construction/destruction

BOOL CGameServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CGameServerDoc serialization

void CGameServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGameServerDoc diagnostics

#ifdef _DEBUG
void CGameServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGameServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

CGameServerDoc::CGameServerDoc()
{
	// TODO: add one-time construction code here
	
	g_pDoc = this;
}

CGameServerDoc::~CGameServerDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGameServerDoc commands

void CGameServerDoc::CreateDisplayView(CWnd* pWnd)
{
	m_DisplayView.Create(IDD_DIALOG_DISPLAY, pWnd);
	m_DisplayView.MoveWindow(18, 40, 400, 400);
	m_DisplayView.ShowWindow(SW_SHOW);
}

void CGameServerDoc::CreateSheetView(CWnd* pWnd)
{
	m_InfoSheet.Construct(_T("Display Sheet"), pWnd);
	m_InfoSheet.Create(pWnd, WS_CHILD|WS_VISIBLE);
	m_InfoSheet.MoveWindow(18+400+18, 35, 250, 400);
}
