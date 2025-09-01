#include "stdafx.h"
#include "EconomySystem.h"
#include "MyUtil.h"
#include "MainThread.h"

#define		__MIN			60000
#define		__HOUR			3600000
#define		__STD_USER		100		//주기마다 전종족 매분 평균 100명이 이상이여만 적용한다.

#define		DEF_EXCHANGE_RATE			1000

static _ECONOMY_SYSTEM				e_EconomySystem;
static _economy_history_data		e_EconomyHistory[MAX_ECONOMY_HISTORY];
static int					e_nCurMgrValue = 1000;
static int					e_nNextMgrValue = 1000;

static DWORD				e_dwMinCount = 0;
static DWORD				e_dwUserCumCount[RACE_NUM] = {0,};

//서버처음 시작할때 DB에서 읽어온 과거기록을 바탕으로 세팅..
void eInitEconomySystem(int nCurMgrValue, int nNextMgrValue, _economy_history_data* pData, int nHisNum, double* pdLastDalant, double* pdLastGold)
{
	if(e_EconomySystem.m_bLoad)
		return;

	if(nHisNum < MAX_ECONOMY_HISTORY)
	{
		::MyMessageBox("Economy Error", "히스토리수%d가 %d보다 적다", nHisNum, MAX_ECONOMY_HISTORY);
		return;
	}

	//경제시스템 초기화..
	e_EconomySystem.Init();
	e_EconomySystem.m_dwLastDate = ::eGetLocalDate();

	e_nCurMgrValue = nCurMgrValue;
	e_nNextMgrValue = nNextMgrValue;

	//히스토리 초기화
	for(int r = 0; r < RACE_NUM; r++)
		e_EconomyHistory[r].Init();

	//히스토리 세팅
	for(int h = 0; h < nHisNum; h++)
	{
		//경제지표계산
		_economy_calc_data Data;

		for(r = 0; r < RACE_NUM; r++)
		{
			Data.dTradeDalant[r] = pData[h].dTradeDalant[r];	
			Data.dTradeGold[r] = 1;//pData[h].dTradeGold[r];			//골드를 적용하지 않는다..
		}

		//계산
		_UpdateNewEconomy(&Data);

		//세팅..
		for(r = 0; r < RACE_NUM; r++)		
		{
			e_EconomyHistory[h].dTradeDalant[r] = Data.dTradeDalant[r];
			e_EconomyHistory[h].dTradeGold[r] = Data.dTradeGold[r];
			e_EconomyHistory[h].wEconomyGuide[r] = Data.wEconomyGuide[r];
		}
	}

	//현재시간 세팅
	e_EconomySystem.m_byCurHour = ::GetCurrentHour();

	//마지막히스토리 기초로 현재 경제지표 작성
	_economy_calc_data Data;
	for(r = 0; r < RACE_NUM; r++)
	{
		Data.dTradeDalant[r] = pData[MAX_ECONOMY_HISTORY-1].dTradeDalant[r];
		Data.dTradeGold[r] = 1;//pData[MAX_ECONOMY_HISTORY-1].dTradeGold[r];			//골드를 적용하지 않는다..
	}

	//계산
	_UpdateNewEconomy(&Data);

	//세팅..
	for(r = 0; r < RACE_NUM; r++)
	{
		//현재지표
		e_EconomySystem.m_CurRate[r].dOldTradeDalant = Data.dTradeDalant[r];
		e_EconomySystem.m_CurRate[r].dOldTradeGold = Data.dTradeGold[r];
		e_EconomySystem.m_CurRate[r].fPayExgRate = Data.fPayExgRate[r];
		e_EconomySystem.m_CurRate[r].fTexRate = Data.fTexRate[r];
		e_EconomySystem.m_CurRate[r].wEconomyGuide = Data.wEconomyGuide[r];

		//현재까지의 누적통화량
		e_EconomySystem.m_dCurTradeDalant[r] = pdLastDalant[r];
		e_EconomySystem.m_dCurTradeGold[r] = pdLastGold[r];
		e_EconomySystem.m_dBufTradeGold[r] = 0;	
		e_EconomySystem.m_dBufTradeDalant[r] = 0;
	}

	e_EconomySystem.m_bLoad = true;
}

