#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"

//투명..
void CPlayer::mgr_tracing(bool bOn)		
{
	if(m_bObserver != bOn)
	{
		m_bObserver = bOn;

		char szRespon[128];

		if(bOn)
			sprintf(szRespon, "투명모드 설정");
		else
			sprintf(szRespon, "투명모드 해제");

		SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, strlen(szRespon), szRespon);			
	}
}

//무적
void CPlayer::mgr_matchless(bool bMatchless)
{
	if(m_bCheat_Matchless != bMatchless)
	{
		m_bCheat_Matchless = bMatchless;

		char szRespon[128];

		if(bMatchless)
			sprintf(szRespon, "무적모드 설정");
		else
			sprintf(szRespon, "무적모드 해제");

		SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, strlen(szRespon), szRespon);			
	}	
}

//강퇴..
void CPlayer::mgr_kick(char* pszCharName)					
{
	CUserDB* pUser = ::SearchAvatorWithName(g_UserDB, MAX_PLAYER, pszCharName);
	if(pUser)
	{
		pUser->ForceCloseCommand(false, 0);	

		//LOG..
		g_Main.m_logCheat.Write("[강퇴] %s: %s", m_Param.GetCharName(), pszCharName);
	}
}

//소환..
void CPlayer::mgr_recall_player(char* pszCharName)	
{
	CUserDB* p = ::SearchAvatorWithName(g_UserDB, MAX_PLAYER, pszCharName);
	if(p)
	{
		CPlayer* pPlayer = &g_Player[p->m_idWorld.wIndex];
		if(!pPlayer->m_bLive)
			return;

		pPlayer->pc_GotoAvatorRequest(m_Param.GetCharName());

		//LOG..
		g_Main.m_logCheat.Write("[소환] %s: %s", m_Param.GetCharName(), pszCharName);
	}	
}

//몬스터 소환..
void CPlayer::mgr_recall_mon(char* pszMonCode, int nCreateNum)	
{
	if(nCreateNum > 100)
		nCreateNum = 100;

	int nRealCreateNum = 0;
	for(int i = 0; i < nCreateNum; i++)
	{
		if(!::CreateRepMonster(m_pCurMap, m_wMapLayerIndex, m_fCurPos, pszMonCode))//, NULL, NULL))
			break;
		nRealCreateNum++;
	}

	if(nRealCreateNum > 0)
	{//LOG..
		g_Main.m_logCheat.Write("[몬소환] %s: %s * %d", m_Param.GetCharName(), pszMonCode, nRealCreateNum);
	}
}

//채팅금지..
void CPlayer::mgr_dumb(char* pszCharName, DWORD	dwDurHour)	
{
	char szRespon[128] = {0, };

	if(dwDurHour > 0)
	{
		CUserDB* pUser = ::SearchAvatorWithName(g_UserDB, MAX_PLAYER, pszCharName);

		if(!pUser)
		{
			sprintf(szRespon, "채팅금지 실패: %s -> 해당 케릭터는 현재 접속중 아님", pszCharName);
			goto RESULT;
		}
		
		if(g_Main.IsChatLockAccount(pUser->m_dwAccountSerial))
		{
			sprintf(szRespon, "채팅금지 실패: %s -> 이미 채팅금지리스트에 있슴", pszCharName);
			goto RESULT;
		}

		if(!g_Main.AddChatLockAccount(pUser, dwDurHour*60*60*1000))
		{
			sprintf(szRespon, "채팅금지 실패: %s -> 동시금지수(%d) 초과 ", pszCharName, CMainThread::max_lock_chat);
			goto RESULT;
		}

		sprintf(szRespon, "채팅금지: %s (%d시간)", pszCharName, dwDurHour);

		//LOG..
		g_Main.m_logCheat.Write("[채팅금지] %s: %s %d시간", m_Param.GetCharName(), pszCharName, dwDurHour);
	}
	else	//해제..
	{
		if(!g_Main.DelChatLockAccount(pszCharName))
		{
			sprintf(szRespon, "채팅금지해제 실패: %s -> 금지대상 케릭터가 아님", pszCharName);
			goto RESULT;
		}

		sprintf(szRespon, "채팅금지해제: %s", pszCharName);

		//LOG..
		g_Main.m_logCheat.Write("[채팅금지해제] %s: %s", m_Param.GetCharName(), pszCharName);
	}

RESULT:

	int nLen = strlen(szRespon);
	if(nLen > 0)
		SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, nLen, szRespon);	
}

