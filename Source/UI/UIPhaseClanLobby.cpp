#include "pch.h"
#include "UIPhaseClanLobby.h"
#include "UIMainFrame.h"
#include "UIFloatChat.h"

#include "../CommunityContext.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "ClanGameContext.h"
#include "ClanSeasonContext.h"
#include "MedalContext.h"
#include "ChannelContext.h"

#include "UIFloatMercenaryList.h"


#include "UIUtil.h"
#include "UIClanFunction.h"

#include "UISignal.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"

UIMatchSquadMemberSlot::UIMatchSquadMemberSlot(void)
{
	Reset();
}

UIMatchSquadMemberSlot::~UIMatchSquadMemberSlot(void)
{
	Reset();
}

void UIMatchSquadMemberSlot::_SetSelectSlot(bool is_select )
{
	if( is_select )
		m_pSlotBG->addState( I3UI_STATE_SELECTED );
	else						
		m_pSlotBG->removeState( I3UI_STATE_SELECTED );

	m_pSlotBG->setModifyState();
}

bool UIMatchSquadMemberSlot::InitSlot(i3UIFrameWnd * pWnd, const INT32 idx)
{
	Reset();

	if( pWnd == nullptr)
		return false;

	char szSlotBGCtrl[ MAX_NAME ]	= {0,};
	char szRankCtrl[ MAX_NAME ]		= {0,};
	char szHostMarkCtrl[ MAX_NAME ] = {0,};
	char szNickCtl [ MAX_NAME ]		= {0,};

	i3::snprintf(szSlotBGCtrl,		MAX_NAME, "Btn_SlotBG%d",		idx);
	i3::snprintf(szRankCtrl,		MAX_NAME, "Ico_Rank%d",			idx);
	i3::snprintf(szHostMarkCtrl,	MAX_NAME, "ImgBox_Host%d",		idx);
	i3::snprintf(szNickCtl,			MAX_NAME, "StaticText_Nick%d",	idx);

	m_pSlotBG	= static_cast<i3UIButton *>( pWnd->FindChildByName(szSlotBGCtrl) );
	m_pRank		= static_cast<i3UIButtonImageSet *>( pWnd->FindChildByName(szRankCtrl) );
	m_pHostMark = static_cast<i3UIImageBox *>( pWnd->FindChildByName(szHostMarkCtrl) );
	m_pNickName = static_cast<i3UIStaticText *>( pWnd->FindChildByName(szNickCtl) );

	I3ASSERT( m_pSlotBG );
	I3ASSERT( m_pRank );
	I3ASSERT( m_pHostMark );
	I3ASSERT( m_pNickName );

	m_pSlotBG->SkipUnselect(true);

	return true;
}

void UIMatchSquadMemberSlot::Reset(void)
{
	m_pSlotBG = nullptr;
	m_pRank = nullptr;
	m_pHostMark = nullptr;
	m_pNickName = nullptr;
}

void UIMatchSquadMemberSlot::SetSlot(const CLAN_MATCH_SQUAD_INFO * pInfo, const CLAN_MATCH_SQUAD_MEMBER & Member, INT32 Rank)
{
	bool is_my_slot = pInfo->isMySlot(Member._i8SlotIdx);
	bool is_leader	= pInfo->isTeamLeader(Member._i8SlotIdx);
	INT32 my_rank	= static_cast<INT32>( static_cast<INT8>(Member.getRank()) );

	if(Member._MemberType == MEMBER_MERCENARY)
	{
		my_rank = Rank;
	}

	_SetSelectSlot(is_my_slot);
	m_pRank->SetShapeIdx(my_rank);
	m_pHostMark->EnableCtrl(is_leader);
	m_pNickName->SetText(Member._wstrNickName);
}

