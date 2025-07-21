#include "pch.h"
#include "GameChat_Body.h"
#include "GlobalVariables.h"
#include "GuiNotifyReceiver.h"
#include "GameMaster.h"
#include "CommunityContext.h"

static void *		s_pPointer = NULL;
static INT32 g_WhisperLength = 0;

I3_CLASS_INSTANCE( CGameChatBody);//, i3GameObjBase);

CGameChatBody::CGameChatBody(void)
{
	m_pChatList = NULL;
	m_pChatCombo = NULL;
	m_pChatButton = NULL;
	m_pChatPopup = NULL;
	m_pChatBox = NULL;
	m_pChatLang = NULL;
	m_pTextBox = NULL;
	i3mem::FillZero(m_pChatChannelButton, sizeof(m_pChatChannelButton));

	m_nChatType = CHATTING_TYPE_ALL;
	m_nOldChatType = CHATTING_TYPE_ALL;
	m_nMaxChatLineCount = 0;
	m_ChatBodyState = CHAT_BODY_STATE_LOBBY;
	m_bOnceChatType = FALSE;
	m_bOnceChatTypeAll = FALSE;
	m_nCurrentChatBufferType = CHATTING_TYPE_ALL;
	m_nCurrentSelectedChatType = 0;
}

CGameChatBody::~CGameChatBody(void)
{

}

void  CGameChatBody::InitChatBody( CHAT_BODY_STATE state, INT32 nMaxLineCount)
{
	m_ChatBodyState = state;
	m_nMaxChatLineCount = nMaxLineCount;

	_InitChat( state);
}

void  CGameChatBody::OnUpdate( REAL32 rDeltaSeconds, void* pPointers)
{
#if ! defined(DEV_BUILD)
	// �� �񱳹� ������ Step�� Phase �Դٰ��� �ϸ鼭 ä�ÿ� ������ �߻��մϴ�.
	// �ش� Step �Ǵ� Phase���� �ڽ��� this �����͸� �Ѱ��ִµ�
	// Step�� ���Ǿ�� �ϴµ� Step -> Phase�� ����Ǳ� ���������
	// ������ �߻��մϴ�.
	// �׷��� �� �ڵ尡 �� �ִ��� �𸣰� ����Ʈ ����Ʈ�� ������ ������.
	// Dev ���忡�� �׽�Ʈ �ϰ� ���쵵�� �ϰڽ��ϴ�.
	//
	// �� �񱳹� ������ Ŭ�������� ��ġ����ŷ �� Ŭ�� �ӿ��� ä�� �ȵǴ� ������ �߻��մϴ�.
	//
	if( s_pPointer != pPointers)
		return;
#endif

	if( g_pFramework->getChatBox()->GetBackUpBufferCount() < m_nMaxChatLineCount)
	{
		m_pChatList->SetSliderPos(100.0f);
		m_pChatList->setInputDisable( TRUE);
	}
	else
	{
		m_pChatList->setInputDisable( FALSE);
	}

	// ��ũ��
	_UpdateMacroChat();

	// �Է� ��� ����
	_UpdateLang();

	// ����
	_UpdateParsingType();

	// ���ۿ� ���?
	_UpdateChatBuffer();

	// ä�� ä�� ������Ʈ
	_UpdateChatChannelType();
}

void CGameChatBody::UpdateChatFocus(void)
{
	// Enter�� ������ ��� ä���Է�â�� Focus���°� �ƴ϶��, Focus���·� �մϴ�.
	if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER))
	{
		if( !m_pChatBox->getIsFocus())
		{
			m_pChatBox->SetFocus( TRUE);
		}
	}
}

void CGameChatBody::UpdateChatType(void)
{
	if( g_pGUIRoot->getFocusControl() != m_pChatBox) return;

	// ä�� ���¼���.
	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ))
	{
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
		{
			if( m_ChatBodyState == CHAT_BODY_STATE_READYROOM)
			{
				if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
				{
					m_pChatBox->SetIMEText("");
				}

				m_bOnceChatType = TRUE;
				m_pChatBox->SetFocus( TRUE);
			}
		}
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F2 )
	{
		if( m_nChatType != CHATTING_TYPE_ALL || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetIMEText("");
			}

			m_bOnceChatType = FALSE;
			m_bOnceChatTypeAll = FALSE;
			m_nChatType = CHATTING_TYPE_ALL;
			g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_ALL"));
		}
		m_pChatBox->SetFocus( TRUE);
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F3 )
	{
		if( m_ChatBodyState == CHAT_BODY_STATE_READYROOM)
		{
			if( m_nChatType != CHATTING_TYPE_TEAM || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
			{
				if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
				{
					m_pChatBox->SetIMEText("");
				}

				m_bOnceChatType = FALSE;
				m_bOnceChatTypeAll = FALSE;
				m_nChatType = CHATTING_TYPE_TEAM;
				g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"));
			}
			m_pChatBox->SetFocus( TRUE);
		}
	}
	else if( g_pFramework->getKeyStroke() & GAME_KEY_FUNC_F4 )
	{
		if( m_nChatType != CHATTING_TYPE_CLAN || m_bOnceChatType == TRUE || m_bOnceChatTypeAll == TRUE)
		{
			if(m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
			{
				m_pChatBox->SetIMEText("");
			}

			m_bOnceChatType = FALSE;
			m_bOnceChatTypeAll = FALSE;
			m_nChatType = CHATTING_TYPE_CLAN;
			g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"));
		}
		m_pChatBox->SetFocus( TRUE);
	}
}

void CGameChatBody::SetEntranceStart(void * pPointer)
{
	s_pPointer = pPointer;

	m_pTextBox->SetText("");
	m_pChatBox->SetIMEText("");
	m_pChatBox->SetIMEMode( IME_MODE_NATIVE);
	m_pChatList->Clear();

	// ä�� ���� �ʱ�ȭ
	g_pFramework->getChatBox()->ResetChatBuffer();

	// Update Whisper Backup Buffer
	_UpdateWhisperBackupBuffer();

	// ä��ä�� �ʱ�ȭ
	OnChatChannel(0);
}

void CGameChatBody::OnExitEnd(void)
{
	s_pPointer = NULL;

	m_nChatType = CHATTING_TYPE_ALL;

	// ä�� ���� �ʱ�ȭ
	g_pFramework->getChatBox()->ResetChatBuffer();
}

void CGameChatBody::OnWhisper( INT32 slotIdx)
{
	const char * pszNick = g_pGameContext->getNickForSlot(slotIdx);

	OnWhisper(pszNick);
}

void CGameChatBody::OnWhisper( const char * pszNick)
{
	char whisperCommand[MAX_STRING_COUNT] = "";

	// ä��â�� �ӼӸ� Ŀ��� �Է�
	i3String::Copy( whisperCommand, "[To:", MAX_STRING_COUNT );
	i3String::Concat( whisperCommand, pszNick);
	i3String::Concat( whisperCommand, "] ");

	m_pChatBox->SetIMEText(whisperCommand);
	m_pChatBox->SetFocus(TRUE);

	m_nChatType = CHATTING_TYPE_WHISPER;
	g_WhisperLength = i3String::Length( whisperCommand );
	g_pFramework->getChatBox()->SetChatTarget(pszNick);

	m_pChatBox->SetTextColor( GameGUI::GetColor(GCT_WISPER_CHAT) );
}

void CGameChatBody::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for(INT32 i = 0; i < scrollby; i++)
		{
			m_pChatList->MovePrev();
		}
	}
	else
	{
		for(INT32 i = 0; i > scrollby; i--)
		{
			m_pChatList->MoveNext();
		}
	}

	g_pFramework->getChatBox()->CalcBackupBufferByRatio(m_pChatList->getCurValue()*0.01f, m_nMaxChatLineCount);
}

