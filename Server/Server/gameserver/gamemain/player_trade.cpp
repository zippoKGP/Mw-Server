#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "EconomySystem.h"

void CPlayer::pc_BuyItemStore(CItemStore* pStore, BYTE byOfferNum, _buy_store_request_clzo::_list* pList)
{
	int i;
	_buy_offer Offer[trade_item_max_num];
	char szErrCode[64];
	bool bSucc = true;
	BYTE byAddBuffer[_STORAGE_POS::STORAGE_NUM] = {0, };

	if(pStore->m_pExistMap != m_pCurMap)
	{
		sprintf(szErrCode, "�ƹ�Ÿ�� ���� �ʻ� �����ϴ� ������ �ƴմϴ�.");
		bSucc = false;
		goto RESULT;
	}

	//�� ���丮���� ���� üũ
	//����, ���, ����, ����, �ִϸӽ����� ���丮���ڵ带 ����� �־ ���´���..
	for(i = 0; i < byOfferNum; i++)
	{
		byAddBuffer[pList[i].byStorageCode]++;
	}

	for(i = 0; i < _STORAGE_POS::STORAGE_NUM; i++)
	{
		if(m_Param.m_pStoragePtr[i]->GetNumEmptyCon() < byAddBuffer[i])
		{
			sprintf(szErrCode, "���濡 ���� ������ �����մϴ�.");
			bSucc = false;
			goto RESULT;
		}
	}

	for(i = 0; i < byOfferNum; i++)
	{
		Offer[i].byGoodIndex = pList[i].dwGoodSerial;
		Offer[i].byGoodAmount = pList[i].byAmount;
		Offer[i].byStorageCode = pList[i].byStorageCode;
	}

RESULT:

	if(bSucc)
	{
		if(pStore->IsSell(byOfferNum, Offer, m_Param.GetDalant(), m_Param.GetGold(), m_EP.m_fEff_Have[_EFF_HAVE::Trade_Prof], szErrCode))//PARAMETER EDIT (�������� �ŷ������� �̵�)
		{
			for(i = 0; i < byOfferNum; i++)
			{
				//������ ������ �ֱⰡ���� ���丮������..
				if(!::IsStorageCodeWithItemKind(Offer[i].Item.m_byTableCode, pList[i].byStorageCode))
				{
					sprintf(szErrCode, "���������� ���丮����");
					bSucc = false;
					break;
				}

				if(pList[i].byStorageCode == _STORAGE_POS::EQUIP)
				{//�����̶�� �� �ڸ��� �̹� �������� �ִ°�..
					__ITEM* pItem = (__ITEM*)&m_Param.m_dbEquip.m_pStorageList[pList[i].byStorageCode];
					if(pItem->m_bLoad)
					{
						sprintf(szErrCode, "�̹���������");
						bSucc = false;
						break;			
					}
				}

				if(pList[i].byStorageCode == _STORAGE_POS::EQUIP || pList[i].byStorageCode == _STORAGE_POS::EMBELLISH)
				{
					//��밡����������Ȯ��
					if(!::IsItemEquipCivil(Offer[i].Item.m_byTableCode, Offer[i].Item.m_wItemIndex, m_Param.GetRaceSexCode()))
					{
						sprintf(szErrCode, "�����Ҽ����������Դϴ�");
						bSucc = false;
						break;			
					}
					//�������� �ɷ�����.
					if(!IsEffectableEquip(&Offer[i].Item))
					{
						sprintf(szErrCode, "�����ɷº���");
						bSucc = false;
						break;
					}
				}

				else if(pList[i].byStorageCode == _STORAGE_POS::FORCE)
				{	//���� ȿ���� ������ �ִ���..
					for(int s = 0; s < force_storage_num; s++)
					{
						__ITEM* pItem = (__ITEM*)&m_Param.m_dbForce.m_pStorageList[s];

						if(!pItem->m_bLoad)
							continue;

						if(s_pnLinkForceItemToEffect[pItem->m_wItemIndex] == s_pnLinkForceItemToEffect[Offer[i].Item.m_wItemIndex])
						{
							sprintf(szErrCode, "�̹� ����ȿ���� ������ ����");
							bSucc = false;
							break;
						}
					}
				}

				else if(pList[i].byStorageCode == _STORAGE_POS::ANIMUS)
				{	//���� �ִϸӽ��� �ִ���..
					for(int s = 0; s < animus_storage_num; s++)
					{
						__ITEM* pItem = (__ITEM*)&m_Param.m_dbAnimus.m_pStorageList[s];

						if(!pItem->m_bLoad)
							continue;

						if(pItem->m_wItemIndex == Offer[i].Item.m_wItemIndex)
						{
							sprintf(szErrCode, "�̹� ���� �ִϸӽ��� ����");
							bSucc = false;
							break;
						}
					}
				}

				if(!bSucc)
					break;
			}
		}
		else
			bSucc = false;
	}

	if(bSucc)
	{
		AlterMoney(-pStore->GetLastTradeGold(), -pStore->GetLastTradeDalant());				

		for(i = 0; i < byOfferNum; i++)
		{
			Offer[i].wSerial = m_Param.GetNewItemSerial();
			Offer[i].Item.m_wSerial = Offer[i].wSerial;

			Emb_AddStorage(pList[i].byStorageCode, &Offer[i].Item);			
		}

		//ITEM HISTORY..
		s_MgrItemHistory.buy_item(Offer, byOfferNum, pStore->GetLastTradeDalant(), pStore->GetLastTradeGold(), m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);
	}

	SendMsg_BuyItemStoreResult(pStore, bSucc, byOfferNum, Offer, szErrCode);
}

