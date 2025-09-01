#include "stdafx.h"
#include "EconomySystem.h"
#include "MyUtil.h"
#include "MainThread.h"

#define		__MIN			60000
#define		__HOUR			3600000
#define		__STD_USER		100		//�ֱ⸶�� ������ �ź� ��� 100���� �̻��̿��� �����Ѵ�.

#define		DEF_EXCHANGE_RATE			1000

static _ECONOMY_SYSTEM				e_EconomySystem;
static _economy_history_data		e_EconomyHistory[MAX_ECONOMY_HISTORY];
static int					e_nCurMgrValue = 1000;
static int					e_nNextMgrValue = 1000;

static DWORD				e_dwMinCount = 0;
static DWORD				e_dwUserCumCount[RACE_NUM] = {0,};

//����ó�� �����Ҷ� DB���� �о�� ���ű���� �������� ����..
void eInitEconomySystem(int nCurMgrValue, int nNextMgrValue, _economy_history_data* pData, int nHisNum, double* pdLastDalant, double* pdLastGold)
{
	if(e_EconomySystem.m_bLoad)
		return;

	if(nHisNum < MAX_ECONOMY_HISTORY)
	{
		::MyMessageBox("Economy Error", "�����丮��%d�� %d���� ����", nHisNum, MAX_ECONOMY_HISTORY);
		return;
	}

	//�����ý��� �ʱ�ȭ..
	e_EconomySystem.Init();
	e_EconomySystem.m_dwLastDate = ::eGetLocalDate();

	e_nCurMgrValue = nCurMgrValue;
	e_nNextMgrValue = nNextMgrValue;

	//�����丮 �ʱ�ȭ
	for(int r = 0; r < RACE_NUM; r++)
		e_EconomyHistory[r].Init();

	//�����丮 ����
	for(int h = 0; h < nHisNum; h++)
	{
		//������ǥ���
		_economy_calc_data Data;

		for(r = 0; r < RACE_NUM; r++)
		{
			Data.dTradeDalant[r] = pData[h].dTradeDalant[r];	
			Data.dTradeGold[r] = 1;//pData[h].dTradeGold[r];			//��带 �������� �ʴ´�..
		}

		//���
		_UpdateNewEconomy(&Data);

		//����..
		for(r = 0; r < RACE_NUM; r++)		
		{
			e_EconomyHistory[h].dTradeDalant[r] = Data.dTradeDalant[r];
			e_EconomyHistory[h].dTradeGold[r] = Data.dTradeGold[r];
			e_EconomyHistory[h].wEconomyGuide[r] = Data.wEconomyGuide[r];
		}
	}

	//����ð� ����
	e_EconomySystem.m_byCurHour = ::GetCurrentHour();

	//�����������丮 ���ʷ� ���� ������ǥ �ۼ�
	_economy_calc_data Data;
	for(r = 0; r < RACE_NUM; r++)
	{
		Data.dTradeDalant[r] = pData[MAX_ECONOMY_HISTORY-1].dTradeDalant[r];
		Data.dTradeGold[r] = 1;//pData[MAX_ECONOMY_HISTORY-1].dTradeGold[r];			//��带 �������� �ʴ´�..
	}

	//���
	_UpdateNewEconomy(&Data);

	//����..
	for(r = 0; r < RACE_NUM; r++)
	{
		//������ǥ
		e_EconomySystem.m_CurRate[r].dOldTradeDalant = Data.dTradeDalant[r];
		e_EconomySystem.m_CurRate[r].dOldTradeGold = Data.dTradeGold[r];
		e_EconomySystem.m_CurRate[r].fPayExgRate = Data.fPayExgRate[r];
		e_EconomySystem.m_CurRate[r].fTexRate = Data.fTexRate[r];
		e_EconomySystem.m_CurRate[r].wEconomyGuide = Data.wEconomyGuide[r];

		//��������� ������ȭ��
		e_EconomySystem.m_dCurTradeDalant[r] = pdLastDalant[r];
		e_EconomySystem.m_dCurTradeGold[r] = pdLastGold[r];
		e_EconomySystem.m_dBufTradeGold[r] = 0;	
		e_EconomySystem.m_dBufTradeDalant[r] = 0;
	}

	e_EconomySystem.m_bLoad = true;
}

