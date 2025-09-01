// Monster.cpp: implementation of the CMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Monster.h"
#include "protocol.h"
#include "MyUtil.h"
#include "MainThread.h"

int	CMonster::s_nLiveNum = 0;
DWORD CMonster::s_dwSerialCnt = 0;
CNetIndexList CMonster::s_listEmpty;

#define RUNAWAY_TIME	9000
#define MAX_LIFE	600000//10분
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonster::CMonster()
{
	m_pPlayerTarget = NULL;

	if(!s_listEmpty.IsSetting())
		s_listEmpty.SetList(MAX_MONSTER);
}

CMonster::~CMonster()
{
}

bool CMonster::Init(_object_id* pID)
{
	CCharacter::Init(pID);	
	m_dwLastDestroyTime = 0;
	m_pBattleDungeon = NULL;

	s_listEmpty.PushNode_Back(pID->m_wIndex);
	
	return true;
}

bool CMonster::Create(_monster_create_setdata* pData)
{
	if(CCharacter::Create((_character_create_setdata*)pData))
	{
		if(pData->pActiveRec)
		{
			m_pActiveRec = pData->pActiveRec;
			m_pActiveRec->SetCurMonNum(+1);
			
			m_pDumPosition = pData->pDumPosition;
		}
		else
			m_pActiveRec = NULL;

		m_pBattleDungeon = pData->pBattleDungeon;

		m_dwObjSerial = GetNewMonSerial();

		InitMonster();
		m_LootMgr.Init();

		m_bTurn = (m_ObjID.m_wIndex%2 == 0 ? false : true);

		m_nHP = (int)((_monster_fld*)m_pRecordSet)->m_fMaxHP;
		SetMonterAI(this);
		SendMsg_Create();
		s_nLiveNum++;

		return true;
	}

	return false;
}

bool CMonster::Destroy(BYTE byDestroyCode, CGameObject* pAttObj)
{
	m_dwLastDestroyTime = timeGetTime();

	if (m_pPlayerTarget)
		m_pPlayerTarget->RemoveSlot(this);//자기가 죽었을경우 플레이어에 자신의 소켓을 지운다.	
	//
	m_pPlayerTarget = NULL;
	m_pMonTarget	= NULL;
	m_pSearchPlayer	= NULL;
	m_pAssistPlayer	= NULL;
	m_pSearchMon	= NULL;
	m_pHelpMonster	= NULL;	
	//

	SendMsg_Destroy(byDestroyCode);

	if(pAttObj)	//살해당한거라면..
	{
		CPlayer* pOwner = NULL;

		if(pAttObj->m_ObjID.m_byKind == obj_kind_char && pAttObj->m_ObjID.m_byID == obj_id_player)
			pOwner = (CPlayer*)pAttObj;
		else if(pAttObj->m_ObjID.m_byKind == obj_kind_char && pAttObj->m_ObjID.m_byID == obj_id_animus)
			pOwner = ((CAnimus*)pAttObj)->m_pMaster;

		if(pOwner)
			LootingItem(pOwner);//소환몬스터라면 아이템을 그라운딩하지 않는다.

		if(g_Main.m_pdwKillMonNum && m_pActiveRec)//일반맵에서 respawn된 몬스터만 대상..
			g_Main.m_pdwKillMonNum[m_pRecordSet->m_dwIndex]++;

		if(strcmp(((_monster_fld*)m_pRecordSet)->m_strChildMon, "0"))
		{	//죽을때 자식 몬스터가 있는경우..뿌린다..
			for(int i = 0; i < ((_monster_fld*)m_pRecordSet)->m_nChildMonNum; i++)
			{
				if(!::CreateRepMonster(m_pCurMap, m_wMapLayerIndex, m_fCurPos, ((_monster_fld*)m_pRecordSet)->m_strChildMon))//, NULL, NULL))
					break;
			}
		}
	}

	CCharacter::Destroy();
	if(m_pActiveRec)
	{
		if(pAttObj)
			m_pActiveRec->AddKillMonNum();
		m_pActiveRec->SetCurMonNum(-1);
		m_pActiveRec = NULL;
	}
	m_pBattleDungeon = NULL;

	m_dwObjSerial = 0xFFFFFFFF;
	
	s_nLiveNum--;

	s_listEmpty.PushNode_Back(m_ObjID.m_wIndex);
	return true;
}

///*MAYUN
void CMonster::InitMonster()
{
	m_StunTime		= 0;
	m_LifeMax		= MAX_LIFE + (::rand()%3)*60000;
	m_LifeCicle		= timeGetTime();
	m_RunAway		= false;
	m_pAssistPlayer	= NULL;
	m_pPlayerTarget	= NULL;
	m_pMonTarget	= NULL;
	m_pSearchPlayer = NULL;
	m_pSearchMon	= NULL;
	m_pHelpMonster	= NULL;
	m_CurNode		= 0;
	m_EndNode		= 0;
	
	m_bNode			= false;
	m_ChaseDist		= ((_monster_fld*)m_pRecordSet)->m_fViewExt;

	if ( (( _monster_fld*)m_pRecordSet)->m_bScaleChange)
	{
		m_Mightiness	= 0.8 + ((float)(::rand()%40 /100.f));
	}
	else
	{
		m_Mightiness	= 0.95+ ((float)(::rand()% 10 /100.f));
	}

	/*
	// 공격력 
	m_StdDmg	= ((_monster_record*)m_pRecordSet)->m_fAttFcStd;
	int halfstd		= m_StdDmg/2;
	m_MinDmg = halfstd + ::rand() % halfstd;
	m_MaxDmg = m_StdDmg * 2 - m_MinDmg;
	m_CritDmg	= int((float)m_MaxDmg*((125 + m_MaxDmg) / ( 50 + m_MaxDmg) + 0.5f));
	*/
	// 방어력 
	int weakpart =((_monster_fld*)m_pRecordSet)->m_fWeakPart + 1;
	float def = ((_monster_fld*)m_pRecordSet)->m_fStdDefFc * m_Mightiness;
	if (weakpart ==0)
	{
		for (int i =0;i<5;i++)
		{
			m_DefPart[i] =def;
		}
	}
	else if(weakpart<=6)
	{
		float r = 0;
		for (int i =0;i<5;i++)
		{
			r = (::rand()%20)/100.f;
			
			m_DefPart[i] =def * r + def;
		}
		if (weakpart!=6)
		{
			m_DefPart[weakpart-1] = def;
		}
	}
	//최대공격기술 
	//if (((_monster_record*)m_pRecordSet)->m_fAttTechID)m_MaxSkill = 2;
	//else m_MaxSkill = 1;
	m_MaxSkill = MAX_SKILL;
	//
	m_Scale =(( _monster_fld*)m_pRecordSet)->m_fScaleRate * m_Mightiness;
	/*
	if ( (( _monster_fld*)m_pRecordSet)->m_bScaleChange)
		m_Scale =(( _monster_fld*)m_pRecordSet)->m_fScaleRate * m_Mightiness;
	else
		m_Scale =(( _monster_fld*)m_pRecordSet)->m_fScaleRate;
	*/
	//
	memset(m_AttackedPlayer,0,sizeof(CCharacter	*)*max_attack_player);

	for (int i = 0; i<MAX_SKILL;i++)
	{
		m_Skill[i].Reset();
	}
}

