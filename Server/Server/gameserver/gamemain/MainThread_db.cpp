#include "StdAfx.h"
#include "MainThread.h"
#include "MyUtil.h"

BYTE CMainThread::db_Reged_Avator(DWORD dwAccountSerial, _REGED* OUT pRegedList)
{
	_worlddb_character_base_info_array dbCharacterDataArray;
	memset( &dbCharacterDataArray, 0, sizeof(_worlddb_character_base_info_array) );
	

	BYTE byDBRet = m_pWorldDB->Select_CharacterBaseInfoBySerial( dwAccountSerial, &dbCharacterDataArray );
	if ( byDBRet == db_result_sql_error )
		return RET_CODE_DB_QUERY_ERR;

	for ( int i=0; i<dbCharacterDataArray.wCharacterCount; i++)
	{
		BYTE bySlot = dbCharacterDataArray.CharacterInfo[i].bySlotIndex;
		if (bySlot > MAX_CHAR_PER_WORLDUSER)
		{
			return RET_CODE_DB_QUERY_ERR;
		}

		pRegedList[bySlot].m_bySlotIndex = bySlot;
		
		pRegedList[bySlot].m_dwRecordNum = dbCharacterDataArray.CharacterInfo[i].dwSerial;	
		strcpy(pRegedList[bySlot].m_szAvatorName, dbCharacterDataArray.CharacterInfo[i].szName);					
		pRegedList[bySlot].m_byRaceSexCode = dbCharacterDataArray.CharacterInfo[i].byRace;
		strcpy(pRegedList[bySlot].m_szClassCode, dbCharacterDataArray.CharacterInfo[i].szClassCode);					
		pRegedList[bySlot].m_byLevel = dbCharacterDataArray.CharacterInfo[i].byLevel;
		pRegedList[bySlot].m_dwDalant = dbCharacterDataArray.CharacterInfo[i].dwDalant;
		pRegedList[bySlot].m_dwGold = dbCharacterDataArray.CharacterInfo[i].dwGold;
		pRegedList[bySlot].m_dwBaseShape = dbCharacterDataArray.CharacterInfo[i].dwBaseShape;
		pRegedList[bySlot].m_dwLastConnTime = dbCharacterDataArray.CharacterInfo[i].dwLastConnTime;

		for(int e = 0; e < equip_fix_num; e++)
		{
			pRegedList[bySlot].m_EquipKey[e].LoadDBKey(dbCharacterDataArray.CharacterInfo[i].shEKArray[e]);
			pRegedList[bySlot].m_dwFixEquipLv[e] = dbCharacterDataArray.CharacterInfo[i].dwEUArray[e];
		}

	}
	

	return RET_CODE_SUCCESS;
}

BYTE CMainThread::db_Insert_Avator(DWORD dwAccountSerial, char* pszAccount, _REGED_AVATOR_DB* pCharDB, DWORD* OUT pdwAvatorSerial)
{
	char szSql[1280] = {0,};
	DWORD dwSerial= 0xFFFFFFFF;
	*pdwAvatorSerial = dwSerial;

	if ( m_pWorldDB->Select_Equal_Name( pCharDB->m_szAvatorName ) == false )
	{
		return RET_CODE_REGED_AVATOR;
	}

	if ( m_pWorldDB->Insert_CharacterData( pCharDB->m_szAvatorName, pCharDB->m_szClassCode, dwAccountSerial, pszAccount, pCharDB->m_bySlotIndex, pCharDB->m_byRaceSexCode, pCharDB->m_dwBaseShape, m_MapOper.GetStartMap(pCharDB->m_byRaceSexCode/2)->m_nMapIndex, &dwSerial ) == false )
	{
		return RET_CODE_DB_QUERY_ERR;
	}
	

	*pdwAvatorSerial = dwSerial;

	return	RET_CODE_SUCCESS;
}

BYTE CMainThread::db_Delete_Avator(DWORD dwSerial, BYTE byRaceCode)
{
	if ( m_pWorldDB->Delete_CharacterData( dwSerial ) == false )
		return RET_CODE_DB_QUERY_ERR;

			
	return RET_CODE_SUCCESS;
}

BYTE CMainThread::db_Load_Avator(DWORD dwSerial, _AVATOR_DATA* OUT pData, bool bAll)
{
	BYTE byRet = RET_CODE_SUCCESS;

	if(bAll)
	{
		byRet = _db_Load_Base(dwSerial, pData);
		if(byRet != RET_CODE_SUCCESS)
		{
			m_logSystemError.Write("_db_Load_Base(%d) => failed ..{%d}", dwSerial, byRet);
			return byRet;
		}
	}

	byRet = _db_Load_General(dwSerial, pData->dbAvator.m_byRaceSexCode/2, pData);
	if(byRet != RET_CODE_SUCCESS)
	{
		m_logSystemError.Write("_db_Load_General(%d) => failed ..{%d}", dwSerial, byRet);
		return byRet;
	}

	byRet = _db_Load_Inven(dwSerial, pData->dbAvator.m_byBagNum, &pData->dbInven);
	if(byRet != RET_CODE_SUCCESS)
	{
		m_logSystemError.Write("RET_CODE_SUCCESS(%d) => failed ..{%d}", dwSerial, byRet);
		return byRet;
	}

	if(pData->dbAvator.m_byRaceSexCode/2 == race_code_bellato)
	{
		byRet = _db_Load_Unit(dwSerial, &pData->dbUnit);
		if(byRet != RET_CODE_SUCCESS)
		{
			m_logSystemError.Write("RET_CODE_SUCCESS(%d) => failed ..{%d}", dwSerial, byRet);
			return byRet;
		}
	}

	return byRet;
}

BYTE CMainThread::db_Update_Avator(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, bool bCheckLowHigh)
{
	BYTE byRet;

	char pSzBaseQuery[2000] = {0, };
	char pSzGeneralQuery[10000] = {0, };
	char pSzInvenQuery[10000] = {0, };
	char pSzUnitQuery[2000] = {0, };

	if(!_db_Update_Base(dwSerial, pNewData, pOldData, pSzBaseQuery, bCheckLowHigh))
	{
		m_logSystemError.Write("db_Update_Avator(sr:%d, name:%s) => _db_Update_Base..failed ..", dwSerial, pNewData->dbAvator.m_szAvatorName);
		return RET_CODE_DB_QUERY_ERR;
	}

	if(!_db_Update_General(dwSerial, pNewData, pOldData, pSzGeneralQuery, bCheckLowHigh))
	{
		m_logSystemError.Write("db_Update_Avator(sr:%d, name:%s) => _db_Update_General..failed ..", dwSerial, pNewData->dbAvator.m_szAvatorName);
		return RET_CODE_DB_QUERY_ERR;
	}

	if(!_db_Update_Inven(dwSerial, pNewData, pOldData, pSzInvenQuery))
	{
		m_logSystemError.Write("db_Update_Avator(sr:%d, name:%s) => _db_Update_Inven..failed ..", dwSerial, pNewData->dbAvator.m_szAvatorName);
		return RET_CODE_DB_QUERY_ERR;
	}

	if(pNewData->dbAvator.m_byRaceSexCode/2 == race_code_bellato)
	{
		if(!_db_Update_Unit(dwSerial, pNewData, pOldData, pSzUnitQuery))
		{
			m_logSystemError.Write("db_Update_Avator(sr:%d, name:%s) => _db_Update_Unit..failed ..", dwSerial, pNewData->dbAvator.m_szAvatorName);
			return RET_CODE_DB_QUERY_ERR;
		}
	}

	if ( m_pWorldDB->Update_CharacterData( pSzBaseQuery, pSzGeneralQuery, pSzInvenQuery, pSzUnitQuery ) )
		byRet = RET_CODE_SUCCESS;
	else
		byRet = RET_CODE_DB_QUERY_ERR;

	return byRet;
}

