#include "stdafx.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"
#include "BattleDungeonItemfld.h"

void CPlayer::pc_OpenBattleDungeon(DWORD dwItemSerial)
{
	BYTE byErrCode = 0;
	__ITEM* pDungeonItem = NULL;
	CBattleDungeon* pDungeon = NULL;
	_BattleDungeonItem_fld* pDungeonFld = NULL;
	CMapData* pTarMap = NULL;
	WORD wLayerSetIndex = 0xFFFF;
	int i;

	//현재가 일반맵인지 확인
	if(m_pCurMap->m_pMapSet->m_nMapType != MAP_TYPE_STD)
	{
		byErrCode = error_dungeon_dis_position;//던젼을 만들수없는 위치
		goto RESULT;
	}

	//아이템보유확인
	pDungeonItem = m_Param.m_dbInven.GetPtrFromSerial(dwItemSerial);
	if(!pDungeonItem)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, dwItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon.. 없는아탬(sr: %d)", m_Param.GetCharName(), dwItemSerial);
		byErrCode = error_dungeon_nothing_ditem;//없는아탬
		goto RESULT;
	}

	//던젼게이트인지 확인
	if(pDungeonItem->m_byTableCode != tbl_code_bdungeon)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon.. 게이트아님", m_Param.GetCharName());
		byErrCode = error_dungeon_not_ditem;
		goto RESULT;
	}
	pDungeonFld = (_BattleDungeonItem_fld*)g_Main.m_tblItemData[tbl_code_bdungeon].GetRecord(pDungeonItem->m_wItemIndex);
	if(!pDungeonFld)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon..없는 던젼 데이터(%d)", m_Param.GetCharName(), pDungeonItem->m_wItemIndex);
		byErrCode = error_dungeon_data_failure;
		goto RESULT;
	}
	pTarMap = g_Main.m_MapOper.GetMap(pDungeonFld->m_strMapCode);
	if(!pTarMap)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon..던젼맵이 없다(%s)", m_Param.GetCharName(), pDungeonFld->m_strMapCode);
		byErrCode = error_dungeon_data_failure;
		goto RESULT;
	}
	//던젼맵인지..
	if(pTarMap->m_pMapSet->m_nMapType != MAP_TYPE_DUNGEON)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon..던젼맵이 아님(%s)", m_Param.GetCharName(), pDungeonFld->m_strMapCode);
		byErrCode = error_dungeon_data_failure;
		goto RESULT;
	}

	//해당맵에 남은 레이이가 있는가..
	for(i = 0; i < pTarMap->m_pMapSet->m_nLayerNum; i++)
	{
		if(!pTarMap->m_ls[i].IsActiveLayer())
		{
			wLayerSetIndex = i;
			break;
		}
	}
	if(wLayerSetIndex == 0xFFFF)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon.. 빈레이어없음", m_Param.GetCharName());
		byErrCode = error_dungeon_full_gate;
		goto RESULT;
	}

	//남은 던젼게이트있는지
	pDungeon = ::FindEmptyDungeonGate(g_BattleDungeon, MAX_BATTLEDUNGEON);
	if(!pDungeon)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon.. 빈게이트없음", m_Param.GetCharName());
		byErrCode = error_dungeon_full_gate;
		goto RESULT;
	}

	if(pDungeonFld->m_nGenMob >= pTarMap->m_pMapSet->m_nMonsterSetFileNum)
	{
		g_Main.m_logSystemError.Write("pc_OpenBattleDungeon.. 몬스터세팅파일인덱스가 이상(%s)", pDungeonFld->m_strCode);
		byErrCode = error_dungeon_data_failure;
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		_battledungeon_create_setdata Data;

		Data.m_pRecordSet = pDungeonFld;
		Data.m_pMap = m_pCurMap;
		Data.m_nLayerIndex = m_wMapLayerIndex;
		m_pCurMap->GetRandPosInRange(m_fCurPos, 10, Data.m_fStartPos);
		Data.pOpener = this;
		Data.pTarMap = pTarMap;
		Data.wTarLayerIndex = wLayerSetIndex;

		if(pDungeon->Create(&Data))
		{
			Emb_DelStorage(_STORAGE_POS::INVEN, pDungeonItem->m_byStorageIndex, false);

			//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pDungeonItem, m_szItemHistoryFileName);

			//LOG..
			g_Main.m_logDungeon.Write("개설: %s, 던젼이름:%s", m_Param.GetCharName(), pDungeonFld->m_strName);
		}
		else
		{
			byErrCode = error_dungeon_create_fail;
		}
	}

	SendMsg_BattleDungeonOpenResult(byErrCode);
}

