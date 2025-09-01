/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : SyncEvent.cpp
*owner        : Ben
*description  : 同步类，和临界区一起使用，很方便。(如CSyncEvent sys(&m_cs))
*modified     : 2004/12/20
******************************************************************************/ 

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSyncEvent::CSyncEvent(CCritical* pCS)
{
	m_pCS = pCS;
	if(m_pCS)
	{
		pCS->lock();
	}
}

CSyncEvent::~CSyncEvent()
{
	if(m_pCS)
	{
		m_pCS->unlock();
	}
}