//BYTE CMainThread::db_Select_Economy_History(_economy_history_data* OUT pData, int* OUT pHistoryNum)
//{
//	_worlddb_economy_history_info_array dbEconomyHistoryArray;
//	memset( &dbEconomyHistoryArray, 0, sizeof(_worlddb_economy_history_info_array) );
//
//	BYTE byDBRet = m_pWorldDB->Select_Economy_History( &dbEconomyHistoryArray );
//	if ( byDBRet == db_result_sql_error )
//		return RET_CODE_DB_QUERY_ERR;
//
//	for ( int i=0; i<dbEconomyHistoryArray.wRowCount; i++ )
//	{
//		pData[i].wEconomyGuide[race_code_bellato]	= dbEconomyHistoryArray.EconomyData[i].dwBRate;
//		pData[i].wEconomyGuide[race_code_cora]		= dbEconomyHistoryArray.EconomyData[i].dwCRate;
//		pData[i].wEconomyGuide[race_code_accretia]	= dbEconomyHistoryArray.EconomyData[i].dwARate;
//	}
//
//	*pHistoryNum = dbEconomyHistoryArray.wRowCount;
//
//	return RET_CODE_SUCCESS;
//
//}
//
//BYTE CMainThread::db_Insert_Economy_History(_economy_history_data* pData)
//{
//	bool bRet = m_pWorldDB->Insert_Economy_History( 
//			pData->dTradeDalant[race_code_bellato],		pData->dTradeGold[race_code_bellato],	pData->wEconomyGuide[race_code_bellato],
//			pData->dTradeDalant[race_code_cora],		pData->dTradeGold[race_code_cora],		pData->wEconomyGuide[race_code_cora],
//			pData->dTradeDalant[race_code_accretia],	pData->dTradeGold[race_code_accretia],	pData->wEconomyGuide[race_code_accretia]);
//
//	return bRet;
//}

// ydq 侶몸匡숭RF_Server_old궐싹劤寧硅
BYTE CMainThread::db_Select_Economy_History(_economy_history_data* OUT pCurData, int* OUT pnCurMgrValue, int* OUT pnNextMgrValue, _economy_history_data* OUT pHisData, int* OUT pHistoryNum, DWORD dwDate)
{
	_worlddb_economy_history_info_array::_worlddb_economy_history_info economyData;
	BYTE byRet = m_pWorldDB->Select_Exist_Economy( dwDate, &economyData );

	if ( byRet == db_result_sql_success )
	{
		pCurData->dTradeDalant[race_code_bellato] = economyData.dTradeDalant[race_code_bellato];
		pCurData->dTradeDalant[race_code_cora] = economyData.dTradeDalant[race_code_cora];
		pCurData->dTradeDalant[race_code_accretia] = economyData.dTradeDalant[race_code_accretia];

		pCurData->dTradeGold[race_code_bellato] = economyData.dTradeGold[race_code_bellato];
		pCurData->dTradeGold[race_code_cora] = economyData.dTradeGold[race_code_cora];
		pCurData->dTradeGold[race_code_accretia] = economyData.dTradeGold[race_code_accretia];

		*pnCurMgrValue = economyData.dwManageValue;
		*pnNextMgrValue = economyData.dwManageValue;
	}

	_worlddb_economy_history_info_array dbEconomyHistoryArray;
	memset( &dbEconomyHistoryArray, 0, sizeof(_worlddb_economy_history_info_array) );

	BYTE byDBRet = m_pWorldDB->Select_Economy_History( &dbEconomyHistoryArray, dwDate );
	if ( byDBRet == db_result_sql_error )
		return RET_CODE_DB_QUERY_ERR;

	int j = 12;

	for ( int i=0; i<dbEconomyHistoryArray.wRowCount; i++ )
	{
		j--;
		if ( j < 0 )
			break;
		pHisData[j].dTradeDalant[race_code_bellato]	= dbEconomyHistoryArray.EconomyData[i].dTradeDalant[race_code_bellato];
		pHisData[j].dTradeDalant[race_code_cora]		= dbEconomyHistoryArray.EconomyData[i].dTradeDalant[race_code_cora];
		pHisData[j].dTradeDalant[race_code_accretia]	= dbEconomyHistoryArray.EconomyData[i].dTradeDalant[race_code_accretia];

		pHisData[j].dTradeGold[race_code_bellato]	= dbEconomyHistoryArray.EconomyData[i].dTradeGold[race_code_bellato];
		pHisData[j].dTradeGold[race_code_cora]		= dbEconomyHistoryArray.EconomyData[i].dTradeGold[race_code_cora];
		pHisData[j].dTradeGold[race_code_accretia]	= dbEconomyHistoryArray.EconomyData[i].dTradeGold[race_code_accretia];
	}

	*pHistoryNum = dbEconomyHistoryArray.wRowCount;

	return RET_CODE_SUCCESS;

}

BYTE CMainThread::db_Insert_Economy_History(DWORD dwDate, double* pdTradeDalant, double* pdTradeGold, int nMgrValue)
{
	return m_pWorldDB->Insert_Economy_History( dwDate, pdTradeDalant, pdTradeGold, nMgrValue );
}



BYTE CMainThread::db_Log_AvatorLevel(DWORD dwTotalPlayMin, DWORD dwSerial, BYTE byLv)
{
	if ( m_pWorldDB->Insert_Level_Log( dwSerial, byLv, dwTotalPlayMin ) )
		return RET_CODE_SUCCESS;
	else
		return RET_CODE_DB_QUERY_ERR;

}

BYTE CMainThread::db_Log_UserNum(int nAveragePerHour, int nMaxPerHour)
{
	if ( m_pWorldDB->Insert_UserNum_Log( nAveragePerHour, nMaxPerHour ) )
		return RET_CODE_SUCCESS;
	else
		return RET_CODE_DB_QUERY_ERR;

}