//메인루프에서 시간마다 업데이트
void eUpdateEconomySystem()	//업데이트되면 true 리턴.. DB Insert..
{
	int r;
	if(!e_EconomySystem.m_bLoad)
		return;

	DWORD dwCurTime = timeGetTime();	//1분마다 체크해서 .. 새 기준시간이 됐는지 체크한다..

	int nGap = dwCurTime - e_EconomySystem.m_dwLastUpdateTime;	//event timer
	if(nGap < __MIN && nGap >= 0)
		return;
	else
		e_EconomySystem.m_dwLastUpdateTime = dwCurTime;

	//누적인원
	e_dwMinCount++;
	for(r = 0; r < RACE_NUM; r++)
		e_dwUserCumCount[r] += CPlayer::s_nRaceNum[r];

	//시간체크
	BYTE byBufferHour = ::GetCurrentHour();

	if(e_EconomySystem.m_byCurHour == byBufferHour)
		return;	//시간이 바뀌였으면 DB에 누적시킴
	e_EconomySystem.m_byCurHour = byBufferHour;

	//날짜체크
	DWORD dwCurDate = ::eGetLocalDate();

	int nAvgUseNum[RACE_NUM] = {e_dwUserCumCount[0]/e_dwMinCount, e_dwUserCumCount[1]/e_dwMinCount, e_dwUserCumCount[2]/e_dwMinCount};
	e_dwMinCount = 0;
	memset(e_dwUserCumCount, 0, sizeof(e_dwUserCumCount));

	//시간만 변경된거라면 누적시키고..
	for(r = 0; r < RACE_NUM; r++)
	{
		if(nAvgUseNum[0] > __STD_USER && nAvgUseNum[1] > __STD_USER && nAvgUseNum[2] > __STD_USER)
		{
			e_EconomySystem.m_dCurTradeDalant[r] += e_EconomySystem.m_dBufTradeDalant[r];
			if(e_EconomySystem.m_dCurTradeDalant[r] < 1)
				e_EconomySystem.m_dCurTradeDalant[r] = 1;

			e_EconomySystem.m_dCurTradeGold[r] += e_EconomySystem.m_dBufTradeGold[r];
			if(e_EconomySystem.m_dCurTradeGold[r] < 1)
				e_EconomySystem.m_dCurTradeGold[r] = 1;
		}
		e_EconomySystem.m_dBufTradeDalant[r] = 0;
		e_EconomySystem.m_dBufTradeGold[r] = 0;
	}

#ifdef __SERVICE
		
	if(nAvgUseNum[0] > __STD_USER && nAvgUseNum[1] > __STD_USER && nAvgUseNum[2] > __STD_USER)
	{
		//데이터베이스기록..
		_log_sheet_economy Sheet;

		Sheet.dwDate = dwCurDate;
		memcpy(Sheet.dTradeDalant, e_EconomySystem.m_dCurTradeDalant, sizeof(double)*RACE_NUM);
		memcpy(Sheet.dTradeGold, e_EconomySystem.m_dCurTradeGold, sizeof(double)*RACE_NUM);
		if(dwCurDate == e_EconomySystem.m_dwLastDate)
			Sheet.nMgrValue = e_nCurMgrValue;
		else
			Sheet.nMgrValue = e_nNextMgrValue;

		g_Main.PushDQSData(0xFFFFFFFF, NULL, _DB_QRY_SYN_DATA::log_case_economy, (char*)&Sheet, Sheet.size());
	}

#endif
	
	if(dwCurDate == e_EconomySystem.m_dwLastDate)
		return;	//날짜가 바뀌였으면 현재지표 새로 적용

	e_EconomySystem.m_dwLastDate = dwCurDate;

	//새로운 관리값 적용
	e_nCurMgrValue = e_nNextMgrValue;	

	//update..
	//파라메터 구조체 세팅
	_economy_calc_data Data;
	for(r = 0; r < RACE_NUM; r++)
	{
		Data.dTradeDalant[r] = e_EconomySystem.m_dCurTradeDalant[r];
		Data.dTradeGold[r] = 1;//e_EconomySystem.m_dCurTradeGold[r];	//골드는 적용하지 않는다..
	}

	//계산
	_UpdateNewEconomy(&Data);

	//세팅..
	for(r = 0; r < RACE_NUM; r++)
	{
		e_EconomySystem.m_CurRate[r].dOldTradeDalant = Data.dTradeDalant[r];
		e_EconomySystem.m_CurRate[r].dOldTradeGold = Data.dTradeGold[r];
		e_EconomySystem.m_CurRate[r].fPayExgRate = Data.fPayExgRate[r];
		e_EconomySystem.m_CurRate[r].fTexRate = Data.fTexRate[r];
		e_EconomySystem.m_CurRate[r].wEconomyGuide = Data.wEconomyGuide[r];
	}

	_economy_history_data LastHistory;

	memcpy(LastHistory.dTradeGold, e_EconomySystem.m_dCurTradeGold, sizeof(double)*RACE_NUM);
	memcpy(LastHistory.dTradeDalant, e_EconomySystem.m_dCurTradeDalant, sizeof(double)*RACE_NUM);
	for(r = 0; r < RACE_NUM; r++)
		LastHistory.wEconomyGuide[r] = e_EconomySystem.m_CurRate[r].wEconomyGuide;

	//히스토리 기록..
		//0번을 1번부터 밀려 적는다
	memcpy(&e_EconomyHistory[0], &e_EconomyHistory[1], sizeof(_economy_history_data)*(MAX_ECONOMY_HISTORY-1));
		//최근꺼 세팅
	memcpy(&e_EconomyHistory[MAX_ECONOMY_HISTORY-1], &LastHistory, sizeof(_economy_history_data));

	//모든유저들에게 알리기..
	_UpdateRateSendToAllPlayer();

	e_EconomySystem.CurTradeMoneyInit();
}

