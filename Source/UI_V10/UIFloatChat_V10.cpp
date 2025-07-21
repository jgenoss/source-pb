#include "pch.h"
#include "UIFloatChat_V10.h"

#include "../UI/UIMainFrame_V11.h"
#include "../UI/UITopMenu.h"
#include "../UI/UIUtil.h"
#include "../UI/UIResizer.h"

#include "../UI/UIPhaseSubBase.h"

#include "../GameGUI.h"
#include "../GameMaster.h"
#include "../ClanContext.h"

#include "../UserInfoContext.h"
#include "../BattleSlotContext.h"

//#include "PaperingFilter.h"

#define USERLIST_UPDATE_TIME 0.5f

#define MEGAPHONE_FRAME_CLOSE 0
#define MEGAPHONE_FRAME_OPEN 1

static INT32 g_WhisperLength = 0;

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
static UIFloatChat_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( pFrame != nullptr)
		return (UIFloatChat_V10*) pFrame->GetFloating( UFW_CHAT);

	return nullptr;
}

extern "C" {

	int UIFloatChat_V10_SetChatType( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		GetThis()->SetChatTypeFromCombo( tab);
		return 0;
	}

	int UIFloatChat_V10_ChangeChatChannel( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		GetThis()->SelectChatChannel( tab);
		return 0;
	}

	int UIFloatChat_V10_ChangeLanguage( LuaState * L)
	{
		GetThis()->ToggleLang();
		return 0;
	}

	//�ϴ� Ȯ���⿡�� ��� �մϴ�.
	int UIFloatChat_V10_ClickMegaPhoneOpen(LuaState * L)
	{
		UIFloatChat_V10 * pThis = static_cast<UIFloatChat_V10 *>(GameUI::GetFloating(UFW_CHAT));
		if (pThis != nullptr)
			pThis->ClickMegaPhoneOpenButton();
		return 0;
	}

}

LuaFuncReg FloatChat_Glue_V10[] = {
	{"SetChatType",				UIFloatChat_V10_SetChatType},	// Chatting type�� ����
	{"ChangeChatChannel",		UIFloatChat_V10_ChangeChatChannel		},
	{"ChangeLanguage",			UIFloatChat_V10_ChangeLanguage	},
	{ "ClickMegaPhoneOpen",		UIFloatChat_V10_ClickMegaPhoneOpen },
	{ nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIFloatChat_V10);

bool UIFloatChat_V10::m_bUseHistory = false;

UIFloatChat_V10::UIFloatChat_V10()
{
//	m_pFrameResizer = nullptr;

	m_pChatMainBox = nullptr;
	m_pSubChatMain = nullptr;

	m_nChatType = CHATTING_TYPE_ALL;
	m_nOldChatType = CHATTING_TYPE_ALL;
	m_nMaxChatLineCount = 0;
	m_ChatArea = CHAT_AREA_LOBBY;
	m_bOnceChatType = false;
	m_bOnceChatTypeAll = false;
	m_nCurrentSelectedChatType = 0;

	m_AlignedPos = UIFLOATING_POS_RIGHTBOTTOM;
	m_bFixedWindow = true;
	
	m_lastWhisperUserNick[ 0] = 0;

#if defined( I3_DEBUG)
	SetName( "ChatBox");
#endif

	m_CurrentLanguage = 0;

	m_pTextBox = nullptr;
	m_pChatList = nullptr;
	m_pChatBox = nullptr;

	m_pParentMegPhoneMain = nullptr;
	m_pMegaPhone_Button = nullptr;
	m_pMegaPhoneMain_EditBox = nullptr;
	m_pMegaPhone_EditBox = nullptr;

	m_bMegaPhoneOpen = false;
}

UIFloatChat_V10::~UIFloatChat_V10()
{
	//if( m_pFrameResizer)
	//{
	//	RemoveChild( m_pFrameResizer);
	//	m_pFrameResizer->Release();
	//}

}

//static void sMaximizeDone( void)
//{
//	GetThis()->OnMaximizeEnd();
//}

void UIFloatChat_V10::OnLoadAllScenes()
{
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::R_TO_L);	
	if(m_pChatMainBox != nullptr)
	{
		m_slide.AddFrameWnd(m_pChatMainBox, UISlide::R_TO_L);	
	}
	if (m_pParentMegPhoneMain != nullptr)
	{
		m_slide.AddFrameWnd(m_pParentMegPhoneMain, UISlide::R_TO_L);
	}
}

/*virtual*/ void UIFloatChat_V10::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);

	m_pFrameWnd = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd");
	I3ASSERT( m_pFrameWnd != nullptr);

	i3UITab * pTab = (i3UITab*) pScene->FindChildByName( "i3UITab");
	I3ASSERT( pTab);

	//m_pFrameResizer->BindFrameWnd( m_pFrameWnd, getY( m_pFrameWnd->getSize()) - (pTab->getHeight()+getY(pTab->getPos())), nullptr, sMaximizeDone, UI_SIZESTYLE_MOVEBOTTOM);

	m_pChatMainBox = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd3");

	//m_pSubChatMain = (i3UIFrameWnd*) m_pChatMainBox->FindChildByName( "FrameWnd_ChatWnd");
	//I3ASSERT( m_pSubChatMain);

	m_pTextBox = (i3UIEditBox*) pScene->FindChildByName( "Edit_ChatLog");

#ifdef LOCALE_MIDDLE_EAST
	m_pTextBox->OnChatBox();
#endif

	I3ASSERT( m_pTextBox);

	if( m_pTextBox->getScrollBar() != nullptr)
		m_pTextBox->getScrollBar()->setTickSize( 10);

	m_pChatBox = (i3UIEditBox*) pScene->FindChildByName( "Edit_Chat");
	I3ASSERT( m_pChatBox);

	m_pChatCombo = (i3UIComboBox*) pScene->FindChildByName( "i3UiComboBox_NoEditBox");
	I3ASSERT( m_pChatCombo);

	m_pMegaPhoneMain_EditBox = (i3UIEditBox *)pScene->FindChildByName("MegaPhone_Main_EditBox");
	I3ASSERT(m_pMegaPhoneMain_EditBox);

	m_pMegaPhone_EditBox = (i3UIEditBox *)pScene->FindChildByName("MegaPhone_EditBox");
	I3ASSERT(m_pMegaPhone_EditBox);

	m_pMegaPhone_Button = (i3UIButton *)pScene->FindChildByName("MegaPhone_OpenButton");
	I3ASSERT(m_pMegaPhone_Button);

	m_pParentMegPhoneMain = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_MegaPhone_Main");
	I3ASSERT(m_pParentMegPhoneMain);

	InitChat( CHAT_AREA_LOBBY, 6);

	//m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::R_TO_L);
}

/*virtual*/ void UIFloatChat_V10::_ClearAtUnload( i3UIScene * pScene)
{
	UIFloatingBase::_ClearAtUnload( pScene);

	m_slide.ClearFrameWndList();

	m_pFrameWnd = nullptr;
	m_pChatMainBox = nullptr;
	m_pSubChatMain = nullptr;
	m_pTextBox = nullptr;
	m_pChatBox = nullptr;
	m_pChatCombo = nullptr;
	m_pMegaPhoneMain_EditBox = nullptr;
	m_pParentMegPhoneMain = nullptr;

}

void UIFloatChat_V10::_FowardToTopMenuScreen( void)
{
	// �ֻ����� �׸����� �Ѵ�.
	INT32 i;
	i3GameNode * pParent;

	// ������ ���� �ڿ� �ٵ��� ����
	for( i = 0; i < GetSceneCount(); i++)
	{
		i3UIScene * pScene = GetScene( i);

		pParent = pScene->getParent();

		if( pParent != nullptr)
		{
			pScene->AddRef();
			pParent->RemoveChild( pScene);
			pParent->AddChild( pScene);
			pScene->Release();
		}
	}
}

void UIFloatChat_V10::_SearchLanguage( void)
{
	INT32 i, j, k;

	m_LanguageCount = ::GetKeyboardLayoutList( 6, m_LanguageList);

	// ���� ���� ����
	
	for( i = 0; i < m_LanguageCount; i ++)
	{
		LANGID src = LOWORD( m_LanguageList[i]);
		for( j = i+1; j < m_LanguageCount; j++)
		{
			LANGID dest = LOWORD( m_LanguageList[j]);

			if( src == dest)
			{
				k = j;
				while( k-1 < m_LanguageCount-1)
				{
					m_LanguageList[k] = m_LanguageList[k+1];
					k++;
				}

				m_LanguageCount--;
				j--;
			}
		}
	}
}

