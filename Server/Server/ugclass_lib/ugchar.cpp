// UGChar.cpp: implementation of the CUGChar class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUGChar::CUGChar()
{

}

CUGChar::~CUGChar()
{

}

UG_PCHAR CUGChar::toUpper(UG_PCHAR pchIn)
{
	UG_INT32 n32Count = strlen(pchIn);
	for(UG_INT32 i = 0; i < n32Count; i ++)
	{
		*(pchIn + i) = toupper(*(pchIn + i));
	}
	return pchIn;
}

UG_PCHAR CUGChar::toLower(UG_PCHAR pchIn)
{
	UG_INT32 n32Count = strlen(pchIn);
	for(UG_INT32 i = 0; i < n32Count; i ++)
	{
		*(pchIn + i) = tolower(*(pchIn + i));
	}
	return pchIn;
}

bool CUGChar::operator()(const UG_PCHAR& pchX, const UG_PCHAR& pchY) const
{
	UG_INT32 n32Ret = strcmp(pchX,pchY);
	if(n32Ret < 0)
	{
		return true;
	}
	return false;
}
