#ifndef	_GuardTower_h_
#define _GuardTower_h_

#include "Character.h"
#include "GuardTowerItemFld.h"
#include "Player.h"

struct _tower_create_setdata : public _character_create_setdata
{
	int			nHP;
	CPlayer*	pMaster;
	__ITEM*		pItem;

	_tower_create_setdata()
	{
		pMaster = NULL;
	}
};  

class CGuardTower : public CCharacter
{
public:

	int			m_nHP;		
	CPlayer*	m_pMaster;
	__ITEM*		m_pItem;
	WORD		m_wItemSerial;

	DWORD		m_dwStartMakeTime;
	bool		m_bComplete;

	CCharacter*	m_pTarget;
	
	DWORD		m_dwLastDestroyTime;

	static int		s_nLiveNum;
	static DWORD    s_dwSerialCnt;
	static CNetIndexList s_listEmpty;

public:

	CGuardTower();
	virtual ~CGuardTower();

	bool Init(_object_id* pID);
	bool Create(_tower_create_setdata* pData);
	bool Destroy(BYTE byDesType);

	bool IsValidTarget();
	void Attack(CCharacter* pTarget);

	virtual void Loop();

	virtual int GetHP()				{	return m_nHP;	}
	virtual int	GetMaxHP( void )	{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nMaxHP;	}
	virtual int SetDamage(int nDam, CCharacter* pDst, int nDstLv, bool bCrt);
	virtual float GetWidth()		{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nWidth;	}
	virtual float GetAttackRange()	{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nGADst;	}
	virtual	int GetDefFC(int nAttactPart, CCharacter* pAttChar)		{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nDefFc;	}
	virtual int GetLevel()			{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nLevel;	}
	virtual int GetDefSkill()		{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nDefSklUnit;	}

	virtual int GetFireTol()		{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nFireTol;	}
	virtual int	GetWaterTol()		{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nWaterTol;	}
	virtual int	GetSoilTol()		{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nSoilTol;	}
	virtual int	GetWindTol()		{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_nWindTol;	}	

	virtual float GetDefGap()		{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_fDefGap;	}	
	virtual float GetDefFacing()	{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_fDefFacing;	}	
	virtual float GetWeaponAdjust()	{	return ((_GuardTowerItem_fld*)m_pRecordSet)->m_fDefGap;	}	

	virtual int	AttackableHeight()	{	return attack_able_height;	}
	virtual int GetGenAttackProb(CCharacter* pDst, int nPart);
	virtual bool IsBeAttackedAble()	{	return true;	}

	virtual	int GetObjRace();//		{	return m_pMaster->GetObjRace();		}

	static DWORD GetNewSerial()		{	return s_dwSerialCnt++;	}
	static bool SetStaticMember();

	void SendMsg_Create();
	void SendMsg_Destroy(BYTE byDesType);
	void SendMsg_Attack(CAttack* pAt);
	virtual void SendMsg_FixPosition(int n);	

	CCharacter* SearchNearEnemy();

	static DWORD GetEmpty()	{	return s_listEmpty.size();	}
};

CGuardTower* CreateGuardTower(CMapData* pMap, WORD wLayer, float* fPos, __ITEM* pItem, CPlayer* pMaster);


#endif
