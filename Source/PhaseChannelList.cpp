#include "pch.h"
#include "PhaseChannelList.h"
#include "GlobalVariables.h"
#include "StageReady.h"
#include "Game.h"
#include "CommunityContext.h"
#include "ServerInfo.h"
#include "MessageBox.h"


//EXPORT_AVALIABLE_CODE_VALUE
#define		REFRESH_LIMIT_TIME		5.0f
#define		CONNECT_LIMIT_TIME		1.0f
#define		WAIT_CONNECT_TIME		1.0f



INT32	LastServerNumber	= INVALID_INDEX;
INT32	LastChannelNumber	= INVALID_INDEX;


ChannelSlot::ChannelSlot()
{
	Index = -1;

	_pColumn = NULL;
	_pSlot = NULL;
	_pNumber = NULL;
	_pChannel = NULL;
	_pGage = NULL;
}



I3_CLASS_INSTANCE(CPhaseChannelList, CPhaseBase);

CPhaseChannelList::CPhaseChannelList()
: CPhaseBase()
{
	m_pChannelGui = NULL;
	m_pChannelWin = NULL;
	m_pChannelCaption = NULL;

	m_pPrev = NULL;
	m_pRefresh = NULL;
	m_pEnter = NULL;
	m_pNext = NULL;
	m_pPrevCaption = NULL;
	m_pRefreshCaption = NULL;
	m_pEnterCaption = NULL;
	m_pNextCaption = NULL;

	m_pSelect = NULL;
	m_pFocus = NULL;



	i3mem::FillZero(m_pChannelLabel, sizeof(m_pChannelLabel));
	i3mem::FillZero(m_pChannelLabelKey, sizeof(m_pChannelLabelKey));
	i3mem::FillZero(m_pServerSelectNumber, sizeof(m_pServerSelectNumber));

	m_pVerifiTexture = NULL;
	m_pVerifiSprite = NULL;


	m_nLastedSelectSlot = -1;

	m_fRefreshClickElapseTime = 0.0f;
	m_fConnectClickElapseTime = 0.0f;
	m_fWaitDelayTime = WAIT_CONNECT_TIME;
	m_bWaitConnect = FALSE;

	m_pMBConnectWait = NULL;



	m_rChannelWinX = 0.0f;


	m_nServerChannelIdx = 0;
	i3mem::FillZero(m_szServerText, sizeof(m_szServerText));

	m_LogInConnectFailedCount = 0;


	m_pGotoServerList			= NULL;
	m_hwnd = NULL;
}

CPhaseChannelList::~CPhaseChannelList()
{

	if (m_pVerifiSprite)
	{
		g_pFramework->getGuiLayer()->RemoveChild(m_pVerifiSprite);
		I3_SAFE_RELEASE(m_pVerifiSprite);
	}

	I3_SAFE_RELEASE(m_pVerifiTexture);

	if( g_pSndMng != NULL)
	{
		g_pSndMng->StopBGM();
	}
}

void CPhaseChannelList::OnGameEvent(INT32 event,INT32 arg)
{
	CPhaseBase::OnGameEvent(event, arg);

	switch(event)
	{
#if defined(CHANGE_SERVER_MOVE)
#else
	case EVENT_CONNECT_SERVER_GAME:
		// ����Ʈ���� -> ���Ӽ��� ���ؽÿ��� �߻��ϴ� �̺�Ʈ.
		if( EV_SUCCESSED(arg))
		{
			g_pGameContext->SetEventOnce(EVENT_USER_ENTER);
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			if( EV_IS_ERROR(arg, EVENT_ERROR_FAIL))
				_EhFailConnect();
			else
				_EhStageError(event,arg);
		}
		break;
	case EVENT_USER_LEAVE:
		InputDisable(FALSE);

		if( EV_SUCCESSED(arg) )
		{
			g_pGameContext->SetEvent(EVENT_DISCONNECT);

			if (g_pCommunity->IsInviteMoving())
			{
				g_pGameContext->SetEvent(EVENT_CONNECT_SERVER_GAME);
			}
			else
			{
				((CStageReady *)m_pParent)->MovePhase(PHASE_SERVER_LIST);
			}
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			ERROR_POPUP(GAME_STRING("STBL_IDX_EP_SERVER_FAIL_MOVE"));
		}
		break;
#endif

	case EVENT_GET_CHANNEL_LIST:
		if( EV_SUCCESSED(arg))
		{
			_CloseWaitMessageBox();

			_UpdateChannelLabel();


			if (g_pCommunity->IsInviteMoving())
			{
				OnInviteMove();
			}
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			_CloseWaitMessageBox();
			g_pGameContext->NetworkDisconnect();
			SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_FAIL_CONNECT_E"), EXIT_CALLBACK(CbExitNormal), this);
		}
		break;
	case EVENT_USER_ENTER:
		InputDisable(FALSE);
		if( EV_SUCCESSED(arg) )
		{
			g_pGameContext->SetEvent(EVENT_GET_CHANNEL_LIST);
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			_CloseWaitMessageBox();
#if defined(CHANGE_SERVER_MOVE)
#else
			g_pGameContext->SetEvent(EVENT_DISCONNECT);
#endif
			((CStageReady *)m_pParent)->MovePhase(PHASE_SERVER_LIST);
			SYSTEM_POPUP(GAME_STRING("STBL_IDX_EP_SERVER_FAIL_MOVE"));
		}
		break;
	case EVENT_SELECT_CHANNEL:
		if( EV_SUCCESSED(arg))
		{
			g_pGameContext->SetEventOnce(EVENT_ENTER_LOBBY);
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			_CloseWaitMessageBox();

			InputDisable(FALSE);

			if (EV_IS_ERROR(arg, EVENT_ERROR_SERVER_USER_FULL))
			{
				ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_SERVER_USER_FULL_C"), MAKE_CALLBACK(CbRefleshServerList), this);
			}
			else if (EV_IS_ERROR(arg, EVENT_ERROR_CHANNEL_ENTER_CONDITION))
			{
				ChannelType channeltype = g_pGameContext->GetConnectedChannelType();
				PrintMessageForChannelEnterError(channeltype);
			}
			else
			{
				_EhStageError(event,arg);
			}
		}
		break;
	case EVENT_ENTER_LOBBY:
		if( EV_SUCCESSED(arg))
		{
			_CloseWaitMessageBox();
			((CStageReady*)m_pParent)->MovePhase(PHASE_LOBBY);
			LastChannelNumber	= g_pServerInfo->GetCurChannelNumber();
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			_CloseWaitMessageBox();
			_EhStageError(event,arg);

			InputDisable(FALSE);
		}
		break;
	case EVENT_LOG_OUT:
		// null
		break;
	case EVENT_M_DRIVE_AWAY:
		_CloseWaitMessageBox();
		break;
	case EVENT_M_ERROR:
		_CloseWaitMessageBox();
		break;
	case EVENT_M_DISCONNECTION:
		_CloseWaitMessageBox();
		break;
	case EVENT_M_GAMEGUARD_ERROR:
		_CloseWaitMessageBox();
		break;
	case EVENT_M_DESTROYED_NETWORK:	// ��Ʈ��ũ�� ��������
		if (g_pCommunity->IsInviteMoving())
		{
			g_pCommunity->ComplateInviteMove();
		}

#if defined(CHANGE_SERVER_MOVE)
		((CStageReady *)m_pParent)->MovePhase(PHASE_SERVER_LIST);
#else
		_EhFailConnect();
#endif

		DEV_LOG(" CPhaseChannelList::OnGameEvent EVENT_M_DESTROYED_NETWORK\n");
		break;
	case EVENT_INVITED_MESSAGE:
		CPhaseBase::OnInvitedMessage();
		break;
	case EVENT_INVITED_CLAN_MESSAGE:
		CPhaseBase::OnInvitedMessageClan();
		break;
	default:
		{

#if defined I3_DEBUG	//������ ���� ó��????
			_CloseWaitMessageBox();
			g_pGameContext->SetEvent( EVENT_DISCONNECT );
			char bug[256];
			sprintf_s(bug, GAME_STRING("STR_TBL_GUI_SERVER_DEBUG_POPUP1"), event);/*����� �˾�: �̺�Ʈ����(%d)\n�����: praptor*/
			g_pFramework->MessageBox(bug, NULL, MBF_GAME_OK);
#endif
			DEV_LOG("ChannelList event error : %d %d", event, arg);

			I3ASSERT_0;//ó������ EVENT�� �����մϴ�. event���� �˷��ּ��� - praptor
		}

		break;
	}
}

