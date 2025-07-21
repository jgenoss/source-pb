#include "pch.h"
#include "UIPhaseSelectChannel.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "../CommunityContext.h"
#include "../GameGUI.h"
#include "../MessageBox.h"
#include "../AwayChecker.h"
#if(0)
#define		REFRESH_LIMIT_TIME		5.0f
#define		CONNECT_LIMIT_TIME		1.0f
#define		WAIT_CONNECT_TIME		1.0f
#endif
static UIPhaseSelectChannel * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	UIPhaseBase * pPhase = pFrame->GetCurrentPhase();
	I3ASSERT( pPhase->IsExactTypeOf( UIPhaseSelectChannel::GetClassMeta()));

	return (UIPhaseSelectChannel*) pPhase;
}
#if(0)
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPhaseSelectChannel_ReqList( LuaState * L)
	{
		GetThis()->RefreshByTime();
		return 0;
	}

	int UIPhaseSelectChannel_ReqPrevList( LuaState * L)
	{
		return 0;
	}

	int UIPhaseSelectChannel_ReqNextList( LuaState * L)
	{
		return 0;
	}

	int UIPhaseSelectChannel_EnterChannel( LuaState * L)
	{
		INT32 channel = i3Lua::GetIntegerArg( L, 1);
		if( channel != -1)
			GetThis()->SelectChannel( channel);
		GetThis()->TryEnterChannel();
		return 0;
	}

	int UIPhaseSelectChannel_SelectChannel( LuaState * L)
	{
		INT32 slot = i3Lua::GetIntegerArg( L, 1);
		GetThis()->SelectChannel( slot);
		return 0;
	}
}

LuaFuncReg PhaseSelectChannel_Glue[] = 
{
	{"Refresh",						UIPhaseSelectChannel_ReqList},		// ���� ����Ʈ ��û
	{"uiReqPrevChannelList",		UIPhaseSelectChannel_ReqPrevList},	// ���� Page ���� ����Ʈ ��û
	{"uiReqNextChannelList",		UIPhaseSelectChannel_ReqNextList},	// ���� Page ���� ����Ʈ ��û
	{"EnterChannel",				UIPhaseSelectChannel_EnterChannel},	// ������ ����
	{"SelectChannel",				UIPhaseSelectChannel_SelectChannel},

	{NULL, NULL}
};
#endif

I3_CLASS_INSTANCE( UIPhaseSelectChannel, UIPhaseBase);

UIPhaseSelectChannel::UIPhaseSelectChannel()
{
	m_LastServerNumber	= INVALID_INDEX;
	m_LastChannelNumber	= INVALID_INDEX;
	m_nLastedSelectSlot = INVALID_INDEX;
#if(0)
	m_fRefreshClickElapseTime = REFRESH_LIMIT_TIME;
	m_fConnectClickElapseTime = CONNECT_LIMIT_TIME;
#endif
	m_bInviteMoving = false;
}

UIPhaseSelectChannel::~UIPhaseSelectChannel()
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


#if(0)
CHANNEL_GAUGE UIPhaseSelectChannel::__CalcChannelGauge(INT32 UserCount, INT32 MaxUserCount, const i3::vector<INT32>& pPercentLevelList)
{
	INT32 i;

	// ������ �������ų� ������ �߸��Ǿ� ������ �ΰ���� 0���� �ɴϴ�.
	if (UserCount == 0 && MaxUserCount == 0)
		return CHANNEL_GAUGE_1;

	I3ASSERT(UserCount >= 0 && MaxUserCount > 0);
	if ( MaxUserCount <= 0 )
		MaxUserCount = 1;

	REAL32	rUserPercent = static_cast<REAL32>(UserCount) / static_cast<REAL32>(MaxUserCount) * 100.0f;
	INT32	UserPercent = MINMAX(0, static_cast<INT32>(rUserPercent), 100);

	for( i = 0 ; i < CHANNEL_GAUSE_MAX ; ++i )
	{
		INT32 Level =pPercentLevelList[i];
		if ( UserPercent <= Level )
		{
			return (CHANNEL_GAUGE) i;
		}
	}

	// for������ ������������ ��ȯ���� ���Ѵٸ�
	// �ִ��������� �Ѿ���̹Ƿ� FULL�� ó���� �Ѵ�.
	return CHANNEL_GAUGE_FULL;
}

