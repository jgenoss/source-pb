#include "pch.h"
#include "ClientTCPSocket.h"
#include "CommunityContext.h"
#include "Shop.h"
#include "IGShopContext.h"
#include "IGShopItemController.h"

#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"

#include "MailBoxContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#ifdef _FROST_
	#include "SecurityHeader.h"
#endif	

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_AUTH_BLOCK_INFO_ACK : return "PROTOCOL_AUTH_BLOCK_INFO_ACK";
		case PROTOCOL_AUTH_FRIEND_INFO_ACK : return "PROTOCOL_AUTH_FRIEND_INFO_ACK";
		case PROTOCOL_AUTH_FRIEND_INVITED_ACK : return "PROTOCOL_AUTH_FRIEND_INVITED_ACK";
		case PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK : return "PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK";
		case PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK : return "PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK";
		case PROTOCOL_AUTH_FRIEND_ACCEPT_ACK : return "PROTOCOL_AUTH_FRIEND_ACCEPT_ACK";
		case PROTOCOL_AUTH_FRIEND_INSERT_ACK : return "PROTOCOL_AUTH_FRIEND_INSERT_ACK";
		case PROTOCOL_AUTH_FRIEND_DELETE_ACK : return "PROTOCOL_AUTH_FRIEND_DELETE_ACK";
		case PROTOCOL_AUTH_SEND_WHISPER_ACK : return "PROTOCOL_AUTH_SEND_WHISPER_ACK";
		case PROTOCOL_AUTH_RECV_WHISPER_ACK : return "PROTOCOL_AUTH_RECV_WHISPER_ACK";
		case PROTOCOL_AUTH_FIND_USER_ACK : return "PROTOCOL_AUTH_FIND_USER_ACK";
		case PROTOCOL_AUTH_CHANGE_NICKNAME_ACK : return "PROTOCOL_AUTH_CHANGE_NICKNAME_ACK";
		case PROTOCOL_MESSENGER_NOTE_SEND_ACK : return "PROTOCOL_MESSENGER_NOTE_SEND_ACK";
		case PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK : return "PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK";
		case PROTOCOL_MESSENGER_NOTE_LIST_ACK : return "PROTOCOL_MESSENGER_NOTE_LIST_ACK";
		case PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK : return "PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK";
		case PROTOCOL_MESSENGER_NOTE_DELETE_ACK : return "PROTOCOL_MESSENGER_NOTE_DELETE_ACK";
		case PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK : return "PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK";
		case PROTOCOL_AUTH_FCM_INFO_ACK : return "PROTOCOL_AUTH_FCM_INFO_ACK";
		case PROTOCOL_AUTH_FCM_STATE_ACK : return "PROTOCOL_AUTH_FCM_STATE_ACK";
		case PROTOCOL_AUTH_BLOCK_INSERT_ACK : return "PROTOCOL_AUTH_BLOCK_INSERT_ACK";
		case PROTOCOL_AUTH_BLOCK_DELETE_ACK : return "PROTOCOL_AUTH_BLOCK_DELETE_ACK";
		case PROTOCOL_AUTH_ACCOUNT_KICK_ACK : return "PROTOCOL_AUTH_ACCOUNT_KICK_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_AUTH_XXX �������� �̸� ��������̼�~");
			return "PROTOCOL_AUTH_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingAuth_server(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Auth.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Auth.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_Auth_Function( void)
{
	RegisterParser( PROTOCOL_AUTH_BLOCK_INFO_ACK,				&ClientTCPSocket::__Parse_Auth_BlockUser);
	RegisterParser( PROTOCOL_AUTH_FRIEND_INFO_ACK,				&ClientTCPSocket::__Parse_Auth_FriendUser);
	RegisterParser( PROTOCOL_AUTH_FRIEND_INVITED_ACK,			&ClientTCPSocket::__Parse_Auth_InviteFriend);
	RegisterParser( PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK,	&ClientTCPSocket::__Parse_Auth_RequestInvitedFriend);
	RegisterParser( PROTOCOL_AUTH_FRIEND_INFO_CHANGE_ACK,		&ClientTCPSocket::__Parse_Auth_ChangeFriendInfo);
	RegisterParser( PROTOCOL_AUTH_FRIEND_ACCEPT_ACK,			&ClientTCPSocket::__Parse_Auth_AcceptFriend);
	RegisterParser( PROTOCOL_AUTH_FRIEND_INSERT_ACK,			&ClientTCPSocket::__Parse_Auth_AddFriend);
	RegisterParser( PROTOCOL_AUTH_FRIEND_DELETE_ACK,			&ClientTCPSocket::__Parse_Auth_DeleteFriend);
	RegisterParser( PROTOCOL_AUTH_SEND_WHISPER_ACK, 			&ClientTCPSocket::__Parse_Auth_SendWhisper);
	RegisterParser( PROTOCOL_AUTH_RECV_WHISPER_ACK, 			&ClientTCPSocket::__Parse_Auth_ReceiveWhisper);
	RegisterParser( PROTOCOL_AUTH_FIND_USER_ACK,				&ClientTCPSocket::__Parse_Auth_FindUser);
	RegisterParser( PROTOCOL_AUTH_CHANGE_NICKNAME_ACK,			&ClientTCPSocket::__Parse_Auth_ChangeNickname);
	RegisterParser( PROTOCOL_MESSENGER_NOTE_SEND_ACK,			&ClientTCPSocket::__Parse_MailBox_SendNote);
	RegisterParser( PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK,		&ClientTCPSocket::__Parse_MailBox_SendClanNote);
	RegisterParser( PROTOCOL_MESSENGER_NOTE_LIST_ACK,			&ClientTCPSocket::__Parse_MailBox_ReceiveNoteList);
	RegisterParser( PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK,	&ClientTCPSocket::__Parse_MailBox_CheckReadedNote);
	RegisterParser( PROTOCOL_MESSENGER_NOTE_DELETE_ACK,			&ClientTCPSocket::__Parse_MailBox_DeleteNote);
	RegisterParser( PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK,		&ClientTCPSocket::__Parse_MailBox_ReceiveNote);
	RegisterParser( PROTOCOL_AUTH_FCM_INFO_ACK,					&ClientTCPSocket::__Parse_Auth_FCMInfo);
	RegisterParser( PROTOCOL_AUTH_FCM_STATE_ACK,				&ClientTCPSocket::__Parse_Auth_FCMState);
	RegisterParser( PROTOCOL_AUTH_BLOCK_INSERT_ACK,				&ClientTCPSocket::__Parse_Auth_AddBlockUser);
	RegisterParser( PROTOCOL_AUTH_BLOCK_DELETE_ACK,				&ClientTCPSocket::__Parse_Auth_DeleteBlockUser);
	RegisterParser( PROTOCOL_AUTH_ACCOUNT_KICK_ACK,				&ClientTCPSocket::__Parse_Auth_AccountKick);

}

