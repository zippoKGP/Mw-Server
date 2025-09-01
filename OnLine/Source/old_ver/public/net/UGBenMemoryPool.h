/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : UGMemoryPool.h
*owner        : Ben
*description  : 内存函数使用类，能跟踪内存使用情况，在内存未被释放时能侦测出来并释放。
*modified     : 2004/12/20
******************************************************************************/ 
#ifndef __UG_MEMORY_POOL__
#define __UG_MEMORY_POOL__
//-------------------------------------------------------------------------------------
//调用时必须加__FILE__和__LINE__，否则无法定位文件的位置，不能使用默认的参数，默认参
//数只是用于编程时能迅速的拷贝。UG_ULONG ulCount中的ulCount为零表示申请一个类，不为
//零时表示申请一块类数组。工程必须定义CUGProgram* g_pUGProgram的指针。

enum MEM_TYPE
{
	CLASS_MEM, //用UGNew申请的类空间，用户必须自己删除程序只能侦测出来。
	CLASS_MEM_ARRAY, //用UGNew申请的类空间数组，用户必须自己删除程序只能侦测出来。
	CHAR_MEM_ARRAY, //用UGMalloc申请的字节空间数组，用户可以不删除，程序也会自己删除。
	MEM_DEL_NO_FIND //用于删除时没有找到
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
