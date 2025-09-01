#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "WeaponItemFld.h"
#include "OreItemFld.h"
#include "ResourceItemFld.h"
#include "ErrorEventFromServer.h"

void CPlayer::pc_MineStart(BYTE byMineIndex)
{
	BYTE byErrCode = 0;
	_WeaponItem_fld* pFld = NULL;
	__ITEM* pEquipProp = NULL;
	_res_dummy* pDummy = NULL;
	int nSector = -1;

	//자원더미체크
	if(byMineIndex >= m_pCurMap->m_nResDumNum)
	{
		byErrCode = error_mining_not_dummy;//없는채굴더미
		goto RESULT;
	}
	pDummy = &m_pCurMap->m_pResDummy[byMineIndex];

	//채굴모드인지 확인
	if(m_dwMineDelayTime != 0xFFFFFFFF)
	{
		if(m_dwMineStartTime+m_dwMineDelayTime > timeGetTime())
		{
			byErrCode = error_mining_in_motion;//채굴모드인
			goto RESULT;
		}
	}

	//더미범위 체크
	nSector = m_pCurMap->GetResDummySector(byMineIndex, m_fCurPos);
	if(nSector == -1)
	{
		byErrCode = error_mining_out_dummy;//채굴더미를 벗어남
		goto RESULT;
	}

	//채굴장비 장착 확인
	pEquipProp = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
	if(!pEquipProp->m_bLoad)
	{
		byErrCode = error_mining_nothing_equip;//채굴장비 에러
		goto RESULT;
	}

	pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon].m_wItemIndex);
	if(pFld->m_nType != type_mine)
	{
		byErrCode = error_mining_not_tool;//채굴장비 에러
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//지연시간체크
		m_dwMineDelayTime = m_pCurMap->m_pResDummy[byMineIndex].GetDelay(nSector);
		m_dwMineStartTime = timeGetTime();
		m_bMineMode = true;
	}

	//메시지 전송
	SendMsg_MineStartResult(byErrCode);
}

void CPlayer::pc_MineCancle(WORD wBatterySerial)
{
	BYTE byErrCode = 0;
	DWORD dwCurTime = timeGetTime();
	__ITEM* pEquipProp = NULL;
	__ITEM* pBatteryItem = NULL;
	_WeaponItem_fld* pFld = NULL;
	int nGap = m_dwMineDelayTime-(dwCurTime-m_dwMineStartTime);

	//채굴지연시간내인지 확인
	if(nGap <= 0 || m_dwMineDelayTime == 0xFFFFFFFF)
	{
		byErrCode = error_mining_over_delay; //지연시간에러
		goto RESULT;
	}

	//채굴장비 장착 확인
	pEquipProp = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
	if(!pEquipProp->m_bLoad)
	{
		byErrCode = error_mining_nothing_equip;//채굴장비에러
		goto RESULT;
	}
	pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon].m_wItemIndex);
	if(pFld->m_nType != type_mine)
	{
		byErrCode = error_mining_not_tool;//채굴장비 에러
		goto RESULT;
	}

	//베터리확인
	pBatteryItem = m_Param.m_dbInven.GetPtrFromSerial(wBatterySerial);
	if(!pBatteryItem)
	{
		byErrCode = error_mining_nothing_battery; //바테리 에러
		goto RESULT;
	}

RESULT:

	DWORD dwBatteryLeftDurPoint = 0;
	if(byErrCode == 0)
	{
		//채굴장비의 내구성 감소
	//	Emb_AlterDurPoint(_STORAGE_POS::EQUIP, tbl_code_weapon, -1);//dlatl..

		//베테리 용량 감소
		DWORD dwConsum = dwCurTime-m_dwMineStartTime;
		_STORAGE_LIST* pList = pBatteryItem->m_pInList;
		dwBatteryLeftDurPoint = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pBatteryItem->m_byStorageIndex, -(dwConsum/1000), false);//소모
		if(dwBatteryLeftDurPoint == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pBatteryItem, m_szItemHistoryFileName);
		}
	}

	//지연시간 초기화
	m_dwMineDelayTime = 0xFFFFFFFF;
	m_bMineMode = false;

	SendMsg_MineCancle(byErrCode, dwBatteryLeftDurPoint);
}

