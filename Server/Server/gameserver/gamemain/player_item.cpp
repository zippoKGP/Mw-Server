#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "WeaponItemFld.h"
#include "PotionItemFld.h"
#include "DfnEquipItemFld.h"
#include "EtcItemFld.h"
#include "ErrorEventFromServer.h"

void CPlayer::pc_UsePotionItem(_STORAGE_POS_INDIV* pItem)
{
	BYTE byErrCode = 0; //1;//저장위치틀림 2;//없는아탬 3;//포션아님 4;//수량초과 5;//회복잠김상태 6;//이미 죽은상태 7;//유닛탑승중
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[pItem->byStorageCode];
	__ITEM* pUseItem = NULL;
	
	if(IsRidingUnit()) //유닛탑승중에는 회복불가
	{
		g_Main.m_logSystemError.Write("%s: pc_UsePotionItem.. 유닛탑승중", m_Param.GetCharName());
		byErrCode = 7;//유닛탑승중
		goto RESULT;
	}
	
	//존재확인
	pUseItem = pList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pUseItem)
	{
		SendMsg_AdjustAmountInform(pItem->byStorageCode, pItem->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_UsePotionItem.. 없는아탬(sd:%d, sr: %d)", m_Param.GetCharName(), pItem->byStorageCode, pItem->wItemSerial);
		byErrCode = 2;//없는아탬
		goto RESULT;
	}

	//생사확인..
	if(m_Param.GetHP() == 0)
	{
		byErrCode = 6;//이미 죽은상태
		goto RESULT;
	}

	//포션인지 확인
	if(pUseItem->m_byTableCode != tbl_code_potion)
	{
		g_Main.m_logSystemError.Write("%s: pc_UsePotionItem.. 포션아님", m_Param.GetCharName());
		byErrCode = 3;//포션아님
		goto RESULT;
	}

	if(m_EP.GetEff_State(_EFF_STATE::Rev_Lck))//######### PARAMETER EDIT (회복잠김)
	{
		g_Main.m_logSystemError.Write("%s: pc_UsePotionItem.. 회복잠김상태", m_Param.GetCharName());
		byErrCode = 5;//회복잠김상태
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//파라메타변경
		_PotionItem_fld* pFld = (_PotionItem_fld*)g_Main.m_tblItemData[tbl_code_potion].GetRecord(pUseItem->m_wItemIndex);

		struct EFFECT{
			int   nCode; 
			float fUnit; 
		};

		EFFECT* pEffect = (EFFECT*)&pFld->m_nEff1Code;
		bool bCont = false;//지속형..

		for(int i = 0; i < MAX_POTION_EFFECT_KIND; i++)
		{
			if(pEffect[i].nCode == 1)//hp
				SetHP(GetHP()+pEffect[i].fUnit, false);
			else if(pEffect[i].nCode == 2)//fp
				SetFP(GetFP()+pEffect[i].fUnit, false);
			else if(pEffect[i].nCode == 3)//sp
				SetSP(GetSP()+pEffect[i].fUnit, false);
			else if(pEffect[i].nCode == 4)//공격력증가
				m_fAFRate = pEffect[i].fUnit;
			else if(pEffect[i].nCode == 5)//방어력증가
				m_fDefRate = pEffect[i].fUnit;
		}

		if(pFld->m_nDuration > 0)
		{
			m_dwStartTime = timeGetTime();
			m_dwDurTime = pFld->m_nDuration;
		}

		//수량소모
		Emb_AlterDurPoint(pList->m_nListCode, pUseItem->m_byStorageIndex, -1, false);//소모
	}

	SendMsg_UsePotionResult(byErrCode, pItem->wItemSerial);
}

void CPlayer::pc_EquipPart(_STORAGE_POS_INDIV* pItem)
{
	BYTE byErrCode = 0;//1;//저장위치틀림 2;//없는아탬 3;//장착아이템아님 4;//사용할수없는종족 5;//방패들었음 6;//양손무기들었음 7;//레벨제한
	_STORAGE_LIST* pBagList = &m_Param.m_dbInven;
	__ITEM* pFixingItem = NULL;	
	char* pszCivilCode = NULL;
	_base_fld* pFixingFld = NULL;

	//존재확인
	pFixingItem = pBagList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pFixingItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_EquipPart.. 없는아탬(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
		byErrCode = 2;//없는아탬
		//REFRESH..	
		goto RESULT;
	}
	pFixingFld = g_Main.m_tblItemData[pFixingItem->m_byTableCode].GetRecord(pFixingItem->m_wItemIndex);

	//장착아이템인지 확인
	if(pFixingItem->m_byTableCode >= equip_fix_num)
	{
		g_Main.m_logSystemError.Write("%s: pc_EquipPart.. 장착아이템아님(tbl:%d)", m_Param.GetCharName(), pFixingItem->m_byTableCode);
		byErrCode = 3;//장착아이템아님
		goto RESULT;
	}

	//사용가능종족인지확인
	if(!::IsItemEquipCivil(pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex, m_Param.GetRaceSexCode()))
	{
		g_Main.m_logSystemError.Write("%s: pc_EquipPart.. 사용할수없는종족", m_Param.GetCharName());
		byErrCode = 4;//사용할수없는종족
		goto RESULT;
	}

	//능력제한
	if(!IsEffectableEquip(pFixingItem))
	{
	//	g_Main.m_logSystemError.Write("%s: pc_EquipPart.. 레벨제한(tbl:%d, idx:%d, upt:%s)", m_Param.GetCharName(), pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex, ::DisplayItemLv(pFixingItem->m_dwLv));
		byErrCode = 7;//레벨제한
		goto RESULT;
	}

	//양손무기일경우 방패있는지확인
	if(pFixingItem->m_byTableCode == tbl_code_weapon)
	{
		int nFixPart = ((_WeaponItem_fld*)pFixingFld)->m_nFixPart;
		if(nFixPart == handle_both && m_Param.m_dbEquip.m_pStorageList[tbl_code_shield].m_bLoad)
		{
			g_Main.m_logSystemError.Write("%s: pc_EquipPart.. 양손무기인데방패들었음", m_Param.GetCharName());
			byErrCode = 5;//양손무기인데방패들었음
			goto RESULT;
		}
	}

	//방패일경우 양손무기들고있는지 확인
	if(pFixingItem->m_byTableCode == tbl_code_shield)
	{
		__ITEM* pWpProp = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pWpProp->m_bLoad)
		{
			_WeaponItem_fld* pWpFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pWpProp->m_wItemIndex);
			if(pWpFld->m_nFixPart == handle_both)
			{
				g_Main.m_logSystemError.Write("%s: pc_EquipPart.. 방패인데양손무기들었음", m_Param.GetCharName());
				byErrCode = 6;//양손무기들었음
				goto RESULT;
			}
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM NewFixItem;
		memcpy(&NewFixItem, pFixingItem, sizeof(__ITEM));

		//가방을 비움
		Emb_DelStorage(pBagList->m_nListCode, pFixingItem->m_byStorageIndex, false);

		//장착되있다면 가방으로 이동시키고 비움
		__ITEM* pProp = &m_Param.m_dbEquip.m_pStorageList[NewFixItem.m_byTableCode];
		if(pProp->m_bLoad)
		{
			__ITEM ReleaseItem;
			memcpy(&ReleaseItem, pProp, sizeof(__ITEM));
			Emb_AddStorage(pBagList->m_nListCode, &ReleaseItem);
			Emb_DelStorage(_STORAGE_POS::EQUIP, pProp->m_byStorageIndex, true);
		}

		//장착시킴
		Emb_AddStorage(_STORAGE_POS::EQUIP, &NewFixItem);
	}

	SendMsg_EquipPartResult(byErrCode);
}

