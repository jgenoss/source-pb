#include "pch.h"

#include "UIFloatCommunity.h"

#include "UI/UIMainFrame.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIPopupReceiveMail.h"
#include "GameGUI.h"
#include "GameContextUtil.h"
#include "ChannelContext.h"
#include "MailBoxContext.h"
#include "UserInfoContext.h"
#include "UI_V10/UITopMenu_V10.h"

#include "Shop.h"
#include "UI/UIUtil.h"
#include "UI/UIShopFunction.h"
#include "UI/UIItemInfoUtil.h"

extern "C" 
{
	int UIPopupCommunity_ClickTab( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->SelectTab( tab);

		return 0;
	}

	int UIPopupCommunity_ClickSubTab( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->SelectSubTab( tab);

		return 0;
	}
	
	int UIPopupCommunity_Click( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->Click( tab);

		return 0;
	}

	int UIPopupCommunity_ReadMail( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L, 1, -1 );	
		INT32 i32Cell = i3Lua::GetIntegerArg(L, 2, -1);

		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if(nullptr != pThis && i32Cell != 0)
			pThis->ReadMail(i32Slot);

		return 0;
	}

	int UIPopupCommunity_ClickUser( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->ListClick( LIST_LCLICK, idx);

		return 0;
	}

	int UIPopupCommunity_ClickOpenUserPopup( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->ListClick( LIST_RCLICK, idx);

		return 0;
	}

	int UIPopupCommunity_ClickFindUser( LuaState * L)
	{
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->FindUserButton();

		return 0;
	}

	int UIPopupCommunity_ClickAddFriend( LuaState * L)
	{
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->AddFriendButton();

		return 0;
	}
}

LuaFuncReg Community_glue[] =
{
	{"ClickTab",		UIPopupCommunity_ClickTab},
	{"ClickSubTab",		UIPopupCommunity_ClickSubTab},
	{"ClickButton",		UIPopupCommunity_Click},
	{"ReadMail",		UIPopupCommunity_ReadMail},
	{"ClickUser",		UIPopupCommunity_ClickUser},
	{"OpenUserPopup",	UIPopupCommunity_ClickOpenUserPopup},
	{"ClickFindUser",	UIPopupCommunity_ClickFindUser},
	{"ClickAddFriend",	UIPopupCommunity_ClickAddFriend},
	{nullptr,				nullptr}
};

I3_CLASS_INSTANCE( UIFloatCommunity);//, UITabBase);

UIFloatCommunity::UIFloatCommunity() : m_i32SelectIdx(-1)
{
	m_MainTab = MAINTAB_USERLIST;
	m_AlignedPos = UIFLOATING_POS_RIGHTBOTTOM;
	m_bFixedWindow = true;
	RegisterGameEventFunc( EVENT_UPDATE_USER_LIST,			&UIFloatCommunity::ReceiveGameEvent_Update_UserList);
	RegisterGameEventFunc( EVENT_INSERT_FRIEND,				&UIFloatCommunity::ReceiveGameEvent_Update_UserList);
	RegisterGameEventFunc( EVENT_CLAN_MEMBER_LIST_N,		&UIFloatCommunity::ReceiveGameEvent_Update_UserList);

	RegisterGameEventFunc( EVENT_DELETE_NOTE,				&UIFloatCommunity::ReceiveGameEvent_Update_DeleteNote);
	RegisterGameEventFunc( EVENT_AUTH_GIFT_GOODS,			&UIFloatCommunity::ReceiveGameEvent_Update_GiftGoods);
	RegisterGameEventFunc( EVENT_RECEIVE_NOTE,				&UIFloatCommunity::ReceiveGameEvent_Update_DeleteNote);
}

UIFloatCommunity::~UIFloatCommunity()
{
	m_CurrentUserTab = USERLIST_TAB_NONE;

	_ClearUserList();
}


/************************//************************/
/*virtual*/ void UIFloatCommunity::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	AddScene("Scene/Popup/PBRe_PopUp_MessageBox_Ver10.i3UIs", Community_glue);
}

/*virtual*/ void UIFloatCommunity::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad(pScene);

	m_pItemListBox = (i3UIListView_Box*) pScene->FindChildByName( "i3UIListView_Box");
	I3ASSERT( m_pItemListBox != nullptr );

	m_pItemListNote = (i3UIListView_Box*) pScene->FindChildByName( "ListView_Note");
	I3ASSERT( m_pItemListNote != nullptr );

	m_pMsgTab = (i3UITab*) pScene->FindChildByName( "i3UITab0");
	I3ASSERT( m_pMsgTab != nullptr );

	m_pUserTab = (i3UITab*) pScene->FindChildByName( "i3UITab");
	I3ASSERT( m_pUserTab != nullptr );

	

	m_pScrollRoomList = m_pItemListBox->getScrollBar();
	I3ASSERT( m_pScrollRoomList != nullptr );
}

