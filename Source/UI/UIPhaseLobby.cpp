#include "pch.h"
#include "UIPhaseLobby.h"

#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UIPopupNickNameSetting.h"
#include "UIPopupQueryPassword.h"
#include "UIFloatChat.h"
#include "UIFloatMiniProfile.h"
#include "UIFloatUserList.h"
#include "UIFloatPlayTimeAlarm.h"
#include "UINewAndHotBanner.h"
#include "UIToolTipLobbyMapInfo.h"
#include "UIPopupAttendanceCheck.h"
#include "UICharaView.h"
#include "UIUtil.h"
#include "../CommunityContext.h"
#include "../GameGUI.h"
#include "../TextSet.h"
#include "IGShopContext.h"
#include "UIPhaseSubBase.h"
#include "MedalContext.h"
#include "Useable_Item_Def.h"

#include "i3Base/itl/algorithm/sort.h"
#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/compare/generic_icompare.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "InviteContext.h"
#include "AttendanceContext.h"
#include "ClanGameContext.h"
#include "AbuseContext.h"
#include "TwitchTVContext.h"

#include "../StageBattle/AIModeContext.h"
#include "../StageBattle/UserContext.h"
#include "UIShopFunction.h"
#include "UIItemInfoUtil.h"

#if legacy_gui_function
	#include "../GuiNotifyReceiver.h"
#endif

#include "GameLoginContext.h"
#include "KickContext.h"
#include "ChannelContext.h"
#include "UserInfoContext.h"
#include "LobbyContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "TutorialContext.h"
#include "../QuickJoinContext.h"

#include "ClientStageInfoUtil.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPhaseLobby_ClickButton( LuaState * L)
	{
		INT32 btn = i3Lua::GetIntegerArg( L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->ClickButton( btn);
		return 0;
	}

	int UIPhaseLobby_JoinRoom( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));

		if( pThis != nullptr)
		{
			// ��ũ�� ��ǥ ����
			idx += MAX(0, pThis->GetRoomListScrollPos() );
			pThis->EnterRoom( idx);
		}

		return 0;
	}

// TODO : ������ �ʴµ� �Ͽ� �ּ� ó�� �մϴ�...  ���߿� ���뼺�� ���ٸ� �׶� ���� (2014.09.19.����)
/*
	int UIPhaseLobby_CreateNickname( LuaState * L)
	{
		char strBuf[MAX_STRING_COUNT];

		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));

		if( pThis != nullptr)
		{
			if( i3Lua::GetStringArg( L, 0, strBuf, MAX_STRING_COUNT) )
			{	
				i3::rc_wstring	wstrNick;
				i3::mb_to_utf16(strBuf, wstrNick);		// MB���� UTF8���� Ȯ���غ��� �Ѵ�. (2014.07.18.����)

				// NickName
				if (false == IsValidNicknameLetterSize(wstrNick))
				{
					// ! praptor - �ּ� ���ڼ� ���Եǵ��� ���� �����ؾ��մϴ�.
					GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_LENGTH"), nullptr, MAKE_CALLBACK(UIPhaseLobby::CbSameNick), pThis);
					return 0;
				}

				if ( ! g_pFramework->CheckValidNickname(wstrNick) )
				{
					GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_WRONG_NICK"), nullptr, MAKE_CALLBACK(UIPhaseLobby::CbSameNick), pThis);
					return 0;
				}

				GameLoginContext::i()->setUserNick(wstrNick);
				GameEventSender::i()->SetEvent(EVENT_CREATE_NICK_NAME);
			}
			else
			{
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_EMPTY"), nullptr, MAKE_CALLBACK(UIPhaseLobby::CbSameNick), pThis);
			}
		}

		return 0;
	}
*/

	int UIPhaseLobby_SelectRoom( LuaState * L)
	{
		INT32 selectIdx = i3Lua::GetIntegerArg( L, 1);

		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
		{
			// ��ũ�� ��ǥ ����
			selectIdx += MAX(0, pThis->GetRoomListScrollPos() );
			pThis->SelectRoom( selectIdx);
		}
		return 0;
	}

	int UIPhaseLobby_SortRoomList( LuaState * L)
	{
		INT32 nSortType = i3Lua::GetIntegerArg(L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->SortRoomList(nSortType);
		return 0;
	}

	int UIPhaseLobby_ViewAdmissionRoom( LuaState * L)
	{
		bool bViewAdmissionRoom = i3Lua::GetBooleanArg(L, 1) == true ? true : false;
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->SortAdmissionRoom(bViewAdmissionRoom);
		return 0;
	}
	int Clan_ClanLobby( LuaState* L)
	{
		g_pFramework->GetUIMainframe()->MovePhase( UIPHASE_CLANLOBBY);
		ClanGameContext::i()->setClanBattleLobbyEnable(true);
		return 0;
	}
	int UIPhaseLobby_CouponInput(LuaState* L)
	{
		g_pFramework->GetUIMainframe()->TogglePopup(UPW_COUPON_INPUT);
		return 0;
	}

	int UIPhaseLobby_ScrollRoomList(LuaState * L)
	{
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->OnScrollRoomList();

		return 0;
	}

	int UIPhaseLobby_WheelRoomList(LuaState * L)
	{
		INT32 zDelta = i3Lua::GetIntegerArg( L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->OnWheelRoomList(zDelta);

		return 0;
	}

	int UIPhaseLobby_AttendanceCheck(LuaState * L)
	{
		INT32 EvtType = i3Lua::GetIntegerArg(L, 1);
		UIPhaseLobby * pThis = static_cast<UIPhaseLobby*>(GameUI::GetPhase(UIPHASE_LOBBY));
		if( pThis != nullptr)
			pThis->AttendanceOpenPopup(EvtType);

		return 0;
	}
}

LuaFuncReg	PhaseLobby_Glue[] =
{
	{"ClickButton",				UIPhaseLobby_ClickButton	},
	{"ClickJoinRoom",			UIPhaseLobby_JoinRoom		},
	{"SelectRoom",				UIPhaseLobby_SelectRoom		},
	{"SortRoomList",			UIPhaseLobby_SortRoomList	},
	{"ViewAdmissionRoom",		UIPhaseLobby_ViewAdmissionRoom},
	{"ClanLobby",				Clan_ClanLobby },
	{"CouponInput",				UIPhaseLobby_CouponInput	},
	{"ScrollRoomList",			UIPhaseLobby_ScrollRoomList },
	{"WheelRoomList",			UIPhaseLobby_WheelRoomList },
	{"AttendanceClick",			UIPhaseLobby_AttendanceCheck },
	{ nullptr,		nullptr}
};

//------------------------------------------------------------------------------//
//								Callback rutine									//
//------------------------------------------------------------------------------//
static void JoinRoomPasswordCallback( bool OK, const i3::string & Password, void * pUserContext)
{
	if ( OK )
	{
		I3ASSERT(Password.size() <= sizeof(char) * NET_ROOM_PW);

		if( Password.size() > 0)
		{
			std::size_t cnt = sizeof(char) * NET_ROOM_PW;
			if ( Password.size() < sizeof(char) * NET_ROOM_PW)
				cnt = Password.size();

			MyRoomInfoContext::i()->SetMyRoomPW( Password.c_str(), cnt );
		}
		else
		{
			MyRoomInfoContext::i()->ClearMyRoomPW();
		}

		UIPhaseLobby *	pLobby = (UIPhaseLobby *)pUserContext;

		// �����ÿ��� Input�� �����ϴ�.
		if( MyRoomInfoContext::i()->JoinRoom(&pLobby->GetJoinTargetRoom()) == true)
			i3UI::setDisableUIInput(true);
	}
}

//------------------------------------------------------------------------------//

bool UIPhaseLobby::sm_bAwayInput = false;
SORT_ORDER_INFO UIPhaseLobby::sm_SortOrder[MAX_SORT_TYPE] = 
{
	{ROOM_SORT_NUMBER,		false}, 
	{ROOM_SORT_ROOMNAME,	false}, 
	{ROOM_SORT_MODE,		false}, 
	{ROOM_SORT_MAP,			false}, 
	{ROOM_SORT_PERSON,		false},
	{ROOM_SORT_STATE,		false}
};
INT32 UIPhaseLobby::sm_nCurSortType = 0;

bool UIPhaseLobby::m_bPcCafeNotice = false;				// PcCafe ���� â�� �����ִ����� ���� ����
bool UIPhaseLobby::m_bEventCardSetNotice = false;
bool UIPhaseLobby::m_bEvnetNotice = false;


I3_CLASS_INSTANCE( UIPhaseLobby);//, UIPhaseBase);

UIPhaseLobby::UIPhaseLobby() : m_bNickNameCreate(false), m_bAllowRefresh(false), m_bRefreshButtonActive(false)
	, m_rEnterLobbyWaitTime(0.f), m_pKickedMassageBox(nullptr), m_pWaitRecevingRoomList(nullptr)
	, m_bCheckInvite(false), m_bEnableNicknameMsgBox(false), m_bEnableCreateRoomMsgBox(false)
	, m_SelectedElement(0), m_fRefreshElapsedTime(0.f), m_bFirstLogin(false), m_bLogin(false), m_bRequestRoomInfoAdd(false)
	, m_LastRefreshTime(0.f), m_pAbusingBox(nullptr), m_bWaitingFromServer(false), m_bViewAdmissionRoom(false)
	, m_pTTMapInfo(nullptr), m_bTutorialEnable(false)
{
	
	ResetInviteInfo(m_InviteInfo);

	i3mem::FillZero( &m_JoinTargetRoom, sizeof( m_JoinTargetRoom));

	RegisterGameEventFunc( EVENT_READY_BATTLE,				&UIPhaseLobby::ReceiveGameEvent_Ready_Battle);
	RegisterGameEventFunc( EVENT_JOIN_QUICK,				&UIPhaseLobby::ReceiveGameEvent_Join_Quick);
	RegisterGameEventFunc( EVENT_CREATE_ROOM,				&UIPhaseLobby::ReceiveGameEvent_Create_Room);
	RegisterGameEventFunc( EVENT_JOIN_ROOM,					&UIPhaseLobby::ReceiveGameEvent_Join_Room);
	RegisterGameEventFunc( EVENT_LEAVE_LOBBY,				&UIPhaseLobby::ReceiveGameEvent_Leave_Lobby);
	RegisterGameEventFunc( EVENT_GET_ROOM_ADDINFO_N,		&UIPhaseLobby::ReceiveGameEvent_Get_RoomAddInfo);
	RegisterGameEventFunc( EVENT_M_DESTROYED_NETWORK,		&UIPhaseLobby::ReceiveGameEvent_DestroyedNetwork);
	RegisterGameEventFunc( EVENT_UPDATE_USER_LIST,			&UIPhaseLobby::ReceiveGameEvent_Update_UserList);
	RegisterGameEventFunc( EVENT_READYBOX_MESSAGE,			&UIPhaseLobby::ReceiveGameEvent_Ready_Box_Message);
	RegisterGameEventFunc( EVENT_ATTENDANCE_CLEAR,			&UIPhaseLobby::ReceiveGameEvent_Attendance_Clear);
	RegisterGameEventFunc( EVENT_PLAYTIME_ALARM,			&UIPhaseLobby::ReceiveGameEvent_PlayTimeAlarm);
	RegisterGameEventFunc( EVENT_SHUTDOWNTIME_ALARM,		&UIPhaseLobby::ReceiveGameEvent_ShutDownTimeAlarm);

	m_bEnableUsingClanChannel = true;
}

UIPhaseLobby::~UIPhaseLobby()
{
	i3::cu::for_each_SafeFree(m_RoomList);

	m_pTTMapInfo->Destroy();
}

void UIPhaseLobby::__AutoRefreshRoomList( REAL32 rDeltaSeconds)
{
	// ���� ������ STEP_nullptr�� �ƴϸ� �渮��Ʈ�� ���� �ִ»��°� �ƴϹǷ�
	// �渮��Ʈ�� ��û���� �ʵ��� �Ѵ�.
	// ����, ���������� �� ��Ȯ�� �ȵǾ� �־ �̷������� �ڵ��� �� �ۿ� ����.
	if( GetMainFrame()->GetCurrentSubPhaseType() != USP_NONE)
		return;

	m_fRefreshElapsedTime -= rDeltaSeconds;

	if( m_fRefreshElapsedTime <= 0.0f)
	{		
#if defined(I3_DEBUG) && 0
		{
			static UINT32 a = 0;
			I3TRACE1("[%d]Refresh Room List....\n", a++);
		}
#endif // _DEBUG

		GameEventSender::i()->SetEvent(EVENT_LOBBY_REFRESH_N);
		m_fRefreshElapsedTime = g_pConfigEx->GetNetwork().RefreshInterval_RoomList;
		m_bRefreshButtonActive = false;
	}
}

void UIPhaseLobby::__ProcessAbusing( REAL32 rDeltaSeconds)
{
	if( m_pAbusingBox != nullptr)
	{
		m_rAbuseTime += rDeltaSeconds;

		if( (UINT32)m_rAbuseTime > AbuseContext::i()->GetAbuseTime())
		{
			GameUI::CloseMsgBox( m_pAbusingBox);
			m_pAbusingBox = nullptr;
		}
	}
}

void UIPhaseLobby::_SetButtonState( void)
{
	LuaState * L = _CallLuaFunction("SetButtonState");
	
	if( L != nullptr)
	{	
		static const bool bEnableTutorial = true;
		i3Lua::PushBoolean(L , bEnableTutorial);
		i3Lua::PushBoolean(L, m_bViewAdmissionRoom);
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L , 3);
	}
}

