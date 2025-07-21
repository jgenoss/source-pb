#include "pch.h"
#include "UIPopupUserProfile_V11.h"

#include "../UI/UIUtil.h"
#include "UIFloatChat_V11.h"
#include "UIFloatUserList_V11.h"
#include "UIPopupUserAction_V11.h"

#include "../UI/UIMainFrame_V11.h"
#include "UIPhaseLobby_V11.h"
#include "../UI/UICharaView.h"
//#include "UIPhaseReadyRoom.h"

#include "../GameContextUtil.h"
#include "../CommunityContext.h"
#include "../UserInfoContext.h"
#include "../BattleSlotContext.h"
#include "../ViewOtherUserItemContext.h"
#include "../StageBattle/NetworkContext.h"
#include "../Record.h"
#include "../GameCharaWeaponContext.h"
#include "GameLoginContext.h"
#include "ChannelContext.h"
#include "ClanGameContext.h"

static UIPopupUserProfile_V11 * GetThis( LuaState * L)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	INT32 idx = pFrame->FindPopup( L);
	return (UIPopupUserProfile_V11*) pFrame->GetPopup( (UIPOPUPWIN)idx);
}

extern "C" {
	int UIPopupUserProfile_V11_ClickButton( LuaState * L)		// �˾� �ϴ�(ã�ư���, �ʴ��ϱ�, Ŭ���ʴ�, ģ���߰�, �ݱ�
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		GetThis(L)->ClickButton( idx);
		return 0;
	}

	int UIPopupUserProfile_V11_ChangeTab( LuaState * L)			// ���� <-> ��� (�Ǻ���)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		GetThis(L)->ChangeTab( (UIP_TAB) idx);
		return 0;
	}

	int UIPopupUserProfile_V11_ClickBuyViewItem( LuaState * L)	// ���� ���� ���� ������ ����
	{
		GetThis(L)->BuyViewItem();
		return 0;
	}
	int UIPopupUserProfile_V11_ChangeChara( LuaState * L)
	{
		GetThis(L)->CharacterChange();
		return 0;
	}

}

LuaFuncReg UIPopupUserProfile_V11_Glue[] = {
	{"ClickButton",				UIPopupUserProfile_V11_ClickButton		},
	{"ChangeTab",				UIPopupUserProfile_V11_ChangeTab		},
	{"ClickBuyViewItem",		UIPopupUserProfile_V11_ClickBuyViewItem	},
	{"CharacterChange",			UIPopupUserProfile_V11_ChangeChara		},
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIPopupUserProfile_V11);

UIPopupUserProfile_V11::UIPopupUserProfile_V11()
{
	m_bWaitReceive		= false;
	m_bCreateChara		= false;
	m_bReceivedUserInfo	= false;
	m_CurrentTab		= UIP_TAB_RECORD;
	m_Info				= nullptr;
	m_pCharaViewFrame	= nullptr;

	m_ReadyRoomSlotIndex = 0;
	m_bClanMercenary	= false;
}

UIPopupUserProfile_V11::~UIPopupUserProfile_V11()
{
}

/*virtual*/ void UIPopupUserProfile_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pFrameWnd = (i3UIFrameWnd*) pScene->FindChildByName( "main");

	m_pCharaViewFrame = (i3UIFrameWnd*) pScene->FindChildByName( "fwdCharaView");
	I3ASSERT( m_pCharaViewFrame != nullptr);

	_InitializeUI();

	// Set Chara View Info
	{
		CharaViewMgr::i()->SetCharaViewInfo(this, m_pCharaViewFrame);
	}
}

/*virtual*/ void UIPopupUserProfile_V11::_ClearAtUnload( i3UIScene * pScene)
{
	CharaViewMgr::getView()->RemoveViewChara(true);
	CharaViewMgr::i()->ResetCharaViewInfo(this);

	m_pCharaViewFrame = nullptr;

	UIPopupBase::_ClearAtUnload( pScene);
}

