#include "Stdafx.h"
#include "MonsterAI.h"
#include "AIMessage.h"
#include "Monster.h"
#include "MainThread.h"


#define AI_DELAY	700

#define AI_ATTACK_DELAY 10

FSMMANAGER	g_FSMManager;

EMOTYPE EmotionType[10] = {
						99,		75,		75,		75,   50,
						99,		75,		50,		25,   50,
						99,		50,		50,		50,   50,
						99,		25,		50,		75,   50,
						99,		25,		25,		25,   50,
						100,	75,		75,		75,   50,
						100,	75,		50,		25,   50,
						100,	50,		50,		50,   50,
						100,	25,		50,		75,   50,
						100,	25,		25,		25,   50
};


void SKILL::InitForce(int level,int mastery,char* forceindex,int forcestdatt,int minprob,int maxprob,int castdelay)
{
	m_bLoad		= true;
	m_Type = 1;//Force
	int halfstd		= forcestdatt*((float)3.0f/4);	//ogogo..
	
	_force_fld* l_pForceFld = NULL;
	//l_pForceFld = (_force_fld *)g_Main.m_tblEffectData[effect_code_force].GetRecord(88);//forceindex);
	l_pForceFld = (_force_fld *)g_Main.m_tblEffectData[effect_code_force].GetRecord(forceindex);

	m_param.nTol = l_pForceFld->m_nProperty;
	m_param.nClass = 0;
	m_param.nMinAF = halfstd + ::rand() % (forcestdatt-halfstd);
	m_param.nMaxAF = forcestdatt * 2 - m_param.nMinAF;
	m_param.nMinSel = minprob;
	m_param.nMaxSel = maxprob;
	m_param.nLevel = 0;
	m_param.nMastery = 0;
//	m_param.nDist = l_pForceFld->m_nActDistance;
	m_param.pFld = (_base_fld*)l_pForceFld;

	m_CastDelay	= castdelay;// 주문동작시간 
	m_Delay		= l_pForceFld->m_fActDelay;	// 얼마나 자주 사용할것인가?
	m_Active	= 1;
	m_BefTime	= 0;
	m_Len		= l_pForceFld->m_nActDistance;

}
void GeCurrentStateString(char *str,DWORD state)
{
	sprintf (str,"NULL");
	switch (state)
	{
	case AI_STATE_NONE:
		sprintf(str,"AI_STATE_NONE");
		break;
	case AI_WAIT:
		sprintf(str,"AI_WAIT");
		break;
	case AI_MOVE:
		sprintf(str,"AI_MOVE");
		break;
	case AI_ATTACK:
		sprintf(str,"AI_ATTACK");
		break;
	case AI_SUPPORT:
		sprintf(str,"AI_SUPPORT");
		break;
	case AI_MOVE_NONE:
		sprintf(str,"AI_MOVE_NONE");
		break;
	case AI_MOVE_PATROL:
		sprintf(str,"AI_MOVE_PATROL");
		break;
	case AI_MOVE_MON:
		sprintf(str,"AI_MOVE_PATROL");
		break;
	case AI_MOVE_PLAYER:
		sprintf(str,"AI_MOVE_PLAYER");
		break;
	case AI_AT_DISABLE:
		sprintf(str,"AI_AT_DISABLE");
		break;
	case AI_AT_CHECK:
		sprintf(str,"AI_AT_CHECK");
		break;
	case AI_AT_ENABLE:
		sprintf(str,"AI_AT_ENABLE");
		break;
	case AI_PLAYER_NONE:
		sprintf(str,"AI_PLAYER_NONE");
		break;
	case AI_PLAYER_SEARCH:
		sprintf(str,"AI_PLAYER_SEARCH");
		break;
	case AI_PLAYER_EXIST:
		sprintf(str,"AI_PLAYER_EXIST");
		break;
	case AI_EM_NORMAL:
		sprintf(str,"AI_EM_NORMAL");
		break;
	case AI_EM_DISCOMPORT:
		sprintf(str,"AI_EM_DISCOMPORT");
		break;
	case AI_EM_ANGER:
		sprintf(str,"AI_EM_ANGER");
		break;
	case AI_EM_FURY:
		sprintf(str,"AI_EM_FURY");
		break;
	case AI_EM_MAD:
		sprintf(str,"AI_EM_MAD");
		break;
	case AI_MON_NONE:
		sprintf(str,"AI_MON_NONE");
		break;
	case AI_MON_SEARCH:
		sprintf(str,"AI_MON_SEARCH");
		break;
	case AI_MON_EXIST:
		sprintf(str,"AI_MON_EXIST");
		break;
	case AI_ACT_NORMAL:
		sprintf(str,"AI_ACT_NORMAL");
		break;
	case AI_ACT_RUNAWAY:
		sprintf(str,"AI_ACT_RUNAWAY");
		break;
	case AI_ACT_COP:
		sprintf(str,"AI_ACT_COP");
		break;
	case AI_CON_VERYGOOD:
		sprintf(str,"AI_CON_VERYGOOD");
		break;
	case AI_CON_GOOD:
		sprintf(str,"AI_CON_GOOD");
		break;
	case AI_CON_NORMAL:
		sprintf(str,"AI_CON_NORMAL");
		break;
	case AI_CON_CHECK:
		sprintf(str,"AI_CON_CHECK");
		break;
	case AI_CON_BAD:
		sprintf(str,"AI_CON_BAD");
		break;
	case AI_CON_VERYBAD:
		sprintf(str,"AI_CON_VERYBAD");
		break;
	case AI_CON_DEAD:
		sprintf(str,"AI_CON_DEAD");
		break;
	case AI_DEFFENSIVE:
		sprintf(str,"AI_DEFFENSIVE");
		break;
	case AI_OFFENSIVE:
		sprintf(str,"AI_OFFENSIVE");
		break;
	case AI_HELP_READY:
		sprintf(str,"AI_HELP_READY");
		break;
	case AI_HELP_ON:
		sprintf(str,"AI_HELP_ON");
		break;
	case AI_ASSIST_OFF:
		sprintf(str,"AI_ASSIST_OFF");
		break;
	case AI_ASSIST_ON:
		sprintf(str,"AI_ASSIST_ON");
		break;
	case AI_EV_DELAY:
		sprintf(str,"AI_EV_DELAY");
		break;
	case AI_EV_AT_CHECK_DELAY:
		sprintf(str,"AI_EV_AT_CHECK_DELAY");
		break;
	case AI_EV_AT_CHECK_TRUE:
		sprintf(str,"AI_EV_AT_CHECK_TRUE");
		break;
	case AI_EV_AT_CHECK_FALSE:
		sprintf(str,"AI_EV_AT_CHECK_FALSE");
		break;
	case AI_EV_ATTACK_ENABLE:
		sprintf(str,"AI_EV_ATTACK_ENABLE");
		break;
	case AI_EV_ATTACK_DISABLE:
		sprintf(str,"AI_EV_ATTACK_DISABLE");
		break;
	case AI_EV_ACT_COP:
		sprintf(str,"AI_EV_ACT_COP");
		break;
	case AI_EV_CON_GOOD:
		sprintf(str,"AI_EV_CON_GOOD");
		break;
	case AI_EV_CON_BAD:
		sprintf(str,"AI_EV_CON_BAD");
		break;
	case AI_EV_CON_DEAD:
		sprintf(str,"AI_EV_CON_DEAD");
		break;
	case AI_EV_PLAYER_DEAD:
		sprintf(str,"AI_EV_PLAYER_DEAD");
		break;
	case AI_EV_MV_MON:
		sprintf(str,"AI_EV_MV_MON");
		break;
	case AI_EV_MV_PLAYER:
		sprintf(str,"AI_EV_MV_PLAYER");
		break;
	case AI_EV_MV_DONE:
		sprintf(str,"AI_EV_MV_DONE");
		break;
	case AI_EV_OFFENSIVE:
		sprintf(str,"AI_EV_OFFENSIVE");
		break;
	case AI_EV_DEFFENSIVE:
		sprintf(str,"AI_EV_DEFFENSIVE");
		break;
	case AI_EV_ASSIST_SKILL:
		sprintf(str,"AI_EV_ASSIST_SKILL");
		break;
	case AI_EV_ASSIST_ON:
		sprintf(str,"AI_EV_ASSIST_ON");
		break;
	case AI_EV_EM_BAD:
		sprintf(str,"AI_EV_ASSIST_ON");
		break;
	case AI_EV_EM_GOOD:
		sprintf(str,"AI_EV_EM_GOOD");
		break;
	case AI_EV_ATTACK:
		sprintf(str,"AI_EV_ATTACK");
		break;
	case AI_EV_PLAYER_NONE:
		sprintf(str,"AI_EV_PLAYER_NONE");
		break;
	case AI_EV_PLAYER_EXIST:
		sprintf(str,"AI_EV_PLAYER_EXIST");
		break;
	case AI_EV_PLAYER_SEARCH:
		sprintf(str,"AI_EV_PLAYER_SEARCH");
		break;
	case AI_EV_MON_NONE:
		sprintf(str,"AI_EV_MON_NONE");
		break;
	case AI_EV_MON_DELAY:
		sprintf(str,"AI_EV_MON_DELAY");
		break;
	case AI_EV_MON_EXIST:
		sprintf(str,"AI_EV_MON_EXIST");
		break;
		}
}

