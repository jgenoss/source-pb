#include "pch.h"
#include "UIPopupQueryItem.h"

#include "ItemImageMgr.h"
#include "UIMainFrame.h"
#include "Useable_Item_Def.h"
#include "ItemConsumer.h"
#include "UIShopFunction.h"
#include "UIUtil.h"

I3_CLASS_INSTANCE( UIPopupQueryItem);

namespace inner
{
	enum { OK, CANCEL, };
	UIPopupQueryItem* pThis = 0;

	void CbQueryUse(void * pThis, INT32 nParam);
}

void inner::CbQueryUse(void * ptr, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		inner::pThis->use_item();
	}
}

/***********************//***********************/
extern "C" 
{
	int query_item_click( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		inner::pThis->clicked( value );
		return 0;
	}
}

LuaFuncReg query_item_glue[] =
{
	{ "Click", query_item_click	},
	{ nullptr, nullptr}
};

/***********************//***********************/
void UIPopupQueryItem::clicked(int idx)
{
	switch( idx )
	{
	case inner::OK:
		{
			switch( item_query_style )
			{
			case IQS_USE: test_use(); break;
			case IQS_REMOVE: remove_item(); break;
			}
		}		
		break;
	case inner::CANCEL:	
		switch( item_query_style )
		{
		case IQS_USE: 
			if( GetMainFrame()->GetCurrentSubPhaseType() == USP_SHOP )
			{
				//�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
					&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
			}
			break;
		case IQS_REMOVE: break;
		}
		break;
	}

	GetMainFrame()->ClosePopup(UPW_QUERY_ITEM);
}

void UIPopupQueryItem::use_item()
{
	ItemConsumer::instance()->use(GetMainFrame(), m_item_id);
}

void UIPopupQueryItem::remove_item()
{
	ItemConsumer::instance()->remove(GetMainFrame(), m_item_id, m_item_db_idx);
}

/***********************//***********************/
UIPopupQueryItem::UIPopupQueryItem(void)
{
	item_query_style = IQS_USE;
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
	inner::pThis = this;
}

UIPopupQueryItem::~UIPopupQueryItem(void)
{
}

/*virtual*/ void UIPopupQueryItem::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene("Scene/Popup/PBRe_PopUp_ItemQuery.i3UIs", query_item_glue);
}

/*virtual*/ bool UIPopupQueryItem::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	item_query_style = *static_cast<ITEM_QUERY_STYLE*>( pArg2 );
	if(item_query_style == IQS_REMOVE)
	{
		const ITEM_INFO* info = static_cast<ITEM_INFO*>( pArg1 );
		m_item_id = info->m_TItemID;
		m_item_db_idx = info->m_TItemDBIdx;
	}
	else
	{
		m_item_id = *( static_cast<T_ItemID*>( pArg1 ) );
	}

	setting_ui();

	GetSlide()->ForceEnd();

	return true;
}

void	UIPopupQueryItem::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	m_pImageCtrl = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet" );
}

void	UIPopupQueryItem::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();

	m_pImageCtrl->RestoreSizePos();
	I3_SAFE_RELEASE(m_pImageCtrl);
}

void UIPopupQueryItem::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	ItemConsumer::instance()->process_game_event_in_query(event, arg, UserData);
}


bool UIPopupQueryItem::OnKey_Enter() 
{ 
	clicked( inner::OK );  

	return UIPopupBase::OnKey_Enter();
}

bool UIPopupQueryItem::OnKey_Escape() 
{ 
	clicked( inner::CANCEL );  

	return UIPopupBase::OnKey_Escape();
}


/***********************//***********************/
void UIPopupQueryItem::setting_ui()
{
	LuaState * L = 0;

	L = _CallLuaFunction( "SetDeleteMode");
	i3Lua::PushBoolean( L, (item_query_style == IQS_REMOVE ));
	_EndLuaFunction( L, 1);

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, m_item_id);

	i3::wstring wstrTermCount;
	usf::GetShopPeriodCountString_only_Item(m_item_id, wstrTermCount);	

	i3::rc_wstring wstrItemDescription;
	usf::GetShopItemDescription(wstrItemDescription, m_item_id);

	L = _CallLuaFunction( "SetItemInfo");
	i3Lua::PushStringUTF16To8(L, wstrItemName );
	i3Lua::PushStringUTF16To8(L, wstrTermCount );
	i3Lua::PushStringUTF16To8(L, wstrItemDescription );
	_EndLuaFunction( L, 3);

	GetItemImageMgr()->SelectImage(m_pImageCtrl, m_item_id);
}

void UIPopupQueryItem::test_use()
{
	// ������ �������� ��쿣 �ٷ� ����Ѵ�.
	if( ITEM_TYPE_WRAP_COUNT == GET_ITEM_TYPE(m_item_id))
	{
		use_item();
		return;
	}

	// - ����
	// �Ⱓ�� �������� ������ ������ ������ �Ⱓ�� �������� �������� �Ⱓ���� �������� �ٲ�� �κ��丮�� ����ȴ�.
	// �Ⱓ�� �������� �ȳ� �޽����� ������ش�.
	T_ItemID ItemID = GetCashItemByGroup( m_item_id);

	UINT8 Type = 0; UINT32 Arg = 0;
	CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM, ItemID, &Type, &Arg);
	if( Arg <= 0)
	{
		// �Ⱓ�� �������ε� �����Ⱓ�� 0�̸� ������ΰ� ���°��̴�.
		// �ٷ� ����ϵ��� �Ѵ�.
		use_item();
		return;
	}

	//	"������� �������Դϴ�.\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
	i3::rc_wstring wstrMessage = GAME_RCSTRING("STR_POPUP_MESSAGE_CAN_NORMAL_ITEM");

	// �Ⱓ���� üũ�ϸ� ���̴�!
	switch( ItemID )
	{
		// �÷� �ѱ� ȭ������ 16/17�� �Ѿ�´�. �����ؾ� �Ѵ�.
		// �̷��� �Ⱓ�� 17�� 16���� ��ȯ�ȴ�. �̰Ÿ� üũ�ϸ� �ȴ�.
	case CASHITEM_ITEMID_COLOR_MUZZLE_FLASH:
		use_item();
		return;

	case CASHITEM_ITEMID_COLOR_CLAN:
	case CASHITEM_ITEMID_COLOR_NICK:
	case CASHITEM_ITEMID_COLOR_CROSSHAIR:
	case CASHITEM_ITEMID_COLOR_CHATTING:
		{
			//	"������� �������Դϴ�.\n���� ������ ������ �� ������\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
			wstrMessage = GAME_RCSTRING("STR_POPUP_MESSAGE_CAN_CHANGE_COLOR");
		}
		break;
	case CASHITEM_ITEMID_DISGUISE_RANK:
		{
			//	"������� �������Դϴ�.\n���� ���� ������� ������ �� ������\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
			wstrMessage = GAME_RCSTRING("STR_POPUP_MESSAGE_CAN_DISGUISE_RANK");
		}
		break;
	case CASHITEM_ITEMID_DISGUISE_NICK:
		{
			//	"������� �������Դϴ�.\n���� �ӽ� �г������� ������ �� ������\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
			wstrMessage = GAME_RCSTRING("STR_POPUP_MESSAGE_CAN_DISGUISE_NICK");
		}
		break;
	}

	// ������� �������̸� �ٽ� �ѹ� ���´�.
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, &GAME_RCSTRING("STR_POPUP_CAPTION_USE_ITEM"), MAKE_CALLBACK(inner::CbQueryUse), this);
}
