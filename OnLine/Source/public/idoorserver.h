#if !defined (__TODOOR_H__)
#define __TODOOR_H__


#define		DOORSERVER_INTERFACE_VERSION			"IDoorServer001"

#define     DOORFILE                                "Door.DAT"

enum { FTP_OK            = 0,     //版本正确，不需要下载，操作成功
FTP_UPDATE_OK            = 1,     //版本不正确，需要下载，操作成功，关闭MAIN.EXE程序
FTP_UPDATE_ERR           = 2,     //版本不正确，需要下载，操作失败，关闭MAIN.EXE程序
DX_ERROR                 = 3,     //检测DX不正确，无法启动游戏
};



typedef struct DOORINFO_HEAD_STRUCT
{
	long  offset_server;	        //图片偏移
	long  offset_callborad;		    //地图属性偏移
	long  total_server;
	long  total_callborad;
} DOORINFO_HEAD;


typedef struct SERVER_INFO_STRUCT {
	char            szServerName[64];       //服务器名字
	unsigned int    nServerID;              //服务器唯一ID
	unsigned int    nIP;                    //服务器IP地址
	unsigned int    nPort;                  //服务器端口号
	unsigned int    nLineNum;               //服务器当前在线人数
	unsigned char   bLine;                  //服务器当前是否开着  0为关闭、其他为开
}SERVERINFO;


typedef struct CALLBORAD_INFO_STRUCT {
	char    szDate[40];             //日期
	char    szContent[10240];        //留言内容
}CALLBORADINFO;



typedef struct FTPSERVER_INFO_STRUCT {
	char    szIP[30];               //FTP服务器IP地址
	char    szPath[MAX_PATH];       //文件路径
}FTPSERVERINFO;


class IDoorServer 
{
public:
	virtual int init(void) = 0;                     //初始化模块     如返回E_FAIL，函数失败
	                  
	virtual int shutdown(void) = 0;                 //关闭模块


	virtual int getGameServer(void) = 0;            //返回游戏服务器的数量，如返回E_FAIL，函数失败

	virtual int getGameServerData(void* pData) = 0; //返回游戏服务器的数据，结构为SERVERINFO, pData指针必须非空
	                                                //pData长度为 sizeof(SERVERINFO)*getGameServer() 
	                                                //返回E_FAIL，函数失败

	virtual int getFtpServer(void) = 0;            //返回FTP服务器的数量，如返回E_FAIL，函数失败
	
	virtual int getFtpServerData(void* pData) = 0; //返回FTP服务器的数据，结构为FTPSERVERINFO, pData指针必须非空
												   //pData长度为 sizeof(FTPSERVERINFO)*getFtpServer() 
	                                               //返回E_FAIL，函数失败

	virtual int getCallBorad(void) = 0;            //返回当日公告栏的数量，如返回E_FAIL，函数失败
	
	virtual int getCallBoradData(void* pData) = 0; //返回当日公告栏的数据，结构为CALLBORADINFO, pData指针必须非空
												   //pData长度为 sizeof(CALLBORADINFO)*getCallBorad() 
	                                               //返回E_FAIL，函数失败

	virtual int exeFtpUpdate(void) = 0;            //执行FTP在线更新操作

};


#endif