//유저들의 상점거래시 매출입 골드를 추가
void eAddGold(int nRaceCode, int nAdd)
{
	e_EconomySystem.m_dBufTradeGold[nRaceCode] += nAdd;
}

//유저들의 상점거래시 매출입 달란트를 추가
void eAddDalant(int nRaceCode, int nAdd)
{
	e_EconomySystem.m_dBufTradeDalant[nRaceCode] += nAdd;
}

double eGetGold(int nRaceCode)
{
	return e_EconomySystem.m_dCurTradeGold[nRaceCode] + e_EconomySystem.m_dBufTradeGold[nRaceCode];
}

double eGetDalant(int nRaceCode)
{
	return 	e_EconomySystem.m_dCurTradeDalant[nRaceCode] + e_EconomySystem.m_dBufTradeDalant[nRaceCode];
}

//현재 세율
float eGetTex(int nRaceCode)
{
	return e_EconomySystem.m_CurRate[nRaceCode].fTexRate;
}

	//현재 지급환율
int eGetRate(int nRaceCode)
{
	return (int)e_EconomySystem.m_CurRate[nRaceCode].fPayExgRate;
}

//현재 경제지표
WORD eGetGuide(int nRaceCode)
{
	return e_EconomySystem.m_CurRate[nRaceCode].wEconomyGuide;
}

//과거 골드 통화량
double eGetOldGold(int nRaceCode)
{
	return e_EconomySystem.m_CurRate[nRaceCode].dOldTradeGold;
}

//과거 달란트 통화량
double eGetOldDalant(int nRaceCode)
{
	return e_EconomySystem.m_CurRate[nRaceCode].dOldTradeDalant;
}	

