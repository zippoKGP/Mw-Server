#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"

void CPlayer::pc_ForceInvenChange(_STORAGE_POS_INDIV* pItem, WORD wReplaceSerial)
{	
	BYTE byErrCode = 0; //1; //소지하지않음 2; //교체대상이소지하지않음 3; //교체대상이다름 4;//포스인벤에이미있음 5;//이동시킬 공간없음
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

	//실제소지하는지 확인
	pStartItem = pStartList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pStartItem)
	{
		byErrCode = 1; //소지하지않음
		goto RESULT;
	}
	if(pStartItem->m_byTableCode != tbl_code_fcitem)
	{
		byErrCode = 1; //소지하지않음
		goto RESULT;
	}

	//교체라면 소지및 같은종류인지 확인..
	if(wReplaceSerial != 0xFFFF)
	{
		pTarItem = pTargetList->GetPtrFromSerial(wReplaceSerial);
		if(!pTarItem)
		{
			byErrCode = 2; //교체대상이소지하지않음
			goto RESULT;
		}
		if(pTarItem->m_byTableCode != tbl_code_fcitem)
		{
			byErrCode = 2; //교체대상이소지하지않음
			goto RESULT;
		}
		if(s_pnLinkForceItemToEffect[pStartItem->m_wItemIndex] != s_pnLinkForceItemToEffect[pTarItem->m_wItemIndex])
		{
			byErrCode = 3; //교체대상이다름
			goto RESULT;
		}
	}

	//교체가 아니면 이동할곳이 포스인벤이라면 같은것이 이미있는지와 빈곳확인
	if(wReplaceSerial == 0xFFFF && 	pItem->byStorageCode == _STORAGE_POS::INVEN)
	{
		for(int i = 0; i < pTargetList->m_nUsedNum; i++)
		{
			__ITEM* pItem = (__ITEM*)&pTargetList->m_pStorageList[i];

			if(!pItem->m_bLoad)
				continue;

			if(s_pnLinkForceItemToEffect[pItem->m_wItemIndex] == s_pnLinkForceItemToEffect[pStartItem->m_wItemIndex])
			{
				byErrCode = 4;//포스인벤에이미있음
				goto RESULT;
			}
		}
		if(pTargetList->GetIndexEmptyCon() == __NSI)
		{
			byErrCode = 5;//이동시킬 공간없음
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM StartItem;
		memcpy(&StartItem, pStartItem, sizeof(__ITEM));
		
		//시작을일단 지움
		Emb_DelStorage(pStartList->m_nListCode, pStartItem->m_byStorageIndex, false);

		//교체라면 교체할것을 채우고 지움
		if(pTarItem)
		{
			__ITEM TarItem;
			memcpy(&TarItem, pTarItem, sizeof(__ITEM));

			Emb_AddStorage(pStartList->m_nListCode, &TarItem);
			Emb_DelStorage(pTargetList->m_nListCode, pTarItem->m_byStorageIndex, false);
		}

		//이동점을 채움
		Emb_AddStorage(pTargetList->m_nListCode, &StartItem);
	}

	SendMsg_ForceInvenChange(byErrCode);
}

void CPlayer::pc_AnimusInvenChange(_STORAGE_POS_INDIV* pItem, WORD wReplaceSerial)
{
	BYTE byErrCode = 0; //1; //소지하지않음 2; //교체대상이소지하지않음 3; //교체대상이다름 4;//포스인벤에이미있음 5;//이동시킬 공간없음 6;//소환중임..
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

	//실제소지하는지 확인
	pStartItem = pStartList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pStartItem)
	{
		byErrCode = 1; //소지하지않음
		goto RESULT;
	}
	if(pStartItem->m_byTableCode != tbl_code_animus)
	{
		byErrCode = 1; //소지하지않음
		goto RESULT;
	}

	//현재 소환중인 애니머스인지..
	if(pStartItem == m_pRecalledAnimusItem)	
	{
		byErrCode = 6; //소환중임
		goto RESULT;
	}

	//교체라면 소지및 같은종류인지 확인..
	if(wReplaceSerial != 0xFFFF)
	{
		pTarItem = pTargetList->GetPtrFromSerial(wReplaceSerial);
		if(!pTarItem)
		{
			byErrCode = 2; //교체대상이소지하지않음
			goto RESULT;
		}
		if(pTarItem->m_byTableCode != tbl_code_animus)
		{
			byErrCode = 2; //교체대상이소지하지않음
			goto RESULT;
		}
		if(pTarItem->m_wItemIndex != pStartItem->m_wItemIndex)
		{
			byErrCode = 3; //교체대상이다름
			goto RESULT;
		}	
		if(pTarItem == m_pRecalledAnimusItem)	//현재 소환중인 애니머스인지..
		{
			byErrCode = 6; //소환중임
			goto RESULT;
		}
	}

	//교체가 아니면 이동할곳이 포스인벤이라면 같은것이 이미있는지와 빈곳확인
	if(wReplaceSerial == 0xFFFF && 	pItem->byStorageCode == _STORAGE_POS::INVEN)
	{
		for(int i = 0; i < pTargetList->m_nUsedNum; i++)
		{
			__ITEM* pItem = (__ITEM*)&pTargetList->m_pStorageList[i];

			if(!pItem->m_bLoad)
				continue;

			if(pItem->m_wItemIndex == pStartItem->m_wItemIndex)
			{
				byErrCode = 4;//포스인벤에이미있음
				goto RESULT;
			}
		}
		if(pTargetList->GetIndexEmptyCon() == __NSI)
		{
			byErrCode = 5;//이동시킬 공간없음
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM StartItem;
		memcpy(&StartItem, pStartItem, sizeof(__ITEM));
		
		//시작을일단 지움
		Emb_DelStorage(pStartList->m_nListCode, pStartItem->m_byStorageIndex, false);

		//교체라면 교체할것을 채우고 지움
		if(pTarItem)
		{
			__ITEM TarItem;
			memcpy(&TarItem, pTarItem, sizeof(__ITEM));

			Emb_AddStorage(pStartList->m_nListCode, &TarItem);
			Emb_DelStorage(pTargetList->m_nListCode, pTarItem->m_byStorageIndex, false);
		}

		//이동점을 채움
		Emb_AddStorage(pTargetList->m_nListCode, &StartItem);
	}

	SendMsg_AnimusInvenChange(byErrCode);
}

