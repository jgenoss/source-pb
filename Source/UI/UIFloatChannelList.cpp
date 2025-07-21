#include "pch.h"
#include "UIFloatChannelList.h"

#include "UIMainFrame.h"
#include "UIUtil.h"
#include "UIPhaseSelectServer.h"

#include "../CommunityContext.h"
#include "../GameGUI.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "InviteContext.h"
#include "ClanGameContext.h"
#include "ChannelContext.h"
#include "UserInfoContext.h"
#include "GameContextUtil.h"

#include "Network/GameNetworkManager.h"

#include "NEW_CREATE_CHARA.h" // V3 V1.1/V1.0 �ӽ� �߰�

#include "UIServerAndChannelBanner.h"

#define		REFRESH_LIMIT_TIME		5.0f
#define		CONNECT_LIMIT_TIME		1.0f
#define		WAIT_CONNECT_TIME		1.0f

#define		MINIMUM_DELTA_TIME		0.01f									// ��ŸŸ���� ���̰��� CONNECT_LIMIT_TIME �� �̿��� ��ƾ�� ���� �ʴ� �κ� �־� �ּ� ��ŸŸ�� �� ����

static UIPhaseSelectServer * GetServerPhase(void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	UIPhaseBase * pPhase = pFrame->GetPhase(UIPHASE_SERVER_CHANNEL_LIST);

	return (UIPhaseSelectServer*) pPhase;

}

extern "C" {
	int UIFloatChannelList_EnterChannel( LuaState * L)
	{
		UIFloatChannelList * pThis = static_cast<UIFloatChannelList*>(GameUI::GetFloating( UFW_CHANNEL_LIST));
		INT32 channel = i3Lua::GetIntegerArg( L, 1);
		if( pThis != nullptr)
		{
			if( channel != -1)
				pThis->SelectChannel( channel);
			pThis->TryEnterChannel();
		}
		return 0;
	}	

	int UIFloatChannelList_SelectChannel( LuaState * L)
	{
		UIFloatChannelList * pThis = static_cast<UIFloatChannelList*>(GameUI::GetFloating( UFW_CHANNEL_LIST));
		INT32 channel = i3Lua::GetIntegerArg( L, 1);
		if( pThis != nullptr && channel != -1)
			pThis->SelectChannel( channel);
		return 0;
	}	

	int UIFloatChannelList_Refresh( LuaState * L)
	{
		UIFloatChannelList * pThis = static_cast<UIFloatChannelList*>(GameUI::GetFloating( UFW_CHANNEL_LIST));
		if( pThis != nullptr)
			pThis->RefreshByTime();
		return 0;
	}	
}

LuaFuncReg UIFloatChannelList_Glue[] =
{
	{"EnterChannel",				UIFloatChannelList_EnterChannel},	// ������ ����
	{"SelectChannel",				UIFloatChannelList_SelectChannel},
	{"Refresh",						UIFloatChannelList_Refresh},
	{nullptr,							nullptr}
};

I3_CLASS_INSTANCE(UIFloatChannelList);//, UIFloatingBase);

UIFloatChannelList::UIFloatChannelList() : m_nServerChannelIdx(0)
{
	m_LastServerNumber	= INVALID_INDEX;
	m_LastChannelNumber	= INVALID_INDEX;
	m_nLastedSelectSlot = INVALID_INDEX;

	m_fRefreshClickElapseTime = REFRESH_LIMIT_TIME;
	m_fConnectClickElapseTime = CONNECT_LIMIT_TIME;

	m_bInviteMoving = false;

	m_AlignedPos = UIFLOATING_POS_RIGHT;

	m_bEnterLobby = false;

#if !defined(USE_LOCALE_UI)
	m_pVerifiSprite = nullptr;
	m_pVerifiTexture = nullptr;
#endif

	m_bWaitConnect	= false;
	m_pMBConnectWait = nullptr;
	m_pMessageBox	 = nullptr;
	m_bMercenaryRegist = false;
	m_isWaitMercenary = false;

	RegisterGameEventFunc( EVENT_GET_CHANNEL_LIST,					&UIFloatChannelList::ReceiveGameEvent_Get_ChannelList);
	RegisterGameEventFunc( EVENT_USER_ENTER,						&UIFloatChannelList::ReceiveGameEvent_UserEnter);
	RegisterGameEventFunc( EVENT_SELECT_CHANNEL,					&UIFloatChannelList::ReceiveGameEvent_SelectChannel);
	RegisterGameEventFunc( EVENT_ENTER_LOBBY,						&UIFloatChannelList::ReceiveGameEvent_EnterLobby);
	RegisterGameEventFunc( EVENT_M_DRIVE_AWAY,						&UIFloatChannelList::ReceiveGameEvent_CloseWaitMessageBox);
	RegisterGameEventFunc( EVENT_M_ERROR,							&UIFloatChannelList::ReceiveGameEvent_CloseWaitMessageBox);
	RegisterGameEventFunc( EVENT_M_DISCONNECTION,					&UIFloatChannelList::ReceiveGameEvent_CloseWaitMessageBox);
	RegisterGameEventFunc( EVENT_M_GAMEGUARD_ERROR,					&UIFloatChannelList::ReceiveGameEvent_CloseWaitMessageBox);
	RegisterGameEventFunc( EVENT_M_SHUTDOWNTIME,					&UIFloatChannelList::ReceiveGameEvent_ShutDownTime);
	RegisterGameEventFunc( EVENT_M_DESTROYED_NETWORK,				&UIFloatChannelList::ReceiveGameEvent_DestroyedNetwork);
}