void CGameChatBody::OnDragChatList(void)
{
	g_pFramework->getChatBox()->CalcBackupBufferByRatio(m_pChatList->getCurValue()*0.01f, m_nMaxChatLineCount);
}

void CGameChatBody::OnChatPopup(UINT32 idxItem)
{
	if( m_ChatBodyState == CHAT_BODY_STATE_READYROOM)
	{
		m_bOnceChatType = FALSE;
		m_bOnceChatTypeAll = FALSE;

		if( idxItem == 0) m_nChatType = CHATTING_TYPE_ALL;
		if( idxItem == 1) m_nChatType = CHATTING_TYPE_TEAM;
		if( idxItem == 2) m_nChatType = CHATTING_TYPE_CLAN;
	}
	else if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY)
	{
		m_bOnceChatType = FALSE;
		m_bOnceChatTypeAll = FALSE;

		if( idxItem == 0) m_nChatType = CHATTING_TYPE_ALL;
		if( idxItem == 1) m_nChatType = CHATTING_TYPE_CLAN;
	}
	else if( m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH)
	{
		m_bOnceChatType = FALSE;
		m_bOnceChatTypeAll = FALSE;

		if( idxItem == 0) m_nChatType = CHATTING_TYPE_ALL;
		if( idxItem == 1) m_nChatType = CHATTING_TYPE_CLAN;
	}
	else if( m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
	{
		m_bOnceChatType = FALSE;
		m_bOnceChatTypeAll = FALSE;

		if( idxItem == 0) m_nChatType = CHATTING_TYPE_ALL;
		if( idxItem == 1) m_nChatType = CHATTING_TYPE_CLAN;
	}

	if( m_nChatType == CHATTING_TYPE_ALL)
	{
		g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_ALL"));
	}
	else if( m_nChatType == CHATTING_TYPE_TEAM)
	{
		g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_TEAM"));
	}
	else if( m_nChatType == CHATTING_TYPE_CLAN)
	{
		g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STBL_IDX_CHATMESSAGE_MODE_CLAN"));
	}
}

void CGameChatBody::OnChatChannel(UINT32 idxItem)
{
	m_nCurrentSelectedChatType = idxItem;

	switch( idxItem)
	{
	case 0:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				m_nCurrentChatBufferType = CHATTING_TYPE_LOBBY;
				break;
			case CHAT_BODY_STATE_READYROOM:
				m_nCurrentChatBufferType = CHATTING_TYPE_ALL;
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				m_nCurrentChatBufferType = CHATTING_TYPE_MATCH;
				break;
			case CHAT_BODY_STATE_CLAN_MEMBER:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN_MEMBER_PAGE;
				break;
			}
		}
		break;
	case 1:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN;
				break;
			case CHAT_BODY_STATE_READYROOM:
				m_nCurrentChatBufferType = CHATTING_TYPE_TEAM;
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN;
				break;
			case CHAT_BODY_STATE_CLAN_MEMBER:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN;
				break;
			}
		}
		break;
	case 2:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				m_nCurrentChatBufferType = CHATTING_TYPE_WHISPER;
				break;
			case CHAT_BODY_STATE_READYROOM:
				m_nCurrentChatBufferType = CHATTING_TYPE_CLAN;
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				m_nCurrentChatBufferType = CHATTING_TYPE_WHISPER;
				break;
			case CHAT_BODY_STATE_CLAN_MEMBER:
				m_nCurrentChatBufferType = CHATTING_TYPE_WHISPER;
				break;
			}
		}
		break;
	case 3:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				break;
			case CHAT_BODY_STATE_READYROOM:
				m_nCurrentChatBufferType = CHATTING_TYPE_WHISPER;
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				break;
			}
		}
		break;
	default:
		{
			switch( m_ChatBodyState)
			{
			case CHAT_BODY_STATE_LOBBY:
				break;
			case CHAT_BODY_STATE_READYROOM:
				break;
			case CHAT_BODY_STATE_CLAN_MATCH:
				break;
			}
		}
		break;
	}
}

