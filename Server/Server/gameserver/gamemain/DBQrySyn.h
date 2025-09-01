#ifndef _DBQrySyn_h_
#define _DBQrySyn_h_

#include "CharacterDB.h"
#include "EconomySystem.h"

#define max_db_syn_data_num		MAX_PLAYER*2

struct _DB_QRY_SYN_DATA
{
	enum	{	max_data_size = sizeof(_AVATOR_DATA)*2+100	};
	enum	{	qry_case_reged, qry_case_insert, qry_case_delete, qry_case_load, qry_case_save, qry_case_logout, qry_case_lobby, 
				log_case_lv, log_case_usernum, log_case_economy	};

	bool	m_bUse;
	bool	m_bLoad;

	DWORD	m_dwAccountSerial;
	_CLID	m_idWorld;		
	BYTE	m_byQryCase;

	BYTE	m_byResult;
	char	m_sData[max_data_size];	
	
	_DB_QRY_SYN_DATA()	
	{
		m_bUse = false;
		m_bLoad = false;
	}
};

struct _qry_sheet_reged
{
	DWORD	dwAccountSerial;
	_REGED	RegedData[MAX_CHAR_PER_WORLDUSER];	

	int size()
	{
		return sizeof(*this);
	}
};

struct _qry_sheet_insert
{
	DWORD				dwAccountSerial;
	char				szAccountID[max_id_len+1];
	_REGED_AVATOR_DB	InsertData;

	int size()
	{
		return sizeof(*this);
	}
};

struct _qry_sheet_delete
{
	BYTE	bySlotIndex;
	BYTE	byRaceCode;
	DWORD	dwAvatorSerial;

	int size()
	{
		return sizeof(*this);
	}
};

struct _qry_sheet_load
{
	DWORD			dwAvatorSerial;
	_AVATOR_DATA	LoadData;
	char			szHBKFileName[64];

	int size()
	{
		return sizeof(*this);
	}
};

struct _qry_sheet_logout
{
	DWORD			dwAvatorSerial;
	_AVATOR_DATA	NewData;
	_AVATOR_DATA	OldData;
	char			szHBKFileName[64];
	bool			bCheckLowHigh;

	int size()
	{
		return sizeof(*this);
	}
};

struct _qry_sheet_lobby
{
	DWORD			dwAvatorSerial;
	_AVATOR_DATA	NewData;
	_AVATOR_DATA	OldData;
	char			szHBKFileName[64];

	int size()
	{
		return sizeof(*this);
	}
};

struct _log_sheet_lv
{
	DWORD			dwAvatorSerial;
	BYTE			byLv;
	DWORD			dwTotalPlayMin;

	int size()
	{
		return sizeof(*this);
	}
};

struct _log_sheet_usernum
{
	int nAveragePerHour;
	int nMaxPerHour;

	int size()
	{
		return sizeof(*this);
	}
};

struct _log_sheet_economy
{
	DWORD	dwDate;							//날짜	
	double	dTradeGold[RACE_NUM];			//총거래골드
	double	dTradeDalant[RACE_NUM];			//총거래달란트
	int		nMgrValue;						//관리값

	int size()
	{
		return sizeof(*this);
	}
};

#endif