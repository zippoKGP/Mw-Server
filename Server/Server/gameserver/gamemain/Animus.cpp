#include "stdafx.h"
#include "Animus.h"
#include "MainThread.h"

int	CAnimus::s_nLiveNum = 0;
DWORD CAnimus::s_dwSerialCnt = 0;

CRecordData	CAnimus::s_tblParameter[animus_storage_num];


float* CAIPathFinder::GetPath( Vector3f &cur, Vector3f &tar )
{
	Vector3f fNew;
			
	if(!m_pLevel->mBsp->CanYouGoThere((Vector3f)cur, (Vector3f)tar, &fNew))
	{
		double len = 100;
		if (m_bNode)
		{
			len = ::GetSqrt( cur, m_MovePos[m_CurNode]);
		}
		
		if (m_bNode&&(len<=0.0))//노드 위치까지 도착했다.
		{
			m_pTarget	= m_MovePos[m_CurNode]; // 과거의 위치를 저장하고
			m_CurNode++;//노드를 증가시킨다.
	
			if (m_CurNode>= m_EndNode )//현재의 위치가 마지막 노드라면
			{
				// 모든 값을 초기화 시키고
				m_CurNode	= m_EndNode = 0;
				m_bNode	= false;
			}
			else 
			{
				// 그렇지 않다면 증가시킨 노드의 포지션을 대입시킨다. 
				m_pTarget	= m_MovePos[m_CurNode];
			}
			return	m_pTarget;
		}
		else
		{
			DWORD cnt = 0;
			m_bNode = m_pLevel->GetPathFromDepth( cur,tar,15,(Vector3f*)m_MovePos,&cnt);	//15Node,패스 파인더.
			m_CurNode = 0;
			m_EndNode = cnt;
			if ( m_bNode )
			{
				m_pTarget = m_MovePos[0];
				return m_pTarget;
			}
			else
			{
				return NULL;
			}
		}
		return NULL;
	}
	else
	{
		
		m_CurNode	= 0;
		m_EndNode	= 0;
		if (m_bNode)
			m_bNode	= false;
		return &tar[0];
		
	}
}

CAnimus::CAnimus()
{
	m_pMaster = NULL;
}

CAnimus::~CAnimus()
{
}
///
void CAnimus::AIInit()
{
	m_pRecord = ::GetAnimusFldFromExp( m_byClassCode, m_dwExp );
	m_nMaxHP	= m_pRecord->m_nMaxHP;
	m_nMaxFP	= m_pRecord->m_nMaxFP;

	m_dwAIMode	= ani_mode_follow;
	m_pTarget	= NULL;
	m_Mightiness= 1.0f;

	m_AITimer[AIT_MOVE].Init( 700 );
	m_AITimer[AIT_ACTION].Init ( 100 );
	m_AITimer[AIT_LIFE].Init( 60000 );
	m_AITimer[AIT_LIFE].Set();
	//
	m_Path.Init( &m_pCurMap->m_Level );
	
	m_Skill[0].Init
	(
		0,
		m_pRecord->m_nAttFcStd* m_Mightiness,
		m_pRecord->m_nMinAFSelProb,
		m_pRecord->m_nMaxAFSelProb,
		m_pRecord->m_nAttExt,
		m_pRecord->m_nAttMoTime1,
		m_pRecord->m_nAttSpd
	);
}

float GetAngle(float mon[3],float plr[3])
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
	if (!dd) rad = - rad;
	return rad;
}
/*
float	GetAngle(float Pos[3],float Tar[3])
{
	double Sqrt = sqrt( pow((Tar[0]-Pos[0]), 2) + pow((Tar[2]-Pos[2]), 2) );
	double Acos = acos((Tar[2]-Pos[2])/ Sqrt);

	return rad;

}
*/
void CAnimus::ChangeMode( DWORD mode )
{
	if ( m_dwAIMode!= mode )
	{
		m_dwAIMode	= mode;
		AlterMode_MasterReport( m_dwAIMode );
	}
}


