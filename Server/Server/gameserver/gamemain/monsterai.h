#ifndef	__MONSTERAI__
#define __MONSTERAI__





#include "Attack.h"
#include "AIMessage.h"




using namespace std;

typedef 		unsigned int	HFSM;

//

class	FSM;
/*
HFSM	CreateFSM();
FSM	*	GetFSM(HFSM hfsm);
bool 	DeleteFSM(HFSM hfsm);
bool	AddFsmToFunc(HFSM fsm,WORD state,WORD in,WORD out);
*/
void	SendAIMessage(FSM &hfsm,DWORD p1,DWORD delay);
void	SendAIMessage(FSM &fsm,DWORD p1,DWORD *delay=NULL);
//*/
//void	SendAIMessage(FSM &fsm,DWORD p1,DWORD *delay=NULL);
//
class FSMStateNInput{
public:
	
	DWORD	m_State;
	DWORD	m_Input;

	void	Set(WORD state,DWORD input)
	{
		m_State = state; m_Input =input;
	}
	FSMStateNInput ()
	{
		m_State = m_Input = 0;
	}
	FSMStateNInput (DWORD state,DWORD input)
	{
		m_State = state;
		m_Input = input;
	}
	
	friend bool operator <(const FSMStateNInput&a,const FSMStateNInput&b)
	{
		if (a.m_State<b.m_State )
		{
			return true;
		}
		else if (a.m_State==b.m_State )
		{
			if (  a.m_Input<b.m_Input)	return true;
			
		}
		return false;

	}
};
typedef map<FSMStateNInput,WORD> MAP;
typedef MAP::value_type valtype;
typedef MAP::iterator	isMapItor;

//typedef list<HFSM>	HFSMLIST;
//typedef list<FSM*>	HFSMLIST;
//typedef HFSMLIST::iterator	isHFSMListItor;


typedef void (*AIMSGFUNC)(void* object,DWORD state);

#define FUN_WIDTH 128
class PFSM {
public:
	void	*	m_pObject;// 상태기계의 소유 Object
	FSM*		m_hParentList;
	BYTE		m_Func[FUN_WIDTH*FUN_WIDTH];
	DWORD		m_State;
	DWORD		m_Time;
	DWORD		m_ElapseTime;// 경과시간 
	AIMSGFUNC	m_MsgFunc;

	FSMStateNInput sni;
	void FSMInit()
	{
		m_pObject	= NULL;
		//m_hParentList.clear();
		m_hParentList= NULL;
		memset(m_Func,0,sizeof(m_Func));
		//m_Func.clear();
		m_State		= 0;
		m_Time		= 0;
		m_ElapseTime= 500000;
		m_MsgFunc	= NULL;
		m_hParentList = NULL;
	}
	PFSM()
	{
		FSMInit();
	}
	void Add(WORD state,WORD in,WORD out)
	{
		if(!m_State)m_State = state;
		//FSMStateNInput sni(state,in);
		//m_Func.insert(valtype(sni,out));
		m_Func[state*FUN_WIDTH+in] = out;
		
	}
};
class FSM {
public:
	void	*	m_pObject;// 상태기계의 소유 Object
	FSM*		m_hParentList;
	//MAP			m_Func;
	BYTE		*m_Func;
	DWORD		m_State;
	DWORD		m_Time;
	DWORD		m_ElapseTime;// 경과시간 
	AIMSGFUNC	m_MsgFunc;

	FSMStateNInput sni;
	void FSMInit()
	{
		m_pObject	= NULL;
		//m_hParentList.clear();
		m_hParentList= NULL;

		m_Func = NULL;
		//memset(m_Func,0,sizeof(m_Func));
		//m_Func.clear();
		m_State		= 0;
		m_Time		= 0;
		m_ElapseTime= 500000;
		m_MsgFunc	= NULL;
		m_hParentList = NULL;

	}
	FSM()
	{
		FSMInit();
	}
	void SetFSMParent(FSM &parent)
	{
		m_hParentList = &parent;
		//m_hParentList.push_back(&parent);
	}
	void SetMsgFunc(void *object,AIMSGFUNC func)
	{
		m_pObject	=	object;
		m_MsgFunc	= 	func;
	}
	void Add(WORD state,WORD in,WORD out)
	{
		if(!m_State)m_State = state;
		//FSMStateNInput sni(state,in);
		//m_Func.insert(valtype(sni,out));
		m_Func[state*FUN_WIDTH+in] = out;
		
	}
	bool Get(DWORD in);
	DWORD GetCurrent()
	{
		return m_State;
	}
	void Process( DWORD time );// 딜레이 메세지 처리를 위해서 필요하다
};



typedef map<DWORD,FSM> FSMMAP;
typedef FSMMAP::value_type	fsmvaltype;
typedef FSMMAP::iterator	fsmisMapItor;

class FSMMANAGER
{
	FSMMAP	m_Map;
	DWORD   m_Identiy;
public:
	FSMMANAGER (){m_Identiy = 0;}

	inline void SetParent(HFSM child, HFSM parent)
	{
		//GetFSM(child)->SetFSMParent(parent);	
	}
	
