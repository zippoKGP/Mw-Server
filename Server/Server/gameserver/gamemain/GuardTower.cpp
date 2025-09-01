#include "stdafx.h"
#include "GuardTower.h"
#include "MainThread.h"

int	CGuardTower::s_nLiveNum = 0;
DWORD CGuardTower::s_dwSerialCnt = 0;
CNetIndexList CGuardTower::s_listEmpty;

CGuardTower::CGuardTower()
{
	m_pMaster = NULL;

	if(!s_listEmpty.IsSetting())
		s_listEmpty.SetList(MAX_ITEMBOX);
}

CGuardTower::~CGuardTower()
{
}

bool CGuardTower::Init(_object_id* pID)
{
	CCharacter::Init(pID);
	
	m_dwLastDestroyTime = 0;

	s_listEmpty.PushNode_Back(pID->m_wIndex);

	return true;
}

bool CGuardTower::Create(_tower_create_setdata* pData)
{
	if(CCharacter::Create((_character_create_setdata*)pData))
	{
		m_nHP = pData->pItem->m_dwDur;
		m_pMaster = pData->pMaster;
		m_pItem = pData->pItem;
		m_wItemSerial = pData->pItem->m_wSerial;
		m_dwObjSerial = GetNewSerial();

		m_dwStartMakeTime = timeGetTime();
		m_bComplete = false;
		m_pTarget = NULL;

		SendMsg_Create();
		s_nLiveNum++;

		return true;
	}

	return false;
}

bool CGuardTower::Destroy(BYTE byDesType)
{
	m_dwLastDestroyTime = timeGetTime();

	if(m_pMaster && byDesType == tower_des_type_destroy)
		m_pMaster->_TowerDestroy(this);

	SendMsg_Destroy(byDesType);

	CCharacter::Destroy();

	m_pMaster = NULL;
	m_dwObjSerial = 0xFFFFFFFF;
	
	s_nLiveNum--;

	s_listEmpty.PushNode_Back(m_ObjID.m_wIndex);

	return true;
}

void CGuardTower::Attack(CCharacter* pTarget)
{
	CAttack at(this);

	_attack_param ap;

	ap.pDst = pTarget;
	ap.nPart = CCharacter::GetAttackRandomPart();
	ap.nTol = ((_GuardTowerItem_fld*)m_pRecordSet)->m_nProperty;
	ap.nClass = wp_class_long;
	ap.nMinAF = ((_GuardTowerItem_fld*)m_pRecordSet)->m_nGAMinAF;
	ap.nMaxAF = ((_GuardTowerItem_fld*)m_pRecordSet)->m_nGAMaxAF;
	ap.nMinSel = ((_GuardTowerItem_fld*)m_pRecordSet)->m_nGAMinSelProb;
	ap.nMaxSel = ((_GuardTowerItem_fld*)m_pRecordSet)->m_nGAMaxSelProb;

	at.AttackGen(&ap);

	int nTotalDam = 0;
	for(int i = 0; i < at.m_nDamagedObjNum; i++)
	{
		_be_damaged_char* p = &at.m_DamList[i];

		nTotalDam += p->m_nDamage;

		//스텔스 상태에서 공격받으면 스텔스 풀린다
		if(p->m_pChar->m_EP.GetEff_State(_EFF_STATE::Stealth))	//PARAMETER EDIT (STEALTH)
			p->m_pChar->RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Stealth);

		//투명 상태에서 공격받으면 잠시 풀린다.
		if(p->m_pChar->m_EP.GetEff_Plus(_EFF_PLUS::Transparency) > 0 && !p->m_pChar->m_bBreakTranspar)	//PARAMETER EDIT (Transparency)
			p->m_pChar->SetBreakTranspar(true);
	}

	//결과전송
	if(at.m_nDamagedObjNum > 0)
		SendMsg_Attack(&at);

	for(i = 0; i < at.m_nDamagedObjNum; i++)
		at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, GetLevel(), at.m_bIsCrtAtt);
}

void CGuardTower::Loop()
{
	DWORD dwCurTime = timeGetTime();
	if(!m_bComplete)//다 졌는지..
	{
		if(dwCurTime - m_dwStartMakeTime > ((_GuardTowerItem_fld*)m_pRecordSet)->m_nExpTime*1000)
			m_bComplete = true;
	}
	else
	{
		if(!m_pTarget)
		{	//타겟이 없을때는 새로 타겟을 찾는다..
			m_pTarget = SearchNearEnemy();
		}
		else
		{
			if(!IsValidTarget())
				m_pTarget = NULL;			
		}

		if(m_pTarget)
		{	//타겟이 있을때는 공격을 한다.
			if(dwCurTime > GetNextAttTime())			//공격딜레이 체크
			{
				Attack(m_pTarget);
				SetNextAttTime(dwCurTime + ((_GuardTowerItem_fld*)m_pRecordSet)->m_nGASpd);		
			}				
		}
	}
}

