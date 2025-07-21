#include "pch.h"
#include "UIFloatUserList_V11.h"
#include "UIFloatChat_V11.h"
#include "UIPopupUserAction_V11.h"

#include "../UI/UIMainFrame_V11.h"
#include "../UI_V11/UIPhaseReadyRoom_V11.h"
#include "../UI/UIUtil.h"

#include "../CommunityContext.h"
#include "../GameGUI.h"

#include "../ChannelContext.h"
#include "../UserInfoContext.h"
#include "../LobbyContext.h"
#include "LatinAmericaNationInfo.h"

static UIFloatUserList_V11 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( pFrame != nullptr)
	{
		UIFloatChat_V11 * pChatWin = (UIFloatChat_V11*) pFrame->GetFloating( UFW_CHAT);
		return pChatWin->GetUserList();
	}

	return nullptr;
}

static void s_ClanMarkProcessor( i3UIListView_Cell * pCell, void * pData)
{
	GameUI::SetClanTextureForListView( pCell, pData);
}


extern "C" {
	int UIFloatUserList_V11_ChangeUserList( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		GetThis()->SelectUserTab( tab);

		return 0;
	}

	int UIFloatUserList_V11_OpenUserPopup( LuaState * L)
	{
		//INT32 tab = i3Lua::GetIntegerArg( L, 1);
		INT32 slot = i3Lua::GetIntegerArg( L, 2);
		GetThis()->OpenUserPopup( slot);
		return 0;
	}

	int UIFloatUserList_V11_SearchUser( LuaState * L)
	{
		GetThis()->SearchUser();
		return 0;
	}
};

LuaFuncReg UIFloatUserList_V11_Glue[] = {
	{"ChangeUserTab",			UIFloatUserList_V11_ChangeUserList},
	{"OpenUserPopup",			UIFloatUserList_V11_OpenUserPopup},
	{"SearchUser",				UIFloatUserList_V11_SearchUser	},
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIFloatUserList_V11);

UIFloatUserList_V11::UIFloatUserList_V11()
{
	m_CurrentUserTab = USERLIST_TAB_CURRENT;
	m_bReqUserInfo = false;

#if defined( I3_DEBUG)
	SetName( "UserList");
#endif

	i3mem::FillZero( m_pClanImg, sizeof( m_pClanImg));

	m_pUserListMainBox = nullptr;
	m_pUserListBox = nullptr;

	m_pEditSearch = nullptr;

	m_RSelectedIndex = -1;
	m_SelectedIndex = -1;

	m_UserUI_NationMarkData._Reset();

	for (int i = 0; i < MAX_NATION_MARK; ++i)
	{
		m_NationMark[i].Reset();
	}
}

UIFloatUserList_V11::~UIFloatUserList_V11()
{
	_ClearUserList();
}

UIFloatChat_V11 * UIFloatUserList_V11::GetChat( void)
{
	return ((UIFloatChat_V11*)m_pParent);
}

void UIFloatUserList_V11::_ClearUserList( void)
{
	//i3::cu::for_each_SafeFree_clear(m_UserInfoList);
	i3::cu::for_each_delete_clear(m_UserInfoList);
	m_UserUI_NationMarkData.vShapeIdxList.clear();
}

void UIFloatUserList_V11::_SetUserInfo( INT32 idx, const USERINFO_V11 * pInfo)
{
	LuaState * L;
	i3::rc_wstring wszRankToolTip;
	i3::rc_wstring wszReasonToolTip;

	if( pInfo != nullptr && pInfo->_Nick[0] != 0)
	{
		//	���
		INT32 rank = (INT32) pInfo->_Rank;

		if( ChannelContext::i()->IsBeginnerChannel1())	//	�ʺ� ����
			rank = RANK_NEWBIE;

		if( rank >= RANK_MAX)
			rank = RANK_MAX-1;

		wszRankToolTip = GetCharaRank( rank);
		if(m_CurrentUserTab == USERLIST_TAB_BLOCK_V11)
			wszReasonToolTip = pInfo->_BlockReason;
		else
			wszReasonToolTip = L"";

		L = _CallLuaFunction( "SetUserInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, idx);
			GameUI::SetClanMarkToLua( L, pInfo->_ClanMark);
			i3Lua::PushStringUTF16To8( L, pInfo->_Clan);
			i3Lua::PushInteger( L, rank);
			i3Lua::PushStringUTF16To8( L, wszRankToolTip);
			i3Lua::PushStringUTF16To8( L, pInfo->_Nick);
			i3Lua::PushStringUTF16To8( L, pInfo->_BlockReason);
			i3Lua::PushStringUTF16To8( L, pInfo->_szPosition);
			i3Lua::PushColor( L, (I3COLOR*) &pInfo->_NickColor);
			i3Lua::PushColor( L, (I3COLOR*) &pInfo->_PosColor);
			_EndLuaFunction( L, 20);
		}
	}
	else
	{
		L = _CallLuaFunction( "SetUserInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, idx);
			GameUI::SetClanMarkToLua( L, 0xFFFFFFFF);
			i3Lua::PushA2UTF8String( L, "");
			i3Lua::PushInteger( L, -1);
			i3Lua::PushA2UTF8String( L, "");
			i3Lua::PushA2UTF8String( L, "");
			i3Lua::PushA2UTF8String( L, "");
			i3Lua::PushA2UTF8String( L, "");
			i3Lua::PushColor( L, GameGUI::GetColor( GCT_DEFAULT));
			i3Lua::PushColor( L, GameGUI::GetColor( GCT_DEFAULT));
			_EndLuaFunction( L, 20);
		}
		
		if (LocaleValue::Enable("EnableNationMark") == TRUE)
		{ 
			if (m_NationMark[idx].pNation != nullptr)
			{
				m_NationMark[idx].pNation->EnableCtrl(false);
				m_NationMark[idx].pEffect->EnableCtrl(false);
			}
		}
	}
}