//운영자귓말
void CPlayer::mgr_whisper(int nSize, char* pszMsg)	
{
	char szRespon[128] = {0, };

	for(int i = 0; i < MAX_PLAYER; i++)
	{
		if(!g_UserDB[i].m_bActive)
			continue;

		if(g_UserDB[i].m_byUserDgr != USER_DEGREE_MGR && g_UserDB[i].m_byUserDgr != USER_DEGREE_DEV)
			continue;

		g_Player[i].SendData_ChatTrans(CHAT_TYPE_MGRWSP, m_dwObjSerial, nSize, pszMsg);			
	}
}

//질의..
void CPlayer::mgr_query(int nWordNum, char* pszMsg0, char* pszMsg1, char* pszMsg2, char* pszMsg3, char* pszMsg4)
{
	char szRespon[128] = {0, };

	if(!strcmp(pszMsg0, "동접"))
	{
		if(nWordNum == 1)
			sprintf(szRespon, "%s : %d", pszMsg0, CUserDB::s_nLoginNum);
		else if(nWordNum == 2)
		{
			if(!strcmp(pszMsg1, "필드"))
				sprintf(szRespon, "%s(%s): %d", pszMsg0, pszMsg1, CPlayer::s_nLiveNum);
		}
	}
	else if(!strcmp(pszMsg0, "열린던젼수"))
	{
		sprintf(szRespon, "%s : %d", pszMsg0, CBattleDungeon::s_nLiveNum);
	}
	else if(!strcmp(pszMsg0, "위치"))
	{
		if(nWordNum == 2)
		{			
			if(strlen(pszMsg1) > max_name_len)
				sprintf(szRespon, "%s: 이름이 길다", pszMsg0);
			else 
			{
				CUserDB* p = ::SearchAvatorWithName(g_UserDB, MAX_PLAYER, pszMsg1);
				if(!p)				
					sprintf(szRespon, "%s: %s는 현재 접속중이 아님", pszMsg0, pszMsg1);
				else
				{
					CPlayer* pPlayer = &g_Player[p->m_idWorld.wIndex];
					if(!pPlayer->m_bLive)
						sprintf(szRespon, "%s: %s는 현재 로비에 있음", pszMsg0, pszMsg1);
					else
					{
						if(pPlayer->m_pCurMap)
						{
							sprintf(szRespon, "%s: %s는 %s맵 (%d, %d, %d)에 있음", pszMsg0, pszMsg1,  
										pPlayer->m_pCurMap->m_pMapSet->m_strCode, (int)pPlayer->m_fCurPos[0], (int)pPlayer->m_fCurPos[1], (int)pPlayer->m_fCurPos[2]);
						}
						else
							sprintf(szRespon, "%s: %s는 현재 로비에 있음", pszMsg0, pszMsg1);
					}
				}
			}
		}
	}
	else if(!strcmp(pszMsg0, "경제"))
	{
		if(nWordNum == 2)
		{
			if(!strcmp(pszMsg1, "지표"))
			{
			//	sprintf(szRespon, "현재지표: B: C: A:", pszMsg0, CBattleDungeon::s_nLiveNum);
			}
			else if(!strcmp(pszMsg1, "통화"))
			{
			}
		}
	}

	int nLen = strlen(szRespon);
	if(nLen > 0)
		SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, nLen, szRespon);			
}

//레벨변경
void CPlayer::dev_lv(int nLv)			
{
	if(nLv > max_level)
		return;

	if(nLv > m_Param.GetLevel())
	{	//현재레벨보다 클때만 세팅..
		BYTE byOldLv = m_Param.GetLevel();
		
		m_Param.SetExp(0);
		m_wExpRate = 0;

		SetLevel(nLv);

		//LOG..
		g_Main.m_logCheat.Write("[레밸업] %s: %d -> %d", m_Param.GetCharName(), byOldLv, nLv);
	}
}

//자살
void CPlayer::dev_die()				
{
	m_nLastBeatenPart = 0;
	SetDamage(m_Param.GetHP(), NULL, 0, false);
}

