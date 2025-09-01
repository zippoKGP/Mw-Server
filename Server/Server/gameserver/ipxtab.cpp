// IPXTab.cpp : implementation file
//

#include "stdafx.h"
#include "ZoneServer.h"
#include "ipxtab.h"
#include "MyNetWorking.h"
#include "ZoneServerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPXTab property page

IMPLEMENT_DYNCREATE(CIPXTab, CPropertyPage)

CIPXTab::CIPXTab() : CPropertyPage(CIPXTab::IDD)
{
	//{{AFX_DATA_INIT(CIPXTab)
	m_edBufList = 0;
	m_edConnSocketNum = 0;
	m_edMaxSocketNum = 0;
	m_edOddMsg = 0;
	m_edRecvErr = 0;
	m_edSendBlock = 0;
	m_edSendErr = 0;
	m_edSendSocketNum = 0;
	m_edRecvByte = 0.0;
	m_edSendByte = 0.0;
	//}}AFX_DATA_INIT

	m_bUpdate = false;
}

CIPXTab::~CIPXTab()
{
}

void CIPXTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPXTab)
	DDX_Text(pDX, IDC_EDIT_BUFLIST, m_edBufList);
	DDX_Text(pDX, IDC_EDIT_CONNSOCKETNUM, m_edConnSocketNum);
	DDX_Text(pDX, IDC_EDIT_MAXSOCKETNUM, m_edMaxSocketNum);
	DDX_Text(pDX, IDC_EDIT_ODDMSG, m_edOddMsg);
	DDX_Text(pDX, IDC_EDIT_RecvErr, m_edRecvErr);
	DDX_Text(pDX, IDC_EDIT_SendBlock, m_edSendBlock);
	DDX_Text(pDX, IDC_EDIT_SendErr, m_edSendErr);
	DDX_Text(pDX, IDC_EDIT_SENDSOCKETNUM, m_edSendSocketNum);
	DDX_Text(pDX, IDC_EDIT_RECVBYTE, m_edRecvByte);
	DDX_Text(pDX, IDC_EDIT_SENDBYTE, m_edSendByte);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPXTab, CPropertyPage)
	//{{AFX_MSG_MAP(CIPXTab)
	ON_BN_CLICKED(IDC_BUTTON_UPDATELISTSIZE, OnButtonUpdatelistsize)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPXTab message handlers

void CIPXTab::OnButtonUpdatelistsize() 
{
	// TODO: Add your control notification handler code here
}

BOOL CIPXTab::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnKillActive();
}

BOOL CIPXTab::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bUpdate = true;
	
	SetTimer(0, 1000, NULL);
	
	return CPropertyPage::OnSetActive();
}

void CIPXTab::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	UpdateTab();
	
	CPropertyPage::OnTimer(nIDEvent);
}

void CIPXTab::UpdateTab()
{
	CNetProcess* pProcess = g_Main.m_Network.m_pProcess[account_line];

	m_edMaxSocketNum = pProcess->m_Type.m_wSocketMaxNum;
	m_edConnSocketNum = pProcess->m_NetSocket.m_TotalCount.m_dwAcceptNum;
	m_edRecvByte = pProcess->m_NetSocket.m_TotalCount.GetTotalRecvSize();
	m_edSendByte = pProcess->m_NetSocket.m_TotalCount.GetTotalSendSize();
	m_edSendBlock = pProcess->m_NetSocket.m_TotalCount.m_dwTotalSendBlockNum;
	m_edSendSocketNum = pProcess->m_NetSocket.m_TotalCount.m_dwSendAbleNum;
	m_edRecvErr = pProcess->m_NetSocket.m_TotalCount.m_dwTotalRecvErrNum;
	m_edSendErr = pProcess->m_NetSocket.m_TotalCount.m_dwTotalSendErrNum;
	m_edOddMsg = pProcess->m_nOddMsgNum;

	UpdateData(FALSE);
}
