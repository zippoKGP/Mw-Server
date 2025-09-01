#include "stdafx.h"
#include "MgrAvatorLvHistory.h"
#include "MyUtil.h"
#include "CharacterDB.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"
#include <process.h>

static char sBuf[128];
static char sData[10000];

static void _IOFileWrite(char* pszFileName, int nLen, char* pszData);

CMgrAvatorLvHistory::CMgrAvatorLvHistory()
{
	m_dwLastLocalDate = 0;
	m_dwLastLocalHour = 0;

	sprintf(m_szStdPath, ".\\History\\Lv\\");
	::CreateDirectory(m_szStdPath, NULL);

	_strtime(m_szCurTime);
	m_szCurTime[5] = NULL;//분까지만 기록
	m_tmrUpdateTime.BeginTimer(60*1000);	//1분

	m_listLogData_2K.SetList(max_log_data_2K);
	m_listLogDataEmpty_2K.SetList(max_log_data_2K);
	for(int i = 0; i < max_log_data_2K; i++)
		m_listLogDataEmpty_2K.PushNode_Back(i);

	m_listLogData_1K.SetList(max_log_data_1K);
	m_listLogDataEmpty_1K.SetList(max_log_data_1K);
	for(i = 0; i < max_log_data_1K; i++)
		m_listLogDataEmpty_1K.PushNode_Back(i);

	m_listLogData_200.SetList(max_log_data_200);
	m_listLogDataEmpty_200.SetList(max_log_data_200);
	for(i = 0; i < max_log_data_200; i++)
		m_listLogDataEmpty_200.PushNode_Back(i);

	m_bIOThread = true;
	::_beginthread(IOThread, 0, (void*)this);
}

CMgrAvatorLvHistory::~CMgrAvatorLvHistory()
{
	m_bIOThread = false;
}

void CMgrAvatorLvHistory::GetNewFileName(DWORD dwAvatorSerial, char* OUT pszFileName)
{	
	DWORD dwNewDate = ::GetLocalDate();
	if(m_dwLastLocalDate != dwNewDate)
	{//상위 디렉토리생성
		char szPath[128];
		sprintf(szPath, "%s\\%d", m_szStdPath, dwNewDate);
		::CreateDirectory(szPath, NULL);
		m_dwLastLocalDate = dwNewDate;
	}

	DWORD dwNewHour = ::GetCurrentHour();
	if(m_dwLastLocalHour != dwNewHour)
	{//하위 디렉토리생성
		char szPath[128];
		sprintf(szPath, "%s\\%d\\%d", m_szStdPath, m_dwLastLocalDate, dwNewHour);
		::CreateDirectory(szPath, NULL);
		m_dwLastLocalHour = dwNewHour;
	}

	//시간.
	char szTime[7], szHour[3], szMin[3], szSec[3];
	if(dwNewHour > 9)
		sprintf(szHour, "%d", dwNewHour);
	else
		sprintf(szHour, "0%d", dwNewHour);

	DWORD dwMin = ::GetCurrentMin();		
	if(dwMin > 9)
		sprintf(szMin, "%d", dwMin);
	else
		sprintf(szMin, "0%d", dwMin);

	DWORD dwSec = ::GetCurrentSec();
	if(dwSec > 9)
		sprintf(szSec, "%d", dwSec);
	else
		sprintf(szSec, "0%d", dwSec);

	sprintf(szTime, "%s%s%s", szHour, szMin, szSec);

	sprintf(pszFileName, "%s\\%d\\%d\\%d_%s.lhi", m_szStdPath, m_dwLastLocalDate, m_dwLastLocalHour, dwAvatorSerial, szTime);
}

void CMgrAvatorLvHistory::OnLoop()
{
	if(m_tmrUpdateTime.CountingTimer())
	{
		_strtime(m_szCurTime);
		m_szCurTime[5] = NULL;//분까지만 기록
	}
}

