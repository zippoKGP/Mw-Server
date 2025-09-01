/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : UGProgram.h
*owner        : Ben
*description  : 程序主类，包含异常处理，内存使用等。
*modified     : 2004/12/20
******************************************************************************/ 


#if !defined(AFX_UGPROGRAM_H__F7F9CDC5_FB50_4AD1_973D_3A6AA2BCE4DB__INCLUDED_)
#define AFX_UGPROGRAM_H__F7F9CDC5_FB50_4AD1_973D_3A6AA2BCE4DB__INCLUDED_

#include "UGBenDef.h"

typedef UG_ULONG (UG_FUN_CALLBACK *UGMain)(UG_PVOID pvKey); //定义回调函数时必须加UG_FUN_CALLBACK的调用方式，否则调用会出错
typedef UG_ULONG (UG_FUN_CALLBACK *UGException)(UG_PVOID pvKey,UG_ULONG ulErrorCode);
typedef UG_ULONG (UG_FUN_CALLBACK *UGMemoryLeaked)(UG_PVOID pvKey,UG_PCHAR pchError);

class CUGProgram  
{
public:
	CUGProgram();
	virtual ~CUGProgram();

public:
	//定义异常处理类，当函数fnMain发生异常时从fnException退出
	UG_ULONG		setExceptionFun(UGMain fnMain,UGException fnException,UG_PVOID pvKey);
	UG_ULONG		cleanup();

public:
	UG_ULONG		init(UGMemoryLeaked fnLeaked,UG_PVOID pvKey); //当内存泄漏时，从该函数导出
	UG_ULONG		newMem(UG_PVOID pvMem,UG_ULONG ulLen,MEM_TYPE ulType,UG_PCHAR pchFile,UG_ULONG ulLine); //外部不能调用该函数,通过UGNew调用
	MEM_TYPE		delMem(UG_PVOID pvMem); //外部不能调用该函数,通过UGDel调用

protected:
	UG_ULONG		cleanupMem();
	
private:
	map<UG_PVOID,UG_PVOID>		m_mapMem;
	UGMemoryLeaked				m_fnLeaked;
	UG_PVOID					m_pvLeakedKey;

};

#endif // !defined(AFX_UGPROGRAM_H__F7F9CDC5_FB50_4AD1_973D_3A6AA2BCE4DB__INCLUDED_)