void CPhaseChannelList::OnPreGuiNotify(void)
{
	m_pFocus->SetEnable(FALSE);
}

void CPhaseChannelList::BackToServerList()
{
	DEV_LOG("CPhaseChannelList::BackToServerList");
#if defined(CHANGE_SERVER_MOVE)
	((CStageReady *)m_pParent)->MovePhase(PHASE_SERVER_LIST);
#else
	InputDisable(TRUE);
	g_pGameContext->SetEventOnce(EVENT_USER_LEAVE);
#endif
}

void	CPhaseChannelList::InputDisable(BOOL Val)
{
	i3GuiRoot::setGuiDisableInput(Val);
	m_pChannelWin->setInputDisable(Val);
}

void CPhaseChannelList::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
    switch(pNotify->m_nID)
    {
    case GCI_SSP_B_REFRESH:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnRefresh();
        break;
	case GCI_SSP_B_ENTER:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnConnect();
        break;
	case GCI_SSP_S_GOTO_SERVERLIST:
		if ( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent )
		{
			BackToServerList();
		}
		break;
	case GCI_SSP_S_SLOT_1:	// fall through
	case GCI_SSP_S_SLOT_2:	// fall through
	case GCI_SSP_S_SLOT_3:	// fall through
	case GCI_SSP_S_SLOT_4:	// fall through
	case GCI_SSP_S_SLOT_5:	// fall through
	case GCI_SSP_S_SLOT_6:	// fall through
	case GCI_SSP_S_SLOT_7:	// fall through
	case GCI_SSP_S_SLOT_8:	// fall through
	case GCI_SSP_S_SLOT_9:	// fall through
	case GCI_SSP_S_SLOT_10: // fall through
	case GCI_SSP_S_SLOT_11: // fall through
	case GCI_SSP_S_SLOT_12: // fall through
	case GCI_SSP_S_SLOT_13:	// fall through
	case GCI_SSP_S_SLOT_14:	// fall through
	case GCI_SSP_S_SLOT_15:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent)
			OnFocus(pNotify->m_nID - GCI_SSP_S_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)
			OnSelect(pNotify->m_nID - GCI_SSP_S_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)
			OnConnect(pNotify->m_nID - GCI_SSP_S_SLOT_1);
		break;
	case GCI_SSP_S_SELECET_NUMBER_1:
	case GCI_SSP_S_SELECET_NUMBER_2:
	case GCI_SSP_S_SELECET_NUMBER_3:
	case GCI_SSP_S_SELECET_NUMBER_4:
	case GCI_SSP_S_SELECET_NUMBER_5:
	case GCI_SSP_S_SELECET_NUMBER_6:
	case GCI_SSP_S_SELECET_NUMBER_7:
	case GCI_SSP_S_SELECET_NUMBER_8:
	case GCI_SSP_S_SELECET_NUMBER_9:
	case GCI_SSP_S_SELECET_NUMBER_10:
	case GCI_SSP_S_SELECET_NUMBER_11:
	case GCI_SSP_S_SELECET_NUMBER_12:
	case GCI_SSP_S_SELECET_NUMBER_13:
	case GCI_SSP_S_SELECET_NUMBER_14:
	case GCI_SSP_S_SELECET_NUMBER_15:
	case GCI_SSP_S_SELECET_NUMBER_16:
	case GCI_SSP_S_SELECET_NUMBER_17:
	case GCI_SSP_S_SELECET_NUMBER_18:
	case GCI_SSP_S_SELECET_NUMBER_19:
	case GCI_SSP_S_SELECET_NUMBER_20:
		I3ASSERT_0;
		//if (I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)
		//	OnServerSelectByIndex(pNotify->m_nID - GCI_SSP_S_SELECET_NUMBER_1);
		break;
	}

	m_AnnounceFrame.OnGuiNotify(pNotify);
}

BOOL CPhaseChannelList::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "Server", m_pChannelGui, -1);
	LinkControlEx(plist, "ssServerWin", m_pChannelWin, -1);
	LinkControlEx(plist, "ssServerCaption", m_pChannelCaption, -1);

	LinkControlEx(plist, "ssPrev", m_pPrev, GCI_SSP_B_PREV);
	LinkControlEx(plist, "ssRefresh", m_pRefresh, GCI_SSP_B_REFRESH);
	LinkControlEx(plist, "ssEnter", m_pEnter, GCI_SSP_B_ENTER);
	LinkControlEx(plist, "ssNext", m_pNext, GCI_SSP_B_NEXT);
	LinkControlEx(plist, "ssPrevCaption", m_pPrevCaption, -1);
	LinkControlEx(plist, "ssRefreshCaption", m_pRefreshCaption, -1);
	LinkControlEx(plist, "ssEnterCaption", m_pEnterCaption, -1);
	LinkControlEx(plist, "ssNextCaption", m_pNextCaption, -1);

	for(UINT32 i = 0; i < MAX_CHANNEL_CONTROL_COUNT; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "ssColumn%d", i + 1);
		LinkControlEx(plist, strName, m_ChannelSlot[i]._pColumn, -1);

		sprintf_s(strName, "ssSlot%d", i + 1);
		LinkControlEx(plist, strName, m_ChannelSlot[i]._pSlot, GCI_SSP_S_SLOT_1 + i);

		sprintf_s(strName, "ssSlotNumber%d", i + 1);
		LinkControlEx(plist, strName, m_ChannelSlot[i]._pNumber, -1);

		sprintf_s(strName, "ssSlotServer%d", i + 1);
		LinkControlEx(plist, strName, m_ChannelSlot[i]._pChannel, -1);

		sprintf_s(strName, "ssSlotUserGage%d", i + 1);
		LinkControlEx(plist, strName, m_ChannelSlot[i]._pGage, -1);
	}

	for(UINT32 i = 0; i < MAX_CHANNEL_LABEL; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "ServerLabel%d", i + 1);
		LinkControlEx(plist, strName, m_pChannelLabel[i], -1);

		sprintf_s(strName, "ServerLabelKey%d", i + 1);
		LinkControlEx(plist, strName, m_pChannelLabelKey[i], -1);
	}

	for(UINT32 i = 0; i < MAX_SERVER_SELECT_NUMBER_COUNT; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "ssServerSelectNumber_%d", i + 1);
		LinkControlEx(plist, strName, m_pServerSelectNumber[i], GCI_SSP_S_SELECET_NUMBER_1 + i);
		m_pServerSelectNumber[i]->SetEnable( FALSE);
	}

	LinkControlEx(plist, "ssSelect", m_pSelect, -1);
	LinkControlEx(plist, "ssFocus", m_pFocus, -1);

	LinkControlEx(plist, "ssGotoServerList", m_pGotoServerList, GCI_SSP_S_GOTO_SERVERLIST);

	m_AnnounceFrame.OnLinkControl(plist, pControl);

	return FALSE;
}

