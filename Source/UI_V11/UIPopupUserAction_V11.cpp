#include "pch.h"
#include "UIPopupUserAction_V11.h"

#include "../UI/UIMainFrame_V11.h"
#include "UIPhaseReadyRoom_V11.h"
#include "UIPhaseLobby_V11.h"
#include "UIFloatChat_V11.h"
#include "UIFloatUserList_V11.h"
#include "../UI/UIUtil.h"
#include "../UI/UIFloatMercenaryList.h"

#include "../ClanDef.h"
#include "../CommunityContext.h"
#include "../UserInfoContext.h"
#include "../MyRoomInfoContext.h"
#include "../BattleSlotContext.h"
#include "../ViewOtherUserItemContext.h"
#include "../LobbyContext.h"
#include "../InviteContext.h"
#include "../ClanGameContext.h"
#include "../ChannelContext.h"

static UIPopupUserAction_V11 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	return (UIPopupUserAction_V11*) pFrame->GetPopup( UPW_USERACTION);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupUserAction_V11_Click( LuaState * L)
	{
		INT32 slot = i3Lua::GetIntegerArg( L, 1);
		GetThis()->OnCommand( slot);
		return 0;
	}
}

LuaFuncReg UIPopupUserAction_V11_Glue[] =
{
	{"ClickAction",		UIPopupUserAction_V11_Click},
	{nullptr,				nullptr}
};

//------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIPopupUserAction_V11);

UIPopupUserAction_V11::UIPopupUserAction_V11()
{
	INT32 i;

	m_pObserver = nullptr;
	
	for( i = 0; i < UCM_MAX; i++)
	{
		m_commandText[i].clear();
	}

	m_pListBox = nullptr;

	m_UIStyle = UI_STYLE_NO_CLOSEBUTTON | UI_STYLE_REOPENTYPE;
}

UIPopupUserAction_V11::~UIPopupUserAction_V11()
{
}

/*virtual*/ void UIPopupUserAction_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pListBox = (i3UIListView_Box*) pScene->FindChildByName( "cmdList");
	I3ASSERT( m_pListBox);

	i3Vector::Copy( &m_vOriginSize, m_pListBox->getSize());
}

/*virtual*/ void UIPopupUserAction_V11::_ClearAtUnload( i3UIScene * pScene)
{
	m_pListBox = nullptr;

	UIPopupBase::_ClearAtUnload( pScene);
}

INT32 UIPopupUserAction_V11::_GetCommandIcon( INT32 idx)
{
	switch( m_commandMap[ idx])
	{	
	case UCM_PROFILE :		return 0;
	case UCM_ADD_FRIEND :	return 1;
	case UCM_CLAN_INVITE :	return 2;
	case UCM_SEND_MAIL :	return 3;
	case UCM_ADD_BLOCK :	return 4;
	case UCM_USER_REPORT :	return 5;
	case UCM_FIND_AWAY :	return 6;
	case UCM_SEND_PRESENT : return 7;
	case UCM_REMOVE_FRIEND : return 8;

	case UCM_REMOVE_BLOCK : return 8;
	case UCM_KICK :			return 8;
	case UCM_WHISPER :		return 3;
	case UCM_INVITE :		return 6;
	case UCM_ACCEPT_FRIEND : return 1;
	case UCM_REFUSE_FRIEND : return 8;
	case UCM_TRANSFER_HOST : return 13;
	case UCM_VIEW_OTHER_USER_ITEM_INFO : return 14;
	}

	return -1;
}

void UIPopupUserAction_V11::_ExecuteCommand( INT32 cmd)
{
	I3ASSERT( cmd < (INT32) m_commandCount);

	switch( m_commandMap[cmd])
	{
		case UCM_PROFILE		:	_CmdViewUserInfo();		break;
		case UCM_ADD_FRIEND		:	_CmdAddFriend();		break;
		case UCM_CLAN_INVITE	:	_CmdInviteClan();		break;
		case UCM_SEND_MAIL :		_CmdSendMail();		break;
		case UCM_ADD_BLOCK		:	_CmdAddBlock();		break;
		case UCM_REMOVE_BLOCK :		_CmdRemoveBlock();	break;
		case UCM_USER_REPORT :		_CmdReportUser();		break;
		case UCM_FIND_AWAY :		GameUI::FindAwayUser( m_RequestInfo._UserTab, m_RequestInfo._CommunityIndex);		break;
		case UCM_SEND_PRESENT :		_CmdSendPresent();	break;
		case UCM_REMOVE_FRIEND :	GameUI::RemoveFriend( m_RequestInfo._CommunityIndex);			break;
		case UCM_KICK :				_CmdKickUser( m_RequestInfo._CommunityIndex);		break;
		case UCM_INVITE :			GameUI::InviteUser( m_RequestInfo._UserTab, m_RequestInfo._CommunityIndex);		break;
		case UCM_WHISPER		:	_CmdWhispToUser();		break;
		case UCM_ACCEPT_FRIEND :	GameUI::AcceptFriend( m_RequestInfo._CommunityIndex);				break;
		case UCM_REFUSE_FRIEND :	GameUI::RefuseFriend( m_RequestInfo._CommunityIndex, MAKE_CALLBACK(CbDeleteFriend), this);	break;
		case UCM_TRANSFER_HOST :	GameEventSender::i()->SetEvent( EVENT_TRANSFER_HOST, &m_RequestInfo._CommunityIndex);	break;	// ���� �ѱ��
		case UCM_VIEW_OTHER_USER_ITEM_INFO: _CmdViewOtherItem( m_RequestInfo._tUserInfo._Nick); break;
		case UCM_GMOPTION :			_CmdGMOption(); break;
		default :
			I3PRINTLOG(I3LOG_WARN, "Not yet made functions..!!! call swoongoo!!");
			break;
	}
}