int CMonster::GetDefFC( int nAttactPart, CCharacter* pAttChar)
{ 
	if (nAttactPart == -1)
	{
		int r = ::rand()%max_part;
		return m_DefPart[r];
	}
	else
	{
		return m_DefPart[nAttactPart];
	}
}

//*/
/*
void CMonster::Move()
{
	if(m_bMove)
	{		
//		float fCopyPos[3];
//		memcpy(fCopyPos, m_fCurPos, sizeof(float)*3);

		CCharacter::Move();

//		float fGap = fCopyPos[1] - m_fCurPos[1];
//		if(fGap < 0)
//			fGap = -fGap;

//		if(fGap > 50.0f)
//		{
//			double dsprt = GetSqrt(fCopyPos, m_fCurPos);
//			NetTrace("%s.Map, %d.Mon, 시작좌표(%d, %d, %d) 타켓좌표(%d, %d, %d) ygap:%d, dist:%d \n", 
//				m_pCurMap->m_pMapSet->m_strCode, m_ObjID.m_wIndex, (int)fCopyPos[0], (int)fCopyPos[1], (int)fCopyPos[2], 
//				(int)m_fCurPos[0], (int)m_fCurPos[1], (int)m_fCurPos[2], (int)fGap, (int)dsprt);
//		}
	}
}*/
/*
bool CMonster::MoveLater( float later )	
{
	int nAngle = ::GetYAngle(m_fCurPos, m_fTarPos);	
	int nSqrt = (int)::GetSqrt(m_fCurPos, m_fTarPos);
	float fLoopSpeed = ::R3GetLoopTime()*STANDARD_MOVE_SPEED;
	float fMoveSpeed = GetMoveSpeed(fLoopSpeed)*later;

	float fOldCur[3];
	memcpy(fOldCur, m_fCurPos, sizeof(float)*3);

	if(nSqrt <= fMoveSpeed)
	{
		m_fTarPos[1] = m_fCurPos[1];
		if(!m_pCurMap->m_Level.GetNextYposForServer(m_fTarPos, &m_fTarPos[1]))
		{
			if(!m_pCurMap->m_Level.GetNextYposForServerFar(m_fCurPos,m_fTarPos, &m_fTarPos[1]))
			{
				memcpy(m_fCurPos, fOldCur, sizeof(float)*3);
				memcpy(m_fTarPos, m_fCurPos, sizeof(float)*3);
			}
		}
		else		
			memcpy(m_fCurPos, m_fTarPos, sizeof(float)*3);

		Stop();

		return true;
	}
	else
	{
		double dAngleToPi = AngleToPi(nAngle);
	
		m_fCurPos[0] -= (float)sin(dAngleToPi)*fMoveSpeed;
		m_fCurPos[2] -= (float)cos(dAngleToPi)*fMoveSpeed;
	}

	BOOL bRet;

	m_fCurPos[1]=fOldCur[1];
	bRet = m_pCurMap->m_Level.GetNextYposForServerFar(fOldCur,m_fCurPos,&m_fCurPos[1]);

	if(!bRet)	//경사가 심할경우 올수있다.
	{
		Vector3f vectMin,vectMax;
		float fTempYpos;
		m_fCurPos[0]+=0.5f;		//틈사이때문에 그런거 방지.
		bRet = m_pCurMap->m_Level.GetNextYposForServerFar(fOldCur,m_fCurPos,&m_fCurPos[1]);
		m_fCurPos[0]-=0.5f;
		if( !bRet )	//안구해졌을경우 위쪽에서 가까운걸 얻어낸다.
		{
			Vector3fCopy(m_fCurPos,vectMin);
			Vector3fCopy(m_fCurPos,vectMax);
			vectMax[1]=30000;
			fTempYpos = m_pCurMap->m_Level.GetFirstYpos(m_fCurPos,vectMin,vectMax);
			if( fTempYpos == -65535 )	//안구해졌을경우 아래쪽에 가까운걸 얻어낸다.
			{
				vectMin[1]=-30000;
				fTempYpos = m_pCurMap->m_Level.GetFirstYpos(m_fCurPos,vectMin,vectMax);
				if( fTempYpos == -65535 )	//그래도 안구해졌을경우 암것두 없는 곳이다.
				{
					memcpy(m_fCurPos, fOldCur, sizeof(float)*3);
					memcpy(m_fTarPos, m_fCurPos, sizeof(float)*3);
					Stop();
					SendMsg_BreakStop();
					return true;
				}
			}
		}
	}
	return false;
}
*/
void CMonster::Loop()
{
	/*
	if(m_bMove)
	{
		if(m_pPlayerTarget)
			GetTarPosChase();
	}
	*/
	DWORD time = timeGetTime();

	if(!m_pBattleDungeon)	//battle dungeon 소속이 아닌것만..//ogogo(12/26)
	{
		DWORD life = time - m_LifeCicle;
		if (life > m_LifeMax )
		{
			Destroy(mon_destroy_code_respawn, NULL);	//Destroy()호출시 데이터가 초기화됨으로 반드시 바로 리턴해야한다...ogogo
			return;
		}
	}

	//
//	int nSqrt = (int)::GetSqrt(m_fCurPos, m_fTarPos);
//	float fLoopSpeed = ::R3GetLoopTime()*STANDARD_MOVE_SPEED;
//	float fMoveSpeed = GetMoveSpeed(fLoopSpeed);
//	float later = ( fMoveSpeed - nSqrt ) / fMoveSpeed;
	//
	//Move();
	//MoveLater( 0.3 );
	if ( m_StunTime )
	{
		if ( ( time - m_StunTime ) > ((_monster_fld*)m_pRecordSet)->m_fCrtMoTime )
		{
			m_StunTime = 0;
			SendMsg_Move();
		}
	}

	if ( !GetStun() && !m_StunTime)
	{
		//if(time - m_dwLastDestroyTime >= 1000)
		if(m_bTurn)
		{
		//	m_dwLastDestroyTime = time;

			m_MonAI.Move_Target_State.Process(time);		
			
			m_MonAI.Player_State.Process( time );
			if ( m_pSearchPlayer )
			{
				SendAIMessage( m_MonAI.Player_State,AI_EV_PLAYER_EXIST);
			}
			else
			{
				SendAIMessage( m_MonAI.Player_State,AI_EV_PLAYER_NONE);
			}
			//
			m_MonAI.Assist_State.Process(time);
		}
		else
		{
			m_MonAI.Mon_State.Process(time);
			m_MonAI.Emotion_State.Process(time);
			m_MonAI.Act_State.Process(time);
			m_MonAI.Con_State.Process(time);
			m_MonAI.Help_State.Process(time);
			//
			if(!m_bMove)
			{
				//SendAIMessage(m_MonAI.Move_Target_State,		AI_EV_MV_DONE,&delay);
				SendAIMessage(m_MonAI.Move_Target_State,		AI_EV_MV_DONE,((_monster_fld*)m_pRecordSet)->m_fWaitTime);
			}
		}
		
		if(m_pPlayerTarget)
			m_MonAI.Attack_State.Process(time);

		CCharacter::Move(GetMoveSpeed()*0.9f);//속도 보정..

		m_bTurn = !m_bTurn;
		//
	}
	//MoveLater( 0.7 );
	//GetTargetPosProcess();
	

	//DWORD	delay = 2000;
	
	CCharacter::UpdateSFCont();
}

