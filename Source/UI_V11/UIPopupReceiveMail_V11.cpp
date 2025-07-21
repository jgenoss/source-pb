#include "pch.h"
#include "UI/UIMainFrame.h"
#include "UIPopupReceiveMail_V11.h"
#include "../UI/UIUtil.h"
#include "../UI/UIShopFunction.h"
#include "../UI/UIItemInfoUtil.h"

#include "../Shop.h"

//------------------------------------------------------------------------------//
//							Lua Glue Functions									//
//------------------------------------------------------------------------------//
static UIPopupReceiveMail_V11* GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPopupReceiveMail_V11*) pFrame->GetPopup( UPW_MAILREV);
}


extern "C" {

	int UIPopupReceiveMail_V11_Accept( LuaState* L)
	{
		GetThis()->Accpet_Join_Clan();
		return 0;
	}

	int UIPopupReceiveMail_V11_Deny( LuaState* L)
	{
		GetThis()->Deny_Join_Clan();
		return 0;
	}

	int UIPopupReceiveMail_V11_Reply( LuaState * L)
	{
		GetThis()->Reply();
		return 0;
	}

	int UIPopupReceiveMail_V11_ReceivePresent( LuaState * L)
	{
		GetThis()->ReceiveGift();
		return 0;
	}
}

LuaFuncReg UIPopupReceiveMail_V11_Glue[] = {
	{"Accept",		UIPopupReceiveMail_V11_Accept },
	{"Deny",		UIPopupReceiveMail_V11_Deny },
	{"Reply",			UIPopupReceiveMail_V11_Reply},
	{"ReceivePresent",	UIPopupReceiveMail_V11_ReceivePresent},
	{nullptr,				nullptr}
};

I3_CLASS_INSTANCE( UIPopupReceiveMail_V11);

UIPopupReceiveMail_V11::UIPopupReceiveMail_V11()
{
	m_pGiftInfo = nullptr;
}

UIPopupReceiveMail_V11::~UIPopupReceiveMail_V11()
{
}

