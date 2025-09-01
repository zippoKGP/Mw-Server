/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : Critical.h
*owner        : Ben
*description  : 临界区类。
*modified     : 2004/12/20
******************************************************************************/ 

#if !defined(AFX_CRITICAL_H__5E089DC0_2543_4E30_B9C0_59A7C82F8D99__INCLUDED_)
#define AFX_CRITICAL_H__5E089DC0_2543_4E30_B9C0_59A7C82F8D99__INCLUDED_

#include "UGBenDef.h"

class CCritical  
{
public:
	CCritical();
	virtual ~CCritical();

public:
	UG_VOID		lock(); //锁定，时间永远直到解锁
	UG_VOID		unlock(); //解锁
	
private:
	UG_PVOID	m_pCs;
	
};

#endif // !defined(AFX_CRITICAL_H__5E089DC0_2543_4E30_B9C0_59A7C82F8D99__INCLUDED_)
