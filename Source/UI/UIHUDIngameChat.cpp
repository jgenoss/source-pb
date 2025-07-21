#include "pch.h"

#include "UIHUDIngameChat.h"
#include "UIBattleFrame.h"

#include "BattleGui.h"
#include "BattleHud.h"
#include "UIUtil.h"

#include "../GameMaster.h"

#include "GameGUI.h"

#include "UI/UIHudManager.h"
#include "UI/UIHudPlayerState.h"
#include "UI/UIHudNotice.h"

#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/format_string.h"

#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "GMAuthorityContext.h"
#include "GameMissionManager.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#include "UI/UIBattleFrame.h"
#include "UI/UIHudObserver.h"
#include "UI/UIHudMultiWeapon.h"


namespace
{
	const REAL32 USERLIST_UPDATE_TIME = 0.5f;
	REAL32 g_LocalTime = 0.0f;
	INT32 g_Last_Buffer_Idx = 0;	// 10�ʵ��� ä���� ���� �� ������ ���� �ε���
	INT32 g_Old_Buffer_Idx = 0;		// ������Ʈ ���� ������ ���� �ε��� (���� ���� ä������ Ȯ���ϱ� ���� ��)
	INT32 g_WhisperLength = 0;
	size_t inputed_text_length = 0; //�Էµ� string size

	const size_t MAX_CHATBOX_BUFF = MAX_CHAT_BUFFER * (MAX_STRING_COUNT + MAX_STRING_COUNT + 10);
	const size_t GAME_CHATLINE_MAX = 10;
	const size_t CHATBOXPOS_Y = 610;

	void init_global_value(void)
	{
		g_LocalTime = 0.0f;
		g_Last_Buffer_Idx = 0;
		g_Old_Buffer_Idx = 0; 
	}
}

UIHUDIngameChat* UIHUDIngameChat::instance()
{
	UIBattleFrame * pFrame = UIBattleFrame::i();
	return (UIHUDIngameChat*) pFrame->GetHUD( UIHUD_INGAMECHATING );
}

I3_CLASS_INSTANCE( UIHUDIngameChat);//, UIHUDBase);

UIHUDIngameChat::UIHUDIngameChat()
{
#if defined( I3_DEBUG)
	SetName( "ChatBox");
#endif

	m_keyboard_mgr = 0;
	m_chatbox_mgr = 0;

	m_pChatMainBox = nullptr;
	m_pSubChatMain = nullptr;

	m_nChatType = CHATTING_TYPE_ALL;
	m_nOldChatType = CHATTING_TYPE_ALL;

	m_bOnceChatType = false;
	m_bOnceChatTypeAll = false;

	m_lastWhisperUserNick[ 0] = 0;

	m_bChatScrollEnable = false;
	m_bUseHistory = false;
	m_bUseOneTimeTeamChat = false;
	m_bEnableChat = false;
}

UIHUDIngameChat::~UIHUDIngameChat()
{
}

/*virtual*/ void UIHUDIngameChat::_InitializeAtLoad( i3UIScene * pScene)
{
	UIHUDBase::_InitializeAtLoad( pScene);

	m_pFrameWnd = (i3UIFrameWnd*) pScene->FindChildByName( "Chatting");

	m_pChatMainBox = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd");
	m_pSubChatMain = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd0");

	m_pTextBox = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox");
	m_pTextBox->getScrollBar()->setTickSize(10);
	m_pTextBg = (i3UIImageBox*) pScene->FindChildByName( "i3UIImageBox0");

	m_pChatBox = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox0");
	m_pChatBox->SetTextColor( GameGUI::GetColor(GCT_DEFAULT) );

	m_pFrameWnd->EnableCtrl(false);
}

/*virtual*/ void UIHUDIngameChat::_ClearAtUnload( i3UIScene * pScene)
{
	UIHUDBase::_ClearAtUnload( pScene);

	m_pFrameWnd = nullptr;

	m_pChatMainBox = nullptr;
	m_pSubChatMain = nullptr;

	m_pTextBox = nullptr;
	m_pChatBox = nullptr;

	m_bEnableChat = false;
}

void UIHUDIngameChat::SetEnableChat(bool bchat)
{
	i3_prof_func();

	if( CGameMaster::i()->IsPaused() && (UserInfoContext::i()->IsGM_Observer() == false) )
		return;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		return;

	if(false == UIBattleFrame::i()->IsPopupFocused())
	{
		m_bEnableChat = bchat;
		m_pChatMainBox->EnableCtrl(bchat);
		m_pChatBox->setChatEnable(bchat);

		m_bChatScrollEnable = bchat;	
	
		if(bchat)
		{
			re_position();

			i3UI::getGlobalIme()->SetInputText( L"", 0);

			m_pChatBox->SetText(L"");

			i3UI::getGlobalIme()->SetFocus(true);
			g_pFramework->SetFocus( m_pChatBox);
			g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);

			m_pTextBox->enableScrollBar(false);
			m_pTextBg->EnableCtrl(true);
			
			const i3::rc_wstring& wstrUserNick = GMAuthorityContext::i()->GetUserNickName();

			if (wstrUserNick.size() > 0)
			{
				OnWhisper(GMAuthorityContext::i()->GetUserNickName());
				m_nChatType = CHATTING_TYPE_REPLY;
			}
		}
		else
		{
			i3UI::getGlobalIme()->KillFocus();
			g_pFramework->SetFocus(0);

			if(!m_bChatScrollEnable)
				g_pFramework->SetGameKeyMode( GAMEKEY_MODE_GAME);

			m_pTextBox->enableScrollBar(false);
			m_pTextBg->EnableCtrl(true);
		}
		
		UpdateChatBuffer();

		observer_hud::Context::i()->Notify(observer_hud::EVT_CHATBOX_APPEARED, bchat);
	}
}


void UIHUDIngameChat::setting_language( void)
{
	i3_prof_func();


	HKL LanguageList[6];

	// ���� ���� ����
	INT32 LanguageCount = ::GetKeyboardLayoutList( 6, LanguageList);
	for(INT32 i = 0; i < LanguageCount; i ++)
	{
		LANGID src = LOWORD( LanguageList[i]);

		for(INT32 j = i+1; j < LanguageCount; j++)
		{
			LANGID dest = LOWORD( LanguageList[j]);

			if( src == dest)
			{
				INT32 k = j;
				while( k-1 < LanguageCount-1)
				{
					LanguageList[k] = LanguageList[k+1];
					k++;
				}

				LanguageCount--;
				j--;
			}
		}
	}

	i3Ime * pIME = i3UI::getGlobalIme();
	if( pIME == nullptr)	return;

	WPARAM param = INPUTLANGCHANGE_FORWARD;
	SendMessage( pIME->getHwnd(), WM_INPUTLANGCHANGEREQUEST, param, (LPARAM) LanguageList[0]);
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIHUDIngameChat::OnCreate( i3GameNode * pParent)
{
	UIHUDBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/HUD/PBRe_Ingame_Chatting.i3UIs", nullptr);

	setting_language();
}

/*virtual*/ bool UIHUDIngameChat::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	i3_prof_func();


	bool bRv = UIHUDBase::OnEvent( event, pObj, param2, code);

	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_CLEAR :
		case EDIT_STATE_RETURN:
		case EDIT_STATE_CHANGE_COMPOSITION_CHAR :
		case EDIT_STATE_CHANGE_INPUT_CHAR :
				_UpdateParsingType( (EDIT_STATE) param2);
			break;

		case EDIT_STATE_ONKEYDOWN_UP:
		case EDIT_STATE_ONKEYDOWN_DOWN:
			_ChatHistory((EDIT_STATE) param2);
			break;

		case EDIT_STATE_CHANGE_LANGUAGE :
		case EDIT_STATE_CHANGE_IME_MODE :
			_UpdateLang();
			break;
		}
	}

	return bRv;
}

