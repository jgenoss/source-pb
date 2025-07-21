#include "pch.h"
#include "UIPopupSendMail_V11.h"

#include "../UserInfoContext.h"

#include "../UI/UIMainFrame_V11.h"
#include "../UI/UIUtil.h"

static UIPopupSendMail_V11 * GetThis( LuaState * L)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	if( pFrame != nullptr)
	{
		UIPOPUPWIN id = pFrame->FindPopup( L);
		I3ASSERT( id != UPW_NULL);

		return (UIPopupSendMail_V11*) pFrame->GetPopup( id);
	}

	return nullptr;
}

//--------------------------------------------------------------------------//
//								Lua Glue function							//
//--------------------------------------------------------------------------//
extern "C" {
	int UIPopupSendMail_V11_Search( LuaState * L)
	{
		GetThis( L)->CheckUser();
		return 0;
	}
}

LuaFuncReg UIPopupSendMail_V11_Glue[] = {
	{ "Search",					UIPopupSendMail_V11_Search},
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIPopupSendMail_V11);

UIPopupSendMail_V11::UIPopupSendMail_V11()
{
}

UIPopupSendMail_V11::~UIPopupSendMail_V11()
{
}

/*virtual*/ void UIPopupSendMail_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupInteractBase::_InitializeAtLoad( pScene);

	m_pWho = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox0");
	I3ASSERT( m_pWho);

	m_pNote = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox3");
	//���̵� �˻� ��ɹ̱��������ּ�ó���մϴ�. -���
	m_pSearch = (i3UIButtonComposed3 *) pScene->FindChildByName("i3UIButtonComposed31");
	m_pSearch->EnableCtrl(false);

	I3ASSERT( m_pNote);
}

#ifdef USE_MESS_TEST
INT64 UIPopupSendMail_V11::_FindNoteByUid( const char* nick)
{
	INT32 length;

	// �ڽ��� ���´�.
	if (g_pGameContext->IsMyNickName( nick))
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"));/*�ڽ��� �г����� �Է��ϼ̽��ϴ�.*/
		return 0;
	}

	NOTE_RECEIVE* pNote = nullptr;
	pNote = (NOTE_RECEIVE*)MailBoxContext::i()->GetReceiveNote( m_iListIdx);

	if( pNote == nullptr )
	{
		I3NOTICE( "pNote == nullptr \n");
		return 0;
	}

	if ( 0 != i3String::Compare(pNote->body._sender, nick))
	{
		NOTE_BODY note;
		length = i3String::LengthW( m_pWho->getText());
		i3String::CopyWCHARtoASCII( note._sender, sizeof(note._sender), m_pWho->getText(), length);

		length = i3String::LengthW( m_pNote->getText());
		i3String::CopyWCHARtoASCII( note._message, sizeof( note._message), m_pNote->getText(), length);
		note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
		note._messageSize = (UINT8)(i3String::Length( note._message) + 1);
		g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
		return 0;
	}

	return pNote->header._suid;
}
#endif

/*virtual*/ void UIPopupSendMail_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupInteractBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PointBlankRe_PopUp_MessageSend.i3UIs", UIPopupSendMail_V11_Glue);
}

/*virtual*/ bool UIPopupSendMail_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupInteractBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	LuaState * L = _CallLuaFunction( "SetUser");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, m_wstrNick);
		_EndLuaFunction( L, 1);
	}

	if( pArg2 != nullptr)
	{
		m_iListIdx = *static_cast<INT32*>(pArg2);
	}

	return true;
}