void UIPhaseLobby::_SetCurSelectItem( void)
{
	if(m_SelectedElement < 0)
		return;

	INT32 i32Selidx = m_SelectedElement - MAX(0, GetRoomListScrollPos() );

	LuaState * L = _CallLuaFunction("SetCurSelectItem");

	if( L != nullptr)
	{	
		i3Lua::PushInteger(L, i32Selidx);
		_EndLuaFunction(L, 1);		
	}
}

void UIPhaseLobby::_InitRoomList(void)
{
	LuaState * L = _CallLuaFunction("InitRoomList");
	if(L != nullptr)
		_EndLuaFunction(L , 0);

	// Init Room List Scroll
	{
		m_pScrollRoomList->setTickSize(1);
		m_pScrollRoomList->setPageSize(UI_LOBBY_ROOMLIST_VIEWCOUNT);
		m_pScrollRoomList->setScrollPos(0);
		m_pScrollRoomList->setScrollRange(0, m_RoomList.size());
	}
}

void UIPhaseLobby::_SetRoomList( void)
{
	// ��µǴ� ������ �ִٸ�, ��й�, �� ǥ��
	bool bEnterRoom;

	for( size_t idx = 0 ; idx < UI_LOBBY_ROOMLIST_VIEWCOUNT ; ++idx)
	{
		UINT32 u32FindIdx = idx + MAX(0, m_pScrollRoomList->getScrollPos());

		if( u32FindIdx >= m_RoomList.size() )
		{
			_SetEmptyRoomInfo( idx);
			continue;
		}

		LOBBY_ROOM_INFO * pInfo = m_RoomList[ u32FindIdx ];
		if (ROOM_STATE_EMPTY == pInfo->_RoomInfo._State)
		{
			_SetEmptyRoomInfo( idx);
			continue;
		}

		ROOM_INFO_BASIC* pRoomInfo = &pInfo->_RoomInfo;

		bEnterRoom = true;

		if( pRoomInfo->_PersonNow >= pRoomInfo->_PersonPlayerMax)			// ����
			bEnterRoom = false;
		else if( (GET_ROOM_INFO_INTERENTER( pRoomInfo->_InfoFlag) > 0) &&
			(pRoomInfo->_State != ROOM_STATE_READY))						//	���� ����, �÷�����.
			bEnterRoom = false;

		_SetRoomInfo(idx, pRoomInfo->_Idx, pRoomInfo);

		//	���� �� �� ���ο� ���� �� ����Ʈ�� ������ ����
		if( bEnterRoom)
			_RestoreRoomColor( idx);				//GCT_VALID_C
		else
			_SetRoomColor( idx, GameGUI::GetColor( GCT_INVALID_C));
	}

	m_pScrollRoomList->setScrollRange(0, m_RoomList.size());

	INT32 i32NewPos = GetRoomListScrollPos();
	i32NewPos = MINMAX( 0, i32NewPos, (INT32)m_RoomList.size() );
	m_pScrollRoomList->setScrollPos( i32NewPos );
}

void UIPhaseLobby::_BuildRoomListDummy(void)
{
#if defined (TEST_LOBBY_ROOM_LIST)
	INT32 i;

	static bool BuildedDummyRoomList = false;
	static INT32 g_nRoomInfoDummyCount = 100;

	if( BuildedDummyRoomList)		return;

	BuildedDummyRoomList = true;
	
	for( i=0; i<g_nRoomInfoDummyCount; i++)
	{
		LOBBY_ROOM_INFO * pDummy = (LOBBY_ROOM_INFO*) i3MemAlloc( sizeof( LOBBY_ROOM_INFO));

		i3mem::FillZero( pDummy, sizeof( ROOM_INFO_BASIC));

		pDummy->_RoomInfo._Idx = i;
		pDummy->_RoomInfo._State = (UINT8) (i3Math::Randf() * (REAL32)(ROOM_STATE_BATTLE_END + 1));

		// EXPORT_AVALIABLE_CODE_VALUE
		while(pDummy->_RoomInfo._StageID == 0)
		{
			STAGE_ORDINAL_ID ord = (STAGE_ORDINAL_ID)(i3Math::Rand() % (STAGE_ORD_MAX - 1));
			ord = (STAGE_ORDINAL_ID)(ord == 0 ? ord+1 : ord);

			pDummy->_RoomInfo._StageID = getStageId( ord);
		}


		if( (INT32) (i3Math::Randf() * 2.0))		pDummy->_RoomInfo._StageMask |= STAGE_MASK_TYPE_CLANMATCH;
		if( (INT32) (i3Math::Randf() * 2.0))		pDummy->_RoomInfo._StageMask |= STAGE_MASK_TYPE_CHALLENGE;
		if( (INT32) (i3Math::Randf() * 2.0))		pDummy->_RoomInfo._StageMask |= STAGE_MASK_TYPE_HEADHUNTER;
		
		pDummy->_RoomInfo._PersonPlayerMax = (UINT8) (i3Math::Randf() * 16.0f);
		pDummy->_RoomInfo._PersonPlayerMax = MINMAX( 2, pDummy->_RoomInfo._PersonPlayerMax, 16);
		pDummy->_RoomInfo._PersonNow = (UINT8) (i3Math::Randf() * (REAL32) pDummy->_RoomInfo._PersonPlayerMax);
		pDummy->_RoomInfo._PersonNow = MINMAX( 1, pDummy->_RoomInfo._PersonNow, pDummy->_RoomInfo._PersonPlayerMax);
		pDummy->_RoomInfo._Ping = (UINT8) (i3Math::Randf() * 5.f) + 1;
		pDummy->_RoomInfo._Ping = MINMAX( 1, pDummy->_RoomInfo._Ping, 5);
		pDummy->_RoomInfo._WeaponFlag = 
							((UINT8) (i3Math::Randf() * 2.0)) << 5 |
							((UINT8) (i3Math::Randf() * 2.0)) << 4 |
							((UINT8) (0x00000001)) << 3 |
							((UINT8) (0x00000001)) << 2 |
							((UINT8) (i3Math::Randf() * 2.0)) << 1 |
							((UINT8) (i3Math::Randf() * 2.0));

		i3::snprintf( pDummy->_RoomInfo._Title, NET_ROOM_NAME_SIZE, _TT("DUMMY_STAGE_%d"), i);
		m_RoomList.push_back(pDummy);
	}

#endif
}

void UIPhaseLobby::_UpdateRoomList(void)
{
	if( IsReadyToUpdate() == false)return;


#if defined( TEST_LOBBY_ROOM_LIST)
	_BuildRoomListDummy();
#else
	INT32 i;
	// Empty All
	_EmptyRoomList();

	// Find View
	INT32 nRoomCount = LobbyContext::i()->getLobbyRoomCount();

	for( i = 0; i < nRoomCount; i++)
	{
		const ROOM_INFO_BASIC * pRoomInfo = LobbyContext::i()->getRoomInfo(i);
		
		if ( ! pRoomInfo)	
			continue;

		// PersonNow�� 0�̸� ���� ��
		if (0 == pRoomInfo->_PersonNow)
			continue;

		LOBBY_ROOM_INFO* pRoom = nullptr;

		// ����� ������ Ȯ��
		bool bModify = false;
		
		for(size_t j = 0; j < m_RoomList.size(); j++)
		{
			pRoom = m_RoomList[ j];
			
			if (pRoomInfo->_Idx == pRoom->_RoomInfo._Idx)
			{
				if (ROOM_STATE_EMPTY != pRoom->_RoomInfo._State)
				{
					pRoom->_bModify = false;
					bModify = true;
					break;
				}
			}
		}

		if( bModify)
		{	// �̹� �ִٸ� ����� ���� �ݿ�
			// Modify
			if( (pRoom->_RoomInfo._InfoFlag != pRoomInfo->_InfoFlag) ||
				(pRoom->_RoomInfo._PersonNow != pRoomInfo->_PersonNow) ||
				(pRoom->_RoomInfo._PersonPlayerMax != pRoomInfo->_PersonPlayerMax) ||
				(pRoom->_RoomInfo._Ping != pRoomInfo->_Ping) ||
				(pRoom->_RoomInfo._StageID != pRoomInfo->_StageID) ||
				(pRoom->_RoomInfo._State != pRoomInfo->_State) ||
				(i3::generic_is_iequal( pRoom->_RoomInfo._Title, pRoomInfo->_Title) == false) ||
				(pRoom->_RoomInfo._WeaponFlag != pRoomInfo->_WeaponFlag) )
			{
				pRoom->_bModify = true;

				i3mem::Copy( &pRoom->_RoomInfo, pRoomInfo, sizeof(ROOM_INFO_BASIC));
			}
		}
		else
		{
			// AddTail
			LOBBY_ROOM_INFO* pNewRoom = (LOBBY_ROOM_INFO*)i3MemAlloc(sizeof(LOBBY_ROOM_INFO));
			i3mem::Copy( &pNewRoom->_RoomInfo, pRoomInfo, sizeof(ROOM_INFO_BASIC));
			pNewRoom->_bModify = true;
			m_RoomList.push_back(pNewRoom);
		}
	}
#endif

	_SetRoomList();
}

void UIPhaseLobby::_ProcessFirstLogin(void)
{	
	if( CCharaInfoContext::i()->HasBasicChara() )
	{
		_FirstLoginPopup();
	}

#if defined(USE_LINK_SHOP)
	//�����ٷΰ��� ��� ���� �ð� ����
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	
	i3::sprintf(g_pEnvSet->m_strDay, L"%dMon%dDay",sysTime.wMonth,sysTime.wDay);

	//���� ȭ�� ��� ������ �����ѵ� ��¥�� �����մϴ�.
	g_pEnvSet->m_bFullScreen = g_pViewer->isFullScreen();
	g_pEnvSet->Save();
#endif
	
	GameLoginContext::i()->ResetFirstLogin();
}

void UIPhaseLobby::_ProcessCheckInvite(REAL32 rDeltaSeconds)
{
	
	UIPhaseSubBase*	_SubPhase = (UIPhaseSubBase*)GetMainFrame()->GetCurrentSubPhase();

	if( IsReadyToUpdate() == false)return;
	if( _SubPhase != nullptr )		return;

	// �ʴ� �޽��� �˻�
	if (InviteContext::i()->PopInviteInfo(&m_InviteInfo))
	{	
		bool bPass = false;

		// ���� �����̸� �����Ѵ�.

		for(INT32 i=0; i < CCommunityContext::i()->GetBlockUserCount(); i++)
		{
			const BLOCK_USER * t_User = CCommunityContext::i()->GetBlockUser(i);
			if(i3::generic_is_iequal(t_User->_wstrNick, m_InviteInfo._wstr_nick) )
				return;
		}

		// �ɼǿ� ���� �ʴ��ϱ� â�� Ȱ��/��Ȱ�� ���¸� �����Ѵ�.
		switch( g_pEnvSet->m_nInviteType)
		{
		case COMMUNITY_PERMISSION_ALL		: bPass = true;			break;		// ��ü ���	
		case COMMUNITY_PERMISSION_NO		: bPass = false;		break;		// ��ü ����	
		case COMMUNITY_PERMISSION_FRIEND	: bPass = false;		break;		// ģ��/Ŭ�� ���	
		default:bPass = true;		break;			
		}

		// ģ��/Ŭ�� �����¶��, ������ �˻��մϴ�.
		if( g_pEnvSet->m_nInviteType == COMMUNITY_PERMISSION_FRIEND)
		{
			INT32 i = 0;
			INT32 nFriendCount = CCommunityContext::i()->GetFriendCount();
			for( i=0; i<nFriendCount; i++)
			{
				const FRIEND_NODE* pNode = CCommunityContext::i()->GetFriend(i);
				if(pNode->_uid == m_InviteInfo._iUID)
				{
					bPass = true;
					break;
				}
			}

			if( !bPass)
			{
				INT32 nClanMemberCount = CCommunityContext::i()->GetClanMemberCount();
				for( i=0; i<nClanMemberCount; i++)
				{
					const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetClanMember(i);
					if(pNode->_uid == m_InviteInfo._iUID)
					{
						bPass = true;
						break;
					}
				}
			}
		}
		
		if( bPass)
		{
			// �ѹ��� �ϳ��� ó���ϱ����� �޽��� �˻� ����
			m_bCheckInvite = false;

			i3::rc_wstring wstrBuf;

			// ���� ���� Ȯ��		
			i3::sprintf(wstrBuf, GAME_RCSTRING("STR_TBL_GUI_LOBBY_INVITE_MESSAGE1"), m_InviteInfo._idxRoom + 1, m_InviteInfo._wstr_nick);

		//	const char * str = GAME_RCSTRING("STR_TBL_GUI_LOBBY_INVITE_MESSAGE1");		/*%d�� ���� %s������ ���� �ʴ� ��û �޽����� *//*�����Ͽ����ϴ�.\n���� �Ͻðڽ��ϱ�?*/
		//	std::string fmt = BOOST_FORMAT2( str, m_InviteInfo._idxRoom + 1, m_InviteInfo._nick);
		//	i3::safe_string_copy( strBuf, fmt.c_str(), MAX_STRING_COUNT );
			
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrBuf, nullptr, MAKE_CALLBACK(CbBeInvited), this);
		}
	}
}

