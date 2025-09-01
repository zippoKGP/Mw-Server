#include "stdafx.h"
#include "MainThread.h"

CPartyPlayer::CPartyPlayer()
{
	m_bLogin = false;
}

void CPartyPlayer::Init(WORD wIndex)
{
	m_bLogin = false;
	m_id.wIndex = wIndex;
	PartyListInit();
}

void CPartyPlayer::EnterWorld(_WA_AVATOR_CODE* pData, WORD wZoneIndex)
{
	m_bLogin = true;
	m_wZoneIndex = wZoneIndex;
	memcpy(&m_id, &pData->m_id, sizeof(_CLID));
	strcpy(m_szName, pData->m_szName);
//	m_byRaceSexCode = pData->m_byRaceSexCode;	

	PartyListInit();
}

void CPartyPlayer::ExitWorld(CPartyPlayer** OUT ppoutNewBoss)
{
	m_bLogin = false;
	m_id.dwSerial = 0xFFFFFFFF;

	if(!IsPartyMode())
		ppoutNewBoss = NULL;
	else
		m_pPartyBoss->RemovePartyMember(this, ppoutNewBoss);
}

void CPartyPlayer::PartyListInit()
{
	m_pPartyBoss = NULL;

	for(int i = 0; i < member_per_party; i++)
		m_pPartyMember[i] = NULL;	
	
	m_bLock = false;
}

bool CPartyPlayer::IsPartyMode()
{
	if(m_pPartyBoss)
		return true;
	return false;
}

bool CPartyPlayer::IsPartyBoss()
{
	if(m_pPartyBoss == this)
		return true;
	return false;
}

bool CPartyPlayer::IsPartyLock()
{
	if(m_pPartyBoss)
		return m_pPartyBoss->m_bLock;
	return false;
}

int CPartyPlayer::GetPopPartyMember()
{
	if(!IsPartyMode())
		return 0;

	int nCnt = 0;
	for(int i = 0; i < member_per_party; i++)
	{
		if(m_pPartyBoss->m_pPartyMember[i])
			nCnt++;
		else
			break;
	}
	return nCnt;
}

CPartyPlayer** CPartyPlayer::GetPtrPartyMember()
{
	if(!m_pPartyBoss)
		return NULL;

	return m_pPartyBoss->m_pPartyMember;
}

CPartyPlayer* CPartyPlayer::GetPtrFromSerial(DWORD dwWorldSerial)
{
	if(!m_pPartyBoss)
		return NULL;

	for(int i = 0; i < member_per_party; i++)
	{
		if(m_pPartyBoss->m_pPartyMember[i])
		{
			if(m_pPartyBoss->m_pPartyMember[i]->m_id.dwSerial == dwWorldSerial)
				return m_pPartyBoss->m_pPartyMember[i];
		}
		else
			return NULL;
	}

	return NULL;
}

bool CPartyPlayer::FoundParty(CPartyPlayer* pParticiper)
{
	if(IsPartyMode())
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::FoundParty(pParticiper[%s]) : IsPartyMode()", m_szName, pParticiper->m_szName);
		return false;
	}

	if(pParticiper->IsPartyMode())
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::FoundParty(pParticiper[%s]) : pParticiper->IsPartyMode()", m_szName, pParticiper->m_szName);
		return false;
	}

	PartyListInit();

	m_pPartyBoss = this;
	m_pPartyMember[0] = this;	

	InsertPartyMember(pParticiper);

	return true;
}

bool CPartyPlayer::DisjointParty()
{
	if(!IsPartyBoss())
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::DisjointParty() : IsPartyMode()", m_szName, m_szName);
		return false;
	}

	for(int i = 1; i < member_per_party; i++)//보스 다음 순위맴버부터 초기화 시킴..
	{
		if(m_pPartyMember[i])
		{
			m_pPartyMember[i]->PartyListInit();
		}
		else
			break;
	}

	PartyListInit();

	return true;
}

