#ifndef __CLAN_CONTEXT_H
#define __CLAN_CONTEXT_H


#include "ClanDef.h"
#include "i3Base//itl/user_data.h"

struct SORT_CLAN_INFO
{
	CLAN_BASIC_INFO 		_info;
	CLAN_BASIC_RECORD		_RecordInfo;

	CLAN_BASIC_SORT_INFO	_SortInfo;
	TTCHAR					_Intro[NET_CLAN_INTRO_SIZE];

	void	CopyFromClanListInfo(CLAN_LIST_BASIC_INFO * pSrc)
	{
		i3mem::Copy( &_info, &pSrc->m_ClanBasicInfo, sizeof(CLAN_BASIC_INFO));
		i3mem::Copy( &_RecordInfo, &pSrc->m_ClanBasicRecord, sizeof(CLAN_BASIC_RECORD));
		i3mem::Copy( &_SortInfo, &pSrc->m_ClanBsicSortInfo, sizeof(CLAN_BASIC_SORT_INFO));
		i3::safe_string_copy(_Intro, pSrc->m_intro, NET_CLAN_INTRO_SIZE);
	}
};


//	����Ʈ���� �ѹ��� �����Ҽ� �ִ� �׸� ����. (����! 256���� ���� �� ����. )
#define MAX_ARRAY_COUNT			256
#define STAFF_AUTHORITY			4

class CClanContext : public i3ElementBase
{
	I3_CLASS_DEFINE( CClanContext, i3ElementBase);

public:
	CClanContext();
	virtual ~CClanContext();

	void EnterServer(void * pArg = nullptr);
	void LeaveServer(void * pArg = nullptr);
	void SetEnter(bool bEnter);
	bool IsEnter(void)			{ return m_bEnter;					}
	bool IsMaster(void);		//	�ڽ��� Ŭ�� ������ ����

	void Create(void);
	void OnGameEvent(GAME_EVENT event,INT32 arg, const i3::user_data & userData);

	void	SetMemberInfoChange( INT64 i64UID, UINT64 ui64State );
	void	SetMemberBasicInfo( CLAN_MEMBER_BASIC_INFO* pMemberBasicInfo );
	

	bool ProcessSecessionFromClan(void);	//	Ż��
	bool ProcessAppointMember(INT64 * pdbIdx, INT32 count);		//	���� �Ӹ�
	bool ProcessDemoteMember(INT64 * pdbIdx, INT32 count);		//	���ܿ��� �Ϲ����� ����
	bool ProcessCommissionMaster(INT64 * pdbIdx, INT32 count);		//	������ ����
	bool ProcessDismissalMember(INT64* pdbIdx, INT32 count);		//	��� ����
	bool ProcessViewMember(INT64 * pdbIdx, INT32 count);			//	���� ���� ����
	bool ProcessIntromissionMember(INT64 * pdbIdx, INT32 count);	//	���� ����
	bool ProcessRejectionMember(INT64 * pdbIdx, INT32 count);		//	���� �ź�
	bool ProcessChangeClanName(INT64 authFlag, const i3::rc_wstring & clan_name);		//	Ŭ���� ����
	bool ProcessChangeClanMark(INT64 authFlag, UINT32 nClanMarkFlag);	//	Ŭ�� ��ũ ���� (������ �Ἥ ����)
	bool ProcessChangeClanMark(UINT32 nClanMarkFlag);	//	Ŭ�� ��ũ ���� (�׳� ����)
	bool ProcessDissolutionClan(void);				//	Ŭ�� ��ü	
	bool ProcessManagementModify(CLAN_MANAGEMENT * pModify);	//	Ŭ�� ��Ÿ ����
	bool SendCreateClan(const i3::wliteral_range& wClanNameRng, const i3::wliteral_range& wIntroRng, const i3::wliteral_range& wURLRng, UINT32 nClanMarkFlag);		//	Ŭ�� ����
	bool SendOverlappingClanMark(UINT32 nClanMarkFlag);			//	Ŭ�� ��ũ �ߺ� üũ
	bool SendOverlappingClanName(const i3::wliteral_range& wTextRng);				//	Ŭ�� �̸� �ߺ� üũ
	bool SendOverlappingClanAddress(const i3::wliteral_range& wTextRng);			//	Ŭ�� �ּ� �ߺ� üũ
	bool SendRequestJoining(UINT32 nClanIdx);					//	Ŭ���� �����ϱ� ���� �������ǿ� �´��� ��û.
	bool SendJoinToClanMember(UINT32 nClanIdx, const i3::rc_wstring& wstrMessage );			//	Ŭ�� ���� 
	bool SendCancelJoinFromClanMember();			//	Ŭ�� ���� ���


