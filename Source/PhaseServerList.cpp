#include "pch.h"
#include "PhaseServerList.h"
#include "GlobalVariables.h"
#include "ServerInfo.h"
#include "CommunityContext.h"
#include "StageGui/Popup/PopupHeader.h"
#if defined(LOCALE_CHINA)
#include "../SDOALink/SDOALink.h"
#endif

#define		SERVER_LIST_SLOT_HORI_COUNT		4
#define		SERVER_LIST_SLOT_VERT_COUNT		6
#define		INVALID_SERVER_INDEX			-1

#if defined(LOCALE_INDONESIA)
	#define A_LOT_OF_SERVERS
#endif

I3_CLASS_INSTANCE(CPhaseServerList, CPhaseBase);

const TCHAR * GetServerTabTypeText( SChannelType TabType )
{
	switch(TabType)
	{
	case S_CHANNEL_TYPE_ALL			:	return GAME_STRING("STR_TBL_NETWORK_SERVER_TAB_ALL");
	case S_CHANNEL_TYPE_NORMAL		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_TAB_NORMAL");
	case S_CHANNEL_TYPE_NORMAL2		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_TAB_NORMAL2");
	case S_CHANNEL_TYPE_BEGIN		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_TAB_BEGIN");
	case S_CHANNEL_TYPE_CLAN		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_TAB_CLAN");
	case S_CHANNEL_TYPE_EXPERT		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_TAB_EXPERT");
	case S_CHANNEL_TYPE_CONBINATION	:	return GAME_STRING("STR_TBL_NETWORK_SERVER_TAB_COMBINATION");
	case S_CHANNEL_TYPE_CHAMPIONSHIP:	return GAME_STRING("STR_TBL_NETWORK_SERVER_TAB_CHAMPIONSHIP");
	case S_CHANNEL_TYPE_NATIONAL:	return GAME_STRING("STR_TBL_NETWORK_SERVER_TAB_NATIONAL");
	}

	return INVALID_TEXT;
}

const TCHAR * GetServerTypeText( SChannelType ServerType )
{
	switch(ServerType)
	{
	case S_CHANNEL_TYPE_NORMAL		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_NORMAL");
	case S_CHANNEL_TYPE_NORMAL2		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_NORMAL");
	case S_CHANNEL_TYPE_BEGIN		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_BEGIN");
	case S_CHANNEL_TYPE_CLAN		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_CLAN");
	case S_CHANNEL_TYPE_EXPERT		:	return GAME_STRING("STR_TBL_NETWORK_SERVER_EXPERT");
	case S_CHANNEL_TYPE_CONBINATION	:	return GAME_STRING("STR_TBL_NETWORK_SERVER_COMBINATION");
	case S_CHANNEL_TYPE_CHAMPIONSHIP:	return GAME_STRING("STR_TBL_NETWORK_SERVER_CHAMPIONSHIP");
	case S_CHANNEL_TYPE_NATIONAL:	return GAME_STRING("STR_TBL_NETWORK_SERVER_NATIONAL");
	}
	return INVALID_TEXT;
}

//////////////////////////////////////////////////////////////////////////
//	ServerListSlot

CPhaseServerList::ServerListSlot::ServerListSlot()
{
	pFrame = NULL;
	pText = NULL;
	pUserGauge = NULL;
}


void CPhaseServerList::ServerListSlot::SetEnable( BOOL Enable )
{
	pFrame->setInputDisable(!Enable);
	if ( !Enable )
	{
		pFrame->SetAllShapeEnable(FALSE);
		pFrame->SetShapeEnable(SLOT_SHAPE_INDEX_NORMAL, TRUE);
	}
	pText->SetEnable(Enable);
	pUserGauge->SetEnable(Enable);
}


