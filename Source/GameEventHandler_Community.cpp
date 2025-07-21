#include "pch.h"
#include "GameEventHandler.h"

#include "CommunityContext.h"
#include "LobbyContext.h"
#include "UserInfoContext.h"

#include "UI/UIUtil.h"

#include "i3Base/string/ext/utf16_to_mb.h"


const char * CGameEventHandler::__getCommunityEventName( GAME_EVENT evt) const
{
	switch( evt)
	{
	case EVENT_INSERT_FRIEND : return "EVENT_INSERT_FRIEND";
	case EVENT_ACCEPT_FRIEND : return "EVENT_ACCEPT_FRIEND";
	case EVENT_DELETE_FRIEND : return "EVENT_DELETE_FRIEND";
	case EVENT_INVITE_FRIEND : return "EVENT_INVITE_FRIEND";
	case EVENT_INVITE_CLAN : return "EVENT_INVITE_CLAN";
	case EVENT_INSERT_BLOCK : return "EVENT_INSERT_BLOCK";
	case EVENT_DELETE_BLOCK : return "EVENT_DELETE_BLOCK";
	case EVENT_SEND_WHISPER : return "EVENT_SEND_WHISPER";
	case EVENT_SEND_MEGAPHONE : return "EVENT_SEND_MEGAPHONE";
	case EVENT_SEND_WHISPER_FIND_UID : return "EVENT_SEND_WHISPER_FIND_UID";
	case EVENT_FIND_USER : return "EVENT_FIND_USER";
	case EVENT_SEND_NOTE : return "EVENT_SEND_NOTE";
	case EVENT_SEND_NOTE_BY_UID : return "EVENT_SEND_NOTE_BY_UID";
	case EVENT_CHECK_READED_NOTE : return "EVENT_CHECK_READED_NOTE";
	case EVENT_DELETE_NOTE : return "EVENT_DELETE_NOTE";
	case EVENT_COMMUNITY_RECORD_DB_N : return "EVENT_COMMUNITY_RECORD_DB_N";
	case EVENT_INVITE_NORMAL_USER : return "EVENT_INVITE_NORMAL_USER";
	default :			return nullptr;
	}
}

void CGameEventHandler::__register_Community( void)
{
	regSendEvent( EVENT_INSERT_FRIEND,				&CGameEventHandler::__evtComm_AddFriend);
	regSendEvent( EVENT_ACCEPT_FRIEND,				&CGameEventHandler::__evtComm_AcceptFriend);
	regSendEvent( EVENT_DELETE_FRIEND,				&CGameEventHandler::__evtComm_DeleteFriend);
	regSendEvent( EVENT_INVITE_FRIEND,				&CGameEventHandler::__evtComm_InviteFriend);
	regSendEvent( EVENT_INVITE_CLAN,  				&CGameEventHandler::__evtComm_InviteClan);
	regSendEvent( EVENT_INSERT_BLOCK, 				&CGameEventHandler::__evtComm_AddBlock);
	regSendEvent( EVENT_DELETE_BLOCK, 				&CGameEventHandler::__evtComm_DeleteBlock);
	regSendEvent( EVENT_SEND_WHISPER, 				&CGameEventHandler::__evtComm_SendWhisper);
	regSendEvent( EVENT_SEND_WHISPER_FIND_UID,		&CGameEventHandler::__evtComm_SendWhisperWithUID);
	regSendEvent( EVENT_SEND_MEGAPHONE, 			&CGameEventHandler::__evtComm_SendMegaPhone);
	regSendEvent( EVENT_FIND_USER,					&CGameEventHandler::__evtComm_FindUser);
	regSendEvent( EVENT_SEND_NOTE,					&CGameEventHandler::__evtComm_SendNote);
	regSendEvent( EVENT_SEND_NOTE_BY_UID,			&CGameEventHandler::__evtComm_SendNoteWithUID);
	regSendEvent( EVENT_CHECK_READED_NOTE,			&CGameEventHandler::__evtComm_CheckReadedNote);
	regSendEvent( EVENT_DELETE_NOTE,				&CGameEventHandler::__evtComm_DeleteNote);
	regSendEvent( EVENT_COMMUNITY_RECORD_DB_N,		&CGameEventHandler::__evtComm_RecordDB);
	regSendEvent( EVENT_INVITE_NORMAL_USER,			&CGameEventHandler::__evtComm_InviteNormalUser);
}