	inline HFSM	CreateFSM()
	{
		FSM	f;
		m_Identiy++;
		m_Map.insert(fsmvaltype(m_Identiy,f));
		return m_Identiy;
	}
	/*
	inline HFSM	CreateChildFSM(HFSM	parent)
	{
		FSM	f;
		f.SetFSMParent(parent);
		m_Identiy++;
		m_Map.insert(fsmvaltype(m_Identiy,f));
		return m_Identiy;
	}
	*/
	inline FSM	*	GetFSM(HFSM hfsm)
	{
		fsmisMapItor i = m_Map.find(hfsm);
		if(i != m_Map.end() ) 
		{
			return &((*i).second);
		}
		return  NULL;
	}
	inline bool 	DeleteFSM(HFSM hfsm)
	{
		fsmisMapItor i = m_Map.find(hfsm);
		if(i != m_Map.end() ) 
		{
			m_Map.erase(i);
			return true;
		}
		return false;
	}

};


struct GMSG {
	DWORD	m_Active;
	HFSM	m_hFsm;
	DWORD	m_Param1;
//	DWORD	m_Param2;
	DWORD	m_Delay;
	GMSG()
	{
		m_Active = false;
		m_hFsm= m_Param1 = 0;
	}
	void SetMsg(HFSM h,DWORD p1,DWORD d)
	{
		m_Active= true;
		m_hFsm = h; m_Param1 = p1;m_Delay = d;
	}
	inline GMSG(HFSM h,DWORD p1,DWORD d)
	{
		m_Active= true;
		m_hFsm = h; m_Param1 = p1;m_Delay = d;
	}
	friend bool operator <(const GMSG&a,const GMSG&b)
	{
		if (a.m_Delay<b.m_Delay)
		{
			return true;
		}
		else
			return false;

	}

};

//typedef list<GMSG>	LIST;
typedef vector<GMSG>	LIST;
typedef LIST::iterator	isListItor;
/*
class ROUTER {
public:
	enum {	max_msg = 20000};
	LIST	m_MsgList;
	//GMSG	m_Msg[max_msg];
	DWORD	m_First;
	DWORD	m_Last;
	DWORD	m_CurTime;
	DWORD	m_BefTime;
	ROUTER(){
		m_First =0;
		m_Last	=0;	
		m_CurTime	= m_BefTime= 0;
	}

	inline void	SendAIMessage(HFSM hfsm,DWORD p1,DWORD delay= 0)
	{
		
		GMSG msg(hfsm,p1,m_CurTime+delay);
		m_MsgList.push_back(msg);	
		
		//m_Last++;
		//m_Msg[m_Last++].SetMsg(hfsm,p1,m_CurTime+delay);
		
		//m_Msg[m_Last++].SetMsg(hfsm,p1,m_CurTime+delay);
		//if (m_Last>=max_msg)m_Last = 0;
	}	
	
	void	Process();	
	void	Sort();// 시간의 순서로 정렬한다
	
};
*/
///////////////////////////////////////////////////////////////////////////////
// 몬스터 AI 설정 
/*
struct MONSTERAI
{
	
	HFSM Prime_State;
	HFSM Move_Target_State;
	HFSM Attack_State;
	////////////////////////////////////////////////////////
	HFSM Player_State;
	HFSM Mon_State;
	HFSM Con_State;
	HFSM Emotion_State;
	////////////////////////////////////////////////////////
	HFSM Attack_Delay_State;
	HFSM Personal_State;
	HFSM Help_State;
	HFSM Assist_State;
};
*/


struct PMONSTERAI
{
	
	PFSM Prime_State;
	PFSM Move_Target_State;
	PFSM Attack_State;
	////////////////////////////////////////////////////////
	PFSM Player_State;
	PFSM Mon_State;
	PFSM Act_State;
	PFSM Con_State;
	PFSM Emotion_State;
	////////////////////////////////////////////////////////

	PFSM Personal_State;
	PFSM Help_State;
	PFSM Assist_State;
};

struct MONSTERAI
{
	
	FSM Prime_State;
	FSM Move_Target_State;
	FSM Attack_State;
	////////////////////////////////////////////////////////
	FSM Player_State;
	FSM Mon_State;
	FSM Act_State;
	FSM Con_State;
	FSM Emotion_State;
	////////////////////////////////////////////////////////

	FSM Personal_State;
	FSM Help_State;
	FSM Assist_State;
};

struct RECALLMONAI
{
	FSM Prime_State;
	FSM Move_Target_State;
	FSM Attack_State;
	////////////////////////////////////////////////////////
	FSM Enemy_State;
	FSM Mon_State;
	FSM Act_State;
	////////////////////////////////////////////////////////
};

struct SKILL {
	enum { FIREEL , WATEREL , SOILEL , WINDEL , NONE = -1 };
		int			m_Type;
		int			m_Element;
		int			m_MinDmg;// 최소구간  최소 공격력
		int			m_StdDmg;// 평균 공격력 
		int			m_MaxDmg;// 최대구간  최대 공격력
		int			m_CritDmg;// 크리티컬 공격력 
		int			m_MinProb;
		int			m_MaxProb;
		BOOL		m_IsCritical;

