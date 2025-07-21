#include "pch.h"
#include "UIFloatChat.h"
#include "UIFloatUserList.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "UIPhaseSubBase.h"

#include "../GameGUI.h"
#include "../GameMaster.h"
#include "../ClanContext.h"
#include "../TextSet.h"

#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/atoi.h"

#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/generic_string_copy.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "ClanGameContext.h"
#include "../OutGameQACommandSet.h"

#define USERLIST_UPDATE_TIME 0.5f

#define MEGAPHONE_FRAME_CLOSE 0
#define MEGAPHONE_FRAME_OPEN 1

static INT32 g_WhisperLength = 0;

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {

	//�ϴ� Ȯ���⿡�� ��� �մϴ�.
	int UIFloatChat_ClickMegaPhoneOpen( LuaState * L )
	{
		UIFloatChat * pThis = static_cast<UIFloatChat *>(GameUI::GetFloating(UFW_CHAT));
		if( pThis != nullptr )
			pThis->ClickMegaPhoneOpenButton();
		return 0;
	}
};

LuaFuncReg FloatChat_Glue[] = {
	{ "ClickMegaPhoneOpen",	UIFloatChat_ClickMegaPhoneOpen },
	{ nullptr, nullptr }
};

I3_CLASS_INSTANCE( UIFloatChat);//, UIFloatingBase);

bool UIFloatChat::m_bUseHistory = false;

UIFloatChat::UIFloatChat() : m_pChatMainBox(nullptr), m_pSubChatMain(nullptr), m_nChatType(CHATTING_TYPE_ALL)
	, m_nOldChatType(CHATTING_TYPE_ALL), m_ChatArea(CHAT_AREA_LOBBY), m_bOnceTeamChat(false), m_bNotice(false)
	, m_CurrentLanguage(0), m_bMegaPhoneOpen(false), m_pParentMegPhoneMain(nullptr)
{	
	m_lastWhisperUserNick[ 0] = 0;

	m_AlignedPos = UIFLOATING_POS_RIGHTBOTTOM;
	m_bFixedWindow = true;

	m_pMegaPhone_Button = nullptr;
	m_pMegaPhoneMain_EditBox = nullptr;
	m_pMegaPhone_EditBox = nullptr;

	RegisterGameEventFunc( EVENT_CHANGE_READYROOM_INFO,		&UIFloatChat::ReceiveGameEvent_Change_ReadyroomInfo);

#if defined( I3_DEBUG)
	SetName( "ChatBox");
#endif
	m_pOutGameQACommandSet = g_pFramework->getOutGameQACommandSet();
}

UIFloatChat::~UIFloatChat()
{
}

/*virtual*/ void UIFloatChat::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);

	m_pFrameWnd = (i3UIFrameWnd*) pScene->FindChildByName( "main");
	I3ASSERT( m_pFrameWnd != nullptr);

	m_pChatMainBox = (i3UIFrameWnd*) pScene->FindChildByName( "FrameWnd_ChatWnd");
	I3ASSERT( m_pChatMainBox);

	m_pSubChatMain = (i3UIFrameWnd*) m_pChatMainBox->FindChildByName( "FrameWnd_Chat");	// 
	I3ASSERT( m_pSubChatMain);

	m_pTextBox = (i3UIEditBox*) pScene->FindChildByName( "Edit_ChatLog");
	I3ASSERT( m_pTextBox);

	m_pChatBox = (i3UIEditBox*) pScene->FindChildByName( "Edit_Chat");
	I3ASSERT( m_pChatBox);

	m_ChatArea = CHAT_AREA_LOBBY;

	m_pMegaPhoneMain_EditBox = (i3UIEditBox *) pScene->FindChildByName( "MegaPhone_Main_EditBox" );
	I3ASSERT( m_pMegaPhoneMain_EditBox );

	m_pMegaPhone_EditBox = (i3UIEditBox *) pScene->FindChildByName( "MegaPhone_EditBox" );
	I3ASSERT( m_pMegaPhone_EditBox );

	m_pMegaPhone_Button = (i3UIButton *) pScene->FindChildByName( "MegaPhone_OpenButton" );
	I3ASSERT( m_pMegaPhone_Button );

	m_pParentMegPhoneMain = (i3UIFrameWnd*) pScene->FindChildByName( "FrameWnd_MegaPhone_Main");
	I3ASSERT( m_pParentMegPhoneMain);
	
}

/*virtual*/ void UIFloatChat::_ClearAtUnload( i3UIScene * pScene)
{
	UIFloatingBase::_ClearAtUnload( pScene);

	m_pFrameWnd = nullptr;
	m_pChatMainBox = nullptr;
	m_pSubChatMain = nullptr;
	m_pTextBox = nullptr;
	m_pChatBox = nullptr;
	m_pMegaPhoneMain_EditBox = nullptr;
	m_pParentMegPhoneMain = nullptr;
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIFloatChat::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_ChattingWindow.i3UIs", FloatChat_Glue, true, false);

	_SearchLanguage();
	_ResetLanguage();
}

void UIFloatChat::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
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

/*virtual*/ bool UIFloatChat::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
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

/*virtual*/ bool UIFloatChat::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	bool res = UIFloatingBase::OnKeyInput( pInputMng);

	// ��ũ��
	_UpdateMacroChat();

	_UpdateChatType();

	return res;
}

