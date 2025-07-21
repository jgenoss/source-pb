#include "pch.h"
#include "UIMainFrame.h"
#include "UIPopupReceiveMail.h"
#include "UIUtil.h"

#include "../Shop.h"

#include "MailBoxContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"

extern "C" 
{
	int UIPopupReceiveMail_CheckID( LuaState * L)
	{
		UIPopupReceiveMail * pThis = static_cast<UIPopupReceiveMail*>(GameUI::GetPopup(UPW_MAILREV));
		if( pThis != nullptr)
			pThis->CheckID();
		return 0;
	}

	int UIPopupReceiveMail_ClanAskSend( LuaState * L)
	{
		bool bAccept = i3Lua::GetBooleanArg(L , 1);
		UIPopupReceiveMail * pThis = static_cast<UIPopupReceiveMail*>(GameUI::GetPopup(UPW_MAILREV));
		if( pThis != nullptr)
			pThis->SendClanJoinAsk(bAccept);
		return 0;
	}
}

LuaFuncReg UIPopupReceiveMail_Glue[] =
{
	{"CheckID",		UIPopupReceiveMail_CheckID},
	{"ClanAskSend", UIPopupReceiveMail_ClanAskSend},
	{nullptr,			nullptr}
};


I3_CLASS_INSTANCE( UIPopupReceiveMail);//, UIPopupBase );

UIPopupReceiveMail::UIPopupReceiveMail() 
: m_pEditNickName	( nullptr )
, m_pEditMessage	( nullptr )
, m_pMsgboxWait		( nullptr )
{
	m_bReOpen = false;
	m_bCheckedID = false;
}

UIPopupReceiveMail::~UIPopupReceiveMail()
{
}

void	UIPopupReceiveMail::OnCreate( i3GameNode* pParent )
{
	UIPopupBase::OnCreate( pParent);
	AddScene( "Scene/Popup/PBRe_PopUp_Message.i3UIs", UIPopupReceiveMail_Glue );
}

bool	UIPopupReceiveMail::OnEntranceStart( void* pArg1, void* pArg2 )
{
	bool bRes = UIPopupBase::OnEntranceStart( pArg1, pArg2 );
	I3ASSERT( nullptr != pArg1 );

	if( false == bRes || nullptr == pArg1 )
	{
		return false;
	}

	m_data = *static_cast<OPEN_DATA*>(pArg1);

	LuaState* lua = nullptr; 

	switch( m_data.mailType )
	{
	case	SEND:
			lua = _CallLuaFunction( "SetMessageType_Send" );			
			break;

	case	READ:
			lua = _CallLuaFunction( "SetMessageType_Read" );
			break;

	case	GIFT:
			lua = _CallLuaFunction( "SetMessageType_Gift" );
			break;

	case	REPLY:
			lua = _CallLuaFunction( "SetMessageType_Reply" );
			break;

	case	CLAN_SEND:
			lua = _CallLuaFunction( "SetMessageType_ClanSend");
			break;

	case	CLAN_READ:
			lua = _CallLuaFunction( "SetMessageType_ClanRead");
			break;

	case	CLAN_READ_ASK:
			lua = _CallLuaFunction( "SetMessageType_ClanReadAsk");
			break;

	default:
		I3PRINTLOG(I3LOG_NOTICE, "invalid pArg1...." );
		return false;
	}

	if( nullptr != lua )
		_EndLuaFunction( lua, 0 );

	m_bReOpen		= false;
	m_bCheckedID	= false;
	return true;
}

void	UIPopupReceiveMail::_UpdateOpenData()
{
	LuaState* lua = nullptr; 

	// set nick name
	{
		lua = _CallLuaFunction( "SetNickName" );
		i3Lua::PushStringUTF16To8( lua, m_data.wstrNickName );
		_EndLuaFunction( lua, 1 );
	}

	// set message
	{
		lua = _CallLuaFunction( "SetMessage" );
		i3Lua::PushStringUTF16To8( lua, m_data.wstrMessage );
		_EndLuaFunction( lua, 1 );
	}

	// set gift
	{
		lua = _CallLuaFunction( "SetGiftName" );
		i3Lua::PushStringUTF16To8( lua, m_data.wstrAttachedItem);
		_EndLuaFunction( lua, 1 );
	}

	i3UI::SetFocusControl( nullptr );
	switch( m_data.mailType )
	{	
	case	SEND:
			i3UI::SetFocusControl( m_data.wstrNickName.empty() ? m_pEditNickName : m_pEditMessage );
			break;

	case	REPLY:
			i3UI::SetFocusControl( m_pEditMessage );
			break;
	}
}