void CMgrAvatorLvHistory::start_mastery(char* pszAvatorName, DWORD dwLv, double dExp, _STAT_DB_BASE* pData, char* pszFileName)
{
	sData[0] = NULL;

	//기본정보
	sprintf(sBuf, "%s lv:%d /exp:%f [%s]\n", pszAvatorName, dwLv, dExp, m_szCurTime);
	strcat(sData, sBuf);

	//무기
	for(int i = 0; i < WP_CLASS_NUM; i++)
	{
		if(pData->m_dwDamWpCnt[i] > 0)
		{
			sprintf(sBuf, "\tW%d: %d\n", i, pData->m_dwDamWpCnt[i]); 
			strcat(sData, sBuf);
		}
	}

	//방어
	if(pData->m_dwDefenceCnt > 0)
	{
		sprintf(sBuf, "\tD: %d\n", pData->m_dwDefenceCnt); 
		strcat(sData, sBuf);
	}

	//방패
	if(pData->m_dwShieldCnt > 0)
	{
		sprintf(sBuf, "\tP: %d\n", pData->m_dwShieldCnt); 
		strcat(sData, sBuf);
	}

	//스킬
	for(i = 0; i < max_skill_num; i++)
	{
		if(pData->m_dwSkillCum[i] > 0)
		{
			sprintf(sBuf, "\tS%d: %d\n", i, pData->m_dwSkillCum[i]); 
			strcat(sData, sBuf);
		}
	}

	//포스
	for(i = 0; i < force_mastery_num; i++)
	{
		if(pData->m_dwForceCum[i] > 0)
		{
			sprintf(sBuf, "\tF%d: %d\n", i, pData->m_dwForceCum[i]); 
			strcat(sData, sBuf);
		}
	}

	//제작
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		if(pData->m_dwMakeCum[i] > 0)
		{
			sprintf(sBuf, "\tM%d: %d\n", i, pData->m_dwMakeCum[i]); 
			strcat(sData, sBuf);
		}
	}

	//종족특화
	if(pData->m_dwSpecialCum > 0)
	{
		sprintf(sBuf, "\tR: %d\n", pData->m_dwSpecialCum); 
		strcat(sData, sBuf);
	}

	sprintf(sBuf, "\n", pData->m_dwSpecialCum); 
	strcat(sData, sBuf);

	WriteFile(pszFileName, sData);
}

//15분마다 변경됀 마스터리 기록
void CMgrAvatorLvHistory::update_mastery(DWORD dwLv, double dExp, _STAT_DB_BASE* pData, bool* pbAlter, char* pszFileName, bool bEnd)
{
	sData[0] = NULL;

	if(!bEnd)
		sprintf(sBuf, "UPDATE lv:%d/ exp:%f [%s]\n", dwLv, dExp, m_szCurTime); 
	else
		sprintf(sBuf, "END lv:%d/ exp:%f [%s]\n", dwLv, dExp, m_szCurTime); 
	strcat(sData, sBuf);

	//무기
	for(int i = 0; i < WP_CLASS_NUM; i++)
	{
		if(pbAlter[_STAT_DB_BASE::DATAPOS_WPCNT+1])
		{
			sprintf(sBuf, "\tW%d: %d\n", i, pData->m_dwDamWpCnt[i]); 
			strcat(sData, sBuf);
		}
	}

	//방어
	if(pbAlter[_STAT_DB_BASE::DATAPOS_DEF])
	{
		sprintf(sBuf, "\tD: %d\n", pData->m_dwDefenceCnt); 
		strcat(sData, sBuf);
	}

	//방패
	if(pbAlter[_STAT_DB_BASE::DATAPOS_SHLD])
	{
		sprintf(sBuf, "\tP: %d\n", pData->m_dwShieldCnt); 
		strcat(sData, sBuf);
	}

	//스킬
	for(i = 0; i < max_skill_num; i++)
	{
		if(pbAlter[_STAT_DB_BASE::DATAPOS_SKILL+i])
		{
			sprintf(sBuf, "\tS%d: %d\n", i, pData->m_dwSkillCum[i]); 
			strcat(sData, sBuf);
		}
	}

	//포스
	for(i = 0; i < force_mastery_num; i++)
	{
		if(pbAlter[_STAT_DB_BASE::DATAPOS_FORCE+i])
		{
			sprintf(sBuf, "\tF%d: %d\n", i, pData->m_dwForceCum[i]); 
			strcat(sData, sBuf);
		}
	}

	//제작
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		if(pbAlter[_STAT_DB_BASE::DATAPOS_MAKE+i])
		{
			sprintf(sBuf, "\tM%d: %d\n", i, pData->m_dwMakeCum[i]); 
			strcat(sData, sBuf);
		}
	}

	//특화
	if(pbAlter[_STAT_DB_BASE::DATAPOS_SPECIAL])
	{
		sprintf(sBuf, "\tR: %d\n", pData->m_dwSpecialCum); 
		strcat(sData, sBuf);
	}

	sprintf(sBuf, "\n", pData->m_dwSpecialCum); 
	strcat(sData, sBuf);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorLvHistory::upgrade_lv(DWORD dwLv, char* pszFileName)
{
	sprintf(sData, "LV UP %d [%s]\n\n", dwLv, m_szCurTime); 

	WriteFile(pszFileName, sData);
}