void UIMatchSquadMemberSlot::EnableSlot(bool is_enable)
{
	_SetSelectSlot(false);
	m_pSlotBG->OnActivate(is_enable);
	m_pRank->EnableCtrl(is_enable);
	m_pHostMark->EnableCtrl(is_enable);
	m_pNickName->EnableCtrl(is_enable);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {


	int ClanLobby_CreateTeam( LuaState* L)
	{
		UISignal::ClanLobbyCreateTeam();
	//	UI::call_cb("clan_lobby_create_team");
		return 0;
	}

	int ClanLobby_JoinTeam( LuaState* L)
	{
		INT32 RoomIdx = i3Lua::GetIntegerArg(L, 1, -1);

		UISignal::ClanLobbyJoinTeam(RoomIdx);

	//	UI::call_cb("clan_lobby_join_team", &RoomIdx );
		return 0;
	}

	int ClanLobby_LeaveTeam( LuaState* L)
	{
		UISignal::ClanLobbyLeaveTeam();
	//	UI::call_cb("clan_lobby_leave_team");
		return 0;
	}

	int ClanLobby_QuickMatch( LuaState* L)
	{
		UISignal::ClanLobbyQuickMatch();

	//	UI::call_cb("clan_lobby_quick_match");
		return 0;
	}

	int ClanLobby_QuickMatchCancel(LuaState* L)
	{
		UISignal::ClanLobbyQuickMatchCancel();
		//UI::call_cb("clan_lobby_quick_match_cancel");
		return 0;
	}

	int ClanLobby_EnterRoomList( LuaState* L)
	{
		UISignal::ClanLobbyEnterRoomList();
	//	UI::call_cb("clan_lobby_enter_room_list");

		return 0;
	}

	int ClanLobby_ScrollTeamList( LuaState* L)
	{
		INT32 scroll_pos = i3Lua::GetIntegerArg(L, 1, -1);

		UISignal::ClanLobbyScrollTeamList(scroll_pos);

	//	UI::call_cb("clan_lobby_scroll_team_list", &scroll_pos);
		return 0;
	}

	int ClanLobby_ChangePerson( LuaState* L)
	{
		INT32 match_type = i3Lua::GetIntegerArg(L, 1, -1);
		UISignal::ClanLobbyChangePerson(match_type);

	//	UI::call_cb("clan_lobby_change_person", &match_type);
		return 0;
	}

	int ClanLobby_ChangeOperation( LuaState* L)
	{
		char strBuf[256] = {0,};
		i3Lua::GetStringArg(L, 1, strBuf, 256);

		UISignal::ClanLobbyChangeOperation(strBuf);

	//	UI::call_cb("clan_lobby_change_operation", strBuf);
		return 0;
	}
}

LuaFuncReg ClanLobby_Glue[] =
{
	{ "CreateTeam",			ClanLobby_CreateTeam },
	{ "JoinTeam",			ClanLobby_JoinTeam },
	{ "LeaveTeam",			ClanLobby_LeaveTeam },
	{ "QuickMatch",			ClanLobby_QuickMatch },
	{ "QuickMatchCancel",	ClanLobby_QuickMatchCancel },
	{ "EnterRoomList",		ClanLobby_EnterRoomList },
	{ "ScrollTeamList",		ClanLobby_ScrollTeamList },
	{ "ChangePerson",		ClanLobby_ChangePerson },
	{ "ChangeOperation",	ClanLobby_ChangeOperation },

	{ nullptr,	nullptr }
};

//------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIPhaseClanLobby);//, UIPhaseBase);

UIPhaseClanLobby::UIPhaseClanLobby(void) :	m_fUpdateTeamListTime(0.0f),
											m_pClanMark(nullptr), m_nScroolPage(0),
											m_bInRoomState(false), m_pLoadingIcon(nullptr),
											m_bIsProcessInviteMove(false),
											m_eLoadingType(NONE),
											m_bPushedBackPhase(false)
{
	RegisterGameEventFunc( EVENT_CLAN_WAR_GET_TEAM_LIST,				&UIPhaseClanLobby::ReceiveEvent_ClanWar_GetTeamList);
	RegisterGameEventFunc( EVENT_CLAN_WAR_CREATE_TEAM,					&UIPhaseClanLobby::ReceiveEvent_ClanWar_CreateTeam);
	RegisterGameEventFunc( EVENT_CLAN_WAR_JOIN_TEAM,					&UIPhaseClanLobby::ReceiveEvent_ClanWar_JoinTeam);
	RegisterGameEventFunc( EVENT_CLAN_WAR_LEAVE_TEAM,					&UIPhaseClanLobby::ReceiveEvent_ClanWar_LeaveTeam);
	RegisterGameEventFunc( EVENT_CLAN_WAR_CHANGE_OPERATION,				&UIPhaseClanLobby::ReceiveEvent_ClanWar_ChangeOperation);
	RegisterGameEventFunc( EVENT_CLAN_WAR_CHANGE_MAX_PERSON,			&UIPhaseClanLobby::ReceiveEvent_ClanWar_ChangeMaxPerson);
	RegisterGameEventFunc( EVENT_CLAN_WAR_REGISTER_MATCHMAKING,			&UIPhaseClanLobby::ReceiveEvent_ClanWar_RegisterMatchMaking);
	RegisterGameEventFunc( EVENT_CLAN_WAR_REGISTER_CANCEL_MATCHMAKING,  &UIPhaseClanLobby::ReceiveEvent_ClanWar_RegisterCancelMatchMaking);

	RegisterGameEventFunc( EVENT_CLAN_LIST_CONTEXT,					&UIPhaseClanLobby::ReceiveEvent_Clan_List_Context);
	RegisterGameEventFunc( EVENT_CLAN_DETAIL_INFO,					&UIPhaseClanLobby::ReceiveEvent_Clan_DetailInfo);
	RegisterGameEventFunc( EVENT_CREATE_ROOM,						&UIPhaseClanLobby::ReceiveEvent_Create_Room);
	RegisterGameEventFunc( EVENT_JOIN_ROOM,							&UIPhaseClanLobby::ReceiveEvent_Join_Room);
	RegisterGameEventFunc( EVENT_LEAVE_LOBBY,						&UIPhaseClanLobby::ReceiveEvent_Leave_Lobby);
	RegisterGameEventFunc( EVENT_PLAYTIME_ALARM,					&UIPhaseClanLobby::ReceiveEvent_PlayTimeAlarm);
	RegisterGameEventFunc( EVENT_SHUTDOWNTIME_ALARM,				&UIPhaseClanLobby::ReceiveEvent_ShutDownTimeAlarm);

	/************************************************************************/
	/*                        Lua Glue Callback Func                        */
	/************************************************************************/ 

	m_SigConnection += UISignal::ClanLobbyCreateTeam.connect(i3::bind(&UIPhaseClanLobby::Callback_CreateTeam, this));
	m_SigConnection += UISignal::ClanLobbyJoinTeam.connect(i3::bind(&UIPhaseClanLobby::Callback_JoinTeam, this, i3::_1));
	m_SigConnection += UISignal::ClanLobbyLeaveTeam.connect(i3::bind(&UIPhaseClanLobby::Callback_LeaveTeam, this));
	m_SigConnection += UISignal::ClanLobbyQuickMatch.connect(i3::bind(&UIPhaseClanLobby::Callback_QuickMatch, this));
	m_SigConnection += UISignal::ClanLobbyEnterRoomList.connect(i3::bind(&UIPhaseClanLobby::Callback_EnterRoomList, this));
	m_SigConnection += UISignal::ClanLobbyScrollTeamList.connect(i3::bind(&UIPhaseClanLobby::Callback_ScrollTeamList, this, i3::_1));
	m_SigConnection += UISignal::ClanLobbyChangePerson.connect(i3::bind(&UIPhaseClanLobby::Callback_ChangePerson, this, i3::_1));
	m_SigConnection += UISignal::ClanLobbyChangeOperation.connect(i3::bind(&UIPhaseClanLobby::Callback_ChangeOperation, this, i3::_1));
	m_SigConnection += UISignal::ClanLobbyQuickMatchCancel.connect(i3::bind(&UIPhaseClanLobby::Callback_QuickMatchCancel, this));
	
	m_bProcessLeaveTeam = false; 
	m_bFirstLoad = false;
}

UIPhaseClanLobby::~UIPhaseClanLobby()
{
//	UI::remove_cb(this);
}

/*virtual*/ void UIPhaseClanLobby::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	m_pSquadCombo = (i3UIComboBox*) pScene->FindChildByName( "ComboBox_PersonInfo");
	I3ASSERT( m_pSquadCombo);

	// main window
	m_pFrameWnd = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd");
	I3ASSERT( m_pFrameWnd);

	// member slot init
	{
		char szWndName[ MAX_NAME ];

		for(INT32 idx = 0 ; idx < SQUAD_MEMBER_MAX ; ++idx)
		{
			INT32 real_idx = idx + 1;

			i3::snprintf( szWndName, MAX_NAME, "FrameWnd_MemberSlot%d", real_idx );
			i3UIFrameWnd* pWnd = (i3UIFrameWnd*) pScene->FindChildByName( szWndName );

			bool bRes = m_MemberSlot[idx].InitSlot( pWnd, real_idx);
			I3ASSERT(bRes);
		}
	}

	if(g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		GetSlide()->AddFrameWnd(m_pFrameWnd, UISlide::R_TO_L);

		// 1.5�� �޸� 1.0�� ��ư�� �ؽ�Ʈ �̹����� �������� �ʾ� �ؽ�Ʈ ������ ���־�� ��
		// ��ƿ��� �ϱ⿡�� ���������� ���� �Ѱ��־�� �ϱ� ������ �ʱ�ȭ �� �����մϴ�

		i3UIButtonComposed3* pBtnMatch = (i3UIButtonComposed3*)pScene->FindChildByName("Btn_QuickMatch");
		I3ASSERT( pBtnMatch );
		pBtnMatch->SetText(GAME_RCSTRING("STR_TBL_GUI_LOBBY_QUICK_START_TEXT"));  // ��������

		i3UIButtonComposed3* pBtnList = (i3UIButtonComposed3*)pScene->FindChildByName("Btn_RoomList");
		I3ASSERT( pBtnList );
		pBtnList->SetText("Room List");  //Room List - ��Ʈ�� ����. GD ��û�ؾ���

		i3UIButtonComposed3* pBtnExit = (i3UIButtonComposed3*)pScene->FindChildByName("Btn_LeaveTeam");
		I3ASSERT( pBtnExit );
		pBtnExit->SetText(GAME_RCSTRING("STR_POPUP_LEAVE"));  // ������
		
	}
	else if(g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		// �ε� ������ �ø� FrameWnd // �̷��� �ϸ� �ϴ� ������ ��ġ�� �ߵȴ�.
		m_pFrameWnd = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_CL");
		I3ASSERT(m_pFrameWnd);

		// main window
		//@eunil. Ŭ�� ��ġ����ŷ UI ���� ���� ������ �����ӿ� �ε������� ���� �ʿ䰡 ����.
		//���� ��ġ�� ������...
		i3UIFrameWnd* mainFrame = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd");
		I3ASSERT(mainFrame);

		GetSlide()->AddFrameWnd(mainFrame, UISlide::R_TO_L);
	}
}

/*virtua*/ void UIPhaseClanLobby::_ClearAtUnload(i3UIScene * pScene)
{
	UIPhaseBase::_ClearAtUnload( pScene);

	m_pFrameWnd = nullptr;
	m_pSquadCombo = nullptr;
}