/*virtual*/ bool UIHUDIngameChat::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	i3_prof_func();

	bool res = UIHUDBase::OnKeyInput( pInputMng);

	_UpdateMacroChat();
	_UpdateChatType();

	return res;
}

/*virtual*/ bool UIHUDIngameChat::OnKey_Enter( void)
{
	i3_prof_func();

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true) //Ʃ�丮�� ���� ä���� �� �Ѵ�.
	{
		SetEnableChat( false);
		return false;
	}

	BOOL bMultiWeapon = TRUE;

	if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

		if (!pHud->IsVisible())
		{
			bMultiWeapon = FALSE;
		}
	}

	if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && bMultiWeapon)
	{
		SetEnableChat( FALSE);
		return false;
	}

	if( m_keyboard_mgr->GetPressState( I3I_KEY_LCTRL ) == true )
	{
		SetEnableChat( true );
		return false;
	}

	if(m_bEnableChat)
	{
		_UpdateParsingType( EDIT_STATE_RETURN );

		SetEnableChat( false );

		if(inputed_text_length)
		{
			m_pTextBg->EnableCtrl(true);
			m_pTextBox->enableScrollBar(false);

			g_LocalTime = 0.f;
			m_bChatScrollEnable = false;
		}
	}
	else
	{
		if (isVisible() == false)
			SetVisible(true);

		SetEnableChat( true );
	}

	return false;
}


/*virtual*/ bool UIHUDIngameChat::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIHUDBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		SetEnable(false);
		SetEnableChat(false);
		return false;
	}

	m_pFrameWnd->EnableCtrl(true);

	GetSlide()->ForceEnd();

	m_pTextBox->SetText( L"");
	m_pChatBox->SetText(L"");

	m_keyboard_mgr = g_pFramework->getKeyboard();
	m_chatbox_mgr = g_pFramework->getChatBox();

	// ä�� ���� �ʱ�ȭ
	m_chatbox_mgr->ResetChatBuffer();

	// Update Whisper Backup Buffer
	_UpdateWhisperBackupBuffer();

	// ä��ä�� �ʱ�ȭ
	SelectChatChannel( 0);

	// �Է� ��� ��ư
	_UpdateLang();

	// ä�ý�ũ�� �ʱ�ȭ
	SetEnableChatScroll(false);

	init_global_value();
	SetEnableChat(false);

#if defined(LOCALE_RUSSIA)
	m_pChatBox->setUseCopyNPaste(false);
#else
	m_pChatBox->setUseCopyNPaste(true);
#endif

	return true;
}

void UIHUDIngameChat::PrintStartingMessage()
{

	i3_prof_func();

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true) return; //Ʃ�丮�� ���� ä���� �� �Ѵ�.

	m_chatbox_mgr = g_pFramework->getChatBox();
	I3ASSERT( m_chatbox_mgr != nullptr);

	// �ʹ� ����� �޼���
	m_chatbox_mgr->PutSystemChatting(g_pFramework->getChatNotice());

}

void UIHUDIngameChat::OnLoadAllScenes()
{
	LuaState * L = _CallLuaFunction( "LoadAllScenes");
	UILua::EndLuaFunction(L , 0);
}


void	UIHUDIngameChat::OnEntranceEnd()
{
	UIHUDBase::OnEntranceEnd();

	LuaState* L = _CallLuaFunction( "SetFocusChatEdit");
	_EndLuaFunction(L, 0);
}

/*virtual*/ bool UIHUDIngameChat::OnExitStart( void)
{
	if( UIHUDBase::OnExitStart() == false)
		return false;

	GetSlide()->ForceEnd();

	return true;
}

void UIHUDIngameChat::SelectChatChannel( INT32 idx)
{
	i3_prof_func();

	switch( m_Tab[idx])
	{
	case CHAT_TYPE_ALL :		SetChatType( CHATTING_TYPE_ALL);	break;
	case CHAT_TYPE_TEAM :		SetChatType( CHATTING_TYPE_TEAM);	break;
	case CHAT_TYPE_CLAN:		SetChatType( CHATTING_TYPE_CLAN);	break;
	case CHAT_TYPE_WHISPER :	SetChatType( CHATTING_TYPE_WHISPER);	break;
	}
	m_CurrentTab = m_Tab[idx];

	UpdateChatBuffer();
}

void UIHUDIngameChat::OnUpdate(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if ( CGameMaster::i()->IsPaused() && UserInfoContext::i()->IsGM_Observer() )
	{
		return;
	}

	UIHUDBase::OnUpdate(rDeltaSeconds);

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true) return; //Ʃ�丮�� ���� ä���� �� �Ѵ�.

	if(!m_bEnableChat)
	{
		UINT64 nStrokeKey = g_pFramework->getKeyStroke();

		BOOL bMultiWeapon = TRUE;

		if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);

			if (pHud->IsVisible() && !pHud->GetAutoView())
			{
				bMultiWeapon = FALSE;
			}
		}

		// Chat Scroll
		if( ( nStrokeKey & GAME_KEY_FUNC_CHATSCROLL || g_pFramework->getSysKeyStroke() & GAME_KEY_FUNC_CHATSCROLL) && bMultiWeapon)
		{
			if( !CBattleGui::i()->GetEnablePopup() )
			{
				// �̼������� ��ź ��ġ�� ������ �˴ϴ�. ��ġ�� ��ü���̶�� �۵����� ���ƾ��մϴ�.
				CGameMissionManager * pMissionMng = CGameMissionManager::i(); 
				INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();

				if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) &&( pMissionMng->IsInstalling(mySlotIdx) || pMissionMng->IsUninstalling(mySlotIdx) ) )
				{
				}
				else
				{
					SetEnableChatScroll(!m_bChatScrollEnable);
				}
			}
		}
	}

	//�ΰ��� ä�� ���� ������Ʈ�Դϴ�.
	if(!GetEnableChatScroll())
	{
		UpdateHistoryChatBuffer( rDeltaSeconds);
		UpdateChatBuffer();	
	}
}


void UIHUDIngameChat::_UpdateMacroChat(void)
{
	i3_prof_func();


	int idx = -1;

	// Macro�� �̿��� �Է�ó��
	CHAT_INFO info;

	if( m_keyboard_mgr->GetPressState( I3I_KEY_LALT ))
	{
		for(size_t i = I3I_KEY_1; i <= I3I_KEY_5; ++i )
		{
			if( m_keyboard_mgr->GetStrokeState( (I3I_KEY)i ) )
			{
				idx = i - I3I_KEY_1;
				break;
			}
		}
	}

	if( idx == -1 ) return;
	if( i3::generic_string_size( g_pEnvSet->m_wstrMacro[idx]) <= 0) return;

	// �弳����
	i3::rc_wstring wstrErrorMsg;

	if( g_pFramework->CheckFilter_EnvSetMacroString(g_pEnvSet->m_wstrMacro[idx], wstrErrorMsg) )
	{
		if (  !TestCommand( g_pEnvSet->m_wstrMacro[idx]) )
		{
			info._Type = (g_pEnvSet->m_bMacroEnableFriend[idx] == 0) ? (UINT16)CHATTING_TYPE_ALL : (UINT16)CHATTING_TYPE_TEAM;
			
			i3::rc_wstring wstrMacro = g_pEnvSet->m_wstrMacro[idx];
			info.setString( wstrMacro );
			
			/*info.setString( g_pEnvSet->m_wstrMacro[idx]);*/

			GameEventSender::i()->SetEvent(EVENT_CHATTING_N , &info ); 
		}
	}
	else
	{
		g_pFramework->getChatBox()->PutSystemChatting(wstrErrorMsg);
	}
}

