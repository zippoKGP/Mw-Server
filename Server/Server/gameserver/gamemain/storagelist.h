#ifndef _Storage_List_h_
#define _Storage_List_h_

#include "CharacterDB.h"
#include "NetUtil.h"
#include "SkillFld.h"
#include <math.h>

#pragma pack(push, 1)

//struct _character_db_load

struct _character_db_load
{
	char		m_szCharID[max_name_len+1];
	DWORD		m_dwSerial;
	BYTE		m_byRaceSexCode;
	DWORD		m_dwHP;
	DWORD		m_dwFP;
	DWORD		m_dwSP;
	double		m_dExp;
	BYTE		m_byLevel;
	DWORD		m_dwDalant;
	DWORD		m_dwGold;

	char		m_sStartMapCode;
	float		m_fStartPos[3];

	BYTE		m_byDftPart_Face;
	BYTE		m_byDftPart[base_fix_num];
	BYTE		m_byUseBagNum;

	_character_db_load(){
		memset(this, 0, sizeof(*this));
	}
};

struct _STORAGE_LIST
{
public:

	struct _storage_con{

		BYTE	m_bLoad;

		BYTE	m_byTableCode;
		BYTE	m_byClientIndex;
		WORD	m_wItemIndex;

		DWORD	m_dwDur;
		DWORD	m_dwLv;

		WORD	m_wSerial;
		bool	m_bLock;	//이동, 버리기, 거래 금지..

		_storage_con(){
			memset(this, 0, sizeof(*this));
			m_dwLv = __DF_UPT;
		}

		_storage_con(BYTE byTableCode, WORD wItemIndex, DWORD dwDur, DWORD dwLv, WORD wSerial){
			memset(this, 0, sizeof(*this));

			m_byTableCode = byTableCode;
			m_wItemIndex = wItemIndex;
			m_dwDur = dwDur;
			m_dwLv = dwLv;
			m_wSerial = wSerial;
		}
	};

	struct _db_con : public _storage_con{

		_STORAGE_LIST*	m_pInList;
		BYTE			m_byStorageIndex;

		_db_con(){
			m_pInList = NULL;
			m_byStorageIndex = 0xFF;
		}

		_db_con(BYTE byTableCode, WORD wItemIndex, DWORD dwDur, DWORD dwLv, WORD wSerial) 
			: _storage_con(byTableCode, wItemIndex, dwDur, dwLv, wSerial){
			m_pInList = NULL;
			m_byStorageIndex = 0xFF;
		}

	};

	int				m_nListNum;
	int				m_nUsedNum;
	int				m_nListCode;

//private:

	_db_con*		m_pStorageList; 

public:

	_STORAGE_LIST();
	//리스트 초기에 생성할때 한번 파생클래스에서 호출..
	void SetMemory(_db_con* pList, int nListName, int nListNum, int nUsedNum = 0);
	//사용할 리스트를 따로 정할때..
	bool SetUseListNum(int nUsedNum);
	//모든슬롯의 내용을 비운다..
	void SetAllEmpty();
	//리스트에서 빈슬롯의 수
	int GetNumEmptyCon();
	//리스트에서 찬슬롯의 수
	int GetNumUseCon();
	//빈슬롯의 첫번째 인덱스 가져오기..
	int GetIndexEmptyCon();
	//아이템시리얼번호로 슬롯인덱스를 가져온다.
	int GetIndexFromSerial(WORD wSerial);
	//아이템시리얼번호로 슬롯의 포인터를 가져온다.
	_STORAGE_LIST::_db_con* GetPtrFromSerial(WORD wSerial);
	//n번째 슬롯을 비운다. 
	bool EmptyCon(int n);
	//pCon아이템을 첫번째 빈 슬롯에 삽입한다.
	DWORD TransInCon(_storage_con* pCon);
	//n번째 슬롯아이템의 m_dwCurDurPoint를 만큼 더한다
	bool AlterCurDur(int n, int nAlter, DWORD* OUT pdwLeftDur);
	//n번째 슬롯아이템의 m_dwCurDurPoint과 m_dwLimDurPoint를 dwSetDur로 세팅한다.
	void SetLimCurDur(int n, DWORD dwSetDur);
	//n번째 슬롯아이템의 다음업그레이드단계에 byLvCode로 업그레이드시킨다.
	bool GradeUp(int n, DWORD dwUptInfo);
	//n번째 슬롯아이템의 현재업그레이드단계에 업그레이드코드를 삭제한다.
	bool GradeDown(int n, DWORD dwUptInfo);
	//시리얼번호로 해당아이템의 클라이언트의 인덱스를 세팅한다.
	bool SetGrade(int n, BYTE byLv, DWORD dwUptInfo); 
	//n번째 슬롯아이템에락을 설정/해제한다.
	void SetLock(int n, bool bLock);
	//시리얼을 찾아서 클라이언트 인덱스를 세팅한다.	
	BYTE SetClientIndexFromSerial(WORD wSerial, BYTE byClientIndex, BYTE* pbyoutOldClientIndex);
}; 

//struct _bag_db_load

struct _bag_db_load : public _STORAGE_LIST
{
	_db_con m_List[bag_storage_num]; 

	_bag_db_load(){
		SetMemory(m_List, _STORAGE_POS::INVEN, bag_storage_num);
	}
};

//struct _equip_db_load

struct _equip_db_load : public _STORAGE_LIST
{
	_db_con		m_List[equip_fix_num]; 

	_equip_db_load(){
		SetMemory(m_List, _STORAGE_POS::EQUIP, equip_fix_num);
	}
};

//struct _embellish_db_load

struct _embellish_db_load : public _STORAGE_LIST
{
	_db_con		m_List[embellish_fix_num]; 

	_embellish_db_load(){
		SetMemory(m_List, _STORAGE_POS::EMBELLISH, embellish_fix_num);
	}
};

//struct _belt_db_load

struct _belt_db_load : public _STORAGE_LIST
{
	_db_con		m_List[potion_belt_num]; 

	_belt_db_load(){
		SetMemory(m_List, _STORAGE_POS::BELT, potion_belt_num);
	}
};

//struct _force_inven_db_load

struct _force_db_load : public _STORAGE_LIST
{
	_db_con		m_List[force_storage_num]; 

	_force_db_load(){
		SetMemory(m_List, _STORAGE_POS::FORCE, force_storage_num);
	}	
};

//struct _animus_db_load

struct _animus_db_load : public _STORAGE_LIST
{
	_db_con		m_List[animus_storage_num]; 

	_animus_db_load(){
		SetMemory(m_List, _STORAGE_POS::ANIMUS, animus_storage_num);
	}
};

//struct _sf_linker_db_load
struct _SF_LINK{

	bool	m_bLoad;
	BYTE	m_byEffectCode;
	WORD	m_wEffectIndex;
	char	m_sClientIndex;

	_SF_LINK(){ Init();	}

	void Init(){
		memset(this, 0, sizeof(*this));
	}
};

typedef _STORAGE_LIST::_db_con __ITEM;



#pragma pack(pop)

#endif