void UIPopupUserProfile_V11::_SetButtonState( bool bClan)
{
	LuaState * L;

	i3::wstring wnick = CCommunityContext::i()->GetCommnunityInfoNick();
	if(wnick.empty())
	{
		I3PRINTLOG(I3LOG_NOTICE, "nick Value is nullptr");
	}
	UINT32 connectState = CCommunityContext::i()->GetFindUserInfo()->m_i32State;

	//	ģ�� ��û�����̸� 'ģ������' ��ư����, ���� ģ���� 'ģ������' ��ư����,
	bool	bTracking = false;		// ã�ư���
	bool	bInvite = false;		// �ʴ��ϱ�
	bool	bClanInvite = false;	// Ŭ���ʴ�
	INT32	FriendState = -1;		// ģ���߰� 0, ģ������/���� 1, ģ������ 2
	bool	bClanwarInvite = false; 

	if( GameUI::IsCommunityFriend( wnick))
	{	// ģ���Դϴ�.

		switch(GET_FRIEND_STATE(connectState))
		{
		//	ģ���� ��� ���
		case FRIEND_STATE_HOLD:			break;
		//	ģ���� ���� ��û �Ǵ� ��������
		case FRIEND_STATE_REQUEST:		FriendState = 1;	break;
		case FRIEND_STATE_OFFLINE:		FriendState = 2;	break;
		//	ģ���� �����
		case FRIEND_STATE_ONLINE:
		case FRIEND_STATE_LOBBY:
			{
				if( bClan)
				{
					const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( m_SelectedIndex);

					if( GET_FRIEND_MATCH( node->_state ) != MATCH_TEAM_INVALID_IDX)
					{// ����� ��Ʋ �� �����϶�
						if( false == g_pFramework->GetClanContext()->InBattleTeam() ) // ���� ���� ������
						{	// ã�ư��� ��
							bTracking = true;
						}
					}
					else
					{// ����� ��Ʋ �� ���� �ƴҶ�
						// �ʴ��ϱ� ��
						if( UIPHASE_READY_ROOM == GetMainFrame()->GetCurrentPhaseType() )
						{
							bInvite = true;
						}
						else
						{
							if( g_pFramework->GetClanContext()->InBattleTeam() )
							{
								bInvite = true;
							}
						}
					}
				}
				else
				{
					FriendState = 2;

					if( false == g_pFramework->GetClanContext()->InBattleTeam() )
					{	// ���� ��Ʋ ���� �ƴ϶��
						if( GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
						{	//	�ڽ��� �濡 �� ������ ģ���ʴ� Ȱ��
							bInvite = true;
						}
					}

					if (i3::generic_string_size(CCommunityContext::i()->GetFindUserInfo()->m_strClanName) == 0)
						bClanInvite = true;
				}
			}
			break;
		//	ģ���� �濡 ������
		case FRIEND_STATE_ROOM:
			{
				if( bClan)
				{
					const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( m_SelectedIndex);

					if( GET_FRIEND_MATCH( node->_state ) == MATCH_TEAM_INVALID_IDX)			// ����� ��Ʋ �� ���°� �ƴҶ�
					{
						if( false == g_pFramework->GetClanContext()->InBattleTeam() )
						{
							bTracking = true;

							//	�ڽ��� �濡 �� ������ ģ���ʴ� Ȱ��
							if( UIPHASE_READY_ROOM == GetMainFrame()->GetCurrentPhaseType())
							{
								bInvite = true;
							}
						}
						else
						{
							bInvite = true;
						}
					}

					if (i3::generic_string_size(CCommunityContext::i()->GetFindUserInfo()->m_strClanName) == 0)
						bClanInvite = true;
				}
				else
				{
					FriendState = 2;

					if( false == g_pFramework->GetClanContext()->InBattleTeam() )
					{	// ���� ��Ʋ ���� �ƴ϶��
						//	�ڽ��� �濡 �� ������ ģ���ʴ� Ȱ��
						if( UIPHASE_READY_ROOM == GetMainFrame()->GetCurrentPhaseType())
						{
							if( gcu::IsSameRoom(connectState) == false)
							{	// �ٸ� ���̴�.
								bInvite = true;
								bTracking = true;
							}
						}
						else
						{	
							bTracking = true;
						}
					}

					if (i3::generic_string_size(CCommunityContext::i()->GetFindUserInfo()->m_strClanName) == 0)
						bClanInvite = true;
				}
			}
			break;
		default:
			break;				
		}
	}
	else
	{	// ģ���� �ƴ�
		if (UserInfoContext::i()->IsMyNickName( wnick) == false)
			FriendState = 0;
		if (i3::generic_string_size(CCommunityContext::i()->GetFindUserInfo()->m_strClanName) == 0)
			bClanInvite = true;

		if (m_bClanMercenary)
		{
			if (ClanGameContext::i()->isMatchTeamLeader())
			{
				bClanwarInvite = true;
			}
		}
	}

	//Ŭ���� �������� Ÿ���� �󼼺��� â���� Ŭ���ʴ� ��ư�� ��µǴ� ���� (PBBUG-16641) �ڽ��� Ŭ�������̰ų� �������϶��� 'Ŭ���ʴ�'��ư Ȱ��ȭ
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	if ( MyUserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_STAFF || MyUserInfoBasic.m_ui32ClanState == CLAN_MEMBER_LEVEL_MASTER)
		bClanInvite = true;
	else
		bClanInvite = false;

	m_BtnState1 = UIP_BTN_STATE_NONE;
	m_BtnState2 = UIP_BTN_STATE_NONE;
	m_BtnState3 = UIP_BTN_STATE_NONE;
	m_BtnState4 = UIP_BTN_STATE_NONE;

	//2012.09.04 �ڱ⼺
	//Ŭ���ʴ� �޴� Ȱ��ȭ. 
	//���� ������ ������ ����, �ִ��� �����.

	if( bTracking)
	{	// ģ���� ��
		if( FriendState == 2)
		{
			// ģ������Ʈtab�� ���
			if( bInvite)
			{	// �ڽŵ� ��
				if(bClanInvite)
				{
					m_BtnState1 = UIP_BTN_STATE_TRACKING;		// ã�ư���
					m_BtnState2 = UIP_BTN_STATE_INVITE;			// �ʴ��ϱ�
					m_BtnState3 = UIP_BTN_STATE_INVITECLAN;		// Ŭ���ʴ�
					m_BtnState4 = UIP_BTN_STATE_REMOVEFRIEND;	// ģ������
				}
				else
				{
					m_BtnState2 = UIP_BTN_STATE_TRACKING;		// ã�ư���
					m_BtnState3 = UIP_BTN_STATE_INVITE;			// �ʴ��ϱ�
					m_BtnState4 = UIP_BTN_STATE_REMOVEFRIEND;	// ģ������
				}
				
			}
			else
			{	// �ڽ��� �κ�
				if(bClanInvite)
				{
					m_BtnState2 = UIP_BTN_STATE_TRACKING;		// ã�ư���
					m_BtnState3 = UIP_BTN_STATE_INVITECLAN;		// Ŭ���ʴ�
					m_BtnState4 = UIP_BTN_STATE_REMOVEFRIEND;	// ģ������
				}
				else
				{
					m_BtnState3 = UIP_BTN_STATE_TRACKING;		// ã�ư���
					m_BtnState4 = UIP_BTN_STATE_REMOVEFRIEND;	// ģ������
				}

			}
		}
		else
		{	// clan list tab
			if( bInvite)
			{
				m_BtnState3 = UIP_BTN_STATE_TRACKING;
				m_BtnState4 = UIP_BTN_STATE_INVITE;
			}
			else
			{
				m_BtnState4 = UIP_BTN_STATE_TRACKING;
			}
		}
	}
	else
	{
		if( bInvite)
		{	// ģ���� �κ�, �ڽ��� ��
			if( FriendState == 2)
			{
				// ģ�� list tab
				if(bClanInvite)
				{
					m_BtnState2 = UIP_BTN_STATE_INVITE;		// �ʴ��ϱ�
					m_BtnState3 = UIP_BTN_STATE_INVITECLAN;		// Ŭ���ʴ�
					m_BtnState4 = UIP_BTN_STATE_REMOVEFRIEND;	// ģ������
				}
				else
				{
					m_BtnState3 = UIP_BTN_STATE_INVITE;		// �ʴ��ϱ�
					m_BtnState4 = UIP_BTN_STATE_REMOVEFRIEND;	// ģ������
				}
				
			}
			else
			{
				m_BtnState4 = UIP_BTN_STATE_INVITE;

				if (m_bClanMercenary == true)
					m_BtnState4 = UIP_BTN_STATE_INVITEMERC;

			}
		}
		else
		{

			if( FriendState == 0)
			{

				if(bClanInvite)
				{
					if (bClanwarInvite)
						m_BtnState2 = UIP_BTN_STATE_INVITEMERC;

					m_BtnState3 = UIP_BTN_STATE_INVITECLAN;	// Ŭ���ʴ�
				}
				else
				{
					if (bClanwarInvite)
						m_BtnState3 = UIP_BTN_STATE_INVITEMERC;
				}
				m_BtnState4 = UIP_BTN_STATE_ADDFRIEND;		// ģ���߰�
			}
			else if( FriendState == 1)
			{	// ��û �����
				// �����ڵ�κ��� bClanInvite �� ������ false �� ������. --> Ŀ����Ƽ ������ �б⹮ ����..
				m_BtnState3 = UIP_BTN_STATE_ACCEPTFRIEND;	// ģ������
				m_BtnState4 = UIP_BTN_STATE_REFUSEFRIEND;	// ģ������
			}
			else if( FriendState == 2)
			{	// Offline
				if(bClanInvite)
				{
					m_BtnState3 = UIP_BTN_STATE_INVITECLAN;
				}
				m_BtnState4 = UIP_BTN_STATE_REMOVEFRIEND;	// ģ������
			}
		}
	}

	L = _CallLuaFunction( "SetButtonState");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, m_BtnState1);
		i3Lua::PushInteger( L, m_BtnState2);
		i3Lua::PushInteger( L, m_BtnState3);
		i3Lua::PushInteger( L, m_BtnState4);
		_EndLuaFunction( L, 4);
	}
}

