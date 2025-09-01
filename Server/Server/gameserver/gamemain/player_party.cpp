#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"

void CPlayer::pc_PartyJoinInvitation(WORD wDstIndex)
{
	CPlayer* pDst = &g_Player[wDstIndex];

	//대상이 살아있고,  시체가 아니며.. 무소속이여야함..
	if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap) 
		return;

	if(pDst->m_pPartyMgr->IsPartyMode())
		return;

	if(m_pPartyMgr->m_bLock)
		return;

	//자신은 보스나 무소속이여야함..
	if(m_pPartyMgr->IsPartyBoss() || !m_pPartyMgr->IsPartyMode())
		SendMsg_PartyJoinInvitationQuestion(wDstIndex);	
}

void CPlayer::pc_PartyJoinInvitationAnswer(_CLID* pidBoss)
{
	CPlayer* pBoss = &g_Player[pidBoss->wIndex];

	if(pBoss->m_dwObjSerial != pidBoss->dwSerial)
		return;

	if(!pBoss->m_bLive)
		return;

	//무소속이여햐암
	if(m_pPartyMgr->IsPartyMode())
		return;

	if(pBoss->m_pPartyMgr->m_bLock)
		return;

	::wa_PartyJoin(&pBoss->m_id, &m_id);
}

void CPlayer::pc_PartyJoinApplication(WORD wBossIndex)
{
	CPlayer* pBoss = &g_Player[wBossIndex];
	
	if(!pBoss->m_bLive || pBoss->m_bCorpse)
		return;

	//자신은 무소속이여야함..
	if(m_pPartyMgr->IsPartyMode())
		return;

	//보스는 보스여야함..
	if(!pBoss->m_pPartyMgr->IsPartyBoss())
		return;

	if(pBoss->m_pPartyMgr->m_bLock)
		return;

	pBoss->SendMsg_PartyJoinApplicationQuestion(this);
}

void CPlayer::pc_PartyJoinApplicationAnswer(_CLID* pidApplicant)
{
	CPlayer* pJoiner = &g_Player[pidApplicant->wIndex];

	if(pJoiner->m_id.dwSerial != pidApplicant->dwSerial)
		return;

	if(!pJoiner->m_bLive || pJoiner->m_bCorpse || pJoiner->m_pCurMap != m_pCurMap) 
		return;

	if(m_pPartyMgr->m_bLock)
		return;

	::wa_PartyJoin(&m_id, &pJoiner->m_id);

}

void CPlayer::pc_PartyLeaveSelfReqeuest()
{
	//자신이 조직원인지 확인..
	if(!m_pPartyMgr->IsPartyMode())
	{
		SendMsg_PartyLeaveSelfResult(NULL, false);
		return;
	}

	::wa_PartySelfLeave(&m_id);
}

void CPlayer::pc_PartyLeaveCompulsionReqeuest(DWORD dwExiterSerial)
{
	//자신이 보스인지..
	if(!m_pPartyMgr->IsPartyBoss())
	{
		SendMsg_PartyLeaveCompulsionResult(NULL);
		return;
	}

	//강퇴자가 있는 자인가..
	CPartyPlayer* pLeaver = m_pPartyMgr->GetPtrFromSerial(dwExiterSerial);
	if(!pLeaver)
	{
		SendMsg_PartyLeaveCompulsionResult(NULL);
		return;
	}

	::wa_PartyForceLeave(&m_id, &pLeaver->m_id);
}

void CPlayer::pc_PartyDisJointReqeuest()
{
	//자신이 보스인지..
	if(!m_pPartyMgr->IsPartyBoss())
	{
		SendMsg_PartyDisjointResult(0);
		return;
	}

	::wa_PartyDisjoint(&m_id);
}

void CPlayer::pc_PartySuccessionReqeuest(DWORD dwSuccessorSerial)
{
	//자신이 보스인지..
	if(!m_pPartyMgr->IsPartyBoss())
	{
		SendMsg_PartySuccessResult(NULL);
		return;
	}

	//승계자가 있는 자인가..
	CPartyPlayer* pSuccessor = m_pPartyMgr->GetPtrFromSerial(dwSuccessorSerial);
	if(!pSuccessor)
	{
		SendMsg_PartySuccessResult(NULL);
		return;
	}

	::wa_PartySuccession(&m_id, &pSuccessor->m_id);
}

void CPlayer::pc_PartyLockReqeuest(bool bLock)
{
	//자신이 보스인지..
	if(!m_pPartyMgr->IsPartyBoss())
	{
		SendMsg_PartyLockResult(0xFF);
		return;
	}

	//락 상태인지..
	if(m_pPartyMgr->m_bLock == bLock)
	{
		SendMsg_PartyLockResult(0xFF);
		return;
	}

	::wa_PartyLock(&m_id, bLock);
}

