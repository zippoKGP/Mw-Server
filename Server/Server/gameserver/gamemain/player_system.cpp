#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"

void CPlayer::pc_ExitSaveDataRequest(WORD wDataSize, char* psData)
{
	m_bOper = false;	//Exit Save�� ��û������ ĳ���� �۵��� ���߰� ��..

	if(m_Param.m_bLineUp)
	{
		g_Main.m_logSystemError.Write("Lineup: %s >> %d��° lineup", m_Param.GetCharName(), m_Param.m_bLineUp);
		return;
	}
	m_Param.m_bLineUp++;

	if(!m_pUserDB)
		return;

//inven �����[1], { serial[2], client_index[1] }
///belt �����[1], { serial[2], client_index[1] }
///link �����[1], { effectcode[1], effectindex[1], client_index[1] }
///embell �����[1], { serial[2], client_index[1] }

	int nOffset = 0;
	WORD wSerial;
	BYTE bySaveNum, byClientIndex;

	///////////////
	//INVEN
	memcpy(&bySaveNum, &psData[nOffset], sizeof(bySaveNum));
	nOffset += sizeof(bySaveNum);

	for(int i = 0; i < bySaveNum; i++)
	{
		memcpy(&wSerial, &psData[nOffset], sizeof(wSerial));
		nOffset += sizeof(wSerial);
		memcpy(&byClientIndex, &psData[nOffset], sizeof(byClientIndex));
		nOffset += sizeof(byClientIndex);

		BYTE byOldClientIndex;
		BYTE bySlotIndex = m_Param.m_dbInven.SetClientIndexFromSerial(wSerial, byClientIndex, &byOldClientIndex);
		if(bySlotIndex == __NSI)
		{
			g_Main.m_logSystemError.Write("Lineup: %s >> �κ� / ���� ������ [�κ���: %d]", m_Param.GetCharName(), bySaveNum);
			continue;
		}

		if(byOldClientIndex != byClientIndex)		//�ٲ� ������ �ε����� �޸�
			m_pUserDB->Update_ItemSlot(_STORAGE_POS::INVEN, bySlotIndex, byClientIndex);		
	}

	/////////////
	//BELT
	memcpy(&bySaveNum, &psData[nOffset], sizeof(bySaveNum));
	nOffset += sizeof(bySaveNum);

	for(i = 0; i < bySaveNum; i++)
	{
		memcpy(&wSerial, &psData[nOffset], sizeof(wSerial));
		nOffset += sizeof(wSerial);
		memcpy(&byClientIndex, &psData[nOffset], sizeof(byClientIndex));
		nOffset += sizeof(byClientIndex);

		BYTE byOldClientIndex;
		BYTE bySlotIndex = m_Param.m_dbBelt.SetClientIndexFromSerial(wSerial, byClientIndex, &byOldClientIndex);
		if(bySlotIndex == __NSI)
		{
			g_Main.m_logSystemError.Write("Lineup: %s >> ��Ʈ / ���� ������ [��Ʈ��: %d]", m_Param.GetCharName(), bySaveNum);
			continue;
		}

		if(byOldClientIndex != byClientIndex)//�ٲ� ������ �ε����� �޸�
			m_pUserDB->Update_ItemSlot(_STORAGE_POS::BELT, bySlotIndex, byClientIndex);		
	}

	////////////////
	//LINK
	m_Param.InitSFLink();//��ũ �ʱ�ȭ..
	memcpy(&bySaveNum, &psData[nOffset], sizeof(bySaveNum));
	nOffset += sizeof(bySaveNum);

	for(i = 0; i < bySaveNum; i++)
	{
		_SF_LINK* pLink = &m_Param.m_SFLink[i];

		memcpy(&pLink->m_byEffectCode, &psData[nOffset], sizeof(BYTE));
		nOffset += sizeof(BYTE);
		memcpy(&pLink->m_wEffectIndex, &psData[nOffset], sizeof(BYTE));
		nOffset += sizeof(BYTE);
		memcpy(&pLink->m_sClientIndex, &psData[nOffset], sizeof(BYTE));
		nOffset += sizeof(BYTE);

		pLink->m_bLoad = true;
	}

	////////////////
	//EMBELL
	memcpy(&bySaveNum, &psData[nOffset], sizeof(bySaveNum));
	nOffset += sizeof(bySaveNum);

	for(i = 0; i < bySaveNum; i++)
	{
		memcpy(&wSerial, &psData[nOffset], sizeof(wSerial));
		nOffset += sizeof(wSerial);
		memcpy(&byClientIndex, &psData[nOffset], sizeof(byClientIndex));
		nOffset += sizeof(byClientIndex);

		BYTE byOldClientIndex;
		BYTE bySlotIndex = m_Param.m_dbEmbellish.SetClientIndexFromSerial(wSerial, byClientIndex, &byOldClientIndex);
		if(bySlotIndex == __NSI)
		{
			g_Main.m_logSystemError.Write("Lineup: %s >> ��� / ���� ������ [��ļ�: %d]", m_Param.GetCharName(), bySaveNum);
			continue;
		}

		if(byOldClientIndex != byClientIndex)//�ٲ� ������ �ε����� �޸�
			m_pUserDB->Update_ItemSlot(_STORAGE_POS::EMBELLISH, bySlotIndex, byClientIndex);		
	}

	SendMsg_ExitSaveDataResult(0);
}