void CPhaseServerList::ServerListSlot::Update( const CServerInfo * pInfo, BOOL Selected )
{
	// Ȱ��ȭ ���� ���� ������ ��Ȱ��ȭ ��Ų��.
	if ( ! pInfo->GetActive() )
	{
		pFrame->setInputDisable(!pInfo->GetActive());
#if defined(BUILD_RELEASE_QA_VERSION)
		pFrame->setInputDisable(FALSE);
#endif
	}

	// ���õ� �������� Ȯ���Ѵ�.
	SLOT_SHAPE_INDEX	FrameShape = (Selected) ? SLOT_SHAPE_INDEX_SELECTED : SLOT_SHAPE_INDEX_NORMAL;


	// ���� �� ��ŭ �������� ä���ش�.
	INT32 userGauge = GameGUI::GetServerGauge(pInfo->GetUserCount(), pInfo->GetMaxUserCount());
	pUserGauge->SetAllShapeEnable(FALSE);
	pUserGauge->SetShapeEnable(GUI_SHAPE_OFFSET + userGauge, TRUE);


	//
	// ���� �ؽ�Ʈ�� �����Ѵ�.
	//
	const TCHAR * ServerTypeName = GetServerTypeText(pInfo->GetType());
	TCHAR	ServerText[MAX_PATH] = {};


#if	0	//defined(I3_DEBUG)
	i3String::Format(ServerText, MAX_PATH, _T("%s %03d\n [%d/%d] %s"), ServerTypeName, pInfo->GetNumber()+1, pInfo->GetUserCount(), pInfo->GetMaxUserCount(),
		(pInfo->GetActive() ? "Active" : "Unactive"));
#else

	// ���� �̸��� ����� �ش�.
	// ���� ��ȣ�� 1������ �����ϵ����մϴ�.
	i3String::Format(ServerText, MAX_PATH, _T("%s %03d"), ServerTypeName, pInfo->GetNumber()+1);

#endif
	pText->SetText(ServerText);


	// �ؽ�Ʈ ������ ����.
	GUI_COLOR_TYPE		TextColor	= GCT_DEFAULT;	
	if ( ! pInfo->GetActive() )
		TextColor = GCT_INVALID_C;
	else if ( userGauge == GameGUI::BLOCK_GAUGE_FULL )
		TextColor = GCT_RED;

	pText->SetTextColor( GetColor(TextColor) );

	// ���� ����� ����.
	pFrame->SetAllShapeEnable(FALSE);
	pFrame->SetShapeEnable(FrameShape, TRUE);
}



//////////////////////////////////////////////////////////////////////////
//	ServerTab
CPhaseServerList::ServerTab::ServerTab()
{
	pCtrl = NULL;
	Type = S_CHANNEL_TYPE_UNDEFINED;
}

void	CPhaseServerList::ServerTab::SetSelect(BOOL Enable)
{
	GUI_CONTROL_STATE	State;
	GUI_COLOR_TYPE		Color;
	if ( Enable )
	{
		State = I3GUI_CONTROL_STATE_DISABLED;
		Color = GCT_YELLOW;
	}
	else
	{
		State = I3GUI_CONTROL_STATE_NORMAL;
		Color = GCT_DEFAULT;
	}

	pCtrl->setControlState(State);
	//pCtrl->SetCaptionColor(GetColor(Color));
}

void	CPhaseServerList::ServerTab::SetEnable(BOOL Enable)
{
	pCtrl->SetEnable(Enable);
}

void	CPhaseServerList::ServerTab::SetType(SChannelType _Type)
{
	pCtrl->SetCaption(GetServerTabTypeText(_Type));
	Type = _Type;
}



//////////////////////////////////////////////////////////////////////////
//	CPhaseServerList
CPhaseServerList::CPhaseServerList()
{
	m_pServerListWin			= NULL;
	m_pServerListFrame			= NULL;
	m_pServerListScrollFrame	= NULL;
	m_pServerListScrollList		= NULL;
	m_ServerListStartSlot		= 0;
	m_SelectedServerIndex		= INVALID_SERVER_INDEX;

	m_pServerRefresh			= NULL;
	m_pServerEnter				= NULL;
	m_LastRefreshTime			= 0.0f;

	m_CurrentTab				= S_CHANNEL_TYPE_UNDEFINED;
	m_pMBInfoWait				= NULL;
	m_ConnectFailedCount		= 0;
	m_ConnectFailedTotalCount	= 0;

	m_PrevServerIndex			= INVALID_SERVER_INDEX;
	m_PrevServerTab				= S_CHANNEL_TYPE_UNDEFINED;

	m_TargetServerIndex			= INVALID_SERVER_INDEX;
	m_TargetServerTab			= S_CHANNEL_TYPE_UNDEFINED;

}