//다음 경제관리값 임의적용
void eSetNextMgrValue(int nNextMgrValue)
{
	e_nNextMgrValue = nNextMgrValue;

#ifdef __SERVICE
		
	//데이터베이스기록..
	_log_sheet_economy Sheet;

	Sheet.dwDate = e_EconomySystem.m_dwLastDate;
	memcpy(Sheet.dTradeDalant, e_EconomySystem.m_dCurTradeDalant, sizeof(double)*RACE_NUM);
	memcpy(Sheet.dTradeGold, e_EconomySystem.m_dCurTradeGold, sizeof(double)*RACE_NUM);
	Sheet.nMgrValue = nNextMgrValue;

	g_Main.PushDQSData(0xFFFFFFFF, NULL, _DB_QRY_SYN_DATA::log_case_economy, (char*)&Sheet, Sheet.size());

#endif

}

int	eGetNextMgrValue()
{
	return e_nNextMgrValue;
}

int eGetMgrValue()
{
	return e_nCurMgrValue;
}

//과거 12개 경제지표
_economy_history_data* eGetGuideHistory()
{
	return e_EconomyHistory;
}

//경제지표 계산
void _UpdateNewEconomy(_economy_calc_data* pData)
{
	float		fStandardGold[RACE_NUM];

	//종족별 평균거래량 환산지수 계산
	float fAvrTrade[RACE_NUM];
	for(int r = 0; r < RACE_NUM; r++)
	{
		fAvrTrade[r] = sqrt(pData->dTradeGold[r] + pData->dTradeDalant[r]/DEF_EXCHANGE_RATE + 1);
		if(fAvrTrade[r] < 1.0f)
			fAvrTrade[r] = 1.0f;	//1이상으로해서 fPayExgRate 이 0이 안나옴..
	}

	//종족별 지급환율 계산..
	for(r = 0; r < RACE_NUM; r++)
	{
		//종족별 지급환율
		pData->fPayExgRate[r] = _CalcPayExgRatePerRace(fAvrTrade, r);			

		//종족별 골드기준
		fStandardGold[r] = pData->dTradeGold[r] + (pData->dTradeDalant[r]/pData->fPayExgRate[r]);		
		if(fStandardGold[r] < 1.0f)
			fStandardGold[r] = 1.0f;
	}

	//종족평균골드기준
	float fAvgStandardGold = (float)(fStandardGold[0] + fStandardGold[1] + fStandardGold[2])/RACE_NUM;

	if(fAvgStandardGold < 1.0f)
		fAvgStandardGold = 1.0f;

	//종족별 세율 계산..
	for(r = 0; r < RACE_NUM; r++)
	{
		//종족별 경제지표
		pData->wEconomyGuide[r] = fStandardGold[r] / fAvgStandardGold * 100 + 0.5f;	//0.5는 반올림..

		//종족별 세율
		pData->fTexRate[r] = fStandardGold[r] / fAvgStandardGold * 0.1f;//..
	}			
}

//지급환율 계산
int _CalcPayExgRatePerRace(float* pfAvrTradeMoney, int nRaceCode)
{
	float r0 = pfAvrTradeMoney[nRaceCode];
	float r1 = pfAvrTradeMoney[(nRaceCode+1)%RACE_NUM];
	float r2 = pfAvrTradeMoney[(nRaceCode+2)%RACE_NUM];

	//최종값..
	int nR;
	if(r0+r1+r2 == 0.0f)
		nR = 0;
	else
		nR = ((r1+r2)*0.5) / (r0+r1+r2) * 3000 + e_nCurMgrValue;

	if(nR < 1)
		nR = 1;

	return nR;
}

//업데이트시 모든 유저들에게 알림..
void _UpdateRateSendToAllPlayer()
{
	for(int i = 0; i < MAX_PLAYER; i++)
	{
		if(!g_Player[i].m_bLive)
			continue;

		g_Player[i].SendMsg_EconomyRateInform(false);
	}
}

DWORD eGetLocalDate()
{
	DWORD Y = GetCurrentYear();
	DWORD M = GetCurrentMonth();
	DWORD D = GetCurrentDay();

	return Y*10000 + M*100 + D;	
}