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

	m_tmrAlterState.BeginTimer(timer_alter_state);	//1��..
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
	m_id.dwSerial = pUser->m_AvatorData.dbAvator.m_dwRecordNum;//serial�� avator DB �ø���
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

	//ȿ�����������ʱ�ȭ
	m_bUpCheckEquipEffect = true;	//ó�� ������������ effect_wait�����̹Ƿ� ������������ ȿ�������� �Ѵ�
	m_bDownCheckEquipEffect = false;
	memset(m_byEffectEquipCode, effect_none, sizeof(m_byEffectEquipCode));

	CMapData* pMap = g_Main.m_MapOper.GetMap(pData->dbAvator.m_byMapCode);
	if(!(pMap && pMap->IsMapIn(pData->dbAvator.m_fStartPos)))
	{
		CMapData* p = g_Main.m_MapOper.GetPosStartMap(pData->dbAvator.m_byRaceSexCode/2, pData->dbAvator.m_fStartPos);
		if(!p)
		{
			g_Main.m_logSystemError.Write("Load() : ���� : %s.. ������ġ�� ��ã��(race:%d, x:%d, y:%d, z:%d)..", 
				pData->dbAvator.m_szAvatorName, pData->dbAvator.m_byRaceSexCode, (int)pData->dbAvator.m_fStartPos[0], (int)pData->dbAvator.m_fStartPos[1], (int)pData->dbAvator.m_fStartPos[2]);
			return false;
		}		
		pData->dbAvator.m_byMapCode = p->m_pMapSet->m_dwIndex;
	}
	pMap = g_Main.m_MapOper.GetMap(pData->dbAvator.m_byMapCode);
	if(!pMap->m_bUse)
	{
		g_Main.m_logSystemError.Write("Load() : ���� : %s.. ��������� ���� ��(%s)..", pData->dbAvator.m_szAvatorName, pMap->m_pMapSet->m_strCode);
		return false;
	}

	//������ ����
	if(m_Param.ConvertAvatorDB(pData) && 
		m_Param.ConvertGeneralDB(pData))
	{
		m_bLoad = true;
		m_bMapLoading = true;	//Ŭ���̾�Ʈ�� �ʷε带 ���..

		//������ �ø��� ����
		m_Param.AppointSerialStorageItem();	
		
		//����ġ ���з����
		m_wExpRate = m_Param.GetExp() / s_dExpLimPerLv[m_Param.GetLevel()] * 10000;

		//���� �Ķ���� �ʱ�ȭ..(�ʱ�ȭ��ġ..ReCalcMaxHFSP�� ����)
		m_pmMst.Init(&pUser->m_AvatorData.dbStat, m_Param.GetRaceSexCode()/2);//���õ� �ʱ�ȭ
		m_pmTrd.Init();//���ŷ������ۼ����ʱ�ȭ
		m_pmWpn.Init();//���������ʱ�ȭ
		m_pmGrg.Init();	//�׷α���� �ʱ�ȭ
		m_pmTwr.Init();

		//�ִ� HFSP���
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
		g_Main.m_logSystemError.Write("Load() : ���� : %d:%s", pData->dbAvator.m_dwRecordNum, pData->dbAvator.m_szAvatorName);
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
		g_Main.m_logSystemError.Write("Create(%s) : ����.. ��ġ: Map: %s(%d,%d,%d)", m_Param.GetCharName(), m_pCurMap->m_pMapSet->m_strCode, (int)m_fCurPos[0], (int)m_fCurPos[1], (int)m_fCurPos[2]);
		g_Main.m_Network.Close(client_line, m_ObjID.m_wIndex);
		return false;
	}

	m_bOper = true;
	m_nModeType = mode_type_demili;
	m_nMoveType = move_type_run;
	m_bMineMode = false;

	//version�� setting..
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


	//Ÿ�̸� �ʱ�ȭ..
	m_tmrAlterState.TermTimeRun();
	m_tmrIntervalSec.TermTimeRun();
	m_dwUMWHLastTime = timeGetTime();

	if(m_Param.GetHP() == 0)
		m_bCorpse = true;
	else
		m_bCorpse = false;

	CalcDefTol();//�������..
	PastWhisperInit();//���űӼӸ�����ʱ�ȭ
	SetHaveEffect();//�����ۼ���ȿ���˻�
	SetStateFlag(true);//�����÷���
	m_QuestMgr.InitMgr(this, &m_Param.m_QuestDB);	//����Ʈ������� �ʱ�ȭ..

	m_dwMineDelayTime = 0xFFFFFFFF;

	//�Ƶ巹���� ����
	m_dwStartTime = 0;
	m_dwDurTime = 0;
	m_fAFRate = 1.0f;
	m_fDefRate = 1.0f;

	//�������� ��ġ..(������ �׳� ī��)
	memcpy(m_fBeforeDungeonPos, m_fCurPos, sizeof(float)*3);
	m_pBeforeDungeonMap = m_pCurMap;
	m_bPosMainBase = m_pCurMap->IsBaseTown(m_fCurPos);

	//���̵�, � ����� �ϴ� ������ ��������..
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

	//������ǥ �˸�..
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
	//���ֿ� CutTime�� ������ٸ� ����ð��� ���ؼ� 5���̳���� ����, ���κ� ���� �� �ش� ������ ����Ŵ
	CheckUnitCutTime();

	//�޽����� �ʱ� �ɸ��� ��絥���͸� �˸�
	if(g_Main.m_bMsgrOpen) 
		SendMsg_StartShape_Msgr();

	//������ �ܰ谡�Ǹ� ������ �϶�� �˷���..
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

		//��ġ�� ����Ÿ���� ��ü�Ѵ�..
		_TowerAllReturn();

		SendMsg_Destroy();
		CCharacter::Destroy();

		//��ȯ�����ִٸ� ���Ͻ�Ŵ
		_AnimusReturn();

		//��ŷ���� ������ ������ ���� ����
		ForcePullUnit(false, true);

		//�����ʿ��� �����ߴٸ� 
		if(m_pCurMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
		{//������ġ�� �ǵ���..
			m_pCurMap = m_pBeforeDungeonMap;
			memcpy(m_fCurPos, m_fBeforeDungeonPos, sizeof(float)*3);
		}
		//7�������ϴ� ������ ����ġ�� ����ְ� �����Ų��.
		if(m_Param.GetLevel() <= 7 && m_Param.GetHP() > 0)
		{
			if(!m_bOper && m_pUserDB)//����������ÿ��� �������ʴ´�.
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
			s_MgrItemHistory.close("������", m_szItemHistoryFileName);
		else
			s_MgrItemHistory.close("����", m_szItemHistoryFileName);

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
	
	m_dwMineDelayTime = 0xFFFFFFFF;//ä���ʱ�ȭ
	m_bMineMode = false;

	m_pmGrg.EndGroggy();//�׷α��ʱ�ȭ

	SendMsg_Die();

	if(pAtter)
	{
		if(m_Param.GetExp() > 0 && pAtter->m_ObjID.m_byID == obj_id_monster)
		{	//���Ϳ��� ���ѰŶ�� ����ġ�� ��´�.********************************************
			if(m_Param.GetLevel() > 7 && m_Param.GetLevel() < max_level && m_rtPer100.GetRand()%2)
			{
				float fLvGap = abs(m_Param.GetLevel()-pAtter->GetLevel()) + 5;				
				fLvGap = min(fLvGap, 20);
				fLvGap *= 0.01f;

				double dCurExp = m_Param.GetExp();
				float fDecRate = (float)(::rand()%100)/100;
				double dAlterExp = dCurExp * fLvGap * fDecRate;
				dAlterExp = max(dAlterExp, 0);			//0���� Ŀ���ϸ�..
				dAlterExp = min(dAlterExp, dCurExp);	//���� ����ġ���ٴ� �۾ƾ��Ѵ�..
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

	//��ȯ�����ִٸ� ������ ���Ͻ�Ŵ
	if(m_pRecalledAnimusChar)
		s_AnimusReturnDelay.Push(m_ObjID.m_wIndex, m_dwObjSerial);

	m_bCorpse = true;
	m_bMineMode = false;
	m_nModeType = mode_type_demili;
	m_nMoveType = move_type_run;

	//�������� �����Ŷ�� �����޽����� ��ٸ����� ���� ������..
	if(m_pCurMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
		pc_Revival();
	
	return true;
}

void CPlayer::Resurrect()		//��Ȱ
{
	BYTE byErrCode = 0;//1;//��ü���°��ƴ� 2;//����������

	//��ü�������� Ȯ��
	if(!m_bCorpse)
	{
		byErrCode = 1;//��ü���°��ƴ�
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//��ȯ�����ִٸ� ���Ͻ�Ŵ
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
	//����ʵ����Ͱ� �ٲ�Ƿ� ��Ŷ�� ���� �״´�.
	SendMsg_MapOut(byMapOutType, pIntoMap->m_pMapSet->m_dwIndex);

	m_pCurMap->ExitMap(this, GetCurSecNum());
	SetCurSecNum(0xFFFFFFFF);

	if(pIntoMap != m_pCurMap)
	{
		if(byMapOutType != mapin_type_dungeon)//�������� ���°��� ������ �ʴ´�
		{
			//���忡 ������Ʈ..	
			if(m_pUserDB)
				m_pUserDB->Update_Map(pIntoMap->m_pMapSet->m_dwIndex, pfStartPos);
		}
	}

	if(s_pSelectObject == this)
		s_pSelectObject = NULL;	

	//�ݰݱ�����
	if(m_EP.m_bEff_State[_EFF_STATE::Res_Att])//PARAMETER EDIT (�ݰݱ�) 
		RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Res_Att);

	m_pCurMap = pIntoMap;	//�̵��� �ʰ�, ��ǥ�� �����Ѵ���.. mapin���� ����Ʈ�� �߰���Ų��.
	m_wMapLayerIndex = wLayerIndex;
	m_Param.SetMapCode(pIntoMap->m_pMapSet->m_dwIndex);
	SetCurPos(pfStartPos);
	memcpy(m_fTarPos, m_fCurPos, sizeof(float)*3);
	m_bMove = false;

	return true;
}

bool CPlayer::IntoMap(BYTE byMapInMode)
{
	//�����̶��.. active���ƾƴϸ� 
	if(m_pCurMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
	{//�ش緹�̾ �׾������� �ٽ� ������..
		if(!m_pCurMap->m_ls[m_wMapLayerIndex].IsActiveLayer())
			return false;
	}

	DWORD dwNewSec = CalcSecIndex();
	if(dwNewSec >= m_pCurMap->GetSecInfo()->m_nSecNum)
	{
		if(m_pUserDB)
		{
			m_pUserDB->ForceCloseCommand(false, 0);
			g_Main.m_logSystemError.Write("%s ��������: ���Ϳ��� (��:%s, ��ġ:%d, %d)", m_Param.GetCharName(), m_pCurMap->m_pMapSet->m_strCode, (int)m_fCurPos[0], (int)m_fCurPos[2]);
		}
		return false;
	}

	m_pCurMap->EnterMap(this, dwNewSec);
	
	SetCurSecNum(dwNewSec);

	SendData_PartyMemberPos();//��Ƽ���̶�� ��Ƽ������ ��ġ�� �˸���..

	return true;
}

void CPlayer::CalcDefTol()
{
	memset(m_dwTolValue, 0, sizeof(m_dwTolValue));

	if(!IsRidingUnit())	//�ʵ����..
	{	
		//����
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

		//����, �����
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
	else	//���ֻ���..
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
		case 1: //SP �ִ뷮 ���� 
			m_EP.SetEff_Rate( _EFF_RATE::SP_Mx, pEffect[i].fEffectValue, bEquip );
			ReCalcMaxHFSP(true);
			break;

		case 2: //��ų ���� ���� FP �Ҹ� ����
			m_EP.SetEff_Rate( _EFF_RATE::FP_Consum, pEffect[i].fEffectValue, bEquip );
			break;

		case 3:	//�Ϲݰ��� ���߷� ����
			for(t = 0; t < WP_CLASS_NUM; t++)
				m_EP.SetEff_Plus( _EFF_PLUS::GE_Hit_+t, pEffect[i].fEffectValue, bEquip );
			break;

		case 4: //�Ϲݰ��� ȸ���� ���� 
			m_EP.SetEff_Plus( _EFF_PLUS::GE_Avd, pEffect[i].fEffectValue, bEquip );
			break;

		case 5:	//HP FP �ִ뷮 ����
			m_EP.SetEff_Rate( _EFF_RATE::HP_Mx, pEffect[i].fEffectValue, bEquip );
			m_EP.SetEff_Rate( _EFF_RATE::FP_Mx, pEffect[i].fEffectValue, bEquip );
			ReCalcMaxHFSP(true);
			break;
	
		case 6:	//��� ���ݷ� ���� 
			for(t = 0; t < WP_CLASS_NUM; t++)
				m_EP.SetEff_Rate( _EFF_RATE::GE_AttFc_+t, pEffect[i].fEffectValue, bEquip );
			m_EP.SetEff_Rate( _EFF_RATE::FC_AttFc, pEffect[i].fEffectValue, bEquip );		
			m_EP.SetEff_Rate( _EFF_RATE::SK_AttFc, pEffect[i].fEffectValue, bEquip );		
			break;

		case 7:	//���� ����    
			m_EP.SetEff_Rate( _EFF_RATE::Part_Def, pEffect[i].fEffectValue, bEquip );
			break;

		case 8:	//��� ������ ��ų���� ���  
			m_EP.SetEff_Plus( _EFF_PLUS::SK_LvUp, pEffect[i].fEffectValue, bEquip );					
			break;

		case 9:	//����(�����ϰ� �ִµ��� ��Ƽ/��带 ������ �ٸ�����鿡�� �Ⱥ���)
			m_EP.SetEff_Plus( _EFF_PLUS::Transparency, 1, bEquip );		
			break;

		case 10://����Ʈ(���ڽ����� ����ڳ� ���ڽ������� ����� �÷��̾�� �� �� ����)
			m_EP.SetEff_Plus( _EFF_PLUS::Detect, 1, bEquip );		
			break;

		case 11://�������غ������ ����(�����ϰ� �ִµ��ȿ��� �ɷ��� ��� ������)
			m_EP.SetEff_Plus( _EFF_PLUS::Anti_CtDm, 1, bEquip );		
			break;

		case 12://�̵��ӵ�_����
			m_EP.SetEff_Plus( _EFF_PLUS::Move_Run_Spd, pEffect[i].fEffectValue, bEquip );		
			break;

		case 13://���ƹ�Ÿ�� ���Ѻ�������
			m_EP.SetEff_Plus( _EFF_PLUS::Know_WeakPoint, 1, bEquip );
			break;

		case 14:	//fp ȸ���ӵ�
			m_EP.SetEff_Plus( _EFF_PLUS::FP_Rev_Add, pEffect[i].fEffectValue, bEquip );
			break;

		case 15:	//���� ���ݷ� ����
			m_EP.SetEff_Rate( _EFF_RATE::FC_AttFc, pEffect[i].fEffectValue, bEquip );
			break;

		case 16:	//�ִ� fp��
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
			if(byUdtInfo != __NO_TALIK)//Ż���߰�..
			{
				int nNum = 1;
				//�� ����Ż���� ��ΰ�..
				for(int t = i+1; t < byLv; t++)
				{
					BYTE byTemp = (BYTE)((dwUdtInfo_Buf>>(t*4))&0x0000000F);
					if(byUdtInfo == byTemp)
					{
						nNum++;
						dwUdtInfo_Buf |= 0x0000000F<<(t*4);//������ ������ ���������� �㿡�� �׳� �Ѿ����..
					}
				}

				_ItemUpgrade_fld* pUptFld = (_ItemUpgrade_fld*)g_Main.m_tblItemUpgrade.GetRecord(byUdtInfo);

				float fEffectValue = ((float*)&pUptFld->m_fUp1)[nNum-1];

				switch(byUdtInfo)
				{
				case talik_type_fc: //������_Ż��..���ݷ�����
					for(t = 0; t < WP_CLASS_NUM; t++)
						m_EP.SetEff_Rate( _EFF_RATE::GE_AttFc_+t, fEffectValue, bEquip );
					m_EP.SetEff_Rate( _EFF_RATE::FC_AttFc, fEffectValue, bEquip );		
					m_EP.SetEff_Rate( _EFF_RATE::SK_AttFc, fEffectValue, bEquip );		
					break;

				case talik_type_hp: //�ĸ���_Ż��..HP�����
					m_EP.SetEff_Rate( _EFF_RATE::HP_Absorb, fEffectValue, bEquip );
					break;

				case talik_type_fp: //������_Ż��..FP�����
					m_EP.SetEff_Rate( _EFF_RATE::FP_Absorb, fEffectValue, bEquip );
					break;

				case talik_type_stun:	//ȥ����_Ż��..Melee���� Stun ����Ȯ�� 
					m_EP.SetEff_Rate( _EFF_RATE::GE_Stun, fEffectValue, bEquip );
					break;

				case talik_type_dist: //������_Ż��..�����Ÿ�
					m_EP.SetEff_Plus( _EFF_PLUS::GE_Att_Dist_+wp_class_long, fEffectValue, bEquip );
					//m_EP.SetEff_Plus( _EFF_PLUS::GE_Att_Dist_+wp_class_throw, fEffectValue, bEquip );
				//	m_EP.SetEff_Plus( _EFF_PLUS::GE_Att_Dist_+wp_class_launcher, fEffectValue, bEquip );
					break;

				case talik_type_def:	//������_Ż��..���� 
					m_EP.SetEff_Rate( _EFF_RATE::Part_Def, fEffectValue, bEquip );
					break;
			
				case talik_type_contdam:	//������_Ż��..�������غ����ð� ���� 
					m_EP.SetEff_Rate( _EFF_RATE::FC_CtDm_Tm, fEffectValue, bEquip );
					break;

				case talik_type_fire:	//��ȭ��_Ż��..�ҳ�������
					if(pItem->m_byTableCode != tbl_code_weapon)
						m_EP.SetEff_Plus( _EFF_PLUS::Part_Tol_+TOL_CODE_FIRE, fEffectValue, bEquip );		
					break;

				case talik_type_water:	//�ų���_Ż��..���������� 
					if(pItem->m_byTableCode != tbl_code_weapon)
						m_EP.SetEff_Plus( _EFF_PLUS::Part_Tol_+TOL_CODE_WATER, fEffectValue, bEquip );		
					break;

				case talik_type_soil:	//��ȣ��_Ż��..�볻������
					if(pItem->m_byTableCode != tbl_code_weapon)
						m_EP.SetEff_Plus( _EFF_PLUS::Part_Tol_+TOL_CODE_SOIL, fEffectValue, bEquip );		
					break;

				case talik_type_wind://������_Ż��..�ٶ���������
					if(pItem->m_byTableCode != tbl_code_weapon)
						m_EP.SetEff_Plus( _EFF_PLUS::Part_Tol_+TOL_CODE_WIND, fEffectValue, bEquip );		
					break;

				case talik_type_rev://������_Ż��..HP ȸ���� ����
					m_EP.SetEff_Rate( _EFF_RATE::HP_Rev, fEffectValue, bEquip );
					break;

				case talik_type_avd://�ں���_Ż��..ȸ���� 
					m_EP.SetEff_Plus( _EFF_PLUS::GE_Avd, fEffectValue, bEquip );		
					break;
				}				
			}
		}
	}	
}


void CPlayer::SetHaveEffect()
{
	//Equip_Lv_Up ���̵� ����..
	float fEqLvupBuffer = m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up];

	//����ȿ������ �ʱ�ȭ..
	m_EP.m_fEff_Have[_EFF_HAVE::Gamble_Prof] = 0;
	m_EP.m_fEff_Have[_EFF_HAVE::Trade_Prof] = 0;
	m_EP.m_fEff_Have[_EFF_HAVE::Exp_Prof] = 0;
	m_EP.m_fEff_Have[_EFF_HAVE::Chat_All_Race] = 0;
	m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up] = 0;

	if(IsRidingUnit())//����ž�»��¶�� ����ȿ���� �ȸ��δ�..
		return;

	for(int i = 0; i < m_Param.GetBagNum()*one_bag_store_num; i++)
	{
		__ITEM* pItem = &m_Param.m_dbInven.m_pStorageList[i];
		if(pItem->m_bLoad)
		{
			if(pItem->m_byTableCode != tbl_code_res)
				continue;

			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex);

			if(pFld->m_nEffCode != -1 && pFld->m_nEffCode < _EFF_HAVE::NUM)//ȿ���� �ִ� �������̶��..
			{
				m_EP.m_fEff_Have[pFld->m_nEffCode] += (pFld->m_fEffUnit*pItem->m_dwDur);
				if(m_EP.m_fEff_Have[pFld->m_nEffCode] > pFld->m_fEffUnitMax)
					m_EP.m_fEff_Have[pFld->m_nEffCode] = pFld->m_fEffUnitMax;
			}			
		}
	}

	//����ȿ������ ������..
	m_EP.m_fEff_Have[_EFF_HAVE::Gamble_Prof] += 1.0f;
	m_EP.m_fEff_Have[_EFF_HAVE::Exp_Prof] += 1.0f;

	//���̵� ������ ����Ǹ� ����ȿ��ó��..
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
����������ų��..
�ڽ��� �׾�����..
�ڽ��� ������ ����������..
FP�� 0�϶�..
���� ��������..
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

	//�Ƶ巹���� ����
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

	//����ȿ��..
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
					if(!IsRidingUnit())				//����ž�»��°� �ƴ϶��
						m_pmWpn.FixWeapon(pItem);					//���⸦ ������Ų��.
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
					if(!IsRidingUnit())	//����ž�»��°� �ƴ϶��
						m_pmWpn.FixWeapon(NULL);					//���⸦ Ż����Ų��.
				}
			}
		}

		m_bDownCheckEquipEffect = false;
	}

	//�ִϸӽ� ��ȯ ��û ó��
	if(m_bNextRecallReturn)
	{
		_AnimusReturn();
		m_bNextRecallReturn = false;
	}

	//�������� ������ ���� ���� üũ..
	_CheckForcePullUnit();

	//�׷α׻���üũ
	m_pmGrg.CheckGroggy(dwCurTime);

	if(m_tmrIntervalSec.CountingTimer())
	{
		//��ġ���
		if(m_pCurMap && m_pUserDB)
		{
			if(m_pCurMap->m_pMapSet->m_nMapType == MAP_TYPE_STD)
			{	//�Ϲݸ��϶���..
				m_pUserDB->Update_Map(m_pCurMap->m_pMapSet->m_dwIndex, m_fCurPos, false);
			}
		}

		//������ �ִ��� üũ..
		m_bPosMainBase = m_pCurMap->IsBaseTown(m_fCurPos);

		//���� �����͸� �����͸��α�
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
	//��ġ��������ְ� �ٷ�����..
	CMapData* pMap = g_Main.m_MapOper.GetPosStartMap(m_Param.GetRaceCode(), m_fCurPos);
	m_Param.SetMapCode(pMap->m_pMapSet->m_dwIndex);

	g_Main.m_logSystemError.Write("CPlayer::OutOfSector() : %s.. Close", m_Param.GetCharName());
	g_Main.m_Network.Close(client_line, m_ObjID.m_wIndex, false);
}
*/
void CPlayer::SetLevel(BYTE byNewLevel)
{
	BYTE byOldLv = m_Param.GetLevel();
	
	if(byOldLv < byNewLevel)	//������ ������ �������.. ���ų� ���������� ġƮ..
	{
		//���忡 ������Ʈ..	
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

	//������ �ܰ谡�Ǹ� ������ �϶�� �˷���..
	if(m_Param.IsClassChangeableLv())
	{
		SendMsg_ChangeClassCommand();
	}
}

void CPlayer::AlterMoney(int nAlterGold, int nAlterDalant)
{
	DWORD dwGold = m_Param.GetGold();
	DWORD dwDalant = m_Param.GetDalant();

	if(nAlterGold > 0)//������..
	{
		if(m_Param.GetGold() > m_Param.GetGold() + nAlterGold)//over..
			dwGold = 0xFFFFFFFF;
		else
			dwGold = m_Param.GetGold() + nAlterGold;
	}
	else if(nAlterGold < 0)	//���ҽ�..
	{
		if(m_Param.GetGold() < m_Param.GetGold() + nAlterGold)//over..
			dwGold = 0;
		else
			dwGold = m_Param.GetGold() + nAlterGold;
	}

	if(nAlterDalant > 0)//������..
	{
		if(m_Param.GetDalant() > m_Param.GetDalant() + nAlterDalant)//over..
			dwDalant = 0xFFFFFFFF;
		else
			dwDalant = m_Param.GetDalant() + nAlterDalant;
	}
	else if(nAlterDalant < 0)	//���ҽ�..
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

		//���忡 ������Ʈ..	
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
	if(IsRidingUnit())	//����ž���߿��� ȸ���Ұ�
		return; 

	if(m_pmGrg.GetGroggy())
		return;			//�׷α���� ȸ���Ұ�

	if(m_EP.GetEff_State(_EFF_STATE::Rev_Lck))  //PARAMETER EDIT (ȸ�����)
		return;

	int nCurHP = GetHP();
	int nCurFP = GetFP();
	int nCurSP = GetSP();

	int nAlterHP = 0;
	int nAlterFP = 0;
	int nAlterSP = 0;

	//nAlterHP..Setting..
	//over�� ���¿����� ȸ����Ű���ʴ´�.

	//HP
	if(nCurHP < GetLimHP())
	{
		if(!m_bMove)//�������´� 4..
			nAlterHP += (4 * m_EP.GetEff_Rate(_EFF_RATE::HP_Rev));//PARAMETER EDIT (HP_ȸ����)
		else if(m_nMoveType == move_type_walk)//�ȱ���´� 1..
			nAlterHP += (2 * m_EP.GetEff_Rate(_EFF_RATE::HP_Rev));

		if(m_Param.GetLevel() > 7 && (float)nCurHP/GetLimHP() < 0.0f)//7�����ʰ��̸� ��HP�� 30%���ϸ� ȸ�������ʴ´�.
			nAlterHP = 0;
	}

	//FP
	if(nCurFP < GetLimFP())
	{
		if(!m_bMove)//�������´� 2..
			nAlterFP += (2 * m_EP.GetEff_Rate(_EFF_RATE::FP_Rev) + m_EP.GetEff_Plus(_EFF_PLUS::FP_Rev_Add)/100);//PARAMETER EDIT (FP_ȸ����)
		else if(m_nMoveType == move_type_walk)//�ȱ���´� 1..
			nAlterFP += (1 * m_EP.GetEff_Rate(_EFF_RATE::FP_Rev) + m_EP.GetEff_Plus(_EFF_PLUS::FP_Rev_Add)/100);//100���� ����..
	}

	//SP
//	if(nCurSP < GetLimSP())
	{
		float fEquipSpeedRate = CalcEquipSpeed();

		if(!m_bMove)	//�������´� 2
			nAlterSP += (2 *  m_EP.GetEff_Rate(_EFF_RATE::SP_Rev));//PARAMETER EDIT (SP_ȸ����)
		else if(m_bMove && m_nMoveType == move_type_walk)	//�ȱ���´� 1..
			nAlterSP += (1 *  m_EP.GetEff_Rate(_EFF_RATE::SP_Rev));
		else if(m_bMove && m_nMoveType == move_type_run && !m_bPosMainBase)	//�ٱ����̰� ������ �ִ»��°� �ƴҶ�..
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
			continue;//������ ���� ����..

		//�Ķ���Ͱ� maxġ���� ����..
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

		if(dwBuffer != p->m_dwLv)//���忡 ������Ʈ
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

	if(!m_bMove || m_pParkingUnit)	//�������³� ���� ��ŷ����
		nAlter = 2;
	else if(m_bMove && m_nMoveType == move_type_run)	//�ٱ���
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
	{//�⺻��..
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[i];

		if(pItem->m_bLoad)
		{		
			_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[i].GetRecord(pItem->m_wItemIndex);
			fSpeedRate *= pFld->m_fEquipSpeed;
		}
	}

	{//����..
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pItem->m_bLoad)
		{		
			_WeaponItem_fld* pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pItem->m_wItemIndex);
			fSpeedRate *= pFld->m_fEquipSpeed;
		}
	}

	{//����..
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

	//�⺻������Ʈ
	for(int i = 0; i < base_fix_num; i++)
	{
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[i];

		if(pItem->m_bLoad)
		{		
			_DfnEquipItem_fld* pFld = (_DfnEquipItem_fld*)g_Main.m_tblItemData[i].GetRecord(pItem->m_wItemIndex);
			nSpeed += pFld->m_nGASpd;
		}
	}

	//����..
	{
		__ITEM* pItem = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pItem->m_bLoad)
		{		
			_WeaponItem_fld* pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pItem->m_wItemIndex);
			nSpeed += pFld->m_nGASpd;
		}
	}

	//����..
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
	//��������
	BYTE byItemLv = ::GetItemEquipLevel(pCon->m_byTableCode, pCon->m_wItemIndex);
	if(byItemLv	> m_Param.GetLevel() + m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up]) //PARAMETER EDIT (�ڽ��� �������� ������ �������� �����Ҽ�����)
	{
	//	g_Main.m_logSystemError.Write("%s: IsEffectableEquip.. lv(%d) < itemlv(%d)", m_Param.GetCharName(), m_Param.GetLevel(), byItemLv);
		return false;
	}

	//���õ�����
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

	{//���忡 ������Ʈ..	
		if(m_pUserDB)
			m_pUserDB->Update_ItemAdd(byStorageCode, dwStorageIndex, pAddItem->m_byTableCode, pAddItem->m_wItemIndex, pAddItem->m_dwDur, pAddItem->m_dwLv);
	}

	//�����ɷ�ġ���	//����, ��� ����Ʈ�ϰ�� //�Ѿ�����
	if(byStorageCode == _STORAGE_POS::EQUIP || byStorageCode == _STORAGE_POS::EMBELLISH)
	{		
		//�����ܰ迡�� ���밡�ɿ��θ� üũ�����Ƿ� üũ����..
		SetEquipEffect(pCon, true);
		SetEffectEquipCode(byStorageCode, dwStorageIndex, effect_apply);
	}

	//�Ӽ����	//����, ��ű�..
	if(byStorageCode == _STORAGE_POS::EQUIP || byStorageCode == _STORAGE_POS::EMBELLISH)
	{
		if(pAddItem->m_byTableCode < base_fix_num || pAddItem->m_byTableCode == tbl_code_ring || pAddItem->m_byTableCode == tbl_code_amulet)
			CalcDefTol();
	}
	
	//���������� �� ���˸�	//�����ϰ��
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

	//����ȿ�� ������
	if(byStorageCode == _STORAGE_POS::INVEN)
	{
		if(pAddItem->m_byTableCode == tbl_code_res)
		{
			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[pAddItem->m_byTableCode].GetRecord(pAddItem->m_wItemIndex);

			if(pFld->m_nEffCode != -1)//ȿ���� �ִ� �������̶��..
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

	//���������� ��ũ�� �ִ°Ÿ� ������..

	{//���忡 ������Ʈ.. 
		if(m_pUserDB)
			m_pUserDB->Update_ItemDelete(byStorageCode, byStorageIndex);
	}

	//�����ɷ�ġ���	//����, ��� ����Ʈ�ϰ�� //�Ѿ�����
	//�Ӽ����	//����, ��ű�..
	if(byStorageCode == _STORAGE_POS::EQUIP || byStorageCode == _STORAGE_POS::EMBELLISH)
	{
		if(GetEffectEquipCode(byStorageCode, byStorageIndex) == effect_apply)
			SetEquipEffect(pCon, false);
		SetEffectEquipCode(byStorageCode, byStorageIndex, effect_none);

		if(pCon->m_byTableCode < base_fix_num || pCon->m_byTableCode == tbl_code_ring || pCon->m_byTableCode == tbl_code_amulet)
			CalcDefTol();
	}
	
	//���������� �� ���˸�	//�����ϰ��
	//�ݰݱ�����
	if(byStorageCode == _STORAGE_POS::EQUIP)
	{
		if(!bEquipChange)	//�ٲٱ����ؼ� �������� ����̳� ���������� �����ʴ´�..
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

		if(pCon->m_byTableCode == tbl_code_weapon && m_EP.m_bEff_State[_EFF_STATE::Res_Att])//PARAMETER EDIT (Res_Att) �ݰݱ�
		{
			RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Res_Att);
		}
	}

	//����ȿ�� ������
	if(byStorageCode == _STORAGE_POS::INVEN)
	{
		if(pCon->m_byTableCode == tbl_code_res)
		{
			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[pCon->m_byTableCode].GetRecord(pCon->m_wItemIndex);

			if(pFld->m_nEffCode != -1)//ȿ���� �ִ� �������̶��..
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

	//����ȿ�� ������
	if(byStorageCode == _STORAGE_POS::INVEN)
	{
		if(pCon->m_byTableCode == tbl_code_res)
		{
			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[pCon->m_byTableCode].GetRecord(pCon->m_wItemIndex);

			if(pFld->m_nEffCode != -1)//ȿ���� �ִ� �������̶��..
			{
				SetHaveEffect();
			}
		}
	}
	
	if(dwLeftDur == 0)
	{
		//���������� ��ũ�� �ִ°Ÿ� ������..

		//���忡 ������Ʈ.. 
		if(m_pUserDB)
			m_pUserDB->Update_ItemDelete(byStorageCode, byStorageIndex);

		//�����ɷ�ġ���	//����, ��� ����Ʈ�ϰ�� //�Ѿ�����
		if(byStorageCode == _STORAGE_POS::EQUIP)
		{
			if(GetEffectEquipCode(byStorageCode, byStorageIndex) == effect_apply)
				SetEquipEffect(pCon, false);
			SetEffectEquipCode(byStorageCode, byStorageIndex, effect_none);
		}

		//�Ӽ����	//����, ��ű�..
		if(byStorageCode == _STORAGE_POS::EQUIP || byStorageCode == _STORAGE_POS::EMBELLISH)
		{
			if(pCon->m_byTableCode < base_fix_num || pCon->m_byTableCode == tbl_code_ring || pCon->m_byTableCode == tbl_code_amulet)
				CalcDefTol();
		}
		
		//���������� �� ���˸�	//�����ϰ��
		if(byStorageCode == _STORAGE_POS::EQUIP)
		{
			UpdateVisualVer();
			//SendMsg_BreakdownEquipItem(pCon->m_byTableCode, wSerial);
			SendMsg_EquipPartChange(pCon->m_byTableCode);
		}	

		//�������� �ı����� �ƹ�Ÿ���� �˸���.
		SendMsg_DeleteStorageInform(byStorageCode, wSerial);
	}
	else
	{
		//Ŭ���̾�Ʈ���� ������ �˸���..
		if(bSend)
			SendMsg_AlterItemDurInform(byStorageCode, wSerial, dwLeftDur);

		//���忡 ������Ʈ..
		if(m_pUserDB)
			m_pUserDB->Update_ItemDur(byStorageCode, byStorageIndex, dwLeftDur, bUpdate);
	}
	
	return dwLeftDur;
}

void CPlayer::Emb_ItemUpgrade(BYTE byUpgradeType, BYTE byStorageCode, BYTE byStorageIndex, DWORD dwGradeInfo)
{
	_STORAGE_LIST::_storage_con* pCon = &m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[byStorageIndex];

	//�����ɷ»���
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

	//�����ɷ�
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
		{	//���⸦ ����, �ٿ�������� �Ӽ��� �ٲ���ִ�..
			m_pmWpn.FixWeapon((__ITEM*)pCon);
		}
	}

	//���������� �� ���˸�	//�����ϰ��
	if(byStorageCode == _STORAGE_POS::EQUIP)
	{
		UpdateVisualVer();
		SendMsg_EquipPartChange(pCon->m_byTableCode);
	}

	//���忡 ����..	
	if(m_pUserDB)
		m_pUserDB->Update_ItemUpgrade(byStorageCode, byStorageIndex, dwGradeInfo);
}

void CPlayer::Emb_UpdateStat(DWORD dwStatIndex, DWORD dwNewData)
{
	if(((DWORD*)&m_pmMst.m_BaseCum)[dwStatIndex] < dwNewData)
	{	//���ο���� ����������Ŭ���� �������ش�..
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

	if(bUpEquip)//�������� �����͸��� �ö��ٸ�..
		m_bUpCheckEquipEffect = true;

	int nStatIndex = _STAT_DB_BASE::GetStatIndex(byMasteryClass, byIndex);

	SendMsg_StatInform(nStatIndex, dwAfterCum);

	if(bUpMty)	//�����͸��� �ö��ٸ�..
		ReCalcMaxHFSP();

	if(m_pUserDB)
		m_pUserDB->Update_Stat(nStatIndex, dwAfterCum, false);

	//LV HISTORY(marking)..
	m_Param.m_bAlterMastery[nStatIndex] = true;
}

void CPlayer::Emb_RidindUnit(bool bRiding, CParkingUnit* pCreateUnit)
{			
	//����������
	UpdateVisualVer();
			
	if(!bRiding) //bRiding�� false�� pCreateUnit�� NULL�̸� �ȉ´�..
	{		
		//���������� �˸�..
		SendMsg_UnitRideChange(false, pCreateUnit);
			
		//�÷��̾ ��ŷ..
		m_pParkingUnit = pCreateUnit;
		m_dwUnitViewOverTime = 0xFFFFFFFF;
		m_EP.SetEffectable(true);
			
		//�ɷ�ġ ����..
		__ITEM* pWeaponItem = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pWeaponItem->m_bLoad)
			m_pmWpn.FixWeapon(pWeaponItem);		
		else
			m_pmWpn.FixWeapon(NULL);	
	}		
	else	
	{		
		//����ž���� �˸�..
		SendMsg_UnitRideChange(true, m_pParkingUnit);
			
		//�÷��̾ ��ŷ..
		m_pParkingUnit = NULL;
		m_EP.SetEffectable(false);
			
		//�ɷ�ġ ����..
		m_pmWpn.FixUnit(m_pUsingUnit);	
	}		
			
	//�������
	CalcDefTol();
			
	//����ȿ��.
	SetHaveEffect();
}

void CPlayer::ExitUpdateDataToWorld()
{
	if(!m_pUserDB)
		return;

	//���忡 ����..	
	_EXIT_ALTER_PARAM ad;

	ad.dwHP = m_Param.GetHP();
	ad.dwFP = m_Param.GetFP();
	ad.dwSP = m_Param.GetSP();
	ad.dExp = m_Param.GetExp();

	ad.byMapCode = m_Param.GetMapCode();
	memcpy(ad.fStartPos, m_Param.GetCurPos(), sizeof(float)*3);

	//��ũ
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

	//���� �ν���..
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
		return ;//��밡 ������ ��쿡�� 

	if(nDam <= 0)
		return;	//�������� 0���� Ŀ����..

	int nLvDif = (int)m_Param.GetLevel()-(int)pDst->GetLevel();
	if(nLvDif > 5)	//�������� ��뺸�� 5���� ũ�� ���ƾ��Ѵ�..
		return;

	CMonster* pMon = (CMonster*)pDst;	
	_monster_fld* pDstRec = (_monster_fld*)pDst->m_pRecordSet;

	if(IsRidingUnit())	//����ž�»���
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
	else //���͸� ���� ���..
	{
		DWORD dwMonEmo = pMon->GetEmotionState();
		if(dwMonEmo == AI_EM_MAD)
			fSetExt += pDstRec->m_fExt * 0.5f;
		else
			fSetExt += pDstRec->m_fExt * 0.3f;

		if(!m_pPartyMgr->IsPartyMode())
		{		//��Ƽ���� �ƴ϶��..
			AlterExp(fSetExt);
		}
		else	//��Ƽ���̶��..
		{
			CPlayer* pMember[member_per_party];
			BYTE byPartyNum = _GetPartyMemberInCircle(pMember, member_per_party);

			if(byPartyNum > 0)
			{
				fSetExt *= s_fExpDivUnderParty_Kill[byPartyNum-1];
			}

			//�ѷ������ϱ�..
			int nTotalLv = 0;
			for(int i = 0; i < byPartyNum; i++)
				nTotalLv += pMember[i]->GetLevel();

			//��Ƽ������ ����ġ ����
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
	//������üũ..
	if(max_level <= m_Param.GetLevel())
		return;

	if(fAlterExp > 0)
		fAlterExp *= m_EP.m_fEff_Have[_EFF_HAVE::Exp_Prof];//PARAMETER EDIT (����ġȹ���� �̵�)
	double dCurExp = m_Param.GetExp() + fAlterExp;

	if(dCurExp >= s_dExpLimPerLv[m_Param.GetLevel()])//������
	{
		m_Param.SetExp(0);		
		m_wExpRate = 0;

		BYTE byCurLv = GetLevel();
		SetLevel(byCurLv+1);	//������		
	}
	else
	{//����ġ�� �����Ŷ�� ����ġ�� ����..
		m_Param.SetExp(dCurExp);
		m_wExpRate = m_Param.GetExp() / s_dExpLimPerLv[m_Param.GetLevel()] * 10000;
		SendMsg_AlterExpInform();
	}

	if(m_pUserDB)
		m_pUserDB->Update_Exp(m_Param.GetExp());
}

bool CPlayer::ConsumEquipItem(int nTableCode, int nDam, __ITEM* poutItem/* = NULL*/)
{
	//���� �������� �������� �Ҹ�Ÿ� true�� ����
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
		if(pWpFld->m_nType == type_throw)	//������ �������� ������ 1�� ����
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
	//INT(80+SQRT(����*�����õ�^2)*10)

	float fHPMst = m_pmMst.GetMasteryPerMast(mastery_code_defence, 0);
	
	return 80 + sqrt(m_Param.GetLevel()*pow(fHPMst, 2)) * 10;
}

int	CPlayer::_CalcMaxFP()
{
/*
- �ִ�FP����: INT(40+SQRT(����*((����Ưȭ�������õ�(������:HOLY,�ڶ�:DARK) *0.4
              +FIRE���õ�*0.075+AQUA���õ�*0.1125+TERRA���õ�*0.1125+WIND���õ�*0.075)
              +(������ų���õ�*0.1125+���Ÿ���ų���õ�*0.1125))^2)*5)

*/
	float fFPMst = 0.0f;

	switch(m_Param.GetRaceSexCode() / 2)
	{
	case 0:	//������
		fFPMst = 
		m_pmMst.GetAveForceMasteryPerClass(fc_class_holy)*0.4f + 
		m_pmMst.GetAveForceMasteryPerClass(fc_class_fire)*0.075f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_aqua)*0.1125f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_terra)*0.1125f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_wind)*0.075f +
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_short)*0.1125f +
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_long)*0.1125f;
		break;

	case 1:	//�ڶ�
		fFPMst = 
		m_pmMst.GetAveForceMasteryPerClass(fc_class_dark)*0.4f + 
		m_pmMst.GetAveForceMasteryPerClass(fc_class_fire)*0.075f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_aqua)*0.1125f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_terra)*0.1125f +
		m_pmMst.GetAveForceMasteryPerClass(fc_class_wind)*0.075f +	
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_short)*0.1125f +
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_long)*0.1125f;
		break;

	case 2:	//��ũ���þ�
		fFPMst = 49.5f	+
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_short)*0.1125f +
		m_pmMst.GetAveSkillMasteryPerClass(attack_range_long)*0.1125f;
		break;
	}

	return 40 + sqrt(m_Param.GetLevel()*pow(fFPMst, 2)) * 5;
}