void FSM::Process(DWORD time )// 딜레이 메세지 처리를 위해서 필요하다
{
	//DWORD time = timeGetTime();
	if (!m_Time) m_Time = time;
	DWORD del  =  time - m_Time;
	if (del>m_ElapseTime)
	{
		SendAIMessage(*this,AI_EV_DELAY,&m_ElapseTime);	
	}
}

bool FSM::Get(DWORD in)
{
	//sni.Set(m_State,in);
	//isMapItor i = m_Func.find(sni);
    //if(i != m_Func.end() )    
	if ( m_Func[m_State*FUN_WIDTH+in] )
	{
		
		//m_State = (*i).second;
		m_State = m_Func[m_State*FUN_WIDTH+in];
		m_Time  = timeGetTime();
		if(m_MsgFunc)
		{
			(*m_MsgFunc)(m_pObject,m_State);
		}
		///////////////////////////////////////////////////////////
		//isHFSMListItor i;
		//for (i=m_hParentList.begin();i!=m_hParentList.end();++i)

		if (m_hParentList)
		{
			//FSM  *parent = (*i);
			//SendAIMessage(*parent,m_State);
			SendAIMessage(*m_hParentList,m_State);
		}
		///////////////////////////////////////////////////////////
		
		return true;
	}
	else
		return false;
	
}

void	SetAIMsgFunc(FSM	&fsm,void *pobj,AIMSGFUNC	func)
{
	fsm.SetMsgFunc(pobj,func);
}

// Finite State Machine Rule 추가 
bool	AddFsmToFunc(FSM &fsm,WORD state,WORD in,WORD out)
{
	fsm.Add(state,in,out);
	return true;
}
bool	AddFsmToFunc(PFSM &fsm,WORD state,WORD in,WORD out)
{
	fsm.Add(state,in,out);
	return true;
}
// FSM에게 P1이라는 MSG를 발생시킨다
// DELAY이간 이후 AI_EV_DELAY가 발생된다
void	SendAIMessage(FSM &hfsm,DWORD p1,DWORD delay)
{
	if (hfsm.Get(p1))
	{
		if (delay)
			hfsm.m_ElapseTime  = (delay);
	}
}

// FSM에게 P1이라는 MSG를 발생시킨다
// DELAY이간 이후 AI_EV_DELAY가 발생된다
void	SendAIMessage(FSM &hfsm,DWORD p1,DWORD *delay)
{
	if (hfsm.Get(p1))
	{
		if (delay)
			hfsm.m_ElapseTime  = (*delay);
		
	}
}