void	UIPopupReceiveMail::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();
	_UpdateOpenData();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}

bool	UIPopupReceiveMail::OnKeyInput( i3InputDeviceManager * pInputMng )
{
	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
	
	if( pKeyboard->GetStrokeState( I3I_KEY_TAB ) )
	{
		if( SEND != m_data.mailType )
		{
			return UIBase::OnKeyInput( pInputMng );
		}

		if( true == m_pEditNickName->isFocused() )
		{
			i3UI::SetFocusControl( m_pEditMessage );
		}
		else
		{
			i3UI::SetFocusControl( m_pEditNickName );
		}

		return true;
	}

	if( i3UI::getGlobalIme()->isFocused() )
	{
		if( pKeyboard->GetStrokeState( I3I_KEY_ENTER ) )
		{
			if( true == m_pEditNickName->isFocused() )
			{
				i3UI::SetFocusControl( m_pEditMessage );				
			}

			return true;
		}		

		if( pKeyboard->GetStrokeState( I3I_KEY_ESC ) )
		{
			i3UI::SetFocusControl( nullptr );
			return true;
		}
	}

	return UIBase::OnKeyInput( pInputMng );
}

bool	UIPopupReceiveMail::_SendMessage()
{
	if( nullptr == m_pEditMessage ||  nullptr == m_pEditNickName )
	{
		return false;
	}

	const i3::const_wchar_range& wstrSender	= m_pEditNickName->getTextRange();
	const i3::const_wchar_range& wstrMessage = m_pEditMessage->getTextRange();


	if( true == wstrSender.empty() )
	{		
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_INPUT_RECEIVER") ); /*�޴� ����� �Է��� �ּ���.*/
		return false;
	}
	
	if( true == wstrMessage.empty() )
	{		
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_INPUT_NOTE") ); /*���� ������ �Է��� �ּ���.*/
		return false;
	}

//	std::string szSender = _bstr_t( wszSender.c_str() );
//	std::string szMessage = _bstr_t( wszMessage.c_str() );

	i3::rc_wstring my_nick_name = UserInfoContext::i()->GetMyNickName();

	if( i3::generic_is_equal( wstrSender, my_nick_name ) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_SEND_SELF") ); /*�ڽſ��� ������ ���� �� �����ϴ�.*/
		m_pEditNickName->SetText( L"");
		g_pFramework->SetFocus( m_pEditNickName );
		return false;
	}

	// �г��� ���̰� 32����Ʈ�� ������ ����ó��
	if( wstrSender.size() >= NET_NICK_NAME_SIZE )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_SEND_NOTE"));/*���� �����⿡ �����Ͽ����ϴ�.*/
		g_pFramework->SetFocus( m_pEditNickName );
		return false;
	}

	if( false == g_pFramework->IsFilterOK( wstrMessage ) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�.*/
		g_pFramework->SetFocus( m_pEditMessage );
		return false;
	}


	NOTE_BODY note;

	i3::safe_string_copy(note._sender, wstrSender, NET_NICK_NAME_SIZE);
	i3::safe_string_copy(note._message, wstrMessage, NET_NOTE_SIZE);

	note._senderSize = (UINT8)(i3::generic_string_size( note._sender) + 1);
	note._messageSize = (UINT8)(i3::generic_string_size( note._message) + 1);

	GameEventSender::i()->SetEvent( EVENT_SEND_NOTE, &note );
	return true;
}

bool	UIPopupReceiveMail::_SendClanMessage()
{
	if( nullptr == m_pEditMessage ||  nullptr == m_pEditNickName )
	{
		return false;
	}

	const i3::const_wchar_range& wstrMessage = m_pEditMessage->getTextRange();

	if( true == wstrMessage.empty() )
	{		
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_INPUT_NOTE") ); /*���� ������ �Է��� �ּ���.*/
		return false;
	}

//	std::string szMessage = _bstr_t( wszMessage.c_str() );

	if( false == g_pFramework->IsFilterOK( wstrMessage ) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�.*/
		g_pFramework->SetFocus( m_pEditMessage );
		return false;
	}


	i3::wliteral_range strMessage = wstrMessage;

	g_pFramework->GetClanContext()->SendClanNote( (UINT8)CLAN_NOTE_ALL, strMessage.c_str());

	return true;
}

