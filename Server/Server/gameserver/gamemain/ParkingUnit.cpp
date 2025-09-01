// ParkingUnit.cpp: implementation of the CParkingUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ParkingUnit.h"
#include "Player.h"
#include "MainThread.h"

int	CParkingUnit::s_nLiveNum = 0;

DWORD CParkingUnit::s_dwSerialCounter = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParkingUnit::CParkingUnit()
{
	m_pOwner = NULL;
	m_dwOwnerSerial = 0xFFFFFFFF;

	m_dwParkingStartTime = 0;
}

void CParkingUnit::Init(_object_id* pID)
{
	CGameObject::Init(pID);
	m_dwLastDestroyTime = 0;
}

bool CParkingUnit::Create(_parkingunit_create_setdata* pParam)
{
	if(CGameObject::Create((_object_create_setdata*)pParam))
	{
		m_dwObjSerial = s_dwSerialCounter++;

		m_pOwner = pParam->pOwner;
		m_dwOwnerSerial = pParam->pOwner->m_dwObjSerial;

		m_byFrame = pParam->byFrame;
		memcpy(m_byPartCode, pParam->byPartCode, sizeof(BYTE)*UNIT_PART_NUM);

		m_byCreateType = pParam->byCreateType;
		m_byTransDistCode = pParam->byTransDistCode;
		m_dwParkingStartTime = timeGetTime();

		s_nLiveNum++;

		SendMsg_Create();
		return true;
	}

	return false;
}

bool CParkingUnit::Destroy(BYTE byDestoryType)
{
	m_pOwner = NULL;
	m_dwOwnerSerial = 0xFFFFFFFF;
	m_dwParkingStartTime = 0;

	m_dwLastDestroyTime = timeGetTime();
	SendMsg_Destroy(byDestoryType);
	s_nLiveNum--;

	return CGameObject::Destroy();
}

void CParkingUnit::Loop()
{
//	DWORD dwNewTick = timeGetTime();
}

void CParkingUnit::ChangeOwner(CPlayer* pNewOwner, BYTE byUnitSlotIndex)
{
	CPlayer* pOldOwner = m_pOwner;
	m_pOwner = pNewOwner;
	m_dwOwnerSerial = pNewOwner->m_dwObjSerial;

	SendMsg_ChangeOwner(byUnitSlotIndex, pOldOwner);
}

bool CParkingUnit::IsRideRight(CPlayer* pOne)
{
	return false;
}

void CParkingUnit::SendMsg_Create()
{
	_parkingunit_create_zocl Send;

	Send.wObjIndex = m_ObjID.m_wIndex;
	Send.dwObjSerial = m_dwObjSerial;
	Send.byCreateType = m_byCreateType;
	Send.byFrame = m_byFrame;
	memcpy(Send.byPart, m_byPartCode, sizeof(BYTE)*UNIT_PART_NUM);
	Send.dwOwerSerial = m_dwOwnerSerial;
	::FloatToShort(m_fCurPos, Send.zPos, 3);
	Send.byTransDistCode = m_byTransDistCode;

	BYTE byType[msg_header_num] = {init_msg, parkingunit_create_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CParkingUnit::SendMsg_Destroy(BYTE byDestoryType)
{
	_parkingunit_destroy_zocl Send;
	
	Send.wObjIndex = m_ObjID.m_wIndex;
	Send.dwObjSerial = m_dwObjSerial;
	Send.byDestroyCode = byDestoryType;

	BYTE byType[msg_header_num] = {init_msg, parkingunit_destroy_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CParkingUnit::SendMsg_ChangeOwner(BYTE byUnitSlotIndex, CPlayer* pOldOwner)
{
	_parkingunit_change_owner_zocl Send;

	Send.wObjIndex = m_ObjID.m_wIndex;
	Send.dwObjSerial = m_dwObjSerial;
	Send.dwOldOwnerSerial = pOldOwner->m_dwObjSerial;
	Send.dwNewOwnerSerial = m_pOwner->m_dwObjSerial;
	Send.byNewOwnerUnitSlotIndex = byUnitSlotIndex;

	BYTE byType[msg_header_num] = {position_msg, parkingunit_fixpositon_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_pOwner->m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(pOldOwner->m_ObjID.m_wIndex, byType, (char*)&Send, sizeof(Send));
}

void CParkingUnit::SendMsg_FixPosition(int n)
{
	_parkingunit_fixpositon_zocl Send;

	Send.wObjIndex = m_ObjID.m_wIndex;
	Send.dwObjSerial = m_dwObjSerial;
	Send.byFrame = m_byFrame;
	memcpy(Send.byPart, m_byPartCode, sizeof(BYTE)*UNIT_PART_NUM);
	::FloatToShort(m_fCurPos, Send.zPos, 3);

	BYTE byType[msg_header_num] = {position_msg, parkingunit_fixpositon_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));
}

CParkingUnit* FindEmptyParkingUnit(CParkingUnit* pItem, int nMax)
{
	DWORD dwTime = timeGetTime();
	for(int i = 0; i < nMax; i++)
	{
		if(pItem[i].m_bLive)
			continue;

		if(dwTime-pItem[i].m_dwLastDestroyTime <= 60*1000)
			continue;

		return &pItem[i];			
	}	
	return NULL;
}