void UIPopupUserAction_V11::_CmdViewUserInfo( void)
{	
#if !defined (TEST_LOBBY_USER_LIST)
	if( m_RequestInfo._tUserInfo._Nick[0] == 0) 
		return;

	// ���� ���� ����
	CCommunityContext::i()->SetCommnunityInfoNick( m_RequestInfo._tUserInfo._Nick);

	if( i3::generic_compare(m_RequestInfo._tUserInfo._Nick, UserInfoContext::i()->GetMyNickName()) == 0)
	{
		// �ڽ��� ������ ���� �����Ѵ�.
		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic( &MyUserInfoBasic );

		USER_INFO_RECORD* pTarget = CCommunityContext::i()->SetCommnuitRecord();
		const USER_INFO_RECORD* pSource = UserInfoContext::i()->GetMyUserInfoRecord();
		i3mem::Copy(pTarget, pSource, sizeof(USER_INFO_RECORD));		

		UIPhaseBase * pUIPhase = GetMainFrame()->GetCurrentPhase();
		if( i3::same_of<UIPhaseReadyRoom_V11*>(pUIPhase))
		{
			i3::safe_string_copy( m_RequestInfo._tUserInfo._ClanName, MyUserInfoBasic.m_strClanName, 
				sizeof( m_RequestInfo._tUserInfo._ClanName ) );
		}

		CCommunityContext::i()->SetCommunityUserInfo( true);
	}
	else
	{
		// �ٸ� ������ ������ ��û�Ѵ�.
		UIPhaseBase * cStage = GetMainFrame()->GetCurrentPhase();
		CCommunityContext::i()->SetCommunityUserInfo( false);


		// �뺴����Ʈ�� ���, �� �̻� �Ѿ�� ����ϹǷ� �̸� ó�����ش�.
		if (i3::same_of<UIFloatMercenaryList*>(m_pObserver))
		{
			GameEventSender::i()->SetEvent(EVENT_FIND_USER, &m_RequestInfo._tUserInfo._Nick);
			m_RequestInfo._UserTab = USERLIST_TAB_CURRENT;

			if (GetMainFrame()->IsOpenedPopup(UPW_USERPROFILE) == false)
				GetMainFrame()->TogglePopup(UPW_USERPROFILE, &m_RequestInfo);
			return;
		}

		/// �׳� ���� ��ġ�� �ҷ��� �̷��� �����׿� // �׷��� �ȵſ�...
		//CCommunityContext::i()->SetFindUser(true);

		if(!i3::same_of<UIPhaseReadyRoom_V11*>(cStage))
		{
			if(m_RequestInfo._tUserInfo._Session < 0 )
				return;
			GameEventSender::i()->SetEvent(EVENT_FIND_USER, &m_RequestInfo._tUserInfo._Nick);
			GameEventSender::i()->SetEvent(EVENT_USER_DETAIL_INFO, & m_RequestInfo._tUserInfo._Session);

			m_RequestInfo._SlotIdx = 0;
		}
		else
		{
		
			INT32 idx = ((UIPhaseReadyRoom_V11 *)cStage)->getSelectedSlot();
			//GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_N, &idx);
			// ����뵵 ������ ���� ���������� �ʿ��ϱ� ������ ���� ���� �������� ����մϴ�.
			GameEventSender::i()->SetEvent(EVENT_FIND_USER, &m_RequestInfo._tUserInfo._Nick);
			
			m_RequestInfo._SlotIdx = idx;

			// ������� OnEntranceStart() �� �ƴ϶� �̰����� Ŭ���̸��� �����ؾ���
			const SLOT_INFO*	pSlotInfo	= BattleSlotContext::i()->getSlotInfo( idx );

			i3::safe_string_copy( m_RequestInfo._tUserInfo._ClanName, pSlotInfo->_ClanName, 
				sizeof( m_RequestInfo._tUserInfo._ClanName ) );

		}


		//@eunil. Ŭ�� ä�ο����� Ŭ�� ���� ��û�մϴ�. //�ش� �۾� ���� �߰� ���� 16.12.29
		ChannelType channelType = ChannelContext::i()->GetConnectedChannelType();
		bool is_clan_channel = (channelType == CHANNEL_TYPE_CLAN || channelType == CHANNEL_TYPE_PCCAFE_CLAN);
		if (is_clan_channel)
		{
			//CCommunityContext::i()->get;
			//INT64 i64ReceiverUID = GameUI::FindUserUidByNick(wstrTarget);
		}
	}

	//	�̸� ������ �˾��� ����.
	if( GetMainFrame()->IsOpenedPopup( UPW_USERPROFILE) == false)
		GetMainFrame()->TogglePopup(  UPW_USERPROFILE, (void*)&m_RequestInfo);

#endif
}

