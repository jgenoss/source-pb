/**
 * \file	UI\UIPhaseLobby_V11_V11.h
 *
 * Declares the user interface phase lobby class.
 */


#if !defined( __UI_PHASE_LOBBY_V11_H__)
#define __UI_PHASE_LOBBY_V11_H__

#include "UI/UIPhaseBase.h"
#include "UI/UIDefine.h"
#include "UI/UIPhaseLobby.h"
#include "UI/UIMainFrame.h"


class UIPopupNickNameSetting;
//class UIToolTipLobbyMapInfo;
class UILoadingIcon;


enum UI_V11_LOBBY_BUTTON
{
	ULB__V11_NONE = -1,
	ULB__V11_TUTORIAL,		// Ʃ�丮��
	ULB__V11_QUICK,			// ������
	ULB__V11_REFRESH,		// ���ΰ�ħ
	ULB__V11_CREATEROOM,		// ����� ��ư
	ULB__V11_ENTERROOM,		// ���尡�� ��ư
	ULB__V11_CLANMATCH,		// Ŭ�� ��ġ

	ULB__V11_MAX
};


class UIPhaseLobby_V11 : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseLobby_V11, UIPhaseBase);

private:
	REAL32			ButtonTermTime;
	bool			m_bAllowRefresh;
	bool			m_bWaitingFromServer;

	i3::vector<LOBBY_ROOM_INFO*> m_RoomList;
	REAL32			m_rEnterLobbyWaitTime;

	// Room List �޴��� message
	//UIMessageBox*	m_pWaitRoomListMassageBox;
	UILoadingIcon *	m_pWaitRecevingRoomList;
	UIMessageBox*	m_pKickedMassageBox;

	// Ȯ��� ����� �˾�
	UINT32			m_nStageID;
	UINT8 			m_SubType;
	UINT8 			m_LockWeapon;
	UINT8 			m_LockInterEnter;
	UINT8 			m_LockObserver;
	UINT8 			m_LockTeamBalance;

	// �ʴ� ����
	INVITE_INFO		m_InviteInfo;
	bool			m_bCheckInvite;

	bool			m_bEnableCreateRoomMsgBox;
	bool			m_bEnableUsingClanChannel;

	INT32			m_SelectedElement;	// Room Index
	bool			m_bQueRoomInfo;		// RoomInfo â�� ť�׵Ǹ� true,
	bool			m_bRefreshButtonActive;	// �������ù�ư�� ������, ������������� true
	REAL32			m_fRefreshElapsedTime;

	bool			m_bFirstLogin;
	bool			m_bLogin;

	bool			m_bRequestRoomInfoAdd;
	bool			m_bNickNameCreate;
	REAL32			m_LastRefreshTime;

	// �κ� �� ���� �����Դϴ�.
	//UIToolTipLobbyMapInfo *	m_pTTMapInfo;

	// �����ϴ� ���� ����
	ROOM_INFO_BASIC	m_JoinTargetRoom;

	// ���¡ ����
	UIMessageBox *	m_pAbusingBox;
	REAL32			m_rAbuseTime;

	i3UIButton *		m_pBtnTutorial;
	i3UIButton *		m_pBtnTeamLIst;
	i3UIListView_Box*	m_pItemListBox;
	bool			m_bTutorialEnable;
	bool m_is_sort_room_list; //������ ���� ������ �ް�, �� ���� ���� ����

	CLAN_MEMBER_LEVEL_TYPE m_PreClanMemberLevel; // ���� Ŭ����� ����

	// �� ��� ��ũ��
	i3UIScrollBar * m_pScrollRoomList;

	// ���� ���� or �Ұ��� �� ������ ���� ���� ����
	bool			m_bViewAdmissionRoom;			

	static SORT_ORDER_INFO sm_SortOrder[MAX_SORT_TYPE];
	static INT32	sm_nCurSortType;
	static bool		sm_bAwayInput;
	static bool		m_bPcCafeNotice;				// PcCafe ���� â�� �����ִ����� ���� ����
	static bool		m_bEventCardSetNotice;
	static bool		m_bEvnetNotice;

	void			__AutoRefreshRoomList( REAL32 rDeltaSeconds);
	void			__ProcessAbusing( REAL32 rDeltaSeconds);

protected:
	void			_SetButtonState( void);
	void			_SetCurSelectItem(void);

	// Room List
	void			_BuildRoomListDummy(void);
	void			_UpdateRoomList(void); // Room Update: AutoRefresh
	void			_SortRoomList(void); // Room Sort: Refresh
	void			_EmptyRoomList(void);
	void			_WaitConnect(REAL32 rDeltaSeconds);
	void			_CloseWaitMessageBox(void);

	void			_ProcessFirstLogin( void);
	void			_ProcessCheckInvite(REAL32 rDeltaSeconds);

	void			_ProcessCommon( REAL32 rDeltaSeconds);

	void			_InitRoomList(void);
	void			_SetRoomList( void);
	void			_SetRoomInfo( INT32 idx, INT32 RoomIdx, ROOM_INFO_BASIC* pInfo);
	void			_SetEmptyRoomInfo( INT32 idx);
	void			_SetRoomColor( INT32 RoomIdx, I3COLOR * pColor);
	void			_RestoreRoomColor(INT32 RoomIdx);

	void			_FirstLoginPopup( void);

	void			_CloseAllSubPhaseAndPopup( void);

	void			_ClearRoomInfo( void);

	void			_SetUserFilterCallback( bool bEnter);
	static void		UserFilterCallback(i3UIControl * pThis);
	void			ChangeUserFilterPosition(i3UIListView_Box* pThis);

	void			_ClearRoomList( INT32 count);

	// SubPhase-> Phase ��ȯ�� Msg Box�� Floating Window���� �ϴܿ� �ֱ⶧����
	// ������ ��ġ�� �������ݴϴ�.
	void			_AddMsgBoxToTopMenuScreen(void);

	//------------------------------------------------------------------------------//
	//								Inline Functions								//
	//------------------------------------------------------------------------------//