/*virtual*/ void UIFloatCommunity::_ClearAtUnload( i3UIScene * pScene)
{
	UIFloatingBase::_ClearAtUnload( pScene);
}

/*virtual*/ bool UIFloatCommunity::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	//GetFirstFrameWnd()->EnableCtrl(false);

	LuaState * L = _CallLuaFunction("SetTab");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, 0);
		_EndLuaFunction(L , 1);
	}

	SelectUserTab(USERLIST_TAB_FRIEND);

	m_pScrollRoomList->setScrollPos(0);

	__BuildNoteDataList();
	m_eSortType = MAIL_SORT_INDEX;
	m_i32SelectIdx = -1;
	return true;
}

/*virtual*/ void UIFloatCommunity::OnEntranceEnd(void)
{
	UIFloatingBase::OnEntranceEnd();
	INT32 note_count = MailBoxContext::i()->GetNoteReceiveCount();

	SelectTab(0);

	if( note_count >= MAX_NORMAL_NOTE_COUNT)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_FULL_NOTE"));//�������� �뷮�� �����Ͽ� �� �̻� ������ ������ �� �����ϴ�.\n���ʿ��� ������ ������ �ֽʽÿ�.
	}
	else if( note_count >= ( MAX_NORMAL_NOTE_COUNT - 20 ) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_NEEDLESS_NOTE"));//�������� �뷮�� �� ���� �ʾҽ��ϴ�.\n�������� �� ���� �Ǹ� ������ ������ �� ������\n���ʿ��� ������ ������ �ֽʽÿ�.
	}

	_UpdateNoteList();
}

void UIFloatCommunity::OnLoadAllScenes()
{
	m_bOnLoad = true;
}

bool UIFloatCommunity::OnExitStart()
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	return true;
}

void	UIFloatCommunity::OnUnloadAllScenes()
{
	m_bOnLoad = false;
}

/*virtual*/ void UIFloatCommunity::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate(rDeltaSeconds);

	if(m_bOnLoad)
	{
		if( m_CurrentUserTab == USERLIST_TAB_CLAN)
		{
			if( g_pFramework->GetClanContext() != nullptr)
				g_pFramework->GetClanContext()->UpdateClanMember( rDeltaSeconds);
		}
		UpdateAlarmNote(rDeltaSeconds);
	}
}

bool UIFloatCommunity::OnKey_Escape()
{
	return UIFloatingBase::OnKey_Escape();
}

bool UIFloatCommunity::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	return UIFloatingBase::OnKeyInput(pInputMng);
}

void UIFloatCommunity::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
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

void UIFloatCommunity::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIFloatCommunity::ReceiveGameEvent_Update_UserList( INT32, const i3::user_data&)
{
	_UpdateUserList();
}
void UIFloatCommunity::ReceiveGameEvent_Update_DeleteNote( INT32, const i3::user_data&)
{
	__BuildNoteDataList();
	_UpdateNoteList();
	//__UpdateMailList( false );
}
void UIFloatCommunity::ReceiveGameEvent_Update_GiftGoods( INT32, const i3::user_data&)
{
	if( !MailBoxContext::i()->IsWaitResult() )
	{
		switch( MailBoxContext::i()->GetGiftReqType() )
		{
		case TYPE_SHOP_GIFT_READ :
			{
				_UpdateNoteList();
				//__UpdateMailList( true );
			}
			break;
		case TYPE_SHOP_GIFT_AUTH :
			{
				_UpdateNoteList();
				//__UpdateMailList( true );
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_INFORM_SUCCESS_GIFT"));
			}
			break;
		case TYPE_SHOP_GIFT_DELETE :
			{
				__BuildNoteDataList();
				_UpdateNoteList();
				//__UpdateMailList( false );

				INT32 i32Count =  MailBoxContext::i()->GetGiftReqSuccessCount();
				i3::rc_wstring wstrMsg = i3::format_string( GAME_RCSTRING("STR_GIFT_DELETEALL"), i32Count);

				GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMsg );
			}
			break;
		}		

		i3UI::setDisableUIInput(false);
		MailBoxContext::i()->ResetGiftReqList();
	}
}

void UIFloatCommunity::_ClearUserList( void)
{
	i3::cu::for_each_delete_clear(m_UserInfoList);
}

void UIFloatCommunity::_UpdateFriendList()
{
	UINT32 i;

	for( i = 0; i  < CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( i);
		if(nullptr == node)	continue;

		USERINFO * pInfo = new USERINFO;
		pInfo->Reset();
		pInfo->CopyFromFriend( node);

		// ��
		if( FRIEND_STATE_OFFLINE == pInfo->_friendState && FRIEND_STATE_CANT_INVITE != pInfo->_friendState )
		{	// offline
			i3Color::Copy( &pInfo->_NickColor, GameGUI::GetColor(GCT_DEFAULT));
		}

		m_UserInfoList.push_back( pInfo);
	}
}