void	CPhaseServerList::OnGameEvent(INT32 event,INT32 arg)
{
	CPhaseBase::OnGameEvent(event, arg);

	switch(event)
	{
		// ���Ӽ����� �ִٰ� ����Ʈ���̷� ������ �߻��ϴ� �̺�Ʈ�Դϴ�.
#if defined(CHANGE_SERVER_MOVE)
	case EVENT_CONNECT_SERVER_GAME:
		{
			if( EV_SUCCESSED(arg))
			{
				const CServerInfo * pInfo = g_pServerInfo->GetServerInfo(m_TargetServerIndex, m_TargetServerTab);
				I3ASSERT(pInfo != NULL);
				if (!pInfo)
					return;

				if ( pInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
				{
					// NULL���� ũ�⵵ �Բ� ������.
					UINT8 Size = static_cast<UINT8>(m_ServerPassword.size()+1);
					g_pGameContext->SetEventOnce(EVENT_ENTER_PW, &Size, m_ServerPassword.c_str());
				}
				else
				{
					g_pGameContext->SetEventOnce(EVENT_USER_LEAVE);
					DEV_LOG("Phase Server List - EVENT_USER_LEAVE Req [%d][%d] // EVENT_CONNECT_SERVER_GAME\n", m_SelectedServerIndex, m_CurrentTab);
				}
			}
			else
			{
				if (g_pCommunity->IsInviteMoving())
				{
					g_pCommunity->ComplateInviteMove();
				}

				SYSTEM_POPUP(GAME_STRING("STBL_IDX_EP_SERVER_FAIL_MOVE"));
				InputDiable(FALSE);
			}
		}
		break;

#else
	case EVENT_CONNECT_SERVER_GATEWAY:
		if( EV_SUCCESSED(arg))
		{
			g_pGameContext->SetEventOnce(EVENT_USER_ENTER);
		}
		else
		{
			// ��ȸ ���ӽõ��� ������ ���������� üũ.
			if ( m_ConnectFailedCount++ < g_pConfigEx->GetNetwork().ServerReconnectCount )
			{
				g_pGameContext->SetEvent(EVENT_CONNECT_SERVER_GATEWAY);
				I3NOTICE("EVENT_CONNECT_SERVER_GATEWAY Fail. reconnect count L %d, [%x/%d]\n", m_ConnectFailedCount, arg, arg);
			}
			else
			{
				InputDiable(FALSE);
				// ����Ʈ���� ������ ���� �õ��� ��� �����ϸ� ������ �����Ѵ�.
				if ( m_ConnectFailedTotalCount++ < g_pConfigEx->GetNetwork().ServerReconnectCount )
				{
					m_ConnectFailedTotalCount++;
					((CStageReady *)m_pParent)->MovePhase(PHASE_CHANNEL_LIST);
				}
				else
				{
					SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_FAIL_CONNECT_E"), EXIT_CALLBACK(CbExitNormal), this);
				}
			}
		}
		break;
#endif

	////1.5 Check GetMyInfo
	//case EVENT_GET_MYINFO:
	//	if ( EV_FAILED(arg) )
	//	{
	//		SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_SERVER_FAIL_GET_MYINFO"), EXIT_CALLBACK(CbExitNormal), this);
	//	}
	//	else
	//	{
	//		UpdateCharacter(VT_DEFAULT);
	//	}

	//	if ( m_pMBInfoWait )
	//	{
	//		CLOSE_POPUP(m_pMBInfoWait);
	//		m_pMBInfoWait	= NULL;
	//	}
	//	break;
#if defined(CHANGE_SERVER_MOVE)
#else
	case EVENT_USER_ENTER:
		InputDiable(FALSE);
		if( EV_FAILED(arg))
		{
			SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_FAIL_CONNECT_E"), EXIT_CALLBACK(CbExitNormal), this);
		}
		break;
#endif
	case EVENT_USER_LEAVE:
		{
#if defined(CHANGE_SERVER_MOVE)
			if (EV_SUCCESSED(arg))
			{
				g_pGameContext->ChangeSocket();
				((CStageReady *)m_pParent)->MovePhase(PHASE_CHANNEL_LIST);
				InputDiable(FALSE);
			}
			else
			{
				SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_SERVER_FAIL_MOVE"), EXIT_CALLBACK(CbExitNormal), this);
			}
#else
			I3NOTICE("Phase Server List - EVENT_USER_LEAVE Ack\n");
			g_pGameContext->SetEvent(EVENT_DISCONNECT);
			((CStageReady *)m_pParent)->MovePhase(PHASE_CHANNEL_LIST);
			InputDiable(FALSE);
#endif
		}
		break;
	case EVENT_M_DESTROYED_NETWORK:
		{
			DEV_LOG("Phase Server List - DESTROYED NETWORK\n");
			SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DESTROYED_NETWORK_E"), EXIT_CALLBACK(CbExitNormal), this);
		}
		break;
	case EVENT_ENTER_PW:
		{
			if ( EV_SUCCESSED(arg))
			{
				g_pGameContext->SetEventOnce(EVENT_USER_LEAVE);
				DEV_LOG("Phase Server List - EVENT_USER_LEAVE Req [%d][%d] // EVENT_ENTER_PW\n", m_SelectedServerIndex, g_pServerInfo->GetCurGameServerType());
			}
			else
			{
				// �����ϸ� �پ� �ִ� ������ �����Ѵ�.
				g_pServerInfo->SetGameServer(m_PrevServerIndex, m_PrevServerTab);

				QueryPasswordParam	Param;
				Param.pMsg			= GAME_STRING("STBL_IDX_EP_LOBBY_WRONG_PW_Q");
				Param.LimitCount	= NET_SERVER_PW-1;
				Param.OnlyNumber	= FALSE;
				Param.ShowPassword	= FALSE;
				Param.Callback		= ServerPasswordCallback;
				Param.pUserContext	= this;

				((CStageReady*)m_pParent)->TogglePopup(POPUP_QUERY_PASSWORD, &Param);

				InputDiable(TRUE);
			}
		}
		break;
	default:
		I3ASSERT("ó���� �� ���� �޽���.. ServerListFrame");
		break;
	}
}


BOOL	CPhaseServerList::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "ssServerListWin", m_pServerListWin, -1);
	LinkControlEx(plist, "ssServerListFrame", m_pServerListFrame, -1);
	LinkControlEx(plist, "ssServerListScrollFrame", m_pServerListScrollFrame, -1);
	LinkControlEx(plist, "ssServerListScrollList", m_pServerListScrollList, GCI_SSP_S_SERVER_SCROLL);

	INT32 Index;
	char	ControlName[MAX_PATH] = {};
	for ( INT32 i = 0 ; i < MAX_SERVER_LIST_SLOT ; ++i )
	{
		Index = i+1;

		i3String::Format(ControlName, MAX_PATH, _T("ssServerSlot%d"), Index);
		LinkControlEx(plist, ControlName, m_ServerListSlots[i].pFrame, GCI_SSP_S_SERVER_SLOT_1+i);

		i3String::Format(ControlName, MAX_PATH, _T("ssServerSlotText%d"), Index);
		LinkControlEx(plist, ControlName, m_ServerListSlots[i].pText, -1);

		i3String::Format(ControlName, MAX_PATH, _T("ssServerSlotUserGage%d"), Index);
		LinkControlEx(plist, ControlName, m_ServerListSlots[i].pUserGauge, -1);
	}

	LinkControlEx(plist, "ssServer_ListRefresh", m_pServerRefresh, GCI_SSP_S_SERVER_REFRESH);
	LinkControlEx(plist, "ssServer_Enter", m_pServerEnter, GCI_SSP_S_SERVER_ENTER);

	for ( INT32 i = 0 ; i < MAX_UI_TAP ; ++i )
	{
		i3String::Format(ControlName, MAX_PATH, _T("ssServer_Tab%d"), i);
		LinkControlEx(plist, ControlName, m_Tab[i].pCtrl, GCI_SSP_S_SERVER_TAB_0+i);
	}

	return TRUE;
}


