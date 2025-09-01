#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "WeaponItemFld.h"
#include "OreItemFld.h"
#include "ResourceItemFld.h"
#include "ErrorEventFromServer.h"

void CPlayer::pc_MineStart(BYTE byMineIndex)
{
	BYTE byErrCode = 0;
	_WeaponItem_fld* pFld = NULL;
	__ITEM* pEquipProp = NULL;
	_res_dummy* pDummy = NULL;
	int nSector = -1;

	//�ڿ�����üũ
	if(byMineIndex >= m_pCurMap->m_nResDumNum)
	{
		byErrCode = error_mining_not_dummy;//����ä������
		goto RESULT;
	}
	pDummy = &m_pCurMap->m_pResDummy[byMineIndex];

	//ä��������� Ȯ��
	if(m_dwMineDelayTime != 0xFFFFFFFF)
	{
		if(m_dwMineStartTime+m_dwMineDelayTime > timeGetTime())
		{
			byErrCode = error_mining_in_motion;//ä�������
			goto RESULT;
		}
	}

	//���̹��� üũ
	nSector = m_pCurMap->GetResDummySector(byMineIndex, m_fCurPos);
	if(nSector == -1)
	{
		byErrCode = error_mining_out_dummy;//ä�����̸� ���
		goto RESULT;
	}

	//ä����� ���� Ȯ��
	pEquipProp = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
	if(!pEquipProp->m_bLoad)
	{
		byErrCode = error_mining_nothing_equip;//ä����� ����
		goto RESULT;
	}

	pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon].m_wItemIndex);
	if(pFld->m_nType != type_mine)
	{
		byErrCode = error_mining_not_tool;//ä����� ����
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//�����ð�üũ
		m_dwMineDelayTime = m_pCurMap->m_pResDummy[byMineIndex].GetDelay(nSector);
		m_dwMineStartTime = timeGetTime();
		m_bMineMode = true;
	}

	//�޽��� ����
	SendMsg_MineStartResult(byErrCode);
}

void CPlayer::pc_MineCancle(WORD wBatterySerial)
{
	BYTE byErrCode = 0;
	DWORD dwCurTime = timeGetTime();
	__ITEM* pEquipProp = NULL;
	__ITEM* pBatteryItem = NULL;
	_WeaponItem_fld* pFld = NULL;
	int nGap = m_dwMineDelayTime-(dwCurTime-m_dwMineStartTime);

	//ä�������ð������� Ȯ��
	if(nGap <= 0 || m_dwMineDelayTime == 0xFFFFFFFF)
	{
		byErrCode = error_mining_over_delay; //�����ð�����
		goto RESULT;
	}

	//ä����� ���� Ȯ��
	pEquipProp = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
	if(!pEquipProp->m_bLoad)
	{
		byErrCode = error_mining_nothing_equip;//ä����񿡷�
		goto RESULT;
	}
	pFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon].m_wItemIndex);
	if(pFld->m_nType != type_mine)
	{
		byErrCode = error_mining_not_tool;//ä����� ����
		goto RESULT;
	}

	//���͸�Ȯ��
	pBatteryItem = m_Param.m_dbInven.GetPtrFromSerial(wBatterySerial);
	if(!pBatteryItem)
	{
		byErrCode = error_mining_nothing_battery; //���׸� ����
		goto RESULT;
	}

RESULT:

	DWORD dwBatteryLeftDurPoint = 0;
	if(byErrCode == 0)
	{
		//ä������� ������ ����
	//	Emb_AlterDurPoint(_STORAGE_POS::EQUIP, tbl_code_weapon, -1);//dlatl..

		//���׸� �뷮 ����
		DWORD dwConsum = dwCurTime-m_dwMineStartTime;
		_STORAGE_LIST* pList = pBatteryItem->m_pInList;
		dwBatteryLeftDurPoint = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pBatteryItem->m_byStorageIndex, -(dwConsum/1000), false);//�Ҹ�
		if(dwBatteryLeftDurPoint == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pBatteryItem, m_szItemHistoryFileName);
		}
	}

	//�����ð� �ʱ�ȭ
	m_dwMineDelayTime = 0xFFFFFFFF;
	m_bMineMode = false;

	SendMsg_MineCancle(byErrCode, dwBatteryLeftDurPoint);
}