BYTE CMainThread::_db_Load_Base(DWORD dwSerial, _AVATOR_DATA* OUT pCon)
{
	_worlddb_character_base_info dbCharacterData;
	memset( &dbCharacterData, 0, sizeof(_worlddb_character_base_info) );
	

	BYTE byDBRet = m_pWorldDB->Select_CharacterBaseInfo( dwSerial, &dbCharacterData );
	if ( byDBRet == db_result_sql_error )
		return RET_CODE_DB_QUERY_ERR;

	if ( byDBRet == db_result_no_data )
		return RET_CODE_DB_QUERY_ERR;

	strcpy(pCon->dbAvator.m_szAvatorName, dbCharacterData.szName );	
	pCon->dbAvator.m_dwRecordNum = dwSerial;
	pCon->dbAvator.m_byRaceSexCode = dbCharacterData.byRace;
	strcpy(pCon->dbAvator.m_szClassCode, dbCharacterData.szClassCode);	
	pCon->dbAvator.m_bySlotIndex = dbCharacterData.bySlotIndex;	
	pCon->dbAvator.m_byLevel = dbCharacterData.byLevel;
	pCon->dbAvator.m_dwDalant = dbCharacterData.dwDalant;
	pCon->dbAvator.m_dwGold = dbCharacterData.dwGold;
	pCon->dbAvator.m_dwBaseShape = dbCharacterData.dwBaseShape;
	pCon->dbAvator.m_dwLastConnTime = dbCharacterData.dwLastConnTime;

	for(int i = 0; i < equip_fix_num; i++)
	{
		pCon->dbAvator.m_EquipKey[i].LoadDBKey(dbCharacterData.shEKArray[i]);
		pCon->dbAvator.m_dwFixEquipLv[i] = dbCharacterData.dwEUArray[i];
	}

	return RET_CODE_SUCCESS;
}

BYTE CMainThread::_db_Load_General(DWORD dwSerial, BYTE byRaceCode, _AVATOR_DATA* OUT pCon)
{
	char szBuf[128] = {0,};
	memset( szBuf, 0, sizeof(szBuf) );

	_worlddb_character_general_info dbCharacterData;
	memset( &dbCharacterData, 0, sizeof(_worlddb_character_general_info) );
	

	BYTE byDBRet = m_pWorldDB->Select_CharacterGeneralInfo( dwSerial, &dbCharacterData );
	if ( byDBRet == db_result_sql_error )
		return RET_CODE_DB_QUERY_ERR;
	else if ( byDBRet == db_result_no_data )
		return RET_CODE_NONEXIST_DB_RECORD;

	pCon->dbAvator.m_dwHP = dbCharacterData.dwHP;
	pCon->dbAvator.m_dwFP = dbCharacterData.dwFP;
	pCon->dbAvator.m_dwSP = dbCharacterData.dwSP;
	pCon->dbAvator.m_dExp = dbCharacterData.dExp;			
	pCon->dbAvator.m_byBagNum = dbCharacterData.byBagNum;
	pCon->dbAvator.m_byMapCode = dbCharacterData.byMapCode;
	pCon->dbAvator.m_fStartPos[0] = dbCharacterData.fStartPos[0];
	pCon->dbAvator.m_fStartPos[1] = dbCharacterData.fStartPos[1];
	pCon->dbAvator.m_fStartPos[2] = dbCharacterData.fStartPos[2];
	pCon->dbAvator.m_dwTotalPlayMin = dbCharacterData.dwTotalPlayMin;

	char szLeftResData[200];
	strcpy(szLeftResData, dbCharacterData.szLeftResList);

	pCon->dbCutting.Init();
	if(szLeftResData != "*")
	{
		int nSize = strlen(szLeftResData);

		if(nSize%5 == 0)	//5자딱 떨어져야한다..
		{
			pCon->dbCutting.m_byLeftNum = nSize/5;	//res의 종류수

			int nOffSet = 0;
			for(int i = 0; i < pCon->dbCutting.m_byLeftNum; i++)
			{
				memcpy(szBuf, &szLeftResData[nOffSet], 2);
				szBuf[2] = NULL;
				nOffSet += 2;

				pCon->dbCutting.m_List[i].byResIndex = atoi(szBuf);

				memcpy(szBuf, &szLeftResData[nOffSet], 3);
				szBuf[3] = NULL;
				nOffSet += 3;

				pCon->dbCutting.m_List[i].byAmt = atoi(szBuf);
			}
		}
	}

	for(int i = 0; i < potion_belt_num; i++)
	{
		pCon->dbBelt.m_PotionList[i].Key.LoadDBKey(dbCharacterData.lPK[i]);
	}
	for(i = 0; i < sf_linker_num; i++)
	{
		pCon->dbBelt.m_LinkList[i].Key.LoadDBKey(dbCharacterData.shLK[i]);
	}
	for(i = 0; i < embellish_fix_num; i++)
	{
		pCon->dbEquip.m_EmbellishList[i].Key.LoadDBKey(dbCharacterData.lEK[i]);
		pCon->dbEquip.m_EmbellishList[i].wAmount = dbCharacterData.shED[i];
	}
	for(i = 0; i < force_storage_num; i++)
	{
		pCon->dbForce.m_List[i].Key.LoadDBKey(dbCharacterData.lF[i]);
	}
	if(byRaceCode == race_code_cora)
	{
		for(i = 0; i < animus_storage_num; i++)
		{
			pCon->dbAnimus.m_List[i].Key.LoadDBKey(dbCharacterData.dwAK[i]);
			pCon->dbAnimus.m_List[i].dwExp = dbCharacterData.dwAD[i];
			pCon->dbAnimus.m_List[i].dwParam = dbCharacterData.dwAP[i];
		}
	}
	for(i = 0; i < WP_CLASS_NUM; i++)
	{
		sprintf(szBuf, "WM%d", i);
		pCon->dbStat.m_dwDamWpCnt[i] = dbCharacterData.dwWM[i];
	}
	for(i = 0; i < force_mastery_num; i++)
	{
		sprintf(szBuf, "FM%d", i);
		pCon->dbStat.m_dwForceCum[i] = dbCharacterData.dwFM[i];
	}
	for(i = 0; i < max_skill_num; i++)
	{
		sprintf(szBuf, "SM%d", i);
		pCon->dbStat.m_dwSkillCum[i] = dbCharacterData.dwSM[i];
	}
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		sprintf(szBuf, "MI%d", i);
		pCon->dbStat.m_dwMakeCum[i] = dbCharacterData.dwMI[i];
	}

	pCon->dbStat.m_dwSpecialCum = dbCharacterData.dwSR;
	pCon->dbStat.m_dwDefenceCnt = dbCharacterData.dwDM;
	pCon->dbStat.m_dwShieldCnt = dbCharacterData.dwPM;

	for(i = 0; i < class_history_num; i++)
	{
		pCon->dbAvator.m_zClassHistory[i] = dbCharacterData.zClassHistory[i];
	}
	
	return RET_CODE_SUCCESS;
}