void	CPhaseServerList::OnInitControl(void)
{
	INIT_BUTTON_EX(m_pServerRefresh);
	INIT_BUTTON_EX(m_pServerEnter);

	for ( INT32 i = 0 ; i < MAX_SERVER_LIST_SLOT ; ++i )
	{
		INIT_BUTTON_EX(m_ServerListSlots[i].pFrame);
		m_ServerListSlots[i].pText->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_ServerListSlots[i].pText->SetTextColor(GetColor(GCT_DEFAULT));
		m_ServerListSlots[i].pText->setInputDisable(TRUE);
		m_ServerListSlots[i].pUserGauge->setInputDisable(TRUE);
	}

	m_pServerListScrollList->SetCurValue(0.0f);
	m_pServerListScrollList->CreateTextEx(SERVER_LIST_SLOT_VERT_COUNT,  GetDefaultFontName());
	m_pServerListScrollList->SetSelColor(0, 0, 0, 0);


	for ( INT32 i = 0 ; i < MAX_UI_TAP ; ++i )
	{
		INIT_CAPTION_BUTTON(m_Tab[i].pCtrl, INVALID_TEXT);
	}


#ifdef A_LOT_OF_SERVERS
	SChannelType	Type = S_CHANNEL_TYPE_NORMAL;
	SetCurrentTab(S_CHANNEL_TYPE_NORMAL);
	UpdateServerTab();
	OnChangeTab(S_CHANNEL_TYPE_NORMAL);
#else
	SChannelType	Type = S_CHANNEL_TYPE_ALL;
	SetCurrentTab(S_CHANNEL_TYPE_ALL);
	UpdateServerTab();
	OnChangeTab(S_CHANNEL_TYPE_ALL);
#endif

	// StageReady�� �����ɶ����� ȣ��Ǳ� ������ �Ź� SetCurGameServerType�� ȣ���ϸ� �ȵȴ�.
	// ������ �����ȵǾ� ���� ���� �����ϵ��� �ؾ��Ѵ�.
	if ( g_pServerInfo->GetCurGameServerType() == S_CHANNEL_TYPE_UNDEFINED )
	{
		g_pServerInfo->SetCurGameServerType(Type);
	}


	m_pServerListWin->SetEnable(FALSE);

	m_SlidingY = m_pServerListWin->getPositionY();
}