void CPlayer::pc_EmbellishPart(_STORAGE_POS_INDIV* pItem, WORD wChangeSerial)
{	
	BYTE byErrCode = 0;//1;//저장위치틀림 2;//없는아탬 3;//착용아이템아님 4;//사용할수없는종족 5;//공간없음 6;//바꾸려는것이틀림 7;//레벨제한
	_STORAGE_LIST* pBagList = &m_Param.m_dbInven;
	_STORAGE_LIST* pEmbelList = &m_Param.m_dbEmbellish;
	__ITEM* pFixingItem = NULL;	
	__ITEM* pFixedItem = NULL;	
	char* pszCivilCode = NULL;
	int nEqualNum = 0;
	int i;

	//존재확인
	pFixingItem = pBagList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pFixingItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. 없는아탬(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
		byErrCode = 2;//없는아탬
		//REFRESH..
		goto RESULT;
	}

	//착용아이템인지 확인
	if(pFixingItem->m_byTableCode != tbl_code_ring
		&& pFixingItem->m_byTableCode != tbl_code_amulet
		&& pFixingItem->m_byTableCode != tbl_code_bullet)
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. 착용아이템아님(tbl:%d)", m_Param.GetCharName(), pFixingItem->m_byTableCode);
		byErrCode = 3;//착용아이템아님
		goto RESULT;
	}

	//사용가능종족인지확인
	if(!::IsItemEquipCivil(pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex, m_Param.GetRaceSexCode()))
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. 사용할수없는종족", m_Param.GetCharName());
		byErrCode = 4;//사용할수없는종족
		goto RESULT;
	}

	//레벨체크..
	if(::GetItemEquipLevel(pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex) > m_Param.GetLevel()+m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up])
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. 레벨제한", m_Param.GetCharName());
		byErrCode = 7;//레벨제한
		goto RESULT;
	}

	//바꾸려는것이 같은건지확인
	if(wChangeSerial != 0xFFFF)
	{
		pFixedItem = pEmbelList->GetPtrFromSerial(wChangeSerial);
		if(!pFixedItem)
		{
			g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. 바꾸려는것이없는아탬(%d)", m_Param.GetCharName(), wChangeSerial);
			byErrCode = 2;//없는아탬
			//REFRESH..
			goto RESULT;
		}
		if(pFixedItem->m_byTableCode != pFixingItem->m_byTableCode)
		{
			g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. 바꾸려는것이 같은종류의아템이 아님(%d)", m_Param.GetCharName(), pFixingItem->m_byTableCode);
			byErrCode = 6;//바꾸려는것이틀림
			goto RESULT;
		}
	}

	//빈공간 있나 확인
	if(wChangeSerial == 0xFFFF)
	{
		if(pEmbelList->GetIndexEmptyCon() == __NSI)
		{
			g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. 공간없음", m_Param.GetCharName());
			byErrCode = 5;//공간없음
			//REFRESH..
			goto RESULT;
		}		
	}

	//두개이상 존재하는지 확인
	for(i = 0; i < embellish_fix_num; i++)
	{
		if(pEmbelList->m_pStorageList[i].m_bLoad && 
			pEmbelList->m_pStorageList[i].m_byTableCode == pFixingItem->m_byTableCode)
			nEqualNum++;	
	}
	if((wChangeSerial == 0xFFFF && nEqualNum > 1)	|| //이동
		 (wChangeSerial != 0xFFFF && nEqualNum > 2))	//교체
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. 같은것이 이미 2개있음", m_Param.GetCharName());
		byErrCode = 5;//공간없음
		//REFRESH..
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM NewFixItem;
		memcpy(&NewFixItem, pFixingItem, sizeof(__ITEM));

		//가방을 비움
		Emb_DelStorage(pBagList->m_nListCode, pFixingItem->m_byStorageIndex, false);

		//장착되있다면 가방으로 이동시키고 비움
		if(pFixedItem)
		{
			__ITEM FixedItem;
			memcpy(&FixedItem, pFixedItem, sizeof(__ITEM));
			Emb_AddStorage(pBagList->m_nListCode, &FixedItem);

			Emb_DelStorage(pEmbelList->m_nListCode, pFixedItem->m_byStorageIndex, false);
		}
			
		//장착시킴
		Emb_AddStorage(pEmbelList->m_nListCode, &NewFixItem);
	}

	SendMsg_EmbellishResult(byErrCode);	
}