/*virtual*/ bool UIFloatChat::OnKey_Enter( void)
{
	UIFloatUserList * pFrame = static_cast<UIFloatUserList *>(GameUI::GetFloating(UFW_USERLIST));
	
	if( pFrame != nullptr && pFrame->getUseSearchFrame() == false &&
		GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE)
	{
		if(!g_pFramework->IsMessageBoxEnabled())
		{
			if( !m_pChatBox->isFocused())
			{
				g_pFramework->SetFocus( m_pChatBox);
				return true;
			}
		}
	}

	return false;
}


/*virtual*/ bool UIFloatChat::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	LuaState* L = _CallLuaFunction( "__init");
	if( L != nullptr)
		_EndLuaFunction(L, 0);

	m_slide.ForceEnd();	//slide �ൿ�� �����Ѵ�.

	m_pTextBox->SetText( L"");
	m_pChatBox->SetText( L"");

	// ä�� ���� �ʱ�ȭ
	//g_pFramework->getChatBox()->ResetChatBuffer();

	// Update Whisper Backup Buffer
	_UpdateWhisperBackupBuffer();

	// �Է� ��� ��ư
	_UpdateLang();

	// �ʹ� ����� �޼���
	if( m_bNotice == false)
	{
		g_pFramework->getChatBox()->PutSystemChatting(g_pFramework->getChatNotice());
		m_bNotice = true;
	}

	UpdateChatBuffer();

	m_pChatBox->setUseCopyNPaste(true);

#if defined( LOCALE_KOREA) && !defined( SALES_BUILD)
#else
	// �ѱ��� ������ ������ LanguageButton ��Ȱ��	
	L = _CallLuaFunction( "EnableLanguageButton");
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

void	UIFloatChat::OnEntranceEnd()
{
	UIFloatingBase::OnEntranceEnd();

	UIFloatChat::SetFocus_ChatEditBox();
}

/*virtual*/ bool UIFloatChat::OnExitStart( void)
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	m_slide.ForceEnd();	//slide �ൿ�� �����Ѵ�.
	
	return true;
}

/*virtual*/ bool UIFloatChat::OnEnterMouse( i3UIControl * pControl)
{
	if( UIFloatingBase::OnEnterMouse( pControl) == false)
		return false;

	// Phase�� ���� �̺�Ʈ�� �˷��ش�.
	// �̰��� ToolTip�� �ݾ��ֱ� ���� ��) �κ��丮, ��
	UIPhaseSubBase * pSubPhase = GetMainFrame()->GetCurrentSubPhase();
	if( pSubPhase != nullptr)
	{
		pSubPhase->OnEnterMouse( pControl);
	}

	return true;
}

/*virtual*/ void UIFloatChat::OnMovePhase( UIPHASE phase)
{
	switch( phase)
	{
	case UIPHASE_LOBBY :		
		m_ChatArea = CHAT_AREA_LOBBY;	
		break;
	case UIPHASE_READY_ROOM :	
		m_ChatArea = CHAT_AREA_READYROOM; 
		break;
	case UIPHASE_CLANLOBBY :	
		UIMainFrame* pFrame = g_pFramework->GetUIMainframe();
		if( pFrame != nullptr)
		{
			if( ClanGameContext::i()->getMyMatchSquadInfo()->isJoinTeam() )
				m_ChatArea = CHAT_AREA_CLAN_MATCH; 
			else
				m_ChatArea = CHAT_AREA_LOBBY;	
		}
		break;
	}

	_SetMegaPhoneMainFrame(false);
}

/*virtual*/ void UIFloatChat::OnMoveSubPhase( UISUBPHASE phase )
{
	_SetMegaPhoneMainFrame(false);
}

/*virtual*/ void UIFloatChat::OnUpdate(REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate(rDeltaSeconds);

	m_pOutGameQACommandSet->ProcessCommand(rDeltaSeconds);
}

void UIFloatChat::ClickMegaPhoneOpenButton()
{
	{
		// �ֻ����� �׸����� �Ѵ�. 
		//( NEWANDHOT, USERLIST�� �̳༮�� �麸�带 �����ϹǷ� �׳༮�鵵 �ٽ� �ֻ�����...)
		i3GameNode * pParent;

		i3UIScene * pScene = GetScene(0);

		pParent = pScene->getParent();

		if( pParent != nullptr)
		{
			pScene->ChangeParent( pParent);
		}
		
	}
	
	_SetMegaPhoneMainFrame(!m_bMegaPhoneOpen);
	
	return;
}

bool UIFloatChat::OnRevive( i3RenderContext * pCtx)
{	
	if( UIFloatingBase::OnRevive( pCtx) == false)
		return false;
	
	return true;
}

//-------------------------------------------------------------------------------//
//                        Member Function (Protected)                            //
//-------------------------------------------------------------------------------//

void UIFloatChat::_SearchLanguage( void)
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

void UIFloatChat::_ResetLanguage( void)
{
	i3Ime * pIME = i3UI::getGlobalIme();

	if( pIME == nullptr)
		return;

	m_CurrentLanguage = 0;
	WPARAM param = INPUTLANGCHANGE_FORWARD;
	SendMessage( pIME->getHwnd(), WM_INPUTLANGCHANGEREQUEST, param, (LPARAM) m_LanguageList[m_CurrentLanguage]);
}

