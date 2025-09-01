#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"
#include "WeaponItemFld.h"
#include "PotionItemFld.h"
#include "DfnEquipItemFld.h"
#include "EtcItemFld.h"
#include "ErrorEventFromServer.h"

void CPlayer::pc_UsePotionItem(_STORAGE_POS_INDIV* pItem)
{
	BYTE byErrCode = 0; //1;//������ġƲ�� 2;//���¾��� 3;//���Ǿƴ� 4;//�����ʰ� 5;//ȸ�������� 6;//�̹� �������� 7;//����ž����
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[pItem->byStorageCode];
	__ITEM* pUseItem = NULL;
	
	if(IsRidingUnit()) //����ž���߿��� ȸ���Ұ�
	{
		g_Main.m_logSystemError.Write("%s: pc_UsePotionItem.. ����ž����", m_Param.GetCharName());
		byErrCode = 7;//����ž����
		goto RESULT;
	}
	
	//����Ȯ��
	pUseItem = pList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pUseItem)
	{
		SendMsg_AdjustAmountInform(pItem->byStorageCode, pItem->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_UsePotionItem.. ���¾���(sd:%d, sr: %d)", m_Param.GetCharName(), pItem->byStorageCode, pItem->wItemSerial);
		byErrCode = 2;//���¾���
		goto RESULT;
	}

	//����Ȯ��..
	if(m_Param.GetHP() == 0)
	{
		byErrCode = 6;//�̹� ��������
		goto RESULT;
	}

	//�������� Ȯ��
	if(pUseItem->m_byTableCode != tbl_code_potion)
	{
		g_Main.m_logSystemError.Write("%s: pc_UsePotionItem.. ���Ǿƴ�", m_Param.GetCharName());
		byErrCode = 3;//���Ǿƴ�
		goto RESULT;
	}

	if(m_EP.GetEff_State(_EFF_STATE::Rev_Lck))//######### PARAMETER EDIT (ȸ�����)
	{
		g_Main.m_logSystemError.Write("%s: pc_UsePotionItem.. ȸ��������", m_Param.GetCharName());
		byErrCode = 5;//ȸ��������
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//�Ķ��Ÿ����
		_PotionItem_fld* pFld = (_PotionItem_fld*)g_Main.m_tblItemData[tbl_code_potion].GetRecord(pUseItem->m_wItemIndex);

		struct EFFECT{
			int   nCode; 
			float fUnit; 
		};

		EFFECT* pEffect = (EFFECT*)&pFld->m_nEff1Code;
		bool bCont = false;//������..

		for(int i = 0; i < MAX_POTION_EFFECT_KIND; i++)
		{
			if(pEffect[i].nCode == 1)//hp
				SetHP(GetHP()+pEffect[i].fUnit, false);
			else if(pEffect[i].nCode == 2)//fp
				SetFP(GetFP()+pEffect[i].fUnit, false);
			else if(pEffect[i].nCode == 3)//sp
				SetSP(GetSP()+pEffect[i].fUnit, false);
			else if(pEffect[i].nCode == 4)//���ݷ�����
				m_fAFRate = pEffect[i].fUnit;
			else if(pEffect[i].nCode == 5)//��������
				m_fDefRate = pEffect[i].fUnit;
		}

		if(pFld->m_nDuration > 0)
		{
			m_dwStartTime = timeGetTime();
			m_dwDurTime = pFld->m_nDuration;
		}

		//�����Ҹ�
		Emb_AlterDurPoint(pList->m_nListCode, pUseItem->m_byStorageIndex, -1, false);//�Ҹ�
	}

	SendMsg_UsePotionResult(byErrCode, pItem->wItemSerial);
}

