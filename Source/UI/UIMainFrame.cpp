#include "pch.h"
#include "UIMainFrame.h"

#include "UIPhase.h"
#include "UIFloating.h"
#include "UI_V10/UIFloating_V10.h"
#include "UI_V11/UIFloating_V11.h"
#include "UIPopup.h"
#include "UIToolTip.h"

#include "UIServerAndChannelBanner.h"
#include "UINewAndHotBanner.h"
#include "UILobbyBanner.h"
#include "UILobbyClanInfo.h"
#include "UIPopupMedalRestriction.h"

#include "UITopMenu.h"
#include "UISideMenu.h"
#include "UICharaView.h"

#include "../AwayChecker.h"
#include "../RoomMasterKickManager.h"

#include "ItemImageMgr.h"
#include "PurchaseMgr.h"
#include "MyRoomInfoContext.h"
#include "ChannelContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#define VRAM_SIZE_WIDTH 2048
#define VRAM_SIZE_HEIGHT 2048

I3_CLASS_INSTANCE( UIMainFrame);//, i3GameNode);

namespace
{

class UIPopupFocusCallback_MainFrame : public UIPopupFocusCallback
{
public:
	virtual void	ProcessInputDisableMainUI(bool bEnable)
	{
		g_pFramework->GetUIMainframe()->InputDisableMainUI(bEnable);
	}
	virtual void ProcessInputDisableFloatWindow(bool bEnable)
	{
		g_pFramework->GetUIMainframe()->InputDisableFloatWindow(bEnable);
	}
} g_popup_focus_cb;


	UIMainFrame* g_this = 0;
}


UIPHASE UIMainFrame::m_lastPhase = UIPHASE_START;

UIMainFrame::UIMainFrame(): m_pTopMenu(nullptr), m_pSideMenu(nullptr)
	, m_bNickSpace(false), m_phaseState(PS_START), m_SubPhaseState(PS_START), m_currentPhase(UIPHASE_START)
	, m_CurrentSubPhase(USP_NONE), m_SubOldPhase(USP_NONE)
{
	g_this = this;

	m_popup_focus.SetCallback(&g_popup_focus_cb);

	m_Phase.clear();
	m_SubPhase.clear();
	m_FloatWins.clear();
	m_Popup.clear();

	m_bUIEffectState = false;
	m_bUIEffectBegin = false;
	m_rUIEffectActiveTime = 0.f;

	m_pAwayChecker = nullptr;
	m_pAwayMessageBox = nullptr;
	m_bProcessLeaveRoom = false;
	m_oldAwayPhase = USP_NONE;

	m_bClanMember = false;
	m_bClanNonMember = false;

	m_bChangeClanPopup = false;

	m_ExclusedEnterEventSubPhase = USP_NONE;

	m_CurrentBGM = UI_BGM_NONE;
	m_bNetload = false;
	m_bLockInput = false;

	m_rLockInputTimer = 0.0f;

	m_bExiting = false;
	m_fExitTimer = 0;
	m_pWaitExitingMessageBox = nullptr;

	m_pRoomMasterAwayCheckerManager = nullptr;

	m_bDeprotationClanChannel = false;

	// Create CharaView Instance
	CharaViewMgr::i()->CreateCharaView();

	// ä�� ����Ʈ Text�� �̸� �ҷ��ɴϴ�.
	ChannelContext::i()->LoadServerTextFromFile();
}

UIMainFrame::~UIMainFrame()
{
	m_Phase.clear();
	m_SubPhase.clear();
	m_FloatWins.clear();
	m_Popup.clear();

	// Release CharaView Instance
	CharaViewMgr::i()->ReleaseCharaView();
}

/*virtual*/ void UIMainFrame::OnFinish( void)
{
	g_pFramework->SetFocus(nullptr);

	// �Ʒ� �޴������� ���� �����Ǿ� �Ѵ�.
	if( g_pSndMng != nullptr)
		g_pSndMng->LeaveStage();

	if( g_pCharaManager != nullptr)
		g_pCharaManager->LeaveStage();

	if( g_pWeaponManager != nullptr)
		g_pWeaponManager->LeaveStage();

	// Key : User Guide System
	{
		UserGuideMgr::i()->RemoveUserGuideList();
	}

	I3_SAFE_RELEASE( m_pAwayChecker);

	I3_SAFE_RELEASE( m_pRoomMasterAwayCheckerManager);

	GameUI::ReleaseLoadingIcon();

	i3VirtualTexture* pVTex = i3UI::getVirtualTexture();
	pVTex->getCache()->TrashAllTexture();

#if legacy_gui_operator
	m_gui_operator.Destroy();
#endif

}
void UIMainFrame::__CheckAwayInput(void)
{
	switch( m_currentPhase)
	{
	case UIPHASE_SERVER_CHANNEL_LIST:	// ���� ���� �� Reset
		// Empty
		break;
	case UIPHASE_READY_ROOM:		// �Է��� ������ Restart
		// �Է��� ������ Restart�ε� ���������� ������ �� �̹� Reset�� �ع�����
		// Restart ���ڸ��� ������ �Ǵ� ����. SubPhase�� ���� ������ �����ϵ��� ����. Jinsik(2014.11.27)
		if (m_CurrentSubPhase == USP_NONE && g_pFramework->BeInput(true))
		{
			m_pAwayChecker->Restart();
			if (m_pAwayMessageBox) 
			{
				GameUI::CloseMsgBox(m_pAwayMessageBox);
				m_pAwayMessageBox = nullptr;
			}
		}
		break;
	default:
		// null;
		break;
	}
}