void	CPhaseServerList::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_SSP_S_SERVER_SCROLL:
		if ( I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent )
			OnChangeServerListScroll();
		break;

	case GCI_SSP_S_SERVER_TAB_0:
	case GCI_SSP_S_SERVER_TAB_1:
	case GCI_SSP_S_SERVER_TAB_2:
	case GCI_SSP_S_SERVER_TAB_3:
	case GCI_SSP_S_SERVER_TAB_4:
	case GCI_SSP_S_SERVER_TAB_5:
		{
			if ( I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent )
			{
				INT32 Index = pNotify->m_nID - GCI_SSP_S_SERVER_TAB_0;
				OnChangeTab(m_Tab[Index].Type);
			}
		}
		break;

	case GCI_SSP_S_SERVER_SLOT_1:
	case GCI_SSP_S_SERVER_SLOT_2:
	case GCI_SSP_S_SERVER_SLOT_3:
	case GCI_SSP_S_SERVER_SLOT_4:
	case GCI_SSP_S_SERVER_SLOT_5:
	case GCI_SSP_S_SERVER_SLOT_6:
	case GCI_SSP_S_SERVER_SLOT_7:
	case GCI_SSP_S_SERVER_SLOT_8:
	case GCI_SSP_S_SERVER_SLOT_9:
	case GCI_SSP_S_SERVER_SLOT_10:
	case GCI_SSP_S_SERVER_SLOT_11:
	case GCI_SSP_S_SERVER_SLOT_12:
	case GCI_SSP_S_SERVER_SLOT_13:
	case GCI_SSP_S_SERVER_SLOT_14:
	case GCI_SSP_S_SERVER_SLOT_15:
	case GCI_SSP_S_SERVER_SLOT_16:
	case GCI_SSP_S_SERVER_SLOT_17:
	case GCI_SSP_S_SERVER_SLOT_18:
	case GCI_SSP_S_SERVER_SLOT_19:
	case GCI_SSP_S_SERVER_SLOT_20:
	case GCI_SSP_S_SERVER_SLOT_21:
	case GCI_SSP_S_SERVER_SLOT_22:
	case GCI_SSP_S_SERVER_SLOT_23:
	case GCI_SSP_S_SERVER_SLOT_24:
	case GCI_SSP_S_SERVER_SLOT_25:
		{
			INT32 SlotIndex = pNotify->m_nID - GCI_SSP_S_SERVER_SLOT_1;
			I3ASSERT(SlotIndex >= 0 && SlotIndex < MAX_SERVER_LIST_SLOT);
			if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent)
			{
				if (FALSE == m_ServerListSlots[SlotIndex].pFrame->GetShapeEnable(SLOT_SHAPE_INDEX_SELECTED) &&
					FALSE == m_ServerListSlots[SlotIndex].pFrame->GetShapeEnable(SLOT_SHAPE_INDEX_HOVER))
				{
					m_ServerListSlots[SlotIndex].pFrame->SetAllShapeEnable(FALSE);
					m_ServerListSlots[SlotIndex].pFrame->SetShapeEnable(SLOT_SHAPE_INDEX_HOVER, TRUE);
				}

			}
			if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)
			{
				m_SelectedServerIndex = m_ServerListStartSlot + SlotIndex;
				m_ServerListSlots[SlotIndex].pFrame->SetAllShapeEnable(FALSE);
				m_ServerListSlots[SlotIndex].pFrame->SetShapeEnable(SLOT_SHAPE_INDEX_SELECTED, TRUE);
			}
			if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)
			{
				TryEnterServer(m_SelectedServerIndex, m_CurrentTab);
			}
		}
		break;

	case GCI_SSP_S_SERVER_REFRESH:
		{
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			{
				if ( g_pGameContext->GetAccTime() - m_LastRefreshTime > g_pConfigEx->GetNetwork().RefreshInterval_ServerList )
				{
					m_LastRefreshTime = g_pGameContext->GetAccTime();
					I3TRACE0("EVENT_GET_SERVER_LIST\n");
					g_pGameContext->SetEvent(EVENT_GET_SERVER_LIST);
				}
			}
		}
		break;

	case GCI_SSP_S_SERVER_ENTER:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			TryEnterServer(m_SelectedServerIndex, m_CurrentTab);
		break;
	}
}


void	CPhaseServerList::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	if ( INVALID_SERVER_INDEX != m_SelectedServerIndex )
	{
		INT32 index = m_SelectedServerIndex - m_ServerListStartSlot;
		if ( index >= 0 && index < MAX_SERVER_LIST_SLOT )
		{
			m_ServerListSlots[index].pFrame->SetAllShapeEnable(FALSE);
			m_ServerListSlots[index].pFrame->SetShapeEnable(SLOT_SHAPE_INDEX_SELECTED, TRUE);
		}
	}

	// �Ϲ��� ��쿡�� ���� ���� �ε� �� Ŀ���Ҹ޼����� ����Ѵ�.(1ȸ)