void CPlayer::pc_EquipPart(_STORAGE_POS_INDIV* pItem)
{
	BYTE byErrCode = 0;//1;//������ġƲ�� 2;//���¾��� 3;//���������۾ƴ� 4;//����Ҽ��������� 5;//���е���� 6;//��չ������� 7;//��������
	_STORAGE_LIST* pBagList = &m_Param.m_dbInven;
	__ITEM* pFixingItem = NULL;	
	char* pszCivilCode = NULL;
	_base_fld* pFixingFld = NULL;

	//����Ȯ��
	pFixingItem = pBagList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pFixingItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_EquipPart.. ���¾���(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
		byErrCode = 2;//���¾���
		//REFRESH..	
		goto RESULT;
	}
	pFixingFld = g_Main.m_tblItemData[pFixingItem->m_byTableCode].GetRecord(pFixingItem->m_wItemIndex);

	//�������������� Ȯ��
	if(pFixingItem->m_byTableCode >= equip_fix_num)
	{
		g_Main.m_logSystemError.Write("%s: pc_EquipPart.. ���������۾ƴ�(tbl:%d)", m_Param.GetCharName(), pFixingItem->m_byTableCode);
		byErrCode = 3;//���������۾ƴ�
		goto RESULT;
	}

	//��밡����������Ȯ��
	if(!::IsItemEquipCivil(pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex, m_Param.GetRaceSexCode()))
	{
		g_Main.m_logSystemError.Write("%s: pc_EquipPart.. ����Ҽ���������", m_Param.GetCharName());
		byErrCode = 4;//����Ҽ���������
		goto RESULT;
	}

	//�ɷ�����
	if(!IsEffectableEquip(pFixingItem))
	{
	//	g_Main.m_logSystemError.Write("%s: pc_EquipPart.. ��������(tbl:%d, idx:%d, upt:%s)", m_Param.GetCharName(), pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex, ::DisplayItemLv(pFixingItem->m_dwLv));
		byErrCode = 7;//��������
		goto RESULT;
	}

	//��չ����ϰ�� �����ִ���Ȯ��
	if(pFixingItem->m_byTableCode == tbl_code_weapon)
	{
		int nFixPart = ((_WeaponItem_fld*)pFixingFld)->m_nFixPart;
		if(nFixPart == handle_both && m_Param.m_dbEquip.m_pStorageList[tbl_code_shield].m_bLoad)
		{
			g_Main.m_logSystemError.Write("%s: pc_EquipPart.. ��չ����ε����е����", m_Param.GetCharName());
			byErrCode = 5;//��չ����ε����е����
			goto RESULT;
		}
	}

	//�����ϰ�� ��չ������ִ��� Ȯ��
	if(pFixingItem->m_byTableCode == tbl_code_shield)
	{
		__ITEM* pWpProp = &m_Param.m_dbEquip.m_pStorageList[tbl_code_weapon];
		if(pWpProp->m_bLoad)
		{
			_WeaponItem_fld* pWpFld = (_WeaponItem_fld*)g_Main.m_tblItemData[tbl_code_weapon].GetRecord(pWpProp->m_wItemIndex);
			if(pWpFld->m_nFixPart == handle_both)
			{
				g_Main.m_logSystemError.Write("%s: pc_EquipPart.. �����ε���չ�������", m_Param.GetCharName());
				byErrCode = 6;//��չ�������
				goto RESULT;
			}
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM NewFixItem;
		memcpy(&NewFixItem, pFixingItem, sizeof(__ITEM));

		//������ ���
		Emb_DelStorage(pBagList->m_nListCode, pFixingItem->m_byStorageIndex, false);

		//�������ִٸ� �������� �̵���Ű�� ���
		__ITEM* pProp = &m_Param.m_dbEquip.m_pStorageList[NewFixItem.m_byTableCode];
		if(pProp->m_bLoad)
		{
			__ITEM ReleaseItem;
			memcpy(&ReleaseItem, pProp, sizeof(__ITEM));
			Emb_AddStorage(pBagList->m_nListCode, &ReleaseItem);
			Emb_DelStorage(_STORAGE_POS::EQUIP, pProp->m_byStorageIndex, true);
		}

		//������Ŵ
		Emb_AddStorage(_STORAGE_POS::EQUIP, &NewFixItem);
	}

	SendMsg_EquipPartResult(byErrCode);
}

void CPlayer::pc_EmbellishPart(_STORAGE_POS_INDIV* pItem, WORD wChangeSerial)
{	
	BYTE byErrCode = 0;//1;//������ġƲ�� 2;//���¾��� 3;//��������۾ƴ� 4;//����Ҽ��������� 5;//�������� 6;//�ٲٷ��°���Ʋ�� 7;//��������
	_STORAGE_LIST* pBagList = &m_Param.m_dbInven;
	_STORAGE_LIST* pEmbelList = &m_Param.m_dbEmbellish;
	__ITEM* pFixingItem = NULL;	
	__ITEM* pFixedItem = NULL;	
	char* pszCivilCode = NULL;
	int nEqualNum = 0;
	int i;

	//����Ȯ��
	pFixingItem = pBagList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pFixingItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. ���¾���(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
		byErrCode = 2;//���¾���
		//REFRESH..
		goto RESULT;
	}

	//������������� Ȯ��
	if(pFixingItem->m_byTableCode != tbl_code_ring
		&& pFixingItem->m_byTableCode != tbl_code_amulet
		&& pFixingItem->m_byTableCode != tbl_code_bullet)
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. ��������۾ƴ�(tbl:%d)", m_Param.GetCharName(), pFixingItem->m_byTableCode);
		byErrCode = 3;//��������۾ƴ�
		goto RESULT;
	}

	//��밡����������Ȯ��
	if(!::IsItemEquipCivil(pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex, m_Param.GetRaceSexCode()))
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. ����Ҽ���������", m_Param.GetCharName());
		byErrCode = 4;//����Ҽ���������
		goto RESULT;
	}

	//����üũ..
	if(::GetItemEquipLevel(pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex) > m_Param.GetLevel()+m_EP.m_fEff_Have[_EFF_HAVE::Equip_Lv_Up])
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. ��������", m_Param.GetCharName());
		byErrCode = 7;//��������
		goto RESULT;
	}

	//�ٲٷ��°��� ��������Ȯ��
	if(wChangeSerial != 0xFFFF)
	{
		pFixedItem = pEmbelList->GetPtrFromSerial(wChangeSerial);
		if(!pFixedItem)
		{
			g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. �ٲٷ��°��̾��¾���(%d)", m_Param.GetCharName(), wChangeSerial);
			byErrCode = 2;//���¾���
			//REFRESH..
			goto RESULT;
		}
		if(pFixedItem->m_byTableCode != pFixingItem->m_byTableCode)
		{
			g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. �ٲٷ��°��� ���������Ǿ����� �ƴ�(%d)", m_Param.GetCharName(), pFixingItem->m_byTableCode);
			byErrCode = 6;//�ٲٷ��°���Ʋ��
			goto RESULT;
		}
	}

	//����� �ֳ� Ȯ��
	if(wChangeSerial == 0xFFFF)
	{
		if(pEmbelList->GetIndexEmptyCon() == __NSI)
		{
			g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. ��������", m_Param.GetCharName());
			byErrCode = 5;//��������
			//REFRESH..
			goto RESULT;
		}		
	}

	//�ΰ��̻� �����ϴ��� Ȯ��
	for(i = 0; i < embellish_fix_num; i++)
	{
		if(pEmbelList->m_pStorageList[i].m_bLoad && 
			pEmbelList->m_pStorageList[i].m_byTableCode == pFixingItem->m_byTableCode)
			nEqualNum++;	
	}
	if((wChangeSerial == 0xFFFF && nEqualNum > 1)	|| //�̵�
		 (wChangeSerial != 0xFFFF && nEqualNum > 2))	//��ü
	{
		g_Main.m_logSystemError.Write("%s: pc_EmbellishPart.. �������� �̹� 2������", m_Param.GetCharName());
		byErrCode = 5;//��������
		//REFRESH..
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM NewFixItem;
		memcpy(&NewFixItem, pFixingItem, sizeof(__ITEM));

		//������ ���
		Emb_DelStorage(pBagList->m_nListCode, pFixingItem->m_byStorageIndex, false);

		//�������ִٸ� �������� �̵���Ű�� ���
		if(pFixedItem)
		{
			__ITEM FixedItem;
			memcpy(&FixedItem, pFixedItem, sizeof(__ITEM));
			Emb_AddStorage(pBagList->m_nListCode, &FixedItem);

			Emb_DelStorage(pEmbelList->m_nListCode, pFixedItem->m_byStorageIndex, false);
		}
			
		//������Ŵ
		Emb_AddStorage(pEmbelList->m_nListCode, &NewFixItem);
	}

	SendMsg_EmbellishResult(byErrCode);	
}

