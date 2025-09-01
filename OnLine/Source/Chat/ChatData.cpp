// ChatData.cpp: implementation of the CChatData class.
//
//////////////////////////////////////////////////////////////////////
#include "ChatDef.h"
#include "ChatData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CChatData  g_sChatData;

CChatData* getChatData(){return &g_sChatData;}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatData::CChatData()
{
	
}

CChatData::~CChatData()
{
	
}

void CChatData::init()
{
	m_screenBuffer=NULL;
	memset(&m_panelRect,0,sizeof(RECT));
	memset(&m_chatRect,0,sizeof(RECT));
	m_arrfaceandstringlist.clear();
	m_arrlist.clear();
	m_chat_list.clear();
	m_uarrlist.clear();
	m_uchat_list.clear();
	m_outer_arrlist.clear();
	m_outer_chat_list.clear();
	m_nVal=0;
	s_nOldtime=game_now_time;
	s_nNewtime=game_now_time;
	m_bOuter=TRUE;
	m_hPen=NULL;
	m_hFont=NULL;
	
	
	m_pBmp=create_bitmap24(USERBMPWIDTH,USERBMPHEIGHT);
	m_pBuffer=(BYTE*)malloc(m_pBmp->h*m_pBmp->pitch);
}

void CChatData::shutDown()
{
	free(m_pBuffer);
	destroy_bitmap24(&m_pBmp);

	if (m_hPen)
	{
		DeleteObject(m_hPen);
		m_hPen=NULL;
	}
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont=NULL;
	}
	m_screenBuffer=NULL;
	m_chat_list.clear();
	m_arrlist.clear();
	m_uarrlist.clear();
	m_uchat_list.clear();
	m_arrfaceandstringlist.clear();
	m_outer_arrlist.clear();
	m_outer_chat_list.clear();
	m_nVal=0;
}