void UIPopupUserProfile_V11::_InitializeUI( void)
{
	LuaState * L = _CallLuaFunction( "Init");
	_EndLuaFunction( L, 0);

	m_CurrentTab = UIP_TAB_RECORD;
}

bool UIPopupUserProfile_V11::_InitInfoForUserList(void * pArg)
{
	USERACTION_INFO * pInfo = (USERACTION_INFO*) pArg;

	m_UserTab = (USERLIST_TAB) pInfo->_UserTab;
	m_Info = (LOBBY_USER_INFO_LIST*) &pInfo->_tUserInfo;
//	if( m_Info->_Session < 0 )
//		return false;

	I3ASSERT(m_Info);
	i3mem::Copy( &m_CurrentUser, m_Info, sizeof( m_CurrentUser));

	if( i3::generic_compare(m_CurrentUser._Nick, UserInfoContext::i()->GetMyNickName()) == 0)
	{	
		if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_LOBBY)
			m_ReadyRoomSlotIndex = 0;
		else
			m_ReadyRoomSlotIndex = BattleSlotContext::i()->getMySlotIdx();

		m_bReceivedUserInfo = true;
	}
	else
	{
		// �κ񿡼� �ٸ� ������ -1�� �Ǵ��մϴ�.
		m_ReadyRoomSlotIndex = -1;
		//m_bReceivedUserInfo = true;
		//�޾Ҵ��� �� �޾Ҵ��� ������ �𸣸鼭 �� true��
	}

	return true;
}

bool UIPopupUserProfile_V11::_InitinfoForSearchUser(void)
{
	m_UserTab = USERLIST_TAB_CURRENT;
	
	i3::sprintf(m_CurrentUser._ClanName, L"%s", CCommunityContext::i()->GetFindUserInfo()->m_strClanName);
	i3::sprintf(m_CurrentUser._Nick, L"%s", CCommunityContext::i()->GetCommnunityInfoNick());

	m_CurrentUser._Rank = CCommunityContext::i()->GetFindUserInfo()->m_ui8Rank;
	
	// �κ񿡼� �ٸ� ������ -1�� �Ǵ��մϴ�.
	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_LOBBY && UserInfoContext::i()->IsMyNickName(m_CurrentUser._Nick) == false)
		m_ReadyRoomSlotIndex = -1;
	//m_bReceivedUserInfo = true;

	// ���Ҳ���!! ���Ҳ����!!!
	//if( UserInfoContext::i()->IsMyNickName(m_CurrentUser._Nick) != true)
	//{	
	//	// ĳ���� ���� ���� - ��Ǵ뿵���� ������� �� �´���
	//	// �� ��� ����ó���� �ʿ��ұ�
	//	// ����뿡���� ��ġ�� ã�µ� ���⼭ ĳ���� �����ϸ� �ȵȴ�.
	//	//if(UIPHASE_READY_ROOM != GetMainFrame()->GetCurrentPhaseType())
	//	//_SetCharacter();
	//	/*TEAM_TYPE team = (m_ReadyRoomSlotIndex % 2) == 0 ? TEAM_RED : TEAM_BLUE;
	//	CharaViewMgr::i()->UpdateCharacter( m_ReadyRoomSlotIndex, false, WEAPON_SLOT_UNKNOWN, team, m_pFrameWnd);*/
	//}
	
	return true;
}

