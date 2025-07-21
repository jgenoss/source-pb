#include "pch.h"
#include "GlobalVariables.h"
#include "StageReady.h"
#include "MenuBar.h"
#include "AwayChecker.h"
#include "PhaseHeader.h"
#include "StepHeader.h"
#include "StageGui/Popup/PopupHeader.h"
#include "ReadyBg.h"
#include "ContextMenu.h"
#include "SpecialCharInput.h"
#include "BgChallenge.h"
#include "CommunityContext.h"
#include "GameContextScript.h"
#include "PhaseLobby_dmy.h"

I3_CLASS_INSTANCE(CStageReady, i3Stage);

PhaseType CStageReady::m_lastPhase = PHASE_START;

CStageReady::CStageReady()
{
	m_pGuiRoot = NULL;

	m_pMenuBar = NULL;
	m_pContextMenu = NULL;

	m_pChallenge = NULL;

	i3mem::FillZero(m_pPhase, sizeof(m_pPhase));
	i3mem::FillZero(m_pStep, sizeof(m_pStep));
	i3mem::FillZero(m_pPopup, sizeof(m_pPopup));

	m_currentPhase = PHASE_START;
	m_phaseState = PS_IDLE;

	m_currentStep = STEP_NULL;
	m_oldStep = STEP_NULL;

	m_pReadyBg = NULL;
	m_pSpCharInput = NULL;



	m_currentLook = CL_IDLE;
	//ButtonEffect ����	
	m_ActiveTime = 0;
	BeginEffect = FALSE;

	m_rWheelDelta = 0.0f;

	m_bInventory = FALSE;
	m_bShop = FALSE;
	m_bInfo = FALSE;
	m_bClanMember = FALSE;
	m_bClanNonMember = FALSE;
	m_ExclusedEnterEventStep = STEP_NULL;
	
#if defined	USE_AWAY_INPUT
	m_pAwayChecker = NULL;
	m_pAwayMessageBox = NULL;
	m_bProcessLeaveRoom = FALSE;

	//Room Master GameStart Timer After All User Ready
	InitRoomMasterKick();
#endif
	m_iRemainCount = -1;
	m_rRemainTime = 1.f;

	Button_Effect_State = FALSE;
	m_bBeHostMessageOK = FALSE;
	m_bInClanBattleLobby = FALSE;
	m_oldAwayStep = STEP_NULL;
}

CStageReady::~CStageReady()
{
#if defined	USE_AWAY_INPUT
	RemoveRoomMasterKick();
	
	if (m_pAwayMessageBox)
	{
		g_pFramework->CloseMessageBox(m_pAwayMessageBox);
		m_pAwayMessageBox = NULL;
	}

	if (m_pAwayChecker)
	{
		RemoveChild(m_pAwayChecker);
		I3_SAFE_RELEASE( m_pAwayChecker);
	}
#endif

	if (m_pReadyBg)
	{
		RemoveChild(m_pReadyBg);
		I3_SAFE_RELEASE(m_pReadyBg);
	}
	
	if( m_pSpCharInput)
	{
		RemoveChild(m_pSpCharInput);
		I3_SAFE_RELEASE(m_pSpCharInput);
	}

	if (m_pGuiRoot)
	{
		m_pGuiRoot->getSceneGraphNode()->RemoveFromParent();
	}

	g_pGUIRoot = NULL;
	
	g_pWeaponManager->LeaveStage();
	g_pCharaManager->LeaveStage();

	{
		I3_SAFE_RELEASE(m_pContextMenu);
		I3_SAFE_RELEASE( m_pMenuBar);

		I3_SAFE_RELEASE(m_pChallenge);

		for(UINT32 i = 0; i < MAX_PHASE; i++)
		{
			I3_SAFE_RELEASE( m_pPhase[i]);
		}

		for(UINT32 i = 0; i < MAX_STEP; i++)
		{
			I3_SAFE_RELEASE( m_pStep[i]);
		}

		for(UINT32 i = 0; i < MAX_POPUP; i++)
		{
			I3_SAFE_RELEASE( m_pPopup[i]);
		}
	}
}



void CStageReady::StartPhase(PhaseType MovePhase)
{
	m_currentPhase = MovePhase;

	switch(m_currentPhase)
	{
	case PHASE_SERVER_LIST:
		break;
	case PHASE_CHANNEL_LIST:
		{
		}
		break;
	case PHASE_LOBBY:
		break;
	case PHASE_READY_ROOM:
		break;
	}	 
}

void CStageReady::CheckCheating()
{
	USER_INFO_EQUIPMENT_CHARA	InfoEquipChara;
	USER_INFO_EQUIPMENT_CHARA	InfoEquipChara2;

	UINT32 nRemainBattleTime = 0;
	REAL32 rMoveSpeed = 0.f;

	g_pGameContext->GetMyUserInfoEquipmentChara(InfoEquipChara);
	g_pGameContext->GetMyUserInfoEquipmentChara2(InfoEquipChara2);

	if(InfoEquipChara.IsSame(InfoEquipChara2) == false)
	{
		rMoveSpeed = 10000.f;
		g_pGameContext->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rMoveSpeed);
	}
}

void CStageReady::Set_bGameStart(BOOL bFlag)
{ 
	m_bGameStart = bFlag;
}

void CStageReady::StartCount(INT32 iCount)
{ 
	m_rRemainTime = 1.f;
	m_iRemainCount = iCount;
}

void CStageReady::SetRemainCount(INT32 iCount)
{ 
	m_iRemainCount = iCount; 
	g_pGameContext->PushEventGame(EVENT_BATTLE_COUNTDOWN, m_iRemainCount);
}

void CStageReady::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	static BOOL bOnceUpdate = FALSE;

	if(m_iRemainCount > 0)
	{
		m_rRemainTime -= rDeltaSeconds;
		if(m_rRemainTime <= 0)
		{
			m_rRemainTime = 1.f;
			DecreaseRemainCount();
		}
	}

	// tooltip
	m_ToolTip.Update(rDeltaSeconds);	// gui notify���� ���� update�Ǿ���Ѵ�
	UpdateEffectTime(rDeltaSeconds);	// ButtonEffect ����

	CheckCheating();
	//GetLightTime(rDeltaSeconds);
	// phase
	switch(m_phaseState)
	{
	case PS_IDLE:
		{
			PhaseType	MovePhase = (PhaseType)(INT32)m_PhaseQueue.GetItem(0);
			m_PhaseQueue.Delete(0);

			if (MovePhase == PHASE_CHANNEL_LIST || MovePhase == PHASE_SERVER_LIST)
			{
				m_pMenuBar->SetServerMode();
			}
			else
			{
				m_pMenuBar->SetReadyMode();
			}

			StartPhase(MovePhase);
			m_phaseState = PS_ENTRANCE_START;
			m_lastPhase = m_currentPhase;
		}
		break;
	case PS_ENTRANCE_START:
		m_pPhase[m_currentPhase]->OnEntranceStart();
		m_phaseState = PS_ENTRANCING;

#if defined	USE_AWAY_INPUT
		if (m_pAwayMessageBox)
		{
			g_pFramework->CloseMessageBox(m_pAwayMessageBox);
			m_pAwayMessageBox = NULL;
			#ifdef USE_ROOM_MASTER_KICK
			if(TRUE == m_bSetRoomMasterKick)
			{
				m_bSetRoomMasterKick = FALSE;
				STRING_POPUP(GAME_STRING("STR_POPUP_READYROOM_NOTICE_GET_AWAY_COMPLETE"));/*���ѽð� ���� ������ �������� �ʾ�\n���� ����Ǿ����ϴ�.*/
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_GET_AWAY_COMPLETE"));/*��ð� �������� ����\n�κ�� �̵��Ǿ����ϴ�.*/
			}
			#else
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_GET_AWAY_COMPLETE"));/*��ð� �������� ����\n�κ�� �̵��Ǿ����ϴ�.*/
			#endif
		}
#endif
		break;
	case PS_ENTRANCING:
		if (m_pPhase[m_currentPhase]->OnEntrancing(rDeltaSeconds))
		{
			m_phaseState = PS_ENTRANCE_END;
		}
		break;
	case PS_ENTRANCE_END:

		i3GuiRoot::setGuiDisableInput(FALSE);
		m_pPhase[m_currentPhase]->OnEntranceEnd();
		m_pPhase[m_currentPhase]->SetEnable( TRUE);
		m_phaseState = PS_PLAYING;

		MoveCamera(CL_IDLE);

		if( !bOnceUpdate)
		{
			bOnceUpdate = TRUE;
			m_pPopup[POPUP_OPTION]->OnClosePopup();
		}

#if defined	USE_AWAY_INPUT
		// �ڸ���� �˻� ����
		switch(m_currentPhase)
		{
		case PHASE_CHANNEL_LIST:
			m_pAwayChecker->SetTime(AWAY_INPUT_SERVER_TIME, AWAY_INPUT_SERVER_CAUTION);
			break;
		case PHASE_READY_ROOM:
			// ���Ǿ� �ñ��
			// �տ��� �� �ð�, �ڿ����� 5���� �̵��մϴٿ��� 5������ ������ֱ�
			// ��) m_pAwayChecker->SetTime(10, AWAY_INPUT_ROOM_CAUTION);
			m_pAwayChecker->SetTime(AWAY_INPUT_ROOM_TIME, AWAY_INPUT_ROOM_CAUTION);
			break;
		}
#endif
		break;
	case PS_PLAYING:
		if ( m_PhaseQueue.GetCount() > 0 )
		{
			m_phaseState = PS_EXIT_START;
		}

		// �������� ������ ���¿��� ������ ���� ���� ������ ���� ��Ų��.
		//Room Master GameStart Timer After All User Ready
		CheckRoomMasterKick();		

		// GuiNotify ó��
		_GuiNotifyPump();

		// GameEventPump
		_GameEventPump();

#if defined	USE_AWAY_INPUT
		if (m_bProcessLeaveRoom)
		{
			if (STEP_NULL == m_oldAwayStep || FALSE == m_pStep[m_oldAwayStep]->isEnable())
			{
				g_pGameContext->SetEventOnce(EVENT_ENTER_LOBBY);
				m_bProcessLeaveRoom = FALSE;
			}
			else
			{
				ChangeStep(STEP_NULL);
			}
		}
		else
		{
			// �ڸ���� �˻�
			_CheckAwayInput();
		}
#endif

		_ProcessWheel();
		break;
	case PS_EXIT_START:
		m_pPhase[m_currentPhase]->SetEnable( FALSE);
		m_pPhase[m_currentPhase]->OnExitStart();
		m_phaseState = PS_EXITING;

#if defined	USE_AWAY_INPUT
		m_pAwayChecker->Reset();
		m_bProcessLeaveRoom = FALSE;

		//Room Master GameStart Timer After All User Ready		
		StopRoomMasterKick();		
		Set_bGameStart(FALSE);
#endif
		break;
	case PS_EXITING:
		if (m_pPhase[m_currentPhase]->OnExiting(rDeltaSeconds))
		{
			m_phaseState = PS_EXIT_END;
		}
		break;
	case PS_EXIT_END:
		m_pPhase[m_currentPhase]->OnExitEnd();
		m_phaseState = PS_IDLE;
		break;
	}


	// ī�޶� ����
	m_pReadyBg->MoveCamera(m_currentLook);


	// �Է� ��Ŀ�� ����
	{
		for(UINT32 i = 0; i < MAX_POPUP; i++)
		{
			m_pPopup[i]->SetInputDisable(TRUE);
		}

		m_pPhase[m_currentPhase]->SetInputDisable(TRUE);
		if (STEP_NULL != m_currentStep)
			m_pStep[m_currentStep]->SetInputDisable(TRUE);

		BOOL bFocusModal = FALSE;

		//	�Է��� Ư������ �Է±� �켱
		if( m_pSpCharInput->IsActive())
			bFocusModal = TRUE;

		if (FALSE == bFocusModal)
		{
			for(UINT32 i = 0; i < MAX_POPUP; i++)
			{
				if (m_pPopup[i]->isEnable())
				{
					m_pPopup[i]->SetInputDisable(FALSE);

					bFocusModal = TRUE;
				}
			}
		}

		if (FALSE == bFocusModal)
		{
			if (STEP_NULL != m_currentStep)
			{
				m_pStep[m_currentStep]->SetInputDisable(FALSE);
			}
			else
			{
				m_pPhase[m_currentPhase]->SetInputDisable(FALSE);
			}
		}	
	}



	// ���� ��Ʈ�� - �ӹ��˸�â Ȱ��ȭ ���� üũ
	_BackgroupEnable();

	i3Stage::OnUpdate(rDeltaSeconds);
}