void CPhaseChannelList::OnInitControl(void)
{
	_LoadServerTextFromFile();

	I3NOTICE( "CPhaseChannelList::OnInitControl() Enter");

	// ���ӿ��� ����ϴ� ���������� �ε��մϴ�.
	g_pFramework->LoadNoticeFromFile();

	//Sound SetPendingEnable�� disable�ϱ� ���� ����� �ֽ��ϴ�.
	::InitSound();

	// ServerGui �⺻ ����
	m_pChannelGui->SetEnable(FALSE);

	m_rChannelWinX = m_pChannelWin->getPositionX();

	INIT_WIN_CAPTION(m_pChannelCaption, GAME_STRING("STR_TBL_GUI_SERVER_LIST"));/*�������*/


	INIT_BUTTON_EX(m_pPrev);
	INIT_BUTTON_EX(m_pRefresh);
	INIT_BUTTON_EX(m_pEnter);
	INIT_BUTTON_EX(m_pNext);

	if ( g_pServerInfo->IsUseServerSelectPage() )
	{
		m_pPrev->SetEnable(FALSE);
		m_pNext->SetEnable(FALSE);
	}

	m_pPrevCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRefreshCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pEnterCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNextCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPrevCaption->SetTextColor(GetColor(GCT_BLUE));
	m_pRefreshCaption->SetTextColor(GetColor(GCT_BLUE));
	m_pEnterCaption->SetTextColor(GetColor(GCT_BLUE));
	m_pNextCaption->SetTextColor(GetColor(GCT_BLUE));
	m_pPrevCaption->SetText(GAME_STRING("STR_TBL_GUI_SERVER_PREV_PAGE"));/*�������*/
	m_pRefreshCaption->SetText(GAME_STRING("STR_TBL_GUI_SERVER_REFRESH"));/*���ΰ�ħ*/
	m_pEnterCaption->SetText(GAME_STRING("STR_TBL_GUI_SERVER_CONNECT"));/*����*/
	m_pNextCaption->SetText(GAME_STRING("STR_TBL_GUI_SERVER_NEXT_PAGE"));/*�������*/


	// ����Ʈ �ʱ�ȭ
	for(UINT32 i = 0; i < MAX_CHANNEL_CONTROL_COUNT; i++)
	{
		m_ChannelSlot[i]._pNumber->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_ChannelSlot[i]._pChannel->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_ChannelSlot[i]._pNumber->SetTextColor(GetColor(GCT_DEFAULT));
		m_ChannelSlot[i]._pChannel->SetTextColor(GetColor(GCT_DEFAULT));
		m_ChannelSlot[i]._pNumber->setInputDisable(TRUE);
		m_ChannelSlot[i]._pChannel->setInputDisable(TRUE);
		m_ChannelSlot[i]._pGage->setInputDisable(TRUE);
		m_ChannelSlot[i]._pGage->SetAllShapeEnable(FALSE);
		m_ChannelSlot[i]._pColumn->SetEnable( FALSE);
	}

	for(UINT32 i = 0; i < MAX_CHANNEL_LABEL; i++)
	{
		m_pChannelLabelKey[i]->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pChannelLabelKey[i]->SetTextColor(GetColor(GCT_DEFAULT));
		m_pChannelLabelKey[i]->setInputDisable(TRUE);
		m_pChannelLabelKey[i]->SetOffsetPos(3.0f, 0.0f);

		m_pChannelLabel[i]->SetEnable( FALSE);
		m_pChannelLabelKey[i]->SetEnable( FALSE);
	}

	// ����Ʈ ����Ʈ, ��Ŀ�� �ʱ�ȭ
	m_pSelect->SetEnable(FALSE);
	m_pFocus->SetEnable(FALSE);

	// ���� ��ũ
	switch( GetCodePage())
	{
	case I3_LANG_CODE_KOREAN:		//	�ѱ�
		{
			m_pVerifiTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Main/grade.i3i");

			m_pVerifiSprite = i3Sprite2D::NewObject();
			m_pVerifiSprite->Create(2, TRUE, FALSE);
			m_pVerifiSprite->SetTexture(m_pVerifiTexture);

			if (g_pGameContext->IsAdultUser())
			{
				m_pVerifiSprite->SetTextureCoord(0, 120.0f, 1.0f, 234.0f, 137.0f);
			}
			else
			{
				m_pVerifiSprite->SetTextureCoord(0, 1.0f, 1.0f, 115.0f, 137.0f);
			}

			m_pVerifiSprite->SetTextureCoord(1, 238.0f, 1.0f, 331.0f, 137.0f);

			POINT2D pt = g_pFramework->GetSreenSize();
			m_pVerifiSprite->SetRect(0, pt.x - 120.0f, 5.0f, 115.0f, 137.0f);
			m_pVerifiSprite->SetRect(1, 5.0f, pt.y - 167.0f, 94.0f, 137.0f);
			m_pVerifiSprite->SetEnable(FALSE);

			g_pFramework->getGuiLayer()->AddChild(m_pVerifiSprite);
		}
	}

	//if( g_pSndMng!= NULL)
	//{
	//	g_pSndMng->StartBGM(g_pConfigEx->GetMisc().LobbyBGM.c_str(), true);
	//}

	INIT_BUTTON_EX(m_pGotoServerList);

	m_AnnounceFrame.OnInitControl();

	CPhaseBase::OnInitControl();

	I3NOTICE( "CPhaseChannelList::OnInitControl() Leave");
}

void CPhaseChannelList::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPhaseBase::OnUpdate(rDeltaSeconds);

	m_AnnounceFrame.OnUpdate(rDeltaSeconds);
	m_AnnounceFrame.OnUpdateAnnounceImage(rDeltaSeconds);

	_LimitRefresh(rDeltaSeconds);
	_LimitConnect(rDeltaSeconds);
	_WaitConnect(rDeltaSeconds);
}