void CAnimus::GetTarget()
{
	CCharacter *enemy  = NULL;
	if ( !m_pMaster ) return;
	double len = ::GetSqrt( m_pMaster->m_fCurPos, m_fCurPos);


	if ( len >= 400 ) //너무 멀다 소환해재
	{
		m_pTarget = NULL;
		Return_MasterRequest();
		return;
	}

	if ( len >= m_pRecord->m_nViewExt ) //주인과 너무 많이 떨어져있다.
	{
		m_pTarget	= NULL; 
		ChangeMode ( ani_mode_follow );	 
		return;
	}

	if ( m_pTarget ) // 타겟이 이미 있다!
	{
		if ( !m_pTarget->m_bLive )// 타겟이 죽었다 -_-;;
		{
			m_pTarget = NULL;
			return;
		}
		else // 살았는데 거리가 멀다 ;;;
		{
			double tlen = ::GetSqrt( m_pTarget->m_fCurPos, m_fCurPos);
			if ( tlen >= ( m_pRecord->m_nViewExt / 2 ) )
			{
				m_pTarget	= NULL; //거리가 멀면 타겟이 풀린다 
				ChangeMode ( ani_mode_follow );	 // 유저에게 다가간다.
				return;
			}
		}
	}

	if ( !m_pTarget ) // 쓸만한 타겟이 없다
	{
		// 1. 첫번째로 주인을 공격한 몹을 찾는다.
		// enemy  = SearchNearPlayerAttack();
		// 2. 두번째로 자신을 공격한 몹을 찾는다.
		if ( !enemy )
		{
			enemy = SearchNearEnemy();
		}
		m_pTarget = enemy;
	}
	
	
}
float	CAnimus::GetAttackProb()
{
	float res = m_pRecord->m_nAttSklUnit - 
		float( m_pTarget->GetLevel() * 10.f + m_pTarget->GetDefSkill())/4.0 + 70.f;
	
	if(res < 5) res = 5;
	else if(res > 95) res = 95;

	return res;
}
int		CAnimus::GetAttackPart()
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

