#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"
#include "TownItemFld.h"

void CPlayer::pc_NewPosStart(BYTE byMapInMode)
{
	bool bRet = false;

	if(!m_bMapLoading)	//맵로딩중이 아니라면..
		goto RESULT;
	
	bRet = true;

RESULT:

	if(bRet)
	{
		switch(byMapInMode)
		{		
		case mapin_type_start:
			bRet = Create();
			break;

		case mapin_type_move:
		case mapin_type_revival:
		case mapin_type_goto:
		case mapin_type_recall:
		case mapin_type_dungeon:
			bRet = IntoMap(byMapInMode);
			break;
		}
	}

	if(bRet)
	{
		m_bMapLoading = false;
		SetStateFlag(true);
		
		SendMsg_StartNewPos(byMapInMode);
		SendMsg_NewViewOther(byMapInMode);
		NewViewCircleObject();

		if(byMapInMode == mapin_type_start)
			CreateComplete();
	}
	else
	{
		switch(byMapInMode)
		{		
		case mapin_type_start:	//시작실패는 바로 접속을 종료시킴..
			g_Main.m_Network.Close(client_line, m_ObjID.m_wIndex, false);
			break;
		case mapin_type_move:	
			break;
		case mapin_type_revival:	
			break;
		case mapin_type_goto:	
			break;
		case mapin_type_recall:	
			break;
		case mapin_type_dungeon://던젼입장 실패시 바로 나오게..	
			SendMsg_CloseDungeon(dungeon_complete_code_timeout);
			pc_OutofDungeon(dungeon_complete_code_timeout, NULL);
			break;
		}
	}
}

void CPlayer::pc_Revival()
{
	BYTE byErrCode = 0;//1;//시체상태가아님 2;//맵을못읽음 5;//맵이동중
	CMapData* pTarMap = NULL;
	float fPos[3];

	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = 5;//맵이동중
		goto RESULT;
	}

	//시체상태인지 확인
	if(!m_bCorpse)
	{
		byErrCode = 1;//시체상태가아님
		goto RESULT;
	}

	pTarMap = g_Main.m_MapOper.GetPosStartMap(m_Param.GetRaceCode(), fPos);
	if(!pTarMap)
	{
		byErrCode = 2;//맵을못읽음
		goto RESULT;
	}

RESULT:

	bool bEquialZone = false;
	if(byErrCode == 0)
	{
		//소환물이있다면 리턴시킴
		_AnimusReturn();

		m_bCorpse = false;

		m_nModeType = mode_type_demili;
		m_nMoveType = move_type_run;
		SetHP(m_nMaxHP, true);
		SetFP(m_nMaxFP, true);
		SetSP(m_nMaxSP, true);

		//소환물이있다면 리턴시킴
		_AnimusReturn();

		m_bMapLoading = true;
		OutOfMap(pTarMap, 0, mapin_type_revival, fPos);
	}

	SendMsg_Revival(byErrCode, true);
}

void CPlayer::pc_MovePortal(int nPortalIndex)
{
	BYTE byErrCode = 0;//1;//잘못됀포탈번호 2;//포탈과먼위치 3;//잘못됀맵코드 4;//잘못됀포탈코드 5;//연결좌표영역에러 6;//맵이동중 7;//거래중 8;//포탈데이터위치이상
	BYTE byMoveMapCode = 0xFF;
	BYTE byMovePotalIndex = 0xFF;	
	_portal_dummy* pCurPotal = NULL;
	_portal_dummy* pTarPotal = NULL;
	CMapData* pTarMap = NULL;
	float fStartPos[3];	
	int nCnt = 0;
	
	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = 6;//맵이동중
		goto RESULT;
	}

	if(m_pmTrd.bDTradeMode)
	{
		byErrCode = 7;//거래중
		goto RESULT;
	}

	//포털 인덱스 확인
	pCurPotal = m_pCurMap->GetPortal(nPortalIndex);
	if(!pCurPotal)
	{
		byErrCode = 1;//잘못됀포탈번호
		goto RESULT;
	}

