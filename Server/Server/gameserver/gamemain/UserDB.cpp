#include "StdAfx.h"
#include "UserDB.h"
#include "MyUtil.h"
#include "ClassFld.h"
#include "pt_account_world.h"
#include "pt_zone_client.h"
#include "pt_world_msg.h"
#include "Player.h"
#include "MainThread.h"

int	CUserDB::s_nLoginNum = 0;

_MOVE_LOBBY_DELAY		CUserDB::s_MoveLobbyDelay;
_MESSENGER_KEY_DELAY	CUserDB::s_MessengerKeyDelay;
_MESSENGER_ENTER_CHECK	CUserDB::s_MessengerEnterCheck;

CUserDB::CUserDB()
{
	m_dwAccountSerial = 0xFFFFFFFF;
	m_AvatorData.InitData();
	m_AvatorData_bk.InitData();
	m_bActive = false;
	m_bField = false;
	m_bChatLock = false;
	m_bNoneUpdateData = false;
	m_dwSerial = 0xFFFFFFFF;
	m_idWorld.dwSerial = 0xFFFFFFFF;
	m_gidGlobal.dwSerial = 0xFFFFFFFF;
	m_idMessenger.dwSerial = 0xFFFFFFFF;
}

void CUserDB::Init(DWORD dwIndex)
{
	m_idWorld.wIndex =  (WORD)dwIndex;

	ParamInit();
	m_tmrCheckPlayMin.BeginTimer(60000);
	m_tmrCheckVariableUpdate.BeginTimer(10000);
	m_szFileName[0] = NULL;
}

void CUserDB::ParamInit()
{
	m_idWorld.dwSerial = 0xFFFFFFFF;
	m_gidGlobal.dwSerial = 0xFFFFFFFF;
	m_idMessenger.dwSerial = 0xFFFFFFFF;

	m_AvatorData.InitData();
	m_AvatorData_bk.InitData();
	for(int i = 0; i < MAX_CHAR_PER_WORLDUSER; i++)
		m_RegedList[i].init();

	m_bActive = false;
	m_bDBWaitState = false;
	m_bChatLock = false;
	m_bNoneUpdateData = false;
	m_byUserDgr = USER_DEGREE_STD;
	m_bySubDgr = 0;
	m_ss.Init();

	m_dwTotalPlayMin = 0;
	m_szFileName[0] = NULL;
}

void CUserDB::SetWorldCLID(DWORD dwSerial, IP_ADDR* pipAddr)
{//소켓이 첨 연결됐을때..
	m_idWorld.dwSerial = dwSerial;
	m_ipAddress = *pipAddr;
	m_ss.Init();
}

void CUserDB::StartFieldMode()
{
	m_bField = true;
}

void CUserDB::ForceCloseCommand(bool bPushClose, DWORD dwPushIP)
{
	//강제접속을 알린다.
	//
	_server_notify_inform_zone Send;

	Send.wMsgCode = NOTI_CODE_FORCE_CLOSE;
	Send.wMsgCode/*bPushClose,ydq*/ = bPushClose;
	Send.dwPushIP = dwPushIP;

	BYTE byType[msg_header_num] = {system_msg, server_notify_inform_zone};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());

	//강제로 끊는다.
	//
	g_Main.m_Network.Close(client_line, m_idWorld.wIndex, true);
}

void CUserDB::DirectClose()
{
	g_Main.m_Network.Close(client_line, m_idWorld.wIndex, false);
}

void CUserDB::SetChatLock(bool bLock)
{
	m_bChatLock = bLock;
}

void CUserDB::GenerateMessengerKey()
{
	for(int i = 0; i < CHECK_KEY_NUM; i++)
	{
		DWORD dwR1 = ::rand();
		m_dwMessengerKey[i] = (dwR1<<16) + ::rand();
	}
}

void CUserDB::EnterMessenger(_CLID* pidWorld, _CLID* pidLocal)
{
	if(!m_bActive || m_idWorld.dwSerial != pidWorld->dwSerial)
	{//그사이에 나감..
		_close_user_command_wrmg Send;

		memcpy(&Send.idLocal, pidLocal, sizeof(_CLID));

		g_Main.m_Network.m_pProcess[messenger_line]->LoadSendMsg(0, Send.head(), (char*)&Send, Send.size());		
	}
	else
	{
		if(m_idMessenger.dwSerial == 0xFFFFFFFF)
		{
			//로컬아이디세팅
			memcpy(&m_idMessenger, pidLocal, sizeof(_CLID));

			//케릭터 선택결과 알리기..
			_sel_char_result_zone Send;

			Send.byRetCode = RET_CODE_SUCCESS;
			Send.bySlotIndex = m_AvatorData.dbAvator.m_bySlotIndex;
			Send.dwWorldSerial = m_AvatorData.dbAvator.m_dwRecordNum;//케릭터db시리얼로..

			BYTE byType[msg_header_num] = {system_msg, sel_char_result_zone};
			g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());		
		}
		else
		{//log..
		}
	}					
}

void CUserDB::ExitMessenger(_CLID* pidWorld)
{
	if(m_idWorld.dwSerial == pidWorld->dwSerial)
	{	//아직 남아있음..
		//강제종료
		ForceCloseCommand(false, 0xFFFFFFFF);
	}

	//메신져서버에서도 나감을 표시(m_idMessenger의 시리얼을초기화)
	m_idMessenger.dwSerial = 0xFFFFFFFF;
}

void CUserDB::DummyCreate(DWORD dwSerial)
{
	Init(0);
	m_bActive = true;
	m_dwSerial = dwSerial;
}

bool CUserDB::Enter_Account(DWORD dwAccountSerial, DWORD dwIP, bool bFullMode)
{
	DWORD dwSerial = 0;
	_WAIT_ENTER_ACCOUNT* pWaitData = NULL;

	if(m_bActive)
		return false;

	if(!m_ss.chk_enter())
		return false;

	//월드 입장 인증을 받은 대기자 리스트에 있는지 확인..
	for(int i = 0; i < MAX_WAIT_NETMOVE_FROM_WORLD; i++)
	{
		_WAIT_ENTER_ACCOUNT* p = &g_Main.m_WaitEnterAccount[i];

		if(!p->m_bLoad)
			continue;

		if(p->m_dwAccountSerial != dwAccountSerial)
			continue;

		//find..
		p->Release();
		pWaitData = p;
		break;
	}

	if(!pWaitData)
		return false;

	if(g_Player[m_idWorld.wIndex].m_bLoad)
		return false;

	m_bActive = true;

	m_dwAccountSerial = dwAccountSerial;
	strcpy(m_szAccountID, pWaitData->m_szAccountID);
	m_byUserDgr = pWaitData->m_byUserDgr;
	m_bySubDgr = pWaitData->m_bySubDgr;
	m_dwIP = dwIP;
	memcpy(&m_gidGlobal, &pWaitData->m_gidGlobal, sizeof(_GLBID));
	m_dwTotalPlayMin = 0;
	m_dwSerial = 0xFFFFFFFF;
	m_pDataMapping = NULL;
	m_bDBWaitState = false;
	m_bFileUpdate = false;
	m_bVariableUpdate = false;
	m_bChatLock = g_Main.IsChatLockAccount(dwAccountSerial);
	m_bWndFullMode = bFullMode;

	{//send to account server
		_enter_world_request_wrac Query;

		memcpy(&Query.gidGlobal, &m_gidGlobal, sizeof(_GLBID));
		memcpy(&Query.idLocal, &m_idWorld, sizeof(_CLID));

		BYTE byType[msg_header_num] = {system_msg, enter_world_request_wrac};
		g_Main.m_Network.m_pProcess[account_line]->LoadSendMsg(0, byType, (char*)&Query, Query.size());		
	}

	{//result
		_enter_world_result_zone Send;

		Send.byResult = 0;
		Send.byUserGrade = m_byUserDgr;

		BYTE byType[msg_header_num] = {system_msg, enter_world_result_zone};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());
	}

	return true;
}

