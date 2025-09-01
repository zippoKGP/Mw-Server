#ifndef _GAMEOBJECTLIST_H_
#define _GAMEOBJECTLIST_H_

class CGameObject;

struct _object_list_point
{
	CGameObject*		m_pItem;
	_object_list_point* m_pNext;
	_object_list_point* m_pPrev;

	_object_list_point()
	{
		m_pItem = NULL;
		InitLink();
	}

	void SetPoint(CGameObject* pItem)
	{
		m_pItem = pItem;
		InitLink();
	}
	
	void InitLink()
	{
		m_pNext = NULL;
		m_pPrev = NULL;
	}
};

class CObjectList
{
public:
	_object_list_point m_Head;
	_object_list_point m_Tail;
	int m_nSize;

public:
	CObjectList()
	{
		InitList();
	}

	virtual ~CObjectList()
	{
	}
	
	bool PushItem(_object_list_point* pItem)
	{
		if(pItem->m_pNext != NULL || pItem->m_pPrev != NULL)
		{
			::MessageBox(NULL, "Èþ..¤Ð¤Ð", "PushItem Err", MB_OK);
			return false;
		}

		pItem->m_pNext = &m_Tail;
		pItem->m_pPrev = m_Tail.m_pPrev;
		m_Tail.m_pPrev->m_pNext = pItem;
		m_Tail.m_pPrev = pItem;

		m_nSize++;
		
		return true;
	}

	bool DeleteItem(_object_list_point* pItem)
	{
		_object_list_point* pPoint = m_Head.m_pNext;
		while(pPoint != &m_Tail)
		{
			if(pPoint == pItem)
			{
				pPoint->m_pPrev->m_pNext = pPoint->m_pNext;
				pPoint->m_pNext->m_pPrev = pPoint->m_pPrev;
				pPoint->m_pNext = NULL;
				pPoint->m_pPrev = NULL;

				m_nSize--;
				return true;
			}
			else
				pPoint = pPoint->m_pNext;
		}

		::MessageBox(NULL, "Èþ..¤Ð¤Ð", "DeleteItem Err", MB_OK);

		return false;
	}

	void InitList()
	{
		m_nSize = 0;
			  	
		m_Head.m_pNext = &m_Tail;
		m_Head.m_pPrev = &m_Head;	
		m_Tail.m_pNext = &m_Tail;
		m_Tail.m_pPrev = &m_Head;
	}

	int GetSize()
	{
		return m_nSize;
	}
};

#endif