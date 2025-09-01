// TCPTab.cpp : implementation file
//

#include "stdafx.h"
#include "ZoneServer.h"
#include "TCPTab.h"
#include "MyNetWorking.h"
#include "ZoneServerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTCPTab property page

IMPLEMENT_DYNCREATE(CTCPTab, CPropertyPage)

CTCPTab::CTCPTab() : CPropertyPage(CTCPTab::IDD)
{
	//{{AFX_DATA_INIT(CTCPTab)
	m_edConnSocketNum = 0;
	m_edOddMsg = 0;
	m_edRecvErr = 0;
	m_edSendBlock = 0;
	m_edSendErr = 0;
	m_edRecvBlk = 0;
	m_edRecvByte = 0.0;
	m_edSendByte = 0.0;
	m_edConnectUserNum = 0;
	//}}AFX_DATA_INIT

	m_bUpdate = false;
}

CTCPTab::~CTCPTab()
{
}

void CTCPTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTCPTab)
	DDX_Text(pDX, IDC_EDIT_CONNSOCKETNUM, m_edConnSocketNum);
	DDX_Text(pDX, IDC_EDIT_ODDMSG, m_edOddMsg);
	DDX_Text(pDX, IDC_EDIT_RecvErr, m_edRecvErr);
	DDX_Text(pDX, IDC_EDIT_SendBlock, m_edSendBlock);
	DDX_Text(pDX, IDC_EDIT_SendErr, m_edSendErr);
	DDX_Text(pDX, IDC_EDIT_RecvBlk, m_edRecvBlk);
	DDX_Text(pDX, IDC_EDIT_RECVBYTE, m_edRecvByte);
	DDX_Text(pDX, IDC_EDIT_SENDBYTE, m_edSendByte);
	DDX_Text(pDX, IDC_EDIT_CONNNUM, m_edConnectUserNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTCPTab, CPropertyPage)
	//{{AFX_MSG_MAP(CTCPTab)
	ON_BN_CLICKED(IDC_BUTTON_UPDATELISTSIZE, OnButtonUpdatelistsize)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTCPTab message handlers

void CTCPTab::OnButtonUpdatelistsize() 
{
	// TODO: Add your control notification handler code here
	CNetProcess* pProcess = g_Main.m_Network.m_pProcess[client_line];
//	pProcess->m_pNetData->ReCountEmtpyListSize();
	
}

void CTCPTab::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	UpdateTab();
	CPropertyPage::OnTimer(nIDEvent);
}

BOOL CTCPTab::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bUpdate = true;
	
	SetTimer(0, 1000, NULL);

	return CPropertyPage::OnSetActive();
}

#define		__MEGA	1000000

void CTCPTab::UpdateTab()
{
	CNetProcess* pProcess = g_Main.m_Network.m_pProcess[client_line];

	m_edConnectUserNum = CUserDB::s_nLoginNum - CPlayer::s_nLiveNum;
	m_edConnSocketNum = CPlayer::s_nLiveNum;
	m_edRecvByte = pProcess->m_NetSocket.m_TotalCount.GetTotalRecvSize()/__MEGA;
	m_edSendByte = pProcess->m_NetSocket.m_TotalCount.GetTotalSendSize()/__MEGA;
	m_edSendBlock = pProcess->m_NetSocket.m_TotalCount.m_dwTotalSendBlockNum;
	m_edRecvBlk = pProcess->m_NetSocket.m_TotalCount.m_dwTotalRecvBlockNum;
	m_edRecvErr = pProcess->m_NetSocket.m_TotalCount.m_dwTotalRecvErrNum;
	m_edSendErr = pProcess->m_NetSocket.m_TotalCount.m_dwTotalSendErrNum;
	m_edOddMsg = pProcess->m_nOddMsgNum;

	UpdateData(FALSE);
}

BOOL CTCPTab::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(0);
	
	return CPropertyPage::OnKillActive();
}
