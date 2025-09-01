// UGMySql.cpp: implementation of the CUGMySql class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUGMySql::CUGMySql()
{
	m_pMySql = NULL;
}

CUGMySql::~CUGMySql()
{
	cleanup();
}

UG_ULONG CUGMySql::openDB(UG_PCHAR pchHost,UG_PCHAR pchUser,UG_PCHAR pchPassword,UG_PCHAR pchDB,UINT32 u32Port,UG_PCHAR pchUnixSocket,UINT32 u32ClientFlag)
{
	m_pMySql = new MYSQL;
	mysql_init(m_pMySql);
	if(mysql_real_connect(m_pMySql,pchHost,pchUser,pchPassword,pchDB,u32Port,pchUnixSocket,u32ClientFlag))
	{
		return 0;
	}
	ReleaseP<MYSQL*>(m_pMySql);
	return -1;
}

UG_ULONG CUGMySql::closeDB()
{
	cleanup();
	return 0;
}

UG_ULONG CUGMySql::query(UG_PCHAR pchQuery,UG_PVOID& pvResulty)
{
	if(m_pMySql)
	{
		if(!mysql_query(m_pMySql,pchQuery))
		{
			pvResulty = mysql_store_result(m_pMySql);
//			return mysql_affected_rows(m_pMySql);
			return 0;
		}
	}
	return -1;
}

UG_ULONG CUGMySql::query(UG_PCHAR pchQuery,UG_PVOID& pvResult,UG_ULONG& ulRows)
{
	if(m_pMySql)
	{
		if(!mysql_query(m_pMySql,pchQuery))
		{
			pvResult = mysql_store_result(m_pMySql);
			ulRows = mysql_affected_rows(m_pMySql);
			return 0;
		}
	}
	return -1;
}

const UG_CHAR* CUGMySql::getError()
{
	if(m_pMySql)
	{
		return mysql_error(m_pMySql);
	}
	return NULL;
}

UG_ULONG CUGMySql::getErrorCode()
{
	if(m_pMySql)
	{
		return mysql_errno(m_pMySql);
	}
	return -1;
}

UG_ULONG CUGMySql::cleanup()
{
	if(m_pMySql)
	{
		mysql_close(m_pMySql);
		ReleaseP(m_pMySql);
	}
	return 0;
}

UG_ULONG CUGMySql::getQueryResult(UG_PVOID pvResult,UG_ULONG ulRowIndex,UG_ULONG ulFieldIndex,UG_PCHAR& pchResult)
{
	MYSQL_RES* pResult = (MYSQL_RES*)pvResult;
	if(m_pMySql)
	{
		if(pResult)
		{
			if(ulRowIndex >= pResult->data->rows)
			{
				return -1;
			}
			if(ulFieldIndex >= pResult->data->fields)
			{
				return -1;
			}
			MYSQL_ROWS* pRow = pResult->data->data;
			for(UG_INT32 i = 0; i < ulRowIndex; i ++)
			{
				pRow = pRow->next;
			}
			pchResult = *(pRow->data + ulFieldIndex);
			return pResult->data->rows;
		}
		return -1;
	}
	return -1;
}

const UG_CHAR* CUGMySql::getClientVersion()
{
	return mysql_get_client_info();
}

const UG_CHAR* CUGMySql::getServerVersion()
{
	if(m_pMySql)
	{
		return mysql_get_server_info(m_pMySql);
	}
	return NULL;
}

UG_VOID CUGMySql::freeResult(UG_PVOID pvResult)
{
	MYSQL_RES* pResult = (MYSQL_RES*)pvResult;
	mysql_free_result(pResult);
	return;
}

UG_ULONG CUGMySql::getQueryResultRows(UG_PVOID pvResult)
{
	MYSQL_RES* pResult = (MYSQL_RES*)pvResult;
	if(pResult)
	{
		return pResult->data->rows;
	}
	return 0;
}

UG_ULONG CUGMySql::getInsertID()
{
	if(m_pMySql)
	{
		return mysql_insert_id(m_pMySql);
	}
	return 0;
}