void ClientTCPSocket::__Parse_Auth_BlockUser( i3NetworkPacket * pPacket)
{
	UINT8 _ui8BlockCount;
	pPacket->ReadData(&_ui8BlockCount, sizeof(UINT8));
	CCommunityContext::i()->SetBlockUserCount(0);

	if(_ui8BlockCount > BLOCK_MAX_COUNT)
	{
		return;
	}

	TTCHAR szNick[NET_NICK_NAME_SIZE];

	for(UINT32 i=0; i < _ui8BlockCount; i++)
	{
		BLOCK_USER t_Block;
		t_Block._Activated = true;
		UINT8 nicksize;
	
		pPacket->ReadData(&nicksize, sizeof(UINT8));

		::memset(szNick, 0, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
		pPacket->ReadData(szNick, sizeof(TTCHAR) * nicksize);

		t_Block._wstrNick = szNick;

		UINT8 commentsize;
		pPacket->ReadData(&commentsize, sizeof(UINT8));

		TTCHAR szComment[BLOCK_COMMENT_MAX];		::memset(szComment, 0, sizeof(szComment));
		pPacket->ReadData(szComment, sizeof(TTCHAR) * commentsize);

		t_Block._wstr_comment = szComment;

		pPacket->ReadData(&t_Block._blockInfo, sizeof(BLOCK_INFO));
		CCommunityContext::i()->AddBlockUser(&t_Block);
	}
}

void ClientTCPSocket::__Parse_Auth_FriendUser( i3NetworkPacket * pPacket)
{
	// ��ü������ �ް�
	UINT8 count;

	pPacket->ReadData(&count, sizeof(UINT8));

	if (count > FRIEND_MAX_COUNT)
	{
		I3PRINTLOG(I3LOG_FATAL, "Invalid friend count(%d)", count);
		return;
	}

	// 1.5�������� ģ�� ������ ���, PROTOCOL_AUTH_FRIEND_DELETE_ACK���� �̺�Ʈ ó����.
	// ���� ���� �� ������ �����Դϴ�. - 2012.07.30 ���õ
	// ������ ���� ������Ʈ�� ��� �뺸�Ѵ�.
	// ���� idx�� nickname Ȯ���� CommunityContext�� ����.
	//if (count < CCommunityContext::i()->GetFriendCount())
	//{				
	//	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_DELETE_FRIEND);
	//}

	CCommunityContext::i()->ClearFriend();	// (1)

	// ������ŭ ���鼭
	for(UINT32 i = 0; i < count; i++)			
	{	
		// ����� �о� ����Ѵ�
		FRIEND_NODE node;

		_ReadFriendInfoToNode(pPacket, &node);
		CCommunityContext::i()->AddFriend(&node); // (2)
	}
}

void ClientTCPSocket::__Parse_Auth_InviteFriend( i3NetworkPacket * pPacket)
{
	INT32 returnCode;

	pPacket->ReadData(&returnCode, sizeof(INT32));

	if (EV_SUCCESSED(returnCode))
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVITED_MESSAGE, returnCode);
	}

	switch( returnCode)
	{
	case EVENT_ERROR_EVENT_FAIL_INVITED_USER:
		{
			/*�ʴ��� ������ ä�� ������ ���� ���� �����Դϴ�.\nä�� ���� �� �ʴ��� �ֽñ� �ٶ��ϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_INVITED_USER"));
		}
		break;			
	case EVENT_ERROR_EVENT_FAIL_INVITED_USER_IN_CLAN_MATCH:
		{
			/*Ŭ������ ��� ���� �����Դϴ�.\n�ش� ������ ����� ���°� �ƴϸ�\n�ʴ� �����մϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_INVITED_USER_IN_CLAN_MATCH"));
		}
		break;
	default:
		break;
	}
}

void ClientTCPSocket::__Parse_Auth_RequestInvitedFriend( i3NetworkPacket * pPacket)
{
	if( g_pFramework->IsUIStage() == false )
	{				
		return;	// ReadyStage�� �ƴϸ� ó������ �ʴ´�.
	}

	if (g_pFramework->GetClanContext()->InBattleTeam())
	{
		return;	// ���� Ŭ�� ���̸� ģ�� �ʴ밡 ���� �ʴ´�.
	}

	UINT8 idx;
	pPacket->ReadData(&idx, sizeof(UINT8));

	const FRIEND_NODE* node = CCommunityContext::i()->GetFriend(idx);

	if (node)
	{
		INT32 viewIndex = CCommunityContext::i()->GetViewIndexOfRealFriend((UINT32)idx);
		
		if (CCommunityContext::i()->SetMovingToFriendUser(viewIndex))
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVITED_MESSAGE);
		}
		else
		{
			I3TRACE("[NOTICE] PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK - Invited message full");
		}
	}	
	else
	{
		I3TRACE("[WARNING] PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK - Invalid friend idx(%d)\n", idx);
	}

}

void ClientTCPSocket::__Parse_Auth_ChangeFriendInfo( i3NetworkPacket * pPacket)
{
	// ���� ������ �ް�
	UINT8 state;	// 1:�߰�, 2:����, 3:Reserved, 4:���º���
	UINT8 idx;

	pPacket->ReadData(&state, sizeof(UINT8));
	pPacket->ReadData(&idx, sizeof(UINT8));

	// ����� �о�
	FRIEND_NODE node;

	_ReadFriendInfoToNode(pPacket, &node);
	
	// ����Ѵ�
	switch(state)
	{
	case FRIEND_CHANGE_STATE_INSERT:
		CCommunityContext::i()->AddFriend(&node);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INSERT_FRIEND, idx);
		break;
	case FRIEND_CHANGE_STATE_ACCEPT:
		CCommunityContext::i()->SetFriend(idx, &node);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ACCEPT_FRIEND, idx);
		break;
	case FRIEND_CHANGE_STATE_CHANGE:
		CCommunityContext::i()->SetFriend(idx, &node);
		break;
	default:
		/* Do nothing */
		break;
	}
	
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_UPDATE_USER_LIST);
}

