#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"
#include "TownItemFld.h"

void CPlayer::pc_NewPosStart(BYTE byMapInMode)
{
	bool bRet = false;

	if(!m_bMapLoading)	//�ʷε����� �ƴ϶��..
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
		case mapin_type_start:	//���۽��д� �ٷ� ������ �����Ŵ..
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
		case mapin_type_dungeon://�������� ���н� �ٷ� ������..	
			SendMsg_CloseDungeon(dungeon_complete_code_timeout);
			pc_OutofDungeon(dungeon_complete_code_timeout, NULL);
			break;
		}
	}
}

void CPlayer::pc_Revival()
{
	BYTE byErrCode = 0;//1;//��ü���°��ƴ� 2;//���������� 5;//���̵���
	CMapData* pTarMap = NULL;
	float fPos[3];

	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = 5;//���̵���
		goto RESULT;
	}

	//��ü�������� Ȯ��
	if(!m_bCorpse)
	{
		byErrCode = 1;//��ü���°��ƴ�
		goto RESULT;
	}

	pTarMap = g_Main.m_MapOper.GetPosStartMap(m_Param.GetRaceCode(), fPos);
	if(!pTarMap)
	{
		byErrCode = 2;//����������
		goto RESULT;
	}

RESULT:

	bool bEquialZone = false;
	if(byErrCode == 0)
	{
		//��ȯ�����ִٸ� ���Ͻ�Ŵ
		_AnimusReturn();

		m_bCorpse = false;

		m_nModeType = mode_type_demili;
		m_nMoveType = move_type_run;
		SetHP(m_nMaxHP, true);
		SetFP(m_nMaxFP, true);
		SetSP(m_nMaxSP, true);

		//��ȯ�����ִٸ� ���Ͻ�Ŵ
		_AnimusReturn();

		m_bMapLoading = true;
		OutOfMap(pTarMap, 0, mapin_type_revival, fPos);
	}

	SendMsg_Revival(byErrCode, true);
}

void CPlayer::pc_MovePortal(int nPortalIndex)
{
	BYTE byErrCode = 0;//1;//�߸�����Ż��ȣ 2;//��Ż������ġ 3;//�߸��¸��ڵ� 4;//�߸�����Ż�ڵ� 5;//������ǥ�������� 6;//���̵��� 7;//�ŷ��� 8;//��Ż��������ġ�̻�
	BYTE byMoveMapCode = 0xFF;
	BYTE byMovePotalIndex = 0xFF;	
	_portal_dummy* pCurPotal = NULL;
	_portal_dummy* pTarPotal = NULL;
	CMapData* pTarMap = NULL;
	float fStartPos[3];	
	int nCnt = 0;
	
	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = 6;//���̵���
		goto RESULT;
	}

	if(m_pmTrd.bDTradeMode)
	{
		byErrCode = 7;//�ŷ���
		goto RESULT;
	}

	//���� �ε��� Ȯ��
	pCurPotal = m_pCurMap->GetPortal(nPortalIndex);
	if(!pCurPotal)
	{
		byErrCode = 1;//�߸�����Ż��ȣ
		goto RESULT;
	}

//	if(m_pExtDummy_Town->IsInBBox(i, (Vector3f&)fPos))


	//��ġ Ȯ��..	(������ ��ġ ���)
