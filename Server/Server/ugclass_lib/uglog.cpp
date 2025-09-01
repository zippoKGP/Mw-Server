// UGLog.cpp: implementation of the CUGLog class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUGLog::CUGLog()
{
	m_pFile = NULL;
}

CUGLog::~CUGLog()
{
	cleanup();
}

UG_BOOL CUGLog::init(UG_PCHAR pchPathName)
{
	if(!pchPathName)
	{
		return FALSE;
	}
	if(!(m_pFile = fopen(pchPathName, "w")))
	{
		return FALSE;
	}
	fprintf((FILE*)m_pFile,getTime());
	fprintf((FILE*)m_pFile,"\r\n");
	fflush((FILE*)m_pFile);
	return TRUE;
}

UG_BOOL CUGLog::UGLog(UG_BOOL bFlush, UG_PCHAR pchMsg, ...)
{
	if(!m_pFile)
	{
		return FALSE;
	}
	static UG_CHAR szBuffer[1024];
	static UG_CHAR szBufferF[1024];
	va_list listArgs;
	va_start(listArgs, pchMsg);
	_vsnprintf(szBuffer,sizeof(szBuffer),pchMsg,listArgs);
	va_end(listArgs);
	sprintf(szBufferF,"%s  %s.\r\n",getTime(),szBuffer);
	fprintf((FILE*)m_pFile,szBufferF);
	if(bFlush)
	{
		fflush((FILE*)m_pFile);
	}
	return TRUE;
}

UG_BOOL CUGLog::cleanup()
{
	if(m_pFile)
	{
		fclose((FILE*)m_pFile);
		m_pFile = NULL;
	}
	return TRUE;
}

UG_PCHAR CUGLog::getTime()
{
	static UG_CHAR szTimer[1024];
	struct tm* pTimer = NULL;
	time_t timeLong;
	time(&timeLong);
	pTimer = localtime(&timeLong);
	sprintf(szTimer,"%d-%d-%d %d:%d:%d",pTimer->tm_year + 1900,pTimer->tm_mon + 1,pTimer->tm_mday,pTimer->tm_hour,pTimer->tm_min,pTimer->tm_sec);
	return szTimer;
}

UG_BOOL CUGLog::UGLog(UG_PCHAR pchMsg, ...)
{
	if(!m_pFile)
	{
		return FALSE;
	}
	static UG_CHAR szBuffer[1024];
	static UG_CHAR szBufferF[1024];
	va_list listArgs;
	va_start(listArgs, pchMsg);
	_vsnprintf(szBuffer,sizeof(szBuffer),pchMsg,listArgs);
	va_end(listArgs);
	sprintf(szBufferF,"%s %s\r\n",getTime(),szBuffer);
	fprintf((FILE*)m_pFile,szBufferF);
	fflush((FILE*)m_pFile);
	return TRUE;
}
