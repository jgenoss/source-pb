#pragma once

#include "Network/ClientP2PDef.h"

//
// �켱 �������ؽ�Ʈ���� �и����� �ϰ�, ���߿��� ��ü������ ����� �����ؾߵ�...
//

enum CASHITEMSKILL_LIST
{
	SLOTITEM_SHORT_RESPAWN_100 =			0x400000,
	SLOTITEM_SHORT_RESPAWN_50 =				0x200000,
	SLOTITEM_SHORT_RESPAWN_30 =				0x100000,
	SLOTITEM_SHORT_RESPAWN_20 =				0x80000,
	SLOTITEM_AMMO_UP		 =				0x40000	,			// Extended Magazines
	SLOTITEM_PICKUP_WEAPON	 =				0x20000	,		 
	SLOTITEM_VALENTINE_CHOCOLATE =			0x10000	,			// �߷�Ÿ�� ���ڷ� (HP+10)
	SLOTITEM_QUICK_CHANGE_WEAPON =			0x8000,
	SLOTITEM_QUICK_CHANGE_MAGAZINE =		0x4000,
	SLOTITEM_MEGA_HP10				 =		0x2000,
	SLOTITEM_INCREASE_INVINCIBLE_TIME =		0x1000,
	SLOTITEM_BULLET_PROOF_VEST		 =		0x800,
	SLOTITEM_FULLMETALJACKETAMMO	 =		0x400,
	SLOTITEM_HOLLOW_POINT_AMMO		 =		0x200,
	SLOTITEM_C4_SPEED_KIT			 =		0x100,
	SLOTITEM_INCREASE_GRENADE_SLOT	 =		0x80,
	SLOTITEM_JACKETED_HELLOW_POINT_AMMO =	0x40,
	SLOTITEM_MEGA_HP5				 =		0x20,
	SLOTITEM_BULLET_PROOF_VEST_PLUS	 =		0x10,
	SLOTITEM_HOLLOW_POINT_AMMO_PLUS	 =		0x08,
	SLOTITEM_BULLET_PROOF_VEST_METAL =		0x04,
	SLOTITEM_KETUPAT				 =		0x02,	
	SLOTITEM_BULLET_PROOF_VEST_GM	 =		0x01,
};

enum CASHITEMSKILL_TYPE
{
	CASHITEMSKILL_UNKNOWN	= -1,		//
	CASHITEMSKILL_ACCURACY	= 0,		//���߷�
	CASHITEMSKILL_PENETRATE,			//�����
	CASHITEMSKILL_RECOIL,				//�ݵ�����			
	CASHITEMSKILL_SWAPWEAPON_RATE,		//���ⱳü
	CASHITEMSKILL_REBULLET_RATE,		//�������ӵ�
	CASHITEMSKILL_BULLETADD,			//ź�� ����

	CASHITEMSKILL_DAMAGE,				//���ݷ�
	CASHITEMSKILL_DEFENSE_RATE,			//����
	CASHITEMSKILL_DEFENSE_OVERLAP,		//�����ߺ�
	CASHITEMSKILL_MOVE_RATE,			//�̵��ӵ� ����
	CASHITEMSKILL_HP_ADD,				//HP����
	CASHITEMSKILL_HP_RATE,				//�۽�Ʈ ����

	CASHITEMSKILL_RESPAWN,				//������
	CASHITEMSKILL_INVINCIBLE_TIME,		//�����ð�
	CASHITEMSKILL_PICKUP_WEAPON,		// �����ݱ�
	CASHITEMSKILL_C4SPEED,				//C4��ü�ӵ�
	CASHITEMSKILL_GRENADE_ADD,			//GRENADE �߰�

	CASHITEMSKILL_SETITEM1,				//��Ʈ������ �߰�

	CASHITEMSKILL_MAX,
};

struct	UseItemSlotInfo		// [array initializer]
{
	CASHITEMSKILL_LIST	SlotID;
	CASHITEMSKILL_TYPE	SlotType;
	REAL32				iValue;
};

