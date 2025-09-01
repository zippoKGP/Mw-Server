#ifndef _MgrAvatorLvHistory_h_
#define _MgrAvatorLvHistory_h_

#include "MyClassUtil.h"
#include "NetUtil.h"
#include "CharacterDB.h"


class CMgrAvatorLvHistory
{
public:

	struct	__LOG_DATA
	{
		char	szFileName[64];
		int		nLen;
	};	

	enum	{	LOG_SIZE_2K = 2000, LOG_SIZE_1K = 1000, LOG_SIZE_200 = 200	};

	struct __LOG_DATA_2K : public __LOG_DATA
	{
		char sData[LOG_SIZE_2K];
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

	enum		{	max_log_data_2K		= MAX_PLAYER/3+1, 	
					max_log_data_1K		= MAX_PLAYER/3+1,
					max_log_data_200	= MAX_PLAYER		};

	__LOG_DATA_2K	m_LogData_2K[max_log_data_2K];
	CNetIndexList	m_listLogData_2K;
	CNetIndexList	m_listLogDataEmpty_2K;

	__LOG_DATA_1K	m_LogData_1K[max_log_data_1K];
	CNetIndexList	m_listLogData_1K;
	CNetIndexList	m_listLogDataEmpty_1K;

	__LOG_DATA_200	m_LogData_200[max_log_data_200];
	CNetIndexList	m_listLogData_200;
	CNetIndexList	m_listLogDataEmpty_200;

	bool			m_bIOThread;

public:

	CMgrAvatorLvHistory();
	~CMgrAvatorLvHistory();

	void GetNewFileName(DWORD dwAvatorSerial, char* OUT pszFileName);
	void OnLoop();

	void start_mastery(char* pszAvatorName, DWORD dwLv, double dExp, _STAT_DB_BASE* pData, char* pszFileName);
	void update_mastery(DWORD dwLv, double dExp, _STAT_DB_BASE* pData, bool* pbAlter, char* pszFileName, bool bEnd = false);
	void upgrade_lv(DWORD dwLv, char* pszFileName);
	void down_exp(double dOldExp, double dNewExp, char* pszFileName);
	void char_copy(DWORD dwDstSerial, char* pszFileName);

	void WriteFile(char* pszFileName, char* pszLog);
	static void IOThread(void* pv);
};



#endif