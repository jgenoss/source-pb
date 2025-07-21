#include "pch.h"
#include "UIPopupUseItemChangeNick.h"

#include "UIMainFrame.h"
#include "UIFloatCharacterView.h"
#include "UIUtil.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "../GameGUI.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( UIPopupUseItemChangeNick);//, UIPopupUseItemBase);

extern "C" {
	int UIPopupUseItemChangeNick_CheckOverlap( LuaState * L)
	{
		UIPopupUseItemChangeNick * pThis = static_cast<UIPopupUseItemChangeNick*>(GameUI::GetPopup( UPW_CHANGENICK));
		if( pThis != nullptr)
			pThis->DuplicateChangeName();
		return 0;
	}

	int UIPopupUseItemChangeNick_ChangeEd( LuaState * L)
	{
		UIPopupUseItemChangeNick * pThis = static_cast<UIPopupUseItemChangeNick*>(GameUI::GetPopup( UPW_CHANGENICK));
		if( pThis != nullptr)
			pThis->ResetChangeNameCheck();
		return 0;
	}
}

LuaFuncReg UIPopupUseItemChangeNick_Glue[] = {
	{ "CheckOverlap",	UIPopupUseItemChangeNick_CheckOverlap},
	{ "ResetChangeName",	UIPopupUseItemChangeNick_ChangeEd},
	{ nullptr,				nullptr}
};

UIPopupUseItemChangeNick::UIPopupUseItemChangeNick()
{
	m_bChangeNameChecked = false;
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupUseItemChangeNick::~UIPopupUseItemChangeNick()
{

}

/*virtual*/ void UIPopupUseItemChangeNick::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupUseItemBase::_InitializeAtLoad( pScene);

	m_pEd = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox");
	I3ASSERT( m_pEd);
}

/*virtual*/ LuaState * UIPopupUseItemChangeNick::_SetItemInfo( void)
{
	LuaState * L = UIPopupUseItemBase::_SetItemInfo();
	if( L != nullptr)
	{
//		char szCurNick[ MAX_STRING_COUNT];

		i3::wstring wstrResultCurNick;

		if( m_Type == USE_ITEM_CHANGE_CLAN_NAME)
		{
			wstrResultCurNick = ClanGameContext::i()->getMyClanDetailInfo()->m_name;
		}
		else
		{
			// �г���
			USER_INFO_BASIC		MyUserInfoBasic;
			UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
			
			USER_INFO_INVITEM_DATA invData;
			UserInfoContext::i()->GetMyUserInfoInvItemData(&invData);

			i3::wliteral_range wstrNick = MyUserInfoBasic.m_strNick;
			SetNameUsingColorTable( wstrResultCurNick, wstrNick, invData._ui8NickColor);
		}

		i3Lua::PushStringUTF16To8( L, wstrResultCurNick);			
		_EndLuaFunction( L, 2);
	}

	return L;
}

