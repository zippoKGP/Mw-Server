#ifndef _MonsterLootingMgr_h_
#define _MonsterLootingMgr_h_

#include "Player.h"

/////////////////////////	
//������ ���ñ����� ����
///////////////////////// 

class CLootingMgr
{
public:

	struct _list
	{
		CPlayer*	pAtter;
		DWORD		dwAtterSerial;
		BYTE		byAttCount;
		WORD		wDamage;
		DWORD		dwLastAttTime;

		_list()
		{	
			Init();		
		};

		void Init()	
		{	
			memset(this, 0, sizeof(*this));
		}

		int GetScore(bool bBonus = false)
		{
			DWORD dwScore = byAttCount * wDamage;
			if(bBonus)
				return dwScore * 1.2f;

			return dwScore;
		}
	};

	enum	{	list_num	=	16		};

	bool	m_bFirst;
	_list	m_AtterList[list_num];

public:

	CLootingMgr()	{}

	void Init()
	{
		for(int i = 0; i < list_num; i++)
			m_AtterList[i].Init();

		m_bFirst = true;
	}

	void PushDamage(CPlayer* pAtter, WORD wDamage)
	{
		//����Ʈ�� �ִ��� ã��..
		for(int i = 0; i < list_num; i++)
		{
			_list* p = &m_AtterList[i];
			if(p->pAtter != pAtter)
				continue;
			if(p->dwAtterSerial == pAtter->m_dwObjSerial)
			{//ã�Ҵ�..
				p->byAttCount = max(p->byAttCount, p->byAttCount + 1);//�ٽ� 255���� 0���� �Ŵ°� ����..
				p->wDamage = max(p->wDamage, p->wDamage + wDamage);
				p->dwLastAttTime = timeGetTime();
				return;
			}
			else
			{	//�ش��� ����..
				p->Init();
				break;
			}
		}

		//��ã�Ҵٸ� ��� ã��..
		_list* pEmptyNode = NULL;
		for(i = 0; i < list_num; i++)
		{
			_list* p = &m_AtterList[i];
			if(!p->pAtter)
			{
				pEmptyNode = p;
				break;
			}
		}

		//��ã�Ҵٸ� ���� �������� ������ �÷��̾ ����..
		if(!pEmptyNode)
		{	//full�λ��°� �⺻�����̹Ƿ� ��ó�� 0�ε����� ���� ã���ʱⰪ���� ����
			DWORD dwMinTime = m_AtterList[0].dwLastAttTime;
			pEmptyNode = &m_AtterList[0];

			for(i = 1; i < list_num; i++)
			{
				_list* p = &m_AtterList[i];
				if(dwMinTime > p->dwLastAttTime)
				{
					dwMinTime = p->dwLastAttTime;
					pEmptyNode = p;
				}				
			}

			pEmptyNode->Init();

			if(pEmptyNode == &m_AtterList[0])	
				m_bFirst = false;		//ù�涧���ڰ� �ٲ���� 1.2 �ǳ��� ������Դ�
		}

		//ã�� ��ſ� ���� �߰��ϱ�..
		pEmptyNode->pAtter = pAtter;
		pEmptyNode->dwAtterSerial = pAtter->m_dwObjSerial;
		pEmptyNode->byAttCount = max(pEmptyNode->byAttCount, pEmptyNode->byAttCount + 1);
		pEmptyNode->wDamage = max(pEmptyNode->wDamage, pEmptyNode->wDamage + wDamage);
		pEmptyNode->dwLastAttTime = timeGetTime();
	}

	CPlayer* GetLooter()
	{
		DWORD dwMaxScore = 0;
		_list* pLooter = NULL;

		for(int i = 0; i < list_num; i++)
		{
			_list* p = &m_AtterList[i];
			if(!p->pAtter)
				continue;

			DWORD dwScore = m_AtterList[i].GetScore(i == 0 ? m_bFirst : false);
			if(dwMaxScore < dwScore)
			{
				dwMaxScore = dwScore;
				pLooter = p;
			}
		}		
	
		//��ҳ�.. �ø���˻�..
		if(pLooter->pAtter->m_bLive && pLooter->pAtter->m_dwObjSerial == pLooter->dwAtterSerial)
			return pLooter->pAtter;

		return NULL;
	}
};

#endif