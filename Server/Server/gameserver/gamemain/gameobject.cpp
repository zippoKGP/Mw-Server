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
		::MyMessageBox("error", "CGameObject::Init : 오브젝트수가 모자람");
		exit(0);
	}
}

bool CGameObject::Create(_object_create_setdata* pData)
{
	if(m_bLive)
		return false;
	
	m_pCurMap = pData->m_pMap;//CalcSecIndex()땜에 미리 세팅..
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

	m_bLive = true;		//m_bLive 와 m_pCurMap은 동시에 세팅..
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
	
	Loop();//중간에 스스로 timeout등으로 죽어서 데이터가 초기화댈수도 있으니 맨나중에 호출한다.
}

//Set Position..

bool CGameObject::SetCurPos(float* pPos)
{
	//먼저 유효한 좌표인지 check..
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
{	//true면 2칸이상 이동..
	if(dwOld == 0xFFFFFFFF)
	{
		*pdwAdj = dwNew;
		return false;
	}

	//한칸이상 이동인지 학인
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
	DWORD dwNewNum = CalcSecIndex();	//새로운 sector index를 구하여..

	if(m_dwCurSec == dwNewNum)
		return true;

	DWORD dwOldSec = m_dwCurSec;
	DWORD dwAdjNum = dwNewNum;
	bool bWhole = false;
	if(m_ObjID.m_byKind == obj_kind_char && m_ObjID.m_byID == obj_id_player)
		bWhole = RerangeSecIndex(m_dwCurSec, dwNewNum, &dwAdjNum);

	if(m_dwCurSec != dwAdjNum)	//지금의 sector index와 다르다면..
	{
		if(!m_pCurMap->UpdateSecterList(this, m_dwCurSec, dwAdjNum))
			return false;

		SetCurSecNum(dwAdjNum);	//현재의 sector를 새로운 sector로 갱신한다.

		if(dwOldSec != 0xFFFFFFFF)
			ResetSector(dwOldSec, dwAdjNum, bWhole);
		AlterSec();
	}

	return true;
}

void CGameObject::ResetSector(DWORD dwOldSec, DWORD dwNewSec, bool bWhole)
{	
	//이동중이라면..테두리에 있는 플레이어에게만 이동을 다시 날린다.
	_sec_info* pSec = m_pCurMap->GetSecInfo();
	int nRight = dwNewSec%pSec->m_nSecNumW - dwOldSec%pSec->m_nSecNumW;
	int nBottom = dwNewSec/pSec->m_nSecNumW - dwOldSec/pSec->m_nSecNumW;

	//테두리에 있는 위치고정 플레이어에게 위치를 받는다.
	_pnt_rect Rect;
	m_pCurMap->GetRectInRadius(&Rect, MAP_STD_RADIUS, GetCurSecNum());

	int x = GetCurSecNum()%pSec->m_nSecNumW;
	int y = GetCurSecNum()/pSec->m_nSecNumW;

	for(int h = Rect.nStarty; h <= Rect.nEndy; h++)
	{
		for(int w = Rect.nStartx; w <= Rect.nEndx; w++)
		{
			if(!bWhole)//전체 갱신이 아니면 테두리만 골라낸다..
			{
				bool bWide = false;
				if(nRight > 0)	//x축 증가일때 
				{
					if(w == Rect.nEndx)		bWide = true;//오른쪽모서리면 						
				}
				else if(nRight < 0)	//x축 감소일때 
				{
					if(w == Rect.nStartx)	bWide = true; 	//왼쪽모서리면  											
				}
				if(!bWide)
				{
					if(nBottom > 0)	//y축 증가일때
					{
						if(h == Rect.nEndy)	bWide = true;	//하단모서리면  													
					}
					else if(nBottom < 0)	//y축 감소고
					{
						if(h == Rect.nStarty)	bWide = true;	//상단모서리면  													
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
				if(m_bMove)		//모든오브젝트는 이동중일때 플레이어에게 이동메시지를 보낸다..
				{
					if(pObj->m_ObjID.m_byKind == obj_kind_char && pObj->m_ObjID.m_byID == obj_id_player)
						SendMsg_RealMovePoint(pObj->m_ObjID.m_wIndex);	//특정인한테..
				}
				if(pObj->m_bMove)	//플레이어는 모든오브젝트가 이동중일때 이동메시지를 보내게한다...
				{																					
					if(m_ObjID.m_byKind == obj_kind_char && m_ObjID.m_byID == obj_id_player)
						pObj->SendMsg_RealMovePoint(m_ObjID.m_wIndex);
				}

				//fix pos..
				if(!pObj->m_bMove)		//플레이어는 모든오브젝트가 정지중일때 정지메시지를 보내게한다.
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

	//헌재의 m_dwCurSec m_nRadius에 해당하는 면적을 Rect에 구한다.
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
			//CircleReport에 포함된 섹터인지 판단..
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