void CPlayer::pc_MineComplete(BYTE byOreIndex, WORD wBatterySerial)
{
	BYTE byErrCode = 0;
	int i;
	DWORD dwBatteryLeftDurPoint;
	_OreItem_fld* pFld;
	int nStorageIndex = __NSI;
	BYTE byNewOreIndex;
	WORD wSerial;
	DWORD dwOreDurPoint = 0;
	__ITEM* pEquipProp = NULL;
	__ITEM* pBatteryProp = NULL;

	//ä�������ð��� ����߳� Ȯ��
	if(timeGetTime() < (m_dwMineStartTime+m_dwMineDelayTime))
	{
		byErrCode = error_mining_less_delay; //�����ð�����
		goto RESULT;
	}

	//�ùٸ� �����ε������� Ȯ��(������ ����ε����̿�����)
	pFld = (_OreItem_fld*)g_Main.m_tblItemData[tbl_code_ore].GetRecord(byOreIndex);
	if((DWORD)pFld->m_nOre_Level > ore_high_quality_grade)
	{
		byErrCode = error_mining_select_fail; //������������ 
		goto RESULT;
	}	

	//ä����� ���� Ȯ��
	pEquipProp = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
	if(!pEquipProp->m_bLoad)
	{
		byErrCode = error_mining_nothing_equip;//ä����� ����
		goto RESULT;
	}
	else
	{
		_WeaponItem_fld* pWFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon].m_wItemIndex);
		if(pWFld->m_nType != type_mine)
		{
			byErrCode = error_mining_not_tool;//ä����� ����
			goto RESULT;
		}
	}

	//���͸�Ȯ��
	pBatteryProp = m_Param.m_dbInven.GetPtrFromSerial(wBatterySerial);
	if(!pBatteryProp)
	{
		byErrCode = error_mining_nothing_battery; //���׸� ����
		goto RESULT;
	}
	
	byNewOreIndex = rand()%3 + byOreIndex;

	//�κ��丮�� ����..���� ������ �ִ��� Ȯ��..
	for(i = 0; i < m_Param.GetUseSlot(); i++)
	{
		__ITEM* pItem = &m_Param.m_dbInven.m_pStorageList[i];

		if(!pItem->m_bLoad)
			continue;
		if(pItem->m_byTableCode == tbl_code_ore && pItem->m_wItemIndex == byNewOreIndex)
		{
			if(pItem->m_dwDur >= max_overlap_num)
				continue;
			else
			{
				nStorageIndex = i;
				break;
			}
		}
	}

	//������ �ִ°��� �Ƹ��� ���� ���ϰ��̶�� ������� �־�߉´�.
	if(nStorageIndex == __NSI)
	{
		if(m_Param.m_dbInven.GetIndexEmptyCon() == __NSI)
		{
			byErrCode = error_mining_full_inven; //������������
			goto RESULT;
		}
	}

RESULT:	

	if(byErrCode == 0)
	{
		if(nStorageIndex != __NSI)	//������ ���� �κ��丮�� �ִٸ�..
		{
			dwOreDurPoint = Emb_AlterDurPoint(_STORAGE_POS::INVEN, nStorageIndex, 1, true);//ä��
			wSerial = m_Param.m_dbInven.m_pStorageList[nStorageIndex].m_wSerial;
		}
		else
		{
			__ITEM NewProp;

			NewProp.m_byTableCode = tbl_code_ore;
			NewProp.m_dwDur = 1;
			NewProp.m_wItemIndex = byNewOreIndex;
			NewProp.m_dwLv = __DF_UPT;
			NewProp.m_wSerial = m_Param.GetNewItemSerial();

			Emb_AddStorage(_STORAGE_POS::INVEN, &NewProp);
			wSerial = NewProp.m_wSerial;
			dwOreDurPoint = 1;
		}

		//ä������� ������ ����
	//	Emb_AlterDurPoint(_STORAGE_POS::EQUIP, tbl_code_weapon, -1);//dlatl..

		//���׸� �뷮 ����
		dwBatteryLeftDurPoint = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pBatteryProp->m_byStorageIndex, -(m_dwMineDelayTime/1000), false);
		if(dwBatteryLeftDurPoint == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pBatteryProp, m_szItemHistoryFileName);
		}
	}

	//�����ð� �ʱ�ȭ
	m_dwMineDelayTime = 0xFFFFFFFF;
	m_bMineMode = false;

	SendMsg_MineCompleteResult(byErrCode, byNewOreIndex, wSerial, dwOreDurPoint, dwBatteryLeftDurPoint);		
}