void CUserDB::Exit_Account_Request()
{
//	if(m_bDBWaitState)
//		return;

	if(!m_bActive)
	{
		ParamInit();	//connect하자마자 끊기는것들을 위해..
		return;
	}

	//메신져서버에도 알린다..
	if(m_idMessenger.dwSerial != 0xFFFFFFFF)	//초기화됀상태인것은 아직 그쪽에 접속을 안하거나, 
	{											//그쪽에서 나감을 처리한수 나간것..
		_close_user_command_wrmg Send;

		memcpy(&Send.idLocal, &m_idMessenger, sizeof(_CLID));

		g_Main.m_Network.m_pProcess[messenger_line]->LoadSendMsg(0, Send.head(), (char*)&Send, Send.size());
	}

	if(m_dwSerial != 0xFFFFFFFF)	//선택한 캐릭터가 있을때 업데이트..
	{
		UpdateBKFile(true);

		_qry_sheet_logout Sheet;

		Sheet.dwAvatorSerial = m_dwSerial;
		memcpy(&Sheet.NewData, &m_AvatorData, sizeof(_AVATOR_DATA));
		memcpy(&Sheet.OldData, &m_AvatorData_bk, sizeof(_AVATOR_DATA));
		strcpy(Sheet.szHBKFileName, m_szFileName);
		Sheet.bCheckLowHigh = !m_bNoneUpdateData;

		if(!g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, _DB_QRY_SYN_DATA::qry_case_logout, (char*)&Sheet, Sheet.size()))
		{
			Exit_Account_Complete(RET_CODE_INTERNAL_ERROR);
			return;
		}

		m_bDBWaitState = true;
	}
	else
	{
		Exit_Account_Complete(RET_CODE_SUCCESS);
	}
}

void CUserDB::Exit_Account_Complete(BYTE byRetCode)
{
	m_bDBWaitState = false;

	_logout_account_request_wrac Query;		//계정서버에게 계정로그아웃을 알림

	memcpy(&Query.gidGlobal, &m_gidGlobal, sizeof(_GLBID));

	BYTE byType[msg_header_num] = {system_msg, logout_account_request_wrac};
	g_Main.m_Network.m_pProcess[account_line]->LoadSendMsg(0, byType, (char*)&Query, Query.size());

	ParamInit();
}

bool CUserDB::Reged_Char_Request()
{
	if(!m_bActive)
		return false;

	if(!m_ss.chk_reged())
		return false;

	if(m_bDBWaitState)
		return false;

	_qry_sheet_reged Sheet;

	Sheet.dwAccountSerial = m_dwAccountSerial;

	if(!g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, _DB_QRY_SYN_DATA::qry_case_reged, (char*)&Sheet, Sheet.size()))
		return false;

	m_bDBWaitState = true;

	return true;
}

void CUserDB::Reged_Char_Complete(BYTE byRetCode, _REGED* pRegedList)
{
	m_bDBWaitState = false;

	if(byRetCode == RET_CODE_SUCCESS)
	{
		memcpy(m_RegedList, pRegedList, sizeof(_REGED)*MAX_CHAR_PER_WORLDUSER);

		//케릭터창에 보여줄 장착 아이템의 업그레이드레벨수를 계산
		for(int i = 0; i < MAX_CHAR_PER_WORLDUSER; i++)
		{
			_REGED* p = &m_RegedList[i];

			if(p->m_bySlotIndex == 0xFF)
				continue;
		
			p->UpdateEquipLv();//장착레벨계산
		}
	}

	_reged_char_result_zone Send;

	Send.byRetCode = byRetCode;

	int nCnt = 0;
	if(byRetCode == RET_CODE_SUCCESS)
	{
		for(int i = 0; i < MAX_CHAR_PER_WORLDUSER; i++)
		{
			if(m_RegedList[i].m_bySlotIndex != 0xFF)
			{
				memcpy(&Send.RegedList[nCnt], &m_RegedList[i], sizeof(_REGED_AVATOR_DB));
				if(Send.RegedList[nCnt].m_byLevel == 0)//예전에 생성만하고 종료한경우..
					Send.RegedList[nCnt].m_byLevel = 1;

				nCnt++;
			}
		}
		Send.byCharNum = nCnt;
	}
	
	BYTE byType[msg_header_num] = {system_msg, reged_char_result_zone};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());
}

bool CUserDB::Insert_Char_Request(char* pszCharName, BYTE bySlotIndex, BYTE byRaceSexCode, char* pszClassCode, DWORD dwBaseShape)
{//false로 return하면 자동 disconnect..

	BYTE byRet = RET_CODE_SUCCESS;

	if(!m_bActive)
		return false;

	if(!(m_ss.bReged && !m_ss.bSelect))
		return false;

	if(m_bDBWaitState)
		return false;

	if(m_RegedList[bySlotIndex].m_bySlotIndex != 0xFF)
		return false;

	//첫글자가 '*'이여서는 안됌.. (케릭터지울때 임시로 앞에 *표시를 하므로..혼동우려)
	if(pszCharName[0] == '*')
	{
		Insert_Char_Complete(RET_CODE_ERR_CHAR_NAME, NULL);
		return true;
	}

	_class_fld* pClassFld = (_class_fld*)g_Main.m_tblClass.GetRecord(pszClassCode);
	if(!pClassFld)
		return false;

	if(pClassFld->m_nGrade != 0)
		return false;

	if(pClassFld->m_nRaceCode != byRaceSexCode/2)
		return false;

	_qry_sheet_insert Sheet;

	Sheet.dwAccountSerial = m_dwAccountSerial;
	strcpy(Sheet.szAccountID, m_szAccountID);
	strcpy(Sheet.InsertData.m_szAvatorName, pszCharName);
	Sheet.InsertData.m_byRaceSexCode = byRaceSexCode;
	Sheet.InsertData.m_bySlotIndex = bySlotIndex;
	Sheet.InsertData.m_dwBaseShape = dwBaseShape;
	strcpy(Sheet.InsertData.m_szClassCode, pClassFld->m_strCode);

	if(!g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, _DB_QRY_SYN_DATA::qry_case_insert, (char*)&Sheet, Sheet.size()))
		return false;

	m_bDBWaitState = true;

	return true;
}

void CUserDB::Insert_Char_Complete(BYTE byRetCode, _REGED_AVATOR_DB* pInsertData)
{
	m_bDBWaitState = false;

	BYTE bySlotIndex = 0xFF;
	if(byRetCode == RET_CODE_SUCCESS)
	{
		bySlotIndex = pInsertData->m_bySlotIndex;
		memcpy(&m_RegedList[bySlotIndex], pInsertData, sizeof(_REGED_AVATOR_DB));
	}
	
	_add_char_result_zone Send;

	Send.byRetCode = byRetCode;
	Send.byAddSlotIndex = bySlotIndex;

	BYTE byType[msg_header_num] = {system_msg, add_char_result_zone};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());		
}

bool CUserDB::Delete_Char_Request(BYTE bySlotIndex)
{
	if(!m_bActive)
		return false;

	if(!(m_ss.bReged && !m_ss.bSelect))
		return false;

	if(m_bDBWaitState)
		return false;

	if(m_RegedList[bySlotIndex].m_bySlotIndex == 0xFF)
		return false;

	_qry_sheet_delete Sheet;
	Sheet.bySlotIndex = bySlotIndex;
	Sheet.byRaceCode = m_RegedList[bySlotIndex].m_byRaceSexCode/2;
	Sheet.dwAvatorSerial = m_RegedList[bySlotIndex].m_dwRecordNum;

	if(!g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, _DB_QRY_SYN_DATA::qry_case_delete, (char*)&Sheet, Sheet.size()))
		return false;

	m_bDBWaitState = true;

	return true;
}

void CUserDB::Delete_Char_Complete(BYTE byRetCode, BYTE bySlotIndex)
{
	m_bDBWaitState = false;

	if(byRetCode == RET_CODE_SUCCESS)
	{
		m_RegedList[bySlotIndex].m_bySlotIndex = 0xFF;
	}

	_del_char_result_zone Send;

	Send.byRetCode = byRetCode;
	Send.bySlotIndex = bySlotIndex;

	BYTE byType[msg_header_num] = {system_msg, del_char_result_zone};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());			
}

bool CUserDB::Select_Char_Request(BYTE bySlotIndex)
{
	if(!m_bActive)
		return false;

	if(!m_ss.bReged)
		return false;

	if(!m_ss.chk_select())
		return false;

	if(m_bDBWaitState)
		return false;

	if(m_RegedList[bySlotIndex].m_bySlotIndex == 0xFF)
		return false;

	_qry_sheet_load	Sheet;

	Sheet.dwAvatorSerial = m_RegedList[bySlotIndex].m_dwRecordNum;
	memcpy(&Sheet.LoadData.dbAvator, &m_RegedList[bySlotIndex], sizeof(_REGED));//base 파트 먼저 카피..

	if(!g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, _DB_QRY_SYN_DATA::qry_case_load, (char*)&Sheet, Sheet.size()))
		return false;

	m_bDBWaitState = true;

	return true;
}