void CPhaseChannelList::OnEntranceStart(void)
{
	CPhaseBase::OnEntranceStart();

	m_AnnounceFrame.OnEntranceStart();

	InputDisable(FALSE);

	if ( g_pGameContext->GetEnteredServer() )
	{
		// ���ͻ��¶�� �κ񿡼� ä�θ���Ʈ�� �� ���Դϴ�.
		if (g_pCommunity->IsInviteMoving())
		{
			OnInviteMove();
		}
		else
		{
			g_pGameContext->SetEvent(EVENT_GET_CHANNEL_LIST);
		}
	}
	else
	{
#if defined(CHANGE_SERVER_MOVE)
		// ���ͻ��°� �ƴ϶�� ���� ���Ӽ����� ���� ���̴�.
		g_pGameContext->SetEventOnce(EVENT_USER_ENTER);
#else
		// ���ͻ��°� �ƴ϶�� ����Ʈ���� ��������
		// ���Ӽ����� �̵��� �� ���Դϴ�.
		g_pGameContext->SetEvent(EVENT_CONNECT_SERVER_GAME);
#endif

		InputDisable(TRUE);
	}

	for( INT32 i = 0 ; i < MAX_CHANNEL_LABEL ; i++)
		m_pChannelLabel[i]->SetEnable( FALSE);
	for( INT32 i = 0 ; i < MAX_CHANNEL_CONTROL_COUNT ; i++)
		m_ChannelSlot[i]._pColumn->SetEnable( FALSE);


	// ������ �پ��� ä���� �����ؼ� �����ݴϴ�.
	// ������ �پ��� ������ �ٸ� ������ ������ �����մϴ�.
	if ( LastServerNumber != g_pServerInfo->GetCurGameServerIndex() )
	{
		OnSelect(INVALID_INDEX);
		LastChannelNumber	= INVALID_INDEX;
	}
	else
	{
		if ( LastChannelNumber != INVALID_INDEX)
		{
			UINT8 OldChannelType =	0;
			INT32 CurLabelCount =	0;
			INT32 CurServerIndex = 0;
			for( INT32 i = 0 ; i < GAME_CHANNEL_COUNT ; i++)
			{
				CurServerIndex = g_pServerInfo->GetCurGameServerIndex();
				UINT8 channelType = g_pGameContext->GetChannelType(CurServerIndex, i);

				// �ʺ������� ä���� ����� Ʋ������ ���� ī�װ��� �����.
				if( channelType == CHANNEL_TYPE_BEGIN2)
					channelType = CHANNEL_TYPE_BEGIN1;

				// ��������� ä���� ����� Ʋ������ ���� ī�װ��� �����.
				if( channelType == CHANNEL_TYPE_EXPERT2)
					channelType = CHANNEL_TYPE_EXPERT;
				if( OldChannelType != channelType)
				{
					CurLabelCount++;
				}
				// �������� �����մϴ�.
				OldChannelType = channelType;
				if(i == LastChannelNumber)
				{
					OnSelect(LastChannelNumber+CurLabelCount);
					break;
				}
				
			}
		}
	}

	LastServerNumber	= g_pServerInfo->GetCurGameServerIndex();

	DEV_LOG("CPhaseChannelList::OnEntranceStart");
}

BOOL CPhaseChannelList::OnEntrancing(RT_REAL32 rDeltaSeconds)
{
	return CPhaseBase::OnEntrancing(rDeltaSeconds);
}

void CPhaseChannelList::OnEntranceEnd(void)
{
	CPhaseBase::OnEntranceEnd();

	m_AnnounceFrame.OnEntranceEnd(this);

	m_fWaitDelayTime	= WAIT_CONNECT_TIME;
	m_bWaitConnect		= FALSE;
	m_pMBConnectWait	= NULL;
	m_LogInConnectFailedCount	= 0;

	// ���� ��� �ʱ�ȭ
	g_pFramework->PrintVersion();

	//	Korea only
	if( GetCodePage() == I3_LANG_CODE_KOREAN)
	{
		m_pVerifiSprite->SetEnable(TRUE);
	}


	//	Ŭ������ ���� ���� ������ Ŭ�� ��ư�� Ȱ��ȭ �Ѵ�.
	if( !g_pFramework->GetClanContext()->InBattleTeam())
	{
		SetPushedStateButton(MBT_CLAN, FALSE);
	}

	DEV_LOG("CPhaseChannelList::OnEntranceEnd");
}

void CPhaseChannelList::OnExitStart(void)
{
	CPhaseBase::OnExitStart();

	m_AnnounceFrame.OnExitStart(this);

	//	Korea only
	if( GetCodePage() == I3_LANG_CODE_KOREAN)
	{
		m_pVerifiSprite->SetEnable(FALSE);
	}
}

BOOL CPhaseChannelList::OnExiting(RT_REAL32 rDeltaSeconds)
{
	return CPhaseBase::OnExiting(rDeltaSeconds);
}

void CPhaseChannelList::OnExitEnd(void)
{
	CPhaseBase::OnExitEnd();

	m_AnnounceFrame.OnExitEnd();

	if( m_pMBConnectWait )
	{
		g_pFramework->CloseMessageBox(m_pMBConnectWait);
	}

	DEV_LOG("CPhaseChannelList::OnExitEnd");
}

void CPhaseChannelList::SetInputDisable(BOOL bDisable)
{
	m_pChannelGui->setInputDisable(bDisable);
}

BOOL CPhaseChannelList::GetInputDisable(void)
{
	return m_pChannelGui->getInputDisable();
}

void CPhaseChannelList::OnChangeVideo(i3VideoInfo* pInfo)
{
	//	Korea only
	if( GetCodePage() == I3_LANG_CODE_KOREAN)
	{
		// ���Ǹ�ũ ��ġ ������
		POINT2D pt = g_pFramework->GetSreenSize();
		m_pVerifiSprite->SetRect(0, pt.x - 120.0f, 5.0f, 115.0f, 137.0f);
		m_pVerifiSprite->SetRect(1, 5.0f, pt.y - 167.0f, 94.0f, 137.0f);
	}
}

BOOL CPhaseChannelList::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CPhaseBase::OnSliding(rDeltaSeconds))
	{
		return FALSE;	// ��������� ����.
	}


	// ����/���� �ð��� ���� ��ǥ ������ ����
	m_AnnounceFrame.OnSliding(rDeltaSeconds, this);

	m_pChannelWin->setPositionX(m_rChannelWinX + CPhaseBase::GetSlideX());


	// ���� �Ϸ� �� ��Ȱ��ȭ. �׿� Ȱ��ȭ.
	BOOL enableWindow = TRUE;

	if (SS_OUT == CPhaseBase::GetSlideState())
	{
		enableWindow = FALSE;
	}

	m_pChannelGui->SetEnable(enableWindow);

	return TRUE;
}



void CPhaseChannelList::OnSubExitKey( void )
{
	CMessageBox * pMessageBox = SYSTEM_POPUP_Q(GAME_STRING("STBL_IDX_EP_GAME_EXIT_NORMAL_QE"), EXIT_CALLBACK(CPhaseChannelList::CbExitNormal), this);

	if ( g_pConfigEx->GetMisc().webAnnounce.IsActive() )
	{
		REAL32 widthRate = i3GuiRoot::getGuiResolutionWidth();
		REAL32 heightRate = i3GuiRoot::getGuiResolutionHeight();

		POINT2D ptCenter = g_pFramework->GetCenterPosition(pMessageBox->GetWidth(), pMessageBox->GetHeight());
		ptCenter.x += static_cast<int>(static_cast<REAL32>(g_pConfigEx->GetMisc().webAnnounce.msgBoxOffsetX) * widthRate);
		ptCenter.y += static_cast<int>(static_cast<REAL32>(g_pConfigEx->GetMisc().webAnnounce.msgBoxOffsetY) * heightRate);
		pMessageBox->SetPosition(ptCenter.x, ptCenter.y);
	}
}

