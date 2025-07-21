/**
 * \file	UI\UIPhaseReadyRoom.h
 *
 * Declares the user interface phase ready room class.
 */


#if !defined( __UI_PHASE_READY_ROOM_H__)
#define __UI_PHASE_READY_ROOM_H__

#include "UIPhaseBase.h"
#include "room_detail.h"
#include "UIDefine.h"

#include "CSI_Rule.h"

#define		PRIZE_SLOT_VIEW_COUNT	3

//
enum READYROOM_BTN_STATE
{
	RBS_UNKNOWN = -1,
	RBS_SEARCH_HOST,		// ���� �˻� ��ư
	RBS_INVITE,				// �ʴ� ��ư
	RBS_ROOMSET,			// �� ���� ��ư

	RBS_TEAMCHANGE,			// �� ��ü ��ư
	RBS_READY,				// �����ư
	RBS_START,				// ���� ���� ��ư

	RBS_MAX
};

enum UI_READYSTATE
{
	UIRS_NONE = -1,
	UIRS_SETTING,
	UIRS_READY,
	UIRS_PLAY,
};

enum UI_READYSTATE_V10
{
	UIRS_V10_NONE = -1,
	UIRS_V10_READY,
	UIRS_V10_PLAY,
	UIRS_V10_SHOP,
	UIRS_V10_INVEN,
	UIRS_V10_INFO,
	UIRS_V10_CLAN,
};


enum READY_ROOM_TYPE
{
	ROOM_T_NORMAL = 0,
	ROOM_T_DOMI,

	ROOM_T_MAX
};

enum COMBO_TYPE { 
	COMBO_RULE = 0,			// ���� ��
	COMBO_STAGE,			// ��������
	COMBO_ROUND,			// �¸�����(ų/����)
	COMBO_TIME,				// �¸�����(���� �ð�)
	COMBO_TEAM_CHANGE,		// ��������
	COMBO_AI_LEVEL,			// AI��� ���̵�
	COMBO_AI_COUNT,			// AI��� �ο���
	COMBO_MAX
};

class CAwayChecker;

struct CBBoxData
{
	i3::vector<INT32>	i32Idx;
	i3::vector<INT32>	_value;
	i3::vector<UINT8>	_type;	// Round, Time �� ����ؾ� �� ��쿡 ����� ��

	CBBoxData(void)
	{
		i32Idx.clear();
		_value.clear();
		_type.clear();
	}


	// set�Լ��� Add�Լ��� ����� �����մϴ�
	void SetData(INT32 idx, INT32 value)
	{
		if( idx < (INT32)i32Idx.size() )
		{
			i32Idx[idx] = idx;
			_value[idx] = value;
		}
		else
		{
			i32Idx.push_back(idx);
			_value.push_back(value);
		}
	}
	void SetData_subType(INT32 idx, UINT8 value)
	{
		if(  idx < (INT32)i32Idx.size()  )
		{
			i32Idx[idx] = idx;
			_type[idx] = value;
		}
		else
		{
			i32Idx.push_back(idx);
			_type.push_back(value);
		}
	}

	void ClearData()
	{
		_value.clear();
		_type.clear();
		i32Idx.clear();
	}

	INT32 GetData(INT32 idx)
	{
		return _value[idx];
	}

	UINT8 GetData_subType(INT32 idx)
	{
		return _type[idx];
	}
};

class UIPhaseReadyRoom : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseReadyRoom, UIPhaseBase);

	friend class room_base;
	friend class room_normal;
	friend class room_domi;
	friend class room_normal_V10;

protected:
	room_base*			m_room_detail[ROOM_T_MAX];
	room_base*			m_curr_room;

	CBBoxData			m_cb_Detail[COMBO_MAX];
	i3UIComboBox*		m_cbBox[COMBO_MAX];
	i3UIButton*			m_btnRoomSet;

	i3UIFrameWnd*		m_hostFmw;
	char				pw[NET_ROOM_PW];
	i3UIEditBox*		m_pPwEditBox;
	i3UIEditBox*		m_pTitleEditBox;
	i3::rc_wstring		m_strTitle;

	void				SetRoomTitle();

	void				SetLuaPasswordOpt();							// �� ��� �ɼ� ���� ��� ����
public:
	READY_ROOM_TYPE		get_room_type( STAGE_ID e);
	void				change_room_detail( STAGE_ID e);
	room_base*			get_room_detail( READY_ROOM_TYPE e) { I3_BOUNDCHK( e, ROOM_T_MAX); return m_room_detail[e];}
	room_base*			getCurrRoom()	{	return m_curr_room;	}

