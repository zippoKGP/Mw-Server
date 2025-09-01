// Player.cpp: implementation of the CPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Player.h"
#include "UserDB.h"
#include "MainThread.h"
#include "MyUtil.h"
#include "WeaponItemFld.h"
#include "DfnEquipItemFld.h"
#include "PotionItemFld.h"
#include "MonsterCharacterFld.h"
#include "PlayerCharacterFld.h"
#include "ExpFld.h"
#include "ItemMakeDataTable.h"
#include "RingItemFld.h"
#include "AmuletItemFld.h"
#include "forceFld.h"
#include "skillFld.h"
#include "ResourceItemFld.h"
#include "UnitKeyItemFld.h"
#include "ForceItemFld.h"
#include "ParkingUnit.h"
#include "UnitPartFld.h"
#include "UnitFrameFld.h"
#include "ClassFld.h"

int CPlayer::s_nLiveNum = 0;
int	CPlayer::s_nRaceNum[RACE_NUM] = {0,};
DWORD CPlayer::s_dwItemUpgSucRate[MAX_ITEM_LV] = {100, 75, 50, 25, 10, 5, 1};
float CPlayer::s_fExpDivUnderParty_Dam[member_per_party] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
float CPlayer::s_fExpDivUnderParty_Kill[member_per_party] = {1.0f, 1.25f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f};
float CPlayer::s_fWPGapAdjust[wp_type_num] = {wp_adjust_knife, wp_adjust_sword, wp_adjust_axe, wp_adjust_mace, wp_adjust_spear, 
												wp_adjust_bow, wp_adjust_firearm, wp_adjust_launcher, wp_adjust_throw, wp_adjust_staff};
double CPlayer::s_dExpLimPerLv[max_level];
int* CPlayer::s_pnLinkForceItemToEffect = NULL;

CMgrAvatorItemHistory CPlayer::s_MgrItemHistory;
CMgrAvatorLvHistory CPlayer::s_MgrLvHistory;
_ANIMUS_RETURN_DELAY CPlayer::s_AnimusReturnDelay;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayer::CPlayer()
{
	m_pdwLastForceActTime = NULL;
	m_pdwLastSkillActTime = NULL;
	m_pParkingUnit = NULL;

/*	CTerm t;
	for(int i = 0; i < 100000; i++)
	{
	//	timeGetTime();
		//char szdd[32] = "123456789";
		//itoa(123456,szdd, 10);
		//atoi(szdd);
	}
	int ss = t.GetTerm();*/
}

CPlayer::~CPlayer()
{
	if(m_bLoad)
		Destroy();

	if(s_pnLinkForceItemToEffect)
	{
		delete [] s_pnLinkForceItemToEffect;
		s_pnLinkForceItemToEffect = NULL;
	}

	if(m_pdwLastForceActTime)
		delete [] m_pdwLastForceActTime;

	if(m_pdwLastSkillActTime)
		delete [] m_pdwLastSkillActTime;
}

bool CPlayer::Init(_object_id* pID)
{
	CCharacter::Init(pID);

	m_tmrAlterState.BeginTimer(timer_alter_state);	//1초..
	m_tmrIntervalSec.BeginTimer(1000);

	int nForceActNum = g_Main.m_tblEffectData[effect_code_force].GetRecordNum();
	if(nForceActNum > 0)
	{
		m_pdwLastForceActTime = new DWORD [nForceActNum];
		memset(m_pdwLastForceActTime, 0, sizeof(DWORD)*nForceActNum);
	}

	int nSkillActNum = g_Main.m_tblEffectData[effect_code_skill].GetRecordNum();
	if(nSkillActNum > 0)
	{
		m_pdwLastSkillActTime = new DWORD [nSkillActNum];
		memset(m_pdwLastSkillActTime, 0, sizeof(DWORD)*nSkillActNum);
	}

	m_pPartyMgr = &g_PartyPlayer[pID->m_wIndex];
	m_bLoad = false;
	m_bOper = false;
	m_bMoveOut = false;
	m_bFullMode = false;
	m_bPosMainBase = false;
	m_bCheat_100SuccMake = false;
	m_bCheat_Matchless = false;
	m_bFreeRecallWaitTime = false;
	m_pUserDB = NULL;
	m_pUsingUnit = NULL;
	m_pParkingUnit = NULL;
	m_dwUnitViewOverTime = 0xFFFFFFFF;
	m_pRecalledAnimusItem = NULL;
	m_pRecalledAnimusChar = NULL;
	m_dwLastRecallTime = 0;
	m_bNextRecallReturn = false;
	m_id.dwSerial = 0xFFFFFFFF;
	m_id.wIndex = pID->m_wIndex;
	m_dwObjSerial = 0xFFFFFFFF;
	m_byUserDgr = USER_DEGREE_STD;
	
	PastWhisperInit();

	return true;
}

bool CPlayer::Load(CUserDB* pUser)
{
	BYTE byRet = 0;

	_AVATOR_DATA* pData = &pUser->m_AvatorData;
	m_id.dwSerial = pUser->m_AvatorData.dbAvator.m_dwRecordNum;//serial은 avator DB 시리얼
	m_pUserDB = pUser;
	m_byUserDgr = pUser->m_byUserDgr;
	m_bySubDgr = pUser->m_bySubDgr;

	m_Param.InitPlayerDB(this);
	m_bMapLoading = false;
	m_bOper = false;
	m_bFullMode = pUser->m_bWndFullMode;
	m_bCheat_100SuccMake = false;
	m_bCheat_Matchless = false;
	m_bFreeRecallWaitTime = false;

	m_bBaseDownload = m_bInvenDownload = m_bForceDownload = m_bCumDownload = m_bSpecialDownload = false;

	//효과장착상태초기화
	m_bUpCheckEquipEffect = true;	//처음 장착아이템은 effect_wait상태이므로 다음루프에서 효과세팅을 한다
	m_bDownCheckEquipEffect = false;
	memset(m_byEffectEquipCode, effect_none, sizeof(m_byEffectEquipCode));

	CMapData* pMap = g_Main.m_MapOper.GetMap(pData->dbAvator.m_byMapCode);
	if(!(pMap && pMap->IsMapIn(pData->dbAvator.m_fStartPos)))
	{
		CMapData* p = g_Main.m_MapOper.GetPosStartMap(pData->dbAvator.m_byRaceSexCode/2, pData->dbAvator.m_fStartPos);
		if(!p)
		{
			g_Main.m_logSystemError.Write("Load() : 실패 : %s.. 시작위치를 못찾음(race:%d, x:%d, y:%d, z:%d)..", 
				pData->dbAvator.m_szAvatorName, pData->dbAvator.m_byRaceSexCode, (int)pData->dbAvator.m_fStartPos[0], (int)pData->dbAvator.m_fStartPos[1], (int)pData->dbAvator.m_fStartPos[2]);
			return false;
		}		
		pData->dbAvator.m_byMapCode = p->m_pMapSet->m_dwIndex;
	}
	pMap = g_Main.m_MapOper.GetMap(pData->dbAvator.m_byMapCode);
	if(!pMap->m_bUse)
	{
		g_Main.m_logSystemError.Write("Load() : 실패 : %s.. 사용중이지 않은 맵(%s)..", pData->dbAvator.m_szAvatorName, pMap->m_pMapSet->m_strCode);
		return false;
	}

	//데이터 언팩
	if(m_Param.ConvertAvatorDB(pData) && 
		m_Param.ConvertGeneralDB(pData))
	{
		m_bLoad = true;
		m_bMapLoading = true;	//클라이언트의 맵로드를 대기..

		//아이템 시리얼 지정
		m_Param.AppointSerialStorageItem();	
		
		//경험치 만분률계산
		m_wExpRate = m_Param.GetExp() / s_dExpLimPerLv[m_Param.GetLevel()] * 10000;

		//각종 파라메터 초기화..(초기화위치..ReCalcMaxHFSP에 영향)
		m_pmMst.Init(&pUser->m_AvatorData.dbStat, m_Param.GetRaceSexCode()/2);//숙련도 초기화
		m_pmTrd.Init();//직거래아이템선택초기화
		m_pmWpn.Init();//소지무기초기화
		m_pmGrg.Init();	//그로기상태 초기화
		m_pmTwr.Init();

		//최대 HFSP계산
		ReCalcMaxHFSP(false);
		if(m_Param.GetHP() > m_nMaxHP)
			m_Param.SetHP(m_nMaxHP);
		if(m_Param.GetFP() > m_nMaxFP)
			m_Param.SetFP(m_nMaxFP);
		if(m_Param.GetSP() > m_nMaxSP)
			m_Param.SetSP(m_nMaxSP);
	}
	else
	{
		g_Main.m_logSystemError.Write("Load() : 실패 : %d:%s", pData->dbAvator.m_dwRecordNum, pData->dbAvator.m_szAvatorName);
		return false;
	}

	return true;
}

bool CPlayer::Create()
{
	if(!m_bLoad)
		return false;

	if(m_bLive)
		return false;

	m_dwObjSerial = m_id.dwSerial;

	_character_create_setdata Data;

	Data.m_pMap = g_Main.m_MapOper.GetMap(m_Param.m_dbChar.m_sStartMapCode);
	Data.m_fStartPos[0] = m_Param.m_dbChar.m_fStartPos[0];
	Data.m_fStartPos[1] = m_Param.m_dbChar.m_fStartPos[1];
	Data.m_fStartPos[2] = m_Param.m_dbChar.m_fStartPos[2];
	Data.m_pRecordSet = g_Main.m_tblPlayer.GetRecord(m_Param.m_dbChar.m_byRaceSexCode);

	if(!CCharacter::Create((_character_create_setdata*)&Data))
	{
		g_Main.m_logSystemError.Write("Create(%s) : 실패.. 위치: Map: %s(%d,%d,%d)", m_Param.GetCharName(), m_pCurMap->m_pMapSet->m_strCode, (int)m_fCurPos[0], (int)m_fCurPos[1], (int)m_fCurPos[2]);
		g_Main.m_Network.Close(client_line, m_ObjID.m_wIndex);
		return false;
	}

	m_bOper = true;
	m_nModeType = mode_type_demili;
	m_nMoveType = move_type_run;
	m_bMineMode = false;

	//version을 setting..
	m_wVisualVer = 1;
	m_dwLastState = 0x00000000;
	m_pCountAttackDst = NULL;
	m_pBeforeDungeonMap = NULL;
	m_nLastBeatenPart = -1;
	m_pUsingUnit = NULL;
	m_pParkingUnit = NULL;
	m_dwUnitViewOverTime = 0xFFFFFFFF;
	m_byUsingWeaponPart = unit_bullet_arms;
	m_pRecalledAnimusItem = NULL;
	m_pRecalledAnimusChar = NULL;
	m_dwLastRecallTime = 0;
	m_byDelayErrorCount = 0;
	m_bNextRecallReturn = false;


	//타이머 초기화..
	m_tmrAlterState.TermTimeRun();
	m_tmrIntervalSec.TermTimeRun();
	m_dwUMWHLastTime = timeGetTime();

	if(m_Param.GetHP() == 0)
		m_bCorpse = true;
	else
		m_bCorpse = false;

	CalcDefTol();//내성계산..
	PastWhisperInit();//과거귓속말상대초기화
	SetHaveEffect();//아이템소지효과검사
	SetStateFlag(true);//상태플래그
	m_QuestMgr.InitMgr(this, &m_Param.m_QuestDB);	//퀘스트관리모듈 초기화..

	m_dwMineDelayTime = 0xFFFFFFFF;

	//아드레날린 포션
	m_dwStartTime = 0;
	m_dwDurTime = 0;
	m_fAFRate = 1.0f;
	m_fDefRate = 1.0f;

	//던젼전의 위치..(안전상 그냥 카피)
	memcpy(m_fBeforeDungeonPos, m_fCurPos, sizeof(float)*3);
	m_pBeforeDungeonMap = m_pCurMap;
	m_bPosMainBase = m_pCurMap->IsBaseTown(m_fCurPos);

	//가이드, 운영 등급은 일단 돈부터 세팅해줌..
	if(m_byUserDgr == USER_DEGREE_GID || m_byUserDgr == USER_DEGREE_MGR)
	{
		int nAlterGold = 10000 - m_Param.GetGold();
		int nAlterDalant = 1000000 - m_Param.GetDalant();

		AlterMoney(nAlterGold, nAlterDalant);
		SendMsg_ExchangeMoneyResult(0);
	}

	if(m_pUserDB)
	{
		m_pUserDB->StartFieldMode();	

		//ITEM & LV HISTORY SET..
		s_MgrItemHistory.GetNewFileName(m_pUserDB->m_dwSerial, m_szItemHistoryFileName);
		s_MgrLvHistory.GetNewFileName(m_pUserDB->m_dwSerial, m_szLvHistoryFileName);

		//ITEM HISTORY..
		s_MgrItemHistory.init_load_item(&m_pUserDB->m_AvatorData, m_szItemHistoryFileName);

		//LV HISTORY..
		s_MgrLvHistory.start_mastery(m_Param.GetCharName(), m_Param.GetLevel(), m_Param.GetExp(), &m_pmMst.m_BaseCum, m_szLvHistoryFileName);
	}

	//경제지표 알림..
	SendMsg_EconomyHistoryInform();
	SendMsg_EconomyRateInform(true);

	s_nLiveNum++;

	_WA_AVATOR_CODE ad;

	memcpy(&ad.m_id, &m_id, sizeof(_CLID));
	strcpy(ad.m_szName, m_Param.GetCharName());

	::wa_EnterWorld(&ad, m_ObjID.m_wIndex);

	return true;
}

