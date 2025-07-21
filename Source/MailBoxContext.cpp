#include "pch.h"
#include "MailBoxContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "i3Base/string/ext/lrtrim.h"
#include "i3Base/string/ext/atoi.h"

//

bool _Note::comp_idx( const _Note& lhs, const _Note& rhs )
{
	return rhs.pNote->header._id < lhs.pNote->header._id;
}

bool _Note::comp_sender( const _Note& lhs, const _Note& rhs )
{
	return 0 > i3::generic_icompare( lhs.pNote->body._sender, rhs.pNote->body._sender );
}

bool _Note::comp_sender_rev( const _Note& lhs, const _Note& rhs )
{
	return 0 < i3::generic_icompare( lhs.pNote->body._sender, rhs.pNote->body._sender );
}

bool _Note::comp_period( const _Note& lhs, const _Note& rhs )
{
	const GIFT_BUFFER * pLeftGift = reinterpret_cast<GIFT_BUFFER*>(lhs.pNote->body._user); 
	const GIFT_BUFFER * pRightGift = reinterpret_cast<GIFT_BUFFER*>(rhs.pNote->body._user); 

	return pLeftGift->_ui32Expire > pRightGift->_ui32Expire;
}

bool _Note::comp_period_rev( const _Note& lhs, const _Note& rhs )
{
	const GIFT_BUFFER * pLeftGift = reinterpret_cast<GIFT_BUFFER*>(lhs.pNote->body._user); 
	const GIFT_BUFFER * pRightGift = reinterpret_cast<GIFT_BUFFER*>(rhs.pNote->body._user); 

	return pLeftGift->_ui32Expire < pRightGift->_ui32Expire;
}

bool _Note::comp_gift_state( const _Note& lhs, const _Note& rhs )
{
	const GIFT_BUFFER * pLeftGift = reinterpret_cast<GIFT_BUFFER*>(lhs.pNote->body._user); 
	const GIFT_BUFFER * pRightGift = reinterpret_cast<GIFT_BUFFER*>(rhs.pNote->body._user); 

	return pLeftGift->_eDeliver > pRightGift->_eDeliver;
}

bool _Note::comp_gift_state_rev( const _Note& lhs, const _Note& rhs )
{
	const GIFT_BUFFER * pLeftGift = reinterpret_cast<GIFT_BUFFER*>(lhs.pNote->body._user); 
	const GIFT_BUFFER * pRightGift = reinterpret_cast<GIFT_BUFFER*>(rhs.pNote->body._user); 

	return pLeftGift->_eDeliver < pRightGift->_eDeliver;
}


MailBoxContext::~MailBoxContext()
{
	i3::cu::for_each_SafeFree_clear(m_NoteReceiveList); 
	i3::cu::for_each_SafeFree_clear(m_NoteGiftList);

	m_GiftReqList.clear();
}


bool MailBoxContext::AddNote( const NOTE_RECEIVE * pNew, bool bAlram)
{
	NOTE_RECEIVE * pNote = (NOTE_RECEIVE*)i3MemAlloc( sizeof(NOTE_RECEIVE));
	i3mem::Copy( pNote, pNew, sizeof( NOTE_RECEIVE));

	AddReceiveNote( pNote);
	this->SetNoteNewFlag();

	if(bAlram)
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RECEIVE_NOTE);

	return true;
}

bool MailBoxContext::DeleteNote( INT32 noteId)
{
	for( UINT32 x = 0; x < (UINT32)GetNoteReceiveCount(); x++)
	{
		NOTE_RECEIVE* pNote = (NOTE_RECEIVE*)GetReceiveNote(x);
		if( pNote->header._id == noteId)
		{
			I3MEM_SAFE_FREE( pNote);
			i3::vu::erase_index(m_NoteReceiveList, x);
			return true;
		}
	}

	return false;
}

//bool MailBoxContext::DeleteAllGift( INT32 noteId)
//{
//	for( UINT32 x = 0; x < (UINT32)GetAllNoteReceiveCount(); x++)
//	{
//		NOTE_RECEIVE* pNote = (NOTE_RECEIVE*)GetReceiveAllNote(x);
//		if( pNote->header._id == noteId)
//		{
//			I3MEM_SAFE_FREE( pNote);
//			i3::vu::erase_index(m_AllNoteReceiveList, x);
//			return true;
//		}
//	}
//
//	return false;
//
//	INT32	nCount	= GetNoteGiftCount();
//
//	for( INT32 i = 0; i < nCount; i++)
//	{
//		NOTE_GIFT* pNoteGift	= (NOTE_GIFT*)GetNoteGift( i);
//		if( pNoteGift->gift._i32GiftWareDBIdx == GiftDBIdx )
//		{
//			I3MEM_SAFE_FREE( pNoteGift);
//			i3::vu::erase_index(m_NoteGiftList, i);
//			return true;
//		}
//	}
//
//	return false;
//}

