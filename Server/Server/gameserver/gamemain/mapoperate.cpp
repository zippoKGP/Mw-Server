// MapOperate.cpp: implementation of the CMapOperate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapOperate.h"
#include "Character.h"
#include <mmsystem.h>
#include "MyUtil.h"
#include <process.h>
#include "MainThread.h"
#include "CommonUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMapOperation::CMapOperation()
{	
	::InitR3Engine(USE_SERVER);	//R3engine 초기화

	m_bReSpawnMonster = true;
	m_nMapNum = 0;
	m_nStdMapNum = 0;
	m_Map = NULL;
	m_nLoopStartPoint = 0;
	m_tmrObjTerm.BeginTimer(obj_loop_term);
	m_tmrRealFixposTerm.BeginTimer(2000);
	m_tmrRecover.BeginTimer(2000);	
	m_dwLastTimeMonNumCheck = 0;
}

CMapOperation::~CMapOperation()
{
	if(m_Map)
		delete [] m_Map;

	::ReleaseR3Engine();	//R3engine 해제
}

bool CMapOperation::Init()
{
	m_nMapNum = g_Main.m_tblMapData.GetRecordNum();

	m_Map = new CMapData[m_nMapNum];

	if(!LoadMaps())	//맵 로드
		return false;
	return true;
}

bool CMapOperation::LoadMaps()	//맵 스크립트를 읽어서 로드..
{
	for(int i = 0; i < m_nMapNum; i++)
	{
		_map_fld* pSet = (_map_fld*)g_Main.m_tblMapData.GetRecord(i);

		m_Map[i].Init(pSet);

		::NetTrace("%s맵 로드시작", pSet->m_strCode);
		if(!m_Map[i].OpenMap(pSet->m_strFileName, pSet, true))
		{
			::MyMessageBox("Map Load Error", "%s - Read Error", pSet->m_strFileName);
			return false;
		}
		::NetTrace("%s맵 로드완료", pSet->m_strCode);

		if(pSet->m_nMapType == MAP_TYPE_STD)	//일반맵은 바로 NPC를 생성하고..
		{												//던젼맵은 실기간으로 액티브할때 생성시킨다..
			//상점 npc생성..

			for(int n = 0; n < m_Map[i].m_nItemStoreDumNum; n++)
			{
				CNPC* pNpc = ::FindEmptyNPC(g_NPC, MAX_NPC);
				if(!pNpc)
				{
					::MyMessageBox("error", "CMapOperation::LoadMaps() : NPC가 모질라..");
					break;
				}

				_npc_create_setdata Data;

				m_Map[i].GetRandPosInDummy(m_Map[i].m_pItemStoreDummy[n].m_pDumPos, Data.m_fStartPos, true);

				Data.m_pLinkItemStore = &m_Map[i].m_ItemStore[n];
				Data.m_pMap = &m_Map[i];
				Data.m_nLayerIndex = 0;
				Data.m_pRecordSet = m_Map[i].m_ItemStore[n].GetNpcRecord();

				pNpc->Create(&Data);
			}

			m_nStdMapNum++;
		}
	}

	CheckMapPortalLink();

	return true;
}	

void CMapOperation::OnLoop()
{
//	MoveCompleteProcess();

	//플레이어회복
	if(m_tmrRecover.CountingTimer())//2초마다..
	{
		for(int i = 0; i < MAX_PLAYER; i ++)
		{
			CPlayer* p = &g_Player[i];
			if(!p->m_bLive)
				continue;

			if(!p->m_bCorpse)
			{
				p->AutoRecover();

				if(p->m_Param.GetRaceSexCode()/2 == race_code_cora)//코라종족만..
					p->AutoRecover_Animus();
			}
			
			if(p->m_Param.GetRaceSexCode()/2 == race_code_bellato)//벨라토종족만..
				p->AutoCharge_Booster();
		}
	}

	//오브젝트 루프
	if(m_tmrObjTerm.CountingTimer())
	{
		if(m_nLoopStartPoint == 0)
		{
			::R3CalculateTime();

			if(m_bReSpawnMonster)
				RespawnMonster();

			for(int m = 0; m < m_nMapNum; m++)
			{
				if(m_Map[m].m_bUse)
					m_Map[m].OnLoop();
			}
		}

		//모든 오브젝트를 5파트로 나눠서 루프를 돌리자..
		for(int i = m_nLoopStartPoint; i < CGameObject::s_nTotalObjectNum; i += obj_loop_unit)
		{
			CGameObject* pObj = CGameObject::s_pTotalObject[i];
			if(pObj->m_bLive)
			{//dlatl..
			//	if(pObj->m_ObjID.m_byKind == obj_kind_char && pObj->m_ObjID.m_byID == obj_id_monster && pObj->m_ObjID.m_wIndex == 0)
					pObj->OnLoop();
			}
		}

		m_nLoopStartPoint++;
		m_nLoopStartPoint %= obj_loop_unit;

	}

	//오브젝트 리얼 메시지..
	if(m_tmrRealFixposTerm.CountingTimer())
	{
		DWORD dwCurTime = timeGetTime();

		for(int i = 0; i < CGameObject::s_nTotalObjectNum; i++)
		{
			CGameObject* pObj = CGameObject::s_pTotalObject[i];

			if(!pObj->m_bLive)
				continue;

			if(pObj->m_bMove)
				continue;

			//주위메시지를 2초이상안보낸것만 보낸다..
			if(dwCurTime - pObj->m_dwLastSendTime > real_fixpos_term)
			{
				pObj->SendMsg_RealFixPosition();
			}
		}
	}
}

