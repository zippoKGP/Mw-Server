#ifndef _world_avator_h_
#define _world_avator_h_

struct _WA_AVATOR_CODE{

	_CLID		m_id;
	char		m_szName[max_name_len+1];
//	BYTE		m_byRaceSexCode;
};

class CPartyPlayer
{
public:

	bool				m_bLogin;
	_CLID				m_id;
	char				m_szName[max_name_len+1];

	WORD				m_wZoneIndex;	
//	BYTE				m_byRaceSexCode;
	
	//파티시스템..
	CPartyPlayer*		m_pPartyBoss;
	CPartyPlayer*		m_pPartyMember[member_per_party];//보스만 사용.. 맵버는 보스꺼 참조..
	bool				m_bLock;

public:

	CPartyPlayer();

	void Init(WORD wIndex);

	void EnterWorld(_WA_AVATOR_CODE* pData, WORD wZoneIndex);
	void ExitWorld(CPartyPlayer** OUT ppoutNewBoss);

	//party
	void PartyListInit();
	bool IsPartyMode();
	bool IsPartyBoss();
	bool IsPartyLock();
	int	 GetPopPartyMember();
	CPartyPlayer**  GetPtrPartyMember();
	CPartyPlayer*	GetPtrFromSerial(DWORD dwWorldSerial);

	//party boss
	bool FoundParty(CPartyPlayer* pParticiper);
	bool DisjointParty();	
	bool InsertPartyMember(CPartyPlayer* pJoiner);	
	bool RemovePartyMember(CPartyPlayer* pExiter, CPartyPlayer** ppoutNewBoss);
	bool InheritBoss(CPartyPlayer* pSuccessor);
	bool SetLockMode(bool bLock);
};

void wa_EnterWorld(_WA_AVATOR_CODE* pData, WORD wZoneIndex);
void wa_ExitWorld(_CLID* pidWorld);

void wa_PartyJoin(_CLID* pidBoss, _CLID* pidJoiner);
void wa_PartySelfLeave(_CLID* pidLeaver);
void wa_PartyForceLeave(_CLID* pidBoss, _CLID* pidLeaver);
void wa_PartyDisjoint(_CLID* pidBoss);
void wa_PartySuccession(_CLID* pidBoss, _CLID* pidSuccessor);
void wa_PartyLock(_CLID* pidBoss, bool bLock);


#endif