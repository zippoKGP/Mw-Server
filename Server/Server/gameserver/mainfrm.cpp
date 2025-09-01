// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ZoneServer.h"

#include "MainFrm.h"
#include "ZoneServerDoc.h"
#include "ZoneServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define STATUS_MAINFRAME 0
#define STATUS_SENDFRAME 1
#define STATUS_DBFRAME 2
#define STATUS_MOVEFRAME 3
#define STATUS_LOGIN 4
#define STATUS_SERVICE 5
#define STATUS_MSGR	6

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

extern CGameServerApp theApp;

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bExitConfirm = false;

}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable

	m_wndStatusBar.SetPaneInfo(STATUS_MAINFRAME, ID_SEPARATOR, SBPS_NORMAL, 60);
	m_wndStatusBar.SetPaneInfo(STATUS_SENDFRAME, ID_SEPARATOR, SBPS_NORMAL, 60);
	m_wndStatusBar.SetPaneInfo(STATUS_DBFRAME, ID_SEPARATOR, SBPS_NORMAL, 60);
	m_wndStatusBar.SetPaneInfo(STATUS_MOVEFRAME, ID_SEPARATOR, SBPS_NORMAL, 60);
	m_wndStatusBar.SetPaneInfo(STATUS_LOGIN, ID_SEPARATOR, SBPS_NORMAL, 60);
	m_wndStatusBar.SetPaneInfo(STATUS_SERVICE, ID_SEPARATOR, SBPS_NORMAL, 60);
	m_wndStatusBar.SetPaneInfo(STATUS_MSGR, ID_SEPARATOR, SBPS_NORMAL, 70);

	CString strBuf;
	

	SetTimer(0, 1000, NULL);

	g_pDoc->m_pwndMainFrame = (CWnd*)this;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style &= ~WS_TILEDWINDOW;
	cs.style |= ( WS_SYSMENU | WS_MINIMIZEBOX );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	CRect rc;

	GetWindowRect(&rc);
	MoveWindow(rc.left, rc.top, 700, 520);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	CString strBuf;

	strBuf.Format("M's : %d", g_Main.m_MainFrameRate.GetFPS());
	m_wndStatusBar.SetPaneText(STATUS_MAINFRAME, strBuf);

	strBuf.Format("S's : %d", g_Main.m_Network.m_pProcess[client_line]->GetSendThreadFrame());
	m_wndStatusBar.SetPaneText(STATUS_SENDFRAME, strBuf);

	strBuf.Format("D's : %d", g_Main.m_DBFrameRate.GetFPS());
	m_wndStatusBar.SetPaneText(STATUS_DBFRAME, strBuf);

	strBuf.Format("D'm : %d", g_Main.m_listDQSData.size());
	m_wndStatusBar.SetPaneText(STATUS_MOVEFRAME, strBuf);

	strBuf.Format("OPN : %d", g_Main.m_bWorldOpen);
	m_wndStatusBar.SetPaneText(STATUS_LOGIN, strBuf);

	strBuf.Format("SRV : %d", g_Main.m_bWorldService);
	m_wndStatusBar.SetPaneText(STATUS_SERVICE, strBuf);

	strBuf.Format("MSGR : %d/%d", g_Main.m_bMsgrOpen, g_Main.m_bUsingMsgr);
	m_wndStatusBar.SetPaneText(STATUS_MSGR, strBuf);

	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here

	KillTimer(0);
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	char szCaption[128];
	char szMode[32];

#ifdef __SERVICE
	sprintf(szMode, "서비스모드");
#endif

#ifndef __SERVICE
	sprintf(szMode, "개발모드");
#endif

	switch(message)
	{
	case WM_SETTEXT:
		{
			sprintf(szCaption, "RF Online: %s Server (%s), MAX(%d)", g_Main.m_szWorldName, szMode, MAX_PLAYER);
			lParam = (LPARAM)szCaption;
		}
		break;

	case WM_CLOSE:
/*
		if(g_Main.m_pStoreLinkData->m_bStoreOn && !m_bExitConfirm)
		{
			int nOut = MessageBox("DBStroe Programe Exit?", "Warning", MB_ICONWARNING|MB_YESNOCANCEL);

			switch(nOut)
			{
			case IDYES:
				::SendMessage(g_Main.m_pStoreLinkData->m_hStoreWnd, WM_CLOSE, 0, 0);
				break;

			case IDNO:
				m_bExitConfirm = true;
				break;  

			case IDCANCEL:
				return 0;
			}
		}
		if(g_Main.m_MapDisplay.m_bDisplayMode && !m_bExitConfirm)
		{
			m_bExitConfirm = true;
			g_Main.m_MapDisplay.m_MapExtend.m_bExtendMode = false;
		}
*/
		int nOut = MessageBox("Programe Exit?", "Warning", MB_ICONWARNING|MB_YESNO);

		switch(nOut)
		{
		case IDYES:
			g_Main.gm_ServerClose();
			g_Main.m_MapDisplay.m_MapExtend.m_bExtendMode = false;
//			g_Main.m_MapDisplay.m_bDisplayCollLine = false;
	//		g_Main.m_Network.Release();
			break;

		case IDNO:
			return 0;		
		}

		break;
	}
	
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CFrameWnd::OnPaint() for painting messages
}