void UIHUDIngameChat::_UpdateLang(void)
{

	i3_prof_func();

	i3Ime * pIME = i3UI::getGlobalIme();
	if( pIME == nullptr)	return;

	i3::rc_wstring wstr_language;

	LANGID langId = pIME->getLangID();
	UINT32 nInputLang = PRIMARYLANGID( langId);
	UINT32 nInputSubLang = SUBLANGID( langId);

	// mode off is english
	if( pIME->GetImeMode() != IME_MODE_NATIVE)
		nInputLang = LANG_ENGLISH;

	//	Language mark
	switch(GetCodePage())
	{
	case I3_LANG_CODE_KOREAN:
		{
			switch( nInputLang)
			{
			case LANG_KOREAN:	wstr_language = GAME_RCSTRING("STR_TBL_CHAT_INGAME_CHAT_TARGET_KOREAN");/*��*/	break;
			case LANG_ENGLISH:	wstr_language = GAME_RCSTRING("STR_TBL_CHAT_INGAME_CHAT_TARGET_ENGLISH");/*��*/	break;
			}
		}
		break;
	case I3_LANG_CODE_THAI:
		{
			switch( nInputLang)
			{
			case LANG_THAI:		wstr_language = L"TH";		break;
			case LANG_ENGLISH:	wstr_language = L"EN";		break;
			}
		}
		break;
	case I3_LANG_CODE_JAPANESE:
		{
			switch( nInputLang)
			{
			case LANG_JAPANESE:	wstr_language = L"JP";	break;
			case LANG_ENGLISH:	wstr_language = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:
	case I3_LANG_CODE_CHINESE_TRADITIONAL:
		{
			switch( nInputLang)
			{
			case LANG_CHINESE:	wstr_language = L"CH";	break;
			case LANG_ENGLISH:	wstr_language = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_RUSSIA:
		{
			switch( nInputLang)
			{
			case LANG_RUSSIAN:	wstr_language = L"RU";	break;
			case LANG_ENGLISH:	wstr_language = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_TURKISH:
		{
			switch( nInputLang)
			{
			case LANG_TURKISH:	wstr_language = L"TR";	break;
			case LANG_ENGLISH:	wstr_language = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_WESTEUROPE:
		{
			switch( nInputLang)
			{
			case LANG_PORTUGUESE:	
				{
					switch( nInputSubLang)
					{
					case SUBLANG_PORTUGUESE:			wstr_language = L"PR";	break;
					case SUBLANG_PORTUGUESE_BRAZILIAN:	wstr_language = L"BR";	break;
					}
				}
				break;
			case LANG_SPANISH:	wstr_language = L"SP";	break;
			case LANG_ITALIAN:		wstr_language = L"IT";	break;
			case LANG_ENGLISH:	wstr_language = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_ARABIC:
		{
			switch( nInputLang)
			{
			case LANG_ARABIC:	wstr_language = L"AR";	break;
			case LANG_ENGLISH:	wstr_language = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_VIETNAMESE:
		{
			switch( nInputLang)
			{
			case LANG_VIETNAMESE:	wstr_language = L"VN";	break;
			case LANG_ENGLISH:		wstr_language = L"EN";	break;
			}
		}
		break;
	}

#if defined ( LOCALE_NORTHAMERICA)
	wstr_language = L"";	//	�Ϲ��� ��û���� �Ⱥ��̰� ó��
#endif

	LuaState * L = _CallLuaFunction( "SetLanguage");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstr_language );
		_EndLuaFunction( L, 1);
	}
}

void UIHUDIngameChat::_ChatHistory(EDIT_STATE state)
{
	i3_prof_func();

	i3::rc_wstring	wstrHistoryBufferText;

	m_chatbox_mgr = g_pFramework->getChatBox();
	I3ASSERT( m_chatbox_mgr != nullptr);

	// Chat History���
	if( m_chatbox_mgr->GetHistoryBufferCount() > 0)
	{
		INT32 nChatMode = CHATTING_TYPE_ALL;

		if (state == EDIT_STATE_ONKEYDOWN_UP )
		{
			m_bUseHistory = true;

			if( m_chatbox_mgr->GetHistoryBufferOffset() == 0)
			{
				m_nOldChatType = m_nChatType;
			}

			m_chatbox_mgr->GetHistoryBufferText(1, wstrHistoryBufferText, &nChatMode);

			if(!wstrHistoryBufferText.empty())		// szText[0] != nullptr)
			{
				m_nChatType = nChatMode;
			}
			else
			{
				i3UI::getGlobalIme()->SetInputText( L"", 0);
			}

			g_pFramework->SetFocus( m_pChatBox);
		}
		else if (state == EDIT_STATE_ONKEYDOWN_DOWN)
		{
			m_bUseHistory = true;

			m_chatbox_mgr->GetHistoryBufferText(-1, wstrHistoryBufferText, &nChatMode);

			if(!wstrHistoryBufferText.empty())		// szText[0] != nullptr)
			{
				m_nChatType = nChatMode;
			}
			else
			{
				m_bUseHistory = false;
				i3UI::getGlobalIme()->SetInputText( L"", 0);
				m_nChatType = m_nOldChatType;
			}
		}
	}

	COLOR col;
	i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
	i3::wstring wstrText;
	CHAT_SYS_TYPE ChatType = m_chatbox_mgr->ParsingText( wstrHistoryBufferText );			// �Ľ��� ������ �����丮 ��ȭ�� ������...�װ� ����..
	

	if( ChatType == CHAT_SYS_TYPE_WHISPER || ChatType == CHAT_SYS_TYPE_REPLY)
	{
		size_t index = wstrHistoryBufferText.rfind(L" ");

		if( index != i3::rc_wstring::npos )
		{
			i3::const_wchar_range word( wstrHistoryBufferText.begin() + index + 1, wstrHistoryBufferText.end() );

			if( ChatType == CHAT_SYS_TYPE_WHISPER )
			{
				_ToWhisperChatMessage(wstrText, m_chatbox_mgr->GetChatTarget(), word );
				m_nChatType = CHATTING_TYPE_WHISPER;
			}
			else
			{
				_ToWhisperChatMessage(wstrText, m_chatbox_mgr->GetChatReplyTarget(), word );
				m_nChatType = CHATTING_TYPE_REPLY;
			}
			i3Color::Set(&col, GameGUI::GetColor(GCT_WISPER_CHAT));
		}
	}
	else
	{
		wstrText = wstrHistoryBufferText;
	}

	g_pFramework->SetFocus( nullptr );
	m_pChatBox->SetText( wstrText );
	g_pFramework->SetFocus( m_pChatBox );

	m_pChatBox->SetTextColor( &col);
}

void UIHUDIngameChat::_ToWhisperChatMessage(i3::wstring& out, const i3::rc_wstring& target, const i3::wliteral_range& msg )
{
	i3_prof_func();

	i3::sprintf(out, L"[To:%s] %s", target, msg);
}

void UIHUDIngameChat::_UpdateParsingType( EDIT_STATE state)
{
	i3_prof_func();

	inputed_text_length = i3::generic_string_size( m_pChatBox->getText() );
	if( inputed_text_length == 0) return;

	if( m_pChatBox->GetIME() == nullptr) return;

	COLOR col;
	CHAT_SYS_TYPE ChatType = CHAT_SYS_TYPE_NONE;

//	char szTarget[256];				// ���� ������� ���õ� �����Լ��� ��� �ּ�ó��(2014.07.21.����)

	i3::wstring wstrWhisper;


//	szTarget[0] = 0;

	INT32 nWhsiperTextCount = 0;

	m_chatbox_mgr = g_pFramework->getChatBox();
	I3ASSERT( m_chatbox_mgr != nullptr);

	i3::wstring wstrImeBuf = m_pChatBox->getIMETextBufString();	// ������ ���� �ʰ� �ǵ������� �����Ѵ�.(�޺κп� ������ ������ �ִ�..)  
																// 2014.07.21.����
	if( m_bOnceChatTypeAll)
	{
		i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
//		i3::generic_string_cat( szTarget, strImeBuf.c_str());
	}
	else if( m_bOnceChatType)
	{
		i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
//		i3::generic_string_cat( szTarget, strImeBuf.c_str());
	}
	else
	{
		switch( m_nChatType)
		{
		case CHATTING_TYPE_ALL:			// ��ü
		case CHATTING_TYPE_MATCH:
		case CHATTING_TYPE_CLAN_MEMBER_PAGE:
		case CHATTING_TYPE_LOBBY:
			i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
//			i3::generic_string_cat( szTarget, strImeBuf.c_str());
			break;
		case CHATTING_TYPE_TEAM:		// ��
			i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
//			i3::generic_string_cat( szTarget, strImeBuf.c_str());
			break;
		case CHATTING_TYPE_CLAN:		// Ŭ��
			i3Color::Set(&col, GameGUI::GetColor(GCT_CLAN_CHAT));
//			i3::generic_string_cat( szTarget, strImeBuf.c_str());
			break;
		case CHATTING_TYPE_WHISPER:		// �Ӹ�
			i3Color::Set(&col, GameGUI::GetColor(GCT_WISPER_CHAT));
			break;
		case CHATTING_TYPE_REPLY:		// �亯
			i3Color::Set(&col, GameGUI::GetColor(GCT_WISPER_CHAT));
			break;
		default: break;
		}
	}

//	szTarget[i3::generic_string_size( szTarget)] = 0;

	m_chatbox_mgr->SetEnable( true);
	m_pChatBox->SetTextColor( &col);


	if( m_nChatType != CHATTING_TYPE_WHISPER && m_nChatType != CHATTING_TYPE_REPLY)
	{
		ChatType = m_chatbox_mgr->ParsingText( wstrImeBuf );
		{
			switch( ChatType)
			{
			case CHAT_SYS_TYPE_WHISPER:
				{
					if( !m_bUseHistory) m_nOldChatType = m_nChatType;
					m_nChatType = CHATTING_TYPE_WHISPER;
					SelectChatChannel(CHAT_TYPE_WHISPER);

					_ToWhisperChatMessage(wstrWhisper, m_chatbox_mgr->GetChatTarget() );

					g_WhisperLength =	wstrWhisper.length(); //	i3::generic_string_size( szWhisper);
					
					wstrWhisper += m_chatbox_mgr->GetHistoryText();
					
					g_pFramework->SetFocus( nullptr);
					m_pChatBox->SetText( wstrWhisper );
					g_pFramework->SetFocus( m_pChatBox);
					OnWhisper(m_chatbox_mgr->GetChatTarget());
				}
				break;
			case CHAT_SYS_TYPE_REPLY:
				{
					if( !m_bUseHistory) m_nOldChatType = m_nChatType;
					m_nChatType = CHATTING_TYPE_REPLY;
					if( i3::generic_string_size(m_chatbox_mgr->GetChatReplyTarget()) > 0)
					{
						_ToWhisperChatMessage(wstrWhisper, m_chatbox_mgr->GetChatReplyTarget() );

						g_WhisperLength = wstrWhisper.length(); // i3::generic_string_size( szWhisper);
						
						wstrWhisper += m_chatbox_mgr->GetHistoryText();

						g_pFramework->SetFocus( nullptr);
						m_pChatBox->SetText( wstrWhisper );
						g_pFramework->SetFocus( m_pChatBox);
					}
					else
					{
						m_nChatType = m_nOldChatType;
						m_pChatBox->SetText(L"");
						i3UI::getGlobalIme()->SetInputText( L"", 0);
						g_pFramework->SetFocus( m_pChatBox);
					}
				}
				break;
			case CHAT_SYS_TYPE_ONETIME_TEAM:	// ��ȸ�� �� ê��!!
				m_bUseOneTimeTeamChat = true;
				if( !m_bUseHistory) m_nOldChatType = m_nChatType;
				m_nChatType = CHATTING_TYPE_TEAM;
				SelectChatChannel(CHAT_TYPE_TEAM);

				m_bOnceChatType = true;
				m_pChatBox->SetText(L"");
				i3UI::getGlobalIme()->SetInputText( L"", 0);
				g_pFramework->SetFocus( m_pChatBox);
				break;
			}
		}
	}

	if( m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
	{
		// �ӼӸ� ���¿��� �ƹ��� �ؽ�Ʈ�� �Էµ��� �������¿��� ���ڰ� �������� �ʱ�ȭ�մϴ�.
		i3::string strConv;
		i3::utf16_to_mb(m_pChatBox->getText(), strConv);
		GameUI::RemoveColorKeyword( strConv);

		INT32 len = INT32(strConv.size());

		if( len  < g_WhisperLength)
		{
			m_pChatBox->SetText( L"");
			i3UI::getGlobalIme()->SetInputText( L"", 0);
			m_nChatType = m_nOldChatType;
			g_WhisperLength = 0;
		}

		if( m_nChatType == CHATTING_TYPE_WHISPER)
		{
			_ToWhisperChatMessage(wstrWhisper, m_chatbox_mgr->GetChatTarget() );
				
			//if( i3String::Contain( szContents, szWhisper) == -1)
			if (i3::contain_string(m_pChatBox->getText(), wstrWhisper) == -1 )
			{
				m_pChatBox->SetText( L"");
				i3UI::getGlobalIme()->SetInputText( L"", 0);
				m_nChatType = m_nOldChatType;
				g_WhisperLength = 0;
			}
		}

		if( m_nChatType == CHATTING_TYPE_REPLY)
		{
			
			_ToWhisperChatMessage(wstrWhisper, m_chatbox_mgr->GetChatReplyTarget() );

			//if( i3String::Contain( szContents, szWhisper) == -1)
			if (i3::contain_string(m_pChatBox->getText(), wstrWhisper) == -1 )
			{
				m_pChatBox->SetText( L"");
				i3UI::getGlobalIme()->SetInputText( L"", 0);
				m_nChatType = m_nOldChatType;
				g_WhisperLength = 0;
			}
		}
	}

	bool bWhisper = false;
	bool bReply = false;
	INT32 nReplyTextCount = 0;

	if( state != EDIT_STATE_RETURN)
		return;

	switch( ChatType)
	{
	case CHAT_SYS_TYPE_CLEAR:
		m_chatbox_mgr->ResetChatBuffer();
		m_pChatBox->SetText(L"");
		i3UI::getGlobalIme()->SetInputText( L"", 0);
		break;
	case CHAT_SYS_TYPE_HELP:
		m_chatbox_mgr->SetHelpText();
		m_pChatBox->SetText(L"");
		i3UI::getGlobalIme()->SetInputText( L"", 0);
		break;
	}

	g_pFramework->ChangeFilterStringOK(wstrImeBuf);

	CChattingInputChecker* input_checker = g_pFramework->GetChattingInputChecker();
	
	input_checker->UpdateText(wstrImeBuf);

	//�׽�Ʈ ���
	if(TestCommand(wstrImeBuf))
	{

	}
	else if( !(( ChatType == CHAT_SYS_TYPE_CLEAR) || ( ChatType == CHAT_SYS_TYPE_HELP)) 
				&& input_checker->IsInputBlock() 
				&& ( UserInfoContext::i()->IsGM_Observer() == false ))
	{
		m_chatbox_mgr->PutSystemChatting(GAME_RCSTRING("STR_TBL_CHAT_INGAME_NOTICE_WARNING_REPEAT_WORD"));/*<���踦 �����ϱ� ���� �������� ä���� �Էµ��� �ʽ��ϴ�.>*/
	}
	else if( input_checker->IsRepeatBlock() ) {
		m_chatbox_mgr->PutSystemChatting(GAME_RCSTRING("STR_TBL_CHAT_INGAME_NOTICE_WARNING_DUPLICATED_WORD"));/*<���� ������ �����Ͽ� �Է��� �� �����ϴ�.>*/
	}
	else
	{
		// ��Ʈ��ũ�� �޽����� �����ϴ�.

		i3::stack_wstring wstrTemp;
		i3::stack_wstring wstrTemp2;		

		CHAT_INFO info;
		info._Type = (UINT16)m_nChatType;

		if(ChatType == CHAT_SYS_TYPE_CLEAR)
			info._Type = CHATTING_TYPE_ALL;

		if( m_bOnceChatTypeAll)
		{
			if( IsGhostMessage())
			{
				wstrTemp = GAME_RCSTRING("STR_TBL_CHAT_GHOST_TEXT");/*(��Ʈ) */
			}
			i3::generic_string_cat( wstrTemp, wstrImeBuf);
		}
		else if( m_bOnceChatType)
		{
			if( IsGhostMessage())
			{
				wstrTemp = GAME_RCSTRING("STR_TBL_CHAT_GHOST_TEXT");/*(��Ʈ) */
			}
			i3::generic_string_cat( wstrTemp, wstrImeBuf);
			info._Type = CHATTING_TYPE_TEAM;
		}
		else if(UserInfoContext::i()->IsGM_Observer() == true)
		{
			info._Type = CHATTING_TYPE_RADIO;
			i3::generic_string_cat( wstrTemp, wstrImeBuf);
		}
		else
		{

			switch( m_nChatType)
			{
			case CHATTING_TYPE_LOBBY :
			case CHATTING_TYPE_ALL:	// ��ü
				if( IsGhostMessage())
				{
					wstrTemp = GAME_RCSTRING("STR_TBL_CHAT_GHOST_TEXT");/*(��Ʈ) */
				}
				i3::generic_string_cat( wstrTemp, wstrImeBuf);
				break;
			case CHATTING_TYPE_TEAM:// ��
				if( IsGhostMessage())
				{
					wstrTemp = GAME_RCSTRING("STR_TBL_CHAT_GHOST_TEXT");/*(��Ʈ) */
				}
				i3::generic_string_cat( wstrTemp, wstrImeBuf);
				break;
			case CHATTING_TYPE_MATCH :
			case CHATTING_TYPE_CLAN_MEMBER_PAGE :
			case CHATTING_TYPE_CLAN:// Ŭ��
				wstrTemp = wstrImeBuf;
				break;
			case CHATTING_TYPE_WHISPER: // �Ӹ�
				{
					wstrTemp2 = L"{col:237, 86, 250, 255}";
					i3::generic_string_cat( wstrTemp2, wstrImeBuf);
					i3::generic_string_cat( wstrTemp2, L"{/col}");

					_GetOnlyChatMessage(wstrTemp, wstrImeBuf );
					
					if ( (INT32)wstrImeBuf.size() > g_WhisperLength )
					{
						nWhsiperTextCount = wstrImeBuf.size() - g_WhisperLength;
					}

					bWhisper = true;
					m_nChatType = m_nOldChatType;
				}
				break;
			case CHATTING_TYPE_REPLY:// �亯
				{
					wstrTemp2 = L"{col:237, 86, 250, 255}";
					i3::generic_string_cat( wstrTemp2, wstrImeBuf);
					i3::generic_string_cat( wstrTemp2, L"{/col}");

					_GetOnlyChatMessage(wstrTemp, wstrImeBuf);
				
					if ( (INT32) wstrImeBuf.size() > g_WhisperLength)
					{
						nReplyTextCount = wstrImeBuf.size() - g_WhisperLength;
					}

					bReply = true;
					m_nChatType = m_nOldChatType;
				}
				break;
			default: break;
			}
		}

		// �Ӹ��� ��� �ٸ� ������� ó���ؾ� �մϴ�.
		if( bWhisper)
		{
			if( nWhsiperTextCount > 0)
			{
				m_chatbox_mgr->PutSystemChatting(wstrTemp2);

				// ģ�� ��Ͽ� �ִ� �г����� ��� �ش� UID�� ã�Ƽ� UID�� ������.
				INT64 ReceiverUID = GameUI::FindUserUidByNick(m_chatbox_mgr->GetChatTarget());
				if (0 < ReceiverUID)
				{
					// ä�� �Է�
					// Send Event
					
					const i3::stack_wstring& strMsg = wstrTemp;

					GameEventSender::i()->SetEvent( EVENT_SEND_WHISPER_FIND_UID, &m_chatbox_mgr->GetChatTarget(), strMsg.c_str(), &ReceiverUID);
					
					i3::stack_wstring wstrHistory;

					wstrHistory = L"/w ";
					wstrHistory += m_chatbox_mgr->GetChatTarget();
					wstrHistory += L' '; 
					
					i3::const_wchar_range only_chat_rng = wstrImeBuf;
					only_chat_rng.advance_begin(g_WhisperLength);
					wstrHistory.append( only_chat_rng.begin(), only_chat_rng.end() );
					m_chatbox_mgr->PutHistory(wstrHistory, m_nChatType);
				}
				else
				{
					// ä�� �Է�
					// Send Event
					if( enable_whisper_chat( m_chatbox_mgr->GetChatTarget() )== true )
					{

						const i3::stack_wstring& strMsg = wstrTemp;
						GameEventSender::i()->SetEvent( EVENT_SEND_WHISPER, &m_chatbox_mgr->GetChatTarget(), strMsg.c_str());

						i3::stack_wstring wstrHistory;

						wstrHistory = L"/w ";
						wstrHistory += m_chatbox_mgr->GetChatTarget();
						wstrHistory += L' '; 
						i3::const_wchar_range only_chat_rng = wstrImeBuf;
						only_chat_rng.advance_begin(g_WhisperLength);
						wstrHistory.append( only_chat_rng.begin(), only_chat_rng.end() );

						m_chatbox_mgr->PutHistory(wstrHistory, m_nChatType);
					}
					else
					{
						g_pFramework->PutSystemChatting(GAME_RCSTRING("STR_TBL_CHAT_NOTICE_WARNING_WHISPER"));/*�������� ���� �ʴ� ����� ��� ���� �濡�� play ���� ����ڿ��� �ӼӸ��� ���� �� �����ϴ�.*/
					}
				}
			}
		}
		else if( bReply)
		{
			if( nReplyTextCount > 0)
			{
				// ä�� �Է�
				// Send Event
				if( enable_whisper_chat( m_chatbox_mgr->GetChatTarget() )== true )
				{
					m_chatbox_mgr->PutSystemChatting(wstrTemp2);

					// GM�� 1:1 ä���� ��� GM ���� �˾����� �޽����� ������ 
					if (i3::generic_is_equal(m_chatbox_mgr->GetChatReplyTarget(), GMAuthorityContext::i()->GetUserNickName()))
					{
						i3::wstring wstrMessage = i3::format_string(L"[%s] %s", UserInfoContext::i()->GetMyNickName(), wstrTemp);
						
						UINT16 uiLen = (UINT16)wstrMessage.size();

						T_UID _GMUID = 0;
						GameEventSender::i()->SetEvent( EVENT_GM_AUTHORITY_CHAT_SEND, &_GMUID, &uiLen, wstrMessage.c_str());
					}
					else
					{
						const i3::stack_wstring& strMsg = wstrTemp;
						GameEventSender::i()->SetEvent( EVENT_SEND_WHISPER, &m_chatbox_mgr->GetChatReplyTarget(), strMsg.c_str());
					}

					i3::stack_wstring wstrHistory = L"/r ";
					wstrHistory += L' '; 

					i3::const_wchar_range only_chat_rng = wstrImeBuf;
					only_chat_rng.advance_begin(g_WhisperLength);
					wstrHistory.append(only_chat_rng.begin(), only_chat_rng.end());
				
					m_chatbox_mgr->PutHistory(wstrHistory, m_nChatType);
				}
				else
				{
					g_pFramework->PutSystemChatting(GAME_RCSTRING("STR_TBL_CHAT_NOTICE_WARNING_WHISPER"));/*�������� ���� �ʴ� ����� ��� ���� �濡�� play ���� ����ڿ��� �ӼӸ��� ���� �� �����ϴ�.*/
				}
			}
		}
		else
		{
			info.setString( wstrTemp);

			if( info._Type == CHATTING_TYPE_CLAN)	// Ŭ�� ä���̶��..
			{
				// ä�� �Է�

				// Ŭ���̰��Ի����Դϴ�.
				USER_INFO_BASIC		MyUserInfoBasic;
				UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

				if ((MyUserInfoBasic.m_ui32Clanidx == 0 || MyUserInfoBasic.m_ui32ClanState == 0))
				{
					g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_CHAT_INGAME_NOTICE_NO_JOIN_CLAN"));/*<Ŭ�����̰����ѻ����Դϴ�.>*/
				}
				else
				{
					GameEventSender::i()->SetEvent(EVENT_CHATTING_N, &info);
					m_chatbox_mgr->PutHistory(wstrImeBuf, m_nChatType);
				}

			}
			else
			{
				if( info._Type == CHATTING_TYPE_TEAM)
				{
					// ä�� �Է�
					GameEventSender::i()->SetEvent(EVENT_CHATTING_N , &info );
					m_chatbox_mgr->PutHistory(wstrImeBuf, m_nChatType);
				}
				else
				{
					// Ŀ�´�Ƽ �ɼǿ����� ä�� ������ �ɷ��ִٸ�, �װͿ� ���� ó���� �մϴ�.
					if( g_pEnvSet->m_nChatType != 0 && (m_nChatType != CHATTING_TYPE_TEAM && m_nChatType != CHATTING_TYPE_CLAN) )
					{
						wstrTemp2 = L"[";
						wstrTemp2 += UserInfoContext::i()->GetMyNickName();
						wstrTemp2 += L"] ";
						wstrTemp2 += wstrTemp;

						m_chatbox_mgr->PutSystemChatting(wstrTemp2);
						m_chatbox_mgr->PutHistory(wstrImeBuf, m_nChatType);
					}
					else
					{
						// ä�� �Է�
						const i3::rc_wstring& info_get_string = info.getString();

						if( !i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_HELP")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_HELP2")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE1")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE2")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE3")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE4")) && 
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY1")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY2")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY3")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY4")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM1")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM2")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM3")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM4")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER1")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER2")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER3")) &&
							!i3::generic_is_iequal( info_get_string, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER4")) 	)
						{// "/?" "/? "�� ������ ������ �ʽ��ϴ�.
							GameEventSender::i()->SetEvent(EVENT_CHATTING_N , &info );
						}
						m_chatbox_mgr->PutHistory(wstrImeBuf, m_nChatType);
					}
				}
			}
		}

		g_WhisperLength = 0;
	}

	m_chatbox_mgr->ResetHistoryBufferIndex();

	m_pChatBox->SetText( L"");
	if ( state == EDIT_STATE_RETURN)
		i3UI::getGlobalIme()->SetInputText( L"", 0);

	m_bOnceChatType = false;
	m_bOnceChatTypeAll = false;

	if( m_bUseHistory)
	{
		m_nChatType = m_nOldChatType;
		m_bUseHistory = false;
	}

	if( m_bUseOneTimeTeamChat == true )
	{
		m_nChatType = m_nOldChatType;
		m_bUseOneTimeTeamChat = false;
	}
}

bool UIHUDIngameChat::enable_whisper_chat(const i3::rc_wstring & wstr_nick)
{
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION))
	{
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			// ��Ʋ ���� ����
			const SLOT_INFO* slot = BattleSlotContext::i()->getSlotInfo(i);

			if (SLOT_STATE_LOAD <= slot->_State)
			{
				// �ӼӸ� �������� ����
				if(wstr_nick == BattleSlotContext::i()->getNickForSlot(i))
				{
					return false;
				}
				else if (!BattleSlotContext::i()->IsMyTeam(i))
				{
					return false;
				}
			}
		}
	}
	return true;
}

void UIHUDIngameChat::UpdateHistoryChatBuffer(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if(m_bChatScrollEnable) return;

	if( m_bNothingText_inChatBox == false)
		m_pTextBg->EnableCtrl(false);
	else
		m_pTextBg->EnableCtrl(true);

	m_chatbox_mgr = g_pFramework->getChatBox();
	I3ASSERT( m_chatbox_mgr != nullptr);

	// ���� �ð��� ������ ä�� ���۸� ���� ��Ŵ���� �ڵ� ��ũ�� ��ŵ�ϴ�.
	if( m_chatbox_mgr->GetBufferCount() > 0)
	{
		g_LocalTime += rDeltaSeconds;
		
		if( g_LocalTime > 10.0f)
		{
			g_LocalTime = 0.0f;
			g_Last_Buffer_Idx = m_chatbox_mgr->GetCurrentBufferIndex();
		}
	}
}

// �Ʒ� ��ƾ ���ϰ� �� �����δ�.....( UILibrary�ʿ����� �μ�Ʈ ����� �����Ѱ� ó�� ���δ�..) 13.02.19.����.
// �Դٰ�, �μ�Ʈ�� �ڿ� ���� �ʰ� �ڲ� ������ ���� ���� �� ���� �ʾ� ���δ�... 13.02.19.����.

void UIHUDIngameChat::UpdateChatBuffer(void)
{
	i3_prof_func();

	i3::wstring wstrTotalText;		wstrTotalText.reserve(1024);

	// Buffer�� ���鼭 ����� ���ڸ� ����ϴ�.

	if( m_chatbox_mgr != nullptr)
	{
		INT32 nLoopCount = 0;
		INT32 BufferIdx= 0;
		i3::rc_wstring wstrChatBoxText;			// ���۹��ڿ�
		INT32 CurrentLine = 0;
	
		if( !m_bChatScrollEnable )
		{
			if( m_chatbox_mgr->GetCurrentBufferIndex() == 0)
				BufferIdx = MAX_CHAT_BUFFER-1;
			else
				BufferIdx = m_chatbox_mgr->GetCurrentBufferIndex() - 1;

			CurrentLine = GAME_CHATLINE_MAX;
		}
		else
		{
			if( m_chatbox_mgr->GetBackUpBufferIndex() == 0)
				BufferIdx = MAX_CHAT_BUFFER-1;
			else
				BufferIdx = m_chatbox_mgr->GetBackUpBufferIndex() - 1;

			CurrentLine = BufferIdx;
		}

		for( INT32 idx = BufferIdx ; nLoopCount <= CurrentLine ; --idx)
		{
			if( !m_bChatScrollEnable )
			{
				if( nLoopCount > m_chatbox_mgr->GetCurrentBufferIndex() )
					break;

				// ������ ä�� ���� �ε����� ���� ä�� ���� �ε����� ������ ó�� ����.
				if( g_Last_Buffer_Idx >= m_chatbox_mgr->GetCurrentBufferIndex() )
					break;

				// ���� ���� ä���� �ִ� ��� LocalTime�� ���� ���� �ε����� ����
				if( g_Old_Buffer_Idx < m_chatbox_mgr->GetCurrentBufferIndex() )
				{
					g_LocalTime = 0.0f;
					g_Old_Buffer_Idx = m_chatbox_mgr->GetCurrentBufferIndex();
				}
			}

			if( idx < 0)
				idx = MAX_CHAT_BUFFER-1;

			nLoopCount++;

			if( m_bChatScrollEnable)
				wstrChatBoxText = m_chatbox_mgr->GetBackUpBufferText(idx);
			else
				wstrChatBoxText = m_chatbox_mgr->GetBufferText(idx);

			if( !wstrChatBoxText.empty() )
			{
				i3::stack_wstring wstr_StackChatBoxText = wstrChatBoxText;
				wstr_StackChatBoxText += L'\n';
				i3::insert_string(wstrTotalText, 0, wstr_StackChatBoxText);
			}
		}

		m_bNothingText_inChatBox =	!wstrTotalText.empty();		//   length ? true : false;			//���ڵ带 �ּ��� ���ܵд�...

		// ���� ������ '\n'�� �����ϴ� ��� �������ش�.( �׻� ä��â �ϴܿ� ���� ������ ���� �� ó����)
		if (!wstrTotalText.empty() )
		{
			const size_t size_m1 = wstrTotalText.size() - 1;
			if (wstrTotalText[size_m1] == L'\n')
				wstrTotalText.erase(size_m1);
		}

		if(m_pTextBox)
			m_pTextBox->SetText(wstrTotalText);

		if( m_chatbox_mgr->GetBufferCount() != 0 )
		{
			re_position();
		}
	}
}

void UIHUDIngameChat::_UpdateWhisperBackupBuffer(void)
{

	i3_prof_func();

	m_chatbox_mgr = g_pFramework->getChatBox();
	I3ASSERT( m_chatbox_mgr != nullptr);


	for( INT32  i = 0 ; i < m_chatbox_mgr->GetWhisperBufferCount() ; i++)
	{
		INT32 index = (m_chatbox_mgr->GetCurrentWhisperBufferIndex() + i) % 5;
		m_chatbox_mgr->PutSystemChatting(m_chatbox_mgr->GetWhisperBufferText(index), CHATTING_TYPE_WHISPER);
	}

	m_chatbox_mgr->ResetWhisperBuffer();
}

void UIHUDIngameChat::_UpdateChatType( void)
{
	i3_prof_func();

	// ä�� ���¼���.
	if( m_keyboard_mgr->GetPressState( I3I_KEY_LCTRL ) )
	{
		if( m_keyboard_mgr->GetStrokeState( I3I_KEY_ENTER ))
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetText(L"");
				i3UI::getGlobalIme()->SetInputText( L"", 0);
			}

			m_bOnceChatType = true;
			g_pFramework->SetFocus( m_pChatBox);
		}
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F2 )
	{
		if( m_nChatType != CHATTING_TYPE_ALL || m_bOnceChatType == true || m_bOnceChatTypeAll == true)
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetText(L"");
				i3UI::getGlobalIme()->SetInputText( L"", 0);
			}

			m_bOnceChatType = m_bOnceChatTypeAll = false;
			m_nChatType = CHATTING_TYPE_ALL;
			g_pFramework->getChatBox()->PutSystemChatting( GAME_RCSTRING("STBL_IDX_CHATMESSAGE_MODE_ALL"));

			if (isVisible() == false)
				SetVisible(true);
		}
		//g_pFramework->SetFocus( m_pChatBox);
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F3 )
	{
		if( m_nChatType != CHATTING_TYPE_TEAM || m_bOnceChatType == true || m_bOnceChatTypeAll == true)
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetText(L"");
				i3UI::getGlobalIme()->SetInputText( L"", 0);
			}

			m_bOnceChatType = m_bOnceChatTypeAll = false;
			m_nChatType = CHATTING_TYPE_TEAM;
			g_pFramework->getChatBox()->PutSystemChatting( GAME_RCSTRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"));

			if (isVisible() == false)
				SetVisible(true);
		}
		//g_pFramework->SetFocus( m_pChatBox);
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F4 )
	{
		if( m_nChatType != CHATTING_TYPE_CLAN || m_bOnceChatType == true || m_bOnceChatTypeAll == true)
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetText(L"");
				i3UI::getGlobalIme()->SetInputText( L"", 0);
			}

			m_bOnceChatType = m_bOnceChatTypeAll = false;
			m_nChatType = CHATTING_TYPE_CLAN;
			g_pFramework->getChatBox()->PutSystemChatting( GAME_RCSTRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"));

			if (isVisible() == false)
				SetVisible(true);
		}
		//g_pFramework->SetFocus( m_pChatBox);
	}
}
// �Ʒ��ڵ�� pszOut�� ũ�Ⱚ�� �����ϱ� ����ϹǷ�, �� ���ľ��Ѵ�.. (�ϴ��� i3::string�� ����μ��� ���°� ���ڴ�..)

