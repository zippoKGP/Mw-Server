#include "CCodeConvert.h"

#ifndef WIN32 // for testing under unix
int main(void)
{
	CCodeConvert uniTst;
	char dst[64] = "";
	char msg[256] = "";
	unsigned short wchrSrc[3] ;
	wchrSrc[0] = 22823;
	wchrSrc[1] = 22823;
	wchrSrc[2] = 22824;
	char wchrTgt[8] = "      ";
	int idst = 0;
	uniTst.MultiByteToWideChar(CP_ACP,"",4,wchrSrc,2);
	uniTst.WideCharToMultiByte(CP_ACP,wchrSrc,2,wchrTgt,4);
	wchrTgt[4] = NULL;
	sprintf(msg,"(BIG5-UNICODE-BIG5) Source :  Result : %s\n",wchrTgt);
	printf(msg);
	uniTst.initializeBig5ToSjisTable();
	sprintf(msg,"(BIG5-SJIS) %s to %s \n","らセ礷",uniTst.SourceToTarget("らセ礷"));
	printf(msg);
	uniTst.initializeSjisToBig5Table();
	sprintf(msg,"(SJIS-BIG5) %s to %s \n","擔杮堦斣",uniTst.SourceToTarget("擔杮堦斣"));
	printf(msg);
	uniTst.initializeBig5ToGbTable();
	sprintf(msg,"(BIG5-GB) %s to %s \n","参い瓣",uniTst.SourceToTarget("参い瓣"));
	printf(msg);
	uniTst.initializeGbToBig5Table();
	sprintf(msg,"(GB-BIG5) %s to %s \n","统一中国",uniTst.SourceToTarget("统一中国"));
	printf(msg);
}
#endif

void CCodeConvert::initializeUnicodeToBig5Table(void)
{
	datFile = fopen("u2b.dat","r+b");
	if(datFile == NULL)
		strcpy(errMsg,"unicode-big5 data file is unavailable");
	else
	{
		fseek(datFile,0,0);
		int i = fread(bufUnicode,1,131072,datFile);
		if(fread(bufUnicode,1,131072,datFile) != 131072)
			strcpy(errMsg,"unicode data file size incorrect");
		if(datFile != NULL)
			fclose(datFile);
	}
}

void CCodeConvert::initializeBig5ToUnicodeTable(void)
{
	datFile = fopen("b2u.dat","r+b");
	if(datFile == NULL)
		strcpy(errMsg,"big5-unicode data file is unavailable");
	else
	{
		fseek(datFile,0,0);
		int i = fread(bufBig5,1,131072,datFile);
		if(fread(bufBig5,1,131072,datFile) != 131072)
			strcpy(errMsg,"big5 data file size incorrect");
		if(datFile != NULL)
			fclose(datFile);
	}
}

void CCodeConvert::initializeBig5ToGbTable()
{
	datFile = fopen("b2g.dat","r+b");
	if(datFile == NULL)
		strcpy(errMsg,"big5-gb data file is unavailable");
	else
	{
		fseek(datFile,0,0);
		int i = fread(bufs2t,1,131072,datFile);
		if(fread(bufs2t,1,131072,datFile) != 131072)
			strcpy(errMsg,"big5 data file size incorrect");
		if(datFile != NULL)
			fclose(datFile);
	}
}

void CCodeConvert::initializeBig5ToSjisTable()
{
	datFile = fopen("b2j.dat","r+b");
	if(datFile == NULL)
		strcpy(errMsg,"big5 to sjis data file is unavailable");
	else
	{
		fseek(datFile,0,0);
		int i = fread(bufs2t,1,131072,datFile);
		if(fread(bufs2t,1,131072,datFile) != 131072)
			strcpy(errMsg,"b2j data file size incorrect");
		if(datFile != NULL)
			fclose(datFile);
	}
}

void CCodeConvert::initializeSjisToBig5Table()
{
	datFile = fopen("j2b.dat","r+b");
	if(datFile == NULL)
		strcpy(errMsg,"big5 to sjis data file is unavailable");
	else
	{
		fseek(datFile,0,0);
		int i = fread(bufs2t,1,131072,datFile);
		if(i != 131072)
			strcpy(errMsg,"j2b data file size incorrect");
		if(datFile != NULL)
			fclose(datFile);
	}
}

void CCodeConvert::initializeGbToBig5Table()
{
	datFile = fopen("g2b.dat","r+b");
	if(datFile == NULL)
		strcpy(errMsg,"gb-big5 data file is unavailable");
	else
	{
		fseek(datFile,0,0);
		int i = fread(bufs2t,1,131072,datFile);
		if(fread(bufs2t,1,131072,datFile) != 131072)
			strcpy(errMsg,"big5 data file size incorrect");
		if(datFile != NULL)
			fclose(datFile);
	}
}

CCodeConvert::CCodeConvert()
{
	initializeUnicodeToBig5Table();
	initializeBig5ToUnicodeTable();
}

CCodeConvert::~CCodeConvert()
{
	if(datFile != NULL)
		fclose(datFile);
}

int CCodeConvert::WideCharToMultiByte(UINT CodePage, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte)
{
	BOOL failFlag = FALSE;
	int nByteWritten = 0;
	if(cchWideChar * 2 >= cbMultiByte && cbMultiByte > 0)
	{
		if(CodePage == CP_ACP)
		{
			unsigned int idx = 0;
			for(nByteWritten = 0 ; nByteWritten < cchWideChar ; nByteWritten++)
			{
				idx = *(lpWideCharStr+nByteWritten);
				if(idx >=0 && idx <= 65536)
					strncpy(lpMultiByteStr+nByteWritten*2,bufUnicode+idx*2,2);
				else
					failFlag = TRUE;
			}
		}
	}
	else
	{   // requires more bytes in multibyte char buffer
		return cchWideChar*2; // the required size, in bytes, for a buffer that can receive the translated string. 
	}
	if(failFlag == FALSE)
		return nByteWritten;
	else
		return 0;
}

int CCodeConvert::MultiByteToWideChar(UINT CodePage, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
	BOOL failFlag = FALSE;
	int nByteWritten = 0;
	if(cchWideChar * 2 >= cbMultiByte && cchWideChar > 0)
	{
		if(CodePage == CP_ACP)
		{
			unsigned int idx = 0;
			for(nByteWritten = 0 ; nByteWritten < cbMultiByte ; nByteWritten++)
			{
				idx = (unsigned char) *(lpMultiByteStr+nByteWritten*2+0)*256 + (unsigned char) *(lpMultiByteStr+nByteWritten*2+1) ;
				if(idx >=0 && idx <= 65536)
					*(lpWideCharStr+nByteWritten) = *(bufBig5+idx*2)*256 + *(bufBig5+idx*2+1);
				else
					failFlag = TRUE;
			}
		}
	}
	else
	{   // requires more bytes in multibyte char buffer
		return cchWideChar; // the required size, in bytes, for a buffer that can receive the translated string. 
	}
	if(failFlag == FALSE)
		return nByteWritten;
	else
		return 0;
}

char* CCodeConvert::SourceToTarget(char *str)
{
	int slen = strlen(str);
	memset(tgt,0x0,slen+1);
	unsigned short look = 0;
	unsigned short out = 0;
	unsigned int i = 0;
	for(i=0;i<strlen(str);i++)
	{
		look = (unsigned char) *(str+i)*256 + (unsigned char) *(str+i+1);
		*(tgt+i) = *(bufs2t+(look)*2);//-512);
		*(tgt+i+1) = *(bufs2t+(look)*2+1);//-512);
		i++;	
	}
	*(tgt+i) = NULL;
	return tgt;
}
