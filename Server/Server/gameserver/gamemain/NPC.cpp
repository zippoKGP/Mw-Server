#include "stdafx.h"
#include "NPC.h"
#include "MainThread.h"
#include "pt_zone_client.h"

int	CNPC::s_nLiveNum = 0;
DWORD CNPC::s_dwSerialCnt = 0;

CNPC::CNPC()
{
}

CNPC::~CNPC()
{
}

bool CNPC::Init(_object_id* pID)
{
	CCharacter::Init(pID);	

	m_pItemStore = NULL;
	m_dwLastDestroyTime = 0;

	return true;
}

bool CNPC::Create(_npc_create_setdata* pData)
{
	DWORD dwSt = timeGetTime();

	if(CCharacter::Create((_character_create_setdata*)pData))
	{
		m_pItemStore = pData->m_pLinkItemStore;

		SendMsg_Create();

		s_nLiveNum++;

		return true;
	}

	return false;
}

bool CNPC::Destroy(CGameObject* pAttObj)
{
	m_dwLastDestroyTime = timeGetTime();

	SendMsg_Destroy();

	CCharacter::Destroy();

	s_nLiveNum--;

	return true;
}

void CNPC::Loop()
{
	CCharacter::UpdateSFCont();
}

void CNPC::SendMsg_Create()
{
	_npc_create_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zPos, 3);

	BYTE byType[msg_header_num] = {init_msg, npc_create_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CNPC::SendMsg_Destroy()
{
	_npc_destroy_zocl Send;

	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;

	BYTE byType[msg_header_num] = {init_msg, npc_destroy_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CNPC::SendMsg_Move()
{
	_npc_move_zocl Send;

	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);
	Send.zTar[0] = (short)m_fTarPos[0];
	Send.zTar[1] = (short)m_fTarPos[2];
	Send.wLastEffectCode = m_wLastContEffect;

	BYTE byType[msg_header_num] = {position_msg, npc_move_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CNPC::SendMsg_FixPosition(int n)
{
	_npc_fixpositon_zocl Send;

	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);	
	Send.wLastEffectCode = m_wLastContEffect;

	BYTE byType[msg_header_num] = {position_msg, npc_fixpositon_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));	
}

void CNPC::SendMsg_RealMovePoint(int n)
{
	_npc_real_move_zocl Send;

	Send.wRecIndex = (WORD)m_pRecordSet->m_dwIndex;
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);
	Send.zTar[0] = m_fTarPos[0];
	Send.zTar[1] = m_fTarPos[2];
	Send.wLastEffectCode = m_wLastContEffect;

	BYTE byType[msg_header_num] = {position_msg, npc_real_move_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));
}

CNPC* FindEmptyNPC(CNPC* pList, int nMax)
{
	DWORD dwTime = timeGetTime();

	for(int i = 0; i < nMax; i++)
	{
		if(pList[i].m_bLive)
			continue;

		if(dwTime - pList[i].m_dwLastDestroyTime <= 60*1000)
			continue;

		return &pList[i];		
	}

	return NULL;	
}