/*virtual*/ void UIFloatUserList_V11::OnUpdate( REAL32 rDeltaSeconds)
{
	UISubBase::OnUpdate( rDeltaSeconds);

	if( m_CurrentUserTab == USERLIST_TAB_CLAN)
	{
		if( g_pFramework->GetClanContext() != nullptr)
			g_pFramework->GetClanContext()->UpdateClanMember( rDeltaSeconds);
	}

	// �ʴ� �˾� ��ġ
	//UIPHASE ePhase = GetMainFrame()->GetCurrentPhaseType();

	//if( (ePhase == UIPHASE_LOBBY || ePhase == UIPHASE_READY_ROOM)  && 
	//	GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE && g_pCommunity->GetInviteCount() > 0)
	//{
	//	GetMainFrame()->GetCurrentPhase()->InvitedMessage();
	//}

	// ���� ǥ�� ����
	if (LocaleValue::Enable("EnableNationMark") == TRUE && m_CurrentUserTab == USERLIST_TAB_CURRENT)
	{
		GameUI::Update_RankAndNationMark_Swap(rDeltaSeconds, m_UserUI_NationMarkData);
		GameUI::Update_RankMark_AlphaValue(m_pUserListBox, 1, 1 - m_UserUI_NationMarkData.rAlphaValue);
		_UpdateNationMarkAndEffect();
	}
}

/*virtual*/ void UIFloatUserList_V11::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	switch( evt)
	{
	case EVENT_CLAN_MEMBER_LIST_N :
		if(SUCCEEDED(arg))
		{
			_UpdateClanMemberList();
		}
		else
		{
			//��Ʈ�� �˾� �߰�����.
		}
		break;
	case EVENT_DELETE_BLOCK :
	case EVENT_UPDATE_USER_LIST :
		_UpdateUserList();
		break;
	case EVENT_FIND_USER:
		{
			if(CCommunityContext::i()->IsFindUser())
			{
				if(EV_SUCCESSED(arg))
				{
					GetMainFrame()->OpenPopup( UPW_USERPROFILE);
					m_pEditSearch->SetText(L"");
					i3UI::getGlobalIme()->SetInputText( L"", 0);
				}
				else
				{
					i3::stack_wstring wstrString;

					// Offline�� �ƴѰ� �����մϴ�.
					if(arg == EVENT_ERROR_NEW_USER_FIND_FUSER_OFFLINE)
						wstrString = GAME_STRING("STR_TBL_USERSEARCH_DISCONNECT_USER_POPUP");
					else
						wstrString = GAME_STRING("STR_TBL_USERSEARCH_ERROR_POPUP");

					m_pEditSearch->SetTextColor(GameGUI::GetColor(GCT_RED));
					GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrString);
					//2014-6-27 ENG_��뿵 ������ ã�� ������ ��쿡 ���� ���ε� ���¸� �����ϴ� ���� �߰�.
					CCommunityContext::i()->SetFindUser(false);
				}
			}	
			break;
		}
	case EVENT_INSERT_FRIEND:
		_AddFriend(arg);
		_UpdateUserList();
		break;
	}
}