void UIFloatCommunity::_UpdateClanMemberList( void)
{
	UINT32 i;

	I3ASSERT( CCommunityContext::i() != nullptr);

	for( i = 0; i < CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( i);

		if( node)
		{
			USERINFO * pInfo = new USERINFO;
			pInfo->Reset();
			pInfo->CopyFromClanUser( node);

			m_UserInfoList.push_back( pInfo );
		}
	}
}

void UIFloatCommunity::_UpdateUserList( void)
{
	_ClearUserList();

	switch( m_CurrentUserTab)
	{
	case USERLIST_TAB_FRIEND :	_UpdateFriendList();		break;
	case USERLIST_TAB_CLAN :
		if( m_bReqClanMemberList == false)
		{
			//GameEventSender::i()->SetEvent( EVENT_CLAN_COMMUNITY_LIST);
			m_bReqClanMemberList = true;
		}
		else
			_UpdateClanMemberList();
		break;
	}

	LuaState * L = _CallLuaFunction("InitUserList");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, m_UserInfoList.size());
		_EndLuaFunction(L , 1);
	}
	// Set Slot
	_SetUserSlot();

}

void UIFloatCommunity::_UpdateNoteList( void)
{
	for(INT32 idx = 0; idx < GetNoteCount() ; ++idx)
	{
		//NOTE_RECEIVE * pNote = m_NoteList.at(idx).pNote;

		//_SetMailData(idx, pNote, bMailStateOnly);
		//_SetMailTextColor(idx, pNote);
	}

	LuaState * L = _CallLuaFunction("InitNoteList");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, m_NoteList.size());
		_EndLuaFunction(L , 1);
	}
	// Set Slot
	_SetNoteSlot();

}

void UIFloatCommunity::UpdateAlarmNote(REAL32 rDeltaSeconds)
{
	if( m_bAlarmCommunity)
	{
		m_rAlarmTerm += rDeltaSeconds;

		if (0.25f < m_rAlarmTerm)
		{
			m_rAlarmTerm = 0.0f;
			m_bAlarmFlag = !m_bAlarmFlag;
		}
		if(m_bAlarmFlag)
			m_pMsgTab->OnTabSelected();
		else
			m_pMsgTab->RemoveTabSelect();
	}
}

void UIFloatCommunity::CheckUnReadNote()
{
	if(m_MainTab == MAINTAB_USERLIST)
	{
		const NOTE_RECEIVE * pNote = nullptr;

		INT32 nCount = MailBoxContext::i()->GetNoteReceiveCount();
		for(INT32 i = 0 ; i < nCount ; ++i)
		{
			pNote = MailBoxContext::i()->GetReceiveNote(i);
			if(pNote->header._state == NS_UNREAD)
			{
				StartHighlightNote();
				return;
			}
		}
	}
	EndHighlightNote();
}

void UIFloatCommunity::StartHighlightNote( void)
{
	m_bAlarmCommunity = true;
	m_bAlarmFlag = true;
	m_rAlarmTerm = 0.0f;
}

void UIFloatCommunity::EndHighlightNote( void)
{
	m_bAlarmCommunity = false;
	m_bAlarmFlag = false;

	if(m_MainTab == MAINTAB_USERLIST)
		m_pMsgTab->OnTabSelected();
	else
		m_pMsgTab->RemoveTabSelect();

}

void UIFloatCommunity::_SetNoteSlot(void)
{
	for(INT32 idx = 0; idx < GetNoteCount() ; ++idx)
	{
		NOTE_RECEIVE * pNote = m_NoteList.at(idx).pNote;

		COLOR col;
		i3::wstring ReamindDay;
		i3::sprintf(ReamindDay, GAME_STRING("STR_POPUP_WHAT_DAY"), pNote->header._remainDay);

		if(pNote->header._state == NS_UNREAD)
			i3Color::Set(&col, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);
		else
			i3Color::Set(&col, (UINT8)136, (UINT8)136, (UINT8)139, (UINT8)255);

		m_pItemListNote->SetItemCellTextColor(idx, 0, &col, false);
		m_pItemListNote->SetItemCellTextColor(idx, 1, &col, false);
		m_pItemListNote->SetItemCellTextColor(idx, 2, &col, false);
		m_pItemListNote->SetItemCellTextColor(idx, 3, &col, false);

		m_pItemListNote->SetItemCell(idx, 0, 0);	// �޽���
		m_pItemListNote->SetItemCell(idx, 1, pNote->body._message);	// �޽���
		m_pItemListNote->SetItemCell(idx, 2, pNote->body._sender);		// ��������
		m_pItemListNote->SetItemCell(idx, 3, ReamindDay.c_str());	// ������¥

	}
}
void UIFloatCommunity::_SetUserSlot(void)
{
	if( m_CurrentUserTab != USERLIST_TAB_NONE)
	{
		size_t count = m_UserInfoList.size();

		for( size_t i = 0; i < count; i++)
		{
			if( i < m_UserInfoList.size())
			{
				USERINFO * pUser = getUserInfo( i);

				_SetUserInfo(i, pUser);
			}
			else
			{
				_SetUserInfo( i, nullptr);
			}
		}
	}
}