bool CPartyPlayer::InsertPartyMember(CPartyPlayer* pJoiner)
{
	if(!IsPartyBoss())
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::InsertPartyMember(pJoiner[%s]) : IsPartyBoss()", m_szName, pJoiner->m_szName);
		return false;
	}

	for(int i = 0; i < member_per_party; i++)
	{
		if(!m_pPartyMember[i])
		{
			m_pPartyMember[i] = pJoiner;
			pJoiner->m_pPartyBoss = this;
			return true;
		}
	}

	//::NetTrace(">>ERROR: [%s]CPartyPlayer::InsertPartyMember(pJoiner[%s]) : 파티정원이 참)", m_szName, pJoiner->m_szName);
	return false;
}

bool CPartyPlayer::RemovePartyMember(CPartyPlayer* pExiter, CPartyPlayer** ppoutNewBoss)
{
	if(!IsPartyBoss())
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::RemovePartyMember(pExiter[%s]) : IsPartyBoss()", m_szName, pExiter->m_szName);
		return false;
	}

	if(pExiter->m_pPartyBoss != this)
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::RemovePartyMember(pExiter[%s]) : pExiter[%s]->m_pPartyBoss[%s] != this[%s]", m_szName, pExiter->m_szName, pExiter->m_pPartyBoss->m_szName, m_szName);
		return false;
	}

	bool bExist = false;
	for(int i = 0; i < member_per_party; i++)
	{
		if(m_pPartyMember[i] == pExiter)
		{
			bExist = true;
			break;
		}
	}
	if(!bExist)
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::RemovePartyMember(pExiter[%s]) : 나가는사람이 리스트에 없다", m_szName, pExiter->m_szName);
		return false;
	}

	//현재 파티멤버가 2명이하라면 해체한다.
	int nPartyNum = GetPopPartyMember();
	if(nPartyNum <= 2)
	{
		DisjointParty();
		if(ppoutNewBoss)
		{
			*ppoutNewBoss = NULL;
		}
		return true;
	}

	CPartyPlayer* pNewBoss = this;

	//나가는 자가 보스라면 후순위자에게 보스부터 승계한다.
	if(this == pExiter)
	{
		//승계자리스트로 복사..
		pNewBoss = m_pPartyMember[1];
		pNewBoss->PartyListInit();

		pNewBoss->m_pPartyBoss = pNewBoss;
		for(int i = 1; i < member_per_party; i++)
		{
			if(m_pPartyMember[i])
			{
				pNewBoss->m_pPartyMember[i-1] = m_pPartyMember[i];
				pNewBoss->m_pPartyMember[i-1]->m_pPartyBoss = pNewBoss;
			}
		}

		//맴버 밑부분 다 null로.
		for(i = nPartyNum-1; i < member_per_party; i++)
		{
			pNewBoss->m_pPartyMember[i] = NULL;
		}
	}
	else
	{
		bool bFind = false;
		for(i = 1; i < member_per_party; i++)
		{
			if(m_pPartyMember[i] == pExiter)
			{
				bFind = true;
				for(int c = i+1; c < member_per_party; c++)
					m_pPartyMember[c-1] = m_pPartyMember[c];
				break;
			}
			else if(!m_pPartyMember[i])
				break;
		}

		//맴버 밑부분 다 null로.
		for(i = nPartyNum-1; i < member_per_party; i++)
		{
			m_pPartyMember[i] = NULL;
		}
	}

	pExiter->PartyListInit();

	if(ppoutNewBoss)
	{
		*ppoutNewBoss = pNewBoss;
	}
	
	return true;
}