void UIPhaseLobby::_ProcessCommon( REAL32 rDeltaSeconds)
{
	
	// �ʴ� �޽��� �˻�
	if( m_bCheckInvite)
	{
		_ProcessCheckInvite(rDeltaSeconds);
	}

	// ���� ����Ʈ �ý��� - �ϻ��̻� �ʱ� ���ʽ�
	// ������ üũ
	if (UserInfoContext::i()->IsInitialRankUp())
	{
		i3::rc_wstring wstrMessage;
		i3::sprintf( wstrMessage, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_UPGRADE_CHARA"), g_InitialRankUpPoint);/*���� �帳�ϴ�!!\n \n���� ����Ʈ ���� �̺�Ʈ�� {col:255,0,0,255}%d ����Ʈ{/col}��\n���� ����Ʈ�� �����մϴ�.\n \n(���ĺ��ʹ� ���� �� ����Ʈ�� ȹ���մϴ�.)*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMessage);
	}	

	if (UserInfoContext::i()->IsGeneralRankUp())
	{
		UINT32 point = UserInfoContext::i()->GetRankUpPoint();

		i3::rc_wstring wstrMessage;
		i3::sprintf( wstrMessage, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_GENERAL_RANK_UP"), point);/*������ ���� �帳�ϴ�!!\n \n({col:255,0,0,255}%d ����Ʈ{/col}��\n���� ����Ʈ�� �����մϴ�.)*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMessage);
		
		UserInfoContext::i()->ResetRankUp();
		UserInfoContext::i()->SetGeneralRankUpFlag(false);
	}

	if (UserInfoContext::i()->isEventReawardFlage())
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_INFORM_SUCCESS_GIFT"));
		UserInfoContext::i()->SetEventReawardFlag(false);
	}
}

void UIPhaseLobby::_SetRoomInfo( INT32 idx, INT32 RoomIdx, ROOM_INFO_BASIC* pInfo)
{
	LuaState * L;

	// �� ��ȣ
	i3::stack_wstring wstrNumber;
	i3::sprintf( wstrNumber, L"%03d", pInfo->_Idx + 1);

	// ���Ӹ��
	UI_MODE_TYPE mode = GameUI::GetStageModeType( CStageInfoUtil::GetStageData(pInfo->_StageID), pInfo->_IsClanMatch );

	// ������
	i3::stack_wstring wstr_mapNameText;
	INT32 domi_round_shape = -1;
	{
		{

			if (GET_ROOM_INFO_RANDOM_MODE( pInfo->_InfoFlag ))
			{
				wstr_mapNameText = GAME_RCSTRING("STR_POPUP_RANDOMMAP");
			}
			else
			{
				if ( CStageInfoUtil::GetStageData(pInfo->_StageID)->m_ui8Mark == GOODS_MARK_NEW )
				{
					wstr_mapNameText = GAME_RCSTRING("STR_TBL_GUI_LOBBY_NEWMAP_PREFIX");
				}

				i3::wstring wstrStageName = CStageInfoUtil::GetMapData((UINT32)pInfo->_StageID)->getDisplayName();
				i3::generic_string_cat(wstr_mapNameText, wstrStageName);
			}
		}
	}

	// ����
	UI_LOBBY_ROOM_STATE state = ULRS_ADMISSION;
	i3::rc_wstring wstrState;

	//���� �̼� ���̵�
	INT32 nDifficulty = -1;

	// ���� ���¿� ���� "������" or "�����" ǥ��
	if (ROOM_STATE_READY != pInfo->_State)
	{
		// �������϶� �������� or �� �ο��� �������� ���� �Ұ� ó��
		if( GET_ROOM_INFO_INTERENTER(pInfo->_InfoFlag) > 0 ||
			pInfo->_PersonNow == pInfo->_PersonPlayerMax)
		{
			state = ULRS_NO_ADMISSION;
			wstrState = GAME_RCSTRING("STR_UI_CANT_ENTER");
		}
		else
		{
			state = ULRS_PLAY;
			wstrState = GAME_RCSTRING("STBL_IDX_ROOM_STATE_PLAY");
			
			//���� �̼� ���� �� �� ��쿡�� ���̵� ǥ��
			if( mode == UMT_DOMINATION)
			{
				nDifficulty = 0;
				wstrState.clear();
			}
		}
	}
	else
	{
		// ������϶� �� �ο��� ���� ���� ���� �Ұ� ó��
		if( pInfo->_PersonNow == pInfo->_PersonPlayerMax)
		{
			state = ULRS_NO_ADMISSION;
			wstrState = GAME_RCSTRING("STR_UI_CANT_ENTER");
		}
		else
		{
			state = ULRS_ADMISSION;
			wstrState = GAME_RCSTRING("STBL_IDX_ROOM_STATE_WAIT");
		}
	}	

	// �����
	i3::stack_wstring wstrPerson;
	i3::sprintf( wstrPerson, L"%02d/%02d", pInfo->_PersonNow, pInfo->_PersonPlayerMax);

	// ��й�
	INT32 secret = GET_ROOM_INFO_PRIVATE_MODE( pInfo->_InfoFlag )?0:-1;

	//	���� ���� & ���� ���� ����
	INT32 mode_option = GameUI::GetModeOptionShapeIndex(*pInfo);

	//	���� IP�� �ƴϸ� ��ũ ǥ��
	bool bRealIP = false;

	//	���� IP�� ����ǥ ��ũ�� ǥ�� �ȵȴ�.
	bRealIP = pInfo->_InfoFlag ? true : false;

	L = _CallLuaFunction( "SetRoomInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, idx);
		i3Lua::PushStringUTF16To8( L, wstrNumber);			// ���ȣ
		i3Lua::PushInteger( L, secret);						// ��й�ǥ��
		i3Lua::PushStringUTF16To8(L, pInfo->_Title);		// ������
		i3Lua::PushInteger( L, mode_option);				// ���� ���� & ���� ���� ����
		i3Lua::PushInteger( L, mode);						// ���
		
		i3Lua::PushInteger( L, domi_round_shape);			// ���� ���� Icon
		i3Lua::PushStringUTF16To8( L, wstr_mapNameText);	// �� �̸�
		
		i3Lua::PushStringUTF16To8( L, wstrPerson);		// ����� ��
		i3Lua::PushInteger( L, state);				// ����(�̹���)
		i3Lua::PushStringUTF16To8( L, wstrState);		// ����

		i3Lua::PushInteger( L, nDifficulty);		// ���� ���̵�
		_EndLuaFunction( L, 12);
	}
}

void UIPhaseLobby::_SetEmptyRoomInfo( INT32 idx)
{
	LuaState * L;

	const i3::wliteral_range wEmptyRng = L""; 

	L = _CallLuaFunction( "SetRoomInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, idx);
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// ���ȣ
		i3Lua::PushInteger( L, -1);				// ��й�ǥ��
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// ������
		i3Lua::PushInteger( L, -1);				// ���� ����
		i3Lua::PushInteger( L, -1);				// ���
		i3Lua::PushInteger( L, -1);				// ���� ���� Icon
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// �� �̸�
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// ����� ��
		i3Lua::PushInteger( L, -1);				// ����(�̹���)
		i3Lua::PushStringUTF16To8( L, wEmptyRng);	// ����
		
		i3Lua::PushInteger( L, -1);				// ���� ���̵�
		_EndLuaFunction( L, 12);
	}
}

void UIPhaseLobby::_SetRoomColor( INT32 RoomIdx, I3COLOR * pColor)
{
	LuaState * L = _CallLuaFunction( "SetRoomColor");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, RoomIdx);
		i3Lua::PushInteger( L, i3Color::GetR( pColor));
		i3Lua::PushInteger( L, i3Color::GetG( pColor));
		i3Lua::PushInteger( L, i3Color::GetB( pColor));
		i3Lua::PushInteger( L, i3Color::GetA( pColor));
		_EndLuaFunction( L, 5);
	}
}

void	UIPhaseLobby::_RestoreRoomColor(INT32 RoomIdx)
{
	LuaState * L = _CallLuaFunction( "RestoreRoomColor");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, RoomIdx);
		_EndLuaFunction( L, 1);
	}	
}

void UIPhaseLobby::_FirstLoginPopup()
{
	if( g_pEnvSet->m_bWrongEnvSet)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_DUPLICATED_KEY"));/*�ߺ��� Ű ������ �־� Ű������ �ʱ�ȭ �Ͽ����ϴ�*/
	}

	// �˾� ü��: �����˾� -> PC�� �˾�
	const COUPON_INFO* info = LobbyContext::i()->getCouponInfo();

	if ( 0 < info->_count)
	{
		i3::rc_wstring wstr_couponMessage;

		/*%d���� ������ ���Ǿ����ϴ�.\n \n���� ����\n \n������������������������%d ����Ʈ*/
		i3::sprintf(wstr_couponMessage, GAME_RCSTRING("STR_TBL_GUI_LOBBY_CUPON_MESSAGE"), info->_count,info->_point);
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_couponMessage, nullptr, MAKE_CALLBACK(CbSuppliedConfirm), this);
		LobbyContext::i()->clearCouponInfo();
		return;
	}

#if defined(USE_LINK_SHOP )
	/*//�˾� : �����ٷΰ��� : ����Ÿ�Ӱ� EnySet�� ����� ��¥�� ���մϴ�. 
	USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
	UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

	char szTemp[128];
	if((i3String::Compare(g_pEnvSet->m_szDay,szTemp)!=0) && (InfoEquipWeapon._noprim == 0) )
	{
		if(g_pViewer->isFullScreen()&&i3String::Contain(g_pEnvSet->m_szDay,szTemp)!=-1 )
		{	//Ǯ��ũ������ m_szDay�� �����ϴ� ���� �޶����Ƿ� Contain���� �ѹ��� �˻��մϴ�.
		}
		else
		{
			//���� �ٷΰ��� ��� 
			GetMainFrame()->TogglePopup( UPW_LINK_SHOP, (void*) 1));
		}
		return;
	}*/
#endif
	if (!m_bPcCafeNotice)
		ShowPcCafePopup();

	// Key : �⼮ üũ �߰�
	if( AttendanceContext::i()->IsAttendanceEvent() )
	{
		ShowAttendancePopup();
	}

	ShowNoticePopup();

	if(!TwitchTVContext::i()->isMsgBox())
	{
		if(!TwitchTVContext::i()->isFastStreamingStop())
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY,GAME_RCSTRING("STR_POPUP_TWITCH_EXCEPTION_EXIT"),&GAME_RCSTRING("STBL_IDX_COMMON_MSGBOX_CAPTION"),MAKE_CALLBACK(TwitchTVContext::i()->CbFastStopStreaming),this);
	}

	// ������ ���� ��� �˾�
	if (UserInfoContext::i()->IsAprilFoolsDay())
	{
		USER_INFO_INVITEM_DATA myUserInfoInvItemData;

		UserInfoContext::i()->GetMyUserInfoInvItemData(&myUserInfoInvItemData);

		INT32 itemID = CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL_CLIENT;
		USE_ITEM_POPUP_TYPE pType = USE_ITEM_DISGUISE_RANK;

		if (myUserInfoInvItemData._wAprilFoolRank == RANK_NONE)
		{
			GetMainFrame()->OpenPopup(UPW_DISGUISE_RANK, &itemID, &pType);
		}
	}
}

void UIPhaseLobby::_CloseAllSubPhaseAndPopup( void)
{
	UIMainFrame * pFrame = GetMainFrame();
	
	// �޼����ڽ��� ������ �����Ѵ�..(11.11.01.����)
	//GameUI::CloseAllMsgBox();
	pFrame->TogglePopup( UPW_NULL);
	
	if (USP_NONE != pFrame->GetCurrentSubPhaseType())
	{
		pFrame->ChangeSubPhase( pFrame->GetCurrentSubPhaseType());
	}
}

void UIPhaseLobby::_ClearRoomInfo( void)
{
	// Lobby Banner�� ���� �̱���..
	//GetMainFrame()->EnableFloatWin( UFW_LOBBYBANNER);
	GetMainFrame()->EnableFloatWin( UFW_CHARACTER);
	
	DisableRoomTooltip();

	m_SelectedElement = -1;
}

void UIPhaseLobby::_AddMsgBoxToTopMenuScreen(void)
{
	if( m_pKickedMassageBox != nullptr)
	{
		// �ֻ����� �׸����� �Ѵ�.
		i3UIScene * pScene = m_pKickedMassageBox->GetScene(0);

		if( pScene != nullptr && pScene->getParent())
		{
			pScene->ChangeParent( pScene->getParent());
		}	
	}
}

bool UIPhaseLobby::CompareRoom( const LOBBY_ROOM_INFO* pItem1, const LOBBY_ROOM_INFO* pItem2)
{
	const ROOM_INFO_BASIC* pRoom[2];

	pRoom[0] = &pItem1->_RoomInfo;
	pRoom[1] = &pItem2->_RoomInfo;

	INT32 nResult = 0;
	
	switch( sm_SortOrder[sm_nCurSortType]._nType)
	{		
	case ROOM_SORT_NUMBER:
		{
			// �� ��ȣ�� ����
			nResult = SortRoom::SortbyNumber(pRoom[0], pRoom[1]);
		}												
		break;	
	case ROOM_SORT_ROOMNAME:
		{
			// ���� ������ ����
			nResult = SortRoom::SortbyRoomName(pRoom[0], pRoom[1]);
		}												
		break;	
	case ROOM_SORT_MODE:
		{
			// ��庰 ����
			nResult = SortRoom::SortbyMode(pRoom[0], pRoom[1]);
		}
		break;
	case ROOM_SORT_MAP:
		{
			// �������� �̸� ������ ����
			nResult = SortRoom::SortbyStageName(pRoom[0], pRoom[1]);
		}
		break;
	case ROOM_SORT_STATE:
		{
			// ���� ���� ������ ����
			nResult = SortRoom::SortbyRoomState(pRoom[0], pRoom[1]);
		}

		break;
	case ROOM_SORT_PERSON:
		{
			// �ο� �� ������ ����
			nResult = SortRoom::SortbyPerson(pRoom[0], pRoom[1]);
		}
		break;
	default:
		nResult = 0;
		break;
	}

	if( sm_SortOrder[sm_nCurSortType]._bAscending == false )
	{
		nResult = -nResult;
	}

	return nResult < 0;
}