//���η������� �ð����� ������Ʈ
void eUpdateEconomySystem()	//������Ʈ�Ǹ� true ����.. DB Insert..
{
	int r;
	if(!e_EconomySystem.m_bLoad)
		return;

	DWORD dwCurTime = timeGetTime();	//1�и��� üũ�ؼ� .. �� ���ؽð��� �ƴ��� üũ�Ѵ�..

	int nGap = dwCurTime - e_EconomySystem.m_dwLastUpdateTime;	//event timer
	if(nGap < __MIN && nGap >= 0)
		return;
	else
		e_EconomySystem.m_dwLastUpdateTime = dwCurTime;

	//�����ο�
	e_dwMinCount++;
	for(r = 0; r < RACE_NUM; r++)
		e_dwUserCumCount[r] += CPlayer::s_nRaceNum[r];

	//�ð�üũ
	BYTE byBufferHour = ::GetCurrentHour();

	if(e_EconomySystem.m_byCurHour == byBufferHour)
		return;	//�ð��� �ٲ���� DB�� ������Ŵ
	e_EconomySystem.m_byCurHour = byBufferHour;

	//��¥üũ
	DWORD dwCurDate = ::eGetLocalDate();

	int nAvgUseNum[RACE_NUM] = {e_dwUserCumCount[0]/e_dwMinCount, e_dwUserCumCount[1]/e_dwMinCount, e_dwUserCumCount[2]/e_dwMinCount};
	e_dwMinCount = 0;
	memset(e_dwUserCumCount, 0, sizeof(e_dwUserCumCount));

	//�ð��� ����ȰŶ�� ������Ű��..
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
		//�����ͺ��̽����..
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
		return;	//��¥�� �ٲ���� ������ǥ ���� ����

	e_EconomySystem.m_dwLastDate = dwCurDate;

	//���ο� ������ ����
	e_nCurMgrValue = e_nNextMgrValue;	

	//update..
	//�Ķ���� ����ü ����
	_economy_calc_data Data;
	for(r = 0; r < RACE_NUM; r++)
	{
		Data.dTradeDalant[r] = e_EconomySystem.m_dCurTradeDalant[r];
		Data.dTradeGold[r] = 1;//e_EconomySystem.m_dCurTradeGold[r];	//���� �������� �ʴ´�..
	}

	//���
	_UpdateNewEconomy(&Data);

	//����..
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

	//�����丮 ���..
		//0���� 1������ �з� ���´�
	memcpy(&e_EconomyHistory[0], &e_EconomyHistory[1], sizeof(_economy_history_data)*(MAX_ECONOMY_HISTORY-1));
		//�ֱٲ� ����
	memcpy(&e_EconomyHistory[MAX_ECONOMY_HISTORY-1], &LastHistory, sizeof(_economy_history_data));

	//��������鿡�� �˸���..
	_UpdateRateSendToAllPlayer();

	e_EconomySystem.CurTradeMoneyInit();
}

//�������� �����ŷ��� ������ ��带 �߰�
void eAddGold(int nRaceCode, int nAdd)
{
	e_EconomySystem.m_dBufTradeGold[nRaceCode] += nAdd;
}

//�������� �����ŷ��� ������ �޶�Ʈ�� �߰�
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

//���� ����
float eGetTex(int nRaceCode)
{
	return e_EconomySystem.m_CurRate[nRaceCode].fTexRate;
}

	//���� ����ȯ��
int eGetRate(int nRaceCode)
{
	return (int)e_EconomySystem.m_CurRate[nRaceCode].fPayExgRate;
}

//���� ������ǥ
WORD eGetGuide(int nRaceCode)
{
	return e_EconomySystem.m_CurRate[nRaceCode].wEconomyGuide;
}

//���� ��� ��ȭ��
double eGetOldGold(int nRaceCode)
{
	return e_EconomySystem.m_CurRate[nRaceCode].dOldTradeGold;
}

//���� �޶�Ʈ ��ȭ��
double eGetOldDalant(int nRaceCode)
{
	return e_EconomySystem.m_CurRate[nRaceCode].dOldTradeDalant;
}	

//���� ���������� ��������
void eSetNextMgrValue(int nNextMgrValue)
{
	e_nNextMgrValue = nNextMgrValue;

#ifdef __SERVICE
		
	//�����ͺ��̽����..
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

//���� 12�� ������ǥ
_economy_history_data* eGetGuideHistory()
{
	return e_EconomyHistory;
}

//������ǥ ���
void _UpdateNewEconomy(_economy_calc_data* pData)
{
	float		fStandardGold[RACE_NUM];

	//������ ��հŷ��� ȯ������ ���
	float fAvrTrade[RACE_NUM];
	for(int r = 0; r < RACE_NUM; r++)
	{
		fAvrTrade[r] = sqrt(pData->dTradeGold[r] + pData->dTradeDalant[r]/DEF_EXCHANGE_RATE + 1);
		if(fAvrTrade[r] < 1.0f)
			fAvrTrade[r] = 1.0f;	//1�̻������ؼ� fPayExgRate �� 0�� �ȳ���..
	}

	//������ ����ȯ�� ���..
	for(r = 0; r < RACE_NUM; r++)
	{
		//������ ����ȯ��
		pData->fPayExgRate[r] = _CalcPayExgRatePerRace(fAvrTrade, r);			

		//������ ������
		fStandardGold[r] = pData->dTradeGold[r] + (pData->dTradeDalant[r]/pData->fPayExgRate[r]);		
		if(fStandardGold[r] < 1.0f)
			fStandardGold[r] = 1.0f;
	}

	//������հ�����
	float fAvgStandardGold = (float)(fStandardGold[0] + fStandardGold[1] + fStandardGold[2])/RACE_NUM;

	if(fAvgStandardGold < 1.0f)
		fAvgStandardGold = 1.0f;

	//������ ���� ���..
	for(r = 0; r < RACE_NUM; r++)
	{
		//������ ������ǥ
		pData->wEconomyGuide[r] = fStandardGold[r] / fAvgStandardGold * 100 + 0.5f;	//0.5�� �ݿø�..

		//������ ����
		pData->fTexRate[r] = fStandardGold[r] / fAvgStandardGold * 0.1f;//..
	}			
}

//����ȯ�� ���
int _CalcPayExgRatePerRace(float* pfAvrTradeMoney, int nRaceCode)
{
	float r0 = pfAvrTradeMoney[nRaceCode];
	float r1 = pfAvrTradeMoney[(nRaceCode+1)%RACE_NUM];
	float r2 = pfAvrTradeMoney[(nRaceCode+2)%RACE_NUM];

	//������..
	int nR;
	if(r0+r1+r2 == 0.0f)
		nR = 0;
	else
		nR = ((r1+r2)*0.5) / (r0+r1+r2) * 3000 + e_nCurMgrValue;

	if(nR < 1)
		nR = 1;

	return nR;
}

//������Ʈ�� ��� �����鿡�� �˸�..
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