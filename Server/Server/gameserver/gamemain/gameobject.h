// GameObject.h: interface for the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GameObject_h_
#define _GameObject_h_

#include "GameObjectList.h"
#include "MyClassUtil.h"
#include "MapData.h"
#include "CharacterDB.h"
#include "NetUtil.h"

class CCharacter;

//오브젝트 구분
#define obj_race_bellato			0x00000000
#define obj_race_cora				0x00000001
#define obj_race_accretia			0x00000002
#define obj_race_monster			0x00000010
#define obj_race_npc				0x00000020
//#define obj_race_animus				0x00000030

//Object 고유식별 ID 구조체
struct _object_id	
{
	BYTE		m_byKind;	//Character or Item 등등
	BYTE		m_byID;		//Character이라면 Player, Monster, NPC.. 
							//Item이라면 그 종류들..
	WORD		m_wIndex;	//index번호

	_object_id(){}

	_object_id(BYTE byKind, BYTE byID, WORD wIndex)
	{
		m_byKind = byKind;
		m_byID = byID;
		m_wIndex = wIndex;
	}
};

//Object Create Setting Data를 담을 구조체
struct _object_create_setdata
{
	_base_fld*		m_pRecordSet;
	CMapData*		m_pMap;				//시작할  CMapData Pointer
	int				m_nLayerIndex;
	float			m_fStartPos[3];		//시작할 Map상의 좌표	

	_object_create_setdata(){	m_nLayerIndex = 0; }
};

class CGameObject
{
public:

	enum				{	stun_term = 1000	};
	enum				{	break_transper_term = 30000	};

	_base_fld*				m_pRecordSet;
	_object_id				m_ObjID;
	DWORD					m_dwObjSerial;		//create, destroy, move, fixpos에는 시리얼을 프로토콜에 넣어서 보낸다.

	bool					m_bLive;
	int						m_nTotalObjIndex;
	bool					m_bCorpse;
	bool					m_bMove;			//이동중인가?
	bool					m_bStun;
	bool					m_bMapLoading;
	DWORD					m_dwLastSendTime;	//최근에 주변전체에 보낸타임

	float					m_fCurPos[3];		//현재 position
	float					m_fAbsPos[3];		//절대 position(맵의 x의 min, y의 min, z의 max를 원점으로하여 재산정 ==> m_nScreenPos 와 secter를 구하는데 사용)
	int						m_nScreenPos[2];	//display view상의 screen좌표
	float					m_fOldPos[3];		//바로전의 position
	CMapData*				m_pCurMap;			//현재 map
	_100_per_random_table	m_rtPer100;

	WORD					m_wMapLayerIndex;
	_object_list_point		m_SectorPoint;		//sector의 Object list에 추가할 자신의 pointer
	_object_list_point		m_SectorNetPoint;	//sector의 Object list에 추가할 자신의 pointer

	DWORD					m_dwOldTickStun;
	DWORD					m_dwOldTickBreakTranspar;
	bool					m_bBreakTranspar;

	static CGameObject*		s_pSelectObject;	//display view상에서 선택된 object
	static int				s_nTotalObjectNum;
	static CGameObject*		s_pTotalObject[MAX_OBJ];
	
private:
	
	DWORD					m_dwCurSec;			//map상의 현재 위치하는 sector index

public:
//Construction/Destruction..
	CGameObject();
	virtual ~CGameObject();

//Data Init..
	void Init(_object_id* pID);
	bool Create(_object_create_setdata* pData);
	bool Destroy();
	bool CalcMsgPerSec();

	void OnLoop();

//Set Position..
	bool SetCurPos(float* pPos);
	bool SetCurBspMap(CMapData* pMap);

//Sector Arrange..
	void CalcAbsPos();		//절대좌표를 계산한다.
	bool UpdateSecList();	//sector index 및 sector list를 추가 및 삭제를 update한다. 
	void ResetSector(DWORD dwOldSec, DWORD dwNewSec, bool bWhole); //섹터가 변경됐을때만 호출
	void SendMsg_BreakStop();
	DWORD  CalcSecIndex();	//절대좌표를 이용하여 sector index를 얻어낸다.
	bool RerangeSecIndex(DWORD dwOld, DWORD dwNew, DWORD* OUT pdwAdj);	//
	bool IsInSector(CGameObject* pObj, int nRadius = -1);	//pObj가 자신의 시야에 존재하는가?
	DWORD GetCurSecNum();
	void  SetCurSecNum(DWORD dwNewSecNum);
	
	void SetStun(bool bStun);
	void SetBreakTranspar(bool bBreak);

//Screen View..
	void CalcScrNormalPoint(CRect* prcWnd);		//등비 디스플레이뷰상의 좌표점
	void CalcScrExtendPoint(CRect* prcWnd, CRect* prcExtend);	//확대 디스플레이뷰상의 좌표점