void CPlayer::pc_SellItemStore(CItemStore* pStore, BYTE byOfferNum, _sell_store_request_clzo::_list* pList)
{
	int i;
	_sell_offer Offer[trade_item_max_num];
	char szErrCode[64] = {0,};
	bool bSucc = true;

	if(pStore->m_pExistMap != m_pCurMap)
	{
		sprintf(szErrCode, "�ƹ�Ÿ�� ���� �ʻ� �����ϴ� ������ �ƴմϴ�.");
		bSucc = false;
		goto RESULT;
	}

	for(i = 0; i < byOfferNum; i++)
	{
		//���� �����ϴ��� �˻�..
		Offer[i].pItem = m_Param.m_pStoragePtr[pList[i].byStorageCode]->GetPtrFromSerial(pList[i].wSerial);
		if(!Offer[i].pItem)
		{
			sprintf(szErrCode, "���� �����ϰ��ִ� �������� �ƴմϴ�.");
			bSucc = false;
			goto RESULT;
		}

		if(Offer[i].pItem->m_byTableCode == tbl_code_key)
		{
			sprintf(szErrCode, "������ �ŷ�����.");
			bSucc = false;
			goto RESULT;
		}

		//���ɸ� ����������..
		if(Offer[i].pItem->m_bLock)
		{
			sprintf(szErrCode, "���ɸ� ������");
			bSucc = false;
			goto RESULT;
		}

		//�������� �÷ȴ��� �˻�
		for(int e = 0; e < i; e++)
		{
			if(Offer[e].pItem == Offer[i].pItem)//��ħ..
			{
				sprintf(szErrCode, "�������� �������� �÷Ƚ��ϴ�.");
				bSucc = false;
				goto RESULT;
			}
		}

		Offer[i].bySlotIndex = Offer[i].pItem->m_byStorageIndex;
		Offer[i].byStorageCode = pList[i].byStorageCode;

		//��ø�̶��..
		if(::IsOverLapItem(Offer[i].pItem->m_byTableCode))
		{
			if(pList[i].byAmount > Offer[i].pItem->m_dwDur)
			{
				sprintf(szErrCode, "������������ ���� ������ �����߽��ϴ�");
				bSucc = false;
				goto RESULT;
			}
			Offer[i].byAmount = pList[i].byAmount;
		}
		else
		{
			Offer[i].byAmount = 1;
		}
	}

RESULT:

	if(bSucc)
	{
		if(pStore->IsBuy(byOfferNum, Offer, m_EP.m_fEff_Have[_EFF_HAVE::Trade_Prof], szErrCode)) //PARAMETER EDIT (�������� �ŷ������� �̵�)
		{
			AlterMoney(pStore->GetLastTradeGold(),  pStore->GetLastTradeDalant());

			//ITEM HISTORY..
			s_MgrItemHistory.sell_item(Offer, byOfferNum, pStore->GetLastTradeDalant(), pStore->GetLastTradeGold(),  m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);

			for(int i = 0; i < byOfferNum; i++)
			{
				if(!::IsOverLapItem(Offer[i].pItem->m_byTableCode))	//��ø�̾ƴϸ� �׳� ����
					Emb_DelStorage(pList[i].byStorageCode, Offer[i].bySlotIndex, false);
				else
					Emb_AlterDurPoint(pList[i].byStorageCode, Offer[i].bySlotIndex, -Offer[i].byAmount, true);//�ȱ�
			}				
		}
		else
		{
			bSucc = false;
		}
	}

	SendMsg_SellItemStoreResult(pStore, bSucc, szErrCode);
}

