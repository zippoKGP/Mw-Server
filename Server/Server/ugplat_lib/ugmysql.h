// UGMySql.h: interface for the CUGMySql class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UGMYSQL_H__E0D9460D_BB2E_4EC1_AAAC_044928C547A0__INCLUDED_)
#define AFX_UGMYSQL_H__E0D9460D_BB2E_4EC1_AAAC_044928C547A0__INCLUDED_

#include "net/UGBenDef.h"

class CUGMySql  
{
public:
	CUGMySql();
	virtual ~CUGMySql();

public:
	UG_ULONG		openDB(UG_PCHAR pchHost,UG_PCHAR pchUser,UG_PCHAR pchPassword,UG_PCHAR pchDB,UINT32 u32Port,UG_PCHAR pchUnixSocket,UINT32 u32ClientFlag);
	UG_ULONG		closeDB();
	UG_ULONG		convertString(UG_PCHAR pchNew,UG_PCHAR pchOld);
	UG_ULONG		convertString(UG_PCHAR pchNew,UG_PCHAR pchOld,UG_INT32 nOldLen);
	UG_ULONG		query(UG_PCHAR pchQuery,UG_PVOID& pvResult);
	UG_ULONG		query(UG_PCHAR pchQuery,UG_PVOID& pvResult,UG_ULONG& ulRows);
	UG_ULONG		getInsertID();
	UG_ULONG		getQueryResultRows(UG_PVOID pvResult);
	UG_ULONG		getQueryResult(UG_PVOID pvResult,UG_ULONG ulRowIndex,UG_ULONG ulFieldIndex,UG_PCHAR& pchResult);
	UG_ULONG		cleanup();
	UG_VOID			freeResult(UG_PVOID pvResult);
	
public:
	const UG_CHAR*		getClientVersion();
	const UG_CHAR*		getServerVersion();
	const UG_CHAR*		getError();
	UG_ULONG			getErrorCode();

private:
	MYSQL*			m_pMySql;
	
};

#endif // !defined(AFX_UGMYSQL_H__E0D9460D_BB2E_4EC1_AAAC_044928C547A0__INCLUDED_)
