/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2004 ALL RIGHTS RESERVED.

*file name    : UGDef.h
*owner        : Ben
*description  : �����������Ͷ����ļ�
*modified     : 2004/09/15
******************************************************************************/ 

#ifndef __UG_DEFINE_DATA_TYPE_HEAD_FILE__
#define __UG_DEFINE_DATA_TYPE_HEAD_FILE__
//-------------------------------------------------------------------------------------
#ifdef	WIN32					//Window Platform
#pragma warning(disable:4786)
#include <windows.h>
#endif
//-------------------------------------------------------------------------------------
#ifdef	__LINUX__				//Linux Platform
#define	UG_FUN_API				__cdecl
#define	UG_FUN_CALLBACK			__cdecl
#else							//Window Platform
#define	UG_FUN_API				__stdcall
#define	UG_FUN_CALLBACK			__stdcall
#ifdef	UG_DLL_IMPORT
#define	UG_DLL_API				__declspec(dllimport)
#else
#define	UG_DLL_API				__declspec(dllexport)
#endif
#endif

//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
typedef	char					UG_INT8; //n8��ͷ
typedef	short					UG_INT16; //n16��ͷ
typedef	int						UG_INT32; //n32��ͷ
typedef	unsigned char			UG_UINT8; //u8��ͷ
typedef	unsigned short			UG_UINT16; //u16��ͷ
typedef	unsigned int			UG_UINT32; //u32��ͷ
typedef	long					UG_LONG; //l��ͷ
typedef	unsigned long			UG_ULONG; //ul��ͷ
typedef	float					UG_FLOAT32; //f32��ͷ
typedef	double					UG_FLOAT64; //f64��ͷ
typedef	unsigned char			UG_BYTE; //by��ͷ
typedef	int						UG_BOOL; //b��ͷ
typedef void					UG_VOID;

//-------------------------------------------------------------------------------------
typedef	UG_INT8					*UG_PINT8; //pn8��ͷ
typedef	UG_INT16				*UG_PINT16; //pn16��ͷ
typedef	UG_INT32				*UG_PINT32; //pn32��ͷ
typedef	UG_UINT8				*UG_PUINT8; //pu8��ͷ
typedef	UG_UINT16				*UG_PUINT16; //pu16��ͷ
typedef	UG_UINT32				*UG_PUINT32; //pu32��ͷ
typedef	UG_LONG					*UG_PLONG; //pl��ͷ
typedef	UG_ULONG				*UG_PULONG; //pul��ͷ
typedef	UG_FLOAT32				*UG_PFLOAT32; //pf32��ͷ
typedef	UG_FLOAT64				*UG_PFLOAT64; //pf64��ͷ
typedef	UG_BYTE					*UG_PBYTE; //pby��ͷ
typedef	UG_BOOL					*UG_PBOOL; //pb��ͷ
typedef UG_VOID					*UG_PVOID;

//-------------------------------------------------------------------------------------
typedef	char					UG_CHAR; //c��ͷ,������sz��ͷ
typedef	unsigned short			UG_WCHAR; //wc��ͷ,������wsz��ͷ

//-------------------------------------------------------------------------------------
typedef	UG_CHAR					*UG_PCHAR; //pc��ͷ == LPSTR, LPTSTR, PTSTR
typedef	const UG_CHAR			*UG_PCCHAR; //pcc��ͷ == LPCTSTR, LPCSTR
typedef	UG_WCHAR				*UG_PWCHAR; //pwc��ͷ == LPWSTR
typedef	const UG_WCHAR			*UG_PCWCHAR; //pcwc��ͷ

//-------------------------------------------------------------------------------------
#ifdef __LINUX__
typedef unsigned short			UG_WORD; //w��ͷ
typedef unsigned long			UG_DWORD; //dw��ͷ
typedef UG_WORD					*UG_PWORD; //pw��ͷ
typedef UG_DWORD				*UG_PDWORD; //pdw��ͷ
#define TRUE					1
#define FALSE					0
#else
typedef WORD					UG_WORD; //w��ͷ
typedef DWORD					UG_DWORD; //dw��ͷ
typedef UG_WORD					*UG_PWORD; //pw��ͷ
typedef UG_DWORD				*UG_PDWORD; //pdw��ͷ
#endif
//-------------------------------------------------------------------------------------
#endif //( __UG_DEFINE_DATA_TYPE_HEAD_FILE__)
