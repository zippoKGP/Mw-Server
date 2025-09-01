/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : SQLDoor.h
*owner        : Ben
*description  : DoorServer的数据库操作定义。
*modified     : 2004/12/29
******************************************************************************/ 
#ifndef __SQL_DOOR__
#define __SQL_DOOR__
//-------------------------------------------------------------------------------------
//设置所有worldbase服务器为非在线状态
const UG_PCHAR	SQL_SET_ALL_NO_ONLINE	=	"update servers set online = 0";
//设置某worldbase服务器为非在线状态
const UG_PCHAR	SQL_SET_WORLD_NO_ONLINE	=	"update servers set online = 0, onlinenum = 0 where id = %d";
//根据ID设置当前最大在线人数
const UG_PCHAR	SQL_SET_MAX_NO_ONLINE	=	"update servers set maxonline = %d where id = %d";
//设置与private_ip项IP相同的项服务器为非在线个数(需要有个数和IP(字符串))
const UG_PCHAR	SQL_UPDATE_ONLINE		=	"update servers set online = 1 where id = %d";
//设置与private_ip项IP相同的项服务器在线个数增加个数(需要有个数和IP(字符串))
const UG_PCHAR	SQL_ADD_USER			=	"update servers set onlinenum = %d where id = %d";
//设置与private_ip项IP相同的项服务器在线个数减少个数(需要有个数和IP(字符串))
const UG_PCHAR	SQL_DROP_USER			=	"update servers set onlinenum = %d where id = %d";
//根据用户账号从billing中选出用户密码，id和金钱(需要有用户名)
const UG_PCHAR	SQL_CHECK_USER			=	"select user_pwd,id,deposit from user where user_name = '%s'";
//取出所有的worldbase信息
const UG_PCHAR	SQL_GET_WORLDBASE		=	"select name,id,ip,port,private_ip,maxonline from servers where private_ip = '%s' and private_port = %d";

//-------------------------------------------------------------------------------------
#endif//(__SQL_DOOR__)