void UIFloatChat::_UpdateMacroChat(void)
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

void UIFloatChat::_SendMacroChat(INT32 nIdx)
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

void UIFloatChat::_UpdateLang(void)
{
#if defined ( LOCALE_NORTHAMERICA) || defined(LOCALE_SINGMAL)  || defined(LOCALE_PHILIPPINES) // �Ϲ�,�̸� ������
#else
	//// IME��忡 ���� ��Ʈ���� �����մϴ�.
	INT32 nLangIndex = 0;

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
			case LANG_KOREAN:	nLangIndex = 0; break;	// ��
			case LANG_ENGLISH:	nLangIndex = 1; break;	// ��
			}
		}
		break;
	case I3_LANG_CODE_THAI:
		{
			switch( nInputLang)
			{
			case LANG_THAI:		nLangIndex = 0; break;	// TH
			case LANG_ENGLISH:	nLangIndex = 1; break;	// EN
			}
		}
		break;
	case I3_LANG_CODE_JAPANESE:
		{
			switch( nInputLang)
			{
			case LANG_JAPANESE:	nLangIndex = 0; break;	// JP
			case LANG_ENGLISH:	nLangIndex = 1; break;	// EN
			}
		}
		break;
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:
	case I3_LANG_CODE_CHINESE_TRADITIONAL:
		{
			switch( nInputLang)
			{
			case LANG_CHINESE:	nLangIndex = 0; break;	// CH 
			case LANG_ENGLISH:	nLangIndex = 1; break;	// EN 
			}
		}
		break;
	case I3_LANG_CODE_RUSSIA:
		{
			switch( nInputLang)
			{
			case LANG_RUSSIAN:	nLangIndex = 0; break;	// RU 
			case LANG_ENGLISH:	nLangIndex = 1; break;	// EN 
			}
		}
		break;
	case I3_LANG_CODE_TURKISH:
		{
			switch( nInputLang)
			{
			case LANG_TURKISH:	nLangIndex = 0; break;	// TR
			case LANG_ENGLISH:	nLangIndex = 1; break;	// EN
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
						case SUBLANG_PORTUGUESE:			nLangIndex = 0; break;	// PR
						case SUBLANG_PORTUGUESE_BRAZILIAN:	nLangIndex = 1; break;	// BR
					}
				}
				break;
			case LANG_SPANISH:	nLangIndex = 0; break;	// SP
			case LANG_ITALIAN:	nLangIndex = 0; break;	// IT
			case LANG_ENGLISH:	nLangIndex = 0; break;	// EN
			}
		}
		break;
	case I3_LANG_CODE_ARABIC:
		{
			switch( nInputLang)
			{
			case LANG_ARABIC:	nLangIndex = 0; break;	// AR
			case LANG_ENGLISH:	nLangIndex = 1; break;	// EN
			}
		}
		break;
	case I3_LANG_CODE_VIETNAMESE:
		{
			switch( nInputLang)
			{
			case LANG_VIETNAMESE:	nLangIndex = 0; break;	// VN
			case  LANG_ENGLISH:		nLangIndex = 1; break;	// EN
			}
		}
		break;
	}

	LuaState * L = _CallLuaFunction( "Language");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L , nLangIndex);
		_EndLuaFunction( L, 1);
	}
#endif
}

void UIFloatChat::_UpdateParsingType( EDIT_STATE state)
{
	if( m_pChatBox->GetIME() == nullptr)
		return;
	
	INT32 textLength = i3::generic_string_size( m_pChatBox->getText());

	if( textLength == 0)
		return;

	CHAT_SYS_TYPE ChatType = CHAT_SYS_TYPE_NONE;

//	char szImeBuf[ MAX_STRING_COUNT];
//	i3mem::FillZero( szImeBuf, sizeof( szImeBuf));
//	i3String::CopyWCHARtoASCII( szImeBuf, sizeof(szImeBuf), m_pChatBox->getIMETextBuf(), textLength);


	i3::wstring wstrImeBuf = m_pChatBox->getIMETextBufString();			// ������ ���� �ʰ� �ǵ������� �����Ѵ�.(�޺κп� ������ ������ �ִ�..)  
																		// (2014.07.15.����)
	// ä�� Ÿ�Ժ� ä�� Color ����
	_SetChatColorFromType();

	// ChatBox Update (return Type CHAT_SYS_TYPE)
	ChatType = _UpdateChatBox(wstrImeBuf);

	// IME ����Ʈ �Է¹޴� �κ��� �޵�� ���¸� üũ�ؼ� ó���ؾ� �˴ϴ�. komet 09.06
	// IME�� ó���ϴ� Windows API Proc�� �޽��� ť�� ������� ó���ϱ� �����Դϴ�.
	if( state == EDIT_STATE_RETURN)
	{
		_SendChattingMsg(wstrImeBuf, ChatType);
	}
}

void UIFloatChat::_UpdateWhisperBackupBuffer(void)
{
	for( INT32  i = 0 ; i < g_pFramework->getChatBox()->GetWhisperBufferCount() ; i++)
	{
		INT32 index = (g_pFramework->getChatBox()->GetCurrentWhisperBufferIndex() + i) % 5;
		g_pFramework->getChatBox()->PutSystemChatting(g_pFramework->getChatBox()->GetWhisperBufferText(index), CHATTING_TYPE_WHISPER);
	}

	g_pFramework->getChatBox()->ResetWhisperBuffer();
}

