/**
 * \file	UI\UIMainFrame.h
 *
 * Declares the user interface main frame class.
 */


#if !defined( __UI_MAINFRAME_H__)
#define __UI_MAINFRAME_H__

#include "UIUserActionDef.h"
#include "UIDefine.h"

#include "UIPopupFocus.h"
#include "../UserGuideMgr.h"

class UIBase;
class UIPhaseBase;
class UIPhaseSubBase;
class UIPhaseLogin;
class UIFloatingBase;
class UIPopupBase;
class UIPopupUserActionInteractBase;
class UIToolTipBase;
class UITopMenu;
class UITopMenu_V10;
class UITopMenu_V11;
class UITopMenu_V15;
class UISideMenu;

class ItemImageMgr;
class CAwayChecker;

class CToolTip;
class CRoomMasterKickManager;

#if legacy_gui_operator
// UIMainFrame Ŭ�������� �� GUI �Լ��鸸 ��͵�.
struct gui_operator
{
public:
	gui_operator() : m_pGuiRoot(0) {}

public:
	void	 _LinkControl( void);
	void	 _InitControl( void);
	void _GuiNotifyPump( void);

public:
	void OnLoadEnd();
	void LoadOldGUI( void);
	void Destroy();

public:
	i3GuiRoot* m_pGuiRoot;		// ���� ������ GUI�Դϴ�.
};
#endif

class UIMainFrame : public i3GameNode
{
	I3_CLASS_DEFINE( UIMainFrame, i3GameNode);

	friend struct gui_operator;

protected:
	UITopMenu *			m_pTopMenu;
	UISideMenu *		m_pSideMenu;

	bool				m_bNickSpace;

	i3::vector<UIFloatingBase *>		m_FloatWins;
	i3::vector<UIPopupBase *>			m_Popup;
	i3::vector<UIPhaseBase *>			m_Phase;
	i3::vector<UIPhaseSubBase *>		m_SubPhase;

	i3::vector<INT32>	m_FloatWinQue;					// �� �ѱ����� ���̴µ�...����� �Ⱦ��̴°� ����...(2012.06.01.����)

	// Popup window
	///UIPopupBase *		m_pPopup[ UPW_MAX];

	// Phase
	//UIPhaseBase *	m_pPhases[ UIPHASE_MAX];
	//UIPhaseSubBase *	m_pSubPhases[ USP_MAX];	

	i3::vector<UIPHASE>	m_PhaseQueue;
	PhaseState			m_phaseState;
	UIPHASE				m_currentPhase;
	static UIPHASE		m_lastPhase;

	UISUBPHASE				m_CurrentSubPhase;
	UISUBPHASE				m_SubOldPhase;
	i3::vector<UISUBPHASE>	m_SubPhaseQueue;
	PhaseState				m_SubPhaseState;

	// ��ư ȿ���� Ÿ�̸� ���� ����
	// true�̸� �ð��� ����Ѵ�.
	bool			m_bUIEffectBegin;

	// UI�� Effect ȿ���� ����
	// Enable�Ǹ� Ư�� ��ư�� �����̵��� �Ѵ�.
	bool			m_bUIEffectState;

	REAL32			m_rUIEffectActiveTime;

	bool			m_bClanMember;
	bool			m_bClanNonMember;

	bool			m_bChangeClanPopup;

	UISUBPHASE m_ExclusedEnterEventSubPhase;

	//i3::rc_wstring	m_awstrServerSlotText[ SS_MAINTYPE_MAX];

	// BGM
	UI_BGM			m_CurrentBGM;
	bool			m_bNetload;		///<<< �������� Option�� ������ ����

	// exit timer
	bool				m_bExiting;
	REAL32				m_fExitTimer;
	UIMessageBox*		m_pWaitExitingMessageBox;
	
	void			__CheckAwayInput( void);

	void			__ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData);
	void			__GameEventPump( void);

	void			__UpdatePhase( REAL32 rDeltaSeconds);
	void			__UpdateSubPhase( REAL32 rDeltaSeconds);
	
	virtual void	_EnableFloatingList() { }

	void _SideMenu_Entrance_() const;
	void _SideMenu_Exit_() const;

protected:
	/** \brief Play BGM */
	void			_PlayBGM( UI_BGM bgm);


	//--------------------------------------------------------------------------//
	//								Inline Functions							//
	//--------------------------------------------------------------------------//