void UIHUDIngameChat::_CopyChatBufferToBox( i3::wstring& inout_wstr, INT32 bufferIdx)
{

	i3_prof_func();

	m_chatbox_mgr = g_pFramework->getChatBox();
	I3ASSERT( m_chatbox_mgr != nullptr);


	CGameChatBox * pChatBox = m_chatbox_mgr;

	const i3::rc_wstring&  wstrChatBoxText = pChatBox->GetBackUpBufferText( bufferIdx);

	if( !wstrChatBoxText.empty() )
	{
		i3::stack_wstring wstr_StackChatBoxText = wstrChatBoxText;
		wstr_StackChatBoxText += L'\n';
		i3::insert_string(inout_wstr, 0, wstr_StackChatBoxText);		// ���ƺ����� �ʴ´�...
	}
}

void UIHUDIngameChat::OnWhisper( INT32 slotIdx)
{
	i3_prof_func();


	i3::rc_wstring wstrNick = BattleSlotContext::i()->getNickForSlot(slotIdx);

	OnWhisper(wstrNick);
}

void UIHUDIngameChat::OnWhisper( const i3::rc_wstring& wstrNick)
{
	i3_prof_func();


	g_pFramework->SetFocus( nullptr);

	// ä��â�� �ӼӸ� Ŀ��� �Է�

	i3::wstring wstrWhisper;
	_ToWhisperChatMessage(wstrWhisper, wstrNick );

	m_pChatBox->SetText(wstrWhisper );
	g_pFramework->SetFocus( m_pChatBox);

	m_nChatType = CHATTING_TYPE_WHISPER;
	g_WhisperLength =	wstrWhisper.length(); //	i3::generic_string_size( szWhisper );

	m_chatbox_mgr = g_pFramework->getChatBox();
	I3ASSERT( m_chatbox_mgr != nullptr);
	m_chatbox_mgr->SetChatTarget(wstrNick);

	m_pChatBox->SetTextColor( GameGUI::GetColor(GCT_WISPER_CHAT) );
	m_pChatBox->SetTextCaretPos( g_WhisperLength);
}