void UIFloatChat::_UpdateChatType( void)
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

				m_bOnceTeamChat = true;
				g_pFramework->SetFocus( m_pChatBox);
			}
		}
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F2 )
	{
		if( m_nChatType != CHATTING_TYPE_ALL || m_bOnceTeamChat == false)
		{
			_ChangeChatType(CHATTING_TYPE_ALL, GAME_RCSTRING("STBL_IDX_CHATMESSAGE_MODE_ALL"), GameGUI::GetColor(GCT_DEFAULT));
		}

		g_pFramework->SetFocus( m_pChatBox);
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F3 )
	{
		if( m_ChatArea == CHAT_AREA_READYROOM)
		{
			if( m_nChatType != CHATTING_TYPE_TEAM || m_bOnceTeamChat == false)
			{
				_ChangeChatType(CHATTING_TYPE_TEAM, GAME_RCSTRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"), GameGUI::GetColor(GCT_TEAM_CHAT));
			}

			g_pFramework->SetFocus( m_pChatBox);
		}
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F4 )
	{
		if( m_nChatType != CHATTING_TYPE_CLAN || m_bOnceTeamChat == false)
		{
			_ChangeChatType(CHATTING_TYPE_CLAN, GAME_RCSTRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"), GameGUI::GetColor(GCT_CLAN_CHAT));
		}

		g_pFramework->SetFocus( m_pChatBox);
	}
}

void UIFloatChat::_ChangeChatType(INT32 nChatType, const i3::rc_wstring& wstrMsg, I3COLOR * pColor)
{
	switch( m_nChatType )
	{
	case CHATTING_TYPE_WHISPER	:
	case CHATTING_TYPE_REPLY	:
		{
			_ClearChatBox();
		}
		break;
	}

	m_bOnceTeamChat = false;

	m_nChatType = nChatType;

	g_pFramework->getChatBox()->PutSystemChatting(wstrMsg, CHATTING_TYPE_COMMENT);

	COLOR col;
	i3Color::Set(&col, pColor);
	m_pChatBox->SetTextColor(&col);
}

// �Ʒ��ڵ�� pszOut�� ũ�Ⱚ�� �����ϱ� ����ϹǷ�, �� ���ľ��Ѵ�.. (�ϴ��� i3::string�� ����μ��� ���°� ���ڴ�..)

void UIFloatChat::_CopyChatBufferToBox( i3::wstring& inout_wstr, INT32 bufferIdx)
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

void UIFloatChat::_CopyMegaPhoneBufferToBox( i3::wstring& inout_wstr, INT32 bufferIdx)
{
	CGameChatBox * pChatBox = g_pFramework->getChatBox();

	const i3::rc_wstring&  wstrChatBoxText = pChatBox->GetMegaPhoneBufferText( bufferIdx);

	if(!wstrChatBoxText.empty())
	{
		i3::stack_wstring wstr_StackChatBoxText = wstrChatBoxText;
		wstr_StackChatBoxText += L'\n';
		i3::insert_string(inout_wstr, 0, wstr_StackChatBoxText);
	}
}

void UIFloatChat::_ClearChatBox(void)
{
	m_nChatType = m_nOldChatType;
	g_WhisperLength = 0;

	m_pChatBox->SetText( L" ");
	m_pChatBox->GetIME()->SetInputText( L" ", 0);
	m_pChatBox->SetTextCaretPos( g_WhisperLength);
}

void UIFloatChat::_UpdateWhisperChat(const i3::rc_wstring& wstrTarget)
{
	i3::wstring wstrWhisper;
	_ToWhisperChatMessage(wstrWhisper, wstrTarget );

	g_WhisperLength = wstrWhisper.length(); 

	const i3::rc_wstring& wstrHistoryText = g_pFramework->getChatBox()->GetHistoryText();

	wstrWhisper.append(wstrHistoryText.begin(), wstrHistoryText.end());

	g_pFramework->SetFocus( nullptr);
	m_pChatBox->SetText( wstrWhisper );
	g_pFramework->SetFocus( m_pChatBox);
}

void UIFloatChat::_ClearMegaPhoneChatBox(const i3::rc_wstring& wstrTarget)
{
	if(	 i3::contain_string( m_pChatBox->getTextRange(), wstrTarget ) == -1 ) //	i3String::Contain( szContents, szWhisper) == -1)
	{
		_ClearChatBox();
	}
}

void UIFloatChat::_ClearWhisperChatBox(const i3::rc_wstring& wstrTarget)
{
	i3::wstring wstrWhisper;

	_ToWhisperChatMessage(wstrWhisper, wstrTarget);

	if(	 i3::contain_string( m_pChatBox->getTextRange(), wstrWhisper ) == -1 ) //	i3String::Contain( szContents, szWhisper) == -1)
	{
		_ClearChatBox();
	}
}

bool UIFloatChat::_SetWhisperChatToSend(i3::stack_wstring& out_only_chat, i3::stack_wstring& out_colored_chat, const i3::wliteral_range& wImeBufRng)
{
	out_colored_chat = L"{col:237, 86, 250, 255}";
	i3::generic_string_cat(out_colored_chat, wImeBufRng);
	i3::generic_string_cat(out_colored_chat, L"{/col}");

	_GetOnlyChatMessage(out_only_chat, wImeBufRng);

	m_nChatType = m_nOldChatType;

	return true;
}


void UIFloatChat::_SendWhisperChatting(const i3::rc_wstring& wstrTarget, const i3::wliteral_range& wMsgRng, const i3::wliteral_range& wImeBufRng, bool bReply)
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

void UIFloatChat::_SendMegaPhoneChatting(const i3::wliteral_range& wMsgRng)
{
	const i3::wliteral_range& strMsg = wMsgRng;

	UINT8 ui8Size = (UINT8)(strMsg.size()+1) * sizeof(TTCHAR);

	INT32 ItemMegaPhoneInfo = CASHITEM_ITEMID_MEGAPHONE;

	T_ItemDBIdx i64UID = CInvenList::i()->FindWareDBIdx_ByItemID(ItemMegaPhoneInfo);

	if( i64UID >= 0 )
		GameEventSender::i()->SetEvent( EVENT_SEND_MEGAPHONE, &i64UID ,strMsg.c_str(), &ui8Size);
	else
	{
		i3InputKeyboard * pKeyboard = g_pFramework->getKeyboard();
		if( pKeyboard->GetStrokeState(I3I_KEY_ENTER) == true) 
			pKeyboard->SetStrokeState(I3I_KEY_ENTER, false); 

		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_MEGAPHONE_USE_FAIL"));
	}
}

void UIFloatChat::_SetChatColorFromType(void)
{
	COLOR col;

	if( m_bOnceTeamChat)
	{
		i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
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
			break;
		case CHATTING_TYPE_TEAM:		// ��
			i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
			break;
		case CHATTING_TYPE_CLAN:		// Ŭ��
			i3Color::Set(&col, GameGUI::GetColor(GCT_CLAN_CHAT));
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

CHAT_SYS_TYPE UIFloatChat::_UpdateChatBox(const i3::wliteral_range& wImeBufRng)
{
	CHAT_SYS_TYPE eChatType = CHAT_SYS_TYPE_NONE;
//	char szTemp[ MAX_STRING_COUNT];
//	char szConv[ MAX_STRING_COUNT] = {0,}; 

	if( m_nChatType != CHATTING_TYPE_WHISPER && m_nChatType != CHATTING_TYPE_REPLY)
	{
		eChatType = g_pFramework->getChatBox()->ParsingText( wImeBufRng );
		{
			switch( eChatType)
			{
			case CHAT_SYS_TYPE_WHISPER:
				{
					if( !m_bUseHistory) m_nOldChatType = m_nChatType;

					m_nChatType = CHATTING_TYPE_WHISPER;

					_UpdateWhisperChat(g_pFramework->getChatBox()->GetChatTarget());

					OnWhisper(g_pFramework->getChatBox()->GetChatTarget());
				}
				break;
			case CHAT_SYS_TYPE_REPLY:
				{
					if( !m_bUseHistory) m_nOldChatType = m_nChatType;

					m_nChatType = CHATTING_TYPE_REPLY;

					if( i3::generic_string_size(g_pFramework->getChatBox()->GetChatReplyTarget()) > 0)
					{
						_UpdateWhisperChat(g_pFramework->getChatBox()->GetChatReplyTarget());
					}
					else
					{
						m_nChatType = m_nOldChatType;
						g_pFramework->SetFocus(nullptr);			// �� �ڵ尡 ���߸� �����۵��Ѵ�...�̰� ���������ε�..�ϴ� �̷��� ó�� ����..(13.02.25.����)
						m_pChatBox->SetText( L"");				// UIHUDIngameChat������ i3UI::getGlobalIme()->SetInputText( L"", 0);�� ���� �����Ǿ� �ִ� ������.
						g_pFramework->SetFocus( m_pChatBox);	// ���������� �������� �����Ǿ�� �Ѵ�..
					}
				}
				break;
			case CHAT_SYS_TYPE_ONETIME_TEAM:	// ��ȸ�� �� ê��!!
				if( m_ChatArea == CHAT_AREA_READYROOM)
				{
					m_bOnceTeamChat = true;
					m_pChatBox->SetText( L"");
					m_pChatBox->SetIMEText(L"");
					g_pFramework->SetFocus( m_pChatBox);
				}
				break;
			case CHAT_SYS_TYPE_MEGAPHONE:
				{
					if( !m_bUseHistory) m_nOldChatType = m_nChatType;
					
					m_nChatType = CHATTING_TYPE_MEGAPHONE;

					g_pFramework->SetFocus( nullptr);
			
					i3::wstring wstrMegaPhone;
					i3::sprintf(wstrMegaPhone, L"%s ", GAME_RCSTRING("STR_Megaphone_Input"));			

					m_pChatBox->SetText(wstrMegaPhone);
					m_pChatBox->SetTextCaretPos( wstrMegaPhone.length() );
					
					g_pFramework->SetFocus( m_pChatBox);

					m_pChatBox->SetTextColor( GameGUI::GetColor(GCT_MEGAPHONE_CHAT) );
				}
				break;
			}
		}
	}

	if( m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY || m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_MEGAPHONE )
	{
	//	i3String::CopyWCHARtoASCII( szConv, sizeof( szConv), m_pChatBox->getText(), i3::generic_string_size( m_pChatBox->getText())); 
		i3::string strConv;
		i3::utf16_to_mb(m_pChatBox->getText(), strConv);
		i3::string strTemp;

		// GameGUI::RemoveColorKeyword( szConv, szTemp, sizeof( szTemp));
		GameUI::RemoveColorKeywordCopy(strConv, strTemp);

	//	INT32 len = i3::generic_string_size( szTemp);

		if( INT32(strTemp.size())  < g_WhisperLength)
		{
			_ClearChatBox();
		}

		if( m_nChatType == CHATTING_TYPE_WHISPER)
		{
			_ClearWhisperChatBox( g_pFramework->getChatBox()->GetChatTarget());
		}
		else if( m_nChatType == CHATTING_TYPE_REPLY)
		{
			_ClearWhisperChatBox( g_pFramework->getChatBox()->GetChatReplyTarget());
		}else if( m_nChatType == CHATTING_TYPE_MEGAPHONE )
		{
			i3::wstring wstrMegaPhone;
			i3::sprintf(wstrMegaPhone, L"%s ", GAME_RCSTRING("STR_Megaphone_Input"));
			_ClearMegaPhoneChatBox( wstrMegaPhone );
		}
	}

	return eChatType;
}

void UIFloatChat::_SendChattingMsg(i3::wstring& inout_wstrImeBuf, CHAT_SYS_TYPE eChatType)
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

	// GM Command �ϰ�쿡�� ���� 
	if(_CheckGMCommand(inout_wstrImeBuf)) return;

	// Medal Command �ϰ�쿡�� ���� 
	if(true == _CheckMedalCommand(inout_wstrImeBuf)) return;

	if(true == m_pOutGameQACommandSet->CheckRandomBoxCommand(inout_wstrImeBuf)) 
	{
		g_pFramework->getChatBox()->PutSystemChatting(L"{col:255,120,0,255}RandomBox Test Enabled.{/col}");
		g_pFramework->getChatBox()->PutSystemChatting(L"{col:255,120,0,255}Please wait...{/col}");
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
		// ��Ʈ��ũ�� �޽����� �����ϴ�.

		i3::stack_wstring wstrTemp;
		i3::stack_wstring wstrTemp2;

		CHAT_INFO info;
		info._Type = (UINT16)m_nChatType;

		if( m_bOnceTeamChat)
		{
			wstrTemp = inout_wstrImeBuf; 
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
					bWhisper = 	_SetWhisperChatToSend(wstrTemp, wstrTemp2, inout_wstrImeBuf);

					if ( (INT32)inout_wstrImeBuf.size() > g_WhisperLength)
					{
						nWhsiperTextCount = inout_wstrImeBuf.size() - g_WhisperLength;
					}
				}
				break;
			case CHATTING_TYPE_REPLY:// �亯
				{
					bReply = 	_SetWhisperChatToSend(wstrTemp, wstrTemp2, inout_wstrImeBuf);

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
		else if( bMegaphone )
		{
			if( nMegaphoneTextCount > 0)
			{
				m_nChatType = m_nOldChatType;
				wstrTemp.erase(0, GAME_RCSTRING("STR_Megaphone_Input").size() + 1);
				_SendMegaPhoneChatting( wstrTemp );
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
					// :ä�� �Է�
					GameEventSender::i()->SetEvent(EVENT_CHATTING_N , &info );
					g_pFramework->getChatBox()->PutHistory(inout_wstrImeBuf, m_nChatType);
				}
			}
			else
			{
				if( info._Type == CHATTING_TYPE_TEAM)
				{

					// :ä�� �Է�
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
						// :ä�� �Է�
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

	m_bOnceTeamChat = false;

	if( m_bUseHistory)
	{
		m_nChatType = m_nOldChatType;
		m_bUseHistory = false;
	}
}

bool UIFloatChat::_CheckGMCommand(const i3::wliteral_range& wImeBufRng)
{
	if( !UserInfoContext::i()->CheckAuthLevel() ) return false;

	// GM�� �� ��ɾ� üũ.
	if ( m_ChatArea == CHAT_AREA_READYROOM  )
	{
		if ( CGameMaster::i()->SetGMCommand(wImeBufRng) )
			m_pChatBox->SetText( L"");
	}
	else if ( m_ChatArea == CHAT_AREA_LOBBY )
	{
		// GM�� �κ� ��ɾ� üũ.
		if ( CGameMaster::i()->SetGMLobbyCommand(wImeBufRng) )
			m_pChatBox->SetText( L"");
	}

	// GM ��ɾ�� ä�ø޽����� �Ѹ��� �ʽ��ϴ�.
	// revision 44121
	if ( GM_COMMAND_UNKNOWN != CGameMaster::i()->GetCommandType(wImeBufRng) )
	{
		m_pChatBox->SetText( L"");
		return true;
	}

	return false;
}


//#define		COMMAND_MEDAL_COUNT			"/MEDALC"	// �޴� ī��Ʈ ���� Ŀ�ǵ�. ex : /MEDALC MEDAL_TYPE MedalIdx MedalCount
//#define		COMMAND_MEDAL_SET_CHANGE	"/MEDALS"	// ��Ʈ �޴� ��ü.			ex : /MEDALS MEDAL_SET_TYPE SetMedalIdx

namespace
{
	const wchar_t*  COMMAND_MEDAL_COUNTW =	L"/MEDALC";					// TODO : Common�� �ڵ�� ���ڰ��� ��ġ���Ѿ� �Ѵ�. ( 2014.07.15.����)
	const wchar_t*  COMMAND_MEDAL_SET_CHANGEW = L"/MEDALS";
}

bool UIFloatChat::_CheckMedalCommand(const i3::wliteral_range& wImeBufRng)
{
	if( !UserInfoContext::i()->CheckSpecialAbility(SA_CHEAT) ) return false;

	if ( i3::generic_string_size(wImeBufRng) <= 0 || i3::generic_string_size(wImeBufRng) >= MAX_PATH )
		return false;

	MEDAL_COMMAND	stMedalCommand;	
	i3mem::FillZero( &stMedalCommand, sizeof( MEDAL_COMMAND) );

	MEDAL_COMMAND_TYPE eType = MEDAL_COMMAND_TYPE_UNKNOWN;

	if ( i3::icontain_string_head( wImeBufRng, COMMAND_MEDAL_COUNTW ) ) 
		eType = MEDAL_COMMAND_TYPE_COUNT;
	else if ( i3::icontain_string_head( wImeBufRng, COMMAND_MEDAL_SET_CHANGEW ) ) 
		eType = MEDAL_COMMAND_TYPE_CHANGE_MEDALSET;
	else return false;

	// �Է°�
	// �޴� ī��Ʈ ���� - ��) /MEDALC �޴�Ÿ�� �޴��ε��� �޴�����Ƚ��
	// �޴� ��Ʈ ��ü - ��) /MEDALS �޴޼�ƮŸ�� �޴޼�Ʈ�ε��� 1(�ǹ̾��°� �ƹ��ų�)

	UINT8 ui8Ordering = (UINT8)MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_COMMAND;
	INT32 iCount = 0; 
	// ������ �Է��� Arg ����
	UINT8 ui8MedalType	= 0; 
	UINT16 ui16MedalIdx	= 0; 
	UINT32 ui32MedalCount = 0; 

	// �� Arg������ ��Ʈ������
	UINT8	ui8MedalType_S	= 0; 
	UINT8	ui8MedalIdx_S	= 0; 
	UINT8	ui8MedalCount_S = 0; 

	wchar_t wstrMedalType[MAX_PATH]; 
	wchar_t wstrMedalIdx[MAX_PATH]; 
	wchar_t wstrMedalCount[MAX_PATH]; 

	bool bLoop = true;

	while(wImeBufRng[iCount] != 0)
	{
		if( false == bLoop ) break;

		switch( ui8Ordering ) 
		{
		case MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_MEDAL_TYPE:
			wstrMedalType[ui8MedalType_S] = wImeBufRng[iCount];
			ui8MedalType_S++;
			break;
		case MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_MEDAL_IDX:
			wstrMedalIdx[ui8MedalIdx_S] = wImeBufRng[iCount];
			ui8MedalIdx_S++;
			break;
		case MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_MEDAL_COUNT:
			wstrMedalCount[ui8MedalCount_S] = wImeBufRng[iCount];
			ui8MedalCount_S++;
			break;

		case MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_MAX:
			bLoop = false;
			break;
		}

		// �����̽� �Ǵ� ���� ������ ���� Argã����
		if( wImeBufRng[iCount] == L' ' || wImeBufRng[iCount] == L'\t' ) ui8Ordering++;	
		iCount++;
	}
	
	I3ASSERT( ui8MedalType_S > 1);
	wstrMedalType[ui8MedalType_S-1] = 0;
	I3ASSERT( ui8MedalIdx_S > 1);
	wstrMedalIdx[ui8MedalIdx_S-1] = 0;
	wstrMedalCount[ui8MedalCount_S] = 0;

	INT32 temp = i3::atoi( wstrMedalType );
	I3_BOUNDCHK( temp, 256);
	ui8MedalType = static_cast<UINT8>(temp);

	temp = i3::atoi( wstrMedalIdx );
	I3_BOUNDCHK( temp, 65536);
	ui16MedalIdx = static_cast<UINT16>(temp);

	temp = i3::atoi( wstrMedalCount );
	I3ASSERT( temp >= 0);
	ui32MedalCount = temp;

	stMedalCommand.m_ui8CommandType = static_cast<UINT8>(eType);
	stMedalCommand.m_ui8MedalType = ui8MedalType;
	stMedalCommand.m_ui16Idx = ui16MedalIdx;
	I3ASSERT( ui32MedalCount < 65536);
	stMedalCommand.m_ui16Count = static_cast<UINT16>(ui32MedalCount);

	GameEventSender::i()->SetEvent(EVENT_MEDAL_COMMAND, &stMedalCommand );
	m_pChatBox->SetText( L"");

	return true;
}

void UIFloatChat::_ChatHistory(EDIT_STATE state)
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

void UIFloatChat::_ToWhisperChatMessage(i3::wstring& out, const i3::rc_wstring& target, const i3::wliteral_range& msg)
{
	i3::sprintf(out, L"[To:%s] %s", target, msg );			
}

void UIFloatChat::_GetOnlyChatMessage(i3::stack_wstring& out,	const i3::wliteral_range& wfullMsgRng)
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

void UIFloatChat::_SetMegaPhoneMainFrame(bool bEnable)
{
	m_bMegaPhoneOpen = bEnable;
	LuaState * L = _CallLuaFunction("SetMegaPhoneFrameEnable");
	if(L != nullptr)
	{
		i3Lua::PushBoolean(L, bEnable);
		_EndLuaFunction(L , 1);
	}
	
	int temp_open_close = MEGAPHONE_FRAME_CLOSE;
	if(bEnable)
		temp_open_close = MEGAPHONE_FRAME_OPEN;
}

//-------------------------------------------------------------------------------//
//                          Member Function (public)                             //
//-------------------------------------------------------------------------------//

void UIFloatChat::OnWhisper( INT32 slotIdx)
{
	i3::rc_wstring wstrNick = BattleSlotContext::i()->getNickForSlot(slotIdx);

	OnWhisper(wstrNick);
}

void UIFloatChat::OnWhisper( const i3::rc_wstring& wstrNick)
{
	g_pFramework->SetFocus( nullptr);

	// ä��â�� �ӼӸ� Ŀ��� �Է�
	i3::wstring wstrWhisper;
	_ToWhisperChatMessage(wstrWhisper, wstrNick );
	
	m_pChatBox->SetText(wstrWhisper);
	g_pFramework->SetFocus( m_pChatBox);

	m_bWhisperMode = true;
	m_nChatType = CHATTING_TYPE_WHISPER;
	g_WhisperLength = wstrWhisper.length();	// i3::generic_string_size( szWhisper );
	g_pFramework->getChatBox()->SetChatTarget(wstrNick);

	m_pChatBox->SetTextColor( GameGUI::GetColor(GCT_WISPER_CHAT) );
	m_pChatBox->SetTextCaretPos( g_WhisperLength);
}

void UIFloatChat::UpdateChatBuffer(void)
{

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
		i3::wstring wstrTotalText;

		const INT32 numBufferCount = pFrameWorkChatBox->GetBufferCount();

		for( INT32 i = BufferIdx; nLoopCount < numBufferCount ; i--)
		{
			nAccumCount++;

			if( i<0)
			{
				i = MAX_CHAT_BUFFER-1;
			}

			// ä�� �α׿� ��µǴ� �޼����� 100���� ����
			// ������ ¥���� �� ���� �ȳ��ð� ����.
			if (nLoopCount < 0 || nLoopCount >= 100)
				break;

			// ä�� ���� 1.5���� ���ŵǾ�, ��ü ��Ʈ���� �ѷ��ݴϴ�.
			_CopyChatBufferToBox( wstrTotalText, i);

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

void UIFloatChat::UpdateMegaPhoneBuffer(void)
{
	INT32 i;
	
	if( g_pFramework->getChatBox()->GetMegaPhoneBufferCount() <= 0 )
		return;

	if( m_pMegaPhoneMain_EditBox != nullptr)
	{
		CGameChatBox * pFrameWorkChatBox = g_pFramework->getChatBox();

		INT32 BufferIdx= 0;
		if( pFrameWorkChatBox->GetMegaPhoneBufferIndex() == 0)
		{
			BufferIdx = MAX_MEGAPHONE_BUFFER-1;
		}
		else
		{
			BufferIdx = pFrameWorkChatBox->GetMegaPhoneBufferIndex() - 1;
		}

		// Buffer�� ���鼭 ����� ���ڸ� ����ϴ�.
		INT32 nAccumCount = 0;
		INT32 nLoopCount = 0;
		i3::wstring wstrTotalText;

		for( i = BufferIdx; nLoopCount < pFrameWorkChatBox->GetMegaPhoneBufferCount(); i--)
		{
			nAccumCount++;

			if( i<0)
			{
				i = MAX_MEGAPHONE_BUFFER-1;
			}

			_CopyMegaPhoneBufferToBox( wstrTotalText, i);

			// ä�� �α׿� ��µǴ� �޼����� 300���� ����
			if(nLoopCount < 0 || nLoopCount >= MAX_MEGAPHONE_BUFFER )
				break;

			nLoopCount++;
		}
		
		// �ް��� ���� �����͹ڽ��� ����մϴ�.
		const i3::rc_wstring&  wstrChatBoxText = pFrameWorkChatBox->GetMegaPhoneBufferText( BufferIdx );
		if(!wstrChatBoxText.empty())
		{
			if(m_pMegaPhone_EditBox != nullptr)
				m_pMegaPhone_EditBox->SetText(wstrChatBoxText);
		}

		// �ް��� ���� �����͹ڽ��� ����մϴ�.
		m_pMegaPhoneMain_EditBox->SetText( wstrTotalText);
	}
}

void	UIFloatChat::SetFocus_ChatEditBox()
{
	LuaState* L = _CallLuaFunction( "StartFocus");
	if( L != nullptr)
		_EndLuaFunction(L, 0);
}

void UIFloatChat::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIFloatChat::ReceiveGameEvent_Change_ReadyroomInfo( INT32 arg, const i3::user_data&)
{
	// Ȯ�� �ʿ�
	//_SetRoomTitle();
}