//static
void CPhaseChannelList::CbExitNormal(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((CPhaseChannelList*)pThis)->m_AnnounceFrame.CloseAnnouce();
	}
	CGuiNotifyReceiver::CbExitNormal(pThis, nParam);
}
void CPhaseChannelList::OnSelect(INT32 idxSlot)
{
	if (idxSlot == INVALID_INDEX )
	{
		m_pSelect->SetEnable(FALSE);
		return;
	}

	m_nLastedSelectSlot = idxSlot;
	m_pSelect->setPositionY(idxSlot * 29.0f + 1.0f);
	m_pSelect->SetEnable(TRUE);
}

void CPhaseChannelList::OnFocus(INT32 idxSlot)
{
	if (idxSlot == INVALID_INDEX )
	{
		m_pFocus->SetEnable(FALSE);
		return;
	}

	m_pFocus->setPositionY(idxSlot * 29.0f + 1.0f);
	m_pFocus->SetEnable(TRUE);
}

void CPhaseChannelList::OnConnect(INT32 idxSlot)
{
	if( m_nLastedSelectSlot == idxSlot)
	{
		OnConnect();
	}
	else
	{
		OnSelect(idxSlot);
	}
}

void CPhaseChannelList::OnConnect(void)
{
	INT32 RealSelectedSlot = 0;

	if (0.0f < m_fConnectClickElapseTime)
		return;

	if (-1 == m_nLastedSelectSlot)
		return;

	// ä���� Index�� ���մϴ�.
	{
		RealSelectedSlot = m_ChannelSlot[m_nLastedSelectSlot].Index;
		I3ASSERT( RealSelectedSlot >= 0);
	}

	if (FALSE == _CheckChannelLimit(RealSelectedSlot))
	{
		return;
	}


	m_bWaitConnect = TRUE;


	g_pGameContext->SetEvent(EVENT_SELECT_CHANNEL, &RealSelectedSlot);

	// �̻�� ��û���� �������� Ȯ���մϴ�.
	// �ݵ�� ���⼭ ȣ���� �ʿ䰡 ���� ������
	// �������� �����Ѵٸ� ȣ�� ��ġ�� �����Ͽ��� �����մϴ�.
	g_pGameContext->SetEvent(EVENT_RUN_PARAM_CHECK);

	InputDisable(TRUE);

	m_fConnectClickElapseTime = CONNECT_LIMIT_TIME;
	m_fRefreshClickElapseTime = REFRESH_LIMIT_TIME;
}


void CPhaseChannelList::OnRefresh(void)
{
	if( m_fRefreshClickElapseTime <= 0.0f)
	{
		_Refresh();
		m_fRefreshClickElapseTime = REFRESH_LIMIT_TIME;
	}
}


//virtual
BOOL CPhaseChannelList::OnInviteMove(void)
{
	if (g_pCommunity->GetMovingServer() != (UINT32)g_pServerInfo->GetCurGameServerIndex())
	{
		DEV_LOG("CPhaseChannelList::_OnInviteMove 1 [%d / %d]\n", g_pCommunity->GetMovingChannel(), g_pServerInfo->GetCurGameServerIndex());

		m_bWaitConnect = TRUE;

#if defined(CHANGE_SERVER_MOVE)
		((CStageReady *)m_pParent)->MovePhase(PHASE_SERVER_LIST);
#else
		INT32 MoveServerIndex = static_cast<INT32>(g_pCommunity->GetMovingServer());
		g_pServerInfo->SetGameServer(MoveServerIndex);
		g_pGameContext->SetEventOnce(EVENT_USER_LEAVE);
#endif

		return TRUE;
	}

	// ģ���ʴ�/ã�ư��� �� ä�� ���ѿ� �ɸ��� �̵� ����
	if (FALSE == _CheckChannelLimit(g_pCommunity->GetMovingChannel()))
	{
		DEV_LOG("CPhaseChannelList::_OnInviteMove 2 [%d / %d]\n", g_pCommunity->GetMovingChannel());
		g_pCommunity->ComplateInviteMove();
		return TRUE;
	}

	DEV_LOG("CPhaseChannelList::_OnInviteMove 3 [%d / %d]\n", g_pCommunity->GetMovingChannel(), g_pServerInfo->GetCurGameServerIndex());

	INT32	ChannelNumber = g_pCommunity->GetMovingChannel();
	g_pGameContext->SetEvent(EVENT_SELECT_CHANNEL, &ChannelNumber);
	InputDisable(TRUE);

	return TRUE;
}


void CPhaseChannelList::OnWheelUpdate(INT32 scrollby)
{
	m_AnnounceFrame.OnWheelUpdate(scrollby);
}



void CPhaseChannelList::_LimitRefresh(REAL32 rDeltaSeconds)
{
	m_fRefreshClickElapseTime -= rDeltaSeconds;
	if( m_fRefreshClickElapseTime < 0.0f)
	{
		m_fRefreshClickElapseTime = 0.0f;
	}
}

void CPhaseChannelList::_LimitConnect(REAL32 rDeltaSeconds)
{
	m_fConnectClickElapseTime -= rDeltaSeconds;
	if( m_fConnectClickElapseTime < 0.0f)
	{
		m_fConnectClickElapseTime = 0.0f;
	}
}

void CPhaseChannelList::_WaitConnect(REAL32 rDeltaSeconds)
{
	if (m_bWaitConnect)
	{
		I3ASSERT(NULL == m_pMBConnectWait);	// ��� �޽��� �ڽ��� �ߺ�ȣ�� �Ǿ�� �ȵȴ�

		m_fWaitDelayTime -= rDeltaSeconds;
		if( m_fWaitDelayTime <= 0.0f)
		{
			m_pMBConnectWait = ERROR_POPUP_W(GAME_STRING("STBL_IDX_EP_SERVER_WAIT_REFRESH_W"));

			m_fWaitDelayTime = WAIT_CONNECT_TIME;
			m_bWaitConnect = FALSE;
		}
	}
}

void CPhaseChannelList::_Refresh(void)
{
	g_pGameContext->SetEvent(EVENT_GET_CHANNEL_LIST);
}

void CPhaseChannelList::_CloseWaitMessageBox(void)
{
	if( m_pMBConnectWait)
	{
		CLOSE_POPUP(m_pMBConnectWait);
		m_pMBConnectWait = NULL;
	}

	m_fWaitDelayTime = WAIT_CONNECT_TIME;
	m_bWaitConnect = FALSE;
}



BOOL CPhaseChannelList::_CheckChannelLimit(INT32 channelIdx)
{
	// ������ ��ã���� ���� �Ұ�
	INT32 userCount = g_pGameContext->GetChannelUserCount(channelIdx);
	if (g_pGameContext->GetChannelMaxUser() <= userCount)
	{
		ERROR_POPUP(GAME_STRING("STBL_IDX_EP_SERVER_USER_FULL_C"));	// _C �̳�, �ݹ� ������� �ʴ´�.
		return FALSE;
	}

	// ������ �� �ִ��� �˻��Ѵ�.
	ChannelType channelType = static_cast<ChannelType>(g_pGameContext->GetChannelType(g_pServerInfo->GetCurGameServerIndex(), channelIdx));
	if ( g_pGameContext->IsValidChannelType(channelType) )
	{
		return TRUE;
	}

	// ������ �� ������ �����޽����� �ѷ��ش�.
	PrintMessageForChannelEnterError(channelType);
	return FALSE;
}

