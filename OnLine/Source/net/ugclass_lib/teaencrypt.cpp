// TEAEncrypt.cpp: implementation of the CTEAEncrypt class.
//
//////////////////////////////////////////////////////////////////////

#include "IncAll.h"

const UG_LONG		EN_ROUNDS		= 32;
const UG_LONG		DE_ROUNDS		= -32;
const UG_ULONG		DELTA			= 0x9e3779b9;//sqr(5)-1 * 2^31

CTEAEncrypt::CTEAEncrypt()
{

}

CTEAEncrypt::~CTEAEncrypt()
{

}

UG_ULONG CTEAEncrypt::TEAEncrypt(PTEA_KEY pKey,UG_PBYTE pbyValue,UG_ULONG& ulLen)
{
	TEASetLen(ulLen);
	UG_ULONG ulCount = ulLen / 16;
	for(UG_ULONG l = 0; l < ulCount; l += 16)
	{
		TEAEnDecrypt(UG_PULONG(pKey),UG_PULONG(pbyValue + l),EN_ROUNDS);
	}
	return 0;
}

UG_ULONG CTEAEncrypt::TEADecrypt(PTEA_KEY pKey,UG_PBYTE pbyValue,UG_ULONG& ulLen)
{
	TEASetLen(ulLen);
	UG_ULONG ulCount = ulLen / 16;
	for(UG_ULONG l = 0; l < ulCount; l += 16)
	{
		TEAEnDecrypt(UG_PULONG(pKey),UG_PULONG(pbyValue + l),DE_ROUNDS);
	}
	return 0;
}

UG_ULONG CTEAEncrypt::TEAEnDecrypt(UG_PULONG k,UG_PULONG v,UG_LONG N)
{
	register UG_ULONG y = v[0];
	register UG_ULONG z = v[1];
	register UG_ULONG limit;
	register UG_ULONG sum = 0;
	if(N > 0) //ENCRYPT
	{ 
		limit = DELTA * N;
		while(sum != limit)
		{
			y += ((z << 4) ^ (z >> 5)) + (z ^ sum) + k[sum & 3];
			sum += DELTA;
			z += ((y << 4) ^ (y >> 5)) + (y ^ sum) + k[(sum >> 11) & 3];
		}
	}
	else //DECRYPT
	{ 
		sum=DELTA*(-N);
		while(sum)
		{
			z -= ((y << 4) ^ (y >> 5)) + (y ^ sum) + k[(sum >> 11) & 3];
			sum -= DELTA;
			y -= ((z << 4) ^ (z >> 5)) + (z ^ sum) + k[sum & 3];
		}
	}
	v[0] = y;
	v[1] = z;
	return 0;
}

inline UG_ULONG CTEAEncrypt::TEASetLen(UG_ULONG& ulLen)
{
	UG_LONG l = ulLen % 16;
	if(!l)
	{
		return 0;
	}
	ulLen = ulLen + (16 - l);
	return 0;
}