void UIPhaseClanLobby::_InitMyTeam(void)
{
	//	����Ʈ ��û
	g_pFramework->GetClanContext()->MatchTeamList();
	UpdateTeam();
	UpdateMember();
}

void UIPhaseClanLobby::_RefreshTeam(void)
{
	if( m_pLoadingIcon == nullptr && m_pFrameWnd != nullptr && _isSameLoading(NONE) )
	{
		m_pLoadingIcon = GameUI::EnableLoadingIcon( this, m_pFrameWnd);	// Team Refresh Loading
		m_eLoadingType = TEAM_REFRESH;
	}

	if (m_pLoadingIcon && m_pFrameWnd && _isSameLoading(MATCH_MAKING))
	{
		GameUI::DisableLoadingIcon(m_pLoadingIcon);	// Match Making Loading
		m_pLoadingIcon = nullptr;
		m_eLoadingType = NONE;
	}

	//	����Ʈ ��û
	g_pFramework->GetClanContext()->MatchTeamList();
}

void UIPhaseClanLobby::_SetSquadInfo(void)
{
	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

	bool bLeader = ClanGameContext::i()->isMatchTeamLeader();

	LuaState* L = UILua::CallLuaFunction( GetScene(0), "SetSquadInfo");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, pMySquadInfo->getSquadName());	// �д� �̸�
		i3Lua::PushBoolean(L, bLeader);
		_EndLuaFunction(L, 2);
	}
}

void	UIPhaseClanLobby::_SetSquadMemberInfo(void)
{
	// Clanlobby�� ��Ȱ��ȭ�Ǹ� UIó���� ��ŵ
	if (GetScene(0) == nullptr)
		return;

	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

	for( UINT8 idx = 0; idx < SQUAD_MEMBER_MAX ; ++idx)
	{
		const CLAN_MATCH_SQUAD_MEMBER & Member = pMySquadInfo->getMember(idx);

		INT32 rankInfo = 0;
		if(Member._MemberType == MEMBER_MERCENARY)
		{
			ClanGameContext::i()->SetMercRank(idx, Member._ui8MerClass, Member._ui8Rank);
			rankInfo = ClanGameContext::i()->GetMercRank(idx);
		}

		if( Member.isEmptySlot() )
		{
			if( idx < pMySquadInfo->getMaxPerson() )		
			{
				m_MemberSlot[idx].EnableSlot(true);
				m_MemberSlot[idx].SetSlot(pMySquadInfo, CLAN_MATCH_SQUAD_MEMBER(), rankInfo);
			}
			else
				m_MemberSlot[idx].EnableSlot(false);
		}
		else
		{
			m_MemberSlot[Member.getSlotIdx()].EnableSlot(true);
			m_MemberSlot[Member.getSlotIdx()].SetSlot(pMySquadInfo, Member,rankInfo);
		}
	}
}

void	UIPhaseClanLobby::_SetSquadOperation(void)
{
	if( ClanGameContext::i()->isMatchTeamLeader() )
		return;

	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

	LuaState* L = UILua::CallLuaFunction( GetScene(0), "SetOperation");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, pMySquadInfo->getOperation() );	// ������
		_EndLuaFunction(L, 1);
	}
}

void UIPhaseClanLobby::_SetSquadCombo(void)
{
	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

	if( pMySquadInfo->isJoinTeam()  == true)
		m_pSquadCombo->SetCurSel( pMySquadInfo->getMaxPerson() - 4);
	else
		m_pSquadCombo->SetCurSel(-1);
}

void UIPhaseClanLobby::_SetQuickMatchButton(void)
{
	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

	bool is_quick_match = false;
	
	if( ClanGameContext::i()->isMatchTeamLeader() )
	{
		for(UINT8 idx = 0; idx < pMySquadInfo->getMaxPerson(); idx++)
		{
			const CLAN_MATCH_SQUAD_MEMBER & Member = pMySquadInfo->getMember(idx); 

			// �󽽷��� ������ ��ġ ��û �ȵ˴ϴ�.
			if( Member.isEmptySlot() )
			{
				is_quick_match = false;
				break;
			}

			is_quick_match = true;
		}
	}

	LuaState * L = UILua::CallLuaFunction( GetScene(0), "SetQuickMatch");
	if(L != nullptr)
	{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		i3Lua::PushBoolean(L, true);
#else
		i3Lua::PushBoolean(L, is_quick_match);
#endif
		UILua::EndLuaFunction(L, 1);
	}
}

void UIPhaseClanLobby::_EnableQuickMatchButton(bool btnEnable)
{
	LuaState * L = UILua::CallLuaFunction(GetScene(0), "EnbableQuickMatch");
	if (L != nullptr)
	{
		i3Lua::PushBoolean(L, btnEnable);
		UILua::EndLuaFunction(L, 1);
	}
}

void UIPhaseClanLobby::_SetJoinTeamState(void)
{
	LuaState * L; 

	if(g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		L = UILua::CallLuaFunction( GetScene(0), "SetJoinTeamState");
		if(L != nullptr)
		{
			i3Lua::PushBoolean(L, ClanGameContext::i()->getMyMatchSquadInfo()->isJoinTeam() );
			UILua::EndLuaFunction(L, 1);
		}

// 		L = UILua::CallLuaFunction( GetScene(0), "SetBtnLeaveTeam" );
// 		if(L != NULL)
// 		{
// 			i3Lua::PushBoolean(L, ClanGameContext::i()->getMyMatchSquadInfo()->isJoinTeam());
// 			UILua::EndLuaFunction(L, 1);
// 		}
	}
	else
	{
		L = UILua::CallLuaFunction( GetScene(0), "SetJoinTeamState");
		if(L != nullptr)
		{
			i3Lua::PushBoolean(L, !ClanGameContext::i()->getMyMatchSquadInfo()->isJoinTeam() );
			UILua::EndLuaFunction(L, 1);
		}

		L = UILua::CallLuaFunction( GetScene(0), "SetBtnLeaveTeam" );
		if(L != nullptr)
		{
			// 1.0�� Ŭ��ä�� ������ ��ư���� ����ϱ� ������ �ϴ� ������ true..
			i3Lua::PushBoolean(L, true);
			UILua::EndLuaFunction(L, 1);
		}
	}
}

void UIPhaseClanLobby::_SetClanDetailInfo(void)
{
	CLAN_DETAIL_INFO *pClanDetailInfo = ClanGameContext::i()->getMyClanDetailInfo();

	if(pClanDetailInfo == nullptr)
		return;

	_SetClanMark(pClanDetailInfo);

	LuaState * L = UILua::CallLuaFunction( GetScene(0), "SetClanInfo");
	if ( L != nullptr)
	{
		i3::stack_wstring wstr_exp_rate;
		REAL32 exp_rate = 0.0f;
		_SetClanExpRate(wstr_exp_rate, exp_rate, pClanDetailInfo->m_exp);

		i3::wstring wstr_clan_level;
		GetClanFullName2( wstr_clan_level, pClanDetailInfo->m_expRank, pClanDetailInfo->m_personRank);

		i3::wstring wstr_clan_point;
		i3::sprintf(wstr_clan_point, GAME_RCSTRING("STR_CLAN_CLANPOINT_IDX"), (INT32)pClanDetailInfo->GetClanPoint());

		i3Lua::PushStringUTF16To8(L, pClanDetailInfo->m_name);	//1 : Ŭ���̸�		
		i3Lua::PushStringUTF16To8(L, wstr_clan_level);			//2 : Ŭ������̸�(ex..�Ż��д�, �߰ߺд�??)
		i3Lua::PushInteger( L, pClanDetailInfo->m_clanRank);	//3 : ��޷���
		i3Lua::PushStringUTF16To8(L, wstr_exp_rate );			//4 : Ŭ�� ����ġ ����� ����
		i3Lua::PushNumber( L, (exp_rate * 0.01f) );				//5 : Ŭ�� ����ġ �����
		i3Lua::PushStringUTF16To8(L, wstr_clan_point);			//6 : Ŭ������Ʈ

		_EndLuaFunction(L, 6);
	}
}