void CPhaseChannelList::_UpdateChannelLabel(void)
{
#if defined(CHANGE_SERVER_MOVE)
#else
	if ( ! g_pGameContext->GetEnteredServer() )
		return;
#endif

	INT32 nIndex = 0;
	UINT8 OldChannelType = 0;
	INT32 nApplyCount = 0;
	INT32 szChannelArray[GAME_CHANNEL_COUNT];

	// ���� ��ġ�� �����մϴ�.
	REAL32 fLabelY = 8.0f;

	// ������ �ʱ�ȭ
	for( INT32 i = 0 ; i < MAX_CHANNEL_LABEL ; i++)
		m_pChannelLabel[i]->SetEnable( FALSE);
	for( INT32 i = 0 ; i < MAX_CHANNEL_CONTROL_COUNT ; i++)
		m_ChannelSlot[i]._pColumn->SetEnable( FALSE);

	INT32 nCurServerIdx = g_pServerInfo->GetCurGameServerIndex();

	for( INT32 i = 0 ; i < GAME_CHANNEL_COUNT ; i++)
	{
		UINT8 channelType = g_pGameContext->GetChannelType(nCurServerIdx, i);
		UINT8 nBackupChannel = channelType;

		// �ʺ������� ä���� ����� Ʋ������ ���� ī�װ��� �����.
		if( channelType == CHANNEL_TYPE_BEGIN2)
			channelType = CHANNEL_TYPE_BEGIN1;

		// ��������� ä���� ����� Ʋ������ ���� ī�װ��� �����.
		if( channelType == CHANNEL_TYPE_EXPERT2)
			channelType = CHANNEL_TYPE_EXPERT;

		INT32 CurLabelCount = -1;

		if( OldChannelType != channelType)
		{
			// ���� Ȱ��ȭ �Ǿ��ִ� ���� �ε����� �����ɴϴ�.
			for( INT32 n = 0 ; n <MAX_CHANNEL_LABEL ; n++)
			{
				if( !m_pChannelLabel[n]->isEnable())
				{
					CurLabelCount = n;
					break;
				}
			}

			if( CurLabelCount != -1)
			{
				// ������(ī�װ�)������ �մϴ�.
				{
					if( CurLabelCount != 0)
						fLabelY += (29.0f * (nApplyCount+1));

					m_pChannelLabel[CurLabelCount]->setPositionY( fLabelY);
					m_pChannelLabel[CurLabelCount]->SetEnable( TRUE);

					m_pChannelLabelKey[CurLabelCount]->SetText(GetCategoryText(static_cast<ChannelType>(channelType)));
					m_pChannelLabelKey[CurLabelCount]->SetEnable( TRUE);
				}
			}

			// ����������Ʈ�� �Ǿ��� �׸�鿡 ���Ͽ� �۾��� �����մϴ�.
			if( nApplyCount > 0)
			{
				for( INT32 n = 0 ; n < nApplyCount ; n++)
				{
					_SetServerEnv(g_pServerInfo->GetCurGameServerIndex(), nIndex, szChannelArray[n]);
					nIndex++;
				}
			}

			// �������� �����մϴ�.
			OldChannelType = channelType;

			// ������Ʈ �׸� ���� �ʱ�ȭ�� �մϴ�.
			nApplyCount = 0;

			// ���� ä�� Array�� ���� ����Ÿ���� �����մϴ�.
			szChannelArray[nApplyCount] = nBackupChannel;

			nIndex++;
		}

		// ���� ä�� Array�� ���� ����Ÿ���� �����մϴ�.
		szChannelArray[nApplyCount] = nBackupChannel;
		nApplyCount++;
	}

	// ��� �۾��� ������ ����������Ʈ�� �Ǿ��� �׸�鿡 ���Ͽ� �۾��� �����մϴ�.
	if( nApplyCount > 0)
	{
		for( INT32 n = 0 ; n < nApplyCount ; n++)
		{
			_SetServerEnv( g_pServerInfo->GetCurGameServerIndex(), nIndex, OldChannelType);
			nIndex++;
		}
	}

	m_nServerChannelIdx = 0;

	if( m_nLastedSelectSlot >= 0)
		OnFocus(m_nLastedSelectSlot);
}

void CPhaseChannelList::_SetServerEnv(INT32 nServer, INT32 nChannel, INT32 nChannelState)
{
	//I3ASSERT(nChannel < MAX_CHANNEL_CONTROL_COUNT);
	if(nChannel >= MAX_CHANNEL_CONTROL_COUNT)
	{
		return;
	}

	INT32 nEvent = 0;
	char strBuf[MAX_STRING_COUNT];


	// ���� ������
	UINT32	userCount		= g_pGameContext->GetChannelUserCount(m_nServerChannelIdx);
	UINT32	MaxUserCount	= g_pGameContext->GetChannelMaxUser();
	INT32	userGage		= GameGUI::GetChannelGauge(userCount, MaxUserCount);

	m_ChannelSlot[ nChannel]._pColumn->setInputDisable( BOOL(userGage == GameGUI::BLOCK_GAUGE_FULL) );
	m_ChannelSlot[ nChannel]._pGage->SetAllShapeEnable(FALSE);
	m_ChannelSlot[ nChannel]._pGage->SetShapeEnable(GUI_SHAPE_OFFSET + userGage, TRUE);

	I3_BOUNDCHK( SERVERSELECT_MAINTYPE_EVENT0 + nEvent, SERVERSELECT_MAINTYPE_COUNT);

	// ���� �̸�
	if( userGage == GameGUI::BLOCK_GAUGE_FULL)		//	���� Ǯ ����
	{
		switch(nChannelState)
		{
			case CHANNEL_TYPE_BEGIN1:
			case CHANNEL_TYPE_BEGIN2:
				sprintf_s( strBuf, "%s",  m_szServerText[SERVERSELECT_MAINTYPE_BEGINNER][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT]);	// �ʺ�����
				break;
			case CHANNEL_TYPE_NORMAL:
			case CHANNEL_TYPE_NORMAL2:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_NORMAL][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT]); // �Ϲݼ���
				break;
			case CHANNEL_TYPE_CLAN:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_CLAN][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT]); // Ŭ������
				break;
			case CHANNEL_TYPE_EXPERT:
			case CHANNEL_TYPE_EXPERT2:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_EXPERT][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT]); // �������
				break;
			case CHANNEL_TYPE_CHAMPIONSHIP:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_CHAMPIONSHIP][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT]); // Ŭ������
				break;

			case CHANNEL_TYPE_NATIONAL:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_NATIONAL][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT]); // Ŭ������
				break;
			case CHANNEL_TYPE_MIDDLE:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_MIDDLE][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT]); // Ŭ������
				break;

			default:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_EVENT0 + nEvent][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT]); // ��Ÿ �̺�Ʈ�� ����
				break;
		}
	}
	else					//	���� �Ϲ� ����
	{
		switch(nChannelState)
		{
			case CHANNEL_TYPE_BEGIN1:
			case CHANNEL_TYPE_BEGIN2:
				sprintf_s( strBuf, "%s",  m_szServerText[SERVERSELECT_MAINTYPE_BEGINNER][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT]);	// �ʺ�����
				break;
			case CHANNEL_TYPE_NORMAL:
			case CHANNEL_TYPE_NORMAL2:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_NORMAL][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT]); // �Ϲݼ���
				break;
			case CHANNEL_TYPE_CLAN:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_CLAN][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT]); // Ŭ������
				break;
			case CHANNEL_TYPE_EXPERT:
			case CHANNEL_TYPE_EXPERT2:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_EXPERT][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT]); // �������
				break;
			case CHANNEL_TYPE_CHAMPIONSHIP:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_CHAMPIONSHIP][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT]); // Ŭ������
				break;

			case CHANNEL_TYPE_NATIONAL:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_NATIONAL][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT]); // Ŭ������
				break;
			case CHANNEL_TYPE_MIDDLE:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_MIDDLE][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT]); // Ŭ������
				break;

			default:
				sprintf_s( strBuf, "%s", m_szServerText[SERVERSELECT_MAINTYPE_EVENT0 + nEvent][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT]); // ��Ÿ �̺�Ʈ�� ����
				break;
		}
	}

