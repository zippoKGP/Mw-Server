#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"

void CPlayer::pc_ForceInvenChange(_STORAGE_POS_INDIV* pItem, WORD wReplaceSerial)
{	
	BYTE byErrCode = 0; //1; //������������ 2; //��ü����̼����������� 3; //��ü����̴ٸ� 4;//�����κ����̹����� 5;//�̵���ų ��������
	__ITEM* pStartItem = NULL;
	__ITEM* pTarItem = NULL;
	_STORAGE_LIST* pStartList = m_Param.m_pStoragePtr[pItem->byStorageCode];
	_STORAGE_LIST* pTargetList = NULL;
	
	if(pItem->byStorageCode == _STORAGE_POS::INVEN)
		pTargetList = m_Param.m_pStoragePtr[_STORAGE_POS::FORCE];
	else if(pItem->byStorageCode == _STORAGE_POS::FORCE)
		pTargetList = m_Param.m_pStoragePtr[_STORAGE_POS::INVEN];
	else
		return;

	//���������ϴ��� Ȯ��
	pStartItem = pStartList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pStartItem)
	{
		byErrCode = 1; //������������
		goto RESULT;
	}
	if(pStartItem->m_byTableCode != tbl_code_fcitem)
	{
		byErrCode = 1; //������������
		goto RESULT;
	}

	//��ü��� ������ ������������ Ȯ��..
	if(wReplaceSerial != 0xFFFF)
	{
		pTarItem = pTargetList->GetPtrFromSerial(wReplaceSerial);
		if(!pTarItem)
		{
			byErrCode = 2; //��ü����̼�����������
			goto RESULT;
		}
		if(pTarItem->m_byTableCode != tbl_code_fcitem)
		{
			byErrCode = 2; //��ü����̼�����������
			goto RESULT;
		}
		if(s_pnLinkForceItemToEffect[pStartItem->m_wItemIndex] != s_pnLinkForceItemToEffect[pTarItem->m_wItemIndex])
		{
			byErrCode = 3; //��ü����̴ٸ�
			goto RESULT;
		}
	}

	//��ü�� �ƴϸ� �̵��Ұ��� �����κ��̶�� �������� �̹��ִ����� ���Ȯ��
	if(wReplaceSerial == 0xFFFF && 	pItem->byStorageCode == _STORAGE_POS::INVEN)
	{
		for(int i = 0; i < pTargetList->m_nUsedNum; i++)
		{
			__ITEM* pItem = (__ITEM*)&pTargetList->m_pStorageList[i];

			if(!pItem->m_bLoad)
				continue;

			if(s_pnLinkForceItemToEffect[pItem->m_wItemIndex] == s_pnLinkForceItemToEffect[pStartItem->m_wItemIndex])
			{
				byErrCode = 4;//�����κ����̹�����
				goto RESULT;
			}
		}
		if(pTargetList->GetIndexEmptyCon() == __NSI)
		{
			byErrCode = 5;//�̵���ų ��������
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM StartItem;
		memcpy(&StartItem, pStartItem, sizeof(__ITEM));
		
		//�������ϴ� ����
		Emb_DelStorage(pStartList->m_nListCode, pStartItem->m_byStorageIndex, false);

		//��ü��� ��ü�Ұ��� ä��� ����
		if(pTarItem)
		{
			__ITEM TarItem;
			memcpy(&TarItem, pTarItem, sizeof(__ITEM));

			Emb_AddStorage(pStartList->m_nListCode, &TarItem);
			Emb_DelStorage(pTargetList->m_nListCode, pTarItem->m_byStorageIndex, false);
		}

		//�̵����� ä��
		Emb_AddStorage(pTargetList->m_nListCode, &StartItem);
	}

	SendMsg_ForceInvenChange(byErrCode);
}