void CAnimus::Attack( DWORD skill)
{
	if ( !m_pTarget ) return;
	
	if (m_Skill[skill].m_Type ==0)
	{
		float atp	=	GetAttackProb();
		int  ratp	=	::rand()%100;
		
		if (ratp>=atp)
		{
			SendMsg_Attack_Gen(m_pTarget, 0, 0, false);// Demage 0
			m_pTarget->SetDamage( 0 , this, GetLevel(), false);
			return;//회피 
		}
		
		
		int		p = GetAttackPart();
		int		dmg = m_Skill[skill].GetDmg();

		int		ret = CCharacter::GetAttackDamPoint(dmg, p, m_Skill[skill].m_Element, m_pTarget);

		SendMsg_Attack_Gen(m_pTarget, ret, p, m_Skill[skill].m_IsCritical);

		m_pTarget->SetDamage(ret, this, GetLevel(), false);

		if ( ( GetLevel() - m_pTarget->GetLevel() )  <= 5 )
		{
			int delta =  ((_monster_fld*)(m_pTarget->m_pRecordSet))->m_fExt/ 500 + m_pTarget->GetLevel();
			
			if ( m_pRecord->m_nForLvUpExp < (m_dwExp + delta ))
			{
				delta = m_pRecord->m_nForLvUpExp - m_dwExp;
			}
			m_dwExp += delta;
			AlterExp_MasterReport( delta );

			//ogogo.. 레벨업 이벤트 처리..
			_animus_fld* pRecordBuffer = ::GetAnimusFldFromExp( m_byClassCode, m_dwExp );	
			if(pRecordBuffer != m_pRecord)
			{//레벨업..
				m_pRecord = pRecordBuffer;
				SendMsg_LevelUp();
			}
		}
	}
	else if (m_Skill[skill].m_Type==1)
	{
		CAttack at(this);
		m_Skill[skill].m_param.pDst = m_pTarget;
		m_Skill[skill].m_param.nPart = GetAttackPart();
		memcpy(m_Skill[skill].m_param.fArea, m_fCurPos, sizeof(float)*3);
		at.AttackForce(&m_Skill[skill].m_param);

		//SendMsg_Attack_Force(&at);

		for(int i = 0; i < at.m_nDamagedObjNum ; i++)
			at.m_DamList[i].m_pChar->SetDamage(at.m_DamList[i].m_nDamage, this, GetLevel(), false);
	}
}
void CAnimus::Action()
{
	if ( !m_pTarget ) return;

	DWORD	time	= timeGetTime();
	double dDist = ::GetSqrt( m_fCurPos, m_pTarget->m_fCurPos);
	int		m_DetectedSkill = -1;
	
	for (int i =0;i < MAX_ANI_SKILL ;i++)
	{
		if ( m_Skill[i].m_bLoad )
		{
			if ( m_Skill[i].m_Active == false)
			{
				DWORD del	= time - m_Skill[i].m_BefTime;
				if (del  > m_Skill[i].m_Delay)
				{
					m_Skill[i].m_Active = true;//딜레이가 초과된 기술들을 사용가능 상태로 돌린다.
				}
			}
			if ( m_Skill[i].m_Active)
			{
				if (dDist<=m_Skill[i].m_Len)
				{
					m_DetectedSkill			= i;

					m_Skill[i].m_Active		= false;
					m_Skill[i].m_BefTime	= time;

					////
					m_AITimer[AIT_ACTION].Set( m_Skill[i].m_CastDelay );
					
					SetTarPos( m_fCurPos, false );
					////
					switch ( 0 ) //소환수 타입 마다 다르다.
					{
						case 0:
							Attack( i );
							break;
						case 1:
							//Heal( i );
							break;
					}
					
					return ;
				}
			}
		}
	}
	return ;
}
CCharacter* CAnimus::SearchNearPlayerAttack()
{
	float rad = GetAngle(m_fCurPos,m_pMaster->m_fCurPos);
	int pos = int((rad / (2 * g_pi))*slot_max + 0.5f)-1;

	int n = pos;
	int m = pos;
	
	for (int i=0;i<slot_max;i++)
	{		
		if (n>=slot_max) n = 0;
		if (m<0) m = slot_max-1;
		
		if ( m_pMaster->m_AroundSlot[n] ) 
		{	
			return m_pMaster->m_AroundSlot[n];
		}
		if ( m_pMaster->m_AroundSlot[m] ) 
		{
			return m_pMaster->m_AroundSlot[m];
		}
		n++;
		m--;

	}
	return NULL;
}

CCharacter* CAnimus::SearchNearEnemy()
{
	for (int i=0;i<slot_max;i++)
	{		
		if ( m_AroundSlot[i] ) 
		{	
			return m_AroundSlot[i];
		}
	}
	return NULL;
}