void CPlayer::pc_RepairItemStore(CItemStore* pStore, BYTE byOfferNum, _repair_request_clzo::_list* pList)
{
	int i;
	bool bSucc = true;
	char szErrCode[64] = {0,};
	_repair_offer Offer[trade_item_max_num];

	if(pStore->m_pExistMap != m_pCurMap)
	{
		sprintf(szErrCode, "�ƹ�Ÿ�� ���� �ʻ� �����ϴ� ������ �ƴմϴ�.");
		bSucc = false;
		goto RESULT;
	}

	for(i = 0; i < byOfferNum; i++)
	{
		//���� �����ϴ��� �˻�..
		int nIndex= -1;
		__ITEM* pItem = NULL;

		if(pList[i].byPositon == _STORAGE_POS::INVEN)
		{
			Offer[i].pItem = m_Param.m_dbInven.GetPtrFromSerial(pList[i].wSerial);
			if(!Offer[i].pItem)
			{
				sprintf(szErrCode, "���� �����ϰ��ִ� �������� �ƴմϴ�.");
				bSucc = false;
				goto RESULT;
			}

			Offer[i].bySlotIndex = Offer[i].pItem->m_byStorageIndex;
		}
		else if(pList[i].byPositon == _STORAGE_POS::EQUIP)
		{	
			Offer[i].pItem = m_Param.m_dbEquip.GetPtrFromSerial(pList[i].wSerial);
			if(!Offer[i].pItem)
			{
				sprintf(szErrCode, "���� �����ϰ��ִ� �������� �ƴմϴ�.");
				bSucc = false;
				goto RESULT;
			}

			Offer[i].bySlotIndex = Offer[i].pItem->m_byStorageIndex;
		}
		else
		{
			bSucc = false;
			goto RESULT;

		}

		if(!IsRepairableItem(Offer[i].pItem->m_byTableCode, Offer[i].pItem->m_wItemIndex))
		{
			sprintf(szErrCode, "������ �Ұ����� ������");
			bSucc = false;
			goto RESULT;
		}
	}

RESULT:

	if(bSucc)
	{
		if(pStore->IsRepair(byOfferNum, Offer, m_Param.GetDalant(), m_Param.GetGold(), szErrCode))
		{
			AlterMoney(-pStore->GetLastTradeGold(), -pStore->GetLastTradeDalant());

			//ITEM HISTORY..
			s_MgrItemHistory.repair_item(Offer, byOfferNum, pStore->GetLastTradeDalant(), pStore->GetLastTradeGold(), m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);

			for(i = 0; i < byOfferNum; i++)
			{	
				Emb_AlterDurPoint(pList[i].byPositon, Offer[i].bySlotIndex, Offer[i].wNewDurPoint-Offer[i].pItem->m_dwDur, true);
			}
		}
		else
		{
			bSucc = false;
		}
	}

	SendMsg_RepairItemStoreResult(pStore, byOfferNum, Offer, bSucc, szErrCode);
}

void CPlayer::pc_ExchangeDalantForGold(DWORD dwDalant)
{
	BYTE byErrCode = 0;
	DWORD dwOldDalant = m_Param.GetDalant();
	DWORD dwOldGold = m_Param.GetGold();

	if(dwDalant > dwOldDalant)
	{
		byErrCode = 1;//�޶�Ʈ���� 
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		int nR = ::eGetRate(m_Param.GetRaceCode());

		DWORD dwExcDalant = dwDalant - dwDalant%nR;
		DWORD dwExcGold = dwExcDalant/nR;

		AlterMoney(dwExcGold, -dwExcDalant);

		//ITEM HISTORY..
		s_MgrItemHistory.exchange_money(dwOldDalant, dwOldGold, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);
	}

	SendMsg_ExchangeMoneyResult(byErrCode);
}

void CPlayer::pc_ExchangeGoldForDalant(DWORD dwGold)
{
	BYTE byErrCode = 0;
	DWORD dwOldGold = m_Param.GetGold();
	DWORD dwOldDalant = m_Param.GetDalant();

	if(dwGold > dwOldGold)
	{
		byErrCode = 1;//������ 
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		int nR = ::eGetRate(m_Param.GetRaceCode()); 

		DWORD dwExcDalant = dwGold * nR;

		AlterMoney(-dwGold, dwExcDalant);

		//ITEM HISTORY..
		s_MgrItemHistory.exchange_money(dwOldDalant, dwOldGold, m_Param.GetDalant(), m_Param.GetGold(), m_szItemHistoryFileName);
	}

	SendMsg_ExchangeMoneyResult(byErrCode);
}