bool UIPhaseLobby::CompareAdmissionRoom( const LOBBY_ROOM_INFO* pItem1, const LOBBY_ROOM_INFO* pItem2)
{
	const ROOM_INFO_BASIC* pRoom[2];

	pRoom[0] = &pItem1->_RoomInfo;
	pRoom[1] = &pItem2->_RoomInfo;

	INT32 nResult = 0;

	INT32 nCompareValue[2] = {0};

	// ���� ����/�Ұ����� üũ,
	// �濡 �� ������ �ִ� 15�̱� ������ 16������ �÷��׸� ��ü�Ͽ� ����մϴ�.
	if((pRoom[0]->_State != ROOM_STATE_READY && GET_ROOM_INFO_INTERENTER(pRoom[0]->_InfoFlag) > 0) ||
		pRoom[0]->_PersonNow == pRoom[0]->_PersonPlayerMax)
	{
		nCompareValue[0] = 0x00000010;
	}

	if((pRoom[1]->_State != ROOM_STATE_READY && GET_ROOM_INFO_INTERENTER(pRoom[1]->_InfoFlag) > 0) ||
		pRoom[1]->_PersonNow == pRoom[1]->_PersonPlayerMax)
	{
		nCompareValue[1] = 0x00000010;
	}

	// ���� �� �ڸ��� üũ�մϴ� (0~15 slot)
	nCompareValue[0] += pRoom[0]->_PersonPlayerMax - pRoom[0]->_PersonNow;
	nCompareValue[1] += pRoom[1]->_PersonPlayerMax - pRoom[1]->_PersonNow;

	nResult = nCompareValue[0] - nCompareValue[1];

	return nResult < 0;
}

bool UIPhaseLobby::OnInviteMove( void)
{
	if( m_bWaitingFromServer)
		return false;

	m_bWaitingFromServer = true;
	m_bAllowRefresh = false;

	i3UI::setDisableUIInput(true);
	// ����, �˾� �ݴ´�.
	_CloseAllSubPhaseAndPopup();

	// ���� / ä���� �ٸ��� Ȯ��
	bool bOtherServer = CCommunityContext::i()->GetMoveTargetServer() != (UINT32)g_pServerInfo->GetCurGameServerIndex();
	bool bOtherChannel = CCommunityContext::i()->GetMoveTargetChannel() != (UINT32)g_pServerInfo->GetCurChannelNumber();

	// ���� �ϳ��� �ٸ��ٸ�, ����/ä�� �������� �̵�
	if( bOtherServer == true || bOtherChannel == true)
	{
		GameEventSender::i()->SetEventOnce( EVENT_LEAVE_LOBBY);
	}
	else
	{
		// �游��� â �ݱ�
		OnCreateRoomCancel();
		
		UINT32 ui32TargetMatch = CCommunityContext::i()->GetMoveTargetMatch();

		if( MATCH_TEAM_INVALID_IDX != ui32TargetMatch)
		{
			GetMainFrame()->MovePhase(UIPHASE_CLANLOBBY);
		}
		else
		{
			m_SelectedElement = static_cast<INT32>(CCommunityContext::i()->GetMoveTargetRoom());
			MyRoomInfoContext::i()->setMyRoomIndex(m_SelectedElement);

			bool bInvite = true;

			GameEventSender::i()->SetEventOnce(EVENT_JOIN_ROOM, &bInvite);
		}
	}

	m_bMyPhase = false;

	return true;
}

void UIPhaseLobby::SelectRoom( INT32 idx)
{
	if( idx >= (INT32)m_RoomList.size() || idx < 0)
	{
		_ClearRoomInfo();
	}
	else
	{
		if( m_SelectedElement != idx)
		{
			m_bRequestRoomInfoAdd = true;
			m_SelectedElement	= idx;
		}
		else
		{
			// ������ �ٽ� ��� ��� ���� ó���� ���� �߰�
			m_bQueRoomInfo = true;
		}
	}
}

void UIPhaseLobby::RefreshSelectedRoom( void)
{
	// ���õǾ� �ִ� ���� ������ �����ϴ� �̺�Ʈ�Դϴ�.
	// �븮��Ʈ�� �����ϴ� �̺�Ʈ�� �ƴմϴ�...
	// �� �̷��� �𸣰ڽ��ϴ�.
	if ( m_SelectedElement == -1 )
		return;

	// �ݺ��ؼ� ������ 2�ʰ������� �ǰ� �մϴ�.
	if ( g_pFramework->GetAccTime() - m_LastRefreshTime <= g_pConfigEx->GetNetwork().RefreshInterval_RoomInfo)
		return;
	m_LastRefreshTime = g_pFramework->GetAccTime();

	GameEventSender::i()->SetEvent( EVENT_GET_ROOM_ADDINFO_N );
	m_bRequestRoomInfoAdd = true;

	I3TRACE0("EVENT_GET_ROOM_ADDINFO_N [CPhaseLobby::OnRefresh]\n");

	_SortRoomList();
	//m_SelectedElement = -1;
}

void	UIPhaseLobby::OnRefreshButton()
{
	// ������ ���� �̺�Ʈ�� ���� �ʵ��� ��� ó���Ѵ�..
	
	if (m_bRefreshButtonActive )
		return;
	
	// �������� ���Žð��� 2���ε�...�̰ͺ��ٴ� ���� ó���ɼ� �ֵ��� �ؾ��Ѵ�..
	
	m_bRefreshButtonActive = true;

	GameEventSender::i()->SetEvent(EVENT_LOBBY_REFRESH_N);		// Send��Ŷ�� ������ ������ ���ο���̴�.
	// �켱 �ش� �̺�Ʈ�� SEND�Ǿ��µ�, RECEIVE�� ���ٸ� ���� ó���� �߰��Ѵ�..

	m_fRefreshElapsedTime = g_pConfigEx->GetNetwork().RefreshInterval_RoomList;

}

void  UIPhaseLobby::OnScrollRoomList(void)
{
	if(m_pScrollRoomList == nullptr)
		return;

	if( m_RoomList.size() <= UI_LOBBY_ROOMLIST_VIEWCOUNT )
		return;

	_SetRoomList();
	_SetCurSelectItem();
}

void  UIPhaseLobby::OnWheelRoomList(INT32 zDelta)
{
	if(m_pScrollRoomList == nullptr)
		return;

	if( m_RoomList.size() <= UI_LOBBY_ROOMLIST_VIEWCOUNT )
		return;

	INT32 i32NewPos = GetRoomListScrollPos() + (zDelta / -120);	
	INT32 i32Min, i32Max = 0;

	m_pScrollRoomList->getScrollRange(&i32Min, &i32Max);
	i32NewPos = MINMAX(i32Min, i32NewPos, i32Max);

	m_pScrollRoomList->setScrollPos( i32NewPos );

	_SetRoomList();
	_SetCurSelectItem();
}

void  UIPhaseLobby::SortRoomList(INT32 nType)
{
	sm_nCurSortType = nType;

	if(sm_SortOrder[nType]._bAscending == true)
		sm_SortOrder[nType]._bAscending = false;
	else
		sm_SortOrder[nType]._bAscending =  true;

	_ClearRoomInfo();

	_SortRoomList();

	// �ٷ� ȭ���� ����
	_UpdateRoomList();
}

void	UIPhaseLobby::SortAdmissionRoom(bool bViewAdmissionRoom)
{
	m_bViewAdmissionRoom = bViewAdmissionRoom;

	_ClearRoomInfo();

	_SortRoomList();

	// �ٷ� ȭ���� ����
	_UpdateRoomList();
}

void UIPhaseLobby::ConfirmAbuseOK( void)
{
	AbuseContext::i()->ResetAbuse();

	GetMainFrame()->SetEffectButtonBegin();

	//���¡ �Ϸ� To ����
	GameEventSender::i()->SetEvent(EVENT_ABUSING_SET);

	SetInputDisable( false);
}

void UIPhaseLobby::DisableRoomTooltip(void)
{
	if(m_pTTMapInfo != nullptr)
	{
		m_pTTMapInfo->DisableToolTip();
		I3TRACE( "Disable RoomInfo\n");
	}
}


void UIPhaseLobby::_SortRoomList(void)
{
#if defined( TEST_LOBBY_ROOM_LIST)
	_BuildRoomListDummy();
#else
	// Clear Room List
	i3::cu::for_each_SafeFree_clear(m_RoomList);

    // Copy Room List
	INT32 nRoomCount = LobbyContext::i()->getLobbyRoomCount();
	for(INT32 i = 0; i < nRoomCount; i++)
	{
		const ROOM_INFO_BASIC * pRoomInfo = LobbyContext::i()->getRoomInfo(i);
		if ( ! pRoomInfo )
		{
			I3ASSERT(0 && "Invalid room index.");
			continue;
		}
		// PersonNow�� 0�̸� ���� ��
		if (0 == pRoomInfo->_PersonNow)
		{
			continue;
		}

		LOBBY_ROOM_INFO* pNewRoom = (LOBBY_ROOM_INFO*)i3MemAlloc(sizeof(LOBBY_ROOM_INFO));
		i3mem::Copy( &pNewRoom->_RoomInfo, pRoomInfo, sizeof(ROOM_INFO_BASIC));
		m_RoomList.push_back(pNewRoom);
		
	}
#endif

	// Sort Room List
	// ������ư�� ����� ���� ����/�Ұ��� �� ������ �� ��쿡��
	// ������ CompareAdmissionRoom �� ����մϴ�.
	if(m_bViewAdmissionRoom == true)
		i3::sort(m_RoomList.begin(), m_RoomList.end(), CompareAdmissionRoom);
	else
		i3::sort(m_RoomList.begin(), m_RoomList.end(), CompareRoom);

	m_is_sort_room_list = true;
}

void UIPhaseLobby::_EmptyRoomList(void)
{
	INT32 j;
	INT32 nPreRemoveRoomCount = 0;

	INT32 nRoomCount = LobbyContext::i()->getLobbyRoomCount();

	for(size_t i = 0; i < m_RoomList.size(); i++)
	{
		bool bBeRoom = false;
		LOBBY_ROOM_INFO * pInfo = m_RoomList[i];
		ROOM_INFO_BASIC* pRoom = &pInfo->_RoomInfo;

		if (ROOM_STATE_EMPTY != pRoom->_State)
		{
			for( j = 0; j < nRoomCount; j++)
			{
				const ROOM_INFO_BASIC* pRoomInfo = LobbyContext::i()->getRoomInfo(j);
				if ( ! pRoomInfo )
					continue;

				if (pRoom->_Idx == pRoomInfo->_Idx)
				{
					// PersonNow�� 0�̸� ���� ��
					if (0 != pRoomInfo->_PersonNow)
					{
						bBeRoom = true;
					}
					break;
				}
			}
		}
		
		if (false == bBeRoom)
		{
			// ȭ�鿡 �������� ��������, ���õ� ���� ���ϴ� �������� ���⸸ �Ѵ�. - ��� �������� �� ����
			//if ((UINT32)i < m_pList->GetCurrentTop() + MAX_ROOM_PER_PAGE_EX || (-1 != m_SelectedElement && i < m_SelectedElement))

			// �������� ���� ���Ŀ� �����ϴ� ���ý����� ���ŵǸ�, ���� ���� �ʱ�ȭ -> �޺κ��� �� �� ���ŵǵ���
			if ((INT32)i < m_SelectedElement)
			{
				nPreRemoveRoomCount++;				
			}
			else if ((INT32)i == m_SelectedElement)
			{
				_ClearRoomInfo();
				m_SelectedElement = -1;
			}

			i3MemFree(m_RoomList[i]);
			i3::vu::erase_index(m_RoomList, i);
			i--;
		}
	}

	// ���� ���� ���Ϻκ� ������ ��ŭ ���� ���� �̵�
	if (-1 != m_SelectedElement)
	{
		m_SelectedElement = MAX(-1, m_SelectedElement - nPreRemoveRoomCount);
	}
}


//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//

/*virtual*/ void UIPhaseLobby::OnCreate( i3GameNode * pParent)
{
	UIPhaseBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_Lobby_RoomList.i3UIs",  PhaseLobby_Glue);

	m_pTTMapInfo = UIToolTipLobbyMapInfo::new_object();
	m_pTTMapInfo->OnCreate(this);
}

void	UIPhaseLobby::OnUpdate_NoneRsc(REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate_NoneRsc(rDeltaSeconds);
	
	if( !GameLoginContext::i()->IsFirstLogin())
	{
		_ProcessCommon( rDeltaSeconds);
	}
}