void GetDirection(Vector3f &cur,Vector3f &tar, Vector3f &out,float deg )
{
	float rad = GetAngle( tar, cur );
	float x1 = cos(rad ) * deg;
	float y1 = sin(rad ) * deg;	
	
	out[0]	=	cur[0] + x1;
	out[2]	=	cur[2] + y1;
	out[1]	=	cur[1];
	/*
	float x = TarPos[0] - cur[0];
	float y = TarPos[2] - cur[2];
	float d = deg /(abs(x)+abs(y));
	float x1= (x)*d;
	float y1= (y)*d;

	out[0]	=	x1+cur[0];
	out[2]	=	y1+cur[2];
	out[1]	=	cur[1];
	*/
}
void CAnimus::GetMoveTarget( CCharacter *target )
{
	if ( !target ) return;
	float TarPos[3];	
	float BufPos[3];	

	float attr = (GetAttackRange()*0.8);
	float rad = GetAngle(m_fCurPos,target->m_fCurPos);
	double tdist	= ::GetSqrt( target->m_fCurPos, m_fCurPos);
	
	
	float x1 = 0;
	float y1 = 0;

	float cdist = 40;
	if ( tdist < cdist )
	{
		memcpy( BufPos, TarPos ,sizeof(float)*3 );
		memcpy( TarPos, target->m_fCurPos ,sizeof(float)*3 );

		x1 = cos(rad ) * attr;
		y1 = sin(rad ) * attr;	

		TarPos[0] += x1;
		TarPos[2] += y1;
	}
	else
	{
		memcpy( BufPos, target->m_fTarPos, sizeof(float)*3 );
		
		double tdist2	= ::GetSqrt( target->m_fTarPos, m_fCurPos);
		if ( tdist2 > attr )  tdist2 -= attr;
		else
		{
			tdist2 = attr;
		}

		
		if ( tdist2 > cdist )
		{
			GetDirection( m_fCurPos, target->m_fTarPos, TarPos, cdist * 1.5 );
		}
		else
		{
			GetDirection( m_fCurPos, target->m_fTarPos, TarPos, tdist2 );
		}
		
	}

	/*	
	float x = TarPos[0] - m_fCurPos[0];
	float y = TarPos[2] - m_fCurPos[2];

	float d = attr / (abs(x)+abs(y));
	float x1= (x)*d;
	float y1= (y)*d;
	*/
	
	//memcpy( BufPos, TarPos ,sizeof(float)*3 );

	/*
	float x1 = cos(rad ) * attr;
	float y1 = sin(rad ) * attr;	

	TarPos[0] += x1;
	TarPos[2] += y1;
	*/
	//memcpy( BufPos, TarPos ,sizeof(float)*3 );

	//TarPos[0]-=x1;
	//TarPos[2]-=y1;
	
	// 소환자와의 거리가 너무 가깝다.

	double mdist = ::GetSqrt( TarPos, m_pMaster->m_fCurPos );
	if ( mdist <= attr* 0.5 )
	{
		memcpy( TarPos, BufPos ,sizeof(float)*3 );
		
		x1 = cos(rad+0.5) * attr;
		y1 = sin(rad+0.5) * attr;	
		
		TarPos[0] += x1;
		TarPos[2] += y1;
	}
	//*/

	Vector3f fNew;
	if( !m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)m_fCurPos, (Vector3f)TarPos, &fNew))
	{
		float *pos = m_Path.GetPath( m_fCurPos, TarPos );
		if ( pos )
			memcpy( TarPos, pos, sizeof (float ) * 3);		

	}
	
	SetTarPos( TarPos );		
}
void CAnimus::SendMove()
{
	if ( m_bMove  )
	{
		//
		double dist = ::GetSqrt( m_BefTarget, m_fTarPos);
		//
		if (m_AITimer[AIT_MOVE].Check() || dist > 15 )
		{
			double tdist = ::GetSqrt( m_fCurPos, m_fTarPos);//과거의 타겟과 현재의 타겟의 차이가 적을경우 무시한다
			if ( tdist>=5.0f )
			{
				memcpy( m_BefTarget, m_fTarPos, sizeof (float ) * 3 );
				SendMsg_Move();
			}
		}
	}
}
void CAnimus::LifeTimeCheck()
{
	double tdist = ::GetSqrt( m_fCurPos, m_fTarPos);
	BOOL move = true;
	if ( tdist == 0.0 ) move = false;
	if ( !m_pTarget && !move )
	{
		if ( m_AITimer[AIT_LIFE].Check() )
		{
			Return_MasterRequest();
			return;
		}
	}
	else
	{
		m_AITimer[AIT_LIFE].Set();
	}
}
void CAnimus::Process()
{

	switch ( m_dwAIMode )
	{
		case ani_mode_action:
			{
				GetTarget();
				if ( m_AITimer[AIT_ACTION].Check() )
				{
					if ( m_pTarget )
					{
						GetMoveTarget( m_pTarget );
						Action();
					}
					else
					{
						GetMoveTarget ( m_pMaster );
						
					}
				}
				break;
			}
		case ani_mode_follow:
			{
				m_pTarget	= NULL;
				GetMoveTarget ( m_pMaster );
				double mdist = ::GetSqrt( m_fCurPos, m_pMaster->m_fCurPos );
				if ( mdist < GetAttackRange() )
				{
					ChangeMode ( ani_mode_action );
				}
				break;
			}
	}
	LifeTimeCheck();
	SendMove();
	CCharacter::Move( m_pRecord->m_nMovSpd );
}