void UIPopupReceiveMail_V11::_GetPresentMessage( i3::rc_wstring* pwstrOut, const size_t sizeOutMax, NOTE_RECEIVE* pNote)
{
	i3::rc_wstring wstrWeaponName;
	INT32 iType = AUTH_UNKNWON;

	i3::sprintf(*pwstrOut, GAME_RCSTRING("STR_POPUP_UNKNOWN_GIFT"));

	//	�����ϱ� Note���� pNote->body._user �����ʹ� GIFT_BUFFER ����ü ��������.
	m_pGiftInfo = (GIFT_BUFFER*) pNote->body._user;
	if(m_pGiftInfo == nullptr)		return;

	INT32 authTerm = 0, authCount = 1;
	bool IsPackageGoods = false;

	UINT32 GoodsID = m_pGiftInfo->_ui32GoodsID;
	CShop* pShop =  CShop::i();

	SHOP_GOODS_LIST * pGoodsList = pShop->GetGoodsListData( m_pGiftInfo->_ui32GoodsID );

	if( pGoodsList != nullptr)
	{
		if (1 < pGoodsList->_ui8ItemCount)
		{
			IsPackageGoods = true;
		}
		else
		{
			iType = pGoodsList->_ppItemData[0]->_ui8AuthType;

			if ( ITEM_TYPE_COUNT == iType )
			{
				authCount = pGoodsList->_pui32ItemArg[0];
			}
			else if ( ITEM_TYPE_TIME == iType )
			{
				authTerm = pGoodsList->_pui32ItemArg[0];
			}
		}
	}

	if (IsPackageGoods)
	{
		bool isPeriod = false;
		INT32 periodItemIdx = 0;
		INT32 notPointItemIdx = 0;

		INT32 ItemCount = pGoodsList->_ui8ItemCount;
		for(INT32 j = 0; j < ItemCount; j++)
		{
			if (ITEM_TYPE_TIME == pGoodsList->_ppItemData[j]->_ui8AuthType)
			{
				isPeriod = true;
				periodItemIdx = j;
				break;
			}
			else if (ITEM_TYPE_POINT != GET_ITEM_TYPE(pGoodsList->_ppItemData[j]->_ui32ItemID))
			{
				notPointItemIdx = j;
			}
		}


		if (isPeriod)
		{

			UINT32 seconds = pGoodsList->_pui32ItemArg[periodItemIdx];
			UINT32 day = ConvertSecToUseDay(seconds);

			ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
			I3ASSERT( pInfo != nullptr);

			i3::sprintf(*pwstrOut, GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), day); // �̸�: %s\n���Ⱓ: %d��
		}
		else
		{
			INT32 ItemID = pGoodsList->_ppItemData[periodItemIdx]->_ui32ItemID;

			//	���� ������ Goods���� pGoods->_AuthArg�� ��� ������ �ֱ� ������ ���� ó���� �ʿ� komet
			switch( (ITEM_TYPE)  GET_ITEM_TYPE(ItemID))
			{
				//	���Ӽ�
			case ITEM_TYPE_MAINTENANCE:
				{
					//	getInfo�� keyItem���� �����;� ��Ȯ�� info�� ���� �� �ִ�. (�׷��������� nullptr�� �� �ִ�.)
					ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
					I3ASSERT( pInfo != nullptr);

					UINT32 Day = pGoodsList->_pui32ItemArg[periodItemIdx];
					i3::sprintf(*pwstrOut, GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), Day); // �̸�: %s\n���Ⱓ: %d��
				}
				break;
				//	�Ҹ�
			case ITEM_TYPE_WRAP_PERIOD:
			case ITEM_TYPE_WRAP_COUNT:
				{
					UINT32 count = pGoodsList->_pui32ItemArg[periodItemIdx];

					//	getInfo�� keyItem���� �����;� ��Ȯ�� info�� ���� �� �ִ�. (�׷��������� nullptr�� �� �ִ�.)
					ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
					I3ASSERT( pInfo != nullptr);

					i3::sprintf(*pwstrOut, GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), count); // �̸�: %s\n���Ⱓ: %d��
				}
				break;
			default:
				return;
			}
		}

		m_PackageGoodsID = GoodsID;
	}
	else if( pGoodsList != nullptr)
	{
		UINT8 ui8AuthType = pGoodsList->_ppItemData[0]->_ui8AuthType;
		UINT32 ui32ItemArg = pGoodsList->_pui32ItemArg[0] ; 
		T_ItemID ItemID = pGoodsList->_ppItemData[0]->_ui32ItemID;

		i3::rc_wstring wstrTemp;
		i3::rc_wstring wstrItemName;	

		usf::GetShopItemName(wstrItemName, ItemID);
		I3ASSERT( ui8AuthType != 0 );
		//	��ǰ ������ ���� �з�
		switch(ui8AuthType)
		{
		case ITEM_TYPE_COUNT :
			{
				if( item_def::get_category(ItemID) == item_def::CTG_ITEM )
				{
					if (GET_ITEM_TYPE(ItemID) != ITEM_TYPE_WRAP_COUNT)
					{
						UINT32 ui32ItemTerm = GET_ITEM_SUBTYPE(ItemID);
						if (ui32ItemTerm > 0)
							wstrItemName = i3::format_string(GAME_RCSTRING("STR_INVEN_CASH_ITEM_TERM"), wstrItemName, ui32ItemTerm);
					}
				}

				wstrTemp = GAME_RCSTRING("STR_POPUP_EXPENDABLES_GIFT_NAME");
			}
			break;
		case ITEM_TYPE_TIME :
			{
				wstrTemp = GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME");
				ui32ItemArg = (INT32) ConvertSecToUseDay(ui32ItemArg);	//	�����Ⱓ (��)�� ��ȯ
			}
			break;
		}

		i3::sprintf( *pwstrOut , wstrTemp, wstrItemName, ui32ItemArg);

	}
}