int CPlayer::_CalcMaxSP()
{
	//INT(160+SQRT(����*(�������ݼ��õ�*0.2+Ȱ���õ�*0.4+�Ѽ��õ�*0.4)^2)*2.5)

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

	//��󿡰� ���� �˸���..
	SendMsg_ModeChange((BYTE)m_nModeType);

	//���ٲ��� �ٷ� �˸�..
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

		//�ν��� �߰�..
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
				return ((_player_fld*)m_pRecordSet)->m_fMoveRunRate + m_EP.GetEff_Plus(_EFF_PLUS::Move_Run_Spd);//## PARAMETER EDIT (�̵��ӵ�) ##
			else
				return fUnitSpeed;//���Ŀ� �ν��� �߰�..
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
		if(!IsRidingUnit())	//����ž�»��°��ƴϸ�..
		{
			if(!m_bCheat_Matchless)
			{
				SetHP(m_Param.GetHP()-nDamage, false);
			}

			if(pDst)
			{
				if(m_Param.GetLevel() - nDstLv < 6)	
				{//�ڽŰ� ����� �������� 6���� ������..(5����)
					int nAddCum = GetMasteryCumAfterAttack(nDstLv);
					if(m_nLastBeatenPart == tbl_code_shield)	//���з� �������..
						Emb_AlterStat(mastery_code_shield, 0, nAddCum);
					else
						Emb_AlterStat(mastery_code_defence, 0, nAddCum);
				}
			}
		}
		else	//����ž�»��¶��
		{
			if(!m_bCheat_Matchless)
			{
				if(m_pUsingUnit->dwGauge > nDamage)
					m_pUsingUnit->dwGauge -= nDamage;
				else
					m_pUsingUnit->dwGauge = 0;
			}

			if(m_pUsingUnit->dwGauge == 0)	//������ �ı��Ÿ� �ڽŵ� �״»��°� ��
			{
				SetHP(0, false);
				
				//���忡 �˸�..
				m_pUserDB->Update_UnitDelete(m_pUsingUnit->bySlotIndex);

				//ITEM HISTORY..
				s_MgrItemHistory.destroy_unit(m_pUsingUnit->bySlotIndex, m_pUsingUnit->byFrame, m_szItemHistoryFileName);
			}
		}

		if(GetHP() == 0)
		{
			Corpse((CCharacter*)pDst);	//��ü�� ����
		}
	}

	//�ִϸӽ� ��ȯ�����ΰ�� �ִϸӽ����� �˸���..
	if(pDst && m_pRecalledAnimusChar)
		m_pRecalledAnimusChar->MasterBeAttacked_MasterInform(pDst);

	m_nLastBeatenPart = -1;

	return m_Param.GetHP();
}