void UIPhaseSelectChannel::_UpdateChannelLabel(void)
{
#if defined(CHANGE_SERVER_MOVE)
#else
	if ( ! g_pGameContext->GetEnteredServer() )
		return;
#endif

	INT32 i;

	LuaState * L = _CallLuaFunction( "ClearList");
	if( L != NULL)
	{
		i3Lua::PushInteger( L, GAME_CHANNEL_COUNT);
		_EndLuaFunction( L, 1);
	}

	for( i = 0 ; i < GAME_CHANNEL_COUNT ; i++)
	{
		UINT8 channelType = g_pGameContext->GetChannelType(i);

		_SetServerEnv( g_pServerInfo->GetCurGameServerIndex(), i, channelType);
	}

	m_nServerChannelIdx = 0;

//���� ������ ���� �ڵ� - ������
#ifdef FAST_FAST_FAST
	m_fConnectClickElapseTime = -0.1f;
	m_nLastedSelectSlot = FAST_CHANNEL;
	TryEnterChannel();
#endif
	//if( m_nLastedSelectSlot >= 0)
	//	OnFocus(m_nLastedSelectSlot);
}

void UIPhaseSelectChannel::_SetServerEnv( INT32 nServer, INT32 nChannel, INT32 nChannelState)
{
	char strBuf[MAX_STRING_COUNT*2];
	
	INT32 nEvent = 0;

	I3ASSERT( nChannel < MAX_CHANNEL_CTRL);

	// ���� ������
	UINT32	userCount		= g_pGameContext->GetChannelUserCount(m_nServerChannelIdx);
	UINT32	MaxUserCount	= g_pGameContext->GetChannelMaxUser();
	INT32	userGage		= __CalcChannelGauge(userCount, MaxUserCount, g_pConfigEx->GetGUI().pGaugeLevel_ChannelList);

	I3_BOUNDCHK( SERVERSELECT_MAINTYPE_EVENT0 + nEvent, SERVERSELECT_MAINTYPE_COUNT);

	SERVERSELECT_MAINTYPE mainType = SERVERSELECT_MAINTYPE_BEGINNER;
	SERVERSELECT_SUBTYPE subType = SERVERSELECT_SUBTYPE_CATEGORY_SLOT_TEXT;

	// ���� �̸�
	if( userGage == CHANNEL_GAUGE_FULL)		//	���� Ǯ ����
		subType = SERVERSELECT_SUBTYPE_CATEGORY_SLOT_FULL_TEXT;

	switch(nChannelState)
	{
	case CHANNEL_TYPE_BEGIN1:		mainType = SERVERSELECT_MAINTYPE_BEGINNER;			break;
	case CHANNEL_TYPE_BEGIN2:		mainType = SERVERSELECT_MAINTYPE_BEGINNER2;			break;
	case CHANNEL_TYPE_NORMAL:
	case CHANNEL_TYPE_NORMAL2:		mainType = SERVERSELECT_MAINTYPE_NORMAL;			break;
	case CHANNEL_TYPE_CLAN:			mainType = SERVERSELECT_MAINTYPE_CLAN;				break;
	case CHANNEL_TYPE_EXPERT:		mainType = SERVERSELECT_MAINTYPE_EXPERT;			break;
	case CHANNEL_TYPE_EXPERT2:		mainType = SERVERSELECT_MAINTYPE_EXPERT2;			break;
	case CHANNEL_TYPE_CHAMPIONSHIP:	mainType = SERVERSELECT_MAINTYPE_CHAMPIONSHIP;		break;
	case CHANNEL_TYPE_MIDDLE:		mainType = SERVERSELECT_MAINTYPE_MIDDLE;			break;
	default:
		mainType = (SERVERSELECT_MAINTYPE)(SERVERSELECT_MAINTYPE_EVENT0 + nEvent);
		break;
	}
	
	i3String::Format( strBuf, sizeof(strBuf), "%s\n", m_szServerText[mainType][subType]);

	// �������� ���δ�.
	i3String::Concat( strBuf, m_szServerText[mainType][SERVERSELECT_SUBTYPE_CATEGORY_TEXT]);

	// ���� ��ȣ
	char szNumber[ MAX_STRING_COUNT];
	sprintf_s( szNumber, "%03d", (nServer * 10) + m_nServerChannelIdx + 1);

	COLOR col;

	if( userGage == CHANNEL_GAUGE_FULL)
	{	
		i3Color::Set( &col, (UINT8) 173, 173, 173, 180);
	}
	else
	{
		i3Color::Copy( &col, GetColor(GCT_DEFAULT));
	}

	SetChannelSlot( nChannel, szNumber, strBuf, -1, userGage, &col, NULL);

	// ���� �ε��� ����.
	m_nServerChannelIdx++;
	m_nServerCount[ mainType]++;
}