bool UIPopupReceiveMail::_SendGiftMessage(UINT8 ui8Type)
{
	if( MailBoxContext::i()->IsWaitResult() )
		return false;

	if(m_data.mailType == GIFT)
	{
		const GIFT_BUFFER * pGift = MailBoxContext::i()->GetNoteGiftGiftID( m_data.noteIdx);	

		if( pGift != nullptr)
		{
			if( pGift->_eDeliver >= TYPE_GIFT_DELIVER_RECEIVED)
				return false;

			if( ui8Type == TYPE_SHOP_GIFT_READ && pGift->_eDeliver >= TYPE_GIFT_DELIVER_READ )
				return false;

			UINT8 ui8Count = 1;
			
			if( GameEventSender::i()->SetEvent( EVENT_AUTH_GIFT_GOODS, &ui8Type, &ui8Count, &pGift->_i32GiftWareDBIdx ) )
			{
				MailBoxContext::i()->BuildGiftReqList(ui8Type, (INT32*)(&(pGift->_i32GiftWareDBIdx)), ui8Count);
				i3UI::setDisableUIInput(true);
				return true;
			}	
		}
	}

	return false;
}

void	UIPopupReceiveMail::_InitializeAtLoad( i3UIScene* pScene )
{
	UIPopupBase::_InitializeAtLoad( pScene );

	m_pEditNickName = (i3UIEditBox*) pScene->FindChildByName( "edit_NickName" );
	m_pEditMessage = (i3UIEditBox*) pScene->FindChildByName( "edit_Message" );

	I3ASSERT( nullptr != m_pEditNickName );
	I3ASSERT( nullptr != m_pEditMessage );
}

void	UIPopupReceiveMail::_ClearAtUnload( i3UIScene * pScene )
{
	UIPopupBase::_ClearAtUnload( pScene );

	m_pEditNickName = nullptr;
	m_pEditMessage = nullptr;
}


bool	UIPopupReceiveMail::OnOKButton()
{
	switch( m_data.mailType )
	{
	case REPLY			:
	case SEND			: return _SendMessage();

	case READ			:		
		{
			m_bReOpen = true;

			m_data.mailType			= REPLY;
			m_data.wstrMessage.clear();
			m_data.wstrAttachedItem.clear(); 
		}
		return true;

	case GIFT			: return _SendGiftMessage(TYPE_SHOP_GIFT_AUTH);
	case CLAN_SEND		: return _SendClanMessage();
	case CLAN_READ		: return false;
	case CLAN_READ_ASK	: return false;
	}

	return true;
}

bool	UIPopupReceiveMail::OnKey_Enter()
{
	bool bRes = false;

	switch( m_data.mailType )
	{
	case	REPLY:
	case	SEND:
			bRes = _SendMessage();
			break;
	case	CLAN_SEND:
			bRes = _SendClanMessage();
			break;
	}

	if( true ==	bRes )
		GetMainFrame()->ClosePopup( UPW_MAILREV );

	return bRes;
}

bool	UIPopupReceiveMail::OnCloseButton()
{
	return true;
}


bool	UIPopupReceiveMail::OnKey_Escape()
{
	if( false == OnCloseButton() )
	{
		return false;
	}

	GetMainFrame()->ClosePopup( UPW_MAILREV );
	return true;
}

void	UIPopupReceiveMail::OnExitEnd()
{
	UIPopupBase::OnExitEnd();

	_SendGiftMessage(TYPE_SHOP_GIFT_READ);

	if( m_bReOpen )
	{
		GetMainFrame()->OpenPopup( UPW_MAILREV, &m_data );
		m_bReOpen = false;
	}

}


void	UIPopupReceiveMail::CheckID()
{
	//  [5/24/2012 dosun.lee]
	// ��Ŷ ��� ĳ���� ������ ���̴°ɷ� ��� �Ϸ� ������.. �̻� �۵��ؼ� �ӽ÷� ����	

	return;	 // <==
	

	/*
	if( true == m_bCheckedID )
	{
		// �̹� üũ��
		return;
	}

	if( nullptr == m_pEditNickName )
	{
		return;
	}

	i3::wliteral_range wstrNick = m_pEditNickName->getTextRange();

	if( true == wstrNick.empty() )
	{
		return;
	}

//	std::string szNcik = _bstr_t( wszNick.c_str() );

	if( false == IsValidNicknameLetterSize( wstrNick ) )
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_LENGTH") );
		return;
	}

	if( false == g_pFramework->CheckValidNickname( wstrNick ) ||
		false == g_pFramework->IsFilterOK( wstrNick ) )
	{

		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_WRONG_NICK") );
		return;
	}

	m_pMsgboxWait = GameUI::MsgBox( MSG_TYPE_SYSTEM_WAIT, GAME_RCSTRING("STBL_IDX_EP_LOBBY_WAIT_ROOM_LIST_W") );
	GameEventSender::i()->SetEvent( EVENT_USE_ITEM_CHECK_NICK, wstrNick.c_str() );	
	*/
}

