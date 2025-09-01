#include "stdafx.h"
#include "BattleDungeon.h"
#include "pt_zone_client.h"
#include "MainThread.h"
#include "BattleDungeonItemFld.h"
#include "Monster.h"

int	CBattleDungeon::s_nLiveNum = 0;

DWORD CBattleDungeon::s_dwSerialCounter = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CBattleDungeon::CBattleDungeon()
{
	m_pTarMap = NULL;
	m_wTarLayer = 0xFFFF;
	m_pLayerSet = NULL;
	m_dwObjSerial = 0xFFFFFFFF;
	m_dwOpenTime = 0;
	m_nStateCode = dungeongate_state_normal;
}

void CBattleDungeon::Init(_object_id* pID)
{
	CGameObject::Init(pID);
	m_pTarMap = NULL;
	m_wTarLayer = 0xFFFF;
	m_pLayerSet = NULL;
	m_dwLastDestroyTime = 0;
	m_bOpen = false;
	m_tmrCheckActive.BeginTimer(1000);
}

bool CBattleDungeon::Create(_battledungeon_create_setdata* pParam)
{
	if(CGameObject::Create((_object_create_setdata*)pParam))
	{
		m_dwOpenerSerial = pParam->pOpener->m_id.dwSerial;
		strcpy(m_szOpenerName, pParam->pOpener->m_Param.GetCharName());

		m_dwObjSerial = s_dwSerialCounter++;

		m_dwOpenTime = timeGetTime();	//timer start
		m_nStateCode = dungeongate_state_normal;

		//던젼세팅..
		_BattleDungeonItem_fld* pFld = (_BattleDungeonItem_fld*)m_pRecordSet;
		m_pTarMap = pParam->pTarMap;
		m_wTarLayer = pParam->wTarLayerIndex;
		m_pLayerSet = &m_pTarMap->m_ls[m_wTarLayer];

		_MULTI_BLOCK* pMonBlkSet = &m_pTarMap->m_mb[pFld->m_nGenMob];
		m_pLayerSet->ActiveLayer(pMonBlkSet);	//레이어 스타트..

		//리스트 초기화..
		if(m_listActiveMonster.m_dwMaxBufNum == 0)
			m_listActiveMonster.SetList(max_monster);
		else
			m_listActiveMonster.ResetList();

		if(m_listActivePlayer.m_dwMaxBufNum == 0)
			m_listActivePlayer.SetList(max_player);
		else
			m_listActivePlayer.ResetList();
		
		//몬스터 생성
		CreateMob(1.0f);

		m_bOpen = true;
		m_nMissionKillMonNum = 0;
		m_nTotalKillMonNum = 0;
		m_dwTerminateTime = pFld->m_nTerminateTime*1000;
		s_nLiveNum++;

		SendMsg_Create();

		return true;
	}

	return false;
}

void CBattleDungeon::GateClose()
{
	m_bOpen = false;
	SendMsg_Destroy();
}

bool CBattleDungeon::Destroy(BYTE byDestroyCode)
{
	DWORD dwList[max_monster];

	int nNum = m_listActivePlayer.CopyIndexList(dwList, max_player);
	for(int i = 0; i < nNum; i++)
	{
		CPlayer* p = &g_Player[dwList[i]];

		if(p->m_bLive && p->m_pCurMap == m_pTarMap && p->m_wMapLayerIndex == m_wTarLayer)
		{		
			p->SendMsg_CloseDungeon(byDestroyCode);
			p->pc_OutofDungeon(byDestroyCode, this);
		}
	}

	nNum = m_listActiveMonster.CopyIndexList(dwList, max_monster);
	for(i = 0; i < nNum; i++)
	{
		CMonster* p = &g_Monster[dwList[i]];

		if(p->m_bLive)
			p->Destroy(mon_destroy_code_die, NULL);		//주기뿐다..	
	}

	m_pLayerSet->InertLayer();

	m_listActivePlayer.ResetList();
	m_listActiveMonster.ResetList();
		
	m_bOpen = false;
	m_dwObjSerial = 0xFFFFFFFF;
	m_pTarMap = NULL;
	m_pLayerSet = NULL;
	m_dwLastDestroyTime = timeGetTime();
	s_nLiveNum--;

	return CGameObject::Destroy();
}