//
//Get Paramter..
//
float CMonster::GetMoveSpeed()
{
	if(GetEmotionState() !=  AI_EM_NORMAL)
		return ((_monster_fld*)m_pRecordSet)->m_fWarMovSpd;
	return ((_monster_fld*)m_pRecordSet)->m_fMovSpd;
}

int CMonster::GetHP()
{
	return m_nHP;
}

void CMonster::SetHP(int nHP, bool bOver)
{
	if(nHP < 0)
		nHP = 0;

	m_nHP = nHP;	
}

int	CMonster::GetMaxHP()
{
	return ((_monster_fld*)m_pRecordSet)->m_fMaxHP;
}

void CMonster::InsertAttackedPlayer(CGameObject* pOri)
{
	//
	bool overlapped = false;
	if (pOri)//NULL일경우 
	{		//중복된 공격 플레이어가 있는가 확인한다.
		for(int i =0;i<max_attack_player;i++)
		{
			if (pOri==m_AttackedPlayer[i])
			{
				overlapped = true;
			}
		}	
	
		if (!overlapped)
		{
			CCharacter	*buffer[max_attack_player];
			memcpy(buffer,					&m_AttackedPlayer[0],	sizeof(CCharacter	*)*(max_attack_player-1));
			memcpy(&m_AttackedPlayer[1],	buffer,					sizeof(CCharacter	*)*(max_attack_player-1));
			m_AttackedPlayer[0] = (CCharacter*)pOri;
		}
	}
	//
	Vector3f fNew;
	if( !m_pPlayerTarget )
	{
		if( !m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)pOri->m_fCurPos, &fNew))	
		{
			double len = 100;
			if (!m_bNode)
			{
				DWORD cnt = 0;
				m_bNode = m_pCurMap->m_Level.GetPathFromDepth((Vector3f)m_fCurPos,(Vector3f)pOri->m_fCurPos,15,(Vector3f*)m_PlayerPos,&cnt);	//15Node,패스 파인더.
				m_CurNode = 0;
				m_EndNode = cnt;
				if ( m_bNode )
				{
					memcpy(m_MoveTarget,m_PlayerPos[0],sizeof(float)*3);
					m_pPlayerTarget = (CCharacter*) pOri;

					if(cnt > 10)
					{
						::__trace("MAP: %s, START: %f, %f, %f, ... TARGET: %f, %f, %f ... CNT: %d", 
							m_pCurMap->m_pMapSet->m_strCode, m_fCurPos[0], m_fCurPos[1], m_fCurPos[2], pOri->m_fCurPos[0], pOri->m_fCurPos[1], pOri->m_fCurPos[2], cnt);
					}
				}
				else
				{
					SendAIMessage(m_MonAI.Act_State,AI_EV_CON_BAD,RUNAWAY_TIME);			
				}
			}
		}
	}
	//
}

int CMonster::SetDamage(int nDamage, CCharacter* pDst, int nDstLv, bool bCrt)
{
	AttackObject(nDamage , pDst);

	if(nDamage >= 0)
	{
		if(pDst->m_ObjID.m_byID == obj_id_player)
		{
			m_LootMgr.PushDamage((CPlayer*)pDst, nDamage);
		}
		else if(pDst->m_ObjID.m_byID == obj_id_animus)
		{
			if(((CAnimus*)pDst)->m_pMaster)
				m_LootMgr.PushDamage(((CAnimus*)pDst)->m_pMaster, nDamage);
		}
	}

	if(nDamage >= 1)
		m_nHP = max(m_nHP-nDamage, 0);

	if(m_nHP == 0)
		Destroy(mon_destroy_code_die, m_LootMgr.GetLooter());

	if ( bCrt )
		m_StunTime = timeGetTime();

	return m_nHP;
}

float CMonster::GetWidth()
{
	return ((_monster_fld*)m_pRecordSet)->m_fWidth;
}

float CMonster::GetAttackRange()
{
	return ((_monster_fld*)m_pRecordSet)->m_fAttExt ;
}

//
//Send Message Load..
//
void CMonster::SendMsg_Create()
{
	_monster_create_zocl Create;

	Create.wIndex = m_ObjID.m_wIndex;
	Create.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Create.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Create.zPos, 3);

	//8/13
	Create.byInfo[0] = GetScale();
	Create.byInfo[1] = GetEmotionState();
	Create.byInfo[2] = GetConditionState();
	Create.byInfo[3] = GetActionState();
	//

	BYTE byType[msg_header_num] = {init_msg, monster_create_zocl};
	CircleReport(byType, (char*)&Create, sizeof(Create));
}

