// WorldbaseConfig.h: interface for the CWorldbaseConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDBASECONFIG_H__C1EAE866_546C_451A_882C_97012D6B6B5C__INCLUDED_)
#define AFX_WORLDBASECONFIG_H__C1EAE866_546C_451A_882C_97012D6B6B5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorldbaseConfig  
{
public:
	CWorldbaseConfig();
	virtual ~CWorldbaseConfig();

public:
	UG_LONG			init(CUGIni* pIni,UG_PCHAR pchApp);
	UG_LONG			cleanup();
	
public:
	UG_LONG			cout(FILE* p);

protected:	
	UG_LONG			initFilterTrade(UG_PCHAR pchFilter);
	UGLONG			getGMLog(UGPCHAR pchGMIDS);
	
public:
	UG_DWORD		m_dwWorldbaseIP; //����Ϊ��ip��worldbase��������
	UG_DWORD		m_dwWorldbaseCheckID; //��ip��worldbase��id��Ŀǰû��ʹ��
	UG_DWORD		m_dwIPForPlayer; //��������ӵ������������ip
	UG_DWORD		m_dwPortForPlayer; //��������ӵ������������port
	UG_ULONG		m_ulMaxPlayer; //��������
	UG_DWORD		m_dwIPForGM; //��GM���ӵ������������ip
	UG_DWORD		m_dwPortForGM; //��GM���ӵ������������port
	
public:
	UG_CHAR			m_szMailDBHost[261]; //�ʼ����ݿ�ĵ�ַ
	UG_CHAR			m_szMailDBName[261]; //�ʼ����ݿ������
	UG_CHAR			m_szMailDBUser[51]; //�ʼ����ݿ���û���
	UG_CHAR			m_szMailDBPassword[51]; //�ʼ����ݿ���û�����

public:
	UGINT			m_nFilterTrade;
	UGCHAR**		m_ppchFilterTrade;

public:
	UG_ULONG		m_ulSceneNum; //�������ݣ���worldbase������

//add by ben 2005-09-21	
public:
	UGINT			m_nGMLogCount;
	UGPINT			m_pGMLog;
	
};

#endif // !defined(AFX_WORLDBASECONFIG_H__C1EAE866_546C_451A_882C_97012D6B6B5C__INCLUDED_)
