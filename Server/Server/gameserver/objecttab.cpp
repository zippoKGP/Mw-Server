// ObjectTab.cpp : implementation file
//

#include "stdafx.h"
#include "ZoneServer.h"
#include "ObjectTab.h"
#include "GameObject.h"
#include "Character.h"
#include "MapData.h"
#include "ZoneServerDoc.h"
#include "ObjectSearchDlg.h"
#include "DfnEquipItemFld.h"
#include "WeaponItemFld.h"
#include "PotionItemFld.h"
#include "ForceItemFld.h"
#include "SkillFld.h"
#include "ForceFld.h"
#include "BattleDungeonItemFld.h"
#include "UnitFrameFld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectTab property page

IMPLEMENT_DYNCREATE(CObjectTab, CPropertyPage)

CObjectTab::CObjectTab() : CPropertyPage(CObjectTab::IDD)
{
	//{{AFX_DATA_INIT(CObjectTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CObjectTab::~CObjectTab()
{
}

void CObjectTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectTab)
	DDX_Control(pDX, IDC_BUTTON_AUTO, m_btAuto);
	DDX_Control(pDX, IDC_TREE_OBJECT, m_trObject);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectTab, CPropertyPage)
	//{{AFX_MSG_MAP(CObjectTab)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, OnButtonAuto)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_DESTORY, OnButtonDestory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectTab message handlers