void CMonster::SendMsg_Destroy(BYTE byDestroyCode)
{
	_monster_destroy_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	Send.byDestroyCode = byDestroyCode;

	BYTE byType[msg_header_num] = {init_msg, monster_destroy_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CMonster::SendMsg_Move()
{
	if(!m_bMove)
		return;

	_monster_move_zocl Move;

	Move.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Move.wIndex = m_ObjID.m_wIndex;
	Move.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Move.zCur, 3);
	Move.zTar[0] = (short)m_fTarPos[0];
	Move.zTar[1] = (short)m_fTarPos[2];
	Move.wHPRate = (WORD)m_nHP;
	Move.wLastEffectCode = m_wLastContEffect;

	//8/13
	Move.byInfo[0] = GetScale();
	Move.byInfo[1] = GetEmotionState();
	Move.byInfo[2] = GetConditionState();
	Move.byInfo[3] = GetActionState();
	//

	BYTE byType[msg_header_num] = {position_msg, monster_move_zocl};
	CircleReport(byType, (char*)&Move, sizeof(Move));

//	::NetTrace("gooo: %d", timeGetTime());
}

void CMonster::SendMsg_FixPosition(int n)
{
	_monster_fixpositon_zocl Pos;

	Pos.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Pos.wIndex = m_ObjID.m_wIndex;
	Pos.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Pos.zCur, 3);	
	Pos.wHPRate = (WORD)m_nHP;
	Pos.wLastEffectCode = m_wLastContEffect;

	//8/13
	Pos.byInfo[0] = GetScale();
	Pos.byInfo[1] = GetEmotionState();
	Pos.byInfo[2] = GetConditionState();
	Pos.byInfo[3] = GetActionState();
	//

	BYTE byType[msg_header_num] = {position_msg, monster_fixpositon_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Pos, sizeof(Pos));	

	//디버그
/*	float y;
	if(!m_pCurMap->m_Level.GetNextYposFarProgress(m_fOldPos,m_fCurPos,&y))
	{
		m_pCurMap->m_Level.GetNextYposFarProgress(m_fOldPos,m_fCurPos,&y);		
	}
	if(fabs( y - Pos.zCur[1]) > 50)
	{
		m_pCurMap->m_Level.GetNextYposFarProgress(m_fOldPos,m_fCurPos,&y);		
	}*/

//	::__trace("mon fix:(idx:%d, sr:%d) x:%d y:%d z:%d", m_ObjID.m_wIndex, m_dwObjSerial, (int)m_fCurPos[0], (int)m_fCurPos[1], (int)m_fCurPos[2]);
}

void CMonster::SendMsg_RealMovePoint(int n)	//특정인한테..
{
	_monster_real_move_zocl Send;

	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);
	Send.zTar[0] = m_fTarPos[0];
	Send.zTar[1] = m_fTarPos[2];
	Send.wHPRate = (WORD)m_nHP;
	Send.wLastEffectCode = m_wLastContEffect;

	//8/13
	Send.byInfo[0] = GetScale();
	Send.byInfo[1] = GetEmotionState();
	Send.byInfo[2] = GetConditionState();
	Send.byInfo[3] = GetActionState();
	//

	BYTE byType[msg_header_num] = {position_msg, monster_real_move_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));
}

CCharacter* CMonster::SearchNearMonster()
{
	_pnt_rect Rect; 
	float MinLen = ((_monster_fld*)m_pRecordSet)->m_fMRefExt;
	CCharacter* pNearChar = NULL;
	Vector3f fNew;

	m_pCurMap->GetRectInRadius(&Rect, 6, GetCurSecNum());

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

				if(pID->m_byKind == obj_kind_char && pID->m_byID == obj_id_monster
					&& pID->m_wIndex !=m_ObjID.m_wIndex )
				{
					if(((CMonster*)pObj)->m_pPlayerTarget != NULL)
						continue;

					float fYGap = pObj->m_fCurPos[1]-m_fCurPos[1];
					if(fYGap < 0) fYGap = -fYGap;
					if(fYGap > 50.0f)
						continue;

					double len = ::GetSqrt(pObj->m_fCurPos, m_fCurPos);
					if(MinLen > len)
					{	
						if(m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)pObj->m_fCurPos, &fNew))
						{
							MinLen = len;
							pNearChar = (CCharacter*)pObj;
						}
					}
				}
			}
		}
	}
	return pNearChar;
}

CCharacter* CMonster::SearchNearPlayer()
{
	_pnt_rect Rect; 
	float MinLen = m_ChaseDist;
	CCharacter* pNearChar = NULL;
	Vector3f fNew;

	m_pCurMap->GetRectInRadius(&Rect, 6, GetCurSecNum());

	for(int h = Rect.nStarty; h <= Rect.nEndy; h++)
	{
		for(int w = Rect.nStartx; w <= Rect.nEndx; w++)
		{
			int nSecNum = w+h*m_pCurMap->GetSecInfo()->m_nSecNumW;

			CObjectList* pList = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, nSecNum);
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
				//2003/8/18
				if(((CCharacter*)pObj)->GetStealth())
					continue;
				
				float fYGap = pObj->m_fCurPos[1]-m_fCurPos[1];
				if(fYGap < 0) fYGap = -fYGap;
				if(fYGap > 30.0f)
					continue;

				double len = ::GetSqrt(pObj->m_fCurPos, m_fCurPos);
				if(MinLen > len)
				{	
					if(m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)pObj->m_fCurPos, &fNew))
					{
						MinLen = len;
						pNearChar = (CCharacter*)pObj;
					}
				}
			}
		}
	}
	if (pNearChar)
	{
		m_LifeCicle		= timeGetTime();//수명 연장~!
	}
	return pNearChar;
}