BYTE CMainThread::_db_Load_Inven(DWORD dwSerial, int nBagNum, _INVEN_DB_BASE* OUT pCon)
{
	_worlddb_inven_info dbInvenInfo;
	memset( &dbInvenInfo, 0, sizeof(_worlddb_inven_info) );
	

	BYTE byDBRet = m_pWorldDB->Select_Inven( dwSerial, nBagNum, &dbInvenInfo );
	if ( byDBRet == db_result_sql_error )
		return RET_CODE_DB_QUERY_ERR;
	else if ( byDBRet == db_result_no_data )
		return RET_CODE_NONEXIST_DB_RECORD;

	for(int i = 0; i < bag_storage_num; i++)
	{
		if(one_bag_store_num*nBagNum > i)
		{
			pCon->m_List[i].Key.LoadDBKey(dbInvenInfo.lK[i]);
			pCon->m_List[i].dwDur = dbInvenInfo.dwD[i];
			pCon->m_List[i].dwUpt = dbInvenInfo.dwU[i];
		}
		else
		{
			pCon->m_List[i].Key.SetRelease();
		}
	}

	return RET_CODE_SUCCESS;
}

BYTE CMainThread::_db_Load_Unit(DWORD dwSerial, _UNIT_DB_BASE* OUT pCon)
{
	_worlddb_unit_info_array dbUnitInfo;
	memset( &dbUnitInfo, 0, sizeof(_worlddb_unit_info_array) );
	

	BYTE byDBRet = m_pWorldDB->Select_Unit( dwSerial, &dbUnitInfo );
	if ( byDBRet == db_result_sql_error )
		return RET_CODE_DB_QUERY_ERR;
	else if ( byDBRet == db_result_no_data )
	{
		if ( m_pWorldDB->Insert_Unit( dwSerial ) == false )
			return RET_CODE_DB_QUERY_ERR;
		else
		{
			if ( m_pWorldDB->Select_Unit( dwSerial, &dbUnitInfo ) != db_result_sql_success )
				return RET_CODE_DB_QUERY_ERR;
		}

	}

	for(int i = 0; i < unit_storage_num; i++)
	{
		pCon->m_List[i].byFrame = dbUnitInfo.UnitInfo[i].byFrame;

		if(pCon->m_List[i].byFrame == 0xFF)
			continue;

		pCon->m_List[i].byPart[unit_part_head] = dbUnitInfo.UnitInfo[i].byPart[unit_part_head];
		pCon->m_List[i].byPart[unit_part_upper] = dbUnitInfo.UnitInfo[i].byPart[unit_part_upper];
		pCon->m_List[i].byPart[unit_part_lower] = dbUnitInfo.UnitInfo[i].byPart[unit_part_lower];
		pCon->m_List[i].byPart[unit_part_arms] = dbUnitInfo.UnitInfo[i].byPart[unit_part_arms];
		pCon->m_List[i].byPart[unit_part_shoulder] = dbUnitInfo.UnitInfo[i].byPart[unit_part_shoulder];
		pCon->m_List[i].byPart[unit_part_back] = dbUnitInfo.UnitInfo[i].byPart[unit_part_back];

		pCon->m_List[i].dwGauge = dbUnitInfo.UnitInfo[i].dwGauge;//ydq,侶몸뒈렘잚謹唐맣긴

		pCon->m_List[i].dwBullet[unit_bullet_arms] = dbUnitInfo.UnitInfo[i].dwBullet[unit_bullet_arms];
		pCon->m_List[i].dwBullet[unit_bullet_shoulder] = dbUnitInfo.UnitInfo[i].dwBullet[unit_bullet_shoulder];

		pCon->m_List[i].nKeepingFee = dbUnitInfo.UnitInfo[i].nKeepingFee;
		pCon->m_List[i].nPullingFee = dbUnitInfo.UnitInfo[i].nPullingFee;

		pCon->m_List[i].dwCutTime = dbUnitInfo.UnitInfo[i].dwCutTime;

		for(int s = 0; s < max_unit_spare; s++)
		{
			pCon->m_List[i].dwSpare[i] = dbUnitInfo.UnitInfo[i].dwSpare[s];
		}
	}

	return RET_CODE_SUCCESS;
}

bool CMainThread::_db_Update_Base(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, char* pSzQuery, bool bCheckLowHigh)
{
	char szBuf[128] = {0,};
	char* szSql = pSzQuery;

	sprintf(szSql, "UPDATE tbl_base SET ");

	if(bCheckLowHigh)
	{
		if(pOldData->dbAvator.m_byLevel < pNewData->dbAvator.m_byLevel)	
		{
			sprintf(szBuf, "Lv=%d,", pNewData->dbAvator.m_byLevel);
			strcat(szSql, szBuf);
		}
		if(pOldData->dbAvator.m_byLevel > pNewData->dbAvator.m_byLevel)	//예외..
			m_logSystemError.Write("%s>> _db_Update_Base: 레벨이 처음(%d)보다 적어짐(%d)", pOldData->dbAvator.m_szAvatorName, pNewData->dbAvator.m_byLevel, pOldData->dbAvator.m_byLevel);
	}
	else
	{
		if(pOldData->dbAvator.m_byLevel != pNewData->dbAvator.m_byLevel)	
		{
			sprintf(szBuf, "Lv=%d,", pNewData->dbAvator.m_byLevel);
			strcat(szSql, szBuf);
		}
	}

	if(strcmp(pOldData->dbAvator.m_szClassCode, pNewData->dbAvator.m_szClassCode))
	{
		sprintf(szBuf, "class='%s',", pNewData->dbAvator.m_szClassCode);
		strcat(szSql, szBuf);
	}

	if(pOldData->dbAvator.m_dwDalant != pNewData->dbAvator.m_dwDalant)
	{
		sprintf(szBuf, "Dalant=%d,", pNewData->dbAvator.m_dwDalant);
		strcat(szSql, szBuf);
	}
	if(pOldData->dbAvator.m_dwGold != pNewData->dbAvator.m_dwGold)
	{
		sprintf(szBuf, "Gold=%d,", pNewData->dbAvator.m_dwGold);
		strcat(szSql, szBuf);
	}

	for(int i = 0; i < equip_fix_num; i++)
	{
		if(!pNewData->dbAvator.m_EquipKey[i].IsFilled())
		{
			if(pOldData->dbAvator.m_EquipKey[i].IsFilled())	//전에는 있었지만 현재는 비워진상태..
			{
				sprintf(szBuf, "EK%d=%d,", i, pNewData->dbAvator.m_EquipKey[i].CovDBKey());
				strcat(szSql, szBuf);				
			}
		}
		else
		{
			if(pOldData->dbAvator.m_EquipKey[i].IsFilled())	//전에도 있었고 현재도 있는상태
			{
				if(pNewData->dbAvator.m_EquipKey[i].CovDBKey() != pOldData->dbAvator.m_EquipKey[i].CovDBKey())
				{
					sprintf(szBuf, "EK%d=%d,", i, pNewData->dbAvator.m_EquipKey[i].CovDBKey());
					strcat(szSql, szBuf);
				}
				if(pNewData->dbAvator.m_dwFixEquipLv[i] != pOldData->dbAvator.m_dwFixEquipLv[i])
				{					
					sprintf(szBuf, "EU%d=%d,", i, pNewData->dbAvator.m_dwFixEquipLv[i]);
					strcat(szSql, szBuf);
				}
			}			
			else		//전에는 없었고 현재는 있는상태
			{
				sprintf(szBuf, "EK%d=%d,EU%d=%d,", i, pNewData->dbAvator.m_EquipKey[i].CovDBKey(), i, pNewData->dbAvator.m_dwFixEquipLv[i]);
				strcat(szSql, szBuf);
			}		
		}	
	}

	sprintf(szBuf, "LastConnTime=%d WHERE Serial=%d", ::GetKorLocalTime(), dwSerial); 
	strcat(szSql, szBuf);

	return true;
}

