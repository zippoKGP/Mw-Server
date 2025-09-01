#ifndef _ECONOMY_SYSTEM_H_
#define _ECONOMY_SYSTEM_H_

#include "CharacterDB.h"
#include <math.h>

struct _economy_history_data
{
	double	dTradeGold[RACE_NUM];			//총거래골드
	double	dTradeDalant[RACE_NUM];			//총거래달란트
	WORD	wEconomyGuide[RACE_NUM];		//경제지표

	_economy_history_data()	{	Init();	}

	void Init()	
	{	
		for(int r = 0; r < RACE_NUM; r++)
		{
			dTradeGold[r] = 1;			//총거래골드
			dTradeDalant[r] = 1;			//총거래달란트
			wEconomyGuide[r] = 100;		//경제지표
		}
	}
};

struct _economy_calc_data
{
	double	dTradeGold[RACE_NUM];			//총거래골드
	double	dTradeDalant[RACE_NUM];			//총거래달란트
	float	fPayExgRate[RACE_NUM];
	float	fTexRate[RACE_NUM];
	WORD	wEconomyGuide[RACE_NUM];		//경제지표
};

struct _ECONOMY_SYSTEM
{	
	bool	m_bLoad;
	double	m_dCurTradeGold[RACE_NUM];				//현재총거래골드
	double	m_dCurTradeDalant[RACE_NUM];			//현재총거래달란트
	double	m_dBufTradeGold[RACE_NUM];				//현재중간거래골드
	double	m_dBufTradeDalant[RACE_NUM];			//현재중간거래달란트

	struct _CUR_RATE
	{
		float	fPayExgRate;			//지급환율		(적용)	//0.1 ~ 3000
		float	fTexRate;				//세율			(적용)	//0.0001 ~ 0.3
		WORD	wEconomyGuide;			//경제지표
		double	dOldTradeGold;				//현재총거래골드
		double	dOldTradeDalant;			//현재총거래달란트
	};

	_CUR_RATE	m_CurRate[RACE_NUM];

	BYTE		m_byCurHour;
	DWORD		m_dwLastUpdateTime;
	DWORD		m_dwSystemOperStartTime;	//시스템 가동시간..(서버열린시간..)
	DWORD		m_dwLastDate;

	_ECONOMY_SYSTEM()	{	Init();	}

	void Init()
	{
		m_bLoad = false;
		CurTradeMoneyInit();		
		m_dwLastUpdateTime = timeGetTime();
		m_dwSystemOperStartTime = timeGetTime();
	}

	void CurTradeMoneyInit()
	{
		for(int i = 0; i < RACE_NUM; i++)
		{
			m_dCurTradeGold[i] = 1;
			m_dCurTradeDalant[i] = 1;
		}
	}
};

void	eInitEconomySystem(int nCurMgrValue, int nNextMgrValue, _economy_history_data* pData, int nHisNum, double* pdLastDalant, double* pdLastGold);
void	eUpdateEconomySystem();
void	eAddGold(int nRaceCode, int nAdd);
void	eAddDalant(int nRaceCode, int nAdd);
double	eGetGold(int nRaceCode);
double	eGetDalant(int nRaceCode);
float	eGetTex(int nRaceCode);
int		eGetRate(int nRaceCode);
WORD	eGetGuide(int nRaceCode);
double	eGetOldGold(int nRaceCode);
double	eGetOldDalant(int nRaceCode);
_economy_history_data*	eGetGuideHistory();
void    eSetNextMgrValue(int nNextMgrValue);
int		eGetNextMgrValue();
int		eGetMgrValue();

void	_UpdateNewEconomy(_economy_calc_data* pData);
int		_CalcPayExgRatePerRace(float* pfAvrTradeMoney, int nRaceCode);
void	_UpdateRateSendToAllPlayer();

DWORD	eGetLocalDate();

#endif