/*virtual*/ UIFloatChannelList::~UIFloatChannelList()
{
#if !defined(USE_LOCALE_UI)
	if (m_pVerifiSprite)
	{
		g_pFramework->getGuiLayer()->RemoveChild(m_pVerifiSprite);
		I3_SAFE_RELEASE(m_pVerifiSprite);
	}

	if( m_pVerifiTexture)
	{
		I3_SAFE_RELEASE(m_pVerifiTexture);
	}
#endif
	
	if( g_pSndMng != nullptr)
	{
		g_pSndMng->StopBGM();
	}
}

#ifdef max
#undef max
#endif

REAL32 UIFloatChannelList::__CalcChannelGauge(INT32 UserCount , INT32 MaxUserCount)
{
	// ������ �������ų� ������ �߸��Ǿ� ������ �ΰ���� 0���� �ɴϴ�.
	//if (UserCount == 0 && MaxUserCount == 0)
	//	return 0;

	I3ASSERT(UserCount >= 0 && MaxUserCount > 0);

	//if ( MaxUserCount <= 0 )
	//	MaxUserCount = 1;

	// hansoft 9288. MaxUserCount�� �������� ��� MaxUserCount�� 1�� �θ� ä�� �������� ���������� ���� ����.
	if (MaxUserCount <= 0)
		MaxUserCount = std::numeric_limits<INT32>::max();

	if (UserCount < 0)
		UserCount = 0;

	REAL32	rUserPercent = static_cast<REAL32>(UserCount) / static_cast<REAL32>(MaxUserCount);

	if(rUserPercent >= 1.0f)
		rUserPercent = 1.0f;

	return rUserPercent;
}


void UIFloatChannelList::_UpdateChannelLabel(void)
{
	INT32 i;

	LuaState * L = _CallLuaFunction( "ClearList");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, GAME_CHANNEL_COUNT);
		_EndLuaFunction( L, 1);
	}

	INT32 ServerIndex = GetServerPhase()->GetTargetServerIdx();
	for( i = 0 ; i < GAME_CHANNEL_COUNT ; i++)
	{
		ChannelType channelType = ChannelContext::i()->GetChannelType(ServerIndex, i);
		_SetServerEnv( ServerIndex, i, channelType);
	}

	m_nServerChannelIdx = 0;
}

