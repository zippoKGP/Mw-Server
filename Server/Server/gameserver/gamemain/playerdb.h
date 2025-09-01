#ifndef _PlayerDB_h_
#define _PlayerDB_h_

#include "StorageList.h"
#include <math.h>

struct _base_fld;
class CAttackChar;
class CPlayer;
struct _class_fld;

#define STAT_KIND_NUM	sizeof(_STAT_DB_BASE)/sizeof(DWORD)

class CPlayerDB
{
public:

	enum				{	total_storage_num	=	_STORAGE_POS::STORAGE_NUM	};

	_character_db_load		m_dbChar;

	_bag_db_load			m_dbInven;
	_equip_db_load			m_dbEquip;
	_embellish_db_load		m_dbEmbellish;
	_belt_db_load			m_dbBelt;
	_force_db_load			m_dbForce;
	_animus_db_load			m_dbAnimus;

	_STORAGE_LIST*			m_pStoragePtr[total_storage_num];
	_SF_LINK				m_SFLink[sf_linker_num];
	_UNIT_DB_BASE			m_UnitDB;
	_QUEST_DB_BASE			m_QuestDB;

	WORD*					m_wCuttingResBuffer;

	bool					m_bLineUp;
	BYTE					m_byChangeInvenIndex[bag_storage_num];		//인벤순서 바뀐것들..
	BYTE					m_byChangeBeltIndex[potion_belt_num];		//벨트순서 바뀐것들..
	BYTE					m_byChangeEmbellIndex[embellish_fix_num];	//바뀐장식창..
	bool					m_bAlterMastery[STAT_KIND_NUM];				//마스터리 로그 term마다 변경된 마스터리..(10분마다 갱신)

	//두개는 항상 세트로.. (포인터사용)
	_class_fld*				m_pClassData;
	_class_fld*				m_pClassHistory[class_history_num];

private:

	WORD					m_wSerialCount;
	CPlayer*				m_pThis;

public:

	CPlayerDB();
	~CPlayerDB();

	void InitPlayerDB(CPlayer* pThis);

	WORD GetNewItemSerial();

	bool ConvertAvatorDB(_AVATOR_DATA* pData);
	bool ConvertGeneralDB(_AVATOR_DATA* pData);

	void AppointSerialStorageItem();
	
	BYTE GetResBufferNum();
	int GetHaveUnitNum();
	bool IsClassChangeableLv();
	bool IsActableClassSkill(char* pszSkillCode);

	void InitSFLink()		{	memset(m_SFLink, 0, sizeof(m_SFLink));	}
	void InitAlterMastery()	{	memset(m_bAlterMastery, false, sizeof(m_bAlterMastery));	}	
	void InitResBuffer();
	int InvenChangeNum();
	int BeltChangeNum();
	int EmbellishChangeNum();
	int UseLinkNum();


	char* GetCharName()			{	return m_dbChar.m_szCharID;	}
	DWORD GetCharSerial()		{	return m_dbChar.m_dwSerial;	}
	int GetRaceSexCode()		{	return m_dbChar.m_byRaceSexCode;	}
	int GetRaceCode()			{	return m_dbChar.m_byRaceSexCode/2;	}
	void SetHP(DWORD dwHP)		{	m_dbChar.m_dwHP = dwHP;	}
	int GetHP()					{	return m_dbChar.m_dwHP;	}
	void SetFP(DWORD dwFP)		{	m_dbChar.m_dwFP = dwFP;	}
	int GetFP()					{	return m_dbChar.m_dwFP;	}
	void SetSP(DWORD dwSP)		{	m_dbChar.m_dwSP = dwSP;	}
	int GetSP()					{	return m_dbChar.m_dwSP;	}
	void SetExp(double dExp)	{	m_dbChar.m_dExp = dExp;	}
	double GetExp()				{	return m_dbChar.m_dExp;	}
	void SetLevel(int nLv)		{	m_dbChar.m_byLevel = nLv;	}
	int GetLevel()				{	return m_dbChar.m_byLevel;	}
	void SetBagNum(BYTE byNum)	{	m_dbChar.m_byUseBagNum = byNum;	}
	BYTE GetBagNum()			{	return m_dbChar.m_byUseBagNum;	}
	BYTE GetUseSlot()			{	return m_dbChar.m_byUseBagNum*one_bag_store_num;	}
	void SetDalant(DWORD dwDt)	{	m_dbChar.m_dwDalant = dwDt;	}
	DWORD GetDalant()			{	return m_dbChar.m_dwDalant;	}
	void SetGold(DWORD dwGold)	{	m_dbChar.m_dwGold = dwGold;	}
	DWORD GetGold()				{   return m_dbChar.m_dwGold;	}
	void SetMapCode(BYTE byCode){	m_dbChar.m_sStartMapCode = byCode;	}
	int GetMapCode()			{	return m_dbChar.m_sStartMapCode;	}
	void SetCurPos(float* fPos)	{	memcpy(	m_dbChar.m_fStartPos, fPos, sizeof(float)*3);	}
	float* GetCurPos()			{	return m_dbChar.m_fStartPos;	}
};

#endif