// MapTab.cpp : implementation file
//

#include "stdafx.h"
#include "ZoneServer.h"
#include "MapTab.h"
#include "ZoneServerDoc.h"
#include "MyUtil.h"
#include "MapFld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapTab property page

IMPLEMENT_DYNCREATE(CMapTab, CPropertyPage)

CMapTab::CMapTab() : CPropertyPage(CMapTab::IDD)
{
	//{{AFX_DATA_INIT(CMapTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMapTab::~CMapTab()
{
}

void CMapTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapTab)
	DDX_Control(pDX, IDC_TREE_MAP, m_trMap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapTab, CPropertyPage)
	//{{AFX_MSG_MAP(CMapTab)
	ON_BN_CLICKED(IDC_BUTTON_MAPCHANGE, OnButtonMapchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapTab message handlers

BOOL CMapTab::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	g_Main.m_GameMsg.PackingMsg(gm_interface_update_map);
	
	return CPropertyPage::OnSetActive();
}

void CMapTab::UpdateTab()
{
	m_trMap.DeleteAllItems();

	CString strBuf;

	for(int i = 0; i < g_Main.m_MapOper.m_nMapNum; i++)
	{
		static char* pszUse[2] = {"미사용", "사용"};

		CMapData* pMap = g_Main.m_MapOper.GetMap(i);

		strBuf.Format("%dth Map : %s..(%s)", i, g_Main.m_MapOper.m_Map[i].m_pMapSet->m_strFileName, pszUse[pMap->m_bUse]);
		m_hMap[i] = m_trMap.InsertItem(strBuf);

		_bsp_info* pBspInfo = pMap->GetBspInfo();
		int* size = pBspInfo->m_nMapMaxSize;
		strBuf.Format("+Size : [%d][%d][%d]", size[0], size[1], size[2]);
		m_trMap.InsertItem(strBuf, m_hMap[i]);

		size = pBspInfo->m_nMapMinSize;
		strBuf.Format("-Size : [%d][%d][%d]", size[0], size[1], size[2]);
		m_trMap.InsertItem(strBuf, m_hMap[i]);

		size = pBspInfo->m_nMapSize;
		strBuf.Format("MapSize : [%d][%d][%d]", size[0], size[1], size[2]);
		m_trMap.InsertItem(strBuf, m_hMap[i]);

		strBuf.Format("LeafNum : %d", pBspInfo->m_nLeafNum);
		m_trMap.InsertItem(strBuf, m_hMap[i]);

		strBuf.Format("Width/Sec : %d", MAP_STD_SEC_SIZE);
		m_trMap.InsertItem(strBuf, m_hMap[i]);

		strBuf.Format("Height/Sec : %d", MAP_STD_SEC_SIZE);
		m_trMap.InsertItem(strBuf, m_hMap[i]);

		_sec_info* pSecInfo = pMap->GetSecInfo();

		strBuf.Format("WidthSecNum : %d", pSecInfo->m_nSecNumW);
		m_trMap.InsertItem(strBuf, m_hMap[i]);

		strBuf.Format("HeightSecNum : %d", pSecInfo->m_nSecNumH);
		m_trMap.InsertItem(strBuf, m_hMap[i]);
	}
}

void CMapTab::OnButtonMapchange() 
{
	// TODO: Add your control notification handler code here

	if(!g_Main.m_MapDisplay.m_bDisplayMode)
	{
		::MyMessageBox("Display Error", "Not Display Mode");
		return ;
	}

	HTREEITEM hItem = m_trMap.GetSelectedItem();

	if((hItem != NULL) && m_trMap.ItemHasChildren(hItem))
	{
		for(int i = 0; i < g_Main.m_MapOper.m_nMapNum; i++)
		{
			if(hItem == m_hMap[i])
			{
				g_Main.m_GameMsg.PackingMsg(gm_interface_map_change, i);

				break;
			}
		}
	}
}