/*virtual*/ void UIFloatUserList_V11::BindScene( i3UIScene * pScene)
{
	I3ASSERT( pScene != nullptr);

	m_pUserListMainBox = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd8");
	I3ASSERT( m_pUserListMainBox);

	m_pUserListBox = (i3UIListView_Box*)m_pUserListMainBox->FindChildByName( "i3UIListView_Box_CW_02");
	I3ASSERT( m_pUserListBox);

	m_pEditSearch = (i3UIEditBox *) pScene->FindChildByName("Edit_Search");
	I3ASSERT(m_pEditSearch);

	if( pScene->getLuaContext() != nullptr)
	{
		_RegisterLuaFunctions( pScene->getLuaContext(), UIFloatUserList_V11_Glue);
	}

	char Nation[MAX_PATH];
	char Effect[MAX_PATH];

	for (int i = 0; i < MAX_NATION_MARK; ++i)
	{

		sprintf_s(Nation, "NationMark_%02d", i + 1); // RED_NATION_01 ~ 08
		m_NationMark[i].pNation = (i3UIButtonImageSet*)pScene->FindChildByName(Nation);
		I3ASSERT(m_NationMark[i].pNation);


		sprintf_s(Effect, "NationEffect_%02d", i + 1); // RED_EFFECT_01 ~ 08
		m_NationMark[i].pEffect = (i3UIButtonImageSet*)pScene->FindChildByName(Effect);
		I3ASSERT(m_NationMark[i].pEffect);


		m_NationMark[i].pNation->EnableCtrl(false);
		m_NationMark[i].pEffect->EnableCtrl(false);
	}

	CCommunityContext::i()->SetFindUser(false);
}

/*virtual*/ void UIFloatUserList_V11::UnBindScene( void)
{	
	m_pUserListMainBox = nullptr;
	m_pUserListBox = nullptr;
	m_pEditSearch = nullptr;
}

void UIFloatUserList_V11::OnMinimizeStart( void)
{	
	I3ASSERT( m_pUserListMainBox != nullptr);
	m_pUserListMainBox->EnableCtrl( false, true);
}

void UIFloatUserList_V11::OnMinimizeEnd( void)
{
	
}

void UIFloatUserList_V11::OnMaximizeStart( void)
{
	I3ASSERT( m_pUserListMainBox != nullptr);

	if( GetChat()->IsMaximizeDone() == false )
	{
		m_pUserListMainBox->EnableCtrl( false, true);
	}
}

void UIFloatUserList_V11::OnMaximizeEnd( void)
{
	I3ASSERT( m_pUserListMainBox != nullptr);
	m_pUserListMainBox->EnableCtrl( true, true);

	// UserList�� �ٽ� Update���ش�.
	// UI�� ImageSet�� ��� Enable��Ű�� ������ ����....
	_UpdateUserList();
}

/*virtual*/ void UIFloatUserList_V11::OnMovePhase( UIPHASE phase)
{
	INT32 i;

INT32 pushedTab = -1;

	switch( phase)
	{
	case UIPHASE_LOBBY :
		m_TabState[0] = USERLIST_TAB_CURRENT;
		m_TabState[1] = USERLIST_TAB_FRIEND;
		m_TabState[2] = USERLIST_TAB_CLAN;
		m_TabState[3] = USERLIST_TAB_NONE;
		if( LocaleValue::Enable("EnableBlock") )
			m_TabState[3] = USERLIST_TAB_BLOCK_V11;
		pushedTab = 0;
		break;

	case UIPHASE_CLANLOBBY:
		m_TabState[0] = USERLIST_TAB_CURRENT;
		m_TabState[1] = USERLIST_TAB_FRIEND;
		m_TabState[2] = USERLIST_TAB_CLAN;
		m_TabState[3] = USERLIST_TAB_NONE;
		if( LocaleValue::Enable("EnableBlock") )
			m_TabState[3] = USERLIST_TAB_BLOCK_V11;
		pushedTab = 0;
		break;

	case UIPHASE_READY_ROOM :
		m_TabState[0] = USERLIST_TAB_FRIEND;
		m_TabState[1] = USERLIST_TAB_CLAN;
		m_TabState[2] = USERLIST_TAB_NONE;
		m_TabState[3] = USERLIST_TAB_NONE;
		if( LocaleValue::Enable("EnableBlock") )
			m_TabState[3] = USERLIST_TAB_BLOCK_V11;
		pushedTab = 0;
		break;
	}
	
	if( i3::same_of<UIFloatChat_V11*>(m_pParent) == true)
	{
		I3TRACE("���⿡ ���´�?\n");
	}
	LuaState * L = _CallLuaFunction( "SetUserTabState");
	if( L != nullptr)
	{
		for( i = 0; i < USERLIST_TAB_MAX; i++)
		{
			i3Lua::PushInteger( L, m_TabState[i]);

			if( pushedTab == -1 && m_TabState[ i] == m_CurrentUserTab)
			{
				pushedTab = i;
			}
		}

		_EndLuaFunction( L, USERLIST_TAB_MAX);
	}

	if( pushedTab == -1)
		pushedTab = 0;

	L = _CallLuaFunction( "SetSelectUserTab");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, pushedTab);
		_EndLuaFunction( L, 1);
	}

	// ģ��ã�� ��� Ȱ��ȭ (�κ� O , ����� X)
	L = _CallLuaFunction("SearchEnable");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L , phase == UIPHASE_LOBBY ? true : false);
		_EndLuaFunction(L , 1);
	}

	SelectUserTab( pushedTab);
}