void UIPopupReceiveMail::SendClanJoinAsk(bool bAccept)
{
	if(m_data.mailType == CLAN_READ_ASK)
	{
		NOTE_RECEIVE * pNote = MailBoxContext::i()->SetReceiveNoteID( m_data.noteIdx);	

		if( pNote != nullptr)
		{
			UINT32 wClanDBIdx = (UINT32)m_data.noteAppendData;
			g_pFramework->GetClanContext()->SendClanInviteAccept(wClanDBIdx, (UINT8) bAccept);
			GetMainFrame()->ClosePopup(UPW_MAILREV);

			int nCnt = 1;
			GameEventSender::i()->SetEvent( EVENT_CHECK_READED_NOTE, &nCnt, &pNote->header._id);

			pNote->header._state = NS_READED;
			pNote->header._remainDay = 7;
		}
	}
}

bool	UIPopupReceiveMail::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code )
{
	UIPopupBase::OnEvent( event, pObj, param2, code );

	switch( event )
	{
	case	EDIT_STATE_CHANGE_INPUT_CHAR:
			{
				if( m_pEditNickName && m_pEditNickName->isFocused() )
				{
					m_bCheckedID = false;
					
					LuaState* lua = _CallLuaFunction( "SetCheckedID" );
					if( nullptr != lua )
					{
						i3Lua::PushBoolean( lua, m_bCheckedID);
						_EndLuaFunction( lua, 1 );
					}
				}
			}
			break;
	}

	return true;
}

void	UIPopupReceiveMail::ProcessGameEvent( GAME_EVENT event, INT32 arg , const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent( event, arg, UserData);
	
	switch(event)
	{
	case EVENT_USE_ITEM_CHECK_NICK:
		{
			_ProcessGameEvent_CheckID( arg );
		}
		break;
	}
}

/*vurtial*/ bool UIPopupReceiveMail::OnPlayClickSound(i3UIControl * pCtrl)
{
	if( i3::same_of<i3UIButtonComposed3*>(pCtrl) )
	{
		// ���� ������ ���¿���, ������ ��ư Ŭ���� OK ����
		if( (m_data.mailType == SEND || m_data.mailType == REPLY) && 
			i3::generic_is_iequal(pCtrl->GetName(), "btn_Send") )
		{
			return PlaySound(UISND_ITEM_CLICK_OK);
		}
		// Ŭ�� ���� ���� or ������ OK ����
		else if( i3::generic_is_iequal(pCtrl->GetName(), "btn_Accept")  || 
			i3::generic_is_iequal(pCtrl->GetName(), "btn_Denial")  )
		{
			return PlaySound(UISND_ITEM_CLICK_OK);
		}
		// ��� ��ư�� Cancel ����
		else if( i3::generic_is_iequal(pCtrl->GetName(), "btn_Close") )
		{
			return PlaySound(UISND_ITEM_CLICK_CANCEL);
		}
	}

	return UIPopupBase::OnPlayClickSound(pCtrl);
}

void UIPopupReceiveMail::_ProcessGameEvent_CheckID( INT32 arg )
{
	if( EV_SUCCESSED(arg) )
	{
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("�г��� ����") );
		m_bCheckedID = false;
	}
	else
	{
		if( EV_IS_ERROR( arg, EVENT_ERROR_EVENT_DUPLICATE_NICK ) )
		{
			GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("���� ������ ������ �г���") );
			m_bCheckedID = true;
		}
		else
		{
			GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("Unknown") );
			m_bCheckedID = false;
		}
	}

	LuaState* lua = _CallLuaFunction( "SetCheckedID" );
	if( nullptr != lua )
	{
		i3Lua::PushBoolean( lua, m_bCheckedID);
		_EndLuaFunction( lua, 1 );
	}
}