void UIPhaseClanLobby::_SetClanMark(CLAN_DETAIL_INFO * pInfo)
{
	if (GetScene(0))
	{
		m_pClanMark = nullptr;
		i3UIControl* pDummy = reinterpret_cast<i3UIControl*>(GetScene(0)->FindChildByName("ImgBox_ClanMark"));
		if (pDummy != nullptr)
			ucf::SelectClanMark(pDummy, CLAN_MARK_MEDIUM, pInfo, m_pClanMark);
	}

}

void UIPhaseClanLobby::_SetClanExpRate(i3::stack_wstring& out_wstr, REAL32 & i32OutRate , UINT32 ui32Exp)
{
	if (ui32Exp > DF_CLAN_RANK_L5)
		ui32Exp = DF_CLAN_RANK_L5;

	UINT32 ui32CurRankExp = GetClanBeforeRankExp(ui32Exp);
	UINT32 ui32NextRankExp = GetClanNextRankExp(ui32Exp);

	UINT32 ui32CurExp = ui32Exp - ui32CurRankExp;			
	UINT32 ui32NeedExp = ui32NextRankExp - ui32CurRankExp;

	if( ui32NeedExp == 0)
		ui32NeedExp = 1;
	i32OutRate = static_cast<REAL32>((ui32CurExp * 100) / ui32NeedExp);
	i32OutRate = MINMAX(0 , i32OutRate , 100);

	i3::sprintf(out_wstr, L"%d%%", (INT32)i32OutRate);
}

void UIPhaseClanLobby::_SetTeamSlotInfo(void)
{
	INT32 iMaxPage = m_nCurrentSlotSquad / SQUAD_TEAM_MAX;

	{
		LuaState* L = UILua::CallLuaFunction(GetScene(0), "SetScroolPage");
		if (L != nullptr)
		{
			i3Lua::PushInteger(L, iMaxPage);
			UILua::EndLuaFunction(L, 1);
		}
	}

	INT32 StartIdx = SQUAD_TEAM_MAX * m_nScroolPage;
	INT32 SlotCount = 0;
	for(INT32 i = StartIdx; i < SQUAD_TEAM_MAX + StartIdx; i++)
	{
		LuaState* L = UILua::CallLuaFunction( GetScene(0), "SetClanTeamInfo");

		if(L != nullptr)
		{
			if((unsigned int)i < m_ClanMatchRoomList.size())
			{
				const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();
				bool  isMyteam = false;

				if( pMySquadInfo->isJoinTeam() == true && 
					pMySquadInfo->getClanTeamIdx() == m_ClanMatchRoomList[i].getRoomIdx() )
				{
					isMyteam = true;
				}

				i3Lua::PushInteger(L, SlotCount);
				i3Lua::PushStringUTF16To8(L, m_ClanMatchRoomList[i].getRoomName() );
				i3Lua::PushBoolean(L, m_ClanMatchRoomList[i].isActivateSlot() ? TRUE : FALSE);
				i3Lua::PushBoolean(L, (BOOL)isMyteam);

			}
			else
			{
				i3Lua::PushInteger(L, SlotCount);
				i3Lua::PushStringUTF16To8(L, L"");
				i3Lua::PushBoolean(L, FALSE);
				i3Lua::PushBoolean(L, FALSE);
			}

			UILua::EndLuaFunction(L, 4);
		}

		++SlotCount;
	}
}

/*virtual*/ void UIPhaseClanLobby::OnCreate( i3GameNode * pParent)
{
	UIPhaseBase::OnCreate( pParent);
	
	// Load Scene
	AddScene("Scene/Clan/PointBlankRe_Clan_Channel.i3UIs", ClanLobby_Glue);
}

/*virtual*/ void UIPhaseClanLobby::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate(rDeltaSeconds);

	// ���� �ʴ� �̵����̶�� ������Ʈ�� ���� �ʴ´�.
	if (m_bIsProcessInviteMove)
		return;

	if (ChannelContext::i()->GetConnectedChannelType() == CHANNEL_TYPE_CLAN &&
		UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN &&
		!UserInfoContext::i()->CheckAuthLevel())
	{
		if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
		{
			if (ClanGameContext::i()->getMyMercenaryState() == false &&
				GetMainFrame()->getDeprotationClanChannel() == false)
			{
				i3UI::setDisableUIInput(true);

				GameEventSender::i()->SetEventOnce(EVENT_LEAVE_LOBBY);
				m_bMyPhase = false;
				return;
			}
		}
	}

	// �뺴�� ���� ��Ż�ϴ� ������ ������Ʈ�� ���� ����
	if (m_bProcessLeaveTeam)
		return;

	//static const REAL32 LIST_UPDATE_TIME = 2.0f;

	m_fUpdateTeamListTime += rDeltaSeconds;

	//	�� ����Ʈ�� �޴´�. (3�ʸ��� ������ ��û)
	if( m_fUpdateTeamListTime > 3.0f )
	{
		m_fUpdateTeamListTime = 0.f;
		g_pFramework->GetClanContext()->MatchTeamList();
		GameEventSender::i()->SetEvent(EVENT_LOBBY_REFRESH_N);
	}

	g_pFramework->GetClanContext()->UpdateClanList(rDeltaSeconds);
}

/*virtual*/ void	UIPhaseClanLobby::ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	// ��� ��ġ������ Lobby Event�� �޾ƾ��մϴ�.
	// ���� Phase�� ���� �ֱ⿡ Base���� ó���մϴ�. --swoongoo
	ClanProcessGameEvent::ProcessGameEvent(evt, arg, UserData);

	UIPhaseBase::ProcessGameEvent(evt, arg, UserData);

	g_pFramework->GetClanContext()->OnGameEvent(evt, arg, UserData);

	{
		i3::unordered_map< GAME_EVENT, call_func >::iterator it;
		it = m_EventFuncMap.find(evt);

		if (it != m_EventFuncMap.end())
		{
			call_func & mf = it->second;
			(this->*mf)(arg, UserData);
		}
	}
}

bool UIPhaseClanLobby::OnEvent(UINT32 event, i3ElementBase* pObj, UINT32 param2, I3_EVT_CODE code)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if (ClanGameContext::i()->getMyMatchSquadInfo()->isJoinTeam() == true &&
		m_pLoadingIcon == nullptr && pFrame->GetCurrentSubPhaseType() == USP_NONE)
	{
		if (code == I3_EVT_CODE_ACTIVATE)
		{
			if (event == I3_EVT_INPUT)
			{
				i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
				i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

				if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
				{
					UISignal::ClanLobbyLeaveTeam();
					//	UI::call_cb("clan_lobby_leave_team");
				}
			

				// F5 Ű �Է½� ���� ����
				if (pKeyboard->GetStrokeState(I3I_KEY_F5))
				{
					UISignal::ClanLobbyQuickMatch();
					// UI::call_cb("clan_lobby_quick_match");
				}
			}
		}

		return true;
	}
	//@eunil. Ŭ�� ��ġ����ŷ ���. 1.1���� ���ߵǾ���
	else if (g_pEnvSet->m_nUIVersion == UFV_1_1 &&
		ClanGameContext::i()->getMyMatchSquadInfo()->isJoinTeam() == true &&
		m_pLoadingIcon != nullptr && pFrame->GetCurrentSubPhaseType() == USP_NONE)
	{
		if (code == I3_EVT_CODE_ACTIVATE)
		{
			if (event == I3_EVT_INPUT)
			{
				i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
				i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

				// F5 Ű �Է½� ��Ī ���
				if (pKeyboard->GetStrokeState(I3I_KEY_F5))
				{
					UISignal::ClanLobbyQuickMatchCancel();
					//UI::call_cb("clan_lobby_quick_match_cancel");
				}
			}
		}
		return true;
	}

	return UIPhaseBase::OnEvent(event, pObj, param2, code);
}

