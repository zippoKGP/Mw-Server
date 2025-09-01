/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : UGSQL.cpp
*owner        : Ben
*description  : SQL²Ù×÷º¯Êý
*modified     : 2004/12/30
******************************************************************************/ 

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUGSQL::CUGSQL()
{
	m_pMySql = NULL;
}

CUGSQL::~CUGSQL()
{
	cleanup();
}

UG_ULONG CUGSQL::openDB(UG_PCHAR pchHost,UG_PCHAR pchUser,UG_PCHAR pchPassword,UG_PCHAR pchDB,UINT32 u32Port,UG_PCHAR pchUnixSocket,UINT32 u32ClientFlag)
{
	if(!m_pMySql)
	{
		CUGMySql* pSql = new CUGMySql;
		m_pMySql = pSql;
		return pSql->openDB(pchHost,pchUser,pchPassword,pchDB,u32Port,pchUnixSocket,u32ClientFlag);
	}
	return -1;
}

UG_ULONG CUGSQL::closeDB()
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		UG_ULONG ulRet = pSql->closeDB();
		ReleaseP<CUGMySql*>(pSql);
		m_pMySql = NULL;
		return ulRet;
	}
	return -1;
}

UG_ULONG CUGSQL::query(UG_PCHAR pchQuery,UG_PVOID& pvResulty)
{
	pvResulty = NULL;
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		return pSql->query(pchQuery,pvResulty);
	}
	return -1;
}

UG_ULONG CUGSQL::query(UG_PCHAR pchQuery,UG_PVOID& pvResult,UG_ULONG& ulRows)
{
	pvResult = NULL;
	ulRows = 0;
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		return pSql->query(pchQuery,pvResult,ulRows);
	}
	return -1;
}

UG_ULONG CUGSQL::getQueryResult(UG_PVOID pvResult,UG_ULONG ulRowIndex,UG_ULONG ulFieldIndex,UG_PCHAR& pchResult)
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		return pSql->getQueryResult(pvResult,ulRowIndex,ulFieldIndex,pchResult);
	}
	return -1;
}

const UG_CHAR* CUGSQL::getError()
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		return pSql->getError();
	}
	return NULL;
}

UG_ULONG CUGSQL::getErrorCode()
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		return pSql->getErrorCode();
	}
	return -1;
}

UG_ULONG CUGSQL::cleanup()
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		ReleaseP<CUGMySql*>(pSql);
		m_pMySql = NULL;
	}
	return 0;
}

const UG_CHAR* CUGSQL::getClientVersion()
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		return pSql->getClientVersion();
	}
	return NULL;
}

const UG_CHAR* CUGSQL::getServerVersion()
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		return pSql->getServerVersion();
	}
	return NULL;
}

UG_VOID CUGSQL::freeResult(UG_PVOID pvResult)
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		if(pvResult)
		{
			pSql->freeResult(pvResult);
		}
	}
	return;
}

UG_ULONG CUGSQL::getQueryResultRows(UG_PVOID pvResult)
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		return pSql->getQueryResultRows(pvResult);
	}
	return -1;
}

UG_ULONG CUGSQL::getInsertID()
{
	CUGMySql* pSql = (CUGMySql*)m_pMySql;
	if(pSql)
	{
		return pSql->getInsertID();
	}
	return 0;
}