int CGuardTower::SetDamage(int nDam, CCharacter* pDst, int nDstLv, bool bCrt)
{
	if(nDam >= 1)
		m_nHP = max(int(m_nHP - nDam), 0);			

	if(m_nHP == 0)
		Destroy(tower_des_type_destroy);
	else
	{
		if(m_pMaster)	//유저에게 알리기..
			m_pMaster->SendMsg_AlterTowerHP(m_wItemSerial, m_nHP);

		//사정거리내라면 타겟으로..
		if(::GetSqrt(m_fCurPos, pDst->m_fCurPos) > ((_GuardTowerItem_fld*)m_pRecordSet)->m_nGADst)
		{	
			if(!m_pTarget)
				m_pTarget = pDst;
			else
			{	//우선순위비교
				if(pDst->GetLevel() < m_pTarget->GetLevel())
					m_pTarget = pDst;
			}
		}
	}
	
	return m_nHP;
}

bool CGuardTower::IsValidTarget()
{
	//그사이에 죽었거나 접속종료시 타겟해제
	if(!m_pTarget->m_bLive || m_pTarget->m_bCorpse)
		return false;

	//사정거리 벗어나면 타겟해제
	else if(::GetSqrt(m_fCurPos, m_pTarget->m_fCurPos) > ((_GuardTowerItem_fld*)m_pRecordSet)->m_nGADst)
		return false;

	//몬스터라면 타겟이 플레이어가 아니라면 타겟해제
	else if(m_pTarget->GetObjRace() == obj_race_monster)
	{
		bool bCont = false;	//타겟설정 지속가능여부
		CCharacter* pTar = ((CMonster*)m_pTarget)->GetAttackTarget();
		if(pTar)
		{
			if(!pTar->m_bLive || pTar->m_bCorpse)
				return false;

			//몬스터의 타겟이 플레이어 또는 애니머스인경우..
			if(pTar->m_ObjID.m_byID == obj_id_player || pTar->m_ObjID.m_byID == obj_id_animus)
			{	//몬스터의 타겟이 타종족일경우
				if(pTar->GetObjRace() != GetObjRace())
					bCont = true;
			}
		}
		if(!bCont)
			return false;
	}

	return true;
}

int	CGuardTower::GetGenAttackProb(CCharacter* pDst, int nPart)
{
	float fRes = ((_GuardTowerItem_fld*)m_pRecordSet)->m_nAttSklUnit - 
		float( pDst->GetLevel() * 10.f + pDst->GetDefSkill())/4.0 + 70.f;
	
	fRes = max(fRes, 5);
	fRes = min(fRes, 95);

	return fRes;
}

int CGuardTower::GetObjRace()
{	
	if(!m_pMaster)
		return -1;
	return m_pMaster->GetObjRace();		
}