void CPlayer::pc_OffPart(_STORAGE_POS_INDIV* pItem)
{
	BYTE byErrCode = 0;//1;//��밡��ƴ� 2;//���¾��� 3;//������ĭ����
	_STORAGE_LIST* pEquList = NULL;
	_STORAGE_LIST* pBagList = &m_Param.m_dbInven;
	__ITEM* pReleaseItem = NULL;	

	//�������ξ������� �ִ���Ȯ��
	if(pItem->byStorageCode == _STORAGE_POS::EQUIP || pItem->byStorageCode == _STORAGE_POS::EMBELLISH)
		pEquList = m_Param.m_pStoragePtr[pItem->byStorageCode];
	else
	{
		g_Main.m_logSystemError.Write("%s: pc_OffPart.. �����ڵ尡 Ʋ��(%d)", m_Param.GetCharName(), pItem->byStorageCode);
		byErrCode = 2;//���¾���
		goto RESULT;
	}
	pReleaseItem = pEquList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pReleaseItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_OffPart.. ���¾���(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
		byErrCode = 2;//���¾���
		//REFRESH..
		goto RESULT;
	}

	//���濡 ����� �ֳ� Ȯ��
	if(pBagList->GetIndexEmptyCon() == __NSI)
	{
		g_Main.m_logSystemError.Write("%s: pc_OffPart.. ��ĭ����", m_Param.GetCharName());
		byErrCode = 3;//������ĭ����
		//REFRESH..
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		__ITEM ReMoveItem;
		memcpy(&ReMoveItem, pReleaseItem, sizeof(__ITEM));

		//��������
		Emb_DelStorage(pEquList->m_nListCode, pReleaseItem->m_byStorageIndex, false);

		//���濡 ����
		Emb_AddStorage(pBagList->m_nListCode, &ReMoveItem);
	}

	SendMsg_OffPartResult(byErrCode);
}

void CPlayer::pc_MakeItem(_STORAGE_POS_INDIV* pipMakeTool, WORD wManualIndex, BYTE byMaterialNum, _STORAGE_POS_INDIV* pipMaterials)
{
	BYTE byErrCode = 0;//1;//����������, 7;//�����߰��Ұ����̾���. 5;//������� 4;//������ 6;//������ս��� 100;//���۽���  7;//�����߰��Ұ����̾���
	__ITEM* pToolProp = NULL;
	__ITEM* pMaterialProp[100];
	int i;
	CItemMakeDataTable::_material_list* pManual = g_Main.m_tblItemMakeData.GetRecord(wManualIndex);
	_ItemMakeData_fld* pManualFld = g_Main.m_tblItemMakeData.GetRecordFld(wManualIndex);
	DWORD dwPreserveKey = 0;
	int nMaterialStorageIndex[100];
	int nMKItemTableCode = ::GetItemTableCode(pManualFld->m_strCode);
	WORD wMKItemIndex = (WORD)(g_Main.m_tblItemData[nMKItemTableCode].GetRecord(pManualFld->m_strCode))->m_dwIndex;
	BYTE byMakeCode;

	//������ ���� Ȯ��
	pToolProp = m_Param.m_dbInven.GetPtrFromSerial(pipMakeTool->wItemSerial);
	if(!pToolProp)
	{
		g_Main.m_logSystemError.Write("%s: pc_MakeItem.. ����������(%d)", m_Param.GetCharName(), pipMakeTool->wItemSerial);
		byErrCode = error_make_nothing_tool;//����������
		//REFRESH..
		goto RESULT;
	}
	if(pToolProp->m_byTableCode != tbl_code_maketool)
	{
		g_Main.m_logSystemError.Write("%s: pc_MakeItem.. ���̾ƴϴ�(%d)", m_Param.GetCharName(), pToolProp->m_byTableCode);
		byErrCode = error_make_not_tool;//���� �ƴϴ�.
		goto RESULT;
	}

	//���� �߰��� ���濡 ������ճ� �˻�
	if(m_Param.m_dbInven.GetIndexEmptyCon() == __NSI)
	{
		g_Main.m_logSystemError.Write("%s: pc_MakeItem.. �����߰��Ұ����̾���", m_Param.GetCharName());
		byErrCode = error_make_full_inven;//�����߰��Ұ����̾���	
		//REFRESH..
		goto RESULT;
	}

	//���õ� Ȯ��
	switch(pManual->byItemTblCode)
	{
	case tbl_code_shield:
	case tbl_code_weapon:
		byMakeCode = mi_class_shield;
		break;
	case tbl_code_bullet:
		byMakeCode = mi_class_bullet;
		break;
	default:
		byMakeCode = mi_class_armor;
		break;
	}

	//���Ȯ��	
	for(i = 0; i < byMaterialNum; i++)
	{
		pMaterialProp[i] = m_Param.m_dbInven.GetPtrFromSerial(pipMaterials[i].wItemSerial);
		if(!pMaterialProp[i])
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pipMaterials[i].wItemSerial, 0);//REFRESH..
			g_Main.m_logSystemError.Write("%s: pc_MakeItem.. �������(%d)", m_Param.GetCharName(), pipMaterials[i].wItemSerial);

			byErrCode = error_make_nothing_material;//�������
			goto RESULT;
		}
		if(pipMaterials[i].byNum > pMaterialProp[i]->m_dwDur)
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pipMaterials[i].wItemSerial, pMaterialProp[i]->m_dwDur);//REFRESH..
			g_Main.m_logSystemError.Write("%s: pc_MakeItem.. ������(%d>%d)", m_Param.GetCharName(), pipMaterials[i].byNum, pMaterialProp[i]->m_dwDur);

			byErrCode = error_make_lack_material;//������
			goto RESULT;
		}
		//�ߺ�Ȯ��
		for(int a = 0; a < i; a++)
		{
			if(pipMaterials[a].wItemSerial == pipMaterials[i].wItemSerial)
			{
				g_Main.m_logSystemError.Write("%s: pc_MakeItem.. ����ߺ�", m_Param.GetCharName());
				byErrCode = error_make_lack_material;//������
				goto RESULT;
			}
		}

		dwPreserveKey += ((pMaterialProp[i]->m_byTableCode+1) * (pMaterialProp[i]->m_wItemIndex+1) * pipMaterials[i].byNum);				
		nMaterialStorageIndex[i] = pMaterialProp[i]->m_byStorageIndex;
	}

	//����Ű�˻�
	if(dwPreserveKey != pManual->dwPreserveKey)
	{
		g_Main.m_logSystemError.Write("%s: pc_MakeItem.. ������ս���", m_Param.GetCharName());

		byErrCode = error_make_mix_fail;//������ս���	
		goto RESULT;
	}
	
	//��������Ȯ��
