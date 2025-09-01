/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : chatpanel.cpp
*owner        : Robert
*description  : 主聊天面板
*modified     : 2004/05/01
******************************************************************************/ 

#include "ChatDef.h"
#include "ChatPanel.h"


//////////////////////////////////////////////////////////////////////////
//function name :CChatPanel
//description	:初始化成员变量
//return		:
//parameter		:RECT rect,CFontMgr *pFont,RECT chatRect
//modified		:Robert
//////////////////////////////////////////////////////////////////////////

CChatPanel::CChatPanel()
{	
	m_ChatBottom=m_chatRect.bottom;
	
	
	m_dwColor=SYSTEM_WHITE;
	
	m_underline=0;
	
	m_flash=0;
	
	m_flashTimeOpen=0;

	m_flashChat=0;
	
	m_flashTimeOff=0;
	
	m_scrollFlag=1;
	
	m_scrollTime=0;

	m_nBgArgb=CHAT_BR_ARGB;
	
	m_showType[0]=1;
	
	m_showType[1]=1;
	
	m_showType[2]=1;
	
	m_showType[3]=1;

	m_bFlash=true;

	m_bMiddle=false;

}

//////////////////////////////////////////////////////////////////////////
//function name :~CChatPanel
//description	:
//return		:
//parameter		:
//modified		:Robert
//////////////////////////////////////////////////////////////////////////

CChatPanel::~CChatPanel()
{
	
}

//////////////////////////////////////////////////////////////////////////
//function name :destory
//description	:释放资源
//return		:int
//parameter		:void
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
int CChatPanel::destory()
{
	return S_OK;
}