//	if(m_pExtDummy_Town->IsInBBox(i, (Vector3f&)fPos))


	//위치 확인..	(포털의 위치 얻기)
/*	if(move_potal_dist < ::GetSqrt(m_fCurPos, pCurPotal->m_pDumPos->m_fCenterPos))
	{
		byErrCode = 2;//포탈과먼위치
		goto RESULT;
	}*/

	//연결됀 맵확인..	=> 다른맵: 맵의 인덱스 알림..
	if(!pCurPotal->m_pPortalRec)
	{
		byErrCode = 4;//잘못됀포탈코드
		goto RESULT;
	}
	pTarMap = g_Main.m_MapOper.GetMap(pCurPotal->m_pPortalRec->m_strLinkMapCode);
	if(!pTarMap)
	{
		byErrCode = 3;//잘못됀맵코드
		goto RESULT;
	}
	byMoveMapCode = pTarMap->m_pMapSet->m_dwIndex;

	//연결포탈확인
	pTarPotal = pTarMap->GetPortal(pCurPotal->m_pPortalRec->m_strLinkPortalCode);
	if(!pTarPotal)
	{
		byErrCode = 4;//잘못됀포탈코드
		goto RESULT;
	}
	byMovePotalIndex = pTarPotal->m_pPortalRec->m_dwIndex;

	//연결좌표계산
	if(!pTarMap->GetRandPosInDummy(pTarPotal->m_pDumPos, fStartPos, true))
	{
		byErrCode = 8;//포탈데이터위치이상
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//소환물이있다면 리턴시킴
		_AnimusReturn();

		m_bMapLoading = true;
		OutOfMap(pTarMap, 0, mapin_type_move, fStartPos);
	}

	SendMsg_MovePortal(byErrCode, byMoveMapCode, byMovePotalIndex, fStartPos, true);
}

void CPlayer::pc_MoveNext(BYTE byMoveType, BYTE byModeType, float* pfCur, float* pfTar)
{
	BYTE byResult = 0;

	if(m_pmTrd.bDTradeMode)
	{
		byResult = error_move_trade_mode;
		goto RESULT;
	}		

	if(m_bCorpse)
	{
		byResult = error_move_corpse_state;
		goto RESULT;
	}
	
	if(m_pmGrg.GetGroggy())
	{
		byResult = error_move_groggy_state;
		goto RESULT;
	}

	if(m_bMapLoading || m_bMoveOut)
	{
		byResult = error_move_map_move;
		goto RESULT;
	}		

	if(m_EP.GetEff_State(_EFF_STATE::Move_Lck))//PARAMETER EDIT (이동제한)
	{
		byResult = error_move_move_lock;
		goto RESULT;
	}	

	if(m_EP.GetEff_State(_EFF_STATE::Run_Lck))//PARAMETER EDIT (뛰기제한)
	{
		byResult = error_move_run_lock;
		goto RESULT;
	}
	
	if(!m_pCurMap->IsMapIn(pfCur))
	{
		byResult = error_move_range_out;
		goto RESULT;
	}
	
	if(byMoveType == move_type_run)
	{
		if(!IsRidingUnit())
		{
			if(GetSP() == 0)
			{
				byResult = error_move_sp_lack;
				goto RESULT;
			}
		}
		else
		{
			if(m_pUsingUnit->wBooster == 0)
			{
				byResult = error_move_sp_lack;
				goto RESULT;
			}
		}
	}

RESULT:

	if(byResult == 0)
	{
		m_nMoveType = byMoveType;
		m_nModeType = byModeType;

		double dSqrt = ::GetSqrt(m_fTarPos, pfTar);

		memcpy(m_fOldPos, m_fCurPos, sizeof(float)*3);
		memcpy(m_fCurPos, pfCur, sizeof(float)*3);//세팅좌표 저장..
		memcpy(m_fTarPos, pfTar, sizeof(float)*3);
		CCharacter::ResetSlot();
		CCharacter::Go();			//move가동..

		bool bSendOther = true;
		if(dSqrt <= 5)
			bSendOther = false;

		SendMsg_MoveNext(bSendOther);

		//반격기해제
		if(m_EP.m_bEff_State[_EFF_STATE::Res_Att])//PARAMETER EDIT (반격기) 
		{
			RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Res_Att);
		}
	}
	else
	{
		SendMsg_MoveError(byResult);
		if(m_bMove)	
		{//이동중이였다면 주위에 스탑을 알림..
			SendMsg_Stop(IsOutExtraStopPos(m_fCurPos));
			CCharacter::Stop();
		}
	}
}

