// MyNetWorking.cpp: implementation of the CNetworkEX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyNetWorking.h"
#include "MapData.h"
#include "MapOperate.h"
#include "Character.h"
#include "Player.h"
#include "MyUtil.h"
#include "protocol.h"
#include "pt_zone_client.h"
#include "pt_account_world.h"
#include "pt_world_msg.h"
#include "pt_msg_client.h"
#include "MainThread.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkEX::CNetworkEX()
{
}

CNetworkEX::~CNetworkEX()
{
}

void CNetworkEX::AcceptClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial)
{
	if(dwProID == client_line)
	{
		if(g_Main.m_bServerClosing)//전부강퇴중이라면 접속을 받지않는다.
		{
			Close(dwProID, dwIndex, false);
		}
		else
		{
			CUserDB* pUser = &g_UserDB[dwIndex];
			if(pUser->m_idWorld.dwSerial != 0xFFFFFFFF)
			{
				g_Main.m_logSystemError.Write("AcceptClientCheck() 이미 누가잇다..%d", dwIndex);
				Close(dwProID, dwIndex, false);
			}
			else
			{
				_socket* pSocket = GetSocket(dwProID, dwIndex);
				pUser->SetWorldCLID(dwSerial, (IP_ADDR*)&pSocket->m_Addr.sin_addr.S_un);
			}
		}

		CUserDB::s_nLoginNum++;
	}
	else if(dwProID == account_line)
	{
		g_Main.AccountServerLogin();
	}
	else if(dwProID == messenger_line)
	{
		//service start한다..
		g_Main.SerivceSelfStart();
	}
}

void CNetworkEX::CloseClientCheck(DWORD	dwProID, DWORD dwIndex, DWORD dwSerial)
{
	if(dwProID == client_line)
	{
		CPlayer* pPlayer = &g_Player[dwIndex];
		pPlayer->NetClose();//AvatorDB보다 먼저..	

		CUserDB* pUser = &g_UserDB[dwIndex];
		pUser->Exit_Account_Request();

		CUserDB::s_nLoginNum--;
	}
	else if(dwProID == account_line)
	{
		g_Main.gm_ServerClose();
		g_Main.m_bWorldOpen = false;
		g_Main.m_bWorldService = false;
	}
	else if(dwProID == messenger_line)
	{
		//service stop한다..
		g_Main.SerivceSelfStop();

		//모든 유저를 쫒는다..
		g_Main.pc_AllUserKickInform();
	}
}

bool CNetworkEX::DataAnalysis(DWORD	dwProID, DWORD dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(dwProID == client_line)
		return ClientLineAnalysis(dwClientIndex, pMsgHeader, pMsg);

	else if(dwProID == account_line)	
		return AccountLineAnalysis(dwClientIndex, pMsgHeader, pMsg);

	else if(dwProID == messenger_line)	
		MessengerLineAnalysis(dwClientIndex, pMsgHeader, pMsg);

	return false;
}

bool CNetworkEX::AccountLineAnalysis(int n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(system_msg == pMsgHeader->m_byType[0])
	{
		if(open_world_success_acwr == pMsgHeader->m_byType[1])
			return OpenWorldSuccessResult(n, pMsg);
		if(open_world_failure_acwr == pMsgHeader->m_byType[1])
			return OpenWorldFailureResult(n, pMsg);
		if(open_control_inform_acwr == pMsgHeader->m_byType[1])
			return true;//OpenControlInform(n, pMsg);
		if(trans_account_inform_acwr == pMsgHeader->m_byType[1])
			return TransAccountInform(n, pMsg);
		if(enter_world_result_acwr == pMsgHeader->m_byType[1])
			return EnterWorldResult(n, pMsg);
		if(force_close_command_acwr == pMsgHeader->m_byType[1])
			return ForceCloseCommand(n, pMsg);
		return false;
	}
	else if(control_msg == pMsgHeader->m_byType[0])
	{
		if(world_service_inform_acwr == pMsgHeader->m_byType[1])
			return WorldServiceInform(n, pMsg);
		if(world_exit_inform_acwr == pMsgHeader->m_byType[1])
			return WorldExitInform(n, pMsg);
		if(world_msg_inform_acwr == pMsgHeader->m_byType[1])
			return WorldMsgInform(n, pMsg);
		return false;
	}

	return false;
}