// �̰� �ѹ��� �ʿ��� ������ ���̴µ�....
// ������ ȣ����� �ʰ� �����ڿ� ���� �ʿ䰡 ����...�ϴ��� ��ġ...
// �����ڼ��ؿ����� ���� �ʿ��� ��Ʈ������ ������ ����, ���� �ε��ڿ��� �����ѵ� �ϴ�..
// 

void MailBoxContext::CreateClanMessageHandle()
{
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_ACCEPT ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_DENIAL ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_ACCEPT ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_DENIAL ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_SECESSION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_SECESSION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_DEPORTATION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_DEPORTATION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_MASTER ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_MASTER");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_STAFF ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_STAFF");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_REGULAR ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_REGULAR");
}



void MailBoxContext::UpdateClanMessage()
{
	for( UINT32 x = 0; x < (UINT32)GetNoteReceiveCount(); x++)
	{
		UpdateClanMessage((NOTE_RECEIVE*)GetReceiveNote(x));
	}
}

void MailBoxContext::UpdateClanMessage( NOTE_RECEIVE* pNote )
{
	if( nullptr == pNote )
		return ;

	i3::stack_wstring wstrMessage;

	if( 0 >= pNote->body._messageSize )
	{
		UINT8* pcSize		= (UINT8*)pNote->body._message;
		UINT8* pcMessazeType = (UINT8*)&pNote->body._message[1];

		switch( *pcMessazeType )
		{
		case NOTE_MESSAGE_CLAN_INVITE_ACCEPT:	// "�����մϴ�!\nŬ�� ������ ���εǾ����ϴ�.\n��ſ� Ŭ�� Ȱ���� ��⼼��."
		case NOTE_MESSAGE_CLAN_MASTER:			// "Ŭ�� ������ ������ ���ӵǾ����ϴ�.\nŬ�� �����Ͱ� �ǽ� ���� ���� �մϴ�."
		case NOTE_MESSAGE_CLAN_STAFF:			// "�����մϴ�.\nŬ�� �����Ͱ� �������� �Ӹ��Ͽ����ϴ�."
		case NOTE_MESSAGE_CLAN_REGULAR:			// "Ŭ�� �����Ͱ� �Ϲ�ȸ������ �Ӹ��Ͽ����ϴ�."
			wstrMessage = GAME_RCSTRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]);
			break;
		case NOTE_MESSAGE_CLAN_INVITE:			// "[%s] Ŭ������ ����\nŬ�����κ��� ������ �ʴ�޾ҽ��ϴ�.\n���Կ� �����Ͻðڽ��ϱ�?"
		case NOTE_MESSAGE_CLAN_INVITE_DENIAL:	// "[%s]\nŬ�����κ��� ���� ��û��\n�ź� �Ǿ����ϴ�."
		case NOTE_MESSAGE_CLAN_DEPORTATION:		// "[%s]\nŬ������ ����Ǿ����ϴ�."
			{
				i3::sprintf( wstrMessage, GAME_RCSTRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), pNote->body._sender );
			}
			break;
		case NOTE_MESSAGE_CLAN_JOIN_ACCEPT:		// "[%s] �Բ���\nŬ�� ���� �ʴ븦 �����Ͽ����ϴ�."
		case NOTE_MESSAGE_CLAN_JOIN_DENIAL:		// "[%s] �Բ���\nŬ�� ���� ��û�� �ź��Ͽ����ϴ�."
		case NOTE_MESSAGE_CLAN_SECESSION:		// "[%s] �Բ���\nŬ������ Ż���Ͽ����ϴ�."
			{
				TTCHAR szName[ NET_NICK_NAME_SIZE ];
				i3::string_ncopy_nullpad( szName, &pNote->body._message[2], *pcSize - 1 );
				i3::sprintf( wstrMessage, GAME_RCSTRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), szName );
			}
			break;
		}
		
		const i3::stack_wstring& strMessage = wstrMessage;

		pNote->body._messageSize = UINT8(sizeof(pNote->body._message) / sizeof(TTCHAR));
		i3::safe_string_copy( pNote->body._message, strMessage, pNote->body._messageSize );			
	}
}