void CPlayer::pc_RealMovPos(float* pfCur)
{
	BYTE byResult = 0;

	if(!m_bMove)
	{
		byResult = error_move_not_move_state;
		goto RESULT;
	}

	if(m_pmTrd.bDTradeMode)
	{
		byResult = error_move_trade_mode;
		goto RESULT;
	}		

	if(m_bCorpse)
	{
		byResult = error_move_corpse_state;
		goto RESULT;
	}		

	if(m_pmGrg.GetGroggy())
	{
		byResult = error_move_groggy_state;
		goto RESULT;
	}

	if(!m_pCurMap->IsMapIn(pfCur))
	{
		byResult = error_move_range_out;
		goto RESULT;
	}

RESULT:

	if(byResult == 0)
	{
		memcpy(m_fOldPos, m_fCurPos, sizeof(float)*3);
		memcpy(m_fCurPos, pfCur, sizeof(float)*3);//세팅좌표 저장..
	}
	else
	{
		SendMsg_MoveError(byResult);
		if(m_bMove)	
		{//이동중이였다면 주위에 스탑을 알림..
			SendMsg_Stop(IsOutExtraStopPos(m_fCurPos));
			CCharacter::Stop();
		}
	}
}

void CPlayer::pc_MoveStop(float* pfCur)
{
	BYTE byResult = 0;

	if(!m_bMove)
	{
		byResult = error_move_not_move_state;
		goto RESULT;
	}

	if(m_pmTrd.bDTradeMode)
	{
		byResult = error_move_trade_mode;
		goto RESULT;
	}		

	if(m_bCorpse)
	{
		byResult = error_move_corpse_state;
		goto RESULT;
	}		

	if(m_pmGrg.GetGroggy())
	{
		byResult = error_move_groggy_state;
		goto RESULT;
	}
	if(!m_pCurMap->IsMapIn(pfCur))
	{
		byResult = error_move_range_out;
		goto RESULT;
	}

RESULT:

	if(byResult == 0)
	{	
		memcpy(m_fOldPos, m_fCurPos, sizeof(float)*3);
		memcpy(m_fCurPos, pfCur, sizeof(float)*3);

		//바로정지하게함..
		SendMsg_Stop(IsOutExtraStopPos(m_fCurPos));
		CCharacter::Stop();
	}
	else
	{
		SendMsg_MoveError(byResult);
		if(m_bMove)	
		{//이동중이였다면 주위에 스탑을 알림..
			SendMsg_Stop(IsOutExtraStopPos(m_fCurPos));
			CCharacter::Stop();
		}
	}
}

