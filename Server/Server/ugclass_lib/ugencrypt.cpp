// UGEncrypt.cpp: implementation of the CUGEncrypt class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUGEncrypt::CUGEncrypt()
{

}

CUGEncrypt::~CUGEncrypt()
{

}

UG_PCHAR CUGEncrypt::MD5Encrypt(UG_PCHAR pchIn,UG_PCHAR pchOut)
{
	if(strlen(pchIn) > 16)
	{
		return NULL;
	}
	return CMD5Encrypt::MD5Encrypt(pchIn,pchOut);
}

UG_ULONG CUGEncrypt::TEAEncrypt(PTEA_KEY pKey,UG_PBYTE pbyValue,UG_ULONG& ulLen)
{
	return CTEAEncrypt::TEAEncrypt(pKey,pbyValue,ulLen);
}

UG_ULONG CUGEncrypt::TEADecrypt(PTEA_KEY pKey,UG_PBYTE pbyValue,UG_ULONG& ulLen)
{
	return CTEAEncrypt::TEADecrypt(pKey,pbyValue,ulLen);
}
