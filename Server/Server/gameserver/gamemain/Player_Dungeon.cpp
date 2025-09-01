#include "stdafx.h"
#include "MainThread.h"
#include "ErrorEventFromServer.h"
#include "BattleDungeonItemfld.h"

void CPlayer::pc_OpenBattleDungeon(DWORD dwItemSerial)
{
	BYTE byErrCode = 0;
	__ITEM* pDungeonItem = NULL;
	CBattleDungeon* pDungeon = NULL;
	_BattleDungeonItem_fld* pDungeonFld = NULL;
	CMapData* pTarMap = NULL;
	WORD wLayerSetIndex = 0xFFFF;
	int i;

	//���簡 �Ϲݸ����� Ȯ��
	if(m_pCurMap->m_pMapSet->m_nMapType != MAP_TYPE_STD)
	{
		byErrCode = error_dungeon_dis_position;//������ ��������� ��ġ
		goto RESULT;
	}

	//�����ۺ���Ȯ��
	pDungeonItem = m_Param.m_dbInven.GetPtrFromSerial(dwItemSerial);
	if(!pDungeonItem)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, dwItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon.. ���¾���(sr: %d)", m_Param.GetCharName(), dwItemSerial);
		byErrCode = error_dungeon_nothing_ditem;//���¾���
		goto RESULT;
	}

	//��������Ʈ���� Ȯ��
	if(pDungeonItem->m_byTableCode != tbl_code_bdungeon)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon.. ����Ʈ�ƴ�", m_Param.GetCharName());
		byErrCode = error_dungeon_not_ditem;
		goto RESULT;
	}
	pDungeonFld = (_BattleDungeonItem_fld*)g_Main.m_tblItemData[tbl_code_bdungeon].GetRecord(pDungeonItem->m_wItemIndex);
	if(!pDungeonFld)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon..���� ���� ������(%d)", m_Param.GetCharName(), pDungeonItem->m_wItemIndex);
		byErrCode = error_dungeon_data_failure;
		goto RESULT;
	}
	pTarMap = g_Main.m_MapOper.GetMap(pDungeonFld->m_strMapCode);
	if(!pTarMap)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon..�������� ����(%s)", m_Param.GetCharName(), pDungeonFld->m_strMapCode);
		byErrCode = error_dungeon_data_failure;
		goto RESULT;
	}
	//����������..
	if(pTarMap->m_pMapSet->m_nMapType != MAP_TYPE_DUNGEON)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon..�������� �ƴ�(%s)", m_Param.GetCharName(), pDungeonFld->m_strMapCode);
		byErrCode = error_dungeon_data_failure;
		goto RESULT;
	}

	//�ش�ʿ� ���� �����̰� �ִ°�..
	for(i = 0; i < pTarMap->m_pMapSet->m_nLayerNum; i++)
	{
		if(!pTarMap->m_ls[i].IsActiveLayer())
		{
			wLayerSetIndex = i;
			break;
		}
	}
	if(wLayerSetIndex == 0xFFFF)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon.. ���̾����", m_Param.GetCharName());
		byErrCode = error_dungeon_full_gate;
		goto RESULT;
	}

	//���� ��������Ʈ�ִ���
	pDungeon = ::FindEmptyDungeonGate(g_BattleDungeon, MAX_BATTLEDUNGEON);
	if(!pDungeon)
	{
		g_Main.m_logSystemError.Write("%s: pc_OpenBattleDungeon.. �����Ʈ����", m_Param.GetCharName());
		byErrCode = error_dungeon_full_gate;
		goto RESULT;
	}

	if(pDungeonFld->m_nGenMob >= pTarMap->m_pMapSet->m_nMonsterSetFileNum)
	{
		g_Main.m_logSystemError.Write("pc_OpenBattleDungeon.. ���ͼ��������ε����� �̻�(%s)", pDungeonFld->m_strCode);
		byErrCode = error_dungeon_data_failure;
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		_battledungeon_create_setdata Data;

		Data.m_pRecordSet = pDungeonFld;
		Data.m_pMap = m_pCurMap;
		Data.m_nLayerIndex = m_wMapLayerIndex;
		m_pCurMap->GetRandPosInRange(m_fCurPos, 10, Data.m_fStartPos);
		Data.pOpener = this;
		Data.pTarMap = pTarMap;
		Data.wTarLayerIndex = wLayerSetIndex;

		if(pDungeon->Create(&Data))
		{
			Emb_DelStorage(_STORAGE_POS::INVEN, pDungeonItem->m_byStorageIndex, false);

			//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pDungeonItem, m_szItemHistoryFileName);

			//LOG..
			g_Main.m_logDungeon.Write("����: %s, �����̸�:%s", m_Param.GetCharName(), pDungeonFld->m_strName);
		}
		else
		{
			byErrCode = error_dungeon_create_fail;
		}
	}

	SendMsg_BattleDungeonOpenResult(byErrCode);
}