BOOL CStageReady::OnQueryLoad(INT32 numStage)
{
	if (FALSE == i3Stage::OnQueryLoad(numStage))
	{
		return FALSE;
	}

	// gui ���� �б�
	i3ResourceFile File;

	if( File.Load("Gui/Project/Ready_Root.gui") == STREAM_ERR)
	{
		I3FATAL( "CStageGUI::LoadGUIFile()");
		return FALSE;
	}

	// GuiRoot�� ã�� �ʱ�ȭ�Ѵ�
	m_pGuiRoot = (i3GuiRoot*)File.FindObjectByMeta(i3GuiRoot::GetClassMeta());
	I3ASSERT(m_pGuiRoot != NULL);

	const i3TextFontSetInfo * pFontTextInfo = GetDefaultFontName();
	m_pGuiRoot->setTextNodeUsage(I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX);
	m_pGuiRoot->Create( g_pFramework->getGuiLayer(), pFontTextInfo->GetFontName(), pFontTextInfo->GetCharset(), FONT_COUNT_DEFAULT, GAME_FONT_DEFAULT_SIZE);
	m_pGuiRoot->SetViewer( g_pViewer);
	m_pGuiRoot->setWidth( g_pViewer->GetViewWidth());
	m_pGuiRoot->setHeight( g_pViewer->GetViewHeight());
	m_pGuiRoot->BuildSceneGraph();	
	m_pGuiRoot->SetEnable(FALSE);		// FadeIn ���������� ��Ȱ��ȭ
	m_pGuiRoot->setManualNotifyDispatch( true);

	m_pGuiRoot->setDefaultTextSpace(g_pConfigEx->GetGUI().DefaultTextSpaceX, g_pConfigEx->GetGUI().DefaultTextSpaceY);
	m_pGuiRoot->setDefaultTextOffset(g_pConfigEx->GetGUI().DefaultTextOffsetX, g_pConfigEx->GetGUI().DefaultTextOffsetY);


	//====================================================================================================
	// Resource�� �۾��� ���ǻ� �������� ������ �ε��մϴ�.
	// �̶� Add�Ǵ� ������ �߿��ϹǷ� ���� ������ �ٲ��� �ȵ˴ϴ�. - �۸���
	// ENABLE_DESIGNATION_INTEGRATION

	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_PhaseServerList.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_PhaseServer.gui");
#if !defined( USE_LUA_UI)
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_PhaseLobby.gui");
#endif
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_PhaseReadyRoom_New.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_StepProfile_New.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_StepClan.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_StepInventory.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_StepShop.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_StepClanBattleLobby.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_Popup.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_MenuBar.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Ready_ContextMenu.gui");
	m_pGuiRoot->ADDControlByGUI("Gui/Project/Popup_Option.gui");

	//====================================================================================================

	// EditBox ��� �ʱ�ȭ
	i3PersistantElement * pObj;

	for( INT32 i = 0; i< File.GetObjectCount() ; ++i)
	{
		pObj = File.GetObject( i);

		// GUI�� EditBox Control�� EditBox�� ����� �޴� ��� Text�� �����ؾ� �մϴ�.
		if( pObj->IsExactTypeOf( i3GuiEditBox::GetClassMeta()) ||
			pObj->IsExactTypeOf( i3GuiTextBox::GetClassMeta()) )
		{
			((i3GuiEditBox*)pObj)->CreateTextEx( GetDefaultFontName(), FONT_COUNT_DEFAULT, GAME_FONT_DEFAULT_SIZE);

			((i3GuiEditBox*)pObj)->SetTextSpace(g_pConfigEx->GetGUI().DefaultTextSpaceX, g_pConfigEx->GetGUI().DefaultTextSpaceY);
			((i3GuiEditBox*)pObj)->SetOffsetPos(g_pConfigEx->GetGUI().DefaultTextOffsetX, g_pConfigEx->GetGUI().DefaultTextOffsetY);
		}
	}

	// AddChild
	I3_GAMENODE_ADDCHILD(this, m_pGuiRoot);
	g_pGUIRoot = m_pGuiRoot;

	return TRUE;	
}

void CStageReady::OnCreate(void)
{
	I3NOTICE( "CStageReady::OnCreate() Enter");

	/*if(g_pGameContext->IsTutorialMode())
	{
		g_pGameContext->SetEventOnce(EVENT_ENTER_LOBBY);
	}*/

	i3Stage::OnCreate();

	g_pFramework->CreateReadyStageImageRes(); 

	// GUI�� ��������쿡�� ������ ����Ű���� Menu�Դϴ�.
	g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);

	m_pReadyBg = CReadyBg::NewObject();
	m_pReadyBg->Create(this);
	I3_GAMENODE_ADDCHILD(this, m_pReadyBg);

	g_pCharaManager->EnterStage( this);
	g_pWeaponManager->EnterStage( this);

	// FadeIn ���� Ȱ��ȭ
	if(m_pGuiRoot == NULL) return;

	m_pGuiRoot->SetEnable(TRUE);


	// Test Code�ν�, ���� ó���� Window�� �׻� Update�ǰ� �մϴ�.
	((i3GuiWindow*)(m_pGuiRoot->getFirstChild()))->setState(I3GUI_WINDOW_STATE_ALLWAYS_FOCUS);


	// ContextMenu ����
	m_pContextMenu =  CContextMenu::NewObject();
	I3_GAMENODE_ADDCHILD(this, m_pContextMenu);

	// MenuBar ����
	m_pMenuBar = CMenuBar::NewObject();	
	//m_pMenuBar->getc
	I3_GAMENODE_ADDCHILD(this, m_pMenuBar);


	// ���� GUI ����
	m_pChallenge = CBgChallenge::NewObject();
	m_pChallenge->SetEnable(FALSE);	
	I3_GAMENODE_ADDCHILD(this, m_pChallenge);

	// Phase ����
	m_pPhase[PHASE_SERVER_LIST] = CPhaseServerList::NewObject();
	m_pPhase[PHASE_SERVER_LIST]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPhase[PHASE_SERVER_LIST]);

	m_pPhase[PHASE_CHANNEL_LIST] = CPhaseChannelList::NewObject();
	m_pPhase[PHASE_CHANNEL_LIST]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPhase[PHASE_CHANNEL_LIST]);

