// IniKeyValue.h: interface for the CIniKeyValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIKEYVALUE_H__E562E194_082D_4CB4_988D_621BF6D247EB__INCLUDED_)
#define AFX_INIKEYVALUE_H__E562E194_082D_4CB4_988D_621BF6D247EB__INCLUDED_

#include "net/UGBenDef.h"

class CIniKeyValue  
{
public:
	CIniKeyValue();
	virtual ~CIniKeyValue();

public:
	UG_ULONG		cleanup();
	UG_ULONG		convertAppAndKeyLower();
	UG_ULONG		getValue(UG_PCHAR pchKey,UG_PCHAR& pchValue);
	UG_ULONG		getMaxKey(UG_ULONG& ulMax);
	
public:
	UG_PCHAR		m_pchKey;
	UG_PCHAR		m_pchValue;
	
};

#endif // !defined(AFX_INIKEYVALUE_H__E562E194_082D_4CB4_988D_621BF6D247EB__INCLUDED_)