void UIFloatChat_V10::_ResetLanguage( void)
{
	i3Ime * pIME = i3UI::getGlobalIme();

	if( pIME == nullptr)
		return;

	m_CurrentLanguage = 0;
	WPARAM param = INPUTLANGCHANGE_FORWARD;
	SendMessage( pIME->getHwnd(), WM_INPUTLANGCHANGEREQUEST, param, (LPARAM) m_LanguageList[m_CurrentLanguage]);
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIFloatChat_V10::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_ChattingWindow.i3UIs", FloatChat_Glue_V10);


	//m_pFrameResizer = UIResizer::new_object();
	//AddChild( m_pFrameResizer);

	_SearchLanguage();
	_ResetLanguage();
}

/*virtual*/ bool UIFloatChat_V10::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	bool bRv = UIFloatingBase::OnEvent( event, pObj, param2, code);

	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_CLEAR :
		case EDIT_STATE_RETURN:
		case EDIT_STATE_CHANGE_COMPOSITION_CHAR :
		case EDIT_STATE_CHANGE_INPUT_CHAR :
			{
				_UpdateParsingType( (EDIT_STATE) param2);
				if( param2 == EDIT_STATE_RETURN)
					m_pChatBox->GetIME()->SetInputText( L"", 0);
			}
			break;

		case EDIT_STATE_ONKEYDOWN_UP:
		case EDIT_STATE_ONKEYDOWN_DOWN:
			_ChatHistory((EDIT_STATE) param2);
			break;

		case EDIT_STATE_CHANGE_LANGUAGE :
		case EDIT_STATE_CHANGE_IME_MODE :
			// �Է� ��� ����
			_UpdateLang();
			break;
		}
	}

	return bRv;
}

/*virtual*/ bool UIFloatChat_V10::OnKeyInput( i3InputDeviceManager * pInputMng)
{
// 	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ) || g_pFramework->getKeyboard()->GetPressState( I3I_KEY_RALT ))
// 	{
// 		return false;
// 	} // ä�� ��ũ�� �����մϴ�. ���� �߻� �� �ּ� �������ּ���

	bool res = UIFloatingBase::OnKeyInput( pInputMng);

	// ��ũ��
	_UpdateMacroChat();

	_UpdateChatType();

	return res;
}

/*virtual*/ bool UIFloatChat_V10::OnKey_Enter( void)
{
	// �˾��� ���� ��� ��Ŀ���� �Űܾ� �մϴ�.
	if ( g_pFramework->IsMessageBoxEnabled() == false ) 
	{
		if( !m_pChatBox->isFocused())
		{
			g_pFramework->SetFocus( m_pChatBox);
			return true;
		}
	}

	return false;
}

/*virtual*/ void UIFloatChat_V10::OnCompleteLoad( void)
{
	/*if( m_pLastCtrl)
	{
		m_LastSpriteHandle = m_pLastCtrl->getEndSpriteHandle();
		
	}*/
//	_FowardToTopMenuScreen();

}

/*virtual*/ void UIFloatChat_V10::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{

}

/*virtual*/ bool UIFloatChat_V10::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;


 	I3TRACE("Chat Entrance Start\n");

	//OnSlideIn();

	// ��ġ ����
	_SetPhaseAtPos();

	m_pTextBox->SetText( L"");
	m_pChatBox->SetText( L"");

	// ä�� ���� �ʱ�ȭ
	g_pFramework->getChatBox()->ResetChatBuffer();

	// Update Whisper Backup Buffer
	_UpdateWhisperBackupBuffer();

	// ä��ä�� �ʱ�ȭ
	SelectChatChannel( 0);

	// �Է� ��� ��ư
	_UpdateLang();

	// �ʹ� ����� �޼���	
	g_pFramework->getChatBox()->PutSystemChatting(g_pFramework->getChatNotice());
#if defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_RUSSIA)
	m_pChatBox->setUseCopyNPaste(false);
#endif

	//_FowardToTopMenuScreen();
	_AttachToTopScreen();

#if defined( LOCALE_NORTHAMERICA)
	// �Ϲ̸� ����ư ��Ȱ��ȭ
	LuaState* L = _CallLuaFunction( "EnableLanguageButton");
	if( L != nullptr)
	{
		i3Lua::PushBoolean( L, false);
		_EndLuaFunction(L, 1);
	}
#endif

	m_pMegaPhoneMain_EditBox->SetText(L"");
	m_pMegaPhone_EditBox->SetText(L"");
	_SetMegaPhoneMainFrame(false);
	UpdateMegaPhoneBuffer();

	return true;
}

void	UIFloatChat_V10::OnEntranceEnd()
{
	UIFloatingBase::OnEntranceEnd();
	
	//UIFloatChat_V10::SetFocus_ChatEditBox();
}

void	UIFloatChat_V10::SetFocus_ChatEditBox()
{
	LuaState* L = _CallLuaFunction( "StartFocus");
	if( L != nullptr)
		_EndLuaFunction(L, 0);
}

/*virtual*/ bool UIFloatChat_V10::OnExitStart( void)
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	I3TRACE("Chat Exit Start\n");

	//m_slide.ForceEnd();
	//OnSlideOut();

	return true;
}

/*virtual*/ bool UIFloatChat_V10::OnEnterMouse( i3UIControl * pControl)
{
	if( UIFloatingBase::OnEnterMouse( pControl) == false)
		return false;

	// Phase�� ���� �̺�Ʈ�� �˷��ش�.
	// �̰��� ToolTip�� �ݾ��ֱ� ���� ��) �κ��丮, ��
	if( GetMainFrame()->GetCurrentSubPhase() != nullptr)
	{
		UIPhaseSubBase * pSubPhase = GetMainFrame()->GetCurrentSubPhase();

		pSubPhase->OnEnterMouse( pControl);
	}

	return true;
}

/*virtual*/ void UIFloatChat_V10::OnMovePhase( UIPHASE phase)
{
	switch( phase)
	{
	case UIPHASE_LOBBY :		m_ChatArea = CHAT_AREA_LOBBY; break;
	case UIPHASE_READY_ROOM :	m_ChatArea = CHAT_AREA_READYROOM; break;
//	case UIPHASE_CLAN :	m_ChatArea = CHAT_AREA_CLAN_MEMBER;	break;
	}

	_SetChattingCombo( 0);

	_SetMegaPhoneMainFrame(false);
}

/*virtual*/ void UIFloatChat_V10::OnMoveSubPhase(UISUBPHASE phase)
{
	_SetMegaPhoneMainFrame(false);
}

void UIFloatChat_V10::ClickMegaPhoneOpenButton()
{
	_SetMegaPhoneMainFrame(!m_bMegaPhoneOpen);
	return;
}

void UIFloatChat_V10::SelectChatChannel( INT32 idx)
{
	//_AttachToTopScreen();
	if( m_bWhisperMode)
	{
		m_pChatBox->SetText( L"");
		m_bWhisperMode = false;
	}

	// Ŭ���̰��Ի����Դϴ�.
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	if((idx == CHAT_TYPE_CLAN) && (MyUserInfoBasic.m_ui32Clanidx == 0 || MyUserInfoBasic.m_ui32ClanState == 0))
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_CHAT_INGAME_NOTICE_NO_JOIN_CLAN"));/*<Ŭ�����̰����ѻ����Դϴ�.>*/					
		m_bOnceChatType = false;
		m_bOnceChatTypeAll = false;
		m_nChatType = CHATTING_TYPE_ALL;
		SelectChatChannel(CHAT_TYPE_ALL);
		g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_ALL"));

		LuaState * L = _CallLuaFunction("SetSelectChattingTab");
		i3Lua::PushInteger(L, 0);
		_EndLuaFunction(L,1);
	}
	else
	{
		m_nCurrentSelectedChatType = idx;
		_SetChannel( m_Tab[idx]);

		UpdateChatBuffer();

		// minimize ���¶�� Ű���ش�.
		/*if( IsMinimizeDone())
			OnMaximizeStart();*/
	}
}

void UIFloatChat_V10::InitChat( CHAT_AREA area, INT32 nMaxLineCount)
{
	LuaState * L;

	m_ChatArea = area;
	m_nMaxChatLineCount = nMaxLineCount;

	m_Tab[0] = CHAT_TYPE_ALL; m_Tab[2] = CHAT_TYPE_CLAN;	m_Tab[3] = CHAT_TYPE_WHISPER;

	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
		m_Tab[1] = CHAT_TYPE_TEAM;
	else
		m_Tab[1] = CHAT_TYPE_NONE;
	

	L = _CallLuaFunction( "InitTab");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, m_Tab[0]);
		i3Lua::PushInteger( L, m_Tab[1]);
		i3Lua::PushInteger( L, m_Tab[2]);
		i3Lua::PushInteger( L, m_Tab[3]);
		_EndLuaFunction( L, 4);
	}

	// Chatting combo�� ����
	INT32 cur = 0;
	switch( area)
	{
	case CHAT_AREA_CLAN_MATCH :		cur = 2;	break;
	case CHAT_AREA_CLAN_MEMBER :	cur = 2;	break;
	}
	_SetChattingCombo( cur);
}