/*	if(move_potal_dist < ::GetSqrt(m_fCurPos, pCurPotal->m_pDumPos->m_fCenterPos))
	{
		byErrCode = 2;//��Ż������ġ
		goto RESULT;
	}*/

	//����� ��Ȯ��..	=> �ٸ���: ���� �ε��� �˸�..
	if(!pCurPotal->m_pPortalRec)
	{
		byErrCode = 4;//�߸�����Ż�ڵ�
		goto RESULT;
	}
	pTarMap = g_Main.m_MapOper.GetMap(pCurPotal->m_pPortalRec->m_strLinkMapCode);
	if(!pTarMap)
	{
		byErrCode = 3;//�߸��¸��ڵ�
		goto RESULT;
	}
	byMoveMapCode = pTarMap->m_pMapSet->m_dwIndex;

	//������ŻȮ��
	pTarPotal = pTarMap->GetPortal(pCurPotal->m_pPortalRec->m_strLinkPortalCode);
	if(!pTarPotal)
	{
		byErrCode = 4;//�߸�����Ż�ڵ�
		goto RESULT;
	}
	byMovePotalIndex = pTarPotal->m_pPortalRec->m_dwIndex;

	//������ǥ���
	if(!pTarMap->GetRandPosInDummy(pTarPotal->m_pDumPos, fStartPos, true))
	{
		byErrCode = 8;//��Ż��������ġ�̻�
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//��ȯ�����ִٸ� ���Ͻ�Ŵ
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

	if(m_EP.GetEff_State(_EFF_STATE::Move_Lck))//PARAMETER EDIT (�̵�����)
	{
		byResult = error_move_move_lock;
		goto RESULT;
	}	

	if(m_EP.GetEff_State(_EFF_STATE::Run_Lck))//PARAMETER EDIT (�ٱ�����)
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
		memcpy(m_fCurPos, pfCur, sizeof(float)*3);//������ǥ ����..
		memcpy(m_fTarPos, pfTar, sizeof(float)*3);
		CCharacter::ResetSlot();
		CCharacter::Go();			//move����..

		bool bSendOther = true;
		if(dSqrt <= 5)
			bSendOther = false;

		SendMsg_MoveNext(bSendOther);

		//�ݰݱ�����
		if(m_EP.m_bEff_State[_EFF_STATE::Res_Att])//PARAMETER EDIT (�ݰݱ�) 
		{
			RemoveSFContHelpByEffect(cont_param_state, _EFF_STATE::Res_Att);
		}
	}
	else
	{
		SendMsg_MoveError(byResult);
		if(m_bMove)	
		{//�̵����̿��ٸ� ������ ��ž�� �˸�..
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
		memcpy(m_fCurPos, pfCur, sizeof(float)*3);//������ǥ ����..
	}
	else
	{
		SendMsg_MoveError(byResult);
		if(m_bMove)	
		{//�̵����̿��ٸ� ������ ��ž�� �˸�..
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

		//�ٷ������ϰ���..
		SendMsg_Stop(IsOutExtraStopPos(m_fCurPos));
		CCharacter::Stop();
	}
	else
	{
		SendMsg_MoveError(byResult);
		if(m_bMove)	
		{//�̵����̿��ٸ� ������ ��ž�� �˸�..
			SendMsg_Stop(IsOutExtraStopPos(m_fCurPos));
			CCharacter::Stop();
		}
	}
}

void CPlayer::pc_GotoBasePortalRequest(WORD wItemSerial)
{	 
	BYTE byErrCode = 0;//1;//��ü������ 2;//��Ż�������� 3;//��ǥ ������ 4;//�ٸ������� 5;//���̵��� 6;//�ŷ��� 7;//���е����� ��ġ �̻�
	CMapData* pTarMap = NULL;	
	_portal_dummy* pTarPotal = NULL;
	__ITEM* pItem = NULL;
	_TOWNItem_fld* pTownFld = NULL;
	float fPos[3];
	int nCnt = 0;

	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = 5;//���̵���
		goto RESULT;
	}
	
	//��ü�������� Ȯ��
	if(m_bCorpse)
	{
		byErrCode = 1;//��ü������
		goto RESULT;
	}

	if(wItemSerial != 0xFFFF)		//�������� �̿��� ���̵�
	{
		pItem = m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
		if(!pItem)
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, wItemSerial, 0);
			byErrCode = 10;//���� ������
			goto RESULT;
		}
		if(pItem->m_byTableCode != tbl_code_town)
		{
			byErrCode = 10;//���� ������
			goto RESULT;
		}
		pTownFld = (_TOWNItem_fld*)g_Main.m_tblItemData[tbl_code_town].GetRecord(pItem->m_wItemIndex);
		if(!pTownFld)
		{
			byErrCode = 10;//���� ������
			goto RESULT;
		}

		pTarMap = g_Main.m_MapOper.GetMap(pTownFld->m_strMapCode);
		if(!pTarMap)
		{
			byErrCode = 11;//�߸��� ������
			goto RESULT;
		}

		//���� �ε��� Ȯ��
		pTarPotal = pTarMap->GetPortal(pTownFld->m_strDummyName);
		if(!pTarPotal)
		{
			byErrCode = 2;//��Ż��������
			goto RESULT;
		}

		//������ǥ���
		if(!pTarMap->GetRandPosInDummy(pTarPotal->m_pDumPos, fPos, true))
		{
			byErrCode = 7;//���е����� ��ġ �̻�
			goto RESULT;
		}
	}
	else						// "/����" �Է������̵� �ϴ°��..
	{//������ ������ġ��..
		pTarMap = g_Main.m_MapOper.GetPosStartMap(m_Param.GetRaceCode(), fPos);
		if(!pTarMap)
		{
			byErrCode = 11;//�߸��� ������
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		//��ȯ�����ִٸ� ���Ͻ�Ŵ
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
	BYTE byErrCode = 0;//0;//����//1;//�������� ���� 2;//���� 5;//���̵��� 6;// �ŷ���
	CPlayer* pDster = NULL;
	int i;

	//�ڻ��� ����ΰ�..
	//dlatl..

	//�ŷ�������
	if(m_pmTrd.bDTradeMode)
	{
		byErrCode = 6;// �ŷ���
		goto RESULT;
	}

	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = 5;//���̵���
		goto RESULT;
	}

	//�������� �ִ°�..
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

	byErrCode = 2;//����

RESULT:

	float fStartPos[3];

	if(byErrCode == 0)
	{
		fStartPos[0] = pDster->m_fCurPos[0];//-10;
		fStartPos[2] = pDster->m_fCurPos[2];//-10;
		fStartPos[1] = pDster->m_fCurPos[1];//-10;

		//��ȯ�����ִٸ� ���Ͻ�Ŵ
		_AnimusReturn();

		m_bMapLoading = true;
		OutOfMap(pDster->m_pCurMap, pDster->m_wMapLayerIndex, mapin_type_recall, fStartPos);
	}
	SendMsg_GotoRecallResult(byErrCode, m_Param.GetMapCode(), fStartPos);
}

void CPlayer::pc_MoveModeChangeRequest(BYTE byMoveType)
{
	m_nMoveType = byMoveType;

	//�ٷ� �˸�..
	m_tmrAlterState.NextTimeRun();
}