void UIPopupUseItemChangeNick::_UsableNick( bool bEnable)
{
	LuaState * L;

	L = _CallLuaFunction( "CheckUsable");
	if( L != nullptr)
	{
		if( bEnable)
		{
			m_bChangeNameChecked = true;

			I3COLOR color;
			i3Color::Set(&color, (UINT8) 139, 217, 37, 255);

			
			if( m_Type == USE_ITEM_CHANGE_CLAN_NAME)
				i3Lua::PushStringUTF16To8( L, GAME_RCSTRING("STR_POPUP_CAN_USE_CLAN_NAME"));/*��� ������ Ŭ�����Դϴ�.*/
			else
				i3Lua::PushStringUTF16To8( L, GAME_RCSTRING("STR_POPUP_CAN_USE_NICK_NAME"));/*��� ������ �г����Դϴ�.*/

			i3Lua::PushColor( L, &color);
			i3Lua::PushBoolean( L, true);	// activate
		}
		else
		{
			if( m_Type == USE_ITEM_CHANGE_CLAN_NAME)
				i3Lua::PushStringUTF16To8( L, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_CLAN_NAME"));/*���� ������̰ų� ����� �� ���� Ŭ�����Դϴ�*/
			else
				i3Lua::PushStringUTF16To8( L, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_NICKNAME"));/*���� ������̰ų� ����� �� ���� �г����Դϴ�*/

			i3Lua::PushColor( L, GameGUI::GetColor(GCT_RED));
			i3Lua::PushBoolean( L, false);

			g_pFramework->SetFocus( m_pEd);
		}

		_EndLuaFunction( L, 6);
	}
}

void UIPopupUseItemChangeNick::_ChangeNickOk(void)
{
	SetInputDisable(true);

	//	������ ���� ������ �г����� ������.
	
	// ���� EVENT_ITEM_AUTH_DATA���� ��Ŷ ������ ����� ��ô ���� ���� ���¶�...�ǵ��� ������ �����ؾ���...
	//
	const i3::rc_wstring& strChangeName = m_wstrChangeName;
	//
	UINT8 ui8Size = (UINT8)(strChangeName.size()+1) * sizeof(TTCHAR);

	GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, strChangeName.c_str());
}

bool UIPopupUseItemChangeNick::_CheckDuplication( void)
{
	bool ret = true;
	i3::rc_wstring wstrError;

	I3ASSERT( m_pEd != nullptr);

	const wchar_t* wstrSrc = m_pEd->getText();

	//	�̹� üũ�� �������̴�.
	if( m_wstrChangeName == wstrSrc ) return true;

	m_bChangeNameChecked = false;
	m_wstrChangeName = wstrSrc;

	if( m_wstrChangeName.empty() == true )
	{
		switch( m_Type)
		{
		case USE_ITEM_CHANGE_NICK:			wstrError = GAME_RCSTRING("STR_POPUP_MESSAGE_PLEASE_NAME");/*"�г����� �����ּ���."*/	break;
		case USE_ITEM_CHANGE_CLAN_NAME:		wstrError = GAME_RCSTRING("STR_POPUP_MESSAGE_INPUT_CLAN_NAME");/*Ŭ������ �����ּ���.*/	break;
		case USE_ITEM_DISGUISE_NICK:		wstrError = GAME_RCSTRING("STR_POPUP_MESSAGE_INPUT_DISGUISED_NICK_1");/*�ӽ� �г����� �����ּ���.*/	break;
		}

		ret = false;

		goto LABEL_CHECK_NAME_EXIT;
	}

	// �г��Ӹ� �ּұ��ڼ� ����
	if( m_Type == USE_ITEM_CHANGE_NICK || m_Type == USE_ITEM_DISGUISE_NICK)
	{
		if (false == IsValidNicknameLetterSize(m_wstrChangeName) )
		{
			//  �ּ� ���ڼ� ���Եǵ��� ���� �����ؾ��մϴ�.
			wstrError = GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_LENGTH");/*2�ڿ��� 16�� ���̷� �����Ͽ� �ֽñ� �ٶ��ϴ�.*/

			ret = false;

			goto LABEL_CHECK_NAME_EXIT;
		}
	}

	// ��ȿ�� �˻�
	{
		if ( ! g_pFramework->CheckValidNickname(m_wstrChangeName) )
		{
			//	�Է� �� �� ���� �ܾ ���ԵǾ� �ֽ��ϴ�"
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
			ret = false;
			goto LABEL_CHECK_NAME_EXIT;
		}
	}

LABEL_CHECK_NAME_EXIT:
	//	�⺻ �˻�
	if( ret)
	{
		switch( m_Type)
		{
		case USE_ITEM_DISGUISE_NICK:
		case USE_ITEM_CHANGE_NICK:			GameEventSender::i()->SetEvent(EVENT_USE_ITEM_CHECK_NICK, m_wstrChangeName.c_str());		break;
		case USE_ITEM_CHANGE_CLAN_NAME:		g_pFramework->GetClanContext()->SendOverlappingClanName(m_wstrChangeName);	break;
		}

		return true;
	}
	else
	{
		g_pFramework->SetFocus( m_pEd);

		LuaState * L = _CallLuaFunction( "SetCheckDesc");
		if( L != nullptr)
		{
			i3Lua::PushStringUTF16To8( L, wstrError);
			i3Lua::PushColor( L, GameGUI::GetColor( GCT_DEFAULT));
			_EndLuaFunction( L, 5);
		}

		return false;
	}
}

void UIPopupUseItemChangeNick::_ChangeClanNameOk(void)
{
	//i3UI::setDisableUIInput( true);
	SetInputDisable( true);

	//	������ ���� ������ �г����� ������.

	g_pFramework->GetClanContext()->ProcessChangeClanName( m_AuthWareDBIndex, m_wstrChangeName);

}

void UIPopupUseItemChangeNick::_ChangeDisguiseNickOk(void)
{
	//i3UI::setDisableUIInput( true);
	SetInputDisable( true);

	// ���� ������ �г��� ���� ���� ������ ����� ���� �ʾƼ� ������� ������...
	const i3::rc_wstring& strChangeName = m_wstrChangeName;
	
	//	������ �ۼ��� �ӽ� �г����� ������.
	UINT8 ui8Size = (UINT8)(strChangeName.size()+1) * sizeof(TTCHAR);
	GameEventSender::i()->SetEvent( EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, strChangeName.c_str());
}


/*virtual*/ void UIPopupUseItemChangeNick::OnCreate( i3GameNode * pParent)
{
	UIPopupUseItemBase::OnCreate( pParent);
	
	AddScene( "Scene/Popup/PBRe_PopUp_useItemChangeName.i3UIs" , UIPopupUseItemChangeNick_Glue);
}

/*virtual*/ void UIPopupUseItemChangeNick::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UIPopupUseItemBase::ProcessGameEvent(event,arg, UserData);

	switch( event)
	{
	case EVENT_USE_ITEM_CHECK_NICK:
	case EVENT_CLAN_DUPLICATE_NAME:
		_UsableNick( (bool)(arg == 0));
		break;

	case EVENT_ITEM_AUTH_DATA :
		{
			// ���� ���Ǿ��ٸ� �ݿ��ؾ� �ϴ� UI�� �ִ�.
			UIFloatCharacterView * pCharaView = static_cast<UIFloatCharacterView *>(GameUI::GetFloating( UFW_CHARACTER));
			if( pCharaView != nullptr)
				pCharaView->DoUpdateCharacter();
		}
		break;
	}
}