void CMgrAvatorLvHistory::down_exp(double dOldExp, double dNewExp, char* pszFileName)
{
	sprintf(sData, "EXP DOWN %f -> %f [%s]\n\n", dOldExp, dNewExp, m_szCurTime); 

	WriteFile(pszFileName, sData);
}

void CMgrAvatorLvHistory::char_copy(DWORD dwDstSerial, char* pszFileName)
{
	sprintf(sData, "캐릭터카피: dst: %d\n", dwDstSerial);

	WriteFile(pszFileName, sData);
}

void CMgrAvatorLvHistory::WriteFile(char* pszFileName, char* pszLog)
{
	static DWORD dwNodeIndex;
		
	//사이즈를 계산
	DWORD	dwLen = strlen(pszLog);	

	if(dwLen < LOG_SIZE_200)
	{
		if(m_listLogDataEmpty_200.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_200* p =  &m_LogData_200[dwNodeIndex];

			strcpy(p->szFileName, pszFileName);
			p->nLen = dwLen;
			memcpy(p->sData, pszLog, dwLen);
			p->sData[dwLen] = NULL;

			m_listLogData_200.PushNode_Back(dwNodeIndex);
			return;
		}
	}
	else if(dwLen < LOG_SIZE_1K)
	{
		if(m_listLogDataEmpty_1K.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_1K* p =  &m_LogData_1K[dwNodeIndex];

			strcpy(p->szFileName, pszFileName);
			p->nLen = dwLen;
			memcpy(p->sData, pszLog, dwLen);
			p->sData[dwLen] = NULL;

			m_listLogData_1K.PushNode_Back(dwNodeIndex);
			return;
		}
	}
	else if(dwLen < LOG_SIZE_2K)
	{
		if(m_listLogDataEmpty_2K.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_2K* p =  &m_LogData_2K[dwNodeIndex];

			strcpy(p->szFileName, pszFileName);
			p->nLen = dwLen;
			memcpy(p->sData, pszLog, dwLen);
			p->sData[dwLen] = NULL;

			m_listLogData_2K.PushNode_Back(dwNodeIndex);
			return;
		}
	}

	//바로..
	_IOFileWrite(pszFileName, dwLen, pszLog);
}

void CMgrAvatorLvHistory::IOThread(void* pv)
{
	CMgrAvatorLvHistory* pDoc = (CMgrAvatorLvHistory*)pv;
	DWORD dwNodeIndex;

	while(pDoc->m_bIOThread)
	{
		while(pDoc->m_listLogData_2K.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_2K* p =  &pDoc->m_LogData_2K[dwNodeIndex];

			_IOFileWrite(p->szFileName, p->nLen, p->sData);

			pDoc->m_listLogDataEmpty_2K.PushNode_Back(dwNodeIndex);
			Sleep(0);
		}

		while(pDoc->m_listLogData_1K.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_1K* p =  &pDoc->m_LogData_1K[dwNodeIndex];

			_IOFileWrite(p->szFileName, p->nLen, p->sData);

			pDoc->m_listLogDataEmpty_1K.PushNode_Back(dwNodeIndex);
			Sleep(0);
		}

		while(pDoc->m_listLogData_200.PopNode_Front(&dwNodeIndex))
		{
			__LOG_DATA_200* p =  &pDoc->m_LogData_200[dwNodeIndex];

			_IOFileWrite(p->szFileName, p->nLen, p->sData);

			pDoc->m_listLogDataEmpty_200.PushNode_Back(dwNodeIndex);
			Sleep(0);
		}

		Sleep(1);
	}
	::_endthreadex(0);
}

void _IOFileWrite(char* pszFileName, int nLen, char* pszData)
{
	HANDLE hFile = ::CreateFile(pszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		::SetFilePointer(hFile, 0, NULL, FILE_END);

		DWORD dwWrite;
		::WriteFile(hFile, pszData, nLen, &dwWrite, NULL);

		::CloseHandle(hFile);	
	}	
}