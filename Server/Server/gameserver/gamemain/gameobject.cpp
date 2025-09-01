// GameObject.cpp: implementation of the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameObject.h"
#include "MapData.h"
#include "MyUtil.h"
#include "Protocol.h"
#include "MainThread.h"

CGameObject* CGameObject::s_pSelectObject = NULL;
int CGameObject::s_nTotalObjectNum = 0;
CGameObject* CGameObject::s_pTotalObject[MAX_OBJ];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameObject::CGameObject()
{
	m_pRecordSet = NULL;
	m_pCurMap = NULL;
	m_wMapLayerIndex = 0;
	m_bLive = false;
	m_bMove = false;
	m_bCorpse = false;
	m_nTotalObjIndex = -1;
}

CGameObject::~CGameObject()
{
}

//Data Init..

void CGameObject::Init(_object_id* pID)
{
	m_ObjID = *pID;	
	m_SectorPoint.SetPoint(this);
	m_SectorNetPoint.SetPoint(this);
	m_bMapLoading = false;

	s_pTotalObject[s_nTotalObjectNum] = this;
	m_nTotalObjIndex = s_nTotalObjectNum;

	s_nTotalObjectNum++;
	if(s_nTotalObjectNum > MAX_OBJ)
	{
		::MyMessageBox("error", "CGameObject::Init : ������Ʈ���� ���ڶ�");
		exit(0);
	}
}

bool CGameObject::Create(_object_create_setdata* pData)
{
	if(m_bLive)
		return false;
	
	m_pCurMap = pData->m_pMap;//CalcSecIndex()���� �̸� ����..
	m_wMapLayerIndex = (WORD)pData->m_nLayerIndex;
	memcpy(m_fCurPos, pData->m_fStartPos, sizeof(float)*3);
	memcpy(m_fOldPos, pData->m_fStartPos, sizeof(float)*3);

	DWORD dwSec = CalcSecIndex();
	if(dwSec >= pData->m_pMap->GetSecInfo()->m_nSecNum)
	{
		m_pCurMap = NULL;
		g_Main.m_logSystemError.Write("CGameObject::Create() : dwSec >= MAX(this[%d-%d-%d])", m_ObjID.m_byKind, m_ObjID.m_byID, m_ObjID.m_wIndex);
		return false;
	}

	pData->m_pMap->EnterMap(this, dwSec);

	m_bLive = true;		//m_bLive �� m_pCurMap�� ���ÿ� ����..
	m_pRecordSet = pData->m_pRecordSet;
	m_dwCurSec = dwSec;
	m_bMove = false;
	m_bCorpse = false;
	m_bMapLoading = false;
	m_bBreakTranspar = false;
	m_dwLastSendTime = timeGetTime();

	return true;
}

bool CGameObject::Destroy()
{
	if(!m_bLive)
		return false;

	if(m_pCurMap)
	{
		_sec_info* pSec = m_pCurMap->GetSecInfo();

		m_pCurMap->ExitMap(this, m_dwCurSec);
	}

	if(s_pSelectObject == this)
		s_pSelectObject = NULL;
	
	m_bLive = false;
	m_bCorpse = false;
	m_bMove = false;
	m_bStun = false;
	m_bMapLoading = false;
	m_SectorPoint.InitLink();
	m_SectorNetPoint.InitLink();

	return true;
}

void CGameObject::OnLoop()
{
	DWORD dwCurTime = timeGetTime();

	if(m_bStun)
	{
		if(m_dwOldTickStun + stun_term > dwCurTime)
			SetStun(false);
	}
	if(m_bBreakTranspar)
	{
		if(m_dwOldTickBreakTranspar + break_transper_term > dwCurTime)
			SetBreakTranspar(false);			
	}

	if(!m_bMapLoading)
	{
		UpdateSecList();
	}
	
	Loop();//�߰��� ������ timeout������ �׾ �����Ͱ� �ʱ�ȭ����� ������ �ǳ��߿� ȣ���Ѵ�.
}

//Set Position..

bool CGameObject::SetCurPos(float* pPos)
{
	//���� ��ȿ�� ��ǥ���� check..
	if(!m_pCurMap->IsMapIn(pPos))
		return false;

	memcpy(m_fCurPos, pPos, sizeof(float)*3);
	
	return true;	
}

bool CGameObject::SetCurBspMap(CMapData* pMap)
{
	if(m_pCurMap != pMap)
	{
		m_pCurMap = pMap;

		return true;
	}

	return false;
}

