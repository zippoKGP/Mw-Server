// MapOperate.h: interface for the CMapOperate class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MapOperate_h_
#define _MapOperate_h_

#include "MapData.h"
#include "R3EngineKernel.h"
#include "MyClassUtil.h"
#include <mmsystem.h>
#include "CharacterDB.h"
#include "NetUtil.h"
#include "Character.h"

class CMapOperation  
{
public:

	enum	{	obj_loop_term = 50,
				obj_loop_unit = 10    };
	enum	{	real_fixpos_term	= 2000	};	//��������ġ�� �ǽð����� �˸��� ��..
	enum	{	mon_num_check_term	= 10000	};	//������ �ѷ��� ���ͼ��� �����ϴ� üũ ��..

	int			m_nLoopStartPoint;	//0~obj_loop_unit-1����..
	DWORD		m_dwSpeedHackStdTime;
	DWORD		m_dwLastTimeMonNumCheck;

	int			m_nMapNum;
	int			m_nStdMapNum;
	CMapData*	m_Map;

	bool		m_bReSpawnMonster;	//���� ���

	CMyTimer	m_tmrObjTerm;
	CMyTimer	m_tmrRealFixposTerm;
	CMyTimer	m_tmrRecover;	//ȸ��Ÿ�̸�

public:

	CMapOperation();
	virtual ~CMapOperation();

	bool Init();
	void OnLoop();

	CMapData* GetMap(int nIndex);
	int GetMap(CMapData* pMap);
	CMapData* GetMap(char* szMapCode);
	bool NearObjToObj(CGameObject* pOne, CGameObject* pOther, int nRadius);
	CMapData* GetStartMap(BYTE byRaceCode);
	CMapData* GetPosStartMap(BYTE byRaceCode, float* pfoutPos);

private:

	bool LoadMaps();
	void RespawnMonster();
	void CheckMapPortalLink();

};

#endif 
