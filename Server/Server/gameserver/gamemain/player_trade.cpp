#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "EconomySystem.h"

void CPlayer::pc_BuyItemStore(CItemStore* pStore, BYTE byOfferNum, _buy_store_request_clzo::_list* pList)
{
	int i;
	_buy_offer Offer[trade_item_max_num];
	char szErrCode[64];
	bool bSucc = true;
	BYTE byAddBuffer[_STORAGE_POS::STORAGE_NUM] = {0, };

	if(pStore->m_pExistMap != m_pCurMap)
	{
		sprintf(szErrCode, "아바타와 같은 맵상에 존재하는 상점이 아닙니다.");
		bSucc = false;
		goto RESULT;
	}

	//각 스토리지당 객수 체크
	//장착, 장식, 포션, 포스, 애니머스등은 스토리지코드를 제대로 넣어서 보냈는지..
	for(i = 0; i < byOfferNum; i++)
	{
		byAddBuffer[pList[i].byStorageCode]++;
	}

	for(i = 0; i < _STORAGE_POS::STORAGE_NUM; i++)
	{
		if(m_Param.m_pStoragePtr[i]->GetNumEmptyCon() < byAddBuffer[i])
		{
			sprintf(szErrCode, "가방에 넣을 공간이 부족합니다.");
			bSucc = false;
			goto RESULT;
		}
	}

	for(i = 0; i < byOfferNum; i++)
	{
		Offer[i].byGoodIndex = pList[i].dwGoodSerial;
		Offer[i].byGoodAmount = pList[i].byAmount;
		Offer[i].byStorageCode = pList[i].byStorageCode;
	}

RESULT:

	if(bSucc)
	{
		if(pStore->IsSell(byOfferNum, Offer, m_Param.GetDalant(), m_Param.GetGold(), m_EP.m_fEff_Have[_EFF_HAVE::Trade_Prof], szErrCode))//PARAMETER EDIT (상점과의 거래에서의 이득)
		{
			for(i = 0; i < byOfferNum; i++)
			{
				//아이템 종류가 넣기가능한 스토리지인지..
				if(!::IsStorageCodeWithItemKind(Offer[i].Item.m_byTableCode, pList[i].byStorageCode))
				{
					sprintf(szErrCode, "넣을수없는 스토리지임");
					bSucc = false;
					break;
				}

				if(pList[i].byStorageCode == _STORAGE_POS::EQUIP)
				{//장착이라면 그 자리에 이미 아이템이 있는가..
					__ITEM* pItem = (__ITEM*)&m_Param.m_dbEquip.m_pStorageList[pList[i].byStorageCode];
					if(pItem->m_bLoad)
					{
						sprintf(szErrCode, "이미장착중임");
						bSucc = false;
						break;			
					}
				}

				if(pList[i].byStorageCode == _STORAGE_POS::EQUIP || pList[i].byStorageCode == _STORAGE_POS::EMBELLISH)
				{
					//사용가능종족인지확인
					if(!::IsItemEquipCivil(Offer[i].Item.m_byTableCode, Offer[i].Item.m_wItemIndex, m_Param.GetRaceSexCode()))
					{
						sprintf(szErrCode, "장착할수없는종족입니다");
						bSucc = false;
						break;			
					}
					//장착가능 능력인지.
					if(!IsEffectableEquip(&Offer[i].Item))
					{
						sprintf(szErrCode, "장착능력부족");
						bSucc = false;
						break;
					}
				}

				else if(pList[i].byStorageCode == _STORAGE_POS::FORCE)
				{	//같은 효과의 포스가 있는지..
					for(int s = 0; s < force_storage_num; s++)
					{
						__ITEM* pItem = (__ITEM*)&m_Param.m_dbForce.m_pStorageList[s];

						if(!pItem->m_bLoad)
							continue;

						if(s_pnLinkForceItemToEffect[pItem->m_wItemIndex] == s_pnLinkForceItemToEffect[Offer[i].Item.m_wItemIndex])
						{
							sprintf(szErrCode, "이미 같은효과의 포스가 있음");
							bSucc = false;
							break;
						}
					}
				}

				else if(pList[i].byStorageCode == _STORAGE_POS::ANIMUS)
				{	//같은 애니머스가 있는지..
					for(int s = 0; s < animus_storage_num; s++)
					{
						__ITEM* pItem = (__ITEM*)&m_Param.m_dbAnimus.m_pStorageList[s];

						if(!pItem->m_bLoad)
							continue;

						if(pItem->m_wItemIndex == Offer[i].Item.m_wItemIndex)
						{
							sprintf(szErrCode, "이미 같은 애니머스가 있음");
							bSucc = false;
							break;
						}
					}
				}

				if(!bSucc)
					break;
			}
		}
		else
			bSucc = false;
	}

	if(bSucc)
	{
		AlterMoney(-pStore->GetLastTradeGold(), -pStore->GetLastTradeDalant());				

		for(i = 0; i < byOfferNum; i++)
		{
			Offer[i].wSerial = m_Param.GetNewItemSerial();
			Offer[i].Item.m_wSerial = Offer[i].wSerial;

			Emb_AddStorage(pList[i].byStorageCode, &Offer[i].Item);			
		}

		//ITEM HISTORY..
		s_MgrItemHistory.buy_item(Offer, byOfferNum, pStore->GetLastTradeDalant(), pStore->GetLastTradeGold(), m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);
	}

	SendMsg_BuyItemStoreResult(pStore, bSucc, byOfferNum, Offer, szErrCode);
}

