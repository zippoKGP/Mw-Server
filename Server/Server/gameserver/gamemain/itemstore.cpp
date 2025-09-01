#include "stdafx.h"

#include "ItemStore.h"
#include "MainThread.h"
#include "Player.h"
#include "DfnEquipItemFld.h"
#include "WeaponItemFld.h"
#include "BulletItemFld.h"
#include "EtcItemFld.h"
#include "MakeToolItemFld.h"
#include "PotionItemFld.h"
#include "StoreListFld.h"
#include "NPCharacterFld.h"
#include "EconomySystem.h"

#define max_discount	0.5f

CItemStore::CItemStore()
{
	memset(this, NULL, sizeof(*this));
}

CItemStore::~CItemStore()
{
	if(m_pStorageItem)
		delete [] m_pStorageItem;
}

bool CItemStore::Init(int nIndex, CMapData* pExistMap, _store_dummy* pDum, _base_fld* pRec)
{
	if(m_bLive)
		return false;

	m_bLive = true;
	m_nIndex = nIndex;
	m_nStorageItemNum = 0;
	m_pExistMap = pExistMap;
	m_pDum = pDum;
	m_pRec = (_StoreList_fld*)pRec;

	m_pStorageItem = new _good_storage_info [max_store_list];
	m_nStorageItemNum = max_store_list;

	for(int i = 0; i < max_store_list; i++)
	{
		int nTable = ::GetItemTableCode(m_pRec->m_strItemlist[i]);
		if(nTable == -1)
		{
			m_nStorageItemNum = i;
			break;
		}
		m_pStorageItem[i].byItemTableCode = nTable;

		_base_fld* pFld = g_Main.m_tblItemData[nTable].GetRecord(m_pRec->m_strItemlist[i]);
		if(!pFld)
		{
			::MyMessageBox("CItemStore::Init error", "%d Store %s Item can't find index", nIndex, m_pRec->m_strItemlist[i]);
			m_nStorageItemNum = i;
			break;
		}
		m_pStorageItem[i].wItemIndex = pFld->m_dwIndex;
		m_pStorageItem[i].nStdPrice = ::GetItemStdPrice(nTable, pFld->m_dwIndex, m_nRaceCode, &m_pStorageItem[i].byMoneyUnit);		
		if(m_pStorageItem[i].nStdPrice == -1)
		{
			::MyMessageBox("CItemStore::Init error", "가격 로드실패..(nTable: %d, pFld->m_dwIndex: %d)", nTable, pFld->m_dwIndex);
			m_nStorageItemNum = i;
			break;
		}
		m_pStorageItem[i].bExist = (BOOL)*(((char*)pFld)+sizeof(_base_fld));
		m_pStorageItem[i].dwDurPoint = ::GetItemDurPoint(nTable, pFld->m_dwIndex);
	}

	m_dwSecIndex = CalcSecIndex(m_pDum->m_pDumPos->m_fCenterPos[0], m_pDum->m_pDumPos->m_fCenterPos[2]);
	if(!GetNpcRaceCode(&m_nRaceCode))
	{
		::MyMessageBox("CItemStore::Init error", "Map:%s, Store:%d.. 없는 npc (%s) ", pExistMap->m_pMapSet->m_strCode, nIndex, m_pRec->m_strStore_NPCcode);
		return false;
	}

	return true;
}