bool CMainThread::_db_Update_General(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, char* pSzQuery, bool bCheckLowHigh)
{
	char szBuf[200] = {0,};
	char* szSql = pSzQuery;
	sprintf(szSql, "UPDATE tbl_general SET ");
	DWORD nQueryHeadSize = strlen(szSql);

	if(pOldData->dbAvator.m_dExp != pNewData->dbAvator.m_dExp)
	{
		sprintf(szBuf, "Exp=%f,", pNewData->dbAvator.m_dExp);
		strcat(szSql, szBuf);
	}
	for(int i = 0; i < class_history_num; i++)
	{
		if(pOldData->dbAvator.m_zClassHistory[i] != pNewData->dbAvator.m_zClassHistory[i])
		{
			sprintf(szBuf, "Class%d=%d,", i, pNewData->dbAvator.m_zClassHistory[i]);
			strcat(szSql, szBuf);
		}
	}

	if(pOldData->dbAvator.m_dwHP != pNewData->dbAvator.m_dwHP)	
	{
		sprintf(szBuf, "HP=%d,", pNewData->dbAvator.m_dwHP);
		strcat(szSql, szBuf);
	}
	if(pOldData->dbAvator.m_dwFP != pNewData->dbAvator.m_dwFP)	
	{
		sprintf(szBuf, "FP=%d,", pNewData->dbAvator.m_dwFP);
		strcat(szSql, szBuf);
	}
	if(pOldData->dbAvator.m_dwSP != pNewData->dbAvator.m_dwSP)
	{
		sprintf(szBuf, "SP=%d,", pNewData->dbAvator.m_dwSP);
		strcat(szSql, szBuf);
	}
	if(pOldData->dbAvator.m_byMapCode != pNewData->dbAvator.m_byMapCode)
	{
		sprintf(szBuf, "Map=%d,", pNewData->dbAvator.m_byMapCode);
		strcat(szSql, szBuf);
	}
	if(pOldData->dbAvator.m_fStartPos[0] != pNewData->dbAvator.m_fStartPos[0])
	{
		sprintf(szBuf, "X=%f,", pNewData->dbAvator.m_fStartPos[0]);
		strcat(szSql, szBuf);
	}
	if(pOldData->dbAvator.m_fStartPos[1] != pNewData->dbAvator.m_fStartPos[1])
	{
		sprintf(szBuf, "Y=%f,", pNewData->dbAvator.m_fStartPos[1]);
		strcat(szSql, szBuf);
	}
	if(pOldData->dbAvator.m_fStartPos[2] != pNewData->dbAvator.m_fStartPos[2])
	{
		sprintf(szBuf, "Z=%f,", pNewData->dbAvator.m_fStartPos[2]);
		strcat(szSql, szBuf);
	}
	if(bCheckLowHigh)
	{
		if(pOldData->dbAvator.m_byBagNum < pNewData->dbAvator.m_byBagNum)
		{
			sprintf(szBuf, "BagNum=%d,", pNewData->dbAvator.m_byBagNum);
			strcat(szSql, szBuf);
		}
	}
	else
	{
		if(pOldData->dbAvator.m_byBagNum != pNewData->dbAvator.m_byBagNum)
		{
			sprintf(szBuf, "BagNum=%d,", pNewData->dbAvator.m_byBagNum);
			strcat(szSql, szBuf);
		}
	}

	for(i = 0; i < embellish_fix_num; i++)
	{
		if(!pNewData->dbEquip.m_EmbellishList[i].Key.IsFilled())
		{
			if(pOldData->dbEquip.m_EmbellishList[i].Key.IsFilled())
			{
				sprintf(szBuf, "EK%d=%d,", i, pNewData->dbEquip.m_EmbellishList[i].Key.CovDBKey());
				strcat(szSql, szBuf);				
			}
		}
		else
		{
			if(pOldData->dbEquip.m_EmbellishList[i].Key.IsFilled())
			{
				if(pNewData->dbEquip.m_EmbellishList[i].Key.CovDBKey() != pOldData->dbEquip.m_EmbellishList[i].Key.CovDBKey())
				{
					sprintf(szBuf, "EK%d=%d,", i, pNewData->dbEquip.m_EmbellishList[i].Key.CovDBKey());
					strcat(szSql, szBuf);
				}
				if(pNewData->dbEquip.m_EmbellishList[i].wAmount != pOldData->dbEquip.m_EmbellishList[i].wAmount)
				{					
					sprintf(szBuf, "ED%d=%d,", i, pNewData->dbEquip.m_EmbellishList[i].wAmount);
					strcat(szSql, szBuf);
				}
			}			
			else
			{
				sprintf(szBuf, "EK%d=%d,ED%d=%d,", i, pNewData->dbEquip.m_EmbellishList[i].Key.CovDBKey(), i, pNewData->dbEquip.m_EmbellishList[i].wAmount);
				strcat(szSql, szBuf);
			}		
		}
	}
	for(i = 0; i < potion_belt_num; i++)
	{
		if(!pNewData->dbBelt.m_PotionList[i].Key.IsFilled())
		{
			if(pOldData->dbBelt.m_PotionList[i].Key.IsFilled())
			{
				sprintf(szBuf, "PK%d=%d,", i, pNewData->dbBelt.m_PotionList[i].Key.CovDBKey());
				strcat(szSql, szBuf);				
			}
		}
		else
		{
			if(pOldData->dbBelt.m_PotionList[i].Key.IsFilled())
			{
				if(pNewData->dbBelt.m_PotionList[i].Key.CovDBKey() != pOldData->dbBelt.m_PotionList[i].Key.CovDBKey())
				{
					sprintf(szBuf, "PK%d=%d,", i, pNewData->dbBelt.m_PotionList[i].Key.CovDBKey());
					strcat(szSql, szBuf);
				}
			}			
			else
			{
				sprintf(szBuf, "PK%d=%d,", i, pNewData->dbBelt.m_PotionList[i].Key.CovDBKey());
				strcat(szSql, szBuf);
			}		
		}	
	}
	for(i = 0; i < sf_linker_num; i++)
	{
		if(pOldData->dbBelt.m_LinkList[i].Key.CovDBKey() != pNewData->dbBelt.m_LinkList[i].Key.CovDBKey())
		{
			sprintf(szBuf, "LK%d=%d,", i, pNewData->dbBelt.m_LinkList[i].Key.CovDBKey());
			strcat(szSql, szBuf);
		}
	}
	for(i = 0; i < force_storage_num; i++)
	{
		if(!pNewData->dbForce.m_List[i].Key.IsFilled())
		{
			if(pOldData->dbForce.m_List[i].Key.IsFilled())
			{
				sprintf(szBuf, "F%d=%d,", i, pNewData->dbForce.m_List[i].Key.CovDBKey());
				strcat(szSql, szBuf);				
			}
		}
		else
		{
			if(pOldData->dbForce.m_List[i].Key.IsFilled())
			{
				if(pNewData->dbForce.m_List[i].Key.CovDBKey() != pOldData->dbForce.m_List[i].Key.CovDBKey())
				{
					sprintf(szBuf, "F%d=%d,", i, pNewData->dbForce.m_List[i].Key.CovDBKey());
					strcat(szSql, szBuf);
				}
			}			
			else
			{
				sprintf(szBuf, "F%d=%d,", i, pNewData->dbForce.m_List[i].Key.CovDBKey());
				strcat(szSql, szBuf);
			}		
		}	
	}
	if(pOldData->dbAvator.m_byRaceSexCode/2 == race_code_cora)
	{
		for(i = 0; i < animus_storage_num; i++)
		{
			if(!pNewData->dbAnimus.m_List[i].Key.IsFilled())
			{
				if(pOldData->dbAnimus.m_List[i].Key.IsFilled())
				{
					sprintf(szBuf, "AK%d=%d,", i, pNewData->dbAnimus.m_List[i].Key.CovDBKey());
					strcat(szSql, szBuf);				
				}
			}
			else
			{
				if(pOldData->dbAnimus.m_List[i].Key.IsFilled())
				{
					if(pNewData->dbAnimus.m_List[i].Key.CovDBKey() != pOldData->dbAnimus.m_List[i].Key.CovDBKey())
					{
						sprintf(szBuf, "AK%d=%d,", i, pNewData->dbAnimus.m_List[i].Key.CovDBKey());
						strcat(szSql, szBuf);
					}
					if(pNewData->dbAnimus.m_List[i].dwExp != pOldData->dbAnimus.m_List[i].dwExp)
					{
						sprintf(szBuf, "AD%d=%d,", i, pNewData->dbAnimus.m_List[i].dwExp);
						strcat(szSql, szBuf);
					}
					if(pNewData->dbAnimus.m_List[i].dwParam != pOldData->dbAnimus.m_List[i].dwParam)
					{
						sprintf(szBuf, "AP%d=%d,", i, pNewData->dbAnimus.m_List[i].dwParam);
						strcat(szSql, szBuf);
					}
				}			
				else
				{
					sprintf(szBuf, "AK%d=%d,", i, pNewData->dbAnimus.m_List[i].Key.CovDBKey());
					strcat(szSql, szBuf);

					sprintf(szBuf, "AD%d=%d,", i, pNewData->dbAnimus.m_List[i].dwExp);
					strcat(szSql, szBuf);

					sprintf(szBuf, "AP%d=%d,", i, pNewData->dbAnimus.m_List[i].dwParam);
					strcat(szSql, szBuf);
				}		
			}	
		}
	}

	//무기숙련도..
	for(i = 0; i < WP_CLASS_NUM; i++)
	{
		if(bCheckLowHigh)
		{
			if(pOldData->dbStat.m_dwDamWpCnt[i] < pNewData->dbStat.m_dwDamWpCnt[i])
			{
				sprintf(szBuf, "WM%d=%d,", i, pNewData->dbStat.m_dwDamWpCnt[i]);
				strcat(szSql, szBuf);
			}
			else if(pOldData->dbStat.m_dwDamWpCnt[i] > pNewData->dbStat.m_dwDamWpCnt[i])
				m_logSystemError.Write("%s>>_db_Update_General:무기마스터리(%d) 처음(%d)보다 작아짐(%d)", pOldData->dbAvator.m_szAvatorName, i, pOldData->dbStat.m_dwDamWpCnt[i], pNewData->dbStat.m_dwDamWpCnt[i]);
		}
		else
		{
			if(pOldData->dbStat.m_dwDamWpCnt[i] != pNewData->dbStat.m_dwDamWpCnt[i])
			{
				sprintf(szBuf, "WM%d=%d,", i, pNewData->dbStat.m_dwDamWpCnt[i]);
				strcat(szSql, szBuf);
			}
		}
	}	

	//포스숙련도..
	for(i = 0; i < force_mastery_num; i++)
	{
		if(bCheckLowHigh)
		{
			if(pOldData->dbStat.m_dwForceCum[i] < pNewData->dbStat.m_dwForceCum[i])
			{
				sprintf(szBuf, "FM%d=%d,", i, pNewData->dbStat.m_dwForceCum[i]);
				strcat(szSql, szBuf);
			}
			else if(pOldData->dbStat.m_dwForceCum[i] > pNewData->dbStat.m_dwForceCum[i])
				m_logSystemError.Write("%s>>_db_Update_General:포스마스터리(%d) 처음(%d)보다 작아짐(%d)", pOldData->dbAvator.m_szAvatorName, i, pOldData->dbStat.m_dwForceCum[i], pNewData->dbStat.m_dwForceCum[i]);
		}
		else
		{
			if(pOldData->dbStat.m_dwForceCum[i] != pNewData->dbStat.m_dwForceCum[i])
			{
				sprintf(szBuf, "FM%d=%d,", i, pNewData->dbStat.m_dwForceCum[i]);
				strcat(szSql, szBuf);
			}
		}
	}

	//스킬사용횟수..
	for(i = 0; i < max_skill_num; i++)
	{
		if(bCheckLowHigh)
		{
			if(pOldData->dbStat.m_dwSkillCum[i] < pNewData->dbStat.m_dwSkillCum[i])
			{
				sprintf(szBuf, "SM%d=%d,", i, pNewData->dbStat.m_dwSkillCum[i]);
				strcat(szSql, szBuf);
			}
			else if(pOldData->dbStat.m_dwSkillCum[i] > pNewData->dbStat.m_dwSkillCum[i])
				m_logSystemError.Write("%s>>_db_Update_General:스킬마스터리(%d) 처음(%d)보다 작아짐(%d)", pOldData->dbAvator.m_szAvatorName, i, pOldData->dbStat.m_dwSkillCum[i], pNewData->dbStat.m_dwSkillCum[i]);
		}
		else
		{
			if(pOldData->dbStat.m_dwSkillCum[i] != pNewData->dbStat.m_dwSkillCum[i])
			{
				sprintf(szBuf, "SM%d=%d,", i, pNewData->dbStat.m_dwSkillCum[i]);
				strcat(szSql, szBuf);
			}
		}
	}

	//제작숙련도..
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		if(bCheckLowHigh)
		{
			if(pOldData->dbStat.m_dwMakeCum[i] < pNewData->dbStat.m_dwMakeCum[i])
			{
				sprintf(szBuf, "MI%d=%d,", i, pNewData->dbStat.m_dwMakeCum[i]);
				strcat(szSql, szBuf);
			}
			else if(pOldData->dbStat.m_dwMakeCum[i] > pNewData->dbStat.m_dwMakeCum[i])
				m_logSystemError.Write("%s>>_db_Update_General:제작마스터리(%d) 처음(%d)보다 작아짐(%d)", pOldData->dbAvator.m_szAvatorName, i, pOldData->dbStat.m_dwMakeCum[i], pNewData->dbStat.m_dwMakeCum[i]);
		}
		else
		{
			if(pOldData->dbStat.m_dwMakeCum[i] != pNewData->dbStat.m_dwMakeCum[i])
			{
				sprintf(szBuf, "MI%d=%d,", i, pNewData->dbStat.m_dwMakeCum[i]);
				strcat(szSql, szBuf);
			}
		}
	}	
 
	//종족별 특화숙련도
	if(bCheckLowHigh)
	{
		if(pOldData->dbStat.m_dwSpecialCum < pNewData->dbStat.m_dwSpecialCum)
		{
			sprintf(szBuf, "SR=%d,", pNewData->dbStat.m_dwSpecialCum);
			strcat(szSql, szBuf);
		}
		else if(pOldData->dbStat.m_dwSpecialCum > pNewData->dbStat.m_dwSpecialCum)
			m_logSystemError.Write("%s>>_db_Update_General:특수마스터리 처음(%d)보다 작아짐(%d)", pOldData->dbAvator.m_szAvatorName, pOldData->dbStat.m_dwSpecialCum, pNewData->dbStat.m_dwSpecialCum);
	}
	else
	{
		if(pOldData->dbStat.m_dwSpecialCum != pNewData->dbStat.m_dwSpecialCum)
		{
			sprintf(szBuf, "SR=%d,", pNewData->dbStat.m_dwSpecialCum);
			strcat(szSql, szBuf);
		}
	}

	//방어숙련도..
	if(bCheckLowHigh)
	{
		if(pOldData->dbStat.m_dwDefenceCnt < pNewData->dbStat.m_dwDefenceCnt)
		{
			sprintf(szBuf, "DM=%d,", pNewData->dbStat.m_dwDefenceCnt);
			strcat(szSql, szBuf);
		}
		else if(pOldData->dbStat.m_dwDefenceCnt > pNewData->dbStat.m_dwDefenceCnt)
			m_logSystemError.Write("%s>>_db_Update_General:방어마스터리 처음(%d)보다 작아짐(%d)", pOldData->dbAvator.m_szAvatorName, pOldData->dbStat.m_dwDefenceCnt, pNewData->dbStat.m_dwDefenceCnt);
	}
	else
	{
		if(pOldData->dbStat.m_dwDefenceCnt != pNewData->dbStat.m_dwDefenceCnt)
		{
			sprintf(szBuf, "DM=%d,", pNewData->dbStat.m_dwDefenceCnt);
			strcat(szSql, szBuf);
		}
	}

	//방패숙련도..
	if(bCheckLowHigh)
	{
		if(pOldData->dbStat.m_dwShieldCnt < pNewData->dbStat.m_dwShieldCnt)
		{
			sprintf(szBuf, "PM=%d,", pNewData->dbStat.m_dwShieldCnt);
			strcat(szSql, szBuf);
		}
		else if(pOldData->dbStat.m_dwShieldCnt > pNewData->dbStat.m_dwShieldCnt)
			m_logSystemError.Write("%s>>_db_Update_General:방패마스터리 처음(%d)보다 작아짐(%d)", pOldData->dbAvator.m_szAvatorName, pOldData->dbStat.m_dwShieldCnt, pNewData->dbStat.m_dwShieldCnt);
	}
	else
	{
		if(pOldData->dbStat.m_dwShieldCnt != pNewData->dbStat.m_dwShieldCnt)
		{
			sprintf(szBuf, "PM=%d,", pNewData->dbStat.m_dwShieldCnt);
			strcat(szSql, szBuf);
		}
	}

	//Left Cutting Res가 변햇는지..
	//변했다면 스트링으로 바꿔서 저장..
	bool bChange = false;
	for(i = 0; i < left_cutting_num; i++)
	{
		if(*((WORD*)&pNewData->dbCutting.m_List[i]) != *((WORD*)&pOldData->dbCutting.m_List[i]))
		{
			bChange = true;
			break;
		}
	}

	if(bChange)
	{
		char sResData[200];
		int nDataSize = 0;
		for(i= 0; i < left_cutting_num; i++)
		{
			if(pNewData->dbCutting.m_List[i].byResIndex != 0xFF)
			{
				char sIndex[3] = "00";
				char sAmt[4] = "000";
				int nIndexOffSet = 0;
				int nAmtOffSet = 0;

				if(pNewData->dbCutting.m_List[i].byResIndex < 10)
					nIndexOffSet = 1;
				if(pNewData->dbCutting.m_List[i].byAmt < 10)
					nAmtOffSet = 2;
				else if(pNewData->dbCutting.m_List[i].byAmt < 100)
					nAmtOffSet = 1;

				_itoa(pNewData->dbCutting.m_List[i].byResIndex, sIndex+nIndexOffSet, 10);
				_itoa(pNewData->dbCutting.m_List[i].byAmt, sAmt+nAmtOffSet, 10);

				memcpy(&sResData[nDataSize], sIndex, 2);
				nDataSize += 2;
				memcpy(&sResData[nDataSize], sAmt, 3);
				nDataSize += 3;
			}
		}
		if(nDataSize == 0)
			strcpy(sResData, "*");
		else
			sResData[nDataSize] = NULL;

		char szResBuf[200];
		sprintf(szResBuf, "LeftResList='%s',", sResData);
		strcat(szSql, szResBuf);
	}

	sprintf(szBuf, "TotalPlayMin=%d,", pNewData->dbAvator.m_dwTotalPlayMin);
	strcat(szSql, szBuf);

	if(strlen(szSql) > nQueryHeadSize)
	{

		sprintf(szBuf, "WHERE Serial = %d", dwSerial); 
		szSql[strlen(szSql)-1] = ' ';		
		strcat(szSql, szBuf);
	}
	else
	{
		memset( szSql, 0, nQueryHeadSize );
	}

	return true;
}