void ClientTCPSocket::__Parse_Auth_AcceptFriend( i3NetworkPacket * pPacket)
{
	INT32 result;
	pPacket->ReadData(&result, sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ACCEPT_FRIEND, result);
}

void ClientTCPSocket::__Parse_Auth_AddFriend( i3NetworkPacket * pPacket)
{
	INT32 result;

	pPacket->ReadData(&result, sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INSERT_FRIEND, result);
}

void ClientTCPSocket::__Parse_Auth_DeleteFriend( i3NetworkPacket * pPacket)
{
	INT32 result;

	pPacket->ReadData(&result, sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_DELETE_FRIEND, result);
}

void ClientTCPSocket::__Parse_Auth_SendWhisper( i3NetworkPacket * pPacket)
{
	INT32 result;
	TTCHAR receiverNick[NET_NICK_NAME_SIZE];

	// �÷��г���
	UINT8 ui8NickColor = 0;

	pPacket->ReadData(&result, sizeof(INT32));
	pPacket->ReadData(&ui8NickColor, sizeof(UINT8));
	pPacket->ReadData(receiverNick, NET_NICK_NAME_SIZE * sizeof(TTCHAR)); 
	receiverNick[NET_NICK_NAME_SIZE - 1] = '\0';

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SEND_WHISPER, result);

	if (EV_FAILED(result))
	{
		I3TRACE("FailWhisper receiverNick = %s\n", receiverNick);
		
		i3::stack_wstring wstrTemp = L"[";
		i3::wliteral_range wstrNick = receiverNick;
		wstrTemp.append(wstrNick.begin(), wstrNick.end());

		wstrTemp += L']';

		const i3::rc_wstring& wstrGameString = GAME_RCSTRING("STR_TBL_NETWORK_FAIL_USER_SEND");
		wstrTemp.append(wstrGameString.begin(), wstrGameString.end() );

		/*���� �������� �ʴ� ����� �̰ų� ���ӿ� ���� ������ �ʽ��ϴ�.*/

		g_pFramework->getChatBox()->PutSystemChatting(wstrTemp);
	}
}

