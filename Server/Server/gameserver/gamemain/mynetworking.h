// MyNetWorking.h: interface for the CMyNetWorking class.
//CNetWorking을 상속받아서 들어온 데이타를분석 및 데이타를 전송한다. 
//////////////////////////////////////////////////////////////////////

#ifndef _MyNetworking_h_
#define _MyNetworking_h_

#include "NetWorking.h"
#include "Protocol.h"
#include "CharacterDB.h"

#define client_line		0
#define account_line	1
#define messenger_line	2
#define NET_LINE_NUM	3	

class CNetworkEX : public CNetWorking
{
private:

public:
	CNetworkEX();
	virtual ~CNetworkEX();

private:

	virtual bool DataAnalysis(DWORD	dwProID, DWORD dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg);
	virtual void AcceptClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial);
	virtual void CloseClientCheck(DWORD	dwProID, DWORD dwIndex, DWORD dwSerial);

	virtual void UserLoop() {};
	
	bool ClientLineAnalysis(int n, _MSG_HEADER* pMsgHeader, char* pMsg);
	bool AccountLineAnalysis(int n, _MSG_HEADER* pMsgHeader, char* pMsg);
	bool MessengerLineAnalysis(int n, _MSG_HEADER* pMsgHeader, char* pMsg);

	//account..
	bool OpenWorldSuccessResult(DWORD n, char* pMsg);
	bool OpenWorldFailureResult(DWORD n, char* pMsg);
	bool OpenControlInform(DWORD n, char* pMsg);
	bool TransAccountInform(DWORD n, char* pMsg);
	bool EnterWorldResult(DWORD n, char* pMsg);
	bool ForceCloseCommand(DWORD n, char* pMsg);
	bool WorldServiceInform(DWORD n, char* pMsg);
	bool WorldExitInform(DWORD n, char* pMsg);
	bool WorldMsgInform(DWORD n, char* pMsg);

	//message..
	bool OpenMsgrRequest(DWORD n, char* pMsg);
	bool EnterUserReport(DWORD n, char* pMsg);
	bool ExitUserReport(DWORD n, char* pMsg);
	
	//System..
	bool EnterWorldRequest(int n, char* pBuf);
	bool RegedCharRequest(int n, char* pBuf);
	bool AddCharRequest(int n, char* pBuf);
	bool DelCharRequest(int n, char* pBuf);
	bool SelCharRequest(int n, char* pBuf);
	bool MoveOutUserRequest(int n, char* pBuf);

	//Attack..
	bool AttackForceRequest(int n, char* pBuf);
	bool AttackPersonalRequest(int n, char* pBuf);
	bool AttackSkillRequest(int n, char* pBuf);
	bool AttackUnitRequest(int n, char* pBuf);
	bool AttackTestRequest(int n, char* pBuf);
	bool ModeChangeRequest(int n, char* pBuf);

	//Init..
	bool BaseDownloadRequest(int n, char* pBuf);
	bool CumDownloadRequest(int n, char* pBuf);
	bool ForceDownloadRequest(int n, char* pBuf);
	bool InvenDownloadRequest(int n, char* pBuf);
	bool SpecialDownloadRequest(int n, char* pBuf);
	bool NewPosStartRequest(int n, char* pBuf);
	bool OtherShapeRequest(int n, char* pBuf);
	bool Revival(int n, char* pBuf);
	bool ExitSaveDataRequest(int n, char* pBuf);

	//UI..
	bool ForceInvenChangeRequest(int n, char* pBuf);
	bool AnimusInvenChangeRequest(int n, char* pBuf);
	bool PotionSocketChangeRequest(int n, char* pBuf);
	bool PotionSocketDivisionRequest(int n, char* pBuf);
	bool PotionSocketSeparationRequest(int n, char* pBuf);
	bool ResDivisionRequest(int n, char* pBuf);
	bool ResSeparationRequest(int n, char* pBuf);

	//Trade..
	bool BuyStoreRequest(int n, char* pBuf);
	bool BuyUsedRequest(int n, char* pBuf);
	bool ExchangeDalantForGoldRequest(int n, char* pBuf);
	bool ExchangeGoldForDalantRequest(int n, char* pBuf);
	bool RepairRequest(int n, char* pBuf);
	bool ResCuttingRequest(int n, char* pBuf);
	bool SellStoreRequest(int n, char* pBuf);
	bool StoreListRequest(int n, char* pBuf);
	
	//Chat..
	bool ChatOperatorRequest(int n, char* pBuf);
	bool ChatCircleRequest(int n, char* pBuf);
	bool ChatWhisperRequest(int n, char* pBuf);
	bool ChatFarRequest(int n, char* pBuf);
	bool ChatPartyRequest(int n, char* pBuf);
	bool ChatRaceRequest(int n, char* pBuf);
	bool ChatCheatRequest(int n, char* pBuf);
	bool ChatManageRequest(int n, char* pBuf);
	bool ChatMgrWhisperRequest(int n, char* pBuf);

	//Resource..
	bool CuttingCompleteRequest(int n, char* pBuf);
	bool MineCancleRequest(int n, char* pBuf);
	bool MineCompleteRequest(int n, char* pBuf);
	bool MineStartRequest(int n, char* pBuf);
	bool OreIntoBagRequest(int n, char* pBuf);

	//Item..
	bool DownGradeItemRequest(int n, char* pBuf);
	bool EmbellishRequest(int n, char* pBuf);
	bool EquipPartRequest(int n, char* pBuf);
	bool ItemboxTakeRequest(int n, char* pBuf);
	bool MakeItemRequest(int n, char* pBuf);
	bool OffPartRequest(int n, char* pBuf);
	bool ThrowStorageRequest(int n, char* pBuf);
	bool UpgradeItemRequest(int n, char* pBuf);
	bool UsePotionRequest(int n, char* pBuf);
	bool WearChangeRequest(int n, char* pBuf);
	bool AddBagRequest(int n, char* pBuf);

	//Positon..
	bool NextPoint(int n, char* pBuf);
	bool RealMovPosRequest(int n, char* pBuf);
	bool Stop(int n, char* pBuf);	
	bool GotoBasePortalRequest(int n, char* pBuf);	
	bool GotoAvatorRequest(int n, char* pBuf);	
	bool MoveTypeChangeRequeset(int n, char* pBuf);	

	//Map..
	bool MovePortalRequest(int n, char* pBuf);

	//party..
	bool PartyJoinInvitation(int n, char* pBuf);
	bool PartyJoinInvitationAnswer(int n, char* pBuf);
	bool PartyJoinApplication(int n, char* pBuf);
	bool PartyJoinApplicatiohAnswer(int n, char* pBuf);
	bool PartyLeaveSelfRequest(int n, char* pBuf);
	bool PartyLeaveCompulsionRequest(int n, char* pBuf);
	bool PartyDisjointRequest(int n, char* pBuf);
	bool PartySuccessionRequest(int n, char* pBuf);
	bool PartyLockRequest(int n, char* pBuf);

	//effect..
	bool ForceRequest(int n, char* pBuf);
	bool SkillRequest(int n, char* pBuf);
	bool ClassSkillRequest(int n, char* pBuf);
	bool MakeTowerRequest(int n, char* pBuf);
	bool BackTowerRequest(int n, char* pBuf);

	//DTrade..
	bool DTradeAskRequest(int n, char* pBuf);
	bool DTradeAnswerRequest(int n, char* pBuf);
	bool DTradeCancleRequest(int n, char* pBuf);
	bool DTradeOKRequest(int n, char* pBuf);
	bool DTradeLockRequest(int n, char* pBuf);
	bool DTradeAddRequest(int n, char* pBuf);
	bool DTradeDelRequest(int n, char* pBuf);
	bool DTradeBetRequest(int n, char* pBuf);

	//dungeon..
	bool BattleDungeonOpenRequest(int n, char* pBuf);
	bool GotoDungeonRequest(int n, char* pBuf);
	bool OutofDungeonRequest(int n, char* pBuf);

	//unit..
	bool UnitFrameBuyRequest(int n, char* pBuf);
	bool UnitSellRequest(int n, char* pBuf);
	bool UnitPartTuningRequest(int n, char* pBuf);
	bool UnitFrameRepairRequest(int n, char* pBuf);
	bool UnitBulletFillRequest(int n, char* pBuf);
	bool UnitPackFillRequest(int n, char* pBuf);
	bool UnitDeliveryRequest(int n, char* pBuf);
	bool UnitReturnRequest(int n, char* pBuf);
	bool UnitTakeRequest(int n, char* pBuf);
	bool UnitLeaveRequest(int n, char* pBuf);
	bool UnitBulletReplaceRequest(int n, char* pBuf);

	//recall..
	bool AnimusRecallRequest(int n, char* pBuf);
	bool AnimusReturnRequest(int n, char* pBuf);
	bool AnimusCommandRequest(int n, char* pBuf);
	bool AnimusTargetRequest(int n, char* pBuf);

	//event..
	bool SelectClassRequest(int n, char* pBuf);

	//sync..
	bool ObjectServerPosRequest(int n, char* pBuf);
};

#endif 