#if defined( USE_LUA_UI)
	m_pPhase[PHASE_LOBBY] = CPhaseLobbyDmy::NewObject();
	m_pPhase[PHASE_LOBBY]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPhase[PHASE_LOBBY]);
#else
	m_pPhase[PHASE_LOBBY] = CPhaseLobby::NewObject();
	m_pPhase[PHASE_LOBBY]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPhase[PHASE_LOBBY]);
#endif
	m_pPhase[PHASE_READY_ROOM] = CPhaseReadyRoom::NewObject();
	m_pPhase[PHASE_READY_ROOM]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPhase[PHASE_READY_ROOM]);


	// Phase �⺻ ����	
	m_PhaseQueue.Add(reinterpret_cast<void*>(m_lastPhase));
	m_currentPhase = m_lastPhase;
	

	// Step ����
	m_pStep[STEP_INVENTORY] = CStepInventory::NewObject();
	m_pStep[STEP_INVENTORY]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pStep[STEP_INVENTORY]);

	m_pStep[STEP_SHOP] = CStepShop::NewObject();
	m_pStep[STEP_SHOP]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pStep[STEP_SHOP]);

	m_pStep[STEP_PROFILE] = CStepProfile::NewObject();
	m_pStep[STEP_PROFILE]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pStep[STEP_PROFILE]);

	m_pStep[STEP_CLAN_MEMBER] = CStepClanMember::NewObject();
	m_pStep[STEP_CLAN_MEMBER]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pStep[STEP_CLAN_MEMBER]);

	m_pStep[STEP_CLAN_NON_MEMBER] = CStepClanNonMember::NewObject();
	m_pStep[STEP_CLAN_NON_MEMBER]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pStep[STEP_CLAN_NON_MEMBER]);

	m_pStep[STEP_CLAN_BATTLE_LOBBY] = CStepClanBattleLobby::NewObject();
	m_pStep[STEP_CLAN_BATTLE_LOBBY]->SetEnable( FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pStep[STEP_CLAN_BATTLE_LOBBY]);	
	

	// Popup ����
	m_pPopup[POPUP_OPTION] = CPopupOption::NewObject();
	m_pPopup[POPUP_OPTION]->SetEnable( FALSE);
	m_pPopup[POPUP_OPTION]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_OPTION]);

	m_pPopup[POPUP_EXTENSION] = CPopupExtension::NewObject();
	m_pPopup[POPUP_EXTENSION]->SetEnable( FALSE);
	m_pPopup[POPUP_EXTENSION]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_EXTENSION]);

	m_pPopup[POPUP_COMMUNITY] = CPopupCommunity::NewObject();
	m_pPopup[POPUP_COMMUNITY]->SetEnable( FALSE);
	m_pPopup[POPUP_COMMUNITY]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_COMMUNITY]);

	m_pPopup[POPUP_SHOP_GIFT] = CPopupShopGift::NewObject();
	m_pPopup[POPUP_SHOP_GIFT]->SetEnable( FALSE);
	m_pPopup[POPUP_SHOP_GIFT]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_SHOP_GIFT]);

	m_pPopup[POPUP_CLAN_CREATE] = CPopupClanCreate::NewObject();
	m_pPopup[POPUP_CLAN_CREATE]->SetEnable( FALSE);
	m_pPopup[POPUP_CLAN_CREATE]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_CLAN_CREATE]);

	m_pPopup[POPUP_CLAN_SUBSCRIBER_INFO] = CPopupClanSubscriberInfo::NewObject();
	m_pPopup[POPUP_CLAN_SUBSCRIBER_INFO]->SetEnable( FALSE);
	m_pPopup[POPUP_CLAN_SUBSCRIBER_INFO]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_CLAN_SUBSCRIBER_INFO]);

	m_pPopup[POPUP_CHANGE_CLANMARK] = CPopupChangeClanMark::NewObject();
	m_pPopup[POPUP_CHANGE_CLANMARK]->SetEnable( FALSE);
	m_pPopup[POPUP_CHANGE_CLANMARK]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_CHANGE_CLANMARK]);

	m_pPopup[POPUP_CLAN_SEND_NOTE] = CPopupClanSendNote::NewObject();
	m_pPopup[POPUP_CLAN_SEND_NOTE]->SetEnable( FALSE);
	m_pPopup[POPUP_CLAN_SEND_NOTE]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_CLAN_SEND_NOTE]);

	m_pPopup[POPUP_USER_INFO] = CPopupUserInfo::NewObject();
	m_pPopup[POPUP_USER_INFO]->SetEnable( FALSE);
	m_pPopup[POPUP_USER_INFO]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_USER_INFO]);

	m_pPopup[POPUP_QUERY_PASSWORD] = CPopupQueryPassword::NewObject();
	m_pPopup[POPUP_QUERY_PASSWORD]->SetEnable( FALSE);
	m_pPopup[POPUP_QUERY_PASSWORD]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_QUERY_PASSWORD]);

	m_pPopup[POPUP_USER_DETAIL_INFO] = CPopupUserDetailInfo::NewObject();
	m_pPopup[POPUP_USER_DETAIL_INFO]->SetEnable( FALSE);
	m_pPopup[POPUP_USER_DETAIL_INFO]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_USER_DETAIL_INFO]);

	m_pPopup[POPUP_RANDOM_MAP] = CPopupRandomMap::NewObject();
	m_pPopup[POPUP_RANDOM_MAP]->SetEnable( FALSE);
	m_pPopup[POPUP_RANDOM_MAP]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_RANDOM_MAP]);

	m_pPopup[POPUP_LINK_SHOP] = CPopupLinkShop::NewObject();
	m_pPopup[POPUP_LINK_SHOP]->SetEnable( FALSE);
	m_pPopup[POPUP_LINK_SHOP]->SetVisible(FALSE);
	I3_GAMENODE_ADDCHILD(this, m_pPopup[POPUP_LINK_SHOP]);

#if defined	USE_AWAY_INPUT
	// AwayChecker ����
	m_pAwayChecker = CAwayChecker::NewObject();
	m_pAwayChecker->SetReceiver(this);
	m_pAwayChecker->Reset();
	I3_GAMENODE_ADDCHILD(this, m_pAwayChecker);

	//Room Master GameStart Timer After All User Ready
	CreateRoomMasterKick();	
#endif

	//	Ư������ �Է±� GUI
	{
		m_pSpCharInput = CSpecialCharInput::NewObject();
		m_pSpCharInput->Create(g_pGUIRoot);
		I3_GAMENODE_ADDCHILD(this, m_pSpCharInput);
	}

	// Gui �ڵ鷯 ����
	_LinkControl();

	// Gui ��Ʈ�� �ʱ�ȭ
	_InitControl();

	m_ToolTip.Init();
	
	// ĳ���� ������Ʈ - ���� ù �����̳�, �������� ������������ + Ŭ����ġ����ŷ(�κ�ο´�), Ʃ�丮�� ��忡�� �κ�� ������ ��
	if (PHASE_START == m_lastPhase || PHASE_READY_ROOM == m_lastPhase || (PHASE_LOBBY == m_lastPhase && g_pFramework->GetClanContext()->InBattleTeam()) 
		|| (PHASE_LOBBY == m_lastPhase && g_pGameContext->WasTutorial()) )
	{
		UpdateCharacter(VT_DEFAULT);
	}
	else
	{
		UpdateCharacter(VT_SLOT);
	}


	// Battle ��� ���ƿö� ģ���ʴ� �޽����� �ʱ�ȭ�Ѵ�
	g_pCommunity->ResetInviteFriend();

	//	Ŭ���� ���� ó��. Ŭ������ ��Ʋ�� ������ Ŭ�� �κ� (StepClanBattleLobby)�� �����Ѵ�.
	/*if( g_pFramework->GetClanContext()->InBattleTeam())
	{
		ReturnClanBattleLobby();
	}*/	

	//====================================================================================================
	I3NOTICE( "CStageReady::OnCreate() Leave");
}

BOOL CStageReady::OnFinish(void)
{
	BOOL bRv = i3Stage::OnFinish(); 

	g_pFramework->DestroyReadyStageImageRes(); 

	return bRv; 
}