///
bool CAnimus::Init(_object_id* pID)
{
	CCharacter::Init(pID);
	
	m_dwLastDestroyTime = 0;

	return true;
}

bool CAnimus::Create(_animus_create_setdata* pData)
{
	if(CCharacter::Create((_character_create_setdata*)pData))
	{
		m_byClassCode = (BYTE)m_pRecordSet->m_dwIndex;
		m_nHP = pData->nHP;
		m_nFP = pData->nFP;
		m_dwExp = pData->dwExp;
		m_pMaster = pData->pMaster;
		m_dwObjSerial = GetNewMonSerial();

		AIInit();

		SendMsg_Create();
		s_nLiveNum++;

		return true;
	}

	return false;
}

bool CAnimus::Destroy()
{
	m_dwLastDestroyTime = timeGetTime();

	SendMsg_Destroy();

	CCharacter::Destroy();

	m_pMaster = NULL;
	m_dwObjSerial = 0xFFFFFFFF;
	
	s_nLiveNum--;

	return true;
}

void CAnimus::Loop()
{
	Process();
}

void CAnimus::ChangeMode_MasterCommand(int nMode)
{
	ChangeMode( nMode );
}

void CAnimus::ChangeTarget_MasterCommand(CCharacter* pTarget)
{
	m_pTarget = pTarget;
}

void CAnimus::MasterAttack_MasterInform(CCharacter* pDst)
{
	if ( !m_pTarget )
		m_pTarget = pDst;
}

void CAnimus::MasterBeAttacked_MasterInform(CCharacter* pDst)
{
	if ( !m_pTarget )
		m_pTarget = pDst;
}

void CAnimus::AlterHP_MasterReport()
{
	if(m_pMaster)
		m_pMaster->AlterHP_Animus(m_nHP);
}

void CAnimus::AlterFP_MasterReport()
{
	if(m_pMaster)
		m_pMaster->AlterFP_Animus(m_nFP);
}

void CAnimus::AlterExp_MasterReport(int nAlterExp)
{
	if(m_pMaster)
		m_pMaster->AlterExp_Animus(nAlterExp);
}

void CAnimus::AlterMode_MasterReport(BYTE byMode)
{
	if(m_pMaster)
		m_pMaster->AlterMode_Animus(byMode);
}

void CAnimus::Return_MasterRequest()
{
	if(m_pMaster)
		m_pMaster->Return_AnimusAsk();
}

int CAnimus::GetHP()
{
	return m_nHP;
}

int	CAnimus::GetMaxHP( void )
{
	return m_nMaxHP;
}

int CAnimus::SetDamage(int nDam, CCharacter* pDst, int nDstLv, bool bCrt)
{
	if ( !m_pTarget )
		m_pTarget = pDst;
	
	//AttackObject(nDamage , pDst);
	if(nDam >= 1)
	{
		m_nHP -= nDam;
		if(m_nHP <= 0)
			m_nHP = 0;

		AlterHP_MasterReport();
	}
	if(m_nHP == 0)
	{
		int ran = ::rand()%2;
		if (0)
		{
			if ( m_dwExp >= m_pRecord->m_fPenalty )
			{
				m_dwExp -= m_pRecord->m_fPenalty;
				AlterExp_MasterReport( -m_pRecord->m_fPenalty );
				m_pRecord = ::GetAnimusFldFromExp( m_byClassCode, m_dwExp );			
			}
		}

		Return_MasterRequest();
	}

	return m_nHP;
}

