/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : UGSQL.h
*owner        : Ben
*description  : SQL���������ӿڣ���ͷ�ļ���mysql��libmySQL.libһ��link�������libmySQL.dllһ��ʹ��
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
	//�����ݿ⣬pchHost���ݿ����ڵ�������,pchUser�û�,pchPassword����,pchDB���ݿ�����,����ֵ0��ʾ��ȷ���������
	UG_ULONG		openDB(UG_PCHAR pchHost,UG_PCHAR pchUser,UG_PCHAR pchPassword,UG_PCHAR pchDB,UINT32 u32Port = 0,UG_PCHAR pchUnixSocket = NULL,UINT32 u32ClientFlag	= 0);
	UG_ULONG		closeDB();
	//����0��ʾ��ȷ���������,ulRows��ʾӰ�������.
	UG_ULONG		query(UG_PCHAR pchQuery,UG_PVOID& pvResult);
	UG_ULONG		query(UG_PCHAR pchQuery,UG_PVOID& pvResult,UG_ULONG& ulRows);
	//��ȡ����ļ�¼������id�š�0��ʾ
	UG_ULONG		getInsertID();
	//����query���ķ��ؽ������������ݣ��ú����ǵõ����صļ�¼������
	UG_ULONG		getQueryResultRows(UG_PVOID pvResult);
	//����query���ķ��ؽ������������ݣ��ú����ǵõ����صĵ�ulRowIndex��¼�ĵ�ulFieldIndex��ѯ�����ֵ��
	UG_ULONG		getQueryResult(UG_PVOID pvResult,UG_ULONG ulRowIndex,UG_ULONG ulFieldIndex,UG_PCHAR& pchResult);
	UG_ULONG		cleanup();
	//��queryһ��ʹ��
	UG_VOID			freeResult(UG_PVOID pvResult);
	
public:
	const UG_CHAR*		getClientVersion(); //��ÿͻ��˰汾
	const UG_CHAR*		getServerVersion(); //��÷������˰汾
	const UG_CHAR*		getError(); //��ô�����Ϣ
	UG_ULONG			getErrorCode(); //��ô������
	
private:
	UG_PVOID		m_pMySql; //CUGMySql*

};

#endif // !defined(AFX_UGSQL_H__CD8FAA77_361E_47E5_8DBE_96C1B8C12E53__INCLUDED_)