void PlayerSearch(void *obj)
{
	CMonster	*mon = (CMonster*)obj;
	///*0805//일반 상태에선 공격하지 않는다 
	mon->m_pSearchPlayer = 0;
	
	if (mon->GetEmotionState() ==AI_EM_NORMAL&&((_monster_fld*)(mon->m_pRecordSet))->m_fEmoType<=5)
	{
		//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_NONE);
		return;
	}

	if (mon->m_MonAI.Emotion_State.m_State==AI_EM_NORMAL)
	{
		float offrate = 100 - ((_monster_fld*)(mon->m_pRecordSet))->m_fOffensiveRate*100.f;
		if ((::rand()%100)< offrate )
		{
			//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_NONE);
			return;
		}
	}
	//
	
	
	//		공격 할 플레이어를 선택한다. 가까운 거리순이다.
	//			1.	찾았을경우 AI_EV_PLAYER_EXIST
	//			2.  못 찾았을경우 AI_EV_PLAYER_NONE 의 메세지를 준다
	double	dNear= 9999999.0;
	CCharacter *p= NULL ;
	for (int i =0; i<mon->max_attack_player;i++)
	{
		if (mon->m_AttackedPlayer[i])
		{
			if (mon->IsValidPlayer(mon->m_AttackedPlayer[i]))
			{
				float fNew[3];
				double dDist = ::GetSqrt(mon->m_fCurPos,mon->m_AttackedPlayer[i]->m_fCurPos );
				if((mon->m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)mon->m_fCurPos, mon->m_AttackedPlayer[i]->m_fCurPos, &fNew)))
				{
					if (dNear>dDist)
					{
						dNear	= dDist;
						p		= mon->m_AttackedPlayer[i];
					}
				}
			}
			else
			{
				mon->m_AttackedPlayer[i] = NULL;
			}
		}
	}
	///* 플레이어가 있고, 액션 상태가 도망 상태인데 주위에 도움을 요청할 몬스터는 없다.
	//   그렇다면, 도망 상태를 해재 하고 전투에 임한다!
	if ( p )
	if ( mon->m_MonAI.Act_State.m_State == AI_ACT_RUNAWAY )
	{
		if (!mon->m_pMonTarget )
		{
			SendAIMessage( mon->m_MonAI.Act_State,AI_EV_CON_GOOD,3000);	//3초간 
		}
	}
	//*/


	// 도망 상태라면, 가장 최근에 자신을 타격한 플레이어를 타겟으로 설정한다.
	//2003/11/1
	if (!p)
	{
		if ( mon->m_MonAI.Act_State.m_State == AI_ACT_RUNAWAY )
		{
			for (int i =0; i<mon->max_attack_player;i++)
			{
				if (mon->m_AttackedPlayer[i])
				{
					if (mon->IsValidPlayer(mon->m_AttackedPlayer[i]))
					{
						p		= mon->m_AttackedPlayer[i];
						break;
					}
					else
					{
						mon->m_AttackedPlayer[i] = NULL;
					}
				}
			}
		}
	}
	//
	if (mon->m_pAssistPlayer )
	{
		if (mon->m_pAssistPlayer->m_bLive)
		{
			Vector3f fNew;
			if(mon->m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)mon->m_fCurPos, mon->m_pAssistPlayer->m_fCurPos, &fNew))
				p = mon->m_pAssistPlayer;
				//mon->m_pSearchPlayer = mon->m_pAssistPlayer;
		}
	}


	//2003/11/1
	if (p)
	{
		double dDist = ::GetSqrt(mon->m_fCurPos,p->m_fCurPos );
		if (dDist>240)//240 최대 Chase Distance
		{
			p = NULL;
		}
	}
	if (!p)
		p = mon->SearchNearPlayer();
	
	
	
	
	if (p)
	{
		//double dDist = ::GetSqrt(mon->m_fCurPos, p->m_fCurPos);
		//if (dDist < mon->m_ChaseDist)
		mon->m_pSearchPlayer = p;
	}
	
	
	//2003/8/18
	if (mon->m_pSearchPlayer)
	{
		if(((CCharacter*)mon->m_pSearchPlayer)->GetStealth())
		{
			//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_NONE);
			return;
		}
	}
	
	if(mon->m_pSearchPlayer && mon->m_pSearchPlayer->m_bLive )
	{
		//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_EXIST);
		return;
	}
	else
	{
		//2003/8/18
		if (mon->m_pPlayerTarget)
		{
			if(((CCharacter*)mon->m_pPlayerTarget)->GetStealth())
			{
				//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_NONE);
				return;
			}
		}
		
		if (mon->m_pPlayerTarget)
		{
			double dDist = ::GetSqrt(mon->m_fCurPos,mon->m_pPlayerTarget->m_fCurPos);
			if (dDist>mon->m_ChaseDist)//거리가 시야 범위 밖이다 
			{
				
				//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_NONE);
				return;
			}
			dDist = ::GetSqrt(mon->m_fCurPos, mon->m_fTarPos);
			if (dDist>=0.5)
			//if (mon->m_bMove )
			{
			
				mon->m_pSearchPlayer = mon->m_pPlayerTarget;
				//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_EXIST);
				return;
			}
			else// 더이상 쫓아갈수 없다.
			{
				if ( mon->m_CurNode>=mon->m_EndNode )
				{
					mon->m_CurNode = mon->m_EndNode = 0;
					mon->m_bNode = false;
					//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_NONE);
					mon->m_pSearchPlayer = mon->m_pPlayerTarget;
					//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_EXIST);
					return;
				}
				else
				{
					mon->m_CurNode++;
					mon->m_pSearchPlayer = mon->m_pPlayerTarget;
					//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_EXIST);
					return;
				}
			}
			
		}
		else
		{
			//SendAIMessage(mon->m_MonAI.Player_State,AI_EV_PLAYER_NONE);
			return;
		}
	}

}

void MonSearch(void *obj)
{
	CMonster	*mon = (CMonster*)obj;
	//			주위의 몬스터를 선택한다.
	//			1.	찾았을경우 AI_EV_MON_EXIST
	//			2.  못 찾았을경우 AI_EV_MON_NONE 의 메세지를 준다
	
	//*2003/7/7
	int AssistReqRate = ((_monster_fld*)mon->m_pRecordSet)->m_nAsitReqRate;
	int Rate			= rand()%100;
	if ( AssistReqRate> Rate)
	{
		//2003/7/7*/
		CMonster *p = NULL;
		//if (mon->m_MonAI.Help_State.m_State == AI_HELP_READY)
		if (mon->m_MonAI.Con_State.m_State >=AI_CON_NORMAL)
		{
			p = (CMonster*)mon->SearchNearMonster();
		}
		
		/*
		if (p)
		{
			double dDist = ::GetSqrt(mon->m_fCurPos, p->m_fCurPos);
			if (dDist < mon->m_ViewFieldLen)
				mon->m_pSearchPlayer = p;
			else
				mon->m_pSearchPlayer = 0;
		}
		*/
		mon->m_pSearchMon = p;
	}
	
	if(mon->m_pSearchMon)
	{
		SendAIMessage(mon->m_MonAI.Mon_State,AI_EV_MON_EXIST);
	}
	else
		SendAIMessage(mon->m_MonAI.Mon_State,AI_EV_MON_NONE);

}

void PrimeMove(void *obj)
{
	//	이동 루틴 
	//	목표 타겟으로 이동한다.
	CMonster	*mon = (CMonster*)obj;
	
	float befpos[3];
	memcpy(befpos,mon->m_fTarPos,sizeof(float)*3);
	BOOL flag = false;
	if (mon->m_bNode)//패스 파인더가 작동 중이다 
	{
		flag = mon->SetTarPos(mon->m_MoveTarget,false);
	}
	else
	{
		flag = mon->SetTarPos(mon->m_MoveTarget,true);
	}

	/*
	if (flag)
	{
		double tdist = ::GetSqrt(mon->m_MoveTarget, befpos);//과거의 타겟과 현재의 타겟의 차이가 적을경우 무시한다
		
		if ( tdist<=10.0f )
		{
			return;
		}
		
		double dDist = ::GetSqrt(mon->m_fCurPos, mon->m_fTarPos);//현재의 위치와 타겟의 위치가 차이가 적을경우 무시한다.
		if (dDist>=10.0f)
		//if (mon->m_bMove)
			mon->SendMsg_Move();
	}
	else
		memcpy(mon->m_fTarPos,mon->m_fCurPos,sizeof(float)* 3);//갈수 없다 
	*/
	
	double tdist = ::GetSqrt(mon->m_MoveTarget, befpos);//과거의 타겟과 현재의 타겟의 차이가 적을경우 무시한다
	double dDist = ::GetSqrt(mon->m_fCurPos, mon->m_fTarPos);//현재의 위치와 타겟의 위치가 차이가 적을경우 무시한다.
	
	/*
	if ( mon->m_pPlayerTarget )
	{
		static DWORD bef =0;
		DWORD	cur = timeGetTime();
		if (!bef)bef = cur;
		//if (bef == cur)
		{
			char buf[256];
			sprintf(buf,"Move Time: %d\n",cur - bef);
			OutputDebugString(buf);
		}
		bef = cur;
	}
	*/
	if ( tdist>=5.0f )
	{
		if ( dDist>=5.0f )
			mon->SendMsg_Move();
	}
	
}