float CAnimus::GetWidth()
{
	return m_pRecord->m_nWidth;
}

float CAnimus::GetAttackRange()
{
	return m_pRecord->m_nAttExt;
}

int CAnimus::GetDefFC(int nAttactPart, CCharacter* pAttChar)
{
	return  m_pRecord->m_nStdDefFc;
}

int CAnimus::GetLevel()
{
	return m_pRecord->m_nLevel;
}

int CAnimus::GetDefSkill()
{
	return  m_pRecord->m_nDefSklUnit;
}

int CAnimus::GetFireTol()
{
	return m_pRecord->m_nFireTol;
}

int	CAnimus::GetWaterTol()
{
	return m_pRecord->m_nWaterTol;
}

int	CAnimus::GetSoilTol()
{
	return m_pRecord->m_nSoilTol;
}

int	CAnimus::GetWindTol()
{
	return m_pRecord->m_nWindTol;
}

float CAnimus::GetDefGap()
{
	return m_pRecord->m_fDefGap;
}

float CAnimus::GetDefFacing()
{
	return m_pRecord->m_fDefFacing;
}

float CAnimus::GetWeaponAdjust()
{
	return m_pRecord->m_fDefGap;
}

int	CAnimus::AttackableHeight()
{
	return attack_able_height;
}

int CAnimus::GetGenAttackProb(CCharacter* pDst, int nPart)
{
	return (int)GetAttackProb();
}

int CAnimus::GetObjRace()
{	
	if(!m_pMaster)
		return -1;
	return m_pMaster->GetObjRace();		
}

void CAnimus::SendMsg_Create()
{
	_animus_create_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zPos, 3);
	if(m_pRecord)
		Send.byLv = m_pRecord->m_nLevel;
	else
		Send.byLv = 1;

	BYTE byType[msg_header_num] = {init_msg, animus_create_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CAnimus::SendMsg_Destroy()
{
	_animus_destroy_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	if(m_nHP > 0)	//귀환
		Send.byDestroyCode = animus_des_type_back;
	else			//꽥..
		Send.byDestroyCode = animus_des_type_die;
	
	BYTE byType[msg_header_num] = {init_msg, animus_destroy_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CAnimus::SendMsg_Move()
{
	_animus_move_zocl Send;

	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);
	Send.zTar[0] = (short)m_fTarPos[0];
	Send.zTar[1] = (short)m_fTarPos[2];
	Send.wHPRate = (WORD)m_nHP;
	Send.wLastEffectCode = m_wLastContEffect;

	BYTE byType[msg_header_num] = {position_msg, animus_move_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CAnimus::SendMsg_FixPosition(int n)
{
	_animus_fixpositon_zocl Send;

	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);	
	Send.wHPRate = (WORD)m_nHP;
	if(m_pRecord)
		Send.byLv = m_pRecord->m_nLevel;
	else
		Send.byLv = 1;
	Send.wLastEffectCode = m_wLastContEffect;

	BYTE byType[msg_header_num] = {position_msg, animus_fixpositon_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));	
}

void CAnimus::SendMsg_RealMovePoint(int n)	//특정인한테..
{
	_animus_real_move_zocl Send;

	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);
	Send.zTar[0] = m_fTarPos[0];
	Send.zTar[1] = m_fTarPos[2];
	Send.wHPRate = (WORD)m_nHP;
	if(m_pRecord)
		Send.byLv = m_pRecord->m_nLevel;
	else
		Send.byLv = 1;

	Send.wLastEffectCode = m_wLastContEffect;

	BYTE byType[msg_header_num] = {position_msg, animus_real_move_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));
}