void UIFloatChannelList::_SetServerEnv( INT32 nServer, INT32 nChannel, INT32 nChannelState)
{
	i3::custom_stack_wstring<512>::type wstrBuf;

	I3ASSERT( nChannel < MAX_CHANNEL_CTRL);

	// ���� ������
	UINT32	userCount		= ChannelContext::i()->GetChannelUserCount(m_nServerChannelIdx);
	UINT32	MaxUserCount	= ChannelContext::i()->GetChannelMaxUser();
	REAL32	rUserGauge		= __CalcChannelGauge(userCount, MaxUserCount);

	//SS_MAINTYPE mainType = SS_MAINTYPE_BEGINNER;
	ChannelType mainType = CHANNEL_TYPE_NONE;
	SS_SUBTYPE subType = SS_SUBTYPE_CATEGORY_SLOT_TEXT;

	// ���� �̸�
	if( rUserGauge >= 1.0f)		//	���� Ǯ ����
		subType = SS_SUBTYPE_CATEGORY_SLOT_FULL_TEXT;

	I3_BOUNDCHK( nChannelState, CHANNEL_TYPE_MAX);
	mainType = (ChannelType)nChannelState;
	
	i3::rc_wstring wstrchannelString;
	i3::rc_wstring wstrCategoryText;

	ChannelContext::i()->GetServerChannelText(wstrchannelString, mainType, subType);
	ChannelContext::i()->GetServerChannelText(wstrCategoryText, mainType, SS_SUBTYPE_CATEGORY_TEXT);
	//wstrBuf = m_aawstrServerText[mainType][subType];

	// �������� ���δ�.
	if(wstrCategoryText.empty() == false)
	{
		i3::generic_string_cat( wstrchannelString, L"\n");		
		i3::generic_string_cat( wstrchannelString, wstrCategoryText);
	}

	COLOR col;

	if( rUserGauge >= 1.0f)
	{	
		i3Color::Set( &col, (UINT8) 204, 0, 0, 255);
	}
	else 
	{
		// ���� ���õ� �����̶�� Text Color ��� 
		if(m_nLastedSelectSlot == m_nServerChannelIdx)
			i3Color::Set( &col, (UINT8)255, 255, 255, 255);
		else
			i3Color::Set( &col, (UINT8)146, 172, 194, 255);
	}

	SetChannelSlot( nChannel, wstrchannelString, -1, rUserGauge, &col);

	// ���� �ε��� ����.
	m_nServerChannelIdx++;
	m_nServerCount[ mainType]++;
}


bool UIFloatChannelList::_CheckEnterChannel( INT32 channel)
{
	// ������ ��ã���� ���� �Ұ�
	INT32 userCount = ChannelContext::i()->GetChannelUserCount( channel);

	if (ChannelContext::i()->GetChannelMaxUser() <= userCount)
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_USER_FULL_C"));	// _C �̳�, �ݹ� ������� �ʴ´�.
		return false;
	}

	// ������ �� �ִ��� �˻��Ѵ�.
	INT32 nCurServerIdx = GetServerPhase()->GetTargetServerIdx();
	ChannelType channelType = ChannelContext::i()->GetChannelType(nCurServerIdx, channel);

	if( ChannelContext::i()->IsValidChannelType(channelType) == false)
	{ 
		// ������ �� ������ �����޽����� �ѷ��ش�.
		PrintMessageForChannelEnterError(channelType);
		return false;	
	}
	
	if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) )
	{
		//MOD,GM�̸� ������ ����
		if (UserInfoContext::i()->CheckAuthLevel())
			return true;

		ClanGameContext::i()->setMyMercenaryState(false);

		bool is_clan_member = (CLAN_MEMBER_LEVEL_UNKNOWN != UserInfoContext::i()->GetMyClanState());
		bool is_clan_channel = (channelType == CHANNEL_TYPE_CLAN || channelType == CHANNEL_TYPE_PCCAFE_CLAN);
		m_isWaitMercenary = false; // �ʱ�ȭ

		// �뺴�ý��� - Ŭ�� �̰����ڿ� ���� ó�� 140807 gyungju.kim
		if( is_clan_member == false && is_clan_channel == true )
		{
			// K/D rate�� 50% �����̸� ���� �Ұ� 
			UINT32  Killcount	= UserInfoContext::i()->GetMyUserInfoRecord()->_killcount;
			UINT32 Deathcount	= UserInfoContext::i()->GetMyUserInfoRecord()->_death;
			if( GameContextUtil::getKillRate(Killcount,Deathcount) < 50 || (Killcount + Deathcount) == 0 )
			{
				GameUI::MsgBox(MSG_TYPE_ERROR, GAME_STRING("STR_MERC_NONCLAN_JOIN_POPUP_FAIL"));
				return false;
			}

			m_isWaitMercenary = true;
			// �ڵ� �뺴 ��� ����. ������ ��� Ŭ�� ä�� ����.
			GameUI::MsgBox(MSG_TYPE_SYSTEM_QUERY, GAME_STRING("STR_MERC_NONCLAN_JOIN_POPUP"), nullptr, MAKE_CALLBACK(cbMercenaryRegist), this);
			return false;
		}
	}

	if(channelType == CHANNEL_TYPE_CLAN)
		ClanGameContext::i()->setClanChannel(true);
	else
		ClanGameContext::i()->setClanChannel(false);

	ClanGameContext::i()->setForceLeaveChannel(false); // init
	// ����
	return true;
}