//모든 포스아이템의 사용누적수를 변경
void CPlayer::dev_up_forceitem(int nCum)	
{
	for(int i = 0; i < force_storage_num; i++)
	{
		__ITEM* pItem = &m_Param.m_dbForce.m_pStorageList[i];
		if(pItem->m_bLoad)
		{
			m_pUserDB->Update_ItemDur(_STORAGE_POS::FORCE, i, nCum, false);
		}
	}

	//LOG..
	g_Main.m_logCheat.Write("[포스리버 업] %s: %d", m_Param.GetCharName(), nCum);
}

//스킬의 사용누적수를 변경
void CPlayer::dev_up_skill(char* pszSkillCode, int nCum)	
{
	_base_fld* pFld = g_Main.m_tblEffectData[effect_code_skill].GetRecord(pszSkillCode);
	if(!pFld)
		return;

	Emb_UpdateStat(_STAT_DB_BASE::DATAPOS_SKILL + pFld->m_dwIndex, nCum);

	//LOG..
	g_Main.m_logCheat.Write("[스킬 업] %s: %s %d", m_Param.GetCharName(), pszSkillCode, nCum);
}

//모든 포스 마스터리의 성공누적수를 변경
void CPlayer::dev_up_forcemastery(int nCum)	
{
	for(int i = 0; i < force_mastery_num; i++)
	{
		Emb_UpdateStat(_STAT_DB_BASE::DATAPOS_FORCE + i, nCum);
	}

	//LOG..
	g_Main.m_logCheat.Write("[포스마스터리 업] %s: %d", m_Param.GetCharName(), nCum);
}

//특정 마스터리증가
void CPlayer::dev_up_mastery(int nMasteryCode, int nMasteryIndex, int nCum)
{
	int nStatIndex = _STAT_DB_BASE::GetStatIndex(nMasteryCode, nMasteryIndex);
	if(nStatIndex == -1)
		return;

	Emb_UpdateStat(nStatIndex, nCum);

	//LOG..
	g_Main.m_logCheat.Write("%s: [%d:%d마스터리 업]: %d", m_Param.GetCharName(), nMasteryCode, nMasteryIndex, nCum);
}

//모든숙련도변경
void CPlayer::dev_up_all(int nCum)	
{
	for(int i = 0; i < WP_CLASS_NUM; i++)//무기숙련도변경
		Emb_UpdateStat(_STAT_DB_BASE::DATAPOS_WPCNT + i, nCum);

	Emb_UpdateStat(_STAT_DB_BASE::DATAPOS_DEF, nCum);//방어숙련도변경

	Emb_UpdateStat(_STAT_DB_BASE::DATAPOS_SHLD, nCum);//방패숙련도변경

	for(i = 0; i < max_skill_num; i++)//스킬의 사용누적수
		Emb_UpdateStat(_STAT_DB_BASE::DATAPOS_SKILL + i, nCum);

	for(i = 0; i < MI_CLASS_NUM; i++)//제작숙련도
		Emb_UpdateStat(_STAT_DB_BASE::DATAPOS_MAKE + i, nCum);

	Emb_UpdateStat(_STAT_DB_BASE::DATAPOS_SPECIAL, nCum);//종족별특수능력숙련도

	for(i = 0; i < force_mastery_num; i++)//포스마스터리변경
		Emb_UpdateStat(_STAT_DB_BASE::DATAPOS_FORCE + i, nCum);

	//LOG..
	g_Main.m_logCheat.Write("[모든마스터리 업] %s: %d", m_Param.GetCharName(), nCum);
}

//애니머스의 소환대기시간을 없앰.
void CPlayer::dev_animus_recall_time_free(bool bFree)	
{
	if(m_bFreeRecallWaitTime != bFree)
	{
		m_bFreeRecallWaitTime = bFree;

		//클라이언트에게 알림..
		_animus_recall_wait_time_free_inform_zocl Send;

		Send.bFree = bFree;

		BYTE byType[msg_header_num] = {recall_msg, animus_recall_wait_time_free_inform_zocl};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
	}
}

