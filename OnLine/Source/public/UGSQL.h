/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : UGSQL.h
*owner        : Ben
*description  : SQL操作函数接口，该头文件和mysql的libmySQL.lib一起link，程序和libmySQL.dll一起使用
*modified     : 2004/12/30
******************************************************************************/ 

#if !defined(AFX_UGSQL_H__CD8FAA77_361E_47E5_8DBE_96C1B8C12E53__INCLUDED_)
#define AFX_UGSQL_H__CD8FAA77_361E_47E5_8DBE_96C1B8C12E53__INCLUDED_

#include "net/UGBenDef.h"

class CUGSQL  
{
public:
	CUGSQL();
	virtual ~CUGSQL();

public:
	//打开数据库，pchHost数据库所在电脑名称,pchUser用户,pchPassword密码,pchDB数据库名称,返回值0表示正确，否则错误
	UG_ULONG		openDB(UG_PCHAR pchHost,UG_PCHAR pchUser,UG_PCHAR pchPassword,UG_PCHAR pchDB,UINT32 u32Port = 0,UG_PCHAR pchUnixSocket = NULL,UINT32 u32ClientFlag	= 0);
	UG_ULONG		closeDB();
	//返回0表示正确，否则错误,ulRows表示影响的行数.
	UG_ULONG		query(UG_PCHAR pchQuery,UG_PVOID& pvResult);
	UG_ULONG		query(UG_PCHAR pchQuery,UG_PVOID& pvResult,UG_ULONG& ulRows);
	//获取插入的纪录的自增id号。0表示
	UG_ULONG		getInsertID();
	//根据query语句的返回结果分析结果数据，该函数是得到返回的记录个数。
	UG_ULONG		getQueryResultRows(UG_PVOID pvResult);
	//根据query语句的返回结果分析结果数据，该函数是得到返回的第ulRowIndex记录的第ulFieldIndex查询的域的值。
	UG_ULONG		getQueryResult(UG_PVOID pvResult,UG_ULONG ulRowIndex,UG_ULONG ulFieldIndex,UG_PCHAR& pchResult);
	UG_ULONG		cleanup();
	//和query一起使用
	UG_VOID			freeResult(UG_PVOID pvResult);
	
public:
	const UG_CHAR*		getClientVersion(); //获得客户端版本
	const UG_CHAR*		getServerVersion(); //获得服务器端版本
	const UG_CHAR*		getError(); //获得错误信息
	UG_ULONG			getErrorCode(); //获得错误代码
	
private:
	UG_PVOID		m_pMySql; //CUGMySql*

};

#endif // !defined(AFX_UGSQL_H__CD8FAA77_361E_47E5_8DBE_96C1B8C12E53__INCLUDED_)