bool CPartyPlayer::InheritBoss(CPartyPlayer* pSuccessor)
{
	if(!IsPartyBoss())
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::InheritBoss(pSuccessor[%s]) : !IsPartyBoss()", m_szName, pSuccessor->m_szName);
		return false;
	}

	if(pSuccessor->m_pPartyBoss != this)
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::InheritBoss(pSuccessor[%s]) : pSuccessor->m_pPartyBoss[%s] != this[%s]", m_szName, pSuccessor->m_szName, pSuccessor->m_pPartyBoss->m_szName, m_szName);
		return false;
	}

	//승계자 찾아서 순서를 바꿔준다.
	bool bFind = false;
	for(int i = 0; i < member_per_party; i++)
	{
		if(m_pPartyMember[i] == pSuccessor)
		{
			CPartyPlayer* pBuffer = m_pPartyMember[0];
			for(int m = 0; m < i; m++)	//보스에서 승계자까지 뒤로 한칸씩..
			{
				CPartyPlayer* p = pBuffer;
				pBuffer = m_pPartyMember[m+1];

				m_pPartyMember[m+1] = p;
			}

			m_pPartyMember[0] = pSuccessor;	//승계자를 맨앞으로..
			bFind = true;
			break;
		}
	}
	if(!bFind)
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::InheritBoss(pSuccessor[%s]) : 승계자가 리스트에 없음", m_szName, pSuccessor->m_szName);
		return false;
	}

	//복사..
	for(i = 0; i < member_per_party; i++)
		pSuccessor->m_pPartyMember[i] = m_pPartyMember[i];

	//파티원들 보스포인터 갱신..
	for(i = 0; i < member_per_party; i++)
	{
		if(pSuccessor->m_pPartyMember[i])
		{
			pSuccessor->m_pPartyMember[i]->m_pPartyBoss = pSuccessor;
		}
		else 
			break;
	}

	return true;
}

bool CPartyPlayer::SetLockMode(bool bLock)
{
	if(!IsPartyBoss())
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::SetLockMode() : !IsPartyBoss()", m_szName);
		return false;
	}

	if(m_bLock == bLock)
	{
		::NetTrace(">>ERROR: [%s]CPartyPlayer::SetLockMode() : m_bLock == bLock", m_szName, m_bLock, bLock);
		return false;
	}

	m_bLock = bLock;

	return true;
}

void wa_EnterWorld(_WA_AVATOR_CODE* pData, WORD wZoneIndex)
{
	CPartyPlayer* pAvator = &g_PartyPlayer[pData->m_id.wIndex];
	if(pAvator->m_bLogin)
	{
		::NetTrace(">>ERROR: [%s]wa_EnterWorld() : 이미 로그인중(%s)", pData->m_szName, pAvator->m_szName);
		return;
	}

	pAvator->EnterWorld(pData, wZoneIndex);
}

void wa_ExitWorld(_CLID* pidWorld)
{
	//::NetTrace(" ");

	CPartyPlayer* pAvator = &g_PartyPlayer[pidWorld->wIndex];
	
	if(pAvator->m_id.dwSerial != pidWorld->dwSerial)
	{
//		::NetTrace(">>ERROR: [%d]wa_ExitWorld() : 시리얼다름(%s)", pidWorld->dwSerial, pAvator->m_szName);
		return;
	}

	if(!pAvator->m_bLogin)
	{
//		::NetTrace(">>ERROR: [%d]wa_ExitWorld() : 이미 로그아웃중(%s)", pidWorld->dwSerial, pAvator->m_szName);
		return;
	}

	CPartyPlayer* pOldBoss = pAvator->m_pPartyBoss;
	CPartyPlayer* pNewBoss = pAvator->m_pPartyBoss;

	if(pAvator->IsPartyMode())
	{
		CPartyPlayer** ppMem = pAvator->GetPtrPartyMember();
		for(int m = 0; m < member_per_party; m++)
		{
			if(ppMem[m])
			{
				if(ppMem[m] == pAvator)
					continue;

				CPlayer* pMemberObj = &g_Player[ppMem[m]->m_wZoneIndex];
				pMemberObj->SendMsg_PartyLeaveSelfResult(pAvator, true);
				
				//::NetTrace(">>SEND: wa_ExitWorld : pMemberObj[%s]->SendMsg_PartyLeaveSelfResult(pExiter[%s])", pMemberObj->m_Param.GetCharName(), pAvator->m_szName);
			}
			else
				break;
		}
	}

	pAvator->ExitWorld(&pNewBoss);

	//보스가 바뀌면 바뀜을 알린다..
	if(pNewBoss && pNewBoss != pOldBoss)
	{
		CPartyPlayer** ppMem = pNewBoss->GetPtrPartyMember();
		for(int m = 0; m < member_per_party; m++)
		{
			if(ppMem[m])
			{
				CPlayer* pMemberObj = &g_Player[ppMem[m]->m_wZoneIndex];
				pMemberObj->SendMsg_PartySuccessResult(pNewBoss);	
			}
			else
				break;
		}
	}
}

