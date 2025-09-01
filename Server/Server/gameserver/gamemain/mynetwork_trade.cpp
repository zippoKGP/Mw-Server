// MyNetWork_Trade.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "MainThread.h"

bool CNetworkEX::BuyStoreRequest(int n, char* pBuf)
{
	_buy_store_request_clzo* pRecv = (_buy_store_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->byStoreIndex >= CMapData::s_tbItemStore.GetRecordNum())
		return false;

	if(pRecv->byBuyNum > trade_item_max_num)
		return false;

	if(!pOne->m_pCurMap)
		return false;
	
	CItemStore* pStore = NULL;
	for(int i = 0; i < pOne->m_pCurMap->m_nItemStoreDumNum; i++)
	{
		if(pOne->m_pCurMap->m_ItemStore[i].m_pRec->m_dwIndex == pRecv->byStoreIndex)
		{
			pStore = &pOne->m_pCurMap->m_ItemStore[i];
			break;
		}
	}
	if(!pStore)
		return false;


	for(i = 0; i < pRecv->byBuyNum; i++)
	{
		if(pRecv->OfferList[i].dwGoodSerial >= pStore->m_nStorageItemNum)
			return false;

		if(pRecv->OfferList[i].byAmount < 1 || pRecv->OfferList[i].byAmount > 99)
			return false;

		if(pRecv->OfferList[i].byStorageCode >= _STORAGE_POS::STORAGE_NUM)
			return false;
	}
	
	pOne->pc_BuyItemStore(pStore, pRecv->byBuyNum, pRecv->OfferList);

	return true;
}

bool CNetworkEX::SellStoreRequest(int n, char* pBuf)
{
	_sell_store_request_clzo* pRecv = (_sell_store_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->wStoreIndex >= CMapData::s_tbItemStore.GetRecordNum())
	{
		m_LogFile.Write("odd.. %s: SellStoreRequest() : pRecv->wStoreIndex(%d) >= CMapData::s_tbItemStore.GetRecordNum()(%d)",
			pOne->m_Param.GetCharName(), pRecv->wStoreIndex, CMapData::s_tbItemStore.GetRecordNum());				
		return false;
	}

	if(pRecv->bySellNum > trade_item_max_num)
	{
		m_LogFile.Write("odd.. %s: SellStoreRequest() : pRecv->bySellNum(%d) >= trade_item_max_num(%d)",
			pOne->m_Param.GetCharName(), pRecv->bySellNum, trade_item_max_num);				
		return false;
	}

	for(int i = 0; i < pRecv->bySellNum; i++)
	{
		if(pRecv->Item[i].byAmount < 1 || pRecv->Item[i].byAmount > 99)
		{
			m_LogFile.Write("odd.. %s: SellStoreRequest() : pRecv->Item[i].byAmount (%d)",
				pOne->m_Param.GetCharName(), pRecv->Item[i].byAmount);				
			return false;	
		}

		if(pRecv->Item[i].byStorageCode >= _STORAGE_POS::STORAGE_NUM)
		{
			m_LogFile.Write("odd.. %s: SellStoreRequest() : pRecv->Item[i].byStorageCode (%d)",
				pOne->m_Param.GetCharName(), pRecv->Item[i].byStorageCode);				
			return false;	
		}
	}
	
	if(!pOne->m_pCurMap)
		return true;
	
	CItemStore* pStore = NULL;
	for(i = 0; i < pOne->m_pCurMap->m_nItemStoreDumNum; i++)
	{
		if(pOne->m_pCurMap->m_ItemStore[i].m_pRec->m_dwIndex == pRecv->wStoreIndex)
		{
			pStore = &pOne->m_pCurMap->m_ItemStore[i];
			break;
		}
	}
	if(!pStore)
		return true;	

	pOne->pc_SellItemStore(pStore, pRecv->bySellNum, pRecv->Item);

	return true;
}

bool CNetworkEX::StoreListRequest(int n, char* pBuf)
{
	CPlayer* pOne = &g_Player[n];

//	if(!pOne->m_bOper)
//		return true;

	pOne->SendMsg_StoreListResult();

	return true;
}

bool CNetworkEX::RepairRequest(int n, char* pBuf)
{
	_repair_request_clzo* pRecv = (_repair_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->byStoreIndex >= CMapData::s_tbItemStore.GetRecordNum())
		return false;

	if(pRecv->byItemNum > trade_item_max_num)
		return false;

	for(int i = 0; i < pRecv->byItemNum; i++)
	{
		if(pRecv->Item[i].byPositon != _STORAGE_POS::INVEN && pRecv->Item[i].byPositon != _STORAGE_POS::EQUIP)
			return false;	
	}
	
	if(!pOne->m_pCurMap)
		return false;
	
	CItemStore* pStore = NULL;
	for(i = 0; i < pOne->m_pCurMap->m_nItemStoreDumNum; i++)
	{
		if(pOne->m_pCurMap->m_ItemStore[i].m_pRec->m_dwIndex == pRecv->byStoreIndex)
		{
			pStore = &pOne->m_pCurMap->m_ItemStore[i];
			break;
		}
	}
	if(!pStore)
		return false;		
	
	pOne->pc_RepairItemStore(pStore, pRecv->byItemNum, pRecv->Item); 

	return true;
}