void UIPopupUserAction_V11::_CmdAddFriend( void)
{
	if( m_RequestInfo._tUserInfo._Nick[0] == 0) 
		return;
	const i3::rc_wstring wstrMessage = i3::format_rcstring(GAME_RCSTRING("STR_UI_POPUP_ADD_FRIEND_DETAIL"),  m_RequestInfo._tUserInfo._Nick);
	GameUI::MsgBox(MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(CbAddFriend), this );
}

void UIPopupUserAction_V11::_CmdInviteClan( void)
{

	//if( m_RequestInfo._tUserInfo._Session < 0 || m_RequestInfo._tUserInfo._Nick[0] == 0) 
	//	return;
	if( m_RequestInfo._tUserInfo._Nick[0] == 0) 
		return;

	// Ŭ�� �ʴ븦 ������ ���� ����� ã���ϴ�.
	CCommunityContext::i()->SetFindUser(false);
	GameEventSender::i()->SetEvent(EVENT_FIND_USER, m_RequestInfo._tUserInfo._Nick);

	GameUI::InviteClan( m_RequestInfo._tUserInfo._Nick, MAKE_CALLBACK(CbClanInvite), this);
}

void UIPopupUserAction_V11::_CmdSendMail( void)
{
	GetMainFrame()->TogglePopup( UPW_SENDMAIL, (void*) m_RequestInfo._tUserInfo._Nick);
}

void UIPopupUserAction_V11::_CmdReportUser( void)
{
}

void UIPopupUserAction_V11::_CmdWhispToUser( void)
{
	if(m_RequestInfo._tUserInfo._Nick[0] == 0)
		return;

	UIFloatChat_V11 * pChat = (UIFloatChat_V11*) GetMainFrame()->GetFloating( UFW_CHAT);
	if( pChat != nullptr)
	{
		pChat->OnWhisper( m_RequestInfo._tUserInfo._Nick);	
	}
}

void UIPopupUserAction_V11::_CmdSendPresent( void)
{
}

//���� �߰��ϱ�
void UIPopupUserAction_V11::_CmdAddBlock( void)
{
	GetMainFrame()->TogglePopup( UPW_ADDBLOCK, &m_RequestInfo._tUserInfo._Nick);
	
}

//���� �����ϱ�
void UIPopupUserAction_V11::_CmdRemoveBlock( void)
{
	//���� ���õ� ������ ������ �����´�.
	//INT32 _BlockCount = g_pCommunity->GetBlockUserCount();

	TTCHAR* Select_User = m_RequestInfo._tUserInfo._Nick;
	for(INT32 i=0; i < BLOCK_MAX_COUNT; i++)
	{
		const BLOCK_USER* t_Block = CCommunityContext::i()->GetBlockUser(i);
		if( i3::generic_compare(t_Block->_wstrNick, Select_User) == 0)
		{
			GameEventSender::i()->SetEvent(EVENT_DELETE_BLOCK, (const void *)&(t_Block->_blockInfo._Idx));
			return;
		}
	}
}

void UIPopupUserAction_V11::_CmdViewOtherItem( TTCHAR * username)
{
	INT32 ItemViewItemInfo = CASHITEM_ITEMID_VIEW_OTHER_USER_ITEM_INFO;
	
	if (CInvenList::i()->IsUsingCashItem(ItemViewItemInfo))
	{
		INT64 i64UID = CInvenList::i()->FindWareDBIdx_ByItemID(ItemViewItemInfo);
		UINT32 Size = sizeof(UINT8);

		UINT8 Type = 0;
		UINT32 Arg = 0;

		UIPhaseBase * pReadyRoom = GetMainFrame()->GetCurrentPhase();
		if( i3::same_of<UIPhaseReadyRoom_V11*>(pReadyRoom))
		{
			UIPhaseReadyRoom_V11* phase = static_cast<UIPhaseReadyRoom_V11*>(pReadyRoom);
			INT32 ReadyRoomSlotIndex = phase->getSelectedSlot();

			CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, ItemViewItemInfo, &Type, &Arg);
		
			i3::rc_wstring wstr_selected_user_name = BattleSlotContext::i()->getNickForSlot(ReadyRoomSlotIndex);
			ViewOtherUserItemContext::i()->GetData()->m_wstrOtherUserName = wstr_selected_user_name;
			
			GameEventSender::i()->SetEvent(EVENT_VIEW_OTHER_USER_ITEM_INFO, &i64UID, &Size, &ReadyRoomSlotIndex);
		}
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_GUI_VIEW_OTHER_USER_ITEM_INFO"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"), MAKE_CALLBACK(CbMoveShop), this);
	}
}