void UIFloatUserList_V11::SelectUserTab( INT32 tab)
{
	if( tab > -1 && tab < USERLIST_TAB_MAX)
	{
		if( m_CurrentUserTab != m_TabState[ tab])
		{
			m_CurrentUserTab = m_TabState[ tab];

			if (m_CurrentUserTab != USERLIST_TAB_CURRENT)
			{
				if (LocaleValue::Enable("EnableNationMark") == TRUE)
				{
					// PBBUG-16760 ��ȹ�ǵ������� '����' ���� ������ ���������� ��ũ Alpha�� ����
					GameUI::Update_RankMark_AlphaValue(m_pUserListBox, 1, (REAL32)1.0f);

					// PBBUG-16760 ��ȹ�ǵ������� '����' ���� ������ ���������� ���⸦ ǥ����������
					for (int i = 0; i < MAX_NATION_MARK; ++i)
					{
						if (m_NationMark[i].pNation != nullptr)
							m_NationMark[i].pNation->EnableCtrl(false);
						if (m_NationMark[i].pEffect != nullptr)
							m_NationMark[i].pEffect->EnableCtrl(false);
					}
				}
			}

			_UpdateUserList();
		}
	}
}

void UIFloatUserList_V11::OpenUserPopup( INT32 slot)
{
	UINT32 i;

	if( _SetSelectedUser( slot))
	{
		USERINFO_V11 * pInfo = getUserInfo( slot);

		GetMainFrame()->ResetUserActionPopup();

		if( pInfo->_Session > -1 &&  
			(GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE || GetMainFrame()->GetCurrentSubPhaseType() == USP_TEMP) )
			GetMainFrame()->AddUserActionCommand( UCM_PROFILE);		//	�󼼺���

		//	�ڽ��� �ƴ� ��츸
		if( ! UserInfoContext::i()->IsMyNickName( m_SelectedUserInfo._Nick))
		{
			bool bFriend = false;
			bool bClan = false;
			for( i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
			{
				const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( i);
				if(nullptr == node)	continue;
				if( i3::generic_compare( node->_wstr_nick, m_SelectedUserInfo._Nick) == 0)
				{
					// �̹� ģ���̴�.
					bFriend = true;
					bClan = _IsMyClanMember(node->_uid);
				}
			}

			if( m_CurrentUserTab == USERLIST_TAB_CURRENT ||
				m_CurrentUserTab == USERLIST_TAB_CLAN )
			{
				if( bFriend == false)
					GetMainFrame()->AddUserActionCommand( UCM_ADD_FRIEND);		//	ģ���߰�
			}

			//if( pInfo->_Session > -1 )
			//{	// ���� �����ڸ� �ʴ밡 �����ϵ��� �Ǿ� �ִ�..
			if( (m_CurrentUserTab != USERLIST_TAB_CLAN &&
				m_CurrentUserTab != USERLIST_TAB_BLOCK_V11) &&
				CanClanInvite() )
			{
				if(m_CurrentUserTab == USERLIST_TAB_CURRENT && g_pFramework->GetClanContext()->IsPossibleClanInvite() && 
					(m_SelectedUserInfo._ClanMark == CLAN_MARK_FLAG_NONE))
				{	
					GetMainFrame()->AddUserActionCommand( UCM_CLAN_INVITE);		// Ŭ���ʴ�(CurrentTab)
				}
				if(m_CurrentUserTab == USERLIST_TAB_FRIEND && g_pFramework->GetClanContext()->IsPossibleClanInvite() && 
					bClan == false)
				{	
					GetMainFrame()->AddUserActionCommand( UCM_CLAN_INVITE);		// Ŭ���ʴ�(FrientTab)
				}

			}

			// ģ���� or Ŭ���ǿ����� �ʴ�/�����ϱ� Ȱ��ȭ
			if( m_CurrentUserTab == USERLIST_TAB_FRIEND ||
				m_CurrentUserTab == USERLIST_TAB_CLAN) 
			{
				if( pInfo->_friendState > FRIEND_STATE_OFFLINE)
				{	// �¶��� ���¶��
					if( (m_CurrentUserTab == USERLIST_TAB_FRIEND ||
						m_CurrentUserTab == USERLIST_TAB_CLAN) &&
						GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE)
					{
						if( GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
						{
							UIPhaseReadyRoom_V11* pPhaseReadyRoom = (UIPhaseReadyRoom_V11*)GetMainFrame()->GetCurrentPhase();
							bool bIsRoomUser = pPhaseReadyRoom->IsRoomUser( pInfo->_Nick );
							//bool bIsRoomUser = pPhaseReadyRoom->IsRoomUser( pInfo->_Nick.c_str() );

							if( !bIsRoomUser )
							{//����뿡 ���� ���ٸ�
								GetMainFrame()->AddUserActionCommand( UCM_INVITE);	// �ʴ��ϱ�

								// ��밡 ����뿡 ������ ã�ư��� ����
								if( pInfo->_friendState == FRIEND_STATE_ROOM)
									GetMainFrame()->AddUserActionCommand( UCM_FIND_AWAY);		// ã�ư���	
							}
						}
						else if( pInfo->_friendState == FRIEND_STATE_ROOM)
						{	// �ڽ��� ������� �ƴϰ� ��밡 ������̸� ã�ư��� ����
							GetMainFrame()->AddUserActionCommand( UCM_FIND_AWAY);		// ã�ư���	
						}
					}

					GetMainFrame()->AddUserActionCommand( UCM_WHISPER);			//	�ӼӸ�
				}
			}
			else
			{
				GetMainFrame()->AddUserActionCommand( UCM_WHISPER);			//	�ӼӸ�
			}

			if( m_CurrentUserTab != USERLIST_TAB_BLOCK_V11)
			{
				GetMainFrame()->AddUserActionCommand( UCM_SEND_MAIL);			// ���� ������
				// ������ �ִٸ�..
				if( GetMainFrame()->GetCurrentSubPhaseType() == USP_SHOP)
					GetMainFrame()->AddUserActionCommand( UCM_SEND_PRESENT);		// ���� ������
			}

			if( LocaleValue::Enable("EnableBlock") ) // �����ϱ� ��ư Ȱ��ȭ ����
			{
				if( m_CurrentUserTab == USERLIST_TAB_CURRENT)
					GetMainFrame()->AddUserActionCommand( UCM_ADD_BLOCK);		//	�����ϱ�
				else if(m_CurrentUserTab == USERLIST_TAB_BLOCK_V11)
					GetMainFrame()->AddUserActionCommand( UCM_REMOVE_BLOCK);
			}
			
			// �Ű��ϱ�
			GetMainFrame()->AddUserActionCommand( UCM_USER_REPORT);

			if( m_CurrentUserTab == USERLIST_TAB_FRIEND)
			{
				if( pInfo->_friendState == FRIEND_STATE_HOLD)
				{	// ģ������
					GetMainFrame()->AddUserActionCommand( UCM_ACCEPT_FRIEND);
					// ģ������
					GetMainFrame()->AddUserActionCommand( UCM_REFUSE_FRIEND);
				}
				else if( pInfo->_friendState == FRIEND_STATE_REQUEST || pInfo->_friendState > FRIEND_STATE_HOLD)
				{
					// ģ������
					GetMainFrame()->AddUserActionCommand( UCM_REMOVE_FRIEND);
				}
			}

			// GM �ɼ�â
			if ( UserInfoContext::i()->GetMyRank() == RANK_GAMEMASTER)
			{
				GetMainFrame()->AddUserActionCommand( UCM_GMOPTION);		// Ŭ���ʴ�(CurrentTab)
			}
		}
		
		// ģ�� �˻����� Profile�� �� ��쿡�� �ش�˴ϴ�.
		CCommunityContext::i()->SetFindUser(false);
		
		GetMainFrame()->OpenUserActionPopup( this, slot);

		PlaySound( UISND_ITEM_SELECTING);
	}
}


bool UIFloatUserList_V11::_SetSelectedUser( INT32 slot)
{
	if( slot > -1 && slot < (INT32)m_UserInfoList.size())
	{
		m_RSelectedIndex = slot;

		USERINFO_V11* pUser = m_UserInfoList[ m_RSelectedIndex];
		//i3mem::Copy( &m_SelectedUserInfo, pUser, sizeof( m_SelectedUserInfo));
		pUser->CopyTo(&m_SelectedUserInfo);

		return true;
	}
	else
	{
		m_RSelectedIndex = -1;
		m_SelectedUserInfo.Reset();

		return false;
	}
}

bool UIFloatUserList_V11::_GetUserLocationInfo( CUserLocationInfo & Info)
{
	if( g_pFramework->GetClanContext()->InBattleTeam() )
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( m_RSelectedIndex);
		if (!node)
			return false;
		
		Info = CUserLocationInfo(node->_state);
		return true;
	}

	switch( m_CurrentUserTab )
	{
	case USERLIST_TAB_FRIEND:
		{
			const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( m_RSelectedIndex);
			if (!node)
				return false;

			Info = CUserLocationInfo(node->_state);
		}
		break;

	case USERLIST_TAB_CLAN:
		{
			const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( m_RSelectedIndex);
			if (!node)
				return false;

			Info = CUserLocationInfo(node->_state);
		}
		break;

	case USERLIST_TAB_BLOCK_V11 :
		break;
	}

	return true;
}

void UIFloatUserList_V11::_UpdateCurrentList( void)
{
	INT32 i, j;

	if( GetChat()->IsMaximizeDone() == false )
		return;

	for( i = 0; i < LobbyContext::i()->GetLobbyUserCount(); i++)
	{
		const LOBBY_USER_INFO_LIST * pUser1 = LobbyContext::i()->getLobbyUserInfo( i);

		I3ASSERT( pUser1 != nullptr);
		
		if( pUser1->_Session < 0 )continue;
		if( pUser1->_Nick[0] == 0 )continue;
		if( CCommunityContext::i()->CheckBlockUser(pUser1->_Nick)) continue;	//���� ������ ��� ����Ʈ���� ����

		//�ߺ��� ���������� �˻��մϴ�.
		bool IsValidUser = true;
		for( j = 0; j < i; j++)
		{
			const LOBBY_USER_INFO_LIST * pUser2 = LobbyContext::i()->getLobbyUserInfo( j);
			if( pUser1->_Session == pUser2->_Session)
			{
				IsValidUser = false;
				break;
			}
		}

		//	�������� ������ ��츸 ����Ʈ�� ���
		if(IsValidUser)
		{
			USERINFO_V11 * pInfo = new USERINFO_V11;
			pInfo->Reset();
			pInfo->CopyFromLobbyUser( pUser1);
			m_UserInfoList.push_back( pInfo );

			if (LocaleValue::Enable("EnableNationMark") == TRUE)
			{
				UINT8 nationCode = pInfo->_ui8NationCode;
				UINT8 shapeIdx = LatinAmericaNationInfo::i()->FindNationShapeIdx(nationCode);
				
				m_UserUI_NationMarkData.vShapeIdxList.push_back(shapeIdx);
			}
		}
	}
}

void UIFloatUserList_V11::_UpdateFriendList()
{
	UINT32 i;

	if( GetChat()->IsMaximizeDone() == false )
		return;
	
	for( i = 0; i  < CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( i);
		if(nullptr == node)	continue;

		USERINFO_V11 * pInfo = new USERINFO_V11;
		pInfo->Reset();
		pInfo->CopyFromFriend( node);
		GameUI::GetTextFriendState(pInfo->_szPosition, FRIEND_STATE_64_TO_32(node->_state));

		// ��
		if( FRIEND_STATE_OFFLINE < pInfo->_friendState)
		{	// online
			i3Color::Copy( &pInfo->_NickColor, GameGUI::GetColor(GCT_VALUE));
		}
		else
		{	// offline
			i3Color::Copy( &pInfo->_NickColor, GameGUI::GetColor(GCT_INVALID_C));
		}

		m_UserInfoList.push_back( pInfo );
	}
}

void UIFloatUserList_V11::_UpdateClanMemberList( void)
{
	UINT32 i;

	if( GetChat()->IsMaximizeDone() == false )
		return;

	//I3ASSERT( CCommunityContext::i() != nullptr);

	for( i = 0; i < CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( i);

 		if( node)
		{
			USERINFO_V11 * pInfo = new USERINFO_V11;
			pInfo->Reset();
			pInfo->CopyFromClanUser( node);
			
			if (GET_FRIEND_MATCH(node->_state) != MATCH_TEAM_INVALID_IDX) // Ŭ�� ��ġ ���� �� ��
			{	// Match��
				GameUI::GetTextClanState(pInfo->_szPosition, node->_state);
			}
			else
			{
				GameUI::GetTextFriendState( pInfo->_szPosition, FRIEND_STATE_64_TO_32(node->_state));
			}

			m_UserInfoList.push_back( pInfo );
		}
	}
}

void UIFloatUserList_V11::_UpdateBlockList( void)
{
	if( GetChat()->IsMaximizeDone() == false )
		return;
	//I3ASSERT( CCommunityContext::i() != nullptr);
	for(INT32 i = 0; i < BLOCK_MAX_COUNT; i++)
	{
		const BLOCK_USER* block_user = CCommunityContext::i()->GetBlockUser(i);
		if(block_user->_Activated)
		{
			USERINFO_V11 * pInfo = new USERINFO_V11;
			pInfo->Reset();
			pInfo->CopyFromBlock( block_user );
			m_UserInfoList.push_back( pInfo );
		}
	}

}

void UIFloatUserList_V11::_UpdateUserList( void)
{
	INT32 i;
	LuaState * L;

	if( GetChat()->IsMaximizeDone() == false )
		return;

	_ClearUserList();

	switch( m_CurrentUserTab)
	{
	case USERLIST_TAB_CURRENT :		_UpdateCurrentList();		break;
	case USERLIST_TAB_FRIEND :		_UpdateFriendList();		break;
	case USERLIST_TAB_CLAN :		_UpdateClanMemberList();	break;
	case USERLIST_TAB_BLOCK_V11 :	_UpdateBlockList();			break;
	}

	INT32 count = (INT32)m_UserInfoList.size();



	// Set Slot
	L = _CallLuaFunction( "ClearUserList");
	if( L != nullptr)
	{
		if( count < 6)
			count = 5;

		i3Lua::PushInteger( L, count);
		i3Lua::PushUserData( L, (I3UI_LISTVIEW_CELL_CALLBACK) s_ClanMarkProcessor);
		_EndLuaFunction( L, 2);
	}

	for( i = 0; i < count; i++)
	{
		if( i < (INT32)m_UserInfoList.size())
		{
			USERINFO_V11 * pUser = getUserInfo( i);

			_SetUserInfo(i, pUser);
		}
		else
		{
			_SetUserInfo( i, nullptr);
		}
	}

	// highlight my slot
	_UpdateMyHighlight();
}


INT32 UIFloatUserList_V11::_FindMyDataIndex( void)
{
	for(size_t i = 0; i < m_UserInfoList.size(); i++)
	{
		USERINFO_V11 * pUser = getUserInfo( i);

		if( i3::generic_compare( pUser->_Nick, UserInfoContext::i()->GetMyNickName()) == 0)
			return i;
	}

	return -1;
}

bool UIFloatUserList_V11::_IsMyClanMember( INT64 uid)
{
	for(UINT32 i=0; i<CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetViewClanMember(i);

		if( pNode->_uid == uid)		return true;
	}

	return false;
}

void UIFloatUserList_V11::_UpdateMyHighlight( void)
{
	I3ASSERT( m_pUserListBox != nullptr);

	INT32 myIndex = _FindMyDataIndex();

	if( myIndex != m_SelectedIndex)
	{
		if( m_SelectedIndex != -1)
		{
			m_pUserListBox->SelectData( m_SelectedIndex, false);
			m_SelectedIndex = -1;
		}

		if( myIndex != -1)
		{
			m_pUserListBox->SelectData( myIndex, true);
			m_SelectedIndex = myIndex;
		}
	}
	
}

void UIFloatUserList_V11::SetUserInfo( INT32 slot, LOBBY_USER_INFO_LIST * pInfo)
{
//	if( m_CurrentUserTab == USERLIST_TAB_CURRENT)
		getUserInfo( slot)->CopyToLobbyUser( pInfo);
}

void UIFloatUserList_V11::InputSearchEditBox( UINT32 Param)
{
	m_pEditSearch->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));

	if(Param == EDIT_STATE_RETURN)
		SearchUser();
}

