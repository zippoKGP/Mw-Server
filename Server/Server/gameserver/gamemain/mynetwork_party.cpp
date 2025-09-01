#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::PartyJoinInvitation(int n, char* pBuf)
{
	_party_join_invitation_clzo* pRecv = (_party_join_invitation_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->wDstIndex >= MAX_PLAYER)
		return false;

	pOne->pc_PartyJoinInvitation(pRecv->wDstIndex);

	return true;
}

bool CNetworkEX::PartyJoinInvitationAnswer(int n, char* pBuf)
{
	_party_join_invitation_answer_clzo* pRecv = (_party_join_invitation_answer_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->idBoss.wIndex >= MAX_PLAYER)
		return false;	

	pOne->pc_PartyJoinInvitationAnswer(&pRecv->idBoss);

	return true;
}

bool CNetworkEX::PartyJoinApplication(int n, char* pBuf)
{
	_party_join_application_clzo* pRecv = (_party_join_application_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->wBossIndex >= MAX_PLAYER)
		return false;

	pOne->pc_PartyJoinApplication(pRecv->wBossIndex);

	return true;
}

bool CNetworkEX::PartyJoinApplicatiohAnswer(int n, char* pBuf)
{
	_party_join_application_answer_clzo* pRecv = (_party_join_application_answer_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->idApplicant.wIndex >= MAX_PLAYER)
		return false;

	pOne->pc_PartyJoinApplicationAnswer(&pRecv->idApplicant);

	return true;
}

bool CNetworkEX::PartyLeaveSelfRequest(int n, char* pBuf)
{
	_party_leave_self_request_clzo* pRecv = (_party_leave_self_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_PartyLeaveSelfReqeuest();

	return true;
}

bool CNetworkEX::PartyLeaveCompulsionRequest(int n, char* pBuf)
{
	_party_leave_compulsion_request_clzo* pRecv = (_party_leave_compulsion_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_PartyLeaveCompulsionReqeuest(pRecv->dwExiterSerial);

	return true;
}

bool CNetworkEX::PartyDisjointRequest(int n, char* pBuf)
{
	_party_disjoint_request_clzo* pRecv = (_party_disjoint_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_PartyDisJointReqeuest();

	return true;
}

bool CNetworkEX::PartySuccessionRequest(int n, char* pBuf)
{
	_party_succession_request_clzo* pRecv = (_party_succession_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_PartySuccessionReqeuest(pRecv->dwSuccessorSerial);

	return true;
}

bool CNetworkEX::PartyLockRequest(int n, char* pBuf)
{
	_party_lock_request_clzo* pRecv = (_party_lock_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_PartyLockReqeuest(pRecv->bLock);

	return true;
}