void CPlayer::pc_GotoDungeon(WORD wGateIndex, DWORD dwGateSerial)
{
	BYTE byErrCode = 0;
	CBattleDungeon* pDungeon = &g_BattleDungeon[wGateIndex]; 

	//����Ʈ�� �����ϴ���..
	if(!pDungeon->m_bLive)
	{
		byErrCode = error_dungeon_none_exist;//���� ��������Ʈ
		goto RESULT;
	}

	if(pDungeon->m_dwObjSerial != dwGateSerial)
	{
		byErrCode = error_dungeon_none_exist;//���� ��������Ʈ
		goto RESULT;
	}

	//���̵�������..
	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = error_dungeon_trans_map;//���� �̵��ϴ���
		goto RESULT;
	}

	//��ġ Ȯ��..	
	if(move_potal_dist < ::GetSqrt(m_fCurPos, pDungeon->m_fCurPos))
	{
		byErrCode = error_dungeon_not_near_gate;//������ ����ġ
		goto RESULT;
	}

	//���� ������ �ִ���..
	if(!pDungeon->IsEnterAblePlayer(this))
	{
		byErrCode = error_dungeon_refuse_enter;//������� ����
		goto RESULT;
	}

RESULT:

	_ENTER_DUNGEON_NEW_POS NewPos;

	if(byErrCode == 0)
	{
		if(pDungeon->GetEnterNewPos(&NewPos))
		{
			//������ġ �� �� ����..
			memcpy(m_fBeforeDungeonPos, m_fCurPos, sizeof(float)*3);
			m_pBeforeDungeonMap = m_pCurMap;

			CMapData* pTarMap = g_Main.m_MapOper.GetMap(NewPos.byMapCode);

			//��ȯ�����ִٸ� ���Ͻ�Ŵ
			_AnimusReturn();
			
			m_bMapLoading = true;		
			OutOfMap(pTarMap, NewPos.wLayerIndex, mapin_type_dungeon, NewPos.fPos);

			pDungeon->EnterPlayer(this);

			//LOG..
			g_Main.m_logDungeon.Write("����: %s, (������:%s, �����̸�:%s)", m_Param.GetCharName(), pDungeon->m_szOpenerName, ((_BattleDungeonItem_fld*)pDungeon->m_pRecordSet)->m_strName);
		}
		else
		{
			byErrCode = error_dungeon_pos_setting;//��ġ���ý���
		}
	}

	SendMsg_GotoDungeon(byErrCode, NewPos.byMapCode, NewPos.fPos, pDungeon);
}

void CPlayer::pc_OutofDungeon(BYTE byCompleteCode, CBattleDungeon* pDungeon /* = NULL*/)
{
	BYTE byErrCode = 0;// 4;//�߸�����Ż�ڵ� 6;//���̵���
	
	//�̵�������..
	if(GetCurSecNum() == 0xFFFFFFFF || m_bMapLoading)
	{
		byErrCode = error_dungeon_trans_map;
		goto RESULT;
	}

	//������ �ִ���..
	if(m_pCurMap->m_pMapSet->m_nMapType != MAP_TYPE_DUNGEON)
	{
		byErrCode = error_dungeon_not_here;//������ ��������
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		if(byCompleteCode == dungeon_complete_code_mission && pDungeon)
		{//�̼Ǽ����������� �ϼ��ϰ�� ������ ���ش�.
			//����ġ..
			AlterExp( ((_BattleDungeonItem_fld*)pDungeon->m_pRecordSet)->m_nExp );

			//��� �� �޶�Ʈ..
			DWORD dwAddDalant = ((_BattleDungeonItem_fld*)pDungeon->m_pRecordSet)->m_nDalant;
			DWORD dwAddGold = ((_BattleDungeonItem_fld*)pDungeon->m_pRecordSet)->m_nGold;

			AlterMoney(dwAddGold, dwAddDalant);
			SendMsg_ExchangeMoneyResult(0);

			//ITEM HISTORY..
			s_MgrItemHistory.reward_add_money("����", dwAddDalant, dwAddGold, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);			
		}

		//��ȯ�����ִٸ� ���Ͻ�Ŵ
		_AnimusReturn();

		m_bMapLoading = true;
		OutOfMap(m_pBeforeDungeonMap, 0, mapin_type_dungeon, m_fBeforeDungeonPos);
	}

	SendMsg_OutofDungeon(byErrCode, m_pBeforeDungeonMap->m_pMapSet->m_dwIndex, m_fBeforeDungeonPos);
}