void UIMainFrame::__ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	if( evt == EVENT_SEND_NOTE)
	{
		switch( arg)
		{
		case EVENT_ERROR_SUCCESS:					/*���� ���޿� �����Ͽ����ϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_SUCCESS_SEND_MESSAGE"));
			break;
		case EVENT_ERROR_NOTE_SEND_UKNOWN_NICK:		/*�г����� ����Ǿ��ų� �������� �ʴ� ������Դϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_NOT_FIND_USER"));
			break;
		case EVENT_ERROR_NOTE_SEND_BOX_FULL:		/*������ �������� ������ ���� �� �����ϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_SEND_MESSAGE_RECEIVER_FULL"));
			break;
		case EVENT_ERROR_NOTE_SEND_DAILY_LIMIT:		/*�Ϸ翡 ���� �� �ִ� ������ ���� �ʰ��Ͽ����ϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FULL_SEND_MESSAGE_PER_DAY"));
			break;
		case EVENT_ERROR_NOTE_SEND_SELF:			/*�ڽſ��� ������ ���� �� �����ϴ�*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_DONT_SEND_MYSELF_MESSAGE"));
			break;			
		case EVENT_ERROR_FAIL:
		default:
			/*���� ���޿� �����Ͽ����ϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_NETWORK_FAIL_SEND_MESSAGE"));
			break;
		}
	}
}

void UIMainFrame::__GameEventPump(void)
{
	GAME_EVENT evt;
	INT32 arg;

	// ���� Phase�� NULL�̸� �۵����� �ʴ´�
	if (nullptr == GetCurrentPhase())
		return;

	
	
	// �߻��� GameContext �̺�Ʈ�� �޾Ƴ��� �����Ѵ�
	
	i3::user_data		UserData;

	while(	GameEventReceiver::i()->PopRecvData(evt, arg, UserData) )	//	i = 0; i < count; i++)
	{
		if( GetCurrentPhase() != nullptr)
			GetCurrentPhase()->ProcessGameEvent( evt, arg, UserData);

		GameEventTransferToSubPhase( evt, arg, UserData);

		// Floating Window�� �̺�Ʈ�� �ش�.
		GameEventTransferToFloatingWin( evt, arg, UserData);

		// Key : User Guide System
		{
			UserGuideMgr::i()->ProcessGameEvent(evt, arg, UserData);
		}

		// Popup window�� �̺�Ʈ�� �ش�.
		GameEventTransferToPopup( evt, arg, UserData);

		PurchaseMgr::i()->ProcessGameEvent( evt, arg, UserData);

		if(m_pSideMenu != nullptr)
			m_pSideMenu->ProcessGameEvent( evt, arg, UserData);

		if(m_pTopMenu != nullptr)
			m_pTopMenu->ProcessGameEvent( evt, arg, UserData);

		__ProcessGameEvent( evt, arg, UserData);
	}
}

void UIMainFrame::__UpdatePhase( REAL32 rDeltaSeconds)
{
	if( m_bLockInput == true)
	{
		m_rLockInputTimer += rDeltaSeconds;
		if( m_rLockInputTimer > 3.f)
		{
			m_bLockInput = false;
			m_rLockInputTimer = 0.0f;
		}
	}

	// phase					// MustBeFix
	switch( m_phaseState)
	{
	case PS_IDLE:
		if( m_PhaseQueue.size() > 0)
		{
			UIPHASE MovePhase = m_PhaseQueue[0];
			i3::vu::erase_index(m_PhaseQueue, 0);

			StartPhase(MovePhase);

			m_phaseState = PS_ENTRANCE_START;

			i3UI::setDisableUIInput(true);
		}
		break;
	case PS_ENTRANCE_START:
		if( GetCurrentPhase()->OnEntranceStart())
		{
			// ���� 1.0���� ����մϴ�.
			// �� ��ġ���� Floating�� �մϴ�.
			_EnableFloatingList();

			m_phaseState = PS_ENTRANCING;

			if (m_pAwayMessageBox)
			{
				GameUI::CloseMsgBox(m_pAwayMessageBox);
				m_pAwayMessageBox = nullptr;
				
				if( m_bKickByNoStartGame)
				{
					m_bKickByNoStartGame = false;

					/*���ѽð� ���� ������ �������� �ʾ�\n���� ����Ǿ����ϴ�.*/
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_READYROOM_NOTICE_GET_AWAY_COMPLETE"));
				}
				else
				{
					/*��ð� �������� ����\n�κ�� �̵��Ǿ����ϴ�.*/
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READYROOM_NOTICE_GET_AWAY_COMPLETE"));
				}
			}
		}

		break;
	case PS_ENTRANCING:
		if (GetCurrentPhase()->GetSlideState() == SS_IN_OK)
		{
			m_phaseState = PS_ENTRANCE_END;
		}
		break;
	case PS_ENTRANCE_END:

		i3UI::setDisableUIInput(false);

		m_bLockInput = false;
		m_rLockInputTimer = 0.0f;

		m_phaseState = PS_PLAYING;

		// �ڸ���� �˻� ����
		switch(m_currentPhase)
		{
		case UIPHASE_SERVER_CHANNEL_LIST:
			m_pAwayChecker->SetTime(AWAY_INPUT_SERVER_TIME, AWAY_INPUT_SERVER_CAUTION);
			break;
		case UIPHASE_READY_ROOM:
			// ���Ǿ� �ñ��
			// �տ��� �� �ð�, �ڿ����� 5���� �̵��մϴٿ��� 5������ ������ֱ�
			// ��) m_pAwayChecker->SetTime(10, AWAY_INPUT_ROOM_CAUTION);
			m_pAwayChecker->SetTime(AWAY_INPUT_ROOM_TIME, AWAY_INPUT_ROOM_CAUTION);
			break;
		}

		break;
	case PS_PLAYING:
		// Update SubPhase
		{

		__UpdateSubPhase( rDeltaSeconds);

		if( m_PhaseQueue.size() > 0 )
		{
			m_phaseState = PS_EXIT_START;
		}

#if legacy_gui_operator
		m_gui_operator._GuiNotifyPump();
#endif

		// GameEventPump
		SLIDESTATE phase_slide_state = GetCurrentPhase()->GetSlideState(); 
		
		SLIDESTATE sp_slide_state = SS_OUT_OK;

		if ( m_CurrentSubPhase != USP_NONE)
		{
			sp_slide_state = GetCurrentSubPhase()->GetSlideState();
		}

		//if  ( (m_SubPhaseState == PS_PLAYING  || m_SubPhaseState == PS_IDLE) &&
		if  ( (sp_slide_state == SS_IN_OK || sp_slide_state == SS_OUT_OK ) &&
			( phase_slide_state == SS_IN_OK || phase_slide_state == SS_OUT_OK ) )
			__GameEventPump(); 

		if (m_bProcessLeaveRoom)
		{
			if( (USP_NONE == m_oldAwayPhase) ||
				(false == GetSubPhase(m_oldAwayPhase)->isEnable()) )
			{
				GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
				m_bProcessLeaveRoom = false;
			}
			else
			{
				ChangeSubPhase(USP_NONE);
			}
		}
		else
		{
			// �ڸ���� �˻�
			__CheckAwayInput();
		}
#if legacy_not_use_operator
		__ProcessWheel();
#endif
		}
		break;

	case PS_EXIT_START:
		{
			i3UI::setDisableUIInput(true);

			EndPhase();
			
			m_phaseState = PS_EXITING;

			m_pAwayChecker->Reset();
			m_bProcessLeaveRoom			= false;
			m_bDeprotationClanChannel	= false;
			m_sDeprotationClanMessage.clear();
		}
		break;
	case PS_EXITING:
		if (GetCurrentPhase()->GetSlideState() == SS_OUT_OK)
		{
			m_phaseState = PS_EXIT_END;
		}
		break;
	case PS_EXIT_END:

		if( m_PhaseQueue.size() == 0)			// ���� ������ ����ñ����� ��ǲ�� Ǯ�� �ʴ´�..
		{
			i3UI::setDisableUIInput(false);
		}

		m_phaseState = PS_IDLE;
		break;
	}
}

bool	UIMainFrame::IsPhaseAndSubPhaseSlideDone()
{
	if (m_currentPhase == UIPHASE_START) 
		return true;

	if ( m_PhaseQueue.size() > 0 )
		return false;

	SLIDESTATE pss = GetCurrentPhase()->GetSlideState(); 
	if (pss == SS_IN_OK ) return true;

	SLIDESTATE spss = SS_OUT_OK;

	if ( m_CurrentSubPhase != USP_NONE)
		spss = GetCurrentSubPhase()->GetSlideState();
	
	if ( spss == SS_IN_OK ) return true;

	return false;
}