void CPlayer::pc_OffPart(_STORAGE_POS_INDIV* pItem)
{
	BYTE byErrCode = 0;//1;//사용가방아님 2;//없는아템 3;//넣을빈칸없음
	_STORAGE_LIST* pEquList = NULL;
	_STORAGE_LIST* pBagList = &m_Param.m_dbInven;
	__ITEM* pReleaseItem = NULL;	

	//장착중인아이템이 있는지확인
	if(pItem->byStorageCode == _STORAGE_POS::EQUIP || pItem->byStorageCode == _STORAGE_POS::EMBELLISH)
		pEquList = m_Param.m_pStoragePtr[pItem->byStorageCode];
	else
	{
		g_Main.m_logSystemError.Write("%s: pc_OffPart.. 저장코드가 틀림(%d)", m_Param.GetCharName(), pItem->byStorageCode);
		byErrCode = 2;//없는아템
		goto RESULT;
	}
	pReleaseItem = pEquList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pReleaseItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_OffPart.. 없는아템(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
		byErrCode = 2;//없는아템
		//REFRESH..
		goto RESULT;
	}

	//가방에 빈곳이 있나 확인
	if(pBagList->GetIndexEmptyCon() == __NSI)
	{
		g_Main.m_logSystemError.Write("%s: pc_OffPart.. 빈칸없음", m_Param.GetCharName());
		byErrCode = 3;//넣을빈칸없음
		//REFRESH..
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM ReMoveItem;
		memcpy(&ReMoveItem, pReleaseItem, sizeof(__ITEM));

		//장착해제
		Emb_DelStorage(pEquList->m_nListCode, pReleaseItem->m_byStorageIndex, false);

		//가방에 삽입
		Emb_AddStorage(pBagList->m_nListCode, &ReMoveItem);
	}

	SendMsg_OffPartResult(byErrCode);
}

void CPlayer::pc_MakeItem(_STORAGE_POS_INDIV* pipMakeTool, WORD wManualIndex, BYTE byMaterialNum, _STORAGE_POS_INDIV* pipMaterials)
{
	BYTE byErrCode = 0;//1;//없는제작툴, 7;//새로추가할공간이없다. 5;//없는재료 4;//재료부족 6;//재료조합실패 100;//제작실패  7;//새로추가할공간이없다
	__ITEM* pToolProp = NULL;
	__ITEM* pMaterialProp[100];
	int i;
	CItemMakeDataTable::_material_list* pManual = g_Main.m_tblItemMakeData.GetRecord(wManualIndex);
	_ItemMakeData_fld* pManualFld = g_Main.m_tblItemMakeData.GetRecordFld(wManualIndex);
	DWORD dwPreserveKey = 0;
	int nMaterialStorageIndex[100];
	int nMKItemTableCode = ::GetItemTableCode(pManualFld->m_strCode);
	WORD wMKItemIndex = (WORD)(g_Main.m_tblItemData[nMKItemTableCode].GetRecord(pManualFld->m_strCode))->m_dwIndex;
	BYTE byMakeCode;

	//제작툴 존재 확인
	pToolProp = m_Param.m_dbInven.GetPtrFromSerial(pipMakeTool->wItemSerial);
	if(!pToolProp)
	{
		g_Main.m_logSystemError.Write("%s: pc_MakeItem.. 없는제작툴(%d)", m_Param.GetCharName(), pipMakeTool->wItemSerial);
		byErrCode = error_make_nothing_tool;//없는제작툴
		//REFRESH..
		goto RESULT;
	}
	if(pToolProp->m_byTableCode != tbl_code_maketool)
	{
		g_Main.m_logSystemError.Write("%s: pc_MakeItem.. 툴이아니다(%d)", m_Param.GetCharName(), pToolProp->m_byTableCode);
		byErrCode = error_make_not_tool;//툴이 아니다.
		goto RESULT;
	}

	//새로 추가할 가방에 빈곳이잇나 검사
	if(m_Param.m_dbInven.GetIndexEmptyCon() == __NSI)
	{
		g_Main.m_logSystemError.Write("%s: pc_MakeItem.. 새로추가할공간이없다", m_Param.GetCharName());
		byErrCode = error_make_full_inven;//새로추가할공간이없다	
		//REFRESH..
		goto RESULT;
	}

	//숙련도 확인
	switch(pManual->byItemTblCode)
	{
	case tbl_code_shield:
	case tbl_code_weapon:
		byMakeCode = mi_class_shield;
		break;
	case tbl_code_bullet:
		byMakeCode = mi_class_bullet;
		break;
	default:
		byMakeCode = mi_class_armor;
		break;
	}

	//재료확인	
	for(i = 0; i < byMaterialNum; i++)
	{
		pMaterialProp[i] = m_Param.m_dbInven.GetPtrFromSerial(pipMaterials[i].wItemSerial);
		if(!pMaterialProp[i])
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pipMaterials[i].wItemSerial, 0);//REFRESH..
			g_Main.m_logSystemError.Write("%s: pc_MakeItem.. 없는재료(%d)", m_Param.GetCharName(), pipMaterials[i].wItemSerial);

			byErrCode = error_make_nothing_material;//없는재료
			goto RESULT;
		}
		if(pipMaterials[i].byNum > pMaterialProp[i]->m_dwDur)
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pipMaterials[i].wItemSerial, pMaterialProp[i]->m_dwDur);//REFRESH..
			g_Main.m_logSystemError.Write("%s: pc_MakeItem.. 재료부족(%d>%d)", m_Param.GetCharName(), pipMaterials[i].byNum, pMaterialProp[i]->m_dwDur);

			byErrCode = error_make_lack_material;//재료부족
			goto RESULT;
		}
		//중복확인
		for(int a = 0; a < i; a++)
		{
			if(pipMaterials[a].wItemSerial == pipMaterials[i].wItemSerial)
			{
				g_Main.m_logSystemError.Write("%s: pc_MakeItem.. 재료중복", m_Param.GetCharName());
				byErrCode = error_make_lack_material;//재료부족
				goto RESULT;
			}
		}

		dwPreserveKey += ((pMaterialProp[i]->m_byTableCode+1) * (pMaterialProp[i]->m_wItemIndex+1) * pipMaterials[i].byNum);				
		nMaterialStorageIndex[i] = pMaterialProp[i]->m_byStorageIndex;
	}

	//보안키검사
	if(dwPreserveKey != pManual->dwPreserveKey)
	{
		g_Main.m_logSystemError.Write("%s: pc_MakeItem.. 재료조합실패", m_Param.GetCharName());

		byErrCode = error_make_mix_fail;//재료조합실패	
		goto RESULT;
	}
	
	//가능종족확인