bool UIPhaseSelectChannel::_CheckEnterChannel( INT32 channel)
{
	// ������ ��ã���� ���� �Ұ�
	INT32 userCount = g_pGameContext->GetChannelUserCount( channel);

	if (g_pGameContext->GetChannelMaxUser() <= userCount)
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_STRING("STBL_IDX_EP_SERVER_USER_FULL_C"));	// _C �̳�, �ݹ� ������� �ʴ´�.
		return false;
	}

	// ������ �� �ִ��� �˻��Ѵ�.
	ChannelType channelType = static_cast<ChannelType>(g_pGameContext->GetChannelType( channel));

	if( g_pGameContext->IsValidChannelType(channelType) == FALSE)
	{
		// ������ �� ������ �����޽����� �ѷ��ش�.
		PrintMessageForChannelEnterError(channelType);
		return false;
		
	}

	// ����
	return true;
}

void UIPhaseSelectChannel::_LoadServerTextFromFile( void)
{
	INT32 nMode = 0;
	WCHAR16 szwReadLine[512] = L"";
	char filePath[MAX_PATH] = "";
	char szReadLine[512*2] = {0,};
	char szTemp[512] = {0,};
	i3FileStream file;

	//	multi path
	ConvertLocalePath( filePath, "Script/String/ServerSelect_Ver2.txt", LOCALE_PATH_STRING_TABLE);

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

		// ����
		if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_BEGINNER1]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_BEGINNER;	// �ʺ�
		}
		else if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_BEGINNER2]" ) >= 0)
		{
			nMode = SERVERSELECT_MAINTYPE_BEGINNER2;	// �ʺ� 2
		}
		else if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_NORMAL]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_NORMAL;	// �Ϲ�
		}
		else if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_EXPERT1]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_EXPERT;	// ���
		}
		else if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_EXPERT2]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_EXPERT2;	// ��� 2
		}
		else if( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_CLAN]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_CLAN;	// Ŭ��
		}
		else if ( i3String::Contain( szReadLine, "[STR_SERVER_SELECT_CATEGORY_CHAMPIONSHIP]" ) >= 0 )
		{
			nMode = SERVERSELECT_MAINTYPE_CHAMPIONSHIP;
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
				i3String::NCopy( GetMainFrame()->getChannelSlotText(nMode), szTemp, nLength);
			}
		}
	}

	file.Close();
}

void UIPhaseSelectChannel::_CreateVerifiMark( void)
{
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
}

void UIPhaseSelectChannel::_SetButtonState( void)
{
	LuaState * L = _CallLuaFunction( "SetButtonState");
	if( L != NULL)
	{
		if( m_nLastedSelectSlot != -1)
			i3Lua::PushInteger( L, 1);
		else
			i3Lua::PushInteger( L, 0);

		i3Lua::PushInteger( L, 1);
		_EndLuaFunction( L, 2);
	}
}


