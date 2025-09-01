/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : Critical.cpp
*owner        : Ben
*description  : ¡ŸΩÁ«¯¿‡°£
*modified     : 2004/12/20
******************************************************************************/ 

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCritical::CCritical()
{
	m_pCs = new CRITICAL_SECTION;
	InitializeCriticalSection((CRITICAL_SECTION*)m_pCs);
}

CCritical::~CCritical()
{
	DeleteCriticalSection((CRITICAL_SECTION*)m_pCs);
	CRITICAL_SECTION *pCs = (CRITICAL_SECTION*)m_pCs;
	ReleaseP<LPCRITICAL_SECTION>(pCs);
	m_pCs = NULL;
}

UG_VOID CCritical::lock()
{
	EnterCriticalSection((CRITICAL_SECTION*)m_pCs);
	return;
}

UG_VOID CCritical::unlock()
{
	LeaveCriticalSection((CRITICAL_SECTION*)m_pCs);
	return;
}