//	if(pManualFld->m_strCivil[m_Param.GetRaceCode()] != 1)
//	{
//		byErrCode = 8;//���۰��������̾ƴ�	
//		goto RESULT;
//	}

RESULT:

	if(byErrCode == 0)
	{	
		__ITEM MaterialCopy[100];
		BYTE  byMtrNumCopy[100];

		for(i = 0; i < byMaterialNum; i++)
		{
			memcpy(&MaterialCopy[i], pMaterialProp[i], sizeof(__ITEM));
			byMtrNumCopy[i] = pipMaterials[i].byNum;
		}

		int nSkillProp = m_pmMst.GetMasteryPerMast(mastery_code_make, byMakeCode);
		int nSuccRate = 0;
		int nStdLv, nItmLv;
		__ITEM NewItem;
		 
		if(pManual->byItemTblCode != tbl_code_bullet)
		{
			//���ط��� = INT(SQRT(���ۼ��õ�*150+1300)/2+0.5)-18
			nStdLv = sqrt(nSkillProp*150+1300.0f)/2+0.5f - 18;
			//���������ۼ���Ȯ�� = 0.5-(((�����۷���-���ط���)*11.25)*0.01)
			nItmLv = ::GetItemEquipLevel(nMKItemTableCode, wMKItemIndex);

			nSuccRate = 50 - (nItmLv-nStdLv) * 11.25f;
		}
		else	//�߻繰�� ����..
		{
			//�߻繰���ۼ���Ȯ�� = INT(40+(�߻繰���ۼ��õ�*50))
			nSuccRate = 40+(nSkillProp*0.5f);
		}

		nSuccRate = min(nSuccRate, 95);
		nSuccRate = max(nSuccRate, 0);

		if(m_bCheat_100SuccMake)//100%����ġƮ..
			nSuccRate = 100;

		if(nSuccRate < ::rand()%100)
		{
			byErrCode = miss_make_random;//���۽���
		}

		//���Ҹ��Ŵ
		for(i = 0; i < byMaterialNum; i++)
		{
			Emb_AlterDurPoint(_STORAGE_POS::INVEN, nMaterialStorageIndex[i], -pipMaterials[i].byNum, true); //����
		}

		//�������� ������ �Ҹ�
		__ITEM* pToolCon = &m_Param.m_pStoragePtr[_STORAGE_POS::INVEN]->m_pStorageList[pToolProp->m_byStorageIndex];
		DWORD dwLeftToolDur = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pToolProp->m_byStorageIndex, -1, false); //�Ҹ�
		if(dwLeftToolDur == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pToolCon, m_szItemHistoryFileName);
		}

		if(byErrCode == 0)
		{
			WORD wNewSerial;
			if(byErrCode == 0)
			{
				//���ο�������߰�
				wNewSerial = m_Param.GetNewItemSerial();
				
				NewItem.m_byTableCode = pManual->byItemTblCode;
				NewItem.m_wItemIndex = pManual->wItemIndex;
				NewItem.m_dwDur = ::GetItemDurPoint(pManual->byItemTblCode, pManual->wItemIndex);

				//���׷��̵尡�� ���ϼ��� ����Ʈ�� ��������..
				BYTE byMaxLv = ::GetDefItemUpgSocketNum(pManual->byItemTblCode, pManual->wItemIndex);
				BYTE byLimSocketNum = byMaxLv > 0 ? 1+::rand()%byMaxLv : 0;

				NewItem.m_dwLv = ::GetBitAfterSetLimSocket(byLimSocketNum);
				NewItem.m_wSerial = wNewSerial;

				Emb_AddStorage(_STORAGE_POS::INVEN, &NewItem);

				//�����͸� ����..
				if(pManual->byItemTblCode != tbl_code_bullet)
				{
					if(nStdLv - nItmLv <= 4)
						Emb_AlterStat(mastery_code_make, byMakeCode, 1);
				}
				else
					Emb_AlterStat(mastery_code_make, byMakeCode, 1);

				//�����ۻ��� ������..
				SendMsg_InsertItemInform(_STORAGE_POS::INVEN, &NewItem);
			}
		}

		//ITEM HISTORY..
		s_MgrItemHistory.make_item(MaterialCopy, byMtrNumCopy, byMaterialNum, byErrCode, &NewItem, m_szItemHistoryFileName);
	}

	SendMsg_MakeItemResult(byErrCode);
}