/*virtual*/ void UIPhaseLobby::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate( rDeltaSeconds);

	if( ChannelContext::i()->GetConnectedChannelType() == CHANNEL_TYPE_CLAN &&
		UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
	{
		if(UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
		{
			// Ŭ����ġON - �뺴���� ���ŵǸ� �κ񿡼� ������ �� 
			// UIFloatMercenaryLIst������ ���� �� ���� ������ ���⼭ ó��
			if(GetMainFrame()->getDeprotationClanChannel() == false &&
				ClanGameContext::i()->getMyMercenaryState() == false &&
				m_PreClanMemberLevel == (CLAN_MEMBER_LEVEL_TYPE)UserInfoContext::i()->GetMyClanState() )
			{
				m_bAllowRefresh = false;
				m_bRefreshButtonActive = false;

				i3UI::setDisableUIInput( true);

				GameEventSender::i()->SetEventOnce( EVENT_LEAVE_LOBBY);
				m_bMyPhase = false;
				return;
			}

			m_PreClanMemberLevel = (CLAN_MEMBER_LEVEL_TYPE)UserInfoContext::i()->GetMyClanState();
		}
	}



	if( isSliding())
		return;

	if( m_bAllowRefresh)
	{
		__AutoRefreshRoomList(rDeltaSeconds);
	}

	// ���¡ ���� �ð� �Լ�
	if( AbuseContext::i()->IsAbusing())
	{
		__ProcessAbusing(rDeltaSeconds);
	}

	// �ƹ�Ÿ �ε����� �ƴ� ���
	if( !CharaViewMgr::i()->isLoadWaiting() && CharaViewMgr::i()->isLoadedChara() )
	{
		m_pBtnTutorial->removeState( I3UI_STATE_DEACTIVATED);
		m_pBtnTutorial->setModifyState();

		m_bTutorialEnable = true;
	}
	else
	{
		m_pBtnTutorial->addState( I3UI_STATE_DEACTIVATED);
		m_pBtnTutorial->setModifyState();
	}

	// EnterLobby �� RoomList�� �޾ƿ������ ��� �ð�
	if( m_rEnterLobbyWaitTime < ENTER_LOBBY_WAIT_TIME )
	{		
		m_rEnterLobbyWaitTime += rDeltaSeconds;
		_AddMsgBoxToTopMenuScreen();
		return;
	}

	//������ ���� �� �� ���ĵǾ�� ���� �ൿ�� �� �� �ִ�.
	//�� �������� �Է��� ������.
	if( m_is_sort_room_list == false ) return; 

	if( m_pWaitRecevingRoomList != nullptr)
	{
		//GameUI::DisableLoadingIcon( m_pWaitRecevingRoomList);
		m_pWaitRecevingRoomList = nullptr;
		
		m_bCheckInvite = true;
	}
	else
	{
		if(ButtonTermTime > 1.5f && !AbuseContext::i()->IsAbusing())
		{
			((UIFramework*) m_pParent)->GetUIMainframe()->SetEffectBegin();
		}
		else
		{
			ButtonTermTime += rDeltaSeconds;
		}
	}

	if( GameLoginContext::i()->IsFirstLogin())
	{
		g_pFramework->GetMessageBoxManager()->m_is_open_message_box = false; //message box Ȱ��ȭ, ��Ȱ��ȭ ������ �˱� ���� ����

		_ProcessFirstLogin();

		if( GetMainFrame()->IsPopupFocused() == false && 
			g_pFramework->GetMessageBoxManager()->m_is_open_message_box == false )
		{
			g_pFramework->SetFocus(nullptr);
			GetMainFrame()->InputDisableMainUI(false);
		}

		/*GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_STRING("STBL_IDX_EP_SERVER_FAIL_REFRESH_C"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);*/
	}

	if( m_SelectedElement > -1 && m_SelectedElement < (INT32)m_RoomList.size() )
	{
		LOBBY_ROOM_INFO * pInfo = m_RoomList[ m_SelectedElement];
		I3ASSERT( pInfo != nullptr);

		// ���� �޼����� ���� ����մϴ�.
		if (m_bQueRoomInfo == true)
		{
			if(m_pTTMapInfo != nullptr)
			{
				if( m_pTTMapInfo->isEnable() == false )
					m_pTTMapInfo->EnableToolTip(&pInfo->_RoomInfo, LobbyContext::i()->getSelectedRoomInfoAdd());
				else
					m_pTTMapInfo->ChangeToolTip(&pInfo->_RoomInfo, LobbyContext::i()->getSelectedRoomInfoAdd());

				m_bQueRoomInfo = false;
			}
		}

		if( m_bRequestRoomInfoAdd)
		{
			// ���� ���� ������ �ݺ��ؼ� ������ 2�ʰ������� �ǰ� �մϴ�.
			if ( g_pFramework->GetAccTime() - m_LastRefreshTime > g_pConfigEx->GetNetwork().RefreshInterval_RoomInfo)
			{
				ROOM_INFO_BASIC * pRoomInfo = &pInfo->_RoomInfo;

				m_LastRefreshTime = g_pFramework->GetAccTime();

				LobbyContext::i()->setSelectedRoomIdx( pRoomInfo->_Idx);

				I3TRACE("EVENT_GET_ROOM_ADDINFO_N [UIPhaseLobby::SelectRoom]\n");
				GameEventSender::i()->SetEvent( EVENT_GET_ROOM_ADDINFO_N );

				m_bRequestRoomInfoAdd = false;
			}
		}
	}

	//new hot ��ǰ ��� �����.
	if( LocaleValue::Enable("EnableLobbyNewHot") )
	{
		UINewAndHotBanner* pfnewAndHot = static_cast<UINewAndHotBanner *>(GameUI::GetFloating( UFW_NEWANDHOT));
		if( pfnewAndHot != nullptr)
		{
			if( pfnewAndHot->isEnable() == true && pfnewAndHot->GetMakeList() == false)
			{
				pfnewAndHot->MakeNewAndHotGoodsList();
				pfnewAndHot->SelectGoodsInList();
			}
		}
	}
}

/*virtual*/ void	UIPhaseLobby::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	// ��� ��ġ������ Lobby Event�� �޾ƾ��մϴ�.
	// ���� Phase�� ���� �ֱ⿡ Base���� ó���մϴ�. --swoongoo
	UIPhaseBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

bool UIPhaseLobby::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == I3_EVT_INPUT)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC) ||
				pKeyboard->GetStrokeState(I3I_KEY_ENTER) )
			{
				if( GetMainFrame()->IsOpenedPopup( UPW_PCCAFE))
					return GetMainFrame()->GetPopup( UPW_PCCAFE)->OnEvent( event, pObj, param2, code);

				//�н����� �˾�â���� editbox�� ��Ŀ���� �ִ� ���¿��� ����Ű�� ������ �� �Լ��� ȣ��Ǳ� ������ 
				//lobby Ŭ�������� �н����� �˾��� OnKey_Enter �� ���������� ȣ���ؾ� �ȴ�.
				//������, �н����� �˾�â ��ü�� ��Ŀ���� �ִ� ��쿣 �н����� �˾�â OnKey_Enter �Լ��� ȣ��ȴ�.
				if( GetMainFrame()->IsOpenedPopup( UPW_QUERY_PASSWORD)) 
					return GetMainFrame()->GetPopup(UPW_QUERY_PASSWORD)->OnKey_Enter();
			}

			// ���� ���� ����
			if( pKeyboard->GetStrokeState(I3I_KEY_F5))
			{
				// SubPhase == NONE , ���� �˾��� ������ ����
				if(GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE && 
					GetMainFrame()->IsPopupFocused() == false && LocaleValue::Enable("EnableQuickJoin") == true)
				{
					UIMainFrame* f = g_pFramework->GetUIMainframe();
					I3ASSERT(f);
					f->OpenPopup(UPW_QUICK_JOIN);
				}
			}
		}
	}

	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;

		i3UIButton * pButton = i3::same_cast<i3UIButton*>(pNotify->m_pCtrl);

		if( pButton != nullptr)
		{
			i3UIStaticText * pTextCtrl = i3::same_cast<i3UIStaticText*>(pButton->FindChildByType(i3UIStaticText::static_meta()));
			if( pTextCtrl != nullptr)
			{
				COLOR color;

				if( pButton->isSelected() )					i3Color::Set(&color, (UINT8)0, 51, 96, 255);
				if( pButton->isState( I3UI_STATE_ONMOUSE) )	i3Color::Set(&color, (UINT8)6, 82, 149, 255);
				else										i3Color::Set(&color, (UINT8)184, 216, 255, 255);

				pTextCtrl->SetTextColor(&color);
			}
		}
	}

	return UIPhaseBase::OnEvent(event, pObj, param2, code);
}

bool UIPhaseLobby::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	//BattleSlotContext::i()->setMySlotIdxEx( 0 );
	
	//	Ŭ���� ����ó��
	if( g_pFramework->GetClanContext()->GetVisibleLobbyPhase())
	{
		if( UIPhaseBase::OnEntranceStart( pArg1, pArg2) == false)
			return false;
	}
	
	_InitRoomList();

	// sort value init
	{
		m_bViewAdmissionRoom = false;

		for(INT32 idx = 0 ; idx < MAX_SORT_TYPE; ++idx)
			sm_SortOrder[idx]._bAscending = false;

		sm_nCurSortType = 0;
		sm_SortOrder[0]._bAscending = true;
	}

	g_pFramework->getChatBox()->setWhisperEnable( true);
	g_pFramework->getChatBox()->ResetHistoryBufferIndex();
	
	//	�κ� ������ ���� ���� ������ ��ǰ ����Ʈ�� ��û�Ѵ�. 
	//	Ŀ�´�Ƽ���� ���� ���� Ȯ���ϱ� ���� �����κ��� ���� ����Ʈ�� ���������� ��� �޾ƾ� ���� ������ ���� �۵��ϰ� �˴ϴ�.  komet
	//	(��ǰ ����Ʈ�� ���� ������ ����)
	GameEventSender::i()->SetEvent( EVENT_REQUEST_GOODS_LIST);

	if ( LocaleValue::Enable("FieldShop"))
	{
		IGShopContext* igcont = g_pFramework->GetIGShopContext();
		igcont->SetEvent_GetInfo();
	}

	UITopMenu * pTop = GetMainFrame()->GetTopMenu();
	if( pTop != nullptr)
		pTop->SetMode(TOPMENU_MODE_READY);

	// Lobby Button�� �����մϴ�.
	m_SelectedElement = -1;
	_SetButtonState();
	
	UI_CHARA_INFO	* pNetCharaInfo = UserInfoContext::i()->SetMyUICharaInfo();
	CCharaInfoContext::i()->GetUICharaInfo( pNetCharaInfo);
	m_bWaitingFromServer = false;

	m_slide.ForceEnd();	//slide �ൿ�� �����Ѵ�.

	if( GameLoginContext::i()->IsFirstLogin() == true )
	{
		g_pFramework->SetFocus(nullptr);
		GetMainFrame()->InputDisableMainUI(true);

		// ������ ����Ʈ ��û. (������ �˶� ������� ���� �κ� ù ���Խ� Recv)
		GameEventSender::i()->SetEvent(EVENT_GET_GIFT_LIST);
	}	

	if(MedalContext::i()->GetReceivedMyMedal() == false)
	{
		GameEventSender::i()->SetEvent(EVENT_MEDAL_GET_NORMAL_INFO);
	}

	GameEventSender::i()->SetEvent(EVENT_GET_REMAIN_MONEY);

	return true;
}

// ������
/*virtual*/ bool UIPhaseLobby::OnEntrancing( REAL32 rDeltaSeconds)
{
	if( g_pFramework->GetClanContext()->GetVisibleLobbyPhase())
	{
		return UIPhaseBase::OnEntrancing( rDeltaSeconds);
	}

	// �κ񿡼��� �ڽ��� ���� ��ȣ�� 0������ ����
	BattleSlotContext::i()->setMySlotIdxEx(0);

	return true;
}