bool CGameEventHandler::__evtComm_AddFriend( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_FRIEND_INSERT_REQ);

	const wchar_t* wszNick = static_cast<const wchar_t*>(pInfo->_pArg);

	TTCHAR szNick[NET_NICK_NAME_SIZE];
	::memset(szNick, 0, sizeof(szNick));

	i3::safe_string_copy(szNick, wszNick, NET_NICK_NAME_SIZE);

	pInfo->_SendPacket.WriteData(szNick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	return true;
}

bool CGameEventHandler::__evtComm_AcceptFriend( GAMEEVENTINFO * pInfo)
{
	UINT8 idx = (UINT8) (*(UINT32*)pInfo->_pArg);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_FRIEND_ACCEPT_REQ);
	pInfo->_SendPacket.WriteData(&idx, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtComm_DeleteFriend( GAMEEVENTINFO * pInfo)
{
	UINT8 idx = (UINT8) (*(UINT32*)pInfo->_pArg);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_FRIEND_DELETE_REQ);
	pInfo->_SendPacket.WriteData(&idx, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtComm_InviteFriend( GAMEEVENTINFO * pInfo)
{
	UINT8 idx = (UINT8) (*(UINT32*)pInfo->_pArg);
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_FRIEND_INVITED_REQ);
	pInfo->_SendPacket.WriteData(&idx, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtComm_InviteClan( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CS_ROOM_INVITED_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(T_UID));
	return true;
}

bool CGameEventHandler::__evtComm_AddBlock( GAMEEVENTINFO * pInfo)
{
	BLOCK_USER *t_Block = (BLOCK_USER *) pInfo->_pArg;
	INT32 _sessionIndex = LobbyContext::i()->getUserSessionIdxByUserNickName(t_Block->_wstrNick);
//	UINT8 nicknameSize = t_Block->_nicksize;						// ������ �ʴ� �͵� �ּ� ó�� (2014.07.10.����)

//	UINT8 messageSize = t_Block->_commentsize;

//	char message2[MAX_STRING_COUNT];
//	i3::string_ncopy_nullpad(message2, (char*)t_Block->_comment, messageSize);
//	message2[messageSize - 1] = '\0';
	
	const i3::rc_wstring& strMessage = t_Block->_wstr_comment;

	UINT8 messageSize = (UINT8)strMessage.size() + 1;				// ���� +1�� �ϴ� ó���� ����..
	
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_BLOCK_INSERT_REQ);
	pInfo->_SendPacket.WriteData(&_sessionIndex, sizeof(INT32));
	pInfo->_SendPacket.WriteData(&messageSize, sizeof(UINT8));
	pInfo->_SendPacket.WriteData(strMessage.c_str(), sizeof(TTCHAR) * messageSize);
	return true;
}

bool CGameEventHandler::__evtComm_DeleteBlock( GAMEEVENTINFO * pInfo)
{
	INT32 *idx = (INT32 *)pInfo->_pArg;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_BLOCK_DELETE_REQ);
	pInfo->_SendPacket.WriteData(idx, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtComm_SendWhisper( GAMEEVENTINFO * pInfo)
{
	// pArg	 - receiverNick				receiverNick�� i3::rc_wstring*  (��ü ������) �������ιٲ۴�..
	// pArg1 - message

	// �ڽſ��� ������ �ʴ´�.
	
	const i3::rc_wstring* pwstrNick = reinterpret_cast<const i3::rc_wstring*>(pInfo->_pArg);

	if (!UserInfoContext::i()->IsMyNickName(*pwstrNick) )
	{
		// �� �޽����� �ڸ���.
		INT16 length = (INT16) i3::generic_string_size( (TTCHAR*)pInfo->_pArg1 );
		length = MIN(length + 1, MAX_STRING_COUNT);

		TTCHAR message[MAX_STRING_COUNT];
		i3::string_ncopy_nullpad(message, (TTCHAR*)pInfo->_pArg1, length);
		message[length - 1] = '\0';

		// �ӼӸ� ��û
		pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_SEND_WHISPER_REQ);

		TTCHAR szNick[NET_NICK_NAME_SIZE];	memset(szNick, 0, sizeof(szNick) );
		i3::safe_string_copy(szNick, *pwstrNick, NET_NICK_NAME_SIZE);
		pInfo->_SendPacket.WriteData(szNick, NET_NICK_NAME_SIZE * sizeof(TTCHAR));

		pInfo->_SendPacket.WriteData(&length, sizeof(INT16));
		pInfo->_SendPacket.WriteData(message, length * sizeof(TTCHAR));

		return true;
	}
	else
	{
		I3TRACE("Fail Whisper - not send self nick\n");
	}

	return false;
}

bool CGameEventHandler::__evtComm_SendWhisperWithUID( GAMEEVENTINFO * pInfo)
{
	// pArg	 - receiverNick		receiverNick�� i3::rc_wstring*  (��ü ������) �������ιٲ۴�..
	// pArg1 - message
	// pArg2 - receiverUID			

	// �ڽſ��� ������ �ʴ´�.
	const i3::rc_wstring* pwstrNick = reinterpret_cast<const i3::rc_wstring*>(pInfo->_pArg);

	if (!UserInfoContext::i()->IsMyNickName(*pwstrNick))
	{
		T_UID ReceiverUID = (T_UID) (*(T_UID*)pInfo->_pArg2);

		// �� �޽����� �ڸ���.
		INT16 length = (INT16) i3::generic_string_size( (TTCHAR*)pInfo->_pArg1 );
		length = MIN(length + 1, MAX_STRING_COUNT);

		TTCHAR message[MAX_STRING_COUNT];

		i3::string_ncopy_nullpad(message, (TTCHAR*)pInfo->_pArg1, length);
		message[length - 1] = '\0';

		// �ӼӸ� ��û
		pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_SEND_WHISPER_FIND_UID_REQ);
		pInfo->_SendPacket.WriteData(&ReceiverUID, sizeof(T_UID));

		TTCHAR szNick[NET_NICK_NAME_SIZE];
		memset(szNick, 0, sizeof(szNick));
		i3::safe_string_copy(szNick, *pwstrNick, NET_NICK_NAME_SIZE);

		pInfo->_SendPacket.WriteData(szNick, NET_NICK_NAME_SIZE * sizeof(TTCHAR) );
		pInfo->_SendPacket.WriteData(&length, sizeof(INT16));
		pInfo->_SendPacket.WriteData(message, length*sizeof(TTCHAR));
		return true;
	}
	else
	{
		I3TRACE("Fail Whisper - not send self nick\n");
	}

	return false;
}