//애니머스의 경험치포인트를 올림..
void CPlayer::dev_set_animus_exp(int nExpPoint)
{
	for(int i = 0; i < animus_storage_num; i++)
	{
		if(m_Param.m_dbAnimus.m_pStorageList[i].m_bLoad)
		{
			if(m_Param.m_dbAnimus.m_pStorageList[i].m_dwDur == nExpPoint)
				continue;

			m_Param.m_dbAnimus.m_pStorageList[i].m_dwDur = nExpPoint;
		
			//월드에 알림..
			if(m_pUserDB)
				m_pUserDB->Update_ItemDur(_STORAGE_POS::ANIMUS, i, nExpPoint, false);

			//클라이언트에게 알림..
			_animus_exp_inform_zocl Send;

			Send.wAnimusItemSerial = m_Param.m_dbAnimus.m_pStorageList[i].m_wSerial;
			Send.dwExp = nExpPoint;

			BYTE byType[msg_header_num] = {recall_msg, animus_exp_inform_zocl};
			g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
		}
	}

	//LOG..
	g_Main.m_logCheat.Write("[애니머스경험치 변경] %s: %d", m_Param.GetCharName(), nExpPoint);
}

//몬스터 초기화
void CPlayer::dev_init_monster()	
{
	bool bRes = g_Main.gm_MonsterInit();

	//LOG..
	g_Main.m_logCheat.Write("[몬스터초기화] %s: %d", m_Param.GetCharName(), bRes);
}

//골드와 달란트 천만으로 변경
void CPlayer::dev_money()		
{
	const int nMaxGold = 100000;
	const int nMaxDalant = 10000000;

	//증가량 계산..
	DWORD dwOldGold = m_Param.GetGold();
	DWORD dwOldDalant = m_Param.GetDalant();

	int nAddGold = nMaxGold - m_Param.GetGold();
	int nAddDalant = nMaxDalant - m_Param.GetDalant();

	AlterMoney(nAddGold, nAddDalant);

	SendMsg_ExchangeMoneyResult(0);

	//ITEM HISTORY..
	s_MgrItemHistory.cheat_alter_money(m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);

	//LOG..
	g_Main.m_logCheat.Write("[돈세팅] %s: dt(%d -> %d), gd(%d -> %d)", m_Param.GetCharName(), dwOldDalant, m_Param.GetDalant(), dwOldGold, m_Param.GetGold());
}

//포스창 가득채움
void CPlayer::dev_full_force()	
{
	__ITEM ItemCopy[force_storage_num];
	BYTE byAddNum = 0;

	for(int i = 0; i < force_storage_num; i++)
	{
		__ITEM* pCon = &m_Param.m_dbForce.m_pStorageList[i];

		if(!pCon->m_bLoad)
		{
			__ITEM Item;
			Item.m_byTableCode = tbl_code_fcitem;
			Item.m_wItemIndex = i;
			Item.m_dwDur = 100000000;
			Item.m_wSerial = m_Param.GetNewItemSerial();
			Item.m_dwLv = __DF_UPT;

			Emb_AddStorage(_STORAGE_POS::FORCE, &Item);

			memcpy(&ItemCopy[byAddNum], &Item, sizeof(__ITEM));
			byAddNum++;
		}
	}

	//ITEM HISTORY..
	s_MgrItemHistory.cheat_add_item(ItemCopy, byAddNum, m_szItemHistoryFileName);

	if(byAddNum > 0)
	{//LOG..
		g_Main.m_logCheat.Write("[포스창채움] %s", m_Param.GetCharName());
	}
}

void CPlayer::dev_loot_material()		//재료루팅
{
	//LOG..
	g_Main.m_logCheat.Write("[재료루팅] %s", m_Param.GetCharName());
	
	//전리품
	for(int i = 0; i < g_Main.m_tblItemData[tbl_code_booty].GetRecordNum(); i++)
	{
		if(!::CreateItemBox(tbl_code_booty, i, max_overlap_num, __DF_UPT, 
							this, NULL, itembox_create_code_cheat,
							m_pCurMap, m_wMapLayerIndex, m_fCurPos, true))
			return;
	}
	//보석		
	for(i = 0; i < g_Main.m_tblItemData[tbl_code_res].GetRecordNum(); i++)
	{
		if(!::CreateItemBox(tbl_code_res, i, max_overlap_num, __DF_UPT, 
							this, NULL, itembox_create_code_cheat, 
							m_pCurMap, m_wMapLayerIndex, m_fCurPos, true))
			return;
	}
	//툴		
	for(i = 0; i < g_Main.m_tblItemData[tbl_code_maketool].GetRecordNum(); i++)
	{
		DWORD dwDur = ::GetItemDurPoint(tbl_code_maketool, i);

		if(!::CreateItemBox(tbl_code_maketool, i, dwDur, __DF_UPT, 
							this, NULL, itembox_create_code_cheat, 
							m_pCurMap, m_wMapLayerIndex, m_fCurPos, true))
			return;
	}
	//가방
	for(i = 0; i < 4; i++)
	{
		if(!::CreateItemBox(tbl_code_bag, i, 0, __DF_UPT, 
							this, NULL, itembox_create_code_cheat, 
							m_pCurMap, m_wMapLayerIndex, m_fCurPos, true))
			return;
	}
}

