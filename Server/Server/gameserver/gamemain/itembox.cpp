// ItemBox.cpp: implementation of the CItemBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ItemBox.h"
#include "MapData.h"
#include "Character.h"
#include "Player.h"
#include "MainThread.h"
#include "PartyPlayer.h"

int	CItemBox::s_nLiveNum = 0;
CNetIndexList CItemBox::s_listEmpty;
DWORD CItemBox::s_dwSerialCounter = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemBox::CItemBox()
{
	m_dwOwnerSerial = 0xFFFFFFFF;
	m_wOwnerIndex = 0xFFFF;
	m_dwThrowerSerial = 0xFFFFFFFF;
	m_wThrowerIndex = 0xFFFF;
	m_dwThrowerCharSerial = 0xFFFFFFFF;
	m_byThrowerID = 0xFF;
	m_pOwnerPtr = NULL;
	m_byThrowerDegree = 0xFF;

	m_dwLootStartTime = 0;
	m_nStateCode = itembox_state_normal;

	if(!s_listEmpty.IsSetting())
		s_listEmpty.SetList(MAX_ITEMBOX);
}

void CItemBox::Init(_object_id* pID)
{
	CGameObject::Init(pID);
	m_dwLastDestroyTime = 0;

	s_listEmpty.PushNode_Back(pID->m_wIndex);
}

bool CItemBox::Create(_itembox_create_setdata* pParam, bool bHide)
{
	if(CGameObject::Create((_object_create_setdata*)pParam))
	{
		m_dwObjSerial = s_dwSerialCounter++;
		m_bHide = bHide;

		if(pParam->pOwner)
		{
			m_dwOwnerSerial = pParam->pOwner->m_dwObjSerial;
			m_wOwnerIndex = pParam->pOwner->m_ObjID.m_wIndex;
			m_pOwnerPtr = ((CPlayer*)pParam->pOwner)->m_pPartyMgr;
		}
		if(pParam->pThrower)
		{
			m_dwThrowerSerial = pParam->pThrower->m_dwObjSerial;
			m_wThrowerIndex = pParam->pThrower->m_ObjID.m_wIndex;
			m_byThrowerID = pParam->pThrower->m_ObjID.m_byID;
			if(pParam->pThrower->m_ObjID.m_byKind == obj_kind_char && pParam->pThrower->m_ObjID.m_byID == obj_id_player)
			{
				m_dwThrowerCharSerial = ((CPlayer*)pParam->pThrower)->m_Param.GetCharSerial();
				m_byThrowerDegree = ((CPlayer*)pParam->pThrower)->m_byUserDgr;	//유저등급 세팅..
			}
		}

		m_byCreateCode = pParam->byCreateCode;
		memcpy(&m_Item, &pParam->Item, sizeof(__ITEM));

		m_dwLootStartTime = timeGetTime();	//timer start

		switch(m_byCreateCode)
		{
		case itembox_create_code_mon:
			m_nStateCode = itembox_state_normal;	break;
		case itembox_create_code_throw:
			m_nStateCode = itembox_state_open;		break;
		case itembox_create_code_cheat:
			m_nStateCode = itembox_state_normal;	break;
		case itembox_create_code_reward:
			m_nStateCode = itembox_state_normal;	break;
		default:
			m_nStateCode = itembox_state_normal;	break;
		}
		
		s_nLiveNum++;

		SendMsg_Create();

		return true;
	}

	return false;
}

bool CItemBox::Destroy()
{
	m_dwOwnerSerial = 0xFFFFFFFF;
	m_wOwnerIndex = 0xFFFF;
	m_dwThrowerSerial = 0xFFFFFFFF;
	m_wThrowerIndex = 0xFFFF;
	m_dwThrowerCharSerial = 0xFFFFFFFF;
	m_byThrowerID = 0xFF;
	m_byThrowerDegree = 0xFF;
	m_pOwnerPtr = NULL;

	m_dwLastDestroyTime = timeGetTime();
	SendMsg_Destroy();
	s_nLiveNum--;

	s_listEmpty.PushNode_Back(m_ObjID.m_wIndex);

	return CGameObject::Destroy();
}

void CItemBox::Loop()
{
	DWORD dwNewTick = timeGetTime();
	//상태체크
	if(m_nStateCode == itembox_state_normal)
	{
		if(m_dwLootStartTime + tm_box_owner_take < dwNewTick)
		{
			if(m_byCreateCode == itembox_create_code_cheat ||
				m_byCreateCode == itembox_create_code_reward)//치트, 보상 아이템은 바로 삭제..
			{
				Destroy();
			}
			else
			{
				m_nStateCode = itembox_state_open;
				SendMsg_StateChange();
			}
		}
	}
	else if(m_nStateCode == itembox_state_open)
	{
		if(m_dwLootStartTime + tm_box_hurry < dwNewTick)
		{
			m_nStateCode = itembox_state_hurry;
			SendMsg_StateChange();
		}
	}
	else if(m_nStateCode == itembox_state_hurry)
	{
		if(m_dwLootStartTime+tm_box_exist < dwNewTick)
		{
			Destroy();
			return;
		}
	}
}

