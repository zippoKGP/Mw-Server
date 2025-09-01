#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"

void CPlayer::pc_ExitSaveDataRequest(WORD wDataSize, char* psData)
{
	m_bOper = false;	//Exit Save를 요청받으면 캐릭터 작동을 멈추게 함..

	if(m_Param.m_bLineUp)
	{
		g_Main.m_logSystemError.Write("Lineup: %s >> %d번째 lineup", m_Param.GetCharName(), m_Param.m_bLineUp);
		return;
	}
	m_Param.m_bLineUp++;

	if(!m_pUserDB)
		return;

//inven 저장수[1], { serial[2], client_index[1] }
///belt 저장수[1], { serial[2], client_index[1] }
///link 저장수[1], { effectcode[1], effectindex[1], client_index[1] }
///embell 저장수[1], { serial[2], client_index[1] }

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
			g_Main.m_logSystemError.Write("Lineup: %s >> 인벤 / 없는 아이템 [인벤수: %d]", m_Param.GetCharName(), bySaveNum);
			continue;
		}

		if(byOldClientIndex != byClientIndex)		//바뀐 슬롯의 인덱스를 메모
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
			g_Main.m_logSystemError.Write("Lineup: %s >> 벨트 / 없는 아이템 [벨트수: %d]", m_Param.GetCharName(), bySaveNum);
			continue;
		}

		if(byOldClientIndex != byClientIndex)//바뀐 슬롯의 인덱스를 메모
			m_pUserDB->Update_ItemSlot(_STORAGE_POS::BELT, bySlotIndex, byClientIndex);		
	}

	////////////////
	//LINK
	m_Param.InitSFLink();//링크 초기화..
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
			g_Main.m_logSystemError.Write("Lineup: %s >> 장식 / 없는 아이템 [장식수: %d]", m_Param.GetCharName(), bySaveNum);
			continue;
		}

		if(byOldClientIndex != byClientIndex)//바뀐 슬롯의 인덱스를 메모
			m_pUserDB->Update_ItemSlot(_STORAGE_POS::EMBELLISH, bySlotIndex, byClientIndex);		
	}

	SendMsg_ExitSaveDataResult(0);
}
