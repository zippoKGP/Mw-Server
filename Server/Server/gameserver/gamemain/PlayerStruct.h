#ifndef _PLAYER_STRUCT_H_
#define _PLAYER_STRUCT_H_

#include "CharacterDB.h"
#include "WeaponItemFld.h"
#include "UnitKeyItemFld.h"
#include "StorageList.h"
#include "ForceFld.h"

//weapon unit class
#define weapon_equip_punch			0
#define weapon_equip_weapon			1
#define weapon_equip_unit			2

//공격종류
#define attack_type_general		0
#define attack_type_skill		1
#define attack_type_force		2

//장착 마스터리 코드
#define equip_mastery_melee		0
#define equip_mastery_range		1
#define equip_mastery_special	2
#define equip_mastery_force		3
#define equip_mastery_shield	4
#define equip_mastery_defence	5
#define equip_mastery_nothing	-1
#define EQUIP_MASTERY_NUM		6

class CPlayer; class CGuardTower;

bool DTradeEqualPerson(CPlayer* IN lp_pOne, CPlayer** OUT lpp_pDst);

/////////////////////////////////
//struct _WEAPON_PARAM
/////////////////////////////////

struct _WEAPON_PARAM
{
	__ITEM*					pFixWp;		//장착무기	
	_UNIT_DB_BASE::_LIST*	pFixUnit;

//	BYTE		byWpUnitClass;	//weapon_equip_punch, weapon_equip_weapon, weapon_equip_unit
	int			nGaMaxAF;
	int			nGaMinAF;
	BYTE		byGaMinSel;
	BYTE		byGaMaxSel;
	int			nMaMaxAF;
	int			nMaMinAF;
	BYTE		byMaMinSel;
	BYTE		byMaMaxSel;
	BYTE		byAttTolType;
	BYTE		byWpClass;	
	BYTE		byWpType;
	WORD		wGaAttRange;
	WORD		wMaAttRange;

	static CRecordData*	s_pWeaponData;

	void Init()
	{
//		byWpUnitClass = weapon_equip_punch;
		nGaMaxAF = 2;
		nGaMinAF = 1;
		nMaMaxAF = 2;
		nMaMinAF = 1;
		byGaMinSel = 35;
		byGaMaxSel = 60;
		byMaMinSel = 35;
		byMaMaxSel = 60;
		byAttTolType = TOL_CODE_NOTHING;	
		byWpClass = wp_class_close;
		byWpType = type_knife;
		pFixWp = NULL;
		pFixUnit = NULL;
		wGaAttRange = 40;
	}

	static SetStaticMember(CRecordData*	pWeaponData)
	{
		s_pWeaponData = pWeaponData;
	}

	void FixWeapon(__ITEM* pWeapon)
	{
		if(!pWeapon)
		{
			Init();
			return;
		}

		if(pWeapon->m_byTableCode != tbl_code_weapon)
			return;

		_WeaponItem_fld* pWPFld = (_WeaponItem_fld*)s_pWeaponData->GetRecord(pWeapon->m_wItemIndex);

		if(pWPFld->m_nType == type_mine)//채굴기면 장착 안할걸루..
		{
			Init();
			return;
		}

		byAttTolType =	GetWeaponTolType(pWeapon);

		byWpClass = ::GetWeaponClass(pWeapon->m_wItemIndex);
		nGaMaxAF = pWPFld->m_fGAMaxAF;
		nGaMinAF = pWPFld->m_fGAMinAF;
		nMaMaxAF = pWPFld->m_fMAMaxAF;
		nMaMinAF = pWPFld->m_fMAMinAF;

		byGaMinSel = pWPFld->m_nGAMinSelProb;
		byGaMaxSel = pWPFld->m_nGAMaxSelProb;
		byMaMinSel = pWPFld->m_nMAMinSelProb;
		byMaMaxSel = pWPFld->m_nMAMaxSelProb;

		byWpType = pWPFld->m_nType;
		wGaAttRange = pWPFld->m_fGADst + 40;

		pFixWp = pWeapon;	
		pFixUnit = NULL;
	}