int CPlayer::GetLimHP()
{
	return m_nMaxHP * m_EP.GetEff_Rate(_EFF_RATE::HP_Mx);	//PARAMETER EDIT ( HPȸ��_�ִ�ġ�� )
}

int CPlayer::GetLimFP()
{
	return m_nMaxFP * m_EP.GetEff_Rate(_EFF_RATE::FP_Mx);	//PARAMETER EDIT ( FPȸ��_�ִ�ġ�� )
}

int CPlayer::GetLimSP()
{
	return m_nMaxSP * m_EP.GetEff_Rate(_EFF_RATE::SP_Mx);	//PARAMETER EDIT ( SPȸ��_�ִ�ġ�� )
}

void CPlayer::SetHP(int nHP, bool bOver)
{ 
	int nCurHP = GetHP();

	if(!bOver)	//�����Ҽ�������..
	{
		if(nHP > nCurHP)	//������ ���..
		{
			if(nCurHP < GetLimHP() && nHP > GetLimHP())	//������ HP�� �Ѱ�ġ�����۰� �ٲ�°��� �Ѱ�ġ���ٴ� Ŭ��..
				nHP = GetLimHP();
			else if(nCurHP >= GetLimHP() && nHP >= nCurHP)	//������ HP�� �Ѱ�ġ����ũ��.. �ٲ�°��� ����ġ���ٴ� Ŭ��..
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

	if(!bOver)	//�����Ҽ�������..
	{
		if(nFP > nCurFP)	//������ ���..
		{
			if(nCurFP < GetLimFP() && nFP > GetLimFP())	//������ HP�� �Ѱ�ġ�����۰� �ٲ�°��� �Ѱ�ġ���ٴ� Ŭ��..
				nFP = GetLimFP();
			else if(nCurFP >= GetLimFP() && nFP >= nCurFP)	//������ HP�� �Ѱ�ġ����ũ��.. �ٲ�°��� ����ġ���ٴ� Ŭ��..
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

	if(!bOver)	//�����Ҽ�������..
	{
		if(nSP > nCurSP)	//������ ���..
		{
			if(nCurSP < GetLimSP() && nSP > GetLimSP())	//������ HP�� �Ѱ�ġ�����۰� �ٲ�°��� �Ѱ�ġ���ٴ� Ŭ��..
				nSP = GetLimSP();
			else if(nCurSP >= GetLimSP() && nSP >= nCurSP)	//������ HP�� �Ѱ�ġ����ũ��.. �ٲ�°��� ����ġ���ٴ� Ŭ��..
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

	// Attack ������ ���а� ������
	// parrying(���з� ���� ���)�� �����ߴ��� üũ�ؼ�
	// ������ ������ ������ �����Ѵ�.	

	bool bIgnorShield = false;
	if(pAttChar)
	{
		if(pAttChar->m_ObjID.m_byID == obj_id_player)
		{
			if(((CPlayer*)pAttChar)->m_pmWpn.byWpClass == wp_class_close)
				bIgnorShield = pAttChar->m_EP.GetEff_State(_EFF_STATE::Dst_No_Shd);	//## PARAMETER EDIT (���й���)
		}
	}

	__ITEM* pShieldCon = &m_Param.m_dbEquip.m_pStorageList[tbl_code_shield];

	bool bShieldable = false;
	if(pShieldCon->m_bLoad && GetEffectEquipCode(_STORAGE_POS::EQUIP, tbl_code_shield) == effect_apply)
		bShieldable = true;
	if(bShieldable)//��չ��⸦ �������Ȯ��..
	{	//��չ���� ���и� ���׷� ���������� ����ó��..
		__ITEM* pWpCon = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pWpCon->m_bLoad)
		{
			_WeaponItem_fld* pWpFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pWpCon->m_wItemIndex);
			if(((_WeaponItem_fld*)pWpFld)->m_nFixPart == handle_both)
				bShieldable = false;//��չ��⸦������� ȿ���� �������Ѵ�..
		}
	}

	if(pAttChar && bShieldable && !bIgnorShield)
	{	
		int l_nShieldSkill = m_pmMst.GetMasteryPerMast(mastery_code_shield, 0);
		if( l_nShieldSkill == 99 )		// parrying ���µ��� 99�̸� ������ ����
		{
			m_nLastBeatenPart = tbl_code_shield;
		}
		else
		{
			//70+(Parrying���õ�/99*30)
			DWORD l_dwSuccRate = 50+(l_nShieldSkill/99*30.0f);
			l_dwSuccRate = min(l_dwSuccRate, 95);
			
			if(m_rtPer100.GetRand() < l_dwSuccRate)
				m_nLastBeatenPart = tbl_code_shield;
		}
	} 

	float fTatalDefFC = 0.0f;

	if(m_nLastBeatenPart == tbl_code_shield)	//���з� �������..
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
		fTatalDefFC *= m_EP.GetEff_Rate(_EFF_RATE::Shield_Def);//## PARAMETER EDIT (���й���)	 
	}
	else
	{
		//���з� ������ ���..
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
	fTatalDefFC *= GetAdrenDef();	//�Ƶ巹����

	return (int)fTatalDefFC;
}

int CPlayer::GetDefSkill()
{
	if(IsRidingUnit())
	{
		//����ž�»��¶�� �Ӹ��� ��ü��Ʈ�� �������ʵ带 ���Ѵ�..
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
	return m_dwTolValue[TOL_CODE_FIRE] + m_EP.GetEff_Plus(_EFF_PLUS::Part_Tol_+TOL_CODE_FIRE); //## PARAMETER EDIT (�ҳ���)
}

int	CPlayer::GetWaterTol()
{
	return m_dwTolValue[TOL_CODE_WATER] + m_EP.GetEff_Plus(_EFF_PLUS::Part_Tol_+TOL_CODE_WATER); //## PARAMETER EDIT (������)
}

int	CPlayer::GetSoilTol()
{
	return m_dwTolValue[TOL_CODE_SOIL] + m_EP.GetEff_Plus(_EFF_PLUS::Part_Tol_+TOL_CODE_SOIL); //## PARAMETER EDIT (�볻��)
}

int	CPlayer::GetWindTol()
{
	return m_dwTolValue[TOL_CODE_WIND] + m_EP.GetEff_Plus(_EFF_PLUS::Part_Tol_+TOL_CODE_WIND); //## PARAMETER EDIT (ǳ����)
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
	//�����ڰ� ������ �������� ������õ��� ���Ѵ�.
	float fWSkill;//�����ڹ�����õ�

	if(m_pmWpn.byWpType == 0xFF)
		fWSkill = 10;//�Ǽ��ϰ�� 10����
	else if(m_pmWpn.byWpType == type_staff)
		fWSkill = 10;//�����̰����ϰ�� 10����	
	else
		fWSkill = m_pmMst.GetMasteryPerMast(mastery_code_weapon, m_pmWpn.byWpClass);

	//�����ں� ���ݼ�����
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

	//����ں� ������
	float fDstRate = 0.0f;//����ں�
	if(pDst->m_ObjID.m_byID == obj_id_player)//�÷����ϰ��
		fDstRate = ((CPlayer*)pDst)->m_Param.GetLevel() * 1 + pDst->GetDefSkill();
	else if(pDst->m_ObjID.m_byID == obj_id_monster)//������ ��� 
		fDstRate = pDst->GetDefSkill();

	//���� ���ݼ�����(�����ں� ���ݼ�����-����ں� ������)/4+70
	int nTotalRate = (int)(fOneRate - fDstRate) / 4 + 70;

	//�������ð��ݽ� �����Ŵ� ��..
	float fRate[base_fix_num] = {0.23f, 0.22f, 0.18f, 0.17f, 0.20f};
	float fPartRate = 5.0f * fRate[nPart];
	nTotalRate *= fPartRate;

	//ȿ������
	int nEffHitRate = m_EP.GetEff_Plus(_EFF_PLUS::GE_Hit_ + m_pmWpn.byWpClass); //## PARAMETER EDIT (�Ϲݸ���)
	int nEffAvdRate = pDst->m_EP.GetEff_Plus(_EFF_PLUS::GE_Avd);				//## PARAMETER EDIT (�Ϲ�ȸ��)

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
//1~10����	11~20����	21~30����	31~40����	41~50����
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

	//15�и��� üũ..
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
//#define FLAG_MOVE_TYPE			0	//�ȱ�, �ٱ� ����
//#define FLAG_MODE_TYPE			1	//����, ����������
//#define FLAG_STEALTH_STATE		2	//���ڽ�����
//#define FLAG_STUN_STATE			3	//���ϻ���
//#define FLAG_PARTY_STATE			4	//��Ƽ����
//#define FLAG_CORPSE_STATE			5	//��ü����
//#define FLAG_LEADER_STATE			6	//��Ƽ����	
//#define FLAG_DTRADE_STATE			7	//���ŷ���
//#define FLAG_DETECT_STATE			8	//detect���
//#define FLAG_OBSERVER_STATE		9	//���������
//#define FLAG_PARTYLOCK_STATE		10	//��Ƽ��ݸ��
//#define FLAG_MINING_STATE			11	//ä�����
//#define FLAG_GROGGY_STATE			12	//�׷α����

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

