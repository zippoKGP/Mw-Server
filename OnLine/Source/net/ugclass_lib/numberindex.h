// NumberIndex.h: interface for the CNumberIndex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUMBERINDEX_H__E1175AA3_C06A_4C02_A022_11D1221027D6__INCLUDED_)
#define AFX_NUMBERINDEX_H__E1175AA3_C06A_4C02_A022_11D1221027D6__INCLUDED_

#include "net/UGBenDef.h"

class CNumberIndex  
{
public:
	CNumberIndex();
	virtual ~CNumberIndex();

public:
	UG_ULONG				init(UG_PCHAR pchPathName,UG_BOOL bLoadInMem);
	UG_ULONG				getValue(UG_ULONG ulKey,UG_PCHAR& pchValue);
	UG_ULONG				cleanup();

protected:	
	UG_ULONG				parseKeyAndValue(UG_PCHAR pchLine,UG_PCHAR pchKey,UG_ULONG ulPos);
	UG_ULONG				removeSpace(UG_PCHAR pchLine,UG_INT32 n32Count,UG_INT32& n32Left,UG_INT32& n32Right);
	
private:
	UG_ULONG				m_ulMaxLen;
	UG_PCHAR				m_pchMax;
	UG_BOOL					m_bLoadInMem;
	FILE*					m_pFile;

private:
	map<UG_ULONG,CIndexKeyValue*>		m_map;

};

#endif // !defined(AFX_NUMBERINDEX_H__E1175AA3_C06A_4C02_A022_11D1221027D6__INCLUDED_)
