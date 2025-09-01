#include "stdafx.h"

#include "PlayerDB.h"
#include "Player.h"
#include "MainThread.h"
#include "WeaponItemFld.h"
#include "DfnEquipItemFld.h"
#include "PotionItemFld.h"
#include "ForceFld.h"
#include "SkillFld.h"
#include "ForceItemFld.h"
#include "ClassFld.h"

CPlayerDB::CPlayerDB()
{
	m_wSerialCount = 0;

	m_pStoragePtr[_STORAGE_POS::INVEN] = &m_dbInven;
	m_pStoragePtr[_STORAGE_POS::EQUIP] = &m_dbEquip;
	m_pStoragePtr[_STORAGE_POS::EMBELLISH] = &m_dbEmbellish;
	m_pStoragePtr[_STORAGE_POS::BELT] = &m_dbBelt;
	m_pStoragePtr[_STORAGE_POS::FORCE] = &m_dbForce;
	m_pStoragePtr[_STORAGE_POS::ANIMUS] = &m_dbAnimus;

	m_wCuttingResBuffer = NULL;
}

CPlayerDB::~CPlayerDB()
{
	if(m_wCuttingResBuffer)
		delete [] m_wCuttingResBuffer;
}

void CPlayerDB::InitPlayerDB(CPlayer* pThis)
{
	if(!m_wCuttingResBuffer)
		m_wCuttingResBuffer = new WORD [::GetMaxResKind()];

	m_pThis = pThis;
	m_wSerialCount = 0;
	InitResBuffer();
	
	for(int i = 0; i < total_storage_num; i++)
		m_pStoragePtr[i]->SetAllEmpty();

	m_QuestDB.Init();
	m_UnitDB.Init();
	InitSFLink();	
	InitAlterMastery();

	m_pClassData = NULL;
	for(i = 0; i < class_history_num; i++)
		m_pClassHistory[i] = NULL;

	m_bLineUp = false;
	memset(m_byChangeInvenIndex, 0, sizeof(m_byChangeInvenIndex));		
	memset(m_byChangeBeltIndex, 0, sizeof(m_byChangeBeltIndex));		
	memset(m_byChangeEmbellIndex, 0, sizeof(m_byChangeEmbellIndex));
}																				

int CPlayerDB::InvenChangeNum()
{
	if(!m_bLineUp)
		return 0;

	int nCnt = 0;
	for(int i = 0; i < bag_storage_num; i++)
		nCnt += m_byChangeInvenIndex[i];

	return nCnt;
}

int CPlayerDB::BeltChangeNum()
{
	if(!m_bLineUp)
		return 0;

	int nCnt = 0;
	for(int i = 0; i < potion_belt_num; i++)
		nCnt += m_byChangeBeltIndex[i];

	return nCnt;
}

int CPlayerDB::EmbellishChangeNum()
{
	if(!m_bLineUp)
		return 0;

	int nCnt = 0;
	for(int i = 0; i < embellish_fix_num; i++)
		nCnt += m_byChangeEmbellIndex[i];

	return nCnt;
}

int CPlayerDB::UseLinkNum()
{
	int nCnt = 0;
	for(int i = 0; i < sf_linker_num; i++)
	{
		if(m_SFLink[i].m_bLoad)
			nCnt++;
	}
	return nCnt;
}

int CPlayerDB::GetHaveUnitNum()
{
	int nCnt = 0;
	for(int i = 0; i < unit_storage_num; i++)
	{
		if(m_UnitDB.m_List[i].byFrame != 0xFF)
			nCnt++;
	}
	return nCnt;
}

bool CPlayerDB::ConvertAvatorDB(_AVATOR_DATA* pData)
{
	strcpy(m_dbChar.m_szCharID, pData->dbAvator.m_szAvatorName);
	m_dbChar.m_dwSerial = pData->dbAvator.m_dwRecordNum;
	m_dbChar.m_byRaceSexCode = pData->dbAvator.m_byRaceSexCode;
	m_dbChar.m_dwHP = pData->dbAvator.m_dwHP;
	m_dbChar.m_dwFP = pData->dbAvator.m_dwFP;
	m_dbChar.m_dwSP = pData->dbAvator.m_dwSP;
	m_dbChar.m_dExp = pData->dbAvator.m_dExp;
	m_dbChar.m_dwDalant = pData->dbAvator.m_dwDalant;
	m_dbChar.m_dwGold = pData->dbAvator.m_dwGold;
	for(int i = 0; i < base_fix_num; i++)
	{
		DWORD dwPart = ((pData->dbAvator.m_dwBaseShape>>(i*4))&0x0000000F); 
		m_dbChar.m_byDftPart[i] = (BYTE)dwPart+(pData->dbAvator.m_byRaceSexCode*default_per_part);
	}
	DWORD dwFace = (pData->dbAvator.m_dwBaseShape>>(base_fix_num*4))&0x0000000F;  
	m_dbChar.m_byDftPart_Face = dwFace+(pData->dbAvator.m_byRaceSexCode*default_per_part);
	m_dbChar.m_byUseBagNum = pData->dbAvator.m_byBagNum;
	m_dbInven.SetUseListNum(pData->dbAvator.m_byBagNum*one_bag_store_num);
	m_dbChar.m_byLevel = pData->dbAvator.m_byLevel;
	m_dbChar.m_sStartMapCode = pData->dbAvator.m_byMapCode;
	memcpy(m_dbChar.m_fStartPos, pData->dbAvator.m_fStartPos, sizeof(float)*3);

	//클래스..
	m_pClassData = (_class_fld*)g_Main.m_tblClass.GetRecord(pData->dbAvator.m_szClassCode);
	if(!m_pClassData)
		return false;

	//과거 클래스
	for(i = 0; i < class_history_num; i++)
	{
		if(pData->dbAvator.m_zClassHistory[i] == 0xFFFF)
			break;

		_class_fld* pFld = (_class_fld*)g_Main.m_tblClass.GetRecord(pData->dbAvator.m_zClassHistory[i]);
		if(!pFld)
			return false;
		m_pClassHistory[i] = pFld;
	}
	return true;
}