void CPlayer::pc_MineComplete(BYTE byOreIndex, WORD wBatterySerial)
{
	BYTE byErrCode = 0;
	int i;
	DWORD dwBatteryLeftDurPoint;
	_OreItem_fld* pFld;
	int nStorageIndex = __NSI;
	BYTE byNewOreIndex;
	WORD wSerial;
	DWORD dwOreDurPoint = 0;
	__ITEM* pEquipProp = NULL;
	__ITEM* pBatteryProp = NULL;

	//채굴지연시간이 경과했나 확인
	if(timeGetTime() < (m_dwMineStartTime+m_dwMineDelayTime))
	{
		byErrCode = error_mining_less_delay; //지연시간에러
		goto RESULT;
	}

	//올바른 원석인덱스인지 확인(종류중 상급인덱스이여야함)
	pFld = (_OreItem_fld*)g_Main.m_tblItemData[tbl_code_ore].GetRecord(byOreIndex);
	if((DWORD)pFld->m_nOre_Level > ore_high_quality_grade)
	{
		byErrCode = error_mining_select_fail; //원석종류에러 
		goto RESULT;
	}	

	//채굴장비 장착 확인
	pEquipProp = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
	if(!pEquipProp->m_bLoad)
	{
		byErrCode = error_mining_nothing_equip;//채굴장비 에러
		goto RESULT;
	}
	else
	{
		_WeaponItem_fld* pWFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon].m_wItemIndex);
		if(pWFld->m_nType != type_mine)
		{
			byErrCode = error_mining_not_tool;//채굴장비 에러
			goto RESULT;
		}
	}

	//베터리확인
	pBatteryProp = m_Param.m_dbInven.GetPtrFromSerial(wBatterySerial);
	if(!pBatteryProp)
	{
		byErrCode = error_mining_nothing_battery; //바테리 에러
		goto RESULT;
	}
	
	byNewOreIndex = rand()%3 + byOreIndex;

	//인벤토리에 적용..같은 원석이 있는지 확인..
	for(i = 0; i < m_Param.GetUseSlot(); i++)
	{
		__ITEM* pItem = &m_Param.m_dbInven.m_pStorageList[i];

		if(!pItem->m_bLoad)
			continue;
		if(pItem->m_byTableCode == tbl_code_ore && pItem->m_wItemIndex == byNewOreIndex)
		{
			if(pItem->m_dwDur >= max_overlap_num)
				continue;
			else
			{
				nStorageIndex = i;
				break;
			}
		}
	}

	//기존에 있는것이 아리나 새로 삽일것이라면 빈공간이 있어야됀다.
	if(nStorageIndex == __NSI)
	{
		if(m_Param.m_dbInven.GetIndexEmptyCon() == __NSI)
		{
			byErrCode = error_mining_full_inven; //담을공간없음
			goto RESULT;
		}
	}

RESULT:	

	if(byErrCode == 0)
	{
		if(nStorageIndex != __NSI)	//기존에 담은 인벤토리가 있다면..
		{
			dwOreDurPoint = Emb_AlterDurPoint(_STORAGE_POS::INVEN, nStorageIndex, 1, true);//채굴
			wSerial = m_Param.m_dbInven.m_pStorageList[nStorageIndex].m_wSerial;
		}
		else
		{
			__ITEM NewProp;

			NewProp.m_byTableCode = tbl_code_ore;
			NewProp.m_dwDur = 1;
			NewProp.m_wItemIndex = byNewOreIndex;
			NewProp.m_dwLv = __DF_UPT;
			NewProp.m_wSerial = m_Param.GetNewItemSerial();

			Emb_AddStorage(_STORAGE_POS::INVEN, &NewProp);
			wSerial = NewProp.m_wSerial;
			dwOreDurPoint = 1;
		}

		//채굴장비의 내구성 감소
	//	Emb_AlterDurPoint(_STORAGE_POS::EQUIP, tbl_code_weapon, -1);//dlatl..

		//베테리 용량 감소
		dwBatteryLeftDurPoint = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pBatteryProp->m_byStorageIndex, -(m_dwMineDelayTime/1000), false);
		if(dwBatteryLeftDurPoint == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pBatteryProp, m_szItemHistoryFileName);
		}
	}

	//지연시간 초기화
	m_dwMineDelayTime = 0xFFFFFFFF;
	m_bMineMode = false;

	SendMsg_MineCompleteResult(byErrCode, byNewOreIndex, wSerial, dwOreDurPoint, dwBatteryLeftDurPoint);		
}

