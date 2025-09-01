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

//������Ʈ ����
#define obj_race_bellato			0x00000000
#define obj_race_cora				0x00000001
#define obj_race_accretia			0x00000002
#define obj_race_monster			0x00000010
#define obj_race_npc				0x00000020
//#define obj_race_animus				0x00000030

//Object �����ĺ� ID ����ü
struct _object_id	
{
	BYTE		m_byKind;	//Character or Item ���
	BYTE		m_byID;		//Character�̶�� Player, Monster, NPC.. 
							//Item�̶�� �� ������..
	WORD		m_wIndex;	//index��ȣ

	_object_id(){}

	_object_id(BYTE byKind, BYTE byID, WORD wIndex)
	{
		m_byKind = byKind;
		m_byID = byID;
		m_wIndex = wIndex;
	}
};

//Object Create Setting Data�� ���� ����ü
struct _object_create_setdata
{
	_base_fld*		m_pRecordSet;
	CMapData*		m_pMap;				//������  CMapData Pointer
	int				m_nLayerIndex;
	float			m_fStartPos[3];		//������ Map���� ��ǥ	

	_object_create_setdata(){	m_nLayerIndex = 0; }
};

class CGameObject
{
public:

	enum				{	stun_term = 1000	};
	enum				{	break_transper_term = 30000	};

	_base_fld*				m_pRecordSet;
	_object_id				m_ObjID;
	DWORD					m_dwObjSerial;		//create, destroy, move, fixpos���� �ø����� �������ݿ� �־ ������.

	bool					m_bLive;
	int						m_nTotalObjIndex;
	bool					m_bCorpse;
	bool					m_bMove;			//�̵����ΰ�?
	bool					m_bStun;
	bool					m_bMapLoading;
	DWORD					m_dwLastSendTime;	//�ֱٿ� �ֺ���ü�� ����Ÿ��

	float					m_fCurPos[3];		//���� position
	float					m_fAbsPos[3];		//���� position(���� x�� min, y�� min, z�� max�� ���������Ͽ� ����� ==> m_nScreenPos �� secter�� ���ϴµ� ���)
	int						m_nScreenPos[2];	//display view���� screen��ǥ
	float					m_fOldPos[3];		//�ٷ����� position
	CMapData*				m_pCurMap;			//���� map
	_100_per_random_table	m_rtPer100;

	WORD					m_wMapLayerIndex;
	_object_list_point		m_SectorPoint;		//sector�� Object list�� �߰��� �ڽ��� pointer
	_object_list_point		m_SectorNetPoint;	//sector�� Object list�� �߰��� �ڽ��� pointer

	DWORD					m_dwOldTickStun;
	DWORD					m_dwOldTickBreakTranspar;
	bool					m_bBreakTranspar;

	static CGameObject*		s_pSelectObject;	//display view�󿡼� ���õ� object
	static int				s_nTotalObjectNum;
	static CGameObject*		s_pTotalObject[MAX_OBJ];
	
private:
	
	DWORD					m_dwCurSec;			//map���� ���� ��ġ�ϴ� sector index

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
	void CalcAbsPos();		//������ǥ�� ����Ѵ�.
	bool UpdateSecList();	//sector index �� sector list�� �߰� �� ������ update�Ѵ�. 
	void ResetSector(DWORD dwOldSec, DWORD dwNewSec, bool bWhole); //���Ͱ� ����������� ȣ��
	void SendMsg_BreakStop();
	DWORD  CalcSecIndex();	//������ǥ�� �̿��Ͽ� sector index�� ����.
	bool RerangeSecIndex(DWORD dwOld, DWORD dwNew, DWORD* OUT pdwAdj);	//
	bool IsInSector(CGameObject* pObj, int nRadius = -1);	//pObj�� �ڽ��� �þ߿� �����ϴ°�?
	DWORD GetCurSecNum();
	void  SetCurSecNum(DWORD dwNewSecNum);
	