void CPlayer::CreateComplete()
{
	//유닛에 CutTime이 설정됬다면 현재시간과 비교해서 5분이내라면 보관, 견인비 탕감 및 해당 유닛을 대기시킴
	CheckUnitCutTime();

	//메신져에 초기 케릭터 모양데이터를 알림
	if(g_Main.m_bMsgrOpen) 
		SendMsg_StartShape_Msgr();

	//전직할 단계가되면 전직을 하라고 알려줌..
	if(m_Param.IsClassChangeableLv())
	{
		SendMsg_ChangeClassCommand();
	}
}

void CPlayer::NetClose()
{
	if(m_bLive)
	{
		DTradeInit();

		//설치한 가드타워를 해체한다..
		_TowerAllReturn();

		SendMsg_Destroy();
		CCharacter::Destroy();

		//소환물이있다면 리턴시킴
		_AnimusReturn();

		//파킹중인 유닛이 있으면 강제 견인
		ForcePullUnit(false, true);

		//던젼맵에서 종료했다면 
		if(m_pCurMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
		{//원래위치로 되돌림..
			m_pCurMap = m_pBeforeDungeonMap;
			memcpy(m_fCurPos, m_fBeforeDungeonPos, sizeof(float)*3);
		}
		//7레벨이하는 마을로 새위치를 잡아주고 종료시킨다.
		if(m_Param.GetLevel() <= 7 && m_Param.GetHP() > 0)
		{
			if(!m_bOper && m_pUserDB)//비정상종료시에는 해주지않는다.
			{
				float fBufPos[3];
				CMapData* pMap = g_Main.m_MapOper.GetPosStartMap(m_Param.GetRaceCode(), fBufPos);
				if(pMap)
				{
					m_pCurMap = pMap;
					memcpy(m_fCurPos, fBufPos, sizeof(float)*3);
				}
			}
		}
		
		m_Param.SetMapCode(m_pCurMap->m_pMapSet->m_dwIndex);
		m_Param.SetCurPos(m_fCurPos);

		ExitUpdateDataToWorld();

		s_nLiveNum--;

		::wa_ExitWorld(&m_id);	

		//ITEM HISTORY..
		if(m_bOper && m_pUserDB)
			s_MgrItemHistory.close("비정상", m_szItemHistoryFileName);
		else
			s_MgrItemHistory.close("정상", m_szItemHistoryFileName);

		//LV HISTORY..
		s_MgrLvHistory.update_mastery(m_Param.GetLevel(), m_Param.GetExp(), &m_pmMst.m_BaseCum, m_Param.m_bAlterMastery, m_szLvHistoryFileName, true);
	}

	m_Param.InitPlayerDB(this);
	m_byUserDgr = USER_DEGREE_STD;

	m_bCorpse = false;
	m_bLoad = false;
	m_bMoveOut = false;
	m_bOper = false;
	m_bMapLoading = false;
	m_bFullMode = false;
	m_pUserDB = NULL;
	m_pUsingUnit = NULL;
	m_pParkingUnit = NULL;
	m_pRecalledAnimusItem = NULL;
	m_pRecalledAnimusChar = NULL;
	m_dwLastRecallTime = 0;
	m_bNextRecallReturn = false;

	m_bBaseDownload = true;
	m_bInvenDownload = true;
	m_bForceDownload = true;
	m_bCumDownload = true;
	m_bSpecialDownload = true;

	m_id.dwSerial = 0xFFFFFFFF;
	m_dwObjSerial = 0xFFFFFFFF;
}

bool CPlayer::Corpse(CCharacter* pAtter)
{
	CCharacter::Stop();
	DTradeInit();
	CCharacter::SFContInit();
	
	m_dwMineDelayTime = 0xFFFFFFFF;//채굴초기화
	m_bMineMode = false;

	m_pmGrg.EndGroggy();//그로기초기화

	SendMsg_Die();

	if(pAtter)
	{
		if(m_Param.GetExp() > 0 && pAtter->m_ObjID.m_byID == obj_id_monster)
		{	//몬스터에게 당한거라면 경험치를 깍는다.********************************************
			if(m_Param.GetLevel() > 7 && m_Param.GetLevel() < max_level && m_rtPer100.GetRand()%2)
			{
				float fLvGap = abs(m_Param.GetLevel()-pAtter->GetLevel()) + 5;				
				fLvGap = min(fLvGap, 20);
				fLvGap *= 0.01f;

				double dCurExp = m_Param.GetExp();
				float fDecRate = (float)(::rand()%100)/100;
				double dAlterExp = dCurExp * fLvGap * fDecRate;
				dAlterExp = max(dAlterExp, 0);			//0보단 커야하며..
				dAlterExp = min(dAlterExp, dCurExp);	//현재 경험치보다는 작아야한다..
				if(dAlterExp > 1)
				{
					AlterExp(-dAlterExp);

					//LV HISTORY..	
					s_MgrLvHistory.down_exp(dCurExp, m_Param.GetExp(), m_szLvHistoryFileName);
				}
			
			/*	dCurExp -= (dCurExp * fLvGap * fDecRate);
				dCurExp = max(dCurExp, 0);
				m_Param.SetExp(dCurExp);
				m_wExpRate = m_Param.GetExp() / s_dExpLimPerLv[m_Param.GetLevel()] * 10000;*/

				
			}
//			SendMsg_PartyExpDivision();
		//**********************************************************************************
		}
	}

	//소환물이있다면 딜레이 리턴시킴
	if(m_pRecalledAnimusChar)
		s_AnimusReturnDelay.Push(m_ObjID.m_wIndex, m_dwObjSerial);

	m_bCorpse = true;
	m_bMineMode = false;
	m_nModeType = mode_type_demili;
	m_nMoveType = move_type_run;

	//던전에서 죽은거라면 유저메시지를 기다림없이 곧장 마을로..
	if(m_pCurMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
		pc_Revival();
	
	return true;
}

void CPlayer::Resurrect()		//부활
{
	BYTE byErrCode = 0;//1;//시체상태가아님 2;//맵을못읽음

	//시체상태인지 확인
	if(!m_bCorpse)
	{
		byErrCode = 1;//시체상태가아님
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//소환물이있다면 리턴시킴
		_AnimusReturn();

		m_bCorpse = false;
		m_nModeType = mode_type_demili;
		m_nMoveType = move_type_run;

		SetHP(GetLimHP(), true);
		SetFP(GetLimFP(), true);
		SetSP(GetLimSP(), true);
	}
	SendMsg_Resurrect(byErrCode);	
}

bool CPlayer::OutOfMap(CMapData* pIntoMap, WORD wLayerIndex, BYTE byMapOutType, float* pfStartPos)
{
	//현재맵데이터가 바뀌므로 패킷을 먼저 쌓는다.
	SendMsg_MapOut(byMapOutType, pIntoMap->m_pMapSet->m_dwIndex);

	m_pCurMap->ExitMap(this, GetCurSecNum());
	SetCurSecNum(0xFFFFFFFF);

	if(pIntoMap != m_pCurMap)
	{
		if(byMapOutType != mapin_type_dungeon)//던젼으로 가는것은 보내지 않는다
		{
			//월드에 업데이트..	
			if(m_pUserDB)
				m_pUserDB->Update_Map(pIntoMap->m_pMapSet->m_dwIndex, pfStartPos);
		}
	}

	if(s_pSelectObject == this)
		s_pSelectObject = NULL;	

	//반격기해제
	if(m_EP.m_bEff_State[_EFF_STATE::Res_Att])//PARAMETER EDIT (반격기) 
		RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Res_Att);

	m_pCurMap = pIntoMap;	//이동할 맵과, 좌표만 저장한다음.. mapin에서 리스트에 추가시킨다.
	m_wMapLayerIndex = wLayerIndex;
	m_Param.SetMapCode(pIntoMap->m_pMapSet->m_dwIndex);
	SetCurPos(pfStartPos);
	memcpy(m_fTarPos, m_fCurPos, sizeof(float)*3);
	m_bMove = false;

	return true;
}

bool CPlayer::IntoMap(BYTE byMapInMode)
{
	//던젼이라면.. active가아아니면 
	if(m_pCurMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
	{//해당레이어가 죽어있으면 다시 밖으로..
		if(!m_pCurMap->m_ls[m_wMapLayerIndex].IsActiveLayer())
			return false;
	}

	DWORD dwNewSec = CalcSecIndex();
	if(dwNewSec >= m_pCurMap->GetSecInfo()->m_nSecNum)
	{
		if(m_pUserDB)
		{
			m_pUserDB->ForceCloseCommand(false, 0);
			g_Main.m_logSystemError.Write("%s 강제종료: 섹터오류 (맵:%s, 위치:%d, %d)", m_Param.GetCharName(), m_pCurMap->m_pMapSet->m_strCode, (int)m_fCurPos[0], (int)m_fCurPos[2]);
		}
		return false;
	}

	m_pCurMap->EnterMap(this, dwNewSec);
	
	SetCurSecNum(dwNewSec);

	SendData_PartyMemberPos();//파티원이라면 파티원에게 위치를 알린다..

	return true;
}

void CPlayer::CalcDefTol()
{
	memset(m_dwTolValue, 0, sizeof(m_dwTolValue));

	if(!IsRidingUnit())	//필드상태..
	{	
		//장착
		for(int i = 0; i < base_fix_num; i++)
		{
			__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[i];
			if(pItem->m_bLoad)
			{
				_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[i].GetRecord(pItem->m_wItemIndex);
				float* pfTol = &pFld->m_fFireTol;
				for(int t = 0 ; t < TOL_CODE_NUM; t++)
				{
					m_dwTolValue[t] += pfTol[t];
				}
			}
		}

		//반지, 목걸이
		for(i = 0; i < embellish_fix_num; i++)
		{
			__ITEM* pItem = &m_Param.m_dbEmbellish.m_pStorageList[i];
			if(!pItem->m_bLoad)
				continue;
			
			float* pfTol = NULL;
			if(pItem->m_byTableCode == tbl_code_ring)
			{
				_RingItem_fld* pFld = (_RingItem_fld*)g_Main.m_tblItemData[i].GetRecord(pItem->m_wItemIndex);
				if(!pFld)
					continue;
				pfTol = &pFld->m_fFireTol;
			}
			else if(pItem->m_byTableCode == tbl_code_amulet)
			{
				_AmuletItem_fld* pFld = (_AmuletItem_fld*)g_Main.m_tblItemData[i].GetRecord(pItem->m_wItemIndex);
				if(!pFld)					
					continue;					
				pfTol = &pFld->m_fFireTol;	
			}
			else
				continue;

			if(pfTol)
			{
				for(int t = 0 ; t < TOL_CODE_NUM; t++)
					m_dwTolValue[t] += pfTol[t];
			}
		}
	}
	else	//유닛상태..
	{
		for(int i = 0; i < UNIT_PART_NUM; i++)
		{
			_UnitPart_fld* pPartFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[i].GetRecord(m_pUsingUnit->byPart[i]);
			if(pPartFld)
			{
				int* pTol = &pPartFld->m_nFireTol;
				for(int i = 0; i < TOL_CODE_NUM; i++)
					m_dwTolValue[i] += pTol[i];
			}
		}
	}
}

void CPlayer::SetEquipEffect(_STORAGE_LIST::_storage_con* pItem, bool bEquip)
{	
	_ITEM_EFFECT* pEffect = _GetItemEffect((__ITEM*)pItem);
	if(!pEffect)
		return;

	int t;
	for(int i = 0; i < MAX_EQUIP_EFFECT_KIND; i++)
	{
		switch(pEffect[i].nEffectCode)
		{
		case 1: //SP 최대량 증가 
			m_EP.SetEff_Rate( _EFF_RATE::SP_Mx, pEffect[i].fEffectValue, bEquip );
			ReCalcMaxHFSP(true);
			break;

		case 2: //스킬 포스 사용시 FP 소모량 감소
			m_EP.SetEff_Rate( _EFF_RATE::FP_Consum, pEffect[i].fEffectValue, bEquip );
			break;

		case 3:	//일반공격 명중률 증가
			for(t = 0; t < WP_CLASS_NUM; t++)
				m_EP.SetEff_Plus( _EFF_PLUS::GE_Hit_+t, pEffect[i].fEffectValue, bEquip );
			break;

		case 4: //일반공격 회피율 증가 
			m_EP.SetEff_Plus( _EFF_PLUS::GE_Avd, pEffect[i].fEffectValue, bEquip );
			break;

		case 5:	//HP FP 최대량 증가
			m_EP.SetEff_Rate( _EFF_RATE::HP_Mx, pEffect[i].fEffectValue, bEquip );
			m_EP.SetEff_Rate( _EFF_RATE::FP_Mx, pEffect[i].fEffectValue, bEquip );
			ReCalcMaxHFSP(true);
			break;
	
		case 6:	//모든 공격력 증가 
			for(t = 0; t < WP_CLASS_NUM; t++)
				m_EP.SetEff_Rate( _EFF_RATE::GE_AttFc_+t, pEffect[i].fEffectValue, bEquip );
			m_EP.SetEff_Rate( _EFF_RATE::FC_AttFc, pEffect[i].fEffectValue, bEquip );		
			m_EP.SetEff_Rate( _EFF_RATE::SK_AttFc, pEffect[i].fEffectValue, bEquip );		
			break;

		case 7:	//방어력 증가    
			m_EP.SetEff_Rate( _EFF_RATE::Part_Def, pEffect[i].fEffectValue, bEquip );
			break;

		case 8:	//사용 가능한 스킬레벨 상승  
			m_EP.SetEff_Plus( _EFF_PLUS::SK_LvUp, pEffect[i].fEffectValue, bEquip );					
			break;

		case 9:	//투명(착용하고 있는동안 파티/길드를 제외한 다른사람들에게 안보임)
			m_EP.SetEff_Plus( _EFF_PLUS::Transparency, 1, bEquip );		
			break;

		case 10://디텍트(스텔스포스 사용자나 스텔스아이템 사용한 플레이어들 볼 수 있음)
			m_EP.SetEff_Plus( _EFF_PLUS::Detect, 1, bEquip );		
			break;

		case 11://지속피해보조기술 해제(착용하고 있는동안에는 걸려도 즉시 해제됨)
			m_EP.SetEff_Plus( _EFF_PLUS::Anti_CtDm, 1, bEquip );		
			break;

		case 12://이동속도_증가
			m_EP.SetEff_Plus( _EFF_PLUS::Move_Run_Spd, pEffect[i].fEffectValue, bEquip );		
			break;

		case 13://상대아바타의 약한부위보기
			m_EP.SetEff_Plus( _EFF_PLUS::Know_WeakPoint, 1, bEquip );
			break;

		case 14:	//fp 회복속도
			m_EP.SetEff_Plus( _EFF_PLUS::FP_Rev_Add, pEffect[i].fEffectValue, bEquip );
			break;

		case 15:	//포스 공격력 증가
			m_EP.SetEff_Rate( _EFF_RATE::FC_AttFc, pEffect[i].fEffectValue, bEquip );
			break;

		case 16:	//최대 fp량
			m_EP.SetEff_Rate( _EFF_RATE::FP_Mx, pEffect[i].fEffectValue, bEquip );
			ReCalcMaxHFSP(true);
			break;

		}
	}

	if(::GetDefItemUpgSocketNum(pItem->m_byTableCode, pItem->m_wItemIndex) > 0)
	{
		DWORD dwUdtInfo_Buf;
		memcpy(&dwUdtInfo_Buf, &pItem->m_dwLv, sizeof(DWORD));
		BYTE byLv = ::GetItemUpgedLv(pItem->m_dwLv);

		for(int i = 0 ; i < byLv; i++)
		{			
			BYTE byUdtInfo = (BYTE)((dwUdtInfo_Buf>>(i*4))&0x0000000F);
			if(byUdtInfo != __NO_TALIK)//탈릭발견..
			{
				int nNum = 1;
				//그 박힌탈릭이 몇개인가..
				for(int t = i+1; t < byLv; t++)
				{
					BYTE byTemp = (BYTE)((dwUdtInfo_Buf>>(t*4))&0x0000000F);
					if(byUdtInfo == byTemp)
					{
						nNum++;
						dwUdtInfo_Buf |= 0x0000000F<<(t*4);//갯수를 센것은 빵구를내서 담에는 그냥 넘어가게함..
					}
				}

				_ItemUpgrade_fld* pUptFld = (_ItemUpgrade_fld*)g_Main.m_tblItemUpgrade.GetRecord(byUdtInfo);

				float fEffectValue = ((float*)&pUptFld->m_fUp1)[nNum-1];

				switch(byUdtInfo)
				{
				case talik_type_fc: //무지의_탈릭..공격력증가
					for(t = 0; t < WP_CLASS_NUM; t++)
						m_EP.SetEff_Rate( _EFF_RATE::GE_AttFc_+t, fEffectValue, bEquip );
					m_EP.SetEff_Rate( _EFF_RATE::FC_AttFc, fEffectValue, bEquip );		
					m_EP.SetEff_Rate( _EFF_RATE::SK_AttFc, fEffectValue, bEquip );		
					break;

				case talik_type_hp: //파멸의_탈릭..HP흡수도
					m_EP.SetEff_Rate( _EFF_RATE::HP_Absorb, fEffectValue, bEquip );
					break;

				case talik_type_fp: //암흑의_탈릭..FP흡수도
					m_EP.SetEff_Rate( _EFF_RATE::FP_Absorb, fEffectValue, bEquip );
					break;

				case talik_type_stun:	//혼돈의_탈릭..Melee공격 Stun 성공확률 
					m_EP.SetEff_Rate( _EFF_RATE::GE_Stun, fEffectValue, bEquip );
					break;

				case talik_type_dist: //증오의_탈릭..사정거리
					m_EP.SetEff_Plus( _EFF_PLUS::GE_Att_Dist_+wp_class_long, fEffectValue, bEquip );
					//m_EP.SetEff_Plus( _EFF_PLUS::GE_Att_Dist_+wp_class_throw, fEffectValue, bEquip );
				//	m_EP.SetEff_Plus( _EFF_PLUS::GE_Att_Dist_+wp_class_launcher, fEffectValue, bEquip );
					break;

				case talik_type_def:	//은총의_탈릭..방어력 
					m_EP.SetEff_Rate( _EFF_RATE::Part_Def, fEffectValue, bEquip );
					break;
			
				case talik_type_contdam:	//지혜의_탈릭..지속피해보조시간 단축 
					m_EP.SetEff_Rate( _EFF_RATE::FC_CtDm_Tm, fEffectValue, bEquip );
					break;

				case talik_type_fire:	//정화의_탈릭..불내성증가
					if(pItem->m_byTableCode != tbl_code_weapon)
						m_EP.SetEff_Plus( _EFF_PLUS::Part_Tol_+TOL_CODE_FIRE, fEffectValue, bEquip );		
					break;

				case talik_type_water:	//신념의_탈릭..물내성증가 
					if(pItem->m_byTableCode != tbl_code_weapon)
						m_EP.SetEff_Plus( _EFF_PLUS::Part_Tol_+TOL_CODE_WATER, fEffectValue, bEquip );		
					break;

				case talik_type_soil:	//수호의_탈릭..흙내성증가
					if(pItem->m_byTableCode != tbl_code_weapon)
						m_EP.SetEff_Plus( _EFF_PLUS::Part_Tol_+TOL_CODE_SOIL, fEffectValue, bEquip );		
					break;

				case talik_type_wind://광휘의_탈릭..바람내성증가
					if(pItem->m_byTableCode != tbl_code_weapon)
						m_EP.SetEff_Plus( _EFF_PLUS::Part_Tol_+TOL_CODE_WIND, fEffectValue, bEquip );		
					break;

				case talik_type_rev://은혜의_탈릭..HP 회복량 증가
					m_EP.SetEff_Rate( _EFF_RATE::HP_Rev, fEffectValue, bEquip );
					break;

				case talik_type_avd://자비의_탈릭..회피율 
					m_EP.SetEff_Plus( _EFF_PLUS::GE_Avd, fEffectValue, bEquip );		
					break;
				}				
			}
		}
	}	
}


void CPlayer::SetHaveEffect()
{
	//Equip_Lv_Up 제이드 관리..
	float fEqLvupBuffer = m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up];

	//소지효과변수 초기화..
	m_EP.m_fEff_Have[_EFF_HAVE::Gamble_Prof] = 0;
	m_EP.m_fEff_Have[_EFF_HAVE::Trade_Prof] = 0;
	m_EP.m_fEff_Have[_EFF_HAVE::Exp_Prof] = 0;
	m_EP.m_fEff_Have[_EFF_HAVE::Chat_All_Race] = 0;
	m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up] = 0;

	if(IsRidingUnit())//유닛탑승상태라면 소지효과를 안먹인다..
		return;

	for(int i = 0; i < m_Param.GetBagNum()*one_bag_store_num; i++)
	{
		__ITEM* pItem = &m_Param.m_dbInven.m_pStorageList[i];
		if(pItem->m_bLoad)
		{
			if(pItem->m_byTableCode != tbl_code_res)
				continue;

			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);

			if(pFld->m_nEffCode != -1 && pFld->m_nEffCode < _EFF_HAVE::NUM)//효과가 있는 아이템이라면..
			{
				m_EP.m_fEff_Have[pFld->m_nEffCode] += (pFld->m_fEffUnit*pItem->m_dwDur);
				if(m_EP.m_fEff_Have[pFld->m_nEffCode] > pFld->m_fEffUnitMax)
					m_EP.m_fEff_Have[pFld->m_nEffCode] = pFld->m_fEffUnitMax;
			}			
		}
	}

	//소지효과변수 마무리..
	m_EP.m_fEff_Have[_EFF_HAVE::Gamble_Prof] += 1.0f;
	m_EP.m_fEff_Have[_EFF_HAVE::Exp_Prof] += 1.0f;

	//제이드 수량이 변경되면 장착효과처리..
	if(fEqLvupBuffer < m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up])
		m_bUpCheckEquipEffect = true;
	else if(fEqLvupBuffer > m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up])
		m_bDownCheckEquipEffect = true;
}