void UIPopupUserAction_V11::_CmdKickUser( INT32 idxSlot)
{
	// �Ʒ��� �� Ư�� �����̻��� ���� ���� �ʵ��� �մϴ�.
	if (MyRoomInfoContext::i()->getStage()->IsAiMode())
	{
		if (8 <= idxSlot)
		{
			return;
		}
	}

	INT32 slotFlag = 0x00000000;
	const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo(idxSlot);

	//	Ŭ������ �ƴ� ��츸 �����ϵ���
#if defined( TEST_CLAN_BATTLE)
	if(idxSlot >= 0)
		return;
#else
	if( g_pFramework->GetClanContext()->InBattleTeam())
		return;
#endif

#if defined( BUILD_RELEASE_QA_VERSION ) && defined( TEMP_QUEST_REWARD)
	// TODO: 4��4 ��忡���� �Ʒ� 8�� ������ �ݾƾ��ϳ�, �Ϲݸ�忡�� ������
	// ����ڰ� �Ʒ� ������ �����ϰ� ���� ��� ������ ���ǵǾ� ���� �ʴ�.
	// ���� ���ö����� ������ ���´�.

	// �Ʒ��� & 4vs4�̸� ���� 8�� ������ �۵� ����
	UINT32 StageID = g_pGameContext->GetMyRoomStageID();

	if (STAGE_SLOT_MODE_4VS4 == getStageSlot(StageID) || g_pGameContext->IsStageMaskTraining())
	{
		if (8 <= idxSlot)
		{
			return;
		}
	}
#endif

	// ���������� ���� �÷��� ����
	if (SLOT_STATE_CLOSE == pInfo->_State)
	{
		slotFlag = 0x10000000;
	}

	slotFlag |= idxSlot;
	GameEventSender::i()->SetEvent(EVENT_CHANGE_SLOT_N, &slotFlag);
}


/*virtual*/ void UIPopupUserAction_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Popup/pb_ChattingWindow_UserList_PopUp.i3UIs", UIPopupUserAction_V11_Glue);

	m_pIconTemplate = i3UI::FindTemplateByName( "ButtonImageSet_UserList_Icon");
	I3ASSERT( m_pIconTemplate != nullptr);
}

/*virtual*/ void UIPopupUserAction_V11::OnReset( void)
{
	m_commandCount = 0;
	m_pObserver = nullptr;
}