	void SendClanInvite();		//	Ŭ�� ���� ����
	void SendClanInviteAccept( UINT32 wClanDBIdx, UINT8 cAccept );// Ŭ�� ���� ����
	void SendClanNote( UINT8 cType, const i3::wliteral_range& wNoteRng );			//	���� ������

	void SendClanDetailInfoNEvent(UINT32 ui32ClanUID, UINT8 uiClanType);

	INT64 * GetIndexBuffer(void)		{ return &m_IndexBuffer[0];			}
	INT64 GetIndexBuffer(INT32 idx)	const	
	{ 
		if (idx < 0 || idx >= MAX_ARRAY_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_ARRAY_COUNT);
			return -1;
		}
		return m_IndexBuffer[idx];		
	}
	INT32 GetIndexBufferCount(void)		{ return m_IndexBufferCount;		}
	void ClearIndexBuffer(void);
	void CopyIndexBuffer(INT64 * pIdxArray, INT32 count);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	�ڽ��� ����
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CLAN_MEMBER_LEVEL_TYPE	GetMyStaffLevel(void);
	INT64 GetMyDBIdx(void);		//	�ڱ� DB �ε���
	bool InBattleTeam(void)						{ return m_bInBattleTeam;			}		//	Ŭ���� ���� �Ҽ��� �ִ��� ����
	void SetInBattleTeam(bool bVal)				{ m_bInBattleTeam = bVal;			}
	void SetInClanBattleLobby(bool bVal)		{ m_bInClanBattleLobby = bVal;		}
	bool GetInClanBattleLobby(void)				{ return m_bInClanBattleLobby;		}
	void SetVisibleLobbyPhase(bool bVal)		{ m_bVisibleLobbyPhase = bVal;		}
	bool GetVisibleLobbyPhase(void)				{ return m_bVisibleLobbyPhase;		}
	bool IsPossibleClanInvite(void);

	//	���� �ڽ��� Ŭ�� �������� �̰����� ���� ����. true�� Ŭ�� ������
	bool IsMember(void)				{ return m_bMember;					}

	bool PossibleChangeClanName(void);	//	Ŭ���� ���� ������ ���� ����
	bool PossibleChangeClanMark(void);	//	Ŭ�� ��ũ ������ ���� ���� ����

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Ŭ�� ����
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CLAN_DETAIL_INFO *	GetMyClanDetailInfo(void);
	CLAN_BASIC_INFO *	GetMyClanBasicInfo(void);	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Ŭ�� ����Ʈ
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void				EnterClanList(bool bReqToServerForList = false);

	bool				UpdateClanList(REAL32 rDeltaSeconds);
	bool				SearchClanListView(const i3::wliteral_range& wKeywordRng);

	void				InitSortClanList(INT32 type = 0);
	void				SortClanList(INT32 type);

	void				SetUpdateClanListAck_BeginClanIdx( INT32 begClanIdx)	{ m_nUpdateClanListAck_BeginClanIdx = begClanIdx; }
	void				SetUpdateClanListAck_CurrRecvClanCount( INT32 nSize)	{ m_nUpdateClanListAck_CurrRecvClanCount = nSize; }
	void				SetUpdatedClanList( bool bUpdate)						{ m_bClanListUpdated = bUpdate; }

	INT32				GetClanSerchRate(void);
	INT32				GetUpdateClanListCount(void)							{ return m_nUpdateClanListCount; }
	UINT32				GetSortClanListCount(void)								{ return m_nSortClanInfoCount; }
	INT32				GetClanListCount(void) const							{ return (INT32)m_sortClanList.size(); }
	SORT_CLAN_INFO *	GetClanList(INT32 idx) const
	{
		I3_BOUNDCHK( idx, (INT32)m_sortClanList.size()); 
		return m_sortClanList[idx];		
	}	

	bool				IsUpdatedClanList( void)									{ return m_bClanListUpdated; }

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Ŭ����
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void EnterClanMember(void);
	void RequestClanMember(void);
	void UpdateClanMember(REAL32 rDeltaSeconds);
	
	//	Ŭ����:GameContext���� �������̽�
	CLAN_MEMBER_CONTEXT * GetMyClanMemberContext(void);
	const CLAN_MEMBER_BASIC_INFO * GetMyClanMember(INT32 idx);
	INT32 GetMyClanMemberCount(void);
	const CLAN_MEMBER_BASIC_INFO * FindMyClanMember(INT64 dbIdx);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	���� �����
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void EnterClanSubscriber(void);
	void RequestClanSubscriber(void);
	void UpdateClanSubscriber(REAL32 rDeltaSeconds);

	//	���� �����:GameContext���� �������̽�
	CLAN_REQUEST_CONTEXT* GetMyClanSubscriberContext(void);
	CLAN_REQUEST_BASIC_INFO* GetMyClanSubscriberList(void);	
	CLAN_REQUEST_BASIC_INFO * GetMyClanSubscriber(INT32 idx);
	CLAN_REQUEST_DETAIL_INFO* GetMyClanSubscriberDetail(void);
	INT32 GetMyClanSubscriberCount(void);	
	void RequsetSubscriberInfo(T_UID dbIdx);

	UINT32 GetJoinClanIdx(void)		{ return m_nJoinClanIdx;		}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	��ġ����ŷ
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	void	BoardNoticeModify( const i3::wliteral_range& wNoticeRng );
	void	BoardGuideModify( const i3::wliteral_range& wGuideRng );
	void	JoinOptionModify( UINT32 ui32State);

	void	MatchTeamList( void );
	void	MatchTeamCreate( UINT8 cPerCount );
	void	MatchTeamJoin( UINT32 wTeamIdx );
	void	MatchTeamLeave( void );
	void	MatchTeamChangeOperation( const i3::rc_wstring & strOperation);
	void	MatchTeamChangeMaxPerson( UINT8 ui8MaxPerson );
	void	MatchTeamRegisterMatchMaking(void);								//	��ġ����ŷ �� ���
	void	MatchTeamCancelRegisterMatchMaking(void);								//	��ġ����ŷ �� ��� ���

	bool	CheckMenagementAuth( CLAN_MEMBER_LEVEL_TYPE eEnumType, CLAN_MENAGE_AUTHORITY_TYPE eMenageType );	// Ŭ�� �ʴ���� Ȯ��

	bool	HasClanCreationInfo()			{return m_HasClanCreationInfo;}
	void	ResetClanCreationInfo()			{ m_HasClanCreationInfo = false; }
	void	SetClanCreationRequirementInfo(UINT8 Rank, UINT32 Point);
	bool	GetClanCreationRequirementInfo(UINT8 * Rank, UINT32 * Point);

	//	���� ���� �˾� �ݹ�
	static void CbSecessionFromClan(void * pThis, INT32 nParam);
	static void CbAppointMember(void* pThis, INT32 nParam);
	static void CbDemoteMember(void* pThis, INT32 nParam);
	static void CbCommissionMaster(void* pThis, INT32 nParam);
	static void CbDismissalMember(void* pThis, INT32 nParam);
	static void CbIntromissionMember(void* pThis, INT32 nParam);
	static void CbRejectionMember(void* pThis, INT32 nParam);
	static void CbDissolutionClan(void* pThis, INT32 nParam);

	// Ŭ�� ���� ��� �� Ŭ�� ����ȭ�� ȣ�� 2011-07-07 ������ �߰�
	static void cbClanCreate(void * pThis, INT32 nParam);
	
	const i3::rc_wstring&	GetClanNameText() const { return m_wstrClanName;}