void CPlayer::PastWhisperInit()
{
	for(int i = 0; i < max_memory_past_whisper; i++)
		m_PastWhiper[i].bMemory = false;
}

void CPlayer::DTradeInit()
{
	if(m_pmTrd.bDTradeMode)
	{
		CPlayer* pDst = NULL;
		if(::DTradeEqualPerson(this, &pDst))		
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCancleInform();
		}

		m_pmTrd.Init();
		SendMsg_DTradeCancleInform();
	}
}

void CPlayer::UpdateVisualVer()
{
	m_wVisualVer++;
}

int CPlayer::GetVisualVer()
{
	return m_wVisualVer;
}

float CPlayer::GetAdrenAF()
{
	if(m_dwDurTime == 0)
		return 1.0f;

	return (1.0f + m_fAFRate);
}

float CPlayer::GetAdrenDef()
{
	if(m_dwDurTime == 0)
		return 1.0f;

	return (1.0f - m_fDefRate);
}


/*
전부해제시킬때..
자신이 죽었을때..
자신이 접속을 종료했을때..
FP가 0일때..
맵을 나갔을때..
*/

void CPlayer::Loop()
{
	if(!m_bOper)
		return;

	DWORD dwCurTime = timeGetTime();

	SenseState();
	CCharacter::UpdateSFCont();

	if(m_pCountAttackDst)
	{
		pc_PlayAttack_Gen(m_pCountAttackDst, GetAttackRandomPart(), 0xFFFF, true);
		m_pCountAttackDst = NULL;
	}

	//아드레날린 포션
	if(m_dwDurTime > 0)
	{
		DWORD dwFlow = dwCurTime - m_dwStartTime;

		if(dwFlow > m_dwDurTime)
			m_dwDurTime = 0;
		else
			m_dwDurTime -= dwFlow;

		if(m_dwDurTime == 0)
		{
			m_fAFRate = 1.0f;
			m_fDefRate = 1.0f;
		}
	}

	//장착효력..
	if(m_bUpCheckEquipEffect)
	{
		for(int i = 0; i < equip_fix_num + embellish_fix_num; i++)
		{
			if(m_byEffectEquipCode[i] != effect_wait)
				continue;

			__ITEM* pItem;
			if(i < equip_fix_num)
				pItem = &m_Param.m_dbEquip.m_pStorageList[i];
			else
				pItem = &m_Param.m_dbEmbellish.m_pStorageList[i-equip_fix_num];

			if(IsEffectableEquip(pItem))
			{
				SetEquipEffect(pItem, true);
				m_byEffectEquipCode[i] = effect_apply;
				if(pItem->m_byTableCode == tbl_code_weapon)
				{
					if(!IsRidingUnit())				//유닛탑승상태가 아니라면
						m_pmWpn.FixWeapon(pItem);					//무기를 장착시킨다.
				}
			}
		}

		m_bUpCheckEquipEffect = false;
	}

	if(m_bDownCheckEquipEffect)
	{
		for(int i = 0; i < equip_fix_num+embellish_fix_num; i++)
		{
			if(m_byEffectEquipCode[i] != effect_apply)
				continue;

			__ITEM* pItem;
			if(i < equip_fix_num)
				pItem = &m_Param.m_dbEquip.m_pStorageList[i];
			else
				pItem = &m_Param.m_dbEmbellish.m_pStorageList[i-equip_fix_num];

			if(!IsEffectableEquip(pItem))
			{
				SetEquipEffect(pItem, false);
				m_byEffectEquipCode[i] = effect_wait;
				if(pItem->m_byTableCode == tbl_code_weapon)
				{
					if(!IsRidingUnit())	//유닛탑승상태가 아니라면
						m_pmWpn.FixWeapon(NULL);					//무기를 탈착시킨다.
				}
			}
		}

		m_bDownCheckEquipEffect = false;
	}

	//애니머스 귀환 요청 처리
	if(m_bNextRecallReturn)
	{
		_AnimusReturn();
		m_bNextRecallReturn = false;
	}

	//주차중인 유닛의 강제 견인 체크..
	_CheckForcePullUnit();

	//그로그생태체크
	m_pmGrg.CheckGroggy(dwCurTime);

	if(m_tmrIntervalSec.CountingTimer())
	{
		//위치백업
		if(m_pCurMap && m_pUserDB)
		{
			if(m_pCurMap->m_pMapSet->m_nMapType == MAP_TYPE_STD)
			{	//일반맵일때만..
				m_pUserDB->Update_Map(m_pCurMap->m_pMapSet->m_dwIndex, m_fCurPos, false);
			}
		}

		//마을에 있는지 체크..
		m_bPosMainBase = m_pCurMap->IsBaseTown(m_fCurPos);

		//변경 마스터리 히스터리로그
		UpdatedMasteryWriteHistory();
	}
}