/*virtual*/ void UIPhaseSelectChannel::OnCreate( i3GameNode * pParent)
{
	I3NOTICE( "UIPhaseSelectChannel::OnCreate() Enter");

	UIPhaseBase::OnCreate( pParent);

	// Load Scene
	AddScene( "UIRe/PointBlankRe_Channel_List.i3UIe", PhaseSelectChannel_Glue);

	_LoadServerTextFromFile();

	// ���ӿ��� ����ϴ� ���������� �ε��մϴ�.
	g_pFramework->LoadNoticeFromFile();

	_CreateVerifiMark();
}

/*virtual*/ void UIPhaseSelectChannel::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate( rDeltaSeconds);

	m_fRefreshClickElapseTime -= rDeltaSeconds;
	m_fConnectClickElapseTime -= rDeltaSeconds;
}

/*virtual*/ void UIPhaseSelectChannel::ProcessGameEvent( GAME_EVENT evt, INT32 arg)
{
	UIPhaseBase::ProcessGameEvent( evt, arg);

	switch( evt)
	{
#if !defined(CHANGE_SERVER_MOVE)
	case EVENT_CONNECT_SERVER_GAME:
		// ����Ʈ���� -> ���Ӽ��� ���ؽÿ��� �߻��ϴ� �̺�Ʈ.
		if( EV_SUCCESSED(arg))
		{
			m_bMyPhase = true;
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
		//InputDisable(FALSE);

		if( EV_SUCCESSED(arg) )
		{
			m_bMyPhase = true;

			g_pGameContext->SetEvent(EVENT_DISCONNECT);

			if (g_pCommunity->IsInviteMoving())
			{
				g_pGameContext->SetEvent(EVENT_CONNECT_SERVER_GAME);
			}
			else
			{
				GetMainFrame()->MovePhase(UIPHASE_SERVER_CHANNEL_LIST);
			}
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_STRING("STBL_IDX_EP_SERVER_FAIL_MOVE"));
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
			// �ϵ��� : Ŭ�� ä�η� �̵��� ó��...
			else if( g_pCommunity->IsClanChannelMoving())
			{
				for( INT32 i = 0 ; i < GAME_CHANNEL_COUNT ; i++)
				{
					UINT8 channelType = g_pGameContext->GetChannelType(i);
					if(channelType == CHANNEL_TYPE_CLAN && g_pGameContext->GetChannelUserCount(i) < g_pGameContext->GetChannelMaxUser())
					{
						SelectChannel(i);
						TryEnterChannel();
						i = GAME_CHANNEL_COUNT;
					}
				}
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
			GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_STRING("STBL_IDX_EP_LOGIN_FAIL_CONNECT_E"), NULL, EXIT_CALLBACK(CbExitNormal,false), this);
		}
		break;
	case EVENT_USER_ENTER:
		SetInputDisable( false);

		if( EV_SUCCESSED(arg) )
		{
			Refresh();
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
			m_bMyPhase = true;
			g_pGameContext->SetEvent(EVENT_DISCONNECT);
#endif
			GetMainFrame()->MovePhase( UIPHASE_SERVER_CHANNEL_LIST);

			m_bMyPhase = false;

			GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_STRING("STBL_IDX_EP_SERVER_FAIL_MOVE"));
		}
		break;
	case EVENT_SELECT_CHANNEL:
		if( EV_SUCCESSED(arg))
		{
			m_bMyPhase = true;
			g_pGameContext->SetEventOnce(EVENT_ENTER_LOBBY);
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			_CloseWaitMessageBox();

			SetInputDisable( false);

			if (EV_IS_ERROR(arg, EVENT_ERROR_SERVER_USER_FULL))
			{
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_STRING("STBL_IDX_EP_SERVER_USER_FULL_C"), NULL, MAKE_CALLBACK(CbRefleshServerList), this);
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
			GetMainFrame()->MovePhase( UIPHASE_LOBBY);
			m_LastChannelNumber = g_pServerInfo->GetCurChannelNumber();
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			_CloseWaitMessageBox();
			_EhStageError(event,arg);

			SetInputDisable( false);
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
		GetMainFrame()->MovePhase(UIPHASE_SERVER_CHANNEL_LIST);
#else
		_EhFailConnect();
#endif

		DEV_LOG(" CPhaseChannelList::OnGameEvent EVENT_M_DESTROYED_NETWORK\n");
		break;
	case EVENT_INVITED_MESSAGE:
		InvitedMessage();
		break;
	case EVENT_INVITED_CLAN_MESSAGE:
		InvitedMessageClan();
		break;
	default:
		{
#if defined I3_DEBUG	//������ ���� ó��????
			_CloseWaitMessageBox();
			g_pGameContext->SetEvent( EVENT_DISCONNECT );

			_DefaultErrorPopup( GAME_STRING("STR_TBL_GUI_SERVER_DEBUG_POPUP1"), event, arg);
#endif
			m_bMyPhase = false;

			DEV_LOG("ChannelList event error : %d %d", event, arg);

			I3ASSERT_0;//ó������ EVENT�� �����մϴ�. event���� �˷��ּ��� - praptor
		}

		break;
	}
}