void CUserDB::Select_Char_Complete(BYTE byRetCode, _AVATOR_DATA* pLoadData, char* pszHBKFileName)
{
	m_bDBWaitState = false;

	BYTE bySlotIndex = 0xFF;

	if(byRetCode == RET_CODE_SUCCESS)
	{
		memcpy(&m_AvatorData, pLoadData, sizeof(_AVATOR_DATA));	
		memcpy(&m_AvatorData_bk, pLoadData, sizeof(_AVATOR_DATA));	

		//아이템 유효성 체크 및 보정
		ItemValidCheckRevise(&m_AvatorData);

		strcpy(m_szAvatorName, pLoadData->dbAvator.m_szAvatorName);
		
		m_tmrCheckPlayMin.TermTimeRun();//재가동..
		m_tmrCheckVariableUpdate.TermTimeRun();//재가동..
		strcpy(m_szFileName, pszHBKFileName);

		m_dwSerial = pLoadData->dbAvator.m_dwRecordNum;
		bySlotIndex = pLoadData->dbAvator.m_bySlotIndex;

		//첫시작하는 케릭터..
		if(m_AvatorData.dbAvator.m_byLevel == 0)
		{
			if(!FirstSettingData())
				g_Main.m_logSystemError.Write("FirstSettingData() == false : char: %s, class: %s", m_szAvatorName, m_AvatorData.dbAvator.m_szClassCode);
		}

		//플레이어 호출..
		if(!g_Player[m_idWorld.wIndex].Load(this))
		{
			g_Main.m_Network.Close(client_line, m_idWorld.wIndex, false);
			return;
		}
	}

	//send..

	//만약 메신져서버를 이용하지 않거나 실패시 바로 결과처리
	if(!g_Main.m_bUsingMsgr)
	{
		_sel_char_result_zone Send;

		Send.byRetCode = byRetCode;
		Send.bySlotIndex = bySlotIndex;
		Send.dwWorldSerial = m_AvatorData.dbAvator.m_dwRecordNum;//케릭터db시리얼로..

		BYTE byType[msg_header_num] = {system_msg, sel_char_result_zone};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());					
	}
	else
	{
		if(byRetCode ==  RET_CODE_SUCCESS)
		{
			GenerateMessengerKey();
			//메신져에 trans..
			_trans_user_inform_wrmg Send;

			memcpy(&Send.idWorld, &m_idWorld, sizeof(_CLID));
			memcpy(Send.dwKey, m_dwMessengerKey, sizeof(DWORD)*CHECK_KEY_NUM);
			Send.dwClientIP = m_dwIP;
			Send.dwAccountSerial = m_dwAccountSerial;
			Send.dwAvatorSerial = m_dwSerial;
			Send.wAvatorIndex = m_idWorld.wIndex;
			strcpy(Send.szAccountID, m_szAccountID);
			strcpy(Send.szAvatorName, m_szAvatorName);
			Send.byUserDgr = m_byUserDgr;
			Send.bySubDgr = m_bySubDgr;
			Send.byRaceSexCode = m_AvatorData.dbAvator.m_byRaceSexCode;			

			BYTE byType[msg_header_num] = {system_msg, trans_user_inform_wrmg};
			g_Main.m_Network.m_pProcess[messenger_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());					

			//유저에 1초후에 key 알림..
			s_MessengerKeyDelay.Push(m_idWorld.wIndex, m_idWorld.dwSerial); 

			//계정서버에 케릭터 선택을 알림..
			_select_avator_report_wrac Report;

			memcpy(&Report.gidGlobal, &m_gidGlobal, sizeof(_GLBID));
			strcpy(Report.szCharName, m_szAvatorName);
			Report.dwAvatorSerial = m_dwSerial;

			BYTE byTypeReport[msg_header_num] = {system_msg, select_avator_report_wrac};
			g_Main.m_Network.m_pProcess[account_line]->LoadSendMsg(0, byTypeReport, (char*)&Report, Report.size());					
		}
		else
		{
			_sel_char_result_zone Send;

			Send.byRetCode = byRetCode;
			Send.bySlotIndex = bySlotIndex;

			BYTE byType[msg_header_num] = {system_msg, sel_char_result_zone};
			g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());					
		}
	}
}

bool CUserDB::Lobby_Char_Request()
{
	if(!m_bActive)
		return false;

	if(!m_bField || m_dwSerial == 0xFFFFFFFF)
		return false;

	g_Player[m_idWorld.wIndex].NetClose();
	
	UpdateBKFile(true);

	_qry_sheet_lobby Sheet;

	Sheet.dwAvatorSerial = m_dwSerial;
	memcpy(&Sheet.NewData, &m_AvatorData, sizeof(_AVATOR_DATA));
	memcpy(&Sheet.OldData, &m_AvatorData_bk, sizeof(_AVATOR_DATA));
	strcpy(Sheet.szHBKFileName, m_szFileName);

	if(!g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, _DB_QRY_SYN_DATA::qry_case_lobby, (char*)&Sheet, Sheet.size()))
		return false;

	m_bDBWaitState = true;

	return true;
}

void CUserDB::Lobby_Char_Complete(BYTE byRetCode)
{
	m_bDBWaitState = false;

	m_bField = false;
	m_dwSerial = 0xFFFFFFFF;
	m_ss.re_lobby();

	m_AvatorData.InitData();
	m_AvatorData_bk.InitData();

	for(int i = 0; i < MAX_CHAR_PER_WORLDUSER; i++)
		m_RegedList[i].init();

	if(m_byUserDgr == USER_DEGREE_STD)
	{
		s_MoveLobbyDelay.Push(m_idWorld.wIndex, m_idWorld.dwSerial); 
	}
	else
	{	//일반 유저가 아니면 바로 결과줌
		_moveout_user_result_zone Send;	
		
		Send.byRetCode = byRetCode;

		BYTE byType[msg_header_num] = {system_msg, moveout_user_result_zone};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());
	}

	//다시 로비로 왔음을 계정서버에 알림
	_enter_lobby_report_wrac Report;

	memcpy(&Report.gidGlobal, &m_gidGlobal, sizeof(_GLBID));

	BYTE byTypeReport[msg_header_num] = {system_msg, enter_lobby_report_wrac};
	g_Main.m_Network.m_pProcess[account_line]->LoadSendMsg(0, byTypeReport, (char*)&Report, Report.size());					
}

void CUserDB::TotalPlayMinCheck()
{
	if(m_dwSerial == 0xFFFFFFFF)
		return;

	if(m_tmrCheckPlayMin.CountingTimer())	
	{
		DWORD dwTime = m_AvatorData.dbAvator.m_dwTotalPlayMin+1;

		Update_PlayTime(dwTime);
	}
}

void CUserDB::VariableUpdateCheck()
{
	if(m_dwSerial == 0xFFFFFFFF)
		return;

	if(m_tmrCheckVariableUpdate.CountingTimer())	
	{
		if(m_bVariableUpdate)
		{
			m_bFileUpdate = true;
			m_bVariableUpdate = false;
		}
	}
}

void CUserDB::WriteLog_Level(BYTE byLv)
{
	if(!m_bField)
		return;

	_log_sheet_lv Sheet;

	Sheet.dwAvatorSerial = m_dwSerial;
	Sheet.byLv = byLv;
	Sheet.dwTotalPlayMin = m_AvatorData.dbAvator.m_dwTotalPlayMin;

	if(!g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, _DB_QRY_SYN_DATA::log_case_lv, (char*)&Sheet, Sheet.size()))
		return;
}
/*
void CUserDB::WriteLog_Mastery(BYTE byMasteryCode, BYTE byCodeIndex, BYTE byMastery)
{
	if(!m_bField)
		return;

	_log_sheet_mastery Sheet;

	Sheet.dwAvatorSerial = m_dwSerial;
	Sheet.byMasteryCode = byMasteryCode;
	Sheet.byCodeIndex = byCodeIndex;
	Sheet.byMastery = byMastery;
	Sheet.dwTotalMin = m_AvatorData.dbAvator.m_dwTotalPlayMin;

	if(!g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, _DB_QRY_SYN_DATA::log_case_mastery, (char*)&Sheet, Sheet.size()))
		return;
}*/

bool CUserDB::SetBKFile(char* pszHBKFileName, _AVATOR_DATA* pData)
{
	HANDLE hFile = ::CreateFile(pszHBKFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		g_Main.m_logSystemError.Write("SetBKFile open Fail..(CreateFile)");
		return false;
	}

	DWORD dwWrite;
	::WriteFile(hFile, pData, sizeof(_AVATOR_DATA), &dwWrite, NULL);
	::CloseHandle(hFile);	

	if(dwWrite != sizeof(_AVATOR_DATA))
	{
		g_Main.m_logSystemError.Write("SetBKFile write Fail..(dwWrite != sizeof(_AVATOR_DATA))");
		::DeleteFile(pszHBKFileName);
		return false;
	}

	return true;
}