protected:
	void _ReleaseSortClanList(void);
	bool _CreateSortClanListStorage( INT32 nCreateCount);

	void _DynamicBuildSortClanList(void);

	void _BuildSortClanList(void);
	void _BuildSortClanAppendList( const i3::wliteral_range& wKeywordRng );
	void _BuildSortClanList(const i3::vector<CLAN_LIST_BASIC_INFO*>& InfoList);
	void _BuildSortClanList(CLAN_LIST_BASIC_INFO * pInfo, INT32 count);

	void _BuildSortClanListDummy(void);

	void _BuildSearchClanList(const i3::wliteral_range& wKeywordRng);
	void _BuildSearchClanList(const i3::wliteral_range& wKeywordRng, CLAN_LIST_BASIC_INFO * pInfo, INT32 count);
	void _BuildSearchClanListDummy(const i3::wliteral_range& wKeywordRng);
	
protected:

	bool m_bMember;		//	true�� Ŭ�� �������̰� FLASE�� �̰�����
	bool m_bEnter;		//	Ŭ�� ������ ���� ����
	bool m_bInBattleTeam;	//	Ŭ���� ���� �����ִ��� ���� (�� ��� Ȥ�� ���� ����)
	bool m_bInClanBattleLobby;	//	���� Ŭ���� �κ� �ִ��� ����
	bool m_bVisibleLobbyPhase;	//	�κ񿡼� ����Ʈ �˾��� ���� ����

	////////////////////////////////////////////////////////////	Ŭ�� ����Ʈ	
	REAL32				m_rUpdateClanListTime; 
	INT32				m_nUpdateClanListReq_BeginClanIdx;
	INT32				m_nUpdateClanListAck_BeginClanIdx; 
	INT32				m_nUpdateClanListAck_CurrRecvClanCount;
	INT32				m_nUpdateClanListCount;
	INT32				m_nAppendClanListCount;
	bool				m_bUpdateClanListFinish; 
	bool				m_bNeedUpdateAppendClanList;
	bool				m_bClanListUpdated;

	i3::vector<SORT_CLAN_INFO*> m_sortClanList;	
	i3::vector<SORT_CLAN_INFO*>	m_pSortClanInfo;
	UINT32				m_nSortClanInfoCount;
	i3::wstring			m_wstrCurSearchClan;

	////////////////////////////////////////////////////////////
	// GameContext:ClanMemberList 
	REAL32				m_rUpdateClanMemberTime; 
	UINT8				m_nUpdateClanMemberSection; 