BOOL CStageReady::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
	#if defined	USE_AWAY_INPUT
		switch(m_currentPhase)
		{
		case PHASE_CHANNEL_LIST:
			switch(event)
			{
			case ACE_AWAY:		g_pGameContext->SetEvent( EVENT_GAME_END ); return TRUE; // 2012.03.10 ���ؿ�. �α׾ƿ� ��û ��Ŷ ���� //GOTO_EXIT_STAGE();	return TRUE;
			case ACE_CAUTION:	STRING_POPUP(GAME_STRING("STR_TBL_GUI_READYROOM_WARNING_SELECT_SERVER"));/*5�� �̳��� ������ �������� ������ ������ ����˴ϴ�.*/	return TRUE;
			}
			break;
		case PHASE_READY_ROOM:
			switch(event)
			{
			case ACE_AWAY:			
				if(TRUE == Get_bGameStart()) 
				{
					StopRoomMasterKick();
					return TRUE;
				}
				m_bProcessLeaveRoom = TRUE;			
				TogglePopup(POPUP_NULL);
				m_oldAwayStep = m_currentStep;
				return TRUE;
			case ACE_CAUTION:
				{
					#ifdef USE_ROOM_MASTER_KICK
					if(TRUE == Get_bRoomMsterAwayCountDown())
					{
						m_bSetRoomMasterKick = TRUE;
						m_pAwayMessageBox = STRING_POPUP_W(GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_ROOM_MASTER_GET_AWAY"));/*���ѽð����� ������ �������� �ʾ� 5���� ���� ����˴ϴ�.*/	return TRUE;
					}
					else
					{
						m_pAwayMessageBox = STRING_POPUP_W(GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_GET_AWAY"));/*��ð� �������� �����Ƿ� 5�� �� �κ�� �̵��մϴ�.*/	return TRUE;
					}
					#else
					m_pAwayMessageBox = STRING_POPUP_W(GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_GET_AWAY"));/*��ð� �������� �����Ƿ� 5�� �� �κ�� �̵��մϴ�.*/	return TRUE;
					#endif
				}
				break;
			}
			break;
		}
	#endif

		if (I3_EVT_INPUT == event)
		{
	//		i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
	//		i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			//	Ư������ �Է±� komet
			if( m_pSpCharInput)
			{
				if(m_pSpCharInput->OnEvent(event, pObj, param2, code))
					return TRUE;
			}

			if (PS_PLAYING == m_phaseState)
			{
				for(UINT32 i = 0; i < MAX_POPUP; i++)
				{
					if (m_pPopup[i]->isEnable() && (POPUP_EXTENSION != i || m_pStep[STEP_INVENTORY]->IsRun()))
					{
						if (m_pPopup[i]->OnEvent(event, pObj, param2, code))
						{
							return TRUE;
						}
					}
				}

				if (STEP_NULL != m_currentStep)
				{
					if (m_pStep[m_currentStep]->OnEvent(event, pObj, param2, code))
					{
						return TRUE;
					}
				}

				if (m_pPhase[m_currentPhase]->OnEvent(event, pObj, param2, code))
				{
					return TRUE;
				}

				if (m_pChallenge->OnEvent(event, pObj, param2, code))
				{
					return TRUE;
				}
			}

			// I3_EVT_INPUT�� ��� ���̻� ó������ �ʴ´�.
			return FALSE;
		}
	}

	return i3Stage::OnEvent(event, pObj, param2, code);
}

void CStageReady::MovePhase(PhaseType phaseType)
{
	CClanContext * pClanContext = g_pFramework->GetClanContext();

	m_PhaseQueue.Add(reinterpret_cast<void*>(phaseType));

	pClanContext->SetVisibleLobbyPhase(TRUE);
	pClanContext->SetInClanBattleLobby(FALSE);
}


void CStageReady::ReturnClanBattleLobby(void)
{	//	���� Ŭ�� ��Ī�� �Ϸ��Ŀ� ��������� �Ȱ��� �ٷ� Ŭ���� �κ�� ���� ������ �� �Լ��� ���˴ϴ�
	CClanContext * pClanContext = g_pFramework->GetClanContext();

	m_PhaseQueue.Add(reinterpret_cast<void*>(PHASE_LOBBY));

	pClanContext->SetVisibleLobbyPhase(FALSE);	//	�κ� �Ⱥ��̹Ƿ� �κ񿡼� �ε� �˾� ����� �ʰ�
	pClanContext->SetInClanBattleLobby(TRUE);
	pClanContext->ClearRequestBattleTeam();

	//	ȣ�� ���� �߿�
	ChangeStep(STEP_CLAN_BATTLE_LOBBY);
}

void CStageReady::SetMenuBarInputDisable(BOOL bEnable)
{
	m_pMenuBar->SetInputDisable(bEnable);
}

void CStageReady::SetMenuClanInputDisable(BOOL bEnable)
{
	m_pMenuBar->SetClanInputDisable(bEnable);
}

void CStageReady::StartAlarmCommunity(void)
{
	if (FALSE == m_pPopup[POPUP_COMMUNITY]->isEnable())
	{
		m_pMenuBar->StartAlarmCommunity();
	}
}

void CStageReady::StartAlarmNote(void)
{
	if (FALSE == m_pPopup[POPUP_COMMUNITY]->isEnable())
	{
		m_pMenuBar->StartAlarmCommunity();
		((CPopupCommunity*)m_pPopup[POPUP_COMMUNITY])->StartAlarmNote();
	}
}

void CStageReady::ChangeStep(StepType stepType)
{
	// �ڽ��� �̹� �۵� ���̸� ó������ �ʴ´�.
	if (STEP_NULL != stepType && FALSE == m_pStep[stepType]->IsRun())
	{
		return;
	}

	// ���� ���� �����찡 �������̰ų� �������̸� ó������ �ʴ´�.
	if (STEP_NULL != m_currentStep && FALSE == m_pStep[m_currentStep]->IsRun())
	{
		return;
	}

	// �ͽ��ټ��˾��� �������̸�, �ݱ� �� �ڽ��� ȣ���� �������� ���ư���.
	// ���� �κ��丮, ���� �ͽ��ټ� �˾��� ȣ����.
	if (STEP_NULL == stepType && m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		if (m_bInventory)	
		{
			stepType = STEP_INVENTORY;
		}
		else if (m_bShop)
		{
			stepType = STEP_SHOP;
		}
	}

	// �κ��丮 �������� �Ѿ��.
	if (STEP_INVENTORY != m_currentStep && STEP_INVENTORY == stepType && FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		EnterInventory();
	}

	// �κ��丮 ���� ��Ŭ
	if (STEP_INVENTORY == m_currentStep && FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		if (m_bInventory)
		{
			LeaveInventory();
		}
	}

	// �ͽ��ټ� ������, �κ��丮�� ���ƿ�.
	if (STEP_INVENTORY != m_currentStep && STEP_INVENTORY != stepType && m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		if (m_bInventory)
		{		
			LeaveInventory();
		}
	}


	if (STEP_SHOP != m_currentStep && STEP_SHOP == stepType && FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
	{
			EnterShop();
	}

	if (STEP_SHOP == m_currentStep && FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		if (m_bShop)
		{
			LeaveShop();
		}
	}

	if (STEP_SHOP != m_currentStep && STEP_SHOP != stepType && m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		if (m_bShop)
		{
			LeaveShop();
		}
	}

	if (STEP_PROFILE != m_currentStep && STEP_PROFILE == stepType)
	{
		EnterInfo();
	}

	if (STEP_PROFILE == m_currentStep)
	{
		if (m_bInfo)
		{
			LeaveInfo();
		}
	}

	if (STEP_CLAN_MEMBER != m_currentStep && STEP_CLAN_MEMBER == stepType && FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		EnterClanMember();
	}

	if (STEP_CLAN_MEMBER == m_currentStep && FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		if (m_bClanMember)
		{
			LeaveClanMember();
		}
	}

	if (STEP_CLAN_MEMBER != m_currentStep && STEP_CLAN_MEMBER != stepType && m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		if (m_bClanMember)
		{		
			LeaveClanMember();
		}
	}

	if (STEP_CLAN_NON_MEMBER != m_currentStep && STEP_CLAN_NON_MEMBER == stepType && FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		EnterClanNonMember();
	}

	if (STEP_CLAN_NON_MEMBER == m_currentStep && FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		if (m_bClanNonMember)
		{
			LeaveClanNonMember();
		}
	}

	if (STEP_CLAN_NON_MEMBER != m_currentStep && STEP_CLAN_NON_MEMBER != stepType && m_pPopup[POPUP_EXTENSION]->isEnable())
	{
		if (m_bClanNonMember)
		{		
			LeaveClanNonMember();
		}
	}

	if (STEP_NULL != m_currentStep)
	{	
		m_pStep[m_currentStep]->OnEnd();		

		if (FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
		{
			if( (stepType == STEP_PROFILE || stepType == STEP_INVENTORY || 
				 stepType == STEP_SHOP || stepType == STEP_CLAN_NON_MEMBER || 
				 stepType == STEP_CLAN_MEMBER || stepType == STEP_CLAN_BATTLE_LOBBY) &&
				(m_currentStep == STEP_PROFILE || m_currentStep == STEP_INVENTORY || 
				 m_currentStep == STEP_SHOP || m_currentStep == STEP_CLAN_NON_MEMBER || 
				 m_currentStep == STEP_CLAN_MEMBER || m_currentStep == STEP_CLAN_BATTLE_LOBBY))
			{
			}
			else
			{
				MoveCamera(CL_IDLE);
			}
		}
	}
	else
	{
		if (FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
		{
			m_pPhase[m_currentPhase]->OnSlideOut(TRUE);			
		}
	}

	m_oldStep = m_currentStep;

	if (stepType == m_currentStep)
	{
		m_currentStep = STEP_NULL;
	}
	else
	{
		m_currentStep = stepType;
	}

	if( m_currentStep == STEP_NULL)
	{
		//	IDLE ī�޶�� ���ư��ϴ�.
		MoveCamera(CL_IDLE);
	}

	if (STEP_NULL != m_currentStep)
	{
		m_pStep[m_currentStep]->OnStart();		

		if (m_pPopup[POPUP_EXTENSION]->isEnable())
		{
			m_pPopup[POPUP_EXTENSION]->OnClosePopup();
		}

		if (STEP_PROFILE == m_currentStep)
		{
			m_currentLook = CL_RECORD;
		}
	}
	else
	{
		if (FALSE == m_pPopup[POPUP_EXTENSION]->isEnable())
		{
			m_pPhase[m_currentPhase]->OnSlideIn(TRUE);
		}
	}
}

void CStageReady::TogglePopup(PopupType popupType, void * pArg1, void * pArg2)
{
	if (POPUP_NULL == popupType)
	{
		for(UINT32 i = 0; i < MAX_POPUP; i++)
		{
			if (m_pPopup[i]->isEnable())
			{
				m_pPopup[i]->OnClosePopup(pArg1, pArg2);
			}
		}

		return;
	}

	if (m_pPopup[popupType]->isEnable())
	{		
		m_pPopup[popupType]->OnClosePopup(pArg1, pArg2);
	}
	else
	{
		for(UINT32 i = 0; i < MAX_POPUP; i++)
		{
			if (m_pPopup[i]->isEnable())
			{
				m_pPopup[i]->OnClosePopup(pArg1, pArg2);
			}
		}

		m_pPopup[popupType]->OnOpenPopup(pArg1, pArg2);
	}
}

void CStageReady::SetTooltip(UINT32 idxTooltip)
{
	m_ToolTip.Set(idxTooltip);
}

BOOL CStageReady::UpdateCharacter(ViewType viewType,BOOL bWeaponLimit, VIEW_CHARA_INFO * pNewChara)
{
	return m_pReadyBg->UpdateCharacter(viewType, bWeaponLimit, pNewChara);
}

void CStageReady::PlayCeremonyCharacter()
{
	m_pReadyBg->PlayCeremonyCharacter();
}

void CStageReady::MoveCamera(CameraLook lookType)
{
	m_currentLook = lookType;	
}

void CStageReady::SetLastPhaseLobby(void)
{
	m_lastPhase = PHASE_LOBBY;
}

void CStageReady::_GuiNotifyPump(void)
{
	// GuiRoot�� �ʱ�ȭ ���� �ʾҴٸ� �۵����� �ʴ´�
	if (NULL == m_pGuiRoot)
	{
		return;
	}


	// ���� Phase�� NULL�̸� �۵����� �ʴ´�
	if (NULL == m_pPhase[m_currentPhase])
	{
		return;
	}


	// �߻��� Notify�� �޾Ƴ��� �����Ѵ�
	I3GUI_CONTROL_NOTIFY notify;

	// PreNotify
	m_pChallenge->OnPreGuiNotify();
	m_pPhase[m_currentPhase]->OnPreGuiNotify();
	if (STEP_NULL != m_currentStep) m_pStep[m_currentStep]->OnPreGuiNotify();

	for(UINT32 i = POPUP_START; i < MAX_POPUP; i++)
	{
		if (m_pPopup[i]->isEnable())
		{
			m_pPopup[i]->OnPreGuiNotify();
		}
	}

	// Notify
	while(m_pGuiRoot->PopControlNotify(&notify))
	{
		m_pChallenge->OnGuiNotify(&notify);
		m_pPhase[m_currentPhase]->OnGuiNotify(&notify);		
		m_pMenuBar->OnGuiNotify(&notify);
		m_pContextMenu->OnGuiNotify(&notify);
		m_pSpCharInput->OnGuiNotify(&notify);

		if (STEP_NULL != m_currentStep) m_pStep[m_currentStep]->OnGuiNotify(&notify);

		for(UINT32 i = POPUP_START; i < MAX_POPUP; i++)
		{
			if (m_pPopup[i]->isEnable())
			{
				m_pPopup[i]->OnGuiNotify(&notify);
			}
		}
	}


	// PostNotify
	for(UINT32 i = POPUP_START; i < MAX_POPUP; i++)
	{
		if (m_pPopup[i]->isEnable())
		{
			m_pPopup[i]->OnPostGuiNotify();
		}
	}

	if (STEP_NULL != m_currentStep) m_pStep[m_currentStep]->OnPostGuiNotify();
	m_pPhase[m_currentPhase]->OnPostGuiNotify();
	m_pChallenge->OnPostGuiNotify();
	
}

void CStageReady::_GameEventPump(void)
{
	// ���� Phase�� NULL�̸� �۵����� �ʴ´�
	if (NULL == m_pPhase[m_currentPhase])
	{
		return;
	}


	// �߻��� GameContext �̺�Ʈ�� �޾Ƴ��� �����Ѵ�
	INT32 count = g_pGameContext->GetRecvEventCount();

	for(INT32 i = 0; i < count; i++)
	{
		INT32 event;
		INT32 arg;

		event = g_pGameContext->GetEventGame(&arg);
		m_pPhase[m_currentPhase]->OnGameEvent(event, arg);
	}
}

#if defined( FINDCTRL_CRC)
//��Ʈ�ѵ��� �̸����� ���Ͽ� ������ ��ũ�ϴºκ��� ���ϸ� �ְ��־.
//��Ʈ���� ���� crc�˻����� �ϱ����� binlist�� �־� �����صӴϴ�.
typedef struct findctrlcrcinfo
{
	i3List		* plist;
	STRCINFO	* pFirst;
	INT32		nCurCnt;
}FCRCIF;

static bool _SetSoundInfoProc( i3GameNode * pNode)
{
	if( !pNode->IsTypeOf( i3GuiObjBase::GetClassMeta()))
		return false;

	i3GuiObjBase * pObj = (i3GuiObjBase *)pNode;
	//i3ResourceManager * pResMng = g_pFramework->GetResourceManager();
	//I3ASSERT( pResMng != NULL);

	for( INT32 i = 0; i < I3GUI_CONTROL_STATE_RESERVED3; ++i)
	{
		I3GUI_EVENT_DATA * pEvent = pObj->getEventData( (GUI_CONTROL_STATE)i);
		if( pEvent != NULL && (pEvent->m_nEventStyle & I3GUI_EVENT_SOUND))
		{
			i3SoundNode * pNode = pEvent->m_pSoundNode;

			i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->FindResourceByOnlyName( pNode->GetName());
			if( pRes == NULL)
				continue;

			i3SoundPlayInfo * pInfo = pNode->getSoundPlayInfo();
			i3SoundPlayInfo * pResInfo = pRes->getSound();

			pInfo->SetDefaultVolume( pResInfo->GetDefaultVolume());
			pInfo->setListenerVolume( pResInfo->getListenerVolume());
		}
	}

	return true;
}

static bool _SetRCCountProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( pNode == NULL)
		return false;

	if( pNode->hasName())
	{
		INT32 * pCnt = (INT32*)pUserData;

		*pCnt = *pCnt+1;
	}

	//�߰���..���� �����ϴ� �κ� �����־����ϴ�.���� ���ν������Ӱ��� ������� �������ּ��� -raja-
	_SetSoundInfoProc( pNode);

	return true;
}


static bool _SetRCInfoAddProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( pNode == NULL)
		return false;

	if( pNode->hasName())
	{
		FCRCIF * pInfo = (FCRCIF*)pUserData;

		STRCINFO * pNewInfo = pInfo->pFirst + pInfo->nCurCnt;

		pNewInfo->crc = CRC32( 0xFFFFFFFF, (UINT8 *) pNode->GetName(), i3String::Length( pNode->GetName()));
		pNewInfo->pCtrl = pNode;

		pInfo->plist->Add( pNewInfo);

		pInfo->nCurCnt++;
	}

	return true;
}

static INT32 _ManagerCompProc( STRCINFO * p1, STRCINFO * p2)
{
	if( p1->crc > p2->crc)
		return 1;
	else if( p2->crc > p1->crc)
		return -1;
	else
		return 0;
	/*
	return p1->crc - p2->crc;
	*/
}

static INT32 _ManagerFindProc( STRCINFO * p1, UINT32 crc)
{
	if( p1->crc > crc)
		return 1;
	else if( crc > p1->crc)
		return -1;
	else
		return 0;

//	return p1->crc - crc;
}
/*
static INT32 _RecAddCtrlToList( i3BinList * list, i3GameNode * pObj)
{
	if( pObj == NULL)
		return -1;

	if( pObj->GetName() == NULL)
		return 0;

	STRCINFO * pNewInfo = (STRCINFO*)i3MemAlloc( sizeof(STRCINFO));

	pNewInfo->crc = CRC32( 0xFFFFFFFF, (UINT8 *) pObj->GetName(), i3String::Length( pObj->GetName()));
	pNewInfo->pCtrl = pObj;

	list->Add( pNewInfo);

	if( pObj->getFirstChild() != NULL)
	{
		_RecAddCtrlToList( list, pObj->getFirstChild());
	}

	if( pObj->getNext() != NULL)
	{
		_RecAddCtrlToList( list, pObj->getNext());
	}

	return 1;
}
*/
#endif

void CStageReady::_LinkControl(void)
{
	// ù��° ��Ʈ���� ���´�
	i3GuiWindow* pWindow = (i3GuiWindow*)m_pGuiRoot->getFirstChild();
	
	// �ڽ� ��Ʈ���� ���ٸ� �������� �ʴ´�
	if (NULL == pWindow)
	{
		return;
	}

	// �⺻ �������� ��Ʈ�ѵ��� ��ȸ�Ѵ�
	i3GuiControl* pControl = (i3GuiControl*)pWindow->getFirstChild();

#if defined( FINDCTRL_CRC)
	i3BinList CtrlList;
	FCRCIF inf;

	CtrlList.SetCompareProc((COMPAREPROC)_ManagerCompProc);

	{
		i3GameNodeTraversal * pTraverse = i3GameNodeTraversal::NewObject();

		INT32 nCnt = 0;
		
		pTraverse->setUserProc( _SetRCCountProc);
		pTraverse->setUserData((void*)&nCnt);

		pTraverse->Traverse( pWindow);

		I3ASSERT( nCnt > 0);
		
		inf.nCurCnt = 0;
		inf.pFirst	= (STRCINFO*)i3MemAlloc( sizeof( STRCINFO) * nCnt);
		inf.plist	= &CtrlList;

		pTraverse->setUserProc( _SetRCInfoAddProc);
		pTraverse->setUserData((void*)&inf);

		pTraverse->Traverse( pWindow);

		I3_SAFE_RELEASE( pTraverse);
	}

	//_RecAddCtrlToList( &CtrlList, pControl);
	CtrlList.SetCompareProc((COMPAREPROC)_ManagerFindProc);
	{
		// ��� ��Ʈ��
		m_pChallenge->OnLinkControl(&CtrlList, pControl);

		// ������ ��Ʈ�� �ʱ�ȭ
		for(UINT32 i = 0; i < MAX_PHASE; i++)
		{	
			m_pPhase[i]->OnLinkControl( &CtrlList, pControl);
		}


		// ���� ��Ʈ�� �ʱ�ȭ
		for(UINT32 i = 0; i < MAX_STEP; i++)
		{
			m_pStep[i]->OnLinkControl( &CtrlList, pControl);
		}

		for(UINT32 i = 0; i < MAX_POPUP; i++)
		{
			m_pPopup[i]->OnLinkControl( &CtrlList, pControl);
		}

		// �޴��� ��Ʈ�� �ʱ�ȭ
		m_pMenuBar->OnLinkControl( &CtrlList, pControl);

		m_pContextMenu->OnLinkControl(&CtrlList, pControl);

		//	Ư������ �Է±�
		m_pSpCharInput->OnLinkControl(&CtrlList, pControl);
	}

	I3MEM_SAFE_FREE_POINTER( inf.pFirst);
	CtrlList.Clear();

#else
	i3List	ControlBackUpList;	// Ʈ�� ��ȸ�� ���� ��� ����
	
	while(pControl)
	{
		// �̸��� �������� �ڵ鷯�� �Ҵ��Ѵ�
		if (pControl->GetName())
		{
			// �� ������� �ڵ鷯�� ã�´�
			BOOL bFinded = FALSE;

			// ��溰 �ڵ鷯
			if (m_pChallenge->OnLinkControl(NULL, pControl))
			{
				bFinded = TRUE;				
			}

			for(UINT32 i = 0; i < MAX_PHASE; i++)
			{	
				// �ڵ鷯�� �����ϰ� ã������ ���� ��Ʈ�ѷ� �̵�
				if (m_pPhase[i]->OnLinkControl(NULL, pControl))
				{
					bFinded = TRUE;
					break;
				}
			}

			// �� ���� ���� �ڵ鷯�� ã�´�
			for(UINT32 i = 0; FALSE == bFinded && i < MAX_STEP; i++)
			{
				if (m_pStep[i]->OnLinkControl(NULL, pControl))
				{
					bFinded = TRUE;
					break;
				}
			}

			// �� �˾� ���� �ڵ巯�� ã�´�
			for(UINT32 i = 0; FALSE == bFinded && i < MAX_POPUP; i++)
			{
				if (m_pPopup[i]->OnLinkControl(NULL, pControl))
				{
					bFinded = TRUE;
					break;
				}
			}

			// �޴��� �ڵ鷯�� ã�´�
			if (FALSE == bFinded)
			{
				if (m_pMenuBar->OnLinkControl(NULL, pControl))
				{
					bFinded = TRUE;
				}
			}

			// ContextMenu
			if (FALSE == bFinded)
			{
				if (m_pContextMenu->OnLinkControl(NULL, pControl))
				{
					bFinded = TRUE;
				}
			}

			//	Ư������ �Է±�
			if (FALSE == bFinded)
			{
				if (m_pSpCharInput->OnLinkControl(NULL, pControl))
				{
					bFinded = TRUE;
				}
			}
		}

		// ���� ��Ʈ��
		if( pControl->getFirstChild() != NULL)
		{
			if((i3GuiControl*)pControl->getNext() != NULL)
			{
				ControlBackUpList.Add( (i3GuiControl*)pControl->getNext());
			}
			pControl = (i3GuiControl*)pControl->getFirstChild();				
		}
		else
		{				
			pControl = (i3GuiControl*)pControl->getNext();
			if(pControl == NULL)
			{
				if( ControlBackUpList.GetCount() != 0)
				{
					pControl = (i3GuiControl*)ControlBackUpList.GetLast();
					ControlBackUpList.Delete(ControlBackUpList.GetCount());
				}
			}	
		}
	}
#endif

	
}

void CStageReady::_InitControl(void)
{
	I3NOTICE( "CStageReady::_InitControl() Enter");

	// ��� ��Ʈ�� �ʱ�ȭ
	m_pChallenge->OnInitControl();

	// ������ ��Ʈ�� �ʱ�ȭ
	for(UINT32 i = 0; i < MAX_PHASE; i++)
	{	
		m_pPhase[i]->OnInitControl();
	}


	// ���� ��Ʈ�� �ʱ�ȭ
	for(UINT32 i = 0; i < MAX_STEP; i++)
	{
		m_pStep[i]->OnInitControl();
	}

	for(UINT32 i = 0; i < MAX_POPUP; i++)
	{
		m_pPopup[i]->OnInitControl();
	}

	// �޴��� ��Ʈ�� �ʱ�ȭ
	m_pMenuBar->OnInitControl();

	// ContextMenu
	m_pContextMenu->OnInitControl();

	//	Ư������ �Է±�
	m_pSpCharInput->OnInitControl();

	I3NOTICE( "CStageReady::_InitControl() Leave");
}

void CStageReady::_CheckAwayInput(void)
{
#if defined	USE_AWAY_INPUT
	switch(m_currentPhase)
	{
	case PHASE_CHANNEL_LIST:	// ���� ���� �� Reset
		// Empty
		break;
	case PHASE_READY_ROOM:		// �Է��� ������ Restart
		if (g_pFramework->BeInput(TRUE))
		{
			m_pAwayChecker->Restart();
			if (m_pAwayMessageBox) 
			{
				g_pFramework->CloseMessageBox(m_pAwayMessageBox);
				m_pAwayMessageBox = NULL;
			}
		}
		break;
	default:
		// null;
		break;
	}
#endif	//USE_AWAY_INPUT
}



void CStageReady::_ProcessWheel(void)
{
	// ���콺 �ٹ�ư �˻�
	VEC3D mouseDelta;
	i3Vector::Copy(&mouseDelta, g_pFramework->getMouseMoveDelta());

	m_rWheelDelta += mouseDelta.z;

	INT32 scrollby = (INT32)(m_rWheelDelta / WHEEL_DELTA);
	
	// ������Ʈ ���� �ʾ����� ó������ �ʴ´�
	if (0 == scrollby)
	{
		return;
	}

	// ���� �ֿ켱 Ȱ��ȭ GuiWin���� ����
	BOOL bUpdated = FALSE;

	if(m_pSpCharInput->IsActive())
	{
		m_pSpCharInput->OnWheelUpdate(scrollby);
		bUpdated = TRUE;
	}

	if (FALSE == bUpdated)
	{
		for(UINT32 i = POPUP_START; i < MAX_POPUP; i++)
		{
			if (m_pPopup[i]->isEnable())
			{
				m_pPopup[i]->OnWheelUpdate(scrollby);
				bUpdated = TRUE;
			}
		}
	}

	if (FALSE == bUpdated)
	{
		if (STEP_NULL != m_currentStep)
		{
			m_pStep[m_currentStep]->OnWheelUpdate(scrollby);		
		}
		else
		{
			m_pPhase[m_currentPhase]->OnWheelUpdate(scrollby);		
		}	
	}

	// �� ��Ÿ �ʱ�ȭ - WHEEL_DELTA���� �۰� �������ٸ�, Ŀ�������� �����ؾ��Ѵ�
	if (WHEEL_DELTA <= m_rWheelDelta || -WHEEL_DELTA >= m_rWheelDelta)
	{
		m_rWheelDelta = 0.0f;
	}
}

BOOL CStageReady::IsSlidingInventory(void)
{
	return FALSE == m_pStep[STEP_INVENTORY]->IsRun();
}

VIEW_CHARA_INFO * CStageReady::GetViewCharaInfo(void)
{ 
	return (m_pReadyBg != NULL ? m_pReadyBg->GetViewCharaInfo() : NULL);	
}

void CStageReady::EnterInventory(void)
{
	CStepInventory* inven = (CStepInventory*)m_pStep[STEP_INVENTORY];
	inven->EnterInventory();
	m_bInventory = TRUE;
}

void CStageReady::LeaveInventory(void)
{
	CStepInventory* inven = (CStepInventory*)m_pStep[STEP_INVENTORY];
	inven->LeaveInventory();
	m_bInventory = FALSE;
}

BOOL CStageReady::IsEnterInventory(void)
{
	return m_bInventory;
}

void CStageReady::AuthStartSuccess(INT32 arg)
{	
	StepType type = GetCurrentStepType();

	if(type == STEP_NULL) {
		return;
	}

	CStepBase* pStep = (CStepBase*)m_pStep[type];
	pStep->AuthStartSuccess( arg);
}

void CStageReady::EnterShop(void)
{
	CStepShop* pStep = (CStepShop*)m_pStep[STEP_SHOP];
	pStep->EnterShop();
	m_bShop = TRUE;
}

void CStageReady::LeaveShop(void)
{
	CStepShop* pStep = (CStepShop*)m_pStep[STEP_SHOP];
	pStep->LeaveShop();
	m_bShop = FALSE;
}

BOOL CStageReady::IsEnterShop(void)
{
	return m_bShop;
}

void CStageReady::EnterInfo(void)
{
#if defined(ENABLE_MISSION_CARD)
	CStepProfile* info = (CStepProfile*)m_pStep[STEP_PROFILE];
	info->EnterInfo();
	m_bInfo = TRUE;
#endif
}

void CStageReady::LeaveInfo(void)
{
#if defined(ENABLE_MISSION_CARD)
	CStepProfile * info = (CStepProfile*)m_pStep[STEP_PROFILE];
	info->LeaveInfo();
	m_bInfo = FALSE;
#endif
}

void CStageReady::EnterClanMember(void)
{
	CStepClanMember* pStep= (CStepClanMember*)m_pStep[STEP_CLAN_MEMBER];
	pStep->EnterServer();
	m_bClanMember = TRUE;
}

void CStageReady::LeaveClanMember(void)
{
	CStepClanMember * pStep = (CStepClanMember*)m_pStep[STEP_CLAN_MEMBER];
	pStep->LeaveServer();
	m_bClanMember = FALSE;
}

void CStageReady::EnterClanNonMember(void)
{
	CStepClanNonMember* pStep = (CStepClanNonMember*)m_pStep[STEP_CLAN_NON_MEMBER];
	pStep->EnterServer();
	m_bClanNonMember = TRUE;
}

void CStageReady::LeaveClanNonMember(void)
{
	CStepClanNonMember * pStep = (CStepClanNonMember*)m_pStep[STEP_CLAN_NON_MEMBER];
	pStep->LeaveServer();
	m_bClanNonMember = FALSE;
}

BOOL CStageReady::IsEnterInfo(void)
{
	return m_bInfo;
}

BOOL CStageReady::IsEnterClanMember(void)
{
	return m_bClanMember;
}

BOOL CStageReady::IsEnterClanNonMember(void)
{
	return m_bClanNonMember;
}

void CStageReady::ExclusedEnter(StepType stepType)
{
	BOOL bImmediatelyEnterEvent = TRUE;

	// â�� �ִٸ� â������
	if (m_bInventory && stepType != STEP_INVENTORY)
	{
		LeaveInventory();
		bImmediatelyEnterEvent = FALSE;
	}

	// ������ �ִٸ� ��������
	if (m_bShop && stepType != STEP_SHOP)
	{
		LeaveShop();		
		bImmediatelyEnterEvent = FALSE;
	}

	// ���������⿡ �ִٸ� ���������� ����
	if (m_bInfo && stepType != STEP_PROFILE)
	{
		LeaveInfo();
		bImmediatelyEnterEvent = FALSE;
	}

	// ��Ŭ������������ �ִٸ� ��Ŭ���������� ����
	if (m_bClanNonMember && stepType != STEP_CLAN_NON_MEMBER)
	{
		LeaveClanNonMember();
		bImmediatelyEnterEvent = FALSE;
	}

	// Ŭ���������� �ִٸ� Ŭ�������� ����
	if (m_bClanMember && stepType != STEP_CLAN_MEMBER)
	{
		LeaveClanMember();
		bImmediatelyEnterEvent = FALSE;
	}


	// ���� �̺�Ʈ�� ���ٸ� ��� ���� �̺�Ʈ ����
	if (bImmediatelyEnterEvent)
	{
		switch(stepType)
		{
		case STEP_INVENTORY:
			g_pGameContext->SetEventOnce(EVENT_ENTER_INVENTORY);
			break;
		case STEP_SHOP:
			g_pGameContext->SetEventOnce(EVENT_ENTER_SHOP);
			break;
		case STEP_PROFILE:
			g_pGameContext->SetEventOnce(EVENT_ENTER_INFO);
			break;
		case STEP_CLAN_NON_MEMBER:	// Fall through
		case STEP_CLAN_MEMBER:
			g_pFramework->GetClanContext()->EnterServer();
			break;
		default:
			// Do nothing
			break;
		}

		if (stepType != STEP_NULL)
		{
			i3GuiRoot::setGuiDisableInput(TRUE);
		}

		m_ExclusedEnterEventStep = STEP_NULL;
	}
	else
	{
		// ���� �̺�Ʈ�� �ִٸ�, ��������ñ��� ���
		// ���� ��, EnterDelayedEventStep()�� ���� ����
		m_ExclusedEnterEventStep = stepType;
	}
}

void CStageReady::EnterDelayedEventStep(void)
{
	switch(m_ExclusedEnterEventStep)
	{
	case STEP_INVENTORY:
		g_pGameContext->SetEventOnce(EVENT_ENTER_INVENTORY);
		break;
	case STEP_SHOP:
		g_pGameContext->SetEventOnce(EVENT_ENTER_SHOP);
		break;
	case STEP_PROFILE:
		g_pGameContext->SetEventOnce(EVENT_ENTER_INFO);
		break;
	case STEP_CLAN_NON_MEMBER:	// Fall through
	case STEP_CLAN_MEMBER:
		g_pFramework->GetClanContext()->EnterServer();
		break;
	default:
		// Do nothing
		break;
	}

	m_ExclusedEnterEventStep = STEP_NULL;
}

void CStageReady::OnInviteMove(void)
{
	if (g_pCommunity->StartInviteMove())
	{
		CPhaseBase * pPhase = m_pPhase[m_currentPhase];

		if ( ! pPhase->OnInviteMove() )
		{
			I3TRACE("[ERROR] CStageReady::OnInviteMove() - Wrong position\n");
			g_pCommunity->ComplateInviteMove();
		}
	}

	g_pCommunity->ResetInviteFriend();
}

void CStageReady::OnInviteMoveClan(void)
{
	if (g_pCommunity->StartInviteMoveClan())
	{
		CPhaseBase * pPhase = m_pPhase[m_currentPhase];
		
		if ( ! pPhase->OnInviteMove() )
		{
			I3TRACE("[ERROR] CStageReady::OnInviteMove() - Wrong position\n");
			g_pCommunity->ComplateInviteMove();			
		}
	}	

	g_pCommunity->ResetInviteFriend();
}

void CStageReady::OnChangeVideo( i3VideoInfo * pInfo)
{
	// û�ҳ��̿밡 ��ġ ������
	m_pPhase[PHASE_CHANNEL_LIST]->OnChangeVideo(pInfo);


	m_pReadyBg->OnChangeVideo( pInfo);
	m_ToolTip.OnChangeVideo();

}

void CStageReady::TransferrToPopupGameEvent( INT32 event, INT32 Arg)
{
	for(UINT32 i = 0; i < MAX_POPUP; i++)
	{
		if (m_pPopup[i]->isEnable())
		{
			m_pPopup[i]->OnGameEvent(event, Arg);
		}
	}
}

void CStageReady::TransferrToStepGameEvent( INT32 event, INT32 Arg)
{
	for(UINT32 i = 0; i < MAX_STEP; i++)
	{
		if (m_pStep[i]->isEnable())
		{
			m_pStep[i]->OnGameEvent(event, Arg);
		}
	}
}

BOOL CStageReady::IsEnablePopup( PopupType type)
{
	return m_pPopup[type]->isEnable();
}

void CStageReady::ContextMenuReset(void)
{
	m_pContextMenu->Reset();
}

void CStageReady::ContextMenuOpen(i3EventReceiver* observer,INT32 requestId)
{
	m_pContextMenu->Open(observer, requestId);
}

void CStageReady::ContextMenuAddCommand(ContextMenuCommand command)
{
	m_pContextMenu->AddCommand(command);
}

#if defined(CHANGE_ADDCOMMAND_METHOD)
void CStageReady::CopyContextMenuList(INT32 MenuList[64])
{
	UINT32 Count = m_pContextMenu->GetCommandCount();
	ContextMenuCommand *	pCmd = m_pContextMenu->GetCommandMap();
	for ( UINT32 i = 0 ; i < Count ; ++i )
	{
		MenuList[i] = pCmd[i];
	}
}
#endif

void CStageReady::SetPushedStateButton(INT32 menuButtonType,BOOL bPushed)
{
	if( m_pMenuBar)
		m_pMenuBar->SetPushedStateButton(menuButtonType, bPushed);
}

BOOL CStageReady::IsPopup_Option(void)
{
	return m_pPopup[POPUP_OPTION]->isEnable();
}

void CStageReady::Popup_OptionClosed(void)
{
	((CPopupOption*)m_pPopup[POPUP_OPTION])->OnClosePopup();
}

void CStageReady::_BackgroupEnable(void)
{
	// ���� GUI Ȱ��ȭ ���� ó��
	BOOL bEnable = FALSE;

	if (m_currentPhase == PHASE_LOBBY || m_currentPhase == PHASE_READY_ROOM)
	{
		if (m_currentStep != STEP_CLAN_NON_MEMBER &&
			m_currentStep != STEP_CLAN_MEMBER &&
			m_currentStep != STEP_CLAN_BATTLE_LOBBY)
		{
			bEnable = TRUE;
		}
	}
	
#if defined( CHAMPETITION_BUILD )
		m_pChallenge->OnClose();
		m_pChallenge->SetEnable(FALSE);
		return;
#endif

	if (bEnable)
	{
		m_pChallenge->OnOpen();
		m_pChallenge->SetEnable(TRUE);
	}
	else
	{
		m_pChallenge->OnClose();
		m_pChallenge->SetEnable(FALSE);
	}
}

void CStageReady::GetCurrentSeverInfo(char * pszString, const size_t sizeStringMax)
{
	I3ASSERT( pszString != NULL);

	//	���� ���� ���	
	INT32 serverNumber = g_pGameContext->GetServerNumber();
	ChannelType channelType = g_pGameContext->GetConnectedChannelType();

	switch(channelType)
	{
	case CHANNEL_TYPE_NORMAL:
	case CHANNEL_TYPE_NORMAL2:
		sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_NORMAL]);//�Ϲݼ��� %d
		break;
	case CHANNEL_TYPE_BEGIN1:
		sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_BEGINNER]);//{col:79, 184, 0, 255}�ʺ�����%d{/col}
		break;
	case CHANNEL_TYPE_BEGIN2:
		sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_BEGINNER]);//{col:79, 184, 0, 255}�ʺ�����%d{/col}
		break;
	case CHANNEL_TYPE_CLAN:
		sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_CLAN]);//{col:255,125,0,255}Ŭ������%d{/col}
		break;
	case CHANNEL_TYPE_EXPERT: 
		sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_EXPERT]);//{col:205,0,0,255}�������%d{/col}
		break; 
	case CHANNEL_TYPE_EXPERT2: 
		sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_EXPERT]);//{col:205,0,0,255}�������%d{/col}
		break;
	case CHANNEL_TYPE_CHAMPIONSHIP:
		sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_CHAMPIONSHIP]);
		break;
	case CHANNEL_TYPE_NATIONAL:
		sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_NATIONAL]);
		break;
	case CHANNEL_TYPE_MIDDLE:
		sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_MIDDLE]);
		break;
	default:
		{
			INT32 nEvent = 0;
			sprintf_s(pszString, sizeStringMax, "%03d %s", serverNumber, m_szServerSlotText[SERVERSELECT_MAINTYPE_EVENT0 + nEvent]);
		}
		break;
	}
}

