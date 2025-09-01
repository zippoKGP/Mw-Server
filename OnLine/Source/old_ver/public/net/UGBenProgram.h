/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : UGProgram.h
*owner        : Ben
*description  : �������࣬�����쳣�����ڴ�ʹ�õȡ�
*modified     : 2004/12/20
******************************************************************************/ 


#if !defined(AFX_UGPROGRAM_H__F7F9CDC5_FB50_4AD1_973D_3A6AA2BCE4DB__INCLUDED_)
#define AFX_UGPROGRAM_H__F7F9CDC5_FB50_4AD1_973D_3A6AA2BCE4DB__INCLUDED_

#include "UGBenDef.h"

typedef UG_ULONG (UG_FUN_CALLBACK *UGMain)(UG_PVOID pvKey); //����ص�����ʱ�����UG_FUN_CALLBACK�ĵ��÷�ʽ��������û����
typedef UG_ULONG (UG_FUN_CALLBACK *UGException)(UG_PVOID pvKey,UG_ULONG ulErrorCode);
typedef UG_ULONG (UG_FUN_CALLBACK *UGMemoryLeaked)(UG_PVOID pvKey,UG_PCHAR pchError);

class CUGProgram  
{
public:
	CUGProgram();
	virtual ~CUGProgram();

public:
	//�����쳣�����࣬������fnMain�����쳣ʱ��fnException�˳�
	UG_ULONG		setExceptionFun(UGMain fnMain,UGException fnException,UG_PVOID pvKey);
	UG_ULONG		cleanup();

public:
	UG_ULONG		init(UGMemoryLeaked fnLeaked,UG_PVOID pvKey); //���ڴ�й©ʱ���Ӹú�������
	UG_ULONG		newMem(UG_PVOID pvMem,UG_ULONG ulLen,MEM_TYPE ulType,UG_PCHAR pchFile,UG_ULONG ulLine); //�ⲿ���ܵ��øú���,ͨ��UGNew����
	MEM_TYPE		delMem(UG_PVOID pvMem); //�ⲿ���ܵ��øú���,ͨ��UGDel����

protected:
	UG_ULONG		cleanupMem();
	
private:
	map<UG_PVOID,UG_PVOID>		m_mapMem;
	UGMemoryLeaked				m_fnLeaked;
	UG_PVOID					m_pvLeakedKey;

};

#endif // !defined(AFX_UGPROGRAM_H__F7F9CDC5_FB50_4AD1_973D_3A6AA2BCE4DB__INCLUDED_)