bool CPlayerDB::ConvertGeneralDB(_AVATOR_DATA* pData)
{
//가방 
	for(int i = 0; i < pData->dbAvator.m_byBagNum*one_bag_store_num; i++)
	{
		_INVEN_DB_BASE::_LIST* pList = &pData->dbInven.m_List[i];

		if(!pList->Key.IsFilled())
			continue;

		m_dbInven.m_pStorageList[i].m_byTableCode = pList->Key.byTableCode;
		m_dbInven.m_pStorageList[i].m_byClientIndex = pList->Key.bySlotIndex;
		m_dbInven.m_pStorageList[i].m_wItemIndex = pList->Key.wItemIndex;
		m_dbInven.m_pStorageList[i].m_dwDur = pList->dwDur;
		m_dbInven.m_pStorageList[i].m_dwLv = pList->dwUpt;
		m_dbInven.m_pStorageList[i].m_bLoad = true;
	}

//장착	
	for(i = 0; i < equip_fix_num; i++)
	{
		if(!pData->dbAvator.m_EquipKey[i].IsFilled())
			continue;

		m_dbEquip.m_pStorageList[i].m_byTableCode = i;
		m_dbEquip.m_pStorageList[i].m_byClientIndex = i;
		m_dbEquip.m_pStorageList[i].m_wItemIndex = pData->dbAvator.m_EquipKey[i].zItemIndex;
		m_dbEquip.m_pStorageList[i].m_dwDur = 0;
		m_dbEquip.m_pStorageList[i].m_dwLv = pData->dbAvator.m_dwFixEquipLv[i];
		m_dbEquip.m_pStorageList[i].m_bLoad = true;

		//장착효과처리..
		m_pThis->SetEffectEquipCode(_STORAGE_POS::EQUIP, i, CPlayer::effect_wait);
	}

//장식  
	for(i = 0; i < embellish_fix_num; i++)
	{
		_EQUIP_DB_BASE::_EMBELLISH_LIST* pList = &pData->dbEquip.m_EmbellishList[i];

		if(!pList->Key.IsFilled())
			continue;

		m_dbEmbellish.m_pStorageList[i].m_byTableCode = pList->Key.byTableCode;
		m_dbEmbellish.m_pStorageList[i].m_byClientIndex = pList->Key.bySlotIndex;
		m_dbEmbellish.m_pStorageList[i].m_wItemIndex = pList->Key.wItemIndex;
		m_dbEmbellish.m_pStorageList[i].m_dwDur = pList->wAmount;
		m_dbEmbellish.m_pStorageList[i].m_dwLv = __DF_UPT;
		m_dbEmbellish.m_pStorageList[i].m_bLoad = true;

		//착용효과처리..
		m_pThis->SetEffectEquipCode(_STORAGE_POS::EMBELLISH, i, CPlayer::effect_wait);
	}

//포스	
	for(i = 0; i < force_storage_num; i++)
	{
		_FORCE_DB_BASE::_LIST* pList = &pData->dbForce.m_List[i];

		if(!pList->Key.IsFilled())
			continue;

		m_dbForce.m_pStorageList[i].m_byTableCode = tbl_code_fcitem;
		m_dbForce.m_pStorageList[i].m_wItemIndex = pList->Key.GetIndex();
		m_dbForce.m_pStorageList[i].m_dwDur = pList->Key.GetStat();
		m_dbForce.m_pStorageList[i].m_dwLv = __DF_UPT;
		m_dbForce.m_pStorageList[i].m_bLoad = true;
	}

//포션	
	for(i = 0; i < potion_belt_num; i++)
	{
		_BELT_DB_BASE::_POTION_LIST* pList = &pData->dbBelt.m_PotionList[i];

		if(!pList->Key.IsFilled())
			continue;

		m_dbBelt.m_pStorageList[i].m_byTableCode = tbl_code_potion;
		m_dbBelt.m_pStorageList[i].m_byClientIndex = pList->Key.bySlotIndex;
		m_dbBelt.m_pStorageList[i].m_wItemIndex = pList->Key.wItemIndex;
		m_dbBelt.m_pStorageList[i].m_dwDur = pList->Key.byAmount;
		m_dbBelt.m_pStorageList[i].m_dwLv = __DF_UPT;
		m_dbBelt.m_pStorageList[i].m_bLoad = true;
	}

//링크 
	for(i = 0; i < sf_linker_num; i++)
	{
		_BELT_DB_BASE::_LINK_LIST* pList = &pData->dbBelt.m_LinkList[i];

		if(!pList->Key.IsFilled())
			continue;

		m_SFLink[i].m_byEffectCode = pList->Key.GetCode();
		m_SFLink[i].m_sClientIndex = i;
		m_SFLink[i].m_wEffectIndex = pList->Key.GetIndex();
		m_SFLink[i].m_bLoad = true;
	}

//가공 
	for(i = 0; i < pData->dbCutting.m_byLeftNum; i++)
	{
		m_wCuttingResBuffer[pData->dbCutting.m_List[i].byResIndex] = pData->dbCutting.m_List[i].byAmt;
	}

//퀘스트
	memcpy(&m_QuestDB, &pData->dbQuest, sizeof(_QUEST_DB_BASE));

//유닛
	memcpy(&m_UnitDB, &pData->dbUnit, sizeof(_UNIT_DB_BASE));

//애니머스
	for(i = 0; i < animus_storage_num; i++)
	{
		_ANIMUS_DB_BASE::_LIST* pList = &pData->dbAnimus.m_List[i];

		if(!pList->Key.IsFilled())
			continue;

		m_dbAnimus.m_pStorageList[i].m_byTableCode = tbl_code_animus;
		m_dbAnimus.m_pStorageList[i].m_wItemIndex = pList->Key.byItemIndex;
		m_dbAnimus.m_pStorageList[i].m_dwDur = pList->dwExp;
		m_dbAnimus.m_pStorageList[i].m_dwLv = pList->dwParam;
		m_dbAnimus.m_pStorageList[i].m_bLoad = true;
	}

	return true;
}