void CObjectTab::UpdateTab()
{
	int i;
	m_trObject.DeleteAllItems();
	
	CString strBuf;

	CGameObject* pObj = CGameObject::s_pSelectObject;
	if(pObj == NULL)
	{
		strBuf = "Nothing Selected Object";
		m_trObject.InsertItem(strBuf);
		return;
	}

	if(pObj->m_ObjID.m_byKind == obj_kind_char &&
		pObj->m_ObjID.m_byID == obj_id_player)
	{
		CPlayer* pPlayer = (CPlayer*)pObj;

		strBuf = "Char";		
		strBuf += "-Player - ";
		strBuf += ((CPlayer*)pObj)->m_Param.GetCharName();
		m_trObject.InsertItem(strBuf);

		strBuf.Format("Degree : %d", pPlayer->m_byUserDgr);
		m_trObject.InsertItem(strBuf);

		strBuf.Format("Index : %d", pObj->m_ObjID.m_wIndex);
		m_trObject.InsertItem(strBuf);

		strBuf.Format("Live : %d", pObj->m_bLive);
		m_trObject.InsertItem(strBuf);

		strBuf.Format("RecIndex : %d", ((_base_fld*)pObj->m_pRecordSet)->m_dwIndex);
		m_trObject.InsertItem(strBuf);

		strBuf.Format("Move : %d", ((CCharacter*)pObj)->m_bMove);
		m_trObject.InsertItem(strBuf);

		strBuf.Format("TarPos : [%d], [%d], [%d]", 
			(int)((CCharacter*)pObj)->m_fTarPos[0], (int)((CCharacter*)pObj)->m_fTarPos[1], (int)((CCharacter*)pObj)->m_fTarPos[2]);
		m_trObject.InsertItem(strBuf);	

		strBuf.Format("m_nMoveType : %d", ((CPlayer*)pObj)->m_nMoveType);
		m_trObject.InsertItem(strBuf);

		strBuf.Format("CurPos : [%d], [%d], [%d]", (int)pObj->m_fCurPos[0], (int)pObj->m_fCurPos[1], (int)pObj->m_fCurPos[2]);
		m_trObject.InsertItem(strBuf);

		strBuf.Format("CurSector : %d",  pObj->GetCurSecNum());
		m_trObject.InsertItem(strBuf);

		strBuf.Format("달란트 : %d",  pPlayer->m_Param.GetDalant());
		m_trObject.InsertItem(strBuf);

		strBuf.Format("골드 : %d",  pPlayer->m_Param.GetGold());
		m_trObject.InsertItem(strBuf);

		static char* pszEquipUnit[] = {"맨손", "무기", "유닛"};
		strBuf.Format("장착무기(%s)", pszEquipUnit[((CPlayer*)pObj)->m_pmWpn.GetAttackToolType()]);
		HTREEITEM hWeapon = m_trObject.InsertItem(strBuf);
		{
			_WEAPON_PARAM* pw = &((CPlayer*)pObj)->m_pmWpn;

			strBuf.Format("nGaMinAF(%d), nGaMaxAF(%d), nMaMaxAF(%d), nMaMinAF(%d)", pw->nGaMinAF, pw->nGaMaxAF, pw->nMaMaxAF, pw->nMaMinAF);
			m_trObject.InsertItem(strBuf, hWeapon);
			strBuf.Format("byGaMinSel(%d), byGaMaxSel(%d), byMaMinSel(%d), byMaMaxSel(%d)", pw->byGaMinSel, pw->byGaMaxSel, pw->byMaMinSel, pw->byMaMaxSel);
			m_trObject.InsertItem(strBuf, hWeapon);
			strBuf.Format("byAttTolType(%d), byWpClass(%d), byWpType(%d)", pw->byAttTolType, pw->byWpClass, pw->byWpType);
			m_trObject.InsertItem(strBuf, hWeapon);
			strBuf.Format("wGaAttRange(%d), wMaAttRange(%d)", pw->wGaAttRange, pw->wMaAttRange);
			m_trObject.InsertItem(strBuf, hWeapon);

			if(pw->pFixWp)
			{
				_WeaponItem_fld* pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pw->pFixWp->m_wItemIndex);
				strBuf.Format("무기..%s", pFld->m_strName);
			}
			else if(pw->pFixUnit)
			{
				_UnitFrame_fld* pFld = (_UnitFrame_fld*)g_Main.m_tblUnitFrame.GetRecord(pw->pFixUnit->byFrame);
				strBuf.Format("유닛..%s", pFld->m_strCode);
			}
			else
			{
				strBuf.Format("맨손");
			}
			m_trObject.InsertItem(strBuf, hWeapon);
		}

		strBuf.Format("장착");
		HTREEITEM hEquip = m_trObject.InsertItem(strBuf);

		static char* pszFixPart[] = {"상의", "하의", "장갑", "신발", "모자", "방패", "무기", "망토"};

		for(i = 0; i < equip_fix_num; i++)
		{
			__ITEM* pCon = &((CPlayer*)pObj)->m_Param.m_dbEquip.m_pStorageList[i];

			if(pCon->m_bLoad)
			{
				_base_fld* pFld = g_Main.m_tblItemData[i].GetRecord(pCon->m_wItemIndex);

				strBuf.Format("%d: [%s]%s, [sr]%d, [dr]%d, [lv]%s", 
					((CPlayer*)pObj)->GetEffectEquipCode(_STORAGE_POS::EQUIP, i), pszFixPart[i], pFld->m_strCode, pCon->m_wSerial, pCon->m_dwDur, ::DisplayItemUpgInfo(i, pCon->m_dwLv));				
			}		
			else
			{
				strBuf.Format("%d: [%s]:def", ((CPlayer*)pObj)->GetEffectEquipCode(_STORAGE_POS::EQUIP, i), pszFixPart[i]);
			}
			m_trObject.InsertItem(strBuf, hEquip);
		}

		strBuf.Format("장신구 (%d개)", embellish_fix_num-pPlayer->m_Param.m_dbEmbellish.GetNumEmptyCon());
		HTREEITEM hEmbellish = m_trObject.InsertItem(strBuf);

		for(i = 0; i < embellish_fix_num; i++)
		{
			__ITEM* pCon = &((CPlayer*)pObj)->m_Param.m_dbEmbellish.m_pStorageList[i];

			if(pCon->m_bLoad)
			{
				_base_fld* pFld = g_Main.m_tblItemData[pCon->m_byTableCode].GetRecord(pCon->m_wItemIndex);
				strBuf.Format("%d: %s, [sr]%d", 
					((CPlayer*)pObj)->GetEffectEquipCode(_STORAGE_POS::EMBELLISH, i), pFld->m_strCode, pCon->m_wSerial);

				m_trObject.InsertItem(strBuf, hEmbellish);
			}		
		}

		BYTE byUseInvenNum = one_bag_store_num*pPlayer->m_Param.GetBagNum();
		strBuf.Format("인벤토리(%d개)", byUseInvenNum-pPlayer->m_Param.m_dbInven.GetNumEmptyCon());
		HTREEITEM hIven = m_trObject.InsertItem(strBuf);
		for(i = 0; i < byUseInvenNum; i++)
		{
			__ITEM* pCon = &pPlayer->m_Param.m_dbInven.m_pStorageList[i];
			if(pCon->m_bLoad)
			{	
				_base_fld* pFld = g_Main.m_tblItemData[pCon->m_byTableCode].GetRecord(pCon->m_wItemIndex);
				if(!pFld)
				{
					strBuf.Format("NULL");
				}
				else
				{
					strBuf.Format("%s, [amt]%d, [sr]%d, [lv]%s", pFld->m_strCode, pCon->m_dwDur, pCon->m_wSerial, ::DisplayItemUpgInfo(pCon->m_byTableCode, pCon->m_dwLv));
				}

				m_trObject.InsertItem(strBuf, hIven);
			}
		}

		strBuf.Format("포스창 (%d개)", force_storage_num-pPlayer->m_Param.m_dbForce.GetNumEmptyCon());
		HTREEITEM hForceIven = m_trObject.InsertItem(strBuf);
		for(i = 0; i < force_storage_num; i++)
		{
			__ITEM* pCon = &pPlayer->m_Param.m_dbForce.m_pStorageList[i];
			if(pCon->m_bLoad)
			{	
				_base_fld* pFld = g_Main.m_tblItemData[pCon->m_byTableCode].GetRecord(pCon->m_wItemIndex);
				if(!pFld)
				{
					strBuf.Format("NULL");
				}
				else
				{
					strBuf.Format("%s, [누적]%d, [sr]%d", pFld->m_strCode, pCon->m_dwDur, pCon->m_wSerial);
				}

				m_trObject.InsertItem(strBuf, hForceIven);
			}
		}

		strBuf.Format("벨트: %d개사용", potion_belt_num-pPlayer->m_Param.m_dbBelt.GetNumEmptyCon());//pPlayer->m_Param.GetUsePoBeltNum());
		HTREEITEM hBelt = m_trObject.InsertItem(strBuf);		
		for(i = 0; i < potion_belt_num; i++)
		{
			__ITEM* pCon = &pPlayer->m_Param.m_dbBelt.m_pStorageList[i];
			if(pCon->m_bLoad)
			{
				_PotionItem_fld* pFld = (_PotionItem_fld*)g_Main.m_tblItemData[tbl_code_potion].GetRecord(pCon->m_wItemIndex);
				strBuf.Format("%s, [수량]%d, [sr]%d, [ci]%d", pFld->m_strName, pCon->m_dwDur, pCon->m_wSerial, pCon->m_byClientIndex);

				m_trObject.InsertItem(strBuf, hBelt);
			}
		}

		if(pPlayer->m_Param.GetRaceSexCode()/2 == race_code_cora)
		{
			strBuf.Format("애니머스창 (%d개)", animus_storage_num-pPlayer->m_Param.m_dbAnimus.GetNumEmptyCon());
			HTREEITEM hAnimusIven = m_trObject.InsertItem(strBuf);
			for(i = 0; i < animus_storage_num; i++)
			{
				__ITEM* pCon = &pPlayer->m_Param.m_dbAnimus.m_pStorageList[i];
				if(pCon->m_bLoad)
				{	
					_base_fld* pFld = g_Main.m_tblItemData[pCon->m_byTableCode].GetRecord(pCon->m_wItemIndex);
					if(!pFld)
					{
						strBuf.Format("NULL");
					}
					else
					{
						_animus_param* pParam = (_animus_param*)&pCon->m_dwLv;
						strBuf.Format("%s, [경험치]%d, [HP]%d, [FP]%d, [sr]%d", pFld->m_strCode, pCon->m_dwDur, pParam->wHP, pParam->wFP, pCon->m_wSerial);
					}

					m_trObject.InsertItem(strBuf, hAnimusIven);
				}
			}
		}

		strBuf.Format("Res Buffer: %d개소지", pPlayer->m_Param.GetResBufferNum());
		HTREEITEM hResBuffer = m_trObject.InsertItem(strBuf);		
		for(i = 0; i < ::GetMaxResKind(); i++)
		{
			WORD wNum = pPlayer->m_Param.m_wCuttingResBuffer[i];
			if(wNum > 0)
			{
				_base_fld* pFld = (_base_fld*)g_Main.m_tblItemData[tbl_code_res].GetRecord(i);
				strBuf.Format("[code]%s, [num]%d", pFld->m_strCode, wNum);

				m_trObject.InsertItem(strBuf, hResBuffer);
			}
		}

		strBuf.Format("Mastery");
		HTREEITEM hMastery = m_trObject.InsertItem(strBuf);		
		for(i = 0; i < WP_CLASS_NUM; i++)
		{
			DWORD dwCum = pPlayer->m_pmMst.GetCumPerMast(mastery_code_weapon, i);
			BYTE byMp = pPlayer->m_pmMst.GetMasteryPerMast(mastery_code_weapon, i);

			strBuf.Format("Weapon Mp.. %d) cum: %d, mp: %d", i, dwCum, byMp);
			m_trObject.InsertItem(strBuf, hMastery);
		}
		{
			DWORD dwCum = pPlayer->m_pmMst.GetCumPerMast(mastery_code_defence, 0);
			BYTE byMp = pPlayer->m_pmMst.GetMasteryPerMast(mastery_code_defence, 0);
			strBuf.Format("Suffer Mp.. %d) cum: %d, mp: %d", i, dwCum, byMp);
			m_trObject.InsertItem(strBuf, hMastery);
		}
		{
			DWORD dwCum = pPlayer->m_pmMst.GetCumPerMast(mastery_code_shield, 0);
			BYTE byMp = pPlayer->m_pmMst.GetMasteryPerMast(mastery_code_shield, 0);
			strBuf.Format("Shield Mp.. %d) cum: %d, mp: %d", i, dwCum, byMp);
			m_trObject.InsertItem(strBuf, hMastery);
		}
		for(i = 0; i < skill_mastery_num; i++)
		{
			DWORD dwCum = pPlayer->m_pmMst.GetCumPerMast(mastery_code_skill, i);
			BYTE byMp = pPlayer->m_pmMst.GetMasteryPerMast(mastery_code_skill, i);

			strBuf.Format("Skill Mp.. %d) cum: %d, mp: %d", i, dwCum, byMp);
			m_trObject.InsertItem(strBuf, hMastery);
		}
		for(i = 0; i < force_mastery_num; i++)
		{
			DWORD dwCum = pPlayer->m_pmMst.GetCumPerMast(mastery_code_force, i);
			BYTE byMp = pPlayer->m_pmMst.GetMasteryPerMast(mastery_code_force, i);

			strBuf.Format("Force Mp.. %d) cum: %d, mp: %d", i, dwCum, byMp);
			m_trObject.InsertItem(strBuf, hMastery);
		}
		{
			strBuf.Format("Staff Mp.. mp: %d", pPlayer->m_pmMst.m_mtyStaff);
			m_trObject.InsertItem(strBuf, hMastery);
		}
		for(i = 0; i < max_skill_num; i++)
		{
			DWORD dwCum = pPlayer->m_pmMst.m_BaseCum.m_dwSkillCum[i];
			BYTE byMp = pPlayer->m_pmMst.GetSkillLv(i);

			strBuf.Format("Skill Lv.. %d) cum: %d, lv: %d", i, dwCum, byMp);
			m_trObject.InsertItem(strBuf, hMastery);
		}

		strBuf.Format("적용중인 지속효과");
		HTREEITEM hSFEffect = m_trObject.InsertItem(strBuf);		
		for(i = 0; i < SF_CONT_CODE_NUM; i++)
		{
			for(int c = 0; c < CONT_SF_SIZE; c++)
			{
				_sf_continous* pCon = &pPlayer->m_SFCont[i][c];			
				if(pCon->m_bExist)
				{
					if(pCon->m_byEffectCode == effect_code_skill)
					{
						_skill_fld* pFld = (_skill_fld*)g_Main.m_tblEffectData[pCon->m_byEffectCode].GetRecord(pCon->m_wEffectIndex);
						strBuf.Format("%s", pFld->m_strKorName);
					}
					else
					{
						_force_fld* pFld = (_force_fld*)g_Main.m_tblEffectData[pCon->m_byEffectCode].GetRecord(pCon->m_wEffectIndex);
						strBuf.Format("%s", pFld->m_strKorName);
					}
					m_trObject.InsertItem(strBuf, hSFEffect);					
				}
			}
		}

		strBuf.Format("파티원리스트 : 파트모드(%d), 보스상태(%d), 파티원수(%d)", ((CPlayer*)pObj)->m_pPartyMgr->IsPartyMode(), ((CPlayer*)pObj)->m_pPartyMgr->IsPartyBoss(), ((CPlayer*)pObj)->m_pPartyMgr->GetPopPartyMember());
		HTREEITEM hPartyList = m_trObject.InsertItem(strBuf);		
		CPartyPlayer** ppMem = ((CPlayer*)pObj)->m_pPartyMgr->GetPtrPartyMember();
		if(ppMem)
		{
			for(int m = 0; m < member_per_party; m++)
			{
				if(ppMem[m])
				{
					strBuf.Format("%s", ppMem[m]->m_szName);
					m_trObject.InsertItem(strBuf, hPartyList);					
				}
				else
					break;
			}
		}

		strBuf.Format("내성 : 물: %d, 불: %d, 흙: %d, 바람: %d", ((CPlayer*)pObj)->GetWaterTol(), ((CPlayer*)pObj)->GetFireTol(), ((CPlayer*)pObj)->GetSoilTol(), ((CPlayer*)pObj)->GetWindTol());
		m_trObject.InsertItem(strBuf);		

		strBuf.Format("공격력 : min(%d), max(%d)", (int)((CPlayer*)pObj)->m_pmWpn.nGaMinAF, (int)((CPlayer*)pObj)->m_pmWpn.nGaMaxAF);
		m_trObject.InsertItem(strBuf);

		strBuf.Format("Parameter");
		HTREEITEM hParam = m_trObject.InsertItem(strBuf);

		strBuf.Format("m_nST : %d", pPlayer->m_Param.GetSP());
		m_trObject.InsertItem(strBuf, hParam);
		strBuf.Format("m_nHP : %d", pPlayer->m_Param.GetHP());
		m_trObject.InsertItem(strBuf, hParam);
		strBuf.Format("m_nFP : %d", pPlayer->m_Param.GetFP());
		m_trObject.InsertItem(strBuf, hParam);
		strBuf.Format("m_dExp : %f", pPlayer->m_Param.GetExp());
		m_trObject.InsertItem(strBuf, hParam);
		strBuf.Format("m_nLevel : %d", pPlayer->m_Param.GetLevel());
		m_trObject.InsertItem(strBuf, hParam);
	}

	if(pObj->m_ObjID.m_byKind == obj_kind_item)
	{
		if(pObj->m_ObjID.m_byID == obj_id_itembox)
		{
			CItemBox* pItem = (CItemBox*)pObj;

			strBuf.Format("Index : %d", pItem->m_ObjID.m_wIndex);
			m_trObject.InsertItem(strBuf);

			_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)pItem->m_pRecordSet;
			strBuf.Format("Item Name : %s(%s)", pItem->m_pRecordSet->m_strCode, pFld->m_strName);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("State : %d", pItem->m_nStateCode);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("Dur or Amount : %d", pItem->m_Item.m_dwDur);
			m_trObject.InsertItem(strBuf);

			if(pItem->m_pOwnerPtr)
				strBuf.Format("Owner : %s", pItem->m_pOwnerPtr->m_szName);
			else
				strBuf.Format("Owner : 없다");
			m_trObject.InsertItem(strBuf);

			strBuf.Format("CurPos : [%d], [%d], [%d]", (int)pObj->m_fCurPos[0], (int)pObj->m_fCurPos[1], (int)pObj->m_fCurPos[2]);
			m_trObject.InsertItem(strBuf);
		}
	}	

	if(pObj->m_ObjID.m_byKind == obj_kind_item)
	{
		if(pObj->m_ObjID.m_byID == obj_id_dungeongate)
		{
			CBattleDungeon* pDungeon = (CBattleDungeon*)pObj;

			strBuf.Format("Index : %d", pDungeon->m_ObjID.m_wIndex);
			m_trObject.InsertItem(strBuf);

			_BattleDungeonItem_fld* pFld = (_BattleDungeonItem_fld*)pDungeon->m_pRecordSet;
			strBuf.Format("Item Name : %s(%s)", pDungeon->m_pRecordSet->m_strCode, pFld->m_strName);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("State : %d", pDungeon->m_nStateCode);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("CurPos : [%d], [%d], [%d]", (int)pObj->m_fCurPos[0], (int)pObj->m_fCurPos[1], (int)pObj->m_fCurPos[2]);
			m_trObject.InsertItem(strBuf);
		}
	}	
	//*MAYUN
	if(pObj->m_ObjID.m_byKind == obj_kind_char)
	{
		if(pObj->m_ObjID.m_byID == obj_id_monster)
		{
			CMonster* pMon = (CMonster*)pObj;
			strBuf = "Mon";		
			strBuf += "-Monster - ";
			strBuf += ((_monster_fld*)pMon->m_pRecordSet)->m_strName;
			m_trObject.InsertItem(strBuf);

			strBuf.Format("Index : %d", pMon->m_ObjID.m_wIndex);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("Serial : %d", pMon->m_dwObjSerial);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("Live : %d", pMon->m_bLive);
			m_trObject.InsertItem(strBuf);
			
			strBuf.Format("RecIndex : %d", ((_base_fld*)pObj->m_pRecordSet)->m_dwIndex);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("StartDummy : %s", pMon->m_pDumPosition->m_szCode);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("Move : %d", ((CCharacter*)pObj)->m_bMove);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("TarPos : [%d], [%d], [%d]", 
				(int)((CCharacter*)pObj)->m_fTarPos[0], (int)((CCharacter*)pObj)->m_fTarPos[1], (int)((CCharacter*)pObj)->m_fTarPos[2]);
			m_trObject.InsertItem(strBuf);	

			strBuf.Format("m_nMoveType : %d", ((CPlayer*)pObj)->m_nMoveType);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("CurPos : [%d], [%d], [%d]", (int)pObj->m_fCurPos[0], (int)pObj->m_fCurPos[1], (int)pObj->m_fCurPos[2]);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("AbsPos : [%d], [%d], [%d]", (int)pObj->m_fAbsPos[0], (int)pObj->m_fAbsPos[1], (int)pObj->m_fAbsPos[2]);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("ScrPos : [%d], [%d]", pObj->m_nScreenPos[0], pObj->m_nScreenPos[1]);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("CurMap : %s", pObj->m_pCurMap->GetBspInfo()->m_szBspName);
			m_trObject.InsertItem(strBuf);

			strBuf.Format("CurSector : %d",  pObj->GetCurSecNum());
			m_trObject.InsertItem(strBuf);

			strBuf.Format("Parameter");
			HTREEITEM hParam = m_trObject.InsertItem(strBuf);

			
			strBuf.Format("m_nHP : %d", pMon->m_nHP);
			m_trObject.InsertItem(strBuf, hParam);
			
			strBuf.Format("m_nLevel : %d", pMon->GetLevel());
			m_trObject.InsertItem(strBuf, hParam);

			strBuf.Format("DefSkill : %d", pMon->GetDefSkill());
			m_trObject.InsertItem(strBuf, hParam);

			strBuf.Format("Skill.m_MinDmg : %d", pMon->m_Skill[0].m_MinDmg);
			m_trObject.InsertItem(strBuf, hParam);

			strBuf.Format("Skill.m_MaxDmg : %d", pMon->m_Skill[0].m_MaxDmg);
			m_trObject.InsertItem(strBuf, hParam);

			strBuf.Format("Skill.m_CrtDmg : %d", pMon->m_Skill[0].m_CritDmg);
			m_trObject.InsertItem(strBuf, hParam);
			
			strBuf.Format("Head Def : %d", pMon->GetDefFC(CMonster::DEFHEAD, NULL));
			m_trObject.InsertItem(strBuf, hParam);

			strBuf.Format("U Body Def : %d", pMon->GetDefFC(CMonster::DEFUP, NULL));
			m_trObject.InsertItem(strBuf, hParam);

			strBuf.Format("D Body Def : %d", pMon->GetDefFC(CMonster::DEFDOWN, NULL));
			m_trObject.InsertItem(strBuf, hParam);

			strBuf.Format("Hand Def : %d", pMon->GetDefFC(CMonster::DEFHAND, NULL));
			m_trObject.InsertItem(strBuf, hParam);
			
			strBuf.Format("Foot Def : %d", pMon->GetDefFC(CMonster::DEFFOOT, NULL));
			m_trObject.InsertItem(strBuf, hParam);

		//	m_trObject.Expand(hParam, TVE_EXPAND);

			strBuf.Format("AI State");
			HTREEITEM hParam2 = m_trObject.InsertItem(strBuf);
			char buf[256];
			GeCurrentStateString(buf,pMon->m_MonAI.Prime_State.m_State);
			strBuf.Format("PrmState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Move_Target_State.m_State);
			strBuf.Format("MvTState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Attack_State.m_State);
			strBuf.Format("AttState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Player_State.m_State);
			strBuf.Format("PlrState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Mon_State.m_State);
			strBuf.Format("MonState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Act_State.m_State);
			strBuf.Format("ActState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Con_State.m_State);
			strBuf.Format("ConState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Emotion_State.m_State);
			strBuf.Format("EmoState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Personal_State.m_State);
			strBuf.Format("PsnState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Help_State.m_State);
			strBuf.Format("HlpState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);

			GeCurrentStateString(buf,pMon->m_MonAI.Assist_State.m_State);
			strBuf.Format("AstState : %s", buf);
			m_trObject.InsertItem(strBuf, hParam2);


	//		m_trObject.Expand(hParam2, TVE_EXPAND);


		}
	}
	//*/
}

void CObjectTab::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	g_Main.m_GameMsg.PackingMsg(gm_interface_update_object);
	
	CPropertyPage::OnTimer(nIDEvent);
}

BOOL CObjectTab::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	m_btAuto.SetWindowText("Auto");
	g_Main.m_GameMsg.PackingMsg(gm_interface_update_object);
	
	return CPropertyPage::OnSetActive();
}

BOOL CObjectTab::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	return CPropertyPage::OnKillActive();
}

void CObjectTab::OnButtonUpdate() 
{
	// TODO: Add your control notification handler code here

	g_Main.m_GameMsg.PackingMsg(gm_interface_update_object);
}

void CObjectTab::OnButtonAuto() 
{
	// TODO: Add your control notification handler code here

	static bool bTurn = true;
	
	if(bTurn)
	{
		SetTimer(0, 1000, NULL);
		m_btAuto.SetWindowText("Abort");
	}
	else
	{
		KillTimer(0);
		m_btAuto.SetWindowText("Start");
	}

	bTurn = !bTurn;
}

void CObjectTab::OnButtonSearch() 
{
	// TODO: Add your control notification handler code here

	CObjectSearchDlg dlg;

	if(dlg.DoModal() == IDOK)
	{
		CGameObject* pObj = NULL;

		if(strlen(dlg.m_szCharName) > 0)
			pObj = g_Main.GetChar(dlg.m_szCharName);
		else
			pObj = g_Main.GetObject(&dlg.m_ID);

		if(pObj && pObj->m_bLive)
		{
			CGameObject::s_pSelectObject = pObj;
			if(g_pDoc->m_InfoSheet.GetActiveIndex() == CInfoSheet::OBJECT_TAB)
				g_Main.m_GameMsg.PackingMsg(gm_interface_update_object);

			if(g_Main.m_MapDisplay.m_bDisplayMode)
			{
				int index = g_Main.m_MapOper.GetMap(pObj->m_pCurMap);
				g_Main.m_GameMsg.PackingMsg(gm_interface_map_change, index);									
			}
		}
	}	
}

void CObjectTab::OnButtonDestory() 
{
	// TODO: Add your control notification handler code here

	g_Main.m_GameMsg.PackingMsg(gm_interface_UserExit);
}
