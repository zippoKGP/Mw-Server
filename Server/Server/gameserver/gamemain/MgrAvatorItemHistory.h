#ifndef _MgrAvatorItemHistory_h_
#define _MgrAvatorItemHistory_h_

#include "StorageList.h"
#include "MyClassUtil.h"

struct _AVATOR_DATA;
class CLogFile;
struct _buy_offer;
struct _sell_offer;
struct _repair_offer;

class CMgrAvatorItemHistory
{
public:

	struct	__LOG_DATA
	{
		char	szFileName[64];
		int		nLen;
	};	

	enum	{	LOG_SIZE_10K = 10000, LOG_SIZE_1K = 1000, LOG_SIZE_200 = 200	};

	struct __LOG_DATA_10K : public __LOG_DATA
	{
		char sData[LOG_SIZE_10K];
	};

	struct __LOG_DATA_1K : public __LOG_DATA
	{
		char sData[LOG_SIZE_1K];
	};	

	struct __LOG_DATA_200 : public __LOG_DATA
	{
		char sData[LOG_SIZE_200];
	};	

public:

	char		m_szStdPath[128];
	DWORD		m_dwLastLocalDate;	//날짜별..
	DWORD		m_dwLastLocalHour;	//시간별..

	CMyTimer	m_tmrUpdateTime;
	char		m_szCurTime[32];

	enum		{	max_log_data_10K	= MAX_PLAYER/3+1, 	
					max_log_data_1K		= MAX_PLAYER/3+1,
					max_log_data_200	= MAX_PLAYER		};

	__LOG_DATA_10K	m_LogData_10K[max_log_data_10K];
	CNetIndexList	m_listLogData_10K;
	CNetIndexList	m_listLogDataEmpty_10K;

	__LOG_DATA_1K	m_LogData_1K[max_log_data_1K];
	CNetIndexList	m_listLogData_1K;
	CNetIndexList	m_listLogDataEmpty_1K;

	__LOG_DATA_200	m_LogData_200[max_log_data_200];
	CNetIndexList	m_listLogData_200;
	CNetIndexList	m_listLogDataEmpty_200;

	bool			m_bIOThread;

public:

	CMgrAvatorItemHistory();
	~CMgrAvatorItemHistory();

	void GetNewFileName(DWORD dwAvatorSerial, char* OUT pszFileName);
	void OnLoop();

	void init_load_item(_AVATOR_DATA* pLoadData, char* pszFileName);
	void close(char* pCloseCode, char* pszFileName);
	
	void pay_money(char* pszClause, DWORD dwPayDalant, DWORD dwPayGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName);
	void take_ground_item(__ITEM* pItem, DWORD dwThrower, char* pszFileName);
	void throw_ground_item(__ITEM* pItem, char* pszFileName);
	void buy_item(_buy_offer* pOffer, BYTE byOfferNum, DWORD dwCostDalant, DWORD dwCostGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName);
	void sell_item(_sell_offer* pOffer, BYTE byOfferNum, DWORD dwIncomeDalant, DWORD dwIncomeGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName);
	void repair_item(_repair_offer* pOffer, BYTE byOfferNum, DWORD dwCostDalant, DWORD dwCostGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName);
	void trade(__ITEM* pOutItem, int nOutItemNum, DWORD dwOutDalant, DWORD dwOutGold, 
							__ITEM* pInItem, int nInItemNum, DWORD dwInDalant, DWORD dwInGold, 
							DWORD dwDstSerial, DWORD dwSumDalant, DWORD dwSumGold, char* pszFileName);
	void del_unit_after_trade(DWORD dwDstSerial, _UNIT_DB_BASE::_LIST* pUnitData, char* pszFileName);
	void add_unit_after_trade(DWORD dwDstSerial, _UNIT_DB_BASE::_LIST* pUnitData, char* pszFileName);
	void grade_up_item(__ITEM* pItem, __ITEM* pTalik, __ITEM* pJewel, BYTE byJewelNum, BYTE byErrCode, DWORD dwAfterLv, char* pszFileName);
	void grade_down_item(__ITEM* pItem, __ITEM* pTalik, DWORD dwAfterLv, char* pszFileName);
	void exchange_money(DWORD dwCurDalant, DWORD dwCurGold, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName);
	void cut_item(__ITEM* pOreItem, int nOreNum, WORD* pwCuttingResBuffer, DWORD dwCostDalant, DWORD dwNewDalant, char* pszFileName);
	void make_item(__ITEM* pMaterial, BYTE* pbyMtrNum, BYTE byMaterialNum, BYTE byRetCode, __ITEM* pMakeItem, char* pszFileName);
	void cheat_alter_money(DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName);
	void cheat_add_item(__ITEM* pItem, BYTE byAddNum, char* pszFileName);
	void cheat_del_item(__ITEM* pItem, BYTE byDelNum, char* pszFileName);
	void cut_clear_item(WORD* pwCuttingResBuffer, DWORD dwAddGold, DWORD dwNewGold, char* pszFileName);
	void consume_del_item(__ITEM* pItem, char* pszFileName);
	void reward_add_money(char* pszClause, DWORD dwAddDalant, DWORD dwAddGold, DWORD dwSumDalant, DWORD dwSumGold, char* pszFileName);
	void reward_add_item(char* pszClause, __ITEM* pItem, char* pszFileName);
	void buy_unit(BYTE bySlotIndex, _UNIT_DB_BASE::_LIST* pData, DWORD* pdwConsumMoney, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName);
	void sell_unit(BYTE bySlotIndex, BYTE byFrameCode, DWORD* pdwAddMoney, DWORD dwPayDalant, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName);
	void destroy_unit(BYTE bySlotIndex, BYTE byFrameCode, char* pszFileName);
	void tuning_unit(BYTE bySlotIndex, _UNIT_DB_BASE::_LIST* pData, int* pnPayMoney, DWORD dwNewDalant, DWORD dwNewGold, char* pszFileName);
	void char_copy(DWORD dwDstSerial, char* pszFileName);


	void WriteFile(char* pszFileName, char* pszLog);
	static void IOThread(void* pv);
};



#endif