void CStageReady::OnRevive( i3RenderContext * pCtx)
{
	//	���յ� Ŭ����ũ �ؽ�ó ����
	g_pClanMarkManager->OnRevive( pCtx);
}

//////////////////////////////////////////////////////////////////////////
//Room Master GameStart Timer After All User Ready
void CStageReady::InitRoomMasterKick()
{
#ifdef USE_ROOM_MASTER_KICK
	ResetRoomMasterKick();	
	Set_bGameStart(FALSE);
	m_bSetRoomMasterKick = FALSE;	

	m_pRoomMasterAwayChecker = NULL;
	//i3mem::FillZero(m_pRoomMasterCountdownSound, sizeof(m_pRoomMasterCountdownSound));
	m_pRoomMasterCountdownSound = NULL;
#endif
}

void CStageReady::CreateRoomMasterKick()
{
#ifdef USE_ROOM_MASTER_KICK
	ResetRoomMasterKick();			
	Set_bGameStart(FALSE);
	m_bSetRoomMasterKick = FALSE;	

	m_pRoomMasterAwayChecker = CAwayChecker::NewObject();
	m_pRoomMasterAwayChecker->SetReceiver(this);
	m_pRoomMasterAwayChecker->Reset();
	I3_GAMENODE_ADDCHILD(this, m_pRoomMasterAwayChecker);

	const char* soundFileName =
	{
		"UI_Count_A.wav"
	};

	m_pRoomMasterCountdownSound = i3SoundPlayInfo::NewObject();

	i3GameResSound* pResSound = (i3GameResSound*)g_pFramework->FindResourceByOnlyName(soundFileName);
	I3ASSERT(pResSound != NULL);

	i3SoundPlayInfo* pSoundInfo = pResSound->getSound();
	pSoundInfo->CopyTo(m_pRoomMasterCountdownSound, I3_COPY_INSTANCE);
#endif
}