#if defined(I3_DEBUG) && 0
	i3String::Format(strBuf, MAX_STRING_COUNT, "%s : %d / %d", strBuf, userCount, MaxUserCount);
#endif

	m_ChannelSlot[ nChannel]._pChannel->SetText(strBuf);

	// ���� ��ȣ
	sprintf_s(strBuf, "%03d", (nServer * 10) + m_nServerChannelIdx + 1);
	m_ChannelSlot[ nChannel]._pNumber->SetText(strBuf);
	if( userGage == GameGUI::BLOCK_GAUGE_FULL)
	{
		COLOR col;
		i3Color::Set( &col, (UINT8) 173, 173, 173, 180);
		m_ChannelSlot[ nChannel]._pNumber->SetTextColor(&col);
	}
	else m_ChannelSlot[ nChannel]._pNumber->SetTextColor(GetColor(GCT_DEFAULT));

	// �÷��� Ȱ��ȭ�մϴ�.
	m_ChannelSlot[ nChannel]._pColumn->SetEnable( TRUE);


	// ���� �ε��� ���̺� ����.
	m_ChannelSlot[ nChannel].Index = m_nServerChannelIdx;

	// ���� �ε��� ����.
	m_nServerChannelIdx++;
}

void CPhaseChannelList::_EhFailConnect(void)
{
	if( m_LogInConnectFailedCount++ < g_pConfigEx->GetNetwork().ServerReconnectCount )
	{
#if defined(CHANGE_SERVER_MOVE)
#else
		g_pGameContext->SetEvent(EVENT_CONNECT_SERVER_GAME);
#endif
	}
	else
	{
		_CloseWaitMessageBox();
		//SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_SERVER_SERVER_OFF_E"), CbExitNormal, this);
		// ���� ���Ӽ����� �������ϸ� ���Ḧ �ϴ� �ڵ忴����
		// ����Ʈ���� ������ �ٰ��մϴ�.
		SYSTEM_POPUP(GAME_STRING("STBL_IDX_EP_SERVER_FAIL_REFRESH_C"));
		((CStageReady *)m_pParent)->MovePhase(PHASE_SERVER_LIST);
	}
}

void CPhaseChannelList::_EhStageError(INT32 Event,INT32 Arg)
{
	g_pGameContext->SetEvent(EVENT_DISCONNECT);

	// ����μ� ���� ������ ���� - praptor
	char bug[256];
	sprintf_s(bug, GAME_STRING("STR_TBL_GUI_SERVER_DEBUG_POPUP2"), Event, Arg);/*����� �˾�: Event(%d), Arg(0x%x)\n�����: praptor*/
	g_pFramework->MessageBox(bug, NULL, MBF_GAME_OK);
}

void CPhaseChannelList::CbRefleshServerList(void* pThis,INT32 nParam)
{
	((CPhaseChannelList*)pThis)->_Refresh();
}

void CPhaseChannelList::_LoadServerTextFromFile(void)
{
	INT32 nMode = 0;
	WCHAR16 szwReadLine[512] = L"";
	char filePath[MAX_PATH] = "";
	char szReadLine[512*2] = {0,};
	char szTemp[512] = {0,};
	i3FileStream file;

	//	multi path
	
	ConvertLocalePath( filePath, "Script/String/ServerSelect.txt", LOCALE_PATH_STRING_TABLE);

	//ConvertLocalePath( filePath, "config/ServerSelect.txt", LOCALE_PATH_CONFIG);

	if( FALSE == file.Open( filePath, STREAM_READ | STREAM_SHAREREAD) )
	{
		I3ASSERT_0;	//	�ݵ�� �־�� ��. ��ŵ�ϸ� ��ũ��Ʈ�� �ȳ���. komet
	}

	//	Check unicode
	{
		file.Read( szwReadLine, sizeof( WCHAR16));

		//	���⿡ �ɸ��� ��ũ��Ʈ ������ �����ڵ尡 �ƴմϴ�. �ݵ�� �����ڵ� �����̾�� �մϴ�. komet
		if( !CHECK_UNICODE(szwReadLine[0]))
		{
			I3FATAL( "ServerSelect file have not unicode !! (%s)", filePath);
			return;
		}
	}

	BOOL bContinue = TRUE;
	while( bContinue )
	{
		UINT32 wBufSize = sizeof( szwReadLine);
		i3mem::FillZero( szwReadLine, wBufSize);
		i3mem::FillZero( szReadLine, sizeof(szReadLine));

		// �����ڵ�� �� �ؽ�Ʈ�� ���پ� �н��ϴ�.
		UINT32 wReadSize = file.ReadLineW( szwReadLine, wBufSize);

		if( wReadSize == 0)	continue;

		I3ASSERT( wReadSize < wBufSize);

		//	�����ڵ� ���ڸ� ��Ƽ����Ʈ ���ڷ� ��ȯ
		WideCharToMultiByte( GetCodePage(), 0, szwReadLine, i3String::LengthW(szwReadLine), szReadLine, sizeof(szReadLine), NULL, NULL);
		I3ASSERT( i3String::Length( szReadLine) < sizeof(szReadLine));	// ���۸� �÷��ּ���.

		{
			char temp[MAX_STRING_COUNT] = "";

			//	�̺�Ʈ ����
			for(INT32 i=0; i<10; i++)
			{
				sprintf_s( temp, "[STR_SERVER_SELECT_CATEGORY_EVENT%d]", i);

				if( i3String::Contain( szReadLine, temp ) >= 0 )
				{
					nMode = SERVERSELECT_MAINTYPE_EVENT0 + i;	// �̺�Ʈ
					I3_BOUNDCHK( nMode, SERVERSELECT_MAINTYPE_COUNT);
					break;
				}
			}
		}

		// SCript/String/korea/ServerSelect.txt ���Ͽ� ��� �� ���� ���� ���´�.
		// nMode�� SERVERSELECT_MAINTYPE���⿡ ��� �� ���� ���´�.
		if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_BEGINNER]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_BEGINNER;	// �ʺ�
		}
		else if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_NORMAL]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_NORMAL;	// �Ϲ�
		}
		else if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_EXPERT]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_EXPERT;	// ���
		}
		else if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_CLAN]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_CLAN;	// Ŭ��
		}
		else if ( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_CHAMPIONSHIP]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_CHAMPIONSHIP;		//��ȸ
		}
		else if ( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_LATIN]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_NATIONAL;	//���� ä��
		}
		else if ( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_MIDDLE]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_MIDDLE;	//�߰�
		}
		else if( i3String::Compare( szReadLine, "[ServerSelectEnd]" ) == 0 )
		{
			bContinue = FALSE;
			continue;
		}
		else if( szReadLine[0] != ';')
		{
			if( i3String::Contain( szReadLine, "[CATEGORY_TEXT]" ) >= 0 )
			{
				i3String::Copy( szTemp, &szReadLine[15], 512 );
				INT32 nLength = i3String::Length(szTemp);
				i3String::NCopy( m_szServerText[nMode][SERVERSELECT_SUBTYPE_CATEGORY_TEXT], szTemp, nLength);
			}
			else if( i3String::Contain( szReadLine, "[CATEGORY_SLOT_TEXT]" ) >= 0 )
			{
				i3String::Copy( szTemp, &szReadLine[20], 512 );
				INT32 nLength = i3String::Length(szTemp);
				i3String::NCopy( m_szServerText[nMode][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT], szTemp, nLength);
			}
			else if( i3String::Contain( szReadLine, "[CATEGORY_SLOT_FULL_TEXT]" ) >= 0 )
			{
				i3String::Copy( szTemp, &szReadLine[25], 512 );
				INT32 nLength = i3String::Length(szTemp);
				i3String::NCopy( m_szServerText[nMode][SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT], szTemp, nLength);
			}
			else if( i3String::Contain( szReadLine, "[CATEGORY_READY_SLOT_TEXT]" ) >= 0 )
			{
				i3String::Copy( szTemp, &szReadLine[26], 512 );
				INT32 nLength = i3String::Length(szTemp);
				//i3String::NCopy( m_szServerText[nMode][SERVERSELECT_SUBTYPE_CATEGORY_READY_SLOT_TEXT], szTemp, nLength);
				i3String::NCopy( ((CStageReady*)m_pParent)->getChannelSlotText(nMode), szTemp, nLength);
			}
		}
	}

	file.Close();
}

