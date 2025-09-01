// ServerTab.cpp : implementation file
//

#include "stdafx.h"
#include "ZoneServer.h"
#include "ServerTab.h"
#include "Player.h"
#include "Monster.h"
#include "MapData.h"
#include "ZoneServerDoc.h"
#include "MapFld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerTab property page

IMPLEMENT_DYNCREATE(CServerTab, CPropertyPage)

CServerTab::CServerTab() : CPropertyPage(CServerTab::IDD)
{
	//{{AFX_DATA_INIT(CServerTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CServerTab::~CServerTab()
{
}

void CServerTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerTab)
	DDX_Control(pDX, IDC_TREE_SERVER, m_trServer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerTab, CPropertyPage)
	//{{AFX_MSG_MAP(CServerTab)
	ON_BN_CLICKED(IDC_BUTTON_UPDATESERVER, OnButtonUpdateserver)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerTab message handlers

BOOL CServerTab::OnSetActive()
{
	g_Main.m_GameMsg.PackingMsg(gm_interface_update_server);

	return CPropertyPage::OnSetActive();
}

BOOL CServerTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CServerTab::UpdateServerTab()
{
	int i;

	m_trServer.DeleteAllItems();

	CString strBuf;

	HTREEITEM hConn = m_trServer.InsertItem("<<접속자수>>");

	strBuf.Format("플레이어수 : %d", CPlayer::s_nLiveNum);
	HTREEITEM hPlyNum = m_trServer.InsertItem(strBuf, hConn);
	for(i = 0; i < g_Main.m_MapOper.m_nMapNum; i++)
	{
		CMapData* pMap = g_Main.m_MapOper.GetMap(i);

		strBuf.Format("%d: %s : %d명", i, pMap->m_pMapSet->m_strCode, pMap->m_nMapInPlayerNum);
		m_trServer.InsertItem(strBuf, hPlyNum);	
	
//		FillServerTabChar(hPlyNum, i, obj_id_player);
	}
	m_trServer.Expand(hPlyNum, TVE_EXPAND);  
	
	strBuf.Format("몬스터수 : %d", CMonster::s_nLiveNum);
	HTREEITEM hMonNum = m_trServer.InsertItem(strBuf, hConn);
	for(i = 0; i < g_Main.m_MapOper.m_nMapNum; i++)
	{
		CMapData* pMap = g_Main.m_MapOper.GetMap(i);

		strBuf.Format("%d: %s : %d명", i, pMap->m_pMapSet->m_strCode, pMap->m_nMapInMonsterNum);
		m_trServer.InsertItem(strBuf, hMonNum);	
	
	//	FillServerTabChar(hMonNum, i, obj_id_monster);
	}
	m_trServer.Expand(hMonNum, TVE_EXPAND);   

	m_trServer.Expand(hConn, TVE_EXPAND);  	
}

void CServerTab::OnButtonUpdateserver() 
{
	// TODO: Add your control notification handler code here

	g_Main.m_GameMsg.PackingMsg(gm_interface_update_server);
}