public:
	UITopMenu *			GetTopMenu( void)				{ return m_pTopMenu; }
	UITopMenu_V15 *		GetTopMenu_V15( void)			{ return (UITopMenu_V15*)m_pTopMenu; }
	UITopMenu_V11 *		GetTopMenu_V11( void)			{ return (UITopMenu_V11*)m_pTopMenu; }
	UITopMenu_V10 *		GetTopMenu_V10( void)			{ return (UITopMenu_V10*)m_pTopMenu; }
	void				SetUICharaCamera(INT32 cam);
	
	UISideMenu *	GetSideMenu( void)				{ return m_pSideMenu; }

	UIPHASE			GetCurrentPhaseType( void)		{ return m_currentPhase; }
	PhaseState		GetCurrentPhaseState( void)		{ return m_phaseState; }
	UIPhaseBase *	GetCurrentPhase( void)			
	{ 
		if( m_currentPhase > UIPHASE_START && m_currentPhase < UIPHASE_MAX) 
			return GetPhase(m_currentPhase); 
		return nullptr;
	}

	UISUBPHASE		GetCurrentSubPhaseType( void)	{ return m_CurrentSubPhase; }

	// ���� CurrentSubPhase�� SubPhaseQueue�� �� �� ���ų� None�� ��쿡�� None���� �Ǿ� ��.
	bool			IsSubPhaseNone()
	{
		if(m_CurrentSubPhase == USP_NONE && m_SubPhaseQueue.empty() == true)
			return true;
		else
			return false;
	}

	/** \brief ���� SubPhase�� ��ȯ�Ѵ�.
		\note nullptr�� �� �ִ�. */
	UIPhaseSubBase*	GetCurrentSubPhase()
	{
		if( m_CurrentSubPhase != USP_NONE)
		{
			UIPhaseSubBase * pSubPhaseBase = m_SubPhase.at(m_CurrentSubPhase);
			return pSubPhaseBase;
		}
			
		return nullptr;
	}

	PhaseState		GetCurrentSubPhaseState( void)	{ return m_SubPhaseState; }

	UIPhaseBase *	GetPhase( UIPHASE phase)
	{
		I3_BOUNDCHK( phase, UIPHASE_MAX);
		UIPhaseBase * pPhasebase = m_Phase.at(phase);

		return pPhasebase;
	}

	UIPhaseSubBase *	GetSubPhase( UISUBPHASE phase)
	{
		I3_BOUNDCHK( phase, USP_MAX);

		UIPhaseSubBase * pSubPhaseBase = m_SubPhase.at(phase);
		return pSubPhaseBase;
	}

	UIFloatingBase * GetFloating( UIFLOATWIN type);

	UIPopupBase *	GetPopup( UIPOPUPWIN type)
	{
		I3_BOUNDCHK( type, UPW_MAX);
		UIPopupBase * pPopupbase =  m_Popup.at(type);
		return pPopupbase;

	}

	UIPOPUPWIN GetCurrentPopupState();

	//������ popup�� ��ȯ�Ѵ�.
	UIPopupBase*	GetCurrentPopup() const	{	return m_popup_focus.GetLastestPopup();	}

	void			setNetLoad( bool bLoad)				{ m_bNetload = bLoad; }

	void			ChangeClanPopup( void);
	bool			isChangeClanPopup( void)			{ return m_bChangeClanPopup; }

