/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : SyncEvent.cpp
*owner        : Ben
*description  : ͬ���࣬���ٽ���һ��ʹ�ã��ܷ��㡣(��CSyncEvent sys(&m_cs))
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
