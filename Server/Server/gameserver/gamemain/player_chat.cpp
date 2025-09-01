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
	{	//������ ��ڸ� ��밡��
		if(m_bySubDgr < MGR_DEGREE_MASTER)
			return;
	}

	//�ڽ��� ������� Ȯ��..
	for(int i = 0; i < MAX_PLAYER; i++)
	{
		CPlayer* p = &g_Player[i];
		if(p->m_bLive)
			p->SendData_ChatTrans(CHAT_TYPE_OPERATOR, 0xFFFFFFFF, bySize, psChatData);
	}

	//LOG..
//	g_Main.m_logChatMsg.Write("[�] %s", psChatData);
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
//	g_Main.m_logChatMsg.Write("[�ֺ�] %s: %s", m_Param.GetCharName(), psChatData);
}

void CPlayer::pc_ChatWhsiperRequest(WORD wDstIndex, BYTE bySize, char* psChatData)
{//�����������..
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

	//���� ���ű����ҿ� �ֳ�Ȯ��..
	CPlayer* pDst = NULL;
	for(int i = 0; i < max_memory_past_whisper; i++)
	{
		_MEM_PAST_WHISPER*	pMem = &m_PastWhiper[i];
	
		if(!pMem->bMemory)
			continue;

		if(strcmp(pMem->szName, pszName))
			continue;

		//��￡�� ã�Ҵµ� ����ƹ�Ÿ�� ������ �ٸ��� ��￡�� �����..
		if(!pMem->pDst || !pMem->pDst->m_bLive || strcmp(pMem->pDst->m_Param.GetCharName(), pMem->szName))
			pMem->bMemory = false;
		else
			//ã�Ҵ�..
			pDst = pMem->pDst;
		break;
	}

	//��ã�Ҵٸ�..��ü���� ������..
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
		if(pDst)//ã�Ҵٸ�..
		{
			//�� ������ ã�´�..
			//���� ����ֳ�..
			_MEM_PAST_WHISPER*	pEmpty = NULL;
			for(int i = 0; i < max_memory_past_whisper; i++)
			{
				if(!m_PastWhiper[i].bMemory)
				{
					pEmpty = &m_PastWhiper[i];
					break;
				}				
			}
			//��Ծ��ٸ� ���� �����°� ����..
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
	{	//�׷��÷��̾ ���ٰ� ����..
		SendMsg_ChatFarFailure();
	}
	else
	{	//�θ����� ����..
		g_Player[pDst->m_ObjID.m_wIndex].SendData_ChatTrans(CHAT_TYPE_WHISPER, m_dwObjSerial, bySize, psChatData);
		SendData_ChatTrans(CHAT_TYPE_WHISPER, m_dwObjSerial, bySize, psChatData);

		//LOG..
//		g_Main.m_logChatMsg.Write("[�Ӽ�] %s->%s: %s", m_Param.GetCharName(), pszName, psChatData);
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
//	g_Main.m_logChatMsg.Write("[��Ƽ] %s: %s", m_Param.GetCharName(), psChatData);
}

void CPlayer::pc_ChatMgrWhisperRequest(BYTE bySize, char* psChatData)//��ڰ��� �Ӹ�
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

//######### PARAMETER EDIT (���� �������� ��� ä���� ����������)#################
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
//	g_Main.m_logChatMsg.Write("[����] %s: %s", m_Param.GetCharName(), psChatData);
}

void CPlayer::pc_ChatCheatRequest(BYTE bySize, char* psChatData)
{
	if(m_byUserDgr == USER_DEGREE_STD)
		return;

	//������
	if(!strncmp("�Ⱥ���", psChatData, 6))
	{
		mgr_tracing(true);
		return;
	}

	//��������
	if(!strncmp("�ߺ���", psChatData, 6))
	{
		mgr_tracing(false);
		return;
	}

	//��������
	if(!strncmp("����", psChatData, 4))
	{
		mgr_matchless(true);
		return;
	}

	//��������
	if(!strncmp("����", psChatData, 4))
	{
		mgr_matchless(false);
		return;
	}

	//�����߹�
	if(!strncmp("kick ", psChatData, 5)) 
	{
		if(::ParsingCommand(&psChatData[5], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			mgr_kick(s_pszDstCheat[0]);
		return;
	}	

	//�ɸ��ͼ�ȯ 
	if(!strncmp("charcall ", psChatData, 9)) 
	{
		if(::ParsingCommand(&psChatData[9], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			mgr_recall_player(s_pszDstCheat[0]);		
		return;
	}

	//����ȯ
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

	//�ɸ���ī��
	if(!strncmp("Ǯ�� ", psChatData, 5)) //ex) Ǯ�� �޷�
	{
		int nWordNum = ::ParsingCommand(&psChatData[5], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum == 1)
			dev_avator_copy(s_pszDstCheat[0]);		
		return;
	}

	//����
	if(!strncmp("? ", psChatData, 2)) 
	{
		int nWordNum = ::ParsingCommand(&psChatData[2], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum > 0)
			mgr_query(nWordNum, s_pszDstCheat[0], s_pszDstCheat[1], s_pszDstCheat[2], s_pszDstCheat[3], s_pszDstCheat[4]);		
		return;
	}

	//ä�ñ���
	if(!strncmp("dumb ", psChatData, 5)) 
	{
		if(::ParsingCommand(&psChatData[5], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			mgr_dumb(s_pszDstCheat[0], atoi(s_pszDstCheat[1]));		
		return;
	}	

	//�ڻ� ex) die
	if(!strncmp("die", psChatData, 3))
	{
		dev_die();		
		return;
	}

	//�������� ex) lv 30
	if(!strncmp("lv ", psChatData, 3))
	{
		if(::ParsingCommand(&psChatData[3], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_lv(atoi(s_pszDstCheat[0]));		
		return;
	}

	//����â�� ��� ������������ ��봩������ ���� ex) upfcitem 10000
	if(!strncmp("upfcitem ", psChatData, 9))
	{
		if(::ParsingCommand(&psChatData[9], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_up_forceitem(atoi(s_pszDstCheat[0]));		
		return;
	}
	
	//��ų�� ��봩������ ���� ex) upskill A000 1000
	if(!strncmp("upskill ", psChatData, 8))
	{
		if(::ParsingCommand(&psChatData[8], 2, s_pszDstCheat, max_cheat_word_size))
			dev_up_skill(s_pszDstCheat[0], atoi(s_pszDstCheat[1]));		
		return;
	}

	//��� ���� �����͸��� ������������ ���� ex) upforce 1000
	if(!strncmp("upforce ", psChatData, 8))
	{
		if(::ParsingCommand(&psChatData[8], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_up_forcemastery(atoi(s_pszDstCheat[0]));		
		return;
	}

	//Ư�� �����͸�����  ex) upmastery 0 1 1000
	if(!strncmp("upmastery ", psChatData, 10))
	{
		if(::ParsingCommand(&psChatData[10], max_cheat_word, s_pszDstCheat, max_cheat_word_size) == 3)
			dev_up_mastery(atoi(s_pszDstCheat[0]), atoi(s_pszDstCheat[1]), atoi(s_pszDstCheat[2]));		
		return;
	}

	//�����õ�����	ex) allskill 1000
	if(!strncmp("allskill ", psChatData, 9))
	{
		if(::ParsingCommand(&psChatData[9], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_up_all(atoi(s_pszDstCheat[0]));		
		return;
	}

	//�ִϸӽ��� ��ȯ���ð��� ����. ex) recalltime 0, recalltime 1
	if(!strncmp("recalltime ", psChatData, 11))
	{
		if(::ParsingCommand(&psChatData[11], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_animus_recall_time_free( atoi(s_pszDstCheat[0]) ? true : false );		
		return;
	}

	//�ִϸӽ��� ����ġ����Ʈ�� �ø�..	ex) recallexp 100000
	if(!strncmp("recallexp ", psChatData, 10))
	{
		if(::ParsingCommand(&psChatData[10], max_cheat_word, s_pszDstCheat, max_cheat_word_size))
			dev_set_animus_exp(atoi(s_pszDstCheat[0]));		
		return;
	}

	//������ ��ġ���� �ʱ�ȭ ex) monset
	if(!strncmp("monset", psChatData, 6))
	{
		dev_init_monster();		
		return;
	}

	//���� �޶�Ʈ õ������ ���� ex) show me the dalant
	if(!strncmp("show me the dalant", psChatData, 18))
	{
		dev_money();		
		return;
	}

	//����â ����ä��.. ex) �λ�����
	if(!strncmp("�λ�����", psChatData, 8))
	{
		dev_full_force();
		return;
	}

	//������
	if(!strncmp("�߸�տ����", psChatData, 12))
	{
		dev_loot_material();		
		return;
	}

	//�������� ������ ���ý�Ŵ ex) *idbca04 2
	if(psChatData[0] == '*')//item looting chitŰ���
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

	//���ϴ� ��Ʈ�� �������� �������� ���� ����
	if(!strncmp("���������� ", psChatData, 11))
	{
		int nWordNum = ::ParsingCommand(&psChatData[11], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum == 2)
		{
			dev_loot_item_lv(s_pszDstCheat[0], atoi(s_pszDstCheat[1]));		
		}
		return;
	}

	//�κ����
	if(!strncmp("�α׸�", psChatData, 6))
	{
		dev_inven_empty();		
		return;
	}

	//����, ���׷��̵� ������ ����
	if(!strncmp("���Ǽ�", psChatData, 6))
	{
		dev_make_succ(true);		
		return;
	}

	//����, ���׷��̵� ������ ���� ����
	if(!strncmp("���Ǽ�", psChatData, 6))
	{
		dev_make_succ(false);		
		return;
	}

	//ȿ���ð� ����
	if(!strncmp("ȿ���ð� ", psChatData, 9))
	{
		int nWordNum = ::ParsingCommand(&psChatData[9], max_cheat_word, s_pszDstCheat, max_cheat_word_size);
		if(nWordNum == 1)
		{
			dev_cont_effect_time(atoi(s_pszDstCheat[0]));		
		}
		return;
	}

	//�ɸ�ȿ������
	if(!strncmp("ȿ���α׸�", psChatData, 10))
	{
		dev_cont_effect_del();		
		return;
	}	
}