bool UIPhaseClanLobby::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPhaseBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	// Ŭ�� �� ��Ī ���� â
	if(g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		GetMainFrame()->EnableFloatWin(UFW_CLANLOBBY);

		//@eunil. ��Ī ����ŷ ���. ����(2017. 01) 1.1�� ����
		_EnableQuickMatchButton(true);
	}

	m_bProcessLeaveTeam = false;
	m_eLoadingType = NONE;
	g_pFramework->GetClanContext()->EnterClanList(TRUE);
	GameEventSender::i()->SetEvent( EVENT_REQUEST_GOODS_LIST);
	if (MedalContext::i()->GetReceivedMyMedal() == false)
	{
		GameEventSender::i()->SetEvent(EVENT_MEDAL_GET_NORMAL_INFO);
	}

	return true;
}

// ���� �Ϸ�
/*virtual*/ void UIPhaseClanLobby::OnEntranceEnd(void)
{
	UIPhaseBase::OnEntranceEnd();

	ClanGameContext::i()->SetMatchEnd(false);

	g_pFramework->getChatBox()->setWhisperEnable(TRUE);
	g_pFramework->getChatBox()->ResetHistoryBufferIndex();

	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();
	m_bInRoomState = (pMySquadInfo->isSameState(TEAM_STATE_NONE) == false);

	_InitMyTeam();

	m_bIsProcessInviteMove = false;

	if (CCommunityContext::i()->IsMovingToUser())
	{
		OnInviteMove();
		return;
	}

	// Ŭ�� ���� ���� ��ŷ �˾� ���
	if (ClanSeasonContext::i()->IsShowPreSeaonRanking())
	{
		UIPOPUPWIN popup_id = UPW_CLAN_SEASON_OVERALL_RANKING;

		if (ClanSeasonContext::i()->IsMyClanInRanking())
			popup_id = UPW_CLAN_SEASON_INDIVISUAL_RANKING;

		g_pFramework->GetUIMainframe()->OpenPopup(popup_id);

		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_PRESEASON_RANKING);
	}

	if (!UserInfoContext::i()->CheckAuthLevel())
	{
		// Ŭ������ Ż���ϰų� Ŭ���� ��ü�� ���, Ŭ���κ� ����� �� ���� �����̰� �뺴�� �ƴ� ���� ���ɴϴ�.
		// �ش� ���� ����ä��������� �ٽ� �Ѱ��ݴϴ�.
		if (UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN &&
			!ClanGameContext::i()->getMyMercenaryState())
		{
			// 1.1�� ���, ä��â�� �̸� ������ѹ����� open�Ǹ鼭 �����ϱ� ������ ������ �߻�.

			if (g_pEnvSet->m_nUIVersion == UFV_1_1)
				GetMainFrame()->DisableFloatWin(UFW_CHAT);

			GameEventSender::i()->SetEventOnce(EVENT_LEAVE_LOBBY);
			m_bMyPhase = false;
			return;
		}

		//@eunil. ��ġ����ŷ ���� ����. �뺴�� �����̰� �г�Ƽ�� �޾Ҵ�
		if (ClanGameContext::i()->getMyMercenaryState() &&
			ClanGameContext::i()->IsMyMercDepartured())
		{
			//Ŭ�� ������, �̰����� ó��

			if (UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
			{ //�̰����ڴ� ä�θ���Ʈ�� �ٷ� ������.
				GameEventSender::i()->SetEventOnce(EVENT_LEAVE_LOBBY);
				m_bMyPhase = false;
				return;
			}
			else
			{ //�����ڴ� �����忡�� ������, �뺴 ����Ʈ������ ���ܵȴ�
				UISignal::ClanLobbyLeaveTeam();
				//UI::call_cb("clan_lobby_leave_team");
			}
		}
	}
}

/*virtual*/ bool UIPhaseClanLobby::OnExitStart( void)
{
	if( UIPhaseBase::OnExitStart() == false)
		return false;

	m_ClanMatchRoomList.clear();

	if(g_pEnvSet->m_nUIVersion != UFV_1_5)
	{
		GetMainFrame()->DisableFloatWin(UFW_CLANLOBBY);
	}

	return true;
}

/*virtual*/ void UIPhaseClanLobby::OnGotoBackPhase( void)
{
	// ��Ʋ���� �����ִ°��, ���� �������մϴ�.
	if (g_pFramework->GetClanContext()->InBattleTeam())
	{
		UISignal::ClanLobbyLeaveTeam();
	//	UI::call_cb("clan_lobby_leave_team");
	}
	else
	{
		i3UI::setDisableUIInput(TRUE);

		// �κ� ������
		GameEventSender::i()->SetEventOnce( EVENT_LEAVE_LOBBY);
		m_bMyPhase = false;
	}
}

/*virtual*/ bool UIPhaseClanLobby::OnInviteMove(void)
{
	if (m_bIsProcessInviteMove)
		return false;

	m_bIsProcessInviteMove = true;

	i3UI::setDisableUIInput(true);

	// ���� / ä���� �ٸ��� Ȯ��
	bool bOtherServer = CCommunityContext::i()->GetMoveTargetServer() != (UINT32)g_pServerInfo->GetCurGameServerIndex();
	bool bOtherChannel = CCommunityContext::i()->GetMoveTargetChannel() != (UINT32)g_pServerInfo->GetCurChannelNumber();

	// ���� �ϳ��� �ٸ��ٸ�, ����/ä�� �������� �̵�
	if (bOtherServer == true || bOtherChannel == true)
	{
		GameEventSender::i()->SetEventOnce(EVENT_LEAVE_LOBBY);
	}
	else
	{
		UINT32 ui32TargetMatch = CCommunityContext::i()->GetMoveTargetMatch();

		if (MATCH_TEAM_INVALID_IDX != ui32TargetMatch)
		{
			UINT32	ui32Match = ui32TargetMatch;
			UISignal::ClanLobbyJoinTeam(ui32Match);
		//	UI::call_cb("clan_lobby_join_team", &ui32Match);
		}
		else
		{
			UIFloatMercenaryList*  pList = static_cast<UIFloatMercenaryList*>(GetMainFrame()->GetFloating(UFW_MERCENARY));
			pList->SetLeaveClanChannel(true);
			GetMainFrame()->MovePhase(UIPHASE_LOBBY);
		}
	}

	return true;
}

//UTIL