protected:
	UIMessageBox* 		m_pBeHostMessageBox;
	INT32				m_transferHostSlotIdx;
	REAL32				m_rSearchHostDelay;
	INT32				m_OldHostIndex;				///<< ���� ���� Index �̰Ͱ� ���� Index�� �ٸ��� ���� ���� �޼����� ǥ���Ѵ�.	

	REAL32				m_rInviteLobbyUserTimer;

	i3SoundPlayInfo*	m_pCountdownSound[COUNTDOWN_COUNT];

	UIMessageBox* 		m_pWaitInviteMB;
	i3UICheckBox *		m_pGMObserver;

	INT32				m_nSelectedSlotIdx;
	i3UIButton *		m_pBtnReady;
	i3UIButton *		m_pBtnStart;

	bool 				m_bInviteMessage;
	bool 				m_bInviteResult;
	bool 				m_bEnableUpdate;
	bool				m_bReadyEnable;
	bool				m_bQueJumpToBattle;	// ��Ʋ ���� ���� (�ƹ�Ÿ �ε��� �Ϸ� �Ǹ� �����Ѵ�)
	bool	 			m_bIsHost;
	bool 				m_bKickingState;		// ���� �� �ľ� �ߺ� ȣ�� �������� �÷��� - ű���� true, LOBBY_ENTER����� false
	bool				m_bLeaveTeamState;		// �뺴�� ��Ʋ�� ��ġ�� �� ��, ������ ����뿡�� ������ �߿� �� ���� ������Ʈ�� ���� ���� �÷���

	bool				m_bGiveupBattle; // Ŭ���� �ߵ� ���� ó����.

	enum{ UserListMaxFrm = 6 }; //PointBlankRe_ReadyRoom_TeamSelect + PBRe_Domi_TeamSelect �� ���Ǵ� ��� FrameWnd ��
	i3UIFrameWnd *		m_pMainWnd[UserListMaxFrm];

	i3UIFrameWnd *		m_pTitleWnd;
	UINT8				m_SubType;
	//i3::rc_wstring		m_aawstrServerText[ SS_MAINTYPE_MAX][ SS_SUBTYPE_COUNT];

	CSI_RULE			m_CurStageRule;
	STAGE_ID			m_nCurStageID;
	INT32				m_nCurComboIdx[COMBO_MAX];	// �޺��ڽ� ����. ���� ��ġ�� ���� Index

	// Util
	ROOM_INFO_ALL		GetCurRoonInfo();			// �ʹ� ���� ���Ǿ� �߰���
	void				Departured();

protected:
	void			_SetButtonTooltip( void);
		
	/** \brief ���� �˻� */
	void			_DoSearchHost( void);

	/** \brief �ʴ��ϱ� */
	void 			_DoInviteFromLobby(void);

	/** \brief EVENT_BATTLE_COUNTDOWN ó�� */
	void			_EventProcess_BattlCountDown( INT32 arg, const i3::user_data&);

	void			_Process_Normal_BattleCountDown(INT32 arg);
	void			_Process_ClanMatch_BattleCountDown(INT32 arg);

	void 			_UpdateMapInfo( ROOM_INFO_ALL * pInfo);
	void 			_UpdateTitlePassword(bool bInit = false);
	void 			_DelayInviteResponse(REAL32 rDeltaSeconds);//for OnUpdate function for Invite list updating
	void 			OnInvitedMessage(void);

	/** \brief �� ���� ���� ó�� */
	void 			_ModifyRoomOption( INT32, const i3::user_data&);
	void			_ProcessInviteLobbyUser( REAL32 rDeltaSeconds);

	/** \breief SubType �ʱⰪ �޾ƿ��� */
	void			_GetMissionRoundType(UINT8& subtype, STAGE_MODE mode);
	/** \breief AiMode�� ����뿡�� �ٲ�ٸ� ����� �ϴ� ó��*/
	void			_SetAiModeConditon(STAGE_ID stageid);

	//--------------------------------------------------------------------------//
	//							inline functions								//
	//--------------------------------------------------------------------------//
public:
	INT32			getSelectedSlot( void)		{ return m_nSelectedSlotIdx; }

	
public:
	UIPhaseReadyRoom();
	virtual ~UIPhaseReadyRoom();

	void 			RunRandomMap(void);
	void 			AllUserReady();
	static void 	PopupTabSkill(void * pThis, INT32 nParam);


//-------------------------------------------------------------------------
//���� ready room �۷� �Լ� room_normal �� ��쿡�� ȣ�� ��.
	void			ClickSlot( INT32 idx);
	void			ClickRSlot( INT32 idx);
	void			ClickCloseButton( INT32 idx);

//-------------------------------------------------------------------------
//���� ready room �۷� �Լ� room_domi �� ���� ���
	void			ClickButton( INT32 BtnIdx);

	// Open Popup
	// ������� CashItem�� ���� �˸�
	static void		OpenPopupUsingCashItem();

	void			CheckTSReward();
	void			CheckQuickMedalReward(void);
	void			Apply_Password();
	
	bool			IsRoomUser( const char* pNick) { return m_curr_room->is_room_user( pNick);}	

	void			AdjustRandomMapData(ROOM_INFO_ALL &Info ) const;

	//--------------------------------------------------------------------------//
	//								Error handler								//
	//--------------------------------------------------------------------------//
