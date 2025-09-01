#ifndef _ECONOMY_SYSTEM_H_
#define _ECONOMY_SYSTEM_H_

#include "CharacterDB.h"
#include <math.h>

struct _economy_history_data
{
	double	dTradeGold[RACE_NUM];			//�Ѱŷ����
	double	dTradeDalant[RACE_NUM];			//�Ѱŷ��޶�Ʈ
	WORD	wEconomyGuide[RACE_NUM];		//������ǥ

	_economy_history_data()	{	Init();	}

	void Init()	
	{	
		for(int r = 0; r < RACE_NUM; r++)
		{
			dTradeGold[r] = 1;			//�Ѱŷ����
			dTradeDalant[r] = 1;			//�Ѱŷ��޶�Ʈ
			wEconomyGuide[r] = 100;		//������ǥ
		}
	}
};

struct _economy_calc_data
{
	double	dTradeGold[RACE_NUM];			//�Ѱŷ����
	double	dTradeDalant[RACE_NUM];			//�Ѱŷ��޶�Ʈ
	float	fPayExgRate[RACE_NUM];
	float	fTexRate[RACE_NUM];
	WORD	wEconomyGuide[RACE_NUM];		//������ǥ
};

struct _ECONOMY_SYSTEM
{	
	bool	m_bLoad;
	double	m_dCurTradeGold[RACE_NUM];				//�����Ѱŷ����
	double	m_dCurTradeDalant[RACE_NUM];			//�����Ѱŷ��޶�Ʈ
	double	m_dBufTradeGold[RACE_NUM];				//�����߰��ŷ����
	double	m_dBufTradeDalant[RACE_NUM];			//�����߰��ŷ��޶�Ʈ

	struct _CUR_RATE
	{
		float	fPayExgRate;			//����ȯ��		(����)	//0.1 ~ 3000
		float	fTexRate;				//����			(����)	//0.0001 ~ 0.3
		WORD	wEconomyGuide;			//������ǥ
		double	dOldTradeGold;				//�����Ѱŷ����
		double	dOldTradeDalant;			//�����Ѱŷ��޶�Ʈ
	};

	_CUR_RATE	m_CurRate[RACE_NUM];

	BYTE		m_byCurHour;
	DWORD		m_dwLastUpdateTime;
	DWORD		m_dwSystemOperStartTime;	//�ý��� �����ð�..(���������ð�..)
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