void UIPhaseClanLobby::UpdateTeam()
{
	if( IsReadyToUpdate() == false)	return;

	const ClanMatchTeamList * pMatchTeamList = ClanGameContext::i()->getMyClanMatchTeamList();
	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

	m_nCurrentSlotSquad = static_cast<INT32>( pMatchTeamList->size() );
	m_ClanMatchRoomList.clear();

	for(INT32 idx = 0 ; idx < m_nCurrentSlotSquad ; ++idx)
	{
		ClanMatchTeamList::const_iterator Iter = &pMatchTeamList->at(idx);

		CLAN_MATCH_ROOM_INFO RoomInfo(*Iter);
		if( Iter->isSameState(TEAM_STATE_WAIT) && !Iter->isMaxPerson() &&
				pMySquadInfo->getClanTeamIdx() != Iter->getClanTeamIdx() && m_bInRoomState == false )
		{
			RoomInfo.setActivateSlot(true);
		}

		m_ClanMatchRoomList.push_back(RoomInfo);
	}

	// Ŭ�� IDX ����
	struct SortClanTeam
	{
		bool operator()(const CLAN_MATCH_ROOM_INFO & pInfo1, const CLAN_MATCH_ROOM_INFO & pInfo2) const
		{
			return pInfo1.getRoomIdx() < pInfo2.getRoomIdx();
		}
	};

	std::sort(m_ClanMatchRoomList.begin(), m_ClanMatchRoomList.end(), SortClanTeam());

	_SetTeamSlotInfo();

	if (m_bFirstLoad)
	{
		Callback_JoinTeam(m_temp); // (void*)&m_temp, nullptr);
		m_bFirstLoad = false;
	}
}

// LUA ���� �Լ�
void UIPhaseClanLobby::UpdateMember()
{
	if( IsReadyToUpdate() == false)				
		return;

	_SetJoinTeamState();

	_SetSquadInfo();
	_SetSquadMemberInfo();

	_SetSquadOperation();
	_SetSquadCombo();

	_SetQuickMatchButton();
}

void UIPhaseClanLobby::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPhaseClanLobby::ReceiveEvent_ClanWar_GetTeamList( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg) )
	{
		UpdateTeam();
		UpdateMember();
	}

	if(m_pLoadingIcon != nullptr && _isSameLoading(TEAM_REFRESH) )
	{
		GameUI::DisableLoadingIcon( m_pLoadingIcon);	// Team Refresh Loading
		m_pLoadingIcon = nullptr;
		m_eLoadingType = NONE;
	}
}

void UIPhaseClanLobby::ReceiveEvent_ClanWar_CreateTeam( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		m_bInRoomState = true;
		UpdateTeam();
		UpdateMember();

		UIFloatChat * pChatWin = static_cast<UIFloatChat *>(GameUI::GetFloating( UFW_CHAT));
		if( pChatWin != nullptr)
			pChatWin->OnMovePhase(UIPHASE_CLANLOBBY);
	}
	else
	{
		if(ClanGameContext::i()->getMyMercenaryState() == true)
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_CLANCHANNEL_MERCENARY_INVITE_FAIL"));
		}
		else
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_CLANCHANNEL_MAKEERROR"));
		}
	}
}

void UIPhaseClanLobby::ReceiveEvent_ClanWar_JoinTeam( INT32 arg, const i3::user_data&)
{
	//	Ŭ�� �� ����
	i3UI::setDisableUIInput(false);

	if( EV_SUCCESSED(arg))
	{
		m_bInRoomState = true;
		_RefreshTeam();

		UpdateTeam();
		UpdateMember();

		UIFloatChat * pChatWin = static_cast<UIFloatChat *>(GameUI::GetFloating( UFW_CHAT));
		if( pChatWin != nullptr)
			pChatWin->OnMovePhase(UIPHASE_CLANLOBBY);
	}
	else
	{
		switch( arg)
		{				
		case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_NOSEARCH_FAIL:
		case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_EMPTY_FAIL:
			//	"�� ������ �����߽��ϴ�.\n �߸��� ���Դϴ�."
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_FAIL_JOIN_TEAM_BY_INVALID"));
			break;
		case EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL:
			//	"���� ������ ��� á�� ������ �ش� ���� ������ �� �����ϴ�."
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_FAIL_JOIN_TEAM_BY_FULL"));
			break;	
		default:
			//	"��Ʈ��ũ ������ ���� �������� ���߽��ϴ�.\n �ٽ� �õ��� �ּ���."
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_FAIL_JOIN_TEAM_BY_SERVER"));
			break;
		}
	}

	CCommunityContext::i()->CompleteMovingToUser();
	m_bIsProcessInviteMove = false;
}

void UIPhaseClanLobby::ReceiveEvent_ClanWar_LeaveTeam( INT32 arg, const i3::user_data&)
{
	//	�� ������
	const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

	if( pMySquadInfo->isJoinTeam() == false )
	{
		m_bInRoomState = false;
		_RefreshTeam();

		UIFloatChat * pChatWin = static_cast<UIFloatChat *>(GameUI::GetFloating( UFW_CHAT));

		if (pChatWin == nullptr) return;

		if(ClanGameContext::i()->getMyMercenaryState() == true) 
		{
			if (ClanGameContext::i()->IsMyMercDepartured() == false)
			{ //�г�Ƽ�� ���� ���� �뺴 ��û �����ϴ�
				m_bProcessLeaveTeam = true;
				GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_REJOIN_POPUP"), nullptr, MAKE_CALLBACK(UIPhaseClanLobby::CbLeaveTeam), this);
			}
			else
			{
				GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
			}
		}
		else
			pChatWin->OnMovePhase(UIPHASE_CLANLOBBY);			
	}
	else
	{
		switch(arg)
		{
		case EVENT_ERROR_SUCCESS							:	_SetSquadMemberInfo(); break;
		case EVENT_ERROR_CLAN_BATTLE_TEAM_CLOSE				:	_SetSquadMemberInfo(); break;
		case EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_EMPTY_FAIL	:
		case EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_NOUSER_FAIL	:
		default:
			//	"��Ʈ��ũ ������ �����⿡ �����߽��ϴ�.\n �ٽ� �õ��� �ּ���."
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_FAIL_OUT_TEAM"));
			break;
		}
	}
}

void UIPhaseClanLobby::ReceiveEvent_ClanWar_ChangeOperation( INT32, const i3::user_data&)
{
	_SetSquadOperation();
}

void UIPhaseClanLobby::ReceiveEvent_ClanWar_ChangeMaxPerson( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
		_SetSquadCombo();
}

void UIPhaseClanLobby::ReceiveEvent_ClanWar_RegisterMatchMaking( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		if( m_pLoadingIcon == nullptr && m_pFrameWnd != nullptr && _isSameLoading(NONE) )
		{
			m_pLoadingIcon = GameUI::EnableLoadingIcon( this, m_pFrameWnd);	// Match Making Loading
			m_eLoadingType = MATCH_MAKING;
		}
	}
	else if(arg == EVENT_ERROR_CLAN_WAR_TIME_OUT)
	{
		//@eunil. Ŭ�� ��ġ����ŷ ���, TimeOut�� ��� ��ư ���� �ǵ���
		if (g_pEnvSet->m_nUIVersion == UFV_1_1 && ClanGameContext::i()->isMatchTeamLeader()) {
			_EnableQuickMatchButton(true);
		}

		if( m_pFrameWnd != nullptr && _isSameLoading(MATCH_MAKING) )
		{
			GameUI::DisableLoadingIcon( m_pLoadingIcon);	// Match Making Loading
			m_pLoadingIcon = nullptr;
			m_eLoadingType = NONE;
		}
	}
}

void UIPhaseClanLobby::ReceiveEvent_ClanWar_RegisterCancelMatchMaking(INT32 arg, const i3::user_data&)
{
	if (EV_SUCCESSED(arg))
	{
		//@eunil. ��ġ����ŷ ���
		if (m_pFrameWnd != nullptr && _isSameLoading(MATCH_MAKING))
		{
			GameUI::DisableLoadingIcon(m_pLoadingIcon);	// Match Making Loading
			m_pLoadingIcon = nullptr;
			m_eLoadingType = NONE;
		}

		if(m_bPushedBackPhase) //���ư�� ���� ��Ī ���
			UISignal::ClanLobbyLeaveTeam();
			//UI::call_cb("clan_lobby_leave_team");
	}
	//else if (arg == EVENT_ERROR_CLAN_WAR_TIME_OUT)
	//{
	//	if (m_pFrameWnd != NULL && _isSameLoading(MATCH_MAKING))
	//	{
	//		GameUI::DisableLoadingIcon(m_pLoadingIcon);	// Match Making Loading
	//		m_pLoadingIcon = NULL;
	//		m_eLoadingType = NONE;
	//	}
	//}
}

