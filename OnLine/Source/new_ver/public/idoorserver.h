#if !defined (__TODOOR_H__)
#define __TODOOR_H__


#define		DOORSERVER_INTERFACE_VERSION			"IDoorServer001"

#define     DOORFILE                                "Door.DAT"

enum { FTP_OK            = 0,     //�汾��ȷ������Ҫ���أ������ɹ�
FTP_UPDATE_OK            = 1,     //�汾����ȷ����Ҫ���أ������ɹ����ر�MAIN.EXE����
FTP_UPDATE_ERR           = 2,     //�汾����ȷ����Ҫ���أ�����ʧ�ܣ��ر�MAIN.EXE����
DX_ERROR                 = 3,     //���DX����ȷ���޷�������Ϸ
};



typedef struct DOORINFO_HEAD_STRUCT
{
	long  offset_server;	        //ͼƬƫ��
	long  offset_callborad;		    //��ͼ����ƫ��
	long  total_server;
	long  total_callborad;
} DOORINFO_HEAD;


typedef struct SERVER_INFO_STRUCT {
	char            szServerName[64];       //����������
	unsigned int    nServerID;              //������ΨһID
	unsigned int    nIP;                    //������IP��ַ
	unsigned int    nPort;                  //�������˿ں�
	unsigned int    nLineNum;               //��������ǰ��������
	unsigned char   bLine;                  //��������ǰ�Ƿ���  0Ϊ�رա�����Ϊ��
}SERVERINFO;


typedef struct CALLBORAD_INFO_STRUCT {
	char    szDate[40];             //����
	char    szContent[10240];        //��������
}CALLBORADINFO;



typedef struct FTPSERVER_INFO_STRUCT {
	char    szIP[30];               //FTP������IP��ַ
	char    szPath[MAX_PATH];       //�ļ�·��
}FTPSERVERINFO;


class IDoorServer 
{
public:
	virtual int init(void) = 0;                     //��ʼ��ģ��     �緵��E_FAIL������ʧ��
	                  
	virtual int shutdown(void) = 0;                 //�ر�ģ��


	virtual int getGameServer(void) = 0;            //������Ϸ���������������緵��E_FAIL������ʧ��

	virtual int getGameServerData(void* pData) = 0; //������Ϸ�����������ݣ��ṹΪSERVERINFO, pDataָ�����ǿ�
	                                                //pData����Ϊ sizeof(SERVERINFO)*getGameServer() 
	                                                //����E_FAIL������ʧ��

	virtual int getFtpServer(void) = 0;            //����FTP���������������緵��E_FAIL������ʧ��
	
	virtual int getFtpServerData(void* pData) = 0; //����FTP�����������ݣ��ṹΪFTPSERVERINFO, pDataָ�����ǿ�
												   //pData����Ϊ sizeof(FTPSERVERINFO)*getFtpServer() 
	                                               //����E_FAIL������ʧ��

	virtual int getCallBorad(void) = 0;            //���ص��չ��������������緵��E_FAIL������ʧ��
	
	virtual int getCallBoradData(void* pData) = 0; //���ص��չ����������ݣ��ṹΪCALLBORADINFO, pDataָ�����ǿ�
												   //pData����Ϊ sizeof(CALLBORADINFO)*getCallBorad() 
	                                               //����E_FAIL������ʧ��

	virtual int exeFtpUpdate(void) = 0;            //ִ��FTP���߸��²���

};


#endif