void UIMainFrame::__UpdateSubPhase( REAL32 rDeltaSeconds)
{
	switch( m_SubPhaseState)
	{
	case PS_IDLE:
		if( m_SubPhaseQueue.size() > 0)
		{
			UISUBPHASE subPhase = m_SubPhaseQueue[0];
			i3::vu::erase_index(m_SubPhaseQueue, 0);

			PreStartSubPhase( subPhase);

			if( subPhase != USP_NONE)
			{
				i3UI::setDisableUIInput( true);
				m_SubPhaseState = PS_ENTRANCE_START;
			}
		}
		break;

	case PS_ENTRANCE_START:
		if( GetCurrentPhase()->isSliding() == false)
		{
			EntranceSubPhase();

			// ReadyRoom�϶� ��ð� ��Ⱑ �����µ� ������ �ʰ� �ϱ� ����.
			// ������ �ʱ�ȭ ��Ų��. Jinsik(2014.11.27)
			if(m_currentPhase == UIPHASE_READY_ROOM && m_pAwayChecker != nullptr)
			{
				m_pAwayChecker->Reset();
			}

			m_SubPhaseState = PS_ENTRANCING;
		}
		break;

	case PS_ENTRANCING:
		if( GetCurrentSubPhase()->isSliding() == false)
		{
			m_SubPhaseState = PS_ENTRANCE_END;
		}
		break;

	case PS_ENTRANCE_END:
		i3UI::setDisableUIInput( false);
		m_SubPhaseState = PS_PLAYING;
		m_bLockInput = false;
		m_rLockInputTimer = 0.0f;
		break;

	case PS_PLAYING:
		// Update SubPhase
		if( m_SubPhaseQueue.size() > 0)
		{
			m_SubPhaseState = PS_EXIT_START;
		}
		break;

	case PS_EXIT_START:
		{
			i3UI::setDisableUIInput(true);
			ExitSubPhase();
			GetCurrentSubPhase()->OnExitStart();
			m_SubPhaseState = PS_EXITING;

			// ReadyRoom�϶� ��ð� ��Ⱑ �����µ� ������ �ʰ� �ϱ� ����.
			// ����뿡 �����ϹǷ� �ٽ� �ʱ�ȭ ��Ų��. Jinsik(2014.11.27)
			if(m_pAwayChecker != nullptr)
			{
				if(m_currentPhase == UIPHASE_READY_ROOM)
				{
					m_pAwayChecker->SetTime(AWAY_INPUT_ROOM_TIME, AWAY_INPUT_ROOM_CAUTION);
				}
			}

		}
		break;

	case PS_EXITING:
		if( GetCurrentSubPhase()->GetSlideState() == SS_OUT_OK)
		{
			m_SubPhaseState = PS_EXIT_END;

			UISUBPHASE subPhase = m_SubPhaseQueue[0];

			//���°� ������ �ʴ� ����� �̸� �����Ѵ�.
			if( m_SubPhaseQueue.size() > 0)
			{
				i3::vu::erase_index(m_SubPhaseQueue, 0);

				PreStartSubPhase( subPhase);

				if( subPhase != USP_NONE)
				{
					i3UI::setDisableUIInput( true);
					m_SubPhaseState = PS_ENTRANCE_START;
				}
			}


			EndSubPhase( subPhase);	
		}
		break;

	case PS_EXIT_END:
		{
			i3UI::setDisableUIInput( false);
			m_SubPhaseState = PS_IDLE;
		}
		break;
	}
}


void UIMainFrame::FocusPopup(UIPopupBase* popup, bool bFocus, LPCSTR szFile /*= NULL*/, int nLine /*= 0*/)
{
	if (bFocus)
	{
		m_popup_focus.AddFocusPopup(popup);
		OnEvent( UIEVENT_SET_FOCUS_POPUP, this, 0, I3_EVT_CODE_ACTIVATE);
	}
	else
	{
		m_popup_focus.RemoveFocusPopup(popup);
		OnEvent( UIEVENT_KILL_FOCUS_POPUP, this, 0, I3_EVT_CODE_ACTIVATE);
	}
	
#if defined( I3_DEBUG)
	if( szFile != nullptr)
		I3TRACE( "%s (%d) : Focus Popup %s (%d)\n", szFile, nLine, popup->meta()->class_name(), bFocus?1:0);
#endif
}


void	UIMainFrame::InputDisableMainUI(bool bEnable)
{
	if( m_pTopMenu != nullptr)
		m_pTopMenu->SetInputDisable( bEnable);

	if( m_pSideMenu != nullptr )
		m_pSideMenu->SetInputDisable( bEnable);
	
	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);
		if( pFloatWin != nullptr)
			pFloatWin->SetInputDisable( bEnable);
	}

	if( GetCurrentPhase() != nullptr)
		GetCurrentPhase()->SetInputDisable( bEnable);

	if( m_CurrentSubPhase != USP_NONE && GetCurrentSubPhase() != nullptr)
		GetCurrentSubPhase()->SetInputDisable( bEnable);
}

void	UIMainFrame::InputDisableFloatWindow(bool bEnable)
{
	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);
		if( pFloatWin != nullptr)
			pFloatWin->SetInputDisable( bEnable);
	}
}

void UIMainFrame::_PlayBGM( UI_BGM bgm)
{
	if( (m_CurrentBGM != bgm) &&
		g_pSndMng != nullptr)
	{
		m_CurrentBGM = bgm;

		g_pSndMng->StopBGM( 0.5f);
		if( m_CurrentBGM == UI_BGM_0)
			g_pSndMng->StartBGM( "Sound/UI/PB_15ver_BGM_1.ogg", true, true, 1.f);

		//::InitSound();
	}
}

/*virtual*/void UIMainFrame::StartPhase( UIPHASE MovePhase)
{
	//ChannelType channeltype;
	_PlayBGM( UI_BGM_0);
	UIPhaseBase * pPhase = GetPhase(MovePhase);
	if ( INT32(MovePhase) > 0 && pPhase != nullptr)
	{
		pPhase->OnNotifyPrevPhase( m_currentPhase );
	}

	m_currentPhase = MovePhase;
}

/*virtual*/ void UIMainFrame::Create( i3GameNode* pParent)
{
	i3GameNode::Create(pParent);

	if( g_pFramework->GetViewer()->GetPhysixContext()->isValidScene() == false)
	{
		g_pFramework->GetViewer()->GetPhysixContext()->CreateScene( NX_TIMESTEP_VARIABLE);
		g_pFramework->InitializePhysXCollisionGroup();
	}

	i3UI::ResizeVRAMCache( VRAM_SIZE_WIDTH, VRAM_SIZE_HEIGHT);

	// GUI�� ��������쿡�� ������ ����Ű���� Menu�Դϴ�.
	g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);
}

/*virtual*/ void UIMainFrame::OnLoadEnd(i3Stage* pStage)
{
	I3ASSERT( g_pSndMng != nullptr);
	g_pSndMng->EnterStage( pStage);

	g_pCharaManager->EnterStage( pStage);
	g_pWeaponManager->EnterStage( pStage);

	// AwayChecker ����
	m_pAwayChecker = CAwayChecker::new_object();
	m_pAwayChecker->SetReceiver(pStage);
	m_pAwayChecker->Reset();
	I3_GAMENODE_ADDCHILD(pStage, m_pAwayChecker);

	m_pRoomMasterAwayCheckerManager = CRoomMasterKickManager::new_object();
	m_pRoomMasterAwayCheckerManager->ResetAwayChecker();
	I3_GAMENODE_ADDCHILD(pStage, m_pRoomMasterAwayCheckerManager);

	// Battle ��� ���ƿö� ģ���ʴ� �޽����� �ʱ�ȭ�Ѵ�
	CCommunityContext::i()->SetRecivedInvite(false);

#if legacy_gui_operator
	m_gui_operator.OnLoadEnd();
#endif
}

/*virtual*/ void UIMainFrame::OnUpdate( REAL32 rDeltaSeconds)
{
	PurchaseMgr::i()->OnUpdate(rDeltaSeconds);

	if( true == m_bExiting )
	{
		m_fExitTimer -= rDeltaSeconds;

		if( m_fExitTimer <= 0.0f )
		{
			GameUI::CloseMsgBox( m_pWaitExitingMessageBox );
			m_pWaitExitingMessageBox = nullptr;

			GOTO_EXIT_STAGE();
			m_bExiting = false;
		}
	}

	UpdateEffectTime(rDeltaSeconds);	// ButtonEffect ����

#if legacy_gui_operator
	__CheckCheating();
#endif

	__UpdatePhase( rDeltaSeconds);

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnUpdate(rDeltaSeconds);
	}
	
	i3GameNode::OnUpdate( rDeltaSeconds);
}

