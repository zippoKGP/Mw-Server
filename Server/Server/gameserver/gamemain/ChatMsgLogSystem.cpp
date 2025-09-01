#include "stdafx.h"
#include "ChatMsgLogSystem.h"
#include "MyUtil.h"
#include <process.h>

CChatMsgLogSystem::CChatMsgLogSystem()
{
	m_byCurMin = ::GetCurrentMin();

	_UpdateFilePath(update_init);

	m_listLogData.SetList(max_log_data);
	m_listLogDataEmpty.SetList(max_log_data);
	for(int i = 0; i < max_log_data; i++)
		m_listLogDataEmpty.PushNode_Back(i);

	m_bIOThread = true;
	::_beginthread(IOThread, 0, (void*)this);
}

void CChatMsgLogSystem::_UpdateFilePath(BYTE byUpdateCode)
{
	char szPath[128];

	//MAKE DIRECTORY..
	if(byUpdateCode == update_init)
	{//make main directory
		sprintf(szPath, ".\\ChatLog");
		::CreateDirectory(szPath, NULL);
	}
	if(byUpdateCode <= update_year)
	{//make year directory
		sprintf(szPath, "%s\\%d", szPath, ::GetCurrentYear());
		::CreateDirectory(szPath, NULL);
	}
	if(byUpdateCode <= update_month)
	{//make month directory
		sprintf(szPath, "%s\\%d", szPath, ::GetCurrentMonth());
		::CreateDirectory(szPath, NULL);
	}
	if(byUpdateCode <= update_day)	
	{//make day directory
		sprintf(szPath, "%s\\%d", szPath, ::GetCurrentDay());
		::CreateDirectory(szPath, NULL);
	}
	if(byUpdateCode <= update_hour)
	{//make hour directory
		sprintf(szPath, "%s\\%d", szPath, ::GetCurrentHour());
		::CreateDirectory(szPath, NULL);
	}

	//MAKE FILE NAME..	
	sprintf(m_szFileName, "%s\\%d", m_szFileName, ::GetCurrentMin());
}