void UIPhaseClanLobby::ReceiveEvent_Clan_List_Context( INT32, const i3::user_data&)
{
	// Ŭ�� ���ؽ�Ʈ �ޱ�
	CLAN_CONTEXT		*	pClanContext	= ClanGameContext::i()->getClanContext();
	CLAN_LIST_BASIC_INFO	*	pClanList	= ClanGameContext::i()->getClanList();
	if(pClanList == nullptr)
	{
		if(pClanContext->_sectionCount > 0)
		{
			ClanGameContext::i()->CreateClanList( pClanContext->_sectionSize * pClanContext->_sectionCount );
		}
	}

	//if(m_bIsProcessInviteMove == false)
	{
		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

		g_pFramework->GetClanContext()->SendClanDetailInfoNEvent(MyUserInfoBasic.m_ui32Clanidx, 0);

		if( m_pLoadingIcon == nullptr && m_pFrameWnd != nullptr && _isSameLoading(NONE))
		{
			m_pLoadingIcon = GameUI::EnableLoadingIcon( this, m_pFrameWnd);	//Clan Detail Info Loading
			m_eLoadingType = CLAN_INFO;
		}
	}
}

void UIPhaseClanLobby::ReceiveEvent_Clan_DetailInfo( INT32 arg, const i3::user_data&)
{
	_SetClanDetailInfo();

	if(m_pLoadingIcon != nullptr && _isSameLoading(CLAN_INFO) )
	{
		GameUI::DisableLoadingIcon( m_pLoadingIcon);	// Clan Detail Info Loading
		m_pLoadingIcon = nullptr;
		m_eLoadingType = NONE;
	}
}

void UIPhaseClanLobby::ReceiveEvent_Create_Room( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		// �������� ������ PersonMax ���� �޾�, ������ ������ �ݾ��ش�.
		ROOM_INFO_BASIC RoomInfo;
		MyRoomInfoContext::i()->getMyRoomInfo(&RoomInfo);
		for(INT32 nSlot = RoomInfo._PersonPlayerMax ; nSlot < SLOT_MAX_COUNT ; nSlot++)
			BattleSlotContext::i()->setSlotState( nSlot, SLOT_STATE_CLOSE);

		if(m_pLoadingIcon != nullptr && _isSameLoading(MATCH_MAKING) )
		{
			GameUI::DisableLoadingIcon( m_pLoadingIcon);	// Match Making Loading
			m_pLoadingIcon = nullptr;
			m_eLoadingType = NONE;
		}

		GetMainFrame()->MovePhase( UIPHASE_READY_ROOM);			
	}
	else
	{
		m_bMyPhase = true;
		i3UI::setDisableUIInput(false);
	}
}

void UIPhaseClanLobby::ReceiveEvent_Join_Room( INT32 arg, const i3::user_data&)
{
	i3UI::setDisableUIInput(false);

	CCommunityContext::i()->CompleteMovingToUser();
	m_bIsProcessInviteMove = false;

	if( EV_SUCCESSED(arg))
	{
		if(m_pLoadingIcon != nullptr && _isSameLoading(MATCH_MAKING) )
		{
			GameUI::DisableLoadingIcon( m_pLoadingIcon);	// Match Making Loading
			m_pLoadingIcon = nullptr;
			m_eLoadingType = NONE;
		}

		GetMainFrame()->MovePhase( UIPHASE_READY_ROOM);
	}
	else
	{
		i3::rc_wstring wstr_bug;
		
		i3::sprintf( wstr_bug, GAME_RCSTRING("STR_TBL_GUI_LOBBY_DEBUG_POPUP_TEXT"), EVENT_JOIN_ROOM, arg);/*����� �˾�: Event(%d), Arg(0x%x)*/
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_bug);
	}
}

void UIPhaseClanLobby::ReceiveEvent_Leave_Lobby( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		GetMainFrame()->MovePhase( UIPHASE_SERVER_CHANNEL_LIST);
	}
	else
	{
		CCommunityContext::i()->CompleteMovingToUser();
		m_bIsProcessInviteMove = false;

		m_bMyPhase = true;
		GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_FAIL_REFRESH_C"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
	}
}

void UIPhaseClanLobby::ReceiveEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

void UIPhaseClanLobby::ReceiveEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

/************************************************************************/
/*                        Lua Glue Callback Func                        */
/************************************************************************/ 

void UIPhaseClanLobby::Callback_CreateTeam()
{
	if(ClanGameContext::i()->getMyMercenaryState() == true)
	{
		/* ���� �����Ϸ���, �뺴 ��û�� ��Ұ� �˴ϴ�. ���� ���� �Ͻðڽ��ϱ�? */
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_CLANCHANNEL_MERCENARY_CANCEL_CRETEAM"), nullptr, MAKE_CALLBACK(UIPhaseClanLobby::CbCreateTeam), this);
		return;
	}
	UINT8 ui8MaxPerson = GetClanBattleMatchMember(CLAN_MATCH_4);

	CClanContext* pClanContext = g_pFramework->GetClanContext();
	pClanContext->MatchTeamCreate( ui8MaxPerson);		
}

void UIPhaseClanLobby::Callback_JoinTeam(INT32 RoomIdx)
{
	INT32 slot_idx = RoomIdx; // *(reinterpret_cast<INT32*>(arg1));

	size_t real_idx = slot_idx + ( m_nScroolPage * SQUAD_TEAM_MAX );

	// ���� ���̸� ����
	if (real_idx >= m_ClanMatchRoomList.size())
	{
		m_bFirstLoad = true;
		m_temp = slot_idx;

		// �ʴ���� ������ ���� ã�� ���� ���¿��� �������� ��� 
		// ������ Ŭ���κ񿡼� �������� ���ϰ� �� �� �����Ƿ� �ϴ� �̵��� ������.
		if (CCommunityContext::i()->IsMovingToUser())
		{
			m_bIsProcessInviteMove = false;
			CCommunityContext::i()->CompleteMovingToUser();
		}

		return;
	}

	const ClanMatchRoomList::iterator iter = &m_ClanMatchRoomList.at(real_idx);
	const CLAN_MATCH_TEAM_INFO * pTeamInfo = ClanGameContext::i()->getMyClanMatchTeam( iter->getRoomIdx());
	I3ASSERT( pTeamInfo != nullptr);

	if( pTeamInfo->isMaxPerson() )
	{
		/* ���� ������ ��� á�� ������ �ش� ���� ������ �� �����ϴ�. */
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CANNOT_JOIN_FULL"));
	}
	else
	{
		ClanGameContext::i()->setMyClanMatchTeamIdx((UINT8)real_idx);
		if(ClanGameContext::i()->getMyMercenaryState() == true)
		{
			/* �����Ͻ� �� ��ġ ���� �����Ͻðڽ��ϱ�?\n ���� �� �뺴 ����Ʈ���� ���ŵ˴ϴ�. */
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_REQUEST_CLAN_MATCH_JOIN"), nullptr, MAKE_CALLBACK(UIPhaseClanLobby::CbJoinTeam), this);
			m_SeletedTeamID = pTeamInfo->getMatchTeamID();
			return;
		}		
		g_pFramework->GetClanContext()->MatchTeamJoin( static_cast<UINT32>( pTeamInfo->getMatchTeamID()) );
	}
}