void UIHUDIngameChat::_GetOnlyChatMessage(i3::stack_wstring& out,	const i3::wliteral_range& wfullMsgRng)
{
	i3_prof_func();

	int idx = i3::contain_string(wfullMsgRng, L"] ");

	if( idx != -1 )
	{
		out.assign( wfullMsgRng.begin() + idx + 2, wfullMsgRng.end());
	}
	else
	{
		out.assign( wfullMsgRng.begin(), wfullMsgRng.end() );
	}
}

void UIHUDIngameChat::SetChatType( INT32 type)
{
	i3_prof_func();


	if( m_nChatType != type)
	{
		m_nChatType = type;

		m_bOnceChatType = false;
		m_bOnceChatTypeAll = false;

		switch( m_nChatType)
		{
		case CHATTING_TYPE_ALL: case CHATTING_TYPE_LOBBY: case CHATTING_TYPE_MATCH:
		case CHATTING_TYPE_CLAN_MEMBER_PAGE:
			if( m_CurrentTab != CHAT_TYPE_ALL)	m_CurrentTab = CHAT_TYPE_ALL;
			break;

		case CHATTING_TYPE_TEAM :
			if( m_CurrentTab != CHAT_TYPE_ALL) m_CurrentTab = CHAT_TYPE_TEAM;
			break;

		case CHATTING_TYPE_CLAN :
			if( m_CurrentTab != CHAT_TYPE_ALL) m_CurrentTab = CHAT_TYPE_CLAN;
			break;

		case CHATTING_TYPE_WHISPER :
			if( m_CurrentTab != CHAT_TYPE_ALL) m_CurrentTab = CHAT_TYPE_WHISPER;
			break;
		}
	}
}