//	if(pManualFld->m_strCivil[m_Param.GetRaceCode()] != 1)
//	{
//		byErrCode = 8;//제작가능족종이아님	
//		goto RESULT;
//	}

RESULT:

	if(byErrCode == 0)
	{	
		__ITEM MaterialCopy[100];
		BYTE  byMtrNumCopy[100];

		for(i = 0; i < byMaterialNum; i++)
		{
			memcpy(&MaterialCopy[i], pMaterialProp[i], sizeof(__ITEM));
			byMtrNumCopy[i] = pipMaterials[i].byNum;
		}

		int nSkillProp = m_pmMst.GetMasteryPerMast(mastery_code_make, byMakeCode);
		int nSuccRate = 0;
		int nStdLv, nItmLv;
		__ITEM NewItem;
		 
		if(pManual->byItemTblCode != tbl_code_bullet)
		{
			//기준레벨 = INT(SQRT(제작숙련도*150+1300)/2+0.5)-18
			nStdLv = sqrt(nSkillProp*150+1300.0f)/2+0.5f - 18;
			//아이템제작성공확률 = 0.5-(((아이템레벨-기준레벨)*11.25)*0.01)
			nItmLv = ::GetItemEquipLevel(nMKItemTableCode, wMKItemIndex);

			nSuccRate = 50 - (nItmLv-nStdLv) * 11.25f;
		}
		else	//발사물은 따로..
		{
			//발사물제작성공확률 = INT(40+(발사물제작숙련도*50))
			nSuccRate = 40+(nSkillProp*0.5f);
		}

		nSuccRate = min(nSuccRate, 95);
		nSuccRate = max(nSuccRate, 0);

		if(m_bCheat_100SuccMake)//100%성공치트..
			nSuccRate = 100;

		if(nSuccRate < ::rand()%100)
		{
			byErrCode = miss_make_random;//제작실패
		}

		//재료소모시킴
		for(i = 0; i < byMaterialNum; i++)
		{
			Emb_AlterDurPoint(_STORAGE_POS::INVEN, nMaterialStorageIndex[i], -pipMaterials[i].byNum, true); //제작
		}

		//툴아이템 내구성 소모
		__ITEM* pToolCon = &m_Param.m_pStoragePtr[_STORAGE_POS::INVEN]->m_pStorageList[pToolProp->m_byStorageIndex];
		DWORD dwLeftToolDur = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pToolProp->m_byStorageIndex, -1, false); //소모
		if(dwLeftToolDur == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pToolCon, m_szItemHistoryFileName);
		}

		if(byErrCode == 0)
		{
			WORD wNewSerial;
			if(byErrCode == 0)
			{
				//새로운아이템추가
				wNewSerial = m_Param.GetNewItemSerial();
				
				NewItem.m_byTableCode = pManual->byItemTblCode;
				NewItem.m_wItemIndex = pManual->wItemIndex;
				NewItem.m_dwDur = ::GetItemDurPoint(pManual->byItemTblCode, pManual->wItemIndex);

				//업그레이드가능 소켓수는 디폴트값 랜덤으로..
				BYTE byMaxLv = ::GetDefItemUpgSocketNum(pManual->byItemTblCode, pManual->wItemIndex);
				BYTE byLimSocketNum = byMaxLv > 0 ? 1+::rand()%byMaxLv : 0;

				NewItem.m_dwLv = ::GetBitAfterSetLimSocket(byLimSocketNum);
				NewItem.m_wSerial = wNewSerial;

				Emb_AddStorage(_STORAGE_POS::INVEN, &NewItem);

				//마스터리 누적..
				if(pManual->byItemTblCode != tbl_code_bullet)
				{
					if(nStdLv - nItmLv <= 4)
						Emb_AlterStat(mastery_code_make, byMakeCode, 1);
				}
				else
					Emb_AlterStat(mastery_code_make, byMakeCode, 1);

				//아이템생성 보내기..
				SendMsg_InsertItemInform(_STORAGE_POS::INVEN, &NewItem);
			}
		}

		//ITEM HISTORY..
		s_MgrItemHistory.make_item(MaterialCopy, byMtrNumCopy, byMaterialNum, byErrCode, &NewItem, m_szItemHistoryFileName);
	}

	SendMsg_MakeItemResult(byErrCode);
}