void CPlayer::pc_OreCutting(WORD wOreSerial, BYTE byProcessNum)
{	
	BYTE byErrCode = 0;
	__ITEM* pOreItem = NULL;
	int nKindNum = 0;
	_OreItem_fld* pOreFld = NULL;
	int i;
	DWORD dwCost = 0;

	//�����ӽð��� ������ֳ� Ȯ��
	if(m_Param.GetResBufferNum() > 0)
	{
		byErrCode = 4; //�����ӽð��� �Ⱥ�� 
		goto RESULT;
	}

	//���濡 �ش� ������ �ִ��� Ȯ��
	pOreItem = m_Param.m_dbInven.GetPtrFromSerial(wOreSerial);
	if(!pOreItem)
	{
		byErrCode = 1; //���� ����, �ø������
		goto RESULT;
	}	
	if(pOreItem->m_byTableCode != tbl_code_ore)
	{
		byErrCode = 1; //���� ����, �ø������
		goto RESULT;
	}
	pOreFld = (_OreItem_fld*)g_Main.m_tblItemData[tbl_code_ore].GetRecord(pOreItem->m_wItemIndex);

	//���������� ������������ ������ Ȯ��
	if(byProcessNum > pOreItem->m_dwDur)
	{
		byErrCode = 2; //��������
		goto RESULT;
	}

	//�������Ȯ��
	dwCost = pOreFld->m_nStdPrice*byProcessNum;
	if(dwCost > m_Param.GetDalant())
	{
		byErrCode = 3;//�����ݺ���
		goto RESULT;
	}
	
RESULT:

	DWORD dwLeftOreNum;
	if(byErrCode == 0)
	{
		//�����ڿ��� ������ ����
		for(i = 0; i < byProcessNum; i++)
		{
			nKindNum = ::rand()%(pOreFld->m_nmax_C_random - pOreFld->m_nmin_C_random + 1)+pOreFld->m_nmin_C_random;
			for(int k = 0; k < nKindNum; k++)
			{
				//0x7FFF7FFF ����..
				DWORD dwR1 = ::rand();
				DWORD dwRand = (dwR1<<16)+::rand();

				DWORD dwResIndex = g_Main.m_tblOreCutting.GetOreIndexFromRate(pOreFld->m_dwIndex, dwRand);
				if(m_Param.m_wCuttingResBuffer[dwResIndex] < 0xFF)	//0xFF�� ������ ����..
					m_Param.m_wCuttingResBuffer[dwResIndex]++;
			}
		}

		//���� ����
		dwLeftOreNum = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pOreItem->m_byStorageIndex, -byProcessNum, true);//����

		//�޶�Ʈ ����
		AlterMoney(0, -dwCost);

		//ITEM HISTORY..
		s_MgrItemHistory.cut_item(pOreItem, byProcessNum, m_Param.m_wCuttingResBuffer, dwCost, m_Param.GetDalant(), m_szItemHistoryFileName);

		//���忡 ������Ʈ..	
		if(m_pUserDB)
		{
			int nCnt = 0;
			_CUTTING_DB_BASE::_LIST BufferList[left_cutting_num];
			for(i = 0; i < ::GetMaxResKind(); i++)
			{
				if(m_Param.m_wCuttingResBuffer[i] > 0)
				{
					BufferList[nCnt].byResIndex = (BYTE)i;
					BufferList[nCnt].byAmt = (BYTE)m_Param.m_wCuttingResBuffer[i];
					nCnt++;
				}
			}

			m_pUserDB->Update_CuttingPush(nCnt, BufferList);	
		}
	}

	//�߰� �� �������
	SendMsg_OreCuttingResult(byErrCode, (BYTE)dwLeftOreNum, dwCost);
}