void CPlayer::pc_UpgradeItem(_STORAGE_POS_INDIV* pposTalik, _STORAGE_POS_INDIV* pposToolItem, _STORAGE_POS_INDIV* pposUpgItem, BYTE byJewelNum, _STORAGE_POS_INDIV* pposUpgJewel)
{
	BYTE byErrCode = 0;//1;//���¾����� 2;//���۸������� 3;//���۾ȵŴ� ������ 5;//���̻��ش�Ż�����߰��Ҽ����� 6;//�̾����ۿ����ش�Ż�����߰��Ҽ����� 7;//�̹� ����Ż���� ���� ������ 100;//��������, 101;//����Ż���ı� 102;	//�����ۺн�..
	int i;
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[pposUpgItem->byStorageCode];
	__ITEM* pTalikProp = NULL;
	__ITEM* pToolProp = NULL;
	__ITEM* pDstItemProp = NULL;
	__ITEM* pJewelProp[upgrade_jewel_num];
	_ItemUpgrade_fld* pTalik = NULL;
	_ItemUpgrade_fld* pJewelFld[upgrade_jewel_num];
	memset(pJewelFld, NULL, sizeof(_ItemUpgrade_fld*)*upgrade_jewel_num);

	//�������ۼ���Ȯ��
	pDstItemProp = pList->GetPtrFromSerial(pposUpgItem->wItemSerial);
	if(!pDstItemProp)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ���´�������(sr:%d)", m_Param.GetCharName(), pposUpgItem->wItemSerial);
		byErrCode = error_upgrade_nothing_item;//���¾�����
		//REFRESH..
		goto RESULT;
	}

	//���׷��̵尡�ɰ˻�
	if(::GetItemKindCode(pDstItemProp->m_byTableCode) != item_kind_std)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ���۾ȵŴ� ������(tbl:%d)", m_Param.GetCharName(), pDstItemProp->m_byTableCode);
		byErrCode = error_upgrade_not_able;//���۾ȵŴ� ������
		goto RESULT;
	}
	if(::GetDefItemUpgSocketNum(pDstItemProp->m_byTableCode, pDstItemProp->m_wItemIndex) == 0)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ���۾ȵŴ� ������", m_Param.GetCharName());
		byErrCode = error_upgrade_not_able;//���۾ȵŴ� ������
		goto RESULT;
	}
	if(::GetItemUpgedLv(pDstItemProp->m_dwLv) >= ::GetItemUpgLimSocket(pDstItemProp->m_dwLv))
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ���۸�������", m_Param.GetCharName());
		byErrCode = error_upgrade_full_upgrade;//���۸�������
		goto RESULT;
	}

	//Ż������Ȯ��
	pTalikProp = m_Param.m_dbInven.GetPtrFromSerial(pposTalik->wItemSerial);
	if(!pTalikProp)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposTalik->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ����Ż��(sr:%d)", m_Param.GetCharName(), pposTalik->wItemSerial);
		byErrCode = error_upgrade_nothing_talik; //���¾�����
		goto RESULT;
	}
	if(pTalikProp->m_byTableCode != tbl_code_res)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. �����ƴ�Ż��(%d)", m_Param.GetCharName(), pTalikProp->m_byTableCode);
		byErrCode = error_upgrade_not_talik;//Ż���� �ƴϴ�.
		goto RESULT;
	}
	pTalik = g_Main.m_tblItemUpgrade.GetRecordFromRes(pTalikProp->m_wItemIndex);	
	if(!pTalik)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. m_tblItemUpgrade.. ����Ż�����ڵ�", m_Param.GetCharName());
		byErrCode = error_upgrade_not_talik;//Ż���� �ƴϴ�.
		goto RESULT;
	}
	if(pTalik->m_dwIndex >= MAX_TALIK_NUM)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. Ż���� �ƴ� MAX_TALIK_NUM�� �ʰ� (%s)", m_Param.GetCharName(), pTalik->m_strCode);
		byErrCode = error_upgrade_not_talik;//Ż���� �ƴϴ�.
		goto RESULT;
	}

	//������Ȯ��
	pToolProp = m_Param.m_dbInven.GetPtrFromSerial(pposToolItem->wItemSerial);
	if(!pToolProp)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposToolItem->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ������(sr:%d)", m_Param.GetCharName(), pposToolItem->wItemSerial);
		byErrCode = error_upgrade_nothing_tool;//���� ����
		goto RESULT;
	}
	if(pToolProp->m_byTableCode != tbl_code_maketool)
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ���̾ƴϴ�(%d)", m_Param.GetCharName(), pToolProp->m_byTableCode);
		byErrCode = error_upgrade_not_tool;//���� �ƴϴ�.
		goto RESULT;
	}

	//��������Ȯ��
	for(i = 0; i < byJewelNum; i++)
	{
		pJewelProp[i] = m_Param.m_dbInven.GetPtrFromSerial(pposUpgJewel[i].wItemSerial);
		if(!pJewelProp[i])
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposUpgJewel[i].wItemSerial, 0);//REFRESH..
			g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ���º���(sr:%d)", m_Param.GetCharName(), pposUpgJewel[i].wItemSerial);
			byErrCode = error_upgrade_nothing_jewel;//���¾�����
			goto RESULT;
		}
		if(pJewelProp[i]->m_byTableCode != tbl_code_res)
		{
			g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. �����ƴѹ���(%d)", m_Param.GetCharName(), pJewelProp[i]->m_byTableCode);
			byErrCode = error_upgrade_not_jewel;//���¾�����
			goto RESULT;
		}
		pJewelFld[i] = g_Main.m_tblItemUpgrade.GetRecordFromRes(pJewelProp[i]->m_wItemIndex);
		if(!pJewelFld[i])
		{
			g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ���º������ڵ�(%d)", m_Param.GetCharName(), pJewelProp[i]->m_wItemIndex);
			return;
		}
		//�ߺ�Ȯ��
		for(int a = 0; a < i; a++)
		{
			if(pJewelProp[a] == pJewelProp[i])
			{
				g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. �ߺ��� ����");
				byErrCode = error_upgrade_not_jewel;//���¾�����
				goto RESULT;
			}
		}
	}

	if(!::IsAddAbleTalikToItem(pDstItemProp->m_byTableCode, pDstItemProp->m_wItemIndex, pDstItemProp->m_dwLv, pTalik->m_dwIndex, &pTalik->m_nUpperUp)) 
	{
		g_Main.m_logSystemError.Write("%s: pc_UpgradeItem.. ���̻��ش�Ż�����߰��Ҽ�����(sr:%d, tkcode:%d)", m_Param.GetCharName(), pDstItemProp->m_wSerial, pTalik->m_dwIndex);
		byErrCode = error_upgrade_full_tailk;//���̻��ش�Ż�����߰��Ҽ�����
		goto RESULT;
	}

