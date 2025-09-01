#if !defined (__CHAT_H__)
#define __CHAT_H__

#define  MSG_CHAT		0X0001		//聊天类消息(包括聊天写信，系统消息等)


#define MSG_CHAT_PUBLIC				0		//公聊
#define MSG_CHAT_PRIVATE			1		//私聊
#define MSG_CHAT_GROUP				2		//组队聊
#define MSG_CHAT_FRIENTS			3		//好友聊
#define MSG_CHAT_SYSINFO			4       //系统消息
#define MSG_CHAT_CURRENT            5       //当前消息


typedef struct chat_net_t{
	DWORD      dwUserA_ID;          //说话人ID
	DWORD      dwUserB_ID;          //说话对象ID
	BYTE       nChatType;           //聊天类型
	char       szUserA_Name[64];    //说话人名字
	char       szUserB_Name[64];    //说话对象ID
	char       szChatDetail[512];   //聊天语句
}CHATNET,*PCHATNET;


class CChat 
{
public:
	CChat(){};
	
	virtual ~CChat(){};
	
	virtual int init(char* szChatFaceFile);
	
	virtual int shutdown(void);
	
	virtual int updateframe(POINT ptMouse,float current_time,BMP *screen_buf,bool bFrame);
	
	virtual int insertString(const char *str,RECT& rectFormat,bool bShow = true,HDC hdc = NULL,bool bOuter = false);	
	
	virtual int deleteString(int nStrID);

	virtual void clearString();

	virtual int updateString(ULONG x,ULONG y,ULONG w,ULONG h,const char *str);
	
	virtual int updateOuterString(ULONG x,ULONG y,ULONG w,ULONG h,const char *str,HDC hdc);	
	
	virtual int updateframe(HDC hdc, BITMAP* pbrbm = NULL);

};

#endif