bool CBattleDungeon::EnterPlayer(CPlayer* pPlayer)
{
	if(m_listActivePlayer.size() >= max_player)
		return false;

	if(m_listActivePlayer.PushNode_Back(pPlayer->m_ObjID.m_wIndex))
	{
		int nMemNum = m_listActivePlayer.size();
		if(nMemNum > 1)
		{
			_BattleDungeonItem_fld* pFld = (_BattleDungeonItem_fld*)m_pRecordSet;

			float fAddRate = 1.0f + pFld->m_fMobAdd * nMemNum;
			CreateMob(fAddRate);
		}
	}
	return true;
}

void CBattleDungeon::Loop()
{
	//상태체크
	DWORD dwTime = timeGetTime();
	if(m_nStateCode == dungeongate_state_normal)
	{
		if(m_dwOpenTime + tm_gate_hurry < dwTime)
		{
			m_nStateCode = dungeongate_state_hurry;
			SendMsg_StateChange();
		}
	}
	else if(m_nStateCode == dungeongate_state_hurry)
	{
		if(m_dwOpenTime + tm_gate_exist < dwTime)
		{
			GateClose();
		}	
	}

	if(!m_bOpen)
	{
		//던젼의 타임아웃체크..
		if(dwTime - m_pLayerSet->m_dwStartActiveTime > m_dwTerminateTime)
		{
			Destroy(dungeon_complete_code_timeout);
			return;
		}
	}

	if(m_tmrCheckActive.CountingTimer())	//던젼 미션을 완수햇는지..
	{
		_BattleDungeonItem_fld* pFld = (_BattleDungeonItem_fld*)m_pRecordSet;
		bool bComplete = false;
		int nCompleteCode = -1;
		int nCopyTotalKillMonNum = m_nTotalKillMonNum;

		//남은 몬스터수..
		DWORD dwList[max_monster];
		int nNum = m_listActiveMonster.CopyIndexList(dwList, max_monster);
		for(int i = 0; i < nNum; i++)
		{
			CMonster* p = &g_Monster[dwList[i]];

			if(!p->m_bLive)
			{
				if(pFld->m_nEndIF == end_case_specific_mon_die)
				{//특정 모블 잡아야 끝나는 던젼이라면 해당 몹인지..파악
					if(!strcmp(p->m_pRecordSet->m_strCode, pFld->m_strMobID))
						m_nMissionKillMonNum++;
				}
				else
					m_nMissionKillMonNum++;

				m_listActiveMonster.FindNode(dwList[i]);
				m_nTotalKillMonNum++;
			}
		}
		
		//남은 플레이어수..
		nNum = m_listActivePlayer.CopyIndexList(dwList, max_player);
		for(i = 0; i < nNum; i++)
		{
			CPlayer* p = &g_Player[dwList[i]];

			if(!p->m_bLive || p->m_pCurMap != m_pTarMap || p->m_wMapLayerIndex != m_wTarLayer)
			{
				m_listActivePlayer.FindNode(dwList[i]);
			}
		}
		
		if(m_nTotalKillMonNum != nCopyTotalKillMonNum)
			SendMsg_KillMonNum();//죽인마리수가 변경됬으면 던젼안에있는 플레이어에게 알림..

		if(m_nMissionKillMonNum >= pFld->m_nMobCount)
		{
			bComplete = true;
			nCompleteCode = dungeon_complete_code_mission;
		}

		if(m_listActiveMonster.size() == 0)
		{
			bComplete = true;
			nCompleteCode = dungeon_complete_code_mission;
		}
		if(m_listActivePlayer.size() == 0)
		{
			if(dwTime - m_dwOpenTime > tm_destroy_able)
			{	//게이트가 닫힌이후로 플레이어가 없으면 닫는다.
				bComplete = true;	
				nCompleteCode = dungeon_complete_code_timeout;
			}
		}

		if(bComplete)
		{
			Destroy(nCompleteCode);
		}
	}
}