void UIFloatCommunity::_SetUserInfo( INT32 idx, const USERINFO * pInfo)
{
	//// ���� ������ �־ nConnectIdx = 0���� ����������� �ڿ� �б⹮ ó���� �Ѱ� ������..
	//// ���븮Ƽ�� �̷��� �ϴ°� �Ⱦ��մϴ�.   ���� ���� �̷��� �ϴ°� ���籸��....
	//// �ϴ��� ���ʿ��� �ڵ�� �������� ��� �ּ�ó���մϴ�.. ( 16.07.29.����)
	
	//INT32 nConnectIdx = 0;

	//LuaState * L;
	//i3::rc_wstring wstrState;

	if( pInfo != nullptr && !pInfo->_wstrNick.empty() )
	{
		//	���
		INT32 rank = (INT32) pInfo->_Rank;

		if( ChannelContext::i()->IsBeginnerChannel1())	//	�ʺ� ����
			rank = RANK_NEWBIE;

		if( rank >= RANK_MAX)
			rank = RANK_MAX-1;

		m_pItemListBox->SetItemCell(idx, 0, pInfo->_Rank);			// ���
		m_pItemListBox->SetItemCell(idx, 1, pInfo->_wstrNick.c_str());		// �г���
		m_pItemListBox->SetItemCell(idx, 2, pInfo->_szPosition.c_str());		// ��������

		COLOR col;
//		if(nConnectIdx == 0)
//		{
			i3Color::Set(&col, pInfo->_NickColor.r, pInfo->_NickColor.g, pInfo->_NickColor.b, pInfo->_NickColor.a);
			m_pItemListBox->SetItemCellTextColor(idx, 1, &col, false);
			i3Color::Set(&col, GameGUI::GetColor( GCT_VALID_C));
			m_pItemListBox->SetItemCellTextColor(idx, 2, &col, false);
//		}
//		else
//		{
//			i3Color::Set(&col, GameGUI::GetColor( GCT_INVALID_C));
//			m_pItemListBox->SetItemCellTextColor(idx, 1, &col, false);
//			m_pItemListBox->SetItemCellTextColor(idx, 2, &col, false);
//		}

	}			
	else
	{
		m_pItemListBox->SetItemCell(idx, 0, -1);			// ���
		m_pItemListBox->SetItemCell(idx, 1, "");		// �г���
		m_pItemListBox->SetItemCell(idx, 2, "");		// ��������
	}
	

}

void UIFloatCommunity::SelectUserTab( INT32 tab)
{
	if( tab > -1 && tab < USERLIST_TAB_MAX)
	{
		m_CurrentUserTab = (USERLIST_TAB)tab;

		_UpdateUserList();
	}		
}


/************************//************************/
void UIFloatCommunity::SelectTab(int idx)
{
	LuaState * L = _CallLuaFunction("SetTab");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, idx);
		_EndLuaFunction(L , 1);
	}

	m_MainTab = (MAINTAB)idx;
	m_CurrentUserTab = USERLIST_TAB_FRIEND;

	if( m_MainTab == MAINTAB_USERLIST)
	{
		_UpdateUserList();
		StartHighlightNote();
		CheckUnReadNote();
		m_pUserTab->OnTabSelected();
		m_pMsgTab->RemoveTabSelect();
	}
	else
	{
		EndHighlightNote();
		m_pUserTab->RemoveTabSelect();
		m_pMsgTab->OnTabSelected();
		_UpdateNoteList();
	}
}

void UIFloatCommunity::SelectSubTab(int idx)
{
	m_CurrentUserTab = (USERLIST_TAB)idx;
	_UpdateUserList();
}

void UIFloatCommunity::Click(int idx)
{
	switch( idx )
	{
	case BUTTON_EXIT: 
		{
			//GetFirstFrameWnd()->EnableCtrl(false);
			UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
			pUIFrame->GetTopMenu_V10()->SetEnableCommunity(false);
		}
		break;
	case BUTTON_SEND:
		{
			SendMail();
		}
		break;
	case BUTTON_DELETE:
		{
			DeleteMail();
		}
		break;
	default:
		break;
	}

}