void UIFloatChat_V10::_UpdateMacroChat(void)
{
	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ))
	{
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_1 ))
		{
			_SendMacroChat(0);
		}
		else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_2 ))
		{
			_SendMacroChat(1);
		}
		else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_3 ))
		{
			_SendMacroChat(2);
		}
		else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_4 ))
		{
			_SendMacroChat(3);
		}
		else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_5 ))
		{
			_SendMacroChat(4);
		}
	}
}

void UIFloatChat_V10::_SendMacroChat( INT32 nIdx )
{
	if(nIdx >= MACRO_MAX_COUNT)
		return;

	// Macro�� �̿��� �Է�ó��
	CHAT_INFO info;

	if( i3::generic_string_size( g_pEnvSet->m_wstrMacro[nIdx]) > 0)
	{	
		if( g_pEnvSet->m_bMacroEnableFriend[nIdx])	
		{
			if( m_ChatArea != CHAT_AREA_READYROOM)
				return;
		}

		// �弳����
		i3::rc_wstring wstrErrorMsg;

		if( g_pFramework->CheckFilter_EnvSetMacroString(g_pEnvSet->m_wstrMacro[nIdx], wstrErrorMsg) )
		{
			if( g_pEnvSet->m_bMacroEnableFriend[nIdx])	
				info._Type = CHATTING_TYPE_TEAM;		// ��
			else										
				info._Type = CHATTING_TYPE_ALL;			// ��ü

			i3::rc_wstring wstrMacro = g_pEnvSet->m_wstrMacro[nIdx];
			info.setString( wstrMacro);

			/*info.setString( g_pEnvSet->m_wstrMacro[nIdx]);*/

			GameEventSender::i()->SetEvent(EVENT_CHATTING_N , &info ); 
		}
		else
		{
			g_pFramework->getChatBox()->PutSystemChatting(wstrErrorMsg);
		}
	}
}