void UIPopupReceiveMail_V11::_SetData( void)
{
	i3::rc_wstring wstrMsg;
	i3::rc_wstring wstrPresent;

	if( m_SelectedIndex < 0 ) return;

	NOTE_RECEIVE*	pNote	= nullptr;

	switch( m_CurrentTab)
	{
	case	MessageTab_Receive :
		{
			INT32 count = MailBoxContext::i()->GetNoteReceiveCount();
			pNote	= const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetReceiveNote( count - m_SelectedIndex - 1 ));
		}
		break;
	case	MessageTab_ReceivePresent :
		{
			INT32 count = MailBoxContext::i()->GetNoteGiftCount();
			pNote	= const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetNoteGiftNote( count - m_SelectedIndex - 1 ));
		}
		break;
	default:	return;
	}

	if( pNote == nullptr ) return;
	if( 0 < pNote->body._messageSize )
		i3::generic_string_copy( wstrMsg, pNote->body._message);

	// ���� ������ ���
	if( pNote->header._type == NT_GIFT && (GIFT_BUFFER*)pNote->body._user != nullptr)
	{
		_GetPresentMessage( &wstrPresent, MAX_STRING_COUNT, pNote);
		ReceivePresent( pNote );
	}
	else
	{
		LuaState* L = _CallLuaFunction("ReceiveGift");
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 1);
	}


	i3::rc_wstring wstrTemp;
	i3::sprintf(wstrTemp, GAME_RCSTRING("STR_POPUP_WHAT_DAY"), pNote->header._remainDay);

	LuaState * L = _CallLuaFunction( "SetData");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, pNote->header._type);			// type
		i3Lua::PushStringUTF16To8( L, pNote->body._sender);		// nick
		i3Lua::PushStringUTF16To8( L, wstrTemp);				// date
		i3Lua::PushStringUTF16To8( L, wstrPresent);				// present
		i3Lua::PushStringUTF16To8( L, wstrMsg);					// message
		i3Lua::PushBoolean( L, (pNote->header._state == NS_READED) );		// read state
		i3Lua::PushBoolean(L , IsGameMasterNickName(pNote->body._sender) ? false : true);	// ���� ������ GM�� ��� �����ϱ� Off
		_EndLuaFunction( L, 7);
	}

	SetSelectedNote( pNote);
}

/*virtual*/ void UIPopupReceiveMail_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_PopUp_Message.i3UIs", UIPopupReceiveMail_V11_Glue);

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

/*virtual*/ void UIPopupReceiveMail_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	i3UIButtonComposed3* pBtnReply = (i3UIButtonComposed3*)	pScene->FindChildByName("btn2");
	I3ASSERT( pBtnReply != nullptr);

	pBtnReply->EnableCtrl(false);
}

/*virtual*/ bool UIPopupReceiveMail_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	m_CurrentTab = *static_cast<MessageTab*>(pArg1);
	m_SelectedIndex	= *static_cast<INT32*>(pArg2);

	_SetData();

	return true;
}

/*virtual*/ bool UIPopupReceiveMail_V11::OnCloseButton( void)
{
	return UIPopupBase::OnCloseButton();
}

/*virtual*/ void UIPopupReceiveMail_V11::ProcessGameEvent(GAME_EVENT evt,INT32 arg, const i3::user_data& UserData)
{
	switch(evt)
	{
	case EVENT_AUTH_GIFT_GOODS:		//	�����ޱ� ��ǰ Ȯ��
		{
			if( EV_SUCCESSED( arg ) )
			{
				NOTE_RECEIVE*	pNote	= nullptr;
				pNote = const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetNoteGiftNote( m_SelectedIndex ));

				// ������ �κ��丮�� ���������� ���޵Ǿ����ϴ�.
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_INFORM_SUCCESS_GIFT"));

				LuaState *L = _CallLuaFunction("ReceiveGift");
				i3Lua::PushBoolean(L, false);
				_EndLuaFunction(L, 1);

				if( pNote != nullptr && pNote->header._type == NT_GIFT )
				{
					pNote->header._state	= NS_READED;		// �������·� �����մϴ�.


					// ���� ������ ������ �����Ⱓ�� 7�Ϸ� �����մϴ�.
//					pNote->header._remainDay = 7;

					INT8	ReadCount	= 1;
					GameEventSender::i()->SetEvent( EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);
					MailBoxContext::i()->SetNoteRefreshFlag();
				}
			}
			else
			{
				// ��Ʈ��ũ ������ �κ��丮�� ���� ������ �����߽��ϴ�.
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_FAILED_GIFT"));
			}
		}
		break;
	}
}