	void FixUnit(_UNIT_DB_BASE::_LIST* pUnit)
	{
		Init();

		//유닛에대한건 공격할때 세팅..
		byWpClass	=	wp_class_close;
		byWpType	=	type_knife;

		pFixWp = NULL;	
		pFixUnit = pUnit;
	}

	int GetAttackToolType()
	{
		if(pFixWp)
			return weapon_equip_weapon;
		if(pFixUnit)
			return weapon_equip_unit;

		return weapon_equip_punch;
	}

	DWORD GetAttackDelay(int nLv, int nAddDelay)
	{
		return nLv/nLv*1000 + nAddDelay;
	}

	int GetWeaponTolType(__ITEM* pItem)
	{
		for(int i = 0; i < MAX_ITEM_LV; i++)
		{
			BYTE t = ::GetTalikFromSocket(pItem->m_dwLv, i);
			if(t == __NO_TALIK)
				break;

			if(t == talik_type_fire)
				return TOL_CODE_FIRE;
			if(t == talik_type_water)
				return TOL_CODE_WATER;
			if(t == talik_type_soil)
				return TOL_CODE_SOIL;
			if(t == talik_type_wind)
				return TOL_CODE_WIND;
		}

		_WeaponItem_fld* pWPFld = (_WeaponItem_fld*)s_pWeaponData->GetRecord(pItem->m_wItemIndex);
		return pWPFld->m_nProperty;
	}
};

/////////////////////////////////
//struct _DTRADE_ITEM
/////////////////////////////////

struct _DTRADE_ITEM
{
	bool	bLoad;
	BYTE	byStorageCode;
	DWORD	dwSerial;
	BYTE	byAmount;	//비중첩은 0xFF

	void SetData(BYTE p_byStorageCode, DWORD p_dwSerial, BYTE p_byAmount)
	{
		bLoad = true;
		byStorageCode = p_byStorageCode;
		dwSerial = p_dwSerial;
		byAmount= p_byAmount;
	}

	void ReleaseData()
	{
		bLoad = false;
	}
};


/////////////////////////////////
//struct _DTRADE_PARAM
/////////////////////////////////

struct _DTRADE_PARAM
{
	bool				bDTradeMode;
	bool				bDTradeLock;
	bool				bDTradeOK;
	WORD				wDTradeDstIndex;
	DWORD				dwDTradeDstSerial;	//world serial.. m_dwObjSerial..
	_DTRADE_ITEM		DItemNode[max_d_trade_item];
	DWORD				dwDTrade_Dalant;
	DWORD				dwDTrade_Gold;
	BYTE				byEmptyInvenNum;
	BYTE				bySellItemNum;
	BYTE				byAddUnitNum;

	_DTRADE_PARAM()
	{
		Init();
	}

	void Init()
	{
		memset(this, 0, sizeof(*this));
		wDTradeDstIndex = 0xFFFF;
		dwDTradeDstSerial = 0xFFFFFFFF;
	}

	void SetDTradeStart(WORD pl_dwDstIndex, DWORD pl_dwDstSerial, int pl_mEmptyInvenNum)
	{
		bDTradeMode = true;
		bDTradeLock = false;	
		bDTradeOK = false;
		wDTradeDstIndex = pl_dwDstIndex;
		dwDTradeDstSerial = pl_dwDstSerial;
		dwDTrade_Dalant = 0;
		dwDTrade_Gold = 0;
		byEmptyInvenNum = pl_mEmptyInvenNum;
		bySellItemNum = 0;
		for(int i= 0; i < max_d_trade_item; i++)
			DItemNode[i].ReleaseData();
	}
};

/////////////////////////////////
//struct _MASTERY_PARAM
/////////////////////////////////

struct _MASTERY_PARAM
{
	BYTE	m_byRaceCode;
	_STAT_DB_BASE	m_BaseCum;