void UIFloatChat_V10::_UpdateLang(void)
{
#if defined ( LOCALE_NORTHAMERICA) || defined(LOCALE_SINGMAL)  || defined(LOCALE_PHILIPPINES) // �Ϲ�,�̸� ������
#else
	// IME��忡 ���� ��Ʈ���� �����մϴ�.
	i3::stack_wstring pszLang = L"?";

	// IME��忡 ���� ��Ʈ���� �����մϴ�.
	if( i3UI::getGlobalIme() == nullptr)
		return;

	i3Ime * pIME = i3UI::getGlobalIme();

	LANGID langId = pIME->getLangID();
	UINT32 nInputLang = PRIMARYLANGID( langId);
	UINT32 nInputSubLang = SUBLANGID( langId);

	// mode off is english
	if( pIME->GetImeMode() != IME_MODE_NATIVE)
		nInputLang = LANG_ENGLISH;

	// EXPORT_AVALIABLE_CODE_VALUE
	//	Language mark

	switch(GetCodePage())
	{
	case I3_LANG_CODE_KOREAN:
		{
			switch( nInputLang)
			{
			case LANG_KOREAN:	pszLang = GAME_STRING("STR_TBL_CHAT_INGAME_CHAT_TARGET_KOREAN");/*��*/	break;
			case LANG_ENGLISH:	pszLang = GAME_STRING("STR_TBL_CHAT_INGAME_CHAT_TARGET_ENGLISH");/*��*/	break;
			}
		}
		break;
	case I3_LANG_CODE_THAI:
		{
			switch( nInputLang)
			{
			case LANG_THAI:		pszLang = L"TH";		break;
			case LANG_ENGLISH:	pszLang = L"EN";		break;
			}
		}
		break;
	case I3_LANG_CODE_JAPANESE:
		{
			switch( nInputLang)
			{
			case LANG_JAPANESE:	pszLang = L"JP";	break;
			case LANG_ENGLISH:	pszLang = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:
	case I3_LANG_CODE_CHINESE_TRADITIONAL:
		{
			switch( nInputLang)
			{
			case LANG_CHINESE:	pszLang = L"CH";	break;
			case LANG_ENGLISH:	pszLang = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_RUSSIA:
		{
			switch( nInputLang)
			{
			case LANG_RUSSIAN:	pszLang = L"RU";	break;
			case LANG_ENGLISH:	pszLang = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_TURKISH:
		{
			switch( nInputLang)
			{
			case LANG_TURKISH:	pszLang = L"TR";	break;
			case LANG_ENGLISH:	pszLang = L"EN";	break;
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
						case SUBLANG_PORTUGUESE:			pszLang = L"PR";	break;
						case SUBLANG_PORTUGUESE_BRAZILIAN:	pszLang = L"BR";	break;
					}
				}
				break;
			case LANG_SPANISH:	pszLang = L"SP";	break;
			case LANG_ITALIAN:	pszLang = L"IT";	break;
			case LANG_ENGLISH:	pszLang = L"EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_ARABIC:
		{
			switch( nInputLang)
			{
			case LANG_ARABIC:	pszLang = L"AR";	break;
			case LANG_ENGLISH:	pszLang = L"EN";	break;
			}
		}
		break;
	}

	LuaState * L = _CallLuaFunction( "Language");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, pszLang);
		_EndLuaFunction( L, 1);
	}
#endif
}

void UIFloatChat_V10::_ChatHistory(EDIT_STATE state)
{
	i3::rc_wstring wstrHistoryBufferText;

	// Chat History���
	if( g_pFramework->getChatBox()->GetHistoryBufferCount() > 0)
	{
		INT32 nChatMode = CHATTING_TYPE_ALL;

		if (state == EDIT_STATE_ONKEYDOWN_UP )
		{
			m_bUseHistory = true;

			if( g_pFramework->getChatBox()->GetHistoryBufferOffset() == 0)
			{
				m_nOldChatType = m_nChatType;
			}

			g_pFramework->getChatBox()->GetHistoryBufferText(1, wstrHistoryBufferText, &nChatMode);

			if( !wstrHistoryBufferText.empty() ) // szText[0] != nullptr)
			{
				m_nChatType = nChatMode;
			}
			else
			{
				m_pChatBox->GetIME()->SetInputText( L"", 0);
			}

			g_pFramework->SetFocus( m_pChatBox);
		}
		else if (state == EDIT_STATE_ONKEYDOWN_DOWN)
		{
			m_bUseHistory = true;

			g_pFramework->getChatBox()->GetHistoryBufferText(-1, wstrHistoryBufferText, &nChatMode);

			if( !wstrHistoryBufferText.empty())
			{
				m_nChatType = nChatMode;
			}
			else
			{
				m_bUseHistory = false;
				m_pChatBox->GetIME()->SetInputText( L"", 0);
				m_nChatType = m_nOldChatType;
			}
		}
	}

	COLOR col;
	i3::wstring		wstrText;

	i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));

	CHAT_SYS_TYPE ChatType = g_pFramework->getChatBox()->ParsingText( wstrHistoryBufferText );		// �Ľ��� ������ �����丮 ��ȭ�� ������..�װ� ����..

	if( ChatType == CHAT_SYS_TYPE_WHISPER || ChatType == CHAT_SYS_TYPE_REPLY)
	{
		size_t index = wstrHistoryBufferText.rfind(L" ");

		if( index != i3::rc_wstring::npos )
		{
			i3::const_wchar_range word( wstrHistoryBufferText.begin() + index + 1, wstrHistoryBufferText.end() );

			if( ChatType == CHAT_SYS_TYPE_WHISPER )
			{
				_ToWhisperChatMessage(wstrText, g_pFramework->getChatBox()->GetChatTarget(), word );
				m_nChatType = CHATTING_TYPE_WHISPER;
			}
			else
			{
				_ToWhisperChatMessage(wstrText, g_pFramework->getChatBox()->GetChatReplyTarget(), word );
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

void UIFloatChat_V10::_ToWhisperChatMessage(i3::wstring& out, const i3::rc_wstring& target, const i3::wliteral_range& msg)
{
	i3::sprintf(out, L"[To:%s] %s", target, msg );			
}

void UIFloatChat_V10::_SetMegaPhoneMainFrame(bool bEnable)
{
	m_bMegaPhoneOpen = bEnable;
	LuaState * L = _CallLuaFunction("SetMegaPhoneFrameEnable");
	if (L != nullptr)
	{
		i3Lua::PushBoolean(L, bEnable == true ? true : false);
		_EndLuaFunction(L, 1);
	}

	int temp_open_close = MEGAPHONE_FRAME_CLOSE;
	if (bEnable)
		temp_open_close = MEGAPHONE_FRAME_OPEN;
}

void UIFloatChat_V10::_SetChatColorFromType( void )
{
	COLOR col;

	if( m_bOnceChatTypeAll)
	{
		i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
		m_pChatCombo->SetTextColor( &col);
		m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_ALL"));
	}
	else if( m_bOnceChatType)
	{
		i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
		m_pChatCombo->SetTextColor( &col);
		m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_TEAM"));
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
			//m_pChatCombo->SetTextColor( &col);
			m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_ALL"));
			break;
		case CHATTING_TYPE_TEAM:		// ��
			i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
			//m_pChatCombo->SetTextColor( &col);
			m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_TEAM"));
			break;
		case CHATTING_TYPE_CLAN:		// Ŭ��
			i3Color::Set(&col, GameGUI::GetColor(GCT_CLAN_CHAT));
			//m_pChatCombo->SetTextColor( &col);
			m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_CLAN"));
			break;
		case CHATTING_TYPE_WHISPER:		// �Ӹ�
			i3Color::Set(&col, GameGUI::GetColor(GCT_WISPER_CHAT));
			break;
		case CHATTING_TYPE_REPLY:		// �亯
			i3Color::Set(&col, GameGUI::GetColor(GCT_WISPER_CHAT));
			break;
		case CHATTING_TYPE_MEGAPHONE:
			i3Color::Set(&col, GameGUI::GetColor(GCT_MEGAPHONE_CHAT));
			break;
		default: break;
		}
	}

	g_pFramework->getChatBox()->SetEnable( true);
	m_pChatBox->SetTextColor( &col);
}

void UIFloatChat_V10::_UpdateWhisperChat( const i3::rc_wstring& wstrTarget )
{
	i3::wstring wszWhisper;
	_ToWhisperChatMessage(wszWhisper, wstrTarget );

	g_WhisperLength = wszWhisper.length();

	const i3::rc_wstring& wstrHistoryText = g_pFramework->getChatBox()->GetHistoryText();

	wszWhisper.append(wstrHistoryText.begin(), wstrHistoryText.end());

	//i3String::CopyASCIItoWCHAR( szWCharTemp, szWhisper);

	g_pFramework->SetFocus( nullptr);
	m_pChatBox->SetText( wszWhisper);
	g_pFramework->SetFocus( m_pChatBox);
}

CHAT_SYS_TYPE UIFloatChat_V10::_UpdateChatBox( const i3::wliteral_range& wImeBufRng )
{
	CHAT_SYS_TYPE eChatType = CHAT_SYS_TYPE_NONE;

	if( m_nChatType != CHATTING_TYPE_WHISPER && m_nChatType != CHATTING_TYPE_REPLY)
	{
		eChatType = g_pFramework->getChatBox()->ParsingText( wImeBufRng);
		{
			switch( eChatType)
			{
			case CHAT_SYS_TYPE_WHISPER:
				{
					if( !m_bUseHistory) m_nOldChatType = m_nChatType;
					m_nChatType = CHATTING_TYPE_WHISPER;

					_UpdateWhisperChat( g_pFramework->getChatBox()->GetChatTarget() );

					OnWhisper(g_pFramework->getChatBox()->GetChatTarget());
				}
				break;
			case CHAT_SYS_TYPE_REPLY:
				{
					if( !m_bUseHistory) m_nOldChatType = m_nChatType;
					m_nChatType = CHATTING_TYPE_REPLY;

					if( i3::generic_string_size(g_pFramework->getChatBox()->GetChatReplyTarget()) > 0)
					{
						_UpdateWhisperChat( g_pFramework->getChatBox()->GetChatReplyTarget() );
					}
					else
					{
						m_nChatType = m_nOldChatType;
						g_pFramework->SetFocus(nullptr);
						m_pChatBox->SetText( L"");
						g_pFramework->SetFocus( m_pChatBox);
					}
				}
				break;
			case CHAT_SYS_TYPE_ONETIME_TEAM:	// ��ȸ�� �� ê��!!
				if( m_ChatArea == CHAT_AREA_READYROOM)
				{
					m_bOnceChatType = true;
					m_pChatBox->SetText( L"");
					m_pChatBox->SetIMEText( L"");
					g_pFramework->SetFocus( m_pChatBox);
				}
				break;
			case CHAT_SYS_TYPE_MEGAPHONE:
				{
					if (!m_bUseHistory) m_nOldChatType = m_nChatType;

					m_nChatType = CHATTING_TYPE_MEGAPHONE;

					g_pFramework->SetFocus(nullptr);

					i3::wstring wstrMegaPhone;
					i3::sprintf(wstrMegaPhone, L"%s ", GAME_RCSTRING("STR_Megaphone_Input"));

					m_pChatBox->SetText(wstrMegaPhone);
					m_pChatBox->SetTextCaretPos(wstrMegaPhone.length());

					g_pFramework->SetFocus(m_pChatBox);

					m_pChatBox->SetTextColor(GameGUI::GetColor(GCT_MEGAPHONE_CHAT));
				}
				break;
			}
		}
	}

	if( m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY || m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_MEGAPHONE)
	{
		// �ӼӸ� ���¿��� �ƹ��� �ؽ�Ʈ�� �Էµ��� �������¿��� ���ڰ� �������� �ʱ�ȭ�մϴ�.
		i3::string strConv;
		i3::utf16_to_mb(m_pChatBox->getText(), strConv);
		i3::string strTemp;

		// GameGUI::RemoveColorKeyword( szConv, szTemp, sizeof( szTemp));
		GameUI::RemoveColorKeywordCopy(strConv, strTemp);

		if( INT32(strTemp.size())  < g_WhisperLength)
		{
			m_nChatType = m_nOldChatType;
			g_WhisperLength = 0;

			m_pChatBox->SetText( L"");
			m_pChatBox->GetIME()->SetInputText( L"", 0);
			m_pChatBox->SetTextCaretPos( g_WhisperLength);
		}

		if( m_nChatType == CHATTING_TYPE_WHISPER)
		{
			i3::wstring wstrWhisper;
			_ToWhisperChatMessage(wstrWhisper, g_pFramework->getChatBox()->GetChatTarget() );

			if( i3::contain_string( m_pChatBox->getTextRange(), wstrWhisper ) == -1)
			{
				m_nChatType = m_nOldChatType;
				g_WhisperLength = 0;

				m_pChatBox->SetText( L"");
				m_pChatBox->GetIME()->SetInputText( L"", 0);
				m_pChatBox->SetTextCaretPos( g_WhisperLength);
			}
		}

		if( m_nChatType == CHATTING_TYPE_REPLY)
		{
			i3::wstring wstrWhisper;
			_ToWhisperChatMessage(wstrWhisper, g_pFramework->getChatBox()->GetChatReplyTarget() );

			if( i3::contain_string( m_pChatBox->getTextRange(), wstrWhisper ) == -1)
			{
				m_nChatType = m_nOldChatType;
				g_WhisperLength = 0;

				m_pChatBox->SetText( L"");
				m_pChatBox->GetIME()->SetInputText( L"", 0);
				m_pChatBox->SetTextCaretPos( g_WhisperLength);
			}
		}

		if (m_nChatType == CHATTING_TYPE_MEGAPHONE)
		{
			i3::wstring wstrMegaPhone;
			i3::sprintf(wstrMegaPhone, L"%s ", GAME_RCSTRING("STR_Megaphone_Input"));

			if (i3::contain_string(m_pChatBox->getTextRange(), wstrMegaPhone) == -1) //	i3String::Contain( szContents, szWhisper) == -1)
			{
				m_nChatType = m_nOldChatType;
				g_WhisperLength = 0;

				m_pChatBox->SetText(L" ");
				m_pChatBox->GetIME()->SetInputText(L" ", 0);
				m_pChatBox->SetTextCaretPos(g_WhisperLength);
			}
		}
	}

	return eChatType;
}

bool UIFloatChat_V10::_SetWhisperChatToSend( i3::stack_wstring& out_only_chat, i3::stack_wstring& out_colored_chat, const i3::wliteral_range& wImeBufRng )
{
	out_colored_chat = L"{col:237, 86, 250, 255}";
	i3::generic_string_cat(out_colored_chat, wImeBufRng);
	i3::generic_string_cat(out_colored_chat, L"{/col}");

	_GetOnlyChatMessage(out_only_chat, wImeBufRng);

	m_nChatType = m_nOldChatType;

	return true;
}

void UIFloatChat_V10::_SendWhisperChatting( const i3::rc_wstring& wstrTarget, const i3::wliteral_range& wMsgRng, const i3::wliteral_range& wImeBufRng, bool bReply )
{
	const i3::wliteral_range& strMsg = wMsgRng;

	i3::stack_wstring wstrHistory;

	// bReply = true - ����, else �Ӹ�

	if(bReply == true)
	{	
		GameEventSender::i()->SetEvent( EVENT_SEND_WHISPER, &wstrTarget, strMsg.c_str() );
		wstrHistory = L"/r ";
	}
	else
	{
		// ģ�� ��Ͽ� �ִ� �г����� ��� �ش� UID�� ã�Ƽ� UID�� ������.
		INT64 i64ReceiverUID = GameUI::FindUserUidByNick(wstrTarget);

		if(i64ReceiverUID > 0)
			GameEventSender::i()->SetEvent( EVENT_SEND_WHISPER_FIND_UID, &wstrTarget, strMsg.c_str(), &i64ReceiverUID);
		else
			GameEventSender::i()->SetEvent( EVENT_SEND_WHISPER, &wstrTarget, strMsg.c_str() );

		wstrHistory = L"/w ";
		wstrHistory.append(wstrTarget.begin(), wstrTarget.end());
	}

	wstrHistory += L" ";

	i3::const_wchar_range only_chat_rng = wImeBufRng;
	only_chat_rng.advance_begin(g_WhisperLength);

	wstrHistory.append(only_chat_rng.begin(), only_chat_rng.end());

	g_pFramework->getChatBox()->PutHistory(wstrHistory, m_nChatType);
}

void UIFloatChat_V10::_SendChattingMsg( i3::wstring& inout_wstrImeBuf, CHAT_SYS_TYPE eChatType )
{
	bool bWhisper = false;
	INT32 nWhsiperTextCount = 0;
	bool bReply = false;
	INT32 nReplyTextCount = 0;
	bool bMegaphone = false;
	INT32 nMegaphoneTextCount = 0;


	switch( eChatType)
	{
	case CHAT_SYS_TYPE_CLEAR:
		g_pFramework->getChatBox()->ResetChatBuffer();
		m_pChatBox->SetText( L"");
		break;
	case CHAT_SYS_TYPE_HELP:
		g_pFramework->getChatBox()->SetHelpText();
		m_pChatBox->SetText( L"");
		break;
	}

	// GM�� �� ��ɾ� üũ.
	if ( m_ChatArea == CHAT_AREA_READYROOM  )
	{
		if ( CGameMaster::i()->SetGMCommand(inout_wstrImeBuf) )
			m_pChatBox->SetText( L"");
	}

	// GM�� �κ� ��ɾ� üũ.
	if ( m_ChatArea == CHAT_AREA_LOBBY )
	{
		if ( CGameMaster::i()->SetGMLobbyCommand(inout_wstrImeBuf) )
			m_pChatBox->SetText( L"");
	}

	// GM ��ɾ�� ä�ø޽����� �Ѹ��� �ʽ��ϴ�.
	if ( GM_COMMAND_UNKNOWN != CGameMaster::i()->GetCommandType(inout_wstrImeBuf) )
	{
		m_pChatBox->SetText( L"");
		return;
	}

	// ���ڰ� ���ų�.. Color �Է��� ���� Command �Է��̸� ����
	if( (i3::generic_string_size( inout_wstrImeBuf) <= 0) || (i3::icontain_string( inout_wstrImeBuf, L"{col:") >= 0)
		|| (i3::icontain_string( inout_wstrImeBuf, L"{/col}") >= 0) )	// revision 56445 ä�ý� �÷� ��ũ��Ʈ ���� �ȵǰ� �ٲ�(���¸� hansoft. 194)
		return;

	g_pFramework->ChangeFilterStringOK(inout_wstrImeBuf);		// inout_wstrImeBuf ���� ���� ����ȴ�.. (2014.07.15.����)

	CChattingInputChecker* input_checker = g_pFramework->GetChattingInputChecker();

	input_checker->UpdateText(inout_wstrImeBuf);

	if( !(( eChatType == CHAT_SYS_TYPE_CLEAR) || ( eChatType == CHAT_SYS_TYPE_HELP)) && input_checker->IsInputBlock() )
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_CHAT_INGAME_NOTICE_WARNING_REPEAT_WORD"));/*<���踦 �����ϱ� ���� �������� ä���� �Էµ��� �ʽ��ϴ�.>*/
	}
	else if( input_checker->IsRepeatBlock() ) 
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_CHAT_INGAME_NOTICE_WARNING_DUPLICATED_WORD"));/*<���� ������ �����Ͽ� �Է��� �� �����ϴ�.>*/
	}
	else
	{
		//g_pFramework->IsFilterChange(szImeBuf);

		i3::stack_wstring wstrTemp;
		i3::stack_wstring wstrTemp2;

		// ��Ʈ��ũ�� �޽����� �����ϴ�.
		CHAT_INFO info;
		info._Type = (UINT16)m_nChatType;

		if( m_bOnceChatTypeAll)
		{
			wstrTemp = inout_wstrImeBuf;
			//i3String::Copy( szTemp, szImeBuf, 256 );
		}
		else if( m_bOnceChatType)
		{
			wstrTemp = inout_wstrImeBuf;
			//i3String::Copy( szTemp, szImeBuf, 256 );
			info._Type = CHATTING_TYPE_TEAM;
		}
		else
		{

			switch( m_nChatType)
			{
			case CHATTING_TYPE_LOBBY :
			case CHATTING_TYPE_ALL:	// ��ü
				wstrTemp = inout_wstrImeBuf;
				break;
			case CHATTING_TYPE_TEAM:// ��
				wstrTemp = inout_wstrImeBuf;
				break;
			case CHATTING_TYPE_MATCH :
			case CHATTING_TYPE_CLAN_MEMBER_PAGE :
			case CHATTING_TYPE_CLAN:// Ŭ��
				wstrTemp = inout_wstrImeBuf;
				break;
			case CHATTING_TYPE_WHISPER: // �Ӹ�
				{
					bWhisper = _SetWhisperChatToSend(wstrTemp, wstrTemp2, inout_wstrImeBuf);

					if ( (INT32)inout_wstrImeBuf.size() > g_WhisperLength)
					{
						nWhsiperTextCount = inout_wstrImeBuf.size() - g_WhisperLength;
					}
				}
				break;
			case CHATTING_TYPE_REPLY:// �亯
				{
					bReply =_SetWhisperChatToSend(wstrTemp, wstrTemp2, inout_wstrImeBuf);

					if ( (INT32) inout_wstrImeBuf.size() > g_WhisperLength)
					{
						nReplyTextCount = inout_wstrImeBuf.size() - g_WhisperLength;
					}
				}
				break;
			case CHATTING_TYPE_MEGAPHONE:
				bMegaphone = true;
				wstrTemp = inout_wstrImeBuf;
				nMegaphoneTextCount = inout_wstrImeBuf.size();
				break;
			default: break;
			}
		}

		// �Ӹ��� ��� �ٸ� ������� ó���ؾ� �մϴ�.
		if( bWhisper)
		{
			if( nWhsiperTextCount > 0)
			{
				g_pFramework->getChatBox()->PutSystemChatting(wstrTemp2);
				_SendWhisperChatting(g_pFramework->getChatBox()->GetChatTarget(), wstrTemp, inout_wstrImeBuf, false);									
			}
		}
		else if( bReply)
		{
			if( nReplyTextCount > 0)
			{
				g_pFramework->getChatBox()->PutSystemChatting(wstrTemp2);
				_SendWhisperChatting(g_pFramework->getChatBox()->GetChatReplyTarget(), wstrTemp, inout_wstrImeBuf, true);
			}
		}
		else if (bMegaphone)
		{
			if (nMegaphoneTextCount > 0)
			{
				m_nChatType = m_nOldChatType;
				wstrTemp.erase(0, GAME_RCSTRING("STR_Megaphone_Input").size() + 1);
				_SendMegaPhoneChatting(wstrTemp);
			}
		}
		else
		{
			info.setString( wstrTemp);

			if( info._Type == CHATTING_TYPE_CLAN)	// Ŭ�� ä���̶��..
			{
				// Ŭ�� �̰��� �����Դϴ�.
				USER_INFO_BASIC		MyUserInfoBasic;
				UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
				if(MyUserInfoBasic.m_ui32Clanidx == 0 || MyUserInfoBasic.m_ui32ClanState == 0)
				{
					g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_CHAT_INGAME_NOTICE_NO_JOIN_CLAN"), CHATTING_TYPE_COMMENT);/*<Ŭ���� �̰����� �����Դϴ�.>*/
				}
				else
				{
					// TODO:ä�� �Է�
					GameEventSender::i()->SetEvent(EVENT_CHATTING_N , &info );
					g_pFramework->getChatBox()->PutHistory(inout_wstrImeBuf, m_nChatType);
				}
			}
			else
			{
				if( info._Type == CHATTING_TYPE_TEAM)
				{
					// TODO:ä�� �Է�
					GameEventSender::i()->SetEvent(EVENT_CHATTING_N , &info );
					g_pFramework->getChatBox()->PutHistory(inout_wstrImeBuf, m_nChatType);
				}
				else
				{
					if( m_ChatArea == CHAT_AREA_CLAN_MATCH)	//	Ŭ���� �κ񿡼� ����
						info._Type = CHATTING_TYPE_MATCH;

					if( m_ChatArea == CHAT_AREA_CLAN_MEMBER)	//	Ŭ�� ������ ���������� ����
						info._Type = CHATTING_TYPE_CLAN_MEMBER_PAGE;

					// Ŀ�´�Ƽ �ɼǿ����� ä�� ������ �ɷ��ִٸ�, �װͿ� ���� ó���� �մϴ�.
					if( g_pEnvSet->m_nChatType != 0 && (m_nChatType != CHATTING_TYPE_TEAM && m_nChatType != CHATTING_TYPE_CLAN) )
					{
						i3::stack_wstring wstr = L"[";
						i3::generic_string_cat( wstr, UserInfoContext::i()->GetMyNickName());
						i3::generic_string_cat( wstr, L"] ");
						i3::generic_string_cat( wstr, wstrTemp);

						g_pFramework->getChatBox()->PutSystemChatting(wstr);
						g_pFramework->getChatBox()->PutHistory(inout_wstrImeBuf, m_nChatType);
					}
					else
					{
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
						g_pFramework->getChatBox()->PutHistory(inout_wstrImeBuf, m_nChatType);
					}
				}
			}
		}

		g_WhisperLength = 0;
	}

	g_pFramework->getChatBox()->ResetHistoryBufferIndex();

	m_pChatBox->SetText( L"");

	m_bOnceChatType = false;
	m_bOnceChatTypeAll = false;

	if( m_bUseHistory)
	{
		m_nChatType = m_nOldChatType;
		m_bUseHistory = false;
	}
}

void UIFloatChat_V10::_SendMegaPhoneChatting(const i3::wliteral_range& wMsgRng)
{
	const i3::wliteral_range& strMsg = wMsgRng;
	UINT8 ui8Size = (UINT8)(strMsg.size() + 1) * sizeof(TTCHAR);

	INT32 ItemMegaPhoneInfo = CASHITEM_ITEMID_MEGAPHONE;

	T_ItemDBIdx i64UID = CInvenList::i()->FindWareDBIdx_ByItemID(ItemMegaPhoneInfo);

	if (i64UID >= 0)
		GameEventSender::i()->SetEvent(EVENT_SEND_MEGAPHONE, &i64UID, strMsg.c_str(), &ui8Size);
	else
	{
		i3InputKeyboard * pKeyboard = g_pFramework->getKeyboard();
		if (pKeyboard->GetStrokeState(I3I_KEY_ENTER) == true)
			pKeyboard->SetStrokeState(I3I_KEY_ENTER, false);

		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_MEGAPHONE_USE_FAIL"));
	}
}

void UIFloatChat_V10::_UpdateParsingType( EDIT_STATE state)
{
	if( m_pChatBox->GetIME() == nullptr)
		return;

	INT32 textLength = i3::generic_string_size( m_pChatBox->getText());
	if( textLength == 0)
		return;

	i3::wstring wstrImeBuf = m_pChatBox->getIMETextBufString();			// ������ ���� �ʰ� �ǵ������� �����Ѵ�.(�޺κп� ������ ������ �ִ�..)  

	/*char szRemoveKeyword[MAX_STRING_COUNT];
	i3String::Copy(szRemoveKeyword,szImeBuf,sizeof(szRemoveKeyword));
	GameGUI::RemoveColorKeyword(szRemoveKeyword,szImeBuf,sizeof(szImeBuf));*/

	// ä��â �÷�
	_SetChatColorFromType();

	// ChatBox Update (Return Type CHAT_SYS_TYPE)
	CHAT_SYS_TYPE eChatType = CHAT_SYS_TYPE_NONE;
	eChatType = _UpdateChatBox(wstrImeBuf);
	
	// IME ����Ʈ �Է¹޴� �κ��� �޵�� ���¸� üũ�ؼ� ó���ؾ� �˴ϴ�. komet 09.06
	// IME�� ó���ϴ� Windows API Proc�� �޽��� ť�� ������� ó���ϱ� �����Դϴ�.
	if( state == EDIT_STATE_RETURN)
	{	
		_SendChattingMsg(wstrImeBuf, eChatType);
	}
}

void UIFloatChat_V10::UpdateChatBuffer(void)
{
	INT32 i;

	if( m_pTextBox != nullptr)
	{
		// dummy chat text �߰�
		CGameChatBox * pFrameWorkChatBox = g_pFramework->getChatBox();

		INT32 BufferIdx= 0;
		if( pFrameWorkChatBox->GetBackUpBufferIndex() == 0)
		{
			BufferIdx = MAX_CHAT_BUFFER-1;
		}
		else
		{
			BufferIdx = pFrameWorkChatBox->GetBackUpBufferIndex() - 1;
		}

		// Buffer�� ���鼭 ����� ���ڸ� ����ϴ�.
		INT32 nAccumCount = 0;
		INT32 nLoopCount = 0;
		//char szTotalText[MAX_CHATBOXBUFF] = {0,};
		i3::wstring wstrTotalText;
		

		//2012. 12. 3 �ڱ⼺
		//szTotalText[52200] ������ ���� ������ ������ ����.
		//���� ������ �����ϴ� ����ۿ� ������.

		for( i = BufferIdx; nLoopCount < pFrameWorkChatBox->GetBufferCount(); i--)
		{
			nAccumCount++;

			if( i<0)
			{
				i = MAX_CHAT_BUFFER-1;
			}

			if (nLoopCount < 0 || nLoopCount >= 100)
				break;

			UINT16 chatType = pFrameWorkChatBox->getBufferType(i);

			if( chatType == 0)
			{// system message
				_CopyChatBufferToBox( wstrTotalText, i);
			}
			else
			{
				switch( m_CurrentTab)
				{
				case CHAT_TYPE_ALL :	_CopyChatBufferToBox( wstrTotalText, i);	break;
				case CHAT_TYPE_TEAM :
					if( chatType == CHATTING_TYPE_TEAM )
						_CopyChatBufferToBox( wstrTotalText, i);
					break;
				case CHAT_TYPE_CLAN :
					if( chatType == CHATTING_TYPE_CLAN ||
						chatType == CHATTING_TYPE_MATCH ||
						chatType == CHATTING_TYPE_CLAN_MEMBER_PAGE)
					{
						_CopyChatBufferToBox( wstrTotalText, i);
					}
					break;

				case CHAT_TYPE_WHISPER :
					if( chatType == CHATTING_TYPE_WHISPER ||
						chatType == CHATTING_TYPE_REPLY)
					{
						_CopyChatBufferToBox( wstrTotalText, i);
					}
					break;
				}
			}

			nLoopCount++;

		}

		// chat list�� ����մϴ�. 

		// ���� ������ '\n'�� �����ϴ� ��� �������ش�.( �׻� ä��â �ϴܿ� ���� ������ ���� �� ó����)
		if (!wstrTotalText.empty() )
		{
			const size_t size_m1 = wstrTotalText.size() - 1;
			if (wstrTotalText[size_m1] == L'\n')
				wstrTotalText.erase(size_m1);
		}

		m_pTextBox->SetText( wstrTotalText);

	}
}

void UIFloatChat_V10::UpdateMegaPhoneBuffer(void)
{
	INT32 i;

	if (g_pFramework->getChatBox()->GetMegaPhoneBufferCount() <= 0)
		return;

	if (m_pMegaPhoneMain_EditBox != nullptr)
	{
		CGameChatBox * pFrameWorkChatBox = g_pFramework->getChatBox();

		INT32 BufferIdx = 0;
		if (pFrameWorkChatBox->GetMegaPhoneBufferIndex() == 0)
		{
			BufferIdx = MAX_MEGAPHONE_BUFFER - 1;
		}
		else
		{
			BufferIdx = pFrameWorkChatBox->GetMegaPhoneBufferIndex() - 1;
		}

		// Buffer�� ���鼭 ����� ���ڸ� ����ϴ�.
		INT32 nAccumCount = 0;
		INT32 nLoopCount = 0;
		i3::wstring wstrTotalText;

		for (i = BufferIdx; nLoopCount < pFrameWorkChatBox->GetMegaPhoneBufferCount(); i--)
		{
			nAccumCount++;

			if (i<0)
			{
				i = MAX_MEGAPHONE_BUFFER - 1;
			}

			_CopyMegaPhoneBufferToBox(wstrTotalText, i);

			// ä�� �α׿� ��µǴ� �޼����� 300���� ����
			if (nLoopCount < 0 || nLoopCount >= MAX_MEGAPHONE_BUFFER)
				break;

			nLoopCount++;
		}

		// �ް��� ���� �����͹ڽ��� ����մϴ�.
		const i3::rc_wstring&  wstrChatBoxText = pFrameWorkChatBox->GetMegaPhoneBufferText(BufferIdx);
		if (!wstrChatBoxText.empty())
		{
			if(m_pMegaPhone_EditBox != nullptr)
				m_pMegaPhone_EditBox->SetText(wstrChatBoxText);
		}

		// �ް��� ���� �����͹ڽ��� ����մϴ�.
		m_pMegaPhoneMain_EditBox->SetText(wstrTotalText);
	}
}

void UIFloatChat_V10::ToggleLang( void)
{	
	i3Ime * pIME = i3UI::getGlobalIme();

	if( pIME == nullptr)
		return;

	m_CurrentLanguage++;
	if( m_CurrentLanguage >= m_LanguageCount)
		m_CurrentLanguage = 0;

	WPARAM param = INPUTLANGCHANGE_FORWARD;
	SendMessage( pIME->getHwnd(), WM_INPUTLANGCHANGEREQUEST, param, (LPARAM) m_LanguageList[m_CurrentLanguage]);

	// �� �����ϰ� �ٽ� Focus�� ê�ڽ���
	if( m_pChatBox)
	{
		g_pFramework->SetFocus( m_pChatBox);
	}
}

void UIFloatChat_V10::_UpdateWhisperBackupBuffer(void)
{
	for( INT32  i = 0 ; i < g_pFramework->getChatBox()->GetWhisperBufferCount() ; i++)
	{
		INT32 index = (g_pFramework->getChatBox()->GetCurrentWhisperBufferIndex() + i) % 5;
		g_pFramework->getChatBox()->PutSystemChatting(g_pFramework->getChatBox()->GetWhisperBufferText(index), CHATTING_TYPE_WHISPER);
	}

	g_pFramework->getChatBox()->ResetWhisperBuffer();
}

void UIFloatChat_V10::_UpdateChatType( void)
{
	if( i3UI::GetFocus() != m_pChatBox) return;

	// ä�� ���¼���.
	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ))
	{
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
		{
			if( m_ChatArea == CHAT_AREA_READYROOM)
			{
				if(m_nChatType == CHATTING_TYPE_WHISPER ||
					m_nChatType == CHATTING_TYPE_REPLY)
				{
					m_pChatBox->SetText( L"");
				}

				m_bOnceChatType = true;
				g_pFramework->SetFocus( m_pChatBox);
			}
		}
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F2 )
	{
		if( m_nChatType != CHATTING_TYPE_ALL ||
			m_bOnceChatType == true ||
			m_bOnceChatTypeAll == true)
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER ||
				m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetText( L"");
			}

			m_bOnceChatType = false;
			m_bOnceChatTypeAll = false;
			m_nChatType = CHATTING_TYPE_ALL;

			//LuaState * L = _CallLuaFunction("SetSelectChattingTab");
			//i3Lua::PushInteger(L, 0);
			//_EndLuaFunction(L,1);
			
			// FunctionŰ�δ� tab�� �ٲ��� ���� ChatType�� �ٲ�� �մϴ�.
			_SetChattingCombo(CHAT_TYPE_ALL);
			g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_ALL"), CHATTING_TYPE_COMMENT);

			COLOR col;
			i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
			m_pChatBox->SetTextColor( &col);
		}
		g_pFramework->SetFocus( m_pChatBox);
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F3 )
	{
		if( m_ChatArea == CHAT_AREA_READYROOM)
		{
			if( m_nChatType != CHATTING_TYPE_TEAM ||
				m_bOnceChatType == true ||
				m_bOnceChatTypeAll == true)
			{
				if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
				{
					m_pChatBox->SetText( L"");
				}

				m_bOnceChatType = false;
				m_bOnceChatTypeAll = false;
				m_nChatType = CHATTING_TYPE_TEAM;
				//LuaState * L = _CallLuaFunction("SetSelectChattingTab");
				//i3Lua::PushInteger(L, 1);
				//_EndLuaFunction(L,1);

				// FunctionŰ�δ� tab�� �ٲ��� ���� ChatType�� �ٲ�� �մϴ�.
				_SetChattingCombo(CHAT_TYPE_TEAM);
				g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"), CHATTING_TYPE_TEAM);

				COLOR col;
				i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
				m_pChatBox->SetTextColor( &col);
			}
			g_pFramework->SetFocus( m_pChatBox);
		}
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F4 )
	{

		if( m_nChatType != CHATTING_TYPE_CLAN ||
			m_bOnceChatType == true ||
			m_bOnceChatTypeAll == true)
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER ||
				m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetText(L"");
				i3UI::getGlobalIme()->SetInputText( L"", 0);
			}

			m_bOnceChatType = false;
			m_bOnceChatTypeAll = false;
			m_nChatType = CHATTING_TYPE_CLAN;
			g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"), CHATTING_TYPE_CLAN);

			//LuaState * L = _CallLuaFunction("SetSelectChattingTab");
			//i3Lua::PushInteger(L, 2);
			//_EndLuaFunction(L,1);
			// FunctionŰ�δ� tab�� �ٲ��� ���� ChatType�� �ٲ�� �մϴ�.
			_SetChattingCombo(CHAT_TYPE_CLAN);
		}
		g_pFramework->SetFocus( m_pChatBox);
	}
}

