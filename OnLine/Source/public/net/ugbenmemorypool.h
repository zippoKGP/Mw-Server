/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : UGMemoryPool.h
*owner        : Ben
*description  : �ڴ溯��ʹ���࣬�ܸ����ڴ�ʹ����������ڴ�δ���ͷ�ʱ�����������ͷš�
*modified     : 2004/12/20
******************************************************************************/ 
#ifndef __UG_MEMORY_POOL__
#define __UG_MEMORY_POOL__
//-------------------------------------------------------------------------------------
//����ʱ�����__FILE__��__LINE__�������޷���λ�ļ���λ�ã�����ʹ��Ĭ�ϵĲ�����Ĭ�ϲ�
//��ֻ�����ڱ��ʱ��Ѹ�ٵĿ�����UG_ULONG ulCount�е�ulCountΪ���ʾ����һ���࣬��Ϊ
//��ʱ��ʾ����һ�������顣���̱��붨��CUGProgram* g_pUGProgram��ָ�롣

enum MEM_TYPE
{
	CLASS_MEM, //��UGNew�������ռ䣬�û������Լ�ɾ������ֻ����������
	CLASS_MEM_ARRAY, //��UGNew�������ռ����飬�û������Լ�ɾ������ֻ����������
	CHAR_MEM_ARRAY, //��UGMalloc������ֽڿռ����飬�û����Բ�ɾ��������Ҳ���Լ�ɾ����
	MEM_DEL_NO_FIND //����ɾ��ʱû���ҵ�
};

template <class T>
T* UGNew(UG_ULONG ulCount,UG_PCHAR pchFile = __FILE__,UG_ULONG ulLine = __LINE__)
{
	if(ulCount)
	{
		T* p = new T[ulCount];
		if(p)
		{
			g_pUGProgram->newMem(p,ulCount * sizeof(T),CLASS_MEM_ARRAY,pchFile,ulLine);
		}
		return p;
	}
	else
	{
		T* p = new T;
		if(p)
		{
			g_pUGProgram->newMem(p,sizeof(T),CLASS_MEM,pchFile,ulLine);
		}
		return p;
	}
}

template <class T>
UG_PCHAR UGMalloc(UG_ULONG ulCount,UG_PCHAR pchFile = __FILE__,UG_ULONG ulLine = __LINE__)
{
	if(ulCount < 1)
	{
		return NULL;
	}
	UG_PCHAR p = new UG_CHAR[ulCount];
	if(p)
	{
		g_pUGProgram->newMem(p,ulCount,CHAR_MEM_ARRAY,pchFile,ulLine);
	}
	return p;
}

template <class T>
UG_ULONG UGDel(T& p)
{
	if(g_pUGProgram)
	{
		MEM_TYPE ulType = g_pUGProgram->delMem(p);
		if(CLASS_MEM == ulType)
		{
			ReleaseP<T>(p);
		}
		else if(CLASS_MEM_ARRAY == ulType)
		{
			ReleasePA<T>(p);
		}
		else if(CHAR_MEM_ARRAY == ulType)
		{
			UG_PCHAR pp = (UG_PCHAR)p;
			ReleasePA<UG_PCHAR>(pp);
			p = NULL;
		}
		else
		{
			return UG_ULONG(-1);
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------
#endif//( __UG_MEMORY_POOL__)
