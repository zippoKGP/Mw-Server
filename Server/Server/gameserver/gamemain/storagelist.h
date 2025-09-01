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
		bool	m_bLock;	//�̵�, ������, �ŷ� ����..

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
	//����Ʈ �ʱ⿡ �����Ҷ� �ѹ� �Ļ�Ŭ�������� ȣ��..
	void SetMemory(_db_con* pList, int nListName, int nListNum, int nUsedNum = 0);
	//����� ����Ʈ�� ���� ���Ҷ�..
	bool SetUseListNum(int nUsedNum);
	//��罽���� ������ ����..
	void SetAllEmpty();
	//����Ʈ���� �󽽷��� ��
	int GetNumEmptyCon();
	//����Ʈ���� �������� ��
	int GetNumUseCon();
	//�󽽷��� ù��° �ε��� ��������..
	int GetIndexEmptyCon();
	//�����۽ø����ȣ�� �����ε����� �����´�.
	int GetIndexFromSerial(WORD wSerial);
	//�����۽ø����ȣ�� ������ �����͸� �����´�.
	_STORAGE_LIST::_db_con* GetPtrFromSerial(WORD wSerial);
	//n��° ������ ����. 
	bool EmptyCon(int n);
	//pCon�������� ù��° �� ���Կ� �����Ѵ�.
	DWORD TransInCon(_storage_con* pCon);
	//n��° ���Ծ������� m_dwCurDurPoint�� ��ŭ ���Ѵ�
	bool AlterCurDur(int n, int nAlter, DWORD* OUT pdwLeftDur);
	//n��° ���Ծ������� m_dwCurDurPoint�� m_dwLimDurPoint�� dwSetDur�� �����Ѵ�.
	void SetLimCurDur(int n, DWORD dwSetDur);
	//n��° ���Ծ������� �������׷��̵�ܰ迡 byLvCode�� ���׷��̵��Ų��.
	bool GradeUp(int n, DWORD dwUptInfo);
	//n��° ���Ծ������� ������׷��̵�ܰ迡 ���׷��̵��ڵ带 �����Ѵ�.
	bool GradeDown(int n, DWORD dwUptInfo);
	//�ø����ȣ�� �ش�������� Ŭ���̾�Ʈ�� �ε����� �����Ѵ�.
	bool SetGrade(int n, BYTE byLv, DWORD dwUptInfo); 
	//n��° ���Ծ����ۿ����� ����/�����Ѵ�.
	void SetLock(int n, bool bLock);
	//�ø����� ã�Ƽ� Ŭ���̾�Ʈ �ε����� �����Ѵ�.	
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