void UIFloatChat_V10::_SetChannel( CHAT_TYPE type)
{
	switch( type)
	{
	case CHAT_TYPE_ALL :
		switch( m_ChatArea)
		{
		case CHAT_AREA_LOBBY:		SetChatType( CHATTING_TYPE_LOBBY);		break;
		case CHAT_AREA_READYROOM:	SetChatType( CHATTING_TYPE_ALL);		break;
		case CHAT_AREA_CLAN_MATCH:	SetChatType( CHATTING_TYPE_MATCH);		break;
		case CHAT_AREA_CLAN_MEMBER:	SetChatType( CHATTING_TYPE_CLAN_MEMBER_PAGE);	break;
		default : I3PRINTLOG(I3LOG_FATAL, "Not support chat area %d !!!", m_ChatArea);		break;
		}
		break;

	case CHAT_TYPE_TEAM :		SetChatType( CHATTING_TYPE_TEAM);	break;
	case CHAT_TYPE_CLAN:		SetChatType( CHATTING_TYPE_CLAN);	break;
	case CHAT_TYPE_WHISPER :	SetChatType( CHATTING_TYPE_WHISPER);	break;
	}
	m_CurrentTab = type;

	// ����� �� ��ä�� �� ���� ��� �޺��ڽ��� ���� ����
	if(type == CHAT_TYPE_TEAM && GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM)
		return;

	// 1.0�� ���� ������ ��� ComboBox�� �������� �ʽ��ϴ�.
	//_SetChattingCombo( type);
}

