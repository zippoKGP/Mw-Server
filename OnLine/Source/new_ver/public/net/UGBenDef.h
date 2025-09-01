/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2004 ALL RIGHTS RESERVED.

*file name    : UGDef.h
*owner        : Ben
*description  : 基本数据类型定义文件
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
typedef	char					UG_INT8; //n8开头
typedef	short					UG_INT16; //n16开头
typedef	int						UG_INT32; //n32开头
typedef	unsigned char			UG_UINT8; //u8开头
typedef	unsigned short			UG_UINT16; //u16开头
typedef	unsigned int			UG_UINT32; //u32开头
typedef	long					UG_LONG; //l开头
typedef	unsigned long			UG_ULONG; //ul开头
typedef	float					UG_FLOAT32; //f32开头
typedef	double					UG_FLOAT64; //f64开头
typedef	unsigned char			UG_BYTE; //by开头
typedef	int						UG_BOOL; //b开头
typedef void					UG_VOID;

//-------------------------------------------------------------------------------------
typedef	UG_INT8					*UG_PINT8; //pn8开头
typedef	UG_INT16				*UG_PINT16; //pn16开头
typedef	UG_INT32				*UG_PINT32; //pn32开头
typedef	UG_UINT8				*UG_PUINT8; //pu8开头
typedef	UG_UINT16				*UG_PUINT16; //pu16开头
typedef	UG_UINT32				*UG_PUINT32; //pu32开头
typedef	UG_LONG					*UG_PLONG; //pl开头
typedef	UG_ULONG				*UG_PULONG; //pul开头
typedef	UG_FLOAT32				*UG_PFLOAT32; //pf32开头
typedef	UG_FLOAT64				*UG_PFLOAT64; //pf64开头
typedef	UG_BYTE					*UG_PBYTE; //pby开头
typedef	UG_BOOL					*UG_PBOOL; //pb开头
typedef UG_VOID					*UG_PVOID;

//-------------------------------------------------------------------------------------
typedef	char					UG_CHAR; //c开头,数组以sz开头
typedef	unsigned short			UG_WCHAR; //wc开头,数组以wsz开头

//-------------------------------------------------------------------------------------
typedef	UG_CHAR					*UG_PCHAR; //pc开头 == LPSTR, LPTSTR, PTSTR
typedef	const UG_CHAR			*UG_PCCHAR; //pcc开头 == LPCTSTR, LPCSTR
typedef	UG_WCHAR				*UG_PWCHAR; //pwc开头 == LPWSTR
typedef	const UG_WCHAR			*UG_PCWCHAR; //pcwc开头

//-------------------------------------------------------------------------------------
#ifdef __LINUX__
typedef unsigned short			UG_WORD; //w开头
typedef unsigned long			UG_DWORD; //dw开头
typedef UG_WORD					*UG_PWORD; //pw开头
typedef UG_DWORD				*UG_PDWORD; //pdw开头
#define TRUE					1
#define FALSE					0
#else
typedef WORD					UG_WORD; //w开头
typedef DWORD					UG_DWORD; //dw开头
typedef UG_WORD					*UG_PWORD; //pw开头
typedef UG_DWORD				*UG_PDWORD; //pdw开头
#endif
//-------------------------------------------------------------------------------------
#endif //( __UG_DEFINE_DATA_TYPE_HEAD_FILE__)