//�ŷ���û
void CPlayer::pc_DTradeAskRequest(WORD wDstIndex)
{
	BYTE byErrCode = 0;//1;//�ڽ����̹�Ʈ���̵��� 2;//�Ÿ����� 3;//��ȿ�Ѵ��ƴ� 4;//��밡Ʈ���̵���
	CPlayer* pDst = &g_Player[wDstIndex];

	//������ Ʈ���̵�������
	if(m_pmTrd.bDTradeMode || pDst->m_pmTrd.bDTradeMode)
	{
		byErrCode = 1;//�̹�Ʈ���̵���..
		goto RESULT;
	}

	//������ ��ȿ�� �������..
	if(!pDst->m_bLive || pDst->m_bCorpse || pDst->m_pCurMap != m_pCurMap || pDst->GetCurSecNum() == 0xFFFFFFFF)
	{
		byErrCode = 3;//��ȿ�� ����� �ƴ�
		goto RESULT;
	}

	if(m_byUserDgr != pDst->m_byUserDgr)
	{//�Ϲݵ���̾Ƴ��� �ŷ�����
		byErrCode = 3;//��ȿ�� ����� �ƴ�
		goto RESULT;
	}
	//�Ÿ�üũ
	if(::GetSqrt(pDst->m_fCurPos, m_fCurPos) > 100.0f)
	{
		byErrCode = 2;//�Ÿ��� ��
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{		
		//����� ���
		m_pmTrd.wDTradeDstIndex = pDst->m_ObjID.m_wIndex;
		m_pmTrd.dwDTradeDstSerial = pDst->m_dwObjSerial;

		//��뿡�� ��û���˸�
		pDst->SendMsg_DTradeAskInform(this);	
	}
	SendMsg_DTradeAskResult(byErrCode);
}

//�ŷ���û������ �¶�
void CPlayer::pc_DTradeAnswerRequest(_CLID* pidAsker)
{
	BYTE byErrCode = 0;//1;//��ȿ��ask���ƴ� 2;//asker���̹�Ʈ���̵��� 3;//answer���̹�Ʈ���̵��� 4;//asker���ڽ�����û�߾ƴ� 5;//�Ÿ��� �� 6;//��ȿ�� ����� �ƴ�
	CPlayer* pAsker = &g_Player[pidAsker->wIndex];

	//asker�� �ø����..
	if(pAsker->m_dwObjSerial != pidAsker->dwSerial)
	{
		byErrCode = 1;//��ȿ��ask���ƴ�
		goto RESULT;
	}

	//asker�� ����� �ڽ�����..
	if(pAsker->m_pmTrd.wDTradeDstIndex != m_ObjID.m_wIndex 
		|| pAsker->m_pmTrd.dwDTradeDstSerial != m_dwObjSerial)
	{
		byErrCode = 4;//asker���ڽ�����û�߾ƴ�
		goto RESULT;
	}
	
	//asker�� �̹�Ʈ���̵�������..
	if(m_pmTrd.bDTradeMode || pAsker->m_pmTrd.bDTradeMode)
	{
		byErrCode = 2;//�̹�Ʈ���̵���
		goto RESULT;
	}

	//������ ��ȿ�� �������..
	if(!pAsker->m_bLive || pAsker->m_bCorpse || pAsker->m_pCurMap != m_pCurMap || pAsker->GetCurSecNum() == 0xFFFFFFFF)
	{
		byErrCode = 6; //��ȿ�� ����� �ƴ�
		goto RESULT;
	}

	if(m_byUserDgr != pAsker->m_byUserDgr)
	{//����̃��������� �ŷ�����
		byErrCode = 6;//��ȿ�� ����� �ƴ�
		goto RESULT;
	}

	//�Ÿ�üũ
	if(::GetSqrt(pAsker->m_fCurPos, m_fCurPos) > 100.0f)
	{
		byErrCode = 5; //�Ÿ��� ��
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//�θ��� ����ڸ� ���ŷ�����.. ����� �󽽷Ժθ�ŭ�� �ȼ��մ�
		m_pmTrd.SetDTradeStart(pAsker->m_ObjID.m_wIndex, pAsker->m_dwObjSerial, m_Param.m_dbInven.GetNumEmptyCon());
		pAsker->m_pmTrd.SetDTradeStart(m_ObjID.m_wIndex, m_dwObjSerial, pAsker->m_Param.m_dbInven.GetNumEmptyCon());

		//�θ��� ���ŷ� ������ �˸���.
		SendMsg_DTradeStartInform(pAsker, this);
		pAsker->SendMsg_DTradeStartInform(pAsker, this);
	}
	else
		SendMsg_DTradeAnswerResult(byErrCode);
}

void CPlayer::pc_DTradeCancleRequest()
{
	BYTE byErrCode = 0;//1;//Ʈ���̵�ƴ� 2;//���λ��� 
	CPlayer* pDst= NULL;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//Ʈ���̵�ƴ�
		goto RESULT;
	}

	//���Ή� �����ΰ�.
//	if(m_pmTrd.bDTradeOK)
//	{
//		byErrCode = 2;//���λ���
//		goto RESULT;
//	}

RESULT:

	if(byErrCode == 0)
	{
		//����ڰŷ� �ʱ�ȭ
		m_pmTrd.Init();
		pDst->m_pmTrd.Init();

		SendMsg_DTradeCancleResult(byErrCode);
		pDst->SendMsg_DTradeCancleInform();
	}
	else
	{	//���н� ��������
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeLockRequest()
{
	BYTE byErrCode = 0;//1;//Ʈ���̵�ƴ� 2;//���ɸ����� 3;//���Ȱɸ����� 4;//���Ή»���
	CPlayer* pDst= NULL;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//Ʈ���̵�ƴ�
		goto RESULT;
	}

	//���� Ǯ�����¿����Ѵ�.
	if(m_pmTrd.bDTradeLock)
	{
		byErrCode = 2;//���ɸ�����
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//���� �Ǵ�
		m_pmTrd.bDTradeLock = true;
		//��뿡�� �˸�
		pDst->SendMsg_DTradeLockInform();

		SendMsg_DTradeLockResult(byErrCode);
	}
	else
	{	//���н� ��������
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeAddRequest(BYTE bySlotIndex, BYTE byStorageCode, DWORD dwSerial, BYTE byAmount)
{
	BYTE byErrCode = 0;//1;//Ʈ���̵���ƴ� 2;//�������� 3;//��ȿ�ѽ����� �ƴ� 4;//ä�������� 5;//���¾��� 6;//�߸��¾� 7;//�ŷ�üũǰ��, 9;//���ɸ� ������
	__ITEM* pItem = NULL;
	CPlayer* pDst= NULL;
	_UNIT_DB_BASE::_LIST* pUnitData = NULL; //���� �����ϰ�� ���ֵ����͸� �����Ѵ�.
	int i;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//Ʈ���̵�ƴ�
		goto RESULT;
	}

	//�������ΰ�..
	if(m_pmTrd.bDTradeLock)
	{
		byErrCode = 2;//��������
		goto RESULT;
	}

	//��ȿ�� �����ΰ�..
	if(bySlotIndex >= max_d_trade_item)
	{
		byErrCode = 3;//��ȿ�ѽ����� �ƴ�
		goto RESULT;
	}

	//ä���������ΰ�..
	if(m_pmTrd.DItemNode[bySlotIndex].bLoad)
	{
		byErrCode = 4;//ä��������
		goto RESULT;
	}

	//��ȿ�Ѿ������ΰ�..
	pItem = m_Param.m_pStoragePtr[byStorageCode]->GetPtrFromSerial(dwSerial);
	if(!pItem)
	{
		byErrCode = 5;//���¾���
		goto RESULT;
	}

	//���ɸ� ����������..
	if(pItem->m_bLock)
	{
		byErrCode = 9;//���ɸ� ������
		goto RESULT;
	}

	//������ ������ ���ּ������� üũ�ؾ��Ѵ�.
	if(pItem->m_byTableCode == tbl_code_key)
	{
		//���� ��밡 ����������..
		if(pDst->m_Param.GetRaceSexCode()/2 != race_code_bellato)
		{
			byErrCode = 111;//�ŷ��Ұ�������
			goto RESULT;
		}
		if(unit_storage_num - pDst->m_Param.GetHaveUnitNum() < m_pmTrd.byAddUnitNum+1)
		{
			byErrCode = 112;//����� ���ּ����� �ʰ�
			goto RESULT;
		}
		if(pItem->m_dwLv >= unit_storage_num)
		{
			byErrCode = 113;//�߸��� ����Ű��ȣ( ���ͼ��� �ȵ� ���� )
			goto RESULT;
		}

		pUnitData = &m_Param.m_UnitDB.m_List[pItem->m_dwLv];

		if(pUnitData->byFrame == 0xFF)
		{
			byErrCode = 114;//���� ����( ���ͼ��� �ȵ� ���� )
			goto RESULT;
		}		
	}

	//���� �´���..
	if(::IsOverLapItem(pItem->m_byTableCode))
	{
		if(byAmount > pItem->m_dwDur || byAmount == 0)
		{
			byErrCode = 6;//�߸��¾�
			goto RESULT;
		}
	}

	//�̹� �ø��ǰ�..
	for(i = 0; i < max_d_trade_item; i++)
	{
		_DTRADE_ITEM* p = &m_pmTrd.DItemNode[i];
		
		if(!p->bLoad)
			continue;

		if(p->dwSerial == dwSerial)
		{
			byErrCode = 6;//�̹��߰��Ѿ�����
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		if(byStorageCode == _STORAGE_POS::INVEN)
		{
			if(::IsOverLapItem(pItem->m_byTableCode))
			{
				if(pItem->m_dwDur == byAmount)
					m_pmTrd.byEmptyInvenNum++;
			}
			else
				m_pmTrd.byEmptyInvenNum++;
		}
		m_pmTrd.bySellItemNum++;

		m_pmTrd.DItemNode[bySlotIndex].SetData(byStorageCode, dwSerial, byAmount);
		pDst->SendMsg_DTradeAddInform(bySlotIndex, pItem, byAmount);

		SendMsg_DTradeAddResult(byErrCode);
		if(pItem->m_byTableCode == tbl_code_key && pUnitData)//������ ��� �÷��������� ��ũ..
		{
			m_pmTrd.byAddUnitNum++;
			pDst->SendMsg_DTradeUnitInfoInform(bySlotIndex, pUnitData);
		}
	}
	else
	{	//���н� ��������
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeDelRequest(BYTE bySlotIndex)
{
	BYTE byErrCode = 0;//1;//Ʈ���̵���ƴ� 2;//�������� 3;//��ȿ�ѽ����� �ƴ� 4;//�󽽷�
	CPlayer* pDst = NULL;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//Ʈ���̵�ƴ�
		goto RESULT;
	}

	//�������ΰ�..
	if(m_pmTrd.bDTradeLock)
	{
		byErrCode = 2;//��������
		goto RESULT;
	}

	//��ȿ�� �����ΰ�..
	if(bySlotIndex >= max_d_trade_item)
	{
		byErrCode = 3;//��ȿ�ѽ����� �ƴ�
		goto RESULT;
	}

	//ä���������ΰ�..
	if(!m_pmTrd.DItemNode[bySlotIndex].bLoad)
	{
		byErrCode = 4;//�󽽷�
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		_DTRADE_ITEM* pData = &m_pmTrd.DItemNode[bySlotIndex];
		if(pData->byStorageCode == _STORAGE_POS::INVEN)
		{
			__ITEM* pItem = m_Param.m_pStoragePtr[pData->byStorageCode]->GetPtrFromSerial(pData->dwSerial);
			if(!pItem)
				return;

			if(::IsOverLapItem(pItem->m_byTableCode))
			{
				if(pItem->m_dwDur == pData->byAmount)
					m_pmTrd.byEmptyInvenNum--;
			}
			else
				m_pmTrd.byEmptyInvenNum--;

			if(pItem->m_byTableCode == tbl_code_key)//������ ��� �÷��������� ��ũ..
				m_pmTrd.byAddUnitNum--;
		}
		m_pmTrd.bySellItemNum--;

		m_pmTrd.DItemNode[bySlotIndex].ReleaseData();
		pDst->SendMsg_DTradeDelInform(bySlotIndex);

		SendMsg_DTradeDelResult(byErrCode);
	}
	else
	{	//���н� ��������
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeBetRequest(BYTE byMoneyUnit, DWORD dwBetAmount)
{
	BYTE byErrCode = 0;//1;//�޶�Ʈ���� 2;//������ 3;//�������� 4;//�������� 5;//Ʈ���̵���ƴ�
	CPlayer* pDst = NULL;

	if(!::DTradeEqualPerson(this, &pDst))
	{
		byErrCode = 1;//Ʈ���̵�ƴ�
		goto RESULT;
	}
	
	//���ɸ������ΰ�..
	if(m_pmTrd.bDTradeLock)
	{
		byErrCode = 4;//��������
		goto RESULT;
	}

	//�׸�ŭ�����ϰ��մ���..
	if(byMoneyUnit == money_unit_dalant)
	{
		if(byMoneyUnit > m_Param.GetDalant())
		{
			byErrCode = 1;//�޶�Ʈ����
			goto RESULT;
		}
	}
	else
	{
		if(byMoneyUnit > m_Param.GetGold())
		{
			byErrCode = 2;//������
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		if(byMoneyUnit == money_unit_dalant)
			m_pmTrd.dwDTrade_Dalant = dwBetAmount;
		else
			m_pmTrd.dwDTrade_Gold = dwBetAmount;	

		pDst->SendMsg_DTradeBetInform(byMoneyUnit, dwBetAmount);

		SendMsg_DTradeBetResult(byErrCode);
	}
	else
	{	//���н� ��������
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pDst)
		{
			pDst->m_pmTrd.Init();
			pDst->SendMsg_DTradeCloseInform(0);
		}
	}
}

void CPlayer::pc_DTradeOKRequest()
{
	BYTE byErrCode = 0;//1;//Ʈ���̵���ƴ� 2;//�����¾ƴ� 3;//OK 100//������������ 4; //����������
	CPlayer* pTrader[2] = {this, NULL};
	__ITEM SellItem[2][max_d_trade_item];
	int nSellItemNum[2] = {0, 0};

	if(!::DTradeEqualPerson(this, &pTrader[1]))
	{
		byErrCode = 1;//Ʈ���̵�ƴ�
		goto RESULT;
	}
	//���� ���ɸ���������..
	if(!m_pmTrd.bDTradeLock || !pTrader[1]->m_pmTrd.bDTradeLock)
	{
		byErrCode = 2;//�����¾ƴ�
		goto RESULT;
	}
	if(m_pmTrd.bDTradeOK)
	{
		byErrCode = 3;//OK
		goto RESULT;
	}
	//�Ǹ��ϰ����ϴ� ������ ����� �κ����� ������ �ִ���..
	if(m_pmTrd.bySellItemNum > pTrader[1]->m_pmTrd.byEmptyInvenNum)
	{
		byErrCode = 100; //������������
		goto RESULT;
	}

	//��뵵 �̹� ok�� ���¶�� ���ù����� ��������....
	if(pTrader[1]->m_pmTrd.bDTradeOK)
	{
		for(int p = 0; p < 2; p++)
		{
			int nCnt = 0;
			for(int i = 0; i < max_d_trade_item; i++)
			{
				_DTRADE_ITEM* pNode = &pTrader[p]->m_pmTrd.DItemNode[i];
				if(!pNode->bLoad)
					continue;

				_STORAGE_LIST* pList = pTrader[p]->m_Param.m_pStoragePtr[pNode->byStorageCode];
				
				__ITEM* pItem = pList->GetPtrFromSerial(pNode->dwSerial);
				if(!pItem)
				{
					g_Main.m_logSystemError.Write("CPlayer::pc_DTradeOKRequest().. ���� �ø��� list(%d), serial(%d)", pNode->byStorageCode, pNode->dwSerial);
					byErrCode = 4; //����������						
					goto RESULT;
				}

				//���ɸ� ����������..
				if(pItem->m_bLock)
				{
					byErrCode = 9;//���ɸ� ������
					goto RESULT;
				}

				memcpy(&SellItem[p][nCnt], pItem, sizeof(__ITEM));

				if(::IsOverLapItem(pItem->m_byTableCode) && pItem->m_dwDur > pNode->byAmount)
				{
					SellItem[p][nCnt].m_dwDur = pNode->byAmount;
				}
				nCnt++;

			}
			
			nSellItemNum[p] = nCnt;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		SendMsg_DTradeOKResult(byErrCode);
		pTrader[1]->SendMsg_DTradeOKInform();
		
		m_pmTrd.bDTradeOK = true;
		//��뵵 ok���̸� �ŷ� �������� ó���Ѵ�.
		if(pTrader[1]->m_pmTrd.bDTradeOK && m_pmTrd.bDTradeOK)
		{
			for(int p = 0; p < 2; p++)
			{
				int dp = (p+1)%2;

				//�ڽ��� �ǰ� ����..
				for(int i = 0; i < nSellItemNum[p]; i++)
				{
					if(::IsOverLapItem(SellItem[p][i].m_byTableCode))
					{
						pTrader[p]->Emb_AlterDurPoint(SellItem[p][i].m_pInList->m_nListCode, SellItem[p][i].m_byStorageIndex, -SellItem[p][i].m_dwDur, true);//���ŷ��ȱ�
					}
					else
					{
						pTrader[p]->Emb_DelStorage(SellItem[p][i].m_pInList->m_nListCode, SellItem[p][i].m_byStorageIndex, false);
					}
				}

				//��밡 �ǰ� ���ϱ�..
				WORD wStartSerial = 0xFFFF;
				for(i = 0; i < nSellItemNum[dp]; i++)
				{
					SellItem[dp][i].m_wSerial = pTrader[p]->m_Param.GetNewItemSerial();
					if(wStartSerial == 0xFFFF)
						wStartSerial = SellItem[dp][i].m_wSerial;
					
					//������ ���.. �߰�ó��
					if(SellItem[dp][i].m_byTableCode == tbl_code_key)
					{
						_UNIT_DB_BASE::_LIST* pSrcUnit = &pTrader[dp]->m_Param.m_UnitDB.m_List[SellItem[dp][i].m_dwLv];

						_UNIT_DB_BASE::_LIST Copy_SrcUnit;
						memcpy(&Copy_SrcUnit, pSrcUnit, sizeof(_UNIT_DB_BASE::_LIST));

						//p -> �߰�..
						for(int e = 0; e < unit_storage_num; e++)
						{
							_UNIT_DB_BASE::_LIST* pDstUnit = &pTrader[p]->m_Param.m_UnitDB.m_List[i];
							if(pDstUnit->byFrame == 0xFF)
							{
								//�߰�
								pDstUnit->PushUnit(pSrcUnit);

								//Ű�� ��ȣ����
								SellItem[dp][i].m_dwLv = i;

								//p -> send..
								pTrader[p]->SendMsg_DTradeUnitAddInform(SellItem[dp][i].m_wSerial, pDstUnit);	//ChangeOwner()���� ���� ȣ��..(��뿡�� ���� �����߰��� �˷�����..)

								//���� ������� �����̶��..
								if(pTrader[dp]->m_pUsingUnit == pSrcUnit)
								{
									if(pTrader[dp]->m_pParkingUnit)//�������̿��ٸ�..
									{
										pTrader[dp]->m_pParkingUnit->ChangeOwner(pTrader[p], pDstUnit->bySlotIndex);

										pTrader[p]->m_pParkingUnit = pTrader[dp]->m_pParkingUnit;
										pTrader[dp]->m_pParkingUnit = NULL;
									}
									pTrader[dp]->m_pUsingUnit = NULL;
									pTrader[p]->m_pUsingUnit = pDstUnit;
								}

								//p ITEM HISTORY..(�߰�)
								s_MgrItemHistory.add_unit_after_trade(pTrader[dp]->m_Param.GetCharSerial(), pDstUnit, pTrader[p]->m_szItemHistoryFileName);

								break;
							}
						}

						//dp-> ����..
						pSrcUnit->DelUnit();

						//dp ITEM HISTORY..(����)
						s_MgrItemHistory.del_unit_after_trade(pTrader[p]->m_Param.GetCharSerial(), &Copy_SrcUnit, pTrader[dp]->m_szItemHistoryFileName);
					}

					pTrader[p]->Emb_AddStorage(_STORAGE_POS::INVEN, &SellItem[dp][i]);//�ŷ�
				}

				int nAlterGold = pTrader[dp]->m_pmTrd.dwDTrade_Gold - pTrader[p]->m_pmTrd.dwDTrade_Gold;
				int nAlterDalant = pTrader[dp]->m_pmTrd.dwDTrade_Dalant - pTrader[p]->m_pmTrd.dwDTrade_Dalant;

				pTrader[p]->AlterMoney(nAlterGold, nAlterDalant);									
//				pTrader[p]->SetMoney(pTrader[p]->m_Param.GetGold() + pTrader[dp]->m_pmTrd.dwDTrade_Gold - pTrader[p]->m_pmTrd.dwDTrade_Gold, 
//									pTrader[p]->m_Param.GetDalant() + pTrader[dp]->m_pmTrd.dwDTrade_Dalant - pTrader[p]->m_pmTrd.dwDTrade_Dalant);

				pTrader[p]->SendMsg_DTradeAccomplishInform(true, wStartSerial);

				//ITEM HISTORY..
				s_MgrItemHistory.trade(&SellItem[p][0], nSellItemNum[p], pTrader[p]->m_pmTrd.dwDTrade_Dalant, pTrader[p]->m_pmTrd.dwDTrade_Gold, 
														&SellItem[dp][0], nSellItemNum[dp], pTrader[dp]->m_pmTrd.dwDTrade_Dalant, pTrader[dp]->m_pmTrd.dwDTrade_Gold,
														pTrader[dp]->m_Param.GetCharSerial(), pTrader[p]->m_Param.GetDalant(), pTrader[p]->m_Param.GetGold(),
														pTrader[p]->m_szItemHistoryFileName);
			}

			//�ŷ����� �ʱ�ȭ
			m_pmTrd.Init();
			pTrader[1]->m_pmTrd.Init();
		}
	}
	else//���н� ��������
	{	
		m_pmTrd.Init();
		SendMsg_DTradeCloseInform(0);
		if(pTrader[1])
		{
			pTrader[1]->m_pmTrd.Init();
			pTrader[1]->SendMsg_DTradeCloseInform(0);
		}
	}
}
