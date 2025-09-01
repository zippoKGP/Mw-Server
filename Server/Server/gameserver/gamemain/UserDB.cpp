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
{//������ ÷ ���������..
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
	//���������� �˸���.
	//
	_server_notify_inform_zone Send;

	Send.wMsgCode = NOTI_CODE_FORCE_CLOSE;
	Send.wMsgCode/*bPushClose,ydq*/ = bPushClose;
	Send.dwPushIP = dwPushIP;

	BYTE byType[msg_header_num] = {system_msg, server_notify_inform_zone};
	g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());

	//������ ���´�.
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
	{//�׻��̿� ����..
		_close_user_command_wrmg Send;

		memcpy(&Send.idLocal, pidLocal, sizeof(_CLID));

		g_Main.m_Network.m_pProcess[messenger_line]->LoadSendMsg(0, Send.head(), (char*)&Send, Send.size());		
	}
	else
	{
		if(m_idMessenger.dwSerial == 0xFFFFFFFF)
		{
			//���þ��̵���
			memcpy(&m_idMessenger, pidLocal, sizeof(_CLID));

			//�ɸ��� ���ð�� �˸���..
			_sel_char_result_zone Send;

			Send.byRetCode = RET_CODE_SUCCESS;
			Send.bySlotIndex = m_AvatorData.dbAvator.m_bySlotIndex;
			Send.dwWorldSerial = m_AvatorData.dbAvator.m_dwRecordNum;//�ɸ���db�ø����..

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
	{	//���� ��������..
		//��������
		ForceCloseCommand(false, 0xFFFFFFFF);
	}

	//�޽������������� ������ ǥ��(m_idMessenger�� �ø������ʱ�ȭ)
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

	//���� ���� ������ ���� ����� ����Ʈ�� �ִ��� Ȯ��..
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
		ParamInit();	//connect���ڸ��� ����°͵��� ����..
		return;
	}

	//�޽����������� �˸���..
	if(m_idMessenger.dwSerial != 0xFFFFFFFF)	//�ʱ�ȭ�»����ΰ��� ���� ���ʿ� ������ ���ϰų�, 
	{											//���ʿ��� ������ ó���Ѽ� ������..
		_close_user_command_wrmg Send;

		memcpy(&Send.idLocal, &m_idMessenger, sizeof(_CLID));

		g_Main.m_Network.m_pProcess[messenger_line]->LoadSendMsg(0, Send.head(), (char*)&Send, Send.size());
	}

	if(m_dwSerial != 0xFFFFFFFF)	//������ ĳ���Ͱ� ������ ������Ʈ..
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

	_logout_account_request_wrac Query;		//������������ �����α׾ƿ��� �˸�

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

		//�ɸ���â�� ������ ���� �������� ���׷��̵巹������ ���
		for(int i = 0; i < MAX_CHAR_PER_WORLDUSER; i++)
		{
			_REGED* p = &m_RegedList[i];

			if(p->m_bySlotIndex == 0xFF)
				continue;
		
			p->UpdateEquipLv();//�����������
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
				if(Send.RegedList[nCnt].m_byLevel == 0)//������ �������ϰ� �����Ѱ��..
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
{//false�� return�ϸ� �ڵ� disconnect..

	BYTE byRet = RET_CODE_SUCCESS;

	if(!m_bActive)
		return false;

	if(!(m_ss.bReged && !m_ss.bSelect))
		return false;

	if(m_bDBWaitState)
		return false;

	if(m_RegedList[bySlotIndex].m_bySlotIndex != 0xFF)
		return false;

	//ù���ڰ� '*'�̿����� �ȉ�.. (�ɸ������ﶧ �ӽ÷� �տ� *ǥ�ø� �ϹǷ�..ȥ�����)
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
	memcpy(&Sheet.LoadData.dbAvator, &m_RegedList[bySlotIndex], sizeof(_REGED));//base ��Ʈ ���� ī��..

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

		//������ ��ȿ�� üũ �� ����
		ItemValidCheckRevise(&m_AvatorData);

		strcpy(m_szAvatorName, pLoadData->dbAvator.m_szAvatorName);
		
		m_tmrCheckPlayMin.TermTimeRun();//�簡��..
		m_tmrCheckVariableUpdate.TermTimeRun();//�簡��..
		strcpy(m_szFileName, pszHBKFileName);

		m_dwSerial = pLoadData->dbAvator.m_dwRecordNum;
		bySlotIndex = pLoadData->dbAvator.m_bySlotIndex;

		//ù�����ϴ� �ɸ���..
		if(m_AvatorData.dbAvator.m_byLevel == 0)
		{
			if(!FirstSettingData())
				g_Main.m_logSystemError.Write("FirstSettingData() == false : char: %s, class: %s", m_szAvatorName, m_AvatorData.dbAvator.m_szClassCode);
		}

		//�÷��̾� ȣ��..
		if(!g_Player[m_idWorld.wIndex].Load(this))
		{
			g_Main.m_Network.Close(client_line, m_idWorld.wIndex, false);
			return;
		}
	}

	//send..

	//���� �޽��������� �̿����� �ʰų� ���н� �ٷ� ���ó��
	if(!g_Main.m_bUsingMsgr)
	{
		_sel_char_result_zone Send;

		Send.byRetCode = byRetCode;
		Send.bySlotIndex = bySlotIndex;
		Send.dwWorldSerial = m_AvatorData.dbAvator.m_dwRecordNum;//�ɸ���db�ø����..

		BYTE byType[msg_header_num] = {system_msg, sel_char_result_zone};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());					
	}
	else
	{
		if(byRetCode ==  RET_CODE_SUCCESS)
		{
			GenerateMessengerKey();
			//�޽����� trans..
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

			//������ 1���Ŀ� key �˸�..
			s_MessengerKeyDelay.Push(m_idWorld.wIndex, m_idWorld.dwSerial); 

			//���������� �ɸ��� ������ �˸�..
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
	{	//�Ϲ� ������ �ƴϸ� �ٷ� �����
		_moveout_user_result_zone Send;	
		
		Send.byRetCode = byRetCode;

		BYTE byType[msg_header_num] = {system_msg, moveout_user_result_zone};
		g_Main.m_Network.m_pProcess[client_line]->LoadSendMsg(m_idWorld.wIndex, byType, (char*)&Send, Send.size());
	}

	//�ٽ� �κ�� ������ ���������� �˸�
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

	if(m_bDBWaitState)	//DB�۾����϶��� ������Ʈ ���ϵ���..
		return true;	//BK������ DB�����忡�� ����, �����ϹǷ�..

	if(!StartBKFileMapping())
	{
		ForceCloseCommand(false, 0);
		g_Main.m_logSystemError.Write("%s ��������: BK���� Mapping����", m_szAvatorName);
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
		g_Main.m_logSystemError.Write("%s : Update_ItemAdd(�ڵ� �ҷ�) : storage : %d, slot : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}
	
	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(pList->Key.IsFilled())//�߰��Ϸ��°��� �ε�Ǿ��մ»��¶��.. ����..
		{
			g_Main.m_logSystemError.Write("%s:Update_Add(�κ�, Idx:%d) ����..(item:%d,%d) �߰�..(item:%d,%d)", 
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
			g_Main.m_logSystemError.Write("%s:Update_Add(���, Idx:%d) ����..(item:%d) �߰�..(item:%d)", 
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
			g_Main.m_logSystemError.Write("%s:Update_Add(���, Idx:%d) ����..(item:%d,%d) �߰�..(item:%d,%d)", 
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
			g_Main.m_logSystemError.Write("%s:Update_Add(��Ʈ, Idx:%d) ����..(item:%d) �߰�..(item:%d)", 
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
			g_Main.m_logSystemError.Write("%s:Update_Add(����, Idx:%d) ����..(%d) �߰�..(item:%d)", 
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
			g_Main.m_logSystemError.Write("%s:Update_Add(��ȯ, Idx:%d) ����..(%d) �߰�..(item:%d)", 
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
		g_Main.m_logSystemError.Write("%s : UpdateItemDelete(�ڵ� �ҷ�) : scode : %d, icode : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}

	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(!pList->Key.IsFilled())//�����Ϸ��°��� �ε�Ǿ������������¶��.. ����..
		{
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(�κ�, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(���, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(���, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(��Ʈ, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(����, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:UpdateItemDelete(��ȯ, Idx:%d)", m_szAvatorName, slot);
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
		g_Main.m_logSystemError.Write("%s : Update_ItemDur(�ڵ� �ҷ�) : scode : %d, icode : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}

	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(!pList->Key.IsFilled())
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(�κ�, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(���, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(��Ʈ, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(����, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:Update_ItemDur(�ִϸӽ�, Idx:%d)", m_szAvatorName, slot);
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
		g_Main.m_logSystemError.Write("%s : Update_Upgrade(�ڵ� �ҷ�) : scode : %d, icode : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}

	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(!pList->Key.IsFilled())//�����Ϸ��°��� �ε�Ǿ������������¶��.. ����..
		{
			g_Main.m_logSystemError.Write("%s:Update_Upgrade(�κ�, Idx:%d)", m_szAvatorName, slot);
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

		if(!pKey->IsFilled())//�����Ϸ��°��� �ε�Ǿ������������¶��.. ����..
		{
			g_Main.m_logSystemError.Write("%s:Update_Upgrade(����, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:Update_Upgrade(�ִϸӽ�, Idx:%d)", m_szAvatorName, slot);
			return false;
		}
		else
		{
			pList->dwParam = upg;
		}
	}

	else
	{
		g_Main.m_logSystemError.Write("%s:Update_Upgrade(Ʋ��â(%d), Idx:%d)", m_szAvatorName, storage, slot);
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
		g_Main.m_logSystemError.Write("%s : Update_UnitInsert(SlotIndex�ʰ�) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame != 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_UnitInsert(�̹�����) : slot : %d", m_szAvatorName,  bySlotIndex);
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
		g_Main.m_logSystemError.Write("%s : Update_UnitDelete(SlotIndex�ʰ�) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame == 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_UnitDelete(�̹̾���) : slot : %d", m_szAvatorName,  bySlotIndex);
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
		g_Main.m_logSystemError.Write("%s : Update_UnitData(SlotIndex�ʰ�) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame == 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_UnitData(�̹̾���) : slot : %d", m_szAvatorName,  bySlotIndex);
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
		g_Main.m_logSystemError.Write("%s : Update_QuestInsert(SlotIndex�ʰ�) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType != 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_QuestInsert(�̹�����) : slot : %d", m_szAvatorName,  bySlotIndex);
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
		g_Main.m_logSystemError.Write("%s : Update_QuestDelete(SlotIndex�ʰ�) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_QuestDelete(�̹̾���) : slot : %d", m_szAvatorName,  bySlotIndex);
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
		g_Main.m_logSystemError.Write("%s : Update_QuestUpdate(SlotIndex�ʰ�) : slot : %d", m_szAvatorName,  bySlotIndex);
		return false;
	}

	if(m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
	{
		g_Main.m_logSystemError.Write("%s : Update_QuestUpdate(����) : slot : %d", m_szAvatorName,  bySlotIndex);
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
		g_Main.m_logSystemError.Write("%s : Update_ItemSlot(�ڵ� �ҷ�) : scode : %d, icode : %d  ", m_szAvatorName,  storage, slot);
		return false;
	}

	if(storage == _STORAGE_POS::INVEN)
	{
		_INVEN_DB_BASE::_LIST* pList = &m_AvatorData.dbInven.m_List[slot];

		if(!pList->Key.IsFilled())//�����Ϸ��°��� �ε�Ǿ������������¶��.. ����..
		{
			g_Main.m_logSystemError.Write("%s:Update_ItemSlot(�κ�, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:Update_ItemSlot(���, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s:Update_ItemSlot(��Ʈ, Idx:%d)", m_szAvatorName, slot);
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
			g_Main.m_logSystemError.Write("%s : Update_Level(): ��������(%d)���� ���ų��۴�(%d) => failed ", m_szAvatorName, m_AvatorData.dbAvator.m_byLevel, lv);
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
//		g_Main.m_logSystemError.Write("%s : Update_Map(�ڵ�ҷ�): byMapCode (%d) => failed ", m_szAvatorName, pCon->byMapCode);
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
		g_Main.m_logSystemError.Write("%s : Update_BagNum(�ڵ�ҷ�) byBagNum (%d) => failed ", m_szAvatorName, bagnum);
		return false;
	}

	if(m_AvatorData.dbAvator.m_byBagNum >= bagnum)
	{
		g_Main.m_logSystemError.Write("%s : Update_BagNum(�ڵ�ҷ�) m_AvatorData.dbAvator.m_byBagNum(%d) > pCon->byBagNum(%d) => failed ", m_szAvatorName, m_AvatorData.dbAvator.m_byBagNum, bagnum);
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
		g_Main.m_logSystemError.Write("%s : Update_CuttingPush(�ڵ�ҷ�) byResNum (%d) => failed ", m_szAvatorName, resnum);
		return false;
	}

	for(int i = 0; i < resnum; i++)
	{
		if(plist[i].byResIndex >= ::GetMaxResKind())
		{
			g_Main.m_logSystemError.Write("%s : Update_CuttingPush(�ڵ�ҷ�) List[%d].byResIndex (%d) => failed ", m_szAvatorName, i, plist[i].byResIndex);
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
		g_Main.m_logSystemError.Write("%s : Update_CuttingTrans(�ڵ�ҷ�) byResItemIndex (%d) => failed ", m_szAvatorName, byResItemIndex);
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
			g_Main.m_logSystemError.Write("%s:Update_Stat(Idx:%d)..%d >= %d �������� ����", 
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

//���̵��ÿ��� wait�� ������ ��..
bool CUserDB::Update_Param(_EXIT_ALTER_PARAM* pCon)
{
	if(m_bNoneUpdateData)
		return true;

	if(pCon->byMapCode >= g_Main.m_MapOper.m_nStdMapNum)
	{
		g_Main.m_logSystemError.Write("%s : Update_Param(�ڵ�ҷ�) byMapCode (%d) => failed ", m_szAvatorName, pCon->byMapCode);
//		return false;
	}

	//�⺻�Ķ����..
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

/*	������ �����͵�..
	char		m_szAvatorName[max_name_len+1];		//Base [Name]						
	DWORD		m_dwRecordNum;						//Base [Serial] :	�ƹ�Ÿ �ø���			
	BYTE		m_byRaceSexCode;					//Base [Race]	:	���� & �����ڵ�		
	BYTE		m_bySlotIndex;						//Base [Slot]	:	�ƹ�Ÿ ����â �ε���	
	DWORD		m_dwBaseShape;						//Base [BaseShape]	: �⺻����..(0x00654321	1:upper, 2:lower, 3:gauntlet, 4:shoe, 5:helmet, 6:face)
	DWORD		m_dwLastConnTime;					//Base [LastConnTime]	:	������ �α׿����� �ð�.. 
	BYTE		m_byMapCode;			//General [Map]	:	������ ���ڵ�			
	float		m_fStartPos[3];			//General [X], [Y], [Z]	:	������ ��ǥ			
	DWORD		m_dwTotalPlayMin;		//General	[TotalPlayMin]	:	�� �÷����� �ð�		
	DWORD		m_dwStartPlayTime;		//������.. �÷��̽��۽ð�		
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

	//���� �����͸� ������Ʈ���� �ʴ´�..
	m_bNoneUpdateData = true;

	m_bFileUpdate = true;

	return false;
}

void CUserDB::ItemValidCheckRevise(_AVATOR_DATA* pData)
{
	//�켱 �κ�..
	for(int i = 0; i < pData->dbAvator.m_byBagNum * one_bag_store_num; i++)
	{
		_INVEN_DB_BASE::_LIST* p = &pData->dbInven.m_List[i];

		if(!p->Key.IsFilled())
			continue;

		//���̺� ��ȣüũ..
		if(p->Key.byTableCode >= item_tbl_num)
		{
			p->Key.SetRelease();
			continue;
		}

		//������ �ε��� üũ..
		if(p->Key.wItemIndex >= g_Main.m_tblItemData[p->Key.byTableCode].GetRecordNum())
		{
			p->Key.SetRelease();
			continue;
		}

		//����üũ..
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

	//����
	for(i = 0; i < potion_belt_num; i++)
	{
		_BELT_DB_BASE::_POTION_LIST* p = &pData->dbBelt.m_PotionList[i];

		if(!p->Key.IsFilled())
			continue;

		//������ �ε��� üũ..
		if(p->Key.wItemIndex >= g_Main.m_tblItemData[tbl_code_potion].GetRecordNum())
		{
			p->Key.SetRelease();
			continue;
		}

		//����üũ..
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
	//Ŭ���̾�Ʈ ���� �ε��� ����..
	CNetIndexList listReArrange;
	bool bPut[100];
	int nMaxArrange;
	DWORD dwList[100];
	int nListNum; 
	int nLastPut = 0;
	int e, n;

	listReArrange.SetList(100);

	//�ߺ����� ��ġ�����°� ã��..
	//��ġ�� ã������ �ش罽���� bPut[]��ġ�� true�� �ٲٰ� ������ ���� ������
	//�������� �߰ߵǸ� listReArrange�� �߰��ؼ� bPut[]�� false�ΰ���ã�Ƽ� �׽������� 
	//�ִ´�..

	//�κ�
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
		//bPut�� false�ΰ� ã��..
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
	
	//��Ʈ
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
		//bPut�� false�ΰ� ã��..
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

	//���
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
		//bPut�� false�ΰ� ã��..
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

	//�ɸ���Ŭ������ ������..
	_class_fld* pClassFld = (_class_fld*)g_Main.m_tblClass.GetRecord(m_AvatorData.dbAvator.m_szClassCode);
	if(!pClassFld)
		return false;

	int* pnCum = NULL;
	_str_code* pCode = NULL;

	//���ʽ� �����͸�..
	//������õ�
	pnCum = pClassFld->m_nBnsMMastery;
	for(int i = 0; i < WP_CLASS_NUM; i++)	
	{
		if(pnCum[i] == 0)
			continue;

		Update_Stat(_STAT_DB_BASE::DATAPOS_WPCNT+i, pnCum[i]);
	}

	//������ Ư�� ���õ�
	if(pClassFld->m_nBnsSMastery != 0)
	{		
		Update_Stat(_STAT_DB_BASE::DATAPOS_SPECIAL, pClassFld->m_nBnsSMastery);
	}

	//�����õ�
	if(pClassFld->m_nBnsDefMastery != 0)
	{
		Update_Stat(_STAT_DB_BASE::DATAPOS_DEF, pClassFld->m_nBnsDefMastery);
	}

	//���� ���õ�
	if(pClassFld->m_nBnsPryMastery != 0)
	{
		Update_Stat(_STAT_DB_BASE::DATAPOS_SHLD, pClassFld->m_nBnsPryMastery);
	}

	//���� ���õ�
	pnCum = pClassFld->m_nBnsMakeMastery;
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		if(pnCum[i] == 0)
			continue;

		Update_Stat(_STAT_DB_BASE::DATAPOS_MAKE+i, pnCum[i]);
	}

	//���� ���õ�
	pnCum = pClassFld->m_nBnsForceMastery;
	for(i = 0; i < force_mastery_num; i++)
	{
		if(pnCum[i] == 0)
			continue;

		Update_Stat(_STAT_DB_BASE::DATAPOS_FORCE+i, pnCum[i]);
	}

	//������ ���ʽ�..
	//������ �κ� �ƾ�����
	pCode = pClassFld->m_strDefaultItem;
	for(i = 0; i < max_class_bns_item; i++)
	{
		if(!strcmp(pCode[i], "0"))
			continue;

		int nTableCode = ::GetItemTableCode(pCode[i]);
		if(nTableCode == -1)
		{
			g_Main.m_logSystemError.Write("�ʱ��ɸ��� ����Ʈ ������ ����.., class: %s, %d��° ������(%s).. ���̺��ڵ忡��", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		if(nTableCode == tbl_code_key)//����Ű�� �ټ�����..
		{
			g_Main.m_logSystemError.Write("�ʱ��ɸ��� ����Ʈ ������ ����.., class: %s, %d��° ������(%s).. ����Ű���ټ�����", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		_base_fld* pFld = g_Main.m_tblItemData[nTableCode].GetRecord(pCode[i]);
		if(!pFld)
		{
			g_Main.m_logSystemError.Write("�ʱ��ɸ��� ����Ʈ ������ ����.., class: %s, %d��° ������(%s).. ���̺� ����", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		DWORD dwAmount = ::GetItemDurPoint(nTableCode, pFld->m_dwIndex);
		DWORD dwUpd = 0;	
		//������ Ÿ�Կ� ���� ���׷��̵� �ɹ� �б�..
		BYTE byItemKindCode = ::GetItemKindCode(nTableCode);
		if(byItemKindCode == item_kind_std)//�Ϲ�..
			dwUpd = __DF_UPT;
		else if(byItemKindCode == item_kind_ani)//�ִϸӽ�..
			dwUpd = ::GetMaxParamFromExp(pFld->m_dwIndex, dwAmount);
		else
			continue;//�ٸ��� �ټ�����..

		Update_ItemAdd(_STORAGE_POS::INVEN, i, nTableCode, pFld->m_dwIndex, dwAmount, dwUpd);
	}

	//������ ���� ������
	pCode = pClassFld->m_strPtSocket;
	for(i = 0; i < potion_belt_num; i++)
	{
		if(!strcmp(pCode[i], "0"))
			continue;

		BYTE nTableCode = ::GetItemTableCode(pCode[i]);
		if(nTableCode == -1)
		{
			g_Main.m_logSystemError.Write("�ʱ��ɸ��� ����Ʈ���� ������ ����.., class: %s, %d��° ������(%s).. ���̺��ڵ忡��", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		if(nTableCode != tbl_code_potion)
		{
			g_Main.m_logSystemError.Write("�ʱ��ɸ��� ����Ʈ���� ������ ����.., class: %s, %d��° ������(%s).. �����̾ƴϴ�", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		_base_fld* pFld = g_Main.m_tblItemData[nTableCode].GetRecord(pCode[i]);
		if(!pFld)
		{
			g_Main.m_logSystemError.Write("�ʱ��ɸ��� ����Ʈ���� ������ ����.., class: %s, %d��° ������(%s).. ���̺� ����", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		Update_ItemAdd(_STORAGE_POS::BELT, i, nTableCode, pFld->m_dwIndex, 10, __DF_UPT);
	}

	//��ų�������
	pCode = pClassFld->m_strSFSocket;
	for(i = 0; i < sf_linker_num; i++)
	{
		if(!strcmp(pCode[i], "0"))
			continue;

		_base_fld* pFld = g_Main.m_tblEffectData[effect_code_skill].GetRecord(pCode[i]);
		if(!pFld)
		{
			g_Main.m_logSystemError.Write("�ʱ��ɸ��� ����Ʈ��ũ ����.., class: %s, %d��° ��ũ(%s).. ���̺� ����", m_AvatorData.dbAvator.m_szClassCode, i, pCode[i]);		
			continue;
		}

		m_AvatorData.dbBelt.m_LinkList[i].Key.SetData(link_code_skill, pFld->m_dwIndex);
	}

	//���������� ������ ������Ŵ..
	Update_Level(1, 0);

	m_AvatorData.dbAvator.m_fStartPos[1] = DEFAULT_Y_POS;	//Y��ǥ�� �̷��� �־���� ������ �����Ҷ� ������ ����ش�.
	
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

		//���ð�üũ ����Ʈ�� ����..
		CUserDB::s_MessengerEnterCheck.Push(p->m_idWorld.wIndex, p->m_idWorld.dwSerial); 
	}
}

void _MESSENGER_ENTER_CHECK::Process(DWORD dwIndex, DWORD dwSerial)
{
	CUserDB* p = &g_UserDB[dwIndex];
	if(p->m_idWorld.dwSerial == dwSerial)
	{
		if(p->m_idMessenger.dwSerial == 0xFFFFFFFF)
		{	//�������ð��ȿ� �ȵ���..
			//��� ���������.
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