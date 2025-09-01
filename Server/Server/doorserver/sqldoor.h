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
const UG_PCHAR	SQL_GET_ALL_WORLDBASE	=	"select * from servers";
//����ĳworldbase������Ϊ������״̬
const UG_PCHAR	SQL_SET_WORLD_NO_ONLINE	=	"update servers set online = 0, onlinenum = 0 where id = %d";
//����ID���õ�ǰ�����������
const UG_PCHAR	SQL_SET_MAX_ONLINE	=	"update servers set maxonline = %d where id = %d";
//����ID��ȡ��ǰ�����������
const UG_PCHAR	SQL_GET_MAX_ONLINE	=	"select maxonline from servers where id = %d";
//������private_ip��IP��ͬ���������Ϊ�����߸���(��Ҫ�и�����IP(�ַ���))
const UG_PCHAR	SQL_UPDATE_ONLINE	=	"update servers set online = 1, onlinenum = 0 where id = %d";
//������private_ip��IP��ͬ������������߸������Ӹ���(��Ҫ�и�����IP(�ַ���))
const UG_PCHAR	SQL_SET_USER_NUM	=	"update servers set onlinenum = %d, maxonline = %d, history_maxonline = %d where id = %d";
//�����û��˺Ŵ�billing��ѡ���û����룬id�ͽ�Ǯ(��Ҫ���û���)
const UG_PCHAR	SQL_CHECK_USER	=	"select user_pwd,id,deposit,activate,activetime from user where user_name = \'%s\'";
//ȡ�����е�worldbase��Ϣ
const UG_PCHAR	SQL_GET_WORLDBASE	=	"select name,id,ip,port,private_ip,maxonline from servers where private_ip = \'%s\' and private_port = %d";
//��Ǯsql���
const UG_PCHAR	SQL_SET_MONEY	=	"update user_info set user_money = %d where billing_id = %d";
const UG_PCHAR	SQL_GET_MONEY	=	"select user_money from user_info where billing_id = %d";


const UG_PCHAR	SQL_CHECK_GM	=	"select id, user_pwd from user where user_name = \'%s\'";

const UG_PCHAR	SQL_MONEY_LOG_INSERT	=	"insert into moneylog (billingid, charid, worldid, begintimer, endtimer, money) values (%d, %d, %d, %d, %d, %d)";
const UG_PCHAR	SQL_MONEY_LOG_UPDATE	=	"update moneylog set endtimer = %d, money = money + %d where id = %d";

const UG_PCHAR	SQL_SET_ACTIVE_TIME_UPDATE	=	"update user set activetime = %d where id = %d";

//�û�����worldbase����ʱ,����ϴα������ļ�¼
const UGPCHAR	SQL_SELECT_UNLOCK_USERCARD	=	"select id, use_user_id, playerid, card_code, card_pwd from web_card_list where serverid = %d and use_flag = %d";
const UGPCHAR	SQL_UNLOCK_WORLDBASE_USERCARD	=	"update web_card_list set use_flag = %d where serverid = %d and use_flag = %d";
//modify by ben in 2005-10-18, add web_card_list.pocket_id, web_card_list.pocket
const UGPCHAR	SQL_SELECT_USERCARD		=	"select web_card_list.id, web_card_list.card_code, web_card_list.card_pwd, web_card_list.use_flag, web_card_type.end_time, web_card_list.pocket_id, web_card_list.pocket from web_card_list, web_card_type where card_code = '%s' and web_card_list.parent_id = web_card_type.id and web_card_list.pocket_id >= 0";
//end by ben
const UGPCHAR	SQL_OPERATE_USERCARD	=	"update web_card_list set use_user_id = %d, serverid = %d, playerid = %d, use_time = '%s', use_flag = %d where id = %d";

//----ben add in 2005-12-22------------------------------------------------------------
const UGPCHAR	SQL_GM_MONITR_LOG	=	"insert into gmmonitor (world_id,operate_time,operate_type,operate_detail) values (%d,%d,%d,\'%s\')";

//-------------------------------------------------------------------------------------
#endif//(__SQL_DOOR__)
