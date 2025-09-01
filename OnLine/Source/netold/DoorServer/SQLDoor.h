/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : SQLDoor.h
*owner        : Ben
*description  : DoorServer�����ݿ�������塣
*modified     : 2004/12/29
******************************************************************************/ 
#ifndef __SQL_DOOR__
#define __SQL_DOOR__
//-------------------------------------------------------------------------------------
//��������worldbase������Ϊ������״̬
const UG_PCHAR	SQL_SET_ALL_NO_ONLINE	=	"update servers set online = 0";
//����ĳworldbase������Ϊ������״̬
const UG_PCHAR	SQL_SET_WORLD_NO_ONLINE	=	"update servers set online = 0, onlinenum = 0 where id = %d";
//����ID���õ�ǰ�����������
const UG_PCHAR	SQL_SET_MAX_NO_ONLINE	=	"update servers set maxonline = %d where id = %d";
//������private_ip��IP��ͬ���������Ϊ�����߸���(��Ҫ�и�����IP(�ַ���))
const UG_PCHAR	SQL_UPDATE_ONLINE		=	"update servers set online = 1 where id = %d";
//������private_ip��IP��ͬ������������߸������Ӹ���(��Ҫ�и�����IP(�ַ���))
const UG_PCHAR	SQL_ADD_USER			=	"update servers set onlinenum = %d where id = %d";
//������private_ip��IP��ͬ������������߸������ٸ���(��Ҫ�и�����IP(�ַ���))
const UG_PCHAR	SQL_DROP_USER			=	"update servers set onlinenum = %d where id = %d";
//�����û��˺Ŵ�billing��ѡ���û����룬id�ͽ�Ǯ(��Ҫ���û���)
const UG_PCHAR	SQL_CHECK_USER			=	"select user_pwd,id,deposit from user where user_name = '%s'";
//ȡ�����е�worldbase��Ϣ
const UG_PCHAR	SQL_GET_WORLDBASE		=	"select name,id,ip,port,private_ip,maxonline from servers where private_ip = '%s' and private_port = %d";

//-------------------------------------------------------------------------------------
#endif//(__SQL_DOOR__)
