/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2004 ALL RIGHTS RESERVED.

  *file name    : account.cpp
  *owner  		: Andy
  *description  : 
  *modified     : 2004/12/17
******************************************************************************/ 

#include "engine_global.h"
#include "control_src/control_src.h"
#include "utility.h"
#include "system.h"
#include "vstdlib/strtools.h"
#include "vari-ext.h"

#include "account.h"
#include "net.h"
#include "player_info.h"
#include "net_msg.h"
#include "login.h"
#include "graph.h"
#include "data.h"
#include "taskman.h"

SLONG account_main_pass = 0;

SLONG account_main_function = 0;

static CUGEditBox *s_pActiveEdit = NULL;


char m_strName[100] = {0};

class CNameEdit : public CUGEditBox
{
public:
    CNameEdit() : CUGEditBox(NULL)
	{
		m_pBrother = NULL;

		s_pActiveEdit = this;

		m_nMaxChar = 16;		
	}
	
private:
	typedef CUGEditBox BaseClass;
	
	CUGEditBox *m_pBrother;

	// driver stuff
private:
	virtual HRESULT OnEnter(void);
	
	virtual HRESULT OnTab(void) { return OnEnter(); }
		
public:
	void setBrother(CUGEditBox *edit) { m_pBrother = edit; }

	virtual CUGControl* LBtnDown(POINT ptCursor);	

	HRESULT Enter(void) { return OnEnter(); }
};

HRESULT CNameEdit::OnEnter(void)
{
	if (m_pBrother)
	{
		setFocus(false);

		m_pBrother->setFocus();

		s_pActiveEdit = m_pBrother;
	}

	return S_OK;
}