void CPlayer::AlterSec()
{
	if(m_pPartyMgr->IsPartyMode())
	{
		SendData_PartyMemberPos();
	}
}
/*
void CPlayer::OutOfSector()
{
	//위치새로잡아주고 바로종료..
	CMapData* pMap = g_Main.m_MapOper.GetPosStartMap(m_Param.GetRaceCode(), m_fCurPos);
	m_Param.SetMapCode(pMap->m_pMapSet->m_dwIndex);

	g_Main.m_logSystemError.Write("CPlayer::OutOfSector() : %s.. Close", m_Param.GetCharName());
	g_Main.m_Network.Close(client_line, m_ObjID.m_wIndex, false);
}
*/
void CPlayer::SetLevel(BYTE byNewLevel)
{
	BYTE byOldLv = m_Param.GetLevel();
	
	if(byOldLv < byNewLevel)	//실제로 레벨이 오른경우.. 같거나 떨어진경우는 치트..
	{
		//월드에 업데이트..	
		if(m_pUserDB)
			m_pUserDB->Update_Level(byNewLevel, m_Param.GetExp());
	
		m_Param.SetLevel(byNewLevel);

		//LV HISTORY..
		s_MgrLvHistory.upgrade_lv(byNewLevel, m_szLvHistoryFileName);
	}

	if(byOldLv < byNewLevel)
		m_bUpCheckEquipEffect = true;
	else if(byOldLv > byNewLevel)
		m_bDownCheckEquipEffect = true;
	
	ReCalcMaxHFSP();

	SetHP(GetLimHP(), true);
	SetFP(GetLimFP(), true);
	SetSP(GetLimSP(), true);

	SendMsg_Level(byNewLevel);	
	SendData_PartyMemberLv();

	//전직할 단계가되면 전직을 하라고 알려줌..
	if(m_Param.IsClassChangeableLv())
	{
		SendMsg_ChangeClassCommand();
	}
}

void CPlayer::AlterMoney(int nAlterGold, int nAlterDalant)
{
	DWORD dwGold = m_Param.GetGold();
	DWORD dwDalant = m_Param.GetDalant();

	if(nAlterGold > 0)//증가시..
	{
		if(m_Param.GetGold() > m_Param.GetGold() + nAlterGold)//over..
			dwGold = 0xFFFFFFFF;
		else
			dwGold = m_Param.GetGold() + nAlterGold;
	}
	else if(nAlterGold < 0)	//감소시..
	{
		if(m_Param.GetGold() < m_Param.GetGold() + nAlterGold)//over..
			dwGold = 0;
		else
			dwGold = m_Param.GetGold() + nAlterGold;
	}

	if(nAlterDalant > 0)//증가시..
	{
		if(m_Param.GetDalant() > m_Param.GetDalant() + nAlterDalant)//over..
			dwDalant = 0xFFFFFFFF;
		else
			dwDalant = m_Param.GetDalant() + nAlterDalant;
	}
	else if(nAlterDalant < 0)	//감소시..
	{
		if(m_Param.GetDalant() < m_Param.GetDalant() + nAlterDalant)//over..
			dwDalant = 0;
		else
			dwDalant = m_Param.GetDalant() + nAlterDalant;
	}

	if(m_Param.GetGold() != dwGold || m_Param.GetDalant() != dwDalant)
	{
		m_Param.SetGold(dwGold);
		m_Param.SetDalant(dwDalant);

		//월드에 업데이트..	
		if(m_pUserDB)
			m_pUserDB->Update_Money(m_Param.GetDalant(), m_Param.GetGold());
	}
}

DWORD CPlayer::GetMoney(BYTE byMoneyCode)
{
	if(byMoneyCode == money_unit_dalant)
		return m_Param.GetDalant();

	return m_Param.GetGold();
}

void CPlayer::AutoRecover()
{
	if(IsRidingUnit())	//유닛탑승중에는 회복불가
		return; 

	if(m_pmGrg.GetGroggy())
		return;			//그로기상태 회복불가

	if(m_EP.GetEff_State(_EFF_STATE::Rev_Lck))  //PARAMETER EDIT (회복잠김)
		return;

	int nCurHP = GetHP();
	int nCurFP = GetFP();
	int nCurSP = GetSP();

	int nAlterHP = 0;
	int nAlterFP = 0;
	int nAlterSP = 0;

	//nAlterHP..Setting..
	//over됀 상태에서는 회복시키지않는다.

	//HP
	if(nCurHP < GetLimHP())
	{
		if(!m_bMove)//정지상태는 4..
			nAlterHP += (4 * m_EP.GetEff_Rate(_EFF_RATE::HP_Rev));//PARAMETER EDIT (HP_회복율)
		else if(m_nMoveType == move_type_walk)//걷기상태는 1..
			nAlterHP += (2 * m_EP.GetEff_Rate(_EFF_RATE::HP_Rev));

		if(m_Param.GetLevel() > 7 && (float)nCurHP/GetLimHP() < 0.0f)//7레벨초과이며 현HP가 30%이하면 회복하지않는다.
			nAlterHP = 0;
	}

	//FP
	if(nCurFP < GetLimFP())
	{
		if(!m_bMove)//정지상태는 2..
			nAlterFP += (2 * m_EP.GetEff_Rate(_EFF_RATE::FP_Rev) + m_EP.GetEff_Plus(_EFF_PLUS::FP_Rev_Add)/100);//PARAMETER EDIT (FP_회복율)
		else if(m_nMoveType == move_type_walk)//걷기상태는 1..
			nAlterFP += (1 * m_EP.GetEff_Rate(_EFF_RATE::FP_Rev) + m_EP.GetEff_Plus(_EFF_PLUS::FP_Rev_Add)/100);//100으로 나눔..
	}

	//SP
//	if(nCurSP < GetLimSP())
	{
		float fEquipSpeedRate = CalcEquipSpeed();

		if(!m_bMove)	//정지상태는 2
			nAlterSP += (2 *  m_EP.GetEff_Rate(_EFF_RATE::SP_Rev));//PARAMETER EDIT (SP_회복율)
		else if(m_bMove && m_nMoveType == move_type_walk)	//걷기상태는 1..
			nAlterSP += (1 *  m_EP.GetEff_Rate(_EFF_RATE::SP_Rev));
		else if(m_bMove && m_nMoveType == move_type_run && !m_bPosMainBase)	//뛰기중이고 마을에 있는상태가 아닐때..
			nAlterSP -= (2 * fEquipSpeedRate);
	}

	if(nAlterHP != 0)
		SetHP(nCurHP+nAlterHP, false);

	if(nAlterFP != 0)
		SetFP(nCurFP+nAlterFP, false);			

	if(nAlterSP != 0)	
		SetSP(nCurSP+nAlterSP, false);

	if(nCurHP != GetHP() || nCurFP != GetFP() || nCurSP != GetSP())
	{
		SendMsg_Recover();
	}
}

void CPlayer::AutoRecover_Animus()
{
	static int nRecoverHPPoint = 8;
	static int nRecoverFPPoint = 4;
	
	for(int i = 0; i < animus_storage_num; i++)
	{	
		__ITEM* p = &m_Param.m_dbAnimus.m_pStorageList[i];

		if(!p->m_bLoad || p == m_pRecalledAnimusItem)
			continue;

		_animus_fld* pFld = ::GetAnimusFldFromExp(i, p->m_dwDur);
		if(!pFld)
			continue;//있을수 없는 에러..

		//파라메터가 max치까지 증가..
		DWORD dwBuffer = p->m_dwLv;
		_animus_param* pParam = (_animus_param*)&p->m_dwLv;

		if(pParam->wHP < pFld->m_nMaxHP)
		{
			pParam->wHP += nRecoverHPPoint;
			if(pParam->wHP > pFld->m_nMaxHP)
				pParam->wHP = pFld->m_nMaxHP;
		}
		
		if(pParam->wFP < pFld->m_nMaxFP)
		{
			pParam->wFP += nRecoverFPPoint;
			if(pParam->wFP > pFld->m_nMaxFP)
				pParam->wFP = pFld->m_nMaxFP;
		}

		if(dwBuffer != p->m_dwLv)//월드에 업데이트
			m_pUserDB->Update_ItemUpgrade(_STORAGE_POS::ANIMUS, i, p->m_dwLv, false);
	}
}

void CPlayer::AutoCharge_Booster()
{
	if(!m_pUsingUnit)
		return;

	_UnitPart_fld* pBackFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_back].GetRecord(m_pUsingUnit->byPart[unit_part_back]);
	if(!pBackFld)
		return;

	if(pBackFld->m_nBstCha == 0)
		return;

	int nCur = m_pUsingUnit->wBooster;
	int nAlter = 0;

	if(!m_bMove || m_pParkingUnit)	//정지상태나 유닛 파킹상태
		nAlter = 2;
	else if(m_bMove && m_nMoveType == move_type_run)	//뛰기중
		nAlter = -2;

	if(nAlter != 0)
	{
		int nNewBooster = nCur + nAlter;

		if(nNewBooster < 0)	
			nNewBooster = 0;
		else if(nNewBooster > pBackFld->m_nBstCha)	
			nNewBooster = pBackFld->m_nBstCha;

		m_pUsingUnit->wBooster = nNewBooster;

		if(nCur != nNewBooster)
			SendMsg_AlterBooster();
	}
}

float CPlayer::CalcEquipSpeed()
{
	float fSpeedRate = 1.0f;

	for(int i = 0; i < base_fix_num; i++)
	{//기본방어구..
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[i];

		if(pItem->m_bLoad)
		{		
			_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[i].GetRecord(pItem->m_wItemIndex);
			fSpeedRate *= pFld->m_fEquipSpeed;
		}
	}

	{//무기..
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pItem->m_bLoad)
		{		
			_WeaponItem_fld* pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pItem->m_wItemIndex);
			fSpeedRate *= pFld->m_fEquipSpeed;
		}
	}

	{//방패..
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[tbl_code_shield];
		if(pItem->m_bLoad)
		{		
			_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[tbl_code_shield].GetRecord(pItem->m_wItemIndex);
			fSpeedRate *= pFld->m_fEquipSpeed;
		}
	}

	return fSpeedRate;
}

int	CPlayer::CalcEquipAttackDelay()
{
	int nSpeed = 0;

	//기본장착파트
	for(int i = 0; i < base_fix_num; i++)
	{
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[i];

		if(pItem->m_bLoad)
		{		
			_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[i].GetRecord(pItem->m_wItemIndex);
			nSpeed += pFld->m_nGASpd;
		}
	}

	//무기..
	{
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pItem->m_bLoad)
		{		
			_WeaponItem_fld* pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pItem->m_wItemIndex);
			nSpeed += pFld->m_nGASpd;
		}
	}

	//방패..
	{
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[tbl_code_shield];
		if(pItem->m_bLoad)
		{		
			_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[tbl_code_shield].GetRecord(pItem->m_wItemIndex);
			nSpeed += pFld->m_nGASpd;
		}
	}

	return nSpeed;
}

bool CPlayer::IsOutExtraStopPos(float* pfStopPos)
{
	if(::GetSqrt(m_fTarPos, pfStopPos) > extra_stoppos_gap)
		return true;
	return false;
}

void CPlayer::SenseState()
{
	if(m_tmrAlterState.CountingTimer())
	{
		//State..
		DWORD dwCopyState = GetStateFlag();
		SetStateFlag(true);
		if(dwCopyState != m_dwLastState)
		{
			SendMsg_StateInform(m_dwLastState);
		}
	}
}