void CPlayer::pc_UpgradeItem(_STORAGE_POS_INDIV* pposTalik, _STORAGE_POS_INDIV* pposToolItem, _STORAGE_POS_INDIV* pposUpgItem, BYTE byJewelNum, _STORAGE_POS_INDIV* pposUpgJewel)
{
	BYTE byErrCode = 0;//1;//없는아이템 2;//업글만땅했음 3;//업글안돼는 아이템 5;//더이상해당탈릭을추가할수없음 6;//이아이템에는해당탈릭을추가할수없음 7;//이미 내성탈릭이 박힌 아이템 100;//랜덤실패, 101;//기존탈릭파괴 102;	//아이템분실..
	int i;
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[pposUpgItem->byStorageCode];
	__ITEM* pTalikProp = NULL;
	__ITEM* pToolProp = NULL;
	__ITEM* pDstItemProp = NULL;
	__ITEM* pJewelProp[upgrade_jewel_num];
	_ItemUpgrade_fld* pTalik = NULL;
	_ItemUpgrade_fld* pJewelFld[upgrade_jewel_num];
	memset(pJewelFld, NULL, sizeof(_ItemUpgrade_fld*)*upgrade_jewel_num);

	//대상아이템소지확인
	pDstItemProp = pList->GetPtrFromSerial(pposUpgItem->wItemSerial);
	if(!pDstItemProp)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 없는대상아이템(sr:%d)", m_Param.GetCharName(), pposUpgItem->wItemSerial);
		byErrCode = error_upgrade_nothing_item;//없는아이템
		//REFRESH..
		goto RESULT;
	}

	//업그레이드가능검사
	if(::GetItemKindCode(pDstItemProp->m_byTableCode) != item_kind_std)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 업글안돼는 아이템(tbl:%d)", m_Param.GetCharName(), pDstItemProp->m_byTableCode);
		byErrCode = error_upgrade_not_able;//업글안돼는 아이템
		goto RESULT;
	}
	if(::GetDefItemUpgSocketNum(pDstItemProp->m_byTableCode, pDstItemProp->m_wItemIndex) == 0)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 업글안돼는 아이템", m_Param.GetCharName());
		byErrCode = error_upgrade_not_able;//업글안돼는 아이템
		goto RESULT;
	}
	if(::GetItemUpgedLv(pDstItemProp->m_dwLv) >= ::GetItemUpgLimSocket(pDstItemProp->m_dwLv))
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 업글만땅했음", m_Param.GetCharName());
		byErrCode = error_upgrade_full_upgrade;//업글만땅했음
		goto RESULT;
	}

	//탈릭소지확인
	pTalikProp = m_Param.m_dbInven.GetPtrFromSerial(pposTalik->wItemSerial);
	if(!pTalikProp)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposTalik->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 없는탈릭(sr:%d)", m_Param.GetCharName(), pposTalik->wItemSerial);
		byErrCode = error_upgrade_nothing_talik; //없는아이템
		goto RESULT;
	}
	if(pTalikProp->m_byTableCode != tbl_code_res)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 보석아닌탈릭(%d)", m_Param.GetCharName(), pTalikProp->m_byTableCode);
		byErrCode = error_upgrade_not_talik;//탈릭이 아니다.
		goto RESULT;
	}
	pTalik = g_Main.m_tblItemUpgrade.GetRecordFromRes(pTalikProp->m_wItemIndex);	
	if(!pTalik)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. m_tblItemUpgrade.. 없는탈릭레코드", m_Param.GetCharName());
		byErrCode = error_upgrade_not_talik;//탈릭이 아니다.
		goto RESULT;
	}
	if(pTalik->m_dwIndex >= MAX_TALIK_NUM)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 탈릭이 아님 MAX_TALIK_NUM을 초과 (%s)", m_Param.GetCharName(), pTalik->m_strCode);
		byErrCode = error_upgrade_not_talik;//탈릭이 아니다.
		goto RESULT;
	}

	//툴소지확인
	pToolProp = m_Param.m_dbInven.GetPtrFromSerial(pposToolItem->wItemSerial);
	if(!pToolProp)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposToolItem->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 없는툴(sr:%d)", m_Param.GetCharName(), pposToolItem->wItemSerial);
		byErrCode = error_upgrade_nothing_tool;//툴이 없다
		goto RESULT;
	}
	if(pToolProp->m_byTableCode != tbl_code_maketool)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 툴이아니다(%d)", m_Param.GetCharName(), pToolProp->m_byTableCode);
		byErrCode = error_upgrade_not_tool;//툴이 아니다.
		goto RESULT;
	}

	//보석소지확인
	for(i = 0; i < byJewelNum; i++)
	{
		pJewelProp[i] = m_Param.m_dbInven.GetPtrFromSerial(pposUpgJewel[i].wItemSerial);
		if(!pJewelProp[i])
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposUpgJewel[i].wItemSerial, 0);//REFRESH..
			g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 없는보석(sr:%d)", m_Param.GetCharName(), pposUpgJewel[i].wItemSerial);
			byErrCode = error_upgrade_nothing_jewel;//없는아이템
			goto RESULT;
		}
		if(pJewelProp[i]->m_byTableCode != tbl_code_res)
		{
			g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 보석아닌버석(%d)", m_Param.GetCharName(), pJewelProp[i]->m_byTableCode);
			byErrCode = error_upgrade_not_jewel;//없는아이템
			goto RESULT;
		}
		pJewelFld[i] = g_Main.m_tblItemUpgrade.GetRecordFromRes(pJewelProp[i]->m_wItemIndex);
		if(!pJewelFld[i])
		{
			g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 없는보석레코드(%d)", m_Param.GetCharName(), pJewelProp[i]->m_wItemIndex);
			return;
		}
		//중복확인
		for(int a = 0; a < i; a++)
		{
			if(pJewelProp[a] == pJewelProp[i])
			{
				g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 중복된 보석");
				byErrCode = error_upgrade_not_jewel;//없는아이템
				goto RESULT;
			}
		}
	}

	if(!::IsAddAbleTalikToItem(pDstItemProp->m_byTableCode, pDstItemProp->m_wItemIndex, pDstItemProp->m_dwLv, pTalik->m_dwIndex, &pTalik->m_nUpperUp)) 
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. 더이상해당탈릭을추가할수없음(sr:%d, tkcode:%d)", m_Param.GetCharName(), pDstItemProp->m_wSerial, pTalik->m_dwIndex);
		byErrCode = error_upgrade_full_tailk;//더이상해당탈릭을추가할수없음
		goto RESULT;
	}