void CAnimus::SendMsg_Attack_Gen(CCharacter* pTarget, int nDamage, int nAttPart, BOOL bCritical)
{
	_attack_animus_inform_zocl Send;

	Send.idAtter.byID = m_ObjID.m_byID;
	Send.idAtter.wIndex = m_ObjID.m_wIndex;
	Send.idAtter.dwSerial = m_dwObjSerial;
	Send.byAttackPart = nAttPart;
	Send.bCritical = (bCritical ? true : false);		

	Send.idDst.byID = pTarget->m_ObjID.m_byID;
	Send.idDst.wIndex = pTarget->m_ObjID.m_wIndex;
	Send.idDst.dwSerial = pTarget->m_dwObjSerial;
	Send.wDamage = nDamage;
	Send.wLeftHP = max(int(pTarget->GetHP() - nDamage), 0);

	BYTE byType[msg_header_num] = {fight_msg, attack_monster_inform_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CAnimus::SendMsg_LevelUp()
{
	_animus_lvup_inform_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	if(m_pRecord)
		Send.byLv = m_pRecord->m_nLevel;
	else
		Send.byLv = 1;

	BYTE byType[msg_header_num] = {recall_msg, animus_lvup_inform_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

bool CAnimus::SetStaticMember()
{
	char* pszFileName[animus_storage_num] = {	".\\script\\PaimonCharacter.dat", 
												".\\script\\HecateCharacter.dat", 
												".\\script\\InannaCharacter.dat", 
												".\\script\\IsisCharacter.dat"		};

	for(int i = 0; i < animus_storage_num; i++)
	{
		if(!s_tblParameter[i].ReadRecord(pszFileName[i], sizeof(_animus_fld)))
			return false;
	}

	return true;
}

CAnimus* FindEmptyAnimus(CAnimus* pObjArray, int nMax)
{
	DWORD dwTime = timeGetTime();

	for(int i = 0; i < nMax; i++)
	{
		if(pObjArray[i].m_bLive)
			continue;

		if(dwTime - pObjArray[i].m_dwLastDestroyTime <= 30*1000)	//30초..
			continue;

		return &pObjArray[i];		
	}

	return NULL;
}

bool CreateAnimus(CMapData* pMap, WORD wLayer, float* fPos, BYTE byClass, int nHP, int nFP, DWORD dwExp, CPlayer* pMaster)
{
	_animus_create_setdata Data;

	Data.m_pMap = pMap;
	Data.m_nLayerIndex = wLayer;	
	Data.m_pRecordSet = g_Main.m_tblAnimus.GetRecord(byClass);
	if(!Data.m_pRecordSet)
		return false;

	memcpy(Data.m_fStartPos, fPos, sizeof(float)*3);

	Data.nHP = nHP;
	Data.nFP = nFP;
	Data.dwExp = dwExp;
	Data.pMaster = pMaster;

	CAnimus* pAni = ::FindEmptyAnimus(g_Animus, MAX_ANIMUS);
	if(!pAni)
		return false;

	pAni->Create(&Data);

	return true;
}

_animus_fld* GetAnimusFldFromExp(int nAnimusClass, DWORD dwExp)
{
	CRecordData* pData = &CAnimus::s_tblParameter[nAnimusClass];

	for(int i = 0; i < max_animus_level; i++)
	{
		_animus_fld* pFld = (_animus_fld*)pData->GetRecord(i);
		if(!pFld)
			return NULL;

		if(pFld->m_nForLvUpExp > dwExp)
		{
			return pFld;
		}
	}

	return NULL;
}

DWORD GetMaxParamFromExp(int nAnimusClass, DWORD dwExp)
{
	CRecordData* pData = &CAnimus::s_tblParameter[nAnimusClass];

	for(int i = 0; i < max_animus_level; i++)
	{
		_animus_fld* pFld = (_animus_fld*)pData->GetRecord(i);
		if(!pFld)
			return 0;

		if(pFld->m_nForLvUpExp > dwExp)
		{
			_animus_param Param;

			Param.wHP = pFld->m_nMaxHP;
			Param.wFP = pFld->m_nMaxFP;

			return *(DWORD*)&Param;
		}
	}

	return 0;
}




