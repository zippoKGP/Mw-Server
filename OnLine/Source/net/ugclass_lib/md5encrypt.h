// MD5Encrypt.h: interface for the CMD5Encrypt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MD5ENCRYPT_H__95BC5014_E10D_4FF1_8521_1708D35428E8__INCLUDED_)
#define AFX_MD5ENCRYPT_H__95BC5014_E10D_4FF1_8521_1708D35428E8__INCLUDED_

#include "net/UGBenDef.h"

typedef struct _tag_md5
{
	UG_UINT32		u32State[4];//state(ABCD)
	UG_UINT32		u32Count[2];//number of bits, modulo 2^64 (lsb first)
	UG_BYTE			szBuffer[64];//input buffer

} MD5_CTX, *PMD5_CTX;

class CMD5Encrypt  
{
public:
	CMD5Encrypt();
	virtual ~CMD5Encrypt();

public:
	static	UG_PCHAR		MD5Encrypt(UG_PCHAR pchIn,UG_PCHAR pchOut);

protected:
	static	UG_VOID			MD5Init(PMD5_CTX pMd5Ctx);
	static	UG_VOID			MD5Final(UG_PBYTE pbyIn,PMD5_CTX pMd5Ctx);
	static	UG_VOID			MD5Update(PMD5_CTX pMd5Ctx,UG_PBYTE pbyIn,UG_UINT32 u32In);
	static	UG_PCHAR		MDConvertString(UG_PBYTE pbySrc,UG_PBYTE pbyDesc);
	static	UG_VOID			Encode(UG_PBYTE pbyOutput,UG_PUINT32 pu32Input,UG_INT32 n32Len);
	static	UG_VOID			Decode(UG_PUINT32 pu32Output,UG_PBYTE pbyInput,UG_INT32 n32Len);
	static	UG_VOID			MD5Transform (UG_UINT32 u32State[4],UG_PBYTE pbyBlock);

};

#endif // !defined(AFX_MD5ENCRYPT_H__95BC5014_E10D_4FF1_8521_1708D35428E8__INCLUDED_)