void CPlayer::dev_loot_item(char* pszItemCode, int nNum)	//아이템을 루팅
{
	int nTable = ::GetItemTableCode(pszItemCode);
	if(nTable == -1)
		return;

	if(nTable == tbl_code_key)//유닛키는 치트금지
		return;

	_base_fld* pFld = g_Main.m_tblItemData[nTable].GetRecord(pszItemCode);
	if(!pFld)
		return;

	DWORD dwDur = 0;
	if(::IsOverLapItem(nTable))
		dwDur = max_overlap_num;
	else
		dwDur = ::GetItemDurPoint(nTable, pFld->m_dwIndex);

	if(nNum > 100)
		nNum = 100;

	//아이템의 종류에따른 아이템레벨 세팅 분기..
	DWORD dwLvBit;
	BYTE byItemKindCode = ::GetItemKindCode(nTable);
	if(byItemKindCode == item_kind_std)
	{	//일반아이템
		BYTE byMaxLv = ::GetDefItemUpgSocketNum(nTable, pFld->m_dwIndex);
		dwLvBit = ::GetBitAfterSetLimSocket(byMaxLv);
	}
	else if(byItemKindCode == item_kind_ani)
	{	//소환아이템
		dwLvBit = ::GetMaxParamFromExp(pFld->m_dwIndex, dwDur);
	}
	else
		return;

	int nRealNum = 0;
	for(int i = 0; i < nNum; i++)
	{
		if(!::CreateItemBox(nTable, pFld->m_dwIndex, dwDur, dwLvBit, 
							this, NULL, itembox_create_code_cheat, 
							m_pCurMap, m_wMapLayerIndex, m_fCurPos, true))
			return;
		nRealNum++;
	}

	if(nRealNum > 0)
	{//LOG..
		g_Main.m_logCheat.Write("[아이템루팅] %s: %s, %d", m_Param.GetCharName(), pszItemCode, nRealNum);
	}
}

void CPlayer::dev_loot_item_lv(char* pszPart, int nLv)	//원하는 파트의 지정레벨 아이템을 전부 루팅
{
	int nTable = -1;
	if(!strcmp(pszPart, "상의"))		nTable = tbl_code_upper;
	else if(!strcmp(pszPart, "하의"))	nTable = tbl_code_lower;
	else if(!strcmp(pszPart, "장갑"))	nTable = tbl_code_gauntlet;
	else if(!strcmp(pszPart, "신발"))	nTable = tbl_code_shoe;
	else if(!strcmp(pszPart, "투구"))	nTable = tbl_code_helmet;
	else if(!strcmp(pszPart, "방패"))	nTable = tbl_code_shield;
	else if(!strcmp(pszPart, "무기"))	nTable = tbl_code_weapon;
	else if(!strcmp(pszPart, "망토"))	nTable = tbl_code_cloak;

	if(nTable == -1)
		return;

	int nItemNum = g_Main.m_tblItemData[nTable].GetRecordNum();

	int nRealNum = 0;
	for(int i = 0; i < nItemNum; i++)
	{
		if(nLv == ::GetItemEquipLevel(nTable, i))
		{
			if(!::IsExistItem(nTable, i))
				continue;

			if(!::IsGroundableItem(nTable, i))
				continue;

			DWORD dwDur = 0;
			if(::IsOverLapItem(nTable))
				dwDur = max_overlap_num;
			else
				dwDur = ::GetItemDurPoint(nTable, i);

			if(!::CreateItemBox(nTable, i, dwDur, __DF_UPT, 
								this, NULL, itembox_create_code_cheat, 
								m_pCurMap, m_wMapLayerIndex, m_fCurPos, true))
				return;

			nRealNum++;
		}
	}

	if(nRealNum > 0)
	{//LOG..
		g_Main.m_logCheat.Write("[파트레벨별 아이템루팅] %s: %s, %d, %d", m_Param.GetCharName(), pszPart, nLv, nRealNum);
	}
}