void PrimeAttack(void *obj)
{
	CMonster	*mon = (CMonster*)obj;
	
	if (mon->m_pHelpMonster)
	{
		//자신한테 도움을 요청한 몬스터가 있는경우, 그 몬스터에게 같이 싸우도록 명령한다
		SendAIMessage(mon->m_pHelpMonster->m_MonAI.Act_State,AI_EV_ACT_COP,60000);
	}
	memcpy(mon->m_MoveTarget,mon->m_fCurPos,sizeof(float)*3);
	mon->SetTarPos(mon->m_fCurPos, false);
	//mon->Stop();//몬스터의 현재 상태를 멈춘다.
	
	/*
	if (mon->m_bMove)//몬스터가 움직이는 상태이면, 멈춘다.
	{
		if (mon->SetTarPos(mon->m_fCurPos))
		{
			mon->SendMsg_Move();
		}
	}*/
	
}

// 몬스터가 플레이어를 공격할 방식(SKILL)을 찾는다.
// SKILL에는 케스팅 딜레이, 일반딜레이가 있고
// 케스팅딜레이는 마법이나 스킬의 모션 딜레이이고,
// 일반딜레이는 그 스킬을 얼마나 자주쓸것인가를 나타낸다.
// 따라서 delay > castdelay 가 되도록 되어있는게 보통이다.

void AttackCheck(CMonster *mon,CCharacter	*player)
{
	mon->m_SearchSkill = -1;
	
	if (!player) 
	{
		SendAIMessage(mon->m_MonAI.Attack_State,AI_EV_AT_CHECK_FALSE,AI_ATTACK_DELAY);//플레이어가 없다. 공격을 할수 없다.
		return;
	}
	
	//
	/*7.9
	if (mon->m_bMove)
	{
		//SendAIMessage(mon->m_MonAI.Attack_State,AI_EV_AT_CHECK_FALSE,100);//이동중이다
		
		if (mon->IsValidPlayer())
		{
			int slot = mon->m_pPlayerTarget->GetSlot(mon);
			float rad =mon->GetAngle(mon->m_fCurPos,mon->m_pPlayerTarget->m_fCurPos);
			int n = int((rad / (2 * g_pi))*mon->slot_max + 0.5f)-1;
			if (slot !=n)
				SendAIMessage(mon->m_MonAI.Attack_State,AI_EV_AT_CHECK_FALSE,100);//이동중이다
		}
	}
	//7.9*/
	//*6.28
	/*
	double dD = ::GetSqrt(mon->m_fCurPos, mon->m_fTarPos);
	
	if (dD>=5.0)
	//if (mon->m_bMove)
	{
		SendAIMessage(mon->m_MonAI.Attack_State,AI_EV_AT_CHECK_FALSE,AI_ATTACK_DELAY);//이동중이다
		return;
	}
	//6.28*/

	DWORD	time	= timeGetTime();
	double dDist = ::GetSqrt(mon->m_fCurPos, player->m_fCurPos);
	int		m_DetectedSkill = -1;
	//for (int i =0;i<MAX_SKILL;i++)
	for (int i =0;i<mon->m_MaxSkill ;i++)
	{
		if (mon->m_Skill[i].m_bLoad )
		{
			if (mon->m_Skill[i].m_Active == false)
			{
				DWORD del	= time - mon->m_Skill[i].m_BefTime;
				if (del  > mon->m_Skill[i].m_Delay)
				{
					mon->m_Skill[i].m_Active = true;//딜레이가 초과된 기술들을 사용가능 상태로 돌린다.
				}
			}
			if (mon->m_Skill[i].m_Active)
			{
				float minlen = mon->m_Skill[i].m_Len * 0.1;
				//if (dDist<=mon->m_Skill[i].m_Len&&dDist>=minlen)
				if (dDist<=mon->m_Skill[i].m_Len)
				{
					mon->m_Skill[i].m_Active = false;
					mon->m_Skill[i].m_BefTime			= time;
					mon->m_SearchSkill = i;//새로 찾은 스킬을 입력한다.
					//SendAIMessage(mon->m_MonAI.Attack_State,AI_EV_AT_CHECK_TRUE,300);
					SendAIMessage(mon->m_MonAI.Attack_State,AI_EV_AT_CHECK_TRUE,&mon->m_Skill[i].m_CastDelay);//공격 가능하다는 메세지를 주고, 케스트 딜레이만큼 지연시킨다.
					return ;
				}
			}
		}
	}
	// 공격할 수있는 기술이 없다!
	SendAIMessage(mon->m_MonAI.Attack_State,AI_EV_AT_CHECK_FALSE,AI_ATTACK_DELAY);
	return ;
}

void ActionStateMsgFunc(void *obj,DWORD state)
{
	CMonster	*mon	= (CMonster*)obj;
	switch(state)
	{
	case	AI_ACT_NORMAL:
		//초기 상태, 도망 상태가 아니다.
		mon->m_RunAway = false;
		//SendAIMessage(mon->m_MonAI.Act_State,AI_EV_CON_BAD);
		break;
	case	AI_ACT_RUNAWAY:
		// 도망 상태
 		mon->m_RunAway = true;
		break;
	case	AI_ACT_COP:
		// 누군가 나를 도와주는 몬스터가 있다. 협동 모드 전환
		mon->m_RunAway = false;
		break;
	}
}

void EmotionStateMsgFunc(void *obj,DWORD state)
{
	CMonster	*mon	= (CMonster*)obj;
	switch(state)
	{
	case AI_EM_NORMAL:
		memset(mon->m_AttackedPlayer,0
			,sizeof(CCharacter	*)*mon->max_attack_player);//몬스터 공격 플레이어로그 삭제
		break;
	case AI_EM_DISCOMPORT:
		break;
	case AI_EM_ANGER:
		break;
	case AI_EM_FURY:
		break;
	case AI_EM_MAD:
		break;
	}
}

void AssistStateMsgFunc(void *obj,DWORD state)
{
	CMonster	*mon	= (CMonster*)obj;
	switch(state)
	{
	case	AI_ASSIST_OFF:
		// 주위에 자신이 도울 몬스터가 없다.
		mon->m_pAssistPlayer = NULL;
		break;
	case	AI_ASSIST_ON:
		// 현재 상태는 다른 몬스터를 조와주고 있는 상태이다.
		break;
	}
}