RESULT:
	
	if(byErrCode == 0)
	{
		__ITEM ItemCopy;
		__ITEM TalikCopy;
		__ITEM JewelCopy[upgrade_jewel_num];

		//일단카피..
		memcpy(&ItemCopy, pDstItemProp, sizeof(__ITEM));
		memcpy(&TalikCopy, pTalikProp, sizeof(__ITEM));
		for(i = 0; i < byJewelNum; i++)			
			memcpy(&JewelCopy[i], pJewelProp[i], sizeof(__ITEM));


		//탈릭 보석 삭제
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pTalikProp->m_byStorageIndex, -1, true);//업글
		for(i = 0; i < byJewelNum; i++)			
			Emb_AlterDurPoint(_STORAGE_POS::INVEN, pJewelProp[i]->m_byStorageIndex, -1, true);//업글
	
		//업글연장내구성소모
		__ITEM* pToolCon = &m_Param.m_pStoragePtr[_STORAGE_POS::INVEN]->m_pStorageList[pToolProp->m_byStorageIndex];
		DWORD dwLeftToolDur = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pToolProp->m_byStorageIndex, -1, false);//소모
		if(dwLeftToolDur == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pToolCon, m_szItemHistoryFileName);
		}

		//아아템업글
		float fRate = 0.0f;
		for(i = 0; i < upgrade_jewel_num; i++)
		{
			if(!pJewelFld[i])
				fRate += 0.125;
			else
				fRate += pJewelFld[i]->m_fJewelFieldValue;
		}

		BYTE byLv = ::GetItemUpgedLv(pDstItemProp->m_dwLv);

		DWORD dwTotalRate = (s_dwItemUpgSucRate[byLv]*fRate/upgrade_jewel_num)*1000;

		DWORD dwR1 = ::rand();
		DWORD dwRand = (dwR1<<16)+::rand();

		if(m_bCheat_100SuccMake)//100%성공치트..
			dwTotalRate = 0xFFFFFFFF;
		
		if(dwTotalRate <= dwRand%100000)//실패시..
		{
			byErrCode = miss_upgrade_random;//랜덤실패

			//기존탈릭분실..
			bool bTalikBreak = false;
			switch(byLv)
			{
			case 5:
				if(125 > ::rand()%1000)
					bTalikBreak = true;
				break;
			case 6:
				if(250 > ::rand()%1000)
					bTalikBreak = true;
				break;
			case 7:
				if(500 > ::rand()%1000)
					bTalikBreak = true;
				break;
			}                         
			if(bTalikBreak)//모든탈릭이 파괴돼어 업그레이드가 0이된다
			{                          
				Emb_ItemUpgrade(item_upgrade_init, pposUpgItem->byStorageCode, pDstItemProp->m_byStorageIndex, 0);
									   	
				byErrCode = miss_upgrade_destroy_talik;	 //레벨 0으로..101;//기존탈릭파괴
			}
			else
			{
				//아이템분실..
				bool bItemBreak = false;
				switch(byLv)
				{
				case 5:
					if(125 > ::rand()%1000)
						bItemBreak = true;
					break;
				case 6:
					if(250 > ::rand()%1000)
						bItemBreak = true;
					break;
				case 7:
					if(500 > ::rand()%1000)
						bItemBreak = true;
					break;
				}

				if(bItemBreak)
				{
					Emb_DelStorage(pList->m_nListCode, pDstItemProp->m_byStorageIndex, false);
					byErrCode = miss_upgrade_destroy_item;	//아이템분실..
				}
			}
		}
		else
		{
			//업글성공
			DWORD dwUpt = ::GetBitAfterUpgrade(pDstItemProp->m_dwLv, pTalik->m_dwIndex, byLv);
			Emb_ItemUpgrade(item_upgrade_up, pList->m_nListCode, pDstItemProp->m_byStorageIndex, dwUpt);
		}

		//ITEM HISTORY..
		s_MgrItemHistory.grade_up_item(&ItemCopy, &TalikCopy, JewelCopy, byJewelNum, byErrCode, pDstItemProp->m_dwLv, m_szItemHistoryFileName);
	}

	SendMsg_ItemUpgrade(byErrCode);
}

void CPlayer::pc_DowngradeItem(_STORAGE_POS_INDIV* pposTalik, _STORAGE_POS_INDIV* pposToolItem, _STORAGE_POS_INDIV* pposUpgItem)
{
	BYTE byErrCode = 0;//1;//없는탈릭 2;//잘못됀탈릭 3;//없는툴 4;//없는아이템 5;//업글안돼는 아이템 6;//업글됀아템아님
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[pposUpgItem->byStorageCode];
	__ITEM* pTalikProp = NULL;
	__ITEM* pToolProp = NULL;
	__ITEM* pDstItemProp = NULL;
	_ItemUpgrade_fld* pTalik = NULL;
	_base_fld* pItemFld = NULL;

	//대상아이템소지확인
	pDstItemProp = pList->GetPtrFromSerial(pposUpgItem->wItemSerial);
	if(!pDstItemProp)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 없는대상아이템(sr:%d)", m_Param.GetCharName(), pposUpgItem->wItemSerial);
		byErrCode = error_upgrade_nothing_item;//없는아이템
		goto RESULT;
	}

	//업그레이드가 가능한 아이템인지확인
	if(::GetItemKindCode(pDstItemProp->m_byTableCode) != item_kind_std)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 업글안돼는 아이템(tbl:%d)", m_Param.GetCharName(), pDstItemProp->m_byTableCode);
		byErrCode = error_upgrade_not_able;//업글안돼는 아이템
		goto RESULT;
	}
	if(::GetDefItemUpgSocketNum(pDstItemProp->m_byTableCode, pDstItemProp->m_wItemIndex) == 0)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 업글안돼는 아이템", m_Param.GetCharName());
		byErrCode = error_upgrade_not_able;//업글안돼는 아이템
		goto RESULT;
	}
	if(::GetItemUpgedLv(pDstItemProp->m_dwLv) == 0)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 업글됀아템아님", m_Param.GetCharName());
		byErrCode = error_upgrade_none_upgrade;//업글됀아템아님
		goto RESULT;
	}

	//탈릭소지확인
	pTalikProp = m_Param.m_dbInven.GetPtrFromSerial(pposTalik->wItemSerial);
	if(!pTalikProp)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposTalik->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 없는탈릭(sr:%d)", m_Param.GetCharName(), pposTalik->wItemSerial);
		byErrCode = error_upgrade_nothing_talik;//없는탈릭
		goto RESULT;
	}
	if(pTalikProp->m_byTableCode != tbl_code_res)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 보석아닌탈릭(sr:%d)", m_Param.GetCharName(), pTalikProp->m_byTableCode);
		byErrCode = error_upgrade_not_talik;//없는탈릭
		goto RESULT;
	}
	pTalik = g_Main.m_tblItemUpgrade.GetRecordFromRes(pTalikProp->m_wItemIndex);
	if(!pTalik)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 레코드에없는탈릭(%d)", m_Param.GetCharName(), pTalikProp->m_wItemIndex);
		byErrCode = error_upgrade_not_talik;//잘못됀탈릭
		goto RESULT;
	}
	if(pTalik->m_nDataEffect != talik_type_down)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 부활의탈릭아님", m_Param.GetCharName());
		byErrCode = error_upgrade_not_talik;//잘못됀탈릭
		goto RESULT;
	}

	//툴소지확인
	pToolProp = m_Param.m_dbInven.GetPtrFromSerial(pposToolItem->wItemSerial);
	if(!pToolProp)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposToolItem->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 없는툴(sr:%d)", m_Param.GetCharName(), pposToolItem->wItemSerial);
		byErrCode = error_upgrade_nothing_tool;//없는툴
		goto RESULT;
	}
	if(pToolProp->m_byTableCode != tbl_code_maketool)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. 툴이아닌툴(%d)", m_Param.GetCharName(), pToolProp->m_byTableCode);
		byErrCode = error_upgrade_not_tool;//없는툴
		goto RESULT;
	}

