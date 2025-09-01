/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : UGProgram.cpp
*owner        : Ben
*description  : 程序主类，包含异常处理，内存使用等。
*modified     : 2004/12/20
******************************************************************************/ 

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUGProgram::CUGProgram()
{
	m_mapMem.clear();
	m_fnLeaked = NULL;
	m_pvLeakedKey = NULL;
}

CUGProgram::~CUGProgram()
{
	cleanup();
	m_fnLeaked = NULL;
	m_pvLeakedKey = NULL;
}

UG_ULONG CUGProgram::setExceptionFun(UGMain fnMain,UGException fnException,UG_PVOID pvKey)
{
	if(fnMain)
	{
		__try
		{
			fnMain(pvKey);
		}
		__except(EXCEPTION_ACCESS_VIOLATION == GetExceptionCode() 
			|| EXCEPTION_BREAKPOINT == GetExceptionCode()
			|| EXCEPTION_DATATYPE_MISALIGNMENT == GetExceptionCode()
			|| EXCEPTION_SINGLE_STEP == GetExceptionCode()
			|| EXCEPTION_ARRAY_BOUNDS_EXCEEDED == GetExceptionCode()
			|| EXCEPTION_FLT_DENORMAL_OPERAND == GetExceptionCode()
			|| EXCEPTION_FLT_DIVIDE_BY_ZERO == GetExceptionCode()
			|| EXCEPTION_FLT_INEXACT_RESULT == GetExceptionCode()
			|| EXCEPTION_FLT_INVALID_OPERATION == GetExceptionCode()
			|| EXCEPTION_FLT_OVERFLOW == GetExceptionCode()
			|| EXCEPTION_FLT_STACK_CHECK == GetExceptionCode()
			|| EXCEPTION_FLT_UNDERFLOW == GetExceptionCode()
			|| EXCEPTION_INT_DIVIDE_BY_ZERO == GetExceptionCode()
			|| EXCEPTION_INT_OVERFLOW == GetExceptionCode()
			|| EXCEPTION_PRIV_INSTRUCTION == GetExceptionCode()
			|| EXCEPTION_NONCONTINUABLE_EXCEPTION == GetExceptionCode())
		{
			if(fnException)
			{
				fnException(pvKey,GetExceptionCode());
			}
		}
	}
	return 0;
}

UG_ULONG CUGProgram::cleanup()
{
	cleanupMem();
	return 0;
}

UG_ULONG CUGProgram::cleanupMem()
{
	static UG_CHAR szLeaked[1024];
	struct tm* pTimer = NULL;
	MEM_MAP::iterator it;
	PMEM p = NULL;
	for(it = m_mapMem.begin(); it != m_mapMem.end(); it ++)
	{
		p = PMEM((*it).second);
		if(p)
		{
			if(CHAR_MEM_ARRAY == p->ulType)
			{
				UG_PCHAR pp = UG_PCHAR(p->pvMem);
				ReleasePA<UG_PCHAR>(pp);
				p->pvMem = NULL;
			}
			else
			{
				if(m_fnLeaked)
				{
					pTimer = localtime(&(p->timer));
					sprintf(szLeaked,"there is a memory leaked, memory len = %d, \r\nfile is %s, \r\nline is %d, malloc timer is %d-%d-%d %d:%d:%d",p->ulLen,p->pchFile,p->ulLine,pTimer->tm_year + 1900,pTimer->tm_mon + 1,pTimer->tm_mday,pTimer->tm_hour,pTimer->tm_min,pTimer->tm_sec);
					m_fnLeaked(m_pvLeakedKey,szLeaked);
				}
//				free(p->pvMem); //i am not sure this is right.
//				p->pvMem = NULL; //please test.
				//Out leak memory;
			}
			ReleaseP<PMEM>(p);
		}
	}
	m_mapMem.clear();
	return 0;
}

UG_ULONG CUGProgram::newMem(UG_PVOID pvMem,UG_ULONG ulLen,MEM_TYPE ulType,UG_PCHAR pchFile,UG_ULONG ulLine)
{
	if(m_mapMem.end() != m_mapMem.find(pvMem))
	{
		return -1;
	}
	PMEM p = new MEM;
	p->ulLine = ulLine;
	p->ulLen = ulLen;
	p->ulType = ulType;
	time(&p->timer);
	p->pvMem = pvMem;
	p->pchFile = new UG_CHAR[strlen(pchFile) + 1];
	memset(p->pchFile,0,strlen(pchFile) + 1);
	strcpy(p->pchFile,pchFile);
	m_mapMem[p->pvMem] = p;
	return 0;
}

MEM_TYPE CUGProgram::delMem(UG_PVOID pvMem)
{
	MEM_TYPE ulRet = MEM_DEL_NO_FIND;
	MEM_MAP::iterator it = m_mapMem.find(pvMem);
	if(m_mapMem.end() == it)
	{
		return ulRet;
	}
	PMEM p = (PMEM)((*it).second);
	ulRet = p->ulType;
	ReleaseP<PMEM>(p);
	m_mapMem.erase(it);
	return ulRet;
}

UG_ULONG CUGProgram::init(UGMemoryLeaked fnLeaked,UG_PVOID pvKey)
{
	m_fnLeaked = fnLeaked;
	m_pvLeakedKey = pvKey;
	return 0;
}