void CStageReady::RemoveRoomMasterKick()
{
#ifdef USE_ROOM_MASTER_KICK
	I3_SAFE_RELEASE(m_pRoomMasterCountdownSound);
	
	if (m_pRoomMasterAwayChecker)
	{
		RemoveChild(m_pRoomMasterAwayChecker);
		I3_SAFE_RELEASE(m_pRoomMasterAwayChecker);
	}
#endif
}

void CStageReady::PauseRoomMasterKick()
{
#ifdef USE_ROOM_MASTER_KICK
	m_bPS_PLAYING = TRUE;
#endif
}

void CStageReady::CheckRoomMasterKick()
{
#ifdef USE_ROOM_MASTER_KICK
	if(TRUE == Get_bGameStart())
	{
		StopRoomMasterKick();
	}

	if (TRUE == Get_bPS_PLAYING())
	{
		ResetRoomMasterKick();		
		m_bSetRoomMasterKick = FALSE;

		m_pRoomMasterAwayChecker->SetTime(AWAY_INPUT_ROOM_MASTER_GAMESTART_TIME + AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION, AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION);
	}

	if (TRUE == Get_bAllUserReady())
	{
		if(0.0f == m_pRoomMasterAwayChecker->GetCurrentTime())
		{
			PauseRoomMasterKick();
			return;
		}

		m_RoomMasterAwayCheckerTime = 0;		
		Set_bAllUserReady(FALSE);
		Set_bRoomMsterAwayCountDown(TRUE);

		m_pRoomMasterAwayChecker->Restart();
		if (m_pAwayMessageBox) 
		{
			g_pFramework->CloseMessageBox(m_pAwayMessageBox);
			m_pAwayMessageBox = NULL;
		}

		m_RoomMasterAwayCheckerTime = INT32(m_pRoomMasterAwayChecker->GetCurrentTime());

		char countdownText[MAX_STRING_COUNT];
		i3String::Format(countdownText, MAX_STRING_COUNT, GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_PRE_ROOM_MSTER_KICK_WARNING"));/*{col:255,120,0,255}10�� ���� ������ �������� ������ ������ ���� ����˴ϴ�.*/
		g_pFramework->getChatBox()->PutSystemChatting(countdownText);
	}		

	if (TRUE == Get_bRoomMsterAwayCountDown())
	{
		// ä��â�� ��� �޽��� ǥ��
		if ( m_pRoomMasterAwayChecker->GetCurrentTime() < m_RoomMasterAwayCheckerTime)
		{
			m_RoomMasterAwayCheckerTime = INT32(m_pRoomMasterAwayChecker->GetCurrentTime());

			if ((INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION - 1.0f) <= m_RoomMasterAwayCheckerTime)// && (INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_TIME + AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION) > m_RoomMasterAwayCheckerTime)
			{
				char countdownText[MAX_STRING_COUNT];
				i3String::Format(countdownText, MAX_STRING_COUNT, GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_PRE_ROOM_MSTER_KICK_ALARM"), m_RoomMasterAwayCheckerTime - (INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION - 1.0f));/*{col:255,120,0,255}���� ���� %d�� ��{/col}*/
				g_pFramework->getChatBox()->PutSystemChatting(countdownText);

				i3SoundContext* pSoundContext = g_pViewer->GetSoundContext();
				if( pSoundContext != NULL)
				{
					pSoundContext->Play(m_pRoomMasterCountdownSound);
				}
			}
		}
	}
#endif
}

void CStageReady::StopRoomMasterKick()
{
#ifdef USE_ROOM_MASTER_KICK
	m_pRoomMasterAwayChecker->Reset();		

	ResetRoomMasterKick();	
#endif
}

void CStageReady::ResetRoomMasterKick()
{
#ifdef USE_ROOM_MASTER_KICK
	Set_bPS_PLAYING(FALSE);
	Set_bAllUserReady(FALSE);	
	Set_bRoomMsterAwayCountDown(FALSE);		
	m_RoomMasterAwayCheckerTime = 0;
#endif
}

#if defined (USE_LUA_UI)
void CStageReady::RegisterLuaFunction( lua_State * pState)
{
	GameContextScript::Init( pState);
}
#endif


void CStageReady::SetExitMsgBoxOffset( INT32 x, INT32 y )
{
	if( m_pMenuBar)
		m_pMenuBar->SetExitMsgBoxOffset(x, y);
}


void CStageReady::UpdateEffectTime(RT_REAL32 rTimes)
{
	if(BeginEffect)
	{
		m_ActiveTime += rTimes;
		if( m_ActiveTime > 1.0f)
		{
			m_ActiveTime		= 0.0f;
			Button_Effect_State = !Button_Effect_State;
		}
	}
	else
	{
		Button_Effect_State = FALSE;
	}
}

void CStageReady::EnterNickSpace()	
{
	if( m_pMenuBar)
		m_pMenuBar->EnterNickSpace();
}

void CStageReady::OutroNickSpace()	
{	
	if( m_pMenuBar)
		m_pMenuBar->OutroNickSpace();
}