void UIFloatChannelList::cbMercenaryRegist(void *pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		// ClanGameContext �� �뺴 ���� �����ϴ� ���� ä������ �����ϰ��� �ؾ���
		// ���� flag���� �����մϴ�.
		((UIFloatChannelList*)pThis)->m_bMercenaryRegist = true;
		((UIFloatChannelList*)pThis)->TryEnterChannel(true);
	}
}

#if !defined(USE_LOCALE_UI)
void UIFloatChannelList::_CreateVerifiMark( void)
{
	// ���� ��ũ
	switch( GetCodePage())
	{
	case I3_LANG_CODE_KOREAN:		//	�ѱ�
		{
#if defined (MULTYLANGUAGE)
			i3::stack_string strLoadFilePath;
			i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Main/grade.i3i", GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
			m_pVerifiTexture = g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
#else
			m_pVerifiTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Main/grade.i3i");
#endif
			I3ASSERT( m_pVerifiTexture != nullptr);

			m_pVerifiSprite = i3Sprite2D::new_object();
			m_pVerifiSprite->Create(2, true, false);
			m_pVerifiSprite->SetTexture(m_pVerifiTexture);

			if (UserInfoContext::i()->IsAdultUser())
			{
				m_pVerifiSprite->SetTextureCoord(0, 120.0f, 1.0f, 234.0f, 137.0f);
			}
			else
			{
				m_pVerifiSprite->SetTextureCoord(0, 1.0f, 1.0f, 115.0f, 137.0f);
			}

			m_pVerifiSprite->SetTextureCoord(1, 238.0f, 1.0f, 331.0f, 137.0f);

			i3::pack::POINT2D pt = g_pFramework->GetSreenSize();
			m_pVerifiSprite->SetRect(0, pt.x - 120.0f, 5.0f, 115.0f, 137.0f);
			m_pVerifiSprite->SetRect(1, 5.0f, pt.y - 167.0f, 94.0f, 137.0f);
			m_pVerifiSprite->SetEnable(false);

			g_pFramework->getGuiLayer()->AddChild(m_pVerifiSprite);
		}
	}
}
#endif


/*virtual*/ void UIFloatChannelList::OnCreate(i3GameNode *pParent)
{
	I3PRINTLOG(I3LOG_NOTICE,"UIFloatchannelList::OnCreate() Enter");
	
	UIFloatingBase::OnCreate(pParent);
	AddScene("Scene/Main/PointBlankRe_Channel_List.i3UIs", UIFloatChannelList_Glue);

	//g_pFramework->LoadNoticeFromFile();

#if !defined(USE_LOCALE_UI)
	_CreateVerifiMark();
#endif

}

/*virtual*/ void UIFloatChannelList::_InitializeAtLoad(i3UIScene *pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);

	m_pFrameWnd = (i3UIFrameWnd*) pScene->FindChildByName( "FrameWnd_CL");
	I3ASSERT( m_pFrameWnd != nullptr);

	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::R_TO_L);
}

/*virtual*/ bool UIFloatChannelList::OnEntranceStart(void *pArg1/* = 0*/, void *pArg2/* = 0*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	INT32 i;

	m_bInviteMoving = false;
	m_bEnterLobby = false;
	m_bWaitConnect = false;

	for( i = 0; i < SS_MAINTYPE_MAX; i++)
		m_nServerCount[i] = 0;

	SetInputDisable( false);

	_UpdateChannelLabel();

	DEV_LOG("CPhaseChannelList::OnEntranceStart");

	return true;
}

/*virtual*/ void UIFloatChannelList::OnEntranceEnd(void)
{
	UIFloatingBase::OnEntranceEnd();

	i3UI::setDisableUIInput(false);
	SetInputDisable(false);

	UIServerAndChannelBanner * pBanner = static_cast<UIServerAndChannelBanner*>(GameUI::GetFloating(UFW_SCBANNER));
	if (pBanner != nullptr)
	{
		pBanner->ChannelListAfterProcess();
	}
	
	DEV_LOG("UIFloatChannelList::OnEntranceEnd");
}

/*virtual*/ bool UIFloatChannelList::OnExitStart( void)
{
	if(UIFloatingBase::OnExitStart() == false)
		return false;

	m_nLastedSelectSlot = -1;

	if(m_bEnterLobby == false)
	{
		UIPhaseSelectServer * pPhase = GetServerPhase();
		if(pPhase != nullptr)
			pPhase->WaitForCloseChannelList(true);
	}

	DEV_LOG("UIFloatChannelList::OnExitStart");

	return true;
}

/*virtual*/ void UIFloatChannelList::OnExitEnd(void)
{
	UIFloatingBase::OnExitEnd();

	//// �ʴ� �޼��� �ʱ�ȭ
	InviteContext::i()->ClearInviteInfo();

	SetInputDisable(false);

	if(m_bEnterLobby == false)
	{
		UIPhaseSelectServer * pPhase = GetServerPhase();
		if(pPhase != nullptr)
			pPhase->WaitForCloseChannelList(false);
	}

	DEV_LOG("UIFloatChannelList::OnExitEnd");
}


/*virtual*/ void UIFloatChannelList::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate( rDeltaSeconds);

	m_fRefreshClickElapseTime -= rDeltaSeconds;
	m_fConnectClickElapseTime -= rDeltaSeconds;

	if( CCommunityContext::i()->IsMovingToUser())
	{
		SelectChannel( CCommunityContext::i()->GetMoveTargetChannel());
		TryEnterChannel();
	}
}

