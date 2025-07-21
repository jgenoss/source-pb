#include "pch.h"
#include "UI_V11/UIPopupQueryUseItem_V11.h"

#include "UI/UIMainFrame_V11.h"
#include "UI/ItemImageMgr.h"
#include "UI/Useable_Item_Def.h"
#include "UI/ItemConsumer.h"
#include "UI/UIShopFunction.h"
#include "UI/UIUtil.h"

I3_CLASS_INSTANCE( UIPopupQueryUseItem_V11);

namespace inner_V11
{
	enum { OK, CANCEL, };
	UIPopupQueryUseItem_V11* pThis = 0;

	void CbQueryUse(void * pThis, INT32 nParam);
}

void inner_V11::CbQueryUse(void * pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		inner_V11::pThis->use_item();
	}
}

/***********************//***********************/
extern "C" 
{
	int query_item_click_v11( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		inner_V11::pThis->clicked( value );
		return 0;
	}
}

LuaFuncReg query_item_glue_v11[] =
{
	{ "Click", query_item_click_v11	},
	{ NULL, NULL}
};

/***********************//***********************/
void UIPopupQueryUseItem_V11::clicked(int idx)
{
	switch( idx )
	{
	case inner_V11::OK:
		{
			switch( item_query_style )
			{
			case IQS_USE: test_use(); break;
			case IQS_REMOVE: remove_item(); break;
			}
		}		
		break;
	case inner_V11::CANCEL:	
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

void UIPopupQueryUseItem_V11::use_item()
{
	ItemConsumer::instance()->use(GetMainFrame(), m_item_id);
}

void UIPopupQueryUseItem_V11::remove_item()
{
	ItemConsumer::instance()->remove(GetMainFrame(), m_item_id, m_item_db_idx);
}

/***********************//***********************/
UIPopupQueryUseItem_V11::UIPopupQueryUseItem_V11(void)
{
	item_query_style = IQS_USE;
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
	inner_V11::pThis = this;
}

UIPopupQueryUseItem_V11::~UIPopupQueryUseItem_V11(void)
{
}

/*virtual*/ void UIPopupQueryUseItem_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "UIRe1/Scene/Popup/PBRe_PopUp_useItemQuery.i3UIs", query_item_glue_v11);
}

/*virtual*/ bool UIPopupQueryUseItem_V11::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	item_query_style = *( static_cast<ITEM_QUERY_STYLE*>( pArg2 ) );
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

void	UIPopupQueryUseItem_V11::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	m_pImageCtrl = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet" );
}

void	UIPopupQueryUseItem_V11::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();

	m_pImageCtrl->RestoreSizePos();
	I3_SAFE_RELEASE(m_pImageCtrl);
}

void UIPopupQueryUseItem_V11::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	ItemConsumer::instance()->process_game_event_in_query(event, arg, UserData);
}


bool UIPopupQueryUseItem_V11::OnKey_Enter() 
{ 
	clicked( inner_V11::OK );  

	return UIPopupBase::OnKey_Enter();
}

bool UIPopupQueryUseItem_V11::OnKey_Escape() 
{ 
	clicked( inner_V11::CANCEL );  

	return UIPopupBase::OnKey_Escape();
}


/***********************//***********************/
void UIPopupQueryUseItem_V11::setting_ui()
{
	LuaState * L = 0;

	L = _CallLuaFunction( "SetDeleteMode");
	i3Lua::PushBoolean( L, (item_query_style == IQS_REMOVE ) ? TRUE : FALSE );
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

void UIPopupQueryUseItem_V11::test_use()
{
	// ������ �������� ��쿣 �ٷ� ����Ѵ�.
	if( ITEM_TYPE_WRAP_COUNT == GET_ITEM_TYPE(m_item_id)  ||
		CASHITEM_GROUP_COLOR_MUZZLE_FLASH == GET_ITEM_NUMBER(m_item_id))
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

	switch( GET_ITEM_SUBTYPE( m_item_id))
	{
	case CASHITEM_GROUP_COLOR_CLAN:
	case CASHITEM_GROUP_COLOR_NICK:
	case CASHITEM_GROUP_COLOR_CROSSHAIR:
	case CASHITEM_GROUP_COLOR_CHATTING:
		{
			//	"������� �������Դϴ�.\n���� ������ ������ �� ������\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
			wstrMessage = GAME_RCSTRING("STR_POPUP_MESSAGE_CAN_CHANGE_COLOR");
		}
		break;
	case CASHITEM_GROUP_DISGUISE_RANK:
		{
			//	"������� �������Դϴ�.\n���� ���� ������� ������ �� ������\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
			wstrMessage = GAME_RCSTRING("STR_POPUP_MESSAGE_CAN_DISGUISE_RANK");
		}
		break;
	case CASHITEM_GROUP_DISGUISE_NICK:
		{
			//	"������� �������Դϴ�.\n���� �ӽ� �г������� ������ �� ������\n�������� �Ⱓ��ŭ ���� �Ⱓ�� �����մϴ�.\n \n����Ͻðڽ��ϱ�?"
			wstrMessage = GAME_RCSTRING("STR_POPUP_MESSAGE_CAN_DISGUISE_NICK");
		}
		break;
	}

	// ������� �������̸� �ٽ� �ѹ� ���´�.
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, &GAME_RCSTRING("STR_POPUP_CAPTION_USE_ITEM"), MAKE_CALLBACK(inner_V11::CbQueryUse), this);
}