bool CGameEventHandler::__evtComm_SendMegaPhone( GAMEEVENTINFO * pInfo)
{
	// pArg	 - wareDBIdx				receiverNick�� i3::rc_wstring*  (��ü ������) �������ιٲ۴�..
	// pArg1 - message
	// pArg2 - message size

	// �� �޽����� �ڸ���.
	INT16 length = (INT16) i3::generic_string_size( (TTCHAR*)pInfo->_pArg1 );
	length = MIN(length + 1, MAX_STRING_COUNT);
	
	TTCHAR message[MAX_STRING_COUNT];
	i3::string_ncopy_nullpad(message, (TTCHAR*)pInfo->_pArg1, length);
	message[length - 1] = '\0';

	if( wcslen (message) > 50)
	{	
		i3InputKeyboard * pKeyboard = g_pFramework->getKeyboard();
		if( pKeyboard->GetStrokeState(I3I_KEY_ENTER) == true) 
			pKeyboard->SetStrokeState(I3I_KEY_ENTER, false); 

		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_CHATMESSAGE_OVER"));
		return false;
	}

	UINT8 i32Size = *((UINT8*)pInfo->_pArg2);
	// �ӼӸ� ��û
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(T_ItemDBIdx));
	pInfo->_SendPacket.WriteData(&i32Size, sizeof(UINT8));
	pInfo->_SendPacket.WriteData(message, i32Size);

	return true;
}