void UIFloatChannelList::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIFloatingBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}


void UIFloatChannelList::_EhFailConnect(void)
{
	if( m_LogInConnectFailedCount++ < g_pConfigEx->GetNetwork().ServerReconnectCount )
	{
	}
	else
	{
		_CloseWaitMessageBox();

		// ���� ���Ӽ����� �������ϸ� ���Ḧ �ϴ� �ڵ忴����
		// ����Ʈ���� ������ �ٰ��մϴ�.
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_FAIL_REFRESH_C"));

		GetMainFrame()->MovePhase( UIPHASE_SERVER_CHANNEL_LIST);
	}
	//m_bMyPhase = false;
}

void UIFloatChannelList::Refresh( void)
{
	// ���� ������ ù��° �迭�� Ʈ���������� Ŭ���̾�Ʈ������ ī������ -1 ���ֱ� ������
	// ���� �ε����� Ŭ���̾�Ʈ�� 1�� �����Ƿ� ����ȭ�� ���� �����ε����� + 1 ���ְ� ��û�Ѵ�.
	INT32 i32ServerIdx = GetServerPhase()->GetTargetServerIdx() + 1;
	I3TRACE("UIFloatChannelList::Refresh() - EVENT_GET_CHANNEL_LIST\n");
	GetServerPhase()->SendEventForChannel(EVENT_GET_CHANNEL_LIST, &i32ServerIdx);
}

void UIFloatChannelList::RefreshByTime( void)
{
	if( m_fRefreshClickElapseTime <= 0.0f)
	{
		Refresh();
		m_fRefreshClickElapseTime = REFRESH_LIMIT_TIME;
	}
}

void UIFloatChannelList::SelectChannel( INT32 channel)
{
	if( channel == INVALID_INDEX )
	{
		return;
	}


	COLOR col;

	i3Color::Set( &col, (UINT8)146, 172, 194, 255);
	SetCellTextColor(m_nLastedSelectSlot, &col);

	i3Color::Set( &col, (UINT8)255, 255, 255, 255);
	SetCellTextColor(channel, &col );

	m_nLastedSelectSlot = channel;

	// ä�� ���ÿ� ���� ��ư ���¸� ������Ʈ�Ѵ�. 
	//_SetButtonState();
}

void UIFloatChannelList::TryEnterChannel( bool OnlyConnect)
{
	if( m_bWaitConnect || m_bEnterLobby )
		return;
	
	if(!OnlyConnect)
	{
		if ( MINIMUM_DELTA_TIME < m_fConnectClickElapseTime)							// 0.0f �� ������ OnEntranceEnd ȣ��� OnUpdate �� �̿��� Deltatime ��꿡 ��߳��ºκ� ����
			return;

		if( m_nLastedSelectSlot == -1)
			return;

		if( _CheckEnterChannel(m_nLastedSelectSlot) == false)
		{
			// ���� �Ұ�
			CCommunityContext::i()->CompleteMovingToUser();
			if (m_isWaitMercenary == false)
				PlaySound( UISND_WARNING);
			return;
		}
	}

	m_bWaitConnect = true;

	GetServerPhase()->TryEnterServerChannel(m_nLastedSelectSlot);

	SetInputDisable( true);

	m_fConnectClickElapseTime = CONNECT_LIMIT_TIME;
	m_fRefreshClickElapseTime = REFRESH_LIMIT_TIME;

	PlaySound( UISND_SERVER_ENTER);
}