public:
	// �����ϴ� Room�� ����
	const ROOM_INFO_BASIC &	GetJoinTargetRoom()	{ return m_JoinTargetRoom; }

public:
	UIPhaseLobby_V11();
	virtual ~UIPhaseLobby_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	OnUpdate_NoneRsc(REAL32 rDeltaSeconds) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	// ���� ����
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;

	// ������
	virtual bool	OnEntrancing( REAL32 rDeltaSeconds) override;

	// ���� �Ϸ�
	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnExitStart( void) override;

	virtual void	OnGotoBackPhase( void) override;

	virtual bool	OnDblClick( i3UIControl * pControl) override;

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;

	void			ClickButton( INT32 BtnIdx);

	/** \brief Tutorial����*/
	void			EnterTutorial( void);

	void			EnterAIMode(void);

	// �� ����� ���
	void			OnCreateRoomCancel( void);

	// �濡 ����
	void			EnterRoom( INT32 idxRoom);

	void			SendEventQuickJoin( void);

	// select room
	void			OnClickRoom( INT32 idxRoom);

	// NickName ���� Popup�� ����.
	void			ShowNicknamePopup( void);

	// PcCafe�� Popup�� ����.
	void			ShowPcCafePopup(void);

	// EventItem���޿� Popup�� ����.
	void			EventItemPopup(void);

	// Event �⼮üũ
	void			ShowAttendancePopup(void);

	// ���� ���� â�� ����.
	void			ShowNoticePopup(void);

	static bool		CompareRoom( const LOBBY_ROOM_INFO* pItem1, const LOBBY_ROOM_INFO* pItem2);
	static bool		CompareAdmissionRoom(const LOBBY_ROOM_INFO* pItem1, const LOBBY_ROOM_INFO* pItem2);

	bool			OnInviteMove( void);

	void			SelectRoom( INT32 idx);

	void			RefreshSelectedRoom( void);

	void			OnRefreshButton();

	// �� ��� ��ũ��
	void			OnScrollRoomList(void);
	void			OnWheelRoomList(INT32 zDelta);
	INT32			GetRoomListScrollPos(void)	{ return m_pScrollRoomList->getScrollPos(); }

	void			SortRoomList(INT32 nType , INT32 bAscend);
	void			SortAdmissionRoom(bool bViewAdmissionRoom);

	/** \brief AbusePopup���� OK�ϸ� ȣ�� */
	void			ConfirmAbuseOK( void);

	void			DisableRoomTooltip(void);

	void			EnableAllowRefresh(bool enable) { m_bAllowRefresh = enable; }

	//------------------------------------------------------------------------------//
	//								Error Handler									//
	//------------------------------------------------------------------------------//
public:
	void			EhWrongQuickJoin(void);
	void			EhSlotFull(void);
	void			EhKickedByVote(void);
	void			EhWrongPW(void);
	void			EhNotExist(void);
	void			EhLimitInterenter(void);
	void			EhLimitLever(void);
	void			EhLimitTraining(void);
	void			EhLimitMERC(void);
	void			EhStageError(INT32 Event,INT32 Arg);
	void			TutorialPopup( void);

	static void		CbSameNick(void* pThis,INT32 nParam);
	static void		CbBeInvited(void* pThis,INT32 nParam);
	static void		CbSuppliedConfirm(void* pThis,INT32 nParam);
	static void		CbEventCardSetConfirm(void* pThis,INT32 nParam);
	static void		CbClanInvite(void* pThis, INT32 nParam);
	static void		CbBlockUser(void* pThis, INT32 nParam);
	static void     CbTutorialMode(void* pThis, INT32 nParam);
	static void		CbAIMode(void* pThis, INT32 nParam);
	static void		CbKickedByVote( void * pThis, INT32 nParam);
	// Key : �⼮ üũ �߰�
	static void		CbShowAttendancePopup(void * pThis, INT32 nParam);
	static void		CbOnGotoBackPhase(void * pThis, INT32 nParam);

	/// ���� UI
private:
	bool			m_bEnableNicknameMsgBox;

public:
	void			CreateNickName();
	void			ResetSelectedElement()
	{
		m_SelectedElement = -1;
		GetMainFrame()->EnableFloatWin( UFW_LOBBYBANNER);
	}


private:
	typedef void	(UIPhaseLobby_V11::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Ready_Battle( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Join_Quick( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Create_Room( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Join_Room( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Lobby( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Get_RoomAddInfo( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_DestroyedNetwork( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Update_UserList( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Ready_Box_Message( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Attendance_Clear( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Create_NickName( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Create_Chara( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Temp( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Temp( INT32 arg, const i3::user_data&);
};

#endif