/*virtual*/ bool UIMainFrame::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( m_currentPhase == UIPHASE_SERVER_CHANNEL_LIST)
		{
			switch(event)
			{
			case ACE_AWAY:		GOTO_EXIT_STAGE();	return true;
			//case ACE_AWAY:	GameEventSender::i()->SetEvent( EVENT_GAME_END ); return TRUE; // 2012.03.10 ���ؿ�. �α׾ƿ� ��û ��Ŷ ����
				/*5�� �̳��� ������ �������� ������ ������ ����˴ϴ�.*/
			case ACE_CAUTION:	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READYROOM_WARNING_SELECT_SERVER"));	return true;
			}
		}

		switch( event)
		{
		case UIEVENT_OPEN_POPUP :
			{
				UIEVENT_OPEN_POPUP_INFO * pInfo = (UIEVENT_OPEN_POPUP_INFO*) param2;

				if( pInfo->_Type > UPW_NULL && pInfo->_Type < UPW_MAX)
				{
					OpenPopup( pInfo->_Type, pInfo->_pParam1, pInfo->_pParam2);
				}
			}
			break;
		case UIEVENT_CLOSE_POPUP :
			{
				UIEVENT_OPEN_POPUP_INFO * pInfo = (UIEVENT_OPEN_POPUP_INFO*) param2;

				if( pInfo->_Type > UPW_NULL && pInfo->_Type < UPW_MAX)
				{
					ClosePopup( pInfo->_Type);
				}
			}
			break;
		}

		
	}

	if( m_pTopMenu != nullptr)
	{
		if( m_pTopMenu->OnEvent( event, pObj, param2, code))
			return true;
	}

	if( m_pSideMenu != nullptr)
	{
		if( m_pSideMenu->OnEvent( event, pObj, param2, code))
			return true;
	}

	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);
		if( pFloatWin && pFloatWin->isEnable())
		{
			if( pFloatWin->OnEvent( event, pObj, param2, code))
				return true;
		}
	}

	for(size_t i = 0; i < m_Popup.size(); i++)
	{
		UIPopupBase * pPopupbase =  m_Popup.at(i);
		if( pPopupbase != nullptr && pPopupbase->isEnable())
		{
			if( pPopupbase->OnEvent( event, pObj, param2, code))
				return true;
		}
	}

	if( m_CurrentSubPhase != USP_NONE)
	{
		I3ASSERT( GetCurrentSubPhase() != nullptr);
		if( GetCurrentSubPhase()->OnEvent( event, pObj, param2, code))
			return true;
	}

	if( GetCurrentPhase() != nullptr)
	{
		if( GetCurrentPhase()->OnEvent( event, pObj, param2, code))
			return true;
	}

	return i3GameNode::OnEvent( event, pObj, param2, code);
}

/*virtual*/void UIMainFrame::MovePhase( UIPHASE Phase)
{
	if( m_currentPhase != Phase)
	{
		if ( m_CurrentSubPhase == USP_NONE)
		{
			I3TRACE("m_CurrentSubPhase == USP_NONE\n");
			CClanContext * pClanContext = g_pFramework->GetClanContext();
			m_PhaseQueue.push_back(Phase);
			pClanContext->SetVisibleLobbyPhase(true);
			//		pClanContext->SetInClanBattleLobby(FALSE);				// �ش� Ŭ���� �̺�Ʈ�� ���� �����ϵ��� �ؼ� ���⼭ ���ܽ�ŵ�ϴ�.(11.07.25.����)
		}
		else
		{
			I3TRACE("m_CurrentSubPhase != USP_NONE\n");

			// ��������� �����Ѵٸ� MovePhaseó���� ���� �ʵ��� �մϴ�..
			// ��������� ������, ������ ��������� �� �������ΰ�ó�� ���Ը� �մϴ�.
			// ���� ������Ŷ ���ؿ��� disable_input�� �Ǿ��� ���ɼ��� ����� ���� ������,
			// disable_input�� ���⼭ ������ Ǯ�� �ۿ� �����ϴ�.
			
			UIPhaseBase * tempPhase = GetPhase(Phase);
			if ( INT32(Phase) > 0 && tempPhase != nullptr)
				tempPhase->OnNotifyPrevPhase( m_currentPhase );
			
			m_currentPhase = Phase;

			for(size_t i = 0; i < m_FloatWins.size(); i++)
			{
				UIFloatingBase * pFloatWin = m_FloatWins.at(i);
				if( pFloatWin != nullptr && pFloatWin->isEnable())
				{
					pFloatWin->OnMovePhase( m_currentPhase);
				}
			}

			UpdateLocation();

			m_lastPhase = m_currentPhase;
			GetCurrentPhase()->m_bMyPhase = true;

			// 14492 ���� ����. Ŭ��ä�ο��� Ŭ����ü�� ���� ������ Ŭ��ä�ο��� ������ ��
			// TopMenu�� ServerChannel ���� �ٲ����� �ʾ� ũ���� �߻��մϴ�
			if (g_pEnvSet->GetUIVersion() == UFV_1_1)
			{
				if (m_currentPhase == UIPHASE_SERVER_CHANNEL_LIST)
					m_pTopMenu->SetMode(TOPMENU_MODE_CHANNEL);
			}

			i3UI::setDisableUIInput(false);
		}

		// �ڸ���� �˻� ����
		if( m_pAwayChecker != nullptr)
		{
			switch( Phase)
			{
			case UIPHASE_SERVER_CHANNEL_LIST:
				m_pAwayChecker->SetTime(AWAY_INPUT_SERVER_TIME, AWAY_INPUT_SERVER_CAUTION);
				break;
			case UIPHASE_READY_ROOM:
				// �տ��� �� �ð�, �ڿ����� 5���� �̵��մϴٿ��� 5������ ������ֱ�
				// ��) m_pAwayChecker->SetTime(10, AWAY_INPUT_ROOM_CAUTION);
				m_pAwayChecker->SetTime(AWAY_INPUT_ROOM_TIME, AWAY_INPUT_ROOM_CAUTION);
				break;
			}
		}

		// RoomMaster Kick ����
		if(m_pRoomMasterAwayCheckerManager != nullptr)
		{
			switch (Phase)
			{
			case UIPHASE_READY_ROOM: break;
			default:
				m_pRoomMasterAwayCheckerManager->RemoveAwayChecker();
				break;
			}
		}
		
		if( CCommunityContext::i() != nullptr &&
			CCommunityContext::i()->IsMovingToUser() && 
			m_currentPhase != UIPHASE_START )
		{
			UIPhaseBase * pPhase = GetCurrentPhase();
			pPhase->OnInviteMove();
			return;
		}
	}
}

void UIMainFrame::SetStartPhase( UIPHASE Phase)
{
	m_lastPhase = Phase;
}

