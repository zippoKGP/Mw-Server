// Chat.cpp : Defines the initialization routines for the DLL.
//

#include "ChatDef.h"
#include "graph.h"

static CChatFace*          g_pChatFace=NULL;
static CChatPanel*         g_pChatPanel=NULL;
static vector<chat_word_t> s_chat_list;
static bool                s_bFlash=true;
static ULONG               s_fCurrentTime=0;
static CChat*              g_sChat=NULL;

CChatFace*           GetChatFace(){return g_pChatFace;}
CChatPanel*          GetChatPanel(){return g_pChatPanel;}
ULONG                getCurrentTime(void) { return s_fCurrentTime; }
bool                 getChatFlash(void) { return s_bFlash;}
bool                 setChatFlash(bool bFlash) { return (s_bFlash=bFlash); }
vector<chat_word_t>* getChatList(void){return &s_chat_list;}
CChat*               getChatObject(){return g_sChat;}

int CChat::init(char* szChatFaceFile) 
{
	getChatData()->init();

	g_pChatFace=new CChatFace();
	if (FAILED(g_pChatFace->initChatFace(szChatFaceFile)))
	{
		return E_FAIL;
	}
	g_sChat=this;
	
	g_pChatPanel=new CChatPanel();
	
	g_pChatPanel->init();


	s_chat_list.clear();

	return S_OK;
}

int CChat::shutdown(void) 
{
	if (g_pChatPanel)
	{
		g_pChatPanel->destory();
		delete g_pChatPanel;
	}
	
	if (g_pChatFace)
	{
		g_pChatFace->shutDown();
		delete g_pChatFace;
	}

	getChatData()->shutDown();

	return S_OK;
}

int CChat::updateframe(POINT ptMouse,float current_time,BMP *screen_buf,bool bFrame)
{
	getChatData()->m_bOuter=false;

	getChatData()->s_nNewtime=game_now_time;
	s_fCurrentTime=getChatData()->s_nNewtime-getChatData()->s_nOldtime;
	getChatData()->s_nOldtime=getChatData()->s_nNewtime;
	
	
	getChatData()->m_screenBuffer=screen_buf;
	
	if (getChatData()->m_screenBuffer==NULL)
	{
		return E_FAIL;
	}
	g_pChatFace->InitDraw();

	GetChatPanel()->showUserDefText(getChatData()->m_uchat_list,getChatData()->m_uarrlist);

	GetChatPanel()->showUserDefText(getChatData()->m_chat_list,getChatData()->m_arrlist);

	GetChatPanel()->ShowChatFrame();

	getChatData()->m_uchat_list.clear();

	getChatData()->m_uarrlist.clear();
	
	return S_OK;
}


int CChat::updateframe(HDC hdc, BITMAP* pbrbm)
{
	getChatData()->m_bOuter=true;

	getChatData()->s_nNewtime=game_now_time;
	s_fCurrentTime=getChatData()->s_nNewtime-getChatData()->s_nOldtime;
	getChatData()->s_nOldtime=getChatData()->s_nNewtime;
	
	getChatData()->m_hDC=hdc;
	
	g_pChatFace->InitDraw();
	
	GetChatPanel()->showUserDefText(getChatData()->m_outer_chat_list,
		getChatData()->m_outer_arrlist, pbrbm);

	GetChatPanel()->ShowChatFrame();
	
	getChatData()->m_outer_chat_list.clear();
	
	getChatData()->m_outer_arrlist.clear();

	return S_OK;
}

