// UGEncrypt.h: interface for the CUGEncrypt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UGENCRYPT_H__CC105DF2_DC88_4F12_BB7F_0BC35ACBF229__INCLUDED_)
#define AFX_UGENCRYPT_H__CC105DF2_DC88_4F12_BB7F_0BC35ACBF229__INCLUDED_

#include "UGBenDef.h"

typedef struct _tag_tea_key
{
	UG_ULONG	ulKey[4];
	
} TEA_KEY, *PTEA_KEY;

class CUGEncrypt  
{
public:
	CUGEncrypt();
	virtual ~CUGEncrypt();

public:
	static	UG_PCHAR	MD5Encrypt(UG_PCHAR pchIn,UG_PCHAR pchOut);//pchInΪ16λ�Ĵ����ֽ�,pchOutΪ33λ�Ĵ����ֽ�,������ֻ��32λ��Ч.

public:
	static	UG_ULONG	TEAEncrypt(PTEA_KEY pKey,UG_PBYTE pbyValue,UG_ULONG& ulLen); //TEA���ܣ��ַ�����ת��Ϊ16�ı���
	static	UG_ULONG	TEADecrypt(PTEA_KEY pKey,UG_PBYTE pbyValue,UG_ULONG& ulLen); //TEA���ܣ��ַ����ȱ���Ϊ16�ı���
	
};

#endif // !defined(AFX_UGENCRYPT_H__CC105DF2_DC88_4F12_BB7F_0BC35ACBF229__INCLUDED_)