CUGControl* CNameEdit::LBtnDown(POINT ptCursor)
{
	if (BaseClass::LBtnDown(ptCursor))
	{
		if (m_pBrother)
		{
			m_pBrother->setFocus(false);
		}		

		setFocus(true);
				
		s_pActiveEdit = this;

		return this;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//
class CPasswordEdit : public CUGEditBox
{
public:
    CPasswordEdit() : CUGEditBox(NULL)
	{
		m_pBrother = NULL;

		m_nMaxChar = 16;
	}
	
private:
	typedef CUGEditBox BaseClass;

	CUtlVector<char> m_strPassword;

	CUGEditBox *m_pBrother;
	
	// driver stuff
private:
	virtual HRESULT insertChar(UINT nChar);

	virtual HRESULT drawText(void *bmp = NULL, DWORD dwColor = 0xffffffff);

	virtual HRESULT OnEnter(void);

	virtual HRESULT OnBackSpace(void);

	virtual HRESULT OnDelete(void);

	virtual HRESULT OnTab(void);
	
public:
	void setBrother(CUGEditBox *edit) { m_pBrother = edit; }	

	virtual CUGControl* LBtnDown(POINT ptCursor);

public:
	HRESULT Enter(void) { return OnEnter(); }

};

HRESULT CPasswordEdit::insertChar(UINT nChar)
{
	if (nChar & 0x80)
	{
		return E_FAIL;
	}

	int insert_idx = m_nCursorIndex-1;

	if (BaseClass::insertChar('*') == S_OK)
	{
		m_strPassword.InsertAfter(insert_idx, nChar);
	}

	return S_OK;
}

HRESULT CPasswordEdit::drawText(void *bmp, DWORD dwColor)
{
	BaseClass::drawText(bmp, dwColor);

	return S_OK;
}

HRESULT CPasswordEdit::OnEnter(void)
{	
	// 检测用户名是否已经填充
	if (m_pBrother)
	{
		if (m_pBrother->getCharNum() == 0)
		{
			display_error_message((UCHR *)MSG_ACCOUNT_EMERY,true);

			return E_FAIL;
		}
	}

	// 检测密码是否已经填充
	if (getCharNum() == 0)
	{
		display_error_message((UCHR*)MSG_ACCOUNT_EMERY,true);

		return E_FAIL;
	}

	MW_ACCOUNT account = {0};

	char text[1024] = {0};

	m_pBrother->getText(text);

	Q_strncpy(account.name, text, sizeof(account.name));

	Q_strncpy(m_strName, account.name, sizeof(m_strName));

	Q_memset(text, 0, sizeof(text));

	int count = min(sizeof(text), m_strPassword.Count());

	Q_strncpy(text, m_strPassword.Base(), count+1);

	Q_strncpy(account.passwd, text, sizeof(account.passwd));

	postNetMessage(NET_MSG_CHECK_USER, 0, (char*)&account, sizeof(account) );
	
	account_main_function = RETURN_OK;

	return S_OK;
}

HRESULT CPasswordEdit::OnBackSpace(void)
{
	if (m_nSelectStart != m_nSelectEnd)
	{
		m_strPassword.RemoveMultiple(m_nSelectStart, m_nSelectEnd-m_nSelectStart);
	}
	else if (m_nCursorIndex-1 >= 0)
	{
		m_strPassword.Remove(m_nCursorIndex-1);
	}
	
	return BaseClass::OnBackSpace();
}

HRESULT CPasswordEdit::OnDelete(void)
{
	if(m_nCursorIndex < m_wordList.Size() && m_nSelectStart == m_nSelectEnd)
	{
		m_strPassword.Remove(m_nCursorIndex);
	}
	else if (m_nSelectStart < m_nSelectEnd)
	{
		m_strPassword.RemoveMultiple(m_nSelectStart, m_nSelectEnd-m_nSelectStart);
	}

	BaseClass::OnDelete();

	return S_OK;
}

HRESULT CPasswordEdit::OnTab(void) 
{ 
	if (m_pBrother)
	{
		setFocus(false);
		
		m_pBrother->setFocus();
		
		s_pActiveEdit = m_pBrother;
	}
	
	return S_OK;	
}

CUGControl* CPasswordEdit::LBtnDown(POINT ptCursor)
{
	if (BaseClass::LBtnDown(ptCursor))
	{
		if (m_pBrother)
		{
			m_pBrother->setFocus(false);
		}		
		
		setFocus(true);
		
		s_pActiveEdit = this;
		
		return this;
	}
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////
SLONG account_function(void)
{
	SLONG old_mouse_x,old_mouse_y;
	SLONG main_function;
	POINT ptMouse={ 400,300 };
	//	CUGButton button[2];			// IDX ( 0,1,2 ) 开始游戏
	
//	const char *med_name = "SYSTEM/c1meu008.MDA";
	
	CNameEdit editName;
	CPasswordEdit editPassword;	
	
	SLONG button_handle;
	SLONG menu_handle;
	SLONG mda_command;
	ULONG timer;
	SLONG button_status[2];
	SLONG i;
	RECT menu_rect[2];
	POSITION menu_center[2];
	SLONG idx;
	UCHR command_text[80];
	SLONG button_select,old_button_select;
	UCHR big_mask[80];
	UCHR small_mask[80];
	UCHR push_big_mask[80];
	UCHR push_small_mask[80];
	
	
	POSITION mask_pos[2]={
		{ 609,464 },
		{ 609,513 }
		
	};
	
	POSITION crystal_dark_pos[2]=
	{
		{ 752,449 },
		{ 752,498 }
		
	};
	
	POSITION crystal_light_pos[2]=
	{
		{ 741,438 },
		{ 741,487 }
		
	};
	
	SLONG crystal_size[2]={						// 水晶大小 0 -> 小, 1 -> 大
		1,
			1
	};
	
	SLONG mask_size[2]={						// 光晕大小 0 -> 小, 1 -> 大
		0,
			0
	};
	
	
	//////////////////////////////////////////////////////////////////////////
	//	
	RECT rect_text = { 334, 215, 456, 230 };	
	RECT rect_cur = rect_text;
	//	editName.Create(med_name, NULL, NULL, 9);
	editName.setEnable();
	editName.setPosRect(&rect_text);
	editName.setTextOutRect(&rect_text);
	editName.setCursorRect(&rect_cur);
	editName.setBrother(&editPassword);
	
	//////////////////////////////////////////////////////////////////////////
	//
	SetRect(&rect_text, 334, 269, 456, 284);
	rect_cur = rect_text;
	editPassword.setEnable();
	editPassword.setFont(FONT24);
	editPassword.setPosRect(&rect_text);
	editPassword.setTextOutRect(&rect_text);
	editPassword.setCursorRect(&rect_cur);
	editPassword.setBrother(&editName);
	
	if (s_pActiveEdit)
	{
		s_pActiveEdit->setFocus();		
	}
	
	if(load_jpg_file((UCHR *)"SYSTEM/c1meu008.jpg",screen_channel0,0,0,800,600)!=TTN_OK)
	{
		return(RETURN_ERROR);
	}
	
	if(mda_group_open((UCHR *)"system\\Button.mda",&button_handle)!=TTN_OK)
		return(RETURN_ERROR);
	if(mda_group_open((UCHR *)"system\\c1meu008.MDA",&menu_handle)!=TTN_OK)
		return(RETURN_ERROR);
	
	
	
	for(i=0;i<2;i++)
	{
		sprintf((char *)command_text,"MainCommand%d",i);
		mda_group_exec(menu_handle,command_text,0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
		idx=get_mda_index(menu_handle);
		menu_rect[i].left=mda_group[idx].img256_head->orig_x;
		menu_rect[i].top=mda_group[idx].img256_head->orig_y;
		menu_rect[i].right=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->xlen;
		menu_rect[i].bottom=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->ylen;
		menu_center[i].x=mda_group[idx].img256_head->orig_x+mda_group[idx].img256_head->center_x;
		menu_center[i].y=mda_group[idx].img256_head->orig_y+mda_group[idx].img256_head->center_y;
		button_status[i]=0;
		sprintf((char *)command_text,"MainCommand%d",i);
		mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_channel0,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
		
	}
	
	strcpy((char *)big_mask,"MainCommand0");
	strcpy((char *)small_mask,"MainCommand2");
	strcpy((char *)push_big_mask,"MainCommand1");
	strcpy((char *)push_small_mask,"MainCommand3");
	
	
	
	
	change_screen_effect3(8);
	
	
	timer=GetTickCount();
	account_main_pass = 0;
	old_button_select=-1;

	reset_ripple();
	old_mouse_x = old_mouse_y = 0;
	
	while(account_main_pass==0)
	{
		system_idle_loop();
		
		if(!IsIconic(g_hDDWnd))
		{	
			
			main_function=-1;
			
			get_bitmap(0,0,800,600,screen_buffer,screen_channel0);
			
			RippleSpread();
			RippleRender();
			
			mda_command=MDA_COMMAND_LOOP;
			if(system_control.system_timer>=timer+SYSTEM_MAIN_DELAY)
			{
				mda_command=mda_command|MDA_COMMAND_NEXT_FRAME;
				timer=system_control.system_timer;
			}
			if (s_pActiveEdit == &editPassword)			// Password
			{
				mda_group_exec(button_handle,(UCHR *)"MainCommand7",293,228,screen_buffer,mda_command,0,0,0);
				mda_group_exec(button_handle,(UCHR *)"MainCommand7",468,228,screen_buffer,mda_command,0,0,0);
			}
			else 										// Account
			{
				mda_group_exec(button_handle,(UCHR *)"MainCommand7",293,176,screen_buffer,mda_command,0,0,0);
				mda_group_exec(button_handle,(UCHR *)"MainCommand7",468,176,screen_buffer,mda_command,0,0,0);
				
			}
			
			
			button_select=-1;
			for(i=0;i<2;i++)
			{
				button_status[i]=0;
				if((system_control.mouse_x>=menu_rect[i].left)&&(system_control.mouse_x<menu_rect[i].right)&&
					(system_control.mouse_y>=menu_rect[i].top)&&(system_control.mouse_y<menu_rect[i].bottom))
				{
					if(system_control.mouse_key==MS_LDn)				
						button_status[i]=2;
					else
						button_status[i]=1;
					button_select=i;
					if(old_button_select!=button_select)
					{
						idx=get_mda_index(button_handle);
						mda_group[idx].now_frame[0]=0;
						mda_group[idx].now_frame[2]=0;
						old_button_select=button_select;
					}
					
				}
			}
			
			if(main_function==-1)
			{
				idx=get_mda_index(button_handle);
				mda_group[idx].now_frame[1]=0;
				mda_group[idx].now_frame[3]=0;
			}
			
			UCHR key_num = get_char_key_num();
			
			bool bkey_press = false;
			
			for (int key_count = 0; key_count < key_num; key_count ++)
			{
				UCHR key_char = get_char_key(key_count);
				
				if ((key_char >= 'a' && key_char <= 'z') || 
					(key_char >= 'A' && key_char <= 'Z') ||
					(key_char >= '0' && key_char <= '9') ||
					(key_char == '_'))
				{
					s_pActiveEdit->OnChar(key_char, 0, 0);
					
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
//					if (!g_bImeActive) 
					{
						s_pActiveEdit->OnKeyDown(key_system, 0, 0);						
					}
				}
			}
			
			reset_data_key();
			
			ptMouse.x = system_control.mouse_x;
			ptMouse.y = system_control.mouse_y;
			
			//		mouse_key=get_mouse_key();
			
			switch(system_control.mouse_key)
			{
			case MS_LDn:
				{
					main_function=-1;
					
					if (editName.LBtnDown(ptMouse))
					{
						
					}
					else if (editPassword.LBtnDown(ptMouse))
					{
					}
					else // 按钮
					{
						
					}
					
					reset_mouse_key();
				}
				push_ripple(system_control.mouse_x,system_control.mouse_y,1000);
				
				break;
				
			case MS_LUp:
				
				switch(button_select)
				{
				case 0:					// Back
					{
						disconnectServer();
					}
					account_main_pass=1;
					main_function=RETURN_CANCEL;
					account_main_function = RETURN_CANCEL;
					break;
				case 1:					// Next
					{
						editPassword.Enter();
					}
					// test 
					//				account_main_pass=1;
					
					main_function=RETURN_OK;
					account_main_function = RETURN_OK;
					
					break;
				}
				
				
				break;
				case MS_LDrag:
					if((old_mouse_x!=system_control.mouse_x)||
						(old_mouse_y!=system_control.mouse_y))
					{
						old_mouse_x=system_control.mouse_x;
						old_mouse_y=system_control.mouse_y;
						push_ripple(system_control.mouse_x,system_control.mouse_y,2000);
						
					}			
					break;
					
				default:
					if((old_mouse_x!=system_control.mouse_x)||
						(old_mouse_y!=system_control.mouse_y))
					{
						old_mouse_x=system_control.mouse_x;
						old_mouse_y=system_control.mouse_y;
						push_ripple(system_control.mouse_x,system_control.mouse_y,100);
					}
					break;
			}
			
			editName.Update(ptMouse);
			editName.Paint(screen_buffer);
			editPassword.Update(ptMouse);
			editPassword.Paint(screen_buffer);
			
			reset_mouse_key();				
			
			
			// --- Draw button
			for(i=0;i<2;i++)
			{
				switch(button_status[i])
				{
				case 0:				// None 
					sprintf((char *)command_text,"MainCommand%d",i);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME|MDA_COMMAND_LOOP,0,0,0);
					if(crystal_size[i]==1)
						mda_group_exec(button_handle,(UCHR *)"MainCommand4",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					else
						mda_group_exec(button_handle,(UCHR *)"MainCommand6",crystal_dark_pos[i].x,
						crystal_dark_pos[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 1:			// Select
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,(UCHR *)command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)big_mask);
						else
							strcpy((char *)command_text,(char *)small_mask);
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,mda_command,0,0,0);
						
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				case 2:			// Push
					if(crystal_size[i]==1)
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand5",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					else
					{
						if(mask_size[i]==1)
							strcpy((char *)command_text,(char *)push_big_mask);
						else
							strcpy((char *)command_text,(char *)push_small_mask);
						
						mda_group_exec(button_handle,command_text,mask_pos[i].x,
							mask_pos[i].y,screen_buffer,MDA_COMMAND_NEXT_FRAME,0,0,0);
						mda_group_exec(button_handle,(UCHR *)"MainCommand7",crystal_light_pos[i].x,
							crystal_light_pos[i].y,screen_buffer,mda_command,0,0,0);
					}
					sprintf((char *)command_text,"MainCommand%d",i);
					mda_group_exec(menu_handle,command_text,menu_center[i].x,menu_center[i].y,screen_buffer,mda_command,0,0,0);
					break;
				}
			}
			
			execute_window_base();
			update_screen(screen_buffer);
		}
	}
	
	
	
	mda_group_close(&button_handle);
	mda_group_close(&menu_handle);
	
	postNetMessage(NET_MSG_PLAYER_CHARACTER_LIST, 0);
	
	return (account_main_function);
	
}


