#ifndef __UG_GCP__
#define __UG_GCP__
#include "UGBenDef.h"

typedef struct _tag_ug_gcp
{
	UG_BYTE 	byVersion :7;		/* protocol version  版本 */
	UG_BYTE		byEncrypted:1;	/* data is encrypted */
	UG_BYTE 	byReserved:1;		/* reserved 保留 如果=1表示不需要向上层投递*/
	UG_BYTE		byStuffLen:7;	/* 加密前填充数据的长度*/
	UG_WORD 	wDataLen ;	/* length of data message 数据报长度(不包括报头)*/
	UG_ULONG 	ulSvrType ;	/* type of  service 服务类型*/
	UG_ULONG 	ulSvrParam ;	/* parameter of service type 服务类型的参数*/
	
} UGGCP_T, *PUGGCP_T;

#endif//(__UG_GCP__)