void CPlayer::pc_AnimusInvenChange(_STORAGE_POS_INDIV* pItem, WORD wReplaceSerial)
{
	BYTE byErrCode = 0; //1; //������������ 2; //��ü����̼����������� 3; //��ü����̴ٸ� 4;//�����κ����̹����� 5;//�̵���ų �������� 6;//��ȯ����..
	__ITEM* pStartItem = NULL;
	__ITEM* pTarItem = NULL;
	_STORAGE_LIST* pStartList = m_Param.m_pStoragePtr[pItem->byStorageCode];
	_STORAGE_LIST* pTargetList = NULL;
	
	if(pItem->byStorageCode == _STORAGE_POS::INVEN)
		pTargetList = m_Param.m_pStoragePtr[_STORAGE_POS::ANIMUS];
	else if(pItem->byStorageCode == _STORAGE_POS::ANIMUS)
		pTargetList = m_Param.m_pStoragePtr[_STORAGE_POS::INVEN];
	else
		return;

	//���������ϴ��� Ȯ��
	pStartItem = pStartList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pStartItem)
	{
		byErrCode = 1; //������������
		goto RESULT;
	}
	if(pStartItem->m_byTableCode != tbl_code_animus)
	{
		byErrCode = 1; //������������
		goto RESULT;
	}

	//���� ��ȯ���� �ִϸӽ�����..
	if(pStartItem == m_pRecalledAnimusItem)	
	{
		byErrCode = 6; //��ȯ����
		goto RESULT;
	}

	//��ü��� ������ ������������ Ȯ��..
	if(wReplaceSerial != 0xFFFF)
	{
		pTarItem = pTargetList->GetPtrFromSerial(wReplaceSerial);
		if(!pTarItem)
		{
			byErrCode = 2; //��ü����̼�����������
			goto RESULT;
		}
		if(pTarItem->m_byTableCode != tbl_code_animus)
		{
			byErrCode = 2; //��ü����̼�����������
			goto RESULT;
		}
		if(pTarItem->m_wItemIndex != pStartItem->m_wItemIndex)
		{
			byErrCode = 3; //��ü����̴ٸ�
			goto RESULT;
		}	
		if(pTarItem == m_pRecalledAnimusItem)	//���� ��ȯ���� �ִϸӽ�����..
		{
			byErrCode = 6; //��ȯ����
			goto RESULT;
		}
	}

	//��ü�� �ƴϸ� �̵��Ұ��� �����κ��̶�� �������� �̹��ִ����� ���Ȯ��
	if(wReplaceSerial == 0xFFFF && 	pItem->byStorageCode == _STORAGE_POS::INVEN)
	{
		for(int i = 0; i < pTargetList->m_nUsedNum; i++)
		{
			__ITEM* pItem = (__ITEM*)&pTargetList->m_pStorageList[i];

			if(!pItem->m_bLoad)
				continue;

			if(pItem->m_wItemIndex == pStartItem->m_wItemIndex)
			{
				byErrCode = 4;//�����κ����̹�����
				goto RESULT;
			}
		}
		if(pTargetList->GetIndexEmptyCon() == __NSI)
		{
			byErrCode = 5;//�̵���ų ��������
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM StartItem;
		memcpy(&StartItem, pStartItem, sizeof(__ITEM));
		
		//�������ϴ� ����
		Emb_DelStorage(pStartList->m_nListCode, pStartItem->m_byStorageIndex, false);

		//��ü��� ��ü�Ұ��� ä��� ����
		if(pTarItem)
		{
			__ITEM TarItem;
			memcpy(&TarItem, pTarItem, sizeof(__ITEM));

			Emb_AddStorage(pStartList->m_nListCode, &TarItem);
			Emb_DelStorage(pTargetList->m_nListCode, pTarItem->m_byStorageIndex, false);
		}

		//�̵����� ä��
		Emb_AddStorage(pTargetList->m_nListCode, &StartItem);
	}

	SendMsg_AnimusInvenChange(byErrCode);
}