void UIFloatChat_V10::_SetChattingCombo( INT32 cur)
{
	LuaState * L;
	i3::wstring wszChatTab;

	i3::generic_string_cat(wszChatTab, L"{col:255, 255, 255, 255}");
	i3::generic_string_cat(wszChatTab, GAME_STRING("STR_TBL_CHAT_CHANNEL_TYPE_ALL"));
	i3::generic_string_cat(wszChatTab, L"{/col}");
	i3::generic_string_cat(wszChatTab, L";");

	// ReadyRoom �� ��쿡�� ����� ���������. �� �ܿ��� ��ä�� �޺��� ����.
	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
	{
		i3::generic_string_cat(wszChatTab, L"{col:98, 180, 253, 255}");
		i3::generic_string_cat(wszChatTab, GAME_STRING("STR_TBL_CHAT_CHANNEL_TYPE_TEAM"));
		i3::generic_string_cat(wszChatTab, L"{/col}");
		i3::generic_string_cat(wszChatTab, L";");
	}

	i3::generic_string_cat(wszChatTab, L"{col:80, 255, 60, 255}");
	i3::generic_string_cat(wszChatTab, GAME_STRING("STR_TBL_CHAT_CHANNEL_TYPE_CLAN"));
	i3::generic_string_cat(wszChatTab, L"{/col}");

	//if( i3String::Length( m_lastWhisperUserNick) > 0)
	{
		i3::generic_string_cat(wszChatTab, L";");
		i3::generic_string_cat(wszChatTab, L"{col:237, 86, 250, 255}");
		i3::generic_string_cat(wszChatTab, GAME_STRING("STR_TBL_CHAT_CHANNEL_TYPE_WHISPER"));
		i3::generic_string_cat(wszChatTab, L"{/col}");
	}
	
	L = _CallLuaFunction( "SetChatTypeCombo");
	if( L != nullptr)
	{
		if(GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM && cur > 0)
		{
			cur -= 1;
		}

		i3Lua::PushStringUTF16To8( L, wszChatTab);
		i3Lua::PushInteger( L, cur);
		_EndLuaFunction( L, 2);
	}
}