bool CItemStore::IsSell(BYTE byOfferNum, _buy_offer* pOffer, DWORD dwHasDalant, DWORD dwHasGold, float fDiscountRate, char* pszErrCode)	//수량 및 금액검사
{
	SetZeroTradeMoney();
	float fR = 1.0f + ::eGetTex(m_nRaceCode);

	for(int i = 0; i < byOfferNum; i++)
	{	
		_good_storage_info* pStorage = &m_pStorageItem[pOffer[i].byGoodIndex];

		BYTE byMoneyUnit;
		//판매가격 = 개당가격 * 세율 * 수량
		float fSellPrice = (CalcSellPrice(pOffer[i].byGoodIndex, &byMoneyUnit) * fR) * pOffer[i].byGoodAmount;
		
		if(byMoneyUnit == money_unit_dalant)
		{
			m_nLastTradeDalant += fSellPrice;
			if(dwHasDalant < m_nLastTradeDalant)	//보유금이 가격보다 작으면..실패
			{
				sprintf(pszErrCode, "보유금액이 모자랍니다.");
				return false;
			}
		}
		else
		{
			m_nLastTradeGold += fSellPrice;
			if(dwHasGold < m_nLastTradeGold)	//보유금이 가격보다 작으면..실패
			{
				sprintf(pszErrCode, "보유금액이 모자랍니다.");
				return false;
			}
		}
	}

	//할인룰 적용
	if(fDiscountRate > 0.0f && m_nLastTradeDalant > 1)
	{
		if(fDiscountRate > max_discount)
			fDiscountRate = max_discount;

		int nDiscount = m_nLastTradeDalant * fDiscountRate;
		m_nLastTradeDalant -= nDiscount;

		if(m_nLastTradeDalant <= 0)
			m_nLastTradeDalant = 1;
	}

	for(i = 0; i < byOfferNum; i++)
	{
		_good_storage_info* pStorage = &m_pStorageItem[pOffer[i].byGoodIndex];

		pOffer[i].Item.m_byTableCode = pStorage->byItemTableCode;
		pOffer[i].Item.m_wItemIndex = pStorage->wItemIndex;

		if(::IsOverLapItem(pStorage->byItemTableCode))
			pOffer[i].Item.m_dwDur = pOffer[i].byGoodAmount;
		else
			pOffer[i].Item.m_dwDur = pStorage->dwDurPoint;

		//아이템의 종류에따른 아이템레벨 세팅 분기..
		BYTE byItemKindCode = ::GetItemKindCode(pStorage->byItemTableCode);			
		if(byItemKindCode == item_kind_std)
		{	//일반아이템
			BYTE byLimSocketNum = ::GetDefItemUpgSocketNum(pStorage->byItemTableCode, pStorage->wItemIndex);
			if(byLimSocketNum > DEFAULT_ITEM_LV)	//3레벨이상은 3으로 제한..
				byLimSocketNum = DEFAULT_ITEM_LV;

			pOffer[i].Item.m_dwLv = ::GetBitAfterSetLimSocket(byLimSocketNum);
		}
		else if(byItemKindCode == item_kind_ani)
		{	//소환아이템
			pOffer[i].Item.m_dwLv = ::GetMaxParamFromExp(pStorage->wItemIndex, pOffer[i].Item.m_dwDur);
		}
		else
		{
			sprintf(pszErrCode, "유닛은 팔지않습니다");
			return false;
		}
	}

	if(m_nRaceCode != -1)
	{
		if(m_nLastTradeDalant > 0)
			::eAddDalant(m_nRaceCode, m_nLastTradeDalant);
		if(m_nLastTradeGold > 0)
			::eAddGold(m_nRaceCode, m_nLastTradeGold);
	}

	return true;
}

bool CItemStore::IsBuy(BYTE byOfferNum, _sell_offer* pOffer, float fDiscountRate, char* pszErrCode)
{
	SetZeroTradeMoney();
	float fR = 1.0f - ::eGetTex(m_nRaceCode);

	//상점이 살수잇는 품목인지 확인
	for(int i = 0; i < byOfferNum; i++)
	{
		if(pOffer[i].pItem->m_byTableCode == tbl_code_key)
		{
			sprintf(pszErrCode, "이상점에는 거래할수없는 품목입니다.");			
			return false;
		}
	}
		
	for(i = 0; i < byOfferNum; i++)
	{
		//표준가격 산출
		BYTE byMoneyUnit;
		
		//구입가격 = 개당가격 * 세율 * 수량
		int nUnitPrice = (int)CalcBuyPrice(pOffer[i].pItem->m_byTableCode, pOffer[i].pItem->m_wItemIndex, &byMoneyUnit);
		//마모돼는 아이템은 내구성에 따라 가격산출
		if(::IsAbrItem(pOffer[i].pItem->m_byTableCode, pOffer[i].pItem->m_wItemIndex))
		{			
			DWORD dwStdDur = (DWORD)::GetItemDurPoint(pOffer[i].pItem->m_byTableCode, pOffer[i].pItem->m_wItemIndex);
			float fDurRate = (float)pOffer[i].pItem->m_dwDur / dwStdDur;
			if(fDurRate > 1.0f)
				fDurRate = 1.0f;

			nUnitPrice *= fDurRate;
		}

		float fBuyPrice = (nUnitPrice * fR) * pOffer[i].byAmount;
			
		if(byMoneyUnit == money_unit_dalant)
		{
			m_nLastTradeDalant += fBuyPrice;
		}
		else
		{
			m_nLastTradeGold += fBuyPrice;
		}
	}

	//할인룰 적용
	if(fDiscountRate > 0.0f && m_nLastTradeDalant > 1)
	{
		if(fDiscountRate > max_discount)
			fDiscountRate = max_discount;

		int nDiscount = m_nLastTradeDalant * fDiscountRate;
		m_nLastTradeDalant += nDiscount;
	}

	if(m_nRaceCode != -1)
	{
		if(m_nLastTradeDalant > 0)
			::eAddDalant(m_nRaceCode, m_nLastTradeDalant);
		if(m_nLastTradeGold > 0)
			::eAddGold(m_nRaceCode, m_nLastTradeGold);
	}

	return true;
}