public:
	void 			EhKicked(bool isGM);

	//--------------------------------------------------------------------------//
	//							Callback rutine									//
	//--------------------------------------------------------------------------//
	static void 	CbBeHost(void* pThis,INT32 nParam);
	static void 	CbSearchHost(void* pThis,INT32 nParam);
	static void 	cbQueryLeaveClanBattle(void * pThis, INT32 nParam);
	static void 	CbMoveChannel(void* pThis,INT32 nParam);
	static void 	CbBlockUser(void* pThis, INT32 nParam);
	static void 	CbFinalComplateConfirm(void* pThis,INT32 nParam);
	static void 	CbMoveShop(void* pThis, INT32 nParam);
	static void		CbDomiEnter(void* pThis, INT32 nParam);

	//-----------------------------------------------------------------------------//
	//							Kick RoomMaster

	//Room Master GameStart Timer After All User Ready
private:
	bool			m_bBeHostMessageOK;
	
private:
	void			 _GetUserInfo(INT32);

public:
	void			Set_bGameStart(bool);
	void			DecreaseRemainCount() { m_iRemainCount -= 1; SetRemainCount(m_iRemainCount); }
	void			StartCount(INT32);
	void			SetRemainCount(INT32);
	
	bool			OnInviteMove( void);

	const bool		IsBattleCountDown(void) { return (m_iRemainCount > 0) || (m_bGameStart == true); }

private:
	bool			m_bGameStart;
	REAL32			m_rRemainTime;
	INT32			m_iRemainCount;

public:
	//bool			_IsAlone();											//m_curr_room->is_alone()
	virtual void			JoinGame(void) { m_curr_room->do_ready_game();}


	//GM Observer ���
	void			_InitGMObserverEnable( void);
	void			_UpdateGMObserverControlState(void);	
	void			ChangeGMObserver(bool bChecked);


private:
	typedef void	(UIPhaseReadyRoom::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveEvent_Ready_Battle( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Start_Game( INT32 result, const i3::user_data&);
	void			ReceiveEvent_GetLobbyUserInfo( INT32 result, const i3::user_data&);
	void			ReceiveEvent_EnterLobby( INT32 result, const i3::user_data&);
	void			ReceiveEvent_KickUser( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Unready_4vs4( INT32, const i3::user_data&);
	void			ReceiveEvent_GetRoomAddInfo( INT32, const i3::user_data&);
	void			ReceiveEvent_DestroyNetwork( INT32, const i3::user_data&);
	void			ReceiveEvent_SearchHost( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Request_HostChange( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Transfer_Host( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Item_Auth( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Item_Auth_Data( INT32 result, const i3::user_data&);
	void			ReceiveEvent_User_Slot( INT32, const i3::user_data&);
	void			ReceiveEvent_Get_UserEquipmentInfo( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Change_ReadyRoomInfo( INT32 result, const i3::user_data& UserData);
	void			ReceiveEvent_Enter_Domination( INT32, const i3::user_data&);
	void			ReceiveEvent_RoomScore( INT32, const i3::user_data&);
	void			ReceiveEvent_Giveup_Battle( INT32, const i3::user_data&);
	void			ReceiveEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveEvent_PCCafeChangedStatus( INT32 arg, const i3::user_data&);
	void			ReceiveEvent_ForceLeaveMatchroom(INT32 arg, const i3::user_data&);
	void			ReceiveEvent_Get_UserEquipmentInfo_V10( INT32 result, const i3::user_data&);

public:
	virtual void		OnCreate( i3GameNode * pParent) override;
	virtual void		_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void		_ClearAtUnload( i3UIScene * pScene) override;
	virtual bool		OnRevive( i3RenderContext * pCtx) override;

	virtual void		ProcessGameEvent( GAME_EVENT evt,INT32 arg, const i3::user_data& UserData) override;
	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void		OnGotoBackPhase( void) override;

	virtual bool		OnKey_Escape( void) override;
	virtual bool		OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void		OnEntranceEnd( void) override;
	virtual bool		OnExitStart( void) override;
	virtual void		OnExitEnd( void) override;

	virtual bool		OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code) override;
	virtual bool		OnPlayClickSound(i3UIControl * pCtrl) override;

	virtual void		SetFrmWndPos();							// Frame window ����
	virtual void		UpdateCBBoxInfo(COMBO_TYPE BoxType);

	virtual void		ApplyLua_cbbox(COMBO_TYPE _cType, const i3::wliteral_range& wstrCondition, INT32 _cel);
	virtual void		ApplyLua_auto_team_change_enable(UINT32 stageID);
	virtual void		ChangeGameMode(INT32 _mode, INT32 _celidx);

	virtual void		SetLuaText();
	virtual void 		CheckBeHost(void){};
};

#endif