bool CBattleDungeon::IsEnterAblePlayer(CPlayer* p)
{
	if(!m_bOpen)
		return false;

	if(m_listActivePlayer.size() >= max_player)
		return false;

	_BattleDungeonItem_fld* pFld = (_BattleDungeonItem_fld*)m_pRecordSet;

	if(p->m_dwObjSerial == m_dwOpenerSerial)
		return true;

	if(pFld->m_bQualification == enter_quf_free)
		return true;

	if(pFld->m_bQualification == enter_guf_party)
	{
		if(p->m_pPartyMgr->IsPartyMode())
		{
			CPartyPlayer** ppMem = p->m_pPartyMgr->GetPtrPartyMember();
			if(ppMem)
			{
				for(int m = 0; m < member_per_party; m++)
				{
					if(ppMem[m])
					{
						if(ppMem[m]->m_id.dwSerial == m_dwOpenerSerial)
							return true;
					}
					else
						break;
				}
			}
		}
	}

	return false;
}

bool CBattleDungeon::GetEnterNewPos(_ENTER_DUNGEON_NEW_POS* OUT pPosData)
{
	_BattleDungeonItem_fld* pFld = (_BattleDungeonItem_fld*)m_pRecordSet;

	//연결포탈확인
	_portal_dummy* pTarPotal = m_pTarMap->GetPortal(pFld->m_strDummyName);
	if(!pTarPotal)
		return false;

	//연결좌표계산
	if(!m_pTarMap->GetRandPosInDummy(pTarPotal->m_pDumPos, pPosData->fPos, true))
		return false;

	pPosData->byMapCode = m_pTarMap->m_nMapIndex;
	pPosData->wLayerIndex = m_wTarLayer;

	return true;
}

int CBattleDungeon::GetLeftTimeoutSec()
{
	if(!m_pLayerSet)
		return 0;

	return (m_dwTerminateTime - (timeGetTime() - m_pLayerSet->m_dwStartActiveTime))/1000;//초단위..
}

void CBattleDungeon::CreateMob(float fAddRate)
{
	if(!m_pLayerSet->IsActiveLayer())
		return;

	_MULTI_BLOCK* pb = m_pLayerSet->m_pMB;

	for(int b = 0; b < m_pTarMap->m_nMonBlockNum; b++)	//block수 만큼
	{
		_mon_block* pMonBlock = &m_pTarMap->m_pMonBlock[b];
		_mon_block_fld* pBlkRec = pMonBlock->m_pBlkRec;
		int nActNum = pb->m_ptbMonBlock[b].GetRecordNum();
		
		for(int d = 0; d < nActNum; d++)	//nActNum 만큼
		{
			_mon_active* pMonDummy = &m_pLayerSet->m_MonAct[b][d];
			_mon_active_fld* pDumRec = pMonDummy->m_pActRec;
			DWORD dwRegenLimNum = pDumRec->m_dwRegenLimNum * fAddRate;
			if(dwRegenLimNum > max_monster)
				dwRegenLimNum = max_monster;

			if(pMonDummy->m_dwCumMonNum >= dwRegenLimNum)
				continue;	//던젼맵이고, 누적몬스터수가 한계마리수이상이상이면 더이상 안한다..

			if(pMonDummy->m_wMonRecIndex == 0xffff)
				continue;

			if(pDumRec->m_dwRegenProp <= ::rand()%100)
				continue;

			int nExtra = dwRegenLimNum - pMonDummy->m_dwCumMonNum;
			if(nExtra < 0) nExtra = 0;
			for(int e = 0; e < nExtra; e++)
			{
				int nSel = pMonBlock->SelectDummyIndex();
				if(nSel == -1)
					continue;

				if(!pMonBlock->m_pDumPos[nSel]->m_bPosAble)
					continue;

				if(m_listActiveMonster.size() >= max_monster)
					return;

				CMonster* pMon = ::CreateRespawnMonster(m_pTarMap, m_wTarLayer, pMonDummy->m_wMonRecIndex, pMonDummy, pMonBlock->m_pDumPos[nSel], this);
				if(pMon)
					m_listActiveMonster.PushNode_Back(pMon->m_ObjID.m_wIndex);
				else
					return;
			}
		}
	}
}