void CMonster::SendMsg_Attack_Gen(CCharacter* pTarget, int nDamage, int nAttPart, BOOL bCritical)
{
	_attack_monster_inform_zocl Send;

	Send.idAtter.byID = m_ObjID.m_byID;
	Send.idAtter.wIndex = m_ObjID.m_wIndex;
	Send.idAtter.dwSerial = m_dwObjSerial;
	Send.byAttackPart = nAttPart;
	Send.bCritical = (bCritical ? true : false) ;		

	Send.idDst.byID = pTarget->m_ObjID.m_byID;
	Send.idDst.wIndex = pTarget->m_ObjID.m_wIndex;
	Send.idDst.dwSerial = pTarget->m_dwObjSerial;
	Send.wDamage = nDamage;
	Send.wLeftHP = max(int(m_pPlayerTarget->GetHP() - nDamage), 0);

	BYTE byType[msg_header_num] = {fight_msg, attack_monster_inform_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CMonster::SendMsg_Attack_Force(CAttack* pAtt)
{/*
	if(pAtt->m_nFailureCode == 1)
	{
		_attack_force_success_other_zocl Send;

		Send.idAtter.byID = m_ObjID.m_byID;
		Send.idAtter.wIndex = m_ObjID.m_wIndex;
		Send.idAtter.dwSerial = m_dwObjSerial;
		Send.byAttackPart = pAtt->m_pp->nPart;
		Send.byForceIndex = pAtt->m_pp->pFld->m_dwIndex;
		Send.byForceLv = pAtt->m_pp->nLevel;		
		Send.byExtraInfo = pAtt->m_bIsCrtAtt;
		Send.zAreaPos[0] = pAtt->m_pp->fArea[0];
		Send.zAreaPos[1] = pAtt->m_pp->fArea[2];

		Send.byListNum = pAtt->m_nDamagedObjNum;
		for(int i = 0; i < pAtt->m_nDamagedObjNum; i++)
		{
			Send.DamList[i].idDster.byID = pAtt->m_DamList[i].m_pChar->m_ObjID.m_byID;
			Send.DamList[i].idDster.wIndex = pAtt->m_DamList[i].m_pChar->m_ObjID.m_wIndex;
			Send.DamList[i].idDster.dwSerial = pAtt->m_DamList[i].m_pChar->m_dwObjSerial;
			Send.DamList[i].wDamage = pAtt->m_DamList[i].m_nDamage;
			int nLeftHP =  pAtt->m_DamList[i].m_pChar->GetHP() - pAtt->m_DamList[i].m_nDamage;
			if(nLeftHP < 0) nLeftHP = 0;
			Send.DamList[i].wLeftHP = nLeftHP;
		}	

		BYTE byType[msg_header_num] = {fight_msg, attack_force_success_other_zocl};
		CircleReport(byType, (char*)&Send, Send.size());
	}
	else
	{
		_attack_force_failure_all_zocl Send;

		Send.sFailureCode = pAtt->m_nFailureCode;
		Send.idAtter.byID = m_ObjID.m_byID;
		Send.idAtter.wIndex = m_ObjID.m_wIndex;
		Send.idAtter.dwSerial = m_dwObjSerial;

		Send.idDster.wIndex = pAtt->m_pp->pDst->m_ObjID.m_wIndex;
		Send.idDster.byID = pAtt->m_pp->pDst->m_ObjID.m_byID;
		Send.idDster.dwSerial = pAtt->m_pp->pDst->m_dwObjSerial;

		BYTE byType[msg_header_num] = {fight_msg, attack_force_failure_all_zocl};
		CircleReport(byType, (char*)&Send, sizeof(Send));
	}*/
}

void CMonster::GetPlayerPos()
{	
	float d = GetAttackRange()- 0.2f*GetAttackRange();
	float rad = GetAngle(m_fCurPos,m_pPlayerTarget->m_fCurPos);
	

	static D3DXVECTOR2 ct,ta;
	float TarPos[3];	
	if (!IsValidPlayer()) return;

	//2003/3/21
	double len = ::GetSqrt(m_pPlayerTarget->m_fCurPos, m_fCurPos);
	if (len<=d&&d>=10) return;
	//////
	//
	

	Vector3f fNew;
	
	float tlen = ::GetSqrt(m_pPlayerTarget->m_fCurPos,m_pPlayerTarget->m_fTarPos);

	if (tlen <1 )//멈춘상태 
	if(!m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)m_pPlayerTarget->m_fCurPos, &fNew))
	{
		double len = 100;
		if (m_bNode)
		{
			len = ::GetSqrt(m_pPlayerTarget->m_fCurPos, m_PlayerPos[m_EndNode-1]);
		}
		if (m_bNode&&(len<=0.0))
			//SetTarPos(m_PlayerPos[m_CurNode]);
			memcpy(m_MoveTarget,m_PlayerPos[m_CurNode],sizeof(float)*3);
		else
		{
			DWORD cnt = 0;
			m_bNode = m_pCurMap->m_Level.GetPathFromDepth((Vector3f)m_fCurPos,(Vector3f)m_pPlayerTarget->m_fCurPos,15,(Vector3f*)m_PlayerPos,&cnt);	//15Node,패스 파인더.
			m_CurNode = 0;
			m_EndNode = cnt;
			if ( m_bNode )
			{
				//SetTarPos(m_PlayerPos[0]);
				memcpy(m_MoveTarget,m_PlayerPos[0],sizeof(float)*3);
				
				if(cnt > 10)
				{
					::__trace("MAP: %s, START: %f, %f, %f, ... TARGET: %f, %f, %f ... CNT: %d", 
						m_pCurMap->m_pMapSet->m_strCode, m_fCurPos[0], m_fCurPos[1], m_fCurPos[2], m_pPlayerTarget->m_fCurPos[0], m_pPlayerTarget->m_fCurPos[1], m_pPlayerTarget->m_fCurPos[2], cnt);
				}
			}
			else
			{
				SendAIMessage(m_MonAI.Act_State,AI_EV_CON_BAD,RUNAWAY_TIME);			
				/*
				memcpy(TarPos, m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
				m_AroundX = cos(rad)*d;
				m_AroundY = sin(rad)*d;	
				TarPos[0] += m_AroundX;
				TarPos[2] += m_AroundY;
				//SetTarPos(TarPos);
				memcpy(m_MoveTarget,TarPos,sizeof(float)*3);
				*/
			}
		}
		return;
	}
	else
	{
		m_CurNode	= 0;
		m_EndNode	= 0;
		if (m_bNode)
			m_bNode		= false;
	}
	
	
	float maxslot= slot_max;
	int n = int((rad / (2 * g_pi))*maxslot + 0.5f)-1;
	
	m_pPlayerTarget->RemoveSlot(this);
	int r = m_pPlayerTarget->GetNearEmptySlot(n,d,m_fCurPos);
	

	if (r>=0)
	{
		int res =m_pPlayerTarget->InsertSlot(this,r);
		if (res)
		{
			//
			//플레이어 미래 좌표!!
			//float tlen = ::GetSqrt(m_pPlayerTarget->m_fCurPos,m_pPlayerTarget->m_fTarPos);
			if ( tlen > 30)//뛰어도망 간다 
			{
				memcpy( TarPos, m_pPlayerTarget->m_fCurPos ,sizeof(float)*3 );
				float x = TarPos[0] - m_pPlayerTarget->m_fCurPos[0];
				float y = TarPos[2] - m_pPlayerTarget->m_fCurPos[2];

				float d = 70.0/(abs(x)+abs(y));
				float x1= (x)*d;
				float y1= (y)*d;
			
				TarPos[0]+=x1;
				TarPos[2]+=y1;
			
				/*
				memcpy( TarPos, m_pPlayerTarget->m_fCurPos ,sizeof(float)*3 );
				float ptangle = GetAngle(m_pPlayerTarget->m_fCurPos,m_pPlayerTarget->m_fTarPos);
				TarPos[0]+=cos(ptangle)*70;
				TarPos[2]+=sin(ptangle)*70;
				*/
			}
			else
			{			
				float rad2 = (2*g_pi)*(float(r+1))/maxslot;
				m_AroundX = cos(rad2)*d;
				m_AroundY = sin(rad2)*d;
				if (tlen <1 )//멈춘상태 
					memcpy(TarPos, m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
				else// 걷는 상태 
					memcpy(TarPos, m_pPlayerTarget->m_fTarPos, sizeof(float)*3);
			
				TarPos[0] += m_AroundX;
				TarPos[2] += m_AroundY;
			}
			
			if(!m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)TarPos, &fNew))
			{

				//슬롯으로 이동 할 수 없다면, 플레이어와 최단거리로 이동한다.
				memcpy(TarPos, m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
				
				m_AroundX = cos(rad)*d;
				m_AroundY = sin(rad)*d;	
				TarPos[0] += m_AroundX;
				TarPos[2] += m_AroundY;
			}
			//SetTarPos(TarPos);
			memcpy(m_MoveTarget,TarPos,sizeof(float)*3);
			//
		}
		else
			int error = 0;
	}
	else
	{
		float rad3 = 1.0 - (float(::rand()%30)/15.f);//-1 ~ 1 rad 사이를 배회한다.
		m_AroundX = cos(rad3+rad)*d*2.0;
		m_AroundY = sin(rad3+rad)*d*2.0;
		memcpy(TarPos, m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
		TarPos[0] += m_AroundX;
		TarPos[2] += m_AroundY;
		//SetTarPos(TarPos);
		memcpy(m_MoveTarget,TarPos,sizeof(float)*3);
	}
}
void CMonster::GetTarPosChase()
{
	
	
   	int d = GetAttackRange()-1.0;
	//int d = 20.0f;

	while (TRUE)
	{
		if (!(m_AroundX ||m_AroundY ))
		{
			/*
			static D3DXVECTOR2 ct,ta;
			ct.x = m_fCurPos[0] - m_pPlayerTarget->m_fCurPos[0];
			ct.y = m_fCurPos[2] - m_pPlayerTarget->m_fCurPos[2];
			ta.x = 1;
			ta.y = 0;

			D3DXVec2Normalize(&ct,&ct);
			D3DXVec2Normalize(&ta,&ta);
			float res = D3DXVec2Dot(&ct,&ta);
			float rad = acos(res);
			rad = -(pi/2.0)+ rad + (pi * (float(rand()%50))/50.0);
			*/
			float rad = (2*g_pi)*(float(rand()%50))/50.0;
			m_AroundX = cos(rad)*d;
			m_AroundY = sin(rad)*d;
			
		
		}
		
		int b = IsFront(m_fCurPos,m_pPlayerTarget->m_fCurPos,m_AroundX,m_AroundY);
		if (!b)
			m_AroundX = m_AroundY = 0;
		else
			break;
		
	}
	/*
	Vector3f fNew;
	if(!m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)m_pPlayerTarget->m_fCurPos, &fNew))
		memcpy(m_fTarPos, m_PlayerPos, sizeof(float)*3);
	*/
	/*
	Vector3f fNew;
	if(m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)m_pPlayerTarget->m_fCurPos, &fNew))
		memcpy(m_fTarPos, m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
	else
		return;
	*/
	memcpy(m_fTarPos, m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
	m_fTarPos[0] += m_AroundX;
	m_fTarPos[2] += m_AroundY;

	/*
	//float fAttackRange = CalcAttackRange(m_pPlayerTarget, 0)*2/3;	//사정거리보다 조금짧게..
	//float fAttackRange = m_AttackDist*(2.0f/3.0f);
	float fAttackRange = d;
	double dDist = ::GetSqrt(m_fCurPos, m_pPlayerTarget->m_fCurPos);

	if(dDist <= fAttackRange)
	{
		//CCharacter *pMon = SearchNearMonster();
		double dDist2 = 100;//충분히 몬스터와의 거리가 멀다는 가정 
		if (m_pMonTarget)
			dDist2 = ::GetSqrt(m_fCurPos, m_pMonTarget->m_fCurPos);
		
		// 주위의 몬스터와 거리가 가까울 경우 위치를 재설정한다.
		if (dDist2<5.0)
		{
			{
				float rad = (2*pi)*(float(rand()%100))/100.0;
				m_AroundX = cos(rad)*d;
				m_AroundY = sin(rad)*d;
				memcpy(m_fTarPos, m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
				m_fTarPos[0] += m_AroundX;
				m_fTarPos[2] += m_AroundY;
			}
		}
		/*
		else
		{
			//
			m_AroundX = m_AroundY = 0;
			CCharacter::Stop();
			//SendMsg_Chase_Stop();
		}
		return;
	
	}*/
	/*
	// 시야범위 밖일경우 따라가는것을 포기한다. 
	if(dDist >= m_ChaseDist)
	{
		m_AroundX = m_AroundY = 0;
		CCharacter::Stop();
		//SetChaseChar(NULL);
		//SendMsg_Chase_Change(NULL);
		return;
	}
	*/
}
float  CMonster::GetAttackProb()
{
	if (IsValidPlayer())
	{
		float res = ((_monster_fld*)m_pRecordSet)->m_fAttSklUnit - 
			float( m_pPlayerTarget->GetLevel() * 10.f+m_pPlayerTarget->GetDefSkill())/4.0 + 70.f;
		
		if(res < 5) res = 5;
		else if(res > 95) res = 95;

		return res;
	}
	return 0;
}


int CMonster::GetAttackPart()
{
	const int headprob	= 20;
	const int upprob	= headprob	+	23;
	const int downprob	= upprob	+	22;
	const int handprob	= downprob	+	18;
	const int footprob	= handprob	+	17;

	int r = ::rand()%100;
	if (r<=headprob)	{
		return DEFHEAD;
	}
	else if (r<=upprob)	{
		return DEFUP;
	}
	else if (r<=downprob)	{
		return DEFDOWN;
	}
	else if (r<=handprob)	{
		return DEFHAND;
	}
	else if (r<=footprob)	{
		return DEFFOOT;
	}
	return -1;
}

void CMonster::Attack()
{
	if (!IsValidPlayer())return;
//	if (!IsAttackAble(m_pPlayerTarget))return;	//ogogo..

	m_CurrentSkill = m_SearchSkill;
	
	if (m_Skill[m_CurrentSkill].m_Type ==0)
	{
		float atp	=	GetAttackProb();
		int  ratp	=	::rand()%100;
		int	damage  = 0;
		
		if (ratp>=atp)//회피 
		{
			SendMsg_Attack_Gen(m_pPlayerTarget, 0, 0, false);// Demage 0			
		}
		else
		{		
			int		p = GetAttackPart();
			int		dmg = m_Skill[m_CurrentSkill].GetDmg();

			damage	= CCharacter::GetAttackDamPoint(dmg, p, m_Skill[m_CurrentSkill].m_Element, m_pPlayerTarget);

			SendMsg_Attack_Gen(m_pPlayerTarget, damage, p, m_Skill[m_CurrentSkill].m_IsCritical);
		}
		m_pPlayerTarget->SetDamage(damage, this, GetLevel(), false);
	}
	else if (m_Skill[m_CurrentSkill].m_Type==1)
	{
		CAttack at(this);
		m_Skill[m_CurrentSkill].m_param.pDst = m_pPlayerTarget;
		m_Skill[m_CurrentSkill].m_param.nPart = GetAttackPart();
		memcpy(m_Skill[m_CurrentSkill].m_param.fArea, m_fCurPos, sizeof(float)*3);
		at.AttackForce(&m_Skill[m_CurrentSkill].m_param);

		SendMsg_Attack_Force(&at);

		for(int i = 0; i < at.m_nDamagedObjNum ; i++)
			at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, GetLevel(), false);
	}
}

float	CMonster::GetAngle(float mon[3],float plr[3])
{
	//////
	D3DXVECTOR2 ct,ta;
	ct.x = mon[0] - plr[0];
	ct.y = mon[2] - plr[2];
	ta.x = 1;
	ta.y = 0;

	D3DXVec2Normalize(&ct,&ct);
	D3DXVec2Normalize(&ta,&ta);

	bool dd = true;
	if ( ct.y <=0 )dd = false;
	
	
	float res = D3DXVec2Dot(&ct,&ta);
	float rad = acos(res);
	if (!dd) rad = (2 * g_pi) - rad;
	return rad;

}
BOOL	CMonster::IsFront(float *cur,float *tar,float x,float y)
{
	
	static D3DXVECTOR2 ct,ta;
	ct.x = tar[0] - cur[0];
	ct.y = tar[2] - cur[2];
	ta.x = x;
	ta.y = y;

	D3DXVec2Normalize(&ct,&ct);
	D3DXVec2Normalize(&ta,&ta);
	float r = D3DXVec2Dot(&ct,&ta);
	//if (r<=0) return false;
	if (r>0) return false;
	return true;
}

void CMonster::GetTargetPosProcess()
{
	if (IsValidPlayer())
	{
		Vector3f fNew;
		//if (m_CurNode>0&&m_CurNode<=max_path)
		if (m_EndNode==0)
		{
			if(m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)m_pPlayerTarget->m_fCurPos, &fNew))
			{
				memcpy(m_PlayerPos[m_EndNode], m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
				m_bNode = true;
			}
			else
			{
				if (m_bNode)
				{
					m_EndNode++;
					memcpy(m_PlayerPos[m_EndNode], m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
					m_bNode = false;
				}
			}
		}
		else if (m_EndNode<max_path)
		{
			if(m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_PlayerPos[m_EndNode-1], (Vector3f)m_pPlayerTarget->m_fCurPos, &fNew))
			{
				memcpy(m_PlayerPos[m_EndNode], m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
				m_bNode = true;
			}
			else
			{
				if (m_bNode)
				{
					m_EndNode++;
					if ( m_EndNode >=max_path )m_EndNode = max_path-1;
					else 
					{
						memcpy(m_PlayerPos[m_EndNode], m_pPlayerTarget->m_fCurPos, sizeof(float)*3);
					}
					m_bNode = false;
				}
			}
		}
	}
}

void CMonster::LootingItem(CPlayer* pOwner)
{
	int nLv = ((_monster_fld*)m_pRecordSet)->m_fLevel;
	int nStartRecIndex = 10*(nLv-1);//level당 10개의 항목을 가짐
	int nEndRecIndex = nStartRecIndex+10;
	int nEtcCnt = 0;

	for(int i = nStartRecIndex; i < nEndRecIndex; i++)
	{
		_ItemLooting_fld* pRec = (_ItemLooting_fld*)g_Main.m_tblItemLoot.m_tblLoot.GetRecord(i);
		if(!pRec)
		{
			::MyMessageBox("CMonster::LootingItem()", "_item_loot_record* pRec(%d), lv(%d) == NULL", i, nLv);
			break;
		}

		DWORD dwR1 = ::rand();
		DWORD dwRand = (dwR1<<16)+::rand();

		if(dwRand >= (DWORD)pRec->m_nLootRate)
			continue;

		//몬스터 아이디 검사..
		bool bTrue = false;
		_str_code* pstrMon = &pRec->m_strLootMobIDX1;
		for(int m = 0; m < 3; m++)
		{
			if(!strcmp(pstrMon[m], m_pRecordSet->m_strCode))
			{
				bTrue = true;
				break;
			}
		}
		if(!bTrue)
			continue;

		int nRanCode;
		int nCnt = 0;
		bool bFind = false;

		_ItemLooting_fld* pLootRec = NULL;
		CItemLootTable::_linker_code* pLinker = NULL;
		while(1)	//10번에 걸쳐서 못찾으면 그냥 넘어간다.
		{
			nRanCode = ::rand()%pRec->m_nLootListCount;
			pLinker = &g_Main.m_tblItemLoot.m_ppLinkCode[pRec->m_dwIndex][nRanCode];
			if(pLinker->bExist)
			{
				bFind = true;
				break;
			}
			if(nCnt++ > 9)
				break;
		}
		if(!bFind)
			continue;

		DWORD dwDur = ::GetItemDurPoint(pLinker->byTableCode, pLinker->wItemIndex);
		DWORD dwLv = __DF_UPT;

		BYTE byItemKindCode = ::GetItemKindCode(pLinker->byTableCode);
		if(byItemKindCode == item_kind_std)
		{	//일반아이템
			BYTE byMaxLv = ::GetDefItemUpgSocketNum(pLinker->byTableCode, pLinker->wItemIndex);
			BYTE byLimSocketNum = 0;
			if(byMaxLv > 0)
				byLimSocketNum = 1+::rand()%byMaxLv;

			dwLv = ::GetBitAfterSetLimSocket(byLimSocketNum);
		}
		else if(byItemKindCode == item_kind_ani)
		{	//소환아이템
			dwLv = ::GetMaxParamFromExp(pLinker->wItemIndex, dwDur);
		}
		else
			continue;//유닛은 흘릿수없다..

		::CreateItemBox(pLinker->byTableCode, pLinker->wItemIndex, dwDur, dwLv, 
						   pOwner, this, itembox_create_code_mon,
						   m_pCurMap, m_wMapLayerIndex, m_fCurPos, false);
	}
}

DWORD CMonster::GetEmotionState()
{
	return m_MonAI.Emotion_State.m_State;
}

DWORD CMonster::GetConditionState()
{
	return m_MonAI.Con_State.m_State;
}
DWORD CMonster::GetActionState()
{
	return m_MonAI.Act_State.m_State;
}

void CMonster::CheckCondition()
{
	float chp = m_nHP;
	float mhp = (int)((_monster_fld*)m_pRecordSet)->m_fMaxHP;
	float hpp = (chp/mhp)*100.f;
	if (chp ==mhp )
	{
		//매우 좋음 
		SendAIMessage(m_MonAI.Con_State,AI_EV_CON_VERYGOOD,10000);
	}
	else if (hpp >((_monster_fld*)m_pRecordSet)->m_fGoodToOrdHPPer)
	{
		//좋음 
		SendAIMessage(m_MonAI.Con_State,AI_EV_CON_GOOD,10000);
	}
	else if (hpp >((_monster_fld*)m_pRecordSet)->m_fOrdToBadHPPer)
	{
		//보통 
		SendAIMessage(m_MonAI.Con_State,AI_EV_CON_NORMAL,10000);
	}
	else if (hpp >((_monster_fld*)m_pRecordSet)->m_fBadToWorseHPPer)
	{
		//나쁨 
		SendAIMessage(m_MonAI.Con_State,AI_EV_CON_BAD,10000);
		if ( m_pMonTarget )
		{
			// 타겟 몬스터가 있다는것은, 곧 도움 요청을 하는 몬스터다
			// 도움 요청을 하는 몬스터에 한해서 도망을 간다. 
			if ( m_pMonTarget->m_bLive ) 
			{
				int rand = ::rand()%100;
				if(rand < ((_monster_fld*)m_pRecordSet)->m_fEspTFProb)
					SendAIMessage(m_MonAI.Act_State,AI_EV_CON_BAD,RUNAWAY_TIME);
					//SendAIMessage(m_MonAI.Act_State,AI_EV_CON_BAD);
			}
		}
	}
	else 
	{
		//매우 나쁨 
		SendAIMessage(m_MonAI.Con_State,AI_EV_CON_VERYBAD,2000);// 도움 요청을 위해서 딜레이를 2000으로 수정
		if ( m_pMonTarget )
		{
			// 타겟 몬스터가 있다는것은, 곧 도움 요청을 하는 몬스터다
			// 도움 요청을 하는 몬스터에 한해서 도망을 간다. 
			if ( m_pMonTarget->m_bLive ) 
			{
				int rand = ::rand()%100;
				if(rand < ((_monster_fld*)m_pRecordSet)->m_fEspTFProb)
					SendAIMessage(m_MonAI.Act_State,AI_EV_CON_BAD,RUNAWAY_TIME);
					//SendAIMessage(m_MonAI.Act_State,AI_EV_CON_BAD);
			}
		}
	}
	
}

int	CMonster::AttackObject(int nDamage , CGameObject* pOri/* = NULL*/)
{
	m_LifeCicle		= timeGetTime();
	///*MAYUN
	InsertAttackedPlayer(pOri);
	//if (nDamage >= 0)
	{
		int percent = 100.0f;
		switch(GetEmotionState())
		{
		case	AI_EM_NORMAL:
			percent = EmotionType[(int)((_monster_fld*)m_pRecordSet)->m_fEmoType-1].m_Normal;
			break;
		case	AI_EM_DISCOMPORT:
			percent = EmotionType[(int)((_monster_fld*)m_pRecordSet)->m_fEmoType-1].m_Discomport;
			break;
		case	AI_EM_ANGER:
			percent = EmotionType[(int)((_monster_fld*)m_pRecordSet)->m_fEmoType-1].m_Anger;
			break;
		case	AI_EM_FURY:
			percent = EmotionType[(int)((_monster_fld*)m_pRecordSet)->m_fEmoType-1].m_Fury;
			break;
		case	AI_EM_MAD:
			percent = EmotionType[(int)((_monster_fld*)m_pRecordSet)->m_fEmoType-1].m_Mad;
			break;

		default:
			::MyMessageBox("mon", "%d, %d", GetEmotionState(), percent);
			break;

			
		}
		int rand = ::rand()%100;
		if (rand<=percent)				
		{
			if(m_pRecordSet)
				SendAIMessage(m_MonAI.Emotion_State,AI_EV_EM_BAD,((_monster_fld*)m_pRecordSet)->m_fEmoImpStdTime);
			//else
			//	SendAIMessage(m_MonAI.Emotion_State,AI_EV_EM_BAD,30000);
		}
		//else
		//	::MyMessageBox("mon", "(%d > %d)", rand, percent);

		return true;
	}
	return false;
}

CMonster* CreateRespawnMonster(CMapData* pMap, WORD wLayer, int nMonsterIndex, _mon_active* pActiveRec, _dummy_position* pDumPosition, CBattleDungeon* pDungeon)
{
	CMonster* pMon = NULL;
	DWORD dwTime = timeGetTime();
	DWORD dwNode;
	while(CMonster::s_listEmpty.CopyFront(&dwNode))
	{
		if(g_Monster[dwNode].m_bLive)
		{
			CMonster::s_listEmpty.PopNode_Front(&dwNode);
			g_Main.m_logSystemError.Write("FindEmptyMonster() live = true (%d:item)", dwNode);
			continue;
		}

		if(dwTime - g_Monster[dwNode].m_dwLastDestroyTime < 30000)
			break;

		pMon = &g_Monster[dwNode]; 
		break;
	}

	if(!pMon)
		return NULL;
	
	_monster_create_setdata Data;

	Data.m_pMap = pMap;
	Data.m_nLayerIndex = wLayer;	
	Data.m_pRecordSet = g_Main.m_tblMonster.GetRecord(nMonsterIndex);
	if(!Data.m_pRecordSet)
		return NULL;

	if(!pMap->GetRandPosInDummy(pDumPosition, Data.m_fStartPos, false))
		return NULL;

	Data.pActiveRec = pActiveRec;
	Data.pDumPosition = pDumPosition;
	Data.pBattleDungeon = pDungeon;

	if(!pMon->Create(&Data))
		return NULL;

	CMonster::s_listEmpty.PopNode_Front(&dwNode);

	return pMon;
}

bool CreateRepMonster(CMapData* pMap, WORD wLayer, float* fPos, char* pszMonsterCode)//, _mon_active* pActiveRec, _dummy_position* pDumPosition)
{
	CMonster* pMon = NULL;
	DWORD dwTime = timeGetTime();
	DWORD dwNode;
	while(CMonster::s_listEmpty.CopyFront(&dwNode))
	{
		if(g_Monster[dwNode].m_bLive)
		{
			CMonster::s_listEmpty.PopNode_Front(&dwNode);
			g_Main.m_logSystemError.Write("FindEmptyMonster() live = true (%d:item)", dwNode);
			continue;
		}

		if(dwTime - g_Monster[dwNode].m_dwLastDestroyTime < 30*1000)
			break;

		pMon = &g_Monster[dwNode]; 
		break;
	}

	if(!pMon)
		return false;

	_monster_create_setdata Data;

	Data.m_pMap = pMap;
	Data.m_nLayerIndex = wLayer;	
	Data.m_pRecordSet = g_Main.m_tblMonster.GetRecord(pszMonsterCode);
	if(!Data.m_pRecordSet)
		return false;

	memcpy(Data.m_fStartPos, fPos, sizeof(float)*3);
	Data.pActiveRec = NULL;
	Data.pDumPosition = NULL;

	pMon->Create(&Data);

	CMonster::s_listEmpty.PopNode_Front(&dwNode);

	//던젼일경우 던젼몹리스트에 추가시킨다.
	if(pMap->m_pMapSet->m_nMapType == MAP_TYPE_DUNGEON)
	{
		CBattleDungeon* pDungeon = ::FindDungeonFromLayer(g_BattleDungeon, MAX_BATTLEDUNGEON, pMap, wLayer);
		if(!pDungeon)
		{
			pMon->Destroy(mon_destroy_code_respawn, NULL);
			return false;
		}
		if(!pDungeon->m_listActiveMonster.PushNode_Back(pMon->m_ObjID.m_wIndex))
		{
			pMon->Destroy(mon_destroy_code_respawn, NULL);
			return false;
		}
	}	

	return true;
}