void wa_PartyJoin(_CLID* pidBoss, _CLID* pidJoiner)
{
	//::NetTrace(" ");

	CPartyPlayer* pBoss = &g_PartyPlayer[pidBoss->wIndex];
	CPartyPlayer* pJoiner = &g_PartyPlayer[pidJoiner->wIndex];
	
	//avator check..
	//
	if(pBoss->m_id.dwSerial != pidBoss->dwSerial)
		return;

	if(pJoiner->m_id.dwSerial != pidJoiner->dwSerial)
		return;

	if(!pBoss->m_bLogin)
		return;

	if(!pJoiner->m_bLogin)
	{
		::NetTrace(">>ERROR: wa_PartyJoin : !pJoiner->m_bLogin");
//		return;
	}

	if(pJoiner->IsPartyMode())
	{
//		::NetTrace(">>ERROR: wa_PartyJoin : !pJoiner[%s]->IsPartyMode()", pJoiner->m_szName);
		return;
	}

	if(pBoss->IsPartyMode())
	{
		if(!pBoss->IsPartyBoss())
		{
//			::NetTrace(">>ERROR: wa_PartyJoin : !pBoss[%s]->IsPartyBoss()", pBoss->m_szName);
			return;
		}
	}
	if(pBoss->IsPartyLock())
	{
//		::NetTrace(">>ERROR: wa_PartyJoin : pBoss[%s]->IsLockMode()", pBoss->m_szName);
		return;
	}

	//data set..
	//
	if(pBoss->IsPartyBoss())
	{
		if(!pBoss->InsertPartyMember(pJoiner))
		{
			::NetTrace(">>ERROR: wa_PartyJoin : !pBoss[%s]->InsertPartyMember(pJoiner[%s])", pBoss->m_szName, pJoiner->m_szName);
			return;
		}
	}
	else if(!pBoss->IsPartyMode())
	{
		if(!pBoss->FoundParty(pJoiner))
		{
			::NetTrace(">>ERROR: wa_PartyJoin : !pBoss[%s]->FoundParty(pJoiner[%s])", pBoss->m_szName, pJoiner->m_szName);
			return;
		}
	}

//	CPlayer* pBossObj = NULL;
	CPlayer* pJoinerObj = &g_Player[pJoiner->m_wZoneIndex];

	//send data..
	//

	//파티원추가를 알림..
	for(int m = 0; m < member_per_party; m++)
	{
		CPartyPlayer* pMem = pBoss->m_pPartyMember[m];
		if(pMem)
		{
			if(pMem == pJoiner)
				continue;
			
			CPlayer* pMemberObj = &g_Player[pMem->m_wZoneIndex];
			pMemberObj->SendMsg_PartyJoinMemberResult(pJoiner);			//파티원추가를 알림
		}
		else
			break;
	}

	//가입자의 공유정보를 전부에게 보낸다. 
	if(pJoinerObj)
	{
		pJoinerObj->SendMsg_PartyJoinJoinerResult();	//가입자에게 기존멤버의 리스트를 준다..
	}

	//파티원의정보를 알린다.. =>SendMsg_PartyJoinMemberResult는 SetPartyMemberMsgType를 따라하는이유는 패킷 순서땜에..
	for(m = 0; m < member_per_party; m++)
	{
		CPartyPlayer* pMem = pBoss->m_pPartyMember[m];
		if(pMem)
		{
			if(pMem == pJoiner)
				continue;
			
			CPlayer* pMemberObj = &g_Player[pMem->m_wZoneIndex];
			pMemberObj->SendData_PartyMemberInfo(pJoiner->m_wZoneIndex);
			//	::NetTrace(">>wa_PartyJoin : pMemberObj(%s)->SendData_PartyMemberInfo(%s)", pMemberObj->m_Param.GetCharName(), pJoiner->m_szName);
		}
		else
			break;
	}
	if(pJoinerObj)
	{
		pJoinerObj->SendData_PartyMemberInfoToMembers();	//Joiner => Members
	//	::NetTrace(">>wa_PartyJoin : pJoinerObj(%s)->SendData_PartyMemberInfoToMembers()", pJoinerObj->m_Param.GetCharName());
	}
}