void CPlayer::pc_SellItemStore(CItemStore* pStore, BYTE byOfferNum, _sell_store_request_clzo::_list* pList)
{
	int i;
	_sell_offer Offer[trade_item_max_num];
	char szErrCode[64] = {0,};
	bool bSucc = true;

	if(pStore->m_pExistMap != m_pCurMap)
	{
		sprintf(szErrCode, "아바타와 같은 맵상에 존재하는 상점이 아닙니다.");
		bSucc = false;
		goto RESULT;
	}

	for(i = 0; i < byOfferNum; i++)
	{
		//실제 소유하는지 검사..
		Offer[i].pItem = m_Param.m_pStoragePtr[pList[i].byStorageCode]->GetPtrFromSerial(pList[i].wSerial);
		if(!Offer[i].pItem)
		{
			sprintf(szErrCode, "현재 소유하고있는 아이템이 아닙니다.");
			bSucc = false;
			goto RESULT;
		}

		if(Offer[i].pItem->m_byTableCode == tbl_code_key)
		{
			sprintf(szErrCode, "유닛은 거래금지.");
			bSucc = false;
			goto RESULT;
		}

		//락걸린 아이템인지..
		if(Offer[i].pItem->m_bLock)
		{
			sprintf(szErrCode, "락걸린 아이템");
			bSucc = false;
			goto RESULT;
		}

		//이중으로 올렸는지 검사
		for(int e = 0; e < i; e++)
		{
			if(Offer[e].pItem == Offer[i].pItem)//겹침..
			{
				sprintf(szErrCode, "아이템을 이중으로 올렸습니다.");
				bSucc = false;
				goto RESULT;
			}
		}

		Offer[i].bySlotIndex = Offer[i].pItem->m_byStorageIndex;
		Offer[i].byStorageCode = pList[i].byStorageCode;

		//중첩이라면..
		if(::IsOverLapItem(Offer[i].pItem->m_byTableCode))
		{
			if(pList[i].byAmount > Offer[i].pItem->m_dwDur)
			{
				sprintf(szErrCode, "소지수량보다 많은 수량을 선택했습니다");
				bSucc = false;
				goto RESULT;
			}
			Offer[i].byAmount = pList[i].byAmount;
		}
		else
		{
			Offer[i].byAmount = 1;
		}
	}

RESULT:

	if(bSucc)
	{
		if(pStore->IsBuy(byOfferNum, Offer, m_EP.m_fEff_Have[_EFF_HAVE::Trade_Prof], szErrCode)) //PARAMETER EDIT (상점과의 거래에서의 이득)
		{
			AlterMoney(pStore->GetLastTradeGold(),  pStore->GetLastTradeDalant());

			//ITEM HISTORY..
			s_MgrItemHistory.sell_item(Offer, byOfferNum, pStore->GetLastTradeDalant(), pStore->GetLastTradeGold(),  m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);

			for(int i = 0; i < byOfferNum; i++)
			{
				if(!::IsOverLapItem(Offer[i].pItem->m_byTableCode))	//중첩이아니면 그냥 삭제
					Emb_DelStorage(pList[i].byStorageCode, Offer[i].bySlotIndex, false);
				else
					Emb_AlterDurPoint(pList[i].byStorageCode, Offer[i].bySlotIndex, -Offer[i].byAmount, true);//팔기
			}				
		}
		else
		{
			bSucc = false;
		}
	}

	SendMsg_SellItemStoreResult(pStore, bSucc, szErrCode);
}