void CPlayerDB::AppointSerialStorageItem()
{
	//시리얼부여순서..
	//1. 장착, 2. 장식, 3. 벨트, 4. 인벤토리, 5. 포스, 6. 애니머스

	for(int i = 0; i < equip_fix_num; i++)
	{
		if(m_dbEquip.m_pStorageList[i].m_bLoad)
			m_dbEquip.m_pStorageList[i].m_wSerial = GetNewItemSerial();
	}

	for(i = 0; i < embellish_fix_num; i++)
	{	
		if(m_dbEmbellish.m_pStorageList[i].m_bLoad)
			m_dbEmbellish.m_pStorageList[i].m_wSerial= GetNewItemSerial();
	}

	for(i = 0; i < potion_belt_num; i++)
	{
		if(m_dbBelt.m_pStorageList[i].m_bLoad)
			m_dbBelt.m_pStorageList[i].m_wSerial = GetNewItemSerial();
	}

	for(i = 0; i < bag_storage_num; i++)
	{
		if(m_dbInven.m_pStorageList[i].m_bLoad)
			m_dbInven.m_pStorageList[i].m_wSerial = GetNewItemSerial();
	}

	for(i = 0; i < force_storage_num; i++)
	{
		if(m_dbForce.m_pStorageList[i].m_bLoad)
			m_dbForce.m_pStorageList[i].m_wSerial = GetNewItemSerial();
	}

	for(i = 0; i < animus_storage_num; i++)
	{
		if(m_dbAnimus.m_pStorageList[i].m_bLoad)
			m_dbAnimus.m_pStorageList[i].m_wSerial = GetNewItemSerial();
	}
}

BYTE CPlayerDB::GetResBufferNum()
{
	BYTE byCnt = 0;
	for(int i = 0; i < ::GetMaxResKind(); i++)
	{
		if(m_wCuttingResBuffer[i] > 0)
			byCnt++;
	}
	return byCnt;
}

void CPlayerDB::InitResBuffer()
{
	if(m_wCuttingResBuffer)
	{
		for(int i = 0; i < ::GetMaxResKind(); i++)
		{
			m_wCuttingResBuffer[i] = 0;
		}
	}
}

bool CPlayerDB::IsClassChangeableLv()
{
	if(m_pClassData->m_nUpGradeLv <= GetLevel())
		return true;

	return false;
}

bool CPlayerDB::IsActableClassSkill(char* pszSkillCode)
{
	bool bFind = false;
	_class_fld** ppClass = &m_pClassData;
	for(int i = 0; i < class_history_num+1; i++)
	{
		if(!ppClass[i])
			break;
		for(int s = 0; s < max_link_claskill; s++)
		{
			if(!strcmp(ppClass[i]->m_strLinkClassSkill[s], "-1"))
				break;
			if(!strcmp(ppClass[i]->m_strLinkClassSkill[s], pszSkillCode))
			{
				bFind = true;
				break;
			}
		}
		if(bFind)
			break;
	}	
	if(!bFind)
		return false;

	return true;
}

WORD CPlayerDB::GetNewItemSerial()
{
	WORD wBufferSerial = m_wSerialCount;
	m_wSerialCount++;

	return wBufferSerial;
}