bool CPlayer::IsEffectableEquip(_STORAGE_LIST::_storage_con* pCon)
{
	//레벨제한
	BYTE byItemLv = ::GetItemEquipLevel(pCon->m_byTableCode, pCon->m_wItemIndex);
	if(byItemLv	> m_Param.GetLevel() + m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up]) //PARAMETER EDIT (자신의 레벨보다 상위의 아이템을 장착할수있음)
	{
	//	g_Main.m_logSystemError.Write("%s: IsEffectableEquip.. lv(%d) < itemlv(%d)", m_Param.GetCharName(), m_Param.GetLevel(), byItemLv);
		return false;
	}

	//숙련도제한
	int nLimNum = 0;
	_EQUIP_MASTERY_LIM* pML = ::GetItemEquipMastery(pCon->m_byTableCode, pCon->m_wItemIndex, &nLimNum);
	if(!pML)	
		return true;

	for(int i = 0; i < nLimNum; i++)
	{
		int code = pML[i].nMasteryCode;

		if(code == equip_mastery_nothing)
			continue;

		if(m_pmMst.GetEquipMastery(code) < pML[i].nLimMastery)
		{
	//		char* pItemName = ::GetItemKorName(pCon->m_byTableCode, pCon->m_wItemIndex, &g_Main.m_tblItemData[pCon->m_byTableCode]);
	//		g_Main.m_logSystemError.Write("%s: IsEffectableEquip.. %s) item mastery(%d - %d).. > mastery(%d)", 
	//			m_Param.GetCharName(), pItemName, code, pML[i].nLimMastery, m_pmMst.GetEquipMastery(code));
			return false;
		}
	}

	return true;
}

void CPlayer::SetEffectEquipCode(BYTE byStorageCode, BYTE bySlotIndex, BYTE byCode)
{
	BYTE* pbyCoder = NULL;
	if(byStorageCode == _STORAGE_POS::EQUIP)
	{
		pbyCoder = &m_byEffectEquipCode[bySlotIndex];
	}
	else
	{
		pbyCoder = &m_byEffectEquipCode[equip_fix_num + bySlotIndex];
	}

	*pbyCoder = byCode;
}

BYTE CPlayer::GetEffectEquipCode(BYTE byStorageCode, BYTE bySlotIndex)
{
	BYTE* pbyCoder = NULL;
	if(byStorageCode == _STORAGE_POS::EQUIP)
	{
		pbyCoder = &m_byEffectEquipCode[bySlotIndex];
	}
	else
	{
		pbyCoder = &m_byEffectEquipCode[equip_fix_num + bySlotIndex];
	}

	return *pbyCoder;
}

void CPlayer::Emb_AddStorage(BYTE byStorageCode, _STORAGE_LIST::_storage_con* pCon)
{
	if(byStorageCode >= CPlayerDB::total_storage_num)
		return;

	DWORD dwStorageIndex = m_Param.m_pStoragePtr[byStorageCode]->TransInCon(pCon);
	if(dwStorageIndex == __NSI)
	{
		g_Main.m_logSystemError.Write("%s: Emb_AddStorage.. TransInCon()error storage: %d, item: %d-%d: ", m_Param.GetCharName(), byStorageCode, pCon->m_byTableCode, pCon->m_wItemIndex);
		return;	
	}

	__ITEM* pAddItem = &m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[dwStorageIndex];

	{//월드에 업데이트..	
		if(m_pUserDB)
			m_pUserDB->Update_ItemAdd(byStorageCode, dwStorageIndex, pAddItem->m_byTableCode, pAddItem->m_wItemIndex, pAddItem->m_dwDur, pAddItem->m_dwLv);
	}

	//장착능력치계산	//장착, 장식 리스트일경우 //총알제외
	if(byStorageCode == _STORAGE_POS::EQUIP || byStorageCode == _STORAGE_POS::EMBELLISH)
	{		
		//장착단계에서 착용가능여부를 체크했으므로 체크생략..
		SetEquipEffect(pCon, true);
		SetEffectEquipCode(byStorageCode, dwStorageIndex, effect_apply);
	}

	//속성계산	//장착, 장신구..
	if(byStorageCode == _STORAGE_POS::EQUIP || byStorageCode == _STORAGE_POS::EMBELLISH)
	{
		if(pAddItem->m_byTableCode < base_fix_num || pAddItem->m_byTableCode == tbl_code_ring || pAddItem->m_byTableCode == tbl_code_amulet)
			CalcDefTol();
	}
	
	//모양버젼갱신 및 모양알림	//장착일경우
	if(byStorageCode == _STORAGE_POS::EQUIP )
	{
		UpdateVisualVer();
		SendMsg_EquipPartChange(pCon->m_byTableCode);

		if(pAddItem->m_byTableCode == tbl_code_weapon)
		{
			if(!IsRidingUnit())
			{
				m_pmWpn.FixWeapon(pAddItem);
			}
		}
	}

	//소지효과 아이템
	if(byStorageCode == _STORAGE_POS::INVEN)
	{
		if(pAddItem->m_byTableCode == tbl_code_res)
		{
			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[pAddItem->m_byTableCode].GetRecord(pAddItem->m_wItemIndex);

			if(pFld->m_nEffCode != -1)//효과가 있는 아이템이라면..
			{
				SetHaveEffect();
			}
		}
	}
}

void CPlayer::Emb_DelStorage(BYTE byStorageCode, BYTE byStorageIndex, bool bEquipChange)
{
	if(byStorageCode >= CPlayerDB::total_storage_num)
		return;

	_STORAGE_LIST::_storage_con* pCon = &m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[byStorageIndex];

	if(!m_Param.m_pStoragePtr[byStorageCode]->EmptyCon(byStorageIndex))
	{
		g_Main.m_logSystemError.Write("%s: Emb_DelStorage.. EmptyCon()error storage: %d, slot: %d: ", m_Param.GetCharName(), byStorageCode, byStorageIndex);
		return;
	}

	//내구성변경 링크에 있는거면 제거함..

	{//월드에 업데이트.. 
		if(m_pUserDB)
			m_pUserDB->Update_ItemDelete(byStorageCode, byStorageIndex);
	}

	//장착능력치계산	//장착, 장식 리스트일경우 //총알제외
	//속성계산	//장착, 장신구..
	if(byStorageCode == _STORAGE_POS::EQUIP || byStorageCode == _STORAGE_POS::EMBELLISH)
	{
		if(GetEffectEquipCode(byStorageCode, byStorageIndex) == effect_apply)
			SetEquipEffect(pCon, false);
		SetEffectEquipCode(byStorageCode, byStorageIndex, effect_none);

		if(pCon->m_byTableCode < base_fix_num || pCon->m_byTableCode == tbl_code_ring || pCon->m_byTableCode == tbl_code_amulet)
			CalcDefTol();
	}
	
	//모양버젼갱신 및 모양알림	//장착일경우
	//반격기해제
	if(byStorageCode == _STORAGE_POS::EQUIP)
	{
		if(!bEquipChange)	//바꾸기위해서 벗은것은 모양이나 무기해제를 하지않는다..
		{
			UpdateVisualVer();
			SendMsg_EquipPartChange(pCon->m_byTableCode);

			if(pCon->m_byTableCode == tbl_code_weapon)
			{
				if(!IsRidingUnit())
				{
					m_pmWpn.FixWeapon(NULL);
				}
			}
		}

		if(pCon->m_byTableCode == tbl_code_weapon && m_EP.m_bEff_State[_EFF_STATE::Res_Att])//PARAMETER EDIT (Res_Att) 반격기
		{
			RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Res_Att);
		}
	}

	//소지효과 아이템
	if(byStorageCode == _STORAGE_POS::INVEN)
	{
		if(pCon->m_byTableCode == tbl_code_res)
		{
			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[pCon->m_byTableCode].GetRecord(pCon->m_wItemIndex);

			if(pFld->m_nEffCode != -1)//효과가 있는 아이템이라면..
			{
				SetHaveEffect();
			}
		}
	}
}

DWORD CPlayer::Emb_AlterDurPoint(BYTE byStorageCode, BYTE byStorageIndex, int nAlter, bool bUpdate, bool bSend)
{
	__ITEM* pCon = &m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[byStorageIndex];
	if(!pCon->m_bLoad)
		return 0;

	WORD wSerial = pCon->m_wSerial;
	DWORD dwLeftDur = 0;
	if(!m_Param.m_pStoragePtr[byStorageCode]->AlterCurDur(byStorageIndex, nAlter, &dwLeftDur))
	{
		g_Main.m_logSystemError.Write("%s: Emb_AlterDurPoint.. AlterCurDur()error storage: %d, slot: %d: ", m_Param.GetCharName(), byStorageCode, byStorageIndex);
		return 0;
	}

	//소지효과 아이템
	if(byStorageCode == _STORAGE_POS::INVEN)
	{
		if(pCon->m_byTableCode == tbl_code_res)
		{
			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[pCon->m_byTableCode].GetRecord(pCon->m_wItemIndex);

			if(pFld->m_nEffCode != -1)//효과가 있는 아이템이라면..
			{
				SetHaveEffect();
			}
		}
	}
	
	if(dwLeftDur == 0)
	{
		//내구성변경 링크에 있는거면 제거함..

		//월드에 업데이트.. 
		if(m_pUserDB)
			m_pUserDB->Update_ItemDelete(byStorageCode, byStorageIndex);

		//장착능력치계산	//장착, 장식 리스트일경우 //총알제외
		if(byStorageCode == _STORAGE_POS::EQUIP)
		{
			if(GetEffectEquipCode(byStorageCode, byStorageIndex) == effect_apply)
				SetEquipEffect(pCon, false);
			SetEffectEquipCode(byStorageCode, byStorageIndex, effect_none);
		}

		//속성계산	//장착, 장신구..
		if(byStorageCode == _STORAGE_POS::EQUIP || byStorageCode == _STORAGE_POS::EMBELLISH)
		{
			if(pCon->m_byTableCode < base_fix_num || pCon->m_byTableCode == tbl_code_ring || pCon->m_byTableCode == tbl_code_amulet)
				CalcDefTol();
		}
		
		//모양버젼갱신 및 모양알림	//장착일경우
		if(byStorageCode == _STORAGE_POS::EQUIP)
		{
			UpdateVisualVer();
			//SendMsg_BreakdownEquipItem(pCon->m_byTableCode, wSerial);
			SendMsg_EquipPartChange(pCon->m_byTableCode);
		}	

		//아이템이 파괴됨을 아바타에게 알린다.
		SendMsg_DeleteStorageInform(byStorageCode, wSerial);
	}
	else
	{
		//클라이언트에게 변경을 알린다..
		if(bSend)
			SendMsg_AlterItemDurInform(byStorageCode, wSerial, dwLeftDur);

		//월드에 업데이트..
		if(m_pUserDB)
			m_pUserDB->Update_ItemDur(byStorageCode, byStorageIndex, dwLeftDur, bUpdate);
	}
	
	return dwLeftDur;
}

void CPlayer::Emb_ItemUpgrade(BYTE byUpgradeType, BYTE byStorageCode, BYTE byStorageIndex, DWORD dwGradeInfo)
{
	_STORAGE_LIST::_storage_con* pCon = &m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[byStorageIndex];

	//장착능력빼기
	if(byStorageCode == _STORAGE_POS::EQUIP)
	{
		if(GetEffectEquipCode(byStorageCode, byStorageIndex) == effect_apply)
			SetEquipEffect(pCon, false);
		SetEffectEquipCode(byStorageCode, byStorageIndex, effect_none);
	}

	switch(byUpgradeType)
	{
	case item_upgrade_up:
		m_Param.m_pStoragePtr[byStorageCode]->GradeUp(byStorageIndex, dwGradeInfo);
		break;

	case item_upgrade_down:
		m_Param.m_pStoragePtr[byStorageCode]->GradeDown(byStorageIndex, dwGradeInfo);
		break;

	case item_upgrade_init:		{
		DWORD dwNewUpt = GetBitAfterSetLimSocket(::GetItemUpgLimSocket(pCon->m_dwLv));
		m_Param.m_pStoragePtr[byStorageCode]->SetGrade(byStorageIndex, 0, dwNewUpt);
		}break;
	}

	//장착능력
	if(byStorageCode == _STORAGE_POS::EQUIP)
	{
		if(IsEffectableEquip(pCon))
		{
			SetEquipEffect(pCon, true);
			SetEffectEquipCode(byStorageCode, byStorageIndex, effect_apply);
		}
		else
			SetEffectEquipCode(byStorageCode, byStorageIndex, effect_wait);

		if(pCon->m_byTableCode == tbl_code_weapon)	
		{	//무기를 업글, 다운글했을때 속성이 바뀔수있다..
			m_pmWpn.FixWeapon((__ITEM*)pCon);
		}
	}

	//모양버젼갱신 및 모양알림	//장착일경우
	if(byStorageCode == _STORAGE_POS::EQUIP)
	{
		UpdateVisualVer();
		SendMsg_EquipPartChange(pCon->m_byTableCode);
	}

	//월드에 갱신..	
	if(m_pUserDB)
		m_pUserDB->Update_ItemUpgrade(byStorageCode, byStorageIndex, dwGradeInfo);
}

void CPlayer::Emb_UpdateStat(DWORD dwStatIndex, DWORD dwNewData)
{
	if(((DWORD*)&m_pmMst.m_BaseCum)[dwStatIndex] < dwNewData)
	{	//새로운것이 기존꺼보다클때만 세팅해준다..
		((DWORD*)&m_pmMst.m_BaseCum)[dwStatIndex] = dwNewData;

		if(m_pUserDB)
			m_pUserDB->Update_Stat(dwStatIndex, dwNewData, false);

		//LV HISTORY(marking)..
		m_Param.m_bAlterMastery[dwStatIndex] = true;
	}
}

