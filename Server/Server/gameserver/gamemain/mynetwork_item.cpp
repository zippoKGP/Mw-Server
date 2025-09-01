// MyNetWork_Item.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "Player.h"
#include "MyUtil.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::ItemboxTakeRequest(int n, char* pBuf)
{
	_itembox_take_request_clzo* pRecv = (_itembox_take_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->wItemBoxIndex >= MAX_ITEMBOX)
		return false;
	
	CItemBox* pBox = &g_ItemBox[pRecv->wItemBoxIndex];
	
	pOne->pc_TakeLootingItem(pBox, pRecv->wAddSerial);

	return true;
}

bool CNetworkEX::ThrowStorageRequest(int n, char* pBuf)
{
	_throw_storage_request_clzo* pRecv =  (_throw_storage_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN && pRecv->Item.byStorageCode != _STORAGE_POS::EQUIP
		&& pRecv->Item.byStorageCode != _STORAGE_POS::EMBELLISH && pRecv->Item.byStorageCode != _STORAGE_POS::BELT)
		return false;

	pOne->pc_ThrowStorageItem(&pRecv->Item);

	return true;
}

bool CNetworkEX::UsePotionRequest(int n, char* pBuf)
{
	_use_potion_request_clzo* pRecv = (_use_potion_request_clzo*)pBuf;
	
	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN && pRecv->Item.byStorageCode != _STORAGE_POS::BELT)
		return false;
	
	pOne->pc_UsePotionItem(&pRecv->Item);
	
	return true;
}

bool CNetworkEX::EquipPartRequest(int n, char* pBuf)
{
	_equip_part_request_clzo* pRecv = (_equip_part_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)
		return false;

	pOne->pc_EquipPart(&pRecv->Item);

	return true;
}

bool CNetworkEX::EmbellishRequest(int n, char* pBuf)
{
	_embellish_request_clzo* pRecv = (_embellish_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->Item.byStorageCode != _STORAGE_POS::INVEN)
		return false;

	pOne->pc_EmbellishPart(&pRecv->Item, pRecv->wChangeSerial);

	return true;
}

bool CNetworkEX::OffPartRequest(int n, char* pBuf)
{
	_off_part_request_clzo* pRecv = (_off_part_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->Item.byStorageCode != _STORAGE_POS::EQUIP && pRecv->Item.byStorageCode != _STORAGE_POS::EMBELLISH)
		return false;

	pOne->pc_OffPart(&pRecv->Item);

	return true;
}

bool CNetworkEX::MakeItemRequest(int n, char* pBuf)
{
	_make_item_request_clzo* pRecv = (_make_item_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->byMaterialNum > _make_item_request_clzo::material_num)
		return false;

	if(pRecv->wManualIndex >= g_Main.m_tblItemMakeData.GetSize())
		return false;

	pOne->pc_MakeItem(&pRecv->ipMakeTool, pRecv->wManualIndex, pRecv->byMaterialNum, pRecv->ipMaterials);

	return true;
}

bool CNetworkEX::UpgradeItemRequest(int n, char* pBuf)
{
	_upgrade_item_request_clzo* pRecv = (_upgrade_item_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->byJewelNum > upgrade_jewel_num)
		return false;

	if(pRecv->m_posTalik.byStorageCode != _STORAGE_POS::INVEN)//탈릭은 인벤만..
		return false;

	for(int i = 0; i < pRecv->byJewelNum; i++)
	{
		if(pRecv->m_posUpgJewel[i].byStorageCode != _STORAGE_POS::INVEN)//보석 인벤만..
			return false;
	}

	if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)//도구는 인벤만..
		return false;
		
	if(pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::INVEN && pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::EQUIP)//대상은 인벤or장착..
		return false;

	pOne->pc_UpgradeItem(&pRecv->m_posTalik, &pRecv->m_posToolItem, &pRecv->m_posUpgItem, pRecv->byJewelNum, pRecv->m_posUpgJewel);

	return true;
}

bool CNetworkEX::DownGradeItemRequest(int n, char* pBuf)
{
	_downgrade_item_request_clzo* pRecv = (_downgrade_item_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	if(pRecv->m_posTalik.byStorageCode != _STORAGE_POS::INVEN)//탈릭은 인벤만..
		return false;

	if(pRecv->m_posToolItem.byStorageCode != _STORAGE_POS::INVEN)//도구는 인벤만..
		return false;
		
	if(pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::INVEN && pRecv->m_posUpgItem.byStorageCode != _STORAGE_POS::EQUIP)//대상은 인벤or장착..
		return false;
	
	pOne->pc_DowngradeItem(&pRecv->m_posTalik, &pRecv->m_posToolItem, &pRecv->m_posUpgItem);

	return true;
}

bool CNetworkEX::AddBagRequest(int n, char* pBuf)
{
	_add_bag_request_clzo* pRecv = (_add_bag_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse || pOne->m_pmGrg.GetGroggy())
		return true;

	pOne->pc_AddBag(pRecv->wBagItemSerial);

	return true;
}


