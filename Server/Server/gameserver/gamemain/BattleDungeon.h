#ifndef _BattleDungeon_h_
#define _BattleDungeon_h_

#include "GameObject.h"
#include "NetUtil.h"

class CPlayer;
class CMonster;

struct _battledungeon_create_setdata : public _object_create_setdata
{
	CPlayer*	pOpener;		
	CMapData*	pTarMap;
	WORD		wTarLayerIndex;
};

struct _ENTER_DUNGEON_NEW_POS	
{
	BYTE		byMapCode;
	WORD		wLayerIndex;
	float		fPos[3];
};

class CBattleDungeon : public CGameObject
{
public:

	enum		{	enter_quf_free, enter_guf_party	};	
	enum		{	max_player = 16, max_monster = 500	};	
	enum		{	tm_gate_exist = 30000, tm_gate_hurry = 20000, tm_destroy_able = 40000	};
	enum		{	end_case_all_mon_die = 1, end_case_specific_mon_die = 2	};

	char			m_szOpenerName[max_name_len+1];
	CMapData*		m_pTarMap;
	WORD			m_wTarLayer;
	_LAYER_SET*		m_pLayerSet;
	DWORD			m_dwOpenerSerial;

	DWORD			m_dwOpenTime;
	int				m_nStateCode;
	DWORD			m_dwLastDestroyTime;
	DWORD			m_dwTerminateTime;
	CMyTimer		m_tmrCheckActive;

	bool			m_bOpen;
	CNetIndexList	m_listActiveMonster;
	CNetIndexList	m_listActivePlayer;
	int				m_nMissionKillMonNum;
	int				m_nTotalKillMonNum;

	static int		s_nLiveNum;

	static DWORD	s_dwSerialCounter;

public:

	CBattleDungeon();
	virtual ~CBattleDungeon(){}

	void Init(_object_id* pID);
	bool Create(_battledungeon_create_setdata* pParam);
	void GateClose();
	bool Destroy(BYTE byDestroyCode);
	void CreateMob(float fAddRate);

	bool EnterPlayer(CPlayer* pPlayer);

	virtual void Loop();

	bool IsEnterAblePlayer(CPlayer* p);
	bool GetEnterNewPos(_ENTER_DUNGEON_NEW_POS* OUT pPosData);
	int  GetLeftTimeoutSec();

	void SendMsg_Create();
	void SendMsg_Destroy();
	void SendMsg_StateChange();
	void SendMsg_KillMonNum();
	virtual void SendMsg_FixPosition(int n);
};

CBattleDungeon* FindEmptyDungeonGate(CBattleDungeon* pDungeonGate, int nMax);
CBattleDungeon* FindDungeonFromLayer(CBattleDungeon* pDungeonGate, int nMax, CMapData* pMap, int nLayerIndex);

#endif