// ���� �Ϸ�
/*virtual*/ void UIPhaseLobby::OnEntranceEnd( void)
{
	UIPhaseBase::OnEntranceEnd();

	SortRoom::InitPriority();

#ifndef NC_BUILD
	AutoCheckEquipments();
#endif

	m_rEnterLobbyWaitTime = 0.0f;
	m_bAllowRefresh = true;
	m_bRefreshButtonActive = false;

	// enable invite
	InviteContext::i()->ClearInviteInfo();
	m_bCheckInvite = true;

	LuaState * L = _CallLuaFunction("SetClanButton");
	if( L != nullptr)
	{
		bool bClanChannel = ChannelContext::i()->GetBattleChannel();
		bool bClanMatchEnable =	UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW); 

		i3Lua::PushBoolean(L , bClanChannel && bClanMatchEnable);
		_EndLuaFunction(L , 1);
	}

	if ((UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN))
	{ 
		m_pBtnTeamLIst->disable_btn();
	}
	else
		m_pBtnTeamLIst->enable_btn();


	//	Ŭ���� ����ó��
	if( g_pFramework->GetClanContext()->GetVisibleLobbyPhase())
	{
		//  ���¡ ���� �ϵ��� 20110402
		if(AbuseContext::i()->IsAbusing())
		{
			//GetMainFrame()->TogglePopup( UPW_ABUSE);
			//�������� ���� ���¡ ������ �°� �ؽ�Ʈ ���� �� ���� ���� �ʿ�
			if( m_pAbusingBox == nullptr)
			{
				switch( AbuseContext::i()->GetAbuseLevel())
				{
				case ABUSING_LEVEL_NORMAL:
					m_pAbusingBox = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, GAME_RCSTRING("STR_ABUSING_LEVEL_NORMAL"));
					break;
				case ABUSING_LEVEL_STRONG:
					m_pAbusingBox = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, GAME_RCSTRING("STR_ABUSING_LEVEL_STRONG"));
					break;
				case ABUSING_LEVEL_STRONGEST:
					m_pAbusingBox = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, GAME_RCSTRING("STR_ABUSING_LEVEL_STRONGEST"));
					break;
				default :
					I3PRINTLOG(I3LOG_WARN,  "switch invalid call!!!");
					break;
				}
			}
			
			m_rAbuseTime = 0.f;
		}
		else
		{
			if ( KickContext::i()->IsKickedByVote() )
			{		// �������� ��� ���޼��� ��� ����Ǿ��ٰ� �˷��ش�..
				USER_INFO_BASIC basic;
				UserInfoContext::i()->GetMyUserInfoBasic(&basic);

				i3::rc_wstring wstrMsg;
				i3::sprintf(wstrMsg, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_KICKED_BY_VOTE"), basic.m_strNick);		// �ڵ�����ȯ..? ( 2014.07.21.����)

				i3UI::getGlobalIme()->KillFocus();
				GetMainFrame()->EnableFloatWin( UFW_MINIPROFILE);
				m_pKickedMassageBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMsg, nullptr, MAKE_CALLBACK(CbKickedByVote), this); /*���� �˷���*/

				KickContext::i()->ClearKickedByVote();
			}
		}
	}

	m_is_sort_room_list = false;
	m_bRequestRoomInfoAdd = false;
	m_SelectedElement = -1;

	// Battle���� �����˾� �� ���� �� ó��
	if( g_pFramework->IsMessageBoxEnabled() )
	{
		i3UI::setDisableUIInput( true);
	}

	// ���� ��ϳ� ���� �߿��մϴ�. - praptor
	{
		if (CCommunityContext::i()->IsMovingToUser())
		{
			OnInviteMove();
			return;
		}

		INT32 Channel_Index = g_pServerInfo->GetCurChannelNumber();
		INT32 Server_Index = g_pServerInfo->GetCurGameServerIndex();
		ChannelType Channel_Type = ChannelContext::i()->GetChannelType(Server_Index, Channel_Index);

		if (ChannelContext::i()->IsMoveChannel()
			&& (Channel_Type != CHANNEL_TYPE_CLAN && Channel_Type != CHANNEL_TYPE_PCCAFE_CLAN) )
		{
			m_bAllowRefresh = false;
			ChannelContext::i()->ClearMoveChannel(); 

			i3UI::setDisableUIInput(true);
			GameEventSender::i()->SetEventOnce(EVENT_LEAVE_LOBBY);
			m_bMyPhase = false;	// �ڽ��� Phase�� �ƴϴ�.
			return;
		}
	}

	if( TutorialContext::i()->WasTutorial())
	{
		TutorialContext::i()->SetWasTutorial(false);
		GetMainFrame()->EnableFloatWin( UFW_MINIPROFILE);
		GetMainFrame()->UpdateLocation();

		if( TutorialContext::i()->IsFirstFinishTutorial() && TutorialContext::i()->WasTutorial())
		{
			//GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("Ʃ�丮�� ��� �ϷḦ ���ϵ帳�ϴ�. AI���� �Ͻðڽ��ϱ�?"), nullptr, MAKE_CALLBACK(CbAIMode), this);
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_COMPLETE_TUTORIAL_TO_AI"), nullptr, MAKE_CALLBACK(CbAIMode), this);
		}
	}

	// ������ üũ
	if( UserInfoContext::i()->IsNewRepair() )
		GameUI::CheckWeaponEndurance();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}

	// �ڱ��ڽ��� Ŭ���� �� ��Ż�� �뺴(�г�Ƽ ����� �뺴)�̸� Ŭ��ä�� �Ϲݷκ񿡼� ���� ����Ǿ�� ��

	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) &&
		ChannelContext::i()->GetBattleChannel())
	{
		if (ClanGameContext::i()->getMyMercenaryState() == true &&
			ClanGameContext::i()->IsMyMercDepartured())
		{
			if (UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_MERC_BATTLE_ESCAPE"), nullptr, MAKE_CALLBACK(CbMercPenalty), this);
			}
			else
			{
				GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_MERC_BATTLE_ESCAPE"), nullptr, nullptr, this);
			}

		}
	}
}

/*virtual*/ bool UIPhaseLobby::OnExitStart( void)
{
	if( UIPhaseBase::OnExitStart() == false)
		return false;

	if( m_pKickedMassageBox)
	{
		GameUI::CloseMsgBox( m_pKickedMassageBox);
		m_pKickedMassageBox = nullptr;
	}

	if( m_pWaitRecevingRoomList != nullptr)
	{
		//GameUI::DisableLoadingIcon( m_pWaitRecevingRoomList);
		m_pWaitRecevingRoomList = nullptr;
	}

	m_bCheckInvite = false;

	DisableRoomTooltip();

	//slide �ൿ�� �����Ѵ�.
	// ���� �� �Լ��� ȣ��Ǹ� ������ �޸� ������ �ȴ�. �������� ȣ��Ǿ���!!!!
	m_slide.ForceEnd();

	return true;
}

/*virtual*/ void UIPhaseLobby::OnGotoBackPhase( void)
{
	m_bAllowRefresh = false;
	m_bRefreshButtonActive = false;

	i3UI::setDisableUIInput( true);
	
	GameEventSender::i()->SetEventOnce( EVENT_LEAVE_LOBBY);
	m_bMyPhase = false;
}

/*virtual*/ bool UIPhaseLobby::OnDblClick( i3UIControl * pControl)
{
	if( UIPhaseBase::OnDblClick( pControl) == false)
		return false;

	if( i3::same_of<i3UIListView_Item*>(pControl))
		PlaySound( UISND_CLICK);

	return true;
}

void UIPhaseLobby::ClickButton( INT32 BtnIdx)
{
	if( BtnIdx > -1 && BtnIdx < UI_LOBBY_BUTTON_COUNT)
	{
		switch(BtnIdx)
		{
		case ULB_QUICKJOIN :	SendEventQuickJoin();	break;
		//case ULB_CREATEROOM :	GetMainFrame()->TogglePopup( UPW_CREATEROOM);	break;
		case ULB_CREATEROOM :
			{
				if(	
					ClanGameContext::i()->getMyMercenaryState() == true &&
					ChannelContext::i()->GetBattleChannel()
				  )
				{
					GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_MERC_JOIN_NORMAL_MATCH_ROOM"));
					break;
				}

				bool isLobby = true;
				GetMainFrame()->TogglePopup( UPW_CREATEROOM_EX, (void*)&isLobby );	
			}			
			break;
		case ULB_ENTERROOM :	EnterRoom( -1);			break;
		case ULB_TUTORIAL :		EnterTutorial();		break;
		}
	}
}

void UIPhaseLobby::EnterAIMode(void)
{
	// ����� �̸��� �⺻ �����ϴ� �����̶�� ���͸����� �ʴ´�. �⺻ �������� ���Ϳ� �ɷ� �� ������ �ȵǴ� ������ �ذ���. komet
	const i3::rc_wstring& wstrDefaultRoomName = GAME_RCSTRING("STBL_IDX_ROOMNAME_1");	// 1 ~ 4

	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;
	MyRoomInfoContext::i()->getMyRoomInfo(&MyRoomInfo);
	MyRoomInfoContext::i()->getMyRoomInfoAdd(&MyRoomInfoAdd);
	
	SetInputDisable( false);

	// ����� ������ ���մϴ�!
	
	// 1.���̸�
	i3::safe_string_copy(MyRoomInfo._Title, wstrDefaultRoomName, NET_ROOM_NAME_SIZE);

	// 2.���Ӹ��
	{	
		MyRoomInfo._StageID = CStageInfoUtil::GetRuleData((INT32)0)->MakeDefaultStageID();
	}

	// 3.��й� ����
	{
		MyRoomInfo._InfoFlag = 0;
	}

	// �ο���
	{
		MyRoomInfo._PersonPlayerMax = 16;
	}

	// 4. ���� ��� ���� �޴�
	{			
		//// ���� ����	
		m_LockWeapon = 0;

		UNSETBIT(m_LockWeapon, LOCK_PRIMARY);
		UNSETBIT(m_LockWeapon, LOCK_SECONDARY);
		UNSETBIT(m_LockWeapon, LOCK_RPG7);
		UNSETBIT(m_LockWeapon, LOCK_THROW);
		SETBIT( m_LockWeapon, LOCK_PRIMARY); 
		SETBIT( m_LockWeapon, LOCK_SECONDARY); 
		SETBIT( m_LockWeapon, LOCK_THROW);
		UNSETBIT(m_LockWeapon, LOCK_RPG7);
		SETBIT( m_LockWeapon, LOCK_MELEE);

		MyRoomInfo._WeaponFlag = m_LockWeapon;

		UNSETBIT(m_LockObserver, LOCK_OBSERVER_CHASE);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_FREE);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_1PVONLY);

		// ��������
		m_LockInterEnter = 0;
		m_LockTeamBalance = 1;
		m_SubType = BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;

		MyRoomInfoAdd._SubType = m_SubType;
		MyRoomInfoAdd._LockInterEnter = m_LockInterEnter;
		MyRoomInfoAdd._LockObserver = m_LockObserver;
		MyRoomInfoAdd._LockTeamBalance = m_LockTeamBalance;
	}

	// AI������ ���� �� ��� 0���� �缳���Ǳ⶧���� �����մϴ�.
	MyRoomInfo._WeaponFlag = 0x0F;

	MyRoomInfoAdd._NowRoundCount		= 0;
	MyRoomInfoAdd._NowIngTime			= 0;
	MyRoomInfoAdd._LockInterEnter		= 0x00;	// �⺻ ������
	MyRoomInfoAdd._LockTeamBalance	= 2;	// �⺻ �뷱�� (2��°)

	AIModeContext::i()->EnemyCount = 8;
	AIModeContext::i()->StartLevel = 1;
	AIModeContext::i()->CurrentLevel = 1;

	MyRoomInfoContext::i()->setMyRoomInfo(&MyRoomInfo);
	MyRoomInfoContext::i()->setMyRoomInfoAdd(&MyRoomInfoAdd);

	GameEventSender::i()->SetEventOnce( EVENT_CREATE_ROOM);

	i3UI::setDisableUIInput( true);

}

void UIPhaseLobby::EnterTutorial( void)
{
	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;

	if( m_bWaitingFromServer == false)
	{
		MyRoomInfoContext::i()->getMyRoomInfo(&MyRoomInfo);
		MyRoomInfoContext::i()->getMyRoomInfoAdd(&MyRoomInfoAdd);


		i3::safe_string_copy(MyRoomInfo._Title, _TT("Tutorial"), NET_ROOM_NAME_SIZE );/*�Ʒ���*/

		MyRoomInfo._StageID			= StageID::MakeStageID(STAGE_MODE_TUTORIAL, WEAPON_ALL, AI_USE_NO, false, false, 0, STAGE_UID_TUTORIAL);;
		MyRoomInfo._WeaponFlag		= 0x0F;	

		// Add
		MyRoomInfoAdd._SubType				= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10;
		MyRoomInfoAdd._NowRoundCount		= 0;
		MyRoomInfoAdd._NowIngTime			= 0;
		MyRoomInfoAdd._LockInterEnter		= 0x01;	// �⺻ ������
		MyRoomInfoAdd._LockTeamBalance		= 0x00;	// �⺻ �뷱�� ����

		MyRoomInfoContext::i()->setMyRoomInfo(&MyRoomInfo);
		MyRoomInfoContext::i()->setMyRoomInfoAdd(&MyRoomInfoAdd);

		GameEventSender::i()->SetEventOnce( EVENT_CREATE_ROOM);

		i3UI::setDisableUIInput( false);

		m_bWaitingFromServer = true;
	}
}

void UIPhaseLobby::OnCreateRoomCancel( void)
{

	m_bEnableCreateRoomMsgBox = false;

	// CreateRoom Window�� �ݴ´�.
	//GetMainFrame()->ClosePopup( UPW_CREATEROOM );
	GetMainFrame()->ClosePopup( UPW_CREATEROOM_EX );
}