		_attack_param	m_param;

		DWORD m_Len;
		DWORD m_CastDelay;
		DWORD m_Delay;
public:
		DWORD m_bLoad;
		DWORD m_Active;
		DWORD m_BefTime;
		void Reset()
		{
			memset(this,0,sizeof(SKILL));
			m_Active = false;
		}
		void Init(int type,int dmg,int minprob,int maxprob,int len,int castdelay,int delay,int el=NONE	)
		{
			m_bLoad		= true;
			m_Type		= 0;
			m_Element	= el;
			m_StdDmg	= dmg;
			m_IsCritical= false;
			
			int halfstd		= m_StdDmg*((float)3.0f/4);	//ogogo..
			if ( (m_StdDmg-halfstd) )
			{
				m_MinDmg = halfstd + ::rand() % (m_StdDmg-halfstd);
			}
			else
				m_MinDmg = 0;
			m_MaxDmg = m_StdDmg * 2 - m_MinDmg;
			float fdmg = (float)(125 + m_MaxDmg) / (float)( 50 + m_MaxDmg);
			m_CritDmg	= int((float)m_MaxDmg*fdmg + 0.5f);
			//m_CritDmg	= int((float)m_MaxDmg*((125 + m_MaxDmg) / ( 50 + m_MaxDmg)) + 0.5f);

			//
			m_MinProb =	minprob;
			m_MaxProb =	maxprob;
			//
			m_Len		= len;
			m_CastDelay	= castdelay;// 주문동작시간 
			m_Delay		= delay;	// 얼마나 자주 사용할것인가?
			m_Active	= 1;
			m_BefTime	= 0;
		}
		void SKILL::InitForce(int level,int mastery,char *forceindex,int forcestdatt,int minprob,int maxprob,int castdelay);
		
		int	 GetDmg()
		{
			m_IsCritical = false;
			if ((m_StdDmg-m_MinDmg)==0 || (m_MaxDmg-m_StdDmg)==0 ) return 0;
			float r		=::rand()%100;
			float r2	= 0;
			if (r<=m_MinProb)
			{
				r2 =  ::rand()%(m_StdDmg-m_MinDmg);
				return m_MinDmg + r2;
			}
			else if (r<=(m_MaxProb+m_MinProb))
			{
				r2 =  ::rand()%(m_MaxDmg-m_StdDmg);
				return m_StdDmg + r2;
			}
			else 
			{
				m_IsCritical = true;
				return m_CritDmg;
			}
		}
};
#define MAX_SKILL	3


//extern ROUTER g_Router;
//void	SetParent(HFSM child, HFSM parent);
void	SetAIMsgFunc(FSM	&hfsm,void *pobj,AIMSGFUNC	func);
bool	AddFsmToFunc(FSM &fsm,WORD state,WORD in,WORD out);
//FSM	*	GetFSM(HFSM hfsm);
//bool 	DeleteFSM(HFSM hfsm);

//DWORD	GetCurrentAIState(HFSM hfsm);
class CMonster;
class CRecallMon;
void SetMonterAI(CMonster	*pMonster);
void SetRecallMonAI(CRecallMon	*pMonster);
void GeCurrentStateString(char *str,DWORD state);

struct EMOTYPE
{
	DWORD	m_Normal;
	DWORD	m_Discomport;
	DWORD	m_Anger;
	DWORD	m_Fury;
	DWORD	m_Mad;
};

class		CAITimer 
{
public:
	DWORD	m_BefTime;
	DWORD	m_Delay;
	DWORD	m_DDelay;

	CAITimer()
	{
		m_BefTime	= 0;
		m_Delay		= 0;
		m_DDelay	= 0;
	}
	void	Init (DWORD delay) { m_DDelay = m_Delay = delay;}
	void	Set  ( DWORD delay  = 0 ) 
	{
			if ( delay ) m_Delay = delay;
			else m_Delay = m_DDelay;
			m_BefTime = timeGetTime(); 
	}
	BOOL	Check()
	{
		
		if ( m_Delay )
		{
			DWORD curtime = timeGetTime();

			
			DWORD delta =  curtime - m_BefTime;
			if ( m_Delay < delta )
			{
				m_Delay		= m_DDelay;
				m_BefTime	= curtime;
				return true;
			}
		}
		return false;
	}
};

class CLevel;//ogogo..

class CAIPathFinder
{
public:
	enum	{  max_path = 16};

	float*		m_pTarget;
	float		m_MovePos[max_path][3];//
	DWORD		m_CurNode;
	DWORD		m_EndNode;
	BOOL		m_bNode;//
	CLevel*		m_pLevel;
	
	void Init(CLevel *level)
	{
		m_pTarget	= NULL;
		m_CurNode	= 0;
		m_EndNode	= 0;
		m_bNode		= 0;
		m_pLevel	= level;
	}
	float* CAIPathFinder::GetPath( Vector3f &cur, Vector3f &tar );
};
extern EMOTYPE EmotionType[10];
#endif