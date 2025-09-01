#ifndef __UG_GCP__
#define __UG_GCP__
#include "UGBenDef.h"

typedef struct _tag_ug_gcp
{
	UG_BYTE 	byVersion :7;		/* protocol version  �汾 */
	UG_BYTE		byEncrypted:1;	/* data is encrypted */
	UG_BYTE 	byReserved:1;		/* reserved ���� ���=1��ʾ����Ҫ���ϲ�Ͷ��*/
	UG_BYTE		byStuffLen:7;	/* ����ǰ������ݵĳ���*/
	UG_WORD 	wDataLen ;	/* length of data message ���ݱ�����(��������ͷ)*/
	UG_ULONG 	ulSvrType ;	/* type of  service ��������*/
	UG_ULONG 	ulSvrParam ;	/* parameter of service type �������͵Ĳ���*/
	
} UGGCP_T, *PUGGCP_T;

#endif//(__UG_GCP__)