void UIFloatUserList_V11::SearchUser( void)
{
	if (m_pEditSearch->getText() == nullptr)
	{
		// �̸��� �Է� ������ ���
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_EMPTY"));
		return;
	}

	i3::wliteral_range wTempRange = m_pEditSearch->getTextRange();

	//g_pCommunity->SetFindUser(true);
	CCommunityContext::i()->SetFindUser(true);

	if(i3::generic_string_size(wTempRange) > 0)
	{
		CCommunityContext::i()->SetCommnunityInfoNick( wTempRange);

		if(UserInfoContext::i()->IsMyNickName(wTempRange) == true)
		{
			// �ڱ� ������ �����մϴ�
			NEW_FIND_USER_INFO pInfo; //= CCommunityContext::i()->SetFindUserInfo();

			// ���� ����
			pInfo.m_i32State = 0;

			// Ŭ������ ����
			USER_INFO_BASIC	 MyUserInfoBasic;
			UserInfoContext::i()->GetMyUserInfoBasic( &MyUserInfoBasic );

			i3mem::Copy(pInfo.m_strClanName, MyUserInfoBasic.m_strClanName, NET_CLAN_NAME_SIZE );

			pInfo.m_ui8Rank = static_cast<INT8>(MyUserInfoBasic.m_ui32Rank);

			// ���� ����
			const USER_INFO_RECORD* pRecord = UserInfoContext::i()->GetMyUserInfoRecord();
			i3mem::Copy(&pInfo.m_Record, pRecord, sizeof(USER_INFO_RECORD));		
			
			// ��� ����(ĳ����)
			/*USER_INFO_EQUIPMENT_CHARA	EquipCharaInfo;
			g_pGameContext->GetMyUserInfoEquipmentChara( EquipCharaInfo);
			i3mem::Copy(&pInfo->m_Chara, &EquipCharaInfo, sizeof(USER_INFO_EQUIPMENT_CHARA));*/

			// ���� ����
			USER_INFO_EQUIPMENT_WEAPON	EquipWeaponInfo;
			UserInfoContext::i()->GetMyUserInfoEquipmentWeapon( EquipWeaponInfo);
			i3mem::Copy(&pInfo.m_Weapon, &EquipWeaponInfo, sizeof(USER_INFO_EQUIPMENT_WEAPON));

			CCommunityContext::i()->SetFindUserInfo(pInfo);

			GetMainFrame()->OpenPopup( UPW_USERPROFILE);

			m_pEditSearch->SetText(L"");
			i3UI::getGlobalIme()->SetInputText( L"", 0);
		}
		else
		{
			GameEventSender::i()->SetEvent(EVENT_FIND_USER, wTempRange.c_str());
		}		
	}
	else
	{
		// �̸��� �Է� ������ ���
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_EMPTY"));
	}

}