//	bool				m_bListUpdateClanMemberFinish; 

	// GameContext:ClanRequestList
	REAL32				m_rUpdateClanRequestTime; 
	UINT8				m_nUpdateClanRequestSection; 
	bool				m_bListUpdateClanRequestFinish; 

	UINT32		m_nJoinClanIdx;			//	�����Ϸ��� Ŭ�� �ε���

	INT32 m_IndexBufferCount;
	INT64 m_IndexBuffer[MAX_ARRAY_COUNT];

	bool	m_HasClanCreationInfo;
	UINT8	m_ClanCreationRequirementRank;
	UINT32	m_ClanCreationRequirementPoint;

	i3::rc_wstring	m_wstrClanName;

public:
		bool m_bEnableReservePhase;

	//////////////////	Ŭ�� ��ġ ��� // ///////////////
private:
	CLAN_MATCH_RESULT_CONTEXT	m_ClanResultContext;
	CLAN_MATCH_RESULT			m_ClanResultList[MAX_MATCH_RESULT_COUNT];

	INT32						m_i32EmotionReqIndex;	// �̸�� ������ �ε���

	REAL32						m_rMatchListReqTime;		// ��ġ ��� ��û Ÿ��
	UINT8						m_ui8MatchListReqSection;	// ��û�� ������ �ε���
	bool						m_bMatchListReqFinish;		// �������� ���� ��û�ߴ��� �Ǵ�


protected:
	void								_ClearMatchResultList(void);

