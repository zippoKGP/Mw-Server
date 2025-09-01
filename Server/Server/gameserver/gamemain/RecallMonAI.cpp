#include "stdafx.h"
#include "RecallMon.h"
#include "MonsterAI.h"
#define AI_DELAY	700
/*
void ActionStateMsgFunc(void *obj,DWORD state);

void AttackStateMsgFunc(void *obj,DWORD state);


void EnemyStateMsgFunc(void *obj,DWORD state);

void MoveTargetStateFunc(void *obj,DWORD state);


void PrimeMove(void *obj);

void PrimeAttack(void *obj);

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
	}
}

void SetRecallMonAI(CRecallMon	*pMonster)
{
	_monster_fld* record =  ((_monster_fld*)pMonster->m_pRecordSet);
	
	pMonster->m_Skill[0].Init
		(0,record->m_fAttFcStd*pMonster->m_Mightiness,record->m_fMinAFSelProb,
		record->m_fMaxAFSelProb,record->m_fAttExt,record->m_fAttMoTime1,record->m_fAttSpd);
	

	RECALLMONAI *ai= &pMonster->m_MonAI;
	//8/14//Init
	ai->Act_State.FSMInit();
	ai->Attack_State.FSMInit();
	ai->Mon_State.FSMInit();
	ai->Move_Target_State.FSMInit();
	ai->Enemy_State.FSMInit();
	ai->Prime_State.FSMInit();
	//
	//////////////////////////////////////////////////////
	ai->Move_Target_State.SetFSMParent(ai->Prime_State);
	ai->Attack_State.SetFSMParent(ai->Prime_State);
	////////////////////////////////////////////////////////
	ai->Enemy_State.SetFSMParent(ai->Move_Target_State);
	ai->Mon_State.SetFSMParent(ai->Move_Target_State);
	

	SetAIMsgFunc(ai->Prime_State,(void*)pMonster,PrimeStateMsgFunc);
	SetAIMsgFunc(ai->Move_Target_State,(void*)pMonster,MoveTargetStateFunc);
	SetAIMsgFunc(ai->Enemy_State,(void*)pMonster,EnemyStateMsgFunc);
	SetAIMsgFunc(ai->Attack_State,(void*)pMonster,AttackStateMsgFunc);
	SetAIMsgFunc(ai->Act_State,(void*)pMonster,ActionStateMsgFunc);
	
	
	

	////////////////////////////////////////////////////////
	AddFsmToFunc(ai->Prime_State,	AI_WAIT,	AI_EV_DELAY,		AI_MOVE);
	AddFsmToFunc(ai->Prime_State,	AI_WAIT,	AI_MOVE_PATROL,		AI_MOVE);
	AddFsmToFunc(ai->Prime_State,	AI_WAIT,	AI_MOVE_MON,		AI_MOVE);
	AddFsmToFunc(ai->Prime_State,	AI_WAIT,	AI_MOVE_PLAYER,		AI_MOVE);
	AddFsmToFunc(ai->Prime_State,	AI_WAIT,	AI_AT_ENABLE,		AI_ATTACK);
	////////////////////////////////////////////////////////
	AddFsmToFunc(ai->Prime_State,	AI_MOVE,	AI_MOVE_NONE,		AI_WAIT);
	AddFsmToFunc(ai->Prime_State,	AI_MOVE,	AI_MOVE_MON,		AI_MOVE);
	AddFsmToFunc(ai->Prime_State,	AI_MOVE,	AI_MOVE_PLAYER,		AI_MOVE);
	AddFsmToFunc(ai->Prime_State,	AI_MOVE,	AI_AT_ENABLE,		AI_ATTACK);
	////////////////////////////////////////////////////////
	AddFsmToFunc(ai->Prime_State,	AI_ATTACK,	AI_AT_DISABLE,		AI_MOVE);
	AddFsmToFunc(ai->Prime_State,	AI_ATTACK,	AI_AT_ENABLE,		AI_ATTACK);
	////////////////////////////////////////////////////////
	AddFsmToFunc(ai->Attack_State,		AI_AT_DISABLE,		AI_EV_DELAY,			AI_AT_CHECK);
	AddFsmToFunc(ai->Attack_State,		AI_AT_CHECK,		AI_EV_AT_CHECK_TRUE,	AI_AT_ENABLE);
	AddFsmToFunc(ai->Attack_State,		AI_AT_CHECK,		AI_EV_AT_CHECK_FALSE,	AI_AT_DISABLE);
	AddFsmToFunc(ai->Attack_State,		AI_AT_ENABLE,		AI_EV_DELAY,		AI_AT_CHECK);
	////////////////////////////////////////////////////////
	AddFsmToFunc(ai->Move_Target_State,		AI_MOVE_NONE,	AI_EV_DELAY,			AI_MOVE_PATROL);
	AddFsmToFunc(ai->Move_Target_State,		AI_MOVE_NONE,	AI_EV_MV_MON,			AI_MOVE_MON);
	AddFsmToFunc(ai->Move_Target_State,		AI_MOVE_NONE,	AI_PLAYER_EXIST,		AI_MOVE_PLAYER);
	AddFsmToFunc(ai->Move_Target_State,		AI_MOVE_PATROL,	AI_EV_MV_DONE,			AI_MOVE_NONE);
	AddFsmToFunc(ai->Move_Target_State,		AI_MOVE_PATROL,	AI_PLAYER_EXIST,		AI_MOVE_PLAYER);
	AddFsmToFunc(ai->Move_Target_State,		AI_MOVE_MON,	AI_MON_NONE,			AI_MOVE_NONE);
	AddFsmToFunc(ai->Move_Target_State,		AI_MOVE_PLAYER,	AI_PLAYER_NONE,			AI_MOVE_NONE);
	AddFsmToFunc(ai->Move_Target_State,		AI_MOVE_PLAYER, AI_PLAYER_EXIST,		AI_MOVE_PLAYER);
	
	
	AddFsmToFunc(ai->Enemy_State,		AI_PLAYER_NONE,		AI_EV_DELAY,			AI_PLAYER_SEARCH);
	AddFsmToFunc(ai->Enemy_State,		AI_PLAYER_NONE,		AI_ASSIST_ON,			AI_PLAYER_SEARCH);
	AddFsmToFunc(ai->Enemy_State,		AI_PLAYER_SEARCH,	AI_EV_PLAYER_NONE,		AI_PLAYER_NONE);
	AddFsmToFunc(ai->Enemy_State,		AI_PLAYER_SEARCH,	AI_EV_PLAYER_EXIST,		AI_PLAYER_EXIST);
	AddFsmToFunc(ai->Enemy_State,		AI_PLAYER_EXIST,	AI_ASSIST_OFF,			AI_PLAYER_SEARCH);
	AddFsmToFunc(ai->Enemy_State,		AI_PLAYER_EXIST,	AI_EV_DELAY,	AI_PLAYER_SEARCH);



	
	
	AddFsmToFunc(ai->Mon_State,			AI_MON_NONE,		AI_EV_DELAY,			AI_MON_SEARCH);
	AddFsmToFunc(ai->Mon_State,			AI_MON_SEARCH,		AI_EV_MON_NONE,			AI_MON_NONE);
	AddFsmToFunc(ai->Mon_State,			AI_MON_SEARCH,		AI_EV_MON_EXIST,		AI_MON_EXIST);
	AddFsmToFunc(ai->Mon_State,			AI_MON_EXIST,		AI_EV_DELAY,			AI_MON_SEARCH);
	
	AddFsmToFunc(ai->Act_State,			AI_ACT_NORMAL,		AI_EV_CON_BAD,			AI_ACT_RUNAWAY);
	AddFsmToFunc(ai->Act_State,			AI_ACT_RUNAWAY,		AI_EV_ACT_COP,			AI_ACT_COP);
	AddFsmToFunc(ai->Act_State,			AI_ACT_RUNAWAY,		AI_EV_CON_GOOD,			AI_ACT_NORMAL);
	AddFsmToFunc(ai->Act_State,			AI_ACT_COP,			AI_EV_DELAY,			AI_ACT_NORMAL);
	

	
	

	SendAIMessage(ai->Act_State,			AI_EV_DELAY,	1000);
	SendAIMessage(ai->Move_Target_State,	AI_EV_DELAY,	700);
	SendAIMessage(ai->Enemy_State,			AI_EV_DELAY,	1000);
	SendAIMessage(ai->Mon_State,			AI_EV_DELAY,	1000);
	SendAIMessage(ai->Attack_State,			AI_EV_DELAY,	100);
}
*/