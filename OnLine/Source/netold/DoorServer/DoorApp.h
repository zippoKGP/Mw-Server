// DoorApp.h: interface for the CDoorApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOORAPP_H__E7D0E5BF_FBE9_43AB_B242_14D52B97E690__INCLUDED_)
#define AFX_DOORAPP_H__E7D0E5BF_FBE9_43AB_B242_14D52B97E690__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDoorApp  
{
public:
	CDoorApp();
	virtual ~CDoorApp();

public:
	UG_LONG			init(UG_PCHAR pchPathName); //��ʼ������
	UG_LONG			render(UG_ULONG ulTimer); //ѭ��֡
	UG_LONG			cleanup(); //��գ����ó�ʼ���ͱ��������պ���
	
protected:
	UG_LONG			mallocMemory();
	UG_LONG			freeMemory();

private:
	CDoorConfig		m_config;
	
public:
	CSysModule*				m_pSysTCPComm;
	CWorldbaseManager*		m_pWorldbaseManager;
	CGMManager*				m_pGMManager;

public:
	UG_LONG			coutGM();
	UG_LONG			coutWB();
	UG_LONG			coutConfig();
	UG_LONG			coutAll();
	
};

#endif // !defined(AFX_DOORAPP_H__E7D0E5BF_FBE9_43AB_B242_14D52B97E690__INCLUDED_)
