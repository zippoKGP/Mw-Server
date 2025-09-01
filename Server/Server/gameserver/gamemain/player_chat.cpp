#include "stdafx.h"
#include "Player.h"
#include "MainThread.h"

#define max_cheat_word 5
#define max_cheat_word_size 32
static char s_szCheatWord[max_cheat_word][max_cheat_word_size];
static char* s_pszDstCheat[max_cheat_word] = {&s_szCheatWord[0][0], &s_szCheatWord[1][0], &s_szCheatWord[2][0], &s_szCheatWord[3][0], &s_szCheatWord[4][0]};

void CPlayer::pc_ChatOperatorRequest(BYTE bySize, char* psChatData)
{
	if(m_byUserDgr < USER_DEGREE_MGR)
		return;

	if(m_byUserDgr == USER_DEGREE_MGR)
	{	//마스터 운영자만 사용가능
		if(m_bySubDgr < MGR_DEGREE_MASTER)
			return;
	}

	//자신이 운영자인지 확인..
	for(int i = 0; i < MAX_PLAYER; i++)
	{
		CPlayer* p = &g_Player[i];
		if(p->m_bLive)
			p->SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, bySize, psChatData);
	}

	//LOG..
//	g_Main.m_logChatMsg.Write("[운영] %s", psChatData);
}

void CPlayer::pc_ChatCircleRequest(BYTE bySize, char* psChatData)
{
	if(m_pUserDB)
	{
		if(m_pUserDB->m_bChatLock)
			return;
	}

	_chat_message_receipt_udp Send;

	Send.byMessageType = CHAT_TYPE_CIRCLE;
	Send.dwSenderSerial = m_dwObjSerial;
	Send.bySize = bySize+1;
	memcpy(Send.sChatData, psChatData, bySize);
	Send.sChatData[bySize] = NULL;

	BYTE byType[msg_header_num] = {chat_msg, chat_message_receipt_udp};
	CircleReport(byType, (char*)&Send, Send.size(), true);

	//LOG..
//	g_Main.m_logChatMsg.Write("[주변] %s: %s", m_Param.GetCharName(), psChatData);
}

void CPlayer::pc_ChatWhsiperRequest(WORD wDstIndex, BYTE bySize, char* psChatData)
{//사용하지않음..
//	CPlayer* pDst = &g_Main.m_Player[wDstIndex];

//	::__trace("%s: pc_ChatWhsiperRequest(%s, %d, %s)", m_Param.GetCharName(), pDst->m_Param.GetCharName(), bySize, psChatData);

//	pDst->SendData_ChatTrans(CHAT_TYPE_WHISPER, m_dwObjSerial, bySize, psChatData);
//	SendData_ChatTrans(CHAT_TYPE_WHISPER, m_dwObjSerial, bySize, psChatData);
}