int CChat::updateString(ULONG x,ULONG y,ULONG w,ULONG h,const char *strChat)
{
	int len = strlen(strChat);
	char str[1024]={0};
	RECT rectFormat={x,y,x+w,y+h};	

	getChatData()->m_bOuter=false;
	
	strncpy(str,strChat,len);

	getChatData()->m_nVal++;
	
	wchar_t szWStr[1024]={0};
	
	MultiByteToWideChar(CP_ACP, 0, str, len, (wchar_t*)szWStr, len);
	
	wstring w_str=szWStr;	
	
	SIZE size = {0};
	
	chat_word_t chat_word;
	
	memset(&chat_word,0,sizeof(chat_word_t));
	chat_word.str_word     = w_str;
	
	GetChatPanel()->CreateUseDefText(chat_word,rectFormat);
	
	return S_OK;
}


int CChat::updateOuterString(ULONG x,ULONG y,ULONG w,ULONG h,const char *strChat,HDC hdc)
{
	int len = strlen(strChat);
	char str[1024]={0};
	RECT rectFormat={x,y,x+w,y+h};

	getChatData()->m_nVal++;
	
	strncpy(str,strChat,len);
	
	wchar_t szWStr[1024]={0};
	
	MultiByteToWideChar(CP_ACP, 0, str, len, (wchar_t*)szWStr, len);
	
	wstring w_str=szWStr;	
	
	SIZE size = {0};
	
	chat_word_t chat_word;
	
	memset(&chat_word,0,sizeof(chat_word_t));
	chat_word.str_word     = w_str;
	
	GetChatPanel()->CreateOuterUseDefText(chat_word,rectFormat);
	
	return S_OK;
}

int CChat::insertString(const char *strChat, RECT& rectFormat,bool bShow,HDC hdc,bool bOuter)
{
	if (rectFormat.bottom-rectFormat.top<20||rectFormat.right-rectFormat.left<20)
	{
		return E_FAIL;
	}
	
	getChatData()->m_bOuter=bOuter;
	if (getChatData()->m_bOuter)
	{
		getChatData()->m_hDC=hdc;
	}

	int len = strlen(strChat);
	char str[1024]={0};

	getChatData()->m_nVal++;
	
	strncpy(str,strChat,len);

	wchar_t szWStr[1024]={0};

	MultiByteToWideChar(CP_ACP, 0, str, len, (wchar_t*)szWStr, len);
	
	wstring w_str=szWStr;
	
	SIZE size = {0};
	
	chat_word_t chat_word;
	
	memset(&chat_word,0,sizeof(chat_word_t));
	chat_word.str_word     = w_str;

	int yRet=GetChatPanel()->CreateUseDefText(chat_word,rectFormat,bShow);

	memcpy(&rectFormat,&(chat_word.rectShow),sizeof(RECT));

	getChatData()->m_bOuter=true;

	return yRet-19;
}

int CChat::deleteString(int nStrID)
{
	getChatData()->m_chat_list.erase((UINT)nStrID);
	getChatData()->m_arrlist.erase((UINT)nStrID);
	return S_OK;
}

void CChat::clearString()
{
	getChatData()->m_chat_list.clear();
	getChatData()->m_arrlist.clear();
}