void UIPhaseClanLobby::Callback_LeaveTeam()
{

	if (ClanGameContext::i()->getMyMatchSquadInfo()->isJoinTeam())
	{
		if (ClanGameContext::i()->getMyMercenaryState())
			m_bProcessLeaveTeam = true;

		if (g_pEnvSet->m_nUIVersion == UFV_1_1 &&	//��ġ����ŷ ��Ҵ� 1.1�� ����
			m_pFrameWnd != nullptr && _isSameLoading(MATCH_MAKING))
		{
			//@eunil. ��ġ����ŷ ���. ��û ���̸�, ���� ��Һ���
			// �ݹ��� �Ʒ�ó�� ������� ��µ� �� ��Ŷó���� �Ųٷ� ����. �������� ó���ϴ� ��Ŀ� ���� Ư������ �ֳ�.
			//UI::call_cb("clan_lobby_quick_match_cancel");
			//UI::call_cb("clan_lobby_leave_team");

			m_bPushedBackPhase = true;
			if (ClanGameContext::i()->isMatchTeamLeader())
				UISignal::ClanLobbyQuickMatchCancel();
				//UI::call_cb("clan_lobby_quick_match_cancel");
			else
				g_pFramework->GetClanContext()->MatchTeamLeave();		//	Ŭ���� ������ ����
		
		}
		else
		{
			g_pFramework->GetClanContext()->MatchTeamLeave();		//	Ŭ���� ������ ����
		}
	}
	else
	{
		if(g_pEnvSet->m_nUIVersion != UFV_1_5)
		{
			GameEventSender::i()->SetEvent(EVENT_LEAVE_LOBBY);

			// 14538 ������ �����⸦ �������� ������ ClanContext���� EVENT_CLAN_LIST_N �� ��û�ؼ� ©��.
			m_bMyPhase = false;
		}
	}
}

void UIPhaseClanLobby::Callback_QuickMatch()
{
	if (ClanGameContext::i()->isMatchTeamLeader()) {
		_EnableQuickMatchButton(false);
	}

	g_pFramework->GetClanContext()->MatchTeamRegisterMatchMaking();
}

void UIPhaseClanLobby::Callback_QuickMatchCancel()
{
	if (ClanGameContext::i()->isMatchTeamLeader()) {
		_EnableQuickMatchButton(true);
	}
	g_pFramework->GetClanContext()->MatchTeamCancelRegisterMatchMaking();
}

void UIPhaseClanLobby::Callback_EnterRoomList()
{
	g_pFramework->GetUIMainframe()->MovePhase(UIPHASE_LOBBY);
	ClanGameContext::i()->setClanBattleLobbyEnable(false);
}

void UIPhaseClanLobby::Callback_ScrollTeamList(INT32 scrollPos)
{
	INT32 pos = scrollPos; // *(reinterpret_cast<INT32*>(arg1));
	if (pos < 0) pos = 0;

	m_nScroolPage = pos;
	_SetTeamSlotInfo();
}

void UIPhaseClanLobby::Callback_ChangePerson(INT32 matchType)
{
	CLAN_MATCH_TYPE type = CLAN_MATCH_TYPE(matchType); //*(reinterpret_cast<CLAN_MATCH_TYPE*>(arg1));

	I3_BOUNDCHK( type, CLAN_MATCH_MAX);

	//	������ �����ϴ�.
	if( ClanGameContext::i()->isMatchTeamLeader() )
	{		
		bool is_possible_change = false ;

		UINT8 ui8MaxPerson = GetClanBattleMatchMember(type);
		const UINT8 ui8CurPerson = ClanGameContext::i()->getMyMatchSquadInfo()->getCurPerson();

		if( ui8MaxPerson >= ui8CurPerson)
			is_possible_change = true;

		// ���� ���Կ� ������ ������, ������ �Ұ����մϴ�.
		if( is_possible_change )
		{
			const CLAN_MATCH_SQUAD_INFO * pMySquadInfo = ClanGameContext::i()->getMyMatchSquadInfo();

			for( UINT8 idx = 0; idx < ui8CurPerson ; ++idx)
			{
				const CLAN_MATCH_SQUAD_MEMBER & Member = pMySquadInfo->getMember(idx); 
				if (Member.getSlotIdx() >= ui8MaxPerson)
				{
					is_possible_change = false;
				}
			}
		}

		if( !is_possible_change )
		{
			//	"�� ���� �ο��� ������ �� �����ϴ�. \n ���ϴ� �������� �°� �� �ο��� ������ ��\n �ٽ� �õ��� �ֽʽÿ�."
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_CANNOT_ORGANIZE_TEAM"));
			//UpdateCombo_MatchType();		//  �� ��� ������ ������ Ÿ������ �ǵ����� �Ѵ�..
		}
		else
		{
			// �� ��쿡�� ������ �����ڿ� �ʿ��� ������ �޾� �����ϵ��� ó���Ѵ�..
			g_pFramework->GetClanContext()->MatchTeamChangeMaxPerson(ui8MaxPerson);			
		}
	}
	else
	{
		//	"�� ������ ������ �� �ֽ��ϴ�."
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_MODIFY_ONLY_LEADER"));
	}	
}

void UIPhaseClanLobby::Callback_ChangeOperation(const i3::literal_range& strOperation)
{
//	const char * pstrOperation = reinterpret_cast<const char *>(arg1);
	i3::rc_wstring wstrOperation;
	i3::utf8_to_utf16(strOperation, wstrOperation);

	g_pFramework->GetClanContext()->MatchTeamChangeOperation(wstrOperation);
}

void UIPhaseClanLobby::CbCreateTeam(void* pThis,INT32 nParam)
{
	if (nParam == MB_OK)
	{
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
		UINT8 ui8MaxPerson = GetClanBattleMatchMember(CLAN_MATCH_4);

		CClanContext* pClanContext = g_pFramework->GetClanContext();
		pClanContext->MatchTeamCreate( ui8MaxPerson);
	}
}

void UIPhaseClanLobby::CbJoinTeam(void* pThis,INT32 nParam)
{
	if (nParam == MB_OK)
	{
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
		const T_TeamID team_id = ((UIPhaseClanLobby*)pThis)->m_SeletedTeamID;

		g_pFramework->GetClanContext()->MatchTeamJoin( static_cast<UINT32>( team_id ));
		ClanGameContext::i()->setClanBattleLobbyEnable(true);
	}
	else
		ClanGameContext::i()->setMyClanMatchTeamIdx(0); // ���� �������� �ʾ����Ƿ� �ʱ�ȭ
}

void UIPhaseClanLobby::CbLeaveTeam(void* pThis, INT32 nParam)
{
	// �뺴 ������. �κ�� ���� 
	if(nParam == MB_OK)
	{
		g_pFramework->GetUIMainframe()->MovePhase(UIPHASE_LOBBY);
		ClanGameContext::i()->setClanBattleLobbyEnable(false);
	}
	else	// �뺴 ���� ����. �뺴 ����Ʈ���� �����ϰ� ä�θ���Ʈ�� ���� 
	{
		if(UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
		{
			((UIPhaseClanLobby*)pThis)->SetLeaveTeamFlag();
			GameEventSender::i()->SetEvent(EVENT_LEAVE_LOBBY);
			ClanGameContext::i()->setClanBattleLobbyEnable(false);
			return;
		}
		else
		{
			GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
			g_pFramework->GetUIMainframe()->MovePhase(UIPHASE_CLANLOBBY);
		}
	}
}