void ClientTCPSocket::__Parse_Auth_ReceiveWhisper( i3NetworkPacket * pPacket)
{
	TTCHAR senderNick[NET_NICK_NAME_SIZE];
	INT16 messageSize;
	TTCHAR message[MAX_STRING_COUNT];
	bool bGameMaster = false;

	// �÷��г���
	UINT8	ui8NickColor;

	pPacket->ReadData(senderNick, NET_NICK_NAME_SIZE * sizeof(TTCHAR) );
	senderNick[NET_NICK_NAME_SIZE - 1] = '\0';

	pPacket->ReadData(&bGameMaster, sizeof(bool)); // 20140924 ����� GM FLAG
	pPacket->ReadData(&ui8NickColor, sizeof(UINT8) );

	pPacket->ReadData(&messageSize, sizeof(INT16));
	messageSize = MIN(messageSize, MAX_STRING_COUNT);

	pPacket->ReadData(message, messageSize * sizeof(TTCHAR) );
	message[messageSize - 1] = '\0';

	i3::rc_wstring wstr_senderNick = senderNick;

	i3::wliteral_range wstr_message = message;

	g_pFramework->getChatBox()->SetChatReplyTarget( wstr_senderNick);

	g_pFramework->getChatBox()->PutUserWhisper( wstr_senderNick, wstr_message, bGameMaster); 
}