void UIFloatChat_V10::_CopyChatBufferToBox(i3::wstring& inout_wstr, INT32 bufferIdx)
{
	CGameChatBox * pChatBox = g_pFramework->getChatBox();

	const i3::rc_wstring&  wstrChatBoxText = pChatBox->GetBackUpBufferText( bufferIdx);

	if(!wstrChatBoxText.empty())
	{
		i3::stack_wstring wstr_StackChatBoxText = wstrChatBoxText;
		wstr_StackChatBoxText += L'\n';
		i3::insert_string(inout_wstr, 0, wstr_StackChatBoxText);
	}
}

void UIFloatChat_V10::_CopyMegaPhoneBufferToBox(i3::wstring& inout_wstr, INT32 bufferIdx)
{
	CGameChatBox * pChatBox = g_pFramework->getChatBox();

	const i3::rc_wstring&  wstrChatBoxText = pChatBox->GetMegaPhoneBufferText(bufferIdx);

	if (!wstrChatBoxText.empty())
	{
		i3::stack_wstring wstr_StackChatBoxText = wstrChatBoxText;
		wstr_StackChatBoxText += L'\n';
		i3::insert_string(inout_wstr, 0, wstr_StackChatBoxText);
	}
}

void UIFloatChat_V10::SetWhisperCommand(void)
{
	i3::stack_wstring wszWhisperCommand = L"/w ";
	
	m_pChatBox->SetText(wszWhisperCommand);
	g_pFramework->SetFocus( m_pChatBox);
	g_WhisperLength = wszWhisperCommand.length();
	m_pChatBox->SetTextCaretPos( g_WhisperLength);
}