void UIFloatChannelList::SetChannelSlot( INT32 slot, const i3::wliteral_range& wNameRng,
										INT32 event, REAL32 rGauge, COLOR * pColor, const i3::wliteral_range& wToolTipRng)
{
	ChangeChannelUserGauge(slot, rGauge);

	LuaState * L = _CallLuaFunction( "SetSlotInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);
		i3Lua::PushStringUTF16To8( L, wNameRng);
		i3Lua::PushInteger( L, event);
		i3Lua::PushNumber(L , rGauge);
		i3Lua::PushColor( L, pColor);
		i3Lua::PushStringUTF16To8(L, wToolTipRng);

		_EndLuaFunction( L, 9);
	}
}

void UIFloatChannelList::ChangeChannelUserGauge(INT32 nSlot, REAL32 rGauge)
{
	LuaState * L = _CallLuaFunction("GetChannelUserGauge");

	if( L != nullptr)
	{
		i3Lua::PushInteger(L , nSlot);

		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L , 1));

		if( pCtrl != nullptr)
		{
			// 30% �̻� �Ѿ�� �Ǹ� ����� 100%�� ���������� ǥ�� �˴ϴ�.
			if(rGauge >= 1.0f)
				pCtrl->SetTemplateByName("4T_Progress05_channel");
			else if(rGauge > 0.3f)
				pCtrl->SetTemplateByName("4T_Progress06_channel");
			else
				pCtrl->SetTemplateByName("4T_Progress04_channel");
		}
	}
}

void UIFloatChannelList::SetCellTextColor(INT32 nSlot, COLOR * pColor)
{
	if(nSlot == -1)
		return;

	UINT32	userCount		= ChannelContext::i()->GetChannelUserCount(nSlot);
	UINT32	MaxUserCount	= ChannelContext::i()->GetChannelMaxUser();
	REAL32	rUserGauge		= __CalcChannelGauge(userCount, MaxUserCount);

	if(rUserGauge >= 1.0f)
		return;

	LuaState * L = _CallLuaFunction("SetCellTextColor");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L , nSlot);
		i3Lua::PushColor(L , pColor);
		_EndLuaFunction(L , 5);		
	}
}


void UIFloatChannelList::_EhStageError(INT32 Event,INT32 Arg)
{
	//m_bMyPhase = true;
	GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
	//m_bMyPhase = false;

	_DefaultErrorPopup( GAME_RCSTRING("STR_TBL_GUI_SERVER_DEBUG_POPUP2"), Event, Arg);
}

void UIFloatChannelList::PrintMessageForChannelEnterError( ChannelType channelType )
{
	i3::rc_wstring wstrMsg;

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

			I3ASSERT(pCondiInfo != nullptr);
			I3ASSERT(pCondiInfo->condition == ConfigLogic::ChannelConditionInfo::KILLDEATH);

			i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_GUI_SERVER_NOTICE_CONDITION_KILLDEATH"), pCondiInfo->value_min );
		}
		break;
	case CHANNEL_TYPE_CLAN:
	case CHANNEL_TYPE_PCCAFE_CLAN :
		{
			//Ŭ������ �ƴϸ� ������ �� �����ϴ�.
			wstrMsg = GAME_RCSTRING("STR_TBL_GUI_SERVER_NOTICE_LIMIT_JOIN_BY_CLANMEMBER");
		}
		break;

	case CHANNEL_TYPE_BEGIN2:	
	case CHANNEL_TYPE_EXPERT:
	case CHANNEL_TYPE_EXPERT2:
	case CHANNEL_TYPE_MIDDLE:
		{
			const ConfigLogic::ChannelConditionInfo * pCondiInfo = 
				g_pConfigEx->GetLogic().GetChannelConditionInfo(channelType);

			I3ASSERT(pCondiInfo != nullptr);
			I3ASSERT(pCondiInfo->condition == ConfigLogic::ChannelConditionInfo::RANK);

			i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_GUI_SERVER_NOTICE_CONDITION_RANK"), 
				GetCharaRank (pCondiInfo->value_min ),
				GetCharaRank( pCondiInfo->value_max ) );
		}
		break;

		// revision 40858
	case CHANNEL_TYPE_PCCAFE_SILVER:
		{
			wstrMsg = GAME_RCSTRING("STR_UI_GOTO_GWARNET_CHANNEL_ERROR");
		}
		break;

	case CHANNEL_TYPE_PCCAFE_GOLD:
		{
			wstrMsg = GAME_RCSTRING("STR_UI_GOTO_GWARNET_GOLD_CHANNEL_ERROR");
		}
		break;
	}

	GameUI::MsgBox( MSG_TYPE_ERROR, wstrMsg, nullptr, MAKE_CALLBACK(CbRefleshServerList), this);
}