INT64 CGameChatBody::FindUserUIDByNick(char* nick)
{
	char szTemp[512];
	// �ڽ��� ���´�.
	if ( g_pGameContext->IsMyNickName(nick) )
	{
		i3String::Copy( szTemp, GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"), 512 );//�ڽ��� �г����� �Է��ϼ̽��ϴ�.
		g_pFramework->getChatBox()->PutSystemChatting(szTemp);
		return 0;
	}

	// ģ�� ��Ͽ� �ִ��� Ȯ���Ѵ�.
	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID ����
			return node->_uid;
		}
	}

	// Ŭ�� ��Ͽ� �ִ��� Ȯ���Ѵ�.
	for(UINT32 i = 0; i < g_pCommunity->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(i);

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID ����
			return node->_uid;
		}
	}

	return 0;
}


//Protect
void CGameChatBody::_InitChat(CHAT_BODY_STATE state)
{
	m_pChatList->CreateTextEx(m_nMaxChatLineCount,  GetDefaultFontName(), FONT_COUNT_256);
	m_pChatList->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pChatList->SetSelColor(0, 0, 0, 0);
	m_pChatList->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));

	for(INT32 i = 0; i < m_nMaxChatLineCount; i++)
	{
		m_pChatList->AddElement("");
	}
	m_pTextBox->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_2048, GAME_FONT_CHAT_SIZE, FALSE, NULL, FW_NORMAL, TRUE);	

	m_pTextBox->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));	
	m_pTextBox->SetTextAutoWrap( TRUE);
	m_pTextBox->SetTextAlign( ALIGN_LEFT, ALIGN_BOTTOM);
	m_pTextBox->SetWordLineFeed( FALSE);
	m_pTextBox->SetOutCharaProcess( TRUE);
#if defined(GUI_ADJUST_RUSSIA)
	m_pTextBox->SetOffsetPos(3, 0);