const char *	CPhaseChannelList::GetCategoryText(ChannelType Type)
{
	switch( Type)
	{
	case CHANNEL_TYPE_BEGIN1:
	case CHANNEL_TYPE_BEGIN2:
		return m_szServerText[SERVERSELECT_MAINTYPE_BEGINNER][SERVERSELECT_SUBTYPE_CATEGORY_TEXT];	// �ʺ�����
		break;
	case CHANNEL_TYPE_NORMAL:
	case CHANNEL_TYPE_NORMAL2:
		return m_szServerText[SERVERSELECT_MAINTYPE_NORMAL][SERVERSELECT_SUBTYPE_CATEGORY_TEXT]; // �Ϲݼ���
		break;
	case CHANNEL_TYPE_CLAN:
		return m_szServerText[SERVERSELECT_MAINTYPE_CLAN][SERVERSELECT_SUBTYPE_CATEGORY_TEXT]; // Ŭ������
		break;
	case CHANNEL_TYPE_EXPERT:
	case CHANNEL_TYPE_EXPERT2:
		return m_szServerText[SERVERSELECT_MAINTYPE_EXPERT][SERVERSELECT_SUBTYPE_CATEGORY_TEXT]; // �������
		break;
	case CHANNEL_TYPE_CHAMPIONSHIP:
		return m_szServerText[SERVERSELECT_MAINTYPE_CHAMPIONSHIP][SERVERSELECT_SUBTYPE_CATEGORY_TEXT]; // Ŭ������
		break;

	case CHANNEL_TYPE_NATIONAL:
		return m_szServerText[SERVERSELECT_MAINTYPE_NATIONAL][SERVERSELECT_SUBTYPE_CATEGORY_TEXT]; // Ŭ������
		break;
	case CHANNEL_TYPE_MIDDLE:
		return m_szServerText[SERVERSELECT_MAINTYPE_MIDDLE][SERVERSELECT_SUBTYPE_CATEGORY_TEXT]; // Ŭ������
		break;
	}

	return m_szServerText[SERVERSELECT_MAINTYPE_EVENT0][SERVERSELECT_SUBTYPE_CATEGORY_TEXT];
}

void CPhaseChannelList::PrintMessageForChannelEnterError( ChannelType channelType )
{
	char MsgBuffer[MAX_STRING_COUNT] = {};

	switch(channelType)
	{
	case CHANNEL_TYPE_BEGIN1:
		{
			ChannelType FindChannelType = CHANNEL_TYPE_NONE;

			switch(channelType)
			{
				case CHANNEL_TYPE_BEGIN1:	
					FindChannelType = CHANNEL_TYPE_BEGIN1;
					break;
			}

			const ConfigLogic::ChannelConditionInfo * pCondiInfo = 
				g_pConfigEx->GetLogic().GetChannelConditionInfo(FindChannelType);

			I3ASSERT(pCondiInfo != NULL);
			I3ASSERT(pCondiInfo->condition == ConfigLogic::ChannelConditionInfo::KILLDEATH);

			i3String::Format(MsgBuffer, MAX_STRING_COUNT, 
				GAME_STRING("STR_TBL_GUI_SERVER_NOTICE_CONDITION_KILLDEATH"), 
				pCondiInfo->value_min );
		}
		break;
	case CHANNEL_TYPE_CLAN:
		{
			//Ŭ������ �ƴϸ� ������ �� �����ϴ�.
			i3String::Copy(MsgBuffer, GAME_STRING("STR_TBL_GUI_SERVER_NOTICE_LIMIT_JOIN_BY_CLANMEMBER"), MAX_STRING_COUNT );
		}
		break;

	case CHANNEL_TYPE_BEGIN2:	
	case CHANNEL_TYPE_EXPERT:
	case CHANNEL_TYPE_EXPERT2:
	case CHANNEL_TYPE_MIDDLE:
		{
			const ConfigLogic::ChannelConditionInfo * pCondiInfo = 
				g_pConfigEx->GetLogic().GetChannelConditionInfo(channelType);

			I3ASSERT(pCondiInfo != NULL);
			I3ASSERT(pCondiInfo->condition == ConfigLogic::ChannelConditionInfo::RANK);

			i3String::Format(MsgBuffer, MAX_STRING_COUNT, 
				GAME_STRING("STR_TBL_GUI_SERVER_NOTICE_CONDITION_RANK"), 
				GetCharaRank (pCondiInfo->value_min ),
				GetCharaRank( pCondiInfo->value_max ) );
		}
		break;
	}
	g_pServerInfo->SetCurChannelNumber(INVALID_INDEX);
	STRING_POPUP_C(MsgBuffer, MAKE_CALLBACK(CbRefleshServerList), this);
}