	void SetStun(bool bStun);
	void SetBreakTranspar(bool bBreak);

//Screen View..
	void CalcScrNormalPoint(CRect* prcWnd);		//��� ���÷��̺���� ��ǥ��
	void CalcScrExtendPoint(CRect* prcWnd, CRect* prcExtend);	//Ȯ�� ���÷��̺���� ��ǥ��

	void CircleReport(BYTE* pbyType, char* szMsg, int nMsgSize, bool bToOne = false);
	void RangeCircleReport(DWORD dwRangeSecNum, BYTE* pbyType, char* szMsg, int nMsgSize, bool bToOne = false);

//virtual..
	virtual void Loop() {};
	virtual void LoopPerSec() {};
	virtual	int GetObjRace()	{	return -1;	}
	virtual void AlterSec() {};

	void SendMsg_RealFixPosition();
	virtual void SendMsg_FixPosition(int n) {}	//pDst���� �ڽ��� ���ִ� ��ġ�� �˸���.
	virtual void SendMsg_RealMovePoint(int n) {}	
	virtual void SendMsg_StunInform() {}	//stun������ ��ȭ�� �˸���.
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
	virtual int	AttackableHeight() {	return attack_able_height;	}	//���ݰ��ɳ���
	virtual int GetGenAttackProb(CCharacter* pDst, int nPart)	{	return 0;	}	//�Ϲݰ���Ȯ��
	virtual int GetLevel() { return 1; }
	virtual int GetDefSkill(){ return 1; }
	virtual bool IsRecvableContEffect() {	return true;	}
	virtual bool IsBeAttackedAble()	{	return false;	}
	virtual bool IsRewardExp()	{	return false;	}		//�ڱ⸦ �����ϸ� ����ġ ������ ���ٰ�����..

	//SF �Ͻ�ȿ��
	virtual bool F_AttHPtoDstFP_Once(CCharacter* pDstObj){	return false; }	// �ڽ��� HP�� ����� FP�� �����
	virtual bool F_ContDamageTimeInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// ���� ���غ��� ���� �ð� ���̱�
	virtual	bool F_Resurrect_Once(CCharacter* pDstObj){	return false; }		// ���� ������ ����Ѵ�.
	virtual bool F_HPInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// ���� HP����
	virtual bool F_STInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// ���� ST����
	virtual bool F_ContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// ���� �̵溸���� ���� �ð� �ø���	
	virtual bool F_OverHealing_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// overhealing
	virtual bool F_LateContHelpSkillRemove_Once(CCharacter* pDstObj){	return false; }	// ���� �ֱ� �����̵溸�� ��ų ����	
	virtual bool F_LateContHelpForceRemove_Once(CCharacter* pDstObj){	return false; }	// ���� �ֱ� �����̵溸�� ���� ����
	virtual bool F_LateContDamageForceRemove_Once(CCharacter* pDstObj){	return false; }	// ���� �ֱ� �������غ��� ���� ����
	virtual bool F_AllContHelpSkillRemove_Once(CCharacter* pDstObj){	return false; }	// ��� �ɷ��ִ� �����̵溸�� ��ų ����
	virtual bool F_AllContHelpForceRemove_Once(CCharacter* pDstObj){	return false; }	// ��� �ɷ��ִ� �����̵溸�� ���� ����
	virtual bool F_AllContDamageForceRemove_Once(CCharacter* pDstObj){	return false; }	// ��� �ɷ��ִ� ���� ���� ���� ���� ����
	virtual bool F_OthersContHelpSFRemove_Once(float fEffectValue){	return false; }	// ������ ���� �ٸ� �������� ���� �̵� ���� ��ų & ���� ����
	virtual bool S_SkillContHelpTimeInc_Once(CCharacter* pDstObj, float fEffectValue){	return false; }	// skill�� ���� �������� �̵� ���� �ð� ���̱�
	virtual bool S_ConvertMonsterTarget(CCharacter* pDstObj, float fEffectValue){	return false; }
	virtual bool S_TransMonsterHP(CCharacter* pDstObj, float fEffectValue){	return false; }
	virtual bool S_ReleaseMonsterTarget(CCharacter* pDstObj, float fEffectValue){	return false; }


};

extern const double g_pi;

#endif 