void UIFloatChannelList::_CloseWaitMessageBox(void)
{
	if( m_pMBConnectWait)
	{
		GameUI::CloseMsgBox( m_pMBConnectWait);
		m_pMBConnectWait = nullptr;
	}

	m_fWaitDelayTime = WAIT_CONNECT_TIME;
	m_bWaitConnect = false;
}


void UIFloatChannelList::CbRefleshServerList(void* pThis,INT32 nParam)
{
	((UIFloatChannelList*)pThis)->Refresh();
}

void UIFloatChannelList::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIFloatChannelList::ReceiveGameEvent_ConnectServer( INT32 arg, const i3::user_data&)
{
	// ����Ʈ���� -> ���Ӽ��� ���ؽÿ��� �߻��ϴ� �̺�Ʈ.
	if( EV_SUCCESSED(arg))
	{
		////m_bMyPhase = true;
		GameEventSender::i()->SetEventOnce(EVENT_USER_ENTER);
	}
	else
	{
		CCommunityContext::i()->CompleteMovingToUser();

		if( EV_IS_ERROR(arg, EVENT_ERROR_FAIL))
			_EhFailConnect();
		else
			_EhStageError( EVENT_CONNECT_SERVER_GAME, arg);
	}
}

void UIFloatChannelList::ReceiveGameEvent_LeaveServer( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg) )
	{
		GameEventSender::i()->SetEvent(EVENT_DISCONNECT);

		if ( CCommunityContext::i()->IsMovingToUser())
		{
			GameEventSender::i()->SetEvent(EVENT_CONNECT_SERVER_GAME);
		}
		else
		{
			GetMainFrame()->MovePhase(UIPHASE_SERVER_CHANNEL_LIST);
		}
	}
	else
	{
		CCommunityContext::i()->CompleteMovingToUser();

		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_FAIL_MOVE"));
	}
}

void UIFloatChannelList::ReceiveGameEvent_Get_ChannelList( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		_UpdateChannelLabel();
	}
	else
	{
		CGameNetworkManager::i()->DisconnectTCP();
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_FAIL_CONNECT_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
	}
}

void UIFloatChannelList::ReceiveGameEvent_UserEnter( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg) )
	{
		GetServerPhase()->SendEventForChannel(EVENT_SELECT_CHANNEL, &m_nLastedSelectSlot);

		// �̻�� ��û���� �������� Ȯ���մϴ�.
		// �ݵ�� ���⼭ ȣ���� �ʿ䰡 ���� ������
		// �������� �����Ѵٸ� ȣ�� ��ġ�� �����Ͽ��� �����մϴ�.
		GetServerPhase()->SendEventForChannel(EVENT_RUN_PARAM_CHECK);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_FAIL_MOVE"));
	}
}

