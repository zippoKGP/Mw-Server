/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : chatface.cpp
*owner        : Robert
*description  : 表情动画的播放
*modified     : 2004/05/01
******************************************************************************/ 

#include "ChatDef.h"
#include "ChatFace.h"


#pragma comment(lib,"Msimg32.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//function name :CChatFace
//description	:初始化模块
//return		:
//parameter		:void
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
CChatFace::CChatFace()
{
	pointer=NULL;
	
	time(&timeNow);

	timeCheck=timeNow;

	m_last_time=0;

	m_handle=-1;

	m_mda_index=-1;

	chatface_list.clear();


	memset(&m_bi,0,sizeof(BITMAPINFO));
	m_bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bi.bmiHeader.biWidth = 256;//getChatData()->m_pBmp->w;
	m_bi.bmiHeader.biHeight = -256;//-getChatData()->m_pBmp->h; //上下颠倒图片
	m_bi.bmiHeader.biSizeImage = 0;//getChatData()->m_pBmp->pitch * getChatData()->m_pBmp->h;
	m_bi.bmiHeader.biPlanes = 1;
	m_bi.bmiHeader.biBitCount = 24;
	m_bi.bmiHeader.biCompression = BI_RGB;
	m_bi.bmiHeader.biXPelsPerMeter = 0;//3780;
	m_bi.bmiHeader.biYPelsPerMeter  = 0;//3780;
	m_bi.bmiHeader.biClrUsed = 0;
	m_bi.bmiHeader.biClrImportant = 0;			

	m_pvData = NULL;
	m_hBitMap = (HBITMAP)CreateDIBSection(getChatData()->m_hDC,&m_bi,DIB_RGB_COLORS,&m_pvData,NULL,0);
	hMenDC=CreateCompatibleDC(getChatData()->m_hDC);
}

//////////////////////////////////////////////////////////////////////////
//function name :~CChatFace
//description	:
//return		:
//parameter		:void
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
CChatFace::~CChatFace()
{
	
			DeleteDC(hMenDC);
}

//////////////////////////////////////////////////////////////////////////
//function name :initChatFace
//description	:初始化模块
//return		:void
//parameter		:void
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
int CChatFace::initChatFace(char* szChatFaceFile)
{
	UCHR command_text[32]={0};
	char command_chat[4]={0};
	char tempchat[4]={0};

	mda_group_open((UCHR *)szChatFaceFile,&m_handle);
	m_mda_index=get_mda_index(m_handle);
	
	
	if (m_handle==-1)
	{
		return E_FAIL;
	}

	int ntotal_ani_command=mda_group[m_mda_index].total_ani_command;
	for(int i=0;i<ntotal_ani_command;i++)
	{
		
		sprintf((char *)command_text,"MainCommand%d",i);
		sprintf((char *)tempchat,"%d",i);
		
		mda_group_exec(m_handle,command_text,0,0,screen_channel2,MDA_COMMAND_FIRST,0,0,0);
		
		int nLen=strlen(tempchat);
		switch(nLen) {
		case 1:
			sprintf((char *)command_chat,"00%s",tempchat);
			break;
		case 2:
			sprintf((char *)command_chat,"0%s",tempchat);
			break;
		}
		InsertFace(command_chat,(char*)command_text,i,
			mda_group[m_mda_index].img256_head->xlen,mda_group[m_mda_index].img256_head->ylen);

	}
	
	return S_OK;

}

void CChatFace::InitDraw()
{
	for (pointer=chatface_list.begin();pointer!=chatface_list.end();pointer++)
	{
		pointer->second.pageFlag=0;
	}
}
//////////////////////////////////////////////////////////////////////////
//function name :InsertFace
//description	:插入一个聊天动画
//return		:void
//parameter		:string faceId,unsigned short iWidth,unsigned short iHeight,UINT iPageNum
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
void CChatFace::InsertFace(string faceId,char* szCommand,UINT iPos,UINT16 iWidth,UINT16 iHeight)
{
	
	memset(&facedata,0,sizeof(chatface_data));
	
	memcpy(facedata.szCommand,szCommand,sizeof(facedata.szCommand));
	
	facedata.nWidth=iWidth;

	facedata.nHeight=iHeight;
	
	facedata.pageNum=(int)mda_group[m_mda_index].total_frame[iPos];
	
	facedata.currentNum=0;
	
	facedata.pageFlag=0;
	
	chatface_list.insert(face_map::value_type(faceId,facedata));
}