class BattleSlotContext : public i3::shared_ginst<BattleSlotContext>
{
public:
	BattleSlotContext();

	void				InitCharaType();
	CHARA::TYPE			getCharaType( INT32 idx) const				{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT); return m_CharaType[ idx]; }
	void				setCharaType( INT32 idx, CHARA::TYPE type)	{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT); m_CharaType[ idx] = type; }

	void				SetAICharaInfo_P2P();		

	const NET_CHARA_INFO *	getNetCharaInfo( INT32 idx, CHARA::BUFFER_TYPE eType = CHARA::BUFFER_USER) const	{ return (this->*m_getNetCharaInfo[eType])(idx); }
	NET_CHARA_INFO *	setNetCharaInfo( INT32 idx, CHARA::BUFFER_TYPE eType = CHARA::BUFFER_USER)				{ return (this->*m_setNetCharaInfo[eType])(idx); }

	const GAMEINFO_CHARACTER *	getGameInfo_Chara( INT32 idx, CHARA::BUFFER_TYPE eType = CHARA::BUFFER_USER) const 	{ return (this->*m_getGameInfo_Chara[eType])(idx); }
	GAMEINFO_CHARACTER *	setGameInfo_Chara( INT32 idx, CHARA::BUFFER_TYPE eType = CHARA::BUFFER_USER)			{ return (this->*m_setGameInfo_Chara[eType])(idx); }

	void				ResetSlotInfos( void)			{ for(UINT32 i = 0; i < SLOT_MAX_COUNT; i++) m_SlotInfo[i].Reset(); }
	const SLOT_INFO *	getSlotInfo( INT32 idx) const	{ I3_BOUNDCHK_RETURN( idx, USER_SLOT_COUNT, nullptr); return &m_SlotInfo[ idx]; }
	SLOT_INFO *			setSlotInfo( INT32 idx)			{ I3_BOUNDCHK( idx, USER_SLOT_COUNT); return &m_SlotInfo[ idx]; }
	const SLOT_INFO *	getMySlotInfo( void) const		{ I3_BOUNDCHK( m_MySlotIdx, USER_SLOT_COUNT); return &m_SlotInfo[ m_MySlotIdx]; }

	void				setSlotState( INT32 idx, SLOT_STATE state)	{ I3_BOUNDCHK( idx, USER_SLOT_COUNT);	m_SlotInfo[idx]._State = (UINT8) state; }
	void				setSlotRank( INT32 idx, UINT8 rank)			{ I3_BOUNDCHK( idx, USER_SLOT_COUNT);	m_SlotInfo[idx]._Rank = rank; }
	void				setSlotCafe( INT32 idx, UINT8 cafe)			{ I3_BOUNDCHK( idx, USER_SLOT_COUNT);	m_SlotInfo[idx]._pcCafe = cafe; }
	void				SetSlotInfo( INT32 idx, SLOT_STATE state, UINT8 rank, UINT32 clanIdx, UINT32 clanState, UINT8 clanExpRank,
							UINT32 clanMark, UINT8 cafe, UINT8 esportID, UINT8 NationCode)
	{
		m_SlotInfo[idx]._State = (UINT8) state;
		m_SlotInfo[idx]._Rank = rank;
		m_SlotInfo[idx]._clanidx = clanIdx;
		m_SlotInfo[idx]._clanstate = clanState;
		m_SlotInfo[idx]._clanExpRank = clanExpRank;
		m_SlotInfo[idx]._clanMark = clanMark;
		m_SlotInfo[idx]._pcCafe = cafe;
		m_SlotInfo[idx]._eSportID = esportID;
		m_SlotInfo[idx]._ui8NationalCode = NationCode;
	}

	bool				IsGMUserForClient( UINT32 idx) const		{ return (m_SlotInfo[idx]._Rank == RANK_GAMEMASTER); }	//GM slot ����.
	bool				IsGMObserverForClient( UINT32 idx) const	{ return m_SlotInfo[idx]._bIsGmObserverMode;}		//GM�� observermode ����.


	INT32 				getMainSlotIdx( void ) const		{ ISNETWORKRETURNV(false,0);		return m_MainSlotIdx;	}
	void				setMainSlotIdx( INT32 nValue )		{ I3_BOUNDCHK( nValue, SLOT_MAX_COUNT); m_MainSlotIdx = nValue; }
	INT32 				getMySlotIdx( void ) const;
	void				setMySlotIdx( INT32 nValue )		{	I3_BOUNDCHK( nValue, USER_SLOT_COUNT);		m_MySlotIdx = nValue; }

	bool				IsRoomMaster( void ) const			{	return m_MySlotIdx == m_MainSlotIdx?true:false;		}