void UIPopupUserProfile_V11::_SetDefault( void)
{
	// ���� ��ġ/����ǥ��
	i3::wstring position;
	
	UINT32 connectState = 0;
	
	// ģ��ã�� ���
	if(CCommunityContext::i()->IsFindUser())
		connectState = CCommunityContext::i()->GetFindUserInfo()->m_i32State;
	else
		connectState = CCommunityContext::i()->GetCommnunityConnectState();

	GameUI::GetTextFriendState( position, connectState);

	bool bOffLineUser = false;
	
	if (CCommunityContext::i()->IsFindUser() == true)
	{
		bOffLineUser = CCommunityContext::i()->IsFindUserOffLine();
		i3::sprintf(m_CurrentUser._ClanName, L"%s", CCommunityContext::i()->GetFindUserInfo()->m_strClanName);
		i3::sprintf(m_CurrentUser._Nick, L"%s", CCommunityContext::i()->GetCommnunityInfoNick());
		m_CurrentUser._Rank = CCommunityContext::i()->GetFindUserInfo()->m_ui8Rank;
	}
	
	LuaState * L = _CallLuaFunction( "SetDefault");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, bOffLineUser == true ? -1 : m_CurrentUser._Rank); // rank (�������� ������ ��� -1)
		i3Lua::PushStringUTF16To8( L, m_CurrentUser._ClanName);				// clan name
		i3Lua::PushInteger( L, -1);											// clan level
		i3Lua::PushStringUTF16To8( L, m_CurrentUser._Nick);	// �г���
		i3Lua::PushStringUTF16To8( L, position);								// ������ ������
		_EndLuaFunction( L, 5);
	}
}

void UIPopupUserProfile_V11::_SetRecord( void)
{
	LuaState * L;
	USER_INFO_RECORD* pRecord = _GetUserRecordInfo();

	// �뺴 ������ �ִٸ� �뺴����Ʈ���� ��û�� ���̱� ������ �뺴 ������ �������ش�
	if (ClanGameContext::i()->IsExistMercDetailInfo())
	{
		pRecord = const_cast<USER_INFO_RECORD*>(CCommunityContext::i()->GetUserRecordInfo());
	}

	//@eunil. Ŭ�� ä�ο����� Ŭ�� ������ �����ش�  
	ChannelType channelType = ChannelContext::i()->GetConnectedChannelType();
	bool is_clan_channel = (channelType == CHANNEL_TYPE_CLAN || channelType == CHANNEL_TYPE_PCCAFE_CLAN);
	if (is_clan_channel)
	{
	//	pRecord = const_cast<USER_INFO_RECORD*>(CCommunityContext::i()->GetUserRecordInfo());
	}

	if( pRecord != nullptr)
	{
		// ��ü
		INT32 headshotPercent =	TotalHeadShotPercent(pRecord);
		//match,win,lose,kill,death,headshot,killstreak,leave
		L = _CallLuaFunction( "SetTotalRecord");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, pRecord->_match);		// ��
			i3Lua::PushInteger( L, pRecord->_win);			// ��
			i3Lua::PushInteger( L, pRecord->_lose);			// ��
			i3Lua::PushInteger( L, pRecord->_killcount);	// kill
			i3Lua::PushInteger( L, pRecord->_AssistCount);	// assist
			i3Lua::PushInteger( L, pRecord->_death);		// death
			i3Lua::PushInteger( L, headshotPercent);		// headshot
			i3Lua::PushInteger( L, 0);						// kill streak
			i3Lua::PushInteger( L, pRecord->_dis);			// ��Ż
			_EndLuaFunction( L, 9);
		}
		// ���� ���
		INT32 sheadshotPercent = SeasonHeadShotPercent( pRecord);		

		L = _CallLuaFunction( "SetSeasonRecord");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, pRecord->_smatch);		// ��
			i3Lua::PushInteger( L, pRecord->_swin);			// ��
			i3Lua::PushInteger( L, pRecord->_slose);		// ��
			i3Lua::PushInteger( L, pRecord->_skillcount);	// kill
			i3Lua::PushInteger( L, pRecord->_sAssistCount);	// assist
			i3Lua::PushInteger( L, pRecord->_sdeath);		// death
			i3Lua::PushInteger( L, sheadshotPercent);		// headshot
			i3Lua::PushInteger( L, 0);						// kill streak
			i3Lua::PushInteger( L, pRecord->_sdis);			// ��Ż
			_EndLuaFunction( L, 9);
		}
	}
	else
	{
		// ��ü
		L = _CallLuaFunction( "SetTotalRecord");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, 0);		// ��
			i3Lua::PushInteger( L, 0);		// ��
			i3Lua::PushInteger( L, 0);		// ��
			i3Lua::PushInteger( L, 0);		// kill
			i3Lua::PushInteger( L, 0);		// assist
			i3Lua::PushInteger( L, 0);		// death
			i3Lua::PushInteger( L, 0);		// headshot
			i3Lua::PushInteger( L, 0);		// kill streak
			i3Lua::PushInteger( L, 0);		// ��Ż
			_EndLuaFunction( L, 9);
		}

		// ���� ���		
		L = _CallLuaFunction( "SetSeasonRecord");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L,0);		// ��
			i3Lua::PushInteger( L,0);		// ��
			i3Lua::PushInteger( L,0);		// ��
			i3Lua::PushInteger( L,0);		// kill
			i3Lua::PushInteger( L,0);		// assist
			i3Lua::PushInteger( L,0);		// death
			i3Lua::PushInteger( L,0);		// headshot
			i3Lua::PushInteger( L,0);		// kill streak
			i3Lua::PushInteger( L,0);		// ��Ż
			_EndLuaFunction( L, 9);
		}
	}
}

USER_INFO_RECORD * UIPopupUserProfile_V11::_GetUserRecordInfo(void)
{
	USER_INFO_RECORD * pInfo = nullptr;

	if(CCommunityContext::i()->IsFindUser())
	{
		pInfo = const_cast<USER_INFO_RECORD *>(CCommunityContext::i()->GetFindUserRecord());
	}
	else
	{
		if	 (i3::generic_compare(m_Info->_Nick, UserInfoContext::i()->GetMyNickName()) != 0)//)
			pInfo =  const_cast<USER_INFO_RECORD *>(CCommunityContext::i()->GetFindUserRecord());
		else
			pInfo =  const_cast<USER_INFO_RECORD *>(CCommunityContext::i()->GetCommunityRecord());
	}

	return pInfo;
}