void UIMainFrame::ChangeSubPhase( UISUBPHASE phase)
{
	UIPopupBase * pPopupbase = GetPopup(UPW_SHOPEXTENSION);

	I3ASSERT_RETURN(pPopupbase != nullptr);

	if (m_bDeprotationClanChannel)
	{
		//Ŭ��ä�ο��� ������� ���·� ���� �������� ��ȯ�ϸ� �ش� �������������� �������� ��.
		UISUBPHASE sub_phase_type = GetCurrentSubPhaseType();
		GAME_EVENT set_game_event = EVENT_NOTHING;

		switch (sub_phase_type)
		{
		case USP_SHOP:			set_game_event = EVENT_LEAVE_SHOP;		break;
		case USP_INVENTORY:		set_game_event = EVENT_LEAVE_INVENTORY; break;
		case USP_GACHA:			set_game_event = EVENT_LEAVE_GACHA;		break;
		case USP_GIFTSHOP:		set_game_event = EVENT_LEAVE_GIFTSHOP;	break;
		case USP_PROFILE:		set_game_event = EVENT_LEAVE_PROFILE;	break;
		case USP_TEMP:			set_game_event = EVENT_LEAVE_TEMP;		break;
		}

		if (set_game_event != EVENT_NOTHING)
			GameEventReceiver::i()->PushReceivedGameEvent(set_game_event);

		return;
	}

	// �ڽ��� �̹� �۵� ���̸� ó������ �ʴ´�.
	if (USP_NONE != phase && false == GetSubPhase(phase)->IsRun())
	{
		return;
	}

	// �ͽ��ټ��˾��� �������̸�, �ݱ� �� �ڽ��� ȣ���� �������� ���ư���.
	// ���� �κ��丮, ���� �ͽ��ټ� �˾��� ȣ����.
	if (USP_NONE == phase && pPopupbase->isEnable())
	{
		if( IsEnteredSubPhase(USP_INVENTORY))
		{
			phase = USP_INVENTORY;
		}
		else if( IsEnteredSubPhase(USP_SHOP))
		{
			phase = USP_SHOP;
		}
		else if( IsEnteredSubPhase(USP_GACHA))
		{
			phase = USP_GACHA;
		}
		else if( IsEnteredSubPhase(USP_GIFTSHOP) )
		{
			phase = USP_GIFTSHOP;
		}
		else if( IsEnteredSubPhase(USP_PROFILE))
		{
			phase = USP_PROFILE;
		}
		else if( IsEnteredSubPhase(USP_TEMP))
		{
			phase = USP_TEMP;
		}
	}

	i3::vector<UISUBPHASE>::iterator itr;
	itr = m_SubPhaseQueue.begin();
	while( itr != m_SubPhaseQueue.end())
	{
		UISUBPHASE p = *itr;
		if( p == phase)
			return;
		++itr;
	}

	// �κ��丮 �������� �Ѿ��.o
	if (USP_INVENTORY != m_CurrentSubPhase &&
		USP_INVENTORY == phase &&
		false == pPopupbase->isEnable())
	{
		m_SubPhaseQueue.push_back( USP_INVENTORY);
	}

	if (USP_SHOP != m_CurrentSubPhase &&
		USP_SHOP == phase &&
		false == pPopupbase->isEnable())
	{
		// ChangeSubPhase�� ���� ���� �� ��Ŷ�� ������ ������ ��� �߰� - sanghun.han(2015.07.21)
		GameEventSender::i()->SetEventOnce(EVENT_ENTER_SHOP);

		m_SubPhaseQueue.push_back( USP_SHOP);
	}

	if (USP_GACHA != m_CurrentSubPhase &&
		USP_GACHA == phase &&
		false == pPopupbase->isEnable())
	{
		m_SubPhaseQueue.push_back( USP_GACHA);
	}

	if (USP_GIFTSHOP != m_CurrentSubPhase &&
		USP_GIFTSHOP == phase &&
		false == pPopupbase->isEnable())
	{
		m_SubPhaseQueue.push_back( USP_GIFTSHOP);
	}

	if (USP_PROFILE != m_CurrentSubPhase &&
		USP_PROFILE == phase )//&&
		//FALSE == pPopupbase->isEnable())
	{
		m_SubPhaseQueue.push_back( USP_PROFILE);
	}

	if (USP_TEMP != m_CurrentSubPhase &&
		USP_TEMP == phase )//&&
		//FALSE == pPopupbase->isEnable())
	{
		m_SubPhaseQueue.push_back( USP_TEMP);
	}


	if(m_CurrentSubPhase == phase)
	{
		i3UI::setDisableUIInput( false);
		m_bLockInput = false;
	}
}

void UIMainFrame::GameEventTransferToSubPhase( GAME_EVENT evt, INT32 Arg, const i3::user_data& UserData)
{
	for(size_t i = 0; i < m_SubPhase.size(); i++)
	{
		UIPhaseSubBase * pSubPhaseBase = m_SubPhase.at(i);

		if (pSubPhaseBase != nullptr && pSubPhaseBase->isEnable())
		{
			pSubPhaseBase->ProcessGameEvent( evt, Arg, UserData);
		}
	}
	
}

void UIMainFrame::GameEventTransferToFloatingWin( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	//Flating������ ��� �ش�����մ� ��츸 �̺�Ʈ �����ݴϴ�.
	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);
		if( pFloatWin && pFloatWin->isEnable())
			pFloatWin->ProcessGameEvent(evt, arg, UserData);
	}
}

void UIMainFrame::EnableFloatWin( UIFLOATWIN type, void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	UIFloatingBase * curfloat = GetFloating(type);
	if( curfloat == nullptr)
		return;

	UIFLOATING_POS pos = curfloat->getAlignedPos();
	UIFLOATING_POS pos2 = curfloat->getAlignedPos2();
	// ���� ��ġ�� �ִ� FloatWindow�� �˻�
	I3ASSERT( pos != UIFLOATING_POS_NONE);

	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);
		if(( i == (size_t) type) || pFloatWin == nullptr )
			continue;

		bool bQue = false;

		if( pFloatWin->getAlignedPos() == pos)
		{
			if( pos2 != UIFLOATING_POS_NONE)
			{
				if( (pFloatWin->getAlignedPos2() != UIFLOATING_POS_NONE) &&
					(pFloatWin->getAlignedPos2() != pos2) )
					bQue = true;
				else if( pFloatWin->getAlignedPos2() == UIFLOATING_POS_NONE)
					bQue = true;
			}
			else
				bQue = true;

			// �� FloatWindow�� ���ϴ�.
			if( pFloatWin->isEnable() && bQue &&
				pFloatWin->IsFixedWindow() == false)
			{
				pFloatWin->OnExitStart();
				m_FloatWinQue.push_back( i);
			}
		}
	}

	// ���ο� FloatWindow�� ������ �մϴ�.
	if( curfloat->OnEntranceStart( pArg1, pArg2) )	// han.7278
	{
		curfloat->OnEntranceEnd();
	}
}

void UIMainFrame::DisableFloatWin( UIFLOATWIN type)
{
	UIFloatingBase * curfloat = GetFloating(type);
	if(curfloat == nullptr)	return; 
	if( curfloat->isEnable())
	{
		curfloat->OnExitStart();
	}
}

BOOL UIMainFrame::IsEnableFloatWin(UIFLOATWIN type)
{
	UIFloatingBase * curfloat = GetFloating(type);
	if (curfloat == NULL)	return false;
	return curfloat->isEnable();
}

void UIMainFrame::DisableFloatWinInput( UIFLOATWIN type,bool bTrue)
{
	UIFloatingBase * curfloat = GetFloating(type);
	if(curfloat == nullptr)	return; 
	if( curfloat->isEnable())
	{
		curfloat->SetInputDisable(bTrue);
	}
}


void UIMainFrame::TogglePopup( UIPOPUPWIN popupType, void * pArg1, void * pArg2)
{
	if( UPW_NULL == popupType)
	{
		for(size_t i = 0; i < m_Popup.size(); i++)
		{
			UIPopupBase * pPopupbase =  m_Popup.at(i);

			if( pPopupbase != nullptr && pPopupbase->isEnable())
			{
				pPopupbase->OnExitStart();
				FocusPopup(pPopupbase, false, __FILE__, __LINE__);

				// topmenu���� ���Ǵ� �˾��� �����Ŀ� ������ ó���� ���ϱ⋚����
				if (m_pTopMenu != nullptr)
					m_pTopMenu->uglyOnMainFrameClosePopupAfter(i);
			}
		}

		return;
	}

	UIPopupBase * curPopupbase = GetPopup(popupType);

	if( curPopupbase != nullptr && curPopupbase->isEnable())
	{		
		if( curPopupbase->OnExitStart())
		{
			FocusPopup(curPopupbase, false, __FILE__, __LINE__);

			// topmenu���� ���Ǵ� �˾��� �����Ŀ� ������ ó���� ���ϱ⋚����
			if (m_pTopMenu != nullptr)
				m_pTopMenu->uglyOnMainFrameClosePopupAfter(popupType);
		}
	}
	else
	{
		// �������� �ݰ�
		for(size_t i = 0; i < m_Popup.size(); i++)
		{
			UIPopupBase * pPopupbase =  m_Popup.at(i);

			if (!g_pEnvSet->IsV2Version() && (i3::same_of<UIPhaseClanMember*>(pPopupbase) || i3::same_of<UIPhaseNonClanMember*>(pPopupbase)))
				continue;

			if( pPopupbase != nullptr && pPopupbase->isEnable())
			{
				pPopupbase->OnExitStart();
				FocusPopup(pPopupbase, false, __FILE__, __LINE__);

				// topmenu���� ���Ǵ� �˾��� �����Ŀ� ������ ó���� ���ϱ⋚����
				if (m_pTopMenu != nullptr)
					m_pTopMenu->uglyOnMainFrameClosePopupAfter(i);
			}
		}

		if(curPopupbase != nullptr && curPopupbase->OnEntranceStart(pArg1, pArg2))
		{
			// menubar�� phase�� input�� ���´�.
				FocusPopup(curPopupbase, true, __FILE__, __LINE__);
		}
	}
}