bool CItemBox::IsTakeRight(CPlayer* pOne)
{
	if(m_nStateCode > itembox_state_normal)
		return true;

	//먹기권한있는지..
	if(m_dwOwnerSerial == pOne->m_dwObjSerial)
		return true;

	if(m_dwOwnerSerial == 0xFFFFFFFF)
	{
		return true;
	}
	else
	{
		//같은 파티원끼리는 루팅아이템을 공유
		CPartyPlayer** ppMem = pOne->m_pPartyMgr->GetPtrPartyMember();
		if(ppMem)
		{
			for(int i = 0; i < member_per_party; i++)
			{
				if(!ppMem[i])
					break;
				if(ppMem[i]->m_id.dwSerial == m_dwOwnerSerial)
					return true;
			}
		}
	}
	return false;
}

void CItemBox::SendMsg_Create()
{
	_itembox_create_zocl Send;

	Send.byItemTableCode = m_Item.m_byTableCode;
	Send.wItemRecIndex = m_pRecordSet->m_dwIndex;
	Send.byAmount = m_Item.m_dwDur;
	Send.wBoxIndex = m_ObjID.m_wIndex;
	Send.byState = (BYTE)m_nStateCode;

	Send.dwOwerSerial = m_dwOwnerSerial;
	Send.idDumber.byID = m_byThrowerID;
	Send.idDumber.wIndex = m_wThrowerIndex;
	Send.idDumber.dwSerial = m_dwThrowerSerial;

	::FloatToShort(m_fCurPos, Send.zPos, 3);

	BYTE byType[msg_header_num] = {init_msg, itembox_create_zocl};

	if(!m_bHide)
		CircleReport(byType, (char*)&Send, sizeof(Send));
	else
	{
		if(m_wOwnerIndex != 0xFFFF)
			g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_wOwnerIndex, byType, (char*)&Send, sizeof(Send));
	}
}

void CItemBox::SendMsg_Destroy()
{
	_itembox_destroy_zocl Send;
	
	Send.wIndex = m_ObjID.m_wIndex;

	BYTE byType[msg_header_num] = {init_msg, itembox_destroy_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CItemBox::SendMsg_FixPosition(int n)
{
	if(m_bHide)
		return;

	_itembox_fixpositon_zocl Send;

	Send.byItemTableCode = m_Item.m_byTableCode;
	Send.wItemRecIndex = m_pRecordSet->m_dwIndex;
	Send.byAmount = m_Item.m_dwDur;
	Send.byState = (BYTE)m_nStateCode;
	Send.wItemBoxIndex = m_ObjID.m_wIndex;
	Send.dwOwerSerial = m_dwOwnerSerial;

	::FloatToShort(m_fCurPos, Send.zPos, 3);

	BYTE byType[msg_header_num] = {position_msg, itembox_fixpositon_zocl};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(n, byType, (char*)&Send, sizeof(Send));
}

void CItemBox::SendMsg_StateChange()
{
	if(m_bHide)
		return;

	_itembox_state_change_zocl Send;

	Send.byState = m_nStateCode;
	Send.wItemBoxIndex = m_ObjID.m_wIndex;
	Send.dwOwerSerial = m_dwOwnerSerial;

	BYTE byType[msg_header_num] = {item_msg, itembox_state_change_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}


bool CreateItemBox(BYTE byTblCode, WORD wItemIdx, DWORD	dwDur, DWORD dwLv, 
				   CPlayer* pOwner, CGameObject* pThrower, BYTE byCreateCode,
				   CMapData* pMap, WORD wLayerIndex, float* pStdPos, bool bHide)
{
	CItemBox* pBox = NULL;
	DWORD dwTime = timeGetTime();
	DWORD dwNode;
	while(CItemBox::s_listEmpty.CopyFront(&dwNode))
	{
		if(g_ItemBox[dwNode].m_bLive)
		{
			CItemBox::s_listEmpty.PopNode_Front(&dwNode);
			g_Main.m_logSystemError.Write("FindEmptyItemBox() live = true (%d:item)", dwNode);
			continue;
		}

		if(dwTime - g_ItemBox[dwNode].m_dwLastDestroyTime < 60*1000)
			break;			

		pBox = &g_ItemBox[dwNode];
		break;
	}

	if(!pBox)
		return false;

	_itembox_create_setdata Data;

	Data.Item.m_byTableCode = byTblCode;
	Data.Item.m_wItemIndex = wItemIdx;
	Data.Item.m_dwDur = dwDur;
	Data.Item.m_dwLv = dwLv;
	Data.m_pRecordSet = g_Main.m_tblItemData[byTblCode].GetRecord(wItemIdx);
	if(!Data.m_pRecordSet)
		return false;

	Data.byCreateCode = byCreateCode;

	Data.pOwner = pOwner;
	Data.pThrower = pThrower;
	Data.m_pMap = pMap;
	Data.m_nLayerIndex = wLayerIndex;
	pMap->GetRandPosInRange(pStdPos, 10, Data.m_fStartPos);

	if(pBox->Create(&Data, bHide))
	{
		CItemBox::s_listEmpty.PopNode_Front(&dwNode);
		return true;
	}
	return false;
}

bool CreateItemBox(__ITEM* pItem, CPlayer* pOwner, CGameObject* pThrower, BYTE byCreateCode,
								CMapData* pMap, WORD wLayerIndex, float* pStdPos, bool bHide)
{
	return CreateItemBox(pItem->m_byTableCode, pItem->m_wItemIndex, pItem->m_dwDur, pItem->m_dwLv, 
				   pOwner, pThrower, byCreateCode,
				   pMap, wLayerIndex, pStdPos, bHide);
}