/*virtual*/ bool UIPopupUserAction_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	// �˾��� ������ �ȵǴ� ��Ȳ..
	i3EventReceiver* observer = static_cast<i3EventReceiver*>(pArg1);
	INT32 UserSlot = *static_cast<INT32*>(pArg2);

	if (0 == m_commandCount)	return false;
	if (nullptr == observer)		return false;

	m_pObserver = observer;

	//�̰��� ���� ������ �ٽ� ������...

	if( isEnable())
	{
		SetEnable( false);

		// ���� �޸𸮿� �ִ� Focus�� �ʱ�ȭ..
		// �ڱ� �ڽ��� �ٽ� ������ ��� �޸𸮴� �����Ǿ����� Framework�� Focus�� ó���� �ȵǰ� �ִ� ��Ȳ.
		g_pFramework->ResetFocus();
	}

	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( i3::same_of<UIFloatUserList_V11*>(m_pObserver))
	{
		UIFloatUserList_V11 * pUserList = (UIFloatUserList_V11*) m_pObserver;

		pUserList->SetUserInfo( UserSlot, &m_RequestInfo._tUserInfo);

		m_RequestInfo._CommunityIndex = UserSlot;
		m_RequestInfo._UserTab = pUserList->getCurrentUserTab();
	}
	else if (i3::same_of<UIFloatMercenaryList*>(m_pObserver))
	{
		UIFloatMercenaryList* pMercList = static_cast<UIFloatMercenaryList*>(m_pObserver);

		if (pMercList == nullptr)
			return false;
		else
		{
			const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();
			const CLAN_MERCENARY_INFO *pMercInfo = ClanGameContext::i()->getClanMercenaryInfo(UserSlot);

			i3::generic_string_copy(m_RequestInfo._tUserInfo._Nick, pInfo->_UserInfo.m_nickName);
			i3::generic_string_copy(m_RequestInfo._tUserInfo._ClanName, pInfo->_UserInfo.m_strClanName);

			m_RequestInfo._tUserInfo._ClanMark = pInfo->_UserInfo.m_clanMark;
			m_RequestInfo._tUserInfo._NickColor = pInfo->_UserInfo.m_nickColor;
			m_RequestInfo._tUserInfo._Rank = (UINT8)ClanGameContext::i()->GetMercIconIdx(pMercInfo);

			m_RequestInfo._CommunityIndex = UserSlot;
			m_RequestInfo._SlotIdx = UserSlot;
		}
	}
	else if( i3::same_of<UIPhaseReadyRoom_V11*>(m_pObserver))
	{
		UIPhaseReadyRoom_V11 * pPhaseReadyRoom = (UIPhaseReadyRoom_V11*) m_pObserver;

		const SLOT_INFO * pSlotInfo = BattleSlotContext::i()->getSlotInfo( UserSlot);
		
		m_RequestInfo._tUserInfo._ClanMark = pSlotInfo->_clanMark;
		m_RequestInfo._tUserInfo._eSportID = pSlotInfo->_eSportID;
		i3::generic_string_copy( m_RequestInfo._tUserInfo._Nick , BattleSlotContext::i()->getNickForSlot( UserSlot));
		
		m_RequestInfo._tUserInfo._NickColor = BattleSlotContext::i()->getNickColorForSlot( UserSlot);
		m_RequestInfo._tUserInfo._Rank = pSlotInfo->_Rank;

		m_RequestInfo._UserTab = USERLIST_TAB_CURRENT;
		m_RequestInfo._CommunityIndex = pPhaseReadyRoom->getSelectedSlot();
	}
	
	// data �������� Size�� ��������� Scrollbar�� �ȳ��´�.
	m_pListBox->setSize( &m_vOriginSize);

	LuaState * L = _CallLuaFunction( "ClearList");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, m_commandCount);
		_EndLuaFunction( L, 1);
	}

	for(INT32 i = 0; i < m_commandCount; i++)
	{
		L = _CallLuaFunction( "SetSlotInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, i);
			i3Lua::PushInteger( L, _GetCommandIcon( i));
			i3Lua::PushStringUTF16To8( L, m_commandText[ i]);
			_EndLuaFunction( L, 3);
		}
	}

	return true;
}

void UIPopupUserAction_V11::ResetCommand( void)
{
	m_commandCount = 0;
	m_pObserver = nullptr;
}

void UIPopupUserAction_V11::AddCommand( USER_CONTEXT_MENU cmd)
{
	I3ASSERT( m_commandCount < UCM_MAX);

	// �̰��� ���� ����� �����Ƿ� �����մϴ�. swoongoo
	// �Ű��ϱ� ��� ���ܷ� ���� �� �Ʒ��ڵ嵵 �ּ�ó��.  (16.07.29.����)
	if( cmd == UCM_USER_REPORT)
		return;

	switch( cmd)
	{
	case UCM_TRANSFER_HOST:					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_GIVE_MASTER"), MAX_STRING_COUNT);		break;	// ����ѱ��
	case UCM_KICK:							i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_FORCE_GET_AWAY"), MAX_STRING_COUNT);	break;	// ��������
	case UCM_PROFILE:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_DETAIL_INFO"), MAX_STRING_COUNT);		break;	// ������
	case UCM_WHISPER:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_WHISPER"), MAX_STRING_COUNT);			break;	// �ӼӸ�
	case UCM_INVITE:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_INVITE"), MAX_STRING_COUNT);			break;	// �ʴ��ϱ�
	case UCM_FIND_AWAY:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_FIND_OUT"), MAX_STRING_COUNT);			break;	// ã�ư���
	case UCM_ADD_FRIEND:					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_ADD_FRIEND"), MAX_STRING_COUNT);		break;	// ģ���߰�
	case UCM_ADD_BLOCK:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_CUT_OFF"), MAX_STRING_COUNT);			break;	// �����ϱ�
	case UCM_REMOVE_BLOCK :					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_REMOVE_BLOCK"), MAX_STRING_COUNT);		break;	// ���ܻ���
	case UCM_CLAN_INVITE:					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_CLAN_INVITE"), MAX_STRING_COUNT);		break;	// Ŭ���ʴ�
//	case UCM_USER_REPORT :					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_USER_REPORT"), MAX_STRING_COUNT);		break;	// �Ű��ϱ�
	case UCM_SEND_MAIL :					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_SEND_MAIL"), MAX_STRING_COUNT);			break;	// ���� ������
	case UCM_REMOVE_FRIEND:					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_REMOVE_FRIEND"), MAX_STRING_COUNT);		break;	// ģ�� ����
	case UCM_SEND_PRESENT :					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_SEND_PRESENT"), MAX_STRING_COUNT);		break;	// ���� ������
	case UCM_ACCEPT_FRIEND :				i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_ACCEPT_FRIEND"), MAX_STRING_COUNT);		break;	// ģ�� ����
	case UCM_REFUSE_FRIEND :				i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_POPUP_FRIEND_REFUSE"), MAX_STRING_COUNT);					break;	// ģ������
	case UCM_VIEW_OTHER_USER_ITEM_INFO :	i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_OTHERUSER_INFORMATION"), MAX_STRING_COUNT);				break;	// ���� ������ ����
	case UCM_GMOPTION :						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_GM_POPUP_INFO"), MAX_STRING_COUNT);				break;	// GM �ɼ�

	default:
		I3PRINTLOG(I3LOG_FATAL, "Not yet setting command!!");
		break;
	}

	m_commandMap[m_commandCount] = cmd;
	m_commandCount++;
}


