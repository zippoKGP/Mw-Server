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
	::InitR3Engine(USE_SERVER);	//R3engine �ʱ�ȭ

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

	::ReleaseR3Engine();	//R3engine ����
}

bool CMapOperation::Init()
{
	m_nMapNum = g_Main.m_tblMapData.GetRecordNum();

	m_Map = new CMapData[m_nMapNum];

	if(!LoadMaps())	//�� �ε�
		return false;
	return true;
}

bool CMapOperation::LoadMaps()	//�� ��ũ��Ʈ�� �о �ε�..
{
	for(int i = 0; i < m_nMapNum; i++)
	{
		_map_fld* pSet = (_map_fld*)g_Main.m_tblMapData.GetRecord(i);

		m_Map[i].Init(pSet);

		::NetTrace("%s�� �ε����", pSet->m_strCode);
		if(!m_Map[i].OpenMap(pSet->m_strFileName, pSet, true))
		{
			::MyMessageBox("Map Load Error", "%s - Read Error", pSet->m_strFileName);
			return false;
		}
		::NetTrace("%s�� �ε�Ϸ�", pSet->m_strCode);

		if(pSet->m_nMapType == MAP_TYPE_STD)	//�Ϲݸ��� �ٷ� NPC�� �����ϰ�..
		{												//�������� �ǱⰣ���� ��Ƽ���Ҷ� ������Ų��..
			//���� npc����..

			for(int n = 0; n < m_Map[i].m_nItemStoreDumNum; n++)
			{
				CNPC* pNpc = ::FindEmptyNPC(g_NPC, MAX_NPC);
				if(!pNpc)
				{
					::MyMessageBox("error", "CMapOperation::LoadMaps() : NPC�� ������..");
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

	//�÷��̾�ȸ��
	if(m_tmrRecover.CountingTimer())//2�ʸ���..
	{
		for(int i = 0; i < MAX_PLAYER; i ++)
		{
			CPlayer* p = &g_Player[i];
			if(!p->m_bLive)
				continue;

			if(!p->m_bCorpse)
			{
				p->AutoRecover();

				if(p->m_Param.GetRaceSexCode()/2 == race_code_cora)//�ڶ�������..
					p->AutoRecover_Animus();
			}
			
			if(p->m_Param.GetRaceSexCode()/2 == race_code_bellato)//������������..
				p->AutoCharge_Booster();
		}
	}

	//������Ʈ ����
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

		//��� ������Ʈ�� 5��Ʈ�� ������ ������ ������..
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

	//������Ʈ ���� �޽���..
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

			//�����޽����� 2���̻�Ⱥ����͸� ������..
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
	for(int m = 0; m < m_nMapNum; m++)//map�� ��ŭ
	{
		CMapData* pMap = &m_Map[m];
		if(!pMap->m_bUse)
			continue;

		if(pMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
			continue;	//�Ϲݸʸ�..

		if(!pMap->m_ls[0].IsActiveLayer())
			continue;

		_MULTI_BLOCK* pb = pMap->m_ls[0].m_pMB;

		for(int b = 0; b < pMap->m_nMonBlockNum; b++)	//block�� ��ŭ
		{
			_mon_block* pMonBlock = &pMap->m_pMonBlock[b];
			_mon_block_fld* pBlkRec = pMonBlock->m_pBlkRec;
			int nActNum = pb->m_ptbMonBlock[b].GetRecordNum();
			
			for(int d = 0; d < nActNum; d++)	//nActNum ��ŭ
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

				if(bReCheckRegenNum)	//regenüũ �ð��� �Ǹ� ���Ѹ������� �ٽ� ����.
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
			//���� �ε��� Ȯ��
			_portal_dummy* pCurPotal = pMap->GetPortal(p);

			//����� ��Ȯ��..	=> �ٸ���: ���� �ε��� �˸�..
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
			//������ŻȮ��
			_portal_dummy* pTarPotal = pTarMap->GetPortal(pCurPotal->m_pPortalRec->m_strLinkPortalCode);
			if(!pTarPotal)
			{
				g_Main.m_logSystemError.Write("Portal Link Check: %s.. %dth Portal: LinkPortalCode(%s)", pMap->m_pMapSet->m_strCode, p, pCurPotal->m_pPortalRec->m_strLinkPortalCode);
				continue;
			}
		}
	}
}


