#include "stdafx.h"
#include "PlayerStruct.h"
#include "MainThread.h"

CRecordData*	_WEAPON_PARAM::s_pWeaponData = NULL;
CRecordData*	_MASTERY_PARAM::s_pSkillData = NULL;
CRecordData*	_MASTERY_PARAM::s_pForceData = NULL;
int				_MASTERY_PARAM::s_nSkillLvPerMastery[skill_mastery_num];
int				_MASTERY_PARAM::s_nForceLvPerMastery[force_mastery_num];

//거래당사자 매칭..
bool DTradeEqualPerson(CPlayer* IN lp_pOne, CPlayer** OUT lpp_pDst)
{
	if(!lp_pOne->m_bLive || lp_pOne->m_bCorpse || lp_pOne->GetCurSecNum() == 0xFFFFFFFF)
		return false;

	if(!lp_pOne->m_bOper)
		return false;

	if(!lp_pOne->m_pmTrd.bDTradeMode || lp_pOne->m_pmTrd.wDTradeDstIndex == 0xFFFF)
		return false;

	CPlayer* pDst = &g_Player[lp_pOne->m_pmTrd.wDTradeDstIndex];
	if(lp_pOne->m_pmTrd.dwDTradeDstSerial != pDst->m_dwObjSerial)
		return false;

	if(!pDst->m_bLive || pDst->m_bCorpse || pDst->GetCurSecNum() == 0xFFFFFFFF || pDst->m_pCurMap != lp_pOne->m_pCurMap)
		return false;

	if(!pDst->m_pmTrd.bDTradeMode)
		return false;

	if(pDst->m_pmTrd.wDTradeDstIndex != lp_pOne->m_ObjID.m_wIndex || pDst->m_pmTrd.dwDTradeDstSerial != lp_pOne->m_dwObjSerial)
		return false;

	if(lpp_pDst)
		*lpp_pDst = pDst;

	return true;
}