#endif

	REAL32 ry = i3GuiRoot::getViewer()->GetViewHeight() * 0.13020833333333333333333333333333e-2f; // / I3GUI_RESOLUTION_HEIGHT;	
	m_pTextBox->SetTextSpace( 0, (INT32)(5 * ry));

	m_pChatLang->CreateCaption( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_CHAT_SIZE);

	switch(GetCodePage())
	{
	case I3_LANG_CODE_KOREAN:
		m_pChatLang->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_CHAT_TARGET_KOREAN"));/*��*/
		break;
	default :
		m_pChatLang->SetCaption("");
		break;
	}	
	m_pChatLang->SetCaptionColor(GameGUI::GetColor(GCT_FOCUS));

	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = ROOM_CHAT_SIZE;
	ime._StyleIME = 0;

	m_pChatBox->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_CHAT_SIZE, TRUE, &ime, FW_NORMAL, TRUE);

	m_pChatBox->SetTextAlign(ALIGN_LEFT, ALIGN_TOP);
	m_pChatBox->SetTextAutoWrap(FALSE);
	m_pChatBox->SetOffsetPos(0.0f, 2.0f);
	m_pChatBox->setCaretPosOffset(0.0f, 2.0f);	
	m_pChatBox->SetIMEMode( IME_MODE_NATIVE);
	m_pChatBox->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));

	g_pFramework->getChatBox()->ResetChatBuffer();
	m_pChatBox->SetIMEText("");

	m_pChatCombo->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pChatCombo->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));
	m_pChatCombo->SetOffsetPos(2, 0);

	INIT_COMBO_EX(m_pChatButton);	

	INIT_COMBO_EX( m_pChatChannelButton[0]);
	INIT_COMBO_EX( m_pChatChannelButton[1]);
	INIT_COMBO_EX( m_pChatChannelButton[2]);
	INIT_COMBO_EX( m_pChatChannelButton[3]);

	for(INT32 i=0; i<4; i++)
	{
		m_pChatChannelButton[i]->CreateCaption(  GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE);
		m_pChatChannelButton[i]->SetEnable( FALSE);
	}

	// �� type���� �ʱ� ����ġ�� �����մϴ�.
	switch(state)
	{
	case CHAT_BODY_STATE_LOBBY:
		m_pChatCombo->SetText(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*Ŭ��*/
		m_pChatChannelButton[0]->SetEnable( TRUE);
		m_pChatChannelButton[1]->SetEnable( TRUE);
		m_pChatChannelButton[2]->SetEnable( TRUE);
		m_pChatChannelButton[0]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatChannelButton[1]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*Ŭ��*/
		m_pChatChannelButton[2]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_CHANNEL_WHISPER"));/*�ӼӸ�*/
		break;
	case CHAT_BODY_STATE_READYROOM:
		m_pChatCombo->SetText(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatPopup->CreateTextEx(3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_TEAM"));/*��*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*Ŭ��*/
		m_pChatChannelButton[0]->SetEnable( TRUE);
		m_pChatChannelButton[1]->SetEnable( TRUE);
		m_pChatChannelButton[2]->SetEnable( TRUE);
		m_pChatChannelButton[3]->SetEnable( TRUE);
		m_pChatChannelButton[0]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatChannelButton[1]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_TEAM"));/*��*/
		m_pChatChannelButton[2]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*Ŭ��*/
		m_pChatChannelButton[3]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_WHISPER_TEXT"));/*�Ӹ�*/
		break;
	case CHAT_BODY_STATE_CLAN_MATCH:
		m_pChatCombo->SetText(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*Ŭ��*/
		m_pChatChannelButton[0]->SetEnable( TRUE);
		m_pChatChannelButton[1]->SetEnable( TRUE);
		m_pChatChannelButton[2]->SetEnable( TRUE);
		m_pChatChannelButton[0]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatChannelButton[1]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*Ŭ��*/
		m_pChatChannelButton[2]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_CHANNEL_WHISPER"));/*�ӼӸ�*/
		break;
	case CHAT_BODY_STATE_CLAN_MEMBER:
		m_pChatCombo->SetText(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatPopup->AddElement(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*Ŭ��*/
		m_pChatChannelButton[0]->SetEnable( TRUE);
		m_pChatChannelButton[1]->SetEnable( TRUE);
		m_pChatChannelButton[2]->SetEnable( TRUE);
		m_pChatChannelButton[0]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_ALL"));/*��ü*/
		m_pChatChannelButton[1]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_POPUP_CLAN"));/*Ŭ��*/
		m_pChatChannelButton[2]->SetCaption(GAME_STRING("STR_TBL_CHAT_INGAME_CHANNEL_WHISPER"));/*�ӼӸ�*/
		break;
	}

	m_pChatPopup->SetSelColor(100, 100, 100, 100);
	m_pChatPopup->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));
	m_pChatPopup->SetExclusiveEnableControl(TRUE);
	m_pChatPopup->SetEnable(FALSE);
}

void CGameChatBody::_UpdateMacroChat(void)
{
	// Macro�� �̿��� �Է�ó��
	CHAT_INFO info;

	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ))
	{
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_1 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[0]) > 0)
			{	
				if( g_pEnvSet->m_bMacroEnableFriend[0])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				// �弳����
				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[0], 1))
				{

					if( g_pEnvSet->m_bMacroEnableFriend[0])	info._Type = CHATTING_TYPE_TEAM;	// ��
					else									info._Type = CHATTING_TYPE_ALL;		// ��ü

					info.setString( g_pEnvSet->m_szMacro[0]);

					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_2 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[1]) > 0)
			{
				if( g_pEnvSet->m_bMacroEnableFriend[1])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[1], 1))
				{
					if( g_pEnvSet->m_bMacroEnableFriend[1])	info._Type = CHATTING_TYPE_TEAM;	// ��
					else									info._Type = CHATTING_TYPE_ALL;		// ��ü

					info.setString( g_pEnvSet->m_szMacro[1]);
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_3 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[2]) > 0)
			{
				if( g_pEnvSet->m_bMacroEnableFriend[2])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[2]), 1)
				{
					if( g_pEnvSet->m_bMacroEnableFriend[2])	info._Type = CHATTING_TYPE_TEAM;	// ��
					else									info._Type = CHATTING_TYPE_ALL;		// ��ü

					info.setString( g_pEnvSet->m_szMacro[2]);
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_4 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[3]) > 0)
			{
				if( g_pEnvSet->m_bMacroEnableFriend[3])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[3], 1))
				{
					if( g_pEnvSet->m_bMacroEnableFriend[3])	info._Type = CHATTING_TYPE_TEAM;	// ��
					else									info._Type = CHATTING_TYPE_ALL;		// ��ü

					info.setString( g_pEnvSet->m_szMacro[3]);
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
		if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_5 ))
		{
			if( i3String::Length( g_pEnvSet->m_szMacro[4]) > 0)
			{
				if( g_pEnvSet->m_bMacroEnableFriend[4])	
				{
					if( m_ChatBodyState == CHAT_BODY_STATE_LOBBY || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH || m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)
					{
						return;
					}
				}

				if(g_pFramework->AddFilterMessage(g_pEnvSet->m_szMacro[4], 1))
				{
					if( g_pEnvSet->m_bMacroEnableFriend[4])	info._Type = CHATTING_TYPE_TEAM;	// ��
					else									info._Type = CHATTING_TYPE_ALL;		// ��ü

					info.setString( g_pEnvSet->m_szMacro[4]);
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info ); 
				}
			}
		}
	}
}

