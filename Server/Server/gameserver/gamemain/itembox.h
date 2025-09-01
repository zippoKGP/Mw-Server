// ItemBox.h: interface for the CItemBox class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _ItemBox_h_
#define _ItemBox_h_

#include "GameObject.h"
#include "StorageList.h"

class CPartyPlayer;

//#define obj_id_itembox 0

#define itembox_create_code_mon		0
#define itembox_create_code_throw	1
#define itembox_create_code_cheat	2
#define itembox_create_code_reward	3

struct _itembox_create_setdata : public _object_create_setdata
{
	CGameObject*	pOwner;	//우선권자..
	CGameObject*	pThrower;	//버린사람..
	BYTE			byCreateCode;

	__ITEM Item;
	
	_itembox_create_setdata(){
		pOwner = NULL;
		pThrower = NULL;
		byCreateCode = 0;
	}
};

class CPlayer;

class CItemBox : public CGameObject
{
public:

	enum	{	tm_box_exist = 60000, tm_box_hurry = 50000, tm_box_owner_take = 20000	};

	DWORD		m_dwOwnerSerial;
	WORD		m_wOwnerIndex;	

	DWORD		m_dwThrowerSerial;
	BYTE		m_byThrowerID;
	WORD		m_wThrowerIndex;
	DWORD		m_dwThrowerCharSerial;
	BYTE		m_byThrowerDegree;	//버린사람 등급..

	DWORD		m_dwLootStartTime;
	int			m_nStateCode;
	DWORD		m_dwLastDestroyTime;
	BYTE		m_byCreateCode;

	CPartyPlayer*	m_pOwnerPtr;

	__ITEM		m_Item;
	bool		m_bHide;	//치트로 나온것은 fixpos를 안보냄.. create시에도 주인만 보내공..

	static int	s_nLiveNum;

	static DWORD s_dwSerialCounter;
	static CNetIndexList s_listEmpty;

public:

	CItemBox();
	virtual ~CItemBox(){}

	void Init(_object_id* pID);
	bool Create(_itembox_create_setdata* pParam, bool bHide);
	bool Destroy();

	bool IsTakeRight(CPlayer* pOne);

	virtual void Loop();

	void SendMsg_Create();
	void SendMsg_Destroy();
	void SendMsg_StateChange();
	
	virtual void SendMsg_FixPosition(int n);
};

bool CreateItemBox(BYTE byTblCode, WORD wItemIdx, DWORD dwDur, DWORD dwLv, 
								 CPlayer* pOwner, CGameObject* pThrower, BYTE byCreateCode,
								 CMapData* pMap, WORD wLayerIndex, float* pStdPos, bool bHide);

bool CreateItemBox(__ITEM* pItem, CPlayer* pOwner, CGameObject* pThrower, BYTE byCreateCode,
								CMapData* pMap, WORD wLayerIndex, float* pStdPos, bool bHide);


#endif 