void UIPopupUserProfile_V11::_SetEquipment( void)
{

	// bool bItemViewEnable = false;
	USER_INFO_EQUIPMENT_WEAPON  EquipWeapon;
	
	_GetUserEquipWeapon(EquipWeapon);

	static const bool bItemViewEnable = true;
	
	LuaState * L = _CallLuaFunction( "SetEquipment");
	if( L != nullptr)
	{
		struct PrintLua_ItemViewEnable
		{
			static void call(LuaState * L, const USER_INFO_EQUIPMENT_WEAPON& EquipWeapon)
			{
				i3::rc_wstring wszName;
				GameUI::GetWeaponString(wszName, nullptr, nullptr, EquipWeapon.m_TItemID[CHAR_EQUIPMENT_WEAPON_PRIMARY]);
				i3Lua::PushStringUTF16To8(L, wszName);
				GameUI::GetWeaponString(wszName, nullptr, nullptr, EquipWeapon.m_TItemID[CHAR_EQUIPMENT_WEAPON_SECONDARY]);
				i3Lua::PushStringUTF16To8(L, wszName);
				GameUI::GetWeaponString(wszName, nullptr, nullptr, EquipWeapon.m_TItemID[CHAR_EQUIPMENT_WEAPON_MELEE]);
				i3Lua::PushStringUTF16To8(L, wszName);
				GameUI::GetWeaponString(wszName, nullptr, nullptr, EquipWeapon.m_TItemID[CHAR_EQUIPMENT_WEAPON_THROWING1]);
				i3Lua::PushStringUTF16To8(L, wszName);
				GameUI::GetWeaponString(wszName, nullptr, nullptr, EquipWeapon.m_TItemID[CHAR_EQUIPMENT_WEAPON_THROWING2]);
				i3Lua::PushStringUTF16To8(L, wszName);

				// ĳ�� ������
				i3Lua::PushStringUTF16To8(L, GAME_STRING("STR_HEADER_UNKNOWN"));
			}
		};
		
		struct PrintLua_ItemViewDisable
		{
			static void call(LuaState * L, const USER_INFO_EQUIPMENT_WEAPON& EquipWeapon)
			{
				i3Lua::PushStringUTF16To8(L, GAME_STRING("STR_HEADER_UNKNOWN"));
				i3Lua::PushStringUTF16To8(L, GAME_STRING("STR_HEADER_UNKNOWN"));
				i3Lua::PushStringUTF16To8(L, GAME_STRING("STR_HEADER_UNKNOWN"));
				i3Lua::PushStringUTF16To8(L, GAME_STRING("STR_HEADER_UNKNOWN"));
				i3Lua::PushStringUTF16To8(L, GAME_STRING("STR_HEADER_UNKNOWN"));
				i3Lua::PushStringUTF16To8(L, GAME_STRING("STR_HEADER_UNKNOWN"));
			}
		};

		typedef i3::mpl::if_c<bItemViewEnable, PrintLua_ItemViewEnable, PrintLua_ItemViewDisable>::type PrintLuaType;

		PrintLuaType::call(L, EquipWeapon);

		_EndLuaFunction( L, 6);
	}
}

void UIPopupUserProfile_V11::_GetUserEquipWeapon(USER_INFO_EQUIPMENT_WEAPON & EquipInfo)
{
	if(CCommunityContext::i()->IsFindUser())
	{
		EquipInfo = *(CCommunityContext::i()->GetFindUserWeapon());
	}
	else
	{
		if( UserInfoContext::i()->IsMyNickName(m_CurrentUser._Nick) == true)
		{		
			// �ڽ��� �����Դϴ�.
			UserInfoContext::i()->GetMyUserInfoEquipmentWeapon( EquipInfo);
		}
		else
		{
			// ������ ���//
			// ������ ���Ⱑ �������� üũ
			bool viewitemEmpty = ViewOtherUserItemContext::i()->GetData()->m_OtherUserItemIdList.empty();
			for(INT32 i = 0; i < CHAR_EQUIPMENT_WEAPON_COUNT; i++)
			{
				if(viewitemEmpty)
					EquipInfo.m_TItemID[i] = 0;
				else
					EquipInfo.m_TItemID[i] = ViewOtherUserItemContext::i()->GetData()->m_OtherUserItemIdList[i];
			}
			//EquipInfo = *CCommunityContext::i()->GetUserInfoEquipmentWeapon();
		}
	}
}

void UIPopupUserProfile_V11::_ProcessClickButton( UIP_BTN_STATE state)
{
	i3::wstring wstrNick = m_CurrentUser._Nick;

	switch( state)
	{
	case UIP_BTN_STATE_TRACKING :		GameUI::FindAwayUser( m_UserTab, m_SelectedIndex);	break;
	case UIP_BTN_STATE_INVITE :			GameUI::InviteUser( m_UserTab, m_SelectedIndex);	break;
	case UIP_BTN_STATE_ADDFRIEND :		GameUI::AddFriend( wstrNick);						break;
	case UIP_BTN_STATE_ACCEPTFRIEND :	GameUI::AcceptFriend( m_SelectedIndex);				break;
	case UIP_BTN_STATE_REFUSEFRIEND :	GameUI::RefuseFriend( m_SelectedIndex, MAKE_CALLBACK( CbInviteClan), this);		break;
	case UIP_BTN_STATE_REMOVEFRIEND :	GameUI::RemoveFriend( m_SelectedIndex);				break;
	case UIP_BTN_STATE_INVITECLAN :		GameUI::InviteClan( wstrNick, MAKE_CALLBACK( CbInviteClan), this);	break;
	case UIP_BTN_STATE_INVITEMERC:
	{
		T_MerID mID = ClanGameContext::i()->GetSelectedMerID();
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_SEND_INVITE_MERCENARY, &mID);
	}
	break;
	default :
		I3PRINTLOG(I3LOG_FATAL, "not set button state function!!");
		break;
	}
}


/*virtual*/ void UIPopupUserProfile_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// AddScene
	AddScene( "Scene/Popup/PBRe_PopUp_UserInfo.i3UIs", UIPopupUserProfile_V11_Glue);
}