bool CItemStore::IsRepair(BYTE byOfferNum, _repair_offer* pCard, DWORD dwHasDalant, DWORD dwHasGold, char* pszErrCode)
{
	//상점이 수리할수있는 품목인지 확인
	SetZeroTradeMoney();
	float fR = 1.0f + ::eGetTex(m_nRaceCode);

	for(int i = 0; i < byOfferNum; i++)
	{
		//수리가능한지 검사
		int nMaxDurPoint = ::GetItemDurPoint(pCard[i].pItem->m_byTableCode, pCard[i].pItem->m_wItemIndex);
		if(nMaxDurPoint <= 0)
		{
			sprintf(pszErrCode, "표준최대내구성 산출 실패");
			return false;
		}

		if(nMaxDurPoint >= pCard[i].pItem->m_dwDur)
		{
			sprintf(pszErrCode, "수리가능한 최소내구성보다 작어서 수리가 불가능합니다.");			
			return false;
		}

		int nGapPoint = nMaxDurPoint-pCard[i].pItem->m_dwDur;
		
		int nStdPrice = ::GetItemStdPrice(pCard[i].pItem->m_byTableCode, pCard[i].pItem->m_wItemIndex, m_nRaceCode, NULL);
		if(nStdPrice == -1)
		{
			sprintf(pszErrCode, "표준가격 산출 실패");
			return false;
		}
		float fRepairPrice = (nStdPrice * (float)nGapPoint/nMaxDurPoint) * fR + 0.5f;

		m_nLastTradeDalant += (int)fRepairPrice;
		if(dwHasDalant < m_nLastTradeDalant)
		{
			sprintf(pszErrCode, "보유금액이 모자랍니다.");
			return false;
		}
		pCard[i].wNewDurPoint = nMaxDurPoint;
	}

	if(m_nRaceCode != -1)
	{
		if(m_nLastTradeDalant > 0)
			::eAddDalant(m_nRaceCode, m_nLastTradeDalant);
		if(m_nLastTradeGold > 0)
			::eAddGold(m_nRaceCode, m_nLastTradeGold);
	}
	return true;
}

int CItemStore::GetLastTradeDalant()
{
	return m_nLastTradeDalant;
}

int CItemStore::GetLastTradeGold()
{
	return m_nLastTradeGold;
}

void CItemStore::SetZeroTradeMoney()
{
	m_nLastTradeDalant = 0;
	m_nLastTradeGold = 0;
}

int CItemStore::CalcSecIndex(float x, float z)
{
	_bsp_info* pBspInfo = m_pExistMap->GetBspInfo();

	float fAx = -pBspInfo->m_nMapMinSize[0]+x;
	float fAz = pBspInfo->m_nMapMaxSize[2]-z;

	int nSecX = (int)fAx/MAP_STD_SEC_SIZE;
	int nSecY = (int)fAz/MAP_STD_SEC_SIZE;

	_sec_info* pSecInfo = m_pExistMap->GetSecInfo();

	return nSecX+nSecY*pSecInfo->m_nSecNumW;
}

bool CItemStore::GetNpcRaceCode(int* OUT pnRaceCode)
{
	_npc_fld* pNpcFld = (_npc_fld*)g_Main.m_tblNPC.GetRecord(m_pRec->m_strStore_NPCcode);
	if(!pNpcFld)
	{
		return false;
	}

	*pnRaceCode = pNpcFld->m_nRace;

	return true;
}

_base_fld* CItemStore::GetNpcRecord()
{
	return g_Main.m_tblNPC.GetRecord(m_pRec->m_strStore_NPCcode);
}

float* CItemStore::GetStorePos()
{
	return m_pDum->m_pDumPos->m_fCenterPos;
}

float CItemStore::CalcSellPrice(int nGoodIndex, BYTE* pbyMoneyUnit)
{
	*pbyMoneyUnit = m_pStorageItem[nGoodIndex].byMoneyUnit;
	return m_pStorageItem[nGoodIndex].nStdPrice;
}

float CItemStore::CalcBuyPrice(BYTE byTableCode, WORD wItemIndex, BYTE* pbyMoneyUnit)
{
	int nRate = 2;

	if(m_nRaceCode != -1)	//성별이 없다.
	{
		if(!::IsItemEquipCivil(byTableCode, wItemIndex, m_nRaceCode*2) &&
			!::IsItemEquipCivil(byTableCode, wItemIndex, m_nRaceCode*2+1))
		{
			nRate = 1;
		}
	}

	float fStdPrice = ::GetItemStdPrice(byTableCode, wItemIndex, m_nRaceCode, pbyMoneyUnit);
	return fStdPrice/nRate+0.5f;
}