void UIMainFrame::OpenPopup( UIPOPUPWIN popupType, void * pArg1/* = NULL*/, void * pArg2/* = NULL*/)
{
	if( IsPossibleToOpenPopup() == false )
		return;

	UIPopupBase * pPopupbase = GetPopup(popupType);

	if( pPopupbase != nullptr && popupType != UPW_NULL )
	{
		if( pPopupbase->OnEntranceStart(pArg1, pArg2))
		{
			// menubar�� phase�� input�� ���´�.
			if( pPopupbase->isNoCloseButton() == false)
				FocusPopup( pPopupbase, true, __FILE__, __LINE__);
		}
	}
}

void UIMainFrame::ClosePopup( UIPOPUPWIN popupType)
{
	if( popupType == UPW_NULL ) return;
	UIPopupBase * pPopupbase = GetPopup(popupType);

	if( pPopupbase != nullptr && popupType != UPW_NULL )
	{
		if( pPopupbase->OnExitStart())
			FocusPopup(pPopupbase, false, __FILE__, __LINE__);
	}

	// topmenu���� ���Ǵ� �˾��� �����Ŀ� ������ ó���� ���ϱ⋚����
	if (m_pTopMenu != nullptr)
		m_pTopMenu->uglyOnMainFrameClosePopupAfter(popupType);
}

void UIMainFrame::CloseAllPopup()
{
	for(INT32 i=UPW_OPTION;i<UPW_MAX;++i)
	{
		ClosePopup( (UIPOPUPWIN)i );
	}
}

bool UIMainFrame::IsOpenedPopup( UIPOPUPWIN type)
{
	I3_BOUNDCHK( type, UPW_MAX);
	UIPopupBase * pPopupbase = GetPopup(type);
	return (pPopupbase!=nullptr && pPopupbase->isEnable()) ? true : false;
}

UIPOPUPWIN UIMainFrame::FindPopup( LuaState * L)
{
	for(size_t i = 0; i < m_Popup.size(); i++)
	{
		UIPopupBase * pPopupbase =  m_Popup.at(i);

		if( pPopupbase != nullptr)
		{
			I3ASSERT( pPopupbase);
			if( pPopupbase->IsLuaContext( L))
				return (UIPOPUPWIN)i;
		}
	}

	return UPW_NULL;
}

UIPOPUPWIN UIMainFrame::FindPopupByPointer( UIPopupBase * pPopup)
{
	for(size_t i = 0; i < m_Popup.size(); i++)
	{
		UIPopupBase * pPopupbase =  m_Popup.at(i);

		if( pPopupbase != nullptr)
		{
			I3ASSERT( pPopupbase);
			if( pPopupbase == pPopup)
				return (UIPOPUPWIN)i;
		}
	}

	return UPW_NULL;
}

bool UIMainFrame::IsPossibleToOpenPopup(void)
{
	// sub phase ��ȯ�� �ִ� ��쿡�� �˾��� ���½�Ű�� �ʽ��ϴ�.
	{
		if( m_SubPhaseQueue.empty() )
			return true;
	}

	return false;
}

UIBase * UIMainFrame::FindUIBase( LuaState * L)
{
	if( m_pTopMenu != nullptr)
	{
		if( m_pTopMenu->IsLuaContext( L))
			return m_pTopMenu;
	}

	if( m_pSideMenu != nullptr )
	{
		if( m_pSideMenu->IsLuaContext( L))
			return m_pSideMenu;
	}

	if( GetCurrentPhase()->IsLuaContext( L))
		return GetCurrentPhase();

	if( m_CurrentSubPhase != USP_NONE)
	{
		if( GetCurrentSubPhase()->IsLuaContext( L))
			return GetCurrentSubPhase();
	}

	for(size_t i = 0; i < m_FloatWins.size(); i++)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(i);
		if( pFloatWin)
		{
			if( pFloatWin->IsLuaContext( L))
				return pFloatWin;
		}
	}

	for(size_t i = 0; i < m_Popup.size(); i++)
	{
		UIPopupBase * pPopupbase =  m_Popup.at(i);

		if( pPopupbase != nullptr && pPopupbase->IsLuaContext( L))
			return pPopupbase;
	}

	return nullptr;
}

void UIMainFrame::GameEventTransferToPopup( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	for(size_t i = 0; i < m_Popup.size(); i++)
	{
		UIPopupBase * pPopupbase =  m_Popup.at(i);

		if( pPopupbase != nullptr && ( pPopupbase->isEnable() || ( i == UPW_QUERY_ITEM) ))
		{
			pPopupbase->ProcessGameEvent( event, Arg, UserData);
		}
	}
}


void UIMainFrame::GetCurrentSeverInfo( i3::stack_wstring& outServerInfo )
{
	
	//	���� ���� ���	
	INT32 serverNumber = g_pServerInfo->GetServerNumber();
	ChannelType channelType = ChannelContext::i()->GetConnectedChannelType();

	i3::rc_wstring wstrChannelSlotText;
	ChannelContext::i()->GetServerChannelText(wstrChannelSlotText, channelType, SS_SUBTYPE_CATEGORY_READY_SLOT_TEXT);

	i3::sprintf(outServerInfo, L"%03d %s", serverNumber, wstrChannelSlotText);
}

void UIMainFrame::PlayCeremonyCharacter( void)
{
	UIFloatingBase * curfloat = GetFloating(UFW_CHARACTER);
	I3ASSERT( curfloat);
	if(curfloat != nullptr)
		((UIFloatCharacterView*)curfloat)->PlayCeremonyCharacter();
}

bool UIMainFrame::EscapeKey( void)
{
	for(size_t i = 0; i < m_Popup.size(); i++)
	{
		UIPopupBase * pPopupbase =  m_Popup.at(i);

		if( pPopupbase->isEnable())
		{
			ClosePopup( (UIPOPUPWIN) i);
			return false;
		}
	}

	if( m_CurrentSubPhase != USP_NONE)
	{
		GetCurrentSubPhase()->OnEscapeKey();
		return false;
	}

	return true;
}

void UIMainFrame::OnEscape( void)
{
	if( m_currentPhase != UIPHASE_START)
		GetCurrentPhase()->OnEscapeKey();
}

void UIMainFrame::UpdateEffectTime( REAL32 rDeltaSeconds)
{
	if( m_bUIEffectBegin)
	{
		m_rUIEffectActiveTime += rDeltaSeconds;
		if( m_rUIEffectActiveTime > 1.0f)
		{
			m_rUIEffectActiveTime = 0.0f;
			m_bUIEffectState = !m_bUIEffectState;
		}
	}
	else
	{
		m_bUIEffectState = false;
	}
}