void CGameObject::SetStun(bool bStun)
{
	m_bStun = bStun;
	if(m_bStun)
		m_dwOldTickStun = timeGetTime();
}

void CGameObject::SetBreakTranspar(bool bBreak)
{
	m_bBreakTranspar = bBreak;
	if(m_bBreakTranspar)
		m_dwOldTickBreakTranspar = timeGetTime();
}

void CGameObject::SendMsg_RealFixPosition()
{
	_object_real_fixpositon_zocl Send;

	Send.byObjKind = m_ObjID.m_byKind;
	Send.byObjID = m_ObjID.m_byID;
	Send.wIndex = m_ObjID.m_wIndex;
	Send.dwSerial = m_dwObjSerial;

	BYTE byType[msg_header_num] = {position_msg, object_real_fixpositon_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

void CGameObject::SendMsg_BreakStop()
{
	_break_stop_result_zocl Send;

	Send.byObjID = m_ObjID.m_byID;
	Send.dwObjSerial = m_dwObjSerial;
	::FloatToShort(m_fCurPos, Send.zCur, 3);

	BYTE byType[msg_header_num] = {position_msg, break_stop_result_zocl};
	CircleReport(byType, (char*)&Send, sizeof(Send));
}

//Sector Arrange..

void CGameObject::CalcAbsPos()
{
	_bsp_info* pBspInfo = m_pCurMap->GetBspInfo();

	m_fAbsPos[0] = -pBspInfo->m_nMapMinSize[0]+m_fCurPos[0];
	m_fAbsPos[1] = pBspInfo->m_nMapMaxSize[1]-m_fCurPos[1];
	m_fAbsPos[2] = pBspInfo->m_nMapMaxSize[2]-m_fCurPos[2];
}

bool CGameObject::RerangeSecIndex(DWORD dwOld, DWORD dwNew, DWORD* OUT pdwAdj)
{	//true�� 2ĭ�̻� �̵�..
	if(dwOld == 0xFFFFFFFF)
	{
		*pdwAdj = dwNew;
		return false;
	}

	//��ĭ�̻� �̵����� ����
	int ox = dwOld % m_pCurMap->GetSecInfo()->m_nSecNumW;
	int oy = dwOld / m_pCurMap->GetSecInfo()->m_nSecNumW;

	int nx = dwNew % m_pCurMap->GetSecInfo()->m_nSecNumW;
	int ny = dwNew / m_pCurMap->GetSecInfo()->m_nSecNumW;

	int nxjump = nx - ox;
	int nyjump = ny - oy;

	if(nxjump > 1)			nx--;
	else if(nxjump < -1)	nx++;
	if(nyjump > 1)			ny--;
	else if(nyjump < -1)	ny++;

	*pdwAdj = nx + ny * m_pCurMap->GetSecInfo()->m_nSecNumW;
	
	if(nxjump > 2 || nxjump < -2 || nyjump > 2 || nyjump < -2)
		return true;

	return false;
}

bool CGameObject::UpdateSecList()
{
	DWORD dwNewNum = CalcSecIndex();	//���ο� sector index�� ���Ͽ�..

	if(m_dwCurSec == dwNewNum)
		return true;

	DWORD dwOldSec = m_dwCurSec;
	DWORD dwAdjNum = dwNewNum;
	bool bWhole = false;
	if(m_ObjID.m_byKind == obj_kind_char && m_ObjID.m_byID == obj_id_player)
		bWhole = RerangeSecIndex(m_dwCurSec, dwNewNum, &dwAdjNum);

	if(m_dwCurSec != dwAdjNum)	//������ sector index�� �ٸ��ٸ�..
	{
		if(!m_pCurMap->UpdateSecterList(this, m_dwCurSec, dwAdjNum))
			return false;

		SetCurSecNum(dwAdjNum);	//������ sector�� ���ο� sector�� �����Ѵ�.

		if(dwOldSec != 0xFFFFFFFF)
			ResetSector(dwOldSec, dwAdjNum, bWhole);
		AlterSec();
	}

	return true;
}

void CGameObject::ResetSector(DWORD dwOldSec, DWORD dwNewSec, bool bWhole)
{	
	//�̵����̶��..�׵θ��� �ִ� �÷��̾�Ը� �̵��� �ٽ� ������.
	_sec_info* pSec = m_pCurMap->GetSecInfo();
	int nRight = dwNewSec%pSec->m_nSecNumW - dwOldSec%pSec->m_nSecNumW;
	int nBottom = dwNewSec/pSec->m_nSecNumW - dwOldSec/pSec->m_nSecNumW;

	//�׵θ��� �ִ� ��ġ���� �÷��̾�� ��ġ�� �޴´�.
	_pnt_rect Rect;
	m_pCurMap->GetRectInRadius(&Rect, MAP_STD_RADIUS, GetCurSecNum());

	int x = GetCurSecNum()%pSec->m_nSecNumW;
	int y = GetCurSecNum()/pSec->m_nSecNumW;

	for(int h = Rect.nStarty; h <= Rect.nEndy; h++)
	{
		for(int w = Rect.nStartx; w <= Rect.nEndx; w++)
		{
			if(!bWhole)//��ü ������ �ƴϸ� �׵θ��� ��󳽴�..
			{
				bool bWide = false;
				if(nRight > 0)	//x�� �����϶� 
				{
					if(w == Rect.nEndx)		bWide = true;//�����ʸ𼭸��� 						
				}
				else if(nRight < 0)	//x�� �����϶� 
				{
					if(w == Rect.nStartx)	bWide = true; 	//���ʸ𼭸���  											
				}
				if(!bWide)
				{
					if(nBottom > 0)	//y�� �����϶�
					{
						if(h == Rect.nEndy)	bWide = true;	//�ϴܸ𼭸���  													
					}
					else if(nBottom < 0)	//y�� ���Ұ�
					{
						if(h == Rect.nStarty)	bWide = true;	//��ܸ𼭸���  													
					}
				}
				if(!bWide)
					continue;
			}

			int nSecNum = w+h*pSec->m_nSecNumW;

			CObjectList* pList = m_pCurMap->GetSectorListObj(m_wMapLayerIndex, nSecNum);
			if(!pList)
				continue;
			
			_object_list_point* pPoint = pList->m_Head.m_pNext;
			while(pPoint != &pList->m_Tail)
			{
				CGameObject* pObj = pPoint->m_pItem;
				pPoint = pPoint->m_pNext;

				if(pObj == this)
					continue;			

				//real move
				if(m_bMove)		//��������Ʈ�� �̵����϶� �÷��̾�� �̵��޽����� ������..
				{
					if(pObj->m_ObjID.m_byKind == obj_kind_char && pObj->m_ObjID.m_byID == obj_id_player)
						SendMsg_RealMovePoint(pObj->m_ObjID.m_wIndex);	//Ư��������..
				}
				if(pObj->m_bMove)	//�÷��̾�� ��������Ʈ�� �̵����϶� �̵��޽����� �������Ѵ�...
				{																					
					if(m_ObjID.m_byKind == obj_kind_char && m_ObjID.m_byID == obj_id_player)
						pObj->SendMsg_RealMovePoint(m_ObjID.m_wIndex);
				}

				//fix pos..
				if(!pObj->m_bMove)		//�÷��̾�� ��������Ʈ�� �������϶� �����޽����� �������Ѵ�.
				{
					if(m_ObjID.m_byKind == obj_kind_char && m_ObjID.m_byID == obj_id_player)
						pObj->SendMsg_FixPosition(m_ObjID.m_wIndex);
				}
			}
		}
	}	
}

DWORD CGameObject::CalcSecIndex()
{
	CalcAbsPos();

	DWORD nSecX = m_fAbsPos[0]/MAP_STD_SEC_SIZE;
	DWORD nSecY = m_fAbsPos[2]/MAP_STD_SEC_SIZE;

	_sec_info* pSecInfo = m_pCurMap->GetSecInfo();

	if(nSecX >= pSecInfo->m_nSecNumW || nSecY >= pSecInfo->m_nSecNumH)
	{
		g_Main.m_logSystemError.Write("kind(%d), id(%d).. Out of Sector", m_ObjID.m_byKind, m_ObjID.m_byID);
		return m_dwCurSec;
	}

	return nSecX+nSecY*pSecInfo->m_nSecNumW;
}

bool CGameObject::IsInSector(CGameObject* pObj, int nRadius/* = -1*/)
{
	if(m_pCurMap != pObj->m_pCurMap)
		return false;

	_pnt_rect Rect;
	if(nRadius == -1)
		nRadius = MAP_STD_RADIUS;

	//������ m_dwCurSec m_nRadius�� �ش��ϴ� ������ Rect�� ���Ѵ�.
	m_pCurMap->GetRectInRadius(&Rect, nRadius, m_dwCurSec);

	for(int h = Rect.nStarty; h < Rect.nEndy; h++)
	{
		for(int w = Rect.nStartx; w < Rect.nEndx; w++)
		{
			int nSecNum = w+h*m_pCurMap->GetSecInfo()->m_nSecNumW;
			if(nSecNum == pObj->m_dwCurSec)
				return true;
		}
	}

	return false;
}

DWORD CGameObject::GetCurSecNum()
{
	return m_dwCurSec;
}

void CGameObject::SetCurSecNum(DWORD dwNewSecNum)
{
	m_dwCurSec = dwNewSecNum;
}

//Screen View..

void CGameObject::CalcScrNormalPoint(CRect* prcWnd)
{
	int cx = prcWnd->right;
	int cy = prcWnd->bottom;
	_bsp_info* pBspInfo = m_pCurMap->GetBspInfo();
		
	m_nScreenPos[0] = ((int)m_fAbsPos[0]*cx)/pBspInfo->m_nMapSize[0];
	m_nScreenPos[1] = ((int)m_fAbsPos[2]*cy)/pBspInfo->m_nMapSize[2];
}

void CGameObject::CalcScrExtendPoint(CRect* prcWnd, CRect* prcExtend)
{
	float fExtendPos[2];

	fExtendPos[0] = m_fAbsPos[0]-prcExtend->left;
	fExtendPos[1] = m_fAbsPos[2]-prcExtend->top;

	m_nScreenPos[0] = (int)((fExtendPos[0]*prcWnd->right)/(prcExtend->right-prcExtend->left));
	m_nScreenPos[1] = (int)((fExtendPos[1]*prcWnd->bottom)/(prcExtend->bottom-prcExtend->top));
}

static _pnt_rect __Rect;
void CGameObject::CircleReport(BYTE* pbyType, char* szMsg, int nMsgSize, bool bToOne)
{
	if(!m_pCurMap)
		return;

	m_dwLastSendTime = timeGetTime();

	_sec_info* pSec = m_pCurMap->GetSecInfo();

	m_pCurMap->GetRectInRadius(&__Rect, MAP_STD_RADIUS, m_dwCurSec);

	for(int h = __Rect.nStarty; h <= __Rect.nEndy; h++)
	{
		for(int w = __Rect.nStartx; w <= __Rect.nEndx; w++)
		{
			int nSecNum = w+h*pSec->m_nSecNumW;

			CObjectList* pList = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, nSecNum);
			if(!pList)
				continue;
			
			_object_list_point* pPoint = pList->m_Head.m_pNext;
			while(pPoint != &pList->m_Tail)
			{
				CGameObject* pObj = pPoint->m_pItem;
				pPoint = pPoint->m_pNext;

				_object_id* pID = &pObj->m_ObjID;				

				if(pObj == this && !bToOne)
					continue;					

				g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(pID->m_wIndex, pbyType, szMsg, nMsgSize);	
			}
		}
	}
}

void CGameObject::RangeCircleReport(DWORD dwRangeSecNum, BYTE* pbyType, char* szMsg, int nMsgSize, bool bToOne)
{
	if(!m_pCurMap)
		return;

	CircleReport(pbyType, szMsg, nMsgSize, bToOne);

	if(m_dwCurSec == dwRangeSecNum)
		return;

	_sec_info* pSec = m_pCurMap->GetSecInfo();

	_pnt_rect Rect;
	m_pCurMap->GetRectInRadius(&Rect, MAP_STD_RADIUS, dwRangeSecNum);

	for(int h = Rect.nStarty; h <= Rect.nEndy; h++)
	{
		for(int w = Rect.nStartx; w <= Rect.nEndx; w++)
		{
			//CircleReport�� ���Ե� �������� �Ǵ�..
			if(h < __Rect.nStarty || h > __Rect.nEndy ||
				w < __Rect.nStartx || w > __Rect.nEndx)
			{
				int nSecNum = w+h*pSec->m_nSecNumW;

				CObjectList* pList = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, nSecNum);
				if(!pList)
					continue;
				
				_object_list_point* pPoint = pList->m_Head.m_pNext;
				while(pPoint != &pList->m_Tail)
				{
					CGameObject* pObj = pPoint->m_pItem;
					pPoint = pPoint->m_pNext;

					_object_id* pID = &pObj->m_ObjID;				

					g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(pID->m_wIndex, pbyType, szMsg, nMsgSize);	
				}
			}
		}
	}
}