	DWORD	m_dwSkillMasteryCum[skill_mastery_num];
	DWORD	m_dwForceLvCum[mastery_level_num];

	BYTE	m_mtyWp[WP_CLASS_NUM];	
	BYTE	m_mtySuffer;				
	BYTE	m_mtyShield;	
	BYTE	m_lvSkill[max_skill_num];
	BYTE	m_mtySkill[skill_mastery_num];
	BYTE	m_mtyForce[force_mastery_num];
	BYTE	m_mtyStaff;
	BYTE	m_mtyMakeItem[MI_CLASS_NUM];
	BYTE	m_mtySpecial;

	DWORD*	m_ppdwMasteryCumPtr[MASTERY_CODE_NUM];		//마스터리별의 누적수 포인터배열
	BYTE*	m_ppbyMasteryPtr[MASTERY_CODE_NUM];			//마스터리별의 숙련도 포인터배열
	BYTE*	m_ppbyEquipMasteryPrt[EQUIP_MASTERY_NUM];	//무기, 방어구 장착을 위한 마스터리 포인터

	static CRecordData* s_pSkillData;
	static CRecordData* s_pForceData;
	static int			s_nSkillLvPerMastery[skill_mastery_num];
	static int			s_nForceLvPerMastery[force_mastery_num];
	
	bool Init(_STAT_DB_BASE* pStatBase, BYTE byRaceCode)
	{
		m_byRaceCode = byRaceCode;
		memcpy(&m_BaseCum, pStatBase, sizeof(_STAT_DB_BASE));		
		memset(m_dwForceLvCum, 0, sizeof(m_dwForceLvCum));

		//무기숙련도
		for(int i = 0; i < WP_CLASS_NUM; i++)
		{
			m_mtyWp[i] = CalcMastery(mastery_code_weapon, i, m_BaseCum.m_dwDamWpCnt[i]);
		}

		//방어숙련도
		m_mtySuffer = CalcMastery(mastery_code_defence, 0, m_BaseCum.m_dwDefenceCnt);

		//방패숙련도
		m_mtyShield = CalcMastery(mastery_code_shield, 0, m_BaseCum.m_dwShieldCnt);

		//스킬레벨 및 마스터리별 누적수
		memset(m_dwSkillMasteryCum, 0, sizeof(m_dwSkillMasteryCum));
		for(i = 0; i < max_skill_num; i++)
		{
			_skill_fld* pFld = (_skill_fld*)s_pSkillData->GetRecord(i);
			if(!pFld)				
				return false;
			if(pFld->m_nLv > 3)				
				return false;

			if(pFld->m_nLv >= 0)
				m_lvSkill[i] = GetSFLevel(pFld->m_nLv, m_BaseCum.m_dwSkillCum[i]);

			if((DWORD)pFld->m_nMastIndex < skill_mastery_num)
				m_dwSkillMasteryCum[pFld->m_nMastIndex] += m_BaseCum.m_dwSkillCum[i];
		}

		//스킬 마스터리별 숙련도
		for(i = 0; i < skill_mastery_num; i++)
		{
			m_mtySkill[i] = CalcMastery(mastery_code_skill, i, m_dwSkillMasteryCum[i]);
		}

		//포스 마스터리별 숙련도
		for(i = 0; i < force_mastery_num; i++)
		{
			m_mtyForce[i] = CalcMastery(mastery_code_force, i, m_BaseCum.m_dwForceCum[i]);
			m_dwForceLvCum[i%mastery_level_num] += m_BaseCum.m_dwForceCum[i];
		}

		//스테프 마스터리
		m_mtyStaff = GetStaffMastery(m_dwForceLvCum);

		//아이템제작 숙련도
		m_mtyMakeItem[mi_class_shield] = CalcMastery(mastery_code_make, mi_class_shield, m_BaseCum.m_dwMakeCum[mi_class_shield]);
		m_mtyMakeItem[mi_class_armor] = CalcMastery(mastery_code_make, mi_class_armor, m_BaseCum.m_dwMakeCum[mi_class_armor]);
		m_mtyMakeItem[mi_class_bullet] = CalcMastery(mastery_code_make, mi_class_bullet, m_BaseCum.m_dwMakeCum[mi_class_bullet]);

		//종족별특별능력 숙련도
		m_mtySpecial = CalcMastery(mastery_code_special, 0, m_BaseCum.m_dwSpecialCum);

		m_ppdwMasteryCumPtr[mastery_code_weapon] = m_BaseCum.m_dwDamWpCnt;
		m_ppbyMasteryPtr[mastery_code_weapon] = m_mtyWp;
		m_ppdwMasteryCumPtr[mastery_code_defence] = &m_BaseCum.m_dwDefenceCnt;
		m_ppbyMasteryPtr[mastery_code_defence] = &m_mtySuffer;
		m_ppdwMasteryCumPtr[mastery_code_shield] = &m_BaseCum.m_dwShieldCnt;
		m_ppbyMasteryPtr[mastery_code_shield] = &m_mtyShield;
		m_ppdwMasteryCumPtr[mastery_code_skill] = m_dwSkillMasteryCum;
		m_ppbyMasteryPtr[mastery_code_skill] = m_mtySkill;
		m_ppdwMasteryCumPtr[mastery_code_force] = m_BaseCum.m_dwForceCum;
		m_ppbyMasteryPtr[mastery_code_force] = m_mtyForce;
		m_ppdwMasteryCumPtr[mastery_code_make] = m_BaseCum.m_dwMakeCum;
		m_ppbyMasteryPtr[mastery_code_make] = m_mtyMakeItem;
		m_ppdwMasteryCumPtr[mastery_code_special] = &m_BaseCum.m_dwSpecialCum;
		m_ppbyMasteryPtr[mastery_code_special] = &m_mtySpecial;

		//장착마스터리 세팅
		m_ppbyEquipMasteryPrt[equip_mastery_melee] = &m_mtyWp[wp_class_close];
		m_ppbyEquipMasteryPrt[equip_mastery_range] = &m_mtyWp[wp_class_long];
		m_ppbyEquipMasteryPrt[equip_mastery_special] = &m_mtySpecial;
		m_ppbyEquipMasteryPrt[equip_mastery_force] = &m_mtyStaff;
		m_ppbyEquipMasteryPrt[equip_mastery_shield] = &m_mtyShield;
		m_ppbyEquipMasteryPrt[equip_mastery_defence] = &m_mtySuffer;

		return true;
	}