/*virtual*/ bool UIPhaseSelectChannel::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPhaseBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	INT32 i;

	m_bInviteMoving = false;

	for( i = 0; i < SERVERSELECT_MAINTYPE_COUNT; i++)
		m_nServerCount[i] = 0;

	SetInputDisable( false);

	if( g_pGameContext->GetEnteredServer() )
	{
		// ���ͻ��¶�� �κ񿡼� ä�θ���Ʈ�� �� ���Դϴ�.
		if( g_pCommunity->IsInviteMoving())
		{
			OnInviteMove();
		}

		else
		{
			Refresh();
		}
	}
	else
	{
#if defined(CHANGE_SERVER_MOVE)
		// ���ͻ��°� �ƴ϶�� ���� ���Ӽ����� ���� ���̴�.
		g_pGameContext->SetEvent(EVENT_USER_ENTER);
#else
		// ���ͻ��°� �ƴ϶�� ����Ʈ���� ��������
		// ���Ӽ����� �̵��� �� ���Դϴ�.
		g_pGameContext->SetEvent(EVENT_CONNECT_SERVER_GAME);
#endif
		SetInputDisable( true);
		m_bMyPhase = false;
	}


	// ������ �پ��� ä���� �����ؼ� �����ݴϴ�.
	// ������ �پ��� ������ �ٸ� ������ ������ �����մϴ�.
	if( m_LastServerNumber != g_pServerInfo->GetCurGameServerIndex() )
	{
		SelectChannel( INVALID_INDEX);
		m_LastChannelNumber	= INVALID_INDEX;
	}
	else
	{
		if ( m_LastChannelNumber != INVALID_INDEX)
			SelectChannel( m_LastChannelNumber+1);
	}

	m_LastServerNumber	= g_pServerInfo->GetCurGameServerIndex();

	// ��ư ����
	_SetButtonState();

	DEV_LOG("CPhaseChannelList::OnEntranceStart");

	return true;
}

/*virtual*/ void UIPhaseSelectChannel::OnEntranceEnd( void)
{
	UIPhaseBase::OnEntranceEnd();

	i3UI::setDisableUIInput( false);
}

/*virtual*/ bool UIPhaseSelectChannel::OnExitStart( void)
{
	OnSlideOut();

	m_nLastedSelectSlot = -1;

	return true;
}

/*virtual*/void UIPhaseSelectChannel::OnExitEnd( void)
{
	UIPhaseBase::OnExitEnd();

	// �ʴ� �޼��� �ʱ�ȭ
	g_pGameContext->ClearInviteInfo();

}

