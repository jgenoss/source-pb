#ifndef _CLAN_STRUCT_DEF_H
#define _CLAN_STRUCT_DEF_H

// Ŭ�� �� ����
typedef struct PACKET_CLAN_WAR_MATCH_TEAM_LIST_ACK		CLAN_MATCH_TEAM_LIST_DATA;	// �� Ŭ�� ��ġ �� ����Ʈ ����
struct CLAN_MATCH_TEAM_INFO
{
	T_TeamID		_TMatchTeamID;		// ��ġ�� ID
	INT8			_i8ClanTeamIdx;		// Ŭ�� �� Idx
	TEAM_STATE		_TeamState;			// �� ����
	UINT8			_ui8MaxPerson;		// �ִ� �ο���
	UINT8 			_ui8CurPerson;		// ���� �ο���	

	CLAN_MATCH_TEAM_INFO(void)
	{
		Reset();
	}

	CLAN_MATCH_TEAM_INFO(const MATCH_TEAM_INFO & Info )
	{
		ParseData(Info);
	}

	void				ParseData(const MATCH_TEAM_INFO & Info);
	void				Reset(void);

	void				setMaxPerson(const UINT8 value)	{ _ui8MaxPerson = value; }

	const T_TeamID		getMatchTeamID(void)	const	{ return _TMatchTeamID; }
	const INT8			getClanTeamIdx(void)	const	{ return _i8ClanTeamIdx; }
	const TEAM_STATE	getTeamState(void)		const	{ return _TeamState; }
	const UINT8			getMaxPerson(void)		const	{ return _ui8MaxPerson; }
	const UINT8			getCurPerson(void)		const	{ return _ui8CurPerson; }

	const bool			isSameState(const TEAM_STATE state)		const	{ return _TeamState == state; }
	const bool			isMaxPerson(void)						const	{ return _ui8CurPerson == _ui8MaxPerson; }
};

typedef i3::vector<CLAN_MATCH_TEAM_INFO>	ClanMatchTeamList;

// Ŭ�� ��Ī ����
typedef struct PACKET_CLAN_WAR_MATCH_TEAM_COUNT_ACK		CLAN_ALL_MATCHING_DATA;	// �� ���� Ŭ�� ��ġ �� ī��Ʈ

struct CLAN_ALL_MATCHING_INFO
{
	INT32	_i32MatchCompleteTeamCount;		// ��Ī �� ��� �� ���� (���� & ��Ʋ��)
	INT32	_i32MatchWaitngTeamCount;		// ��Ī ������� ��� �� ���� (��Ī ��ư ���� ��)

	CLAN_ALL_MATCHING_INFO(void)
	{
		Reset();
	}

	void			ParseData(const CLAN_ALL_MATCHING_DATA & Data);
	void			Reset(void);

	const INT32		getMatchCompleteTeamCount(void)					const	{ return _i32MatchCompleteTeamCount; }
	const INT32		getMatchWaitTeamCount(void)						const	{ return _i32MatchWaitngTeamCount; }
};

// Ŭ�� ��ġ �� ����
struct CLAN_MATCH_ROOM_INFO
{
	i3::rc_wstring	_wstrRoomName;		// �� �̸�
	UINT8			_ui8RoomIdx;		// �� �ε���
	bool			_ActivateSlot;		// ���� Ȱ�� ����

	CLAN_MATCH_ROOM_INFO(void)
	{	
		Reset();
	}

	CLAN_MATCH_ROOM_INFO( const CLAN_MATCH_TEAM_INFO & Info)
	{
		ParseData(Info);
	}

	void					ParseData(const CLAN_MATCH_TEAM_INFO & Info);
	void					Reset(void);
	void					setActivateSlot(const bool bFlag)	{ _ActivateSlot = bFlag; }

	const i3::rc_wstring &	getRoomName(void)			const	{ return _wstrRoomName; }
	const UINT8				getRoomIdx(void)			const	{ return _ui8RoomIdx; }
	const bool				isActivateSlot(void)		const	{ return _ActivateSlot; }
};

typedef i3::vector<CLAN_MATCH_ROOM_INFO>	ClanMatchRoomList;

// Ŭ���� �д� ��� ����
struct CLAN_MATCH_SQUAD_MEMBER
{
	T_UID			_ui16UID;
	MEMBER_TYPE		_MemberType;
	INT8			_i8SlotIdx;
	UINT8			_ui8Rank;
	UINT8			_ui8NickColor;
	i3::rc_wstring	_wstrNickName;
	UINT8			_ui8MerClass;		// �뺴 �ֹ��� ����. WEAPON_CLASS_TYPE