bool CNetworkEX::MessengerLineAnalysis(int n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(system_msg == pMsgHeader->m_byType[0])
	{
		if(open_msgr_request_mgwr == pMsgHeader->m_byType[1])
			return OpenMsgrRequest(n, pMsg);
		if(enter_user_report_mgwr == pMsgHeader->m_byType[1])
			return EnterUserReport(n, pMsg);
		if(exit_user_report_mgwr == pMsgHeader->m_byType[1])
			return ExitUserReport(n, pMsg);
		return false;
	}

	return false;
}

bool CNetworkEX::ClientLineAnalysis(int n, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(system_msg == pMsgHeader->m_byType[0])
	{
		if(enter_world_request_zone == pMsgHeader->m_byType[1])
			return EnterWorldRequest(n, pMsg);
		if(reged_char_request_zone == pMsgHeader->m_byType[1])
			return RegedCharRequest(n, pMsg);
		if(add_char_request_zone == pMsgHeader->m_byType[1])
			return AddCharRequest(n, pMsg);		
		if(del_char_request_zone == pMsgHeader->m_byType[1])
			return DelCharRequest(n, pMsg);
		if(sel_char_request_zone == pMsgHeader->m_byType[1])
			return SelCharRequest(n, pMsg);
		if(moveout_user_request_zone == pMsgHeader->m_byType[1])
			return MoveOutUserRequest(n, pMsg);
		return false;
	}
	if(init_msg == pMsgHeader->m_byType[0])
	{
		if(base_download_request_clzo == pMsgHeader->m_byType[1])	
			return BaseDownloadRequest(n, pMsg);
		if(inven_download_request_clzo == pMsgHeader->m_byType[1])	
			return InvenDownloadRequest(n, pMsg);
		if(cum_download_request_clzo == pMsgHeader->m_byType[1])	
			return CumDownloadRequest(n, pMsg);
		if(force_download_request_clzo == pMsgHeader->m_byType[1])	
			return ForceDownloadRequest(n, pMsg);
		if(special_download_request_clzo == pMsgHeader->m_byType[1])	
			return SpecialDownloadRequest(n, pMsg);
		if(new_pos_start_request_clzo == pMsgHeader->m_byType[1])	
			return NewPosStartRequest(n, pMsg);
		if(other_shape_request_clzo == pMsgHeader->m_byType[1])	
			return OtherShapeRequest(n, pMsg);					
		if(player_revival_request_clzo == pMsgHeader->m_byType[1])	
			return Revival(n, pMsg);
		if(exit_save_data_request_clzo == pMsgHeader->m_byType[1])	
			return ExitSaveDataRequest(n, pMsg);
		return false;
	}
	if(position_msg == pMsgHeader->m_byType[0])
	{
		if(move_request_clzo == pMsgHeader->m_byType[1])	
			return NextPoint(n, pMsg);
		if(real_movpos_request_clzo == pMsgHeader->m_byType[1])	
			return RealMovPosRequest(n, pMsg);
		if(player_stop_clzo == pMsgHeader->m_byType[1])
			return Stop(n, pMsg);
		if(goto_baseportal_request_clzo == pMsgHeader->m_byType[1])
			return GotoBasePortalRequest(n, pMsg);
		if(goto_avator_request_clzo == pMsgHeader->m_byType[1])
			return GotoAvatorRequest(n, pMsg);
		if(move_type_change_request_clzo == pMsgHeader->m_byType[1])
			return MoveTypeChangeRequeset(n, pMsg);
		return false;
	}
	if(fight_msg == pMsgHeader->m_byType[0])
	{
		if(attack_gen_request_clzo == pMsgHeader->m_byType[1])
			return AttackPersonalRequest(n, pMsg);
		if(attack_skill_request_clzo == pMsgHeader->m_byType[1])
			return AttackSkillRequest(n, pMsg);
		if(attack_force_request_clzo == pMsgHeader->m_byType[1])
			return AttackForceRequest(n, pMsg);
		if(attack_unit_request_clzo == pMsgHeader->m_byType[1])
			return AttackUnitRequest(n, pMsg);
		if(attack_test_request_clzo == pMsgHeader->m_byType[1])
			return AttackTestRequest(n, pMsg);
		if(mode_change_request_clzo == pMsgHeader->m_byType[1])
			return ModeChangeRequest(n, pMsg);
		return false;
	}
	if(item_msg == pMsgHeader->m_byType[0])
	{
		if(itembox_take_request_clzo == pMsgHeader->m_byType[1])
			return ItemboxTakeRequest(n, pMsg);
		if(throw_storage_request_clzo == pMsgHeader->m_byType[1])
			return ThrowStorageRequest(n, pMsg);
		if(use_potion_request_clzo == pMsgHeader->m_byType[1])
			return UsePotionRequest(n, pMsg); 
		if(equip_part_request_clzo == pMsgHeader->m_byType[1])
			return EquipPartRequest(n, pMsg);		
		if(embellish_request_clzo == pMsgHeader->m_byType[1])
			return EmbellishRequest(n, pMsg);
		if(off_part_request_clzo == pMsgHeader->m_byType[1])
			return OffPartRequest(n, pMsg); 
		if(make_item_request_clzo == pMsgHeader->m_byType[1])
			return MakeItemRequest(n, pMsg); 
		if(upgrade_item_request_clzo == pMsgHeader->m_byType[1])
			return UpgradeItemRequest(n, pMsg); 
		if(downgrade_item_request_clzo == pMsgHeader->m_byType[1])
			return DownGradeItemRequest(n, pMsg); 
		if(add_bag_request_clzo == pMsgHeader->m_byType[1])
			return AddBagRequest(n, pMsg);
		return false;
	}		
	if(map_msg == pMsgHeader->m_byType[0])
	{
		if(move_potal_request_clzo == pMsgHeader->m_byType[1])
			return MovePortalRequest(n, pMsg);
		return false;
	}
	if(trade_msg == pMsgHeader->m_byType[0])
	{
		if(buy_store_request_clzo == pMsgHeader->m_byType[1])
			return BuyStoreRequest(n, pMsg);
		if(sell_store_request_clzo == pMsgHeader->m_byType[1])
			return SellStoreRequest(n, pMsg);
		if(repair_request_clzo == pMsgHeader->m_byType[1])
			return RepairRequest(n, pMsg);
		if(store_list_request_clzo == pMsgHeader->m_byType[1])
			return StoreListRequest(n, pMsg);
		if(exchange_dalant_for_gold_request_clzo == pMsgHeader->m_byType[1])
			return ExchangeDalantForGoldRequest(n, pMsg);
		if(exchange_gold_for_dalant_request_clzo == pMsgHeader->m_byType[1])
			return ExchangeGoldForDalantRequest(n, pMsg);
		return false;
	}
	if(chat_msg == pMsgHeader->m_byType[0])
	{
		if(chat_operator_request_clzo == pMsgHeader->m_byType[1])
			return ChatOperatorRequest(n, pMsg);
		if(chat_circle_request_clzo == pMsgHeader->m_byType[1])
			return ChatCircleRequest(n, pMsg);
		if(chat_whisper_request_clzo == pMsgHeader->m_byType[1])
			return ChatWhisperRequest(n, pMsg);
		if(chat_far_request_clzo == pMsgHeader->m_byType[1])
			return ChatFarRequest(n, pMsg);
		if(chat_party_request_clzo == pMsgHeader->m_byType[1])
			return ChatPartyRequest(n, pMsg);
		if(chat_race_request_clzo == pMsgHeader->m_byType[1])
			return ChatRaceRequest(n, pMsg);
		if(chat_cheat_request_clzo == pMsgHeader->m_byType[1])
			return ChatCheatRequest(n, pMsg);
		if(chat_manage_request_clzo == pMsgHeader->m_byType[1])
			return ChatManageRequest(n, pMsg);
		if(chat_mgr_whisper_request_clzo == pMsgHeader->m_byType[1])
			return ChatMgrWhisperRequest(n, pMsg);
		return false;
	}		
	if(ui_msg == pMsgHeader->m_byType[0])
	{
		if(force_inven_change_request_clzo == pMsgHeader->m_byType[1])
			return ForceInvenChangeRequest(n, pMsg);	
		if(animus_inven_change_request_clzo == pMsgHeader->m_byType[1])
			return AnimusInvenChangeRequest(n, pMsg);	
		if(res_separation_request_clzo == pMsgHeader->m_byType[1])
			return ResSeparationRequest(n, pMsg);
		if(res_division_request_clzo == pMsgHeader->m_byType[1])
			return ResDivisionRequest(n, pMsg);
		if(potionsocket_change_request_clzo == pMsgHeader->m_byType[1])
			return PotionSocketChangeRequest(n, pMsg); 
		if(potionsocket_separation_request_clzo == pMsgHeader->m_byType[1])
			return PotionSocketSeparationRequest(n, pMsg); 
		if(potionsocket_division_request_clzo == pMsgHeader->m_byType[1])
			return PotionSocketDivisionRequest(n, pMsg); 
		return false;
	}	
	if(resource_msg == pMsgHeader->m_byType[0])
	{
		if(mine_start_request_clzo == pMsgHeader->m_byType[1])
			return MineStartRequest(n, pMsg);
		if(mine_cancle_request_clzo == pMsgHeader->m_byType[1])
			return MineCancleRequest(n, pMsg);
		if(mine_complete_request_clzo == pMsgHeader->m_byType[1])
			return MineCompleteRequest(n, pMsg);
		if(ore_cutting_request_clzo == pMsgHeader->m_byType[1])
			return ResCuttingRequest(n, pMsg);
		if(ore_into_bag_request_clzo == pMsgHeader->m_byType[1])
			return OreIntoBagRequest(n, pMsg);
		if(cutting_complete_request_clzo == pMsgHeader->m_byType[1])
			return CuttingCompleteRequest(n, pMsg);
		return false;
	}
	if(party_msg == pMsgHeader->m_byType[0])
	{
		if(party_join_invitation_clzo == pMsgHeader->m_byType[1])
			return PartyJoinInvitation(n, pMsg);
		if(party_join_invitation_answer_clzo == pMsgHeader->m_byType[1])
			return PartyJoinInvitationAnswer(n, pMsg);
		if(party_join_application_clzo == pMsgHeader->m_byType[1])
			return PartyJoinApplication(n, pMsg);
		if(party_join_application_answer_clzo == pMsgHeader->m_byType[1])
			return PartyJoinApplicatiohAnswer(n, pMsg);
		if(party_leave_self_request_clzo == pMsgHeader->m_byType[1])
			return PartyLeaveSelfRequest(n, pMsg);
		if(party_leave_compulsion_request_clzo == pMsgHeader->m_byType[1])
			return PartyLeaveCompulsionRequest(n, pMsg);
		if(party_disjoint_request_clzo == pMsgHeader->m_byType[1])
			return PartyDisjointRequest(n, pMsg);
		if(party_succession_request_clzo == pMsgHeader->m_byType[1])
			return PartySuccessionRequest(n, pMsg);
		if(party_lock_request_clzo == pMsgHeader->m_byType[1])
			return PartyLockRequest(n, pMsg);
		return false;
	}
	if(effect_msg == pMsgHeader->m_byType[0])
	{
		if(force_request_clzo == pMsgHeader->m_byType[1])
			return ForceRequest(n, pMsg);
		if(skill_request_clzo == pMsgHeader->m_byType[1])
			return SkillRequest(n, pMsg);
		if(class_skill_request_clzo == pMsgHeader->m_byType[1])
			return ClassSkillRequest(n, pMsg);
		if(make_tower_request_clzo == pMsgHeader->m_byType[1])
			return MakeTowerRequest(n, pMsg);
		if(back_tower_request_clzo == pMsgHeader->m_byType[1])
			return BackTowerRequest(n, pMsg);
		return false;
	}	
	if(dtrade_msg == pMsgHeader->m_byType[0])
	{
		if(d_trade_ask_request_clzo == pMsgHeader->m_byType[1])
			return DTradeAskRequest(n, pMsg);
		if(d_trade_answer_request_clzo == pMsgHeader->m_byType[1])
			return DTradeAnswerRequest(n, pMsg);
		if(d_trade_cancle_request_clzo == pMsgHeader->m_byType[1])
			return DTradeCancleRequest(n, pMsg);
		if(d_trade_lock_request_clzo == pMsgHeader->m_byType[1])
			return DTradeLockRequest(n, pMsg);
		if(d_trade_ok_request_clzo == pMsgHeader->m_byType[1])
			return DTradeOKRequest(n, pMsg);
		if(d_trade_add_request_clzo == pMsgHeader->m_byType[1])
			return DTradeAddRequest(n, pMsg);
		if(d_trade_del_request_clzo == pMsgHeader->m_byType[1])
			return DTradeDelRequest(n, pMsg);
		if(d_trade_bet_request_clzo == pMsgHeader->m_byType[1])
			return DTradeBetRequest(n, pMsg);
		return false;
	}
	if(dungeon_msg == pMsgHeader->m_byType[0])
	{
		if(battledungeon_open_request_clzo == pMsgHeader->m_byType[1])
			return BattleDungeonOpenRequest(n, pMsg);
		if(goto_dungeon_request_clzo == pMsgHeader->m_byType[1])
			return GotoDungeonRequest(n, pMsg);
		if(outof_dungeon_request_clzo == pMsgHeader->m_byType[1])
			return OutofDungeonRequest(n, pMsg);
		return false;
	}
	if(unit_msg == pMsgHeader->m_byType[0])
	{
		if(unit_frame_buy_request_clzo == pMsgHeader->m_byType[1])
			return UnitFrameBuyRequest(n, pMsg);
		if(unit_sell_request_clzo == pMsgHeader->m_byType[1])
			return UnitSellRequest(n, pMsg);
		if(unit_part_tuning_request_clzo == pMsgHeader->m_byType[1])
			return UnitPartTuningRequest(n, pMsg);
		if(unit_frame_repair_request_clzo == pMsgHeader->m_byType[1])
			return UnitFrameRepairRequest(n, pMsg);
		if(unit_bullet_fill_request_clzo == pMsgHeader->m_byType[1])
			return UnitBulletFillRequest(n, pMsg);
		if(unit_pack_fill_request_clzo == pMsgHeader->m_byType[1])
			return UnitPackFillRequest(n, pMsg);
		if(unit_delivery_request_clzo == pMsgHeader->m_byType[1])
			return UnitDeliveryRequest(n, pMsg);
		if(unit_return_request_clzo == pMsgHeader->m_byType[1])
			return UnitReturnRequest(n, pMsg);
		if(unit_take_request_clzo == pMsgHeader->m_byType[1])
			return UnitTakeRequest(n, pMsg);
		if(unit_leave_request_clzo == pMsgHeader->m_byType[1])
			return UnitLeaveRequest(n, pMsg);
		if(unit_bullet_replace_request_clzo == pMsgHeader->m_byType[1])
			return UnitBulletReplaceRequest(n, pMsg);
		return false;
	}
	if(recall_msg == pMsgHeader->m_byType[0])
	{
		if(animus_recall_request_clzo == pMsgHeader->m_byType[1])
			return AnimusRecallRequest(n, pMsg);
		if(animus_return_request_clzo == pMsgHeader->m_byType[1])
			return AnimusReturnRequest(n, pMsg);
		if(animus_command_request_clzo == pMsgHeader->m_byType[1])
			return AnimusCommandRequest(n, pMsg);
		if(animus_target_request_clzo == pMsgHeader->m_byType[1])
			return AnimusTargetRequest(n, pMsg);
		return false;
	}	
	if(event_msg == pMsgHeader->m_byType[0])
	{
		if(select_class_request_clzo == pMsgHeader->m_byType[1])
			return SelectClassRequest(n, pMsg);
		return false;
	}
	if(sync_msg == pMsgHeader->m_byType[0])
	{
		if(object_server_pos_request_clzo == pMsgHeader->m_byType[1])
			return ObjectServerPosRequest(n, pMsg);
		return false;
	}
	return false;
}