RESULT:
	
	if(byErrCode == 0)
	{
		__ITEM ItemCopy;
		__ITEM TalikCopy;
		__ITEM JewelCopy[upgrade_jewel_num];

		//�ϴ�ī��..
		memcpy(&ItemCopy, pDstItemProp, sizeof(__ITEM));
		memcpy(&TalikCopy, pTalikProp, sizeof(__ITEM));
		for(i = 0; i < byJewelNum; i++)			
			memcpy(&JewelCopy[i], pJewelProp[i], sizeof(__ITEM));


		//Ż�� ���� ����
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pTalikProp->m_byStorageIndex, -1, true);//����
		for(i = 0; i < byJewelNum; i++)			
			Emb_AlterDurPoint(_STORAGE_POS::INVEN, pJewelProp[i]->m_byStorageIndex, -1, true);//����
	
		//���ۿ��峻�����Ҹ�
		__ITEM* pToolCon = &m_Param.m_pStoragePtr[_STORAGE_POS::INVEN]->m_pStorageList[pToolProp->m_byStorageIndex];
		DWORD dwLeftToolDur = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pToolProp->m_byStorageIndex, -1, false);//�Ҹ�
		if(dwLeftToolDur == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pToolCon, m_szItemHistoryFileName);
		}

		//�ƾ��۾���
		float fRate = 0.0f;
		for(i = 0; i < upgrade_jewel_num; i++)
		{
			if(!pJewelFld[i])
				fRate += 0.125;
			else
				fRate += pJewelFld[i]->m_fJewelFieldValue;
		}

		BYTE byLv = ::GetItemUpgedLv(pDstItemProp->m_dwLv);

		DWORD dwTotalRate = (s_dwItemUpgSucRate[byLv]*fRate/upgrade_jewel_num)*1000;

		DWORD dwR1 = ::rand();
		DWORD dwRand = (dwR1<<16)+::rand();

		if(m_bCheat_100SuccMake)//100%����ġƮ..
			dwTotalRate = 0xFFFFFFFF;
		
		if(dwTotalRate <= dwRand%100000)//���н�..
		{
			byErrCode = miss_upgrade_random;//��������

			//����Ż���н�..
			bool bTalikBreak = false;
			switch(byLv)
			{
			case 5:
				if(125 > ::rand()%1000)
					bTalikBreak = true;
				break;
			case 6:
				if(250 > ::rand()%1000)
					bTalikBreak = true;
				break;
			case 7:
				if(500 > ::rand()%1000)
					bTalikBreak = true;
				break;
			}                         
			if(bTalikBreak)//���Ż���� �ı��ž� ���׷��̵尡 0�̵ȴ�
			{                          
				Emb_ItemUpgrade(item_upgrade_init, pposUpgItem->byStorageCode, pDstItemProp->m_byStorageIndex, 0);
									   	
				byErrCode = miss_upgrade_destroy_talik;	 //���� 0����..101;//����Ż���ı�
			}
			else
			{
				//�����ۺн�..
				bool bItemBreak = false;
				switch(byLv)
				{
				case 5:
					if(125 > ::rand()%1000)
						bItemBreak = true;
					break;
				case 6:
					if(250 > ::rand()%1000)
						bItemBreak = true;
					break;
				case 7:
					if(500 > ::rand()%1000)
						bItemBreak = true;
					break;
				}

				if(bItemBreak)
				{
					Emb_DelStorage(pList->m_nListCode, pDstItemProp->m_byStorageIndex, false);
					byErrCode = miss_upgrade_destroy_item;	//�����ۺн�..
				}
			}
		}
		else
		{
			//���ۼ���
			DWORD dwUpt = ::GetBitAfterUpgrade(pDstItemProp->m_dwLv, pTalik->m_dwIndex, byLv);
			Emb_ItemUpgrade(item_upgrade_up, pList->m_nListCode, pDstItemProp->m_byStorageIndex, dwUpt);
		}

		//ITEM HISTORY..
		s_MgrItemHistory.grade_up_item(&ItemCopy, &TalikCopy, JewelCopy, byJewelNum, byErrCode, pDstItemProp->m_dwLv, m_szItemHistoryFileName);
	}

	SendMsg_ItemUpgrade(byErrCode);
}

void CPlayer::pc_DowngradeItem(_STORAGE_POS_INDIV* pposTalik, _STORAGE_POS_INDIV* pposToolItem, _STORAGE_POS_INDIV* pposUpgItem)
{
	BYTE byErrCode = 0;//1;//����Ż�� 2;//�߸���Ż�� 3;//������ 4;//���¾����� 5;//���۾ȵŴ� ������ 6;//���ۉ¾��۾ƴ�
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[pposUpgItem->byStorageCode];
	__ITEM* pTalikProp = NULL;
	__ITEM* pToolProp = NULL;
	__ITEM* pDstItemProp = NULL;
	_ItemUpgrade_fld* pTalik = NULL;
	_base_fld* pItemFld = NULL;

	//�������ۼ���Ȯ��
	pDstItemProp = pList->GetPtrFromSerial(pposUpgItem->wItemSerial);
	if(!pDstItemProp)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. ���´�������(sr:%d)", m_Param.GetCharName(), pposUpgItem->wItemSerial);
		byErrCode = error_upgrade_nothing_item;//���¾�����
		goto RESULT;
	}

	//���׷��̵尡 ������ ����������Ȯ��
	if(::GetItemKindCode(pDstItemProp->m_byTableCode) != item_kind_std)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. ���۾ȵŴ� ������(tbl:%d)", m_Param.GetCharName(), pDstItemProp->m_byTableCode);
		byErrCode = error_upgrade_not_able;//���۾ȵŴ� ������
		goto RESULT;
	}
	if(::GetDefItemUpgSocketNum(pDstItemProp->m_byTableCode, pDstItemProp->m_wItemIndex) == 0)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. ���۾ȵŴ� ������", m_Param.GetCharName());
		byErrCode = error_upgrade_not_able;//���۾ȵŴ� ������
		goto RESULT;
	}
	if(::GetItemUpgedLv(pDstItemProp->m_dwLv) == 0)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. ���ۉ¾��۾ƴ�", m_Param.GetCharName());
		byErrCode = error_upgrade_none_upgrade;//���ۉ¾��۾ƴ�
		goto RESULT;
	}

	//Ż������Ȯ��
	pTalikProp = m_Param.m_dbInven.GetPtrFromSerial(pposTalik->wItemSerial);
	if(!pTalikProp)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposTalik->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. ����Ż��(sr:%d)", m_Param.GetCharName(), pposTalik->wItemSerial);
		byErrCode = error_upgrade_nothing_talik;//����Ż��
		goto RESULT;
	}
	if(pTalikProp->m_byTableCode != tbl_code_res)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. �����ƴ�Ż��(sr:%d)", m_Param.GetCharName(), pTalikProp->m_byTableCode);
		byErrCode = error_upgrade_not_talik;//����Ż��
		goto RESULT;
	}
	pTalik = g_Main.m_tblItemUpgrade.GetRecordFromRes(pTalikProp->m_wItemIndex);
	if(!pTalik)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. ���ڵ忡����Ż��(%d)", m_Param.GetCharName(), pTalikProp->m_wItemIndex);
		byErrCode = error_upgrade_not_talik;//�߸���Ż��
		goto RESULT;
	}
	if(pTalik->m_nDataEffect != talik_type_down)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. ��Ȱ��Ż���ƴ�", m_Param.GetCharName());
		byErrCode = error_upgrade_not_talik;//�߸���Ż��
		goto RESULT;
	}

	//������Ȯ��
	pToolProp = m_Param.m_dbInven.GetPtrFromSerial(pposToolItem->wItemSerial);
	if(!pToolProp)
	{
		SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, pposToolItem->wItemSerial, 0);//REFRESH..
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. ������(sr:%d)", m_Param.GetCharName(), pposToolItem->wItemSerial);
		byErrCode = error_upgrade_nothing_tool;//������
		goto RESULT;
	}
	if(pToolProp->m_byTableCode != tbl_code_maketool)
	{
		g_Main.m_logSystemError.Write("%s: pc_DowngradeItem.. ���̾ƴ���(%d)", m_Param.GetCharName(), pToolProp->m_byTableCode);
		byErrCode = error_upgrade_not_tool;//������
		goto RESULT;
	}