bool CGameEventHandler::__evtComm_FindUser( GAMEEVENTINFO * pInfo)
{
	const wchar_t* wszNick = static_cast<const wchar_t*>(pInfo->_pArg);

	TTCHAR szNick[NET_NICK_NAME_SIZE];
	::memset(szNick, 0, sizeof(szNick));

	i3::safe_string_copy(szNick, wszNick, NET_NICK_NAME_SIZE);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_FIND_USER_REQ);
	pInfo->_SendPacket.WriteData( szNick, NET_NICK_NAME_SIZE);

	// pArg - findUserNick
	/*pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_FIND_USER_REQ);
	pInfo->_SendPacket.WriteData((wchar_t*)pInfo->_pArg, NET_NICK_NAME_SIZE);*/
	return true;
}

bool CGameEventHandler::__evtComm_SendNote( GAMEEVENTINFO * pInfo)
{
	// pArg	 - NOTE_BODY <- structure
	NOTE_BODY * pNote = (NOTE_BODY*) pInfo->_pArg;

	// �޴� ����� ���� ������ ������.
	UINT8 nicknameSize = pNote->_senderSize;
	UINT8 messageSize = pNote->_messageSize;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_MESSENGER_NOTE_SEND_REQ);
	pInfo->_SendPacket.WriteData(&nicknameSize, sizeof(UINT8));
	pInfo->_SendPacket.WriteData(&messageSize, sizeof(UINT8));
	pInfo->_SendPacket.WriteData(pNote->_sender, nicknameSize * sizeof(TTCHAR));
	pInfo->_SendPacket.WriteData(pNote->_message, messageSize * sizeof(TTCHAR));
	return true;
}

bool CGameEventHandler::__evtComm_SendNoteWithUID( GAMEEVENTINFO * pInfo)
{
	// pArg	 - NOTE_BODY <- structure
	NOTE_BODY_BY_UID * pNote = (NOTE_BODY_BY_UID*) pInfo->_pArg;

	// �޴� ��� UID�� ���� ������ ������.
	pInfo->_SendPacket.SetProtocol(PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ);
	pInfo->_SendPacket.WriteData(&pNote->_receiverUid, sizeof(T_UID));
	pInfo->_SendPacket.WriteData(&pNote->_messageSize, sizeof(UINT8));
	pInfo->_SendPacket.WriteData(pNote->_message, pNote->_messageSize * sizeof(TTCHAR));
	return true;
}

bool CGameEventHandler::__evtComm_CheckReadedNote( GAMEEVENTINFO * pInfo)
{
	// ����ǥ���� ���� ������ ������.
	UINT8 readCount = (UINT8) (*(UINT8*) pInfo->_pArg);
	const INT32 * pNoteID = static_cast<const INT32*>(pInfo->_pArg1);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ);
	pInfo->_SendPacket.WriteData(&readCount, sizeof(UINT8));

	// ����ǥ���� �� ������ Id�� ������.
	pInfo->_SendPacket.WriteData(pNoteID, sizeof(INT32) * readCount);
	return true;
}

bool CGameEventHandler::__evtComm_DeleteNote( GAMEEVENTINFO * pInfo)
{
	// ������ ���� ������ ������.
	UINT8 deleteCount = (UINT8) (*(UINT8*) pInfo->_pArg);
	INT32 * noteId = (INT32*)pInfo->_pArg1;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_MESSENGER_NOTE_DELETE_REQ);
	pInfo->_SendPacket.WriteData(&deleteCount, sizeof(UINT8));

	// ������ Id�� ������.
	pInfo->_SendPacket.WriteData(noteId, sizeof(INT32)*deleteCount);
	return true;
}

bool CGameEventHandler::__evtComm_RecordDB( GAMEEVENTINFO * pInfo)
{
	i3mem::FillZero( CCommunityContext::i()->SetCommnuitRecord(), sizeof(USER_INFO_RECORD));
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(T_UID));
	return true;
}

bool CGameEventHandler::__evtComm_RecordSession( GAMEEVENTINFO * pInfo)
{
	i3mem::FillZero( CCommunityContext::i()->SetCommnuitRecord(), sizeof(USER_INFO_RECORD));

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_GET_RECORD_INFO_SESSION_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(INT32));

	return true;
}

bool CGameEventHandler::__evtComm_InviteNormalUser( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_COMMUNITY_USER_INVITED_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_COMMUNITY_USER_INVITED_REQ));

	return true;
}