/*virtual*/ bool UIPopupSendMail_V11::OnOKButton( void)
{
	i3::stack_wstring Sender;
	i3::stack_wstring Message;

	if(i3::generic_string_size( m_pWho->getText()) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_INPUT_RECEIVER"));/*�޴� ����� �Է��� �ּ���.*/
		return false;
	}
	else if(i3::generic_string_size( m_pNote->getText()) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_INPUT_NOTE"));/*���� ������ �Է��� �ּ���.*/
		return false;
	}
	else
	{
		Sender = m_pWho->getText();
		Message = m_pNote->getText();

		if ( i3::generic_compare( UserInfoContext::i()->GetMyNickName(), Sender) == 0)
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_SEND_SELF"));/*�ڽſ��� ������ ���� �� �����ϴ�.*/
			m_pWho->SetText( L"");
			g_pFramework->SetFocus( m_pWho);
			return false;
		}
		else
		{
			if( g_pFramework->ChangeFilterStringOK( Message) == false)
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�.*/
				g_pFramework->SetFocus( m_pNote);
				return false;
			}
			else
			{
				// ���� ������

#ifdef USE_MESS_TEST
				if( m_bReceiverUIDFlag == false)
				{
					NOTE_BODY note;

					i3String::Copy( note._sender, Sender, NET_NICK_NAME_SIZE );

					i3String::Copy( note._message, Message, NET_NOTE_SIZE );

					note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
					note._messageSize = (UINT8)(i3String::Length( note._message) + 1);

					g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
				}
				else
				{
					NOTE_BODY_BY_UID note;

					m_bReceiverUIDFlag = false;

					// ���� ������� Ȯ��
					note._receiverUid = _FindNoteByUid(Sender);
					if( 0 >= note._receiverUid)
					{
						return true;
					}

					i3String::Copy( note._message, Message, NET_NOTE_SIZE );

					note._messageSize = (UINT8)(i3String::Length( note._message) + 1);

					g_pGameContext->SetEvent( EVENT_SEND_NOTE_BY_UID, &note);
				}
#else
				NOTE_BODY note;
				i3::generic_string_copy( note._sender, Sender);
				i3::generic_string_copy( note._message, Message);
				note._senderSize = (UINT8)(i3::generic_string_size( note._sender) + 1);
				note._messageSize = (UINT8)(i3::generic_string_size( note._message) + 1);

				GameEventSender::i()->SetEvent( EVENT_SEND_NOTE, &note);
#endif
			}
		}
	}

	return true;
}

void UIPopupSendMail_V11::CheckUser( void)
{
	if( GameUI::FindUserUidByNick( m_pWho->getText()) <= 0)
	{
		/* ������ ã�� �� �����ϴ� */
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"));
	}
}

/*virtual*/bool UIPopupSendMail_V11::OnKey_Enter(void)
{
	if (g_pFramework->getFocus() == m_pWho)
	{
		if (m_pNote->getTextRange().empty())			// ��������� ���������� ��Ŀ��..
		{
			g_pFramework->SetFocus(m_pNote);
		}
		else											// ������ ������ �ݴ������� ������...
		{
			if(OnOKButton())
				GetMainFrame()->ClosePopup( UPW_SENDMAIL);
		}
	}
	else
	if (g_pFramework->getFocus() == m_pNote)			// ���� �ۼ��߿� ����Ű�� �׳� �����ǵ�θ� ó��..
	{
		// �ݴ��� ����Ű�� ����Ǽ��� �ȵȴ�..�ϴ� �̴�� �Ӵϴ�..
	}
	else
	if(OnOKButton())
		GetMainFrame()->ClosePopup( UPW_SENDMAIL);

	return true;
}

/*virtual*/ bool UIPopupSendMail_V11::OnKeyInput( i3InputDeviceManager * pInputMng )
{
	i3InputKeyboard*	pKeyboard	= pInputMng->GetKeyboard();
	I3ASSERT( pKeyboard != nullptr);

	// ���������� - ��������� ��Ŀ���� �� tabŰ ������ �޼����ۼ����� ��Ŀ��
	if( pKeyboard->GetStrokeState(I3I_KEY_TAB) )
	{
		if( m_pWho != nullptr && m_pWho->isFocused()
			&& m_pNote != nullptr )
		{
			g_pFramework->SetFocus( m_pNote );
			return true;
		}
	}

	return UIPopupInteractBase::OnKeyInput( pInputMng );
}