void UIFloatCommunity::ListClick( int idx , int listidx)
{
	switch( idx)
	{
	case LIST_LCLICK:
		_OpenUserInfo(listidx);
		break;
	case LIST_RCLICK:
		_OpenUserAction(listidx);
		break;
	default:
		break;
	}
}


void UIFloatCommunity::SwitchClick()
{
	//GetFirstFrameWnd()->EnableCtrl(!GetFirstFrameWnd()->isEnable());
}

/************************//************************/

/*=============================================================================*/

void UIFloatCommunity::__BuildNoteDataList(void)
{
	m_NoteList.clear();

	_Note note;
	
	INT32 i32NoteCnt = MailBoxContext::i()->GetNoteReceiveCount();

	for( int idx = 0; idx < i32NoteCnt; ++idx )
	{
		note.idx	= idx;
		note.pNote	= const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetReceiveNote(idx));

		m_NoteList.push_back( note );
	}

	std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_idx );

	CheckUnReadNote();
}

/*virtual*/ INT32 UIFloatCommunity::_GetMailType(NOTE_RECEIVE & Note)
{
	INT32 i32MailType = UIPopupReceiveMail::READ;

	if(Note.header._type == NT_CLAN_ASK)
	{
		if(Note.header._state == NS_UNREAD)
			i32MailType = UIPopupReceiveMail::CLAN_READ_ASK;
		else
			i32MailType = UIPopupReceiveMail::CLAN_READ;
	}
	else if(Note.header._type == NT_GIFT)
	{
		i32MailType = UIPopupReceiveMail::GIFT;
	}
	else
	{
		if(Note.header._type == NT_CLAN)
			i32MailType = UIPopupReceiveMail::CLAN_READ;
		else
			i32MailType = UIPopupReceiveMail::READ;

		if(Note.header._state != NS_READED)
		{
			Note.header._state = NS_READED;	// �������·� �����մϴ�.
			Note.header._remainDay = 7;		// ���� ������ ������ �����Ⱓ�� 7�Ϸ� �����մϴ�.
		}
	}

	return i32MailType;
}


void UIFloatCommunity::SendMail(void)
{
	UIPopupReceiveMail::OPEN_DATA openData;

	openData.mailType	= UIPopupReceiveMail::SEND;	
	openData.wstrNickName.clear();	

	if( true == _IsSafeNoteIdx( m_i32SelectIdx) )
	{
		_Note& note = m_NoteList[ m_i32SelectIdx ];
		openData.wstrNickName = note.pNote->body._sender;
		openData.noteIdx = note.pNote->header._id;
	}

	GetMainFrame()->OpenPopup( UPW_MAILREV, &openData );
}

void UIFloatCommunity::ReadMail(INT32 i32Idx)
{
	if( false == _IsSafeNoteIdx( i32Idx ) )
		return;

	_Note& note = m_NoteList[ i32Idx ];

	UIPopupReceiveMail::OPEN_DATA openData;

	openData.mailType = static_cast<UIPopupReceiveMail::MAILTYPE>( _GetMailType( *(note.pNote)));

	if(openData.mailType == UIPopupReceiveMail::GIFT)
		_GetAttachedItemInfo(note.pNote, openData.wstrAttachedItem);

	openData.noteIdx		= note.pNote->header._id;
	openData.noteAppendData = (int)note.pNote->header._appendingData;
	openData.wstrNickName	=note.pNote->body._sender;
	openData.wstrMessage		= note.pNote->body._message;

	int nSelCnt = 0;
	int nSelList[ 100 ];
	const NOTE_RECEIVE* pNote = note.pNote;
	nSelList[ nSelCnt++ ] = pNote->header._id;

	GameEventSender::i()->SetEvent( EVENT_CHECK_READED_NOTE, &nSelCnt, nSelList );

	_UpdateNoteList();
	GetMainFrame()->OpenPopup( UPW_MAILREV, &openData );
}