void UIHUDIngameChat::SetEnableChatScroll( bool bFlag)
{
	i3_prof_func();

	m_chatbox_mgr = g_pFramework->getChatBox();
	I3ASSERT( m_chatbox_mgr != nullptr);

	if( bFlag)
	{	
		i3::rc_wstring wstrTemp;

		// "������ȭ���� �������̽��� Ȱ��ȭ �Ǿ����ϴ�."
		i3::sprintf( wstrTemp, L"{col:255,120,0,255}%s{/col}", GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HISTORY_ENABLE1"));	
		m_chatbox_mgr->PutSystemChatting( wstrTemp);

		// "��ư�� ����Ͽ� ��Ȱ��ȭ ��ų �� �ֽ��ϴ�."
		i3::sprintf( wstrTemp, L"{col:255,120,0,255}'%s'%s{/col}", 
			GameUI::GetKeyboardString(&g_pEnvSet->m_KeyMapArray[GAME_KEY_MAP_CHAT_HISTORY]), 
			GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HISTORY_ENABLE2"));
		m_chatbox_mgr->PutSystemChatting( wstrTemp);

		m_chatbox_mgr->ResetBackupBuffer();
		g_pFramework->SetGameKeyMode(GAMEKEY_MODE_MENU);


		m_pTextBox->enableScrollBar(true);
		m_pTextBg->EnableCtrl(true);

		g_pFramework->SetFocus( m_pTextBox->getScrollBar() );
	}
	else
	{
		if( g_pGUIRoot && false == i3GuiRoot::getGuiDisableInput() /*&& false == ((CHud*)m_pParent)->IsEnableGui()*/)
		{
			g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);
		}

		m_pTextBox->enableScrollBar(false);
		m_pTextBg->EnableCtrl(true);
	}

	m_bChatScrollEnable = bFlag;	
	UpdateChatBuffer();
}

bool UIHUDIngameChat::TestCommand(const i3::wliteral_range& wCmdRng)
{
	i3_prof_func();

	if( CommandSet::parsing( wCmdRng))
	{
		// Command�Դϴ�.
#if defined( ENABLE_TEST_COMMAND)
		g_pFramework->getCommandSet()->ProcessCommand();
#endif
	}
	else if ( CGameMaster::i()->SetGMCommand(wCmdRng) )
	{
		return true;
	}
	else
	{
		return false;
	}

	return true;
}

bool UIHUDIngameChat::IsGhostMessage( void)
{
	i3_prof_func();


#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
		return true;
#endif

	CHud* pHud = CHud::i();
	if(pHud != nullptr)
	{
		CGameCharaBase* player = pHud->GetCurrentChara();

		if( ((MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION)) &&
			(player != nullptr && player->isCharaStateMask( CHARA_STATE_DEATH))) ||
			pHud->getHudMode() == HUD_MODE_MISSION_OBSERVER)
			return true;
	}

	return false;
}