// ���� ���� �߰�
bool MailBoxContext::AddNoteGift( NOTE_GIFT * pnoteNew, bool bAlram /*= false*/)
{


	I3ASSERT( pnoteNew != nullptr );

	m_NoteGiftList.push_back( pnoteNew );
	this->SetNoteNewFlag();

	if(bAlram)
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RECEIVE_NOTE);

	return true;
}

// ���� ���� ����
bool MailBoxContext::DeleteNoteGift( T_GiftDBIdx GiftDBIdx )
{
	INT32	nCount	= GetNoteGiftCount();

	for( INT32 i = 0; i < nCount; i++)
	{
		NOTE_GIFT* pNoteGift	= (NOTE_GIFT*)GetNoteGift( i);
		if( pNoteGift->gift._i32GiftWareDBIdx == GiftDBIdx )
		{
			I3MEM_SAFE_FREE( pNoteGift);
			i3::vu::erase_index(m_NoteGiftList, i);
			return true;
		}
	}

	return false;
}

void MailBoxContext::UpdateNoteGift(AUTH_GIFT * pAuthGift)
{
	INT32 nCount = GetNoteGiftCount();

	for( INT32 i = 0; i < nCount; i++)
	{
		NOTE_GIFT* pNoteGift = (NOTE_GIFT*)GetNoteGift( i);

		if( pNoteGift->gift._i32GiftWareDBIdx == pAuthGift->m_TDBIdx )
		{
			pNoteGift->gift._eDeliver = static_cast<ENUM_TYPE_GIFT_DELIVER>(pAuthGift->m_ui8Deliver);
			pNoteGift->gift._ui32Expire = pAuthGift->m_ui32Expire;
			
			switch( pAuthGift->m_ui8Deliver ) 
			{
			case TYPE_GIFT_DELIVER_READ		: pNoteGift->note.header._state	= NS_READED; break;
			case TYPE_GIFT_DELIVER_RECEIVED	: pNoteGift->note.header._state	= NS_READED; break;
			default							: pNoteGift->note.header._state	= NS_UNREAD; break;
			}
		}
	}
}

void MailBoxContext::DeleteOldNoteGift(void)
{
	INT32 i32DeleteIdx = 0;
	NOTE_GIFT * pDeleteGift = (NOTE_GIFT*)GetNoteGift(i32DeleteIdx);

	INT32	nCount	= GetNoteGiftCount();
	for( INT32 idx = 0; idx < nCount; ++idx)
	{
		NOTE_GIFT * pNoteGift = (NOTE_GIFT*)GetNoteGift(idx);

		if(pDeleteGift->gift._i32GiftWareDBIdx > pNoteGift->gift._i32GiftWareDBIdx)
		{
			pDeleteGift = pNoteGift;
			i32DeleteIdx = idx;
		}
	}

	I3MEM_SAFE_FREE( pDeleteGift);
	i3::vu::erase_index(m_NoteGiftList, i32DeleteIdx);
}