void CPlayer::pc_OreCutting(WORD wOreSerial, BYTE byProcessNum)
{	
	BYTE byErrCode = 0;
	__ITEM* pOreItem = NULL;
	int nKindNum = 0;
	_OreItem_fld* pOreFld = NULL;
	int i;
	DWORD dwCost = 0;

	//가공임시공간 비워져있나 확인
	if(m_Param.GetResBufferNum() > 0)
	{
		byErrCode = 4; //가공임시공간 안비움 
		goto RESULT;
	}

	//가방에 해당 원석이 있는지 확인
	pOreItem = m_Param.m_dbInven.GetPtrFromSerial(wOreSerial);
	if(!pOreItem)
	{
		byErrCode = 1; //원석 가방, 시리얼실패
		goto RESULT;
	}	
	if(pOreItem->m_byTableCode != tbl_code_ore)
	{
		byErrCode = 1; //원석 가방, 시리얼실패
		goto RESULT;
	}
	pOreFld = (_OreItem_fld*)g_Main.m_tblItemData[tbl_code_ore].GetRecord(pOreItem->m_wItemIndex);

	//가공수량이 원석수량보다 작은지 확인
	if(byProcessNum > pOreItem->m_dwDur)
	{
		byErrCode = 2; //수량실패
		goto RESULT;
	}

	//가공비용확인
	dwCost = pOreFld->m_nStdPrice*byProcessNum;
	if(dwCost > m_Param.GetDalant())
	{
		byErrCode = 3;//소지금부족
		goto RESULT;
	}
	
RESULT:

	DWORD dwLeftOreNum;
	if(byErrCode == 0)
	{
		//추출자원의 종류를 구함
		for(i = 0; i < byProcessNum; i++)
		{
			nKindNum = ::rand()%(pOreFld->m_nmax_C_random - pOreFld->m_nmin_C_random + 1)+pOreFld->m_nmin_C_random;
			for(int k = 0; k < nKindNum; k++)
			{
				//0x7FFF7FFF 기준..
				DWORD dwR1 = ::rand();
				DWORD dwRand = (dwR1<<16)+::rand();

				DWORD dwResIndex = g_Main.m_tblOreCutting.GetOreIndexFromRate(pOreFld->m_dwIndex, dwRand);
				if(m_Param.m_wCuttingResBuffer[dwResIndex] < 0xFF)	//0xFF를 넘을수 없다..
					m_Param.m_wCuttingResBuffer[dwResIndex]++;
			}
		}

		//원석 감소
		dwLeftOreNum = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pOreItem->m_byStorageIndex, -byProcessNum, true);//가공

		//달란트 감소
		AlterMoney(0, -dwCost);

		//ITEM HISTORY..
		s_MgrItemHistory.cut_item(pOreItem, byProcessNum, m_Param.m_wCuttingResBuffer, dwCost, m_Param.GetDalant(), m_szItemHistoryFileName);

		//월드에 업데이트..	
		if(m_pUserDB)
		{
			int nCnt = 0;
			_CUTTING_DB_BASE::_LIST BufferList[left_cutting_num];
			for(i = 0; i < ::GetMaxResKind(); i++)
			{
				if(m_Param.m_wCuttingResBuffer[i] > 0)
				{
					BufferList[nCnt].byResIndex = (BYTE)i;
					BufferList[nCnt].byAmt = (BYTE)m_Param.m_wCuttingResBuffer[i];
					nCnt++;
				}
			}

			m_pUserDB->Update_CuttingPush(nCnt, BufferList);	
		}
	}

	//추가 및 결과전송
	SendMsg_OreCuttingResult(byErrCode, (BYTE)dwLeftOreNum, dwCost);
}