bool CMainThread::_db_Update_Inven(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, char* pSzQuery)
{
	char szBuf[128] = {0,};
	char* szSql = pSzQuery;
	sprintf(szSql, "UPDATE tbl_inven SET ");
	DWORD nQueryHeadSize = strlen(szSql);
	BYTE byUseInvenNum = one_bag_store_num * pNewData->dbAvator.m_byBagNum;

	for(int i = 0; i < byUseInvenNum; i++)
	{
		if(!pNewData->dbInven.m_List[i].Key.IsFilled())
		{
			if(pOldData->dbInven.m_List[i].Key.IsFilled())
			{
				sprintf(szBuf, "K%d=%d,", i, pNewData->dbInven.m_List[i].Key.CovDBKey());
				strcat(szSql, szBuf);				
			}
		}
		else
		{
			if(pOldData->dbInven.m_List[i].Key.IsFilled())
			{
				if(pNewData->dbInven.m_List[i].Key.CovDBKey() != pOldData->dbInven.m_List[i].Key.CovDBKey())
				{
					sprintf(szBuf, "K%d=%d,", i, pNewData->dbInven.m_List[i].Key.CovDBKey());
					strcat(szSql, szBuf);
				}
				if(pNewData->dbInven.m_List[i].dwDur != pOldData->dbInven.m_List[i].dwDur)
				{					
					sprintf(szBuf, "D%d=%d,", i, pNewData->dbInven.m_List[i].dwDur);
					strcat(szSql, szBuf);
				}
				if(pNewData->dbInven.m_List[i].dwUpt != pOldData->dbInven.m_List[i].dwUpt)
				{					
					sprintf(szBuf, "U%d=%d,", i, pNewData->dbInven.m_List[i].dwUpt);
					strcat(szSql, szBuf);
				}
			}			
			else
			{
				sprintf(szBuf, "K%d=%d,D%d=%d,U%d=%d,", i, pNewData->dbInven.m_List[i].Key.CovDBKey(), i, pNewData->dbInven.m_List[i].dwDur, i, pNewData->dbInven.m_List[i].dwUpt);
				strcat(szSql, szBuf);
			}		
		}	
	}
	
	if(strlen(szSql) > nQueryHeadSize)
	{
		sprintf(szBuf, "WHERE Serial=%d", dwSerial); 
		szSql[strlen(szSql)-1] = ' ';		
		strcat(szSql, szBuf);
	}
	else
	{
		memset( szSql, 0, nQueryHeadSize );
	}

	return true;
}

