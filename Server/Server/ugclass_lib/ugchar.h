// UGChar.h: interface for the CUGChar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UGCHAR_H__135436EF_2699_4574_80F7_ABE4D3BD84D1__INCLUDED_)
#define AFX_UGCHAR_H__135436EF_2699_4574_80F7_ABE4D3BD84D1__INCLUDED_

#include "net/UGBenDef.h"

class CUGChar  
{
public:
	CUGChar();
	virtual ~CUGChar();

public:
	static	UG_PCHAR	toUpper(UG_PCHAR pchIn);
	static	UG_PCHAR	toLower(UG_PCHAR pchIn);

public:
	bool operator()(const UG_PCHAR& pchX, const UG_PCHAR& pchY) const;
	
};

#endif // !defined(AFX_UGCHAR_H__135436EF_2699_4574_80F7_ABE4D3BD84D1__INCLUDED_)