void ClientTCPSocket::__Parse_Auth_FindUser( i3NetworkPacket * pPacket)
{
	INT32 i32Result;

	pPacket->ReadData(&i32Result, sizeof(INT32));

	if(EV_SUCCESSED(i32Result))
	{
		NEW_FIND_USER_INFO pInfo ;//= CCommunityContext::i()->SetFindUserInfo();
		pPacket->ReadData(&pInfo, sizeof(NEW_FIND_USER_INFO));
		
		CCommunityContext::i()->SetFindUserInfo(pInfo);
	}
	else
	{
		CCommunityContext::i()->SetDefaultFindUserData();
	}
	
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_FIND_USER, i32Result);

}

void ClientTCPSocket::__Parse_Auth_ChangeNickname( i3NetworkPacket * pPacket)
{
	UINT8	ui8Size;
	TTCHAR Name[NET_NICK_NAME_SIZE] = {};

	pPacket->ReadData(&ui8Size,			sizeof(UINT8));

	if( ui8Size > NET_NICK_NAME_SIZE - 1 )	ui8Size = NET_NICK_NAME_SIZE - 1;

	pPacket->ReadData(Name,		(ui8Size +1 ) * sizeof(TTCHAR));		// �̰� ����...�ϳ� �� ����� ����..�ι��� ����?

	UserInfoContext::i()->SetMyNickName(Name);

#if defined (_FROST_)
	i3::string strNickName;
	i3::utf16_to_mb( UserInfoContext::i()->GetMyNickName(), strNickName );
	frostSetCharName(strNickName.c_str());
#endif
}

void ClientTCPSocket::__Parse_MailBox_SendNote( i3NetworkPacket * pPacket)
{
	INT32 returnCode;

	pPacket->ReadData(&returnCode, sizeof(INT32));

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SEND_NOTE, returnCode);
}

void ClientTCPSocket::__Parse_MailBox_SendClanNote( i3NetworkPacket * pPacket)
{
	INT32 returnCode;

	pPacket->ReadData(&returnCode, sizeof(INT32));

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SEND_NOTE, returnCode);
}

