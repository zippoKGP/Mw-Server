#ifndef _ChatMsgLogSystem_h_
#define _ChatMsgLogSystem_h_

#include "CharacterDB.h"
#include "MyClassUtil.h"
#include "NetUtil.h"

struct	__LOG_DATA
{
	char	szCharName[max_name_len+1];
	int		nMsgLen;
	char	szMsgData[max_message_size];
};	

class CChatMsgLogSystem
{
public:
																											//ºÐ·ù..					
	enum		{	update_init = -1, update_year = 0, update_month, update_day, update_hour, update_min, time_	};
	enum		{	max_log_data = MAX_PLAYER * 2	};

	CMyTimer		m_tmrUpdateFileName;
	char			m_szFileName[256];

	__LOG_DATA		m_LogData[max_log_data];
	CNetIndexList	m_listLogData;
	CNetIndexList	m_listLogDataEmpty;

	DWORD			m_byCurTime[];

	bool			m_bIOThread;

public:

	CChatMsgLogSystem();
	~CChatMsgLogSystem();

	void WriteFile(char* pszCharName, int nMsgLen, char* pszMsg);
	static void IOThread(void* pv);

	void _UpdateFilePath(int nUpdateCode);

};

#endif