/***************************************************************************************}
{ MDA group	Exec																		}
****************************************************************************************/
SLONG mda_group_exec_24(SLONG handle,UCHR *command,SLONG x,SLONG y,BMP24 *bitmap,SLONG mda_command,SLONG value,SLONG alpha_value,SLONG frame_value,BITMAP* pbrbm,RECT* prt)
{
	SLONG mda_group_index;
	SLONG command_no;
	SLONG database_no;
	SLONG now_frame;
	UCHR database_text[255];
	SLONG offset,size;
	SLONG media_type;
	UCHR *data_buffer=NULL;
	SLONG ret_val;

	if(handle<0)return(MDA_EXEC_ERROR);
//	goto_frame=-1;
	
	// ---- Get mda_group index
	mda_group_index=get_mda_index(handle);
	if(mda_group_index<0)return(MDA_EXEC_ERROR);
	// ---- OK ! next get 
	command_no=decode_command_str(command,mda_group_index);
	if(command_no<0)
	{
		sprintf((char *)print_rec,MSG_MDA_COMMAND_ERROR,command);
		display_error_message(print_rec,true);
		return(MDA_EXEC_ERROR);
	}
	
	now_frame=frame_value;	
	
	if(decode_database_str(command_no,now_frame,mda_group_index,database_text)!=TTN_OK)
	{
        sprintf((char *)print_rec,MSG_MDA_DATABASE_FRAME_ERROR,now_frame);
		display_error_message(print_rec,true);
		return(MDA_EXEC_ERROR);
	}
	
	database_no=find_database(mda_group_index,database_text);
	if(database_no<0)			// It's a user command
	{
		exec_user_command(database_text,data_buffer);
	}
	else
	{
		offset=mda_group[mda_group_index].database_index[database_no].offset;
		size=mda_group[mda_group_index].database_index[database_no].size;
		
		data_buffer=&mda_group[mda_group_index].body[offset];
		media_type=check_media_type(data_buffer);

		mda_group[mda_group_index].img256_head=(struct IMG256_HEAD_STRUCT *)data_buffer;
		
		display_img256_buffer_24(x,y,data_buffer,bitmap, pbrbm,prt);
	}	
	
	ret_val=MDA_EXEC_OK;
	if(now_frame==0)
		ret_val=MDA_EXEC_FIRST;
	// --- Loop control
	return(ret_val);
}


void display_img256_buffer_24(SLONG x,SLONG y,UCHR *data_buffer,BMP24 *bitmap,BITMAP* pbrbm,RECT* prt)
{
	IMG256_HEAD temp_img256_head;
	SLONG display_x,display_y;
	UCHR *img256_buffer;
	SLONG yl;
	SLONG data_size;
	SLONG end_y;
	BMP24_PIXEL dwColor;
	SLONG skip_length,copy_length;
	SLONG decode_index;
	SLONG main_pass;
	UCHR alpha;
	UCHR compress_type;
	SLONG index;
	SLONG i;
	//SLONG xstart;
	
	
	
	// ------ Get Base Data
	memcpy(&temp_img256_head,data_buffer,sizeof(struct IMG256_HEAD_STRUCT));
	yl=temp_img256_head.ylen;
	data_size=temp_img256_head.image_compress_size;
	// stephen
	prt->right = temp_img256_head.xlen;
	prt->bottom = temp_img256_head.ylen;
	prt->left -= temp_img256_head.center_x;
	prt->top -= temp_img256_head.center_y;
	display_x=0; //x -temp_img256_head.center_x;
	display_y=0;//y-temp_img256_head.center_y;
	img256_buffer=&data_buffer[temp_img256_head.img_start_offset];


	// ------ Display Loop
	end_y=display_y+yl;
	skip_length=0;
	copy_length=0;
	decode_index=0;
	main_pass=0;
	while(main_pass==0)
	{
		compress_type=img256_buffer[decode_index];
		decode_index++;
		switch(compress_type)
		{
		case IMG_SKIP:
			skip_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			display_x=display_x+skip_length;
			decode_index=decode_index+2;
			break;			
		case IMG_COPY:
			copy_length=img256_buffer[decode_index]+
				(img256_buffer[decode_index+1]<<8);
			decode_index=decode_index+2;
			
			for(i=0;i<copy_length;i++)
			{
				if(((display_x+i)<bitmap->w)&&
					(display_y<bitmap->h)&&
					(display_x+i>=0)&&
					(display_y>=0))
				{
					index=(SLONG)img256_buffer[decode_index+(i<<1)]*3;
					alpha=img256_buffer[decode_index+(i<<1)+1];
					

					if(alpha< 50)
					{
						continue;						
					}

					UCHR r=temp_img256_head.palette[index];
					UCHR g=temp_img256_head.palette[index+1];
					UCHR b=temp_img256_head.palette[index+2];
					
					//memset(&dwColor, 0, sizeof(dwColor));					

					if(r == 0 && g == 0&& b == 0)
					{
						dwColor.buff[2] = 1; 
						dwColor.buff[1] = 1;
						dwColor.buff[0] = 1;

						if(alpha > 200)
						{
							memcpy(&bitmap->line[display_y][display_x+i] , &dwColor, sizeof(dwColor));
							continue;							
						}
					}
					else
					{
						dwColor.buff[2] = r;
						dwColor.buff[1] = g;
						dwColor.buff[0] = b;
						
						if(alpha > 200)
						{
							memcpy(&bitmap->line[display_y][display_x+i] , &dwColor, sizeof(dwColor));
							continue;							
						}
					}					
					
					SLONG x = (prt->left +display_x+i)%pbrbm->bmWidth;
					SLONG y = (prt->top +display_y)%pbrbm->bmHeight;//pbrbm->bmHeight-1-
					
					if (y<0) 
					{
						break;
					}

					UCHR* pPixel = (UCHR*)pbrbm->bmBits+ (y*pbrbm->bmWidthBytes)+(x*3);
					bitmap->line[display_y][display_x+i].buff[0] = *(pPixel++);
					bitmap->line[display_y][display_x+i].buff[1] = *(pPixel++);
					bitmap->line[display_y][display_x+i].buff[2] = *(pPixel);

					_oper_alpha_color_any_24(&dwColor,(BMP24_PIXEL *)&bitmap->line[display_y][display_x+i],alpha);
				}
				else
				{
	    				int erro = 0;
				}
			}

			display_x=display_x+copy_length;
			decode_index=decode_index+(copy_length<<1);
			break;
			
		case IMG_LINE:
			display_y++;
			display_x=x;//-temp_img256_head.center_x;
			break;
		}
		if((decode_index>=data_size)||(display_y>=end_y))
			main_pass=1;
	}
	
	
}