void CPlayer::pc_RepairItemStore(CItemStore* pStore, BYTE byOfferNum, _repair_request_clzo::_list* pList)
{
	int i;
	bool bSucc = true;
	char szErrCode[64] = {0,};
	_repair_offer Offer[trade_item_max_num];

	if(pStore->m_pExistMap != m_pCurMap)
	{
		sprintf(szErrCode, "아바타와 같은 맵상에 존재하는 상점이 아닙니다.");
		bSucc = false;
		goto RESULT;
	}

	for(i = 0; i < byOfferNum; i++)
	{
		//실제 소유하는지 검사..
		int nIndex= -1;
		__ITEM* pItem = NULL;

		if(pList[i].byPositon == _STORAGE_POS::INVEN)
		{
			Offer[i].pItem = m_Param.m_dbInven.GetPtrFromSerial(pList[i].wSerial);
			if(!Offer[i].pItem)
			{
				sprintf(szErrCode, "현재 소유하고있는 아이템이 아닙니다.");
				bSucc = false;
				goto RESULT;
			}

			Offer[i].bySlotIndex = Offer[i].pItem->m_byStorageIndex;
		}
		else if(pList[i].byPositon == _STORAGE_POS::EQUIP)
		{	
			Offer[i].pItem = m_Param.m_dbEquip.GetPtrFromSerial(pList[i].wSerial);
			if(!Offer[i].pItem)
			{
				sprintf(szErrCode, "현재 소유하고있는 아이템이 아닙니다.");
				bSucc = false;
				goto RESULT;
			}

			Offer[i].bySlotIndex = Offer[i].pItem->m_byStorageIndex;
		}
		else
		{
			bSucc = false;
			goto RESULT;

		}

		if(!IsRepairableItem(Offer[i].pItem->m_byTableCode, Offer[i].pItem->m_wItemIndex))
		{
			sprintf(szErrCode, "수리가 불가능한 아이템");
			bSucc = false;
			goto RESULT;
		}
	}

RESULT:

	if(bSucc)
	{
		if(pStore->IsRepair(byOfferNum, Offer, m_Param.GetDalant(), m_Param.GetGold(), szErrCode))
		{
			AlterMoney(-pStore->GetLastTradeGold(), -pStore->GetLastTradeDalant());

			//ITEM HISTORY..
			s_MgrItemHistory.repair_item(Offer, byOfferNum, pStore->GetLastTradeDalant(), pStore->GetLastTradeGold(), m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);

			for(i = 0; i < byOfferNum; i++)
			{	
				Emb_AlterDurPoint(pList[i].byPositon, Offer[i].bySlotIndex, Offer[i].wNewDurPoint-Offer[i].pItem->m_dwDur, true);
			}
		}
		else
		{
			bSucc = false;
		}
	}

	SendMsg_RepairItemStoreResult(pStore, byOfferNum, Offer, bSucc, szErrCode);
}

void CPlayer::pc_ExchangeDalantForGold(DWORD dwDalant)
{
	BYTE byErrCode = 0;
	DWORD dwOldDalant = m_Param.GetDalant();
	DWORD dwOldGold = m_Param.GetGold();

	if(dwDalant > dwOldDalant)
	{
		byErrCode = 1;//달란트부족 
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		int nR = ::eGetRate(m_Param.GetRaceCode());

		DWORD dwExcDalant = dwDalant - dwDalant%nR;
		DWORD dwExcGold = dwExcDalant/nR;

		AlterMoney(dwExcGold, -dwExcDalant);

		//ITEM HISTORY..
		s_MgrItemHistory.exchange_money(dwOldDalant, dwOldGold, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);
	}

	SendMsg_ExchangeMoneyResult(byErrCode);
}

void CPlayer::pc_ExchangeGoldForDalant(DWORD dwGold)
{
	BYTE byErrCode = 0;
	DWORD dwOldGold = m_Param.GetGold();
	DWORD dwOldDalant = m_Param.GetDalant();

	if(dwGold > dwOldGold)
	{
		byErrCode = 1;//골드부족 
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		int nR = ::eGetRate(m_Param.GetRaceCode()); 

		DWORD dwExcDalant = dwGold * nR;

		AlterMoney(-dwGold, dwExcDalant);

		//ITEM HISTORY..
		s_MgrItemHistory.exchange_money(dwOldDalant, dwOldGold, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);
	}

	SendMsg_ExchangeMoneyResult(byErrCode);
}

