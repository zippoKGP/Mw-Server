#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::BattleDungeonOpenRequest(int n, char* pBuf)
{
	_battledungeon_open_request_clzo* pRecv = (_battledungeon_open_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_OpenBattleDungeon(pRecv->dwItemSerial);

	return true;
}

bool CNetworkEX::GotoDungeonRequest(int n, char* pBuf)
{
	_goto_dungeon_request_clzo* pRecv = (_goto_dungeon_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->wGateIndex >= MAX_BATTLEDUNGEON)
		return false;

	pOne->pc_GotoDungeon(pRecv->wGateIndex, pRecv->dwGateSerial);

	return true;
}

bool CNetworkEX::OutofDungeonRequest(int n, char* pBuf)
{
	_outof_dungeon_request_clzo* pRecv = (_outof_dungeon_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pOne->m_pCurMap->m_pMapSet->m_nMapType != MAP_TYPE_DUNGEON)
		return false;

	pOne->pc_OutofDungeon(dungeon_complete_code_timeout, NULL);

	return true;
}
