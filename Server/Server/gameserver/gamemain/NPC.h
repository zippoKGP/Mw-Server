#ifndef _NPC_H_
#define _NPC_H_

#include "Character.h"
#include "NPCharacterFld.h"

class CItemStore;

struct _npc_create_setdata : public _character_create_setdata
{
	CItemStore*	m_pLinkItemStore; //���� npc�ΰ�� ��ũ�Ŵ� ����������

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
	void SendMsg_Move();	//�̵������ ������ ������ Player���� Move Message�� Send
	virtual void SendMsg_FixPosition(int n);	//�������� ������ ������ Player���� �ǽð����� Fix Position Message�� Send
	virtual void SendMsg_RealMovePoint(int n);	//Ư��������..
};

CNPC* FindEmptyNPC(CNPC* pList, int nMax);

#endif