void wa_PartySelfLeave(_CLID* pidLeaver)
{
	CPartyPlayer* pLeaver = &g_PartyPlayer[pidLeaver->wIndex];
	CPartyPlayer* pBoss = pLeaver->m_pPartyBoss;
	
	//avator check..
	//
	if(pLeaver->m_id.dwSerial != pidLeaver->dwSerial)
		return;

	if(!pLeaver->m_bLogin)
		return;

	if(!pLeaver->IsPartyMode())
		return;

	if(!pBoss->IsPartyBoss())
		return;

	//member copy..
	//
	CPartyPlayer* pCopyMember[member_per_party];
	for(int m = 0; m < member_per_party; m++)
		pCopyMember[m] = pBoss->m_pPartyMember[m];

	//data set..
	//
	CPartyPlayer* pNewBoss = NULL;
	if(!pBoss->RemovePartyMember(pLeaver, &pNewBoss))
	{
		::NetTrace(">>ERROR: wa_PartySelfLeave : !pBoss[%s]->RemovePartyMember(pLeaver[%s], &pNewBoss)", pBoss->m_szName, pLeaver->m_szName);
		return;
	}

	//send data..
	//

	//복사된 파티원에게 알림..
	for(m = 0; m < member_per_party; m++)
	{
		if(pCopyMember[m])
		{
			CPlayer* pMemberObj = &g_Player[pCopyMember[m]->m_wZoneIndex];
			pMemberObj->SendMsg_PartyLeaveSelfResult(pLeaver, false);		

				//::NetTrace(">>SEND: wa_PartySelfLeave : pMemberObj[%s]->SendMsg_PartyLeaveSelfResult(pLeaver[%s])", pMemberObj->m_Param.GetCharName(), pLeaver->m_szName);
		}
		else
			break;
	}
}

void wa_PartyForceLeave(_CLID* pidBoss, _CLID* pidLeaver)
{
	//::NetTrace(" ");

	CPartyPlayer* pBoss = &g_PartyPlayer[pidBoss->wIndex];
	CPartyPlayer* pLeaver = &g_PartyPlayer[pidLeaver->wIndex];

	//avator check..
	//
	if(pBoss->m_id.dwSerial != pidBoss->dwSerial)
		return;

	if(pLeaver->m_id.dwSerial != pidLeaver->dwSerial)
		return;

	if(!pBoss->m_bLogin)
		return;

	if(!pLeaver->m_bLogin)
		return;

	if(!pLeaver->IsPartyMode())
		return;

	if(!pBoss->IsPartyBoss())
		return;

	if(pLeaver->m_pPartyBoss != pBoss)
		return;

	//member copy..
	//
	CPartyPlayer* pCopyMember[member_per_party];
	for(int m = 0; m < member_per_party; m++)
		pCopyMember[m] = pBoss->m_pPartyMember[m];

	//data set..
	//
	CPartyPlayer* pNewBoss = NULL;
	if(!pBoss->RemovePartyMember(pLeaver, &pNewBoss))
	{
		::NetTrace(">>ERROR: wa_PartyForceLeave : !pBoss[%s]->RemovePartyMember(pLeaver[%s], &pNewBoss)", pBoss->m_szName, pLeaver->m_szName);
		return;
	}

	//send data..
	//

	//복사된 파티원에게 알림..
	for(m = 0; m < member_per_party; m++)
	{
		if(pCopyMember[m])
		{
			CPlayer* pMemberObj = &g_Player[pCopyMember[m]->m_wZoneIndex];
			pMemberObj->SendMsg_PartyLeaveCompulsionResult(pLeaver);
				
				//::NetTrace(">>SEND: wa_PartyForceLeave : pMemberObj[%s]->SendMsg_PartyLeaveCompulsionResult(pLeaver[%s])", pMemberObj->m_Param.GetCharName(), pLeaver->m_szName);
		}
		else
			break;
	}
}	