void UIMainFrame::ExclusedEnterSubPhase( UISUBPHASE phase)
{
	bool bImmediatelyEnterEvent = true;

	if( m_CurrentSubPhase != USP_NONE)
	{
		if (phase == USP_GIFTSHOP)
		{
			//����Ʈ ���� ���� ����� ���� ������ �����Ǳ� ������ ��ٸ���
			//�ٷ� ���� �̺�Ʈ�� �߻���Ų��.
			bImmediatelyEnterEvent = true;
			m_ExclusedEnterEventSubPhase = USP_NONE;
		}
		else if( m_CurrentSubPhase != phase)
		{	// �ٸ� Phase�� �����ȴ�.
			// ������ Phase���� ������..
			GetCurrentSubPhase()->Leave();
			bImmediatelyEnterEvent = false;
		}
		else
		{	// ���� Phase
			// �������� ������.
			GetCurrentSubPhase()->Leave();
			m_ExclusedEnterEventSubPhase = USP_NONE;
			return;
		}
	}

	// ���� �̺�Ʈ�� ���ٸ� ��� ���� �̺�Ʈ ����
	if (bImmediatelyEnterEvent)
	{
		EventEnterSubPhase(phase);

		// �Է��� ���´�.
		if (phase != USP_NONE)
		{
			i3UI::setDisableUIInput( true);
		}

		m_ExclusedEnterEventSubPhase = USP_NONE;
	}
	else
	{
		// ���� �̺�Ʈ�� �ִٸ�, ��������ñ��� ���
		// ���� ��, EnterDelayedEventSubPhase()�� ���� ����
		m_ExclusedEnterEventSubPhase = phase;

		// ���ʵ� �ϴ� �Է��� ���´�. ( 11.10.07.����) 
		if (phase != USP_NONE)
		{
			i3UI::setDisableUIInput( true);
		}
	}
}

//������ �ݹ� �Լ�.
void cbDeprotation(void* pThis, INT32 nParam)
{
	UIMainFrame * pMainFrame = g_pFramework->GetUIMainframe();;
	I3ASSERT(pMainFrame != nullptr);

	pMainFrame->setDeprotationClanChannel(false, L"");
	pMainFrame->EnterDelayedEventSubPhase();
};

void UIMainFrame::EnterDelayedEventSubPhase(void)
{
	if( (m_pTopMenu == nullptr) || (m_pTopMenu->IsServerBasePopup()) )	return;	//popup �� Ȱ��ȭ �� ���, ���� sub phase���� �����µ�, �װ� ���´�.

	if (m_bDeprotationClanChannel)
	{
		//Ŭ��ä�ο��� ������߰� ��������� ���� �Ǵ� ��Ȳ�̶�� �ѹ� ���Ͻ�Ű�� �ݹ鿡�� Ȯ�� �޾� ������.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, m_sDeprotationClanMessage,nullptr, MAKE_CALLBACK(cbDeprotation), this); // Ŭ�� ����
		return;
	}

	// SubPhase���� ���� ���...
	if (g_pEnvSet->m_nUIVersion == UFV_1_5 )
	{
		if (!EventEnterSubPhase(m_ExclusedEnterEventSubPhase))
		{
			CloseAllPopup();
			m_SubPhaseQueue.push_back(USP_NONE);
		}
		m_ExclusedEnterEventSubPhase = USP_NONE;
	}
	else
	{
		if (!EventEnterSubPhase(m_ExclusedEnterEventSubPhase))
		{
			CloseAllPopup();
			m_SubPhaseQueue.push_back(USP_NONE);
			m_ExclusedEnterEventSubPhase = USP_NONE;
		}
	}
}

void UIMainFrame::LeaveSubPhase( UISUBPHASE phase)
{
	// ������
	if( phase != USP_NONE)
	{
		GetSubPhase(phase)->Leave();
	}
}

bool UIMainFrame::EventEnterSubPhase( UISUBPHASE phase)
{
	switch( phase)
	{
	case USP_INVENTORY	: GameEventSender::i()->SetEventOnce(EVENT_ENTER_INVENTORY);	break;
	case USP_SHOP		: GameEventSender::i()->SetEventOnce(EVENT_ENTER_SHOP);			break;
	case USP_GACHA		: GameEventSender::i()->SetEventOnce(EVENT_ENTER_GACHA);		break;
	case USP_GIFTSHOP	: GameEventSender::i()->SetEventOnce(EVENT_ENTER_GIFTSHOP);		break;
	case USP_PROFILE	: GameEventSender::i()->SetEventOnce(EVENT_ENTER_PROFILE);		break;
	case USP_TEMP		: 
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ENTER_TEMP);
			break;
			//TogglePopup( UPW_NULL, false);
			//ChangeSubPhase( USP_TEMP);
			//m_ExclusedEnterEventSubPhase = phase;
// 			if(ClanGameContext::i()->getMyMercenaryState() == true &&
// 				UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN )
// 			{
// 				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_MERC_CREATE_MATCH_ROOM"));
// 				return false;
// 			}

// 			UIPOPUPWIN popup = UPW_CLAN_MEMBER;
// 			
// 			if(IsOpenedPopup(UPW_CLAN_NON_MEMBER) == true )
// 				popup = UPW_CLAN_NON_MEMBER;
// 			TogglePopup( popup);
// 			
		}
		break;
		
	default:
		return false;
	}

	return true;
}

void UIMainFrame::ExclusedEnterPopup( UIPOPUPWIN popup)
{
	//�Ű����� popup�� UPW_NULL, ��ȿ�� enum ���� ó���Ѵ�.
	//UPW_NULL �� ���� ���� popup�� close �Ѵ�. ��ȿ�� enum�� open(���� ����) �Ѵ�.
	//ExclusedEnterSubPhase �Լ��� �ٸ� ��
	//1. Ȱ��ȭ�ÿ� ���� ��ư���� ���� �� ����. popup �ܺδ� �Է� �Ұ�.
	//2. Ȱ��ȭ�ÿ� �ٸ� popup enum�� ȣ���� ������.(UPW_NULL ȣ�� �Ŀ� �ٸ� enum ȣ�� ����)

	if( popup == UPW_NULL )
	{
		UIPopupBase *curr_pop = GetCurrentPopup();
		if( curr_pop ) curr_pop->Leave();
	}
	else
	{
		switch(popup)
		{
		case UPW_CLAN_NON_MEMBER:	// Fall through
		case UPW_CLAN_MEMBER:
			g_pFramework->GetClanContext()->EnterServer();
			break;
		}
		i3UI::setDisableUIInput( true);
	}
}

void UIMainFrame::EnterDelayedEventPopup(void)
{
	UIPOPUPWIN curr_state = GetCurrentPopupState();

	DEV_LOG("Popup Number = %d", curr_state );

	ClosePopup(curr_state);

	if(m_bChangeClanPopup)
	{	
		if(g_pEnvSet->m_nUIVersion == UFV_1_5)
		{
			if(curr_state == UPW_CLAN_MEMBER)
				ExclusedEnterPopup(UPW_CLAN_NON_MEMBER);
			else
				ExclusedEnterPopup(UPW_CLAN_MEMBER);
		}
		else
		{
			ExclusedEnterSubPhase(USP_TEMP);
		}

		m_bChangeClanPopup = false;
	}
	else
	{
		if (m_pTopMenu != nullptr)
			m_pTopMenu->ClickPopupBackButton();
	}

	i3UI::setDisableUIInput( false );
}

void UIMainFrame::LeavePopup( UIPOPUPWIN popup)
{
	if( popup != UPW_NULL)
		GetPopup(popup)->Leave();

	i3UI::setDisableUIInput( false );
}

bool UIMainFrame::IsEnteredSubPhase( UISUBPHASE sub )
{
	if( GetSubPhase(sub))
		return GetSubPhase(sub)->isEnable()?true:false;
	return false;
}

bool UIMainFrame::IsCreatingNick( void)
{
	UIPopupBase * pPopupbase = GetPopup(UPW_CREATENICKNAME);

	if( pPopupbase)
		return pPopupbase->isEnable()?true:false;

	return false;
}