void CPlayer::pc_PotionSocketChange(_STORAGE_POS_INDIV* pItem, _STORAGE_POS_INDIV* pTarPos)
{
	BYTE byErrCode = 0;// 1;//사용가능가방아님 2;//존재하지않음 3;//포션아님 4;//옮길공간없음
	_STORAGE_LIST* pListFrom = NULL;
	_STORAGE_LIST* pListTo = NULL;
	__ITEM* pPotionItem = NULL;
	__ITEM* pTarPotionItem = NULL;	//스왑시.. 타겟 아이템..

	//위치체크
	if(pItem->byStorageCode == _STORAGE_POS::INVEN)
		pListFrom = &m_Param.m_dbInven;
	else if(pItem->byStorageCode == _STORAGE_POS::BELT)
		pListFrom = &m_Param.m_dbBelt;
	if(pTarPos->byStorageCode == _STORAGE_POS::INVEN)
		pListTo = &m_Param.m_dbInven;
	else if(pTarPos->byStorageCode == _STORAGE_POS::BELT)
		pListTo = &m_Param.m_dbBelt;

	//존재확인
	pPotionItem = pListFrom->GetPtrFromSerial(pItem->wItemSerial);
	if(!pPotionItem)
	{
		byErrCode = 2;//존재하지않음
		goto RESULT;
	}

	//포션인지 확인
	if(pPotionItem->m_byTableCode != tbl_code_potion)
	{
		byErrCode = 3;//포션아님
		goto RESULT;
	}

	//옮길공간확인
	if(pTarPos->wItemSerial == 0xFFFF)
	{
		if(pListTo->GetIndexEmptyCon() == __NSI)
		{
			byErrCode = 4;//옮길공간없음
			goto RESULT;
		}
	}
	else
	{
		pTarPotionItem = pListTo->GetPtrFromSerial(pTarPos->wItemSerial);
		if(!pTarPotionItem)
		{
			byErrCode = 2;//존재하지않음
			goto RESULT;
		}

		//포션인지 확인
		if(pTarPotionItem->m_byTableCode != tbl_code_potion)
		{
			byErrCode = 3;//포션아님
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		if(pTarPos->wItemSerial == 0xFFFF)
		{
			//From을 비우고
			Emb_DelStorage(pListFrom->m_nListCode, pPotionItem->m_byStorageIndex, false);

			//To를 채운다.
			Emb_AddStorage(pListTo->m_nListCode, pPotionItem);
		}
		else
		{
			//두개다 비우고..
			Emb_DelStorage(pListFrom->m_nListCode, pPotionItem->m_byStorageIndex, false);
			Emb_DelStorage(pListTo->m_nListCode, pTarPotionItem->m_byStorageIndex, false);

			//각각 채운다..
			Emb_AddStorage(pListFrom->m_nListCode, pTarPotionItem);	
			Emb_AddStorage(pListTo->m_nListCode, pPotionItem);			
		}
	}
	
	SendMsg_PotionSocketChange(byErrCode);
}

void CPlayer::pc_PotionSeparation(BYTE byStartPos, BYTE byTargetPos, WORD wSerial, BYTE byAmount)
{
	char sRetCode = 0; //-5;	//없는 시리얼
	__ITEM* pPotion = NULL;
	//그런 시리얼이 있는지 확인

	pPotion = m_Param.m_pStoragePtr[byStartPos]->GetPtrFromSerial(wSerial);
	if(!pPotion)
	{
		sRetCode = -5;	//없는 시리얼
		goto RESULT;
	}

	//포션인지 확인
	if(pPotion->m_byTableCode != tbl_code_potion)
	{
		sRetCode = -3;	//포션아님
		goto RESULT;
	}		

	//전부이동인지 확인
	if(pPotion->m_dwDur <= byAmount)
	{
		sRetCode = -2;	//이동
		goto RESULT;
	}

	//남은 인벤토리가 있는지 확인
	if(m_Param.m_pStoragePtr[byTargetPos]->GetIndexEmptyCon() == __NSI)
	{
		sRetCode = -4;	//새로만들 공간이 없음
		goto RESULT;
	}

RESULT:

	DWORD dwRemain;
	__ITEM Con;

	if(sRetCode == 0)
	{
		dwRemain = Emb_AlterDurPoint(byStartPos, pPotion->m_byStorageIndex, -byAmount, true);//나눔

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
		sRetCode = -5;//없는 시리얼
		goto RESULT;
	}

	pTargetPotion = m_Param.m_pStoragePtr[byTargetPos]->GetPtrFromSerial(wTarSerial);
	if(!pTargetPotion)
	{
		sRetCode = -5;//없는 시리얼
		goto RESULT;
	}

	//포션인지 확인
	if(pStartPotion->m_byTableCode != tbl_code_potion || pTargetPotion->m_byTableCode != tbl_code_potion)
	{
		sRetCode = -3;//포션아님
		goto RESULT;
	}

	//같은 종류인지 확인
	if(pStartPotion->m_wItemIndex != pTargetPotion->m_wItemIndex)
	{
		sRetCode = -6;	//같은 종류가 아님
		goto RESULT;
	}

	//더 많은 양을 이동하는지 확인
	if(pStartPotion->m_dwDur < byAmount)
	{
		sRetCode = -4;	//초과이동
		goto RESULT;
	}

	if(pTargetPotion->m_dwDur+byAmount > max_overlap_num)
	{
		sRetCode = -4;	//초과이동
		goto RESULT;
	}

RESULT:

	BYTE byParentLeft, byChildLeft;
	if(sRetCode == 0)
	{
		byParentLeft = Emb_AlterDurPoint(byStartPos, pStartPotion->m_byStorageIndex, -byAmount, true);//나눔
		byChildLeft = Emb_AlterDurPoint(byTargetPos, pTargetPotion->m_byStorageIndex, byAmount, true);//나눔
	}	

	SendMsg_PotionDivision(wSerial, byParentLeft, wTarSerial, byChildLeft, sRetCode);
}

void CPlayer::pc_ResSeparation(WORD wStartSerial, BYTE byMoveAmount)
{
	BYTE byErrCode = 0;
	__ITEM* pOre = NULL;

	//가방에 있는지 검사
	pOre = m_Param.m_dbInven.GetPtrFromSerial(wStartSerial);
	if(!pOre)
	{
		byErrCode = 1; // 그런시리얼없음
		goto RESULT;
	}

	//중첩인지..
	if(!::IsOverLapItem(pOre->m_byTableCode))
	{
		byErrCode = 3; //중첩아님
		goto RESULT;
	}

	//전부이동인지 검사
	if(pOre->m_dwDur <= byMoveAmount)
	{
		byErrCode = -2;	//전부이동함
		goto RESULT;
	}

	//남은 인벤토리가 있는지 확인
	if(m_Param.m_dbInven.GetNumEmptyCon() <= 0)
	{
		byErrCode = 4;	//남은인벤토리없음
		goto RESULT;
	}

RESULT:

	__ITEM NewOre;

	if(byErrCode == 0)
	{
		//시작에서 덜어냄
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pOre->m_byStorageIndex, -byMoveAmount, true);//나눔

		//새로운 슬롯추가
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

	//가방에 있는지 검사
	pStartOre = m_Param.m_dbInven.GetPtrFromSerial(wStartSerial);
	if(!pStartOre)
	{
		byErrCode = 5; // 그런시리얼없음
		goto RESULT;
	}

	pTargetOre = m_Param.m_dbInven.GetPtrFromSerial(wTarSerial);
	if(!pTargetOre)
	{
		byErrCode = 5; // 그런시리얼없음
		goto RESULT;
	}

	//중첩인지..
	if(!::IsOverLapItem(pStartOre->m_byTableCode) || !::IsOverLapItem(pTargetOre->m_byTableCode))
	{
		byErrCode = 3; //중첩아님
		goto RESULT;
	}

	//서로 같은 종류인지 검사
	if(pStartOre->m_byTableCode != pTargetOre->m_byTableCode || pStartOre->m_wItemIndex != pTargetOre->m_wItemIndex)
	{
		byErrCode = 6;// 같은종류의자원,원석아님
		goto RESULT;
	}

	//더많이이동인지 검사
	if(pStartOre->m_dwDur < byMoveAmount)
	{
		byErrCode = 8;	//더많이이동함
		goto RESULT;
	}

	//이동할곳에 넘치는지 검사
	if(pTargetOre->m_dwDur+byMoveAmount > max_overlap_num)
	{
		byErrCode = 9;// 이동할곳이넘침
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//시작에서 덜어냄
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pStartOre->m_byStorageIndex, -byMoveAmount, true);//나눔

		//타겟에서 추가
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pTargetOre->m_byStorageIndex, byMoveAmount, true);//나눔
	}

	SendMsg_ResDivision(byErrCode, pStartOre, pTargetOre);
}

