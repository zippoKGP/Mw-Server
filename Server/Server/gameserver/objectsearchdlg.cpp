// ObjectSearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZoneServer.h"
#include "ObjectSearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectSearchDlg dialog


CObjectSearchDlg::CObjectSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectSearchDlg)
	m_edIndex = 0;
	m_edStrCharName = _T("");
	//}}AFX_DATA_INIT
}


void CObjectSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectSearchDlg)
	DDX_Control(pDX, IDC_COMBO_ObjectKind, m_cdObjectKind);
	DDX_Control(pDX, IDC_COMBO_ObjectID, m_cdObjectID);
	DDX_Text(pDX, IDC_EDIT_OBJECT_INDEX, m_edIndex);
	DDX_Text(pDX, IDC_EDIT_CharName, m_edStrCharName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectSearchDlg, CDialog)  
	//{{AFX_MSG_MAP(CObjectSearchDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_ObjectKind, OnSelchangeCOMBOObjectKind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectSearchDlg message handlers

void CObjectSearchDlg::OnOK() 
{
	// TODO: Add extra validation here

	UpdateData();

	if(m_edStrCharName.GetLength() > 0)
	{
		int nLen = m_edStrCharName.GetLength();
		if(nLen > 16)
			nLen = 16;

		memcpy(m_szCharName, m_edStrCharName, nLen);
		m_szCharName[nLen] = NULL;
	}
	else
	{
		m_ID.m_byKind = m_cdObjectKind.GetCurSel();
		m_ID.m_byID = m_cdObjectID.GetCurSel();
		m_ID.m_wIndex = m_edIndex;
		m_szCharName[0] = NULL;
	}

	CDialog::OnOK();
}

void CObjectSearchDlg::OnSelchangeCOMBOObjectKind() 
{
	// TODO: Add your control notification handler code here

	int nKind = m_cdObjectKind.GetCurSel();
	switch(nKind)
	{
	case obj_kind_char:
		m_cdObjectID.ResetContent();
		m_cdObjectID.InsertString(obj_id_player, "Player");
		m_cdObjectID.InsertString(obj_id_monster, "Monster");
		m_cdObjectID.InsertString(obj_id_npc, "NPC");
		break;

	case obj_kind_item:
		m_cdObjectID.ResetContent();
		m_cdObjectID.InsertString(obj_id_itembox, "ItemBox");
		m_cdObjectID.InsertString(obj_id_dungeongate, "DungeonGate");
		break;
	}
	m_cdObjectID.SetCurSel(0);	
}

BOOL CObjectSearchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_cdObjectKind.InsertString(0, "Character");
	m_cdObjectKind.InsertString(1, "Item");

	CGameObject* pObj = CGameObject::s_pSelectObject;

	if(pObj)
	{
		m_cdObjectKind.SetCurSel(pObj->m_ObjID.m_byKind);
		OnSelchangeCOMBOObjectKind();
		m_cdObjectID.SetCurSel(pObj->m_ObjID.m_byID);
		m_edIndex = pObj->m_ObjID.m_wIndex;
	}
	else
	{
		m_cdObjectKind.SetCurSel(0);
		OnSelchangeCOMBOObjectKind();
		m_edIndex = 0;
	}
	
	m_szCharName[0] = NULL;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