#if defined(LOCALE_NORTHAMERICA)
	if(!m_pMBInfoWait && i3String::Length(g_pGameContext->GetCustomMSG()) > 0)
	{
		ERROR_POPUP_C(g_pGameContext->GetCustomMSG(), NULL , this);
		g_pGameContext->InitCustomMsg();
	}
#endif

	UpdateServerTab();
	UpdateServerSlotList();
}


void	CPhaseServerList::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for ( INT32 i = 0 ; i < scrollby ; ++i )
			m_pServerListScrollList->MovePrev();
	}
	else
	{
		for ( INT32 i = 0 ; i > scrollby ; --i )
			m_pServerListScrollList->MoveNext();
	}

	OnChangeServerListScroll();
}


void	CPhaseServerList::OnEntranceStart(void)
{
	CPhaseBase::OnEntranceStart();

	// �� ������ �� �ѹ��� �޾ƾ��Ѵ�.
	if ( ! g_pGameContext->GetReceivedMyInfo() )
	{
#if defined(CHANGE_SERVER_MOVE)
#else
		// �������� ���� �ʾҴٸ� ������ �پ��־���մϴ�.
		// �α��οϷ��� ó���� �������� ��û�ϱ� �����Դϴ�.
		I3ASSERT( g_pGameContext->GetEnteredServer() == TRUE );
#endif

		// �α��� �� �� �ѹ��� �޾ƾ��Ѵ�.
		I3ASSERT(m_pMBInfoWait == NULL);
		m_pMBInfoWait = ERROR_POPUP_W(GAME_STRING("STBL_IDX_EP_SERVER_WAIT_GET_MYINFO"));

		//// 1.5 Check GetMyInfo
		//DEV_LOG("CPhaseServerList::OnEntranceStart - EVENT_GET_MYINFO \n");
		//g_pGameContext->SetEvent(EVENT_GET_MYINFO);
		g_pGameContext->SetEvent(EVENT_GET_MYOPTION_N);
	}

#if defined(CHANGE_SERVER_MOVE)
#else
	if ( ! g_pGameContext->GetEnteredServer() )
	{
		// ������ ����Ǿ� �ִ� ���¸� ���ͻ��¶�� �ϰڽ��ϴ�.
		// ���� ���� ���°� �ƴ϶�� ���Ӽ��� ���� ���¿��� ������ ����
		// ����Ʈ���� �������� ������ �õ��� ���Դϴ�.
		// ���� ���¶�� �α��οϷ��� ��������Ʈ�� ���� ���Դϴ�.

		InputDiable(TRUE);
		g_pGameContext->SetEvent(EVENT_CONNECT_SERVER_GATEWAY);
		m_ConnectFailedCount	= 0;
	}
#endif

	m_pServerListWin->SetEnable(TRUE);


	// ���õ� ���� �ε����� �����ϴ� ������ ó�� ��������Ʈ�������� ���� ��� ����
	// ���� �� ���� �����̳� ��Ÿ ���� ������ �ٽ� ��������Ʈ�� ���Եɰ��
	// ���� ������ �־��� ������ ������� �� �� �ְ� �ϱ� ���ؼ� �Դϴ�.
	// ó������ -1�� ���Եǰ����� �������� �ʽ��ϴ�.
	m_SelectedServerIndex	= g_pServerInfo->GetCurGameServerIndexByType();
	SetCurrentTab(g_pServerInfo->GetCurGameServerType());
	OnChangeTab(g_pServerInfo->GetCurGameServerType());

	m_PrevServerIndex	= g_pServerInfo->GetCurGameServerIndexByType();
	m_PrevServerTab		= g_pServerInfo->GetCurGameServerType();

#if defined(CHANGE_SERVER_MOVE)
	if (g_pCommunity->IsInviteMoving())
	{
		INT32 MoveServerIndex = static_cast<INT32>(g_pCommunity->GetMovingServer());
		
#ifdef A_LOT_OF_SERVERS
		UINT32 uiIndex = 0;
		SChannelType eType = S_CHANNEL_TYPE_UNDEFINED;

		g_pServerInfo->GetIndexByTypeAndType(MoveServerIndex, uiIndex, eType);

		if(eType == S_CHANNEL_TYPE_UNDEFINED) {
			I3ASSERT_0;
			return;
		}

		BOOL Rv = TryEnterServer(uiIndex, eType);
#else
		BOOL Rv = TryEnterServer(MoveServerIndex, S_CHANNEL_TYPE_ALL);
#endif
		if (Rv == FALSE)
		{
			g_pCommunity->ComplateInviteMove();
		}
	}
#endif
	DEV_LOG("CPhaseServerList::OnEntranceStart");
}