void CPlayer::pc_OreIntoBag(WORD wResIndex, WORD wSerial, BYTE byAddAmount)
{
	BYTE byErrCode = 0;
	__ITEM* pResItem = NULL;
	WORD wNewSerial = wSerial;

	//�߰��Ϸ��� ������ ���������̳����� Ȯ��
	if(byAddAmount > m_Param.m_wCuttingResBuffer[wResIndex])
	{
		byErrCode = 2;//�ڿ����ų�����
		goto RESULT;
	}

	//�κ��� ���� �߰��ϴ� �Ŷ�� ����Ȯ��
	if(wSerial == 0xFFFF)
	{
		if(m_Param.m_dbInven.GetIndexEmptyCon() == __NSI)
		{
			byErrCode = 4;//�����������
			goto RESULT;	
		}
	}

	//������ �ڿ����� �߰��ϴ°Ŷ�� �ø���� ����, �����߰��� �����Ѱ��� Ȯ��
	if(wSerial != 0xFFFF)
	{
		pResItem = m_Param.m_dbInven.GetPtrFromSerial(wSerial);
		if(!pResItem)
		{
			byErrCode = 5;//���濡���½���
			goto RESULT;	
		}
		if(pResItem->m_byTableCode != tbl_code_res || pResItem->m_wItemIndex != wResIndex)
		{
			byErrCode = 7;//����������̴ٸ�
			goto RESULT;	
		}

		int nCurLoadAmonut = pResItem->m_dwDur;
		if(nCurLoadAmonut+byAddAmount > max_overlap_num)
		{
			byErrCode = 6;//���Դ�뷮�ʰ�
			goto RESULT;				
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		//������������
		m_Param.m_wCuttingResBuffer[wResIndex] -= byAddAmount;

		//�κ��丮 ���� �� ���� �߰��Ŵ� �Ŷ�� �ø����� �ӽ������Ѵ�.
		if(pResItem)
		{
			Emb_AlterDurPoint(_STORAGE_POS::INVEN, pResItem->m_byStorageIndex, byAddAmount, true);//����
		}
		else
		{
			__ITEM NewItem;
			NewItem.m_byTableCode = tbl_code_res;
			NewItem.m_wItemIndex = wResIndex;
			NewItem.m_dwDur = byAddAmount;
			NewItem.m_dwLv = __DF_UPT;
			NewItem.m_wSerial = m_Param.GetNewItemSerial();

			Emb_AddStorage(_STORAGE_POS::INVEN, &NewItem);
			wNewSerial = NewItem.m_wSerial;
		}

		//���忡 ������Ʈ..	
		if(m_pUserDB)
			m_pUserDB->Update_CuttingTrans(wResIndex, m_Param.m_wCuttingResBuffer[wResIndex]);	
	}

	//�޽��� ����
	SendMsg_OreIntoBagResult(byErrCode, wNewSerial);
}

void CPlayer::pc_CuttingComplete(BYTE byNpcRace)
{
	DWORD dwSumGold = 0;
	bool bClear = false;

	//�����ȉ°����ڿ��� �����ִٸ� ���� ȯ���Ѵ�.
	for(int i = 0; i < ::GetMaxResKind(); i++)
	{
		if(m_Param.m_wCuttingResBuffer[i] > 0)
		{
			_ResourceItem_fld* pFld = (_ResourceItem_fld*)g_Main.m_tblItemData[tbl_code_res].GetRecord(i);
			int* pGold = &pFld->m_nAncStdPrice;

			int nR = 2;
			if(byNpcRace != m_Param.GetRaceSexCode()/2)
				nR = 1;

			dwSumGold += (pGold[byNpcRace]/nR * m_Param.m_wCuttingResBuffer[i]);		
			bClear = true;
		}
	}

	if(bClear)
	{
		AlterMoney(dwSumGold, 0);
	
		//ITEM HISTORY..
		s_MgrItemHistory.cut_clear_item(m_Param.m_wCuttingResBuffer, dwSumGold, m_Param.GetGold(), m_szItemHistoryFileName);
	}

	m_Param.InitResBuffer();	

	//���忡 ������Ʈ..	
	if(m_pUserDB)
		m_pUserDB->Update_CuttingEmpty();	

	SendMsg_CuttingCompleteResult();
}