void UIHUDIngameChat::re_position()
{
	i3_prof_func();

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		return;

	CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx() );
	if( pChara == nullptr ) return;

	// ������ ���� ���� ��, Team Change�� �ϰ� �� ���, MySlotIdx���� ����Ǿ�
	// pChara ���� nullptr�� �Ǵ� ��찡 �־� ����ó���մϴ�.
	enum { offset = 10 };

	REAL32 y = (REAL32)GameUI::AutoPosition::get_max_bottom(); //�� �Ʒ���ġ

	iHudBase* phudbase = 0;

	if( g_pEnvSet->m_nGameInterface == HUD::HS_1_5 )
	{
		if( !pChara->IsDino() ) phudbase = HUD::instance()->GetHud(HUD::PLAYER_STATE);
		else phudbase = HUD::instance()->GetHud(HUD::PLAYER_DINO_STATE);

		if( phudbase->IsVisible() )
		{
			y = phudbase->GetFrameWnd()->getPos()->y - offset;
		}
	}

	phudbase = HUD::instance()->GetHud(HUD::NOTICE);
	if( phudbase->IsVisible() )
	{
		y = (phudbase->GetFrameWnd()->getPos()->y - offset);
	}

	// (Hansoft 14033) ���� ���â���� ������ �Կ� �� ����� ä��â�� ���µ� ������ �Կ� ���� �ȳ� ��Ʈ ��� �õ��ϴ� ũ���� �߻� ����
	if (m_pFrameWnd == nullptr)
		return;

	y -= m_pFrameWnd->getSize()->y;

	VEC3D* pMainPos = m_pFrameWnd->getPos();
	m_pFrameWnd->setPos( pMainPos->x, y ) ;
}