void CGameChatBody::_UpdateLang(void)
{
	// IME��忡 ���� ��Ʈ���� �����մϴ�.
	const char * pszLang = "?";

	// IME��忡 ���� ��Ʈ���� �����մϴ�.

	LANGID langId = m_pChatBox->getIME()->GetLangId();
	UINT32 nInputLang = PRIMARYLANGID( langId);
	UINT32 nInputSubLang = SUBLANGID( langId);

	// mode off is english
	if(m_pChatBox->getIME()->GetImeMode() != IME_MODE_NATIVE)
		nInputLang = LANG_ENGLISH;

	// EXPORT_AVALIABLE_CODE_VALUE
	//	Language mark
#if defined ( LOCALE_NORTHAMERICA)
	pszLang = "";	//	�Ϲ��� ��û���� �Ⱥ��̰� ó��
#else
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
			case LANG_THAI:		pszLang = "TH";		break;
			case LANG_ENGLISH:	pszLang = "EN";		break;
			}
		}
		break;
	case I3_LANG_CODE_JAPANESE:
		{
			switch( nInputLang)
			{
			case LANG_JAPANESE:	pszLang = "JP";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:
	case I3_LANG_CODE_CHINESE_TRADITIONAL:
		{
			switch( nInputLang)
			{
			case LANG_CHINESE:	pszLang = "CH";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_RUSSIA:
		{
			switch( nInputLang)
			{
			case LANG_RUSSIAN:	pszLang = "RU";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_TURKISH:
		{
			switch( nInputLang)
			{
			case LANG_TURKISH:	pszLang = "TR";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
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
						case SUBLANG_PORTUGUESE:			pszLang = "PR";	break;
						case SUBLANG_PORTUGUESE_BRAZILIAN:	pszLang = "BR";	break;
					}
				}
				break;
			case LANG_SPANISH:	pszLang = "SP";	break;
			case LANG_ITALIAN:		pszLang = "IT";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	case I3_LANG_CODE_ARABIC:
		{
			switch( nInputLang)
			{
			case LANG_ARABIC:	pszLang = "AR";	break;
			case LANG_ENGLISH:	pszLang = "EN";	break;
			}
		}
		break;
	}
#endif

	m_pChatLang->SetCaption( pszLang);
}

void CGameChatBody::_UpdateParsingType(void)
{
	COLOR col;
	CHAT_SYS_TYPE ChatType = CHAT_SYS_TYPE_NONE;

	char szTarget[256];
	char szHistory[256];
	szTarget[0] = 0;
	szHistory[0] = 0;

	INT32 nWhsiperTextCount = 0;
	static BOOL	bUseHistory = FALSE;

	if( m_bOnceChatTypeAll)
	{
		i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
		m_pChatCombo->SetTextColor( &col);
		m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_ALL"));
		i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
	}
	else if( m_bOnceChatType)
	{
		i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
		m_pChatCombo->SetTextColor( &col);
		m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_TEAM"));
		i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
	}
	else
	{
		switch( m_nChatType)
		{
		case CHATTING_TYPE_ALL:			// ��ü
		case CHATTING_TYPE_MATCH:
		case CHATTING_TYPE_CLAN_MEMBER_PAGE:
			i3Color::Set(&col, GameGUI::GetColor(GCT_DEFAULT));
			i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
			m_pChatCombo->SetTextColor( &col);
			m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_ALL"));
			break;
		case CHATTING_TYPE_TEAM:		// ��
			i3Color::Set(&col, GameGUI::GetColor(GCT_TEAM_CHAT));
			i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
			m_pChatCombo->SetTextColor( &col);
			m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_TEAM"));
			break;
		case CHATTING_TYPE_CLAN:		// Ŭ��
			i3Color::Set(&col, GameGUI::GetColor(GCT_CLAN_CHAT));
			i3String::Concat( szTarget, m_pChatBox->getIMETextBuf());
			m_pChatCombo->SetTextColor( &col);
			m_pChatCombo->SetText( GAME_STRING("STBL_IDX_CHAT_TARGET_CLAN"));
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

	szTarget[i3String::Length( szTarget)] = 0;

	g_pFramework->getChatBox()->SetEnable( TRUE);
	m_pChatBox->SetTextColor( &col);


	if( m_nChatType != CHATTING_TYPE_WHISPER && m_nChatType != CHATTING_TYPE_REPLY)
	{
		ChatType = g_pFramework->getChatBox()->ParsingText( m_pChatBox->getIMETextBuf());
		{
			switch( ChatType)
			{
			case CHAT_SYS_TYPE_WHISPER:
				{
					char szWhisper[128];
					if( !bUseHistory) m_nOldChatType = m_nChatType;
					m_nChatType = CHATTING_TYPE_WHISPER;
					i3String::Copy( szWhisper, "[To:", 128 );
					i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatTarget());
					i3String::Concat( szWhisper, "] ");
					szWhisper[i3String::Length( szWhisper)] = 0;
					g_WhisperLength = i3String::Length( szWhisper);
					i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetHistoryText());
					m_pChatBox->SetIMEText( szWhisper);
				}
				break;
			case CHAT_SYS_TYPE_REPLY:
				{
					char szWhisper[128];
					if( !bUseHistory) m_nOldChatType = m_nChatType;
					m_nChatType = CHATTING_TYPE_REPLY;
					if( i3String::Length(g_pFramework->getChatBox()->GetChatReplyTarget()) > 0)
					{
						i3String::Copy( szWhisper, "[To:", 128 );
						i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatReplyTarget());
						i3String::Concat( szWhisper, "] ");
						szWhisper[i3String::Length( szWhisper)] = 0;
						g_WhisperLength = i3String::Length( szWhisper);
						i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetHistoryText());
						m_pChatBox->SetIMEText( szWhisper);
					}
					else
					{
						m_nChatType = m_nOldChatType;
						m_pChatBox->SetIMEText("");
					}
				}
				break;
			case CHAT_SYS_TYPE_ONETIME_TEAM:	// ��ȸ�� �� ê��!!
				if( m_ChatBodyState == CHAT_BODY_STATE_READYROOM)
				{
					m_bOnceChatType = TRUE;
					m_pChatBox->SetIMEText("");
				}
				break;
			}
		}
	}

	if( m_nChatType == CHATTING_TYPE_WHISPER || m_nChatType == CHATTING_TYPE_REPLY)
	{
		// �ӼӸ� ���¿��� �ƹ��� �ؽ�Ʈ�� �Էµ��� �������¿��� ���ڰ� �������� �ʱ�ȭ�մϴ�.
		if( m_nChatType == CHATTING_TYPE_WHISPER)
		{
			char szWhisper[128];
			i3String::Copy( szWhisper, "[To:", 128 );
			i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatTarget());
			i3String::Concat( szWhisper, "] ");

			const char* szContents = m_pChatBox->getIMETextBuf();

			if( i3String::Contain( szContents, szWhisper) == -1)
			{
				m_pChatBox->SetIMEText("");
				m_nChatType = m_nOldChatType;
				g_WhisperLength = 0;
			}
		}

		if( m_nChatType == CHATTING_TYPE_REPLY)
		{
			char szWhisper[128];
			i3String::Copy( szWhisper, "[To:", 128 );
			i3String::Concat( szWhisper, g_pFramework->getChatBox()->GetChatReplyTarget());
			i3String::Concat( szWhisper, "] ");

			const char* szContents = m_pChatBox->getIMETextBuf();

			if( i3String::Contain( szContents, szWhisper) == -1)
			{
				m_pChatBox->SetIMEText("");
				m_nChatType = m_nOldChatType;
				g_WhisperLength = 0;
			}
		}
	}

	// Chat History���
	if( g_pFramework->getChatBox()->GetHistoryBufferCount() > 0)
	{
		i3InputKeyboard* pKeyboard = g_pFramework->getKeyboard();

		char szText[256];
		szText[0]=0;
		INT32 nChatMode = CHATTING_TYPE_ALL;

		if (pKeyboard->GetStrokeState(I3I_KEY_UP))
		{
			m_pChatBox->SetFocus( TRUE);

			bUseHistory = TRUE;

			if( g_pFramework->getChatBox()->GetHistoryBufferOffset() == 0)
			{
				m_nOldChatType = m_nChatType;
			}

			g_pFramework->getChatBox()->GetHistoryBufferText(1, szText, 256, &nChatMode);

			if(szText[0] != NULL)
			{
				m_pChatBox->SetIMEText( szText);
				m_nChatType = nChatMode;
			}
			else
			{
				m_pChatBox->SetIMEText( "");
			}
		}
		else if (pKeyboard->GetStrokeState(I3I_KEY_DOWN))
		{
			m_pChatBox->SetFocus( TRUE);

			bUseHistory = TRUE;

			g_pFramework->getChatBox()->GetHistoryBufferText(-1, szText, 256, &nChatMode);

			if(szText[0] != NULL)
			{
				m_pChatBox->SetIMEText( szText);
				m_nChatType = nChatMode;
			}
			else
			{
				bUseHistory = FALSE;
				m_pChatBox->SetIMEText( "");
				m_nChatType = m_nOldChatType;
			}
		}
	}


	char szTemp[256] = {};
	char szTemp2[256] = {};

	BOOL bWhisper = FALSE;
	BOOL bReply = FALSE;
	INT32 nReplyTextCount = 0;




	// IME ����Ʈ �Է¹޴� �κ��� �޵�� ���¸� üũ�ؼ� ó���ؾ� �˴ϴ�. komet 09.06
	// IME�� ó���ϴ� Windows API Proc�� �޽��� ť�� ������� ó���ϱ� �����Դϴ�.
	if( ! GUI_IME_CONFIRM(m_pChatBox))
		return;

	switch( ChatType)
	{
	case CHAT_SYS_TYPE_CLEAR:
		g_pFramework->getChatBox()->ResetChatBuffer();
		m_pChatBox->SetIMEText("");
		break;
	case CHAT_SYS_TYPE_HELP:
		g_pFramework->getChatBox()->SetHelpText();
		m_pChatBox->SetIMEText("");
		break;
	}
	const char * ChatText = m_pChatBox->getIMETextBuf();

	// GM�� �� ��ɾ� üũ.
	if ( m_ChatBodyState == CHAT_BODY_STATE_READYROOM  )
	{
		if ( SetGMCommand(ChatText) )
			m_pChatBox->SetIMEText("");
	}

	// GM�� �κ� ��ɾ� üũ.
	if ( m_ChatBodyState == CHAT_BODY_STATE_LOBBY )
	{
		if ( SetGMLobbyCommand(ChatText) )
			m_pChatBox->SetIMEText("");
	}
 
	// GM ��ɾ�� ä�ø޽����� �Ѹ��� �ʽ��ϴ�.
	if ( g_pGameContext->IsGameMaster() == TRUE && GM_COMMAND_UNKNOWN != GetCommandType(ChatText) )
		m_pChatBox->SetIMEText("");


	if( i3String::Length(ChatText) <= 0 || i3String::ContainNoCase(ChatText, "{col:") >= 0)
	{
		m_pChatBox->InputDone();
		return;
	}

	if(g_pFramework->AddFilterMessage(m_pChatBox->getIMETextBuf(), 1))
	{
		// ���͸� �ܾ� *�� ����
		g_pFramework->IsFilterChange(ChatText);

		// ��Ʈ��ũ�� �޽����� �����ϴ�.
		CHAT_INFO info;
		info._Type = (UINT16)m_nChatType;

		if( m_bOnceChatTypeAll)
		{
			i3String::Copy( szTemp, ChatText, 256 );
		}
		else if( m_bOnceChatType)
		{
			i3String::Copy( szTemp, ChatText, 256 );
			info._Type = CHATTING_TYPE_TEAM;
		}
		else
		{

			switch( m_nChatType)
			{
			case CHATTING_TYPE_ALL:	// ��ü
				i3String::Copy( szTemp, ChatText, 256 );
				break;
			case CHATTING_TYPE_TEAM:// ��
				i3String::Concat( szTemp, ChatText );
				break;
			case CHATTING_TYPE_CLAN:// Ŭ��
				i3String::Copy( szTemp, ChatText, 256 );
				break;
			case CHATTING_TYPE_WHISPER: // �Ӹ�
				i3String::Copy( szTemp2, "{col:237, 86, 250, 255}", 256 );
				i3String::Concat( szTemp2, ChatText);
				i3String::Concat( szTemp2, "{/col}");

				i3String::Copy( szTemp, ChatText + g_WhisperLength, 256 );

				nWhsiperTextCount = i3String::Length( ChatText + g_WhisperLength);
				bWhisper = TRUE;
				m_nChatType = m_nOldChatType;
				break;
			case CHATTING_TYPE_REPLY:// �亯
				i3String::Copy( szTemp2, "{col:237, 86, 250, 255}", 256 );
				i3String::Concat( szTemp2, ChatText);
				i3String::Concat( szTemp2, "{/col}");

				i3String::Copy( szTemp, ChatText + g_WhisperLength, 256 );

				nReplyTextCount = i3String::Length( ChatText + g_WhisperLength);
				bReply = TRUE;
				m_nChatType = m_nOldChatType;
				break;
			default: break;
			}
		}

		szTemp[i3String::Length( szTemp)] = 0;
		szTemp2[i3String::Length( szTemp2)] = 0;

		// �Ӹ��� ��� �ٸ� ������� ó���ؾ� �մϴ�.
		if( bWhisper)
		{
			if( nWhsiperTextCount > 0)
			{
				g_pFramework->getChatBox()->PutSystemChatting(szTemp2);

				// ģ�� ��Ͽ� �ִ� �г����� ��� �ش� UID�� ã�Ƽ� UID�� ������.
				INT64 ReceiverUID = FindUserUIDByNick(g_pFramework->getChatBox()->GetChatTarget());
				if (0 < ReceiverUID)
				{
					// TODO:ä�� �Է�
					// Send Event
					g_pGameContext->SetEvent( EVENT_SEND_WHISPER_FIND_UID, g_pFramework->getChatBox()->GetChatTarget(), szTemp, &ReceiverUID);

					i3String::Copy( szHistory, "/w ", 256 );
					strcat( szHistory, g_pFramework->getChatBox()->GetChatTarget());
					strcat( szHistory, " ");
					strcat( szHistory, (ChatText + g_WhisperLength));
					szHistory[i3String::Length( szHistory)] = 0;
					g_pFramework->getChatBox()->PutHistory(szHistory, m_nChatType);
				}
				else
				{
					// TODO:ä�� �Է�
					// Send Event
					g_pGameContext->SetEvent( EVENT_SEND_WHISPER, g_pFramework->getChatBox()->GetChatTarget(), szTemp);

					i3String::Copy( szHistory, "/w ", 256 );
					strcat( szHistory, g_pFramework->getChatBox()->GetChatTarget());
					strcat( szHistory, " ");
					strcat( szHistory, (ChatText + g_WhisperLength));
					szHistory[i3String::Length( szHistory)] = 0;
					g_pFramework->getChatBox()->PutHistory(szHistory, m_nChatType);
				}
			}
		}
		else if( bReply)
		{
			if( nReplyTextCount > 0)
			{
				// ģ�� ��Ͽ� �ִ� �г����� ��� �ش� UID�� ã�Ƽ� UID�� ������.


				// TODO:ä�� �Է�
				// Send Event
				g_pFramework->getChatBox()->PutSystemChatting(szTemp2);
				g_pGameContext->SetEvent( EVENT_SEND_WHISPER, g_pFramework->getChatBox()->GetChatReplyTarget(), szTemp);

				i3String::Copy( szHistory, "/r ", 256 );
				strcat( szHistory, " ");
				strcat( szHistory, ChatText + g_WhisperLength);
				szHistory[i3String::Length( szHistory)] = 0;
				g_pFramework->getChatBox()->PutHistory(szHistory, m_nChatType);
			}
		}
		else
		{
			info.setString( szTemp);

			if( info._Type == CHATTING_TYPE_CLAN)	// Ŭ�� ä���̶��..
			{
				// Ŭ�� �̰��� �����Դϴ�.
				USER_INFO_BASIC		MyUserInfoBasic;
				g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
				if(MyUserInfoBasic._clanidx == 0 || MyUserInfoBasic._clanstate == 0)
				{
					g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_CHAT_INGAME_NOTICE_NO_JOIN_CLAN"));/*<Ŭ���� �̰����� �����Դϴ�.>*/
				}
				else
				{
					// TODO:ä�� �Է�
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info );
					g_pFramework->getChatBox()->PutHistory(ChatText, m_nChatType);
				}
			}
			else
			{
				if( m_ChatBodyState == CHAT_BODY_STATE_CLAN_MATCH)	//	Ŭ���� �κ񿡼� ����
					info._Type = CHATTING_TYPE_MATCH;

				if( m_ChatBodyState == CHAT_BODY_STATE_CLAN_MEMBER)	//	Ŭ�� ������ ���������� ����
					info._Type = CHATTING_TYPE_CLAN_MEMBER_PAGE;

				// Ŀ�´�Ƽ �ɼǿ����� ä�� ������ �ɷ��ִٸ�, �װͿ� ���� ó���� �մϴ�.
				if( g_pEnvSet->m_nChatType != 0 && (m_nChatType != CHATTING_TYPE_TEAM && m_nChatType != CHATTING_TYPE_CLAN) )
				{
					i3String::Copy( szTemp2, "[", 256 );
					i3String::Concat( szTemp2, g_pGameContext->GetMyNickName());
					i3String::Concat( szTemp2, "] ");
					i3String::Concat( szTemp2, szTemp);

					g_pFramework->getChatBox()->PutSystemChatting(szTemp2);
					g_pFramework->getChatBox()->PutHistory(ChatText, m_nChatType);
				}
				else
				{
					// TODO:ä�� �Է�
					g_pGameContext->SetEvent(EVENT_CHATTING_N , &info );
					g_pFramework->getChatBox()->PutHistory(ChatText, m_nChatType);
				}
			}
		}

		g_WhisperLength = 0;
	}

	g_pFramework->getChatBox()->ResetHistoryBufferIndex();

	m_pChatBox->SetIMEText("");

	m_bOnceChatType = FALSE;
	m_bOnceChatTypeAll = FALSE;

	if( bUseHistory) m_nChatType = m_nOldChatType;
	bUseHistory = FALSE;

	m_pChatBox->InputDone();

}