// �濡 ����
void UIPhaseLobby::EnterRoom( INT32 idxRoom)
{
	if( ((m_SelectedElement == idxRoom) && (idxRoom != -1)) ||
		((idxRoom == -1) && (m_SelectedElement != -1)) )
	{
		I3ASSERT( m_SelectedElement < (INT32)m_RoomList.size());

		LOBBY_ROOM_INFO* pLobbyRoom = m_RoomList[m_SelectedElement];
		ROOM_INFO_BASIC* pInfo = (ROOM_INFO_BASIC*) &pLobbyRoom->_RoomInfo;

		// �� ���� ����
		if (pInfo->_PersonNow >= pInfo->_PersonPlayerMax)
		{
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_SLOT_FULL"));
			return;
		}

		// �������ο� ���� ��й�ȣ �Է� �˾� ���. (��, GM , MOD ������ �׳� ���� ����.)
		if ( !UserInfoContext::i()->CheckAuthLevel() &&
			(GET_ROOM_INFO_PRIVATE_MODE( pInfo->_InfoFlag )) )
		{
			QPParam Param;

			Param.wstrMsg			= GAME_RCSTRING("STBL_IDX_EP_LOBBY_JOIN_ROOM_Q");
			Param.LimitCount	= NET_ROOM_PW;
			Param.OnlyNumber	= true;
			Param.ShowPassword	= true;
			Param.Callback		= JoinRoomPasswordCallback;
			Param.pUserContext	= this;

			m_JoinTargetRoom	= *pInfo;

			GetMainFrame()->TogglePopup( UPW_QUERY_PASSWORD, &Param);
		}
		else
		{
			if (
				 ClanGameContext::i()->getMyMercenaryState() == true &&
				 ChannelContext::i()->GetBattleChannel()
			   )
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_MERC_JOIN_NORMAL_MATCH_ROOM"));
				return;
			}

			// ����
			m_bAllowRefresh = false;
			I3TRACE( "--- SetEventOnce(EVENT_JOIN_ROOM)!!\n");
			if( MyRoomInfoContext::i()->JoinRoom(pInfo))
			{
				i3UI::setDisableUIInput( true);

				I3TRACE( "--- setDisableInput -> true\n");
			}
			else
				I3TRACE( "--- can not send event : EVENT_JOIN_ROOM !!\n");
		}
	}
	else
	{
		SelectRoom(idxRoom);
	}
}



void UIPhaseLobby::SendEventQuickJoin( void)
{
	if( GetMainFrame()->IsCreatingNick() == false)
	{
		m_bAllowRefresh = false;

		// Lock�� 2�������� : �����ΰ������� ��Ŭ�� �� ��� �������� 2�� ��ư �̺�Ʈ�� ȣ��Ǳ� ������,
		// ó�� �̺�Ʈ�� ������ ������ �Է��� ������� �մϴ�. �ȱ׷���� ������ Phase ����ȭ�� ������ �� �ֽ��ϴ�.
		if( GetMainFrame()->isLockedInput() == false && LocaleValue::Enable("EnableQuickJoin") == true)
		{
			UIMainFrame* f = g_pFramework->GetUIMainframe();
			I3ASSERT(f);
			f->OpenPopup(UPW_QUICK_JOIN);
		}
	}
}



void UIPhaseLobby::EventItemPopup()
{
	if( UserContext::i()->ReceivedEvent)
	{
		GetMainFrame()->TogglePopup(UPW_EVENT,nullptr);
	}
	else
	{
		ShowPcCafePopup();
	}
	

	m_bEvnetNotice = true;


}
// PcCafe�� Popup�� ����.
void UIPhaseLobby::ShowPcCafePopup(void)
{
	USER_INFO_BASIC	MyUserInfoBasic;

	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	// �ڱ������� �������̸� ó���ؼ� �ȵȴ�.
	if( i3::generic_string_size(MyUserInfoBasic.m_strNick) == 0)
		return;

	// PC������ ������ ���� �˾��� ����Ѵ�.
	if (MyUserInfoBasic.m_ui8PCCafe != PC_NOT_CAFE)
	{
		// Message�� UI�� ����. MustBeFix
		GetMainFrame()->OpenPopup(UPW_PCCAFE, &MyUserInfoBasic);
	}
	//else
	//{
	//	ShowNoticePopup();
	//}

	m_bPcCafeNotice = true;
}

// ���� ���� â�� ����.
void UIPhaseLobby::ShowNoticePopup(void)
{
	if( true )
	{
		if(m_bNickNameCreate)
			TutorialPopup();
	}


	//INT32 nListCount = g_pFramework->getNoticeListCount();
	//i3::stack_wstring wstrTemp;

	//i3::rc_wstring	wstrMyNickName = UserInfoContext::i()->GetMyNickName();		// TODO : �ƹ����� �� �Լ��� ���α����� ��� ���� �ܼ�ȭ�� �Ǿ���Ұ���..(2014.07.21.����)

	if( g_pFramework->getNoticeActivation())
	{
		// �������� ���� ������ ����մϴ�. 16.01.12
		g_pFramework->setNoticeActivation(false);

		i3::stack_wstring wstrText;
		i3::generic_string_cat(wstrText, g_pFramework->getChatNotice());

		GameUI::MsgBox(MSG_TYPE_GAME_OK, i3::rc_wstring(wstrText));

		// 160112 ������ �ҽ��ڵ�.
		//// �ʱ� 1ȸ�� �����Ű���� üũ�մϴ�! (���� �� üũ)
		//CFileFind finder;
		//i3FileStream fileForCheck;

		//bool bWorking = finder.FindFile("Script/Etc/NoticeCheck.log");	
		//finder.Close();

		//if( bWorking)
		//{
		//	::DeleteFile("Script/Etc/NoticeCheck.log");
		//	fileForCheck.Close();

		//	if( fileForCheck.Create( "Script/Etc/NoticeCheck.log", STREAM_WRITE))
		//	{
		//		// BOM �߰��ϰ� �����ڵ� �������� ����..(2014.07.21.����)
		//		WCHAR16 bom = 0xFEFF;
		//		fileForCheck.Write( &bom, sizeof( WCHAR16));
		//		fileForCheck.SetPosition( 0, STREAM_END);

		//		i3::sprintf( wstrTemp, L"%s_%s", g_pFramework->getNoticeVersion(), wstrMyNickName);
		//		fileForCheck.WriteLineW( wstrTemp.c_str() );
		//	}
		//}
		//else
		//{
		//	if( fileForCheck.Create( "Script/Etc/NoticeCheck.log", STREAM_WRITE))
		//	{
		//		// BOM �߰��ϰ� �����ڵ� �������� ����..(2014.07.21.����)
		//		WCHAR16 bom = 0xFEFF;
		//		fileForCheck.Write( &bom, sizeof( WCHAR16));
		//		fileForCheck.SetPosition( 0, STREAM_END);

		//		i3::sprintf( wstrTemp, L"%s_%s", g_pFramework->getNoticeVersion(), wstrMyNickName);
		//		fileForCheck.WriteLineW( wstrTemp.c_str() );
		//	}
		//}				

		//fileForCheck.Close();

		//g_pFramework->setNoticeActivation( false);

		//if( nListCount > 0)
		//{
		//	INT32 i = 0;
		//	i3::stack_wstring wstrText;

		//	for( i=0; i<nListCount; i++)
		//	{
		//		i3::generic_string_cat(wstrText, g_pFramework->getNoticeList(i));
		//	}

		//	GameUI::MsgBox( MSG_TYPE_GAME_OK, i3::rc_wstring(wstrText) );
		//}
	}

	m_bNickNameCreate = false;
}

// �⼮ üũ �߰�
void UIPhaseLobby::ShowAttendancePopup(void)
{
	// �⼮üũ�� ����� 2���� ���̳�, ���� Ȯ�� ���ɼ��� ����Ͽ�
	// ���� Ȱ��ȭ�Ǿ��ִ�(ȭ�鿡�� ��������) �⼮üũ�� ���ʷ� ���
	// ATTENDANCE_TYPE �� ���ǵ� ������� ����ϸ�, ����� �� �ִ� ���°� �ƴ� ���� �ǳʶݴϴ�
	// ATTENDANCE_TYPE �� ���ο� Ÿ���� �̺�Ʈ�� �߰��Ǿ��� ��, �Ʒ� switch-case ���� �߰����ּ���

	for (int i = AttendanceContext::i()->GetCurrrentType(); i < ATTENDANCE_TYPE_MAX; ++i)
	{
		const ATTENDANCE_USER * pUser = AttendanceContext::i()->GetAttendanceUser((ATTENDANCE_TYPE)i);
		if (pUser->m_ui8EventState == ATTENDANCE_STATE_RUNNING)
		{
			switch (i)
			{
			case ATTENDANCE_TYPE_DORMANT:
				GetMainFrame()->OpenPopup(UPW_DORMANT_EVENT);
				break;
			case ATTENDANCE_TYPE_NORMAL:
				GetMainFrame()->OpenPopup(UPW_MONTH_ATTENDANCE);
				break;
			default:
				break;
			}
			
			break;
		}
	}
}

//------------------------------------------------------------------------------//
//								Error Handler									//
//------------------------------------------------------------------------------//

void UIPhaseLobby::EhWrongQuickJoin(void)
{
	// �� ���� Popup�� ����.
	bool isLobby = true;
#if 1
	GetMainFrame()->TogglePopup( UPW_CREATEROOM_EX, (void*) &isLobby );
#else
	GetMainFrame()->TogglePopup( UPW_SETUP_QUICK_ENTRANCE, (void*)&isLobby );
#endif
}

void UIPhaseLobby::EhSlotFull(void)
{
	// TTP # 292 - ���Ծ����� ��ȣƲ������ �켱�Ǿ��Ѵٰ� �մϴ�.
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_SLOT_FULL"));

	m_bCheckInvite = true;
}

void UIPhaseLobby::EhKickedByVote()
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_FAIL_START_C"));

	m_bCheckInvite = true;
}

void UIPhaseLobby::EhWrongPW(void)
{
	QPParam	Param;

	Param.wstrMsg		= GAME_RCSTRING("STBL_IDX_EP_LOBBY_WRONG_PW_Q");
	Param.LimitCount	= NET_ROOM_PW;
	Param.OnlyNumber	= true;
	Param.ShowPassword	= true;
	Param.Callback		= JoinRoomPasswordCallback;
	Param.pUserContext	= this;

	GetMainFrame()->TogglePopup(UPW_QUERY_PASSWORD, &Param);
}

void UIPhaseLobby::EhNotExist(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NOT_EXIST_ROOM"));

	m_SelectedElement = -1;

	m_bCheckInvite = true;
}

void UIPhaseLobby::EhLimitInterenter(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_LOBBY_LIMIT_ENTER_ROOM"));/*������ ���ѵ� ���Դϴ�.*/
	m_bCheckInvite = true;
}

void UIPhaseLobby::EhLimitLever(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_LOBBY_LIMIT_GRADE"));/*����� ���ѵ� ���Դϴ�.*/
	m_bCheckInvite = true;
}

void UIPhaseLobby::EhLimitTraining(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_LOBBY_WARING_TRANING_ENTER"));/*�Ʒ����� ���常 ������ �� �ֽ��ϴ�.*/
	m_bCheckInvite = true;
}

void UIPhaseLobby::EhLimitMERC(void)
{
	GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_POPUP_CANNOT_MERC_VISIT"));/*�뺴�� �Ϲ� Ŭ���� �� ���� �� ������ �Ұ����մϴ�.*/
	m_bCheckInvite = true;
}

void UIPhaseLobby::EhStageError(INT32 Event,INT32 Arg)
{	
	i3::rc_wstring wstr_bug;
	i3::sprintf( wstr_bug, GAME_RCSTRING("STR_TBL_GUI_LOBBY_DEBUG_POPUP_TEXT"), Event, Arg);/*����� �˾�: Event(%d), Arg(0x%x)*/

	GameUI::MsgBox(MSG_TYPE_ERROR, wstr_bug);
	m_SelectedElement = -1;
}



//------------------------------------------------------------------------------//

void UIPhaseLobby::CbSameNick(void* pThis,INT32 nParam)
{
	g_pFramework->GetUIMainframe()->OpenPopup( UPW_CREATENICKNAME);
}

void UIPhaseLobby::CbBeInvited(void* pThis,INT32 nParam)
{
	INT32 i;

	UIPhaseLobby* pLobby = (UIPhaseLobby*)pThis;

	if (MBR_OK == nParam)
	{
		// �ٸ� �˾��� ����.!!
		pLobby->GetMainFrame()->TogglePopup( UPW_NULL);

		// �游��� â �ݱ�
		//pLobby->OnCreateRoomCancel();

		// �ʴ� ����
		for( i = 0; i < LobbyContext::i()->getLobbyRoomCount(); i++)
		{
			const ROOM_INFO_BASIC* pInfo = LobbyContext::i()->getRoomInfo(i);
			if ( ! pInfo )
				continue;

			// �� �ε����� ã�´�
			if (pInfo->_Idx == pLobby->m_InviteInfo._idxRoom)
			{
				
				MyRoomInfoContext::i()->SetMyRoomPW(reinterpret_cast<const char*>(pLobby->m_InviteInfo._pw), NET_ROOM_PW);

				MyRoomInfoContext::i()->JoinRoom(pInfo);
				return;
			}
		}

		// ã�� �� ���ٸ� �����˾�
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NOT_EXIST_ROOM"));
	}
	
	// �ʴ� �޽��� �˻� �ٽ� ����
	pLobby->m_bCheckInvite = true;
}

void UIPhaseLobby::CbSuppliedConfirm(void* pThis,INT32 nParam)
{
	// ������ȭ�� ����
	((UIPhaseLobby*)pThis)->GetMainFrame()->EnableFloatWin( UFW_MINIPROFILE);
}

void UIPhaseLobby::CbEventCardSetConfirm(void* pThis,INT32 nParam)
{
	// �̺�Ʈ ī��� �˾� ��� ��, PC�� �˾� ���
	((UIPhaseLobby*)pThis)->ShowPcCafePopup();
}
// �г��� ���� �� Ʃ�丮�� ���� ������ �������� ���� �˾�â
void UIPhaseLobby::TutorialPopup( void)
{
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_GUI_BASE_NEW_ENTRY_TUTORIAL_MODE"),
		&GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_TEXT"), MAKE_CALLBACK(CbTutorialMode), this);
}