void CPlayer::Emb_AlterStat(BYTE byMasteryClass, BYTE byIndex, DWORD dwAlter)
{	
	BYTE	byUpdateCount = 10;
	DWORD	dwAfterCum = 0;
	BYTE	byAfterMastery = 0;
	bool	bUpMty = false;
	bool	bUpEquip = false;
		
	if(!_STAT_DB_BASE::IsRangePerMastery(byMasteryClass, byIndex))
	{
		g_Main.m_logSystemError.Write("%s: _STAT_DB_BASE::IsRangePerMastery(%d, %d) == false", m_Param.GetCharName(), byMasteryClass, byIndex);
		return;
	}

	if(!m_pmMst.AlterCumPerMast(byMasteryClass, byIndex, dwAlter, &dwAfterCum, &byAfterMastery, &bUpMty, &bUpEquip))
		return;

	if(bUpEquip)//장착관련 마스터리가 올랐다면..
		m_bUpCheckEquipEffect = true;

	int nStatIndex = _STAT_DB_BASE::GetStatIndex(byMasteryClass, byIndex);

	SendMsg_StatInform(nStatIndex, dwAfterCum);

	if(bUpMty)	//마스터리가 올랐다면..
		ReCalcMaxHFSP();

	if(m_pUserDB)
		m_pUserDB->Update_Stat(nStatIndex, dwAfterCum, false);

	//LV HISTORY(marking)..
	m_Param.m_bAlterMastery[nStatIndex] = true;
}

void CPlayer::Emb_RidindUnit(bool bRiding, CParkingUnit* pCreateUnit)
{			
	//모양버젼갱신
	UpdateVisualVer();
			
	if(!bRiding) //bRiding이 false면 pCreateUnit이 NULL이면 안됀다..
	{		
		//유닛하차를 알림..
		SendMsg_UnitRideChange(false, pCreateUnit);
			
		//플레이어에 마킹..
		m_pParkingUnit = pCreateUnit;
		m_dwUnitViewOverTime = 0xFFFFFFFF;
		m_EP.SetEffectable(true);
			
		//능력치 조정..
		__ITEM* pWeaponItem = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pWeaponItem->m_bLoad)
			m_pmWpn.FixWeapon(pWeaponItem);		
		else
			m_pmWpn.FixWeapon(NULL);	
	}		
	else	
	{		
		//유닛탑승을 알림..
		SendMsg_UnitRideChange(true, m_pParkingUnit);
			
		//플레이어에 마킹..
		m_pParkingUnit = NULL;
		m_EP.SetEffectable(false);
			
		//능력치 조정..
		m_pmWpn.FixUnit(m_pUsingUnit);	
	}		
			
	//내성계산
	CalcDefTol();
			
	//소지효과.
	SetHaveEffect();
}

void CPlayer::ExitUpdateDataToWorld()
{
	if(!m_pUserDB)
		return;

	//월드에 갱신..	
	_EXIT_ALTER_PARAM ad;

	ad.dwHP = m_Param.GetHP();
	ad.dwFP = m_Param.GetFP();
	ad.dwSP = m_Param.GetSP();
	ad.dExp = m_Param.GetExp();

	ad.byMapCode = m_Param.GetMapCode();
	memcpy(ad.fStartPos, m_Param.GetCurPos(), sizeof(float)*3);

	//링크
	BYTE byLinkNum = m_Param.UseLinkNum();
	for(int i = 0; i < byLinkNum; i++)
	{
		_SF_LINK* pLink = &m_Param.m_SFLink[i];

		if(pLink->m_bLoad)
		{
			if(pLink->m_sClientIndex >= sf_linker_num) 
				break;
			ad.Link[pLink->m_sClientIndex].Key.SetData(pLink->m_byEffectCode, pLink->m_wEffectIndex);
		}
	}

	m_pUserDB->Update_Param(&ad);

	//유닛 부스터..
	if(m_Param.GetRaceCode() == race_code_bellato)
	{
		for(i = 0; i < unit_storage_num; i++)
		{
			_UNIT_DB_BASE::_LIST* p = &m_Param.m_UnitDB.m_List[i];
			if(p->byFrame == 0xFF)
				continue;

			m_pUserDB->Update_UnitData(i, p);
		}
	}
}

void CPlayer::CalcExp(CCharacter* pDst, int nDam)
{
	if(pDst->m_ObjID.m_byID != obj_id_monster)
		return ;//상대가 몬스터의 경우에만 

	if(nDam <= 0)
		return;	//데미지가 0보다 커야함..

	int nLvDif = (int)m_Param.GetLevel()-(int)pDst->GetLevel();
	if(nLvDif > 5)	//내레벨이 상대보다 5보다 크지 말아야한다..
		return;

	CMonster* pMon = (CMonster*)pDst;	
	_monster_fld* pDstRec = (_monster_fld*)pDst->m_pRecordSet;

	if(IsRidingUnit())	//유닛탑승상태
	{
		int nVal = pDstRec->m_fExt * (nDam / pDstRec->m_fMaxHP);
		if(nVal > 0)
			Emb_AlterStat(mastery_code_special, 0, nVal);
		return;
	}

	int nLeftHP = pDst->GetHP() - nDam;
	int nCurDam = nDam;
	if(nLeftHP < 0)
	{
		nLeftHP = 0;
		nCurDam = pDst->GetHP();
	}

	float fSetExt = (float)pDstRec->m_fExt * 0.7f * (nCurDam / pDstRec->m_fMaxHP);
	
	if(nLeftHP > 0)
	{
		AlterExp(fSetExt);
	}
	else //몬스터를 죽인 경우..
	{
		DWORD dwMonEmo = pMon->GetEmotionState();
		if(dwMonEmo == AI_EM_MAD)
			fSetExt += pDstRec->m_fExt * 0.5f;
		else
			fSetExt += pDstRec->m_fExt * 0.3f;

		if(!m_pPartyMgr->IsPartyMode())
		{		//파티중이 아니라면..
			AlterExp(fSetExt);
		}
		else	//파티중이라면..
		{
			CPlayer* pMember[member_per_party];
			BYTE byPartyNum = _GetPartyMemberInCircle(pMember, member_per_party);

			if(byPartyNum > 0)
			{
				fSetExt *= s_fExpDivUnderParty_Kill[byPartyNum-1];
			}

			//총레벨구하기..
			int nTotalLv = 0;
			for(int i = 0; i < byPartyNum; i++)
				nTotalLv += pMember[i]->GetLevel();

			//파티원별로 경험치 세팅
			for(i = 0; i < byPartyNum; i++)
			{
				float fRate = (float)pMember[i]->GetLevel() / nTotalLv;				
				pMember[i]->AlterExp(fSetExt * fRate);
			}
		}
	}
}

void CPlayer::AlterExp(float fAlterExp)
{
	//레벨업체크..
	if(max_level <= m_Param.GetLevel())
		return;

	if(fAlterExp > 0)
		fAlterExp *= m_EP.m_fEff_Have[_EFF_HAVE::Exp_Prof];//PARAMETER EDIT (경험치획득의 이득)
	double dCurExp = m_Param.GetExp() + fAlterExp;

	if(dCurExp >= s_dExpLimPerLv[m_Param.GetLevel()])//레벨업
	{
		m_Param.SetExp(0);		
		m_wExpRate = 0;

		BYTE byCurLv = GetLevel();
		SetLevel(byCurLv+1);	//레벨셋		
	}
	else
	{//경험치만 오른거라면 경험치만 세팅..
		m_Param.SetExp(dCurExp);
		m_wExpRate = m_Param.GetExp() / s_dExpLimPerLv[m_Param.GetLevel()] * 10000;
		SendMsg_AlterExpInform();
	}

	if(m_pUserDB)
		m_pUserDB->Update_Exp(m_Param.GetExp());
}

bool CPlayer::ConsumEquipItem(int nTableCode, int nDam, __ITEM* poutItem/* = NULL*/)
{
	//실제 아이템의 내구성이 소모돼면 true를 리턴
	__ITEM* pCon = &m_Param.m_dbEquip.m_pStorageList[nTableCode];

	if(!pCon->m_bLoad)
		return false;

	if(poutItem)
		memcpy(poutItem, pCon, sizeof(__ITEM));

	WORD wSerial = pCon->m_wSerial;
	int nDamDur = 0;

	if(nTableCode == tbl_code_weapon)
	{
		_WeaponItem_fld* pWpFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pCon->m_wItemIndex);
		if(pWpFld->m_nType == type_throw)	//던지기 아이템은 무조건 1씩 감소
			nDamDur = 1;
		else if(::IsAbrItem(nTableCode, pWpFld->m_dwIndex))
		{
			if(nDam >= 1 && nDam <= 3)
				nDamDur = 3;			
			else if(nDam >= 4 && nDam <= 5)
				nDamDur = 2;
			else if(nDam >= 6)
				nDamDur = 1;
		}
	}
	else
	{
		if(::IsAbrItem(nTableCode, pCon->m_wItemIndex))
		{
			if(nDam >= 1 && nDam <= 10)
				nDamDur = 1;
			else if(nDam >= 11 && nDam <= 20)
				nDamDur = 2;
			else if(nDam >= 21 && nDam <= 30)
				nDamDur = 3;
			else if(nDam >= 31)
				nDamDur = 5;
		}
	}
	if(nDamDur > 0)
	{
		DWORD dwLeftDur = Emb_AlterDurPoint(_STORAGE_POS::EQUIP, nTableCode, -nDamDur, false);
		if(dwLeftDur == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pCon, m_szItemHistoryFileName);
		}
	}

	if(poutItem)
		poutItem->m_dwDur = pCon->m_dwDur;

	if(nDamDur == 0)
		return false;

	return true;
}

void CPlayer::ReCalcMaxHFSP(bool bSend)
{
	m_nMaxHP = _CalcMaxHP();
	m_nMaxFP = _CalcMaxFP();
	m_nMaxSP = _CalcMaxSP();

	if(m_nMaxHP < GetHP())
		SetHP(m_nMaxHP, false);
	if(m_nMaxFP < GetFP())
		SetFP(m_nMaxFP, false);
	if(m_nMaxSP < GetSP())
		SetSP(m_nMaxSP, false);

	if(bSend)
		SendMsg_MaxHFSP();	
}

_ITEM_EFFECT* CPlayer::_GetItemEffect(__ITEM* pItem)
{
	if(pItem->m_byTableCode < base_fix_num)
	{
		_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
		if(!pFld)
			return NULL;
		return (_ITEM_EFFECT*)&pFld->m_nEff1Code;		
	}
	else
	{
		switch(pItem->m_byTableCode)
		{
		case tbl_code_weapon:
			{
				_WeaponItem_fld* pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
				if(!pFld)
					return NULL;
				return (_ITEM_EFFECT*)&pFld->m_nEff1Code;		
			}
			break;

		case tbl_code_shield:
			{
				_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
				if(!pFld)
					return NULL;
				return (_ITEM_EFFECT*)&pFld->m_nEff1Code;		
			}
			break;

		case tbl_code_cloak:
			{
				_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
				if(!pFld)
					return NULL;
				return (_ITEM_EFFECT*)&pFld->m_nEff1Code;		
			}
			break;

		case tbl_code_ring:
			{
				_RingItem_fld* pFld = (_RingItem_fld*)g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
				if(!pFld)
					return NULL;
				return (_ITEM_EFFECT*)&pFld->m_nEff1Code;		
			}
			break;

		case tbl_code_amulet:
			{
				_AmuletItem_fld* pFld = (_AmuletItem_fld*)g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);
				if(!pFld)
					return NULL;
				return (_ITEM_EFFECT*)&pFld->m_nEff1Code;		
			}
			break;			
		}
	}

	return NULL;
}

int	CPlayer::_CalcMaxHP()
{
	//INT(80+SQRT(레벨*방어숙련도^2)*10)

	float fHPMst = m_pmMst.GetMasteryPerMast(mastery_code_defence, 0);
	
	return 80 + sqrt(m_Param.GetLevel()*pow(fHPMst, 2)) * 10;
}

int	CPlayer::_CalcMaxFP()
{
/*
- 최대FP공식: INT(40+SQRT(레벨*((종족특화포스숙련도(벨라토:HOLY,코라:DARK) *0.4
              +FIRE숙련도*0.075+AQUA숙련도*0.1125+TERRA숙련도*0.1125+WIND숙련도*0.075)
              +(근접스킬숙련도*0.1125+원거리스킬숙련도*0.1125))^2)*5)

*/
	float fFPMst = 0.0f;

	switch(m_Param.GetRaceSexCode() / 2)
	{
	case 0:	//벨라토
		fFPMst = 
		m_pmMst.GetAveForceMasteryPerClass(fc_class_holy)*0.4f + 
		m_pmMst.GetAveForceMasteryPerClass(fc_class_fire)*0.075f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_aqua)*0.1125f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_terra)*0.1125f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_wind)*0.075f +
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_short)*0.1125f +
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_long)*0.1125f;
		break;

	case 1:	//코라
		fFPMst = 
		m_pmMst.GetAveForceMasteryPerClass(fc_class_dark)*0.4f + 
		m_pmMst.GetAveForceMasteryPerClass(fc_class_fire)*0.075f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_aqua)*0.1125f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_terra)*0.1125f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_wind)*0.075f +	
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_short)*0.1125f +
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_long)*0.1125f;
		break;

	case 2:	//아크레시아
		fFPMst = 49.5f	+
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_short)*0.1125f +
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_long)*0.1125f;
		break;
	}

	return 40 + sqrt(m_Param.GetLevel()*pow(fFPMst, 2)) * 5;
}