void CUserDB::ReleaseBKFile(char* pszHBKFileName)
{
	::DeleteFile(pszHBKFileName);
}

bool CUserDB::StartBKFileMapping()
{
	m_hBKFile = ::CreateFile(m_szFileName, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hBKFile == INVALID_HANDLE_VALUE)
	{
		g_Main.m_logSystemError.Write("%s.. BKMappingFile open Fail..(CreateFile)", m_szAvatorName);
		return false;
	}

	m_hBKMap = ::CreateFileMapping(m_hBKFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if(m_hBKMap == NULL)
	{
		g_Main.m_logSystemError.Write("%s.. BKMappingFile open Fail..(CreateFileMapping)", m_szAvatorName);
		return false;
	}

	m_pDataMapping = (_AVATOR_DATA*)::MapViewOfFile(m_hBKMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if(m_pDataMapping == NULL)
	{
		g_Main.m_logSystemError.Write("%s.. BKMappingFile open Fail..(MapViewOfFile)", m_szAvatorName);
		return false;
	}

	return true;
}

bool CUserDB::EndBKFileMapping()
{
	if(::UnmapViewOfFile(m_pDataMapping) == 0)
	{
		g_Main.m_logSystemError.Write("%s.. EndBKFileMapping open Fail..(UnmapViewOfFile)", m_szAvatorName);
		return false;	
	}

	if(::CloseHandle(m_hBKMap) == 0)
	{
		g_Main.m_logSystemError.Write("%s.. EndBKFileMapping open Fail..(CloseHandle(m_hBKMap))", m_szAvatorName);
		return false;
	}

	if(::CloseHandle(m_hBKFile) == 0)
	{
		g_Main.m_logSystemError.Write("%s.. EndBKFileMapping open Fail..(CloseHandle(m_hBKFile))", m_szAvatorName);
		return false;
	}

	return true;
}

bool CUserDB::UpdateBKFile(bool bDirect)
{
	if(m_dwSerial == 0xFFFFFFFF)
		return true;

	if(!bDirect)
	{
		if(!m_bFileUpdate)
			return true;
	}

	if(m_bDBWaitState)	//DB작업중일때는 업데이트 못하도록..
		return true;	//BK파일을 DB스레드에서 생성, 삭제하므로..

	if(!StartBKFileMapping())
	{
		ForceCloseCommand(false, 0);
		g_Main.m_logSystemError.Write("%s 강제종료: BK파일 Mapping오류", m_szAvatorName);
		return false;
	}

	if(m_pDataMapping)
		memcpy(m_pDataMapping, &m_AvatorData, sizeof(_AVATOR_DATA));

	if(!EndBKFileMapping())
		return false;

	m_bFileUpdate = false;

	return true;
}

bool CUserDB::Update_ItemAdd(BYTE storage, BYTE slot, BYTE table, WORD index, DWORD amount, DWORD upg)
{
	if(m_bNoneUpdateData)
		return true;

	bool bStorageIndexErr = false;

	if(storage == _STORAGE_POS::INVEN && slot >= bag_storage_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::EQUIP && slot >= equip_fix_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::EMBELLISH && slot >= embellish_fix_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::BELT && slot >= potion_belt_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::FORCE && slot >= force_storage_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::ANIMUS && slot >= animus_storage_num)
		bStorageIndexErr = true;

	if(bStorageIndexErr)
	{
		g_Main.m_logSystemError.Write("%s : Update_ItemAdd(코드 불량) : storage : %d, slot : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}
	
	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(pList->Key.IsFilled())//추가하려는곳에 로드되어잇는상태라면.. 살패..
		{
			g_Main.m_logSystemError.Write("%s:Update_Add(인벤, Idx:%d) 현재..(item:%d,%d) 추가..(item:%d,%d)", 
				m_szAvatorName, slot, pList->Key.byTableCode, pList->Key.wItemIndex, table, index);
			return false;
		}
		else
		{	
			pList->Key.byTableCode = table;
			pList->Key.wItemIndex = index;
			pList->dwDur = amount;
			pList->dwUpt = upg;
		}
	}
	else if(storage == _STORAGE_POS::EQUIP)
	{
		_EQUIPKEY* pKey = &m_AvatorData.dbAvator.m_EquipKey[slot];
		DWORD* pUpt = &m_AvatorData.dbAvator.m_dwFixEquipLv[slot];
		
		if(pKey->IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_Add(장비, Idx:%d) 현재..(item:%d) 추가..(item:%d)", 
				m_szAvatorName, slot, pKey->zItemIndex, index);
			return false;
		}
		else
		{
			pKey->zItemIndex = index;
			*pUpt = upg; 						
		}
	}
	else if(storage == _STORAGE_POS::EMBELLISH)
	{
		_EQUIP_DB_BASE::_EMBELLISH_LIST* pList = &m_AvatorData.dbEquip.m_EmbellishList[slot];

		if(pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_Add(장식, Idx:%d) 현재..(item:%d,%d) 추가..(item:%d,%d)", 
				m_szAvatorName, slot, pList->Key.byTableCode, pList->Key.wItemIndex, table, index);
			return false;
		}
		else
		{
			pList->Key.byTableCode = table;
			pList->Key.wItemIndex = index;
			pList->wAmount = (WORD)amount;
		}
	}
	else if(storage == _STORAGE_POS::BELT)
	{
		_BELT_DB_BASE::_POTION_LIST* pList = &m_AvatorData.dbBelt.m_PotionList[slot];

		if(pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_Add(벨트, Idx:%d) 현재..(item:%d) 추가..(item:%d)", 
				m_szAvatorName, slot, pList->Key.wItemIndex, index);
			return false;
		}
		else
		{
			pList->Key.wItemIndex = index;
			pList->Key.byAmount = (BYTE)amount;				
		}
	}
	else if(storage == _STORAGE_POS::FORCE)
	{
		_FORCE_DB_BASE::_LIST* pList = &m_AvatorData.dbForce.m_List[slot];

		if(pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_Add(포스, Idx:%d) 현재..(%d) 추가..(item:%d)", 
				m_szAvatorName, slot, pList->Key.GetIndex(), index);
			return false;
		}
		else
		{
			pList->Key.SetKey((BYTE)index, amount);
		}
	}
	else if(storage == _STORAGE_POS::ANIMUS)
	{
		_ANIMUS_DB_BASE::_LIST* pList = &m_AvatorData.dbAnimus.m_List[slot];

		if(pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_Add(소환, Idx:%d) 현재..(%d) 추가..(item:%d)", 
				m_szAvatorName, slot, pList->Key.byItemIndex, index);
			return false;
		}
		else
		{
			pList->Key.LoadDBKey((BYTE)index);
			pList->dwExp = amount;
			pList->dwParam = upg;
		}
	}
	else
		return false;

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_ItemDelete(BYTE storage, BYTE slot)
{
	if(m_bNoneUpdateData)
		return true;

	bool bStorageIndexErr = false;

	if(storage == _STORAGE_POS::INVEN && slot >= bag_storage_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::EQUIP && slot >= equip_fix_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::EMBELLISH && slot >= embellish_fix_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::BELT && slot >= potion_belt_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::FORCE && slot >= force_storage_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::ANIMUS && slot >= animus_storage_num)
		bStorageIndexErr = true;

	if(bStorageIndexErr)
	{
		g_Main.m_logSystemError.Write("%s : UpdateItemDelete(코드 불량) : scode : %d, icode : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}

	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(!pList->Key.IsFilled())//삭제하려는곳에 로드되어잇지않은상태라면.. 살패..
		{
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(인벤, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.SetRelease();
		}
	}
	else if(storage == _STORAGE_POS::EQUIP)
	{
		_EQUIPKEY* pKey = &m_AvatorData.dbAvator.m_EquipKey[slot];

		if(!pKey->IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(장비, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pKey->SetRelease();
		}
	}
	else if(storage == _STORAGE_POS::EMBELLISH)
	{
		_EQUIP_DB_BASE::_EMBELLISH_LIST* pList = &m_AvatorData.dbEquip.m_EmbellishList[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(장식, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.SetRelease();
		}
	}
	else if(storage == _STORAGE_POS::BELT)
	{
		_BELT_DB_BASE::_POTION_LIST* pList = &m_AvatorData.dbBelt.m_PotionList[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(벨트, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.SetRelease();
		}
	}
	else if(storage == _STORAGE_POS::FORCE)
	{
		_FORCE_DB_BASE::_LIST* pList = &m_AvatorData.dbForce.m_List[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(포스, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.SetRelease();
		}
	}
	else if(storage == _STORAGE_POS::ANIMUS)
	{
		_ANIMUS_DB_BASE::_LIST* pList = &m_AvatorData.dbAnimus.m_List[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(소환, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.SetRelease();
		}
	}
	else
		return false;

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_ItemDur(BYTE storage, BYTE slot, DWORD amount, bool bUpdate)
{
	if(m_bNoneUpdateData)
		return true;

	bool bStorageIndexErr = false;

	if(storage == _STORAGE_POS::INVEN && slot >= bag_storage_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::EMBELLISH && slot >= embellish_fix_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::BELT && slot >= potion_belt_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::FORCE && slot >= force_storage_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::ANIMUS && slot >= animus_storage_num)
		bStorageIndexErr = true;

	if(bStorageIndexErr)
	{
		g_Main.m_logSystemError.Write("%s : Update_ItemDur(코드 불량) : scode : %d, icode : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}

	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(인벤, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->dwDur = amount;
		}
	}
	else if(storage == _STORAGE_POS::EMBELLISH)
	{
		_EQUIP_DB_BASE::_EMBELLISH_LIST* pList = &m_AvatorData.dbEquip.m_EmbellishList[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(장식, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->wAmount = (WORD)amount;
		}
	}
	else if(storage == _STORAGE_POS::BELT)
	{
		_BELT_DB_BASE::_POTION_LIST* pList = &m_AvatorData.dbBelt.m_PotionList[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(벨트, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.byAmount = (BYTE)amount;
		}
	}
	else if(storage == _STORAGE_POS::FORCE)
	{
		_FORCE_DB_BASE::_LIST* pList = &m_AvatorData.dbForce.m_List[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(포스, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.SetStat(amount);
		}
	}
	else if(storage == _STORAGE_POS::ANIMUS)
	{
		_ANIMUS_DB_BASE::_LIST* pList = &m_AvatorData.dbAnimus.m_List[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(애니머스, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->dwExp = amount;
		}
	}

	if(bUpdate)
		m_bFileUpdate = true;
	else
		m_bVariableUpdate = true;

	return true;
}

bool CUserDB::Update_ItemUpgrade(BYTE storage, BYTE slot, DWORD upg, bool bUpdate)
{
	if(m_bNoneUpdateData)
		return true;

	bool bStorageIndexErr = false;

	if(storage == _STORAGE_POS::INVEN && slot >= bag_storage_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::EQUIP && slot >= equip_fix_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::ANIMUS && slot >= animus_storage_num)
		bStorageIndexErr = true;

	if(bStorageIndexErr)
	{
		g_Main.m_logSystemError.Write("%s : Update_Upgrade(코드 불량) : scode : %d, icode : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}

	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(!pList->Key.IsFilled())//삭제하려는곳에 로드되어잇지않은상태라면.. 살패..
		{
			g_Main.m_logSystemError.Write("%s:Update_Upgrade(인벤, Idx:%d)", m_szAvatorName, slot);
			return false; 
		}
		else
		{
			pList->dwUpt = upg;
		}
	}
	else if(storage == _STORAGE_POS::EQUIP)
	{
		_EQUIPKEY* pKey = &m_AvatorData.dbAvator.m_EquipKey[slot];

		if(!pKey->IsFilled())//삭제하려는곳에 로드되어잇지않은상태라면.. 살패..
		{
			g_Main.m_logSystemError.Write("%s:Update_Upgrade(장착, Idx:%d)", m_szAvatorName, slot);
			return false; 
		}
		else
		{
			m_AvatorData.dbAvator.m_dwFixEquipLv[slot] = upg;		
		}
	}
	else if(storage == _STORAGE_POS::ANIMUS)
	{
		_ANIMUS_DB_BASE::_LIST* pList = &m_AvatorData.dbAnimus.m_List[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_Upgrade(애니머스, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->dwParam = upg;
		}
	}

	else
	{
		g_Main.m_logSystemError.Write("%s:Update_Upgrade(틀린창(%d), Idx:%d)", m_szAvatorName, storage, slot);
		return false;
	}

	if(bUpdate)
		m_bFileUpdate = true;
	else
		m_bVariableUpdate = true;

	return true;
}

bool CUserDB::Update_UnitInsert(BYTE bySlotIndex, _UNIT_DB_BASE::_LIST* pSlotData)
{
	if(m_bNoneUpdateData)
		return true;

	if(bySlotIndex >= unit_storage_num)
	{
		g_Main.m_logSystemError.Write("%s : Update_UnitInsert(SlotIndex초과) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame != 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_UnitInsert(이미있음) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	memcpy(&m_AvatorData.dbUnit.m_List[bySlotIndex], pSlotData, sizeof(_UNIT_DB_BASE::_LIST));

	m_bFileUpdate = true;
	return true;
}

bool CUserDB::Update_UnitDelete(BYTE bySlotIndex)
{
	if(m_bNoneUpdateData)
		return true;

	if(bySlotIndex >= unit_storage_num)
	{
		g_Main.m_logSystemError.Write("%s : Update_UnitDelete(SlotIndex초과) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame == 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_UnitDelete(이미없음) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame = 0xFF;

	m_bFileUpdate = true;
	return true;
}

bool CUserDB::Update_UnitData(BYTE bySlotIndex, _UNIT_DB_BASE::_LIST* pData)
{
	if(m_bNoneUpdateData)
		return true;

	if(bySlotIndex >= unit_storage_num)
	{
		g_Main.m_logSystemError.Write("%s : Update_UnitData(SlotIndex초과) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame == 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_UnitData(이미없음) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	memcpy(&m_AvatorData.dbUnit.m_List[bySlotIndex], pData, sizeof(_UNIT_DB_BASE::_LIST));

	m_bFileUpdate = true;
	return true;
}

bool CUserDB::Update_QuestInsert(BYTE bySlotIndex, _QUEST_DB_BASE::_LIST* pSlotData)
{
	if(m_bNoneUpdateData)
		return true;

	if(bySlotIndex >= quest_storage_num)
	{
		g_Main.m_logSystemError.Write("%s : Update_QuestInsert(SlotIndex초과) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType != 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_QuestInsert(이미있음) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	memcpy(&m_AvatorData.dbQuest.m_List[bySlotIndex], pSlotData, sizeof(_QUEST_DB_BASE::_LIST));

	m_bFileUpdate = true;
	return true;
}

bool CUserDB::Update_QuestDelete(BYTE bySlotIndex)
{
	if(m_bNoneUpdateData)
		return true;

	if(bySlotIndex >= quest_storage_num)
	{
		g_Main.m_logSystemError.Write("%s : Update_QuestDelete(SlotIndex초과) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_QuestDelete(이미없음) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	m_AvatorData.dbQuest.m_List[bySlotIndex].Init();

	m_bFileUpdate = true;
	return true;
}

bool CUserDB::Update_QuestUpdate(BYTE bySlotIndex, _QUEST_DB_BASE::_LIST* pSlotData)
{
	if(m_bNoneUpdateData)
		return true;

	if(bySlotIndex >= quest_storage_num)
	{
		g_Main.m_logSystemError.Write("%s : Update_QuestUpdate(SlotIndex초과) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_QuestUpdate(없음) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	memcpy(&m_AvatorData.dbQuest.m_List[bySlotIndex], pSlotData, sizeof(_QUEST_DB_BASE::_LIST));

	m_bFileUpdate = true;
	return true;
}

bool CUserDB::Update_ItemSlot(BYTE storage, BYTE slot, BYTE clientpos)
{
	if(m_bNoneUpdateData)
		return true;

	bool bStorageIndexErr = false;

	if(storage == _STORAGE_POS::INVEN && slot >= bag_storage_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::EMBELLISH && slot >= embellish_fix_num)
		bStorageIndexErr = true;
	else if(storage == _STORAGE_POS::BELT && slot >= potion_belt_num)
		bStorageIndexErr = true;

	if(bStorageIndexErr)
	{
		g_Main.m_logSystemError.Write("%s : Update_ItemSlot(코드 불량) : scode : %d, icode : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}

	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(!pList->Key.IsFilled())//삭제하려는곳에 로드되어잇지않은상태라면.. 살패..
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemSlot(인벤, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.bySlotIndex = clientpos;
		}
	}
	else if(storage == _STORAGE_POS::EMBELLISH)
	{
		_EQUIP_DB_BASE::_EMBELLISH_LIST* pList = &m_AvatorData.dbEquip.m_EmbellishList[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemSlot(장식, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.bySlotIndex = clientpos;
		}
	}
	else if(storage == _STORAGE_POS::BELT)
	{
		_BELT_DB_BASE::_POTION_LIST* pList = &m_AvatorData.dbBelt.m_PotionList[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemSlot(벨트, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->Key.bySlotIndex = clientpos;
		}
	}
	else
		return false;

	return true;
}

bool CUserDB::Update_Level(BYTE lv, double exp)
{	
	if(m_bNoneUpdateData)
		return true;

	if(lv > max_level)
	{
		g_Main.m_logSystemError.Write("%s : Update_Level(): pRecv->byLv (%d) => failed ", m_szAvatorName, lv);
		return false;
	}

	if(lv <= m_AvatorData.dbAvator.m_byLevel)
	{
		if(m_byUserDgr == USER_DEGREE_STD)
		{
			g_Main.m_logSystemError.Write("%s : Update_Level(): 기존레벨(%d)보다 같거나작다(%d) => failed ", m_szAvatorName, m_AvatorData.dbAvator.m_byLevel, lv);
			return false;
		}
	}

	m_AvatorData.dbAvator.m_byLevel = lv;
	m_AvatorData.dbAvator.m_dExp = exp;

	if(m_byUserDgr == USER_DEGREE_STD)
		WriteLog_Level(lv);

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_Exp(double exp)
{
	if(m_bNoneUpdateData)
		return true;

	m_AvatorData.dbAvator.m_dExp = exp;

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_Class(char* pszClassCode, BYTE byHistoryRecordNum, WORD wHistoryClassIndex)
{
	if(m_bNoneUpdateData)
		return true;

	if(byHistoryRecordNum >= class_history_num)
	{
		g_Main.m_logSystemError.Write("%s : Update_Class(): byHistoryRecordNum (%d) => failed ", m_szAvatorName, byHistoryRecordNum);
		return false;
	}

	strcpy(m_AvatorData.dbAvator.m_szClassCode, pszClassCode);
	m_AvatorData.dbAvator.m_zClassHistory[byHistoryRecordNum] = wHistoryClassIndex;

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_Money(DWORD dalant, DWORD gold)
{
	if(m_bNoneUpdateData)
		return true;

	m_AvatorData.dbAvator.m_dwDalant = dalant;
	m_AvatorData.dbAvator.m_dwGold = gold;

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_Map(BYTE map, float* pos, bool bUpdate)
{
	if(m_bNoneUpdateData)
		return true;

	if(map >= g_Main.m_MapOper.m_nStdMapNum)
	{
//		g_Main.m_logSystemError.Write("%s : Update_Map(코드불량): byMapCode (%d) => failed ", m_szAvatorName, pCon->byMapCode);
		return false;
	}

	m_AvatorData.dbAvator.m_byMapCode = map;
	m_AvatorData.dbAvator.m_fStartPos[0] = pos[0];
	m_AvatorData.dbAvator.m_fStartPos[1] = pos[1];
	m_AvatorData.dbAvator.m_fStartPos[2] = pos[2];

	if(bUpdate)
		m_bFileUpdate = true;
	else
		m_bVariableUpdate = true;

	return true;
}

bool CUserDB::Update_BagNum(BYTE bagnum)
{
	if(m_bNoneUpdateData)
		return true;

	if(bagnum > max_bag_num)
	{
		g_Main.m_logSystemError.Write("%s : Update_BagNum(코드불량) byBagNum (%d) => failed ", m_szAvatorName, bagnum);
		return false;
	}

	if(m_AvatorData.dbAvator.m_byBagNum >= bagnum)
	{
		g_Main.m_logSystemError.Write("%s : Update_BagNum(코드불량) m_AvatorData.dbAvator.m_byBagNum(%d) > pCon->byBagNum(%d) => failed ", m_szAvatorName, m_AvatorData.dbAvator.m_byBagNum, bagnum);
		return false;
	}

	m_AvatorData.dbAvator.m_byBagNum = bagnum;

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_CuttingPush(BYTE resnum, _CUTTING_DB_BASE::_LIST* plist)
{
	if(m_bNoneUpdateData)
		return true;

	if(resnum > left_cutting_num)
	{
		g_Main.m_logSystemError.Write("%s : Update_CuttingPush(코드불량) byResNum (%d) => failed ", m_szAvatorName, resnum);
		return false;
	}

	for(int i = 0; i < resnum; i++)
	{
		if(plist[i].byResIndex >= ::GetMaxResKind())
		{
			g_Main.m_logSystemError.Write("%s : Update_CuttingPush(코드불량) List[%d].byResIndex (%d) => failed ", m_szAvatorName, i, plist[i].byResIndex);
			return false;
		}
	}
	
	m_AvatorData.dbCutting.Init();

	m_AvatorData.dbCutting.m_byLeftNum = resnum;
	memcpy(m_AvatorData.dbCutting.m_List, plist, sizeof(_CUTTING_DB_BASE::_LIST) * resnum);

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_CuttingTrans(BYTE byResItemIndex, BYTE byLeftAmt)
{
	if(m_bNoneUpdateData)
		return true;

	if(byResItemIndex >= ::GetMaxResKind())
	{
		g_Main.m_logSystemError.Write("%s : Update_CuttingTrans(코드불량) byResItemIndex (%d) => failed ", m_szAvatorName, byResItemIndex);
		return false;
	}

	bool bFind = false;
	for(int i= 0 ; i < left_cutting_num; i++)
	{
		if(m_AvatorData.dbCutting.m_List[i].byResIndex == byResItemIndex)
		{
			m_AvatorData.dbCutting.m_List[i].byAmt = byLeftAmt;

			if(byLeftAmt == 0)
			{
				m_AvatorData.dbCutting.m_List[i].byResIndex = 0xFF;
				m_AvatorData.dbCutting.m_byLeftNum--;
			}
			bFind = true;
			break;
		}		
	}
	if(!bFind)
	{
		g_Main.m_logSystemError.Write("%s:Update_TransRes(Idx:%d)", m_szAvatorName, byResItemIndex);
		return false;
	}

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_CuttingEmpty()
{
	if(m_bNoneUpdateData)
		return true;

	m_AvatorData.dbCutting.Init();

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_Stat(BYTE byStatIndex, DWORD dwNewCum, bool bUpdate)
{
	if(m_bNoneUpdateData)
		return true;

	if(byStatIndex >= sizeof(m_AvatorData.dbStat)/sizeof(DWORD))
	{
		g_Main.m_logSystemError.Write("%s:Update_Stat(Idx:%d)", m_szAvatorName, byStatIndex);
		return false;
	}
	
	if(((DWORD*)&m_AvatorData.dbStat)[byStatIndex] > dwNewCum)
	{
		if(m_byUserDgr == USER_DEGREE_STD)
		{
			g_Main.m_logSystemError.Write("%s:Update_Stat(Idx:%d)..%d >= %d 기존보다 적다", 
				m_szAvatorName, byStatIndex, ((DWORD*)&m_AvatorData.dbStat)[byStatIndex], dwNewCum);
			return false;
		}
	}

	((DWORD*)&m_AvatorData.dbStat)[byStatIndex] = dwNewCum;

	if(bUpdate)
		m_bFileUpdate = true;
	else
		m_bVariableUpdate = true;

	return true;
}

bool CUserDB::Update_PlayTime(DWORD dwTotalTimeMin)
{
	if(m_bNoneUpdateData)
		return true;

	m_AvatorData.dbAvator.m_dwTotalPlayMin = dwTotalTimeMin;

	m_bFileUpdate = true;

	return true;
}

//맵이동시에는 wait에 다음에 옴..
bool CUserDB::Update_Param(_EXIT_ALTER_PARAM* pCon)
{
	if(m_bNoneUpdateData)
		return true;

	if(pCon->byMapCode >= g_Main.m_MapOper.m_nStdMapNum)
	{
		g_Main.m_logSystemError.Write("%s : Update_Param(코드불량) byMapCode (%d) => failed ", m_szAvatorName, pCon->byMapCode);
//		return false;
	}

	//기본파라메터..
	m_AvatorData.dbAvator.m_dwHP = pCon->dwHP;
	m_AvatorData.dbAvator.m_dwFP = pCon->dwFP;
	m_AvatorData.dbAvator.m_dwSP = pCon->dwSP;
	m_AvatorData.dbAvator.m_dExp = pCon->dExp;
	m_AvatorData.dbAvator.m_byMapCode = pCon->byMapCode;
	m_AvatorData.dbAvator.m_fStartPos[0] = pCon->fStartPos[0];
	m_AvatorData.dbAvator.m_fStartPos[1] = pCon->fStartPos[1];
	m_AvatorData.dbAvator.m_fStartPos[2] = pCon->fStartPos[2];

	memcpy(m_AvatorData.dbBelt.m_LinkList, pCon->Link, sizeof(_BELT_DB_BASE::_LINK_LIST)*sf_linker_num);

	m_bFileUpdate = true;

	return true;
}

bool CUserDB::Update_CopyAll(_AVATOR_DATA* pSrc)
{
	if(m_bNoneUpdateData)
		return true;

/*	보존할 데이터들..
	char		m_szAvatorName[max_name_len+1];		//Base [Name]						
	DWORD		m_dwRecordNum;						//Base [Serial] :	아바타 시리얼			
	BYTE		m_byRaceSexCode;					//Base [Race]	:	종족 & 성별코드		
	BYTE		m_bySlotIndex;						//Base [Slot]	:	아바타 선택창 인덱스	
	DWORD		m_dwBaseShape;						//Base [BaseShape]	: 기본생김..(0x00654321	1:upper, 2:lower, 3:gauntlet, 4:shoe, 5:helmet, 6:face)
	DWORD		m_dwLastConnTime;					//Base [LastConnTime]	:	마지막 로그오프한 시간.. 
	BYTE		m_byMapCode;			//General [Map]	:	시작할 맵코드			
	float		m_fStartPos[3];			//General [X], [Y], [Z]	:	시작할 좌표			
	DWORD		m_dwTotalPlayMin;		//General	[TotalPlayMin]	:	총 플레이한 시간		
	DWORD		m_dwStartPlayTime;		//사용안함.. 플레이시작시간		
*/
	_AVATOR_DATA Buffer;

	memcpy(&Buffer, &m_AvatorData, sizeof(_AVATOR_DATA));
	memcpy(&m_AvatorData, pSrc, sizeof(_AVATOR_DATA));

	strcpy(m_AvatorData.dbAvator.m_szAvatorName, Buffer.dbAvator.m_szAvatorName);
	m_AvatorData.dbAvator.m_dwRecordNum = Buffer.dbAvator.m_dwRecordNum;
	m_AvatorData.dbAvator.m_byRaceSexCode = Buffer.dbAvator.m_byRaceSexCode;
	m_AvatorData.dbAvator.m_bySlotIndex = Buffer.dbAvator.m_bySlotIndex;
	m_AvatorData.dbAvator.m_dwBaseShape = Buffer.dbAvator.m_dwBaseShape;
	m_AvatorData.dbAvator.m_dwLastConnTime = Buffer.dbAvator.m_dwLastConnTime;
	m_AvatorData.dbAvator.m_byMapCode = Buffer.dbAvator.m_byMapCode;
	memcpy(m_AvatorData.dbAvator.m_fStartPos, Buffer.dbAvator.m_fStartPos, sizeof(float)*3);
	m_AvatorData.dbAvator.m_dwTotalPlayMin = Buffer.dbAvator.m_dwTotalPlayMin;
	m_AvatorData.dbAvator.m_dwStartPlayTime = Buffer.dbAvator.m_dwStartPlayTime;

	//추후 데이터를 업데이트하지 않는다..
	m_bNoneUpdateData = true;

	m_bFileUpdate = true;

	return false;
}

void CUserDB::ItemValidCheckRevise(_AVATOR_DATA* pData)
{
	//우선 인벤..
	for(int i = 0; i < pData->dbAvator.m_byBagNum * one_bag_store_num; i++)
	{
		_INVEN_DB_BASE::_LIST* p = &pData->dbInven.m_List[i];

		if(!p->Key.IsFilled())
			continue;

		//테이블 번호체크..
		if(p->Key.byTableCode >= item_tbl_num)
		{
			p->Key.SetRelease();
			continue;
		}

		//아이템 인덱스 체크..
		if(p->Key.wItemIndex >= g_Main.m_tblItemData[p->Key.byTableCode].GetRecordNum())
		{
			p->Key.SetRelease();
			continue;
		}

		//수량체크..
		if(::IsOverLapItem(p->Key.byTableCode))
		{
			if(p->dwDur == 0)
			{
				p->Key.SetRelease();
				continue;
			}
			if(p->dwDur > max_overlap_num)
			{
				p->dwDur = max_overlap_num;
			}
		}
	}

	//포션
	for(i = 0; i < potion_belt_num; i++)
	{
		_BELT_DB_BASE::_POTION_LIST* p = &pData->dbBelt.m_PotionList[i];

		if(!p->Key.IsFilled())
			continue;

		//아이템 인덱스 체크..
		if(p->Key.wItemIndex >= g_Main.m_tblItemData[tbl_code_potion].GetRecordNum())
		{
			p->Key.SetRelease();
			continue;
		}

		//수량체크..
		if(p->Key.byAmount == 0)
		{
			p->Key.SetRelease();
			continue;
		}
		if(p->Key.byAmount > max_overlap_num)
		{
			p->Key.byAmount = max_overlap_num;
		}
	}
}

void CUserDB::ReRangeClientIndex(_AVATOR_DATA* pData)
{
	//클라이언트 슬롯 인덱스 정렬..
	CNetIndexList listReArrange;
	bool bPut[100];
	int nMaxArrange;
	DWORD dwList[100];
	int nListNum; 
	int nLastPut = 0;
	int e, n;

	listReArrange.SetList(100);

	//중복으로 위치지정됀거 찾기..
	//위치를 찾은것은 해당슬롯의 bPut[]위치에 true로 바꾸고 다음에 같은 슬롯의
	//아이템이 발견되면 listReArrange에 추가해서 bPut[]가 false인것을찾아서 그슬롯으로 
	//넣는다..

	//인벤
	nMaxArrange = pData->dbAvator.m_byBagNum*one_bag_store_num;
	listReArrange.ResetList();
	memset(bPut, false, sizeof(bPut));
	nLastPut = 0;

	for(e = 0; e < nMaxArrange; e++)
	{		
		if(!pData->dbInven.m_List[e].Key.IsFilled())
			continue;

		if(pData->dbInven.m_List[e].Key.bySlotIndex < nMaxArrange) 
		{
			if(!bPut[pData->dbInven.m_List[e].Key.bySlotIndex])
			{
				bPut[pData->dbInven.m_List[e].Key.bySlotIndex] = true;
				continue;
			}
		}
		listReArrange.PushNode_Back(e);
	}

	nLastPut = 0;
	nListNum = listReArrange.CopyIndexList(dwList, nMaxArrange);
	for(n = 0; n < nListNum; n++)
	{
		//bPut이 false인곳 찾기..
		for(int p = nLastPut; p < nMaxArrange; p++)
		{
			if(!bPut[p])
			{
				pData->dbInven.m_List[dwList[n]].Key.bySlotIndex = (BYTE)p;

				bPut[p] = true;
				nLastPut = p;
				break;
			}	
		}
	}
	listReArrange.ResetList();
	
	//벨트
	nMaxArrange = potion_belt_num;
	memset(bPut, false, sizeof(bPut));
	nLastPut = 0;

	for(e = 0; e < nMaxArrange; e++)
	{		
		if(!pData->dbBelt.m_PotionList[e].Key.IsFilled())
			continue;

		if(pData->dbBelt.m_PotionList[e].Key.bySlotIndex < nMaxArrange) 
		{
			if(!bPut[pData->dbBelt.m_PotionList[e].Key.bySlotIndex])
			{
				bPut[pData->dbBelt.m_PotionList[e].Key.bySlotIndex] = true;
				continue;
			}
		}
		listReArrange.PushNode_Back(e);
	}

	nLastPut = 0;
	nListNum = listReArrange.CopyIndexList(dwList, nMaxArrange);
	for(n = 0; n < nListNum; n++)
	{
		//bPut이 false인곳 찾기..
		for(int p = nLastPut; p < nMaxArrange; p++)
		{
			if(!bPut[p])
			{
				pData->dbBelt.m_PotionList[dwList[n]].Key.bySlotIndex = (BYTE)p;

				bPut[p] = true;
				nLastPut = p;
				break;
			}	
		}
	}
	listReArrange.ResetList();

	//장식
	nMaxArrange = embellish_fix_num;
	memset(bPut, false, sizeof(bPut));
	nLastPut = 0;

	for(e = 0; e < nMaxArrange; e++)
	{		
		if(!pData->dbEquip.m_EmbellishList[e].Key.IsFilled())
			continue;

		if(pData->dbEquip.m_EmbellishList[e].Key.bySlotIndex < nMaxArrange) 
		{
			if(!bPut[pData->dbEquip.m_EmbellishList[e].Key.bySlotIndex])
			{
				bPut[pData->dbEquip.m_EmbellishList[e].Key.bySlotIndex] = true;
				continue;
			}
		}
		listReArrange.PushNode_Back(e);
	}

	nLastPut = 0;
	nListNum = listReArrange.CopyIndexList(dwList, nMaxArrange);
	for(n = 0; n < nListNum; n++)
	{
		//bPut이 false인곳 찾기..
		for(int p = nLastPut; p < nMaxArrange; p++)
		{
			if(!bPut[p])
			{
				pData->dbEquip.m_EmbellishList[dwList[n]].Key.bySlotIndex = (BYTE)p;

				bPut[p] = true;
				nLastPut = p;
				break;
			}	
		}
	}
}

bool CUserDB::FirstSettingData()
{
	if(!m_bActive)
		return false;

	//케릭터클래스의 포인터..
	_class_fld* pClassFld = (_class_fld*)g_Main.m_tblClass.GetRecord(m_AvatorData.dbAvator.m_szClassCode);
	if(!pClassFld)
		return false;

	int* pnCum = NULL;
	_str_code* pCode = NULL;

	//보너스 마스터리..
	//무기숙련도
	pnCum = pClassFld->m_nBnsMMastery;
	for(int i = 0; i < WP_CLASS_NUM; i++)	
	{
		if(pnCum[i] == 0)
			continue;

		Update_Stat(_STAT_DB_BASE::DATAPOS_WPCNT+i, pnCum[i]);
	}

	//종족별 특수 숙련도
	if(pClassFld->m_nBnsSMastery != 0)
	{		
		Update_Stat(_STAT_DB_BASE::DATAPOS_SPECIAL, pClassFld->m_nBnsSMastery);
	}

	//방어숙련도
	if(pClassFld->m_nBnsDefMastery != 0)
	{
		Update_Stat(_STAT_DB_BASE::DATAPOS_DEF, pClassFld->m_nBnsDefMastery);
	}

	//방패 숙련도
	if(pClassFld->m_nBnsPryMastery != 0)
	{
		Update_Stat(_STAT_DB_BASE::DATAPOS_SHLD, pClassFld->m_nBnsPryMastery);
	}

	//제작 숙련도
	pnCum = pClassFld->m_nBnsMakeMastery;
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		if(pnCum[i] == 0)
			continue;

		Update_Stat(_STAT_DB_BASE::DATAPOS_MAKE+i, pnCum[i]);
	}

	//포스 숙련도
	pnCum = pClassFld->m_nBnsForceMastery;
	for(i = 0; i < force_mastery_num; i++)
	{
		if(pnCum[i] == 0)
			continue;

		Update_Stat(_STAT_DB_BASE::DATAPOS_FORCE+i, pnCum[i]);
	}

	//아이템 보너스..
	//디폴드 인벤 아아이템
	pCode = pClassFld->m_strDefaultItem;
	for(i = 0; i < max_class_bns_item; i++)
	{
		if(!strcmp(pCode[i], "0"))
			continue;

		int nTableCode = ::GetItemTableCode(pCode[i]);
		if(nTableCode == -1)
		{
			g_Main.m_logSystemError.Write("초기케릭터 디폴트 아이템 에러.., class: %s, %d번째 아이템(%s).. 테이블코드에러", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		if(nTableCode == tbl_code_key)//유닛키는 줄수없다..
		{
			g_Main.m_logSystemError.Write("초기케릭터 디폴트 아이템 에러.., class: %s, %d번째 아이템(%s).. 유닛키는줄수없다", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		_base_fld* pFld = g_Main.m_tblItemData[nTableCode].GetRecord(pCode[i]);
		if(!pFld)
		{
			g_Main.m_logSystemError.Write("초기케릭터 디폴트 아이템 에러.., class: %s, %d번째 아이템(%s).. 테이블에 없음", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		DWORD dwAmount = ::GetItemDurPoint(nTableCode, pFld->m_dwIndex);
		DWORD dwUpd = 0;	
		//아이템 타입에 따라 업그레이드 맴버 분기..
		BYTE byItemKindCode = ::GetItemKindCode(nTableCode);
		if(byItemKindCode == item_kind_std)//일반..
			dwUpd = __DF_UPT;
		else if(byItemKindCode == item_kind_ani)//애니머스..
			dwUpd = ::GetMaxParamFromExp(pFld->m_dwIndex, dwAmount);
		else
			continue;//다른건 줄수없다..

		Update_ItemAdd(_STORAGE_POS::INVEN, i, nTableCode, pFld->m_dwIndex, dwAmount, dwUpd);
	}

	//디폴드 포션 아이템
	pCode = pClassFld->m_strPtSocket;
	for(i = 0; i < potion_belt_num; i++)
	{
		if(!strcmp(pCode[i], "0"))
			continue;

		BYTE nTableCode = ::GetItemTableCode(pCode[i]);
		if(nTableCode == -1)
		{
			g_Main.m_logSystemError.Write("초기케릭터 디폴트포션 아이템 에러.., class: %s, %d번째 아이템(%s).. 테이블코드에러", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		if(nTableCode != tbl_code_potion)
		{
			g_Main.m_logSystemError.Write("초기케릭터 디폴트포션 아이템 에러.., class: %s, %d번째 아이템(%s).. 포션이아니다", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		_base_fld* pFld = g_Main.m_tblItemData[nTableCode].GetRecord(pCode[i]);
		if(!pFld)
		{
			g_Main.m_logSystemError.Write("초기케릭터 디폴트포션 아이템 에러.., class: %s, %d번째 아이템(%s).. 테이블에 없음", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		Update_ItemAdd(_STORAGE_POS::BELT, i, nTableCode, pFld->m_dwIndex, 10, __DF_UPT);
	}

	//스킬단축소켓
	pCode = pClassFld->m_strSFSocket;
	for(i = 0; i < sf_linker_num; i++)
	{
		if(!strcmp(pCode[i], "0"))
			continue;

		_base_fld* pFld = g_Main.m_tblEffectData[effect_code_skill].GetRecord(pCode[i]);
		if(!pFld)
		{
			g_Main.m_logSystemError.Write("초기케릭터 디폴트링크 에러.., class: %s, %d번째 링크(%s).. 테이블에 없음", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		m_AvatorData.dbBelt.m_LinkList[i].Key.SetData(link_code_skill, pFld->m_dwIndex);
	}

	//마지막으로 레벨을 증가시킴..
	Update_Level(1, 0);

	m_AvatorData.dbAvator.m_fStartPos[1] = DEFAULT_Y_POS;	//Y좌표를 이렇게 넣어줘야 서버에 시작할때 마을로 잡아준다.
	
	return true;
}

void _MOVE_LOBBY_DELAY::Process(DWORD dwIndex, DWORD dwSerial)
{
	CUserDB* p = &g_UserDB[dwIndex];
	if(p->m_idWorld.dwSerial == dwSerial)
	{
		_moveout_user_result_zone Send;	
		
		Send.byRetCode = 0;

		BYTE byType[msg_header_num] = {system_msg, moveout_user_result_zone};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(dwIndex, byType, (char*)&Send, Send.size());
	}
}

void _MESSENGER_KEY_DELAY::Process(DWORD dwIndex, DWORD dwSerial)
{
	CUserDB* p = &g_UserDB[dwIndex];
	if(p->m_idWorld.dwSerial == dwSerial)
	{
		_messenger_enter_key_inform_zocl Send;	
		
		Send.dwAvatorSerial = p->m_dwSerial;
		memcpy(Send.dwKey, p->m_dwMessengerKey, sizeof(DWORD)*CHECK_KEY_NUM);
		Send.dwMessengerIP = g_Main.m_dwMessengerIP;

		BYTE byType[msg_header_num] = {system_msg, messenger_enter_key_inform_zocl};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(dwIndex, byType, (char*)&Send, Send.size());

		//대기시간체크 리스트에 삽입..
		CUserDB::s_MessengerEnterCheck.Push(p->m_idWorld.wIndex, p->m_idWorld.dwSerial); 
	}
}

void _MESSENGER_ENTER_CHECK::Process(DWORD dwIndex, DWORD dwSerial)
{
	CUserDB* p = &g_UserDB[dwIndex];
	if(p->m_idWorld.dwSerial == dwSerial)
	{
		if(p->m_idMessenger.dwSerial == 0xFFFFFFFF)
		{	//정해진시간안에 안들어옴..
			//즉시 끊어버린다.
			g_Main.m_Network.Close(client_line, p->m_idWorld.wIndex, false);
		}
	}
}

CUserDB* SearchAvatorWithName(CUserDB* pList, int nMax, char* pName)
{
	for(int i = 0; i < nMax; i++)
	{
		if(!pList[i].m_bActive)
			continue;

		if(!pList[i].m_bField)
			continue;

		if(!strcmp(pList[i].m_szAvatorName, pName))
		{
			return &pList[i];
		}
	}

	return NULL;
}