void UIFloatChat_V10::SetLastWhsiperNick(const i3::rc_wstring& wstrNick)
{
	if(wstrNick.length() <= 0)
		return;

	// ������ �ӼӸ� �� ���� ����
	if(i3::generic_compare(wstrNick, m_lastWhisperUserNick) != 0)
		i3::safe_string_copy(m_lastWhisperUserNick, wstrNick, NET_CHARA_NICK_NAME_SIZE);
		//i3String::Copy(m_lastWhisperUserNick, pszNick, i3String::Length(pszNick) + 1);
}

void UIFloatChat_V10::OnWhisper( INT32 slotIdx)
{
	i3::rc_wstring wstrNick = BattleSlotContext::i()->getNickForSlot(slotIdx);

	OnWhisper(wstrNick);
}

void UIFloatChat_V10::OnWhisper( const i3::rc_wstring& wstrNick)
{
	g_pFramework->SetFocus( nullptr);

	// ���������� �ӼӸ��� ���� ����
	SetLastWhsiperNick(wstrNick);

	// ä��â�� �ӼӸ� Ŀ��� �Է�
	i3::wstring wstrWhisper;
	_ToWhisperChatMessage(wstrWhisper, wstrNick );

	m_pChatBox->SetText(wstrWhisper);
	g_pFramework->SetFocus( m_pChatBox);

	m_bWhisperMode = true;
	m_nChatType = CHATTING_TYPE_WHISPER;
	g_WhisperLength = wstrWhisper.length();

	g_pFramework->getChatBox()->SetChatTarget(wstrNick);

	m_pChatBox->SetTextColor( GameGUI::GetColor(GCT_WISPER_CHAT) );
	m_pChatBox->SetTextCaretPos( g_WhisperLength);
}

void UIFloatChat_V10::_GetOnlyChatMessage(i3::stack_wstring& out,	const i3::wliteral_range& wfullMsgRng)
{
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

void UIFloatChat_V10::SetChatTypeFromCombo( INT32 idxItem)
{
	if( idxItem == -1)
		return;

	if( m_bWhisperMode)
	{
		m_pChatBox->SetText( L"");
		m_bWhisperMode = false;
	}

	// �κ񿡼��� ��ä�� �޺��� ���⶧���� Ŭ��, �Ӹ� �޺� ������ ��� Item�� +1 ���ݴϴ�.
	if(GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM && idxItem > 0)
		idxItem += 1;

	switch( idxItem )
	{
	case 0 :
		switch( m_ChatArea)
		{
		case CHAT_AREA_READYROOM :	SetChatType( CHATTING_TYPE_ALL);	break;
		case CHAT_AREA_LOBBY :		SetChatType( CHATTING_TYPE_LOBBY);	break;
		case CHAT_AREA_CLAN_MATCH :	SetChatType( CHATTING_TYPE_MATCH);	break;
		case CHAT_AREA_CLAN_MEMBER : SetChatType( CHATTING_TYPE_CLAN_MEMBER_PAGE);	break;
		default : I3PRINTLOG(I3LOG_FATAL, "Invalid Chat area!!!");	break;
		}
		break;

	case 1 :	SetChatType( CHATTING_TYPE_TEAM);	break;
	case 2 :	SetChatType( CHATTING_TYPE_CLAN);	break;
	case 3 :
		
		// ������ �ӼӸ��� �ְ���� ������ ����, ������ /w Command �Է�
		// �̰� ���� ����ó���� ���� �ڵ�
		SetChatType( CHATTING_TYPE_WHISPER);
		if(m_lastWhisperUserNick.length() > 0)
		{
			//SetChatType( CHATTING_TYPE_WHISPER);
			OnWhisper( m_lastWhisperUserNick);
		}
		else
		{
			//SetChatType( CHATTING_TYPE_ALL);
			SetWhisperCommand();
		}
		break;
	default : I3PRINTLOG(I3LOG_FATAL, "Not support value %d!!!", idxItem);	break;
	}

	INT32 i = -1;
	if( m_CurrentTab != CHAT_TYPE_NONE )
	{
		for( i = 0; i < CHAT_TYPE_MAX; i++)
		{
			if( m_Tab[i] == m_CurrentTab)
			{	
				break;
			}
		}
	}
	
	LuaState * L = _CallLuaFunction( "SetSelectTab");
	i3Lua::PushInteger( L, i);
	_EndLuaFunction( L, 1);
}

void UIFloatChat_V10::SetChatType( INT32 type)
{
	if( m_nChatType != type)
	{
		if( m_nChatType == CHATTING_TYPE_WHISPER)
		{
			m_pChatBox->SetText(L"");
		}
			
		m_nChatType = type;

		m_bOnceChatType = false;
		m_bOnceChatTypeAll = false;

		switch( m_nChatType)
		{
		case CHATTING_TYPE_ALL :
		case CHATTING_TYPE_LOBBY :
		case CHATTING_TYPE_MATCH :
		case CHATTING_TYPE_CLAN_MEMBER_PAGE :
			if( m_CurrentTab != CHAT_TYPE_ALL)
				m_CurrentTab = CHAT_TYPE_ALL;
			{
				COLOR col;
				i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
				m_pChatBox->SetTextColor( &col);
				//g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_ALL"), CHATTING_TYPE_COMMENT);
			}
			break;
		case CHATTING_TYPE_TEAM :
			if( m_CurrentTab != CHAT_TYPE_ALL)
				m_CurrentTab = CHAT_TYPE_TEAM;

			{
				COLOR col;
				i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
				m_pChatBox->SetTextColor( &col);
				//g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"), CHATTING_TYPE_COMMENT);
			}
			break;
		case CHATTING_TYPE_CLAN :
			if( m_CurrentTab != CHAT_TYPE_ALL)
				m_CurrentTab = CHAT_TYPE_CLAN;
			{
				COLOR col;
				i3Color::Set(&col, GameGUI::GetColor(GCT_CLAN_CHAT));
				m_pChatBox->SetTextColor( &col);
				//g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"), CHATTING_TYPE_COMMENT);
			}
			break;
		case CHATTING_TYPE_WHISPER :
			if( m_CurrentTab != CHAT_TYPE_ALL)
				m_CurrentTab = CHAT_TYPE_WHISPER;
			{
				COLOR col;
				i3Color::Set(&col, GameGUI::GetColor(GCT_WISPER_CHAT));
				m_pChatBox->SetTextColor( &col);
			}
			break;
		}
	}
}

void UIFloatChat_V10::_SetPhaseAtPos()
{
	if( GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
	{	
		i3UIFrameWnd *	pChatBox = (i3UIFrameWnd*)GetMainFrame()->GetCurrentPhase()->GetScene(0)->FindChildByName("ChatBoxFrameWnd");

		if(pChatBox != nullptr)
		{
			VEC3D * _pos = pChatBox->getPos();
			m_pFrameWnd->setPos(_pos);
		}
	}
}