void UIPopupUserAction_V11::OnCommand( INT32 cmd)
{
	GetMainFrame()->ClosePopup( UPW_USERACTION);

	if( cmd < (INT32) m_commandCount)
	{
		if (m_pObserver)
		{
			USERACTION_INFO info;

			info._Command = m_commandMap[cmd];

			m_pObserver->OnEvent( I3_EVT_USER_ACTION , this, (UINT32) &info, I3_EVT_CODE_ACTIVATE);
		}

		_ExecuteCommand( cmd);
	}

	
	PlaySound( UISND_ITEM_SELECTING);
}

void UIPopupUserAction_V11::DoClanInvite( void)
{
	//	Ŭ�� �ʴ븦 ������.
	//@eunil. 1.0, 1.5�� ���� ������� ����. ���ÿ� ��Ŷ�� ������ �������� ������ ã������ Ŭ�� �ʴ븦 ������ ����� �������� ����

	//if( i3::same_of<UIFloatUserList_V11*>(m_pObserver))
	//{
	//	UIFloatUserList_V11 * pUserList = (UIFloatUserList_V11*) m_pObserver;

	//	// Ŭ�� �ʴ�� Find User�� �Ͽ� �������� UID�� ã�� ���¿��� �ؾ� �մϴ�. 1.5���� (jinsik.kim)
	//	if( pUserList->getCurrentUserTab() == USERLIST_TAB_CURRENT)
	//	{
	//		GameEventSender::i()->SetEvent(EVENT_FIND_USER, m_RequestInfo._tUserInfo._Nick);
	//		g_pFramework->GetClanContext()->SendClanInvite();
	//	}

	//	if( pUserList->getCurrentUserTab() == USERLIST_TAB_FRIEND)
	//	{
	//		//const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriendByNick(m_RequestInfo._tUserInfo._Nick);
	//		GameEventSender::i()->SetEvent(EVENT_FIND_USER, m_RequestInfo._tUserInfo._Nick);
	//		//g_pFramework->GetClanContext()->SendClanInviteUID(node->_uid, 0);
	//		g_pFramework->GetClanContext()->SendClanInvite();
	//	}
	//}
	//else if( i3::same_of<UIPhaseReadyRoom_V11*>(m_pObserver))
	//{
	//	if(i3::generic_compare(m_RequestInfo._tUserInfo._Nick, UserInfoContext::i()->GetMyNickName()) != 0)
	//	{
	//		/*m_RequestInfo._SlotIdx = ((UIPhaseReadyRoom_V11 *)m_pObserver)->getSelectedSlot();
	//		g_pFramework->GetClanContext()->SendClanInviteAccept( m_RequestInfo._SlotIdx , 1);*/
	//		GameEventSender::i()->SetEvent(EVENT_FIND_USER, m_RequestInfo._tUserInfo._Nick);
	//		g_pFramework->GetClanContext()->SendClanInvite();
	//	}
	//}
	//else if (i3::same_of<UIFloatMercenaryList*>(m_pObserver))
	//{
	//	if (i3::generic_compare(m_RequestInfo._tUserInfo._Nick, UserInfoContext::i()->GetMyNickName()) != 0)
	//	{
	//		/*m_RequestInfo._SlotIdx = ((UIPhaseReadyRoom_V11 *)m_pObserver)->getSelectedSlot();
	//		g_pFramework->GetClanContext()->SendClanInviteAccept( m_RequestInfo._SlotIdx , 1);*/
	//		GameEventSender::i()->SetEvent(EVENT_FIND_USER, m_RequestInfo._tUserInfo._Nick);
	//		g_pFramework->GetClanContext()->SendClanInvite();
	//	}
	//}

	g_pFramework->GetClanContext()->SendClanInvite();
}
//
//void UIPopupUserAction_V11::CbBlockUser( void* pThis, INT32 nParam)
//{
//	if (MBR_OK == nParam)
//	{
//		((UIPopupAddBlockList*)pThis)->DoBlockUser();
//	}
//}