bool CMainThread::_db_Update_Unit(DWORD dwSerial, _AVATOR_DATA* pNewData, _AVATOR_DATA* pOldData, char* pSzQuery)
{
	char szBuf[128] = {0,};
	char* szSql = pSzQuery;
	sprintf(szSql, "UPDATE tbl_unit SET ");
	DWORD nQueryHeadSize = strlen(szSql);

	for(int i = 0; i < unit_storage_num; i++)
	{
		if(pNewData->dbUnit.m_List[i].byFrame == 0xFF)
		{
			if(pOldData->dbUnit.m_List[i].byFrame != 0xFF)
			{
				sprintf(szBuf, "F_%d=%d,", i, 0xFF);
				strcat(szSql, szBuf);				
			}
		}
		else
		{
			if(pNewData->dbUnit.m_List[i].byFrame != pOldData->dbUnit.m_List[i].byFrame)
			{
				sprintf(szBuf, "F_%d=%d,", i, pNewData->dbUnit.m_List[i].byFrame);
				strcat(szSql, szBuf);
			}
			if(pNewData->dbUnit.m_List[i].dwGauge != pOldData->dbUnit.m_List[i].dwGauge)
			{
				sprintf(szBuf, "Gg_%d=%d,", i, pNewData->dbUnit.m_List[i].dwGauge);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].byPart[unit_part_head] != pOldData->dbUnit.m_List[i].byPart[unit_part_head])
			{
				sprintf(szBuf, "H_%d=%d,", i, pNewData->dbUnit.m_List[i].byPart[unit_part_head]);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].byPart[unit_part_upper] != pOldData->dbUnit.m_List[i].byPart[unit_part_upper])
			{
				sprintf(szBuf, "U_%d=%d,", i, pNewData->dbUnit.m_List[i].byPart[unit_part_upper]);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].byPart[unit_part_lower] != pOldData->dbUnit.m_List[i].byPart[unit_part_lower])
			{
				sprintf(szBuf, "L_%d=%d,", i, pNewData->dbUnit.m_List[i].byPart[unit_part_lower]);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].byPart[unit_part_arms] != pOldData->dbUnit.m_List[i].byPart[unit_part_arms])
			{
				sprintf(szBuf, "A_%d=%d,", i, pNewData->dbUnit.m_List[i].byPart[unit_part_arms]);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].byPart[unit_part_shoulder] != pOldData->dbUnit.m_List[i].byPart[unit_part_shoulder])
			{
				sprintf(szBuf, "S_%d=%d,", i, pNewData->dbUnit.m_List[i].byPart[unit_part_shoulder]);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].byPart[unit_part_back] != pOldData->dbUnit.m_List[i].byPart[unit_part_back])
			{
				sprintf(szBuf, "B_%d=%d,", i, pNewData->dbUnit.m_List[i].byPart[unit_part_back]);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].dwBullet[unit_bullet_arms] != pOldData->dbUnit.m_List[i].dwBullet[unit_bullet_arms])
			{
				sprintf(szBuf, "AB_%d=%d,", i, pNewData->dbUnit.m_List[i].dwBullet[unit_bullet_arms]);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].dwBullet[unit_bullet_shoulder] != pOldData->dbUnit.m_List[i].dwBullet[unit_bullet_shoulder])
			{
				sprintf(szBuf, "SB_%d=%d,", i, pNewData->dbUnit.m_List[i].dwBullet[unit_bullet_shoulder]);
				strcat(szSql, szBuf);
			}

			for(int s = 0; s < max_unit_spare; s++)
			{
				if(pNewData->dbUnit.m_List[i].dwSpare[s] != pOldData->dbUnit.m_List[i].dwSpare[s])
				{
					sprintf(szBuf, "Sp%s_%d=%d,", s, i, pNewData->dbUnit.m_List[i].dwSpare[s]);
					strcat(szSql, szBuf);
				}
			}

			if(pNewData->dbUnit.m_List[i].nKeepingFee != pOldData->dbUnit.m_List[i].nKeepingFee)
			{
				sprintf(szBuf, "KF_%d=%d,", i, pNewData->dbUnit.m_List[i].nKeepingFee);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].nPullingFee != pOldData->dbUnit.m_List[i].nPullingFee)
			{
				sprintf(szBuf, "PF_%d=%d,", i, pNewData->dbUnit.m_List[i].nPullingFee);
				strcat(szSql, szBuf);
			}

			if(pNewData->dbUnit.m_List[i].dwCutTime != pOldData->dbUnit.m_List[i].dwCutTime)
			{
				sprintf(szBuf, "Cut_%d=%d,", i, pNewData->dbUnit.m_List[i].dwCutTime);
				strcat(szSql, szBuf);
			}
		}	
	}
	
	if(strlen(szSql) > nQueryHeadSize)
	{
		sprintf(szBuf, "WHERE Serial=%d", dwSerial); 
		szSql[strlen(szSql)-1] = ' ';		
		strcat(szSql, szBuf);
	}
	else
	{
		memset( szSql, 0, nQueryHeadSize );
	}

	return true;
}

//BYTE CMainThread::db_Update_PvpInfo(DWORD dwSerial, BYTE byLevel, short* pzClassHistory, __int64 dPvpPoint )
//{
//	if ( m_pWorldDB->Update_PvpPointInfo( dwSerial, byLevel, pzClassHistory, dPvpPoint ) == false )
//		return RET_CODE_DB_QUERY_ERR;
//
//	return RET_CODE_SUCCESS;
//}
//
//BYTE CMainThread::db_Load_PvpRank( BYTE byRace, char* szDate, _PVP_RANK_DATA* OUT rankData )
//{
//	if ( m_pWorldDB->Select_PvpRankInfo( byRace, szDate , rankData ) == false )
//		return RET_CODE_DB_QUERY_ERR;
//
//	return RET_CODE_SUCCESS;
//}