//거래요청
void CPlayer::pc_DTradeAskRequest(WORD wDstIndex)
{
	BYTE byErrCode = 0;//1;//자신이이미트레이드중 2;//거리가멈 3;//유효한대상아님 4;//상대가트레이드중
	CPlayer* pDst = &g_Player[wDstIndex];

	//상대방이 트레이드모드인지
	if(m_pmTrd.bDTradeMode || pDst->m_pmTrd.bDTradeMode)
	{
		byErrCode = 1;//이미트레이드중..
		goto RESULT;
	}

	//상대방이 유효한 상대인지..
	if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
	{
		byErrCode = 3;//유효한 대상이 아님
		goto RESULT;
	}

	if(m_byUserDgr != pDst->m_byUserDgr)
	{//일반등급이아나면 거래금지
		byErrCode = 3;//유효한 대상이 아님
		goto RESULT;
	}
	//거리체크
	if(::GetSqrt(pDst->m_fCurPos, m_fCurPos) > 100.0f)
	{
		byErrCode = 2;//거리가 멈
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{		
		//대상자 기록
		m_pmTrd.wDTradeDstIndex = pDst->m_ObjID.m_wIndex;
		m_pmTrd.dwDTradeDstSerial = pDst->m_dwObjSerial;

		//상대에게 요청을알림
		pDst->SendMsg_DTradeAskInform(this);	
	}
	SendMsg_DTradeAskResult(byErrCode);
}

//거래요청에대한 승락
void CPlayer::pc_DTradeAnswerRequest(_CLID* pidAsker)
{
	BYTE byErrCode = 0;//1;//유효한ask가아님 2;//asker가이미트레이드모드 3;//answer가이미트레이드모드 4;//asker가자신을요청중아님 5;//거리가 멈 6;//유효한 대상이 아님
	CPlayer* pAsker = &g_Player[pidAsker->wIndex];

	//asker의 시리얼비교..
	if(pAsker->m_dwObjSerial != pidAsker->dwSerial)
	{
		byErrCode = 1;//유효한ask가아님
		goto RESULT;
	}

	//asker의 대상이 자신인지..
	if(pAsker->m_pmTrd.wDTradeDstIndex != m_ObjID.m_wIndex 
		|| pAsker->m_pmTrd.dwDTradeDstSerial != m_dwObjSerial)
	{
		byErrCode = 4;//asker가자신을요청중아님
		goto RESULT;
	}
	
	//asker가 이미트레이드모드인지..
	if(m_pmTrd.bDTradeMode || pAsker->m_pmTrd.bDTradeMode)
	{
		byErrCode = 2;//이미트레이드모드
		goto RESULT;
	}

	//상대방이 유효한 상대인지..
	if(!pAsker->m_bLive || pAsker->m_bCorpse || pAsker->m_pCurMap != m_pCurMap || pAsker->GetCurSecNum() == 0xFFFFFFFF)
	{
		byErrCode = 6; //유효한 대상이 아님
		goto RESULT;
	}

	if(m_byUserDgr != pAsker->m_byUserDgr)
	{//등급이깉지않으면 거래금지
		byErrCode = 6;//유효한 대상이 아님
		goto RESULT;
	}

	//거리체크
	if(::GetSqrt(pAsker->m_fCurPos, m_fCurPos) > 100.0f)
	{
		byErrCode = 5; //거리가 멈
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//두명의 당사자를 직거래모드로.. 상대의 빈슬롯부만큼난 팔수잇다
		m_pmTrd.SetDTradeStart(pAsker->m_ObjID.m_wIndex, pAsker->m_dwObjSerial, m_Param.m_dbInven.GetNumEmptyCon());
		pAsker->m_pmTrd.SetDTradeStart(m_ObjID.m_wIndex, m_dwObjSerial, pAsker->m_Param.m_dbInven.GetNumEmptyCon());

		//두명에게 직거래 시작을 알린다.
		SendMsg_DTradeStartInform(pAsker, this);
		pAsker->SendMsg_DTradeStartInform(pAsker, this);
	}
	else
		SendMsg_DTradeAnswerResult(byErrCode);
}

void CPlayer::pc_DTradeCancleRequest()
{
	BYTE byErrCode = 0;//1;//트레이드아님 2;//승인상태 
	CPlayer* pDst= NULL;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//트레이드아님
		goto RESULT;
	}

	//승인됀 상태인가.
//	if(m_pmTrd.bDTradeOK)
//	{
//		byErrCode = 2;//승인상태
//		goto RESULT;
//	}

RESULT:

	if(byErrCode == 0)
	{
		//당사자거래 초기화
		m_pmTrd.Init();
		pDst->m_pmTrd.Init();

		SendMsg_DTradeCancleResult(byErrCode);
		pDst->SendMsg_DTradeCancleInform();
	}
	else
	{	//실패시 직권종료
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeLockRequest()
{
	BYTE byErrCode = 0;//1;//트레이드아님 2;//락걸린상태 3;//락안걸린상태 4;//승인됀상태
	CPlayer* pDst= NULL;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//트레이드아님
		goto RESULT;
	}

	//락이 풀린상태여야한다.
	if(m_pmTrd.bDTradeLock)
	{
		byErrCode = 2;//락걸린상태
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//락을 건다
		m_pmTrd.bDTradeLock = true;
		//상대에게 알림
		pDst->SendMsg_DTradeLockInform();

		SendMsg_DTradeLockResult(byErrCode);
	}
	else
	{	//실패시 직권종료
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeAddRequest(BYTE bySlotIndex, BYTE byStorageCode, DWORD dwSerial, BYTE byAmount)
{
	BYTE byErrCode = 0;//1;//트레이드모드아님 2;//락상태임 3;//유효한슬롯이 아님 4;//채워진슬롯 5;//없는아탬 6;//잘못됀양 7;//거래체크품목, 9;//락걸린 아이템
	__ITEM* pItem = NULL;
	CPlayer* pDst= NULL;
	_UNIT_DB_BASE::_LIST* pUnitData = NULL; //만약 유닛일경우 유닛데이터를 저장한다.
	int i;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//트레이드아님
		goto RESULT;
	}

	//락상태인가..
	if(m_pmTrd.bDTradeLock)
	{
		byErrCode = 2;//락상태임
		goto RESULT;
	}

	//유효한 스롯인가..
	if(bySlotIndex >= max_d_trade_item)
	{
		byErrCode = 3;//유효한슬롯이 아님
		goto RESULT;
	}

	//채워진슬롯인가..
	if(m_pmTrd.DItemNode[bySlotIndex].bLoad)
	{
		byErrCode = 4;//채워진슬롯
		goto RESULT;
	}

	//유효한아이템인가..
	pItem = m_Param.m_pStoragePtr[byStorageCode]->GetPtrFromSerial(dwSerial);
	if(!pItem)
	{
		byErrCode = 5;//없는아탬
		goto RESULT;
	}

	//락걸린 아이템인지..
	if(pItem->m_bLock)
	{
		byErrCode = 9;//락걸린 아이템
		goto RESULT;
	}

	//유닛은 상대방이 유닛소지수를 체크해야한다.
	if(pItem->m_byTableCode == tbl_code_key)
	{
		//먼저 상대가 벨라토인지..
		if(pDst->m_Param.GetRaceSexCode()/2 != race_code_bellato)
		{
			byErrCode = 111;//거래불가능종족
			goto RESULT;
		}
		if(unit_storage_num - pDst->m_Param.GetHaveUnitNum() < m_pmTrd.byAddUnitNum+1)
		{
			byErrCode = 112;//상대의 유닛소지수 초과
			goto RESULT;
		}
		if(pItem->m_dwLv >= unit_storage_num)
		{
			byErrCode = 113;//잘못된 유닛키번호( 나와서는 안될 에러 )
			goto RESULT;
		}

		pUnitData = &m_Param.m_UnitDB.m_List[pItem->m_dwLv];

		if(pUnitData->byFrame == 0xFF)
		{
			byErrCode = 114;//없는 유닛( 나와서는 안될 에러 )
			goto RESULT;
		}		
	}

	//량이 맞느지..
	if(::IsOverLapItem(pItem->m_byTableCode))
	{
		if(byAmount > pItem->m_dwDur || byAmount == 0)
		{
			byErrCode = 6;//잘못됀양
			goto RESULT;
		}
	}

	//이미 올린건가..
	for(i = 0; i < max_d_trade_item; i++)
	{
		_DTRADE_ITEM* p = &m_pmTrd.DItemNode[i];
		
		if(!p->bLoad)
			continue;

		if(p->dwSerial == dwSerial)
		{
			byErrCode = 6;//이미추가한아이템
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		if(byStorageCode == _STORAGE_POS::INVEN)
		{
			if(::IsOverLapItem(pItem->m_byTableCode))
			{
				if(pItem->m_dwDur == byAmount)
					m_pmTrd.byEmptyInvenNum++;
			}
			else
				m_pmTrd.byEmptyInvenNum++;
		}
		m_pmTrd.bySellItemNum++;

		m_pmTrd.DItemNode[bySlotIndex].SetData(byStorageCode, dwSerial, byAmount);
		pDst->SendMsg_DTradeAddInform(bySlotIndex, pItem, byAmount);

		SendMsg_DTradeAddResult(byErrCode);
		if(pItem->m_byTableCode == tbl_code_key && pUnitData)//유닛의 경우 올려진갯수를 케크..
		{
			m_pmTrd.byAddUnitNum++;
			pDst->SendMsg_DTradeUnitInfoInform(bySlotIndex, pUnitData);
		}
	}
	else
	{	//실패시 직권종료
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeDelRequest(BYTE bySlotIndex)
{
	BYTE byErrCode = 0;//1;//트레이드모드아님 2;//락상태임 3;//유효한슬롯이 아님 4;//빈슬롯
	CPlayer* pDst = NULL;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//트레이드아님
		goto RESULT;
	}

	//락상태인가..
	if(m_pmTrd.bDTradeLock)
	{
		byErrCode = 2;//락상태임
		goto RESULT;
	}

	//유효한 스롯인가..
	if(bySlotIndex >= max_d_trade_item)
	{
		byErrCode = 3;//유효한슬롯이 아님
		goto RESULT;
	}

	//채워진슬롯인가..
	if(!m_pmTrd.DItemNode[bySlotIndex].bLoad)
	{
		byErrCode = 4;//빈슬롯
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		_DTRADE_ITEM* pData = &m_pmTrd.DItemNode[bySlotIndex];
		if(pData->byStorageCode == _STORAGE_POS::INVEN)
		{
			__ITEM* pItem = m_Param.m_pStoragePtr[pData->byStorageCode]->GetPtrFromSerial(pData->dwSerial);
			if(!pItem)
				return;

			if(::IsOverLapItem(pItem->m_byTableCode))
			{
				if(pItem->m_dwDur == pData->byAmount)
					m_pmTrd.byEmptyInvenNum--;
			}
			else
				m_pmTrd.byEmptyInvenNum--;

			if(pItem->m_byTableCode == tbl_code_key)//유닛의 경우 올려진갯수를 케크..
				m_pmTrd.byAddUnitNum--;
		}
		m_pmTrd.bySellItemNum--;

		m_pmTrd.DItemNode[bySlotIndex].ReleaseData();
		pDst->SendMsg_DTradeDelInform(bySlotIndex);

		SendMsg_DTradeDelResult(byErrCode);
	}
	else
	{	//실패시 직권종료
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeBetRequest(BYTE byMoneyUnit, DWORD dwBetAmount)
{
	BYTE byErrCode = 0;//1;//달란트부족 2;//골드부족 3;//단위에러 4;//락상태임 5;//트레이드모드아님
	CPlayer* pDst = NULL;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//트레이드아님
		goto RESULT;
	}
	
	//락걸린상태인가..
	if(m_pmTrd.bDTradeLock)
	{
		byErrCode = 4;//락상태임
		goto RESULT;
	}

	//그만큼소지하고잇는지..
	if(byMoneyUnit == money_unit_dalant)
	{
		if(byMoneyUnit > m_Param.GetDalant())
		{
			byErrCode = 1;//달란트부족
			goto RESULT;
		}
	}
	else
	{
		if(byMoneyUnit > m_Param.GetGold())
		{
			byErrCode = 2;//골드부족
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		if(byMoneyUnit == money_unit_dalant)
			m_pmTrd.dwDTrade_Dalant = dwBetAmount;
		else
			m_pmTrd.dwDTrade_Gold = dwBetAmount;	

		pDst->SendMsg_DTradeBetInform(byMoneyUnit, dwBetAmount);

		SendMsg_DTradeBetResult(byErrCode);
	}
	else
	{	//실패시 직권종료
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeOKRequest()
{
	BYTE byErrCode = 0;//1;//트레이드모드아님 2;//락상태아님 3;//OK 100//담을공간없음 4; //없어진물건
	CPlayer* pTrader[2] = {this, NULL};
	__ITEM SellItem[2][max_d_trade_item];
	int nSellItemNum[2] = {0, 0};

	if(!::DTradeEqualPerson(this, &pTrader[1]))
	{
		byErrCode = 1;//트레이드아님
		goto RESULT;
	}
	//서로 락걸린상태인지..
	if(!m_pmTrd.bDTradeLock || !pTrader[1]->m_pmTrd.bDTradeLock)
	{
		byErrCode = 2;//락상태아님
		goto RESULT;
	}
	if(m_pmTrd.bDTradeOK)
	{
		byErrCode = 3;//OK
		goto RESULT;
	}
	//판매하고자하는 물건이 상대의 인벤에다 담을수 있는지..
	if(m_pmTrd.bySellItemNum > pTrader[1]->m_pmTrd.byEmptyInvenNum)
	{
		byErrCode = 100; //담을공간없음
		goto RESULT;
	}

	//상대도 이미 ok한 상태라면 베팅물건이 온전한지....
	if(pTrader[1]->m_pmTrd.bDTradeOK)
	{
		for(int p = 0; p < 2; p++)
		{
			int nCnt = 0;
			for(int i = 0; i < max_d_trade_item; i++)
			{
				_DTRADE_ITEM* pNode = &pTrader[p]->m_pmTrd.DItemNode[i];
				if(!pNode->bLoad)
					continue;

				_STORAGE_LIST* pList = pTrader[p]->m_Param.m_pStoragePtr[pNode->byStorageCode];
				
				__ITEM* pItem = pList->GetPtrFromSerial(pNode->dwSerial);
				if(!pItem)
				{
					g_Main.m_logSystemError.Write("CPlayer::pc_DTradeOKRequest().. 없는 시리얼 list(%d), serial(%d)", pNode->byStorageCode, pNode->dwSerial);
					byErrCode = 4; //없어진물건						
					goto RESULT;
				}

				//락걸린 아이템인지..
				if(pItem->m_bLock)
				{
					byErrCode = 9;//락걸린 아이템
					goto RESULT;
				}

				memcpy(&SellItem[p][nCnt], pItem, sizeof(__ITEM));

				if(::IsOverLapItem(pItem->m_byTableCode) && pItem->m_dwDur > pNode->byAmount)
				{
					SellItem[p][nCnt].m_dwDur = pNode->byAmount;
				}
				nCnt++;

			}
			
			nSellItemNum[p] = nCnt;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		SendMsg_DTradeOKResult(byErrCode);
		pTrader[1]->SendMsg_DTradeOKInform();
		
		m_pmTrd.bDTradeOK = true;
		//상대도 ok중이면 거래 성립으로 처리한다.
		if(pTrader[1]->m_pmTrd.bDTradeOK && m_pmTrd.bDTradeOK)
		{
			for(int p = 0; p < 2; p++)
			{
				int dp = (p+1)%2;

				//자신이 건거 빼기..
				for(int i = 0; i < nSellItemNum[p]; i++)
				{
					if(::IsOverLapItem(SellItem[p][i].m_byTableCode))
					{
						pTrader[p]->Emb_AlterDurPoint(SellItem[p][i].m_pInList->m_nListCode, SellItem[p][i].m_byStorageIndex, -SellItem[p][i].m_dwDur, true);//직거래팔기
					}
					else
					{
						pTrader[p]->Emb_DelStorage(SellItem[p][i].m_pInList->m_nListCode, SellItem[p][i].m_byStorageIndex, false);
					}
				}

				//상대가 건거 더하기..
				WORD wStartSerial = 0xFFFF;
				for(i = 0; i < nSellItemNum[dp]; i++)
				{
					SellItem[dp][i].m_wSerial = pTrader[p]->m_Param.GetNewItemSerial();
					if(wStartSerial == 0xFFFF)
						wStartSerial = SellItem[dp][i].m_wSerial;
					
					//유닛일 경우.. 추가처리
					if(SellItem[dp][i].m_byTableCode == tbl_code_key)
					{
						_UNIT_DB_BASE::_LIST* pSrcUnit = &pTrader[dp]->m_Param.m_UnitDB.m_List[SellItem[dp][i].m_dwLv];

						_UNIT_DB_BASE::_LIST Copy_SrcUnit;
						memcpy(&Copy_SrcUnit, pSrcUnit, sizeof(_UNIT_DB_BASE::_LIST));

						//p -> 추가..
						for(int e = 0; e < unit_storage_num; e++)
						{
							_UNIT_DB_BASE::_LIST* pDstUnit = &pTrader[p]->m_Param.m_UnitDB.m_List[i];
							if(pDstUnit->byFrame == 0xFF)
							{
								//추가
								pDstUnit->PushUnit(pSrcUnit);

								//키의 번호변경
								SellItem[dp][i].m_dwLv = i;

								//p -> send..
								pTrader[p]->SendMsg_DTradeUnitAddInform(SellItem[dp][i].m_wSerial, pDstUnit);	//ChangeOwner()보다 먼저 호출..(상대에게 먼저 유닛추가를 알려야함..)

								//만약 사용중인 유닛이라면..
								if(pTrader[dp]->m_pUsingUnit == pSrcUnit)
								{
									if(pTrader[dp]->m_pParkingUnit)//주차중이였다면..
									{
										pTrader[dp]->m_pParkingUnit->ChangeOwner(pTrader[p], pDstUnit->bySlotIndex);

										pTrader[p]->m_pParkingUnit = pTrader[dp]->m_pParkingUnit;
										pTrader[dp]->m_pParkingUnit = NULL;
									}
									pTrader[dp]->m_pUsingUnit = NULL;
									pTrader[p]->m_pUsingUnit = pDstUnit;
								}

								//p ITEM HISTORY..(추가)
								s_MgrItemHistory.add_unit_after_trade(pTrader[dp]->m_Param.GetCharSerial(), pDstUnit, pTrader[p]->m_szItemHistoryFileName);

								break;
							}
						}

						//dp-> 삭제..
						pSrcUnit->DelUnit();

						//dp ITEM HISTORY..(삭제)
						s_MgrItemHistory.del_unit_after_trade(pTrader[p]->m_Param.GetCharSerial(), &Copy_SrcUnit, pTrader[dp]->m_szItemHistoryFileName);
					}

					pTrader[p]->Emb_AddStorage(_STORAGE_POS::INVEN, &SellItem[dp][i]);//거래
				}

				int nAlterGold = pTrader[dp]->m_pmTrd.dwDTrade_Gold - pTrader[p]->m_pmTrd.dwDTrade_Gold;
				int nAlterDalant = pTrader[dp]->m_pmTrd.dwDTrade_Dalant - pTrader[p]->m_pmTrd.dwDTrade_Dalant;

				pTrader[p]->AlterMoney(nAlterGold, nAlterDalant);									
//				pTrader[p]->SetMoney(pTrader[p]->m_Param.GetGold() + pTrader[dp]->m_pmTrd.dwDTrade_Gold - pTrader[p]->m_pmTrd.dwDTrade_Gold, 
//									pTrader[p]->m_Param.GetDalant() + pTrader[dp]->m_pmTrd.dwDTrade_Dalant - pTrader[p]->m_pmTrd.dwDTrade_Dalant);

				pTrader[p]->SendMsg_DTradeAccomplishInform(true, wStartSerial);

				//ITEM HISTORY..
				s_MgrItemHistory.trade(&SellItem[p][0], nSellItemNum[p], pTrader[p]->m_pmTrd.dwDTrade_Dalant, pTrader[p]->m_pmTrd.dwDTrade_Gold, 
														&SellItem[dp][0], nSellItemNum[dp], pTrader[dp]->m_pmTrd.dwDTrade_Dalant, pTrader[dp]->m_pmTrd.dwDTrade_Gold,
														pTrader[dp]->m_Param.GetCharSerial(), pTrader[p]->m_Param.GetDalant(), pTrader[p]->m_Param.GetGold(),
														pTrader[p]->m_szItemHistoryFileName);
			}

			//거래버퍼 초기화
			m_pmTrd.Init();
			pTrader[1]->m_pmTrd.Init();
		}
	}
	else//실패시 직권종료
	{	
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pTrader[1])
		{
			pTrader[1]->m_pmTrd.Init();
			pTrader[1]->SendMsg_DTradeCloseInform(0);
		}
	}
}