void wa_PartyDisjoint(_CLID* pidBoss)
{
	//::NetTrace(" ");

	CPartyPlayer* pBoss = &g_PartyPlayer[pidBoss->wIndex];

	//avator check..
	//
	if(pBoss->m_id.dwSerial != pidBoss->dwSerial)
		return;

	if(!pBoss->IsPartyBoss())
		return;

	//member copy..
	//
	CPartyPlayer* pCopyMember[member_per_party];
	for(int m = 0; m < member_per_party; m++)
		pCopyMember[m] = pBoss->m_pPartyMember[m];

	//data set..
	//
	pBoss->DisjointParty();

	//복사된 파티원에게 알림..
	for(m = 0; m < member_per_party; m++)
	{
		if(pCopyMember[m])
		{
			CPlayer* pMemberObj = &g_Player[pCopyMember[m]->m_wZoneIndex];
			pMemberObj->SendMsg_PartyDisjointResult(1);	
				
			//::NetTrace(">>SEND: wa_PartyDisjoint : pMemberObj[%s]->SendMsg_PartyDisjointResult(1)", pMemberObj->m_Param.GetCharName());
		}
		else
			break;
	}
}

void wa_PartySuccession(_CLID* pidBoss, _CLID* pidSuccessor)
{
	//::NetTrace(" ");

	CPartyPlayer* pBoss = &g_PartyPlayer[pidBoss->wIndex];
	CPartyPlayer* pSuccessor = &g_PartyPlayer[pidSuccessor->wIndex];

	//avator check..
	//
	if(pBoss->m_id.dwSerial != pidBoss->dwSerial)
		return;

	if(pSuccessor->m_id.dwSerial != pidSuccessor->dwSerial)
		return;

	if(!pBoss->m_bLogin)
		return;

	if(!pSuccessor->m_bLogin)
		return;

	if(!pBoss->IsPartyBoss())
		return;

	if(!pSuccessor->IsPartyMode())
		return;

	if(pSuccessor->m_pPartyBoss != pBoss)
	{
//		::NetTrace(">>ERROR: wa_PartySuccession : pSuccessor[%s]->m_pPartyBoss[%s] != pBoss[%s]", pSuccessor->m_szName, pSuccessor->m_pPartyBoss->m_szName, pBoss->m_szName);
		return;
	}

	//data set..
	//
	if(!pBoss->InheritBoss(pSuccessor))
		return;

	//send data..
	//
	for(int m = 0; m < member_per_party; m++)
	{
		CPartyPlayer* pMem = pSuccessor->m_pPartyMember[m];
		if(pMem)
		{
			CPlayer* pMemberObj = &g_Player[pMem->m_wZoneIndex];
			pMemberObj->SendMsg_PartySuccessResult(pSuccessor);	
			//::NetTrace(">>SEND: wa_PartySuccession : pMemberObj[%s]->SendMsg_PartySuccessResult(pSuccessor[%s])", pMemberObj->m_Param.GetCharName(), pSuccessor->m_szName);
		}
		else
			break;
	}
}

void wa_PartyLock(_CLID* pidBoss, bool bLock)
{
	//::NetTrace(" ");

	CPartyPlayer* pBoss = &g_PartyPlayer[pidBoss->wIndex];

	//avator check..
	//
	if(pBoss->m_id.dwSerial != pidBoss->dwSerial)
		return;

	if(!pBoss->m_bLogin)
		return;

	if(!pBoss->IsPartyBoss())
		return;

	if(pBoss->m_bLock == bLock)
		return;

	//data set..
	//

	if(!pBoss->SetLockMode(bLock))
		return;

	//send data..
	//
	for(int m = 0; m < member_per_party; m++)
	{
		CPartyPlayer* pMem = pBoss->m_pPartyMember[m];
		if(pMem)
		{
			CPlayer* pMemberObj = &g_Player[pMem->m_wZoneIndex];
			pMemberObj->SendMsg_PartyLockResult(bLock);	

			//::NetTrace(">>SEND: wa_PartyLock : pMemberObj[%s]->SendMsg_PartyLockResult(bLock[%d])", pMemberObj->m_Param.GetCharName(), bLock);
		}
		else
			break;
	}
}