/*virtual*/ void UIPhaseSelectChannel::OnGotoBackPhase( void)
{
#if defined(CHANGE_SERVER_MOVE)
	GetMainFrame()->MovePhase( UIPHASE_SERVER_CHANNEL_LIST);
#else
	SetInputDisable( true);
	m_bMyPhase = true;
	g_pGameContext->SetEventOnce( EVENT_USER_LEAVE);
#endif
	m_bMyPhase = false;
}

/*virtual*/ bool UIPhaseSelectChannel::OnEnterMouse( i3UIControl * pControl)
{
	if( UIPhaseBase::OnEnterMouse( pControl) == false)
		return false;

	if( i3String::Compare( pControl->GetName(), "i3UIButtonComposed30") == 0)
		PlaySound( UISND_ENTER_MOUSE1);

	return true;
}

/*virtual*/ bool UIPhaseSelectChannel::OnClick( i3UIControl * pControl)
{
	if( UIPhaseBase::OnClick( pControl) == false)
		return false;

	if( pControl->IsExactTypeOf( i3UIListView_Item::GetClassMeta()) )
		PlaySound( UISND_MOUSE_OVER);

	return true;
}

bool UIPhaseSelectChannel::OnInviteMove( void)
{
	if (g_pCommunity->GetMovingServer() != (UINT32)g_pServerInfo->GetCurGameServerIndex())
	{
		DEV_LOG("UIPhaseSelectChannel::_OnInviteMove 1 [%d / %d]\n", g_pCommunity->GetMovingChannel(), g_pServerInfo->GetCurGameServerIndex());

		m_bWaitConnect = true;

#if defined(CHANGE_SERVER_MOVE)
		g_pFramework->GetUIMainframe()->MovePhase(UIPHASE_SERVER_CHANNEL_LIST);
#else
		m_bMyPhase = true;
		INT32 MoveServerIndex = static_cast<INT32>(g_pCommunity->GetMovingServer());
		g_pServerInfo->SetGameServer(MoveServerIndex);
		g_pGameContext->SetEventOnce(EVENT_USER_LEAVE);
#endif
		m_bMyPhase = false;

		return true;
	}

	// ģ���ʴ�/ã�ư��� �� ä�� ���ѿ� �ɸ��� �̵� ����
	if (false == _CheckEnterChannel(g_pCommunity->GetMovingChannel()))
	{
		DEV_LOG("UIPhaseSelectChannel::_OnInviteMove 2 [%d / %d]\n", g_pCommunity->GetMovingChannel());
		g_pCommunity->ComplateInviteMove();
		return true;
	}

	if( m_bInviteMoving == true)
		return false;

	DEV_LOG("UIPhaseSelectChannel::_OnInviteMove 3 [%d / %d]\n", g_pCommunity->GetMovingChannel(), g_pServerInfo->GetCurGameServerIndex());

	INT32	ChannelNumber = g_pCommunity->GetMovingChannel();
	m_bMyPhase = true;
	g_pGameContext->SetEvent(EVENT_SELECT_CHANNEL, &ChannelNumber);
	SetInputDisable( true);
	m_bMyPhase = false;
	m_bInviteMoving = true;

	return true;
}

void UIPhaseSelectChannel::_EhFailConnect(void)
{
	if( m_LogInConnectFailedCount++ < g_pConfigEx->GetNetwork().ServerReconnectCount )
	{
#if defined(CHANGE_SERVER_MOVE)
#else
		m_bMyPhase = true;
		g_pGameContext->SetEvent(EVENT_CONNECT_SERVER_GAME);
#endif
	}
	else
	{
		_CloseWaitMessageBox();
		//GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_STRING("STBL_IDX_EP_SERVER_SERVER_OFF_E"), NULL, CbExitNormal, this);
		// ���� ���Ӽ����� �������ϸ� ���Ḧ �ϴ� �ڵ忴����
		// ����Ʈ���� ������ �ٰ��մϴ�.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_EP_SERVER_FAIL_REFRESH_C"));
		GetMainFrame()->MovePhase( UIPHASE_SERVER_CHANNEL_LIST);
	}
	m_bMyPhase = false;
}

