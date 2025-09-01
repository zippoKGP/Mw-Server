#ifndef _MonsterLootingMgr_h_
#define _MonsterLootingMgr_h_

#include "Player.h"

/////////////////////////	
//아이템 루팅권한을 관리
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
		//리스트에 있는지 찾기..
		for(int i = 0; i < list_num; i++)
		{
			_list* p = &m_AtterList[i];
			if(p->pAtter != pAtter)
				continue;
			if(p->dwAtterSerial == pAtter->m_dwObjSerial)
			{//찾았당..
				p->byAttCount = max(p->byAttCount, p->byAttCount + 1);//다시 255에서 0으로 돼는거 방지..
				p->wDamage = max(p->wDamage, p->wDamage + wDamage);
				p->dwLastAttTime = timeGetTime();
				return;
			}
			else
			{	//해당노드 삭제..
				p->Init();
				break;
			}
		}

		//못찾았다면 빈거 찾기..
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

		//못찾았다면 가장 공격한지 오래됀 플레이어를 뺀다..
		if(!pEmptyNode)
		{	//full인상태가 기본전제이므로 맨처음 0인덱스의 값을 찾기초기값으로 세팅
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
				m_bFirst = false;		//첫방때린자가 바뀐경우는 1.2 뽀나스 안줘야함다
		}

		//찾은 빈거에 새로 추가하기..
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
	
		//살았나.. 시리얼검사..
		if(pLooter->pAtter->m_bLive && pLooter->pAtter->m_dwObjSerial == pLooter->dwAtterSerial)
			return pLooter->pAtter;

		return NULL;
	}
};

#endif