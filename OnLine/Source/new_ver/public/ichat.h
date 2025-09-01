#if !defined (__CHAT_H__)
#define __CHAT_H__

#define  MSG_CHAT		0X0001		//��������Ϣ(��������д�ţ�ϵͳ��Ϣ��)


#define MSG_CHAT_PUBLIC				0		//����
#define MSG_CHAT_PRIVATE			1		//˽��
#define MSG_CHAT_GROUP				2		//�����
#define MSG_CHAT_FRIENTS			3		//������
#define MSG_CHAT_SYSINFO			4       //ϵͳ��Ϣ
#define MSG_CHAT_CURRENT            5       //��ǰ��Ϣ


typedef struct chat_net_t{
	DWORD      dwUserA_ID;          //˵����ID
	DWORD      dwUserB_ID;          //˵������ID
	BYTE       nChatType;           //��������
	char       szUserA_Name[64];    //˵��������
	char       szUserB_Name[64];    //˵������ID
	char       szChatDetail[512];   //�������
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