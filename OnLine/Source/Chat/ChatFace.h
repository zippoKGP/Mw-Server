/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : chatface.h
*owner        : Robert
*description  : 表情动画的播放
*modified     : 2004/05/01
******************************************************************************/ 

#if !defined(AFX_CHATFACE_H__FE7E2BA0_0DEB_4BE3_B1A5_C21CA5EF511A__INCLUDED_)
#define AFX_CHATFACE_H__FE7E2BA0_0DEB_4BE3_B1A5_C21CA5EF511A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct chatface_data
{
	int           pageNum;
	int           currentNum;
	char          szCommand[32]; 
	BYTE          pageFlag;
	UINT16        nWidth;
	UINT16        nHeight;
	ULONG         nLastTime;
};

typedef map<string,chatface_data,less<string> > face_map;


class CChatFace  
{

private:

	HDC  hMenDC;

	BITMAPINFO			 m_bi;

	void*				 m_pvData;

	HBITMAP				 m_hBitMap;
	
	face_map             chatface_list;
	
	face_map::iterator   pointer;
	
	chatface_data        facedata;
	
	ULONG                m_last_time;
	
	long                 timeCheck,timeNow;

	SLONG                m_handle;

	SLONG                m_mda_index;


//	int       FindChatFace(string faceId);

	void      InsertFace(string faceId,char* szCommand,UINT iPos,UINT16 iWidth,UINT16 iHeight);

public:
	void shutDown();
	
	CChatFace();
	
	virtual   ~CChatFace();

	void      ShowChatFace(string faceId,POINT ptFace,BITMAP* pBrbm= NULL);
	
	int       initChatFace(char* szChatFaceFile);

	unsigned short    GetChatHeight(string faceId);

	unsigned short    GetChatWidth(string faceId);

	void      InitDraw();

	

};

#endif // !defined(AFX_CHATFACE_H__FE7E2BA0_0DEB_4BE3_B1A5_C21CA5EF511A__INCLUDED_)