	void CircleReport(BYTE* pbyType, char* szMsg, int nMsgSize, bool bToOne = false);
	void RangeCircleReport(DWORD dwRangeSecNum, BYTE* pbyType, char* szMsg, int nMsgSize, bool bToOne = false);

//virtual..
	virtual void Loop() {};
	virtual void LoopPerSec() {};
	virtual	int GetObjRace()	{	return -1;	}
	virtual void AlterSec() {};

	void SendMsg_RealFixPosition();
	virtual void SendMsg_FixPosition(int n) {}	//pDst에게 자신의 서있는 위치를 알린다.
	virtual void SendMsg_RealMovePoint(int n) {}	
	virtual void SendMsg_StunInform() {}	//stun상태의 변화를 알린다.
	virtual void SendMsg_SetHPInform() {}

	virtual int GetHP() { return 0; }
	virtual void SetHP(int nHP, bool bOver) {}
	virtual int	GetMaxHP( void ) { return 0; }
	virtual int SetDamage(int nDam, CCharacter* pDst, int nDstLv, bool bCrt) { return 0;	}
	virtual float GetWidth() { return 0.0f; }
	virtual float GetAttackRange() { return 0.0f; }
	virtual int GetDefFC(int nAttactPart, CCharacter* pAttChar) { return 1; }
	virtual void SetAttackPart(int nAttactPart)	{}
	virtual int	GetFireTol() { return 0; }
	virtual int	GetWaterTol() { return 0; }
	virtual int	GetSoilTol() { return 0; }
	virtual int	GetWindTol() { return 0; }	
	virtual float GetDefGap() { return 0.5f;	}
	virtual float GetDefFacing() { return 0.5f;	}
	virtual float GetWeaponAdjust() { return 0.5f; }
	virtual int	AttackableHeight() {	return attack_able_height;	}	//공격가능높이
	virtual int GetGenAttackProb(CCharacter* pDst, int nPart)	{	return 0;	}	//일반공격확률
	virtual int GetLevel() { return 1; }
	virtual int GetDefSkill(){ return 1; }
	virtual bool IsRecvableContEffect() {	return true;	}
	virtual bool IsBeAttackedAble()	{	return false;	}
	virtual bool IsRewardExp()	{	return false;	}		//자기를 공격하면 경험치 보상을 해줄것인지..

	//SF 일시효과
	virtual bool F_AttHPtoDstFP_Once(CCharacter* pDstObj){	return false; }	// 자신의 HP를 대상의 FP로 만든다
	virtual bool F_ContDamageTimeInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// 지속 피해보조 남은 시간 늘이기
	virtual	bool F_Resurrect_Once(CCharacter* pDstObj){	return false; }		// 죽은 유저를 재생한다.
	virtual bool F_HPInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// 현재 HP증가
	virtual bool F_STInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// 현재 ST증가
	virtual bool F_ContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// 지속 이득보조의 남은 시간 늘리기	
	virtual bool F_OverHealing_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// overhealing
	virtual bool F_LateContHelpSkillRemove_Once(CCharacter* pDstObj){	return false; }	// 가장 최근 지속이득보조 스킬 해제	
	virtual bool F_LateContHelpForceRemove_Once(CCharacter* pDstObj){	return false; }	// 가장 최근 지속이득보조 포스 해제
	virtual bool F_LateContDamageForceRemove_Once(CCharacter* pDstObj){	return false; }	// 가장 최근 지속피해보조 포스 해제
	virtual bool F_AllContHelpSkillRemove_Once(CCharacter* pDstObj){	return false; }	// 대상에 걸려있는 지속이득보조 스킬 제거
	virtual bool F_AllContHelpForceRemove_Once(CCharacter* pDstObj){	return false; }	// 대상에 걸려있는 지속이득보조 포스 제거
	virtual bool F_AllContDamageForceRemove_Once(CCharacter* pDstObj){	return false; }	// 대상에 걸려있는 지속 피해 보조 포스 해제
	virtual bool F_OthersContHelpSFRemove_Once(float fEffectValue){	return false; }	// 시전자 주위 다른 종족들의 지속 이득 보조 스킬 & 포스 해제
	virtual bool S_SkillContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// skill에 의한 지속적인 이득 남은 시간 늘이기
	virtual bool S_ConvertMonsterTarget(CCharacter* pDstObj, float fEffectValue){	return false; }
	virtual bool S_TransMonsterHP(CCharacter* pDstObj, float fEffectValue){	return false; }
	virtual bool S_ReleaseMonsterTarget(CCharacter* pDstObj, float fEffectValue){	return false; }


};

extern const double g_pi;

#endif 