	CLAN_MATCH_SQUAD_MEMBER(void)
	{
		Reset();
	}

	void					ParseData(const MATCH_MEMBER_INFO & Info);
	void					Reset(void);

	const i3::rc_wstring &	getNickName(void)			const { return _wstrNickName; }
	const T_UID				getUID(void)				const { return _ui16UID; }
	const MEMBER_TYPE		getMemberType(void)			const { return _MemberType; }
	const INT8				getSlotIdx(void)			const { return _i8SlotIdx; }
	const UINT8				getRank(void)				const { return _ui8Rank; }
	const UINT8				getNickColor(void)			const { return _ui8NickColor; }
	const UINT8				getMerClass(void)			const { return _ui8MerClass; }

	const bool				isEmptySlot(void)			const	{ return getUID() == 0; }
};

// Ŭ���� �д� ����.
typedef struct PACKET_CLAN_WAR_CREATE_TEAM_ACK			CLAN_WAR_CREATE_TEAM_DATA;
typedef struct PACKET_CLAN_WAR_JOIN_TEAM_ACK			CLAN_WAR_JOIN_TEAM_DATA;
typedef struct PACKET_CLAN_WAR_LEAVE_TEAM_ACK			CLAN_WAR_LEAVE_TEAM_DATA;
typedef struct PACKET_CLAN_WAR_CHANGE_OPERATION_ACK		CLAN_WAR_CHANGE_OPERATION_DATA;
typedef struct PACKET_CLAN_WAR_CHANGE_MAX_PER_ACK		CLAN_WAR_CHANGE_MAX_PERSON_DATA;

struct CLAN_MATCH_SQUAD_INFO : CLAN_MATCH_TEAM_INFO
{
	INT8					_i8LeaderSlot;
	INT8					_i8MySlot;
	i3::rc_wstring			_wstrSquadName;
	i3::rc_wstring			_wstrOperation;
	CLAN_MATCH_SQUAD_MEMBER	_Member[MAX_CLAN_MATCH_MEMBER];

	CLAN_MATCH_SQUAD_INFO(void)
	{
		Reset();
	}

	void							ParseData(const CLAN_WAR_CREATE_TEAM_DATA & Data, const MATCH_MEMBER_INFO & Member);
	void							ParseData(const CLAN_WAR_JOIN_TEAM_DATA & Data);
	void							Reset(void);
	
	void							setLeaderSlot(const INT8 idx)			{ _i8LeaderSlot = idx; }
	void							setOperation(const i3::rc_wstring wstr)	{ _wstrOperation = wstr; }
	void							resetMember(const INT8 idx)				{ _Member[idx].Reset(); }

	const INT8						getLeaderSlot(void)				const	{ return _i8LeaderSlot; }
	const INT8						getMySlot(void)					const	{ return _i8MySlot; }
	const i3::rc_wstring &			getSquadName(void)				const	{ return _wstrSquadName; }
	const i3::rc_wstring &			getOperation(void)				const	{ return _wstrOperation; }
	const CLAN_MATCH_SQUAD_MEMBER &	getMember(const UINT8 idx)		const	{ return _Member[idx]; }

	const bool						isJoinTeam(void)				const	{ return getTeamState() != TEAM_STATE_NONE; }
	const bool						isTeamLeader(const INT8 idx)	const	{ return (isJoinTeam() && getLeaderSlot() == idx); }	
	const bool						isMySlot(const INT8 slot)		const	{ return getMySlot() == slot; }
};

typedef struct PACKET_D_CS_NEW_DETAIL_INFO					CLAN_DETAIL_INFO_ALL;		// Ŭ�� �� ����

struct CLAN_MATCH_BATTLE_TEAM
{
	i3::rc_wstring		_wstrClanName;
	UINT32				_ui32ClanMark;
	T_TeamID			_TMatchTeamID;		// ��ġ�� ID

	CLAN_MATCH_BATTLE_TEAM(void)
	{
		Reset();
	}

	void							ParseData(const CLAN_MATCHING_TEAM_INFO & Info);
	void							Reset(void);

	const i3::rc_wstring &			getClanName(void)				const	{ return _wstrClanName; }
	const UINT32					getClanMark(void)				const	{ return _ui32ClanMark; }
	const T_TeamID					getMatchTeamID(void)			const	{ return _TMatchTeamID; }
};

typedef struct PACKET_CLAN_WAR_MATCH_TEAM_INFO_ACK			CLAN_BATTLE_TEAM_DATA;		// Ŭ�� ��Ʋ �� ����

/************************************************************************/
/*								  �뺴 ����								*/
/************************************************************************/
// Ŭ�� �뺴 ����
typedef struct PACKET_CLAN_WAR_MERCENARY_LIST_ACK		CLAN_WAR_MERCENARY_LIST_DATA;
struct CLAN_MERCENARY_INFO
{
	T_MerID				_TMercenaryID;		// �뺴 ID
	MERCENARY_STATE		_State;				// �뺴 ����
	WEAPON_CLASS_TYPE	_ClassType;			// �ֹ��� ����
	UINT8				_ui8MerRank;		// �뺴 ���
	UINT8				_ui8NickColor;		// �г��� �÷�
	i3::rc_wstring		_wstrNickName;		// �г���

	CLAN_MERCENARY_INFO(void)
	{
		Reset();
	}

	CLAN_MERCENARY_INFO(const MERCENARY_INFO & Info )
	{
		ParseData(Info);
	}

	void						Reset(void);
	void						ParseData(const MERCENARY_INFO & Info);

	const T_MerID				getMecenaryID(void)	const	{ return _TMercenaryID; }
	const MERCENARY_STATE		getState(void)		const	{ return _State; } 
	const WEAPON_CLASS_TYPE		getClassType(void)	const	{ return _ClassType; }
	const UINT8					getRank(void)		const	{ return _ui8MerRank; }
	const UINT8					getNickColor(void)	const	{ return _ui8NickColor; }
	const i3::rc_wstring &		getNickName(void)	const	{ return _wstrNickName; }
};

typedef i3::vector<CLAN_MERCENARY_INFO>	ClanMercenaryList;

// Ŭ�� �뺴 ����
// CLAN_MERCENARY_RECOED ���� CLAN_MERCENARY_RECORD ���� ��Ÿ ����
typedef struct PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK		CLAN_WAR_MERCENARY_RECORD_DATA;
struct CLAN_MERCENARY_RECORD
{
	INT32			_i32MatchCount;
	INT32			_i32WinMatchCnt;
	INT32			_i32LoseMatchCnt;
	INT32			_i32DisconMatchCnt;
	INT32			_i32KillCount;
	INT32			_i32DeathCount;
	INT32			_i32Headshot;
	INT32			_i32AssistCount;

	CLAN_MERCENARY_RECORD(void)
	{
			Reset();
	}

	CLAN_MERCENARY_RECORD(const MERCENARY_RECORD & Info)
	{
		ParseData(Info);
	}

	void			Reset();
	void			ParseData(const MERCENARY_RECORD & Info);

	INT32			getMatchCount(void)		const	{	return _i32MatchCount;	}
	INT32			getWinMatchCnt(void)	const	{	return _i32WinMatchCnt;	}
	INT32			getLoseMatchCnt(void)	const	{	return _i32LoseMatchCnt;	}
	INT32			getDisconMatchCnt(void)	const	{	return _i32DisconMatchCnt;	}
	INT32			getKillCount(void)		const	{	return _i32KillCount;	}
	INT32			getDeathCount(void)		const	{	return _i32DeathCount;	}
	INT32			getHeadshotCount(void)	const	{	return _i32Headshot; }
	INT32			getAssistCount(void)	const	{	return _i32AssistCount; }
};

struct CLAN_PERSONAL_MATCH_RECORD
{
	INT8	_i8RoomSlotIdx;					// �� ���� �ε���. ��ġ�� ���� �ε��� �ƴ�. �����̸� ���� ���� ����.
	UINT32	_ui32Killcount;					// ų
	UINT32	_ui32Death;						// ����
	UINT32	_ui32Win;						// ��
	UINT32	_ui32Lose;						// ��

	CLAN_PERSONAL_MATCH_RECORD(void)
	{
		Reset();
	}

	CLAN_PERSONAL_MATCH_RECORD(const MATCHMAKING_RECORD & info)
	{
		ParseData(info);
	}

	void			Reset();
	void			ParseData(const MATCHMAKING_RECORD & info);

	INT8			getRoomSlotIdx(void)	const	{	return _i8RoomSlotIdx;  }
	UINT32			getKillCount(void)		const	{	return _ui32Killcount;	}
	UINT32			getDeathCount(void)		const	{	return _ui32Death;		}
	UINT32			getWinCount(void)		const	{	return _ui32Win;		}
	UINT32			getLoseCount(void)		const	{	return _ui32Lose;		}

};

#endif // _CLAN_STRUCT_DEF_H