void CGameChatBody::_UpdateChatBuffer(void)
{
	// dummy chat text �߰�
	UINT32 nListCount = m_pChatList->GetElementCount();
	CGameChatBox * pFrameWorkChatBox = g_pFramework->getChatBox();
	UINT32 nBufferCount = pFrameWorkChatBox->GetBufferCount();

	for(UINT32 j = nListCount; j < nBufferCount; j++)
	{
		m_pChatList->AddElement("");
	}


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
	char szTotalText[2048] = {0,};
	char szChatBoxText[2048] = {0,};

	for( INT32 i = BufferIdx; nLoopCount < m_nMaxChatLineCount; i--)
	{
		nAccumCount++;

		INT32	ChatBoxBufCount = pFrameWorkChatBox->GetBufferCount();
		if( nLoopCount > ChatBoxBufCount || ChatBoxBufCount <= 0 || nAccumCount > ChatBoxBufCount)
		{
			break;
		}

		if( i<0)
		{
			i = MAX_CHAT_BUFFER-1;
		}

		if( m_nCurrentChatBufferType == CHATTING_TYPE_ALL || m_nCurrentChatBufferType == CHATTING_TYPE_LOBBY ||  m_nCurrentChatBufferType == CHATTING_TYPE_MATCH ||  m_nCurrentChatBufferType == CHATTING_TYPE_CLAN_MEMBER_PAGE)
		{
			nLoopCount++;

			i3String::Copy( szChatBoxText, pFrameWorkChatBox->GetBackUpBufferText(i), 2048 );

			if( i3String::Length( szChatBoxText) > 0)
			{
				i3String::Concat( szChatBoxText, "\n");
				i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));
			}
		}
		else if( m_nCurrentChatBufferType == CHATTING_TYPE_TEAM && ( pFrameWorkChatBox->getBufferType(i) == CHATTING_TYPE_TEAM || pFrameWorkChatBox->getBufferType(i) == CHATTING_TYPE_CLAN) )
		{
			nLoopCount++;

			i3String::Copy( szChatBoxText, pFrameWorkChatBox->GetBackUpBufferText(i), 2048 );

			if( i3String::Length( szChatBoxText) > 0)
			{
				i3String::Concat( szChatBoxText, "\n");
				i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));
			}
		}
		else if( m_nCurrentChatBufferType == CHATTING_TYPE_WHISPER && ( pFrameWorkChatBox->getBufferType(i) == CHATTING_TYPE_WHISPER || pFrameWorkChatBox->getBufferType(i) == CHATTING_TYPE_REPLY) )
		{
			nLoopCount++;

			i3String::Copy( szChatBoxText, pFrameWorkChatBox->GetBackUpBufferText(i), 2048 );

			if( i3String::Length( szChatBoxText) > 0)
			{
				i3String::Concat( szChatBoxText, "\n");
				i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));
			}
		}
		else if( m_nCurrentChatBufferType == pFrameWorkChatBox->getBufferType(i) || pFrameWorkChatBox->getBufferType(i) == 0)
		{
			nLoopCount++;

			i3String::Copy( szChatBoxText, pFrameWorkChatBox->GetBackUpBufferText(i), 2048 );

			if( i3String::Length( szChatBoxText) > 0)
			{
				i3String::Concat( szChatBoxText, "\n");
				i3String::InsertStr( szTotalText, szChatBoxText, 0, sizeof(char)*i3String::Length( szChatBoxText));
			}
		}
	}

	INT32 nLen = i3String::Length( szTotalText);
	if( nLen > 0) szTotalText[nLen-1] = 0;

	// chat list�� ����մϴ�.
	m_pTextBox->SetText( szTotalText);
}