/*virtual*/ bool UIPopupUserProfile_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	{
		// ģ�� �˻����� �˾��� �� ���� �ٸ� ó���� ���־�� �մϴ�.
		if(	CCommunityContext::i()->IsFindUser())
		{
			if(_InitinfoForSearchUser() == false)
				return false;
		}
		else
		{
			if(_InitInfoForUserList(pArg1) == false)
				return false;
		}
	}

	if (ClanGameContext::i()->IsExistMercDetailInfo())
	{
		m_bClanMercenary = true;
		//ClanGameContext::i()->ResetMercDetailInfo(); // reset.
	}
	else
		m_bClanMercenary = false;

	I3PRINTLOG(I3LOG_NOTICE, "UIPopupUserProfile_V11::OnEntranceStart() CurrNick:%s, MyNick:%s\n", m_CurrentUser._Nick, UserInfoContext::i()->GetMyNickName());

	CharaViewMgr::getView()->SetVisibleChara( false);

	UpdateUserInfo();
	return true;
}

/*virtual*/ void UIPopupUserProfile_V11::OnEntranceEnd( void)
{
	UIPopupBase::OnEntranceEnd();

	m_bWaitReceive = true;
	m_bCreateChara = false;
}

/*virtual*/ bool UIPopupUserProfile_V11::OnExitStart( void)
{
	ClanGameContext::i()->ResetMercDetailInfo();

	if( UIPopupBase::OnExitStart() == false)
		return false;

	m_bWaitReceive = false;
	m_bCreateChara = true;

	CharaViewMgr::getView()->SetVisibleChara( false);
	CCommunityContext::i()->SetFindUser(false);

	return true;
}

/*virtual*/ void UIPopupUserProfile_V11::ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	switch(event)
	{
	case EVENT_COMMUNITY_RECORD_SESSION_N:
	case EVENT_COMMUNITY_RECORD_DB_N:
		_SetRecord();	// ����
		break;
	case EVENT_FIND_USER:
		{
			if (EV_SUCCESSED(Arg))
			{
				//if (m_Info == nullptr)
					//return;
				_InitinfoForSearchUser();
			}
			else
			{
				i3::stack_wstring wstrString;

				OnCloseButton();
				GetMainFrame()->TogglePopup(UPW_USERPROFILE);

				// Offline�� �ƴѰ� �����մϴ�.
				if (Arg == EVENT_ERROR_NEW_USER_FIND_FUSER_OFFLINE)
					wstrString = GAME_STRING("STR_TBL_USERSEARCH_DISCONNECT_USER_POPUP");
				else
					wstrString = GAME_STRING("STR_TBL_USERSEARCH_ERROR_POPUP");

				GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrString);
				CCommunityContext::i()->SetFindUser(false);
				return;
			}
		}
		UpdateUserInfo();
	case EVENT_USER_DETAIL_INFO:
	case EVENT_M_RECEIVE_USER_SLOT:
		_SetCharacter();
		_SetEquipment();
		if( isSliding() == false)
		{
			TEAM_TYPE team = (m_ReadyRoomSlotIndex % 2) == 0 ? TEAM_RED : TEAM_BLUE;
			CharaViewMgr::i()->UpdateCharacter( m_ReadyRoomSlotIndex, true, WEAPON_SLOT_UNKNOWN, team, m_pFrameWnd);
		}
		
		m_bReceivedUserInfo = true;
		break;
	}

	UIPopupBase::ProcessGameEvent(event, Arg, UserData); //�� �̰� �ʿ��ϰ���?
}

/*virtual*/ void UIPopupUserProfile_V11::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate( rDeltaSeconds);

	if( m_bReceivedUserInfo )
	{
		if( m_bWaitReceive && (isSliding() == false))
		{	
			TEAM_TYPE team = (m_ReadyRoomSlotIndex % 2) == 0 ? TEAM_RED : TEAM_BLUE;
			CharaViewMgr::i()->UpdateCharacter( m_ReadyRoomSlotIndex, true, WEAPON_SLOT_SECONDARY, team, m_pFrameWnd);
			CharaViewMgr::getView()->SetVisibleChara( true);
			m_bWaitReceive = false;
		}
		if (m_bCreateChara == false && m_bWaitReceive == false && CharaViewMgr::i()->isLoadedChara() == true)
		{
			TEAM_TYPE team = (m_ReadyRoomSlotIndex % 2) == 0 ? TEAM_RED : TEAM_BLUE;
			CharaViewMgr::i()->UpdateCharacter(m_ReadyRoomSlotIndex, true, WEAPON_SLOT_PRIMARY, team, m_pFrameWnd);
			m_bCreateChara = true;
		}
	}
}


/*virtual*/ void UIPopupUserProfile_V11::OnDragStart( i3UIControl * pControl, INT32 x, INT32 y)
{
	I3UI_PICK_INFO info;
	info.init( (REAL32) x, (REAL32) y);

	if( m_pCharaViewFrame->HitTest( &info) )
	{
		CharaViewMgr::i()->SetMouseOldPos(x, y);	
	}
	else
		CharaViewMgr::i()->SetMouseOldPos(0, 0);
}

/*virtual*/ void UIPopupUserProfile_V11::OnDragging( i3UIControl * pControl, INT32 x, INT32 y)
{
	if( m_bInputDisable)
		return;

	I3UI_PICK_INFO info;
	info.init( (REAL32) x, (REAL32) y);

	if( m_pCharaViewFrame->HitTest( &info) )
	{
		CharaViewMgr::i()->RotateCharacter(x, y);		
	}
	else
		CharaViewMgr::i()->SetMouseOldPos(0, 0);

}

/*virtual*/ bool UIPopupUserProfile_V11::OnCloseButton(void)
{
	ClanGameContext::i()->ResetMercDetailInfo();

	return (!CharaViewMgr::i()->isLoadWaiting());
}

void UIPopupUserProfile_V11::UpdateUserInfo( void)
{
	switch( m_UserTab)
	{
	case USERLIST_TAB_CURRENT :		SetUserInfo();			break;
	case USERLIST_TAB_FRIEND :		SetFriendInfo();		break;
	case USERLIST_TAB_CLAN :		SetClanMemberInfo();	break;
	case USERLIST_TAB_BLOCK_V11 :	SetBlockUserInfo();		break;
	}
}