int CPlayer::_CalcMaxSP()
{
	//INT(160+SQRT(레벨*(근접공격숙련도*0.2+활숙련도*0.4+총숙련도*0.4)^2)*2.5)

	float fSPMst =  (float)m_pmMst.GetMasteryPerMast(mastery_code_weapon, wp_class_close)*0.2f + 
					(float)m_pmMst.GetMasteryPerMast(mastery_code_weapon, wp_class_long)*0.8f;

	return 160 + sqrt(m_Param.GetLevel()*pow(fSPMst, 2)) * 2.5f;
}

BYTE CPlayer::_GetPartyMemberInCircle(CPlayer** out_ppMember, int nMax)
{
	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();
	if(!ppMem)
		return 0;

	BYTE byCnt = 0;
	for(int i = 0; i < member_per_party; i++)
	{
		if(!ppMem[i])
			continue;

		CPlayer* pMem = &g_Player[ppMem[i]->m_wZoneIndex];
		if(!pMem->m_bLive || pMem->m_bCorpse)
			continue;

		if(pMem->m_pCurMap != m_pCurMap || pMem->m_wMapLayerIndex != m_wMapLayerIndex)
			continue;

		if(abs(pMem->m_fCurPos[0] - m_fCurPos[0]) > STANDARD_VIEW_SIZE)
			continue;

		if(abs(pMem->m_fCurPos[2] - m_fCurPos[2]) > STANDARD_VIEW_SIZE)
			continue;

		if(abs(pMem->m_fCurPos[1] - m_fCurPos[1]) > 100)
			continue;

		out_ppMember[byCnt] = pMem;
		byCnt++;
	}

	return byCnt;
}

bool CPlayer::pc_ChangeModeType(int nModeType)
{
	m_nModeType = nModeType;

	//대상에게 먼저 알리고..
	SendMsg_ModeChange((BYTE)m_nModeType);

	//모드바뀜은 바로 알림..
	m_tmrAlterState.NextTimeRun();
	
	return true;
}

void CPlayer::NewViewCircleObject()
{
	_sec_info* pSec = m_pCurMap->GetSecInfo();
	_pnt_rect Rect;
	m_pCurMap->GetRectInRadius(&Rect, MAP_STD_RADIUS, GetCurSecNum());

	for(int h = Rect.nStarty; h <= Rect.nEndy; h++)
	{
		for(int w = Rect.nStartx; w <= Rect.nEndx; w++)
		{	
			int nSecNum = w+h*pSec->m_nSecNumW;

			CObjectList* pList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, nSecNum);
			if(!pList)
				continue;
			
			_object_list_point* pPoint = pList->m_Head.m_pNext;
			while(pPoint != &pList->m_Tail)
			{
				CGameObject* pObj = pPoint->m_pItem;
				pPoint = pPoint->m_pNext;

				if(pObj == this)
					continue;

				if(!pObj->m_bMove)
					pObj->SendMsg_FixPosition(m_ObjID.m_wIndex);
				else				
					pObj->SendMsg_RealMovePoint(m_ObjID.m_wIndex);				
			}
		}
	}
}

bool CPlayer::SetTarPos(float* fTarPos, bool bColl)
{
	if(CCharacter::SetTarPos(fTarPos, false))
		return true;

	return false;
}

float CPlayer::GetMoveSpeed()
{
	float fUnitSpeed = 0.0f;
	if(IsRidingUnit())
	{
		_UnitFrame_fld* pFrameFld = (_UnitFrame_fld*)g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame);
		fUnitSpeed = pFrameFld->m_fMoveRate_Seed;
		for(int i = 0; i < UNIT_PART_NUM; i++)
		{
			_UnitPart_fld* pPartFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[i].GetRecord(m_pUsingUnit->byPart[i]);
			if(pPartFld)
				fUnitSpeed = pPartFld->m_fMoveSpdRev;
		}

		//부스터 추가..
		if(m_nMoveType == move_type_run)
		{
			_UnitPart_fld* pPartFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_back].GetRecord(m_pUsingUnit->byPart[unit_part_back]);
			if(pPartFld)
				fUnitSpeed += pPartFld->m_fBstSpd;
		}

		return fUnitSpeed;
	}

	switch(m_nMoveType)
	{
	case move_type_walk:
		{
			if(!IsRidingUnit())
				return ((_player_fld*)m_pRecordSet)->m_fMoveWalkRate;
			else
				return fUnitSpeed;
		}

	case move_type_run:
		{
			if(!IsRidingUnit())
				return ((_player_fld*)m_pRecordSet)->m_fMoveRunRate + m_EP.GetEff_Plus(_EFF_PLUS::Move_Run_Spd);//## PARAMETER EDIT (이동속도) ##
			else
				return fUnitSpeed;//추후에 부스터 추가..
		}
	}
	
	return 0.0f;
}

int CPlayer::SetDamage(int nDamage, CCharacter* pDst, int nDstLv, bool bCrt)
{
	if(m_bCorpse)
		return m_Param.GetHP();

	if(m_nLastBeatenPart == -1)
	{
		if(nDamage > 0)
			g_Main.m_logSystemError.Write("%s: SetDamage() m_nLastBeatenPart == -1", m_Param.GetCharName());
		m_nLastBeatenPart = 0;
	}

	if(nDamage > 0)
	{
		if(!IsRidingUnit())	//유닛탑승상태가아니면..
		{
			if(!m_bCheat_Matchless)
			{
				SetHP(m_Param.GetHP()-nDamage, false);
			}

			if(pDst)
			{
				if(m_Param.GetLevel() - nDstLv < 6)	
				{//자신과 상대의 레벨차가 6보다 작을때..(5이하)
					int nAddCum = GetMasteryCumAfterAttack(nDstLv);
					if(m_nLastBeatenPart == tbl_code_shield)	//방패로 막은경우..
						Emb_AlterStat(mastery_code_shield, 0, nAddCum);
					else
						Emb_AlterStat(mastery_code_defence, 0, nAddCum);
				}
			}
		}
		else	//유닛탑승상태라면
		{
			if(!m_bCheat_Matchless)
			{
				if(m_pUsingUnit->dwGauge > nDamage)
					m_pUsingUnit->dwGauge -= nDamage;
				else
					m_pUsingUnit->dwGauge = 0;
			}

			if(m_pUsingUnit->dwGauge == 0)	//유닛이 파괴돼면 자신도 죽는상태가 됌
			{
				SetHP(0, false);
				
				//월드에 알림..
				m_pUserDB->Update_UnitDelete(m_pUsingUnit->bySlotIndex);

				//ITEM HISTORY..
				s_MgrItemHistory.destroy_unit(m_pUsingUnit->bySlotIndex, m_pUsingUnit->byFrame, m_szItemHistoryFileName);
			}
		}

		if(GetHP() == 0)
		{
			Corpse((CCharacter*)pDst);	//시체로 만듬
		}
	}

	//애니머스 소환상태인경우 애니머스에게 알린다..
	if(pDst && m_pRecalledAnimusChar)
		m_pRecalledAnimusChar->MasterBeAttacked_MasterInform(pDst);

	m_nLastBeatenPart = -1;

	return m_Param.GetHP();
}

int CPlayer::GetLimHP()
{
	return m_nMaxHP * m_EP.GetEff_Rate(_EFF_RATE::HP_Mx);	//PARAMETER EDIT ( HP회복_최대치율 )
}

int CPlayer::GetLimFP()
{
	return m_nMaxFP * m_EP.GetEff_Rate(_EFF_RATE::FP_Mx);	//PARAMETER EDIT ( FP회복_최대치율 )
}

int CPlayer::GetLimSP()
{
	return m_nMaxSP * m_EP.GetEff_Rate(_EFF_RATE::SP_Mx);	//PARAMETER EDIT ( SP회복_최대치율 )
}

void CPlayer::SetHP(int nHP, bool bOver)
{ 
	int nCurHP = GetHP();

	if(!bOver)	//오버할수없으면..
	{
		if(nHP > nCurHP)	//증가의 경우..
		{
			if(nCurHP < GetLimHP() && nHP > GetLimHP())	//현재의 HP가 한계치보다작고 바뀌는것은 한계치보다는 클때..
				nHP = GetLimHP();
			else if(nCurHP >= GetLimHP() && nHP >= nCurHP)	//현재의 HP가 한계치보다크고.. 바뀌는것은 현재치보다는 클때..
				return;
		}
	}

	if(nHP < 0)
		nHP = 0;

	m_Param.SetHP(nHP); 
	
	if(nCurHP != nHP)
	{
		SendData_PartyMemberHP();
	}		
}

void CPlayer::SetFP(int nFP, bool bOver)
{ 
	int nCurFP = GetFP();

	if(!bOver)	//오버할수없으면..
	{
		if(nFP > nCurFP)	//증가의 경우..
		{
			if(nCurFP < GetLimFP() && nFP > GetLimFP())	//현재의 HP가 한계치보다작고 바뀌는것은 한계치보다는 클때..
				nFP = GetLimFP();
			else if(nCurFP >= GetLimFP() && nFP >= nCurFP)	//현재의 HP가 한계치보다크고.. 바뀌는것은 현재치보다는 클때..
				return;
		}
	}	
	
	if(nFP < 0)
		nFP = 0;

	m_Param.SetFP(nFP); 

	if(nCurFP != nFP)
	{
		SendData_PartyMemberFP();
	}
}

void CPlayer::SetSP(int nSP, bool bOver)
{ 
	int nCurSP = m_Param.GetSP();

	if(!bOver)	//오버할수없으면..
	{
		if(nSP > nCurSP)	//증가의 경우..
		{
			if(nCurSP < GetLimSP() && nSP > GetLimSP())	//현재의 HP가 한계치보다작고 바뀌는것은 한계치보다는 클때..
				nSP = GetLimSP();
			else if(nCurSP >= GetLimSP() && nSP >= nCurSP)	//현재의 HP가 한계치보다크고.. 바뀌는것은 현재치보다는 클때..
				return;
		}
	}
	
	if(nSP < 0)
		nSP = 0;

	m_Param.SetSP(nSP);

	if(nCurSP != nSP)
	{
		SendData_PartyMemberSP();
	}
}

float CPlayer::GetWidth()
{
	return (float)((_player_fld*)m_pRecordSet)->m_fWidth;
}

float CPlayer::GetAttackRange()
{
	return m_pmWpn.wGaAttRange;
}

int CPlayer::GetWeaponRange()
{
	return m_pmWpn.wGaAttRange - 40;
}

int CPlayer::GetDefFC(int nAttactPart, CCharacter* pAttChar)
{
	m_nLastBeatenPart = nAttactPart;

	// Attack 부위에 방패가 있으면
	// parrying(방패로 막는 기술)이 성공했는지 체크해서
	// 성공시 방패의 방어력을 적용한다.	

	bool bIgnorShield = false;
	if(pAttChar)
	{
		if(pAttChar->m_ObjID.m_byID == obj_id_player)
		{
			if(((CPlayer*)pAttChar)->m_pmWpn.byWpClass == wp_class_close)
				bIgnorShield = pAttChar->m_EP.GetEff_State(_EFF_STATE::Dst_No_Shd);	//## PARAMETER EDIT (방패무시)
		}
	}

	__ITEM* pShieldCon = &m_Param.m_dbEquip.m_pStorageList[tbl_code_shield];

	bool bShieldable = false;
	if(pShieldCon->m_bLoad && GetEffectEquipCode(_STORAGE_POS::EQUIP, tbl_code_shield) == effect_apply)
		bShieldable = true;
	if(bShieldable)//양손무기를 들었는지확인..
	{	//양손무기와 방패를 버그로 들게했을경우 사후처리..
		__ITEM* pWpCon = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pWpCon->m_bLoad)
		{
			_WeaponItem_fld* pWpFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pWpCon->m_wItemIndex);
			if(((_WeaponItem_fld*)pWpFld)->m_nFixPart == handle_both)
				bShieldable = false;//양손무기를들었으면 효과를 못보게한다..
		}
	}

	if(pAttChar && bShieldable && !bIgnorShield)
	{	
		int l_nShieldSkill = m_pmMst.GetMasteryPerMast(mastery_code_shield, 0);
		if( l_nShieldSkill == 99 )		// parrying 숙력도가 99이면 무조건 성공
		{
			m_nLastBeatenPart = tbl_code_shield;
		}
		else
		{
			//70+(Parrying숙련도/99*30)
			DWORD l_dwSuccRate = 50+(l_nShieldSkill/99*30.0f);
			l_dwSuccRate = min(l_dwSuccRate, 95);
			
			if(m_rtPer100.GetRand() < l_dwSuccRate)
				m_nLastBeatenPart = tbl_code_shield;
		}
	} 

	float fTatalDefFC = 0.0f;

	if(m_nLastBeatenPart == tbl_code_shield)	//방패로 막은경우..
	{
		float fDefFc[base_fix_num];
		for(int i = 0; i < base_fix_num; i++)
		{
			__ITEM* pCon = &m_Param.m_dbEquip.m_pStorageList[i];

			if(pCon->m_bLoad && GetEffectEquipCode(_STORAGE_POS::EQUIP, i) == effect_apply)
			{
				_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[i].GetRecord(pCon->m_wItemIndex);
				fDefFc[i] = pFld->m_fDefFc;
			}
			else
			{
				_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[m_nLastBeatenPart].GetRecord(m_Param.m_dbChar.m_byDftPart[m_nLastBeatenPart]);
				fDefFc[i] = pFld->m_fDefFc;
			}
		}

		_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[tbl_code_shield].GetRecord(pShieldCon->m_wItemIndex);
		float fSdDefFc = pFld->m_fDefFc;

		fTatalDefFC = fDefFc[tbl_code_helmet]*0.2f + fDefFc[tbl_code_upper]*0.23f + fDefFc[tbl_code_lower]*0.22f + fDefFc[tbl_code_gauntlet]*0.18f + fDefFc[tbl_code_shoe]*0.17f + fSdDefFc;
		fTatalDefFC *= m_EP.GetEff_Rate(_EFF_RATE::Shield_Def);//## PARAMETER EDIT (방패방어력)	 
	}
	else
	{
		//방패로 못막은 경우..
		__ITEM* pPartCon = &m_Param.m_dbEquip.m_pStorageList[m_nLastBeatenPart];

		if(pPartCon->m_bLoad && GetEffectEquipCode(_STORAGE_POS::EQUIP, m_nLastBeatenPart) == effect_apply)
		{
			_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[m_nLastBeatenPart].GetRecord(pPartCon->m_wItemIndex);
			fTatalDefFC = pFld->m_fDefFc;
		}
		else
		{
			_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[m_nLastBeatenPart].GetRecord(m_Param.m_dbChar.m_byDftPart[m_nLastBeatenPart]);
			fTatalDefFC = pFld->m_fDefFc;
		}
	}
	fTatalDefFC *= GetAdrenDef();	//아드레날린

	return (int)fTatalDefFC;
}