void	CPhaseServerList::OnEntranceEnd(void)
{
	CPhaseBase::OnEntranceEnd();
#if defined(LOCALE_CHINA) && defined(USE_SDOA_LOGIN)
	if(g_FirstIGW)
	{
		if(g_pSDOA_Link->GetView())
			g_pSDOA_Link->SetSidabar(0,685);
		g_FirstIGW = false;
	}
#endif
}

void	CPhaseServerList::OnExitEnd(void)
{
	CPhaseBase::OnExitEnd();
	I3ASSERT( m_pMBInfoWait == NULL );
	m_pServerListWin->SetEnable(FALSE);

	DEV_LOG("CPhaseServerList::OnExitEnd");
}


BOOL	CPhaseServerList::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if ( ! CPhaseBase::OnSliding(rDeltaSeconds) )
		return FALSE;

	m_pServerListWin->setPositionY(m_SlidingY + CPhaseBase::GetSlideY());
	return TRUE;
}


void	CPhaseServerList::OnChangeTab(SChannelType TabIndex)
{
	ServerTab * pTab = NULL;

	for(INT32 i=S_CHANNEL_TYPE_ALL; i<S_CHANNEL_TYPE_MAX; i++) {
		pTab = GetTab((SChannelType)i);
		if(pTab != NULL) {
			pTab->SetSelect(FALSE);
		}
	}

	ServerTab * pNewTab = GetTab(TabIndex);
	
	I3ASSERT_ONCE(pNewTab != NULL);

	if ( pNewTab )
		pNewTab->SetSelect(TRUE);

	m_CurrentTab = TabIndex;
	m_SelectedServerIndex	= INVALID_SERVER_INDEX;

	m_ServerListStartSlot	= 0;
	m_pServerListScrollList->SetCurValue(0.0f);
}


void	CPhaseServerList::OnChangeServerListScroll()
{
	m_ServerListStartSlot = m_pServerListScrollList->GetCurrentTop() * SERVER_LIST_SLOT_HORI_COUNT;
}


BOOL	CPhaseServerList::TryEnterServer(INT32 ServerIndex, SChannelType SType /*= S_CHANNEL_TYPE_UNKNOWN*/)
{
	if ( ServerIndex == INVALID_SERVER_INDEX )
		return FALSE;


	const CServerInfo * pInfo = g_pServerInfo->GetServerInfo(ServerIndex, SType);
	I3ASSERT(pInfo != NULL);
	if (!pInfo)
		return FALSE;

	//
	// �����ο� üũ (GM�� ������� ����)
	//
	if (g_pGameContext->IsGameMaster() == FALSE &&
		GameGUI::BLOCK_GAUGE_FULL == GameGUI::GetServerGauge(pInfo->GetUserCount(), pInfo->GetMaxUserCount()))
	{
		ERROR_POPUP(GAME_STRING("STBL_IDX_EP_SERVER_USER_FULL_C"));
		return FALSE;
	}

	if (g_pServerInfo->GetGameServerIndex(ServerIndex, SType) != g_pServerInfo->GetCurGameServerIndex())
	{

		if (pInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP)
		{
			QueryPasswordParam	Param;
			Param.pMsg			= GAME_STRING("STBL_IDX_EP_LOBBY_JOIN_ROOM_Q");
			Param.LimitCount	= NET_SERVER_PW-1;
			Param.OnlyNumber	= FALSE;
			Param.ShowPassword	= FALSE;
			Param.Callback		= ServerPasswordCallback;
			Param.pUserContext	= this;

			((CStageReady*)m_pParent)->TogglePopup(POPUP_QUERY_PASSWORD, &Param);
			InputDiable(TRUE);
			return TRUE;
		}
	}

	return ForceEnterServer(ServerIndex, SType);
}


BOOL	CPhaseServerList::ForceEnterServer(INT32 ServerIndex, SChannelType SType /*= S_CHANNEL_TYPE_UNKNOWN*/)
{
	m_TargetServerIndex		= ServerIndex;
	m_TargetServerTab		= SType;

#if defined(CHANGE_SERVER_MOVE)
	if (g_pServerInfo->GetGameServerIndex(ServerIndex, SType) != g_pServerInfo->GetCurGameServerIndex())
	{
		g_pServerInfo->SetGameServer(ServerIndex, SType);
		g_pGameContext->SetEventOnce(EVENT_CONNECT_SERVER_GAME);

		InputDiable(TRUE);
	}
	else
	{
		((CStageReady *)m_pParent)->MovePhase(PHASE_CHANNEL_LIST);
	}
#else
	I3NOTICE("Phase Server List - EVENT_USER_LEAVE Req [%d][%d]\n", ServerIndex, SType);
	g_pServerInfo->SetGameServer(ServerIndex, SType);
	g_pGameContext->SetEventOnce(EVENT_USER_LEAVE);

	InputDiable(TRUE);
#endif

	return TRUE;
}