void UIPopupUserProfile_V11::SetUserInfo( void)
{
	_SetDefault();
	_SetButtonState();
	_SetEquipment();
	_SetRecord();
}

void UIPopupUserProfile_V11::SetReadyroomIdx(INT32 RoomIdx)
{
	m_ReadyRoomSlotIndex = RoomIdx;
}

void UIPopupUserProfile_V11::SetFriendInfo( void)
{
	LuaState * L;
	
	_SetDefault();

	// ������
	const USER_INFO_RECORD* info = CCommunityContext::i()->GetCommunityRecord();

	INT32 headshotPercent = TotalHeadShotPercent(info);

	//match,win,lose,kill,death,headshot,killstreak,leave
	L = _CallLuaFunction( "SetTotalRecord");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, info->_match);		// ��
		i3Lua::PushInteger( L, info->_win);			// ��
		i3Lua::PushInteger( L, info->_lose);		// ��
		i3Lua::PushInteger( L, info->_killcount);	// kill
		i3Lua::PushInteger( L, info->_AssistCount); // assist
		i3Lua::PushInteger( L, info->_death);		// death
		i3Lua::PushInteger( L, headshotPercent);	// headshot
		i3Lua::PushInteger( L, 0);					// kill streak
		i3Lua::PushInteger( L, info->_dis);			// ��Ż
		_EndLuaFunction( L, 9);
	}

	// ����
	INT32 sheadshotPercent = SeasonHeadShotPercent(info);
	
	L = _CallLuaFunction( "SetSeasonRecord");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, info->_smatch);		// ��
		i3Lua::PushInteger( L, info->_swin);		// ��
		i3Lua::PushInteger( L, info->_slose);		// ��
		i3Lua::PushInteger( L, info->_skillcount);	// kill
		i3Lua::PushInteger( L, info->_sAssistCount);// assist
		i3Lua::PushInteger( L, info->_sdeath);		// death
		i3Lua::PushInteger( L, sheadshotPercent);	// headshot
		i3Lua::PushInteger( L, 0);					// kill streak
		i3Lua::PushInteger( L, info->_sdis);		// ��Ż
		_EndLuaFunction( L, 9);
	}
	_SetButtonState();
}

void UIPopupUserProfile_V11::SetClanMemberInfo( void)
{
	LuaState * L;

	i3::wstring position;

	const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( m_SelectedIndex);
	// Node�� nullptr�� ���ɴϴ�. �ε����� �� ũ�� �Ѿ���°��� Ȯ�� �ؾ� ��.
	// Jinsik.kim(2015. 03. 31)
	if(node == nullptr ) 
	{
		I3PRINTLOG(I3LOG_NOTICE, "node�� ���� SelectedIndex : %d", m_SelectedIndex);
		return;
	}

	GameUI::GetTextClanState( position, node->_state);

	i3::stack_wstring record;
	i3::stack_wstring killDeath;
	i3::stack_wstring headshot;

	L = _CallLuaFunction( "SetDefault");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, node->_wstr_nick);
		i3Lua::PushStringUTF16To8( L, position);
		_EndLuaFunction( L, 2);
	}
	
	const USER_INFO_RECORD* info = CCommunityContext::i()->GetCommunityRecord();

	i3::sprintf(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_match, info->_win, info->_lose);/*%d�� %d�� %d��*/
	i3::sprintf(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_killcount, info->_death, gcu::getKillRate(info->_killcount, info->_death));/*KILL/DEATH: %d/%d*/

	INT32 headshotPercent = TotalHeadShotPercent(info);

	i3::sprintf(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);/*HEADSHOT: %d%%*/

	L = _CallLuaFunction( "SetTotalRecord");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, record);
		i3Lua::PushStringUTF16To8( L, killDeath);
		i3Lua::PushStringUTF16To8( L, headshot);
		_EndLuaFunction( L, 3);
	}

	i3::sprintf(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_smatch, info->_swin, info->_slose);/*%d�� %d�� %d��*/
	i3::sprintf(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_skillcount, info->_sdeath, gcu::getKillRate(info->_skillcount, info->_sdeath));/*KILL/DEATH: %d/%d*/
	
	INT32 sheadshotPercent = SeasonHeadShotPercent(info);
	
	i3::sprintf(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), sheadshotPercent);/*HEADSHOT: %d%%*/

	L = _CallLuaFunction( "SetSeasonRecord");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, record);
		i3Lua::PushStringUTF16To8( L, killDeath);
		i3Lua::PushStringUTF16To8( L, headshot);
		_EndLuaFunction( L, 3);
	}

	_SetButtonState();
}

void UIPopupUserProfile_V11::SetBlockUserInfo( void)
{
}

bool UIPopupUserProfile_V11::GetCurrentSelectedUserLocation( CUserLocationInfo & UserLocInfo)
{
	if( g_pFramework->GetClanContext()->InBattleTeam() )
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( m_SelectedIndex);
		if (!node)
			return false;
		
		UserLocInfo = CUserLocationInfo(node->_state);
		return true;
	}


	switch( m_UserTab )
	{
	case USERLIST_TAB_FRIEND:
		{
			const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( m_SelectedIndex);
			if (!node)
				return false;

			UserLocInfo = CUserLocationInfo(node->_state);
		}
		break;
	case USERLIST_TAB_CLAN:
		{
			const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( m_SelectedIndex);
			if (!node)
				return false;

			UserLocInfo = CUserLocationInfo(node->_state);
		}
		break;
	}

	return true;
}

void UIPopupUserProfile_V11::CharacterChange()
{
	TEAM_TYPE team = TEAM_RED;
	if( CharaViewMgr::getView()->getTeam() == TEAM_RED)
		team = TEAM_BLUE;

	CharaViewMgr::i()->UpdateCharacter( m_ReadyRoomSlotIndex, false, WEAPON_SLOT_UNKNOWN, team , m_pFrameWnd);
}