void CPlayer::pc_PotionSocketChange(_STORAGE_POS_INDIV* pItem, _STORAGE_POS_INDIV* pTarPos)
{
	BYTE byErrCode = 0;// 1;//��밡�ɰ���ƴ� 2;//������������ 3;//���Ǿƴ� 4;//�ű��������
	_STORAGE_LIST* pListFrom = NULL;
	_STORAGE_LIST* pListTo = NULL;
	__ITEM* pPotionItem = NULL;
	__ITEM* pTarPotionItem = NULL;	//���ҽ�.. Ÿ�� ������..

	//��ġüũ
	if(pItem->byStorageCode == _STORAGE_POS::INVEN)
		pListFrom = &m_Param.m_dbInven;
	else if(pItem->byStorageCode == _STORAGE_POS::BELT)
		pListFrom = &m_Param.m_dbBelt;
	if(pTarPos->byStorageCode == _STORAGE_POS::INVEN)
		pListTo = &m_Param.m_dbInven;
	else if(pTarPos->byStorageCode == _STORAGE_POS::BELT)
		pListTo = &m_Param.m_dbBelt;

	//����Ȯ��
	pPotionItem = pListFrom->GetPtrFromSerial(pItem->wItemSerial);
	if(!pPotionItem)
	{
		byErrCode = 2;//������������
		goto RESULT;
	}

	//�������� Ȯ��
	if(pPotionItem->m_byTableCode != tbl_code_potion)
	{
		byErrCode = 3;//���Ǿƴ�
		goto RESULT;
	}

	//�ű����Ȯ��
	if(pTarPos->wItemSerial == 0xFFFF)
	{
		if(pListTo->GetIndexEmptyCon() == __NSI)
		{
			byErrCode = 4;//�ű��������
			goto RESULT;
		}
	}
	else
	{
		pTarPotionItem = pListTo->GetPtrFromSerial(pTarPos->wItemSerial);
		if(!pTarPotionItem)
		{
			byErrCode = 2;//������������
			goto RESULT;
		}

		//�������� Ȯ��
		if(pTarPotionItem->m_byTableCode != tbl_code_potion)
		{
			byErrCode = 3;//���Ǿƴ�
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		if(pTarPos->wItemSerial == 0xFFFF)
		{
			//From�� ����
			Emb_DelStorage(pListFrom->m_nListCode, pPotionItem->m_byStorageIndex, false);

			//To�� ä���.
			Emb_AddStorage(pListTo->m_nListCode, pPotionItem);
		}
		else
		{
			//�ΰ��� ����..
			Emb_DelStorage(pListFrom->m_nListCode, pPotionItem->m_byStorageIndex, false);
			Emb_DelStorage(pListTo->m_nListCode, pTarPotionItem->m_byStorageIndex, false);

			//���� ä���..
			Emb_AddStorage(pListFrom->m_nListCode, pTarPotionItem);	
			Emb_AddStorage(pListTo->m_nListCode, pPotionItem);			
		}
	}
	
	SendMsg_PotionSocketChange(byErrCode);
}

void CPlayer::pc_PotionSeparation(BYTE byStartPos, BYTE byTargetPos, WORD wSerial, BYTE byAmount)
{
	char sRetCode = 0; //-5;	//���� �ø���
	__ITEM* pPotion = NULL;
	//�׷� �ø����� �ִ��� Ȯ��

	pPotion = m_Param.m_pStoragePtr[byStartPos]->GetPtrFromSerial(wSerial);
	if(!pPotion)
	{
		sRetCode = -5;	//���� �ø���
		goto RESULT;
	}

	//�������� Ȯ��
	if(pPotion->m_byTableCode != tbl_code_potion)
	{
		sRetCode = -3;	//���Ǿƴ�
		goto RESULT;
	}		

	//�����̵����� Ȯ��
	if(pPotion->m_dwDur <= byAmount)
	{
		sRetCode = -2;	//�̵�
		goto RESULT;
	}

	//���� �κ��丮�� �ִ��� Ȯ��
	if(m_Param.m_pStoragePtr[byTargetPos]->GetIndexEmptyCon() == __NSI)
	{
		sRetCode = -4;	//���θ��� ������ ����
		goto RESULT;
	}

RESULT:

	DWORD dwRemain;
	__ITEM Con;

	if(sRetCode == 0)
	{
		dwRemain = Emb_AlterDurPoint(byStartPos, pPotion->m_byStorageIndex, -byAmount, true);//����

		Con.m_byTableCode = tbl_code_potion;
		Con.m_wItemIndex = pPotion->m_wItemIndex;
		Con.m_wSerial = m_Param.GetNewItemSerial();
		Con.m_dwDur = byAmount;

		Emb_AddStorage(byTargetPos, &Con);
	}

	SendMsg_PotionSeparation(wSerial, dwRemain, Con.m_wSerial, byAmount, sRetCode);
}

void CPlayer::pc_PotionDivision(BYTE byStartPos, BYTE byTargetPos, WORD wSerial, WORD wTarSerial, BYTE byAmount)
{
	char sRetCode = 0;
	__ITEM* pStartPotion = NULL;
	__ITEM* pTargetPotion = NULL;

	pStartPotion = m_Param.m_pStoragePtr[byStartPos]->GetPtrFromSerial(wSerial);
	if(!pStartPotion)
	{
		sRetCode = -5;//���� �ø���
		goto RESULT;
	}

	pTargetPotion = m_Param.m_pStoragePtr[byTargetPos]->GetPtrFromSerial(wTarSerial);
	if(!pTargetPotion)
	{
		sRetCode = -5;//���� �ø���
		goto RESULT;
	}

	//�������� Ȯ��
	if(pStartPotion->m_byTableCode != tbl_code_potion || pTargetPotion->m_byTableCode != tbl_code_potion)
	{
		sRetCode = -3;//���Ǿƴ�
		goto RESULT;
	}

	//���� �������� Ȯ��
	if(pStartPotion->m_wItemIndex != pTargetPotion->m_wItemIndex)
	{
		sRetCode = -6;	//���� ������ �ƴ�
		goto RESULT;
	}

	//�� ���� ���� �̵��ϴ��� Ȯ��
	if(pStartPotion->m_dwDur < byAmount)
	{
		sRetCode = -4;	//�ʰ��̵�
		goto RESULT;
	}

	if(pTargetPotion->m_dwDur+byAmount > max_overlap_num)
	{
		sRetCode = -4;	//�ʰ��̵�
		goto RESULT;
	}

RESULT:

	BYTE byParentLeft, byChildLeft;
	if(sRetCode == 0)
	{
		byParentLeft = Emb_AlterDurPoint(byStartPos, pStartPotion->m_byStorageIndex, -byAmount, true);//����
		byChildLeft = Emb_AlterDurPoint(byTargetPos, pTargetPotion->m_byStorageIndex, byAmount, true);//����
	}	

	SendMsg_PotionDivision(wSerial, byParentLeft, wTarSerial, byChildLeft, sRetCode);
}

void CPlayer::pc_ResSeparation(WORD wStartSerial, BYTE byMoveAmount)
{
	BYTE byErrCode = 0;
	__ITEM* pOre = NULL;

	//���濡 �ִ��� �˻�
	pOre = m_Param.m_dbInven.GetPtrFromSerial(wStartSerial);
	if(!pOre)
	{
		byErrCode = 1; // �׷��ø������
		goto RESULT;
	}

	//��ø����..
	if(!::IsOverLapItem(pOre->m_byTableCode))
	{
		byErrCode = 3; //��ø�ƴ�
		goto RESULT;
	}

	//�����̵����� �˻�
	if(pOre->m_dwDur <= byMoveAmount)
	{
		byErrCode = -2;	//�����̵���
		goto RESULT;
	}

	//���� �κ��丮�� �ִ��� Ȯ��
	if(m_Param.m_dbInven.GetNumEmptyCon() <= 0)
	{
		byErrCode = 4;	//�����κ��丮����
		goto RESULT;
	}

RESULT:

	__ITEM NewOre;

	if(byErrCode == 0)
	{
		//���ۿ��� ���
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pOre->m_byStorageIndex, -byMoveAmount, true);//����

		//���ο� �����߰�
		NewOre.m_byTableCode = pOre->m_byTableCode;
		NewOre.m_wItemIndex = pOre->m_wItemIndex;
		NewOre.m_wSerial = m_Param.GetNewItemSerial();
		NewOre.m_dwDur = byMoveAmount;

		Emb_AddStorage(_STORAGE_POS::INVEN, &NewOre);
	}

	SendMsg_ResSeparation(byErrCode, pOre, &NewOre);
}