public:
	UIMainFrame();
	virtual ~UIMainFrame();
	static  void	InitItemImageMgr(void);

	virtual void	Create( i3GameNode* pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	
	virtual void	OnLoadEnd(i3Stage* pStage);
	virtual void	OnFinish( void);
	/** \brief Phase�� �����մϴ�.
		\desc Phase ���۽� ������ �ϴ� â�� ������ ���⿡ �߰��Ͻʽÿ�.
			Phase ��ȯ�� BGM�� �����մϴ�. */
	virtual void	StartPhase( UIPHASE MovePhase);
	/** \brief Phase�� �����մϴ�.
		\desc Phase�� �����ҽ� �ݴ� â�� �ִٸ� ���⿡ �߰��Ͻʽÿ�.. */
	virtual void	EndPhase( void) = 0;
	/** \brief Phase�� �����մϴ�.
		\desc ���� Frame���� ����Ǿ���.. */
	virtual void	MovePhase( UIPHASE Phase);
	
	/** \brief SubPhase�� �����ϱ� �� ȣ��
		\desc SubPhase�� ���� �����ϴ� ��������� ��������ݴϴ�. 
		(1.0, 1.1), 1.5 ���� �ൿ�� �ٸ��Ƿ� virtual�� ���ڽ��ϴ�*/
	virtual void	PreStartSubPhase( UISUBPHASE phase) {};
	
	/** \brief SubPhase�� ���� �Ϸ�� ȣ�� 
	    (1.0, 1.1), 1.5 ���� �ൿ�� �ٸ��Ƿ� virtual�� ���ڽ��ϴ�*/
	virtual void	EndSubPhase( UISUBPHASE phase) {};

	/** \brief SubPhase ���Խ� ȣ��
		(1.0, 1.1), 1.5 ���� �ൿ�� �ٸ��Ƿ� virtual�� ���ڽ��ϴ�*/
	virtual void	EntranceSubPhase( void) {};

	/** \brief SubPhase�� ����� ȣ��
		 (1.0, 1.1), 1.5 ���� �ൿ�� �ٸ��Ƿ� virtual�� ���ڽ��ϴ�*/
	virtual void	ExitSubPhase( void) {};

	/** \brief �����ϴ��� ��ġǥ�� */
	virtual void	UpdateLocation( void);


	/** \brief */  // �̰��� Ŭ���� ���������� ���� �ֱ� ������, static�Լ��� �ΰ�, static��������� �����մϴ�. ( 11.10.14. ����)
	static void		SetStartPhase( UIPHASE Phase);

	void			ChangeSubPhase( UISUBPHASE phase);

	
	void			SetExitTimer( bool bSet, float fTime = 3.0f );

	void			GameEventTransferToSubPhase( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData);

	void			GameEventTransferToFloatingWin( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData);

	void			EnableFloatWin( UIFLOATWIN type, void * pArg1 = nullptr, void * pArg2 = nullptr);
	void			DisableFloatWin( UIFLOATWIN type);
	void			DisableFloatWinInput( UIFLOATWIN type,bool bTrue);	
	BOOL			IsEnableFloatWin(UIFLOATWIN type);

	/** \brief Popup Window�� Enable/Disable��Ų��.
		\param[in] bOnTop �ֻ��� Popup ����. true�̸� ������ WindowFrame�� ���� Input�� Disable�ϰ� �˴ϴ�.
		\param[in] pArg1 Argument
		#param[in] pArg2 Argument */
	void			TogglePopup( UIPOPUPWIN popupType, void * pArg1 = nullptr, void * pArg2 = nullptr);

	/** \brief ������ Popup�� ���ϴ�. */
	void			OpenPopup( UIPOPUPWIN popupType, void * pArg1 = nullptr, void * pArg2 = nullptr);

	/** \brief ������ Popup�� �ݽ��ϴ�. */
	void			ClosePopup( UIPOPUPWIN popupType);
	void			CloseAllPopup();

	bool			IsOpenedPopup( UIPOPUPWIN type);
	UIPOPUPWIN		FindPopup( LuaState * L);
	UIPOPUPWIN		FindPopupByPointer( UIPopupBase * pPopup);

	bool			IsPossibleToOpenPopup(void);

	// Popup�� Focus�� �Ͽ� ������ �����쿡 ���� input�� ���´�.
	void	FocusPopup(UIPopupBase* popup, bool bFocus, LPCSTR szFile = nullptr, int nLine = 0);
	bool	IsPopupFocused() const { return m_popup_focus.IsAnyPopupOpened(); }
	bool	IsPopupFocused(UIPopupBase* popup) const { return m_popup_focus.IsActivePopup(popup); }
	bool	IsFocusedTopPopup(UIPopupBase* popup) const { return m_popup_focus.IsTopPopup(popup); }
	void	InputDisableMainUI(bool bEnable);
	void	InputDisableFloatWindow(bool bEnable);

	/** \brief Nick Name ������ Ȯ�� */
	bool			IsCreatingNick( void);

	UIBase *		FindUIBase( LuaState * L);

	void			GameEventTransferToPopup( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData);

	void			GetCurrentSeverInfo( i3::stack_wstring& outServerInfo );

	/** \brief character view�� character�� Ceremony animation�ϵ��� �Ѵ�. */
	void			PlayCeremonyCharacter( void);

	/** \brief Popup�� ���� ������ �ݾ��ְ� false�� ��ȯ
		\desc �Ϻ� SubPhase�� Popup�� ���� �ִ� ��� �ش� â�� �ݰ� false�� ��ȯ�ϰԵ˴ϴ�.
				���� Popup�� true�� ��ȯ�ؾ� ����..*/
	bool			EscapeKey( void);

	/** \brief ���� Popup�� ���ϴ�. */
	void			OnEscape( void);

	// ��ư ȿ��
	void			SetEffectBegin(void) { m_bUIEffectBegin = true;}
	void			SetEffectButtonBegin(void) { m_bUIEffectBegin = true; 	m_rUIEffectActiveTime = 0.f;   }
	void			SetEffectButtonStop(void)  { m_bUIEffectBegin = false; 	m_rUIEffectActiveTime = 0.f;   }
	bool			GetButtonEffect() { return m_bUIEffectState; }

	void			UpdateEffectTime( REAL32 rDeltaSeconds);

	void	 ExclusedEnterSubPhase( UISUBPHASE phase);
	void	 EnterDelayedEventSubPhase(void);
	void	 LeaveSubPhase( UISUBPHASE phase);
	bool	 EventEnterSubPhase(UISUBPHASE phase);

	// Profile ���������� Ȯ��
	bool IsEnteredSubPhase(UISUBPHASE sub);

	void ExclusedEnterPopup( UIPOPUPWIN popup);
	void EnterDelayedEventPopup(void);
	void LeavePopup( UIPOPUPWIN popup);

	/**	������ ���� (����� ���) */
	void			AuthStartSuccess(INT32 arg);

	// Announce Message
	void			SetSlideNotice_Permanant( const i3::rc_wstring& wstrMsg);
	void			SetSlideNotice_Temporary( const i3::rc_wstring& wstrMsg);

	void			StartHighlightMailbox( void);
	void			StartAlarmNote( void);
	
	/** \brief �ʴ��ϱ⿡ ���� Phase �̵��� �Ѵ�. */
	void			OnInviteMove( void);

	/** \brief Clan �ʴ��ϱ⿡ ���� Phase �̵��� �Ѵ�. */
	void			OnInviteMoveClan( void);

	/** \brief Chatting window�� ������Ʈ�Ѵ�. */
	virtual void	UpdateChatBuffer( void) { }
	/** \brief Chatting window�� MegaPhone �� ������Ʈ�Ѵ�. */
	virtual void	UpdateMegaPhoneBuffer(void) { }

	/** */
	bool			isLockedInput( void){ return m_bLockInput; };
	void			SetLockInput( bool Sig){ m_bLockInput = Sig;};

	//--------------------------------------------------------------------------//
	//							V10 / V11	User Action							//
	//--------------------------------------------------------------------------//
	virtual void	ResetUserActionPopup( void) { }
	virtual void	AddUserActionCommand( USER_CONTEXT_MENU cmd) { }
	virtual void	OpenUserActionPopup( void * pEventReceiver, INT32 slotIdx) { }

	/** \brief �ٸ� Control �Է¿� ���� Popup�� �����Ѵ�. 
		\desc Popup�� ���� �ִ� ��� ���� �Է��� �ƴ϶�� �� popup�� �ݴ´�. */
	void			ClosePopupByAnotherControlInput( i3UIControl * pControl);

	bool			IsPhaseAndSubPhaseSlideDone();
	//--------------------------------------------------------------------------//
	//								Away Checker								//
	//--------------------------------------------------------------------------//
private:
	CRoomMasterKickManager* m_pRoomMasterAwayCheckerManager;

	CAwayChecker*	m_pAwayChecker;
	UIMessageBox*	m_pAwayMessageBox;

//	List			m_CurrPopupList;
	UIPopupFocus	m_popup_focus;
		

	bool			m_bProcessLeaveRoom;

	UISUBPHASE		m_oldAwayPhase;

	bool			m_bKickByNoStartGame;		// ������ ��� ���� ������ ���� ����� ���
	bool			m_bLockInput;
	REAL32			m_rLockInputTimer;			// Phase Lock �� �ð� ���� �Է� ����

	bool			m_bDeprotationClanChannel;  // ä�θ���Ʈ���� ������ ���� ����.
	i3::rc_wstring  m_sDeprotationClanMessage;	// ä�θ���Ʈ���� ������ ���� ����.

public:
	CRoomMasterKickManager*	GetRoomMasterAwayCheckerManager() { return m_pRoomMasterAwayCheckerManager; }

	void			OpenAwayMessageBox(const i3::rc_wstring& wstrMessage);
	void			CloseAwayMessageBox();
	
	void			setProcessLeaveRoom(const bool bFlag)		{ m_bProcessLeaveRoom = bFlag; if( bFlag){m_oldAwayPhase = m_CurrentSubPhase;}}
	void			setKickBtNoStartGame( bool bHost)			{ m_bKickByNoStartGame = bHost; }
	bool			getDeprotationClanChannel()					{ return m_bDeprotationClanChannel; }
	void			setDeprotationClanChannel(const bool bFlag, const i3::rc_wstring& message) 
					{
						m_bDeprotationClanChannel = bFlag; 
						m_sDeprotationClanMessage = message;
					}

private:
#if legacy_gui_operator
	gui_operator m_gui_operator; //�� gui
#endif

#if legacy_not_use_operator
	// ���콺 �� ��
	REAL32			m_rWheelDelta;
	void			__ProcessWheel( void);
	void			__CheckCheating( void); // �޸� ���� üũ
#endif


};

#endif