void UIFloatChannelList::ReceiveGameEvent_SelectChannel( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		// �κ� ���� �� ���� idx, ä�� type �α� �����.
		char server_log[128] = {0,};
		StringCchPrintf( server_log, 128, "* ENTER SERVER IDX(%d) CHANNEL TYPE(%d)" ,g_pServerInfo->GetCurGameServerIndex(), ChannelContext::i()->GetConnectedChannelType());
		I3PRINTLOG(I3LOG_DEFALUT,  server_log);

		if(m_bMercenaryRegist)
		{
			ClanGameContext::i()->setMyMercenaryState(true);
			m_bMercenaryRegist = false; // �ʱ�ȭ
		}
	
		GetServerPhase()->SendEventForChannel(EVENT_ENTER_LOBBY);
		
	}
	else
	{
		SetInputDisable( false);

		if (EV_IS_ERROR(arg, EVENT_ERROR_SERVER_USER_FULL))
		{
			m_bWaitConnect = false;
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_USER_FULL_C"), nullptr, MAKE_CALLBACK(CbRefleshServerList), this);
		}
		else if (EV_IS_ERROR(arg, EVENT_ERROR_CHANNEL_ENTER_CONDITION))
		{
			m_bWaitConnect = false;
			ChannelType channeltype = ChannelContext::i()->GetConnectedChannelType();
			PrintMessageForChannelEnterError(channeltype);
		}
		else if (EV_IS_ERROR(arg, EVENT_ERROR_NOT_ENOUGH_KILL_DEATH_WEIGHT))
		{
			m_bWaitConnect = false;
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_SERVER_NOTICE_CONDITION_KILL_RATE") );
		}
		else if(EV_IS_ERROR(arg, EVENT_ERROR_CLAN_WAR_MERCENAY_DIS_TIME))
		{
			m_bWaitConnect = false; 
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_MERC_BATTLE_ESCAPE") );
			ClanGameContext::i()->setMyMercenaryState(false);
		}
		// revision 30642�ε��׽þ� �ű� ���� ���� ����
		/*else if (EV_IS_ERROR(arg, EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME))
		{
			g_pServerInfo->SetCurChannelNumber(INVALID_INDEX);
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_NOT_SATISFY_MTS"), nullptr, MAKE_CALLBACK(CbRefleshServerList), this);
		}
		else if(EV_IS_ERROR(arg, EVENT_ERROR_CHANNEL_ENTER_CONDITION_GWARNET))	// revision 40921
		{
			g_pServerInfo->SetCurChannelNumber(INVALID_INDEX);
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_UI_GOTO_GWARNET_CHANNEL_ERROR"), nullptr, MAKE_CALLBACK(CbRefleshServerList), this);
		}
		*/
		else
		{
			_EhStageError( EVENT_SELECT_CHANNEL, arg);
		}
	}
}

void UIFloatChannelList::ReceiveGameEvent_EnterLobby( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		_CloseWaitMessageBox();
		m_bEnterLobby = true;
		m_slide.ClearFrameWndList();

		// ó�� �г��� �����.
		// ó�� �α���.
		if( false == CCharaInfoContext::i()->HasBasicChara() || UserInfoContext::i()->HaveNickName() == false)
		{
			if(g_pEnvSet->m_nUIVersion == UFV_1_5 )
				GetMainFrame()->MovePhase( UIPHASE_CREATE_CHARA);
			else
			{
				// V1.0/1.1�� �г��� ���� ���� ĳ���� �����մϴ�.
				g_pFramework->GetUIMainframe()->MovePhase(UIPHASE_LOBBY);
				ClanGameContext::i()->setClanBattleLobbyEnable(false);
			}
		}
		else
		{// revision 59499
			if( ChannelContext::i()->GetBattleChannel() )
			{
				// Ŭ�� ������� ����� Ȱ��ȭ�� ��� Ŭ���κ�� �̵��մϴ�.- Ŭ�� ��ġ����ŷ
				if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) &&
					ClanGameContext::i()->getMyMercenaryState() == false)
				{
					GetMainFrame()->MovePhase( UIPHASE_CLANLOBBY);
					ClanGameContext::i()->setClanBattleLobbyEnable(true);
					ClanGameContext::i()->resetMyMatchSquadInfo();
				}
				else // Ŭ���κ� �ƴ� �Ϲݷκ�� �ٷ� �̵��մϴ�.
				{
					g_pFramework->GetUIMainframe()->MovePhase(UIPHASE_LOBBY);
					ClanGameContext::i()->setClanBattleLobbyEnable(false);
				}
			}
			else
			{
				g_pFramework->GetUIMainframe()->MovePhase(UIPHASE_LOBBY);
				ClanGameContext::i()->setClanBattleLobbyEnable(false);
			}
				
			m_LastChannelNumber = g_pServerInfo->GetCurChannelNumber();
		}
	}
	else
	{
		CCommunityContext::i()->CompleteMovingToUser();

		_CloseWaitMessageBox();
		_EhStageError( EVENT_ENTER_LOBBY, arg);

		SetInputDisable( false);
	}
}

void UIFloatChannelList::ReceiveGameEvent_DestroyedNetwork( INT32, const i3::user_data&)
{
	// ��Ʈ��ũ�� ��������
	CCommunityContext::i()->CompleteMovingToUser();
	DEV_LOG(" CPhaseChannelList::OnGameEvent EVENT_M_DESTROYED_NETWORK");
}

void UIFloatChannelList::ReceiveGameEvent_CloseWaitMessageBox( INT32, const i3::user_data&)
{
	_CloseWaitMessageBox();
}

void UIFloatChannelList::ReceiveGameEvent_ShutDownTime( INT32, const i3::user_data&)
{
	GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STR_SHUTDOWN_POPUP_ACCESSPROHIBITION"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