void UIFloatCommunity::DeleteMail(void)
{	
	int idx = 0;
	int nSelCnt = 0;
	int nSelList[ 100 ];

	UINT8		ui8Type			= TYPE_SHOP_GIFT_DELETE;
	UINT8		ui8GiftCount	= 0;
	T_GiftDBIdx aGiftIdx[ MAX_SHOP_GOODS_BUY_COUNT ];
	i3mem::FillZero( aGiftIdx, sizeof(aGiftIdx));

	for each( const _Note & note in m_NoteList )
	{
		const NOTE_RECEIVE* pNote = note.pNote;

		if( false == m_pItemListNote->isCheckedData( idx++ , 0 ) )
			continue;

		if( nullptr == pNote )
			continue;

		//@eunil. 1.0�� ������ ������ �����Ƿ� �ٷ� �κ����� ����.
		//�ӽ� ũ���� ����ڵ�
		if(pNote->header._type == NT_GIFT)
		{
			const GIFT_BUFFER * pGift = reinterpret_cast<GIFT_BUFFER*>(pNote->body._user);
			if (pGift) {
				aGiftIdx[ui8GiftCount++] = pGift->_i32GiftWareDBIdx;
			}
			else {
				nSelList[nSelCnt++] = pNote->header._id;
			}
		}
		else
		{
			nSelList[ nSelCnt++ ] = pNote->header._id;
		}
	}

	//_SendEvent(ACTION_DELETE, nSelList, nSelCnt );
	if(nSelCnt > 0)
		GameEventSender::i()->SetEvent( EVENT_DELETE_NOTE, &nSelCnt, nSelList );

	if( ui8GiftCount > 0)
		GameEventSender::i()->SetEvent( EVENT_AUTH_GIFT_GOODS, &ui8Type, &ui8GiftCount, aGiftIdx );
}

/*virtual*/ void UIFloatCommunity::_GetAttachedItemInfo(const NOTE_RECEIVE* pNote, i3::rc_wstring& wstrItemInfo)
{
	wstrItemInfo = GAME_RCSTRING("STR_POPUP_UNKNOWN_GIFT");

	//	�����ϱ� Note���� pNote->body._user �����ʹ� GIFT_BUFFER ����ü ��������.
	GIFT_BUFFER * pGift = (GIFT_BUFFER*)pNote->body._user;

	if (pGift == nullptr)
	{
		// GD�� ��û�������� ��ǰ ������ ���� ��(������ �������� ���� ���� ���) UI 1.0���� �ִ� ��ǰ ���� ����� ��Ų��. - sanghun.han
		wstrItemInfo = L"";

		return;
	}

	SHOP_GOODS_LIST * pGoodsList = CShop::i()->GetGoodsListData( pGift->_ui32GoodsID );

	if( pGoodsList == nullptr )
		return;


	if( pGoodsList->_ui8ItemCount > 1 )
	{
		bool isPeriod = false;
		INT32 i32PeriodIdx = 0;

		for(INT32 j = 0; j < pGoodsList->_ui8ItemCount; j++)
		{
			if (ITEM_TYPE_TIME == pGoodsList->_ppItemData[j]->_ui8AuthType)
			{
				isPeriod = true;
				i32PeriodIdx = j;
				break;
			}
		}

		//	getInfo�� keyItem���� �����;� ��Ȯ�� info�� ���� �� �ִ�. (�׷��������� nullptr�� �� �ִ�.)
		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo( pGift->_ui32GoodsID );
		I3ASSERT( pInfo != nullptr);
		UINT32 ui32ItemArg = pGoodsList->_pui32ItemArg[i32PeriodIdx];

		if( isPeriod )
		{
			UINT32 day = ConvertSecToUseDay(ui32ItemArg);
			// �̸�: %s\n���Ⱓ: %d��
			wstrItemInfo = i3::format_string( GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), day );
		}
		else
		{
			T_ItemID ItemID = pGoodsList->_ppItemData[i32PeriodIdx]->_ui32ItemID;

			i3::rc_wstring wstrTemp;

			//	���� ������ Goods���� pGoods->_AuthArg�� ��� ������ �ֱ� ������ ���� ó���� �ʿ� komet
			switch( (ITEM_TYPE) GET_ITEM_TYPE(ItemID) )
			{	
				//	���Ӽ�
			case	ITEM_TYPE_MAINTENANCE	: wstrTemp = GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME"); break;		
				//	�Ҹ�
			case	ITEM_TYPE_WRAP_PERIOD	:
			case	ITEM_TYPE_WRAP_COUNT	: wstrTemp = GAME_RCSTRING("STR_POPUP_EXPENDABLES_GIFT_NAME"); break;
			default							: return;
			}

			wstrItemInfo = i3::format_string(wstrTemp, pInfo->GetScriptName(), ui32ItemArg);							
		}
	}
	else
	{
		UINT8 ui8AuthType = pGoodsList->_ppItemData[0]->_ui8AuthType;
		UINT32 ui32ItemArg = pGoodsList->_pui32ItemArg[0] ; 
		T_ItemID ItemID = pGoodsList->_ppItemData[0]->_ui32ItemID;

		if( GET_ITEM_TYPE(ItemID) == ITEM_TYPE_SALECOUPON )
		{
#if defined (USE_SALE_COUPON)
			SALE_COUPON * pCoupon = CShop::i()->GetCouponData(ItemID);

			i3::rc_wstring wstrCouponName;				// TODO : ���� ���� ���..(2014.07.16.����)
			i3::mb_to_utf16(pCoupon->m_strCouponName, wstrCouponName);
			wstrItemInfo = wstrCouponName;
#endif		
		}
		else
		{
			i3::rc_wstring wstrTemp;
			i3::rc_wstring wstrItemName;		
			usf::GetShopItemName(wstrItemName, ItemID);

			I3ASSERT( ui8AuthType != 0 );
			switch( ui8AuthType )
			{
			case ITEM_TYPE_COUNT :
				{
					if( item_def::get_category(ItemID) == item_def::CTG_ITEM )
					{
						if (GET_ITEM_TYPE(ItemID) != ITEM_TYPE_WRAP_COUNT)
						{
							UINT32 ui32ItemTerm = GET_ITEM_SUBTYPE(ItemID);
							if (ui32ItemTerm > 0)
								wstrItemName = i3::format_string(GAME_RCSTRING("STR_INVEN_CASH_ITEM_TERM"), wstrItemName, ui32ItemTerm);
						}
					}

					wstrTemp = GAME_RCSTRING("STR_POPUP_EXPENDABLES_GIFT_NAME");
				}
				break;
			case ITEM_TYPE_TIME :
				{
					wstrTemp = GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME");
					ui32ItemArg = (INT32) ConvertSecToUseDay(ui32ItemArg);	//	�����Ⱓ (��)�� ��ȯ
				}
				break;
			}

			wstrItemInfo = i3::format_string(wstrTemp, wstrItemName, ui32ItemArg);
		}	
	}
}

