#ifndef _NPC_H_
#define _NPC_H_

#include "Character.h"
#include "NPCharacterFld.h"

class CItemStore;

struct _npc_create_setdata : public _character_create_setdata
{
	CItemStore*	m_pLinkItemStore; //상점 npc인경우 링크돼는 상점포인터

	_npc_create_setdata() {
		m_pLinkItemStore = NULL;
	}
};  

class CNPC : public CCharacter
{
public:

	CItemStore*		m_pItemStore;	

	DWORD			m_dwLastDestroyTime;

	static int		s_nLiveNum;
	static DWORD    s_dwSerialCnt;

public:

	CNPC();
	~CNPC();

	bool Init(_object_id* pID);
	bool Create(_npc_create_setdata* pData);
	bool Destroy(CGameObject* pAttObj);

	virtual void Loop();

	virtual	int GetObjRace() { return obj_race_npc;		}
	virtual int	GetFireTol() { return ((_npc_fld*)m_pRecordSet)->m_fFireTol;}
	virtual int	GetWaterTol(){ return ((_npc_fld*)m_pRecordSet)->m_fWaterTol; }
	virtual int	GetSoilTol() { return ((_npc_fld*)m_pRecordSet)->m_fSoilTol; }
	virtual int	GetWindTol() { return ((_npc_fld*)m_pRecordSet)->m_fWindTol; }

	static DWORD GetNewMonSerial(){
		return s_dwSerialCnt++;
	}

	void SendMsg_Create();
	void SendMsg_Destroy();
	void SendMsg_Move();	//이동명령을 받으면 주위의 Player에게 Move Message를 Send
	virtual void SendMsg_FixPosition(int n);	//움직이지 않을때 주위의 Player에게 실시간으로 Fix Position Message를 Send
	virtual void SendMsg_RealMovePoint(int n);	//특정인한테..
};

CNPC* FindEmptyNPC(CNPC* pList, int nMax);

#endif