int CPlayer::GetDefSkill()
{
	if(IsRidingUnit())
	{
		//유닛탑승상태라면 머리와 상체파트의 데이터필드를 더한다..
		int nDefMastery = 0;
		_UnitPart_fld* pHeadFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_head].GetRecord(m_pUsingUnit->byPart[unit_part_head]);
		if(pHeadFld)
			nDefMastery += pHeadFld->m_nDefMastery;
		_UnitPart_fld* pUpperFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_upper].GetRecord(m_pUsingUnit->byPart[unit_part_upper]);
		if(pUpperFld)
			nDefMastery += pUpperFld->m_nDefMastery;	

		return nDefMastery;
	}

	return m_pmMst.GetMasteryPerMast(mastery_code_defence, 0);
}

int	CPlayer::GetFireTol()
{
	return m_dwTolValue[TOL_CODE_FIRE] + m_EP.GetEff_Plus(_EFF_PLUS::Part_Tol_+TOL_CODE_FIRE); //## PARAMETER EDIT (불내성)
}

int	CPlayer::GetWaterTol()
{
	return m_dwTolValue[TOL_CODE_WATER] + m_EP.GetEff_Plus(_EFF_PLUS::Part_Tol_+TOL_CODE_WATER); //## PARAMETER EDIT (물내성)
}

int	CPlayer::GetSoilTol()
{
	return m_dwTolValue[TOL_CODE_SOIL] + m_EP.GetEff_Plus(_EFF_PLUS::Part_Tol_+TOL_CODE_SOIL); //## PARAMETER EDIT (흙내성)
}

int	CPlayer::GetWindTol()
{
	return m_dwTolValue[TOL_CODE_WIND] + m_EP.GetEff_Plus(_EFF_PLUS::Part_Tol_+TOL_CODE_WIND); //## PARAMETER EDIT (풍내성)
}

float CPlayer::GetWeaponAdjust()
{
	__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
	if(pItem->m_bLoad)
	{
		_WeaponItem_fld* pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pItem->m_wItemIndex);
		if(pFld->m_nType != type_mine)
			return s_fWPGapAdjust[pFld->m_nType];
	}

	return wp_adjust_bunch;
}

int	CPlayer::AttackableHeight()
{
	if(m_pmWpn.byWpClass != wp_class_close)
		return attack_max_height;
	
	return attack_able_height;
}

int CPlayer::GetGenAttackProb(CCharacter* pDst, int nPart)
{
	//공격자가 착용한 아이템의 무기숙련도를 구한다.
	float fWSkill;//공격자무기숙련도

	if(m_pmWpn.byWpType == 0xFF)
		fWSkill = 10;//맨손일경우 10으로
	else if(m_pmWpn.byWpType == type_staff)
		fWSkill = 10;//지팡이공격일경우 10으로	
	else
		fWSkill = m_pmMst.GetMasteryPerMast(mastery_code_weapon, m_pmWpn.byWpClass);

	//공격자변 공격성공률
	float fOneRate = 1.0f;
	if(!IsRidingUnit())
		fOneRate = m_Param.GetLevel() * 1 + fWSkill;
	else
	{
		BYTE byPart = unit_part_arms;
		if(m_byUsingWeaponPart == unit_bullet_shoulder)
			byPart = unit_part_shoulder;

		_UnitPart_fld* pWeaponFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[byPart].GetRecord(m_pUsingUnit->byPart[byPart]);
		if(pWeaponFld)
			fOneRate = (float)pWeaponFld->m_nAttMastery;
		_UnitPart_fld* pHeadFld = (_UnitPart_fld*)g_Main.m_tblUnitPart[unit_part_head].GetRecord(m_pUsingUnit->byPart[unit_part_head]);
		if(pHeadFld)
			fOneRate += (float)pHeadFld->m_nAttMastery;		
	}

	//방어자변 방어성공률
	float fDstRate = 0.0f;//방어자변
	if(pDst->m_ObjID.m_byID == obj_id_player)//플레이일경우
		fDstRate = ((CPlayer*)pDst)->m_Param.GetLevel() * 1 + pDst->GetDefSkill();
	else if(pDst->m_ObjID.m_byID == obj_id_monster)//몬스터의 경우 
		fDstRate = pDst->GetDefSkill();

	//최종 공격성공률(공격자변 공격성공률-방어자변 방어성공률)/4+70
	int nTotalRate = (int)(fOneRate - fDstRate) / 4 + 70;

	//부위선택공격시 조정돼는 값..
	float fRate[base_fix_num] = {0.23f, 0.22f, 0.18f, 0.17f, 0.20f};
	float fPartRate = 5.0f * fRate[nPart];
	nTotalRate *= fPartRate;

	//효과조종
	int nEffHitRate = m_EP.GetEff_Plus(_EFF_PLUS::GE_Hit_ + m_pmWpn.byWpClass); //## PARAMETER EDIT (일반명중)
	int nEffAvdRate = pDst->m_EP.GetEff_Plus(_EFF_PLUS::GE_Avd);				//## PARAMETER EDIT (일반회피)

	nTotalRate += (nEffHitRate - nEffAvdRate);
	
	nTotalRate = max(nTotalRate, 5);
	nTotalRate = min(nTotalRate, 95);

	return nTotalRate;
}

bool CPlayer::IsRecvableContEffect()
{
	if(IsRidingUnit())
		return false;
	return true;
}

int	CPlayer::GetMasteryCumAfterAttack(int nDstLv)
{
//1~10레벨	11~20레벨	21~30레벨	31~40레벨	41~50레벨
//1			2			3			4			5
	int nAddCum = nDstLv - GetLevel();

	if(nAddCum > 5)	
		nAddCum = 5;
	else if(nAddCum <= 0) 
		nAddCum = 1;

	int nMaxAdd = nDstLv/10;
	if(nDstLv%10 != 0)
		nMaxAdd++;

	if(nAddCum > nMaxAdd)
		nAddCum = nMaxAdd;

	return nAddCum;
}

void CPlayer::UpdatedMasteryWriteHistory()
{
	DWORD dwCurTime = timeGetTime();

	//15분마다 체크..
	if(dwCurTime - m_dwUMWHLastTime > 15*60*1000)
	{
		m_dwUMWHLastTime = dwCurTime;

		//LV HISTORY..
		s_MgrLvHistory.update_mastery(m_Param.GetLevel(), m_Param.GetExp(), &m_pmMst.m_BaseCum, m_Param.m_bAlterMastery, m_szLvHistoryFileName);
		
		//init alter maker ..
		m_Param.InitAlterMastery();
	}
}

DWORD CPlayer::SetStateFlag(bool bUpdate)
{
//#define FLAG_MOVE_TYPE			0	//걷기, 뛰기 상태
//#define FLAG_MODE_TYPE			1	//전투, 비전투상태
//#define FLAG_STEALTH_STATE		2	//스텔스상태
//#define FLAG_STUN_STATE			3	//스턴상태
//#define FLAG_PARTY_STATE			4	//파티상태
//#define FLAG_CORPSE_STATE			5	//시체상태
//#define FLAG_LEADER_STATE			6	//파티리더	
//#define FLAG_DTRADE_STATE			7	//직거래중
//#define FLAG_DETECT_STATE			8	//detect모드
//#define FLAG_OBSERVER_STATE		9	//옵저버모드
//#define FLAG_PARTYLOCK_STATE		10	//파티잠금모드
//#define FLAG_MINING_STATE			11	//채굴모드
//#define FLAG_GROGGY_STATE			12	//그로기상태

	if(!m_pPartyMgr)
		return 0;

	DWORD dwFlag = 0x00000000;

	if(m_nMoveType == move_type_run)
		dwFlag |= (0x00000001 << FLAG_MOVE_TYPE);
	if(m_nModeType == mode_type_mili)
		dwFlag |= (0x00000001 << FLAG_MODE_TYPE);
	if(GetStealth())
		dwFlag |= (0x00000001 << FLAG_STEALTH_STATE);
	if(m_bStun)
		dwFlag |= (0x00000001 << FLAG_STUN_STATE);
	if(m_pPartyMgr->IsPartyMode())
		dwFlag |= (0x00000001 << FLAG_PARTY_STATE);
	if(m_bCorpse)
		dwFlag |= (0x00000001 << FLAG_CORPSE_STATE);
	if(m_pPartyMgr->IsPartyBoss())
		dwFlag |= (0x00000001 << FLAG_LEADER_STATE);
	if(m_pmTrd.bDTradeMode)
		dwFlag |= (0x00000001 << FLAG_DTRADE_STATE);
	if(m_EP.GetEff_Plus(_EFF_PLUS::Detect) > 0)
		dwFlag |= (0x00000001 << FLAG_DETECT_STATE);
	if(m_bObserver)
		dwFlag |= (0x00000001 << FLAG_OBSERVER_STATE);
	if(m_pPartyMgr->IsPartyLock())
		dwFlag |= (0x00000001 << FLAG_PARTYLOCK_STATE);
	if(m_bMineMode)
		dwFlag |= (0x00000001 << FLAG_MINING_STATE);
	if(m_pmGrg.GetGroggy())
		dwFlag |= (0x00000001 << FLAG_GROGGY_STATE);

	if(bUpdate)
		m_dwLastState = dwFlag;
	return dwFlag;
}

DWORD CPlayer::GetLastStateFlag()
{
	return SetStateFlag(false);
}

void  CPlayer::SetStaticMember()
{
	for(int i = 0; i < max_level; i++)
	{
		_exp_fld* pExpRcd = (_exp_fld*)g_Main.m_tblExp.GetRecord(i);
		s_dExpLimPerLv[i] = atof(pExpRcd->m_strBellato);
	}

	s_pnLinkForceItemToEffect = new int [g_Main.m_tblItemData[tbl_code_fcitem].GetRecordNum()];
	memset(s_pnLinkForceItemToEffect, -1, sizeof(s_pnLinkForceItemToEffect));
	for(i = 0; i < g_Main.m_tblItemData[tbl_code_fcitem].GetRecordNum(); i++)
	{
		_ForceItem_fld* pFld = (_ForceItem_fld*)g_Main.m_tblItemData[tbl_code_fcitem].GetRecord(i);
		if(!pFld)
		{
			g_Main.m_logSystemError.Write("CPlayer::SetStaticMember() : %d force..NULL", i);
			break;
		}

		_force_fld* p = (_force_fld*)g_Main.m_tblEffectData[effect_code_force].GetRecord(pFld->m_strForce_Codekey);
		if(!p)
		{
			g_Main.m_logSystemError.Write("CPlayer::SetStaticMember() : %s force..NULL", pFld->m_strForce_Codekey);
			break;
		}

		s_pnLinkForceItemToEffect[i] = p->m_dwIndex;
	}
}

void  CPlayer::OnLoop_Static()
{
	s_MgrItemHistory.OnLoop();
	s_MgrLvHistory.OnLoop();
	s_AnimusReturnDelay.CheckOnLoop();
}

void _ANIMUS_RETURN_DELAY::Process(DWORD dwIndex, DWORD dwSerial)
{
	CPlayer* pOne = &g_Player[dwIndex];

	if(!pOne->m_bLive)
		return;

	if(pOne->m_dwObjSerial != dwSerial)
		return;

	pOne->_AnimusReturn();
}