#if defined( DEF_OBSERVER_MODE)
	bool				isObserver( INT32 idx) const		{	return ((idx >= SLOT_MAX_COUNT) && (idx < (SLOT_ALL_COUNT)));	}
	bool				isObserverGM( void) const			{	return (m_MySlotIdx == SLOT_MAX_COUNT + SLOT_MAX_OBSERVER_COUNT);	}
	bool				isObserverMe( void) const			{	return (m_MySlotIdx >= SLOT_MAX_COUNT && m_MySlotIdx <= SLOT_ALL_COUNT); }
#endif

	bool				IsObservingStateMe() const; // �����÷��̾�� �������ΰ�
	bool				IsObservingState(INT32 slotIdx) const;
	bool				IsObservingTargetSlot(INT32 slotIdx) const; // �����÷��̾ �����ϴ� Ÿ���÷��̾��ΰ�

	// UI��
	INT32				GetMySlotIdxEx(void) const			{ return m_MySlotIdx;	}
	void				setMySlotIdxEx(INT32 nValue)		{ I3_BOUNDCHK(nValue, SLOT_MAX_COUNT); m_MySlotIdx = nValue; }


	//Chara Nick( User, AI)
	i3::rc_wstring		getNickForSlot(INT32 idx, CHARA::BUFFER_TYPE eType = CHARA::BUFFER_USER) const;
	void				setNickForSlot(INT32 idx, const char* szNickName);


	void				setNickForSlot(INT32 idx, const i3::rc_wstring& wstrNickName);
	void				setNickForSlot_BufferNewAI(INT32 idx, const i3::rc_wstring& wstrNickName);
	void				setNickForSlot(INT32 idx, CHARA::BUFFER_TYPE eType, const i3::rc_wstring& wstrNickName);

	INT32				GetUserIndex(const i3::wliteral_range& wNickNameRng);
	
	UINT8				getNickColorForSlot(INT32 idx) const;						
	void				setNickColorForSlot(INT32 idx, UINT8 ui8Color);

	// ColorFireMuzzleFlash (ColorFireEffect)
	UINT16				getFireMuzzleColorForSlot(INT32 idx) const;
	void				setFireMuzzleColorForSlot(INT32 idx, UINT16 ui16Color);
		
	void				getGameRoomOption(GAME_ROOM_OPTION *_opt)
	{
		*_opt = roomOption;
	}
	void				setGameRoomOption(const GAME_ROOM_OPTION *_opt)
	{
		roomOption = *_opt;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���� protected ������ ClanMatch ������ �̵� �Ǿ����ϴ�.
	void			InitRoomData(void);						// ������ �ʱ�ȭ(�濡 �������� ȣ��Ǿ����)

	void			DumpGameInfo(void);

	INT32			GetBattleUserCount() const;
	INT32			GetBattleRedTeamCount() const;
	static CHARACTER_TEAM_TYPE getTeam(INT32 slotIdx);

	// �ڵ��������밡 ����Ǿ����� ���������� �ݴ�� ���´�.	
	CHARACTER_TEAM_TYPE		getMyTeam( void) const;
	CHARACTER_TEAM_TYPE		getEnemyTeamType() const;
	bool					IsRedTeam() const;
	bool					IsRedTeam(INT32 idx) const;
	bool					IsMyTeam(INT32 idx) const;
	
	// �����ε����� ���Ǻ��� ������ ������ �����ش�.
	CHARACTER_TEAM_TYPE		getSvrTeamTypeValue(INT32 slotIdx) 
	{
		I3_BOUNDCHK(slotIdx, SLOT_MAX_COUNT);
		return (slotIdx % 2) ? CHARACTER_TEAM_BLUE : CHARACTER_TEAM_RED;
	}

	bool			CheckAnyoneAlive(const CHARACTER_TEAM_TYPE teamType) const;
	INT32			CheckTotalNumOfPlayers(const CHARACTER_TEAM_TYPE teamType) const;
	INT32			CheckTotalNumOfPlayers(const CHARACTER_TEAM_TYPE teamType, const INT32 begin, const INT32 end) const;
	INT32			CheckTotalNumOfAlive(const CHARACTER_TEAM_TYPE teamType) const;
	INT32			CheckTotalNumOfAlive(const CHARACTER_TEAM_TYPE teamType, const INT32 begin, const INT32 end) const;
	INT32			CheckPlayerOrder(const INT32 idx) const;
	INT32			FindAnyoneAlive() const; 
	INT32			FindAnyoneAlive(const CHARACTER_TEAM_TYPE teamType) const;
	INT32			FindNextPlayerAlive(const INT32 currIdx) const;
	INT32			FindPrevPlayerAlive(const INT32 currIdx) const;
	INT32			FindNextPlayerAlive(const CHARACTER_TEAM_TYPE teamType, const INT32 currIdx) const;
	INT32			FindPrevPlayerAlive(const CHARACTER_TEAM_TYPE teamType, const INT32 currIdx) const;
	void			SetKillerIdxMostRecent(const INT32 idx);
	INT32			GetKillerIdxMostRecent() const;
	bool			IsLocalPlayerDead() const;

	//	���� ������ �ش� �������� ����ϰ� �ִ��� ���� (SLOT_ITEM_ ���� �÷��� �˻�)
	bool				isUseCashItemInSlot( INT32 idxSlot, UINT32 useSlotItemFlag) const;
	void				SetUseCashItem( INT32 idxSlot);
	REAL32				GetUseCashItemValue( INT32 idxSlot, CASHITEMSKILL_TYPE itemtype) const;

	REAL32				getMissionBomb_SetTime_UsingSpeedKit( INT32 idxSlot) const;
	REAL32				getMissionBomb_DisarmTime_UsingSpeedKit(INT32 idxSlot) const;
	REAL32				getMissionBomb_SetTime_UsingSkill(INT32 idxSlot) const;
	REAL32				getMissionBomb_DisarmTime_UsingSkill(INT32 idxSlot) const;

	void				CreateBattle( void);		// ��Ʋ ���Խ� ȣ��

private:
	CHARA::TYPE		m_CharaType[ SLOT_MAX_COUNT];					// [initialize at constructor]

	/** \brief ���� ĳ���� ���� */
	NET_CHARA_INFO	m_CharaInfoBuffer[ SLOT_MAX_COUNT ];
	NET_CHARA_INFO	m_AICharaInfoBuffer[ MAX_COUNT_DOMI_DINO ];

	const NET_CHARA_INFO* getNetCharaInfo_Normal(INT32 slot) const	{ I3_BOUNDCHK( slot, SLOT_MAX_COUNT); return m_CharaInfoBuffer + slot; }
	const NET_CHARA_INFO* getNetCharaInfo_AI(INT32 slot) const		{ I3_BOUNDCHK( slot, MAX_COUNT_DOMI_DINO); return m_AICharaInfoBuffer + slot; }
	const NET_CHARA_INFO* (BattleSlotContext::*m_getNetCharaInfo[CHARA::BUFFER_MAX])(INT32 slot) const;

	NET_CHARA_INFO* setNetCharaInfo_Normal(INT32 slot)			{ I3_BOUNDCHK( slot, SLOT_MAX_COUNT); return m_CharaInfoBuffer + slot; }
	NET_CHARA_INFO* setNetCharaInfo_AI(INT32 slot)				{ I3_BOUNDCHK( slot, MAX_COUNT_DOMI_DINO); return m_AICharaInfoBuffer + slot; }
	NET_CHARA_INFO* (BattleSlotContext::*m_setNetCharaInfo[CHARA::BUFFER_MAX])(INT32 slot);

	// Game Data  Update�ϸ鼭 ���� �����͸� �����մϴ�. Recv, Send�� 
	GAMEINFO_CHARACTER		m_CharacterGameInfo	[ SLOT_MAX_COUNT ];						//User
	GAMEINFO_CHARACTER		m_CharacterGameInfoForAI[ MAX_COUNT_DOMI_DINO ];			//AI

	const GAMEINFO_CHARACTER*	getGameInfo_Chara_Normal( INT32 idx) const	{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT);		return m_CharacterGameInfo + idx; }
	const GAMEINFO_CHARACTER*	getGameInfo_Chara_AI(INT32 idx) const		{ I3_BOUNDCHK( idx, MAX_COUNT_DOMI_DINO);	return m_CharacterGameInfoForAI + idx; }
	const GAMEINFO_CHARACTER*	(BattleSlotContext::*m_getGameInfo_Chara[CHARA::BUFFER_MAX])(INT32 slot) const;

	GAMEINFO_CHARACTER*		setGameInfo_Chara_Normal( INT32 idx)			{ I3_BOUNDCHK( idx, SLOT_MAX_COUNT);		return m_CharacterGameInfo + idx; }
	GAMEINFO_CHARACTER*		setGameInfo_Chara_AI(INT32 idx)					{ I3_BOUNDCHK( idx, MAX_COUNT_DOMI_DINO);	return m_CharacterGameInfoForAI + idx; }
	GAMEINFO_CHARACTER*		(BattleSlotContext::*m_setGameInfo_Chara[CHARA::BUFFER_MAX])(INT32 slot);


	// [Character Name, State] (C->G)
	SLOT_INFO		m_SlotInfo[ USER_SLOT_COUNT ];					// [initialize at constructor]

	/** \brief �ε��� ����		(C->G) \desc Battle������ ���˴ϴ�. */
	INT32			m_MainSlotIdx = 0;

	/** \brief �ڽ��� �ε��� ��ȣ	(C->G) \desc Battle������ ���˴ϴ�. */
	INT32			m_MySlotIdx = 0;

	i3::rc_wstring	m_wstrNickForSlot[USER_SLOT_COUNT];							// ������ 
	i3::rc_wstring	m_wstrAICharaNickForSlot[MAX_COUNT_DOMI_DINO];				// AI��

	UINT8			m_ui8NickColor[USER_SLOT_COUNT] = { 0 };
	UINT16			m_ui16MuzzleFlashColor[USER_SLOT_COUNT] = { 0 };
	REAL32			m_UseCashItemInfo[USER_SLOT_COUNT][CASHITEMSKILL_MAX] = { 0.0f };

	GAME_ROOM_OPTION roomOption;	// �������� C4��ġ�ð� ,

	INT32 m_killerIdxMostRecent = 0;

public:
	// �ֿܼ� ������� ���� �����Լ��Դϴ�. �ٸ��뵵�� �������������
	void __ConsoleDump__(SLOT_INFO dumpSlot[], i3::rc_wstring dumpNick[])
	{
		memcpy(dumpSlot, m_SlotInfo, sizeof(SLOT_INFO)*USER_SLOT_COUNT);

		for(INT32 i=0; i<USER_SLOT_COUNT;i++)
			dumpNick[i] = m_wstrNickForSlot[i];
	}

};




//////////////////////////////// �� �Լ��� �������ϴ� ////////
inline INT32 		BattleSlotContext::getMySlotIdx( void ) const
{
	ISTEST_S
#if defined( DEF_OBSERVER_MODE)
		return 16;
#else
		return 0;
#endif
	ISTEST_E
	return m_MySlotIdx;
}