void CMapOperation::RespawnMonster()
{
	if(CMonster::s_nLiveNum >= MAX_MONSTER)
		return;

	bool bReCheckRegenNum = false;
	DWORD dwCurTime = timeGetTime();

	//MON NUM CHECK..
	if(dwCurTime - m_dwLastTimeMonNumCheck > mon_num_check_term)
	{
		m_dwLastTimeMonNumCheck = dwCurTime;
		bReCheckRegenNum = true;
	}

	//RESPAWN..
	for(int m = 0; m < m_nMapNum; m++)//map수 만큼
	{
		CMapData* pMap = &m_Map[m];
		if(!pMap->m_bUse)
			continue;

		if(pMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
			continue;	//일반맵만..

		if(!pMap->m_ls[0].IsActiveLayer())
			continue;

		_MULTI_BLOCK* pb = pMap->m_ls[0].m_pMB;

		for(int b = 0; b < pMap->m_nMonBlockNum; b++)	//block수 만큼
		{
			_mon_block* pMonBlock = &pMap->m_pMonBlock[b];
			_mon_block_fld* pBlkRec = pMonBlock->m_pBlkRec;
			int nActNum = pb->m_ptbMonBlock[b].GetRecordNum();
			
			for(int d = 0; d < nActNum; d++)	//nActNum 만큼
			{
				_mon_active* pMonDummy = &pMap->m_ls[0].m_MonAct[b][d];
				_mon_active_fld* pDumRec = pMonDummy->m_pActRec;

				if(dwCurTime - pMonDummy->m_dwLastRespawnTime <= pDumRec->m_dwRegenTime)
					continue;
				
				pMonDummy->m_dwLastRespawnTime = dwCurTime;

				if(pMonDummy->m_wMonRecIndex == 0xffff)
					continue;

				if(pDumRec->m_dwRegenProp <= ::rand()%100)
					continue;

				if(bReCheckRegenNum)	//regen체크 시간이 되면 제한마리수를 다시 산정.
					pMonDummy->CalcNextRegenNum();

				int nExtra = pMonDummy->m_nCurRegenNum - pMonDummy->m_zCurMonNum;
				if(nExtra <= 0)
					continue;

				for(int e = 0; e < nExtra; e++)
				{
					if(pMonDummy->m_zCurMonNum >= pMonDummy->m_nCurRegenNum)
						break;

					int nSel = pMonBlock->SelectDummyIndex();
					if(nSel == -1)
						continue;

					if(!pMonBlock->m_pDumPos[nSel]->m_bPosAble)
						continue;

					CMonster* pMon = ::CreateRespawnMonster(pMap, 0, pMonDummy->m_wMonRecIndex, pMonDummy, pMonBlock->m_pDumPos[nSel], NULL);
					if(!pMon)
						return;
				}
			}
		}
	}
}

CMapData* CMapOperation::GetMap(int nIndex)
{
	if(nIndex >= m_nMapNum)
		return NULL;	

	return &m_Map[nIndex];
}

int CMapOperation::GetMap(CMapData* pMap)
{
	for(int i = 0; i < m_nMapNum; i++)
	{
		if(&m_Map[i] == pMap)
		{
			return i;
		}
	}

	return -1;
}

CMapData* CMapOperation::GetMap(char* szMapCode)
{
	for(int i = 0; i < m_nMapNum; i++)
	{
		if(!strcmp(m_Map[i].m_pMapSet->m_strCode, szMapCode))
		{
			return &m_Map[i];
		}
	}

	return NULL;
}

CMapData* CMapOperation::GetStartMap(BYTE byRaceCode)
{	
	int nMapNum = g_Main.m_tblMapData.GetRecordNum();
	for(int i = 0; i < nMapNum; i++)
	{
		_map_fld* pFld = (_map_fld*)g_Main.m_tblMapData.GetRecord(i);
		if(pFld->m_nRaceVillageCode == byRaceCode)
			return &m_Map[i];
	}

	return NULL;
}

CMapData* CMapOperation::GetPosStartMap(BYTE byRaceCode, float* pfoutPos)
{
	CMapData* pMap = GetStartMap(byRaceCode);
	if(!pMap)
		return NULL;

	if(!pMap->GetRandPosInDummy(pMap->m_pStartDummy->m_pDumPos, pfoutPos, true))
		return NULL;

	return pMap;
}

void CMapOperation::CheckMapPortalLink()
{
	for(int i = 0; i < m_nStdMapNum; i++)
	{
		CMapData* pMap = &m_Map[i];

		for(int p = 0; p < pMap->m_nPortalNum; p++)
		{
			//포털 인덱스 확인
			_portal_dummy* pCurPotal = pMap->GetPortal(p);

			//연결됀 맵확인..	=> 다른맵: 맵의 인덱스 알림..
			if(!pCurPotal->m_pPortalRec)
			{
				continue;
			}

			CMapData* pTarMap = GetMap(pCurPotal->m_pPortalRec->m_strLinkMapCode);
			if(!pTarMap)
			{
				if(strcmp(pCurPotal->m_pPortalRec->m_strLinkMapCode, "0"))
					g_Main.m_logSystemError.Write("Portal Link Check: %s.. %dth Portal: LinkMapCode(%s)", pMap->m_pMapSet->m_strCode, p, pCurPotal->m_pPortalRec->m_strLinkMapCode);
				continue;
			}
			//연결포탈확인
			_portal_dummy* pTarPotal = pTarMap->GetPortal(pCurPotal->m_pPortalRec->m_strLinkPortalCode);
			if(!pTarPotal)
			{
				g_Main.m_logSystemError.Write("Portal Link Check: %s.. %dth Portal: LinkPortalCode(%s)", pMap->m_pMapSet->m_strCode, p, pCurPotal->m_pPortalRec->m_strLinkPortalCode);
				continue;
			}
		}
	}
}