public:
	//CLAN_MATCH_RESULT_CONTEXT *			SetClanResultContext(void)			{ return &m_ClanResultContext; }
	//const CLAN_MATCH_RESULT_CONTEXT *	GetClanResultContext(void)			{ return &m_ClanResultContext; }
	//INT32								GetClanMatchResultCount(void) const  {return m_ClanResultContext.m_ui8TotalCount; }

	//CLAN_MATCH_RESULT *					SetClanResultList(INT32 idx)		 { return &m_ClanResultList[idx]; }
	//const CLAN_MATCH_RESULT *			GetClanResultList(void)				 { return m_ClanResultList; }
	//const CLAN_MATCH_RESULT *			GetClanResultListData(INT32 idx)	 { return &m_ClanResultList[idx]; }

	//void								SetEmotionSlotIdx(INT32 i32Slot)	 {m_i32EmotionReqIndex = i32Slot;}
	//INT32								GetEmotionSlotIdx(void )			 { return m_i32EmotionReqIndex; }
	
	//void								RequestClanMatchResultList(void);
	//void								UpdateClanMatchRsultList(REAL32 rDeltaSeconds);

	void								ResetMatchResultContext(void);

	//////////////////////////////////////////////////////

private:
	bool	m_bIsClanVisit;

public:
	void	SetClanVisit(bool bVisit)	{ m_bIsClanVisit = bVisit; }
	bool	IsClanVisit( void)			{ return m_bIsClanVisit; }

// Ŭ�� ��� ���� ���濡 ���� ���� ��� Flag
// ( ���Խ��� / ���԰ź� / �������Ӹ� / �������Ӹ� / �Ϲ�ȸ�� ���� / ��������)
private:
	bool	m_bWaitMemberInfoReq;

public:
	bool	isWaitMemberInfoReq( void)			{ return m_bWaitMemberInfoReq; }
	void	SetWaitMemberinfoReq(bool bFlag)	{ m_bWaitMemberInfoReq = bFlag ; }


	/************************************************************************/
	/*								Game Event Func                         */
	/************************************************************************/
private:
	typedef void	(CClanContext::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map<GAME_EVENT, call_func> m_EventFuncMap;

	void			InitGameEventFunc(void);
	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Clan_Enter (INT32 arg, const i3::user_data & );					
	void			ReceiveGameEvent_Clan_Leave( INT32 arg, const i3::user_data & );					
	void			ReceiveGameEvent_Clan_MemberContext( INT32 arg, const i3::user_data & );		
	void			ReceiveGameEvent_Clan_MemberList( INT32 arg, const i3::user_data & );				
	void			ReceiveGameEvent_Clan_RequestContext (INT32 arg, const i3::user_data & );			
	void			ReceiveGameEvent_Clan_AcceptRequest( INT32 arg, const i3::user_data & );			
	void			ReceiveGameEvent_Clan_DenialRequest( INT32 arg, const i3::user_data & );			
	void			ReceiveGameEvent_Clan_Secession( INT32 arg, const i3::user_data & );				
	void			ReceiveGameEvent_Clan_CommissionMaster( INT32 arg, const i3::user_data & );		
	void			ReceiveGameEvent_Clan_CommissionStaff( INT32 arg, const i3::user_data & );		
	void			ReceiveGameEvent_Clan_CommissionRegular	( INT32 arg, const i3::user_data & );	
	void			ReceiveGameEvent_Clan_DismissalMember( INT32 arg, const i3::user_data & );		
	void			ReceiveGameEvent_Clan_ModifyOption( INT32 arg, const i3::user_data & );				
	void			ReceiveGameEvent_Clan_CancelRequest( INT32 arg, const i3::user_data & );			
	void			ReceiveGameEvent_Clan_Dissolution( INT32 arg, const i3::user_data & );			
	void			ReceiveGameEvent_Clan_Management( INT32 arg, const i3::user_data & );				
	
	void			ReceiveGameEvent_ClanWar_CreateTeam( INT32 arg, const i3::user_data & );	
	void			ReceiveGameEvent_ClanWar_JoinTeam( INT32 arg, const i3::user_data & );					
	void			ReceiveGameEvent_ClanWar_LeaveTeam( INT32 arg, const i3::user_data & );
	void			ReceiveGameEvent_ClanWar_ChangeMaxPerson(INT32 arg, const i3::user_data & );
};

#endif	// __CLAN_CONTEXT_H