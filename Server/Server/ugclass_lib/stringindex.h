// StringIndex.h: interface for the CStringIndex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGINDEX_H__C6A6C2C2_0BD0_4191_A0D5_AD8A1DA55DB4__INCLUDED_)
#define AFX_STRINGINDEX_H__C6A6C2C2_0BD0_4191_A0D5_AD8A1DA55DB4__INCLUDED_

#include "net/UGBenDef.h"

class CStringIndex  
{
public:
	CStringIndex();
	virtual ~CStringIndex();

public:
	UG_ULONG				init(UG_PCHAR pchPathName,UG_BOOL bLoadInMem);
	UG_ULONG				getValue(UG_PCHAR pchKey,UG_PCHAR& pchValue);
	UG_ULONG				cleanup();
	
protected:	
	UG_ULONG				parseKeyAndValue(UG_PCHAR pchLine,UG_ULONG ulPos);
	UG_ULONG				removeSpace(UG_PCHAR pchLine,UG_INT32 n32Count,UG_INT32& n32Left,UG_INT32& n32Right);
	
private:
	UG_ULONG				m_ulMaxLen;
	UG_PCHAR				m_pchMax;
	UG_BOOL					m_bLoadInMem;
	FILE*					m_pFile;
	
private:
	map<UG_PCHAR,CIndexKeyValue*,CUGChar>		m_map;
	
};

#endif // !defined(AFX_STRINGINDEX_H__C6A6C2C2_0BD0_4191_A0D5_AD8A1DA55DB4__INCLUDED_)