void ClientTCPSocket::__Parse_MailBox_ReceiveNoteList( i3NetworkPacket * pPacket)
{
	// ��ü ���� ����
	UINT8 page;
	UINT8 noteCount;

	NOTE_RECEIVE* pNote = nullptr;

	pPacket->ReadData(&page, sizeof(UINT8));
	pPacket->ReadData(&noteCount, sizeof(UINT8));

	// ������ ����Ʈ �ʱ�ȭ
	if (0 == page)
	{
		MailBoxContext::i()->ReceiveNoteFreeAll();	
	}

	// �ش� ���� �ޱ� - ���� �Ӽ�
	for(UINT32 i = 0; i < noteCount; i++)
	{
		pNote = (NOTE_RECEIVE*)i3MemAlloc( sizeof(NOTE_RECEIVE));	
		i3mem::FillZero( pNote, sizeof( NOTE_RECEIVE));

		pPacket->ReadData(&pNote->header, sizeof(NOTE_HEADER));

		MailBoxContext::i()->AddReceiveNote( pNote);
	}

	// ���� ���� �ޱ� - ���� ����
	for(UINT32 i = 0; i < noteCount; i++)
	{
		INT32 NoteIndex = (page * MAX_NOTE_COUNT_PER_PAGE) + i;
		pNote = MailBoxContext::i()->SetReceiveNote(NoteIndex);

		// ���� ���ڿ� ���� �ޱ�
		pPacket->ReadData(&pNote->body._senderSize, sizeof(UINT8));
		pPacket->ReadData(&pNote->body._messageSize, sizeof(UINT8));

		// ���� ���ڿ� �ޱ�
		pPacket->ReadData(pNote->body._sender, pNote->body._senderSize * sizeof(TTCHAR));
		if( 0 < pNote->body._messageSize )
		{
			pPacket->ReadData(pNote->body._message, pNote->body._messageSize * sizeof(TTCHAR) );
			if(pNote->header._type == NT_GIFT)
				MailBoxContext::i()->ConvertEventGiftMessage(pNote->body, pNote->body._message);
			pNote->body._messageSize = (UINT8)(i3::generic_string_size(pNote->body._message) + 1);
		}
		else
		{
			pPacket->ReadData(&pNote->body._message[0], sizeof(TTCHAR));
			pPacket->ReadData(&pNote->body._message[1], pNote->body._message[0] * sizeof(TTCHAR) );	
		}

	}

	MailBoxContext::i()->UpdateClanMessage();
	MailBoxContext::i()->SetNoteNewFlag();
}

void ClientTCPSocket::__Parse_MailBox_CheckReadedNote( i3NetworkPacket * pPacket)
{
	// �������·� ����� ���� ����
	UINT8 readedCount;

	pPacket->ReadData(&readedCount, sizeof(UINT8));

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CHECK_READED_NOTE);

	// ���� ���·� ����� ���� Id
	for(UINT32 i = 0; i < readedCount; i++)
	{
		INT32 noteId;

		pPacket->ReadData(&noteId, sizeof(INT32));
	}

	if( readedCount > 0) 
	{
		// sucess
		MailBoxContext::i()->SetNoteRefreshFlag();
	}
	else
	{
		// fail
	}
}