void CPlayer::pc_ChatFarRequest(char* pszName, BYTE bySize, char* psChatData)
{
	if(m_pUserDB)
	{
		if(m_pUserDB->m_bChatLock)
			return;
	}

//	::__trace("%s: pc_ChatFarRequest(%s, %d, %s)", m_Param.GetCharName(), pszName, bySize, psChatData);

	//먼저 과거기억장소에 있나확인..
	CPlayer* pDst = NULL;
	for(int i = 0; i < max_memory_past_whisper; i++)
	{
		_MEM_PAST_WHISPER*	pMem = &m_PastWhiper[i];
	
		if(!pMem->bMemory)
			continue;

		if(strcmp(pMem->szName, pszName))
			continue;

		//기억에서 찾았는데 월드아바타랑 정보가 다르면 기억에서 지운다..
		if(!pMem->pDst || !pMem->pDst->m_bLive || strcmp(pMem->pDst->m_Param.GetCharName(), pMem->szName))
			pMem->bMemory = false;
		else
			//찾았당..
			pDst = pMem->pDst;
		break;
	}

	//못찾았다면..전체에서 뒤진다..
	if(!pDst)
	{
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			CPlayer* p = &g_Player[i];
			if(!p->m_bLive)
				continue;
			if(!strcmp(p->m_Param.GetCharName(), pszName))
			{
				pDst = p;
				break;
			}
		}
		if(pDst)//찾았다면..
		{
			//빈 기억장소 찾는다..
			//먼저 빈거있나..
			_MEM_PAST_WHISPER*	pEmpty = NULL;
			for(int i = 0; i < max_memory_past_whisper; i++)
			{
				if(!m_PastWhiper[i].bMemory)
				{
					pEmpty = &m_PastWhiper[i];
					break;
				}				
			}
			//빈게없다면 가장 오래됀걸 고른다..
			if(!pEmpty)
			{
				DWORD dwMin = 0xFFFFFFFF;			
				for(int i = 0; i < max_memory_past_whisper; i++)
				{
					if(m_PastWhiper[i].dwMemoryTime < dwMin)
					{
						dwMin = m_PastWhiper[i].dwMemoryTime;
						pEmpty = &m_PastWhiper[i];
					}				
				}
			}
			pEmpty->bMemory = true;
			pEmpty->dwMemoryTime = timeGetTime();
			pEmpty->pDst = pDst;
			strcpy(pEmpty->szName, pDst->m_Param.GetCharName());			
		}
	}

	if(!pDst)
	{	//그런플레이어가 없다고 전함..
		SendMsg_ChatFarFailure();
	}
	else
	{	//두명한테 날림..
		g_Player[pDst->m_ObjID.m_wIndex].SendData_ChatTrans(CHAT_TYPE_WHISPER, m_dwObjSerial, bySize, psChatData);
		SendData_ChatTrans(CHAT_TYPE_WHISPER, m_dwObjSerial, bySize, psChatData);

		//LOG..
//		g_Main.m_logChatMsg.Write("[귓속] %s->%s: %s", m_Param.GetCharName(), pszName, psChatData);
	}
}

void CPlayer::pc_ChatPartyRequest(BYTE bySize, char* psChatData)
{
	if(m_pUserDB)
	{
		if(m_pUserDB->m_bChatLock)
			return;
	}

	if(!m_pPartyMgr->IsPartyMode())
		return;

	_chat_message_receipt_udp Data;

	Data.byMessageType = CHAT_TYPE_PARTY;
	Data.dwSenderSerial = m_dwObjSerial;
	Data.bySize = bySize+1;
	memcpy(Data.sChatData, psChatData, bySize);
	Data.sChatData[bySize] = NULL;

	BYTE byType[msg_header_num] = {chat_msg, chat_message_receipt_udp};
	CPartyPlayer** ppMem = m_pPartyMgr->GetPtrPartyMember();

	for(int i = 0; i < member_per_party; i++)
	{
		if(!ppMem[i])	
			continue;

		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(ppMem[i]->m_wZoneIndex, byType, (char*)&Data, Data.size());
	}

	//LOG..
//	g_Main.m_logChatMsg.Write("[파티] %s: %s", m_Param.GetCharName(), psChatData);
}

void CPlayer::pc_ChatMgrWhisperRequest(BYTE bySize, char* psChatData)//운영자간의 귓말
{
	mgr_whisper(bySize, psChatData);
}

void CPlayer::pc_ChatRaceRequest(BYTE bySize, char* psChatData)
{
	if(m_pUserDB)
	{
		if(m_pUserDB->m_bChatLock)
			return;
	}

//######### PARAMETER EDIT (같은 종족에게 모두 채팅을 보낼수있음)#################
//
	if(m_EP.m_fEff_Have[_EFF_HAVE::Chat_All_Race] == 0)
		return;

	_announ_message_receipt_udp Data;

	Data.byMessageType = CHAT_TYPE_RACE;
	strcpy(Data.szSenderName, m_Param.GetCharName());
	Data.bySize = bySize+1;
	memcpy(Data.sChatData, psChatData, bySize);
	Data.sChatData[bySize] = NULL;

	BYTE byType[msg_header_num] = {chat_msg, announ_message_receipt_udp};

	for(int i = 0; i < MAX_PLAYER; i++)
	{
		CPlayer* p = &g_Player[i];

		if(!p->m_bLive)
			continue;

		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(p->m_ObjID.m_wIndex, byType, (char*)&Data, Data.size());
	}

	//LOG..
//	g_Main.m_logChatMsg.Write("[종족] %s: %s", m_Param.GetCharName(), psChatData);
}

