// TEAEncrypt.h: interface for the CTEAEncrypt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEAENCRYPT_H__A146D022_CF87_4EF2_89F9_D7DEEE458A3C__INCLUDED_)
#define AFX_TEAENCRYPT_H__A146D022_CF87_4EF2_89F9_D7DEEE458A3C__INCLUDED_

#include "net/UGBenDef.h"

class CTEAEncrypt  
{
public:
	CTEAEncrypt();
	virtual ~CTEAEncrypt();

public:
	static	UG_ULONG		TEAEncrypt(PTEA_KEY pKey,UG_PBYTE pbyValue,UG_ULONG& ulLen);
	static	UG_ULONG		TEADecrypt(PTEA_KEY pKey,UG_PBYTE pbyValue,UG_ULONG& ulLen);
	
protected:
	static	UG_ULONG		TEAEnDecrypt(UG_PULONG pu32Key,UG_PULONG pu32Value,UG_LONG N);
	static	UG_ULONG		TEASetLen(UG_ULONG& ulLen);
	
};

#endif // !defined(AFX_TEAENCRYPT_H__A146D022_CF87_4EF2_89F9_D7DEEE458A3C__INCLUDED_)