//////////////////////////////////////////////////////////////////////////
//function name :init
//description	:初始化聊天图片资源
//return		:int
//parameter		:void
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
int CChatPanel::init()
{
//	RECT rect={800,600,820,620};
	memset(m_szChat,0,sizeof(m_szChat));
//	getChatObject()->insertString("",rect,false);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//function name :GetColor
//description	:转换RGB颜色值
//return		:UINT32
//parameter		:wstring colorStr
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
UHINT CChatPanel::GetColor(wstring colorStr,int& pos)
{
	UINT32 dwColorTemp=0;
	
	BYTE   cTemp=0;
	int    j=0;
	
	for (int i=5;i>=0;i--)
	{
		switch((colorStr.at(i))) {
		case L'a': ;
		case L'A': cTemp=10;j++;break;
		case L'b': ;
		case L'B':cTemp=11;j++;break;
		case L'c': ;
		case L'C':cTemp=12;j++;break;
		case L'd': ;
		case L'D':cTemp=13;j++;break;
		case L'e': ;
		case L'E':cTemp=14;j++;break;
		case L'f': ;
		case L'F':cTemp=15;j++;break;
		case L'0':cTemp=0;j++;break;
		case L'1':cTemp=1;j++;break;
		case L'2':cTemp=2;j++;break;
		case L'3':cTemp=3;j++;break;
		case L'4':cTemp=4;j++;break;
		case L'5':cTemp=5;j++;break;
		case L'6':cTemp=6;j++;break;
		case L'7':cTemp=7;j++;break;
		case L'8':cTemp=8;j++;break;
		case L'9':cTemp=9;j++;break;
		default : cTemp=0;j=0;dwColorTemp=0;break;
		}

		dwColorTemp=dwColorTemp+ (cTemp << (4*(j-1)));
		

	}
	dwColorTemp=dwColorTemp+((UINT32)15 << 24)+((UINT32)15 << 28);
	byte *p=(byte*)&dwColorTemp;
	BYTE r,g,b;
	r = p[2];
	g = p[1];
	b = p[0];
	UHINT nColor= (UHINT)( (((UHINT)r<<8)&R_MASK_565) | 
		(((UHINT)g<<3)&G_MASK_565) | (((UHINT)b>>3)&B_MASK_565) );
	pos+=j;

	return nColor;
}


//////////////////////////////////////////////////////////////////////////
//function name :DisposeEffect
//description	:进行聊天#后面的特殊效果处理
//return		:void
//parameter		:wstring wStr,wstring disposeStr,int &position,int &line_width,int &height,int &line_height
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
void CChatPanel::DisposeEffect(wstring wStr,wstring disposeStr,int &position,int &line_width,int &height,int &line_height)
{
	if (disposeStr.length()!=2)
		return ;

	WCHAR ckey=disposeStr.at(1);

	switch((unsigned short)ckey) {
	case 'R': ;
	case 'r': ;
	case (unsigned short) CHAT_RED:
		m_dwColor=SYSTEM_RED;
		break;
	case 'G': ;
	case 'g': ;
	case (unsigned short) CHAT_GREEN:
		m_dwColor=SYSTEM_GREEN;
		break;
	case 'B': ;
	case 'b': ;
	case (unsigned short) CHAT_BLUE:
		m_dwColor=SYSTEM_BLUE;
		break;
	case 'K': ;
	case 'k': ;
	case (unsigned short) CHAT_BLACK:
		m_dwColor=SYSTEM_BLACK;
		break;
	case 'W': ;
	case 'w': ;
	case (unsigned short) CHAT_WHITE:
		m_dwColor=SYSTEM_WHITE;
		break;
	case 'Y': ;
	case 'y': ;
	case (unsigned short) CHAT_YELLOW:
		m_dwColor=SYSTEM_YELLOW;
		break;
	case 'P': ;
	case 'p': ;
	case (unsigned short) CHAT_PINK:
		m_dwColor=SYSTEM_PINK;
		break;

	case 'c': ;
	case 'C': 
		if (wStr.length()<position+8)
		{
			position=wStr.length()-1;

			break;
		}
		m_dwColor=GetColor(wStr.substr(position,6),position);

		break;

	case 'U': ;
	case 'u': 
		m_underline=!m_underline;
		break;
	case '#':
		{
			wstring str=L"#";

			drawChatText(str,line_width,height,line_height,0);
		}
		break;
	case 'e': ;
	case 'E':
		{
			
			if (linewidth!=0)
				linenum=linenum+1;

			linewidth=0;

			
		}
		break;
	case 'S': ;
	case 's': ;
	case (unsigned short) CHAT_FLASH:
		{
			m_flash=!m_flash;
		}
		break;

	case 'n': ;
	case 'N': 
		{
			m_dwColor=SYSTEM_WHITE;

			m_underline=0;

			m_flash=0;
		}
		break;

	case 'm': ;
	case 'M': 
		{
			
			
			if (linewidth!=0)
				linenum=linenum+1;

			linewidth=0;

			m_bMiddle=!m_bMiddle;

		}
		break;
	default:
		break;
	}
	return;

}

//////////////////////////////////////////////////////////////////////////
//function name :SetChatRect
//description	:重新设置聊天文字区域
//return		:void
//parameter		:RECT & destRect,RECT scrRect
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
void CChatPanel::SetChatRect(RECT & destRect,RECT scrRect)
{
	if (scrRect.top<destRect.top)
	{
		destRect.top=scrRect.top;
	}
	if (scrRect.bottom>destRect.bottom)
	{
		destRect.bottom=scrRect.bottom;
	}	
	if (scrRect.left<destRect.left)
	{
		destRect.left=scrRect.left;
	}
	if (scrRect.right>destRect.right)
	{
		destRect.right=scrRect.right;
	}	
	return;
}
/*
//////////////////////////////////////////////////////////////////////////
//function name :InitChatRect
//description	:设置聊天文字区域
//return		:void
//parameter		:RECT & destRect,RECT scrRect
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
void CChatPanel::InitChatRect(RECT & destRect,RECT scrRect)
{
	destRect=scrRect;

	return;
}
*/

HRESULT  CChatPanel::setUseDefLine(chat_word_t &char_word,int &line_height)
{
	int i,j;

	int rectheight=0;

	int offsetheight=0;

	RECT rect;

	for (i=1;i<=linenum;i++)
	{
		rectheight=rectheight+m_lineinfo.find(i)->second;
	}

	char_word.rectChat=m_panelRect;
	
	offsetheight=(m_panelRect.bottom-m_panelRect.top)-rectheight;

	
	SetRect(&char_word.rectShow,1024,768,0,0);
	
	for (i=0;i<m_faceandstring_list.size();i++)
	{
		
		for (j=linenum;j>m_faceandstring_list[i].linenum;j--)
		{
			if (m_faceandstring_list[i].faceFlag)
			{
				if (m_faceandstring_list[i].picFlag==-1)
				{
					m_faceandstring_list[i].pt.y=m_faceandstring_list[i].pt.y-m_lineinfo.find(j)->second;
				}
				else
				{
					OffsetRect(&m_faceandstring_list[i].rect,0,-(m_lineinfo.find(j)->second));
				}
				
			}
			else
			{
				OffsetRect(&m_faceandstring_list[i].rect,0,-(m_lineinfo.find(j)->second));
			}
		}
		if (m_faceandstring_list[i].faceFlag)
		{
			m_faceandstring_list[i].pt.y=m_faceandstring_list[i].pt.y-offsetheight;
		}
		else
		{
			OffsetRect(&m_faceandstring_list[i].rect,0,-(offsetheight));
		}

	
		if (m_faceandstring_list[i].faceFlag)
		{
			if (m_faceandstring_list[i].picFlag==-1)
			{
				::SetRect(&rect,m_faceandstring_list[i].pt.x,m_faceandstring_list[i].pt.y,
					m_faceandstring_list[i].pt.x+m_faceandstring_list[i].faceW,m_faceandstring_list[i].pt.y+m_faceandstring_list[i].faceH);
				
				SetChatRect(char_word.rectShow,rect);
			}
			else
			{
				::SetRect(&rect,m_faceandstring_list[i].rect.left,m_faceandstring_list[i].rect.top,
					m_faceandstring_list[i].rect.right,m_faceandstring_list[i].rect.bottom);			
				
				SetChatRect(char_word.rectShow,rect);
			}
		}
		else
		{
			::SetRect(&rect,m_faceandstring_list[i].rect.left,m_faceandstring_list[i].rect.top,
				m_faceandstring_list[i].rect.right,m_faceandstring_list[i].rect.bottom);			
			
			SetChatRect(char_word.rectShow,rect);
		}
			
	}
	
	return S_OK;
	
}

/*
HRESULT  CChatPanel::setChatLine(chat_word_t &char_word,int &line_height)
{
	int i,j;
	
	RECT rect;
	
	if (!m_faceandstring_list.empty())
	{
		if (m_faceandstring_list[0].faceFlag)
		{
			if (m_faceandstring_list[0].picFlag==-1)
			{
				::SetRect(&rect,m_panelRect.left,m_faceandstring_list[0].pt.y,
					m_chatRect.right,m_faceandstring_list[0].pt.y+m_faceandstring_list[0].faceH);
				
				InitChatRect(char_word.rectChat,rect);
			}
			else
			{
				::SetRect(&rect,m_panelRect.left,m_faceandstring_list[0].rect.top,
					m_chatRect.right,m_faceandstring_list[0].rect.bottom);			
				
				InitChatRect(char_word.rectChat,rect);
			}
		}
		else
		{
			::SetRect(&rect,m_panelRect.left,m_faceandstring_list[0].rect.top,
				m_chatRect.right,m_faceandstring_list[0].rect.bottom);			
			
			InitChatRect(char_word.rectChat,rect);
		}
	}
	
	
	for (i=0;i<m_faceandstring_list.size();i++)
	{
		
		for (j=linenum;j>m_faceandstring_list[i].linenum;j--)
		{
			if (m_faceandstring_list[i].faceFlag)
			{
				if (m_faceandstring_list[i].picFlag==-1)
				{
					m_faceandstring_list[i].pt.y=m_faceandstring_list[i].pt.y-m_lineinfo.find(j)->second;
				}
				else
				{
					OffsetRect(&m_faceandstring_list[i].rect,0,-(m_lineinfo.find(j)->second));
				}
				
			}
			else
			{
				OffsetRect(&m_faceandstring_list[i].rect,0,-(m_lineinfo.find(j)->second));
			}
		}		
		
		if (m_faceandstring_list[i].faceFlag)
		{
			if (m_faceandstring_list[i].picFlag==-1)
			{
				::SetRect(&rect,m_panelRect.left,m_faceandstring_list[i].pt.y,
					m_chatRect.right,m_faceandstring_list[i].pt.y+m_faceandstring_list[i].faceH);
				
				SetChatRect(char_word.rectChat,rect);
			}
			else
			{
				SetChatRect(char_word.rectChat,m_faceandstring_list[i].rect);
			}
		}
		else
		{	
			SetChatRect(char_word.rectChat,m_faceandstring_list[i].rect);
		}
	}
	
	for (i=1;i<=linenum;i++)
	{
		line_height=line_height+m_lineinfo.find(i)->second;
	}

	return S_OK;

}
*/
/*
//////////////////////////////////////////////////////////////////////////
//function name :ShowChat
//description	:显示当前的一句聊天
//return		:void
//parameter		:chat_word_t &char_word,int &line_height
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
void CChatPanel::ShowChat(chat_word_t &char_word,FACEANDSTRINGLIST& m_faceandstring_list)
{	
	RECT lineRect;

	if ((char_word.rectChat.top>getChatData()->m_panelRect.top)&&(char_word.rectChat.bottom<=getChatData()->m_panelRect.bottom))
	{
		for (int index=0;index<m_faceandstring_list.size();index++)
		{
			if (m_faceandstring_list[index].faceFlag)
			{
				if (m_faceandstring_list[index].picFlag==-1)
				{
					GetChatFace()->ShowChatFace(m_faceandstring_list[index].facename,m_faceandstring_list[index].pt);
					
					if (m_faceandstring_list[index].underline)
					{
						SetRect(&lineRect,m_faceandstring_list[index].pt.x,m_faceandstring_list[index].pt.y+m_faceandstring_list[index].faceH,m_faceandstring_list[index].pt.x+m_faceandstring_list[index].faceW,m_faceandstring_list[index].pt.y+m_faceandstring_list[index].faceH);
						
						getUGDraw()->drawLine(&lineRect,getChatData()->m_screenBuffer,m_faceandstring_list[index].dwColor);
					}
				}
			}
			else
			{
				
				

				getUGDraw()->SetWordColor(m_faceandstring_list[index].dwColor);

				if (m_faceandstring_list[index].flash)
				{
					if (m_flashTimeOpen<FLASHTIMEOPEN)
					{
						getUGDraw()->ShowText(m_faceandstring_list[index].rect,m_faceandstring_list[index].w_str.data());
						if (m_faceandstring_list[index].underline)
						{
							SetRect(&lineRect,m_faceandstring_list[index].rect.left,m_faceandstring_list[index].rect.bottom,m_faceandstring_list[index].rect.right,m_faceandstring_list[index].rect.bottom);
							
							getUGDraw()->drawLine(&lineRect,getChatData()->m_screenBuffer,m_faceandstring_list[index].dwColor);
						}
					}
				}
				else
				{
					
					getUGDraw()->ShowText(m_faceandstring_list[index].rect,m_faceandstring_list[index].w_str.data());

					if (m_faceandstring_list[index].underline)
					{
						SetRect(&lineRect,m_faceandstring_list[index].rect.left,m_faceandstring_list[index].rect.bottom,m_faceandstring_list[index].rect.right,m_faceandstring_list[index].rect.bottom);
						
						getUGDraw()->drawLine(&lineRect,getChatData()->m_screenBuffer,m_faceandstring_list[index].dwColor);
					}
				}
	
			}
		}

		getUGDraw()->SetWordColor(SYSTEM_WHITE);
	}
}
*/
//////////////////////////////////////////////////////////////////////////
//function name :ShowChat
//description	:显示当前的一句聊天
//return		:void
//parameter		:chat_word_t &char_word,int &line_height
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
void CChatPanel::ShowUserDef(chat_word_t &char_word,FACEANDSTRINGLIST& m_faceandstring_list, BITMAP* pbrbm)
{	
	RECT lineRect;
	
		for (int index=0;index<m_faceandstring_list.size();index++)
		{

			if (m_faceandstring_list[index].faceFlag)
			{
				if ((m_faceandstring_list[index].pt.y+m_faceandstring_list[index].faceH)>char_word.rectChat.bottom)
				{
					continue;
				}
			}
			else
			{
				if (m_faceandstring_list[index].rect.bottom>char_word.rectChat.bottom)
				{
					continue;
				}
			}


			if (m_faceandstring_list[index].faceFlag)
			{
				if (m_faceandstring_list[index].picFlag==-1)
				{
					
					GetChatFace()->ShowChatFace(m_faceandstring_list[index].facename,m_faceandstring_list[index].pt, pbrbm);
					
					if (m_faceandstring_list[index].underline)
					{
						SetRect(&lineRect,m_faceandstring_list[index].pt.x,m_faceandstring_list[index].pt.y+m_faceandstring_list[index].faceH,m_faceandstring_list[index].pt.x+m_faceandstring_list[index].faceW,m_faceandstring_list[index].pt.y+m_faceandstring_list[index].faceH);
						
						getUGDraw()->drawLine(&lineRect,getChatData()->m_screenBuffer,m_faceandstring_list[index].dwColor);
					}
				}
			}
			else
			{
			
				getUGDraw()->SetWordColor(m_faceandstring_list[index].dwColor);

				if (m_faceandstring_list[index].flash)
				{
					if (m_flashTimeOpen<FLASHTIMEOPEN)
					{
						getUGDraw()->ShowText(m_faceandstring_list[index].rect,m_faceandstring_list[index].w_str.data());
						if (m_faceandstring_list[index].underline)
						{
							SetRect(&lineRect,m_faceandstring_list[index].rect.left,m_faceandstring_list[index].rect.bottom,m_faceandstring_list[index].rect.right,m_faceandstring_list[index].rect.bottom);
							
							getUGDraw()->drawLine(&lineRect,getChatData()->m_screenBuffer,m_faceandstring_list[index].dwColor);
						}
					}
				}
				else
				{	
					getUGDraw()->ShowText(m_faceandstring_list[index].rect,m_faceandstring_list[index].w_str.data());
					
					if (m_faceandstring_list[index].underline)
					{
						SetRect(&lineRect,m_faceandstring_list[index].rect.left,m_faceandstring_list[index].rect.bottom,m_faceandstring_list[index].rect.right,m_faceandstring_list[index].rect.bottom);
						
						getUGDraw()->drawLine(&lineRect,getChatData()->m_screenBuffer,m_faceandstring_list[index].dwColor);
					}
				}
	
			}
		}

		getUGDraw()->SetWordColor(SYSTEM_WHITE);	

}
//////////////////////////////////////////////////////////////////////////
//function name :CheckTextOrFace
//description	:检查聊天内容是文字还是表情动画
//return		:wstring
//parameter		:wstring checkStr,int &position,UINT8 &nKey
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
wstring CChatPanel::CheckTextOrFace(wstring checkStr,int &position,UINT8 &nKey)
{
	wchar_t findcode = CHATCODE ;
	
	wstring returnStr=L"";
	
	wchar_t wchr;
	
	int pos=checkStr.find(findcode,position);

	if (pos==-1)
	{
		position=pos+1;
		
		return returnStr;
	}
	
	
	returnStr.append(1,CHATCODE);

	if (checkStr.length()<=(pos+1))
	{
		position=pos+2;
		
		returnStr.append(1,L'');
		
		nKey=2;
		
		return returnStr;
	}
	
	wchr=checkStr.at(pos+1);
	
	if ((unsigned short)wchr>127)
	{
		position=pos+2;
		
		returnStr.append(1,wchr);
		
		nKey=2;
		
		return returnStr;

	}
	else
	{
		if (isdigit((char)wchr)==0)
		{
			position=pos+2;
			
			returnStr.append(1,wchr);
			
			nKey=2;
				
			return returnStr;
		}	
	}
	
	for (int index=1;index<4;index++)
	{
		if (checkStr.length()<=(pos+index))
		{
			nKey=1;
			
			return returnStr;
		}
		
		wchr=checkStr.at(pos+index);
		
		if ((unsigned short)wchr<127)
		{
			if (isdigit((char)wchr)==0)
			{
				position=pos+index;
				
				break;
			}
			else
			{
				returnStr.append(1,(wchr));
				
				position=pos+index+1;
			}
		}
		else
		{
			position=pos+index;
			
			break;
		}
		
	}

	nKey=1;
	
	return returnStr;

}

//////////////////////////////////////////////////////////////////////////
//function name :drawChatText
//description	:对一段聊天文字进行换行处理
//return		:HRESULT
//parameter		:wstring char_word,int &line_width,int &height,int &line_height,int lineFlag,int chat_type_id
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
HRESULT CChatPanel::drawChatText(wstring char_word,int &line_width,int &height,int &line_height,int lineFlag,int chat_type_id)
{	
	wstring& w_str = char_word;
	
	int pixel_width;
	
	int i,j=0;
	
	SIZE size={0,0};
	
	RECT rect1;

	if (lineFlag)
	{
		linewidth=0;
	}
	
	do 
	{
		pixel_width=0;
		
		if (lineFlag)
		{
			linenum=linenum+1;
			
			if (linenum!=1)
			{
				linewidth=0;
			}
			
		}
		
		wstring w_strtemp;
		
		for (i=j;i<=w_str.size();i++)
		{
			w_strtemp=w_str.substr(j,i-j);
			
			getUGDraw()->getStrSize(w_strtemp,size);

			pixel_width=size.cx+line_width+m_chatRect.left+linewidth;
			
			if (pixel_width>m_chatRect.right)
			{
				lineFlag=1;
				
				break;
			}
		}
		
		int nLen=lineFlag?(i-j-1):(i-j);
		wstring& w_str1=w_str.substr(j,nLen);
		
		j=j+nLen;
		if (w_str.size()==j)
		{
			lineFlag=0;	
		}
		

		getUGDraw()->getStrSize(w_str1,size);
		
		SetRect(&rect1 ,m_chatRect.left+linewidth, m_chatRect.bottom-height-size.cy, 
			size.cx+m_chatRect.left+linewidth, 
			m_chatRect.bottom-height);
		
		
		
		pt.x=rect1.right;
		
		pt.y=rect1.bottom;
		
		SetLineHeight(size.cy);
		
		
		m_faceandstring.faceFlag   =0;
		m_faceandstring.linenum    =linenum;
		m_faceandstring.rect       =rect1;
		m_faceandstring.w_str      =w_str1;
		m_faceandstring.dwColor    =m_dwColor;
		m_faceandstring.underline  =m_underline;
		m_faceandstring.flash      =m_flash;
		
		m_faceandstring_list.push_back(m_faceandstring);

		
		}while (j<w_str.size());

		//linewidth=rect1.right-(line_width+m_chatRect.left);	
		linewidth=linewidth+rect1.right-rect1.left;
		
	
		return S_OK;
}


//////////////////////////////////////////////////////////////////////////
//function name :SetLineHeight
//description	:对当前行进行行高设置
//return		:void
//parameter		:int lineheight
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
void CChatPanel::SetLineHeight(int lineheight)
{
	int findFlag=1;

	for (pointer=m_lineinfo.begin();pointer!=m_lineinfo.end();pointer++)
	{
		if (linenum==(pointer->first))
		{
			findFlag=0;

			if (lineheight>(pointer->second))
			{

				pointer->second=lineheight;
			}
			break;
		}	
	}
	
	if (findFlag)
	{
		m_lineinfo.insert(LINEINFO::value_type(linenum,lineheight));
	}
}

//////////////////////////////////////////////////////////////////////////
//function name :SetLineHeight
//description	:对当前行进行行高设置
//return		:HRESULT
//parameter		:int lineheight
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
HRESULT CChatPanel::drawTextAndFace(chat_word_t char_word,int &line_width,int &height,int &line_height)
{
	static  bool  bMiddle=m_bMiddle;

	static  long  nLineWidthBak=0;  
	
	const int MAXVALUE=3;

	int pos=0,lastpos=pos;

	wstring faceStr;

	wstring findStr=L"#";

	string tempStr;

	string mstr;

	char temp[MAXVALUE];

	wstring w_str;

	UINT8 uKey=0;

	linenum=linenum+1;

	linewidth=0;

	if (linenum==1)
	{		
		linewidth=0;
	}
	do
	{

		uKey=0;
		
		faceStr=CheckTextOrFace(char_word.str_word,pos,uKey);
		
		if (faceStr.length()==0)
			break;
		
		if (faceStr.compare(findStr)!=0)
		{
			
			w_str=char_word.str_word.substr(lastpos,pos-faceStr.length()-lastpos);
			
			
			drawChatText(w_str,line_width,height,line_height,0);
			
			nLineWidthBak=linewidth;
			
			switch (uKey)
			{
			
				case 1:
				{
					mstr="";
					for (int n=0;n<faceStr.length();n++)
					{
						mstr.append(1,(unsigned short)faceStr.at(n));
					}

					tempStr=mstr.substr(1,mstr.length()-1);
					sprintf(temp,"%3d",atoi(tempStr.data()));
					for (int i = 0;i<MAXVALUE; i ++)
					{
						if (temp[i]==(char)(32))
						{
							temp[i]='0';
						}
					}
					mstr=temp;

					linewidth=linewidth+GetChatFace()->GetChatWidth(mstr);

					
					
					if ((line_width+linewidth+m_chatRect.left)>m_chatRect.right)
					{

						linenum=linenum+1;

						linewidth=0;

						pt.x=line_width+linewidth+m_chatRect.left;

						pt.y=m_chatRect.bottom-height-GetChatFace()->GetChatHeight(mstr);

						linewidth=GetChatFace()->GetChatWidth(mstr);

						m_faceandstring.faceFlag   =1;
						m_faceandstring.picFlag    =-1;
						m_faceandstring.facename   =mstr;
						m_faceandstring.pt         =pt;
						m_faceandstring.faceW      =GetChatFace()->GetChatWidth(mstr);
						m_faceandstring.faceH      =GetChatFace()->GetChatHeight(mstr);
						m_faceandstring.linenum    =linenum;
						m_faceandstring.underline  =m_underline;
						
						m_faceandstring_list.push_back(m_faceandstring);

						SetLineHeight(m_faceandstring.faceH);

					}
					else
					{

						m_faceandstring.faceFlag   =1;
						m_faceandstring.picFlag    =-1;
						m_faceandstring.facename   =mstr;
						m_faceandstring.faceW      =GetChatFace()->GetChatWidth(mstr);
						m_faceandstring.faceH      =GetChatFace()->GetChatHeight(mstr);
						pt.y=pt.y-m_faceandstring.faceH;
						m_faceandstring.pt         =pt;
						m_faceandstring.linenum    =linenum;
						m_faceandstring.underline  =m_underline;

						m_faceandstring_list.push_back(m_faceandstring);
					
						SetLineHeight(m_faceandstring.faceH);
					}

				
				}

					break;
				case 2:
					{
						DisposeEffect(char_word.str_word,faceStr,pos,line_width,height,line_height);
					}
					break;

				default:
					break;
			}
			if ((!m_bMiddle)&&(bMiddle))
			{
				int nLeft=(m_chatRect.right-m_chatRect.left-nLineWidthBak)/2;
				
				for (int n=0;n<m_faceandstring_list.size();n++)
				{
					if (m_faceandstring_list[n].linenum==linenum-1)
					{
						m_faceandstring_list[n].pt.x=m_faceandstring_list[n].pt.x+nLeft;
						m_faceandstring_list[n].rect.left=m_faceandstring_list[n].rect.left+nLeft;
						m_faceandstring_list[n].rect.right=m_faceandstring_list[n].rect.right+nLeft;
					}
				}
			}
			bMiddle=m_bMiddle;
			lastpos=pos;
		}
		
	}while (pos<char_word.str_word.size());
	
	if (pos<char_word.str_word.size())
	{
		w_str=char_word.str_word.substr(lastpos,char_word.str_word.length()-lastpos);

		drawChatText(w_str,line_width,height,line_height,0);
	}

	return S_OK;

}

//////////////////////////////////////////////////////////////////////////
//function name :ShowChatFrame
//description	:外部调用的显示所有聊天记录
//return		:HRESULT
//parameter		:void
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
HRESULT CChatPanel::ShowChatFrame()
{	
	if (m_flashTimeOpen<FLASHTIMEOPEN)
	{
		m_flashTimeOpen+=getCurrentTime();
		m_flashTimeOff=0;
	}
	else{
		if (m_flashTimeOff<FLASHTIMEOFF) 
		{
			m_flashTimeOff+=getCurrentTime();
		}
		else
		{
			m_flashTimeOpen=0;
		}
	}
	
	return S_OK;

}
/*
bool  CChatPanel::getFlashChatNow()
{
	if (getChatFlash()||m_flashChat>FLASHCHATTIME)
	{
		SetChat_List(*getChatList());
		setChatFlash(false);
		m_bFlash=true;
		m_flashChat=0;
	}
	return m_bFlash;
}

*/
int CChatPanel::CreateUseDefText(chat_word_t &chat_word,RECT rectFormat,bool bShow)
{	
	AutoShow(chat_word,rectFormat);

	if (bShow)
	{
		getChatData()->m_chat_list.insert(CHATWORDLIST::value_type(getChatData()->m_nVal,chat_word));
		
		getChatData()->m_arrlist.insert(ARFACEANDSTRINGLIST::value_type(getChatData()->m_nVal,m_faceandstring_list));

	}
	
	return m_lineinfo.find(1)->second;
	
}


void CChatPanel::showUserDefText(CHATWORDLIST& chat_list,ARFACEANDSTRINGLIST& arrlist, BITMAP* pbrbm)
{
	for (m_pointerforfaceandstring=arrlist.begin();
	m_pointerforfaceandstring!=arrlist.end();m_pointerforfaceandstring++)
	{
		
		UINT s=m_pointerforfaceandstring->first;
		m_pointerchatword=chat_list.find(s);
		if (m_pointerchatword!=chat_list.end())
		{
			ShowUserDef((m_pointerchatword->second),(m_pointerforfaceandstring->second), pbrbm);
		}
		
	}
	
}


void CChatPanel::CreateUseDefText(chat_word_t &chat_word,RECT rectFormat)
{
	AutoShow(chat_word,rectFormat);	

	getChatData()->m_uchat_list.insert(CHATWORDLIST::value_type(getChatData()->m_nVal,chat_word));
		
	getChatData()->m_uarrlist.insert(ARFACEANDSTRINGLIST::value_type(getChatData()->m_nVal,m_faceandstring_list));
		
}


void CChatPanel::AutoShow(chat_word_t &chat_word, RECT rectFormat)
{
	int line_height = 0;
	
	int line_width = 0;
	
	int height = 0;
	
	int nLineNum=0;
	
	wchar_t findcode = CHATCODE;
	
	int pos=-1;
	
	m_bFlash=false;
	
	m_bMiddle=false;
	
	
	
	m_chatRect=rectFormat;
	
	m_panelRect=rectFormat;
	
	m_dwColor=SYSTEM_WHITE;
	
	m_underline=0;
	
	m_flash=0;
	
	linenum=0;
	
	m_faceandstring_list.clear();
	
	m_lineinfo.clear();
	
	line_height=line_height + 1;
	
	pos=chat_word.str_word.find(findcode);
	
	if (pos==-1)
	{
		drawChatText(chat_word.str_word,line_width,height,line_height,1,chat_word.chat_type_id);
		
	}
	else
	{
		drawTextAndFace(chat_word,line_width,height,line_height);
		
	}
	
	setUseDefLine(chat_word,line_height);
}

void CChatPanel::CreateOuterUseDefText(chat_word_t &chat_word, RECT rectFormat)
{
	AutoShow(chat_word,rectFormat);	
	
	getChatData()->m_outer_chat_list.insert(CHATWORDLIST::value_type(getChatData()->m_nVal,chat_word));
	
	getChatData()->m_outer_arrlist.insert(ARFACEANDSTRINGLIST::value_type(getChatData()->m_nVal,m_faceandstring_list));
	
}