void UIPhaseSelectChannel::Refresh( void)
{
	m_bMyPhase = true;
	g_pGameContext->SetEvent( EVENT_GET_CHANNEL_LIST);
}

void UIPhaseSelectChannel::RefreshByTime( void)
{
	if( m_fRefreshClickElapseTime <= 0.0f)
	{
		Refresh();
		m_fRefreshClickElapseTime = REFRESH_LIMIT_TIME;
	}
}

void UIPhaseSelectChannel::SelectChannel( INT32 channel)
{
	if( channel == INVALID_INDEX )
	{
		return;
	}

	m_nLastedSelectSlot = channel;

	// ä�� ���ÿ� ���� ��ư ���¸� ������Ʈ�Ѵ�. 
	_SetButtonState();
}

void UIPhaseSelectChannel::TryEnterChannel( void)
{
	if (0.0f < m_fConnectClickElapseTime)
		return;

	if( m_nLastedSelectSlot == -1)
		return;

	if( _CheckEnterChannel(m_nLastedSelectSlot) == false)
	{// ���� �Ұ�
		PlaySound( UISND_WARNING);
		return;
	}

	m_bWaitConnect = true;

	m_bMyPhase = true;

	g_pGameContext->SetEvent( EVENT_SELECT_CHANNEL, &m_nLastedSelectSlot);

	// �̻�� ��û���� �������� Ȯ���մϴ�.
	// �ݵ�� ���⼭ ȣ���� �ʿ䰡 ���� ������
	// �������� �����Ѵٸ� ȣ�� ��ġ�� �����Ͽ��� �����մϴ�.
	g_pGameContext->SetEvent( EVENT_RUN_PARAM_CHECK);

	m_bMyPhase = false;

	SetInputDisable( true);

	m_fConnectClickElapseTime = CONNECT_LIMIT_TIME;
	m_fRefreshClickElapseTime = REFRESH_LIMIT_TIME;

	PlaySound( UISND_SERVER_ENTER);
}

void UIPhaseSelectChannel::SetChannelSlot( INT32 slot, const char * pszNumber, const char * pszName,
										  INT32 event, INT32 Gauge, COLOR * pColor, const char * pszToolTip)
{
	LuaState * L = _CallLuaFunction( m_pLuaContextName, "SetSlotInfo");
	if( L != NULL)
	{
		i3Lua::PushInteger( L, slot);
		i3Lua::PushA2UTF8String( L, pszName);
		i3Lua::PushInteger( L, event);
		i3Lua::PushInteger( L, Gauge);
		i3Lua::PushColor( L, pColor);
		if( pszToolTip != NULL)
			i3Lua::PushA2UTF8String( L, pszToolTip);
		else
			i3Lua::PushA2UTF8String( L, "");
		_EndLuaFunction( L, 9);
	}
}


void UIPhaseSelectChannel::_EhStageError(INT32 Event,INT32 Arg)
{
	m_bMyPhase = true;
	g_pGameContext->SetEvent(EVENT_DISCONNECT);
	m_bMyPhase = false;

	_DefaultErrorPopup( GAME_STRING("STR_TBL_GUI_SERVER_DEBUG_POPUP2"), Event, Arg);
}

void UIPhaseSelectChannel::PrintMessageForChannelEnterError( ChannelType channelType )
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

	GameUI::MsgBox( MSG_TYPE_ERROR, MsgBuffer, NULL, MAKE_CALLBACK(CbRefleshServerList), this);
}

void UIPhaseSelectChannel::_CloseWaitMessageBox(void)
{
	if( m_pMBConnectWait)
	{
		GameUI::CloseMsgBox( m_pMBConnectWait);
		m_pMBConnectWait = NULL;
	}

	m_fWaitDelayTime = WAIT_CONNECT_TIME;
	m_bWaitConnect = false;
}


void UIPhaseSelectChannel::CbRefleshServerList(void* pThis,INT32 nParam)
{
	((UIPhaseSelectChannel*)pThis)->Refresh();
}

#endif