	static void SetStaticMember(CRecordData* pSkillData, CRecordData* pForceData)
	{
		s_pSkillData = pSkillData;
		s_pForceData = pForceData;

		memset(s_nSkillLvPerMastery, -1, sizeof(s_nSkillLvPerMastery));
		memset(s_nForceLvPerMastery, -1, sizeof(s_nForceLvPerMastery));

		for(int i = 0; i < s_pSkillData->GetRecordNum(); i++)
		{
			_skill_fld* pFld = (_skill_fld*)s_pSkillData->GetRecord(i);

			if(pFld->m_nMastIndex >= 0 && pFld->m_nMastIndex < skill_mastery_num)
			{
				s_nSkillLvPerMastery[pFld->m_nMastIndex] = pFld->m_nLv;
			}
		}

		for(i = 0; i < s_pForceData->GetRecordNum(); i++)
		{
			_force_fld* pFld = (_force_fld*)s_pForceData->GetRecord(i);

			if(pFld->m_nMastIndex >= 0 && pFld->m_nMastIndex < force_mastery_num)
			{
				s_nForceLvPerMastery[pFld->m_nMastIndex] = pFld->m_nLv;
			}
		}
	}

	int GetCumPerMast(BYTE byCode, BYTE byMast)
	{
		return m_ppdwMasteryCumPtr[byCode][byMast];
	}
	int	GetMasteryPerMast(BYTE byCode, BYTE byMast)
	{
		return m_ppbyMasteryPtr[byCode][byMast];
	}
	int	GetSkillLv(BYTE bySkillIndex)
	{
		return 	m_lvSkill[bySkillIndex];
	}

