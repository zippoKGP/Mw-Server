// MD5Encrypt.cpp: implementation of the CMD5Encrypt class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define FF(a, b, c, d, x, s, ac) {(a) += F ((b), (c), (d)) + (x) + (ac);(a) = ROTATE_LEFT ((a), (s));(a) += (b);}
#define GG(a, b, c, d, x, s, ac) {(a) += G ((b), (c), (d)) + (x) + (ac);(a) = ROTATE_LEFT ((a), (s));(a) += (b);}
#define HH(a, b, c, d, x, s, ac) {(a) += H ((b), (c), (d)) + (x) + (ac);(a) = ROTATE_LEFT ((a), (s));(a) += (b);}
#define II(a, b, c, d, x, s, ac) {(a) += I ((b), (c), (d)) + (x) + (ac);(a) = ROTATE_LEFT ((a), (s));(a) += (b);}

CMD5Encrypt::CMD5Encrypt()
{

}

CMD5Encrypt::~CMD5Encrypt()
{

}

UG_VOID CMD5Encrypt::MD5Init(PMD5_CTX pMd5Ctx)
{
	pMd5Ctx->u32Count[0] = 0;
	pMd5Ctx->u32Count[1] = 0;
	pMd5Ctx->u32State[0] = 0x67452301;
	pMd5Ctx->u32State[1] = 0xefcdab89;
	pMd5Ctx->u32State[2] = 0x98badcfe;
	pMd5Ctx->u32State[3] = 0x10325476;
	return;
}

UG_VOID CMD5Encrypt::MD5Final(UG_PBYTE pbyIn,PMD5_CTX pMd5Ctx)
{
	UG_BYTE PADDING[64];
	memset(PADDING,0,64);
	PADDING[0] = 0x80;
	UG_BYTE szBits[8];
	Encode(szBits,pMd5Ctx->u32Count,8);
	UG_INT32 n32Index = ((pMd5Ctx->u32Count[0]) >> 3) & 0x3f;
	UG_INT32 n32padLen = 56 - n32Index;
	if(n32Index >= 56)
	{
		n32padLen = 120 - n32Index;
	}
	MD5Update(pMd5Ctx,PADDING,n32padLen);
	MD5Update(pMd5Ctx,szBits,8);
	Encode(pbyIn,pMd5Ctx->u32State,16);
	memset(pMd5Ctx,0,sizeof(MD5_CTX));
	return;
}

UG_VOID CMD5Encrypt::MD5Update(PMD5_CTX pMd5Ctx,UG_PBYTE pbyIn,UG_UINT32 u32In)
{
	UG_INT32 n32Index = (pMd5Ctx->u32Count[0] >> 3) & 0x3F;
	if((pMd5Ctx->u32Count[0] += (u32In << 3)) < (u32In << 3))
	{
		(pMd5Ctx->u32Count[1]) ++;
	}
	UG_INT32 i = 0;
	pMd5Ctx->u32Count[1] += (u32In >> 29);
	UG_INT32 n32PartLen = 64 - n32Index;
	if(u32In >= n32PartLen)
	{
		memcpy(pMd5Ctx->szBuffer + n32Index,pbyIn,n32PartLen);
		MD5Transform(pMd5Ctx->u32State,pMd5Ctx->szBuffer);
		for (i = n32PartLen; i + 63 < u32In; i += 64)
		{
			MD5Transform(pMd5Ctx->u32State,pbyIn + i);
		}
		n32Index = 0;
	}
	else
	{
		i = 0;
	}
	memcpy(pMd5Ctx->szBuffer + n32Index,pbyIn + i,u32In - i);
	return;
}

UG_VOID CMD5Encrypt::MD5Transform (UG_UINT32 u32State[4],UG_PBYTE pbyBlock)
{
	UG_UINT32 a = u32State[0];
	UG_UINT32 b = u32State[1];
	UG_UINT32 c = u32State[2];
	UG_UINT32 d = u32State[3];
	UG_UINT32 x[16];
	Decode(x,pbyBlock,64);
	FF(a, b, c, d, x[ 0], 7, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */
	FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[ 8], 7, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */
	
	/* Round 2 */
	GG(a, b, c, d, x[ 1], 5, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[ 6], 9, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[ 5], 5, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], 9,  0x2441453); /* 22 */
	GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */
	
	/* Round 3 */
	HH(a, b, c, d, x[ 5], 4, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[ 1], 4, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */
	HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */
	
	/* Round 4 */
	II(a, b, c, d, x[ 0], 6, 0xf4292244); /* 49 */
	II(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[ 4], 6, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */
	u32State[0] += a;
	u32State[1] += b;
	u32State[2] += c;
	u32State[3] += d;
	memset(x,0,16 * sizeof(UG_UINT32));
	return;
}

UG_PCHAR CMD5Encrypt::MDConvertString(UG_PBYTE pbySrc,UG_PBYTE pbyDesc)
{
	for(UG_INT32 i = 0; i < 16; i ++)
	{
		sprintf((UG_PCHAR)pbyDesc,"%02x",*(pbySrc + i));
		pbyDesc += 2;
	}
	*(pbyDesc + 32) = '\0';
	return (UG_PCHAR)pbyDesc;
}

UG_VOID CMD5Encrypt::Encode(UG_PBYTE pbyOutput,UG_PUINT32 pu32Input,UG_INT32 n32Len)
{
	UG_INT32 i = 0;
	UG_INT32 j = 0;
	for(; j < n32Len;)
	{
		*(pbyOutput + j) = (*(pu32Input + i)) & 0xff;
		*(pbyOutput + j + 1) = (((*(pu32Input + i)) >> 8) & 0xff);
		*(pbyOutput + j + 2) = (((*(pu32Input + i)) >> 16) & 0xff);
		*(pbyOutput + j + 3) = (((*(pu32Input + i)) >> 24) & 0xff);
		i ++;
		j += 4;
	}
	return;
}

UG_VOID CMD5Encrypt::Decode(UG_PUINT32 pu32Output,UG_PBYTE pbyInput,UG_INT32 n32Len)
{
	UG_INT32 i = 0;
	UG_INT32 j = 0;
	for(; j < n32Len;)
	{
		UG_UINT32 a = *(pbyInput + j);
		UG_UINT32 b = *(pbyInput + j + 1);
		UG_UINT32 c = *(pbyInput + j + 2);
		UG_UINT32 d = *(pbyInput + j + 3);
		*(pu32Output + i) = a | (b << 8) | (c << 16) | (d << 24);
		i ++;
		j += 4;
	}
	return;
}

UG_PCHAR CMD5Encrypt::MD5Encrypt(UG_PCHAR pchIn,UG_PCHAR pchOut)
{
	MD5_CTX md5Ctx;
	UG_BYTE byDigest[16];
	MD5Init(&md5Ctx);
	MD5Update(&md5Ctx,(UG_PBYTE)pchIn,strlen(pchIn));
	MD5Final(byDigest,&md5Ctx);
	MDConvertString(byDigest,(UG_PBYTE)pchOut) ;
	return pchOut;
}