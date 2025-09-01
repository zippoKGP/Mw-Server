/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : edit.cpp
  *owner  		: Andy
  *description  : 
  *modified     : 2005/1/29
******************************************************************************/ 

#include "engine_global.h"
#include "edit.h"
#include "system.h"
#include "utility.h"
#include "vari-ext.h"
#include "chat_msg.h"
#include "data_proc.h"
#include "net.h"


inline bool str_all_space(const char* str)
{
	while (*str)
	{
		if (*str != ' ')
		{
			return false;
		}

		str ++;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
static CUtlVector<CUGEditBox*> s_edit_list;

void insertEdit(CUGEditBox* pEdit)
{
	if (s_edit_list.Find(pEdit) == s_edit_list.InvalidIndex())
	{
		s_edit_list.AddToTail(pEdit);
	}
}

void removeEdit(CUGEditBox* pEdit)
{
	s_edit_list.FindAndRemove(pEdit);

	if (s_edit_list.Count() > 0)
	{
		s_edit_list[s_edit_list.Count()-1]->setFocus();
	}
}

void setEditFocus(CUGEditBox *pEdit)
{
	if (s_edit_list.Find(pEdit) != s_edit_list.InvalidIndex())
	{
		for (int i = 0; i < s_edit_list.Count(); i ++)
		{
			if (s_edit_list[i] == pEdit)
			{
				pEdit->setFocus();
			}
			else
			{
				s_edit_list[i]->setFocus(false);
			}
		}
	}
}

CUGEditBox *getActiveEdit(void)
{
	for (int i = 0; i < s_edit_list.Count(); i ++)
	{
		if (s_edit_list[i]->getFocus())
		{
			return s_edit_list[i];
		}
	}

	return NULL;
}

bool UpdateEditKey(void)
{
	for (int i = 0; i < s_edit_list.Count(); i ++)
	{
		if (s_edit_list[i]->getFocus())
		{
			if (s_edit_list[i] == &s_editChat)
			{
				return s_editChat.updateKey();
			}
			else if (s_edit_list[i] == &s_FriendEdit)
			{
				return s_FriendEdit.updateKey();				
			}
			else if (s_edit_list[i] == &g_FindIDEdit)
			{
				return g_FindIDEdit.updateKey();
			}
			else if (s_edit_list[i] == &g_FindNickNameEdit)
			{
				return g_FindNickNameEdit.updateKey();
			}
			else if (s_edit_list[i] == &g_PetNameEdit)
			{
				return g_PetNameEdit.updateKey();
			}
			else if (s_edit_list[i] == &g_GiveEdit)
			{
				return g_GiveEdit.updateKey();
			}
			else if (s_edit_list[i] == &g_DealEdit)
			{
				return g_DealEdit.updateKey();
			}
			else if (s_edit_list[i] == &g_BankEdit)
			{
				return g_BankEdit.updateKey();
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//
CChatEdit::CChatEdit(void) : CUGEditBox(NULL)
{
	m_nMaxChar = MAX_CHAT_LENGTH;
}

CChatEdit::~CChatEdit(void)
{

}

HRESULT CChatEdit::Init(void)
{
	insertEdit(this);
	
	RECT rect_text = { 69, 570, 399, 586 };	
	RECT rect_cur = rect_text;
	
	setEnable();
	setPosRect(&rect_text);
	setTextOutRect(&rect_text);
	setCursorRect(&rect_cur);

	setFocus();

	return S_OK;
}

HRESULT CChatEdit::Shutdown(void)
{
	removeEdit(this);	

	return S_OK;
}

CChatEdit s_editChat;

HRESULT CChatEdit::OnEnter(void)
{
	char chat_str[2048] = {0};

	ULONG check_timer;
	SLONG old_channel;
	
	getText(chat_str);

	if (!(*chat_str) || str_all_space(chat_str))
	{
		goto end_return;
	}

	if(check_is_system_command((UCHR *)&chat_str[0])==TTN_OK)
		goto end_return;

	old_channel=game_control.chat_channel;
	check_chat_command((UCHR *)&chat_str[0]);
	check_emote((UCHR *)&chat_str[0]);
	

	switch(game_control.chat_channel)
	{
	case CHAT_CHANNEL_SCREEN:			// 当前频道
		check_timer=CHAT_CHANNEL_SCREEN_TIMER;
		break;
	case CHAT_CHANNEL_TEAM:				// 队伍
		check_timer=CHAT_CHANNEL_TEAM_TIMER;
		break;
	case CHAT_CHANNEL_PERSON:			// 私聊
		check_timer=CHAT_CHANNEL_PERSON_TIMER;
		break;
	case CHAT_CHANNEL_GROUP:			// 公会
		check_timer=CHAT_CHANNEL_GROUP_TIMER;
		break;
	case CHAT_CHANNEL_SELLBUY:			// 经济
		check_timer=CHAT_CHANNEL_SELLBUY_TIMER;
		break;
	case CHAT_CHANNEL_WORLD:			// 世界
		check_timer=CHAT_CHANNEL_WORLD_TIMER;
		break;	
	default:
		check_timer=CHAT_CHANNEL_SCREEN_TIMER;
		break;
	}
	
	if(system_control.system_timer>game_control.chat_timer+check_timer)
	{
		game_control.chat_timer=system_control.system_timer;

		switch (game_control.chat_channel)
		{
		case CHAT_CHANNEL_WORLD:
			WorldChat(chat_str, system_control.control_user_id);
			break;

		case CHAT_CHANNEL_PERSON:
			PrivateChat(chat_str, system_control.control_user_id, system_control.person_npc_id);
			break;

		case CHAT_CHANNEL_SCREEN:
			CurrentChat(chat_str, system_control.control_user_id);
			break;

		case CHAT_CHANNEL_TEAM:
			GroupChat(chat_str, system_control.control_user_id);
			break;
		}
	}
	else
	{
		push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",(UCHR *)MSG_CHAT_TIMER_ERROR);
	}
	
	if(old_channel>=0)
		game_control.chat_channel=old_channel;

	push_chat_stack((UCHR *)chat_str);
	game_control.chat_stack_find_index=game_control.chat_stack_index;

end_return:
	return CUGEditBox::OnEnter();
}

HRESULT CChatEdit::OnEscape(void)
{
	Erase();

	return S_OK;
}

bool CChatEdit::updateKey(void)
{
	if (!getFocus())
	{
		return false;
	}

	HRESULT hr = S_OK;

	UCHR key_num = get_char_key_num();
	
	bool bkey_press = false;
	
	for (int key_count = 0; key_count < key_num; key_count ++)
	{
		UCHR key_char = get_char_key(key_count);
		
		if ((hr = OnChar(key_char, 0, 0)) == S_OK)
		{
			bkey_press = true;
		}
	}

	reset_char_key();
	
	if (!bkey_press)
	{
		UCHR key_system = system_control.key;
		
		if (key_system == 0)
		{
			key_system = system_control.data_key;
		}

		if (key_system != 0)
		{
//			if (!g_bImeActive)
			{
				hr = OnKeyDown(key_system, 0, 0);
			}
		}
	}
	
	if(system_control.data_key!=NULL)
		reset_data_key();

	return ((hr == S_OK) ? true : false);
}

HRESULT CChatEdit::setFocus(void)
{
	setEditFocus(this);

	reset_char_key();
	reset_data_key();
	
	system_control.key = 0;	
	system_control.data_key = 0;
	
	return S_OK;
}

HRESULT CChatEdit::LBtnDown(int x, int y)
{
	POINT pt = {x, y};
	
	if (CUGEditBox::LBtnDown(pt))
	{
		setFocus();
		
		return 1;
	}
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

CFriendEdit::CFriendEdit(void) : CUGEditBox(NULL)
{
}

CFriendEdit::~CFriendEdit(void)
{
}

CFriendEdit s_FriendEdit;

HRESULT CFriendEdit::Init(void)
{	
	m_nMaxChar = MAX_CHAT_LENGTH;
	
	insertEdit(this);
	
	SetRect(&m_Rect, 33, 57, 368+33, 16+57);

	setPosRect(&m_Rect);	
	setCursorRect(&m_Rect);	
	setTextOutRect(&m_Rect);
	
	setFocus();

	setEnable();
	
	Erase();
	
	return S_OK;
}

HRESULT CFriendEdit::Shutdown(void)
{	
	Erase();
	
	removeEdit(this);

	CUGEditBox::setFocus(false);

	return S_OK;
}

HRESULT CFriendEdit::OnEnter(void)
{
	char chat_str[1024] = {0};
	
	getText(chat_str);	
	
	chat_str[m_nMaxChar] = 0;

	if (!(*chat_str) || str_all_space(chat_str))
	{
		goto end_return;
	}

#if 0
	for (int i = 0; i < 100; i ++)
	{
		char test_str[2048];

		Q_snprintf(test_str, sizeof(test_str), "%d  %s", i, chat_str);

		FriendChat(test_str, system_control.control_user_id, send_message_data.id);
	}
#else
	FriendChat(chat_str, system_control.control_user_id, send_message_data.id);
#endif

	
//	now_delete_window_handle=now_exec_window_handle;
	now_delete_window_handle=game_control.window_send_message;
	
	game_control.window_send_message=-1;
	
end_return:
	CUGEditBox::OnEnter();

	Shutdown();

	return S_OK;
}

HRESULT CFriendEdit::Paint(int x, int y, void *bmp)
{
	RECT rect = m_Rect;

	OffsetRect(&rect, x, y);

	setPosRect(&rect);
	setTextOutRect(&rect);
	setCursorRect(&rect);

	CUGEditBox::Paint(bmp);

	return S_OK;
}

bool CFriendEdit::updateKey(void)
{
	if (!getFocus())
	{
		return false;
	}

	HRESULT hr = S_OK;

	UCHR key_num = get_char_key_num();
	
	bool bkey_press = false;
	
	for (int key_count = 0; key_count < key_num; key_count ++)
	{
		UCHR key_char = get_char_key(key_count);
		
		if ((hr = OnChar(key_char, 0, 0)) == S_OK)
		{
			bkey_press = true;
		}
	}
	
	reset_char_key();
	
	if (!bkey_press)
	{
		UCHR key_system = system_control.key;
		
		if (key_system == 0)
		{
			key_system = system_control.data_key;
		}
		
		if (key_system != 0)
		{
//			if (!g_bImeActive)
			{
				hr = OnKeyDown(key_system, 0, 0);
			}
		}
	}

	if(system_control.data_key!=NULL)
		reset_data_key();
	
	return ((hr == S_OK) ? true : false);
}	

HRESULT CFriendEdit::setFocus(void)
{
	setEditFocus(this);

	reset_char_key();
	reset_data_key();

	system_control.key = 0;	
	system_control.data_key = 0;
	
	return S_OK;
}

HRESULT CFriendEdit::LBtnDown(int x, int y)
{
	POINT pt = {x, y};

	if (CUGEditBox::LBtnDown(pt))
	{
		setFocus();

		return 1;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//
//////////////////////////////////////////////////////////////////////////
CNickNameBox::CNickNameBox(void) : CUGEditBox(NULL)
{
	m_nMaxChar = MAX_NAME_LENGTH;
}
	
CNickNameBox::~CNickNameBox()
{
	
}

HRESULT CNickNameBox::Init(void)
{
	RECT rect_text = { 172, 511, 283, 527 };	
	RECT rect_cur = rect_text;
	
	//	editName.Create(med_name, NULL, NULL, 9);
	
	setEnable();
	setPosRect(&rect_text);
	setTextOutRect(&rect_text);
	setCursorRect(&rect_cur);
	setFocus();
	
	return S_OK;
}

HRESULT CNickNameBox::Shutdown(void)
{
	return S_OK;
}

bool CNickNameBox::updateKey(void)
{
	HRESULT hr = S_OK;
	
	UCHR key_num = get_char_key_num();
	
	bool bkey_press = false;
	
	for (int key_count = 0; key_count < key_num; key_count ++)
	{
		UCHR key_char = get_char_key(key_count);
		
		if ((hr = OnChar(key_char, 0, 0)) == S_OK)
		{
			bkey_press = true;
		}
	}
	
	reset_char_key();
	
	if (!bkey_press)
	{
		UCHR key_system = system_control.key;
		
		if (key_system == 0)
		{
			key_system = system_control.data_key;
		}
		
		if (key_system != 0)
		{
//			if (!g_bImeActive)
			{
				hr = OnKeyDown(key_system, 0, 0);
			}
		}
	}

	if(system_control.data_key!=NULL)
		reset_data_key();
	
	return ((hr == S_OK) ? true : false);
}	

//////////////////////////////////////////////////////////////////////////
// 查找好友用的昵称对话框
CFindNickNameBox::CFindNickNameBox(void)
{

}
	
CFindNickNameBox::~CFindNickNameBox(void)
{

}

CFindNickNameBox g_FindNickNameEdit;

HRESULT CFindNickNameBox::Init(void)
{
	insertEdit(this);
	
	SetRect(&m_Rect, 77, 56, 77+112, 56+16);
	
	setFocus();
	
	setEnable();
		
	m_nMaxChar = MAX_NAME_LENGTH;

	return S_OK;
}

HRESULT CFindNickNameBox::Shutdown(void)
{
	removeEdit(this);	
	
	return S_OK;
}

HRESULT CFindNickNameBox::OnTab(void)
{
	extern CFindIDBox g_FindIDEdit;

	g_FindIDEdit.CFriendEdit::setFocus();

	return S_OK;
}

HRESULT CFindNickNameBox::OnEnter(void)
{
	char name[MAX_NAME_LENGTH+1] = {0};
	
	getText(name);
		
	name[MAX_NAME_LENGTH] = 0;

	postNetMessage(NET_MSG_PLAYER_FIND_FRIEND, FIND_FRIEND_TYPE_NAME, name, strlen(name)+1);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// id
CFindIDBox::CFindIDBox(void)
{

}

CFindIDBox::~CFindIDBox(void)
{

}

CFindIDBox g_FindIDEdit;

HRESULT CFindIDBox::Init(void)
{
	insertEdit(this);
	
	SetRect(&m_Rect, 77, 89, 77+112, 89+16);
	
	setFocus();
	
	setEnable();
	
	m_nMaxChar = 9;
	
	return S_OK;
}

HRESULT CFindIDBox::OnChar(UINT nChar, UINT nRepcnt, UINT nFlags)
{
	if (nChar >= '0' && nChar <= '9')
	{
		return CUGEditBox::OnChar(nChar, nRepcnt, nFlags);
	}

	return E_FAIL;
}

HRESULT CFindIDBox::OnTab(void)
{
	g_FindNickNameEdit.CFriendEdit::setFocus();

	return S_OK;
}

HRESULT CFindIDBox::OnEnter(void)
{
	char str_num[128] = {0};

	getText(str_num);

	int id = Q_atoi(str_num);

	postNetMessage(NET_MSG_PLAYER_FIND_FRIEND, FIND_FRIEND_TYPE_ID, (char*)&id, sizeof(id));

	return S_OK;
}

HRESULT CFindIDBox::Shutdown(void)
{
	removeEdit(this);	
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
//
CPetNameEdit::CPetNameEdit(void)
{

}

CPetNameEdit::~CPetNameEdit(void)
{

}

CPetNameEdit g_PetNameEdit;

HRESULT CPetNameEdit::Init(void)
{
	m_nMaxChar = MAX_BAOBAO_NAME_LENGTH;

	insertEdit(this);

	SetRect(&m_Rect, 76, 210, 76+118, 210+16);
	
	setPosRect(&m_Rect);	
	setCursorRect(&m_Rect);	
	setTextOutRect(&m_Rect);
	
	setFocus();
	
	setEnable();
	
	Erase();

	return S_OK;
}

HRESULT CPetNameEdit::OnEnter(void)
{
	char chat_str[1024] = {0};
	
	getText(chat_str);	
	
	chat_str[m_nMaxChar] = 0;
	
	if (!(*chat_str) || str_all_space(chat_str))
	{
		return E_FAIL;
	}

	//base_baobao_data.data.active_idx

	if (baobao_stack_index >= 0 && baobao_stack_index < MAX_BAOBAO_STACK)
	{
		if (Q_strcmp(base_baobao_data.data.baobao_list[baobao_stack_index].name, chat_str) != 0)
		{
			int id = base_baobao_data.data.baobao_list[baobao_stack_index].baobao_id;

			postNetMessage(NET_MSG_PLAYER_BAOBAO_NAME, id, chat_str, strlen(chat_str)+1);					
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//
//////////////////////////////////////////////////////////////////////////
CNumberEdit::CNumberEdit(void) : CUGEditBox(NULL)
{
	Q_memset(&m_Rect, 0, sizeof(m_Rect));

	m_nMaxNum = (DWORD)-1;
}

CNumberEdit::~CNumberEdit(void)
{

}

CNumberEdit g_GiveEdit;
CNumberEdit g_DealEdit;
CNumberEdit g_BankEdit;

//////////////////////////////////////////////////////////////////////////
//
HRESULT CNumberEdit::Init(RECT& rect, int max_char)
{
	insertEdit(this);
	
	m_Rect = rect;

//	SetRect(&m_Rect, 77, 89, 77+112, 89+16);
	
	setFocus();
	
	setEnable();
	
	m_nMaxChar = max_char;

	return S_OK;
}

HRESULT CNumberEdit::Shutdown(void)
{
	Erase();

	return S_OK;
}

HRESULT CNumberEdit::OnChar(UINT nChar, UINT nRepcnt, UINT nFlags)
{
	if (CheckChar(nChar))
	{
		return CUGEditBox::OnChar(nChar, nRepcnt, nFlags);
	}

	return E_FAIL;
}

HRESULT CNumberEdit::OnEnter(void)
{
//	removeEdit(this);	
	Erase();

	return S_OK;
}

HRESULT CNumberEdit::Paint(int x, int y, void *bmp)
{
	RECT rect = m_Rect;
	
	OffsetRect(&rect, x, y);
	
	setPosRect(&rect);
	setTextOutRect(&rect);
	setCursorRect(&rect);
	
	CUGEditBox::Paint(bmp);
	
	return S_OK;
}

bool CNumberEdit::updateKey(void)
{
	if (!getFocus())
	{
		return false;
	}
	
	HRESULT hr = S_OK;
	
	UCHR key_num = get_char_key_num();
	
	bool bkey_press = false;
	
	for (int key_count = 0; key_count < key_num; key_count ++)
	{
		UCHR key_char = get_char_key(key_count);
		
		if ((hr = OnChar(key_char, 0, 0)) == S_OK)
		{
			bkey_press = true;
		}
	}
	
	reset_char_key();
	
	if (!bkey_press)
	{
		UCHR key_system = system_control.key;
		
		if (key_system == 0)
		{
			key_system = system_control.data_key;
		}
		
		if (key_system != 0)
		{
//			if (!g_bImeActive)
			{
				hr = OnKeyDown(key_system, 0, 0);
			}
		}
	}
	
	if(system_control.data_key!=NULL)
		reset_data_key();
	
	return ((hr == S_OK) ? true : false);	
}

HRESULT CNumberEdit::setFocus(void)
{
	setEditFocus(this);
	
	reset_char_key();
	reset_data_key();
	
	system_control.key = 0;	
	system_control.data_key = 0;
	
	return S_OK;
}

HRESULT CNumberEdit::LBtnDown(int x, int y)
{
	POINT pt = {x, y};

	if (CUGEditBox::LBtnDown(pt))
	{
		setFocus();
		
		return 1;
	}
	
	return S_OK;
}

int CNumberEdit::getNumber(void)
{
	char num[1024];

	getText(num);

	return Q_atoi(num);
}

void CNumberEdit::setNumber(int num)
{
	char str[1024];

	sprintf(str, "%d", num);

	Erase();

	setText(str);
}

void CNumberEdit::setMaxNum(unsigned int max_num)
{
	m_nMaxNum = max_num;
}

bool CNumberEdit::CheckChar(int nChar)
{
	if (nChar >= '0' && nChar <= '9')
	{
		if (getCharNum() == 0 && nChar == '0')
		{
			return false;
		}

		char text[128] = {0};

		getText(text);

		int len = strlen(text);

		text[len] = nChar;

		text[len+1] = 0;

		unsigned int num = Q_atoi(text);

		if (num > m_nMaxNum)
		{
			return false;
		}

		return true;
	}

	return false;
}