	float GetAveForceMasteryPerClass(BYTE byClass)
	{
		int nSum = 0;
		for(int i = 0; i < mastery_level_num; i++)
		{
			nSum += m_ppbyMasteryPtr[mastery_code_force][byClass*mastery_level_num+i];
		}
		return (float)nSum/mastery_level_num;
	}

	float GetAveSkillMasteryPerClass(BYTE byClass)//sk_class_bow이면 근거리.. 아니면 원거리..
	{
		int nSum = 0;
		int nDiv = 1;

		if(byClass == attack_range_short)
		{
			for(int i = 0; i <= 3; i++)	//근거리 마스터리는 0, 1, 2, 3..
				nSum += m_ppbyMasteryPtr[mastery_code_skill][i];
			nDiv = 4;
		}
		else
		{
			for(int i = 4; i <= 6; i++)	//원거리 마스터리는 4, 5, 6..
				nSum += m_ppbyMasteryPtr[mastery_code_skill][i];
			nDiv = 3;
		}

		return (float)nSum/nDiv;
	}

	static bool IsValidMasteryCode(BYTE byCode, BYTE byIndex)
	{
		if(byCode == mastery_code_weapon)
		{
			if(byIndex >= WP_CLASS_NUM)
				return false;
		}
		else if(byCode == mastery_code_skill)
		{
			if(byIndex >= skill_mastery_num)
				return false;
		}
		else if(byCode == mastery_code_force)
		{
			if(byIndex >= force_mastery_num)
				return false;
		}
		else if(byCode == mastery_code_make)
		{
			if(byIndex >= MI_CLASS_NUM)
				return false;
		}
		else if(byCode == mastery_code_defence || byCode == mastery_code_shield || byCode == mastery_code_special)
		{
			if(byIndex > 0)
				return false;
		}
		else
			return false;

		return true;
	}
	