RESULT:

	if(byErrCode== 0)
	{		
		__ITEM ItemCopy;

		memcpy(&ItemCopy, pDstItemProp, sizeof(__ITEM));

		//Ż������
		Emb_AlterDurPoint(_STORAGE_POS::INVEN, pTalikProp->m_byStorageIndex, -1, true);//����

		//���ۿ��峻�����Ҹ�
		__ITEM* pToolCon = &m_Param.m_pStoragePtr[_STORAGE_POS::INVEN]->m_pStorageList[pToolProp->m_byStorageIndex];
		DWORD dwLeftToolDur = Emb_AlterDurPoint(_STORAGE_POS::INVEN, pToolProp->m_byStorageIndex, -1, false);//�Ҹ�
		if(dwLeftToolDur == 0)
		{	//ITEM HISTORY..
			s_MgrItemHistory.consume_del_item(pToolCon, m_szItemHistoryFileName);
		}

		BYTE byLv = ::GetItemUpgedLv(pDstItemProp->m_dwLv);
		DWORD dwUpt = ::GetBitAfterDowngrade(pDstItemProp->m_dwLv, byLv);
		Emb_ItemUpgrade(item_upgrade_down, pList->m_nListCode, pDstItemProp->m_byStorageIndex, dwUpt);

		//ITEM HISTORY..
		s_MgrItemHistory.grade_down_item(&ItemCopy, pTalikProp, pDstItemProp->m_dwLv, m_szItemHistoryFileName);
	}

	SendMsg_ItemDowngrade(byErrCode);
}

void CPlayer::pc_TakeLootingItem(CItemBox* pBox, WORD wAddSerial)
{
	BYTE byErrCode = 0;//1;//��������� 2;//�������۾��� 3;//������ 4;//���¹ڽ� 5;//�������� 6;//�Ÿ����� 7;//��Ķ�����¾����� 8;//������ �ֿ������
	WORD wSerial = 0;
	__ITEM BoxItem;
	__ITEM* pExisting = NULL;

	//�ڽ��� �����ϴ���..
	if(!pBox->m_bLive)
	{
	//	g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. ���¹ڽ�", m_Param.GetCharName());
		byErrCode = 4;//���¹ڽ�
		goto RESULT;
	}

	//�Ա�����ִ���..
	if(!pBox->IsTakeRight(this))
	{
		g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. ��������", m_Param.GetCharName());
		byErrCode = 5;//��������
		goto RESULT;
	}
	if(pBox->m_byThrowerDegree != 0xFF)//���������ϰ�� ���� �������..
	{
		if(pBox->m_byThrowerDegree != m_byUserDgr)
		{
			byErrCode = 5;//��������
			goto RESULT;
		}
	}

	//�Ÿ�����.
	if(itembox_dist_lootable < ::GetSqrt(pBox->m_fCurPos, m_fCurPos))
	{
		g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. �Ÿ�����", m_Param.GetCharName());
		byErrCode = 6;//�Ÿ�����
		goto RESULT;
	}
	memcpy(&BoxItem, &pBox->m_Item, sizeof(__ITEM));

	//�������� üũ..
	if(BoxItem.m_byTableCode == tbl_code_key)
	{
		g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. ������ �ֿ������", m_Param.GetCharName());
		byErrCode = 8;//������ �ֿ������
		goto RESULT;
	}

	//���ο��߰���� �� ������ �ִ���.. �����߰���� �����ø����� �ִ��� Ȯ��..
	if(wAddSerial == 0xFFFF)
	{
		if(m_Param.m_dbInven.GetIndexEmptyCon() == __NSI)
		{
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. ���������", m_Param.GetCharName());
			byErrCode = 1;//���������
			goto RESULT;
		}
	}
	else
	{
		if(!::IsOverLapItem(BoxItem.m_byTableCode))
		{
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem.. ��Ķ�����¾�����", m_Param.GetCharName());
			byErrCode = 7;//��Ķ�����¾�����
			goto RESULT;
		}
		pExisting = m_Param.m_dbInven.GetPtrFromSerial(wAddSerial);
		if(!pExisting)
		{
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, wAddSerial, 0);//REFRESH..
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem..  �������������۾���", m_Param.GetCharName());
			byErrCode = 2;//�������۾���
			goto RESULT;
		}

		if(pExisting->m_byTableCode != BoxItem.m_byTableCode || 
			pExisting->m_wItemIndex != BoxItem.m_wItemIndex)
		{
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem..  �������������۾ƴ�", m_Param.GetCharName());
			byErrCode = 2;//�������۾���
			goto RESULT;
		}
		if(pExisting->m_dwDur+BoxItem.m_dwDur > max_overlap_num) 
		{			
			SendMsg_AdjustAmountInform(_STORAGE_POS::INVEN, wAddSerial, pExisting->m_dwDur);//REFRESH..	
			g_Main.m_logSystemError.Write("%s: pc_TakeLootingItem..  ��������������", m_Param.GetCharName());
			byErrCode = 3;//������
			goto RESULT;
		}
	}

