#include "pch.h"
#include "ItemConsumer.h"

#include "UIDefine.h"
#include "UIItemInfoUtil.h"
#include "UIMainFrame.h"
#include "UIUtil.h"
#include "UIShopFunction.h"

#include "UIMainFrame.h"
#include "UIFloatCharacterView.h"
#include "PurchaseMgr.h"
#include "ClanGameContext.h"
#include "UserInfoContext.h"

#include "UI_V10/UIFloatCharacterView_V10.h"

#include "i3Base/string/ext/format_rcstring.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "UISignal.h"

//- ItemConsumer ----------//
ItemConsumer* ItemConsumer::instance()
{
	static ItemConsumer instance;
	return &instance;
}

ItemConsumer::ItemConsumer() : m_AuthWareDBIndex(0), m_ItemID(0)
{
	m_OpenedUsePopup = UPW_NULL;

	pf_method[USE_METHOD_DEFAULT_AUTH] = &ItemConsumer::auth;
	pf_method[USE_METHOD_DISGUISE_NICK] = &ItemConsumer::open;
	pf_method[USE_METHOD_DISGUISE_RANK] = &ItemConsumer::open;
	pf_method[USE_METHOD_COLOR_NICK] = &ItemConsumer::open;
	pf_method[USE_METHOD_COLOR_CLAN] = &ItemConsumer::open;
	pf_method[USE_METHOD_COLOR_CROSS_HAIR] = &ItemConsumer::open;
	pf_method[USE_METHOD_CHANGE_NICK] = &ItemConsumer::open;
	pf_method[USE_METHOD_CHANGE_CLAN_NAME] = &ItemConsumer::open;
	pf_method[USE_METHOD_CHANGE_CLAN_MARK] = &ItemConsumer::open;
	pf_method[USE_METHOD_BATTLE_FIELD_MOVE] = &ItemConsumer::battle;
	pf_method[USE_METHOD_COLOR_FIRE_MUZZLE] = &ItemConsumer::open;

	m_bClanMaxBy50 = false;
	m_bClanMaxBy10 = false;
}

ItemConsumer::~ItemConsumer()
{
}