BOOL	CPhaseServerList::ForceEnterSelectServer()
{
	return ForceEnterServer(m_SelectedServerIndex, m_CurrentTab);
}


void	CPhaseServerList::UpdateServerTab()
{
	I3ASSERT( g_pConfigEx->GetGUI().pServerListTab != NULL );
	const ConfigGUI::ServerTypeIndexContainer	& ServerTabList = (*g_pConfigEx->GetGUI().pServerListTab);

	// ���� ����Ÿ�Ժ� ������ �� �ִ°� ����Ѵ�.
	// �ش� Ÿ�Կ� �����ϴ� ���� ������ 0�̶�� ���� �����Ѵ�.
	size_t	ValidIndex = 0;
	for ( size_t i = 0 ; i < ServerTabList.size() ; ++i )
	{
		const SChannelType	CheckType = ServerTabList[i].first;

		if ( g_pServerInfo->GetServerTypeCount(CheckType) <= 0 )
			continue;

#ifdef A_LOT_OF_SERVERS
		if(CheckType == S_CHANNEL_TYPE_ALL) {
			continue;
		}
#endif

		if ( ValidIndex < MAX_UI_TAP)
		{
			m_Tab[ValidIndex].SetEnable(TRUE);
			m_Tab[ValidIndex].SetType(CheckType);
			ValidIndex++;
		}
	}

	// ������ �ʴ� ���� ��Ȱ��ȭ�Ѵ�.
	for ( INT32 i = ValidIndex ; i < MAX_UI_TAP ; ++i )
	{
		m_Tab[i].SetEnable(FALSE);
	}

	// ���� ������ ������ ���� �ǹ̾��� ���̶�� ALL���� �����ϵ��� �Ѵ�.
	if ( g_pServerInfo->GetServerTypeCount(GetCurrentTab()) <= 0 )
	{
#ifdef A_LOT_OF_SERVERS
		OnChangeTab(S_CHANNEL_TYPE_NORMAL);
#else
		OnChangeTab(S_CHANNEL_TYPE_ALL);
#endif
	}
}


void	CPhaseServerList::UpdateServerSlotList()
{
	// ���� ��Ÿ�Կ� ���� ���� ������ ����.
	INT32		ServerCount		= g_pServerInfo->GetServerTypeCount(static_cast<SChannelType>(m_CurrentTab));
	i3List *	pServerInfoList	= g_pServerInfo->GetServerInfoList(static_cast<SChannelType>(m_CurrentTab));

	// ����Ʈ ��Ʈ�Ѹ� �������ش�.
	INT32 ScrollCount = GameGUI::CalcuScrollCount(ServerCount, SERVER_LIST_SLOT_HORI_COUNT);
	GameGUI::Reset(m_pServerListScrollList, ScrollCount);

	// ��ũ�ѵȸ�ŭ �������� ���� ������ �����Ѵ�.
	INT32	DispServerCount		= ServerCount - m_ServerListStartSlot;

	for ( INT32 i = 0 ; i < MAX_SERVER_LIST_SLOT ; ++i )
	{
		ServerListSlot * pSlot	= &m_ServerListSlots[i];
		if ( i >= DispServerCount )
		{
			pSlot->SetEnable(FALSE);
			continue;
		}

		INT32	ServerIndex					= m_ServerListStartSlot+i;
		const CServerInfo *	pInfo			= (const CServerInfo *)pServerInfoList->GetSafeItem(ServerIndex);
		I3ASSERT( pInfo != NULL );
		BOOL				SelectedSlot	= m_SelectedServerIndex == ServerIndex;


		pSlot->SetEnable(TRUE);						// ����ϴ� ������ Ȱ��ȭ�Ѵ�.
		pSlot->Update(pInfo, SelectedSlot);
	}
}

//static
void	CPhaseServerList::ServerPasswordCallback(BOOL OK, const string & Password, void * pUserContext)
{
	CPhaseServerList * pPhaseServerList = (CPhaseServerList *)pUserContext;

	if (OK)
	{
		pPhaseServerList->SetServerPassword(Password);
		pPhaseServerList->ForceEnterSelectServer();
	}
	else
	{
		if (g_pCommunity->IsInviteMoving())
		{
			g_pCommunity->ComplateInviteMove();
		}

		pPhaseServerList->InputDiable(FALSE);
	}
}


CPhaseServerList::ServerTab *		CPhaseServerList::GetTab(SChannelType Type)
{
	for ( INT32 i = 0 ; i < MAX_UI_TAP ; ++i )
	{
		if ( m_Tab[i].Type == Type )
			return &m_Tab[i];
	}
	return NULL;
}

void CPhaseServerList::InputDiable( BOOL Val )
{
 	m_pServerListWin->setInputDisable(Val);
}