void CPlayer::dev_inven_empty()	//인벤비우기
{
	__ITEM ItemCopy[bag_storage_num];
	BYTE byDelNum = 0;	

	for(int i = 0; i < m_Param.GetBagNum()*one_bag_store_num; i++)
	{
		if(m_Param.m_dbInven.m_pStorageList[i].m_bLoad)
		{
			__ITEM* pCon = &m_Param.m_dbInven.m_pStorageList[i];
			SendMsg_DeleteStorageInform(_STORAGE_POS::INVEN, pCon->m_wSerial);
			Emb_DelStorage(_STORAGE_POS::INVEN, i, false);

			memcpy(&ItemCopy[byDelNum], pCon, sizeof(__ITEM));
			byDelNum++;
		}
	}

	//ITEM HISTORY..
	s_MgrItemHistory.cheat_del_item(ItemCopy, byDelNum, m_szItemHistoryFileName);

	//LOG..
	g_Main.m_logCheat.Write("[인벤비움] %s", m_Param.GetCharName());
}

void CPlayer::dev_avator_copy(char* pszDstName)
{
	bool bSucc = false;
	//대상케럭터 유무확인..
	CUserDB* pUser = ::SearchAvatorWithName(g_UserDB, MAX_PLAYER, pszDstName);
	if(pUser)
	{
		CPlayer* pDst = &g_Player[pUser->m_idWorld.wIndex];
		if(!pDst->m_bLive)
			return;

		//같은 종족이여야한다..
		if(pDst->m_Param.GetRaceSexCode() != m_Param.GetRaceSexCode())
			return;

		//copy..
		m_pUserDB->Update_CopyAll(&pUser->m_AvatorData);

		//ITEM HISTORY..
		s_MgrItemHistory.char_copy(pUser->m_dwSerial, m_szItemHistoryFileName);

		//LV HISTORY..
		s_MgrLvHistory.char_copy(pUser->m_dwSerial, m_szLvHistoryFileName);

		m_pUserDB->DirectClose();	//접속을 바로 끊어준다..

		bSucc = true;
	}
	
	//LOG..
	if(bSucc)
		g_Main.m_logCheat.Write("[케릭터복사] %s <- %s", m_Param.GetCharName(), pszDstName);
	else
		g_Main.m_logCheat.Write("[케릭터복사실패] %s <- %s", m_Param.GetCharName(), pszDstName);
}

void CPlayer::dev_make_succ(bool bSucc)	//제작 업그에드 무조건 성공
{
	if(m_bCheat_100SuccMake != bSucc)
	{
		m_bCheat_100SuccMake = bSucc;

		//LOG..
		if(bSucc)
			g_Main.m_logCheat.Write("[제작 업그에드 무조건 성공] %s", m_Param.GetCharName());
		else
			g_Main.m_logCheat.Write("[제작 업그에드 무조건 성공 해제] %s", m_Param.GetCharName());
	}
}

void CPlayer::dev_cont_effect_time(DWORD dwSec)	//걸린 지속효과의 시간 조정
{
	if(dwSec == 0)
		m_nContEffectSec = -1;
	else if(dwSec > 9000)
		m_nContEffectSec = 9000;
	else
		m_nContEffectSec = dwSec;	

	//LOG..
	g_Main.m_logCheat.Write("[걸린 지속효과의 시간 조정] %s : sec:%d", m_Param.GetCharName(), dwSec);
}

void CPlayer::dev_cont_effect_del()	//걸린 지속효과를 전부 없앤다
{
	for(int i = 0; i < SF_CONT_CODE_NUM; i++)
	{
		for(int c = 0; c < CONT_SF_SIZE; c++)
		{
			_sf_continous* p = &m_SFCont[i][c];
			if(!p->m_bExist)
				continue;

			RemoveSFContEffect(i, c);
		}
	}
	//LOG..
	g_Main.m_logCheat.Write("[걸린 지속효과 없앰] %s", m_Param.GetCharName());
}