void CPlayer::pc_OreIntoBag(WORD wResIndex, WORD wSerial, BYTE byAddAmount)
{
	BYTE byErrCode = 0;
	__ITEM* pResItem = NULL;
	WORD wNewSerial = wSerial;

	//추가하려는 수량이 가공수량이내인지 확인
	if(byAddAmount > m_Param.m_wCuttingResBuffer[wResIndex])
	{
		byErrCode = 2;//자원없거나부족
		goto RESULT;
	}

	//인벤에 새로 추가하는 거라면 공간확인
	if(wSerial == 0xFFFF)
	{
		if(m_Param.m_dbInven.GetIndexEmptyCon() == __NSI)
		{
			byErrCode = 4;//가방공간부족
			goto RESULT;	
		}
	}

	//기존의 자원위에 추가하는거라면 시리얼과 내용, 수량추가가 가능한건지 확인
	if(wSerial != 0xFFFF)
	{
		pResItem = m_Param.m_dbInven.GetPtrFromSerial(wSerial);
		if(!pResItem)
		{
			byErrCode = 5;//가방에없는슬롯
			goto RESULT;	
		}
		if(pResItem->m_byTableCode != tbl_code_res || pResItem->m_wItemIndex != wResIndex)
		{
			byErrCode = 7;//가방과내용이다름
			goto RESULT;	
		}

		int nCurLoadAmonut = pResItem->m_dwDur;
		if(nCurLoadAmonut+byAddAmount > max_overlap_num)
		{
			byErrCode = 6;//슬롯당용량초과
			goto RESULT;				
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		//가공수량조정
		m_Param.m_wCuttingResBuffer[wResIndex] -= byAddAmount;

		//인벤토리 조정 및 새로 추가돼는 거라면 시리얼을 임시저장한다.
		if(pResItem)
		{
			Emb_AlterDurPoint(_STORAGE_POS::INVEN, pResItem->m_byStorageIndex, byAddAmount, true);//가공
		}
		else
		{
			__ITEM NewItem;
			NewItem.m_byTableCode = tbl_code_res;
			NewItem.m_wItemIndex = wResIndex;
			NewItem.m_dwDur = byAddAmount;
			NewItem.m_dwLv = __DF_UPT;
			NewItem.m_wSerial = m_Param.GetNewItemSerial();

			Emb_AddStorage(_STORAGE_POS::INVEN, &NewItem);
			wNewSerial = NewItem.m_wSerial;
		}

		//월드에 업데이트..	
		if(m_pUserDB)
			m_pUserDB->Update_CuttingTrans(wResIndex, m_Param.m_wCuttingResBuffer[wResIndex]);	
	}

	//메시지 전송
	SendMsg_OreIntoBagResult(byErrCode, wNewSerial);
}

void CPlayer::pc_CuttingComplete(BYTE byNpcRace)
{
	DWORD dwSumGold = 0;
	bool bClear = false;

	//정리안됀가공자원이 남아있다면 골드로 환산한다.
	for(int i = 0; i < ::GetMaxResKind(); i++)
	{
		if(m_Param.m_wCuttingResBuffer[i] > 0)
		{
			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[tbl_code_res].GetRecord(i);
			int* pGold = &pFld->m_nAncStdPrice;

			int nR = 2;
			if(byNpcRace != m_Param.GetRaceSexCode()/2)
				nR = 1;

			dwSumGold += (pGold[byNpcRace]/nR * m_Param.m_wCuttingResBuffer[i]);		
			bClear = true;
		}
	}

	if(bClear)
	{
		AlterMoney(dwSumGold, 0);
	
		//ITEM HISTORY..
		s_MgrItemHistory.cut_clear_item(m_Param.m_wCuttingResBuffer, dwSumGold, m_Param.GetGold(), m_szItemHistoryFileName);
	}

	m_Param.InitResBuffer();	

	//월드에 업데이트..	
	if(m_pUserDB)
		m_pUserDB->Update_CuttingEmpty();	

	SendMsg_CuttingCompleteResult();
}


