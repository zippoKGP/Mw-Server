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
enum // NET_MSG_CHECK_USER_RETURN 消息参数
{
	CHECK_USER_OK,		// 检查用户账号和密码成功
	CHECK_USER_NOT,		// 密码不对
	CHECK_PWD_NOT,		// 用户不存在
	CHECK_USER_ERROR	// 检查用户账号和密码发生其他错误
};

//////////////////////////////////////////////////////////////////////////
//
typedef struct _MW_ACCOUNT
{
	char name[17]; // 最大长度16个字节
	char passwd[17];
} MW_ACCOUNT;

//////////////////////////////////////////////////////////////////////////
//
typedef struct _MW_ACCOUNT_ID
{
	char name[17]; //最大长度16个字节
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