void AttackStateMsgFunc(void *obj,DWORD state)
{
	CMonster	*mon	= (CMonster*)obj;
	CCharacter	*player = mon->m_pPlayerTarget;
	
	
	switch(state)
	{
	case	AI_AT_DISABLE:
		//설정된 기술이 없다
		mon->m_CurrentSkill = -1;
		break;
	case	AI_AT_CHECK:
		//공격 가능한가를 확인한다.
		AttackCheck(mon,player);
		break;
	case	AI_AT_ENABLE:
		//현재 설정된 스킬을 바탕으로 공격한다.
		mon->Attack();
		break;
	}
}


void PlayerStateMsgFunc(void *obj,DWORD state)
{
	CMonster	*mon = (CMonster*)obj;
	double pi = 3.1415926535;
   	
	switch(state)
	{
	case	AI_PLAYER_NONE:
		//
		if (mon->m_pPlayerTarget)
		{
			if(mon->m_bMove)
			{
				//mon->SetTarPos( mon->m_fCurPos );
				mon->Stop();
				mon->SendMsg_BreakStop();
			}
		}
		//
		// 주위에 플레이어가 없다.
		if (mon->m_pPlayerTarget)
		{
			mon->m_pPlayerTarget->RemoveSlot(mon);
		}
		mon->m_pPlayerTarget = NULL;
		//mon->SetChaseChar(NULL);
		break;
	
	case	AI_PLAYER_SEARCH:
		// 주위에 플레이러를 검색한다.
		PlayerSearch(obj);
		break;
	case	AI_PLAYER_EXIST:

		//선공 몬스터는 플레이어의 등장으로 인해 Emotion State가 영향을 받는다.
		if (mon->m_MonAI.Emotion_State.m_State==AI_EM_NORMAL)
		{
			if (((_monster_fld*)(mon->m_pRecordSet))->m_fEmoType>5)// 6부터 선공 
			{
				SendAIMessage(mon->m_MonAI.Emotion_State,AI_EV_EM_BAD,10000);
			}
		}
		// 주위에 플레이어가있다.
		//int  d =10+rand()%10;
		//int	d = mon->GetAttackRange()-1.0;
		//기존의 플레이어와 새로운 플레이어가 다르다 
		if (mon->m_pPlayerTarget != mon->m_pSearchPlayer)
		{
			if (mon->m_pPlayerTarget)mon->m_pPlayerTarget->RemoveSlot(mon);
		}
		
		
		mon->m_pPlayerTarget = mon->m_pSearchPlayer;
		mon->m_pSearchPlayer = NULL;
		// 주위에 몬스터가있다면 나를 돕도록 명령한다.
		if (mon->m_pMonTarget )
		{
			if ( mon->m_pMonTarget->m_bLive ) 
			{
				if (mon->m_MonAI.Help_State.m_State == AI_HELP_READY)
				{
					//DWORD time = timeGetTime();
					//mon->m_MonAI.Mon_State.Process(time);
					//mon->m_pMonTarget =(CMonster*) mon->SearchNearMonster();
					CMonster *other =(CMonster*) mon->m_pMonTarget;
					if (other)
					if (!other->m_pAssistPlayer)//도와주고 있는 타겟이 없을경우만 작동한다
					{
						if (!other->m_RunAway)//도움을 요청 받은 몹이 도망상태가 아니어야한다
						{
							other->m_pHelpMonster	= mon;
						}
						
						//if (((_monster_record*)mon->m_pRecordSet)->m_bAsitReqTF)
						if (mon->m_MonAI.Con_State.m_State >=AI_CON_NORMAL)
						{
							int AssistAceptRate = ((_monster_fld*)other->m_pRecordSet)->m_nAsitAptRate;
							int Rate			= rand()%100;
							if ( AssistAceptRate> Rate)
							{
								other->m_pAssistPlayer 	= mon->m_pSearchPlayer;
								
								//
								SendAIMessage(other->m_MonAI.Emotion_State,AI_EV_EM_BAD,((_monster_fld*)other->m_pRecordSet)->m_fEmoImpStdTime);
								//
								SendAIMessage(other->m_MonAI.Assist_State,AI_EV_ASSIST_ON,30000);
							}
							SendAIMessage(mon->m_MonAI.Help_State,AI_EV_HELP,5000);
						}
						
					}
				}
			}

		}
		break;
	}
}
// 주위의 몬스터들을 확인하는 State
void MonStateMsgFunc(void *obj,DWORD state)
{
	CMonster	*mon = (CMonster*)obj;
	
   	
	switch(state)
	{
	case	AI_MON_NONE:
		mon->m_pMonTarget  = NULL;
		break;
	case	AI_MON_SEARCH:
		MonSearch(obj);
		break;
	case	AI_MON_EXIST:
		mon->m_pMonTarget = mon->m_pSearchMon;
		break;
	}
}

// 어디로 이동해야하는가를 나타내는 State
void MoveTargetStateFunc(void *obj,DWORD state)
{
	
	CMonster	*mon = (CMonster*)obj;
	const float degree = 50.0f;//패트롤 이동 거리
	
	
	switch(state)
	{
	case	AI_MOVE_NONE:
		//
		if ( mon->m_bMove )
		{
			//mon->SetTarPos( mon->m_fCurPos );
			mon->Stop();
			mon->SendMsg_BreakStop();
		}
		//
		break;
	case	AI_MOVE_PATROL:
		
		mon->m_MoveTarget[0]	=mon->m_fCurPos[0]+ (float)(::rand()%(int)(degree+ 1))-degree/2.0f;
		mon->m_MoveTarget[2]	=mon->m_fCurPos[2]+ (float)(::rand()%(int)(degree+ 1))-degree/2.0f;
		mon->m_MoveTarget[1]	=mon->m_fCurPos[1];
		/*
		Vector3f fNew;
		if(!mon->m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)mon->m_fCurPos, (Vector3f)mon->m_MoveTarget, &fNew))
		{
			memcpy(mon->m_MoveTarget,mon->m_fCurPos,sizeof(float)*3);
		}	*/
		//SendAIMessage(mon->m_MonAI.Move_Target_State,AI_EV_MV_DONE,8000);
		break;
	case	AI_MOVE_MON:
		if (mon->m_pMonTarget )
		{
			if ( mon->m_pMonTarget->m_bLive ) 
				memcpy(mon->m_MoveTarget,mon->m_pMonTarget->m_fCurPos,sizeof(float)*3);
		}
		break;
	case	AI_MOVE_PLAYER:
		//도망 상태라면 플레이어의 반대방향으로 움직이고
		//일반 상태라면 플레이어에게 간다.
		if (mon->m_RunAway)
		{
			float x = mon->m_pPlayerTarget->m_fCurPos[0] - mon->m_fCurPos[0];
			float y = mon->m_pPlayerTarget->m_fCurPos[2] - mon->m_fCurPos[2];

			float d = 30.0/(abs(x)+abs(y));
			float x1= (-x)*d;
			float y1= (-y)*d;
			
			
			mon->m_MoveTarget[0]	=x1+mon->m_fCurPos[0];
			mon->m_MoveTarget[2]	=y1+mon->m_fCurPos[2];
			mon->m_MoveTarget[1]	=mon->m_fCurPos[1];


			//도망 갈 수 없는 지형이라면, 랜덤하게 다시 움직인다.
			Vector3f fNew;
			if(!mon->m_pCurMap->m_Level.mBsp->CanYouGoThere((Vector3f)mon->m_fCurPos, (Vector3f)mon->m_MoveTarget, &fNew))
			{
				mon->m_MoveTarget[0]	=mon->m_fCurPos[0]+ (float)(::rand()%(int)(degree+ 1))-degree/2.0f;
				mon->m_MoveTarget[2]	=mon->m_fCurPos[2]+ (float)(::rand()%(int)(degree+ 1))-degree/2.0f;
				mon->m_MoveTarget[1]	=mon->m_fCurPos[1];
			}	
		}
		else
		{
			/*
			//memcpy(mon->m_MoveTarget,mon->m_pMonTarget->m_fCurPos,sizeof(float)*3);
			memcpy(mon->m_MoveTarget,mon->m_pPlayerTarget->m_fCurPos,sizeof(float)*3);
			mon->m_MoveTarget[0] += mon->m_AroundX;
			mon->m_MoveTarget[2] += mon->m_AroundY;
			*/
			//mon->GetTarPosChase();
			mon->GetPlayerPos();
			//mon->m_MoveTarget[0] = mon->m_fTarPos[0];
			//mon->m_MoveTarget[2] = mon->m_fTarPos[2];
		}
		break;
	}
	
}
void ConStateMsgFunc(void *obj,DWORD state)
{
	CMonster	*mon = (CMonster*)obj;
	switch(state)
	{
	case AI_CON_CHECK:
		mon->CheckCondition();
		break;
	}
}