// �ε��� ���� �о����
void UIPopupReceiveMail_V11::SetSelectedNote( NOTE_RECEIVE* pNote )
{
	INT8 ReadCount = 0;

	if( pNote == nullptr ) return;
	switch( pNote->header._type)
	{
	case NT_CLAN_ASK:
		//	���� ��û ���� �޽����� ������ �������� �ʴ´�. ���߿� �亯�Ҷ� ����.
		break;
	case NT_NORMAL:
	case NT_NOTICE:
	case NT_WEB:
	case NT_CLAN:
	case NT_GIFT:
		{
			if(pNote->header._state != NS_READED)
			{
				// �������·� �����մϴ�.
				pNote->header._state = NS_READED;

				// ���� ������ ������ �����Ⱓ�� 7�Ϸ� �����մϴ�.
				if (pNote->header._type != NT_GIFT)
					pNote->header._remainDay = 7;

				ReadCount++;
				GameEventSender::i()->SetEvent( EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);
			}
		}
		break;
	}

	MailBoxContext::i()->SetNoteRefreshFlag();
}

void UIPopupReceiveMail_V11::ReceivePresent( NOTE_RECEIVE* pNote )
{
	if( pNote != nullptr && pNote->header._type == NT_GIFT )
	{
		m_pGiftInfo	= (GIFT_BUFFER*) pNote->body._user;
		if (m_pGiftInfo)
		{
			I3ASSERT(m_pGiftInfo->_ui32GoodsID != 0);

			LuaState* L = _CallLuaFunction("ReceiveGift");
			i3Lua::PushBoolean(L, (m_pGiftInfo->_eDeliver >= TYPE_GIFT_DELIVER_RECEIVED ? false : true));
			_EndLuaFunction(L, 1);
		}
	}
}

void	UIPopupReceiveMail_V11::Accpet_Join_Clan()
{
	UIPopupReceiveMail_V11::OnNoteAccept(true );
}

void	UIPopupReceiveMail_V11::Deny_Join_Clan()
{
	UIPopupReceiveMail_V11::OnNoteAccept(false);
}

void	UIPopupReceiveMail_V11::Reply()
{
	LuaState* L = UILua::CallLuaFunction(GetScene(0), "getSenderName");
	const char* szSender = UILua::ReturnLuaString(L, 0);

	i3::rc_wstring 	wstrSender;
	i3::utf8_to_utf16(szSender, wstrSender);
	GetMainFrame()->OpenPopup(UPW_MAILREP, &wstrSender);
}

void	UIPopupReceiveMail_V11::ReceiveGift()
{
	if(m_pGiftInfo != nullptr)
	{
		UINT8 ui8Type		= TYPE_SHOP_GIFT_AUTH;
		UINT8 ui8GiftCount	= 1;	

		GameEventSender::i()->SetEvent( EVENT_AUTH_GIFT_GOODS, &ui8Type, &ui8GiftCount, &m_pGiftInfo->_i32GiftWareDBIdx );
	}
}

void UIPopupReceiveMail_V11::OnNoteAccept(bool bVal)
{
	// if it Checked...
	INT32 count = MailBoxContext::i()->GetNoteReceiveCount();
	NOTE_RECEIVE * pNote = const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetReceiveNote( count - m_SelectedIndex - 1));

	INT32 ReadCount = 1;

	//	Ŭ�� ���� ��û�� ���
	if(pNote->header._type == NT_CLAN_ASK)
	{
		UINT32 wClanDBIdx = (INT32) pNote->header._appendingData;

		g_pFramework->GetClanContext()->SendClanInviteAccept(wClanDBIdx, (UINT8) bVal);

		// �������·� �����մϴ�.
		pNote->header._state = NS_READED;

		// ���� ������ ������ �����Ⱓ�� 7�Ϸ� �����մϴ�.
		pNote->header._remainDay = 7;
	}
	// �޽����� ���������� ���°� ����
	// ���� NT_CLAN_ASK ���¿����� ���� ���� ����
	GameEventSender::i()->SetEvent( EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);

	GetMainFrame()->ClosePopup(UPW_MAILREV);

}