void MailBoxContext::AddGift( const GIFT_BUFFER& Gift, const UINT8 nSenderNickSize, const UINT8 nMessageSize )
{
	// revision 30185
	if( m_NoteGiftList.size() >= MAX_SHOP_USER_GIFT_COUNT ) 
	{
		I3PRINTLOG(I3LOG_NOTICE, "Gift Note Count OverFlow" );
		DeleteOldNoteGift();
	}

	NOTE_GIFT*		pnoteGift	= (NOTE_GIFT*)i3MemAlloc(sizeof(NOTE_GIFT));
	NOTE_RECEIVE*	pnote		= &(pnoteGift->note);
	GIFT_BUFFER*	pgift		= &(pnoteGift->gift);

	i3mem::FillZero( pnoteGift, sizeof( NOTE_GIFT));

	// ���� ����
	i3mem::Copy( pgift, &Gift, sizeof(GIFT_BUFFER));

	// ���� ���
	pnote->header._id			= pgift->_i32GiftWareDBIdx;
	pnote->header._type			= NT_GIFT;

	switch( Gift._eDeliver ) 
	{
	case TYPE_GIFT_DELIVER_READ		: pnote->header._state	= NS_READED; break;
	case TYPE_GIFT_DELIVER_RECEIVED	: pnote->header._state	= NS_READED; break;
	default							: pnote->header._state	= NS_UNREAD; break;
	}

	UINT32		unSecond	= UserInfoContext::i()->GetInventorySecTime( Gift._ui32Expire );

	if( 0 > (INT32)unSecond ) unSecond = 0;
	if( unSecond >= ITEM_TERM_ADJUST_VALUE) unSecond -= ITEM_TERM_ADJUST_VALUE;

	pnote->header._remainDay	= (UINT8)ConvertSecToUseDay( unSecond );


	// ���� �ٵ�
	pnote->body._user			= pgift;		//	�����ϱ� Note���� pNote->body._user �����ʹ� GIFT_BUFFER ����ü ��������.

	i3::string_ncopy_nullpad( pnote->body._sender, Gift._strSenderNick, nSenderNickSize );
	pnote->body._senderSize		= nSenderNickSize;

	i3::stack_wstring wstrMessage	= Gift._strMessage;

	i3::lrtrim( wstrMessage );

	if( wstrMessage.size() > 0 )	// �޼����� ������ 
	{
		i3::string_ncopy_nullpad( pnote->body._message, wstrMessage, nMessageSize );

		pnote->body._messageSize = nMessageSize;

		ConvertEventGiftMessage(pnote->body, wstrMessage);
	}
	else						// �޼����� ������ �⺻ �޼���
	{
		// [%s] �Բ��� ���� ������ �����߽��ϴ�.\n���� ������ �κ��丮�� ����˴ϴ�.
		i3::sprintf(wstrMessage, GAME_RCSTRING("STR_SHOP_NOTE_RECEIVE_GIFT"), pnote->body._sender);

//		i3::snprintf( pnote->body._message, NET_NOTE_SIZE, GAME_RCSTRING("STR_SHOP_NOTE_RECEIVE_GIFT"), pnote->body._sender );
		const i3::stack_wstring& strMessage = wstrMessage;
		i3::safe_string_copy(pnote->body._message, strMessage, NET_NOTE_SIZE);

		pnote->body._messageSize	= (UINT8)(i3::generic_string_size( pnote->body._message) + 1);								
	}

	// ���� ��� Ÿ��
	if( i3::generic_is_equal( pnote->body._sender, _TT("GM") )  )
	{
		pnoteGift->sendertype	= GST_GM;
	}
	else
	{
		pnoteGift->sendertype	= GST_USER;
	}

	AddNoteGift( pnoteGift );
}