// 가장 기초가 되는 State로 WAIT, MOVE, ATTACK이 있다.
void PrimeStateMsgFunc(void *obj,DWORD state)
{
	
	switch (state)
	{
	case	AI_WAIT:
		break;
	case	AI_MOVE:
		PrimeMove(obj);
		break;
	case	AI_ATTACK:
		PrimeAttack(obj);
		break;
//	case	AI_ATTACK_WAIT:
//		break;
	}
	
}

// 몬스터의 AI를 입력하는 부분이다. 
void SetMonterAI(CMonster	*pMonster)
{
	_monster_fld* record =  ((_monster_fld*)pMonster->m_pRecordSet);
	
	
	pMonster->m_Skill[1].Init
		(0,record->m_fAttFcStd*pMonster->m_Mightiness,record->m_fMinAFSelProb,
		record->m_fMaxAFSelProb,record->m_fAttExt,record->m_fAttMoTime1,record->m_fAttSpd);
	
	_force_fld* l_pForceFld = (_force_fld *)g_Main.m_tblEffectData[effect_code_force].GetRecord(record->m_strAttTechID1);
	if (l_pForceFld)
	{
		pMonster->m_Skill[0].InitForce(
		 record->m_fForceLevel, record->m_fForceMastery, record->m_strAttTechID1,
		 record->m_fAttFcStd,record->m_fMinAFSelProb,record->m_fMaxAFSelProb,
		 record->m_fAttTechID1MotionTime );
	}


		

		//bRet = at.AttackGen(&ap);
	

	

	MONSTERAI *ai= &pMonster->m_MonAI;
	
	//8/14//Init
	ai->Act_State.FSMInit();
	ai->Assist_State.FSMInit();
	ai->Attack_State.FSMInit();
	ai->Con_State.FSMInit();
	ai->Emotion_State.FSMInit();
	ai->Help_State.FSMInit();
	ai->Mon_State.FSMInit();
	ai->Move_Target_State.FSMInit();
	ai->Personal_State.FSMInit();
	ai->Player_State.FSMInit();
	ai->Prime_State.FSMInit();
	//
	//////////////////////////////////////////////////////
	ai->Move_Target_State.SetFSMParent(ai->Prime_State);
	ai->Attack_State.SetFSMParent(ai->Prime_State);
	////////////////////////////////////////////////////////
	ai->Player_State.SetFSMParent(ai->Move_Target_State);
	ai->Mon_State.SetFSMParent(ai->Move_Target_State);
	ai->Con_State.SetFSMParent(ai->Act_State);
	ai->Emotion_State.SetFSMParent(ai->Move_Target_State);
	ai->Assist_State.SetFSMParent(ai->Player_State);
	

	SetAIMsgFunc(ai->Prime_State,(void*)pMonster,PrimeStateMsgFunc);
	SetAIMsgFunc(ai->Move_Target_State,(void*)pMonster,MoveTargetStateFunc);
	SetAIMsgFunc(ai->Player_State,(void*)pMonster,PlayerStateMsgFunc);
	SetAIMsgFunc(ai->Mon_State,(void*)pMonster,MonStateMsgFunc);
	SetAIMsgFunc(ai->Attack_State,(void*)pMonster,AttackStateMsgFunc);
	SetAIMsgFunc(ai->Assist_State,(void*)pMonster,AssistStateMsgFunc);
	SetAIMsgFunc(ai->Act_State,(void*)pMonster,ActionStateMsgFunc);
	SetAIMsgFunc(ai->Con_State,(void*)pMonster,ConStateMsgFunc);
	SetAIMsgFunc(ai->Emotion_State,(void*)pMonster,EmotionStateMsgFunc);
	
	static BOOL firstAIInit = false;
	static PMONSTERAI pai_;
	PMONSTERAI *pai= &pai_;

	if ( !firstAIInit )
	{
		firstAIInit = true;
		
		////////////////////////////////////////////////////////
		//AddFsmToFunc(pai->Prime_State,	AI_WAIT,	AI_EV_DELAY,		AI_MOVE);
		AddFsmToFunc(pai->Prime_State,	AI_WAIT,	AI_MOVE_PATROL,		AI_MOVE);
		//AddFsmToFunc(pai->Prime_State,	AI_WAIT,	AI_MOVE_MON,		AI_MOVE);
		AddFsmToFunc(pai->Prime_State,	AI_WAIT,	AI_MOVE_PLAYER,		AI_MOVE);
		AddFsmToFunc(pai->Prime_State,	AI_WAIT,	AI_AT_ENABLE,		AI_ATTACK);
		////////////////////////////////////////////////////////
		AddFsmToFunc(pai->Prime_State,	AI_MOVE,	AI_MOVE_NONE,		AI_WAIT);
		//AddFsmToFunc(pai->Prime_State,	AI_MOVE,	AI_MOVE_MON,		AI_MOVE);
		AddFsmToFunc(pai->Prime_State,	AI_MOVE,	AI_MOVE_PLAYER,		AI_MOVE);
		AddFsmToFunc(pai->Prime_State,	AI_MOVE,	AI_AT_ENABLE,		AI_ATTACK);
		////////////////////////////////////////////////////////
		//AddFsmToFunc(pai->Prime_State,	AI_ATTACK,	AI_AT_DISABLE,		AI_MOVE);
		AddFsmToFunc(pai->Prime_State,	AI_ATTACK,	AI_AT_DISABLE,		AI_WAIT);
		AddFsmToFunc(pai->Prime_State,	AI_ATTACK,	AI_AT_ENABLE,		AI_ATTACK);
		////////////////////////////////////////////////////////
		AddFsmToFunc(pai->Attack_State,		AI_AT_DISABLE,		AI_EV_DELAY,			AI_AT_CHECK);
		AddFsmToFunc(pai->Attack_State,		AI_AT_CHECK,		AI_EV_AT_CHECK_TRUE,	AI_AT_ENABLE);
		AddFsmToFunc(pai->Attack_State,		AI_AT_CHECK,		AI_EV_AT_CHECK_FALSE,	AI_AT_DISABLE);
		AddFsmToFunc(pai->Attack_State,		AI_AT_ENABLE,		AI_EV_DELAY,		AI_AT_CHECK);
		////////////////////////////////////////////////////////
		AddFsmToFunc(pai->Move_Target_State,		AI_MOVE_NONE,	AI_EV_DELAY,			AI_MOVE_PATROL);
		AddFsmToFunc(pai->Move_Target_State,		AI_MOVE_NONE,	AI_EV_MV_MON,			AI_MOVE_MON);
		AddFsmToFunc(pai->Move_Target_State,		AI_MOVE_NONE,	AI_PLAYER_EXIST,		AI_MOVE_PLAYER);
		

		AddFsmToFunc(pai->Move_Target_State,		AI_MOVE_PATROL,	AI_EV_MV_DONE,			AI_MOVE_NONE);
		AddFsmToFunc(pai->Move_Target_State,		AI_MOVE_PATROL,	AI_PLAYER_EXIST,		AI_MOVE_PLAYER);

		AddFsmToFunc(pai->Move_Target_State,		AI_MOVE_MON,	AI_MON_NONE,			AI_MOVE_NONE);
		AddFsmToFunc(pai->Move_Target_State,		AI_MOVE_PLAYER,	AI_PLAYER_NONE,			AI_MOVE_NONE);
		AddFsmToFunc(pai->Move_Target_State,		AI_MOVE_PLAYER, AI_PLAYER_EXIST,		AI_MOVE_PLAYER);
		///*0702
		//AddFsmToFunc(ai->Move_Target_State,		AI_MOVE_PLAYER,	AI_EV_DELAY,			AI_MOVE_PLAYER);
		//*/

		
		AddFsmToFunc(pai->Player_State,		AI_PLAYER_NONE,		AI_EV_DELAY,			AI_PLAYER_SEARCH);
		AddFsmToFunc(pai->Player_State,		AI_PLAYER_NONE,		AI_ASSIST_ON,			AI_PLAYER_SEARCH);
		AddFsmToFunc(pai->Player_State,		AI_PLAYER_SEARCH,	AI_EV_PLAYER_NONE,		AI_PLAYER_NONE);
		AddFsmToFunc(pai->Player_State,		AI_PLAYER_SEARCH,	AI_EV_PLAYER_EXIST,		AI_PLAYER_EXIST);
		AddFsmToFunc(pai->Player_State,		AI_PLAYER_EXIST,	AI_ASSIST_OFF,			AI_PLAYER_SEARCH);
		AddFsmToFunc(pai->Player_State,		AI_PLAYER_EXIST,	AI_EV_DELAY,	AI_PLAYER_SEARCH);

		AddFsmToFunc(pai->Personal_State,	AI_DEFFENSIVE,		AI_EV_OFFENSIVE,			AI_OFFENSIVE);
		AddFsmToFunc(pai->Personal_State,	AI_OFFENSIVE,		AI_EV_DEFFENSIVE,			AI_DEFFENSIVE);


		//AddFsmToFunc(ai->Assist_State,		AI_ASSIST_OFF,		AI_EV_ASSIST_ON,			AI_ASSIST_ON);
		//AddFsmToFunc(ai->Assist_State,		AI_ASSIST_ON,		AI_EV_DELAY,				AI_ASSIST_OFF);

		AddFsmToFunc(pai->Assist_State,		AI_ASSIST_OFF,		AI_EV_ASSIST_ON,			AI_ASSIST_ON);
		AddFsmToFunc(pai->Assist_State,		AI_ASSIST_OFF,		AI_EV_ASSIST_FAIL,			AI_ASSIST_FAIL);
		AddFsmToFunc(pai->Assist_State,		AI_ASSIST_FAIL,		AI_EV_DELAY,				AI_ASSIST_OFF);
		AddFsmToFunc(pai->Assist_State,		AI_ASSIST_ON,		AI_EV_DELAY,				AI_ASSIST_OFF);
		
		
		AddFsmToFunc(pai->Mon_State,			AI_MON_NONE,		AI_EV_DELAY,			AI_MON_SEARCH);
		AddFsmToFunc(pai->Mon_State,			AI_MON_SEARCH,		AI_EV_MON_NONE,			AI_MON_NONE);
		AddFsmToFunc(pai->Mon_State,			AI_MON_SEARCH,		AI_EV_MON_EXIST,		AI_MON_EXIST);
		AddFsmToFunc(pai->Mon_State,			AI_MON_EXIST,		AI_EV_DELAY,			AI_MON_SEARCH);
		
		AddFsmToFunc(pai->Act_State,			AI_ACT_NORMAL,		AI_EV_CON_BAD,			AI_ACT_RUNAWAY);
		AddFsmToFunc(pai->Act_State,			AI_ACT_RUNAWAY,		AI_EV_ACT_COP,			AI_ACT_COP);
		AddFsmToFunc(pai->Act_State,			AI_ACT_RUNAWAY,		AI_EV_CON_GOOD,			AI_ACT_NORMAL);
		//2003/11/1 도망 상태에서도 일정 시간이 지나면 다시 일반상태로 돌아간다 
		AddFsmToFunc(pai->Act_State,			AI_ACT_RUNAWAY,		AI_EV_DELAY,			AI_ACT_NORMAL);
		//
		AddFsmToFunc(pai->Act_State,			AI_ACT_COP,			AI_EV_DELAY,			AI_ACT_NORMAL);
		

		AddFsmToFunc(pai->Con_State,			AI_CON_VERYGOOD,	AI_EV_DELAY,		AI_CON_CHECK);
		AddFsmToFunc(pai->Con_State,			AI_CON_GOOD,		AI_EV_DELAY,		AI_CON_CHECK);
		AddFsmToFunc(pai->Con_State,			AI_CON_NORMAL,		AI_EV_DELAY,		AI_CON_CHECK);
		AddFsmToFunc(pai->Con_State,			AI_CON_BAD,			AI_EV_DELAY,		AI_CON_CHECK);
		AddFsmToFunc(pai->Con_State,			AI_CON_VERYBAD,		AI_EV_DELAY,		AI_CON_CHECK);
		
		AddFsmToFunc(pai->Con_State,			AI_CON_CHECK,		AI_EV_CON_VERYGOOD,		AI_CON_VERYGOOD);
		AddFsmToFunc(pai->Con_State,			AI_CON_CHECK,		AI_EV_CON_GOOD,			AI_CON_GOOD);
		AddFsmToFunc(pai->Con_State,			AI_CON_CHECK,		AI_EV_CON_NORMAL,		AI_CON_NORMAL);
		AddFsmToFunc(pai->Con_State,			AI_CON_CHECK,		AI_EV_CON_BAD,			AI_CON_BAD);
		AddFsmToFunc(pai->Con_State,			AI_CON_CHECK,		AI_EV_CON_VERYBAD,		AI_CON_VERYBAD);
		AddFsmToFunc(pai->Con_State,			AI_CON_CHECK,		AI_EV_CON_DEAD,		AI_CON_DEAD);
		
		AddFsmToFunc(pai->Emotion_State,		AI_EM_NORMAL,		AI_EV_DELAY,			AI_EM_NORMAL);
		AddFsmToFunc(pai->Emotion_State,		AI_EM_NORMAL,		AI_EV_EM_BAD,			AI_EM_DISCOMPORT);
		AddFsmToFunc(pai->Emotion_State,		AI_EM_DISCOMPORT,	AI_EV_DELAY,			AI_EM_NORMAL);
		AddFsmToFunc(pai->Emotion_State,		AI_EM_DISCOMPORT,	AI_EV_EM_BAD,			AI_EM_ANGER);
		AddFsmToFunc(pai->Emotion_State,		AI_EM_ANGER,		AI_EV_DELAY,			AI_EM_DISCOMPORT);
		AddFsmToFunc(pai->Emotion_State,		AI_EM_ANGER,		AI_EV_EM_BAD,			AI_EM_FURY);
		AddFsmToFunc(pai->Emotion_State,		AI_EM_FURY,			AI_EV_DELAY,			AI_EM_ANGER);
		AddFsmToFunc(pai->Emotion_State,		AI_EM_FURY,			AI_EV_EM_BAD,			AI_EM_MAD);
		AddFsmToFunc(pai->Emotion_State,		AI_EM_MAD,			AI_EV_DELAY,			AI_EM_FURY);
		
		AddFsmToFunc(pai->Help_State,		AI_HELP_READY,		AI_EV_HELP,				AI_HELP_ON);
		AddFsmToFunc(pai->Help_State,		AI_HELP_ON,			AI_EV_DELAY,			AI_HELP_READY);
	}
	//
	ai->Act_State.m_Func	= pai->Act_State.m_Func;
	ai->Act_State.m_State	= pai->Act_State.m_State;
	
	ai->Assist_State.m_Func = pai->Assist_State.m_Func;
	ai->Assist_State.m_State = pai->Assist_State.m_State;

	ai->Attack_State.m_Func = pai->Attack_State.m_Func;
	ai->Attack_State.m_State = pai->Attack_State.m_State;

	ai->Con_State.m_Func	= pai->Con_State.m_Func;
	ai->Con_State.m_State	= pai->Con_State.m_State;

	ai->Emotion_State.m_Func= pai->Emotion_State.m_Func;
	ai->Emotion_State.m_State= pai->Emotion_State.m_State;

	ai->Help_State.m_Func	= pai->Help_State.m_Func;
	ai->Help_State.m_State	= pai->Help_State.m_State;

	ai->Mon_State.m_Func	= pai->Mon_State.m_Func;
	ai->Mon_State.m_State	= pai->Mon_State.m_State;

	ai->Move_Target_State.m_Func = pai->Move_Target_State.m_Func;
	ai->Move_Target_State.m_State = pai->Move_Target_State.m_State;

	ai->Personal_State.m_Func = pai->Personal_State.m_Func;
	ai->Personal_State.m_State = pai->Personal_State.m_State;
	
	ai->Player_State.m_Func = pai->Player_State.m_Func;
	ai->Player_State.m_State = pai->Player_State.m_State;
	
	ai->Prime_State.m_Func  = pai->Prime_State.m_Func;
	ai->Prime_State.m_State  = pai->Prime_State.m_State;
	//
	

	/*
	AddFsmToFunc(ai->Emotion_State,		AI_EM_NORMAL,		AI_EV_EM_BAD,			AI_EM_DISCOMPORT);
	AddFsmToFunc(ai->Emotion_State,		AI_EM_DISCOMPORT,	AI_EV_EM_GOOD,			AI_EM_NORMAL);
	AddFsmToFunc(ai->Emotion_State,		AI_EM_DISCOMPORT,	AI_EV_EM_BAD,			AI_EM_ANGER);
	AddFsmToFunc(ai->Emotion_State,		AI_EM_ANGER,		AI_EV_EM_BAD,			AI_EM_FURY);
	AddFsmToFunc(ai->Emotion_State,		AI_EM_ANGER,		AI_EV_EM_GOOD,			AI_EM_DISCOMPORT);
	AddFsmToFunc(ai->Emotion_State,		AI_EM_FURY,			AI_EV_EM_BAD,			AI_EM_MAD);
	AddFsmToFunc(ai->Emotion_State,		AI_EM_FURY,			AI_EV_EM_GOOD,			AI_EM_ANGER);
	*/
	//처음 행동을 유도한다.Seed
	

	SendAIMessage(ai->Act_State,			AI_EV_DELAY,		999);

	SendAIMessage(ai->Move_Target_State,	AI_EV_DELAY,	499);
	SendAIMessage(ai->Player_State,			AI_EV_DELAY,	250);
	//SendAIMessage(ai->Player_State,			AI_EV_DELAY,	500);
	SendAIMessage(ai->Mon_State,			AI_EV_DELAY,	891);
	SendAIMessage(ai->Attack_State,			AI_EV_DELAY,	AI_ATTACK_DELAY);
	SendAIMessage(ai->Con_State,			AI_EV_DELAY,	1000);
	SendAIMessage(ai->Emotion_State,		AI_EV_DELAY,	30000);
	
	/*
	SendAIMessage(ai->Move_Target_State,	AI_EV_DELAY,	1000);
	SendAIMessage(ai->Player_State,			AI_EV_DELAY,	1500);
	SendAIMessage(ai->Mon_State,			AI_EV_DELAY,	1000);
	SendAIMessage(ai->Attack_State,			AI_EV_DELAY,	500);
	SendAIMessage(ai->Con_State,			AI_EV_DELAY,	1000);
	SendAIMessage(ai->Emotion_State,		AI_EV_DELAY,	10000);
	*/
	
	
	
	
	/*
	int r			= ::rand()%2;
	if (r)
	{
		SendAIMessage(ai->Act_State,			AI_EV_CON_BAD,	&delay);
	}*/
	
	
}