void UIPhaseLobby::CbTutorialMode(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
		((UIPhaseLobby*)pThis)->EnterTutorial();
}

void UIPhaseLobby::CbAIMode(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
		((UIPhaseLobby*)pThis)->EnterAIMode();
	//if( nParam == MBR_OK)
	//	EnterAIMode();
}

void UIPhaseLobby::CbKickedByVote( void * pThis, INT32 nParam)
{
	USER_INFO_BASIC basic;
	UserInfoContext::i()->GetMyUserInfoBasic(&basic);

	i3::rc_wstring wstrMsg;
	i3::sprintf(wstrMsg, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_KICKED_BY_VOTE"), basic.m_strNick);		// ����ȯ (2014.07.21.����)

	g_pFramework->getChatBox()->PutSystemChatting(wstrMsg);

	// �˾��� ������ �����Ǳ⿡ �ʱ�ȭ����� �Ѵ�.
	((UIPhaseLobby*)pThis)->m_pKickedMassageBox = nullptr;
}

void UIPhaseLobby::CbMercPenalty(void* pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		GameEventSender::i()->SetEvent(EVENT_LEAVE_LOBBY);
		((UIPhaseLobby*)pThis)->m_bMyPhase = false;
	}
}

// Key : �⼮ üũ �߰�
void UIPhaseLobby::CbShowAttendancePopup(void *pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		AttendanceContext::i()->SetNextType();
	}
	((UIPhaseLobby*)pThis)->ShowAttendancePopup();
}

void UIPhaseLobby::CbOnGotoBackPhase(void *pThis, INT32 nParam)
{
	((UIPhaseLobby*)pThis)->OnGotoBackPhase();
}

void UIPhaseLobby::CbLeaveTeam(void* pThis, INT32 nParam)
{
	// �뺴 ������
	if(nParam == MB_OK)
	{
		ClanGameContext::i()->setClanBattleLobbyEnable(false);
	}
	else
	{
		GameEventSender::i()->SetEvent(EVENT_LEAVE_LOBBY);
		ClanGameContext::i()->setClanBattleLobbyEnable(false);
	}
}

void UIPhaseLobby::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	m_pBtnTutorial = (i3UIButton*) pScene->FindChildByName( "Btn_Tutorial");
	I3ASSERT( m_pBtnTutorial != nullptr);

	m_pScrollRoomList = i3::same_cast<i3UIScrollBar*>(pScene->FindChildByName("ScrollBar_RL"));
	I3ASSERT( m_pScrollRoomList != nullptr );

	m_pBtnTeamLIst = i3::same_cast<i3UIButton*>(pScene->FindChildByName("Btn_Squad"));
	I3ASSERT( m_pBtnTeamLIst != nullptr);

	m_bTutorialEnable = true;
}

void UIPhaseLobby::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPhaseLobby::ReceiveGameEvent_Ready_Battle( INT32 arg, const i3::user_data&)
{
	// Key : User Guide System
	{
		UserGuideMgr::i()->DisableUserGuideAll();
	}

	OnExitEnd();
	
	m_bMyPhase = false;
	g_pFramework->JumpToBattle();
}

void UIPhaseLobby::ReceiveGameEvent_Join_Quick( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{	
		GetMainFrame()->MovePhase( UIPHASE_READY_ROOM);
	}
	else
	{
		m_bMyPhase = true;
		m_bAllowRefresh = true;
		i3UI::setDisableUIInput(false);
		GetMainFrame()->SetLockInput(false);

		if( EV_IS_ERROR(arg,EVENT_ERROR_FAIL))
			EhWrongQuickJoin();
		else
			EhStageError( EVENT_JOIN_QUICK,arg);
	}
}

void UIPhaseLobby::ReceiveGameEvent_Create_Room( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		// ���� �����Ǿ����� AutoRefresh�� ����.
		m_bAllowRefresh = false;
		m_bEnableCreateRoomMsgBox = false;

		// �������� ������ PersonMax ���� �޾�, ������ ������ �ݾ��ش�.
		{
			ROOM_INFO_BASIC RoomInfo;
			MyRoomInfoContext::i()->getMyRoomInfo(&RoomInfo);
			for( INT32 i = 0; i < USER_SLOT_COUNT; ++i)
			{
				if( i >= RoomInfo._PersonPlayerMax)
					BattleSlotContext::i()->setSlotState( i, SLOT_STATE_CLOSE);
			}
		}

		GetMainFrame()->MovePhase( UIPHASE_READY_ROOM);			
	}
	else
	{
		m_bMyPhase = true;
		m_bAllowRefresh = true;
		i3UI::setDisableUIInput(false);
		EhStageError( EVENT_CREATE_ROOM, arg);
	}

	m_bWaitingFromServer = false;
}

void UIPhaseLobby::ReceiveGameEvent_Join_Room( INT32 arg, const i3::user_data&)
{
	DisableRoomTooltip();

	//g_pFramework->GetUIMainframe()->SetLockInput(false);	// hansoft 11845. ��������� ������ ���� ���� �� PS_ENTRANCE_END �������� �ʱ�ȭ �ǹǷ� �ּ� ó����.
	//i3UI::setDisableUIInput(false);

	CCommunityContext::i()->CompleteMovingToUser();

	// ������������ Ÿ ä�η� ������ ��� ���� ������ �����Ѵ�.
	if (MyRoomInfoContext::i()->IsQuickJoin())
	{
		g_pServerInfo->SetCurGameServerType(static_cast<SChannelType>(QuickJoinContext::i()->getChannelType()));
		g_pServerInfo->SetCurChannelNumber(QuickJoinContext::i()->getChannelIndex());
		g_pServerInfo->SetGameServer(QuickJoinContext::i()->getServerIndex(), S_CHANNEL_TYPE_ALL);
		GetMainFrame()->UpdateLocation();
	}

	if( EV_SUCCESSED(arg))
	{
		GetMainFrame()->MovePhase( UIPHASE_READY_ROOM);
	}
	else
	{
		// ���忡 ���� �� ��� Phase �̵��� �ƴ϶� �ʱ�ȭ�� ���� �ʽ��ϴ�.
		// �ٽ� �� �� �ʴ밡 �߱� ���� �ٽ� �����մϴ�. Jinsik.kim(2014.12.9)
		m_bWaitingFromServer = false;
		m_bAllowRefresh = true;

		g_pFramework->GetUIMainframe()->SetLockInput(false);	// hansoft 11845. ��������� ������ ���� ���� �� SetEventOnce() ���� �����Ͽ� �ٽ� ��û �������.
		i3UI::setDisableUIInput(false);
		m_bMyPhase = true;
		if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL))			EhSlotFull();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_ROOM_JOIN_VOTE))					EhKickedByVote();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM))	EhNotExist();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_WRONG_PASSWORD))	EhWrongPW();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_INTERENTER))	EhLimitInterenter();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_LEVEL))		EhLimitLever();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_TRAINING))		EhLimitTraining();
		else if (EV_IS_ERROR(arg, EVENT_ERROR_CLAN_WAR_MERCENAY))				EhLimitMERC();
		else
			EhStageError( EVENT_JOIN_ROOM,arg);
	}
}

void UIPhaseLobby::ReceiveGameEvent_Leave_Lobby( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		GetMainFrame()->MovePhase( UIPHASE_SERVER_CHANNEL_LIST);
		m_PreClanMemberLevel = (CLAN_MEMBER_LEVEL_TYPE)UserInfoContext::i()->GetMyClanState();

		// �强�� ������ ���޿��� Ȯ��
		if(UserInfoContext::i()->IsGeneralRecvData())
		{
			if(UserInfoContext::i()->IsGeneralMsgBox())
				return;
			else
				UserInfoContext::i()->SetGeneralMsgBox();

			INVEN_BUFFER itemInfo = UserInfoContext::i()->GetGeneralRankInfo();

			// ���� üũ
			if(usf::GetItemStatus(itemInfo._ui32ItemID, itemInfo._TItemWareDBIdx) == ITEM_STATUS_EQUIPPED)
			{
				g_pFramework->EquipWeaponToMyInven(itemInfo._ui32ItemID, true);
			}

			GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PROMOTION4"));
		}
	}
	else
	{
		CCommunityContext::i()->CompleteMovingToUser();

		m_bMyPhase = true;
		m_bAllowRefresh = true;
		GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_FAIL_REFRESH_C"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
	}
}

void UIPhaseLobby::ReceiveGameEvent_Get_RoomAddInfo( INT32 arg, const i3::user_data&)
{
	m_bQueRoomInfo = true;
}

void UIPhaseLobby::ReceiveGameEvent_DestroyedNetwork( INT32 arg, const i3::user_data&)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DESTROYED_NETWORK_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseLobby::ReceiveGameEvent_Update_UserList( INT32 arg, const i3::user_data&)
{
	// �� ����Ʈ�� ���� ����Ʈ ������ ���ÿ� �´�.
	_UpdateRoomList();

	// �� ����Ʈ �����̵Ǹ� AddInfo�� �ٽ� �����ؾߵȴ�.
	RefreshSelectedRoom();

	if( m_is_sort_room_list == false )
		_SortRoomList();

	// Key : User Guide System
	{
			UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}


void UIPhaseLobby::ReceiveGameEvent_Ready_Box_Message( INT32 arg, const i3::user_data&)
{
	//���� ���� ä�� �޼����� ����.
	_ApplyMsgReadyBox();
}

void UIPhaseLobby::ReceiveGameEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

void UIPhaseLobby::ReceiveGameEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

void UIPhaseLobby::ReceiveGameEvent_Attendance_Clear(INT32 arg, const i3::user_data&)
{
	i3::rc_wstring wstrTemp;
	MSG_TYPE eType = MSG_TYPE_ERROR;

	switch (arg)
	{
	case EVENT_ERROR_ATTENDANCE_SUCCESS:

		if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
			wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_SUCCESS");
		else
			return;
		
		eType = MSG_TYPE_GAME_OK;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_SUCCESS");
		break;
	case EVENT_ERROR_ATTENDANCE_PARAMETER_USER:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_PARAMETER_USER");
		break;
	case EVENT_ERROR_ATTENDANCE_NOT_ENOUGH:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_NOT_ENOUGH");
		break;
	case EVENT_ERROR_ATTENDANCE_ALREADY_COMPLETED:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_ALREADY_COMPLETED");
		break;
	case EVENT_ERROR_ATTENDANCE_WRONG_VERSION:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_WRONG_VERSION");
		break;
	case EVENT_ERROR_ATTENDANCE_UNKWON:
	default:
		eType = MSG_TYPE_ERROR;
		wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_UNKNOWN");
		break;
	}

	// ���� & ���� �̺�Ʈ�� ���� �⼮üũ �˾��� Close �� �� ���� �̺�Ʈ �˾��� �˻�������.
	// �Ⱓ�� 1ȸ �̺�Ʈ �˾��� ���� �޼����ڽ��� ���� �� ���� �̺�Ʈ�˾��� �˻��Ѵ�.
	// �ش� �˾� ������ ���� MessageBox CallBack ������ ���� ���� �б⸦ ����Ѵ�.
	GameUI::MsgBox(eType, wstrTemp, nullptr, MAKE_CALLBACK(CbShowAttendancePopup), this);
}

void UIPhaseLobby::AttendanceOpenPopup(INT32 EvtType)
{
	if ( AttendanceContext::i()->IsAttendanceEvent((ATTENDANCE_TYPE)EvtType) &&
		 AttendanceContext::i()->IsEventTarget((ATTENDANCE_TYPE)EvtType)		)
	{
		switch (EvtType)
		{
		case ATTENDANCE_TYPE_DORMANT:
			GetMainFrame()->OpenPopup(UPW_DORMANT_EVENT);
			break;
		case ATTENDANCE_TYPE_NORMAL:
			GetMainFrame()->OpenPopup(UPW_MONTH_ATTENDANCE);
			break;
		default:
			I3PRINTLOG(I3LOG_NOTICE, "Unregist Attendance Event. %d", EvtType);
			break;
		}
	}
	else
	{
		i3::rc_wstring wstrString;
		
		// �̺�Ʈ�� ���� ���� �̺�Ʈ ����� �ƴ� ���. �̺�Ʈ ���� ������ ���� �˻��ؾ� �մϴ� 
		if (!AttendanceContext::i()->IsAttendanceEvent((ATTENDANCE_TYPE)EvtType))
		{
			switch (EvtType)
			{
			case ATTENDANCE_TYPE_DORMANT:
				wstrString = GAME_RCSTRING("STR_TBL_UI_DORMANT_NOT_ATTENDANCE");
			break;
			case ATTENDANCE_TYPE_NORMAL:
				wstrString = GAME_STRING("STR_TBL_UI_ATTENDANCE_NOT_ATTENDANCE");
				break;
			default:
				I3PRINTLOG(I3LOG_NOTICE, "Unregist Attendance Event. %d", EvtType);
				break;
			}
		}
		else if (!AttendanceContext::i()->IsEventTarget((ATTENDANCE_TYPE)EvtType))
		{
			switch (EvtType)
			{
			case ATTENDANCE_TYPE_DORMANT:
				wstrString = GAME_RCSTRING("STR_TBL_UI_DORMANT_NOTUSER_ATTENDANCE");
				break;
			default:
				I3PRINTLOG(I3LOG_NOTICE, "Unregist Attendance Event. %d", EvtType);
				break;
			}
		}

		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrString, &GAME_RCSTRING("STR_TBL_GUI_LOBBY_NOTICE_TEXT"));
	}
}