void UIMainFrame::AuthStartSuccess( INT32 arg)
{
	UISUBPHASE phase = GetCurrentSubPhaseType();

	if( phase == USP_NONE)
	{
		return;
	}

	UIPhaseBase * pSubPhase = (UIPhaseBase*) GetSubPhase(phase);
	pSubPhase->AuthStartSuccess( arg);
}

void UIMainFrame::SetSlideNotice_Permanant( const i3::rc_wstring& wstrMsg)
{
	if (m_pTopMenu != nullptr)
		m_pTopMenu->SetSlideNotice_Permanant( wstrMsg);
}

void UIMainFrame::SetSlideNotice_Temporary( const i3::rc_wstring& wstrMsg)
{
	if (m_pTopMenu != nullptr)
		m_pTopMenu->SetSlideNotice_Temporary( wstrMsg);
}

void UIMainFrame::StartHighlightMailbox(void)
{
	UIPopupBase * pPopupbase = GetPopup(UPW_MAILBOX);

	if( pPopupbase && false == pPopupbase->isEnable())
	{
		if (m_pTopMenu != nullptr)
			m_pTopMenu->StartHighlightMailbox();
	}
}

void UIMainFrame::StartAlarmNote(void)
{
	UIPopupBase * pPopupbase = GetPopup(UPW_MAILBOX);

	if( pPopupbase && false == pPopupbase->isEnable())
	{
		if (m_pTopMenu != nullptr)
			m_pTopMenu->StartHighlightMailbox();
		((UIPopupMailBox*)pPopupbase)->StartAlarmNote();
	}
}

void UIMainFrame::UpdateLocation( void)
{
}

void UIMainFrame::OnInviteMove( void)
{
	// ��� �˾�â�� �ݴ´�.
	TogglePopup( UPW_NULL);

	if( CCommunityContext::i()->StartMovingToUser() )
	{
		UIPhaseBase * pPhase = GetCurrentPhase();

		if( ! pPhase->OnInviteMove() )
		{
			CCommunityContext::i()->CompleteMovingToUser();
		}
	}
}

void UIMainFrame::OnInviteMoveClan( void)
{
	if( CCommunityContext::i()->StartMovingToUser() )
	{
		UIPhaseBase * pPhase = GetCurrentPhase();
		if ( ! pPhase->OnInviteMove() )
		{
			CCommunityContext::i()->CompleteMovingToUser();	
		}
	}
}

void UIMainFrame::ClosePopupByAnotherControlInput( i3UIControl * pControl)
{
	if( pControl == nullptr) return;

	for(size_t i = 0; i < m_Popup.size(); i++)
	{
		UIPopupBase * pPopupbase =  m_Popup.at(i);

		if(pPopupbase != nullptr && pPopupbase->isNoCloseButton() && pPopupbase->isEnable())
		{
			if( pPopupbase->IsIncludeControl( pControl) == false)
				ClosePopup( (UIPOPUPWIN) i);
		}
	}
}

void UIMainFrame::OpenAwayMessageBox(const i3::rc_wstring& wstrMessage)
{
	CloseAwayMessageBox();
	m_pAwayMessageBox = GameUI::MsgBox(MSG_TYPE_GAME_WAIT, wstrMessage);
}

void UIMainFrame::CloseAwayMessageBox( void)
{
	if (m_pAwayMessageBox) 
	{
		GameUI::CloseMsgBox(m_pAwayMessageBox);
		m_pAwayMessageBox = nullptr;
	}
}

void	UIMainFrame::InitItemImageMgr(void)
{
	ItemImageMgr_Init init;

	for (size_t i=0; i<g_pWeaponInfoDataBase->GetUITemplateInfoCount(); i++)
	{
		const CUITemplateInfo_Weapon* info = g_pWeaponInfoDataBase->GetUITemplateInfo(i);
		I3ASSERT(info != nullptr);

		if (info)
		{
			i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
			init.AddWeaponTemplate(strName.c_str());
		}
	}

	for (size_t i=0; i<g_pCharaInfoDataBase->GetUITemplateInfoCount_Chara(); i++)
	{
		const CUITemplateInfo_Chara* info = g_pCharaInfoDataBase->GetUITemplateInfo_Chara(i);
		I3ASSERT(info != nullptr);

		if (info)
		{
			init.AddCharacterTemplate(info->GetType(), info->GetLoopLoad(), info->GetName().c_str());
		}
	}

	for (size_t i=0; i<g_pShopItemInfoDataBase->GetUITemplateCount(); i++)
	{
		const CUITemplateInfo_Item* info = 
			static_cast<CUITemplateInfo_Item*>(g_pShopItemInfoDataBase->GetUITemplateInfo(i));
		I3ASSERT(info != nullptr);

		if (info)
		{
			i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
			init.AddItemTemplate(info->GetType(), strName.c_str());
		}
	}

	for (size_t i=0; i<g_pCharaInfoDataBase->GetUITemplateInfoCount_Parts(); i++)
	{
		const CUITemplateInfo_Parts* info = 
			static_cast<const CUITemplateInfo_Parts*>(g_pCharaInfoDataBase->GetUITemplateInfo_Parts(i));
		I3ASSERT(info != nullptr);

		if (info)
		{
			i3::string strName;		i3::utf16_to_mb(info->GetName(), strName);
			init.AddPartsTemplate(info->GetType(), info->GetLoopLoad(), strName.c_str());
		}
	}

	GetItemImageMgr()->Initialize(init);
}

void UIMainFrame::SetExitTimer( bool bSet, float fTime )
{
	if( true == bSet )
	{
		m_bExiting		= true;
		m_fExitTimer	= fTime;

		m_pWaitExitingMessageBox = GameUI::MsgBox( MSG_TYPE_SYSTEM_WAIT, GAME_RCSTRING("STR_TBL_CLIENT_EXITING"));
	}
	else
	{
		m_bExiting = false;
		m_fExitTimer = 0;

		if( m_pWaitExitingMessageBox )
		{
			GameUI::CloseMsgBox( m_pWaitExitingMessageBox );
			m_pWaitExitingMessageBox = nullptr;
		}
	}
}


void UIMainFrame::_SideMenu_Entrance_() const
{	
	// Key : User Guide System
	if( m_pSideMenu != nullptr )
	{
		m_pSideMenu->Entrance();
		UserGuideMgr::i()->OnLoadGuideTargetInfo(i3::kind_cast<UIBase*>(m_pSideMenu));
	}
}

void UIMainFrame::_SideMenu_Exit_() const
{
	// Key : User Guide System
	if( m_pSideMenu != nullptr )
	{
		m_pSideMenu->Exit();
		UserGuideMgr::i()->UnLoadGuideTargetInfo(i3::kind_cast<UIBase*>(m_pSideMenu));
	}
}

UIPOPUPWIN UIMainFrame::GetCurrentPopupState()
{
	const UIPopupBase *curr_pop = GetCurrentPopup();

	for(size_t i = 0; i < m_Popup.size(); i++)
	{
		UIPopupBase * pPopupbase =  m_Popup.at(i);

		if( curr_pop == pPopupbase )
			return (UIPOPUPWIN)i;
	}
	return UPW_NULL;
}

void UIMainFrame::ChangeClanPopup( void)
{
	ExclusedEnterPopup(UPW_NULL);

	m_bChangeClanPopup = true;	
}

void UIMainFrame::SetUICharaCamera(INT32 cam) 
{
	if (m_pTopMenu != nullptr)
		m_pTopMenu->SetUICharaCamera((CAM_LOOK_CMD)cam);
}

UIFloatingBase * UIMainFrame::GetFloating( UIFLOATWIN type)
{
	I3_BOUNDCHK( type, UFW_MAX);

	if( m_FloatWins.size() > 0)
	{
		UIFloatingBase * pFloatWin = m_FloatWins.at(type);
		if( pFloatWin)
			return pFloatWin;
	}
	return nullptr;
}