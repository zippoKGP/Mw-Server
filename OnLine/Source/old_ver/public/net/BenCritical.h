/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
2004 ALL RIGHTS RESERVED.
*file name    : Critical.h
*owner        : Ben
*description  : �ٽ����ࡣ
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
	UG_VOID		lock(); //������ʱ����Զֱ������
	UG_VOID		unlock(); //����
	
private:
	UG_PVOID	m_pCs;
	
};

#endif // !defined(AFX_CRITICAL_H__5E089DC0_2543_4E30_B9C0_59A7C82F8D99__INCLUDED_)