void UIPopupUserAction_V11::CbClanInvite( void* pThis, INT32 nParam)
{
	if (MBR_OK == nParam)
	{
		((UIPopupUserAction_V11*)pThis)->DoClanInvite();
	}
}

void UIPopupUserAction_V11::CbDeleteFriend( void* pThis,INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		UIPopupUserAction_V11* userlist = (UIPopupUserAction_V11*)pThis;

		if (0 > userlist->m_RequestInfo._CommunityIndex || false == CCommunityContext::i()->DeleteViewFriend( userlist->m_RequestInfo._CommunityIndex))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_DELETE"));/*������ ģ���� �����ϼž��մϴ�.*/
		}
		else
		{
			userlist->m_RequestInfo._CommunityIndex = -1;
			CCommunityContext::i()->SetCommunityUserInfo(false);
		}
	}
}

void UIPopupUserAction_V11::CbMoveShop( void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		/*UIPhaseReadyRoom_V11* pReadyRoom = (UIPhaseReadyRoom_V11*)pThis;
		pReadyRoom->GetMainFrame()->ChangeSubPhase( USP_SHOP);*/
	}
}

void UIPopupUserAction_V11::CbAddFriend(void* pThis, INT32 nParam)
{
	if(nParam == MBR_OK)
	{
		UIPopupUserAction_V11* userlist = (UIPopupUserAction_V11*)pThis;
		if(userlist != nullptr)
		{
			GameUI::AddFriend( userlist->m_RequestInfo._tUserInfo._Nick);
		}
	}
}

void UIPopupUserAction_V11::OnEntranceEnd( void )
{
	REAL32 MaxWidth = _ResizeListView();
	
	_ReposFrameWnd(MaxWidth);

	_ReSizeAndPosIcon();
}

REAL32 UIPopupUserAction_V11::_ResizeListView()
{
	// ���� �� Text�� Width�� ����Ѵ�.
	REAL32 iconWidth = 0.f;
	REAL32 MaxWidth = 0.f;

	i3::vector<i3UIListView_Item*>& item_list = m_pListBox->getItemList();

	for(size_t i = 0; i < item_list.size(); i++)
	{
		i3UIListView_Item * pItem = item_list[ i];

		i3UIListView_Cell * pCell = pItem->getCell( 1);

		iconWidth = m_pIconTemplate->getShape(0)->getWidth() + 3;

		// Text�� ���� ������
		if( pCell->getUIText() != nullptr)
		{
			if( MaxWidth < pCell->getUIText()->getExtentWidth() + 4)
				MaxWidth = (REAL32) pCell->getUIText()->getExtentWidth() + 4;
		}

		i3UIControl * pChild = pCell->getCellChild();
		if( i3::kind_of<i3UICaptionControl*>(pChild))
		{
			if( ((i3UICaptionControl*) pChild)->getUIText() )
			{
				if( MaxWidth < ((i3UICaptionControl*) pChild)->getUIText()->getExtentWidth() + 4)
				{
					MaxWidth = (REAL32) ((i3UICaptionControl*) pChild)->getUIText()->getExtentWidth() + 4;
				}
			}
		}
	}

	for(size_t i = 0; i < item_list.size(); i++)
	{
		i3UIListView_Item * pItem = item_list[ i];

		pItem->UpdateCellPosAndSize( iconWidth + MaxWidth, pItem->getHeight(), true);
		pItem->UpdateShapes();
	}

	return MaxWidth;
}

void UIPopupUserAction_V11::_ReposFrameWnd(REAL32 MaxWidth)
{
	REAL32 h = (m_pListBox->getBoundItem()->getHeight() * m_commandCount);
	REAL32 iconWidth = m_pIconTemplate->getShape(0)->getWidth() + 3;

	// ��ġ�� ���콺 ��ġ��..
	POINT pos;
	REAL32 fScreenWidthRate = i3UI::getUIResolutionWidth();
	REAL32 fScreenHeightRate = i3UI::getUIResolutionHeight();

	I3ASSERT( i3UI::getManager() != nullptr);


	i3InputMouse* mouse = g_pFramework->getMouse();
	REAL32 x = mouse->GetX() / fScreenWidthRate;
	REAL32 y = mouse->GetY() / fScreenHeightRate;

	pos.x = (LONG) x;
	pos.y = (LONG) y;

	I3ASSERT( m_pListBox->getBoundItem() != nullptr);

	m_pFrameWnd->OnMapCoord( &pos);

	x = (REAL32) pos.x;
	y = (REAL32) pos.y;

	h += m_pListBox->getHeader()->getHeight();

	MaxWidth += iconWidth;
	if( MaxWidth == 0.f)
		MaxWidth = getX(m_pListBox->getSize());

	m_pListBox->setSize( MaxWidth, h);

	// ȭ���� ������ �������� �о�����..
	if( x + MaxWidth > i3UI::getManager()->getWidth())
		x = i3UI::getManager()->getWidth() - MaxWidth;

	if( y + h > i3UI::getManager()->getHeight())
		y = i3UI::getManager()->getHeight() - h;

	m_pFrameWnd->setPos( (REAL32)x, (REAL32)y);
	m_pFrameWnd->setSize( MaxWidth, h);
}