void UIFloatCommunity::_OpenUserAction( INT32 idx )
{
	const USERINFO * pInfo = getUserInfo(idx);

	if( pInfo != nullptr)
	{
		GetMainFrame()->ResetUserActionPopup();

		switch( m_CurrentUserTab)
		{
		case USERLIST_TAB_FRIEND :
			_FriendUserAction(pInfo);
			break;
		case USERLIST_TAB_CLAN :
			_ClanUserAction(pInfo);
			break;
		default : 
			break;
		}

		GetMainFrame()->OpenUserActionPopup( this, idx);
	}
}

void UIFloatCommunity::_FriendUserAction( const USERINFO * pInfo )
{
	switch(pInfo->_friendState)
	{
	case FRIEND_STATE_LOBBY:
	case FRIEND_STATE_ONLINE:
		{
			// Ŭ������ �ƴҰ��
			if( true == g_pFramework->GetClanContext()->InBattleTeam())
				break;

			// ���� ����뿡 �ְ� ��밡 �¶����̸�
			if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
				GetMainFrame()->AddUserActionCommand( UCM_INVITE);
		} break;
	case FRIEND_STATE_ROOM:
		{
			// Ŭ������ �ƴҰ��
			if( true == g_pFramework->GetClanContext()->InBattleTeam())
				break;

			// ��밡 �뿡 �ְ� ���� ������� �ƴϸ� ã�ư���
			if( GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM)
			{
				GetMainFrame()->AddUserActionCommand( UCM_FIND_AWAY);
				break;
			}

			// ��밡 �뿡 �ְ� ���� ���� ���� �ƴ� ���
			if( gcu::IsSameRoom(pInfo->_friendState) == false)
			{
				GetMainFrame()->AddUserActionCommand( UCM_INVITE);
				GetMainFrame()->AddUserActionCommand( UCM_FIND_AWAY);
			}
			
		} break;
	}

	// Ŭ�� �ʴ�� �� Ŭ�� �ƴҰ�� ���� �ʴ� ��.
	// �̰� V1�ε� �̷��� �ۿ� ���ϴ� ����ΰ�����. ��?!
	if( CanClanInvite() )
	{
		if( g_pFramework->GetClanContext()->IsPossibleClanInvite() && 
			(_IsMyClanMember(pInfo->_i64UID) == false))
		{	
			GetMainFrame()->AddUserActionCommand( UCM_CLAN_INVITE);
		}
	}

	// �ӼӸ�
	//@eunil. Russia ��û���� ä��â�� ���־�� �մϴ�.
	if ( (GetMainFrame()->IsEnableFloatWin(UFW_CHAT) || GetMainFrame()->IsEnableFloatWin(UFW_READYROOM_CHAT)) && pInfo->_friendState != FRIEND_STATE_OFFLINE)
		GetMainFrame()->AddUserActionCommand( UCM_WHISPER);
}