bool CNetworkEX::ExchangeDalantForGoldRequest(int n, char* pBuf)
{
	_exchange_dalant_for_gold_request_clzo* pRecv = (_exchange_dalant_for_gold_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	pOne->pc_ExchangeDalantForGold(pRecv->dwDalant);

	return true;
}

bool CNetworkEX::ExchangeGoldForDalantRequest(int n, char* pBuf)
{
	_exchange_gold_for_dalant_request_clzo* pRecv = (_exchange_gold_for_dalant_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	pOne->pc_ExchangeGoldForDalant(pRecv->dwGold);

	return true;
}

bool CNetworkEX::DTradeAskRequest(int n, char* pBuf)
{
	_d_trade_ask_request_clzo* pRecv = (_d_trade_ask_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->wDstIndex >= MAX_PLAYER)
		return false;

	pOne->pc_DTradeAskRequest(pRecv->wDstIndex);

	return true;
}

bool CNetworkEX::DTradeAnswerRequest(int n, char* pBuf)
{
	_d_trade_answer_request_clzo* pRecv = (_d_trade_answer_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_pmTrd.bDTradeMode || pOne->m_bCorpse)
		return true;

	if(pRecv->idAsker.wIndex >= MAX_PLAYER)
	{
		m_LogFile.Write("odd.. %s: DTradeAnswerRequest() : pRecv->idAsker.wIndex(%d) >= MAX_PLAYER(%d)", 
			pOne->m_Param.GetCharName(), pRecv->idAsker.wIndex, MAX_PLAYER);
		return false;
	}

	pOne->pc_DTradeAnswerRequest(&pRecv->idAsker);

	return true;
}

bool CNetworkEX::DTradeCancleRequest(int n, char* pBuf)
{
	_d_trade_cancle_request_clzo* pRecv = (_d_trade_cancle_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper)
		return true;

	pOne->pc_DTradeCancleRequest();

	return true;
}

bool CNetworkEX::DTradeLockRequest(int n, char* pBuf)
{
	_d_trade_lock_request_clzo* pRecv = (_d_trade_lock_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_bCorpse)
		return true;

	pOne->pc_DTradeLockRequest();

	return true;
}

bool CNetworkEX::DTradeOKRequest(int n, char* pBuf)
{
	_d_trade_ok_request_clzo* pRecv = (_d_trade_ok_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_bCorpse)
		return true;

	pOne->pc_DTradeOKRequest();

	return true;
}

bool CNetworkEX::DTradeAddRequest(int n, char* pBuf)
{
	_d_trade_add_request_clzo* pRecv = (_d_trade_add_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_bCorpse)
		return true;

	if(pRecv->bySlotIndex > max_d_trade_item)
	{
		m_LogFile.Write("odd.. %s: DTradeAddRequest() : pRecv->bySlotIndex(%d) > max_d_trade_item(%d)", 
			pOne->m_Param.GetCharName(), pRecv->bySlotIndex, max_d_trade_item);
		return false;
	}

	if(pRecv->byStorageCode >= _STORAGE_POS::STORAGE_NUM)
	{
		m_LogFile.Write("odd.. %s: DTradeAddRequest() : pRecv->byStorageCode(%d) >= _STORAGE_POS::STORAGE_NUM(%d)", 
			pOne->m_Param.GetCharName(), pRecv->byStorageCode, _STORAGE_POS::STORAGE_NUM);
		return false;
	}


	pOne->pc_DTradeAddRequest(pRecv->bySlotIndex, pRecv->byStorageCode, pRecv->dwSerial, pRecv->byAmount);

	return true;
}

bool CNetworkEX::DTradeDelRequest(int n, char* pBuf)
{
	_d_trade_del_request_clzo* pRecv = (_d_trade_del_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_bCorpse)
		return true;

	if(pRecv->bySlotIndex > max_d_trade_item)
		return false;

	pOne->pc_DTradeDelRequest(pRecv->bySlotIndex);

	return true;
}

bool CNetworkEX::DTradeBetRequest(int n, char* pBuf)
{
	_d_trade_bet_request_clzo* pRecv = (_d_trade_bet_request_clzo*)pBuf;

	CPlayer* pOne = &g_Player[n];

	if(!pOne->m_bOper || pOne->m_bCorpse)
		return true;

	if(pRecv->byMoneyUnit != money_unit_dalant && pRecv->byMoneyUnit != money_unit_gold)
		return false;

	pOne->pc_DTradeBetRequest(pRecv->byMoneyUnit, pRecv->dwBetAmount);

	return true;
}




