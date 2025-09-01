// ParkingUnit.h: interface for the CParkingUnit class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _ParkingUnit_h_
#define _ParkingUnit_h_

#include "GameObject.h"

class CPlayer;

struct _parkingunit_create_setdata : public _object_create_setdata
{
	CPlayer*		pOwner;	//우선권자..
	BYTE			byCreateType;	//출고냐.. 하차냐..
	BYTE			byFrame;
	BYTE			byPartCode[UNIT_PART_NUM];
	BYTE			byTransDistCode;
	
	_parkingunit_create_setdata(){
		pOwner = NULL;
		byCreateType = unit_create_type_delivery;
		byTransDistCode = unit_trans_short_range;
	}
};

class CParkingUnit : public CGameObject
{
public:

	CPlayer*	m_pOwner;
	DWORD		m_dwOwnerSerial;
	BYTE		m_byFrame;
	BYTE		m_byPartCode[UNIT_PART_NUM];

	BYTE		m_byCreateType;
	BYTE		m_byTransDistCode;
	DWORD		m_dwParkingStartTime;

	DWORD		m_dwLastDestroyTime;

	static int	s_nLiveNum;

	static DWORD s_dwSerialCounter;

public:

	CParkingUnit();
	virtual ~CParkingUnit(){}

	void Init(_object_id* pID);
	bool Create(_parkingunit_create_setdata* pParam);
	bool Destroy(BYTE byDestoryType);

	void ChangeOwner(CPlayer* pNewOwner, BYTE byUnitSlotIndex);
	bool IsRideRight(CPlayer* pOne);

	virtual void Loop();

	void SendMsg_Create();
	void SendMsg_Destroy(BYTE byDestoryType);
	void SendMsg_ChangeOwner(BYTE byUnitSlotIndex, CPlayer* pOldOwner);
	virtual void SendMsg_FixPosition(int n);
};

CParkingUnit* FindEmptyParkingUnit(CParkingUnit* pItem, int nMax);



#endif 