	bool AlterCumPerMast(BYTE byClass, BYTE byIndex, DWORD dwAlterCum, DWORD* OUT pdwAfterCum, BYTE* pbyAfterMastery, bool* pbUpdate, bool* OUT pbUpEquip)
	{	//장착관련 마스터리가 변경돼면 true를 리턴.
		//마스터리가 업그레이드돼면 *pbUpdate를 true로..
		*pbUpdate = false;
		*pbUpEquip = false;
		switch(byClass)
		{
		case mastery_code_weapon:
			{
				m_BaseCum.m_dwDamWpCnt[byIndex] += dwAlterCum;
				*pdwAfterCum = m_BaseCum.m_dwDamWpCnt[byIndex];

				if(m_mtyWp[byIndex] < 99)
				{
					DWORD dwNewMatery = CalcMastery(mastery_code_weapon, byIndex, m_BaseCum.m_dwDamWpCnt[byIndex]);
					*pbyAfterMastery = (BYTE)dwNewMatery;
					if(m_mtyWp[byIndex] < dwNewMatery)
					{
						m_mtyWp[byIndex] = dwNewMatery;
						*pbUpdate = true;
						*pbUpEquip = true;
					}
				}
			}	break;		

		case mastery_code_defence:
			{
				m_BaseCum.m_dwDefenceCnt += dwAlterCum;
				*pdwAfterCum = m_BaseCum.m_dwDefenceCnt;

				if(m_mtySuffer < 99)
				{
					DWORD dwNewMatery = CalcMastery(mastery_code_defence, 0, m_BaseCum.m_dwDefenceCnt);
					*pbyAfterMastery = (BYTE)dwNewMatery;
					if(m_mtySuffer < dwNewMatery)
					{
						m_mtySuffer = dwNewMatery;
						*pbUpdate = true;
						*pbUpEquip = true;
					}
				}
			}	break;		

		case mastery_code_shield:
			{
				m_BaseCum.m_dwShieldCnt += dwAlterCum;
				*pdwAfterCum = m_BaseCum.m_dwShieldCnt;

				if(m_mtyShield < 99)
				{
					DWORD dwNewMatery = CalcMastery(mastery_code_shield, 0, m_BaseCum.m_dwShieldCnt);
					*pbyAfterMastery = (BYTE)dwNewMatery;
					if(m_mtyShield < dwNewMatery)
					{
						m_mtyShield = dwNewMatery;
						*pbUpdate = true;
						*pbUpEquip = true;
					}
				}			
			}	break;			

		case mastery_code_skill:
			{
				m_BaseCum.m_dwSkillCum[byIndex] += dwAlterCum;
				*pdwAfterCum = m_BaseCum.m_dwSkillCum[byIndex];

				_skill_fld* pSkillFld = (_skill_fld*)s_pSkillData->GetRecord(byIndex);
				BYTE byBufLv = GetSFLevel(pSkillFld->m_nLv, m_BaseCum.m_dwSkillCum[byIndex]);
				if(byBufLv > m_lvSkill[byIndex])
				{
					m_lvSkill[byIndex] = byBufLv;
				}

				if(m_mtySkill[pSkillFld->m_nMastIndex] < 99)
				{
					m_dwSkillMasteryCum[pSkillFld->m_nMastIndex] += dwAlterCum;
					DWORD dwNewMatery = CalcMastery(mastery_code_skill, pSkillFld->m_nMastIndex, m_dwSkillMasteryCum[pSkillFld->m_nMastIndex]);
					*pbyAfterMastery = (BYTE)dwNewMatery;
					if(m_mtySkill[pSkillFld->m_nMastIndex] < dwNewMatery)
					{
						m_mtySkill[pSkillFld->m_nMastIndex] = dwNewMatery;
						*pbUpdate = true;
					}
				}
			}	break;

		case mastery_code_force:
			{
				m_BaseCum.m_dwForceCum[byIndex] += dwAlterCum;
				*pdwAfterCum = m_BaseCum.m_dwForceCum[byIndex];
				m_dwForceLvCum[byIndex%mastery_level_num] += dwAlterCum;

				if(m_mtyForce[byIndex] < 99)
				{
					DWORD dwNewMatery = CalcMastery(mastery_code_force, byIndex, m_BaseCum.m_dwForceCum[byIndex]);
					*pbyAfterMastery = (BYTE)dwNewMatery;
					if(m_mtyForce[byIndex] < dwNewMatery)
					{
						m_mtyForce[byIndex] = dwNewMatery;
						*pbUpdate = true;
					}
				}

				if(m_mtyForce[byIndex] < 99)
				{	//staff mastery..
					DWORD dwFLvMty = GetStaffMastery(m_dwForceLvCum);
					if(m_mtyStaff < dwFLvMty)
					{
						m_mtyStaff = dwFLvMty;						
						*pbUpEquip = true;
					}
				}	
			}	break;

		case mastery_code_make:
			{
				m_BaseCum.m_dwMakeCum[byIndex] += dwAlterCum;
				*pdwAfterCum = m_BaseCum.m_dwMakeCum[byIndex];

				if(m_mtyMakeItem[byIndex] < 99)
				{
					DWORD dwNewMatery;
					switch(byIndex)
					{
					case mi_class_shield:
						dwNewMatery = CalcMastery(mastery_code_make, mi_class_shield, m_BaseCum.m_dwMakeCum[byIndex]);	break;
					case mi_class_armor:
						dwNewMatery = CalcMastery(mastery_code_make, mi_class_armor, m_BaseCum.m_dwMakeCum[byIndex]);	break;
					case mi_class_bullet:
						dwNewMatery = CalcMastery(mastery_code_make, mi_class_bullet, m_BaseCum.m_dwMakeCum[byIndex]);	break;
					}

					*pbyAfterMastery = (BYTE)dwNewMatery;
					if(m_mtyMakeItem[byIndex] < dwNewMatery)
					{
						m_mtyMakeItem[byIndex] = dwNewMatery;
						*pbUpdate = true;
					}
				}			
			}	break;

		case mastery_code_special:
			{
				m_BaseCum.m_dwSpecialCum += dwAlterCum;
				*pdwAfterCum = m_BaseCum.m_dwSpecialCum;

				if(m_mtySpecial < 99)
				{
					DWORD dwNewMatery = CalcMastery(mastery_code_special, 0, m_BaseCum.m_dwSpecialCum);
					*pbyAfterMastery = (BYTE)dwNewMatery;
					if(m_mtySpecial < dwNewMatery)
					{
						m_mtySpecial = dwNewMatery;
						*pbUpdate = true;
					}
				}			
			}	break;

		default:
			return false;
		}

		return true;
	}

