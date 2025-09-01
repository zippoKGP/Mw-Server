#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#define MAX_CONV_LEN 65536
#define BOOL int
#define TRUE 1
#define FALSE 0
#define UINT unsigned int
#define DWORD long
#define LPCWSTR unsigned short*
#define LPWSTR unsigned short*
#define LPSTR char*
#define LPCSTR char*
#define LPBOOL int*
#ifndef _WINNLS_
#define MB_PRECOMPOSED            0x00000001  // use precomposed chars
#define MB_COMPOSITE              0x00000002  // use composite chars
#define MB_USEGLYPHCHARS          0x00000004  // use glyph chars, not ctrl chars
#define MB_ERR_INVALID_CHARS      0x00000008  // error for invalid chars
#define WC_COMPOSITECHECK         0x00000200  // convert composite to precomposed
#define WC_DISCARDNS              0x00000010  // discard non-spacing chars
#define WC_SEPCHARS               0x00000020  // generate separate chars
#define WC_DEFAULTCHAR            0x00000040  // replace w/ default char
#define CP_ACP                    0           // default to ANSI code page
#define CP_OEMCP                  1           // default to OEM  code page
#define CP_MACCP                  2           // default to MAC  code page
#define CP_THREAD_ACP             3           // current thread's ANSI code page
#define CP_SYMBOL                 42          // SYMBOL translations
#define CP_UTF7                   65000       // UTF-7 translation
#define CP_UTF8                   65001       // UTF-8 translation
#endif
class CCodeConvert
{
public:
	char tgt[MAX_CONV_LEN];
	char* SourceToTarget(char* str);
	void initializeGbToBig5Table(void);
	void initializeBig5ToGbTable(void);
	void initializeSjisToBig5Table(void);
	void initializeBig5ToSjisTable(void);
	int MultiByteToWideChar(UINT CodePage,LPCSTR lpMultiByteStr,int cbMultiByte,LPWSTR lpWideCharStr,int cchWideChar);
	int WideCharToMultiByte(UINT CodePage,LPCWSTR lpWideCharStr,int cchWideChar,LPSTR lpMultiByteStr,int cbMultiByte);
	CCodeConvert();
	~CCodeConvert();
protected:
private:
	char bufBig5[131072];
	char bufUnicode[131072];
	char bufs2t[131072];
	char errMsg[64];
	FILE * datFile;
	void initializeBig5ToUnicodeTable(void);
	void initializeUnicodeToBig5Table(void);
};