void UIFloatUserList_V11::_AddFriend( INT32 arg )
{
	if (EV_SUCCESSED(arg))
	{
		INT32 realIdx = CCommunityContext::i()->GetViewIndexOfRealFriend(arg);
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(realIdx);			

		if (node)
		{
			if (FRIEND_STATE_HOLD != GET_FRIEND_STATE(node->_state))
			{
				// �ڽ��� ��û�� ���
				i3::rc_wstring wstr_message;
				i3::sprintf(wstr_message, GAME_RCSTRING("STR_TBL_GUI_BASE_NOTICE_ADD_FRIEND_COMPLETE"), node->_wstr_nick);/*[%s]���� ģ����Ͽ� �߰��Ǿ����ϴ�.*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_message);
			}
		}
		else
		{
			I3TRACE("[ERROR] EVENT_INSERT_FRIEND - Invalid friend idx(%d)\n", CCommunityContext::i()->GetRealIndexOfViewFriend(arg));
		}
	}
	else
	{
		i3::rc_wstring wstr_message;

		switch(TESTBIT(arg, EVENT_ERROR_MASK_FAIL))
		{
		case EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST");//������ ã�� �� �����ϴ�.
			break;
		case EVENT_ERROR_EVENT_FRIEND_INSERT_FULL:
			if (FRIEND_MAX_COUNT <= CCommunityContext::i()->GetFriendCount())
			{
				// �ڽ��� ģ���� �߰��� �� ����.
				wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_MORE_GET_FRIEND_STATE");//���̻� ģ���� �߰��� �� �����ϴ�.
			}
			else
			{
				// ��밡 ģ���� �߰��� �� ����.
				wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_REQUEST_FRIEND_BY_LIMIT");//������ �� �̻� ģ���� �߰��� �� ���� �����Դϴ�.
			}
			break;
		case EVENT_ERROR_EVENT_FRIEND_INSERT_QUERY:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST");//������ ã�� �� �����ϴ�.
			break;
		case EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_ALREADY_REGIST_FRIEND_LIST");//�̹� ģ����Ͽ� ��ϵǾ� �ֽ��ϴ�.
			break;				
		case EVENT_ERROR_EVENT_FRIEND_INSERT_NOT_FIND_NICK:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST");//������ ã�� �� �����ϴ�.
			break;
		case EVENT_ERROR_EVENT_FRIEND_INSERT_BLOCK_USER:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_ADD_FRIEND_FAIL_BLOCK");//ģ�� �߰� �� �� ���� �����Դϴ�.
			break;
		default:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST");//������ ã�� �� �����ϴ�.
			break;
		}

		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_message);
	}
}

void UIFloatUserList_V11::_UpdateNationMarkAndEffect()
{
	if (m_pUserListBox == nullptr)
		return;

	INT32 StartVisible_Idx			= m_pUserListBox->GetFirstVisibleIndex();				// ���̴� ù��° ������ �ε���
	REAL32 NationMarkAlpha_Value	= m_UserUI_NationMarkData.rAlphaValue;
	UINT8  NationMarkEffect_Idx		= m_UserUI_NationMarkData.ui8EffectIdx;

	i3::vector<UINT8> vShapeIdxList = m_UserUI_NationMarkData.vShapeIdxList;

	INT32 count = vShapeIdxList.size();
	INT32 ShapeIdx = -1;

	for (int i = 0; i < MAX_NATION_MARK; ++i)
	{
		if (m_NationMark[i].pNation != nullptr)
		{			
			if (count > StartVisible_Idx + i)
			{
				ShapeIdx = vShapeIdxList[StartVisible_Idx + i];
				m_NationMark[i].pNation->EnableCtrl(true);
				m_NationMark[i].pEffect->EnableCtrl(true);;
			}
			else 
			{
				ShapeIdx = -1;
				m_NationMark[i].pNation->EnableCtrl(false);;
			}

			if (ShapeIdx < 0)
				continue;

			COLOR color;
			i3Color::Set(&color, (REAL32)1, (REAL32)1, (REAL32)1, NationMarkAlpha_Value);
			m_NationMark[i].pNation->SetShapeIdx(ShapeIdx);
			m_NationMark[i].pNation->setColor(&color);
		}

		// NationMarkEffect
		if (m_NationMark[i].pEffect != nullptr)
		{
			m_NationMark[i].pEffect->SetShapeIdx(NationMarkEffect_Idx);
		}
	}
}