/*virtual*/ void UIPopupUseItemChangeNick::OnEntranceEnd()
{
	UIPopupUseItemBase::OnEntranceEnd();

	g_pFramework->SetFocus( m_pEd);
	m_pEd->SetText(L"");
}

void UIPopupUseItemChangeNick::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupUseItemBase::OnUpdate(rDeltaSeconds);

	i3UIControl* pCurCtrl = reinterpret_cast<i3UIControl*>(g_pFramework->getFocus());
	if( m_pEd == pCurCtrl )
	{
		if( GameUI::IsKeyboardStroked(I3I_KEY_ENTER) == true )
		{
			g_pFramework->SetFocus( 0 );
			DuplicateChangeName();
		}	
	}
}

/*virtual*/ bool UIPopupUseItemChangeNick::OnOKButton( void)
{
	if( (m_pEd != nullptr) && (m_pEd->getText() != nullptr) )
	{
		INT32 len = i3::generic_string_size( m_pEd->getText());
		if( len > 0)
		{
			return ChangeNameOK();
		}
	}

	return false;
}

bool UIPopupUseItemChangeNick::ChangeNameOK( void)
{
	LuaState * L;

	if( m_Type == USE_ITEM_CHANGE_NICK || m_Type == USE_ITEM_CHANGE_CLAN_NAME || m_Type == USE_ITEM_DISGUISE_NICK)
	{
		//	���� �г��Ӱ� Ŭ���� ����
		if(m_bChangeNameChecked == false)
		{
			/*�ߺ��˻縦 ���ֽʽÿ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));

			return false;
		}
		else
		{
			//	char szInputName[MAX_STRING_COUNT] = "";

			INT32 len = i3::generic_string_size( m_pEd->getText());

			if( len > 0)
			{
				//i3String::CopyWCHARtoASCII( szInputName, sizeof(szInputName), m_pEd->getText(), len);
				i3::wliteral_range wInputNameRange = m_pEd->getText();
			
				//	�ߺ��˻�� �г��Ӱ� �Է�â�� �г����� �ٸ��� �ٽ� �ߺ��˻縦 �ϵ��� �Ѵ�.
				if( !i3::generic_is_equal(m_wstrChangeName, wInputNameRange) )
				{
					m_bChangeNameChecked = false;

					L = _CallLuaFunction( "ShowOverlapMessage");
					if( L != nullptr)
					{
						_EndLuaFunction( L, 1);
					}
					//m_pChangeNameCheckStatic->SetTextColor( GameGUI::GetColor(GCT_DEFAULT));
					//m_pChangeNameCheckStatic->SetText(GAME_RCSTRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"));/*�ߺ��˻縦 ���ֽʽÿ�.*/

					g_pFramework->SetFocus( m_pEd);

					return false;
				}

				switch( m_Type)
				{
				case USE_ITEM_CHANGE_NICK:			_ChangeNickOk();			break;
				case USE_ITEM_CHANGE_CLAN_NAME:		_ChangeClanNameOk();		break;
				case USE_ITEM_DISGUISE_NICK:		_ChangeDisguiseNickOk();	break;
				}

				return false;
			}
		}
	}

	return false;
}

bool UIPopupUseItemChangeNick::DuplicateChangeName( void)
{
	return _CheckDuplication();
}

void UIPopupUseItemChangeNick::CbChangeNickWarnning(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{	// ��â�� �ٽ� ����.
		((UIPopupUseItemChangeNick*)pThis)->GetMainFrame()->OpenPopup( UPW_QUERY_ITEM, &(((UIPopupUseItemChangeNick*)pThis)->m_ItemID));
	}
	else
	{
		((UIPopupUseItemChangeNick*)pThis)->GetMainFrame()->ClosePopup( UPW_CHANGENICK);
	}
}