void CPlayer::pc_ChatCheatRequest(BYTE bySize, char* psChatData)
{
	if(m_byUserDgr == USER_DEGREE_STD)
		return;

	//투명설정
	if(!strncmp("안보여", psChatData, 6))
	{
		mgr_tracing(true);
		return;
	}

	//투명해제
	if(!strncmp("잘보여", psChatData, 6))
	{
		mgr_tracing(false);
		return;
	}

	//무적설정
	if(!strncmp("무적", psChatData, 4))
	{
		mgr_matchless(true);
		return;
	}

	//무적해제
	if(!strncmp("유적", psChatData, 4))
	{
		mgr_matchless(false);
		return;
	}

	//강제추방
	if(!strncmp("kick ", psChatData, 5)) 
	{
		if(::ParsingCommand(&psChatData[5], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			mgr_kick(s_pszDstCheat[0]);
		return;
	}	

	//케릭터소환 
	if(!strncmp("charcall ", psChatData, 9)) 
	{
		if(::ParsingCommand(&psChatData[9], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			mgr_recall_player(s_pszDstCheat[0]);		
		return;
	}

	//몹소환
	if(!strncmp("moncall ", psChatData, 8)) //ex) moncall 05400
	{
		int nWordNum = ::ParsingCommand(&psChatData[8], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum > 0)
		{
			int nNum = 1;
			if(nWordNum > 1)
				nNum = atoi(s_pszDstCheat[1]);

			mgr_recall_mon(s_pszDstCheat[0], nNum);		
		}
		return;
	}

	//케릭터카피
	if(!strncmp("풀빵 ", psChatData, 5)) //ex) 풀빵 메롱
	{
		int nWordNum = ::ParsingCommand(&psChatData[5], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum == 1)
			dev_avator_copy(s_pszDstCheat[0]);		
		return;
	}

	//질의
	if(!strncmp("? ", psChatData, 2)) 
	{
		int nWordNum = ::ParsingCommand(&psChatData[2], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum > 0)
			mgr_query(nWordNum, s_pszDstCheat[0], s_pszDstCheat[1], s_pszDstCheat[2], s_pszDstCheat[3], s_pszDstCheat[4]);		
		return;
	}

	//채팅금지
	if(!strncmp("dumb ", psChatData, 5)) 
	{
		if(::ParsingCommand(&psChatData[5], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			mgr_dumb(s_pszDstCheat[0], atoi(s_pszDstCheat[1]));		
		return;
	}	

	//자살 ex) die
	if(!strncmp("die", psChatData, 3))
	{
		dev_die();		
		return;
	}

	//레벨변경 ex) lv 30
	if(!strncmp("lv ", psChatData, 3))
	{
		if(::ParsingCommand(&psChatData[3], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_lv(atoi(s_pszDstCheat[0]));		
		return;
	}

	//포스창의 모든 포스아이템의 사용누적수를 변경 ex) upfcitem 10000
	if(!strncmp("upfcitem ", psChatData, 9))
	{
		if(::ParsingCommand(&psChatData[9], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_up_forceitem(atoi(s_pszDstCheat[0]));		
		return;
	}
	
	//스킬의 사용누적수를 변경 ex) upskill A000 1000
	if(!strncmp("upskill ", psChatData, 8))
	{
		if(::ParsingCommand(&psChatData[8], 2, s_pszDstCheat, max_cheat_word_size))
			dev_up_skill(s_pszDstCheat[0], atoi(s_pszDstCheat[1]));		
		return;
	}

	//모든 포스 마스터리의 성공누적수를 변경 ex) upforce 1000
	if(!strncmp("upforce ", psChatData, 8))
	{
		if(::ParsingCommand(&psChatData[8], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_up_forcemastery(atoi(s_pszDstCheat[0]));		
		return;
	}

	//특정 마스터리증가  ex) upmastery 0 1 1000
	if(!strncmp("upmastery ", psChatData, 10))
	{
		if(::ParsingCommand(&psChatData[10], max_cheat_word, s_pszDstCheat, max_cheat_word_size) == 3)
			dev_up_mastery(atoi(s_pszDstCheat[0]), atoi(s_pszDstCheat[1]), atoi(s_pszDstCheat[2]));		
		return;
	}

	//모든숙련도변경	ex) allskill 1000
	if(!strncmp("allskill ", psChatData, 9))
	{
		if(::ParsingCommand(&psChatData[9], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_up_all(atoi(s_pszDstCheat[0]));		
		return;
	}

	//애니머스의 소환대기시간을 없앰. ex) recalltime 0, recalltime 1
	if(!strncmp("recalltime ", psChatData, 11))
	{
		if(::ParsingCommand(&psChatData[11], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_animus_recall_time_free( atoi(s_pszDstCheat[0]) ? true : false );		
		return;
	}

	//애니머스의 경험치포인트를 올림..	ex) recallexp 100000
	if(!strncmp("recallexp ", psChatData, 10))
	{
		if(::ParsingCommand(&psChatData[10], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_set_animus_exp(atoi(s_pszDstCheat[0]));		
		return;
	}

	//몬스터의 위치등을 초기화 ex) monset
	if(!strncmp("monset", psChatData, 6))
	{
		dev_init_monster();		
		return;
	}

	//골드와 달란트 천만으로 변경 ex) show me the dalant
	if(!strncmp("show me the dalant", psChatData, 18))
	{
		dev_money();		
		return;
	}

	//포스창 가득채움.. ex) 인생역전
	if(!strncmp("인생역전", psChatData, 8))
	{
		dev_full_force();
		return;
	}

	//재료루팅
	if(!strncmp("발명왕에디손", psChatData, 12))
	{
		dev_loot_material();		
		return;
	}

	//아이템을 주위에 루팅시킴 ex) *idbca04 2
	if(psChatData[0] == '*')//item looting chit키라면
	{
		int nWordNum = ::ParsingCommand(&psChatData[1], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum > 0)
		{
			int nItemNum = 1;
			if(nWordNum == 2)
				nItemNum = atoi(s_pszDstCheat[1]);			

			dev_loot_item(s_pszDstCheat[0], nItemNum);		
		}
		return;
	}

	//원하는 파트의 지정레벨 아이템을 전부 루팅
	if(!strncmp("레벨아이템 ", psChatData, 11))
	{
		int nWordNum = ::ParsingCommand(&psChatData[11], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum == 2)
		{
			dev_loot_item_lv(s_pszDstCheat[0], atoi(s_pszDstCheat[1]));		
		}
		return;
	}

	//인벤비움
	if(!strncmp("싸그리", psChatData, 6))
	{
		dev_inven_empty();		
		return;
	}

	//제작, 업그레이드 무조건 성공
	if(!strncmp("신의손", psChatData, 6))
	{
		dev_make_succ(true);		
		return;
	}

	//제작, 업그레이드 무조건 성공 해제
	if(!strncmp("닭의손", psChatData, 6))
	{
		dev_make_succ(false);		
		return;
	}

	//효과시간 세팅
	if(!strncmp("효과시간 ", psChatData, 9))
	{
		int nWordNum = ::ParsingCommand(&psChatData[9], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum == 1)
		{
			dev_cont_effect_time(atoi(s_pszDstCheat[0]));		
		}
		return;
	}

	//걸린효과제거
	if(!strncmp("효과싸그리", psChatData, 10))
	{
		dev_cont_effect_del();		
		return;
	}	
}