//--------------------////--------------------//
bool ItemConsumer::use(UIMainFrame* frm, T_ItemID item_id)
{
	m_MainFrame = frm;
	m_ItemID = item_id;

	CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();
	const UINT8 PLUS_10 = 10; const UINT8 PLUS_50 = 50;

	// Ŭ�� ���밡�� �ο��� �ִ�ġ�� ������ ���¿��� Ŭ���� �������� ����� �� ����. 
	if(m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_50)
	{
		if( pClanInfo->m_maxPerson + PLUS_50 > MAX_MEMBER_COUNT){
			
			i3::rc_wstring wstrErrorMsg = i3::format_rcstring(GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrErrorMsg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			return false;	// ��� �õ� ���н� ��Ŷ ������ �ʽ��ϴ� ()
		}
	}

	if( m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10)
	{
		if( pClanInfo->m_maxPerson + PLUS_10 > MAX_MEMBER_COUNT){
			
			i3::rc_wstring wstrErrorMsg = i3::format_rcstring(GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);
			GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			return false;
		}
	}

	m_AuthWareDBIndex = CInvenList::i()->FindWareDBIdx_ByItemID( m_ItemID );
	if( m_AuthWareDBIndex == INVALID_WAREDB_INDEX)
	{
		/*����� �� ���� �������Դϴ�.*/ /*������ ��� ����*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
		return false;
	}

	//�ߺ� ��� ������ ���������� ���� �˻�.
	usf::ValidUseType result = usf::IsUseableItem(m_ItemID);
	if( result != usf::vut_Ok )
	{
		usf::Message_UseResert(result);
		return false;
	}

	//ItemConsumer::use �Լ����� ����ϴ� ���������� �Ǵ��Ѵ�.
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(item_id);

	if ( pInfo && pInfo->GetUsePosition() != USE_POS_ANYWHERE )
	{
		const i3::rc_wstring& wstrMsg = pInfo->GetUsePositionDesc();				// ����� �� �ִ� ��ġ �޽���
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMsg, &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));	//������ ��� �Ұ�
		return false;
	}

	if (false == chk_item_period::_CheckUseItemPeriod(m_ItemID))
	{
		// �ִ� �Ⱓ/���� �����˾�, �ϳ��� ���ѿ� �ɸ��°� �ִٸ� ����(����X)
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_INVEN_PERIOD_LIMIT_POPUP1"), &GAME_RCSTRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT2"));
		return false;
	}

	ITEM_USE_METHOD	method = use_item::UseMethodStringToEnum(m_ItemID);
	if(method != USE_METHOD_UNKNOWN)
		(this->*pf_method[method])(method);

	return true;
}

//bool ItemConsumer::use_in_domi(CASHITEM_GROUP_TYPE type)
//{
//	T_ItemID ItemID = MAKE_CASH_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, ITEM_TYPE_DOMIITEM, type, CASHITEM_DAY_NO );
//	T_ItemDBIdx db_idx = CInvenList::i()->FindWareDBIdx_ByItemID(ItemID);
//
//	if(db_idx == INVALID_WAREDB_INDEX) return false;
//
//	PACKET_DOMINATION_USE_ITEM_REQ i;
//	i._TItemWareIdx = db_idx;
//
//	GameEventSender::i()->SetEvent( EVENT_DOMI_USE_ITEM, reinterpret_cast<void*>(&i));
//
//	return true;
//}


bool ItemConsumer::remove(UIMainFrame* frm, T_ItemID item_id, T_ItemDBIdx item_db_idx)
{
	m_MainFrame = frm;
	m_ItemID = item_id;
	m_AuthWareDBIndex = item_db_idx;

	if( item_db_idx == INVALID_WAREDB_INDEX)
	{
		/*����� �� ���� �������Դϴ�.*/ /*������ ��� ����*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
		return false;
	}

	PurchaseMgr::i()->Remove(0, m_ItemID, item_db_idx );   // "finish_remove_item",

	return true;
}

void	 ItemConsumer::process_game_event_in_query( INT32 event, INT32 arg, const i3::user_data& UserData)
{
	if( m_ItemID <= 0) return; //������ ������� �ƴϹǷ� �����Ѵ�. (�� �ڵ忡�� �ʿ��� ������ �𸣰���.. 

	//	�̺�Ʈ ����� �޾� ó���Ѵ�.
	switch( event )
	{
	case EVENT_ITEM_AUTH_DATA_CHAGNE:
	case EVENT_ITEM_AUTH:	// ������ ������ ����
	case EVENT_CLAN_PERSON_PLUS:	// Ŭ���� ���� ĳ�� ������ (CASHITEM_GROUP_CLAN_PERSON_PLUS_50) ��� ���
		{
			if (EV_SUCCESSED(arg) )
			{
				// Ŭ���� ���� ĳ�� �������� EVENT_ITEM_AUTH �̺�Ʈ�϶��� Ŭ������ ���� ������ ��ġ�� ������ ��ũ���� ������ �����Ƿ�
				// ��Ȯ�ϰ� �������� ������Ʈ�Ǵ� EVENT_CLAN_PERSON_PLUS �̺�Ʈ�� �������� ��� ��� â�� ����Ѵ�.
				// 14. 4. 30 Gyunjgu.kim �߰�) ��� ���� ��쿡 ���ؼ��� �ٸ� ��(use)������ ��� â�� ����մϴ�.
				if( event == EVENT_ITEM_AUTH && m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10)
					break;

				if( event == EVENT_ITEM_AUTH && m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_50)
					break;

				m_MainFrame->ClosePopup(UPW_QUERY_ITEM);
				if( m_OpenedUsePopup != -1 )
					m_MainFrame->ClosePopup(m_OpenedUsePopup);
				
				if( m_ItemID != CASHITEM_ITEMID_MEGAPHONE)
					open_popup(USE_ITEM_COMPLETE);
				if(g_pEnvSet->m_nUIVersion == UFV_1_5)
				{
					UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));

					if( pCharaViewWin != nullptr)
						pCharaViewWin->DoUpdateCharacter(); //ĳ���� ���� ����
				}
				else
				{
					UIFloatCharacterView_V10 * pCharaViewWin = static_cast<UIFloatCharacterView_V10*>(GameUI::GetFloating(UFW_CHARACTER));

					if( pCharaViewWin != nullptr)
						pCharaViewWin->DoUpdateCharacter(); //ĳ���� ���� ����
				}
				
			//	UI::call_cb( "update_slot9", (void*) true, (void*) true); //����/�κ� ���� ���� ����
				UISignal::UpdateSlot9(true, true);		//����/�κ� ���� ���� ����
			}
			else
			{
				i3::rc_wstring wstrErrorMsg;

				switch(arg)
				{
				case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
					//{
					//	// Ŭ���� ã���� �����ϴ�.
					//	wstrErrorMsg = GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_NO_FINDING");
					//}
					//break;
				case EVENT_ERROR_CLAN_LEVEL_NOMASTER:
					{
						// Ŭ�� �����͸�\n ����� �� �ִ� �������Դϴ�.
						wstrErrorMsg = GAME_RCSTRING("STR_INVEN_ONLY_MASTER");
					}
					break;
				case EVENT_ERROR_CLAN_INCREASE_MAXPERSON_FAIL:
					{						
						if(m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_50)
							m_bClanMaxBy50 = true;
						else if(m_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10)
							m_bClanMaxBy10 = true;

						// �������� ����� �� �����ϴ�.\n \n Ŭ�� �ִ� �ο�����\n {col:255,153,51,255}%d{/col}������ ���ѵǾ� �ֽ��ϴ�.
						// note. Ŭ�� �ִ� �ο����� MAX_MEMBER_COUNT�� ���ѵ��ֽ��ϴ�,
						wstrErrorMsg = i3::format_rcstring(GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);
					}
					break;
				default:
					{
						// ������ �����Ͽ����ϴ�. ��� �� �ٽ� �õ����ּ���.
						wstrErrorMsg = GAME_RCSTRING("STR_POPUP_MESSAGE_FAILED_AUTH");
					}
					break;
				}

				/*������ ��� ����*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrErrorMsg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			}
		}
		break;
	}
}

//--------------------////--------------------//
void ItemConsumer::auth(ITEM_USE_METHOD method)
{
	GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, &m_AuthWareDBIndex);
}

void ItemConsumer::open(ITEM_USE_METHOD method)
{
	switch(method)
	{
	case USE_METHOD_DISGUISE_NICK: open_popup( USE_ITEM_DISGUISE_NICK); break;
	case USE_METHOD_DISGUISE_RANK: open_popup( USE_ITEM_DISGUISE_RANK); break;
	case USE_METHOD_COLOR_NICK:open_popup( USE_ITEM_COLOR_NICK); break;
	case USE_METHOD_COLOR_CLAN: open_popup( USE_ITEM_COLOR_CLAN_NAME); break;
	case USE_METHOD_COLOR_CROSS_HAIR: open_popup( USE_ITEM_COLOR_CROSSHAIR); break;
	case USE_METHOD_CHANGE_NICK: open_popup( USE_ITEM_CHANGE_NICK); break;
	case USE_METHOD_CHANGE_CLAN_NAME: open_popup( USE_ITEM_CHANGE_CLAN_NAME); break;
	case USE_METHOD_CHANGE_CLAN_MARK: open_popup( USE_ITEM_CHANGE_CLAN_MARK); break;
	case USE_METHOD_COLOR_FIRE_MUZZLE: open_popup( USE_ITEM_COLOR_FIRE_MUZZLE); break;
	}
}

void ItemConsumer::error(ITEM_USE_METHOD method)
{
	/*����� �� ���� �������Դϴ�.*/ /*������ ��� ����*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_WRONG_ITEM_2"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
}

void ItemConsumer::battle(ITEM_USE_METHOD method)
{
	/*�����߿��� ����� �� �ֽ��ϴ�.*/ /*������ ��� ����*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_USE_IN_GAME"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
}

//--------------------////--------------------//
void ItemConsumer::open_popup(USE_ITEM_POPUP_TYPE type)
{
	UINT8 Type = 0;	UINT32 Arg = 0;
	if (CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM,  m_ItemID, &Type, &Arg))
	{		
		//	���� ������� ������
		if ((ITEM_ATTR_TYPE_AUTH == Type) &&
			(m_ItemID != CASHITEM_ITEMID_COLOR_MUZZLE_FLASH)) // �÷��ѱ�ȭ�� �������� ����� ���뺯���� �����մϴ�.
		{
			/*������ ���*//*���� ������� �������Դϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CURRENT_USE_ITEM"), &GAME_RCSTRING("STR_POPUP_CAPTION_USE_ITEM"));
			return;
		}
	}

	UIPOPUPWIN popup = UPW_NULL;
	void* arg1 = &m_ItemID, *arg2 = &type;

	switch( type)
	{
	case USE_ITEM_COMPLETE: popup = UPW_RESULT_USEITEM; break;
	
	case USE_ITEM_CHANGE_NICK: popup = UPW_CHANGENICK; break;
	case USE_ITEM_DISGUISE_NICK: popup = UPW_CHANGENICK; break;
	case USE_ITEM_CHANGE_CLAN_NAME: popup = UPW_CHANGENICK; break;

	case USE_ITEM_COLOR_NICK: popup = UPW_CHANGE_COLOR_NICK; break;
	case USE_ITEM_COLOR_CLAN_NAME: popup = UPW_CHANGE_COLOR_NICK; break;
	case USE_ITEM_COLOR_CROSSHAIR: popup = UPW_CHANGE_CROSSHAIR_COLOR; break;

	case USE_ITEM_DISGUISE_RANK: popup = UPW_DISGUISE_RANK; break;
	case USE_ITEM_COLOR_FIRE_MUZZLE: popup = UPW_COLOR_FIRE_MUZZLE_EFFECT; break;	
	case USE_ITEM_CHANGE_CLAN_MARK: 
		popup = UPW_CHANGE_CLAN_MARK; 
		CLAN_DETAIL_INFO * pClanInfo = g_pFramework->GetClanContext()->GetMyClanDetailInfo();
		arg2 = &pClanInfo->m_mark;
		break;
	}

	if(popup == UPW_NULL) return; //����� �� �� ������
	if( m_MainFrame->IsOpenedPopup(popup) == true ) return; //������ ���â�� ���� ����

	m_MainFrame->OpenPopup(popup, arg1, arg2);

	if( popup != UPW_RESULT_USEITEM )
		m_OpenedUsePopup = popup;
}


//chk_item_period (namespace)
// ���� ������ ����Ⱓüũ
bool chk_item_period::_CheckUseItemPeriod(T_ItemID itemID)
{
	INT32 ItemID = itemID;
	INT32 CheckItemID = 0;

	CheckItemID = GetCashItemByGroup(ItemID);

	UINT8 ui8Type = 0;
	UINT32 ui32Arg = 0;

	UINT32 Inven_Itemterm = 0;
	UINT32 CheckItemTerm = 0;
	UINT32 Max_Term = 0;

	CheckItemTerm = GET_ITEM_TIME(ItemID);

	bool isItem = false;

	INVEN::DB_TYPE dbType = INVEN::ItemID2DBType(ItemID);
	isItem = CInvenList::i()->GetItemTypeByItemID(dbType, CheckItemID, &ui8Type, &ui32Arg);

	// �κ��� �ش�������� ������ return true
	if (false == isItem)
	{
		return true;
	}

	CASHITEM_USAGE_TYPE usageType = CashItem::GetUsageFromItemID(CheckItemID);
	// �Ⱓ�� �������̸�.
	switch (usageType)
	{
	case CASHITEM_USAGE_MAINTENANCE:
	{
		Max_Term		=	UserInfoContext::i()->GetInventorySecTime(LIMIT_ITEM_DATE);
		Inven_Itemterm	=	UserInfoContext::i()->GetInventorySecTime(ui32Arg);
		Max_Term = ConvertSecToUseDay(Max_Term);
		Inven_Itemterm = ConvertSecToUseDay(Inven_Itemterm);

		if (CheckItemTerm + Inven_Itemterm > Max_Term)
			return false;
	}
	break;
	case CASHITEM_USAGE_EXPENDABLES:
	{
	}
	break;
	}

	return true;
}