// GM.cpp: implementation of the CGM class.
//
//////////////////////////////////////////////////////////////////////

#include "incall.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGM::CGM()
{
	m_stepGM = GM_NOT_CONNECTED;
	m_pvConnect = NULL;
	m_dwTimer = 0;
	m_bOnline = FALSE;
	m_dwIP = 0;
	m_dwPort = 0;
	m_dwCheckID = 0;
	m_pParent = NULL;
}

CGM::~CGM()
{

}

UG_LONG CGM::dispatchMsg(UG_DWORD dwMsgID,UG_DWORD wParam,UG_PCHAR pchData,UG_ULONG ulLen)
{
	return 0;
}

UG_LONG CGM::cout(FILE* p)
{
	fprintf(p,"beonline = %d, step = %d, login timer = %d, checkid = %d.\n",m_bOnline,m_stepGM,m_dwTimer,m_dwCheckID);
	fprintf(p,"ip = %s, port = %d.\n",convertip(m_dwIP),m_dwPort);
	fprintf(p,"\n");
	return 0;
}