RESULT:

	if(byErrCode == 0)
	{
		//ITEM HISTORY..
		s_MgrItemHistory.take_ground_item(&BoxItem, pBox->m_dwThrowerCharSerial, m_szItemHistoryFileName);

		if(wAddSerial == 0xFFFF)	//���ο� ���丮�� �߰�..
		{
			BoxItem.m_wSerial = m_Param.GetNewItemSerial();
			Emb_AddStorage(_STORAGE_POS::INVEN, &BoxItem);
		}
		else
		{
			Emb_AlterDurPoint(_STORAGE_POS::INVEN, pExisting->m_byStorageIndex, BoxItem.m_dwDur, true);//����
		}

		pBox->Destroy();
	}

	if(wAddSerial == 0xFFFF)
		SendMsg_TakeNewResult(byErrCode, &BoxItem);
	else	
		SendMsg_TakeAddResult(byErrCode, pExisting);	//�������� ������..
}

void CPlayer::pc_ThrowStorageItem(_STORAGE_POS_INDIV* pItem)
{
	BYTE byErrCode = 0;// 1;//������ġƲ�� 2;//���¾��� 3;//�����ʰ� 4;//���������ǰ��
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[pItem->byStorageCode];
	__ITEM* pDropItem = NULL;

	//����Ȯ��
	pDropItem = pList->GetPtrFromSerial(pItem->wItemSerial);
	if(!pDropItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_ThrowStorageItem..  ���¾���(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
		byErrCode = 2;//���¾���
		goto RESULT;
	}

	//���ɸ� ����������..
	if(pDropItem->m_bLock)
	{
		byErrCode = 9;//���ɸ� ������
		goto RESULT;
	}

	//����, ����, �ڿ��̶�� ����Ȯ��
	if(::IsOverLapItem(pDropItem->m_byTableCode))
	{
		if(pItem->byNum > pDropItem->m_dwDur)
		{
			SendMsg_AdjustAmountInform(pItem->byStorageCode, pItem->wItemSerial, pDropItem->m_dwDur);//REFRESH..	
			g_Main.m_logSystemError.Write("%s: pc_ThrowStorageItem..  �����ʰ�(sr:%d)", m_Param.GetCharName(), pItem->wItemSerial);
			byErrCode = 3;//�����ʰ�
			goto RESULT;
		}
	}

	//������ ���� ǰ������..
	if(pDropItem->m_byTableCode == tbl_code_key)
	{
		byErrCode = 4;//���������ǰ��
		goto RESULT;
	}
	if(!::IsGroundableItem(pDropItem->m_byTableCode, pDropItem->m_wItemIndex))
	{
		byErrCode = 4;//���������ǰ��
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//��ӵ� �������ۼ�
		__ITEM ThrowItem;
		memcpy(&ThrowItem, pDropItem, sizeof(__ITEM));
		if(::IsOverLapItem(pDropItem->m_byTableCode))
			ThrowItem.m_dwDur = pItem->byNum;	

		bool bSucc = true;
	//	if(m_byUserDgr == USER_DEGREE_STD) dlatl..
		{//�Ϲݵ�޸��� �����۹ڽ��� �����..	
			bSucc = ::CreateItemBox(&ThrowItem, NULL, this, itembox_create_code_throw,
							 m_pCurMap, m_wMapLayerIndex, m_fCurPos, false);
		}

		if(bSucc)
		{
			//����, ����, �ڿ��� ��������..
			if(::IsOverLapItem(pDropItem->m_byTableCode))
			{
				Emb_AlterDurPoint(pList->m_nListCode, pDropItem->m_byStorageIndex, -pItem->byNum, true);//����
			}
			else		//�Ϲݾ������� �������
			{
				Emb_DelStorage(pList->m_nListCode, pDropItem->m_byStorageIndex, false);
			}

			//ITEM HISTORY..
			s_MgrItemHistory.throw_ground_item(&ThrowItem, m_szItemHistoryFileName);
		}
		else
		{	//�� �����۹ڽ��ִ��� Ȯ��
			g_Main.m_logSystemError.Write("%s: pc_ThrowStorageItem..  �ڽ����ڸ�", m_Param.GetCharName());
			byErrCode = 4;//�ڽ�����.
		}
	}

	SendMsg_ThrowStorageResult(byErrCode);
}

void CPlayer::pc_AddBag(WORD wBagItemSerial)
{
	BYTE byErrCode = 0;//0;//����, 1;//���°��� 2;//5���پ�������
	_STORAGE_LIST* pList = m_Param.m_pStoragePtr[_STORAGE_POS::INVEN];
	__ITEM* pBagItem = NULL;
	_EtcItem_fld* pFld = NULL;

	//���������� �˻�
	pBagItem = pList->GetPtrFromSerial(wBagItemSerial);
	if(!pBagItem)
	{
		g_Main.m_logSystemError.Write("%s: pc_AddBag..  ���°���(sr:%d)", m_Param.GetCharName(), wBagItemSerial);
		byErrCode = 1;//���°���
		goto RESULT;
	}
	//�������� �˻�..
	if(pBagItem->m_byTableCode != tbl_code_bag)
	{
		g_Main.m_logSystemError.Write("%s: pc_AddBag..  ����ƴѰ���", m_Param.GetCharName());
		byErrCode = 1;//���°���
		goto RESULT;
	}	

	//������� ����� �˻�
	if(m_Param.GetBagNum() >= max_bag_num)
	{
		g_Main.m_logSystemError.Write("%s: pc_AddBag..  �ƽ�����", m_Param.GetCharName());
		byErrCode = 2;//5���پ�������
		goto RESULT;
	}

RESULT:

	if(byErrCode == 0)
	{
		//����� �ø���..
		int nCurBagNum = m_Param.GetBagNum();
		m_Param.SetBagNum(nCurBagNum+1);

		//�κ��丮 ��밡�ɸ���Ʈ �ø���
		pList->SetUseListNum(m_Param.GetBagNum()*one_bag_store_num);

		//�������
		Emb_DelStorage(pList->m_nListCode, pBagItem->m_byStorageIndex, false);		

		//ITEM HISTORY..
		s_MgrItemHistory.consume_del_item(pBagItem, m_szItemHistoryFileName);

		//���忡 ������Ʈ..	
		if(m_pUserDB)
			m_pUserDB->Update_BagNum(m_Param.GetBagNum());	
	}

	SendMsg_AddBagResult(byErrCode);
}