	int CalcMastery(int nMasteryCode, int nMasteryIndex, DWORD dwMasteryCum)
	{
		static float sfSkillLv[mastery_level_num] = {40.0f, 40.0f, 40.0f, 40.0f};
		static float sfForceLv[mastery_level_num] = {40.0f, 40.0f, 40.0f, 40.0f};

		int nNewMastery = 0;

		switch(nMasteryCode)
		{
		case mastery_code_weapon:
			nNewMastery = sqrt( ((float)(dwMasteryCum+1)/1000) + sqrt(dwMasteryCum+1.0f) );
			break;

		case mastery_code_defence:
			nNewMastery = sqrt( ((float)(dwMasteryCum+1)/1000) + sqrt(dwMasteryCum+1.0f) );
			break;

		case mastery_code_shield:
			nNewMastery = sqrt( ((float)(dwMasteryCum+1)/1000) + sqrt(dwMasteryCum+1.0f) );
			break;

		case mastery_code_skill:{
			float fRDiv = sfSkillLv[s_nSkillLvPerMastery[nMasteryIndex]];
			nNewMastery = sqrt( ((float)(dwMasteryCum+1)/fRDiv) * 1.0f ) + 0.99;
								}
			break;

		case mastery_code_force:{
			float fRDiv = sfForceLv[s_nForceLvPerMastery[nMasteryIndex]];
			nNewMastery = sqrt( ((float)(dwMasteryCum+1)/fRDiv) * 1.0f ) + 0.99;
								}
			break;

		case mastery_code_make:{
			switch(nMasteryIndex)
			{
			case mi_class_shield:
				nNewMastery = sqrt( ((float)dwMasteryCum/1.1f) * 3.0f + 1);	
				break;
			case mi_class_armor:
				nNewMastery = sqrt( ((float)dwMasteryCum/1.1f) * 3.0f + 1);	
				break;
			case mi_class_bullet:
				nNewMastery = sqrt( ((float)dwMasteryCum/10.0f) * 3.0f + 1);	
				break;
			}
							   }
			break;

		case mastery_code_special:
			if(m_byRaceCode == race_code_bellato)	//벨라토..	(유닛)
				nNewMastery = sqrt( ((float)dwMasteryCum/10.0f) * 1.0f ) + 0.99;
			else if(m_byRaceCode == race_code_cora)	//코라.. (소환)
				nNewMastery = sqrt( (float)(dwMasteryCum+1)/1000.0f + sqrt(dwMasteryCum+1.0f));
			else									//아크레시아.. (런쳐)
				nNewMastery = sqrt( ((float)dwMasteryCum/10.0f) * 1.0f ) + 0.99;
			break;
		}

		if( nNewMastery > 99 )			
			nNewMastery = 99;
		else if( nNewMastery < 1 )		
			nNewMastery = 1;

		return nNewMastery;
	}

	int GetSFLevel(int nLv, WORD wHitCount)
	{
		static float sR[mastery_level_num] = { 200, 200, 200, 200 };

		int nLevel = (int)(sqrt( (float)(wHitCount+1)/sR[nLv] ) + 0.9999);

		if(nLevel > 7)			
			nLevel = 7;
		else if(nLevel < 1)		
			nLevel = 1;

		return nLevel;
	}