void MailBoxContext::ConvertEventGiftMessage(NOTE_BODY & pOut, const i3::wliteral_range& wMsgRng)
{
	i3::wliteral_range check_msg = L"EVENT_";

	if( i3::contain_string_head(wMsgRng, check_msg) == true )
	{
//		i3::string strEvtNum = strMessage.substr( check_msg.length() );

		i3::wliteral_range wstrEvtNum( wMsgRng.begin() + check_msg.size(), wMsgRng.end() );

		INT32 i32Event = i3::atoi( wstrEvtNum );
		
		if (i32Event == ITEM_INSERT_REQUEST_UNKNOWN) 
			return;

		i3::stack_wstring wstrEventName;
		i3::stack_wstring wstrEventMsg;

		bool bExistEventName = true;

		switch(i32Event)
		{
			case ITEM_INSERT_REQUEST_TS_BONUS		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_TS"); break;
			case ITEM_INSERT_REQUEST_RANKUP			: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_RANKUP"); break;
			case ITEM_INSERT_REQUEST_MISSION_CARD	: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_MISSION_CARD"); break;
			case ITEM_INSERT_REQUEST_GET_TITLE		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_TITLE"); break;
			case ITEM_INSERT_REQUEST_USER_GIFT		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_GIFT"); break;
			case ITEM_INSERT_REQUEST_ATTENDANCE		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_ATTENDANCE"); break;
			
			case ITEM_INSERT_REQUEST_EVENT_CONNECT	: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_LOGIN"); break;
			case ITEM_INSERT_REQUEST_EVENT_NEWUSER	: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_NEWUSER"); break;
			case ITEM_INSERT_REQUEST_WEB_GIFT		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_GMSEND"); break;
			case ITEM_INSERT_REQUEST_DORMANT		: wstrEventName = GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_DORMANT"); break;

			case ITEM_INSERT_REQUEST_BUY			:
			case ITEM_INSERT_REQUEST_CREATE_NEWUSER :
			case ITEM_INSERT_REQUEST_CAPSULE		:
			case ITEM_INSERT_REQUEST_GACHA			: 
			case ITEM_INSERT_REQUEST_MEDAL_REWARD	: 
			case ITEM_INSERT_REQUEST_DOMI_REWARD	: 
			case ITEM_INSERT_REQUEST_AUTH			:
			default									:
				{
					bExistEventName = false;
					
					if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
						wstrEventMsg = GAME_RCSTRING("STR_POPUP_GIFTPRESENT");
					else
						wstrEventMsg = GAME_RCSTRING("STR_POPUP_MESSAGE_EVENT_PRIZE_PAY");
				}
				break;
		}

		if (!UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT)) //�������� ���� ���� ��� �ش� ��Ʈ������ ����
			wstrEventMsg = GAME_RCSTRING("STR_POPUP_MESSAGE_EVENT_PRIZE_PAY");

		else if (bExistEventName)
			i3::sprintf(wstrEventMsg, GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_NAME"), wstrEventName);
		
		const i3::rc_wstring& strEventMsg = wstrEventMsg;
		i3::safe_string_copy(pOut._message, strEventMsg, sizeof(pOut._message) / sizeof(TTCHAR));

		pOut._messageSize = (UINT8)i3::generic_string_size(pOut._message);
	}

	
	else if(UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
	{
		i3::stack_wstring wstrGMMsg;
		i3::stack_wstring wstrEventMsg;

		if (i3::generic_is_equal(pOut._sender, _TT("GM"))) //������ ���� GM���� �±�
			i3::sprintf(wstrEventMsg, GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_NAME"), GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_GMSEND"));
		else                                               //�����鳢�� �������� �̿��ؼ� �����ص� �±׸� �ٿ���� ��
			i3::sprintf(wstrEventMsg, GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_NAME"), GAME_RCSTRING("STR_POPUP_GIFT_SUCCESS_EVENT_GIFT"));

		i3::sprintf(wstrGMMsg, L"%s %s", wstrEventMsg, pOut._message);

		const i3::rc_wstring& strConverMsg = wstrGMMsg;
		i3::safe_string_copy(pOut._message, strConverMsg, sizeof(pOut._message) / sizeof(TTCHAR));
	}
}

void MailBoxContext::ResetGiftReqList(void)
{ 
	m_GiftReqList.clear();
	m_i32ResultCount = 0; 
	m_ui8GiftReqType = 0;
}

void MailBoxContext::BuildGiftReqList(UINT8 type, INT32 * list, INT32 count)
{
	m_ui8GiftReqType = type;

	for(INT32 idx = 0 ; idx < count ; ++idx)
	{
		m_GiftReqList.push_back( GIFT_REQ_DATA(list[idx]) );
	}
}

void MailBoxContext::SetGiftReqResult(INT32 type, AUTH_GIFT * pAuthGift)
{
	if(EV_SUCCESSED(pAuthGift->m_i32Rv ) )
	{
		// �����̸�, ���� ��Ʈ�� �����սô�..
		if(type == TYPE_SHOP_GIFT_DELETE )
		{
			DeleteNoteGift(pAuthGift->m_TDBIdx);
		}
		else
			UpdateNoteGift(pAuthGift);
	}
	else
	{
		// ���и� �α� ����.
		i3::string strErrorLog;
		i3::sprintf( strErrorLog, "AGF[%d] : %x ", type, (UINT32)pAuthGift->m_i32Rv);
		I3PRINTLOG(I3LOG_NOTICE, strErrorLog );
	}

	for( UINT32 idx = 0 ; idx < m_GiftReqList.size() ; ++idx)
	{
		if(m_GiftReqList[idx]._idx == pAuthGift->m_TDBIdx)
		{
			m_GiftReqList[idx]._success = EV_SUCCESSED(pAuthGift->m_i32Rv);
			break;
		}
	}
}

INT32 MailBoxContext::GetGiftReqSuccessCount(void)
{
	INT32 count = 0;

	for( UINT32 idx = 0 ; idx < m_GiftReqList.size() ; ++idx)
	{
		if(m_GiftReqList[idx]._success == true)
			count++;
	}
	
	return count;
}