void CPlayer::pc_ResDivision(WORD wStartSerial, WORD wTarSerial, BYTE byMoveAmount)
{
	BYTE byErrCode = 0;
	__ITEM* pStartOre = NULL;
	__ITEM* pTargetOre = NULL;

	//���濡 �ִ��� �˻�
	pStartOre = m_Param.m_dbInven.GetPtrFromSerial(wStartSerial);
	if(!pStartOre)
	{
		byErrCode = 5; // �׷��ø������
		goto RESULT;
	}

	pTargetOre = m_Param.m_dbInven.GetPtrFromSerial(wTarSerial);
	if(!pTargetOre)
	{
		byErrCode = 5; // �׷��ø������
		goto RESULT;
	}

	//��ø����..
	if(!::IsOverLapItem(pStartOre->m_byTableCode) || !::IsOverLapItem(pTargetOre->m_byTableCode))
	{
		byErrCode = 3; //��ø�ƴ�
		goto RESULT;
	}

	//���� ���� �������� �˻�
	if(pStartOre->m_byTableCode != pTargetOre->m_byTableCode || pStartOre->m_wItemIndex != pTargetOre->m_wItemIndex)
	{
		byErrCode = 6;// �����������ڿ�,�����ƴ�
		goto RESULT;
	}

	//�������̵����� �˻�
	if(pStartOre->m_dwDur < byMoveAmount)
	{
		byErrCode = 8;	//�������̵���
		goto RESULT;
	}

	//�̵��Ұ��� ��ġ���� �˻�
	if(pTargetOre->m_dwDur+byMoveAmount > max_overlap_num)
	{
		byErrCode = 9;// �̵��Ұ��̳�ħ
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//���ۿ��� ���
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pStartOre->m_byStorageIndex, -byMoveAmount, true);//����

		//Ÿ�ٿ��� �߰�
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pTargetOre->m_byStorageIndex, byMoveAmount, true);//����
	}

	SendMsg_ResDivision(byErrCode, pStartOre, pTargetOre);
}

