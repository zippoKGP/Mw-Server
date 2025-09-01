// FileLine.h: interface for the CFileLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELINE_H__99C53E46_C980_482D_997F_85827C7FB891__INCLUDED_)
#define AFX_FILELINE_H__99C53E46_C980_482D_997F_85827C7FB891__INCLUDED_

#include "net/UGBenDef.h"

typedef struct _tag_file_line
{
	UG_ULONG		ulLine; //���򣬴�0��ʼ
	UG_ULONG		ulFilePos; //��ʼλ��
	UG_ULONG		ulLineCount; //���Ȳ�����\r\n
	_tag_file_line()
	{
		ulLine = 0;
		ulFilePos = 0;
		ulLineCount = 0;
	}
	
} FILE_LINE, *PFILE_LINE;

class CFileLine  
{
public:
	CFileLine();
	virtual ~CFileLine();

public:
	UG_ULONG				init(UG_PCHAR pchPathName,UG_ULONG ulLen = 2048);
	UG_ULONG				cleanup();

public:
	list<FILE_LINE>			m_list;
	UG_ULONG				m_ulMaxLen;

};

#endif // !defined(AFX_FILELINE_H__99C53E46_C980_482D_997F_85827C7FB891__INCLUDED_)