	int GetStaffMastery(DWORD* pdwForceLvCum)
	{/*
			  INT(SQRT(((저급포스총누적회수*저급포스상수(1.125)+중급포스총누적회수
                   *중급포스상수(2.25)+고급포스총누적회수*고급포스상수(3.375)
                   +최고급포스총누적회수*최고급포스상수(4.5)+1)/1000)
                   +SQRT(저급포스총누적회수*저급포스상수(1.125)+중급포스총누적회수
                   *중급포스상수(2.25)+고급포스누적회수*고급포스상수(3.375)
                   +최고급포스총누적회수*최고급포스상수(4.5))))
	*/
		int nMastery = sqrt(
							  ((float)(pdwForceLvCum[0]*1.125f+pdwForceLvCum[1]*2.25f+pdwForceLvCum[2]*3.375f+pdwForceLvCum[3]*4.5f+1)/1000)
							   +sqrt(pdwForceLvCum[0]*1.125f+pdwForceLvCum[1]*2.25f+pdwForceLvCum[2]*3.375f+pdwForceLvCum[3]*4.5f+1)
						   );
		if( nMastery > 99 )			nMastery = 99;
		else if( nMastery < 1 )		nMastery = 1;
		return nMastery;
	}

	BYTE GetEquipMastery(int nEquipMasteryCode)
	{
		return *m_ppbyEquipMasteryPrt[nEquipMasteryCode];
	}
};

/////////////////////////////////
//struct _GROGGY_PARAM
/////////////////////////////////

struct _GROGGY_PARAM
{
	bool	m_bGroggy;
	DWORD	m_dwStartGroggyTime;
	DWORD	m_dwDurGroggyTime;

	_GROGGY_PARAM()	{	memset(this, 0, sizeof(*this));		}

	void Init()
	{
		m_bGroggy = false;
	}

	void StartGroggy(DWORD dwDurTime)
	{
		m_bGroggy = true;

		m_dwStartGroggyTime = timeGetTime();
		m_dwDurGroggyTime = dwDurTime;
	}

	void EndGroggy()
	{
		m_bGroggy = false;
	}

	void CheckGroggy(DWORD dwCurTime)
	{
		if(m_bGroggy)
		{
			if(dwCurTime > m_dwStartGroggyTime + m_dwDurGroggyTime)
				m_bGroggy = false;
		}
	}

	bool GetGroggy()
	{
		return m_bGroggy;
	}
};

/////////////////////////////////
//struct _TOWER_PARAM
/////////////////////////////////

struct _TOWER_PARAM
{
	enum	{	max_param = 3	};

	struct _list
	{
		__ITEM*			m_pTowerItem;
		WORD			m_wItemSerial;
		CGuardTower*	m_pTowerObj;

		void init()	{	m_pTowerItem = NULL; m_pTowerObj = NULL;	}
	};

	_list		m_List[max_param];

	void Init()
	{
		for(int i = 0; i < max_param; i++)
			m_List[i].init();
	}

	bool IsExist(__ITEM* pTowerItem)
	{
		for(int i = 0; i < max_param; i++)
		{
			if(m_List[i].m_pTowerItem == pTowerItem && 
				m_List[i].m_wItemSerial == pTowerItem->m_wSerial)
				return true;
		}
		return false;
	}

	bool IsEmpty()
	{
		for(int i = 0; i < max_param; i++)
		{
			if(!m_List[i].m_pTowerItem)
				return true;
		}
		return false;
	}

	bool PushList(__ITEM* pTowerItem, CGuardTower*	pTowerObj)
	{
		for(int i = 0; i < max_param; i++)
		{
			if(!m_List[i].m_pTowerItem)
			{
				m_List[i].m_pTowerItem = pTowerItem;
				m_List[i].m_wItemSerial = pTowerItem->m_wSerial;
				m_List[i].m_pTowerObj = pTowerObj;
				return true;
			}			
		}
		return false;
	}
};

#endif