void CGuardTower::SendMsg_Create()
{
	_tower_create_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zPos, 3);

	BYTE byType[msg_header_num] = {init_msg, tower_create_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CGuardTower::SendMsg_Destroy(BYTE byDesType)
{
	_tower_destroy_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.byDestroyCode = byDesType;
	
	BYTE byType[msg_header_num] = {init_msg, tower_destroy_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CGuardTower::SendMsg_Attack(CAttack* pAt)
{
	_attack_tower_inform_zocl Send;

	Send.idAtter.byID = m_ObjID.m_byID;
	Send.idAtter.wIndex = m_ObjID.m_wIndex;
	Send.idAtter.dwSerial = m_dwObjSerial;	
	
	Send.byAttackPart = pAt->m_pp->nPart;
	Send.bCritical = pAt->m_bIsCrtAtt;

	Send.idDst.byID = pAt->m_DamList[0].m_pChar->m_ObjID.m_byID;
	Send.idDst.wIndex = pAt->m_DamList[0].m_pChar->m_ObjID.m_wIndex;
	Send.idDst.dwSerial = pAt->m_DamList[0].m_pChar->m_dwObjSerial;
	
	Send.wDamage = pAt->m_DamList[0].m_nDamage;
	Send.wLeftHP = max(int(pAt->m_DamList[0].m_pChar->GetHP() - pAt->m_DamList[0].m_nDamage), 0);

	BYTE byType[msg_header_num] = {fight_msg, attack_tower_inform_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CGuardTower::SendMsg_FixPosition(int n)
{
	_tower_fixpositon_zocl Send;

	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);
	if(m_bComplete)
		Send.wCompLeftSec = 0;
	else
		Send.wCompLeftSec = (timeGetTime() - m_dwStartMakeTime)/1000;

	BYTE byType[msg_header_num] = {position_msg, tower_fixpositon_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));	
}

CCharacter* CGuardTower::SearchNearEnemy()
{	//타겟이 없을때..
	//우선 타종족 플레이어..
	//자신의 종족을 타겟으로 삼은 몬스터..

	_pnt_rect Rect; 
	CCharacter* pNearChar = NULL;
	Vector3f fNew;

	int nMonNum = 0;
	int nPlayerNum = 0;
	CCharacter* pMon[10];
	CCharacter* pPlayer[10];

	m_pCurMap->GetRectInRadius(&Rect, 2, GetCurSecNum());

	for(int h = Rect.nStarty; h <= Rect.nEndy; h++)
	{
		for(int w = Rect.nStartx; w <= Rect.nEndx; w++)
		{
			int nSecNum = w+h*m_pCurMap->GetSecInfo()->m_nSecNumW;

			CObjectList* pList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, nSecNum);
			if(!pList)
				continue;
			
			_object_list_point* pPoint = pList->m_Head.m_pNext;
			while(pPoint != &pList->m_Tail)
			{
				CGameObject* pObj = pPoint->m_pItem; 
				pPoint = pPoint->m_pNext;

				_object_id* pID = &pObj->m_ObjID;

				if(pObj->m_bCorpse)
					continue;

				if(pObj->GetObjRace() == GetObjRace())
					continue;

				//타종족인가..
				if(pID->m_byID == obj_id_player)
				{
				}
				else if(pID->m_byID == obj_id_monster)
				{	//몬스터인경우는 우리편을 공격하는 것..
					CCharacter* pTar = ((CMonster*)pObj)->GetAttackTarget();
					if(!pTar)
						continue;

					else if(pTar->m_ObjID.m_byID == obj_id_player)
					{
						if(pTar->GetObjRace() != GetObjRace())
							continue;
					}
				}
				else
				{
					continue;
				}
				
				if(((CCharacter*)pObj)->GetStealth())
					continue;

				if(abs(pObj->m_fCurPos[1] - m_fCurPos[1]) > 100.0f)	
					continue;

				if(((_GuardTowerItem_fld*)m_pRecordSet)->m_nGADst > ::GetSqrt(pObj->m_fCurPos, m_fCurPos))
				{	
					if(m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)pObj->m_fCurPos, &fNew))
					{
						if(pID->m_byID == obj_id_player)
						{
							pPlayer[nPlayerNum] = (CCharacter*)pObj;
							nPlayerNum++;
						}
						else
						{
							pMon[nMonNum] = (CCharacter*)pObj;
							nMonNum++;
						}
					}
				}
			}
		}
	}

	if(nPlayerNum > 0)//타종족이 우선..
		return pPlayer[rand()%nPlayerNum];
	if(nMonNum > 0)
		return pMon[rand()%nMonNum];

	return NULL;
}

CGuardTower* CreateGuardTower(CMapData* pMap, WORD wLayer, float* fPos, __ITEM* pItem, CPlayer* pMaster)
{
	CGuardTower* pTwr = NULL;
	DWORD dwTime = timeGetTime();
	DWORD dwNode;
	while(CGuardTower::s_listEmpty.CopyFront(&dwNode))
	{
		if(g_Tower[dwNode].m_bLive)
		{
			CGuardTower::s_listEmpty.PopNode_Front(&dwNode);
			g_Main.m_logSystemError.Write("FindEmptyTower() live = true (%d:item)", dwNode);
			continue;
		}

		if(dwTime - g_Tower[dwNode].m_dwLastDestroyTime < 30000)
			break;

		pTwr = &g_Tower[dwNode]; 
		break;
	}

	if(!pTwr)
		return NULL;
	
	_tower_create_setdata Data;

	Data.m_pMap = pMap;
	Data.m_nLayerIndex = wLayer;	
	Data.m_pRecordSet = g_Main.m_tblMonster.GetRecord(pItem->m_wItemIndex);
	if(!Data.m_pRecordSet)
		return NULL;

	memcpy(Data.m_fStartPos, fPos, sizeof(float)*3);

	Data.pMaster = pMaster;
	Data.pItem = pItem;

	if(!pTwr->Create(&Data))
		return NULL;

	CGuardTower::s_listEmpty.PopNode_Front(&dwNode);

	return pTwr;
}