void ClientTCPSocket::__Parse_MailBox_DeleteNote( i3NetworkPacket * pPacket)
{
	INT32	result;			// ���� ���
	UINT8	deletedCount;	// ������ ���� ����

	pPacket->ReadData(&result, sizeof(INT32));
	pPacket->ReadData(&deletedCount, sizeof(UINT8));

	// ������ ���� Id
	for(UINT32 i = 0; i < deletedCount; i++)
	{
		INT32 noteId;
		pPacket->ReadData(&noteId, sizeof(INT32));
		MailBoxContext::i()->DeleteNote(noteId);
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_DELETE_NOTE);

	if( deletedCount > 0) 
	{
		i3::rc_wstring wstr_rcTemp;

		// sucess
		MailBoxContext::i()->SetNoteNewFlag();

		switch(result)
		{
		case EVENT_ERROR_SUCCESS:
			/*%d���� ������ �����Ǿ����ϴ�.*/ 
			i3::sprintf( wstr_rcTemp, GAME_RCSTRING("STR_TBL_NETWORK_COMPLETE_DELETE_MESSAGE_BY_COUNT"), deletedCount);
			GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_rcTemp);
			break;
		case EVENT_RECEIVE_CLAN_NOTE:					
			{
				GameEventReceiver::i()->PushReceivedGameEvent(EVENT_DELETE_NOTE_ASK);						
			}				
			break;
		default:					
			break;
		}				
	}
	else
	{
		/*���� ������ �����Ͽ����ϴ�.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_DELETE_MESSAGE"));
	}
}

void ClientTCPSocket::__Parse_MailBox_ReceiveNote( i3NetworkPacket * pPacket)
{
	// Ŀ�´�Ƽ �ɼ��� ������ ���� ������ ������Ʈ���� �����մϴ�.
	switch( g_pEnvSet->m_nMessageType)
	{
	// ��ü����
	case 1:
		// ��� ������ �����ϰ� �����մϴ�.
		return;
		break;

	// ģ��&Ŭ�����
	case 2:
		// �۾��ؾ��մϴ�~!
		break;
	// ��ü���
	default:
		break;
	}

	// ������ �ش� �ޱ�
	NOTE_RECEIVE* pNote = nullptr;

	pNote = (NOTE_RECEIVE*)i3MemAlloc( sizeof(NOTE_RECEIVE));				
	pPacket->ReadData(&pNote->header, sizeof(NOTE_HEADER));			

	// ���� ���ڿ� ���� �ޱ�
	pPacket->ReadData(&pNote->body._senderSize, sizeof(UINT8));
	pPacket->ReadData(&pNote->body._messageSize, sizeof(UINT8));

	// ���� ���ڿ� �ޱ�
	pPacket->ReadData(pNote->body._sender, pNote->body._senderSize * sizeof(TTCHAR));

	// ���ڿ�
	if( 0 < pNote->body._messageSize )
	{
		pPacket->ReadData(pNote->body._message, pNote->body._messageSize * sizeof(TTCHAR));
		if (pNote->header._type == NT_GIFT)
			MailBoxContext::i()->ConvertEventGiftMessage(pNote->body, pNote->body._message);
		pNote->body._messageSize = (UINT8)(i3::generic_string_size(pNote->body._message) + 1);
	}
	else
	{
		pPacket->ReadData(&pNote->body._message[0], sizeof( TTCHAR ) );
		pPacket->ReadData(&pNote->body._message[1], pNote->body._message[0] * sizeof(TTCHAR));
	}

	MailBoxContext::i()->UpdateClanMessage( pNote );

	MailBoxContext::i()->AddReceiveNote( pNote);

	MailBoxContext::i()->SetNoteNewFlag();

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RECEIVE_NOTE);
}

void ClientTCPSocket::__Parse_Auth_FCMInfo( i3NetworkPacket * pPacket)
{
}

void ClientTCPSocket::__Parse_Auth_FCMState( i3NetworkPacket * pPacket)
{
	////2011-03-18 �ϵ��� : ��ȣ�۾�
	//INT32 FCMState;
	//		
	//pPacket->ReadData(&FCMState, sizeof(INT32));

	////ä��â�� Ȱ��ȭ �Ǿ��ִ� ������ ä��â�� ��89�� �־��ֱ�

	//if(FCM_NOMAL_STATE1 <= FCMState && FCMState < FCM_NOMAL_STATE2)
	//{
	//	//�������� �ð��� 1�ð��� �Ǿ����ϴ�. 
	//	g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_CHINA_PLAYTIME_1HOURS"));
	//	g_pFramework->SetAnnounceMessage2(GAME_RCSTRING("STR_CHINA_PLAYTIME_1HOURS"));
	//}
	//else if(FCM_NOMAL_STATE2 <= FCMState && FCMState < FCM_NOMAL_STATE3)
	//{
	//	//�������� �ð��� 2�ð��� �Ǿ����ϴ�. 
	//	g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_CHINA_PLAYTIME_2HOURS"));
	//	g_pFramework->SetAnnounceMessage2(GAME_RCSTRING("STR_CHINA_PLAYTIME_2HOURS"));
	//}
	//else if(FCM_NOMAL_STATE3 <= FCMState && FCMState < FCM_TIRED_STATE)
	//{
	//	//�������� �ð��� 3�ð��� �Ǿ����ϴ�. �������� �� �޽��� ���ϰų� ������ ��ü Ȱ���� �Ͻñ� �ٶ��ϴ�.
	//	g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_CHINA_PLAYTIME_3HOURS"));
	//	g_pFramework->SetAnnounceMessage2(GAME_RCSTRING("STR_CHINA_PLAYTIME_3HOURS"));
	//}
	//else if(FCM_TIRED_STATE <= FCMState && FCMState < FCM_UNHEALTHY_STATE)
	//{
	//	//�Ƿνð��밡 �Ǿ����ϴ�. ����� ���Ӽ����� ����ġ�� 50%�� �����մϴ�. �ǰ��� ���� �������� �� �޽��� ���ϰų� ������ ��ü Ȱ���� �Ͻñ� �ٶ�� �н��� ������ ���� ��ε帳�ϴ�. 
	//	g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_CHINA_PLAYTIME_4HOURS"));
	//	g_pFramework->SetAnnounceMessage2(GAME_RCSTRING("STR_CHINA_PLAYTIME_4HOURS"));
	//}
	//else if(FCM_UNHEALTHY_STATE <= FCMState)
	//{
	//	//��ǰ��ð��밡 �Ǿ����ϴ�. �ǰ��� ���� �ٷ� ���������� �������ֽñ� �ٶ��ϴ�. ������ �������� ���� ���, �ǰ��� �ɰ��� ��ġ�� �˴ϴ�. ����� ������ ���� 0%�̸� ���� �������� �ð��� �� 5�ð��� �� �Ŀ� �������� ȸ���˴ϴ�. 
	//	g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_CHINA_PLAYTIME_5HOURS"));
	//	g_pFramework->SetAnnounceMessage2(GAME_RCSTRING("STR_CHINA_PLAYTIME_5HOURS"));
	//}
}

void ClientTCPSocket::__Parse_Auth_AddBlockUser( i3NetworkPacket * pPacket)
{
	INT32 result;

	pPacket->ReadData(&result, sizeof(INT32));

	if (EV_SUCCESSED(result))
	{
		//���� �߰� �����Ͽ����ϴ�.
		BLOCK_USER t_BlockUser;

		pPacket->ReadData(&t_BlockUser._blockInfo._Idx, sizeof(INT32));
		pPacket->ReadData(&t_BlockUser._blockInfo._rank, sizeof(UINT8));
		
		UINT8 nickSize;
		pPacket->ReadData(&nickSize, sizeof(UINT8));
		TTCHAR szNick[NET_NICK_NAME_SIZE];
		pPacket->ReadData(szNick, sizeof(TTCHAR) * nickSize);

		t_BlockUser._wstrNick = szNick;

		UINT8 commentsize;
		pPacket->ReadData(&commentsize, sizeof(UINT8));

		TTCHAR szComment[BLOCK_COMMENT_MAX];

		pPacket->ReadData(szComment, sizeof(TTCHAR) * commentsize);

		t_BlockUser._wstr_comment = szComment;
		t_BlockUser._Activated = true;

		CCommunityContext::i()->InsertBlockUser(&t_BlockUser);
	}
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INSERT_BLOCK, result);
}

void ClientTCPSocket::__Parse_Auth_DeleteBlockUser( i3NetworkPacket * pPacket)
{
	INT32 result, idx;

	pPacket->ReadData(&result, sizeof(INT32));
	pPacket->ReadData(&idx, sizeof(INT32));
	if (EV_SUCCESSED(result))
	{
		INT32 d_index = CCommunityContext::i()->Find_DeleteIndex(idx);
		CCommunityContext::i()->DeleteBlockUser(d_index);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_UI_BLOCK_DELETE_FAIL_DEFAULT"));
	}
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_DELETE_BLOCK, result);
}

void ClientTCPSocket::__Parse_Auth_AccountKick( i3NetworkPacket * pPacket)
{
	UINT8 nReason;
	pPacket->ReadData(&nReason, sizeof(UINT8));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_DRIVE_AWAY, nReason);
}


void ClientTCPSocket::__Parse_Auth_UseItem_CheckNick( i3NetworkPacket * pPacket)
{
	INT32 result;

	pPacket->ReadData(&result, sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_USE_ITEM_CHECK_NICK, result);
}

