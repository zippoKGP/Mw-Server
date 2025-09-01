/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : SyncEvent.h
*owner        : Ben
*description  : 同步类，和临界区一起使用，很方便。(如CSyncEvent sys(&m_cs))
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_SYNCEVENT_H__AB3A03B6_0D1C_4191_BD9D_08AB1133415D__INCLUDED_)
#define AFX_SYNCEVENT_H__AB3A03B6_0D1C_4191_BD9D_08AB1133415D__INCLUDED_

#include "UGBenDef.h"
#include "BenCritical.h"

class CSyncEvent  
{
public:
	CSyncEvent(CCritical *pCS);
	virtual ~CSyncEvent();

private:
	CCritical*		m_pCS;
	
};

#endif // !defined(AFX_SYNCEVENT_H__AB3A03B6_0D1C_4191_BD9D_08AB1133415D__INCLUDED_)