RESULT:

	if(byErrCode== 0)
	{		
		__ITEM ItemCopy;

		memcpy(&ItemCopy, pDstItemProp, sizeof(__ITEM));

		//탈릭삭제
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pTalikProp->m_byStorageIndex, -1, true);//업글

		//업글연장내구성소모
		__ITEM* pToolCon = &m_Param.m_pStoragePtr[_STORAGE_POS::INVEN]->m_pStorageList[pToolProp->m_byStorageIndex];
		DWORD dwLeftToolDur = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pToolProp->m_byStorageIndex, -1, false);//소모
		if(dwLeftToolDur == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pToolCon, m_szItemHistoryFileName);
		}

		BYTE byLv = ::GetItemUpgedLv(pDstItemProp->m_dwLv);
		DWORD dwUpt = ::GetBitAfterDowngrade(pDstItemProp->m_dwLv, byLv);
		Emb_ItemUpgrade(item_upgrade_down, pList->m_nListCode, pDstItemProp->m_byStorageIndex, dwUpt);

		//ITEM HISTORY..
		s_MgrItemHistory.grade_down_item(&ItemCopy, pTalikProp, pDstItemProp->m_dwLv, m_szItemHistoryFileName);
	}

	SendMsg_ItemDowngrade(byErrCode);
}

void CPlayer::pc_TakeLootingItem(CItemBox* pBox, WORD wAddSerial)
{
	BYTE byErrCode = 0;//1;//빈공간없음 2;//기존아템없음 3;//만땅임 4;//없는박스 5;//권한제한 6;//거리제한 7;//포캘수없는아이템 8;//유닛은 주울수없다
	WORD wSerial = 0;
	__ITEM BoxItem;
	__ITEM* pExisting = NULL;

	//박스가 존재하는지..
	if(!pBox->m_bLive)
	{
	//	g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. 없는박스", m_Param.GetCharName());
		byErrCode = 4;//없는박스
		goto RESULT;
	}

	//먹기권한있는지..
	if(!pBox->IsTakeRight(this))
	{
		g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. 권한제한", m_Param.GetCharName());
		byErrCode = 5;//권한제한
		goto RESULT;
	}
	if(pBox->m_byThrowerDegree != 0xFF)//버린물건일경우 같은 등급인지..
	{
		if(pBox->m_byThrowerDegree != m_byUserDgr)
		{
			byErrCode = 5;//권한제한
			goto RESULT;
		}
	}

	//거리제한.
	if(itembox_dist_lootable < ::GetSqrt(pBox->m_fCurPos, m_fCurPos))
	{
		g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. 거리제한", m_Param.GetCharName());
		byErrCode = 6;//거리제한
		goto RESULT;
	}
	memcpy(&BoxItem, &pBox->m_Item, sizeof(__ITEM));

	//유닛인지 체크..
	if(BoxItem.m_byTableCode == tbl_code_key)
	{
		g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. 유닛을 주울수없음", m_Param.GetCharName());
		byErrCode = 8;//유닛은 주울수없다
		goto RESULT;
	}

	//새로운추가라면 빈 공간이 있는지.. 기존추가라면 기존시리얼이 있는지 확인..
	if(wAddSerial == 0xFFFF)
	{
		if(m_Param.m_dbInven.GetIndexEmptyCon() == __NSI)
		{
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. 빈공간없음", m_Param.GetCharName());
			byErrCode = 1;//빈공간없음
			goto RESULT;
		}
	}
	else
	{
		if(!::IsOverLapItem(BoxItem.m_byTableCode))
		{
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. 포캘수없는아이템", m_Param.GetCharName());
			byErrCode = 7;//포캘수없는아이템
			goto RESULT;
		}
		pExisting = m_Param.m_dbInven.GetPtrFromSerial(wAddSerial);
		if(!pExisting)
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, wAddSerial, 0);//REFRESH..
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem..  포갤때기존아템없음", m_Param.GetCharName());
			byErrCode = 2;//기존아템없음
			goto RESULT;
		}

		if(pExisting->m_byTableCode != BoxItem.m_byTableCode || 
			pExisting->m_wItemIndex != BoxItem.m_wItemIndex)
		{
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem..  포갤때같은아템아님", m_Param.GetCharName());
			byErrCode = 2;//기존아템없음
			goto RESULT;
		}
		if(pExisting->m_dwDur+BoxItem.m_dwDur > max_overlap_num) 
		{			
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, wAddSerial, pExisting->m_dwDur);//REFRESH..	
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem..  포갤때같은만땅", m_Param.GetCharName());
			byErrCode = 3;//만땅임
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		//ITEM HISTORY..
		s_MgrItemHistory.take_ground_item(&BoxItem, pBox->m_dwThrowerCharSerial, m_szItemHistoryFileName);

		if(wAddSerial == 0xFFFF)	//새로운 스토리지 추가..
		{
			BoxItem.m_wSerial = m_Param.GetNewItemSerial();
			Emb_AddStorage(_STORAGE_POS::INVEN, &BoxItem);
		}
		else
		{
			Emb_AlterDurPoint(_STORAGE_POS::INVEN, pExisting->m_byStorageIndex, BoxItem.m_dwDur, true);//루팅
		}

		pBox->Destroy();
	}

	if(wAddSerial == 0xFFFF)
		SendMsg_TakeNewResult(byErrCode, &BoxItem);
	else	
		SendMsg_TakeAddResult(byErrCode, pExisting);	//포개지는 아이템..
}