void UIFloatCommunity::_ClanUserAction( const USERINFO * pInfo )
{
	const CLAN_MEMBER_NODE * pNode = _GetClanMemberNode(pInfo->_i64UID);

	// �ӼӸ�
	//@eunil. Russia ��û���� ä��â�� ���־�� �մϴ�.
	if ( (GetMainFrame()->IsEnableFloatWin(UFW_CHAT) || GetMainFrame()->IsEnableFloatWin(UFW_READYROOM_CHAT)) && pNode->_state != FRIEND_STATE_OFFLINE)
		GetMainFrame()->AddUserActionCommand( UCM_WHISPER);

	// ģ�� �߰�
	if( _IsMyFriend(pInfo->_i64UID) == false)
		GetMainFrame()->AddUserActionCommand( UCM_ADD_FRIEND);

	switch(pInfo->_friendState)
	{
	case FRIEND_STATE_ONLINE:
	case FRIEND_STATE_LOBBY:
		{
			if( GET_FRIEND_MATCH(pNode->_state) != MATCH_TEAM_INVALID_IDX) // Ŭ�� ��ġ ���� �� ��
			{
				if( false == g_pFramework->GetClanContext()->InBattleTeam()) // ���� ���� ���� ���
				{
					GetMainFrame()->AddUserActionCommand(UCM_FIND_AWAY); // ã�ư���
				}
			}
			else // ��밡 Ŭ�� ��ġ �ƴ� ���
			{
				if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM) // ���� ����뿡 ���� ���
				{
					// �ʴ��ϱ�
					GetMainFrame()->AddUserActionCommand( UCM_INVITE);
				}
				else
				{
					// Ŭ���� �� ��� 
					if( true == g_pFramework->GetClanContext()->InBattleTeam())
					{
						// �ʴ��ϱ�
						GetMainFrame()->AddUserActionCommand( UCM_INVITE);	
					}
				}
			}
		} break;
	case FRIEND_STATE_ROOM:
		{
			if( GET_FRIEND_MATCH(pNode->_state) == MATCH_TEAM_INVALID_IDX) // Ŭ�� ��ġ ���°� �ƴ����� ����뿡 ���� ���
			{
				if( false == g_pFramework->GetClanContext()->InBattleTeam()) // ���� Ŭ�� ��ġ ���� ���� ���
				{
					if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM) // ���� �Ϲ� �濡 ���� ���
					{
						// �ʴ��ϱ�
						GetMainFrame()->AddUserActionCommand( UCM_INVITE);	
					}

					GetMainFrame()->AddUserActionCommand(UCM_FIND_AWAY); // ã�ư���
				}
				else // ���� Ŭ�� ��ġ �� ���
				{
					// �ʴ��ϱ�
					GetMainFrame()->AddUserActionCommand( UCM_INVITE);	
				}
			}
		} break;
	}
}

void UIFloatCommunity::_OpenUserInfo( INT32 idx )
{
	const USERINFO * pInfo = getUserInfo(idx);

	if(pInfo != nullptr)
	{
		// �г��� ���
		CCommunityContext::i()->SetCommnunityInfoNick(pInfo->_wstrNick);

		if(pInfo->_i64UID < 0)
			return;

		INT64 UserUID = 0;
		UserUID = pInfo->_i64UID;

		GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_UID, &UserUID);

		INT32 idx2 = 2;
		if (m_CurrentUserTab == USERLIST_TAB_CLAN) idx2 = 4;

		if( GetMainFrame()->IsOpenedPopup( UPW_USERPROFILE) == false)
			GetMainFrame()->TogglePopup(  UPW_USERPROFILE, &idx2);
	}
}

bool UIFloatCommunity::_IsMyClanMember( INT64 uid )
{
	for(UINT32 i=0; i<CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetViewClanMember(i);

		if( pNode->_uid == uid)		return true;
	}

	return false;
}

bool UIFloatCommunity::_IsMyFriend( INT64 uid )
{
	for(UINT32 i=0; i<CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* pNode = CCommunityContext::i()->GetViewFriend(i);
		if(nullptr == pNode) continue;

		if( pNode->_uid == uid)
			return true;
	}

	return false;
}

const CLAN_MEMBER_NODE * UIFloatCommunity::_GetClanMemberNode( INT64 uid )
{
	for(UINT32 i=0; i<CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetViewClanMember(i);

		if( pNode->_uid == uid)
		{
			return pNode;
		}
	}

	return nullptr;
}

i3::rc_wstring UIFloatCommunity::GetNickForUserIdx( INT32 idx )
{
	USERINFO * pInfo = getUserInfo(idx);

	if( pInfo != nullptr)
	{
		return pInfo->_wstrNick;
	}

	return i3::rc_wstring();
}

void UIFloatCommunity::FindUserButton()
{
	INT32 idx = 1;
	GetMainFrame()->OpenPopup(UPW_USERSEARCH, &idx);
}

void UIFloatCommunity::AddFriendButton()
{
	INT32 idx = 2;
	GetMainFrame()->OpenPopup(UPW_USERSEARCH, &idx);
}