void   clear_bitmap24(BMP24 *bmp)
{
    if(NULL==bmp) return;
    for(SLONG y=bmp->h-1; y>=0; y--)
    {
        memset(bmp->line[y],0x00,bmp->pitch);
    }
}

BMP24*   create_bitmap24(SLONG w,SLONG h)
{
    BMP24 *   tmp;
    BMP24_PIXEL * t;
    int     i,bmpw=w;
    
    if (w&3) w=(w+3)&0xfffffffc;    // memory align(4 bytes)
    tmp=(BMP24 *)GlobalAlloc(GPTR, sizeof(BMP24)+(h-1)*sizeof(BMP24_PIXEL *)+w*h*sizeof(BMP24_PIXEL));
    if (!tmp) return NULL;
    tmp->w = bmpw;
    tmp->pitch = w*sizeof(BMP24_PIXEL);
    tmp->h = h;
    t=tmp->line[0]=(BMP24_PIXEL *)((char *)tmp+sizeof(BMP24)+(h-1)*sizeof(BMP24_PIXEL *));
    for (i=1;i<h;i++)
        tmp->line[i]=(t+=w);
    return tmp;
}

void   destroy_bitmap24(BMP24 **bmp)
{
    if(*bmp)
    {
        GlobalFree(*bmp);
        *bmp=NULL;
    }
}


void    _oper_alpha_color_any_24(BMP24_PIXEL *fore_color,BMP24_PIXEL *back_color,SLONG alpha)
{
    UCHR fr,fg,fb;
    UCHR br,bg,bb;
    UCHR r,g,b;
	
	fr = fore_color->buff[2];
	fg = fore_color->buff[1];
	fb = fore_color->buff[0];

	br = back_color->buff[2];
	bg = back_color->buff[1];
	bb = back_color->buff[0];

    r = (UCHR)( br + (fr-br)*alpha/255 );
    g = (UCHR)( bg + (fg-bg)*alpha/255 );
    b = (UCHR)( bb + (fb-bb)*alpha/255 );
    back_color->buff[2] = r;
	back_color->buff[1] = g;
	back_color->buff[0] = b;
		
}