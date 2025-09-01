#ifndef _ItemStore_h_
#define _ItemStore_h_

#include "MapData.h"
#include "StoreListFld.h"
#include "MyUtil.h"
#include "CharacterDB.h"
#include "PlayerDB.h"

#define SellError_Code_Shortage		1		//������
#define SellError_Code_FundLack		2		//������ �ڱݺ���
#define SellError_Code_PriceFailure 3		//���ݻ��� ����
#define SellError_Code_SpaceLack	4		//������ �����������

#define BuyError_Code_NotDealing	1		//��޺Ұ�

class CPlayer;

struct _good_storage_info
{
	BYTE	byItemTableCode;
	WORD	wItemIndex;

	BOOL	bExist;
	BYTE	byMoneyUnit;
	int		nStdPrice;
	DWORD	dwDurPoint;

	_good_storage_info(){
		memset(this, 0, sizeof(*this));
	}
};

struct _buy_offer	//�ƹ�Ÿ�� ������ ������ �춧
{
	BYTE		byGoodIndex;	//avata
	BYTE		byGoodAmount;	//avata
	BYTE		byStorageCode;	//avata
	__ITEM		Item;			//store 
	WORD		wSerial;		//messager
};

struct _sell_offer	//�ƹ�Ÿ�� ������ ������ �ȶ�
{
	__ITEM*		pItem;	//avata
	BYTE		byAmount;	//avata
	BYTE		bySlotIndex;	//avata	
	BYTE		byStorageCode;	//avata
};

struct _repair_offer
{
	__ITEM*		pItem;	//avata
	WORD		wNewDurPoint;	//store
	BYTE		bySlotIndex;	//avata
};

class CItemStore
{
public:

	bool				m_bLive;
	int					m_nIndex;
	DWORD				m_dwSecIndex;
	int					m_nRaceCode;

	CMapData*			m_pExistMap;
	_store_dummy*		m_pDum;
	_StoreList_fld*		m_pRec;
	int					m_nStorageItemNum;
	_good_storage_info* m_pStorageItem;

private:

	int					m_nLastTradeDalant;
	int					m_nLastTradeGold;

public:
	CItemStore();
	~CItemStore();

	bool Init(int nIndex, CMapData* pExistMap, _store_dummy* pDum, _base_fld* pRec);

	bool IsSell(BYTE byOfferNum, _buy_offer* pOffer, DWORD dwHasDalant, DWORD dwHasGold, float fDiscountRate, char* pszErrCode);	//���� �� �ݾװ˻�
	bool IsBuy(BYTE byOfferNum, _sell_offer* pOffer, float fDiscountRate, char* pszErrCode);	//���� �� �ݾװ˻�
	bool IsRepair(BYTE byOfferNum, _repair_offer* pCard, DWORD dwHasDalant, DWORD dwHasGold, char* pszErrCode);	//���� �� �ݾװ˻�

	int GetLastTradeDalant();
	int GetLastTradeGold();

	_base_fld* GetNpcRecord();
	float* GetStorePos();

private:

	bool GetNpcRaceCode(int* OUT pnRaceCode);
	
	float CalcSellPrice(int nGoodIndex, BYTE* pbyMoneyUnit);
	float CalcBuyPrice(BYTE byTableCode, WORD wItemIndex, BYTE* pbyMoneyUnit);

	void SetZeroTradeMoney();
	int CalcSecIndex(float x, float z);

};

#endif