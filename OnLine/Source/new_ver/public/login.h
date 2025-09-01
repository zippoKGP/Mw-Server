/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : login.h
  *owner  		: Andy
  *description  : 
  *modified     : 2005/1/5
******************************************************************************/ 

#ifndef WORLDSERVER_LOGIN_H
#define WORLDSERVER_LOGIN_H

//////////////////////////////////////////////////////////////////////////
//
#define	NET_MSG_CHECK_USER						0x00020000
#define	NET_MSG_CHECK_USER_RETURN				0x00020001
enum // NET_MSG_CHECK_USER_RETURN ��Ϣ����
{
	CHECK_USER_OK,		// ����û��˺ź�����ɹ�
	CHECK_USER_NOT,		// ���벻��
	CHECK_PWD_NOT,		// �û�������
	CHECK_USER_ERROR	// ����û��˺ź����뷢����������
};

//////////////////////////////////////////////////////////////////////////
//
typedef struct _MW_ACCOUNT
{
	char name[17]; // ��󳤶�16���ֽ�
	char passwd[17];
} MW_ACCOUNT;

//////////////////////////////////////////////////////////////////////////
//
typedef struct _MW_ACCOUNT_ID
{
	char name[17]; //��󳤶�16���ֽ�
	char passwd[17];
	int  player_id;
} MW_ACCOUNT_ID;

//////////////////////////////////////////////////////////////////////////
//
typedef struct _MW_BILLING
{
	int billing_id;
	int player_id;
	int money;
}MW_BILLING;

#endif