void CPlayer::pc_ThrowStorageItem(_STORAGE_POS_INDIV* pItem)
{
	BYTE byErrCode = 0;// 1;//저장위치틀림 2;//없는아탬 3;//수량초과 4;//버리기금지품목
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[pItem->byStorageCode];
	__ITEM* pDropItem = NULL;

	//존재확인
	pDropItem = pList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pDropItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_ThrowStorageItem..  없는아탬(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
		byErrCode = 2;//없는아탬
		goto RESULT;
	}

	//락걸린 아이템인지..
	if(pDropItem->m_bLock)
	{
		byErrCode = 9;//락걸린 아이템
		goto RESULT;
	}

	//포션, 광물, 자원이라면 수량확인
	if(::IsOverLapItem(pDropItem->m_byTableCode))
	{
		if(pItem->byNum > pDropItem->m_dwDur)
		{
			SendMsg_AdjustAmountInform(pItem->byStorageCode, pItem->wItemSerial, pDropItem->m_dwDur);//REFRESH..	
			g_Main.m_logSystemError.Write("%s: pc_ThrowStorageItem..  수량초과(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
			byErrCode = 3;//수량초과
			goto RESULT;
		}
	}

	//버리기 금지 품목인지..
	if(pDropItem->m_byTableCode == tbl_code_key)
	{
		byErrCode = 4;//버리기금지품목
		goto RESULT;
	}
	if(!::IsGroundableItem(pDropItem->m_byTableCode, pDropItem->m_wItemIndex))
	{
		byErrCode = 4;//버리기금지품목
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//드롭될 아이템작성
		__ITEM ThrowItem;
		memcpy(&ThrowItem, pDropItem, sizeof(__ITEM));
		if(::IsOverLapItem(pDropItem->m_byTableCode))
			ThrowItem.m_dwDur = pItem->byNum;	

		bool bSucc = true;
	//	if(m_byUserDgr == USER_DEGREE_STD) dlatl..
		{//일반등급만이 아이템박스를 만든다..	
			bSucc = ::CreateItemBox(&ThrowItem, NULL, this, itembox_create_code_throw,
							 m_pCurMap, m_wMapLayerIndex, m_fCurPos, false);
		}

		if(bSucc)
		{
			//포션, 광물, 자원은 수량감소..
			if(::IsOverLapItem(pDropItem->m_byTableCode))
			{
				Emb_AlterDurPoint(pList->m_nListCode, pDropItem->m_byStorageIndex, -pItem->byNum, true);//버림
			}
			else		//일반아이템은 저장삭제
			{
				Emb_DelStorage(pList->m_nListCode, pDropItem->m_byStorageIndex, false);
			}

			//ITEM HISTORY..
			s_MgrItemHistory.throw_ground_item(&ThrowItem, m_szItemHistoryFileName);
		}
		else
		{	//빈 아이템박스있는지 확인
			g_Main.m_logSystemError.Write("%s: pc_ThrowStorageItem..  박스모자름", m_Param.GetCharName());
			byErrCode = 4;//박스없다.
		}
	}

	SendMsg_ThrowStorageResult(byErrCode);
}

void CPlayer::pc_AddBag(WORD wBagItemSerial)
{
	BYTE byErrCode = 0;//0;//성공, 1;//없는가방 2;//5개다쓰고있음
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[_STORAGE_POS::INVEN];
	__ITEM* pBagItem = NULL;
	_EtcItem_fld* pFld = NULL;

	//소지중인지 검사
	pBagItem = pList->GetPtrFromSerial(wBagItemSerial);
	if(!pBagItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_AddBag..  없는가방(sr:%d)", m_Param.GetCharName(), wBagItemSerial);
		byErrCode = 1;//없는가방
		goto RESULT;
	}
	//가방인지 검사..
	if(pBagItem->m_byTableCode != tbl_code_bag)
	{
		g_Main.m_logSystemError.Write("%s: pc_AddBag..  가방아닌가방", m_Param.GetCharName());
		byErrCode = 1;//없는가방
		goto RESULT;
	}	

	//사용중인 가방수 검사
	if(m_Param.GetBagNum() >= max_bag_num)
	{
		g_Main.m_logSystemError.Write("%s: pc_AddBag..  맥스넘음", m_Param.GetCharName());
		byErrCode = 2;//5개다쓰고있음
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//가방수 늘리기..
		int nCurBagNum = m_Param.GetBagNum();
		m_Param.SetBagNum(nCurBagNum+1);

		//인벤토리 사용가능리스트 늘리기
		pList->SetUseListNum(m_Param.GetBagNum()*one_bag_store_num);

		//가방삭제
		Emb_DelStorage(pList->m_nListCode, pBagItem->m_byStorageIndex, false);		

		//ITEM HISTORY..
		s_MgrItemHistory.consume_del_item(pBagItem, m_szItemHistoryFileName);

		//월드에 업데이트..	
		if(m_pUserDB)
			m_pUserDB->Update_BagNum(m_Param.GetBagNum());	
	}

	SendMsg_AddBagResult(byErrCode);
}