void CPlayer::pc_GotoBasePortalRequest(WORD wItemSerial)
{	 
	BYTE byErrCode = 0;//1;//시체상태임 2;//포탈을못읽음 3;//좌표 못얻음 4;//다른종족맵 5;//맵이동중 6;//거래중 7;//포털데이터 위치 이상
	CMapData* pTarMap = NULL;	
	_portal_dummy* pTarPotal = NULL;
	__ITEM* pItem = NULL;
	_TOWNItem_fld* pTownFld = NULL;
	float fPos[3];
	int nCnt = 0;

	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = 5;//맵이동중
		goto RESULT;
	}
	
	//시체상태인지 확인
	if(m_bCorpse)
	{
		byErrCode = 1;//시체상태임
		goto RESULT;
	}

	if(wItemSerial != 0xFFFF)		//아이템을 이용한 맵이동
	{
		pItem = m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
		if(!pItem)
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, wItemSerial, 0);
			byErrCode = 10;//없는 아이템
			goto RESULT;
		}
		if(pItem->m_byTableCode != tbl_code_town)
		{
			byErrCode = 10;//없는 아이템
			goto RESULT;
		}
		pTownFld = (_TOWNItem_fld*)g_Main.m_tblItemData[tbl_code_town].GetRecord(pItem->m_wItemIndex);
		if(!pTownFld)
		{
			byErrCode = 10;//없는 아이템
			goto RESULT;
		}

		pTarMap = g_Main.m_MapOper.GetMap(pTownFld->m_strMapCode);
		if(!pTarMap)
		{
			byErrCode = 11;//잘못됀 데이터
			goto RESULT;
		}

		//포털 인덱스 확인
		pTarPotal = pTarMap->GetPortal(pTownFld->m_strDummyName);
		if(!pTarPotal)
		{
			byErrCode = 2;//포탈을못읽음
			goto RESULT;
		}

		//연결좌표계산
		if(!pTarMap->GetRandPosInDummy(pTarPotal->m_pDumPos, fPos, true))
		{
			byErrCode = 7;//포털데이터 위치 이상
			goto RESULT;
		}
	}
	else						// "/마을" 입력으로이동 하는경우..
	{//무조건 종족위치로..
		pTarMap = g_Main.m_MapOper.GetPosStartMap(m_Param.GetRaceCode(), fPos);
		if(!pTarMap)
		{
			byErrCode = 11;//잘못됀 데이터
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		//소환물이있다면 리턴시킴
		_AnimusReturn();

		m_bMapLoading = true;
		OutOfMap(pTarMap, 0, mapin_type_goto, fPos);

		if(wItemSerial != 0xFFFF)
		{
			Emb_AlterDurPoint(_STORAGE_POS::INVEN, pItem->m_byStorageIndex, -1, false);
		}
	}
	SendMsg_GotoBasePortalResult(byErrCode);
}

void CPlayer::pc_GotoAvatorRequest(char* pszAvatorName)
{
	BYTE byErrCode = 0;//0;//성공//1;//댜른존에 있음 2;//없음 5;//맵이동중 6;// 거래중
	CPlayer* pDster = NULL;
	int i;

	//자산이 운영자인가..
	//dlatl..

	//거래중인지
	if(m_pmTrd.bDTradeMode)
	{
		byErrCode = 6;// 거래중
		goto RESULT;
	}

	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = 5;//맵이동중
		goto RESULT;
	}

	//같은존에 있는가..
	for(i = 0; i < MAX_PLAYER; i++)
	{
		CPlayer* p = &g_Player[i];
		if(!p->m_bLive)
			continue;

		if(!strcmp(p->m_Param.m_dbChar.m_szCharID, pszAvatorName))
		{
			pDster = p;
			goto RESULT;
		}
	}

	byErrCode = 2;//없음

RESULT:

	float fStartPos[3];

	if(byErrCode == 0)
	{
		fStartPos[0] = pDster->m_fCurPos[0];//-10;
		fStartPos[2] = pDster->m_fCurPos[2];//-10;
		fStartPos[1] = pDster->m_fCurPos[1];//-10;

		//소환물이있다면 리턴시킴
		_AnimusReturn();

		m_bMapLoading = true;
		OutOfMap(pDster->m_pCurMap, pDster->m_wMapLayerIndex, mapin_type_recall, fStartPos);
	}
	SendMsg_GotoRecallResult(byErrCode, m_Param.GetMapCode(), fStartPos);
}

void CPlayer::pc_MoveModeChangeRequest(BYTE byMoveType)
{
	m_nMoveType = byMoveType;

	//바로 알림..
	m_tmrAlterState.NextTimeRun();
}