void UIPopupUserAction_V11::_ReSizeAndPosIcon()
{
	i3::vector<i3UIListView_Item*>& item_list = m_pListBox->getItemList();

	for(size_t i = 0; i < item_list.size(); i++)
	{
		i3UIListView_Item * pItem = item_list[ i];

		i3UIListView_Cell * pCell = pItem->getCell( 0);
			
		_SetIconSizeAndPos(pCell, pItem->getSepWidth());
	}
}

void UIPopupUserAction_V11::_SetIconSizeAndPos( i3UIListView_Cell * pCell , REAL32 sepWidth)
{
	i3UIButtonImageSet * pCtrl = static_cast<i3UIButtonImageSet*>(pCell->getCellChild());

	if(pCtrl != nullptr)
	{
		REAL32 width, height;
		width = pCtrl->getWidth();
		height = pCtrl->getHeight();

		REAL32 posX, posY;
		posX = (pCell->getWidth() / 2.0f) - (width / 2.0f) + sepWidth;
		posY = (pCell->getHeight() / 2.0f) - (height / 2.0f);

		pCtrl->setSize(width, height);
		pCtrl->setPos(posX, posY);
	}
}

void UIPopupUserAction_V11::_CmdGMOption( void )
{
	i3::stack_wstring wstrUserNickName;
	i3::sprintf(wstrUserNickName, L"%s", m_RequestInfo._tUserInfo._Nick);

	INT32 _UserIndex = LobbyContext::i()->getUserSessionIdxByUserNickName(wstrUserNickName);
	UINT8 _IndexType = 1;

	// Search�� �̿��� �˻��� �κ� ������ ������ ���ŵ��� �ʱ� ������
	// ���ŵ� InviteContext�� ���� ��Ͽ��� SessionIndex �˻縦 �� �� �� �ؾ���
/*	if ( _UserIndex != -1)
		_UserIndex = InviteContext::i()->GetUserSessionIdxByUserNickName(m_strUserNickName);*/

	// SessionIndex�� ������ ������ �κ� ����
	if ( _UserIndex != -1)
	{
		_IndexType = GMCHAT_INDEX_LOBBY;
		I3TRACE("������ �κ� �ֽ��ϴ�.\n");
	}
	// SessionIndex�� -1�̸� �ش� ������ �κ� ����
	else
	{
		// ������ �ִ� ���� �ε��� ��ȣ
		const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();
		_UserIndex = pInfo->_UserInfo.m_RoomIdx + 1;
		_IndexType = GMCHAT_INDEX_ROOM;

		I3TRACE("������ �濡 �ֽ��ϴ�.\n");

		// ������ GM�� ���� �濡 �ִ��� Ȯ��
		if( g_pFramework->GetUIMainframe()->GetCurrentPhaseType() != UIPHASE_LOBBY)
		{

			I3TRACE("GM�� �濡 �ֽ��ϴ�.\n");
			if ( _UserIndex == MyRoomInfoContext::i()->getMyRoomIndex())
			{
				_UserIndex = BattleSlotContext::i()->GetUserIndex(wstrUserNickName);
				_IndexType = GMCHAT_INDEX_ROOMSLOT;

				I3TRACE("GM�� ������ ���� �濡 �ֽ��ϴ�.\n");
			}
			else
			{
				if (InviteContext::i()->GetUserSessionIdxByUserNickName(wstrUserNickName) != -1)
				{
					I3TRACE("������ �濡 �ֽ��ϴ�.\n");
					
					_IndexType = GMCHAT_INDEX_LOBBY;
					_UserIndex = InviteContext::i()->GetUserSessionIdxByUserNickName(wstrUserNickName);
				}
				else
				{
					I3TRACE("GM�� ������ �ٸ� �濡 �ֽ��ϴ�.\n");
				}
			}
		}
		else
		{
			I3TRACE("GM�� �κ� �ֽ��ϴ�.\n");
		}
	}

	I3TRACE("IndexType : %d\n", _IndexType);
	I3TRACE("UserIndex : %d\n", _UserIndex);
	

	GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_NICK, wstrUserNickName.c_str() );
	GameEventSender::i()->SetEvent(EVENT_GM_AUTHORITY_REQ, &_IndexType, &_UserIndex, wstrUserNickName.c_str() );
}