void CBattleDungeon::SendMsg_Create()
{
	_dungeongate_create_zocl Send;

	Send.wGateIndex = m_ObjID.m_wIndex;
	Send.dwGateSerial = m_dwObjSerial;
	Send.dwOpenerSerial = m_dwOpenerSerial;
	Send.wItemRecIndex = m_pRecordSet->m_dwIndex;
	Send.byState = (BYTE)m_nStateCode;
	::FloatToShort(m_fCurPos, Send.zPos, 3);

	BYTE byType[] = {dungeon_msg, dungeongate_create_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CBattleDungeon::SendMsg_Destroy()
{
	_dungeongate_destroy_zocl Send;
	
	Send.wGateIndex = m_ObjID.m_wIndex;
	Send.dwGateSerial = m_dwObjSerial;

	BYTE byType[] = {dungeon_msg, dungeongate_destroy_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CBattleDungeon::SendMsg_FixPosition(int n)
{
	if(!m_bOpen)
		return;

	_dungeongate_fixpositon_zocl Send;

	Send.wGateIndex = m_ObjID.m_wIndex;
	Send.dwGateSerial = m_dwObjSerial;
	Send.dwOpenerSerial = m_dwOpenerSerial;
	Send.wItemRecIndex = m_pRecordSet->m_dwIndex;
	Send.byState = (BYTE)m_nStateCode;
	::FloatToShort(m_fCurPos, Send.zPos, 3);

	BYTE byType[] = {dungeon_msg, dungeongate_fixpositon_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));
}

void CBattleDungeon::SendMsg_StateChange()
{
	_dungeongate_state_change_zocl Send;

	Send.byState = m_nStateCode;
	Send.wGateIndex = m_ObjID.m_wIndex;
	Send.dwGateSerial = m_dwObjSerial;

	BYTE byType[] = {dungeon_msg, dungeongate_state_change_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CBattleDungeon::SendMsg_KillMonNum()
{
	_killmon_dungeon_inform_zocl Send;

	Send.wTotalKillNum = m_nTotalKillMonNum;
	Send.wMissionKillNum = m_nMissionKillMonNum;

	BYTE byType[] = {dungeon_msg, killmon_dungeon_inform_zocl};

	DWORD dwList[max_player];
	int nNum = m_listActivePlayer.CopyIndexList(dwList, max_player);
	for(int i = 0; i < nNum; i++)
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(dwList[i], byType, (char*)&Send, sizeof(Send));
}

CBattleDungeon* FindEmptyDungeonGate(CBattleDungeon* pDungeonGate, int nMax)
{
	DWORD dwTime = timeGetTime();

	for(int i = 0; i < nMax; i++)
	{
		if(pDungeonGate[i].m_bLive)
			continue;

		if(dwTime - pDungeonGate[i].m_dwLastDestroyTime <= 60*1000)
			continue;

		return &pDungeonGate[i];			
	}	
	return NULL;
}

CBattleDungeon* FindDungeonFromLayer(CBattleDungeon* pDungeonGate, int nMax, CMapData* pMap, int nLayerIndex)
{
	for(int i = 0; i < nMax; i++)
	{
		CBattleDungeon* p = &pDungeonGate[i];

		if(!p->m_bLive)
			continue;

		if(p->m_pTarMap == pMap && p->m_wTarLayer == nLayerIndex)
			return p;
	}	
	return NULL;
}