void CPlayer::pc_GotoDungeon(WORD wGateIndex, DWORD dwGateSerial)
{
	BYTE byErrCode = 0;
	CBattleDungeon* pDungeon = &g_BattleDungeon[wGateIndex]; 

	//게이트가 존재하는지..
	if(!pDungeon->m_bLive)
	{
		byErrCode = error_dungeon_none_exist;//없는 던젼게이트
		goto RESULT;
	}

	if(pDungeon->m_dwObjSerial != dwGateSerial)
	{
		byErrCode = error_dungeon_none_exist;//없는 던젼게이트
		goto RESULT;
	}

	//맵이동중인지..
	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = error_dungeon_trans_map;//맵을 이동하는중
		goto RESULT;
	}

	//위치 확인..	
	if(move_potal_dist < ::GetSqrt(m_fCurPos, pDungeon->m_fCurPos))
	{
		byErrCode = error_dungeon_not_near_gate;//던젼과 먼위치
		goto RESULT;
	}

	//입장 권한이 있는지..
	if(!pDungeon->IsEnterAblePlayer(this))
	{
		byErrCode = error_dungeon_refuse_enter;//입장권한 없음
		goto RESULT;
	}

RESULT:

	_ENTER_DUNGEON_NEW_POS NewPos;

	if(byErrCode == 0)
	{
		if(pDungeon->GetEnterNewPos(&NewPos))
		{
			//현재위치 및 맵 저장..
			memcpy(m_fBeforeDungeonPos, m_fCurPos, sizeof(float)*3);
			m_pBeforeDungeonMap = m_pCurMap;

			CMapData* pTarMap = g_Main.m_MapOper.GetMap(NewPos.byMapCode);

			//소환물이있다면 리턴시킴
			_AnimusReturn();
			
			m_bMapLoading = true;		
			OutOfMap(pTarMap, NewPos.wLayerIndex, mapin_type_dungeon, NewPos.fPos);

			pDungeon->EnterPlayer(this);

			//LOG..
			g_Main.m_logDungeon.Write("입장: %s, (개설자:%s, 던젼이름:%s)", m_Param.GetCharName(), pDungeon->m_szOpenerName, ((_BattleDungeonItem_fld*)pDungeon->m_pRecordSet)->m_strName);
		}
		else
		{
			byErrCode = error_dungeon_pos_setting;//위치세팅실패
		}
	}

	SendMsg_GotoDungeon(byErrCode, NewPos.byMapCode, NewPos.fPos, pDungeon);
}

void CPlayer::pc_OutofDungeon(BYTE byCompleteCode, CBattleDungeon* pDungeon /* = NULL*/)
{
	BYTE byErrCode = 0;// 4;//잘못됀포탈코드 6;//맵이동중
	
	//이동중인지..
	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = error_dungeon_trans_map;
		goto RESULT;
	}

	//던젼에 있는지..
	if(m_pCurMap->m_pMapSet->m_nMapType != MAP_TYPE_DUNGEON)
	{
		byErrCode = error_dungeon_not_here;//던젼에 있지않음
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		if(byCompleteCode == dungeon_complete_code_mission && pDungeon)
		{//미션성공으로인한 완수일경우 보상을 해준다.
			//경험치..
			AlterExp( ((_BattleDungeonItem_fld*)pDungeon->m_pRecordSet)->m_nExp );

			//골드 및 달란트..
			DWORD dwAddDalant = ((_BattleDungeonItem_fld*)pDungeon->m_pRecordSet)->m_nDalant;
			DWORD dwAddGold = ((_BattleDungeonItem_fld*)pDungeon->m_pRecordSet)->m_nGold;

			AlterMoney(dwAddGold, dwAddDalant);
			SendMsg_ExchangeMoneyResult(0);

			//ITEM HISTORY..
			s_MgrItemHistory.reward_add_money("던젼", dwAddDalant, dwAddGold, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);			
		}

		//소환물이있다면 리턴시킴
		_AnimusReturn();

		m_bMapLoading = true;
		OutOfMap(m_pBeforeDungeonMap, 0, mapin_type_dungeon, m_fBeforeDungeonPos);
	}

	SendMsg_OutofDungeon(byErrCode, m_pBeforeDungeonMap->m_pMapSet->m_dwIndex, m_fBeforeDungeonPos);
}