//////////////////////////////////////////////////////////////////////////
//function name :GetChatWidth
//description	:得到聊天动画的宽度
//return		:void
//parameter		:string faceId
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
unsigned short CChatFace::GetChatWidth(string faceId)
{
	faceId=faceId.substr(0,3);
	pointer=chatface_list.find(faceId);
	if (pointer!=chatface_list.end())
	{
		return pointer->second.nWidth;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
//function name :GetChatHeight
//description	:得到聊天动画的高度
//return		:void
//parameter		:string faceId
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
unsigned short CChatFace::GetChatHeight(string faceId)
{
	faceId=faceId.substr(0,3);
	pointer=chatface_list.find(faceId);
	if (pointer!=chatface_list.end())
	{
		return pointer->second.nHeight;
	}
	return 0;
}

/*
//////////////////////////////////////////////////////////////////////////
//function name :FindChatFace
//description	:查找一个聊天动画
//return		:int
//parameter		:string faceId
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
int CChatFace::FindChatFace(string faceId)
{
	for (pointer=chatface_list.begin();pointer!=chatface_list.end();pointer++)
	{
		if (pointer!=chatface_list.end())
		{
			return 1;
		}
	}
	return 0;
}
*/
//////////////////////////////////////////////////////////////////////////
//function name :ShowChatFace
//description	:显示聊天动画
//return		:void
//parameter		:string faceId,POINT ptFace,int iWidth,int iHeight
//modified		:Robert
//////////////////////////////////////////////////////////////////////////
void CChatFace::ShowChatFace(string faceId,POINT ptFace,BITMAP* pBrbm)
{
	CUGImage* pImage=NULL;
	faceId=faceId.substr(0,3);
	
	pointer=chatface_list.find(faceId);
	if (pointer!=chatface_list.end())
	{
		if ((pointer->second.currentNum)>=(pointer->second.pageNum))
		{
			pointer->second.currentNum=0;
		}
		
		if (getChatData()->m_bOuter)
		{
			RECT rect={ptFace.x,
				ptFace.y,0,0};
			
			mda_group_exec_24(m_handle,(UCHR *)pointer->second.szCommand,
					0,0,getChatData()->m_pBmp,MDA_COMMAND_ASSIGN,0,0,
					pointer->second.currentNum, pBrbm, &rect);
			
			//保存的bmp是正的，显示的bmp时倒的
/*			BITMAPINFO bi;
			memset(&bi,0,sizeof(BITMAPINFO));
			bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi.bmiHeader.biWidth = getChatData()->m_pBmp->w;
			bi.bmiHeader.biHeight = -getChatData()->m_pBmp->h; //上下颠倒图片
			bi.bmiHeader.biSizeImage = 0;//getChatData()->m_pBmp->pitch * getChatData()->m_pBmp->h;
			bi.bmiHeader.biPlanes = 1;
			bi.bmiHeader.biBitCount = 24;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biXPelsPerMeter = 0;//3780;
			bi.bmiHeader.biYPelsPerMeter  = 0;//3780;
			bi.bmiHeader.biClrUsed = 0;
			bi.bmiHeader.biClrImportant = 0;			

			void* pvData = NULL;
			HANDLE tempHandle = NULL;
			HBITMAP hBitMap = (HBITMAP)CreateDIBSection(getChatData()->m_hDC,&bi,DIB_RGB_COLORS,&pvData,tempHandle,0);
			if (pvData!=NULL)
			{
				memcpy(pvData,getChatData()->m_pBmp->line[0],getChatData()->m_pBmp->h * getChatData()->m_pBmp->pitch);
			}
			
*/		
/*		
			memset(getChatData()->m_pBuffer,0,getChatData()->m_pBmp->h * getChatData()->m_pBmp->pitch);
			
			for(int i = 0; i < getChatData()->m_pBmp->h; i ++)
			{
				memcpy((void*)(getChatData()->m_pBuffer+(getChatData()->m_pBmp->h-i-1)*getChatData()->m_pBmp->pitch),
					(void*)(getChatData()->m_pBmp->line[i]),getChatData()->m_pBmp->pitch);

				//memset(getChatData()->m_pBmp->line[i],0,getChatData()->m_pBmp->pitch);
			
			}
*/			
			

			if (m_pvData!=NULL)
			{
				BYTE* tempPixel =NULL;
				BYTE * tempBmpPixel = NULL; 
				tempPixel	= (BYTE *)getChatData()->m_pBmp->line[0];
					//+(getChatData()->m_pBmp->h-1)*getChatData()->m_pBmp->pitch;
				
				tempBmpPixel = (BYTE *)m_pvData;
				memset(tempBmpPixel, 0, 256*256*3);//sizeof(BMP24_PIXEL));
				int i;
				for (i = 0; i<getChatData()->m_pBmp->h; i++)
				{
					for (int j =0; j<getChatData()->m_pBmp->pitch; j++)
					{
						if ((tempPixel[j]>0)||(tempPixel[j+1]>0)||(tempPixel[j+1]>0))
						{
							tempBmpPixel[j] = 0x00;
						}
						else
						{
							tempBmpPixel[j] = 0xff;
						}
					}
					tempBmpPixel = tempBmpPixel+256*3;
					tempPixel = tempPixel+getChatData()->m_pBmp->pitch;
				}				
				
				HGDIOBJ hOld = SelectObject(hMenDC,m_hBitMap);
				
				BitBlt(getChatData()->m_hDC,rect.left,rect.top,rect.right,
					rect.bottom,hMenDC,0,0,SRCAND);				
				
				SelectObject(hMenDC,hOld);
				

				tempPixel	= (BYTE *)getChatData()->m_pBmp->line[0];
					//+(getChatData()->m_pBmp->h-1)*getChatData()->m_pBmp->pitch;
				
				tempBmpPixel = (BYTE *)m_pvData;
				memset(tempBmpPixel, 0, 256*256*3);//sizeof(BMP24_PIXEL));
			//	mylog.UGLog("+++");
				for (i = 0; i<getChatData()->m_pBmp->h; i++)
				{
					if (getChatData()->m_pBmp->pitch >= 256*3)
					{
						memcpy(tempBmpPixel,tempPixel, 256*3);
					}
					else
					{
						memcpy(tempBmpPixel,tempPixel, getChatData()->m_pBmp->pitch);

					}
					
					tempBmpPixel = tempBmpPixel+256*3;
					tempPixel = tempPixel+getChatData()->m_pBmp->pitch;
			//		mylog.UGLog("---");
					memset(getChatData()->m_pBmp->line[i],0,getChatData()->m_pBmp->pitch);
				}
				//vector修改
				//memcpy(m_pvData,getChatData()->m_pBmp->line[0],getChatData()->m_pBmp->h * getChatData()->m_pBmp->pitch);
			}			


			
	
	
		
			HGDIOBJ  hOld = SelectObject(hMenDC,m_hBitMap);
			BOOL b11 = BitBlt(getChatData()->m_hDC,rect.left,rect.top,rect.right,
				rect.bottom,hMenDC,0,0,SRCPAINT);
			SelectObject(hMenDC,hOld);

		//	BOOL b11 = TransparentBlt(getChatData()->m_hDC,rect.left,rect.top,rect.right,
		//		rect.bottom,hMenDC,0,0,rect.right,rect.bottom,RGB(0,0,0));
			
			


//			DeleteObject(hBitMap);
//			CloseHandle(tempHandle);
			
		}
		else
		{
			mda_group_exec(m_handle,(UCHR *)pointer->second.szCommand,
				ptFace.x,ptFace.y,getChatData()->m_screenBuffer,MDA_COMMAND_ASSIGN,0,0,pointer->second.currentNum);
			
		}

		if (((getChatData()->s_nNewtime-pointer->second.nLastTime)>CHATFACEREFRESH) && (!pointer->second.pageFlag))
		//if ((getChatData()->m_bFrame) && (!pointer->second.pageFlag))
		{
			pointer->second.currentNum++;
			pointer->second.nLastTime=getChatData()->s_nNewtime;
		}
		
		if (!pointer->second.pageFlag)
		{
			pointer->second.pageFlag=1;
		}
	}

	pointer=NULL;
	return;
}

void CChatFace::shutDown()
{
	mda_group_close(&m_handle);
}
