void UIPopupUserProfile_V11::ChangeTab( UIP_TAB tab)
{
	LuaState * L;
	bool	bViewEnable = false;

	switch( tab)
	{
	case UIP_TAB_RECORD :
		break;
	case UIP_TAB_EQUIPMENT :
		{
			INT32 ItemViewItemInfo = CASHITEM_ITEMID_VIEW_OTHER_USER_ITEM_INFO;
			if( m_CurrentUser._Session == NetworkContext::i()->GetSessionIndex())
			{
				// �ڽ��� ���
				bViewEnable = true;
			}
			else
			{
				// Ÿ���� ���
				// ������ Ȯ��
				if (CInvenList::i()->IsUsingCashItem(ItemViewItemInfo))
				{
					bViewEnable = true;
				}
			}

			UINT8 Type = 0;
			UINT32 Arg = 0;

			CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, ItemViewItemInfo, &Type, &Arg);

			INT64 i64UID = CInvenList::i()->FindWareDBIdx_ByItemID(ItemViewItemInfo);
			UINT32 Size = sizeof(UINT8);

			ViewOtherUserItemContext::i()->GetData()->m_wstrOtherUserName = BattleSlotContext::i()->getNickForSlot(m_ReadyRoomSlotIndex);

			// ����� �������� �������°Ű����� �����Ѱ�.
			GameEventSender::i()->SetEvent(EVENT_VIEW_OTHER_USER_ITEM_INFO, &i64UID, &Size, &m_ReadyRoomSlotIndex);

			L = _CallLuaFunction( "EnableViewEquipment");
			if( L != nullptr)
			{
				i3Lua::PushBoolean( L, bViewEnable);
				_EndLuaFunction( L, 1);
			}
		}
		break;
	}
}

void UIPopupUserProfile_V11::ClickButton( INT32 idx)
{
	switch( idx)
	{
	case 0 :	_ProcessClickButton( m_BtnState1);		break;
	case 1 :	_ProcessClickButton( m_BtnState2);		break;
	case 2 :	_ProcessClickButton( m_BtnState3);		break;
	case 3 :	_ProcessClickButton( m_BtnState4);		break;
	default :
		I3PRINTLOG(I3LOG_FATAL, "no support this button!!");
		break;
	}
}


void UIPopupUserProfile_V11::AddFriend( void)
{
	if (CCommunityContext::i()->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_NO_MORE_FRIEND"));		//	"���̻� ģ���� �߰��� �� �����ϴ�."
		return;
	}

	if( m_CurrentUser._Session <= 0)		
		return;

	CCommunityContext::i()->InsertFriend( m_CurrentUser._Nick);
}

void  UIPopupUserProfile_V11::BuyViewItem( void)
{
}

void UIPopupUserProfile_V11::DoInviteClan( void)
{
	GameEventSender::i()->SetEvent(EVENT_FIND_USER, m_CurrentUser._Nick);

	g_pFramework->GetClanContext()->SendClanInvite();

	//g_pFramework->GetClanContext()->SendClanInvite( m_CurrentUser._Session, 2);
}

/*static*/ void	UIPopupUserProfile_V11::CbInviteClan(void* pThis, INT32 nParam)
{
	if (MBR_OK == nParam)
	{
		((UIPopupUserProfile_V11*)pThis)->DoInviteClan();
	}
}

void UIPopupUserProfile_V11::_SetCharacter()
{
	INT32 slotIndex = m_ReadyRoomSlotIndex;
	if (slotIndex == -1) slotIndex = 0;

	UI_CHARA_INFO * p_AnotherUserInfo = CCommunityContext::i()->SetAnotherUserInfo(slotIndex);

	const NEW_FIND_USER_INFO * pFindInfo = CCommunityContext::i()->GetFindUserInfo();

	if (GET_FRIEND_STATE(pFindInfo->m_i32State) == FRIEND_STATE_OFFLINE)
		return;

	//���� ���� ��ȯ
	CHARA_PARTS_INFO* p_CharaParts = nullptr;
	for( int i = 0 ; i < TEAM_COUNT ; i++ )
	{
		int temp = g_pEnvSet->IsV2Version() ? TEAM_RED : i;
		p_CharaParts = p_AnotherUserInfo->setCharaParts(i);
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA,	pFindInfo->m_CharaItemID[temp] );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_HEAD,	pFindInfo->m_Chara.GetEquipNum(CHAR_EQUIPMENT_PARTS_HEAD)  );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_FACE,	pFindInfo->m_Chara.GetEquipNum(CHAR_EQUIPMENT_PARTS_FACE)  );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_UPPER,	pFindInfo->m_Chara.GetEquipNum(CHAR_EQUIPMENT_PARTS_UPPER) );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_LOWER,	pFindInfo->m_Chara.GetEquipNum(CHAR_EQUIPMENT_PARTS_LOWER) );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_GLOVE,	pFindInfo->m_Chara.GetEquipNum(CHAR_EQUIPMENT_PARTS_GLOVE) );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_BELT,	pFindInfo->m_Chara.GetEquipNum(CHAR_EQUIPMENT_PARTS_BELT)  );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_HOLSTER, pFindInfo->m_Chara.GetEquipNum(CHAR_EQUIPMENT_PARTS_HOLSTER) );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_SKIN,	pFindInfo->m_Chara.GetEquipNum(CHAR_EQUIPMENT_PARTS_SKIN)  );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_BERET,	pFindInfo->m_Chara.GetEquipNum(CHAR_EQUIPMENT_PARTS_BERET) );
	}

	//���� ���� ��ȯ
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_PRIMARY,		pFindInfo->m_Weapon.m_TItemID[CHAR_EQUIPMENT_PRIMARY]);
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_SECONDARY,   pFindInfo->m_Weapon.m_TItemID[CHAR_EQUIPMENT_SECONDARY]);
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_MELEE,		pFindInfo->m_Weapon.m_TItemID[CHAR_EQUIPMENT_MELEE]);
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_THROWING1,	pFindInfo->m_Weapon.m_TItemID[CHAR_EQUIPMENT_THROWING1]);
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_THROWING2,	pFindInfo->m_Weapon.m_TItemID[CHAR_EQUIPMENT_THROWING2]);
}