void CGameChatBody::_UpdateWhisperBackupBuffer(void)
{
	for( INT32  i = 0 ; i < g_pFramework->getChatBox()->GetWhisperBufferCount() ; i++)
	{
		INT32 index = (g_pFramework->getChatBox()->GetCurrentWhisperBufferIndex() + i) % 5;
		g_pFramework->getChatBox()->PutSystemChatting(g_pFramework->getChatBox()->GetWhisperBufferText(index), CHATTING_TYPE_WHISPER);
	}

	g_pFramework->getChatBox()->ResetWhisperBuffer();
}

void CGameChatBody::_UpdateChatChannelType(void)
{
	m_pChatChannelButton[m_nCurrentSelectedChatType]->SetShapeEnable( 1, TRUE);
}

//20110425 �ϵ��� �߱� ���͸� ����
/*
void CGameChatBody::ReConstructText(char *src, int AbuseWordCount)
{
	int j,k,S_Length,A_Length,count,Buf_Count,Buf[100];
	char *AbuseWord;
	for(int i=0; i<AbuseWordCount; i++)
	{
		AbuseWord = g_pGameContext->GetAbuseWord(i);
		S_Length = strlen(src);
		A_Length = strlen(AbuseWord);
		j = 0;
		k = 0;
		count = 0;
		Buf_Count = 0;
		while(1)
		{
			 //�빮���� ��� �ҹ��ڷ� ��ȯ :: AbusingWord�� Lower ���̽�.
			 //���� ��� Ȯ�尡�� �ٸ� ������ ���....
			char T_Char = src[j];
			if(T_Char >= 65 && T_Char <=90)
			{
				T_Char += 32;
			}


			if(T_Char == AbuseWord[k])
			{
				Buf[Buf_Count++] = j;
				count++;
				k++;
				j++;
			}
			else if(T_Char == ' ')
			{
				//������ ��� �׳� ������ �̵�
				j++;
			}
			else
			{
				//��ġ���� ���� ��� ���� ī���� �ʱ�ȭ
				Buf_Count = 0;
				j++;
			}

			//���� �ƿ� ����
			if(count == A_Length && j < S_Length - 1)
			{
				// ����� Word�� *�� ��ȯ(�ʰ��� ��� ���ѷ���)
				for(int u = 0; u < Buf_Count; u++)
				{
					src[Buf[u]] = '*';
				}
				count = Buf_Count = k = 0;
			}
			else if(count == A_Length && j == S_Length)
			{
				// AbusingWord�����ϰ�, ���ڿ��� Ž���� �Ϸ�� ���
				// ����� Word�� *�� ��ȯ(�ʰ��� ��� ���ѷ���)
				for(int u = 0; u < Buf_Count; u++)
				{
					src[Buf[u]] = '*';
				}
				break;
			}
			else if(count < A_Length && j == S_Length)
			{
				break;
			}
		}
	}
	g_pGameContext->ClearAbuseWordInfo();
}
*/