#include "pch.h"
#include "UIPhaseShop.h"

#include "UIPhaseShopSlot.h"
#include "PurchaseMgr.h"
#include "UIClanFunction.h"
#include "ItemImageMgr.h"
#include "UIUtil.h"

#include "UIMainFrame.h"
#include "UIMainFrame_V10.h"
#include "UIMainFrame_V11.h"
#include "UI_V10/UITopMenu_V10.h"

#include "UIPopupBuyItem.h"
#include "UICharaInfoMgr.h"
#include "UISideMenu.h"
#include "InvenUtil.h"
#include "BuyUtil.h"
#include "UIPhasePreViewSlot.h"
#include "UIFloatCharacterView.h"
#include "MyRoomInfoContext.h"
#include "UISMenu_Ability.h"
#include "UISMenu_Equip.h"
#include "ItemConsumer.h"

#include "ShopContext.h"
#include "UserInfoContext.h"
#include "UIShopFunction.h"

#include "UISignal.h"
#include "i3Base/itl/bind/bind.h"

//------------------------------------------------------------------------------//
I3_CLASS_INSTANCE( UIPhaseShop);//, UIPhaseSubBase);

namespace
{
	enum	{ 
		//UITap�� enum ���� ���� ����Ѵ�.
		COMPARE = 6, BUY, CASH,	 PREVIEW_BUY, PREVIEW_CLEAR };			// �̰� ����...�� ... ��.....

	UIPhaseShop* g_this = 0;
}

/********************************/
extern "C" 
{
	int shop_cart_board( LuaState * L)
	{
		g_this->clicked_cart_board();
		return 0;
	}
	int shop_Coupon(LuaState * L)
	{
		g_this->clicked_shop_Coupon();
		return 0;
	}
}

LuaFuncReg shop_board_glue[] =
{
	{ "Click",			shop_cart_board	},
	{ "Click2",			shop_Coupon },
	{ nullptr,							nullptr}
};

/********************************/
void UIPhaseShop::clicked_menu_impl(INT32 idx)
{
	if( idx <= item_def::CTG_NEWHOT && idx >= item_def::CTG_WEAPON)
	{
		SetSortCombo(idx);
		cb_update_preview_slot();
	}

	switch( idx )
	{
	case COMPARE: 	((ShopSlot*)m_pSlot)->clicked(3, true);  break;
	case BUY: 
		//UI::call_cb( "smenu_ability_close" );
		((ShopSlot*)m_pSlot)->double_clicked(true); 
		break;
	case CASH:
	{
		//	ĳ�� ���� �������� ȣ��
#if defined(LOCALE_TAIWAN)
		if (g_pFramework->OpenWebBrowser(UserInfoContext::i()->GetBillingURL(), nullptr, false))
			return;
#else
		if (::ExcuteCashWeb())
			return;
#endif
	}
	break;
	case PREVIEW_BUY:
		{
			GetMainFrame()->OpenPopup(UPW_BUYITEM, (void*)buy::PurchaseRoute_Preview, &UISignal::FinishBuyPopup);		//		"finish_buy_popup");
		}
		break;
	case PREVIEW_CLEAR:
		buy::ClearPreview();
		UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));
		if( pCharaViewWin != nullptr)
			pCharaViewWin->Update_CharaView_Parts_Preview();
		cb_update_preview_slot();
		break;
	}
}

void UIPhaseShop::clicked_tap_impl(INT32 idx1, INT32 idx2)
{
	m_pSlot->SortType((item_def::Category)idx1, GetSortType());

	// PARTS�� ĳ���� �ѹ�ǵ��� ����... �����Ƽ ������ ��ġ�� ��ȹ.
	//���� ī�װ� ������ �ϱ����� ���οø�
	SMenuAbility* sidemenu = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
	sidemenu->EnableCtrlsByShopCategory(static_cast<item_def::Category>(idx1), true);

	if( idx1 == item_def::CTG_PARTS)
	{
		// parts�� �������� ���⸸ ����
		INT32 revert = (idx2 > -1) ? REVERT_TYPE_WEAPON : REVERT_TYPE_ALL;
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REVERT_PARTS, revert);
	}
	else
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REVERT_CHARA, REVERT_TYPE_ALL);
	}

	UISideMenu::instance()->setAbilityButton(false);	// SideMenu�� Ability ��ư�� ����ġ�� �������´�
	sidemenu->EnableCtrlsByShopCategory(static_cast<item_def::Category>(idx1), false);
}
/********************************/
void UIPhaseShop::clicked_cart_board()
{
//	UI::call_cb( "smenu_ability_close" );
	UISignal::SMenuAbilityClose();

	if(	buy::GetCount_CurrCharaCart() > 0 )			
	{
		GetMainFrame()->OpenPopup(UPW_BUYITEM, (void*)buy::PurchaseRoute_Cart, &UISignal::FinishBuyPopup); //			 "finish_buy_popup");
	}
}


void UIPhaseShop::clicked_shop_Coupon()
{
	GetMainFrame()->OpenPopup(UPW_COUPON_INPUT);
}


/********************************/
void UIPhaseShop::open_menu(INT32 idx) const
{
	iMultiTap::open_menu(idx);

	if (idx == item_def::CTG_WEAPON)
	{
		LuaState * L = UILua::CallLuaFunction(m_menu_scn_info.scn, "EnableCompare");
		i3Lua::PushBoolean(L, true);
		UILua::EndLuaFunction(L, 1);
	}
	LuaState * L = UILua::CallLuaFunction(m_menu_scn_info.scn, "Subopen");
	i3Lua::PushInteger(L, idx);
	i3Lua::PushBoolean(L, false);
	UILua::EndLuaFunction(L, 2);
}

/********************************/
void UIPhaseShop::cb_finish_buy_popup(bool arg1, bool arg2)
{
	m_pSlot->Entrance();
};

void UIPhaseShop::clicked_BuyType(INT32 idx)
{
	m_pSlot->SetBuyType((UINT32)idx);
	m_pSlot->SetBuySlotStr();
}

void UIPhaseShop::clicked_BuySlot(INT32 idx)
{
	m_pSlot->SetBuySlot((UINT32)idx);
}

//���� ���� ��Ŷ �Ŀ� ���� ����

void UIPhaseShop::cb_finish_buy_item(bool arg1, const i3::vector<buy::DisplayedGoodsPtr>& arg2)
{
	UISideMenu::instance()->smenu_click_equip( false);	//sidemenu ����
	
	UIPopupBuyItem* popup = (UIPopupBuyItem*)GetMainFrame()->GetPopup(UPW_BUYITEM);

	bool result_buy_item = arg1;    // ((BOOL)arg1 == TRUE) ? true : false;
	if(result_buy_item == true)
	{
		buy::PurchaseRouteType route = popup->getPurchaseRoute();

		switch(route)
		{
		case buy::PurchaseRoute_Immediate: 
			{
				switch( get_current_menu_idx() )
				{
				case item_def::CTG_WEAPON:
					if( GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM )
					{
						UINT8 mode_flag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();

						const i3::vector< buy::DisplayedGoodsPtr >& list = arg2; 

						T_ItemID item_id = list[0]->GetCurrentGoods()->_ItemID;

						if( item_def::is_locked_weapon(mode_flag, item_id) == true)
						{
							GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_WEAPON_LIMIT") ); /*��������*/
							break;
						}
					}
					break;
				case item_def::CTG_PARTS:
				case item_def::CTG_SKIN:
					{
						const i3::vector< buy::DisplayedGoodsPtr >& list = arg2;
						T_ItemID item_id = list[0]->GetCurrentGoods()->_ItemID;

						item_def::PartsType_ShopPreview idx = item_def::get_shop_preview_parts_type(item_id);
													
						if (idx >= 0)
							buy::RemovePreview(idx);
					}
					break;
				}

				buy::DisplayedGoods* displayedGoods = buy::GetDisplayedGoods_Immediate();  
				const SHOP_GOODS* pGoods = displayedGoods->GetCurrentGoods();		 //���� ���õ� goods
				PurchaseMgr::i()->Auth_Shop(this, pGoods, false);		// "finish_buy_auth_item",
			}
			break;
		case buy::PurchaseRoute_Preview:
			{
				PurchaseMgr::i()->Auth_Preview(this); // , "finish_preview_auth_item");
			}
			break;
		}

		buy::Clear_ByPurchaseRoute(route);
		m_pSlot->Entrance();
	}

	popup->ClosePopup();
}

//Immediate ���� �� ��츸 ȣ�� �ȴ�.
void UIPhaseShop::cb_finish_buy_auth_item(bool arg1, const SHOP_GOODS* arg2)
{
//	if( (BOOL)arg1 == FALSE ) return;
	if (arg1 == false)
		return;

	const SHOP_GOODS* buy_goods = arg2; // static_cast<const SHOP_GOODS*>(arg2);

	/*	������ �����Ŀ� ����/���� �� ��� ���� �޴� ������ �����ؾ� �ȴ�.	*/
	ItemInfoConvert item_info( buy_goods->_ItemID );
	item_def::Category ctg = item_def::get_category_goodsID( buy_goods->_GoodsID );

	switch( ctg )
	{
	case item_def::CTG_WEAPON:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
		{
			//�ֹ��⸸ ��������.
			CharaInfoMgr::CurrentState & state = m_pCharaInfoMgr->m_current_state;

			state.ctg = item_info.get_side_category();
			state.info.m_TItemID = buy_goods->_ItemID;
			state.info.m_TItemDBIdx = PurchaseMgr::i()->GetTargetItemDBIndex();
			state.slot_idx = item_info.get_char_slot_idx();

			//��� ���� ��ü �Ѵ�.
			if( UISideMenu::instance() != nullptr)
			{
				//������ ��ȯ
				SMenuEquip * pSMenuEquip = static_cast<SMenuEquip*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_EQUIP));
				if( pSMenuEquip != nullptr)
					pSMenuEquip->UIEvent_ChangedCharaEquip();	// ����� �̳༮�� �޴��� �����̶�.. ���� ȣ��.. ���� �޴�¡�� �� �ִ� �߰� �Լ��� ��ü�ؾ� �Ѵ�.
		
				//��� ���� ��ü �Ѵ�.
				UISideMenu::instance()->send_change_equip();		
			}
		}
		break;
	case item_def::CTG_CHARA:	
		if( item_def::is_chara_category(item_info.m_info.m_TItemID) )
		{
			CCharaInfoContext::i()->SetTeamSlotByCharaID(item_info.m_info.m_TItemID);
			CharaInfoMgr::instance()->list_up();

			// �ƹ�Ÿ �並 ����
			// Character Create���� NetCharaInfo�� �����Ѵ�.
			
			// �� ������ ������ ���� ���Կ� ���� ��� ����ó�� �մϴ�.
			INT32 slot = CCharaInfoContext::i()->FindCharaSlotIdxByItemID(item_info.m_info.m_TItemID);
			if (slot != -1) g_pFramework->OnEvent(UIEVENT_UPDATE_CHARA_SLOT, nullptr, slot, I3_EVT_CODE_ACTIVATE);
			else			g_pFramework->OnEvent( UIEVENT_UPDATE_CHARA_SLOT, nullptr, CCharaInfoContext::i()->GetHasCharacterCount()-1, I3_EVT_CODE_ACTIVATE);

			SHOP_ITEM_DATA* pItem = CShop::i()->GetItemData(item_info.m_info.m_TItemID);
			if (pItem->_ui8AuthType == ITEM_TYPE_ETERNITY)
			{
				CharaInfoMgr::instance()->send_event_change_equip();
			}
			else
			{
				// ���⸦ ����.
				if (CharaInfoMgr::instance()->GetAuthAndStateCharaChange() == true)
					CharaInfoMgr::instance()->send_event_change_equip();
				else
					CharaInfoMgr::instance()->SetAuthAndStateCharaChange();
			}
		}
		else
		{
			CharaInfoMgr::CurrentState & state = m_pCharaInfoMgr->m_current_state;

			state.ctg = item_info.get_side_category();
			state.info.m_TItemID = buy_goods->_ItemID;
			state.info.m_TItemDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID( buy_goods->_ItemID);
			state.slot_idx = item_info.get_char_slot_idx();


			{
				//������ ��ȯ
				if( UISideMenu::instance() != nullptr)
				{
					//������ ��ȯ
					SMenuEquip * pSMenuEquip = static_cast<SMenuEquip*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_EQUIP));
					if( pSMenuEquip != nullptr)
						pSMenuEquip->UIEvent_ChangedCharaEquip();	// ����� �̳༮�� �޴��� �����̶�.. ���� ȣ��.. ���� �޴�¡�� �� �ִ� �߰� �Լ��� ��ü�ؾ� �Ѵ�.
			
					//��� ���� ��ü �Ѵ�.
					UISideMenu::instance()->send_change_equip();				
				}
			}
		}
		break;
	}

	if( CShopContext::i()->IsDurabilityItem(buy_goods->_ItemID) )
	{
		//���� ���� ������(������)�� �Ѱ��� ���� �����ϱ� ������, ī�忡 �ִ� �������� �����Ѵ�.
		if( buy::RemoveDisplayedGoodsInCart(buy_goods) > 0 )
		{
			/*������ ������ �������� ���� ī�忡 ��ϵ� ���� ������ �������� ���� �Ǿ����ϴ�.*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_SHOP_REMOVE_DUPLICATED_PERPETUITY"));
		}
	}

	m_pSlot->Entrance();	//shop slot ����
}

//Preview ���� �Ŀ� ȣ�� �ȴ�.
void UIPhaseShop::cb_finish_preview_auth_item(bool arg1, const SHOP_GOODS*)
{
	if (arg1 == false)
		return;

//	if( (BOOL)arg1 == FALSE ) return;

	//preview�� ������ �������� chara_mgr�� ����Ѵ�.
	const i3::vector<INVEN_BUFFER*>& catched_inven_buffer_list = PurchaseMgr::i()->GetPurchasedInvenBufferList();
		
	size_t inven_buffer_size = catched_inven_buffer_list.size();
	for(size_t i=0;i<inven_buffer_size;++i)
	{
		INVEN_BUFFER* buffer = catched_inven_buffer_list[i];

		ITEM_INFO item_info;
		item_info.m_TItemID = buffer->_ui32ItemID;
		item_info.m_TItemDBIdx = buffer->_TItemWareDBIdx;

		m_pCharaInfoMgr->change_equip_item(item_info);
	}

	//chara_mgr�� ��ϵ� item_info_list ���� chara_view�� ������Ʈ�Ѵ�.
	const ItemInfoList& item_info_list = m_pCharaInfoMgr->get_item_info_list( CCharaInfoContext::i()->GetMainCharaIdx(), item_def::SCTG_PARTS);
	UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));
	if( pCharaViewWin != nullptr)
		pCharaViewWin->Update_CharaView_Parts( item_info_list.get_list() );

	//��� ���� ��ü �Ѵ�.
	UISideMenu::instance()->send_change_equip();

	m_pSlot->Entrance();	//shop slot ����
}



void UIPhaseShop::cb_update_cart_board()
{
	LuaState * L = UILua::CallLuaFunction( m_board_scn_info.scn, "SetCount");
	i3Lua::PushInteger(L, buy::GetCount_CurrCharaCart() );
	UILua::EndLuaFunction(L , 1);
}

void UIPhaseShop::cb_update_preview_slot()
{
	bool enable_frame = (get_current_menu_idx() == item_def::CTG_PARTS);
	bool enable_btn = (buy::GetCount_Preview() > 0);			// ������ ��Ͽ� 1���̻� ���� ���..
	
	m_pPreview_slot->SetEnable(enable_frame);

	LuaState * L = UILua::CallLuaFunction( m_menu_scn_info.scn, "EnablePreviewSlot");
	i3Lua::PushBoolean(L, enable_frame );
	i3Lua::PushBoolean(L, enable_btn );
	UILua::EndLuaFunction(L , 2);
}

void UIPhaseShop::cb_enable_compare_display_attr(bool bEnable)
{
	LuaState *L = _CallLuaFunction(m_menu_scn_info.idx, "EnableWeaponInfo2");
	i3Lua::PushBoolean(L, bEnable);
	UILua::EndLuaFunction(L , 1);
}


void UIPhaseShop::cb_set_compare_skip_button(bool bSkip)
{
	LuaState *L = _CallLuaFunction(m_menu_scn_info.idx, "EnableWeaponCompare");
	i3Lua::PushBoolean(L, bSkip);
	UILua::EndLuaFunction(L , 1);
}


/********************************/
UIPhaseShop::UIPhaseShop() : iMultiTap()
{
	g_this = this;
	m_phase_type = USP_SHOP;

	m_pPreview_slot = new PreviewSlot(this);
	MEMDUMP_NEW( m_pPreview_slot, sizeof(PreviewSlot));

	m_pSlot = new ShopSlot(this);
	MEMDUMP_NEW( m_pSlot, sizeof(ShopSlot));

	m_slide_offset_x = SLIDE_OFFSET;
	m_slide_offset_y = 0;
	slot_scene_name = "Scene/Main/PointBlankRe_Shop_Slot_V3.i3UIs";

	m_StartTab = 5;

}

UIPhaseShop::~UIPhaseShop()
{
	I3_SAFE_DELETE( m_pSlot);
	I3_SAFE_DELETE( m_pPreview_slot);
}


//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIPhaseShop::OnCreate( i3GameNode * pParent)
{
	iMultiTap::OnCreate( pParent);

	m_board_scn_info.idx = GetSceneCount();
	AddScene("Scene/Popup/PBRe_Popup_Shop_Board.i3UIs", shop_board_glue, true, false);

	m_pPreview_slot->OnCreate(pParent);
}

/*virtual*/ void UIPhaseShop::OnLoadAllScenes()
{
	iMultiTap::OnLoadAllScenes();

	m_pPreview_slot->OnLoadAllScenes();

	m_SigConnection += UISignal::UpdateCartBoard.connect(i3::bind(&UIPhaseShop::cb_update_cart_board, this));
	m_SigConnection += UISignal::UpdatePreviewSlot.connect(i3::bind(&UIPhaseShop::cb_update_preview_slot, this));
	m_SigConnection += UISignal::FinishBuyPopup.connect(i3::bind(&UIPhaseShop::cb_finish_buy_popup, this, i3::_1, i3::_2));
	m_SigConnection += UISignal::EnableCompareDisplayAttr.connect(i3::bind(&UIPhaseShop::cb_enable_compare_display_attr, this, i3::_1));
	m_SigConnection += UISignal::SetCompareSkipButton.connect(i3::bind(&UIPhaseShop::cb_set_compare_skip_button, this, i3::_1));

	m_SigConnection += UISignal::FinishBuyItem.connect(i3::bind(&UIPhaseShop::cb_finish_buy_item, this, i3::_1, i3::_2));

	m_SigConnection += UISignal::FinishAuthItem.connect(i3::bind(&UIPhaseShop::cb_finish_buy_auth_item, this, i3::_1, i3::_2));

	m_SigConnection += UISignal::FinishPreviewAuthItem.connect(i3::bind(&UIPhaseShop::cb_finish_preview_auth_item, this, i3::_1, i3::_2));

//	UI::insert_cb("update_cart_board", this, &UIPhaseShop::cb_update_cart_board);
//	UI::insert_cb("update_preview_slot", this, &UIPhaseShop::cb_update_preview_slot);
//	UI::insert_cb( "finish_buy_popup", this, &UIPhaseShop::cb_finish_buy_popup);
//	UI::insert_cb("enable_compare_display_attr", this, &UIPhaseShop::cb_enable_compare_display_attr);
//	UI::insert_cb("set_compare_skip_button", this, &UIPhaseShop::cb_set_compare_skip_button);

//	UI::insert_cb( "finish_buy_item", this, &UIPhaseShop::cb_finish_buy_item);

//	UI::insert_cb( "finish_buy_auth_item", this, &UIPhaseShop::cb_finish_buy_auth_item);
//	UI::insert_cb( "finish_preview_auth_item", this, &UIPhaseShop::cb_finish_preview_auth_item);



	LuaState * L = 0;
	
	L = _CallLuaFunction(m_menu_scn_info.idx, "UseInvenBtn");
	i3Lua::PushBoolean(L, false);
	UILua::EndLuaFunction(L , 1);

	L = _CallLuaFunction(m_menu_scn_info.idx, "EnableCash");	//ĳ�� ���� ���� ����
	i3Lua::PushBoolean(L,  UserInfoContext::i()->IsChargeCashEnable() ? true : false );
	UILua::EndLuaFunction(L , 1);

	m_board_scn_info.scn = GetScene( m_board_scn_info.idx );
	L = _CallLuaFunction(m_board_scn_info.idx, "LoadAllScenes");
	UILua::EndLuaFunction(L , 0); 

	L = _CallLuaFunction(m_menu_scn_info.idx, "TabEnable");
	i3Lua::PushInteger(L, 5);
	UILua::EndLuaFunction(L, 1);

	//���⸦ �����ش�.
	//clicked_tap(item_def::CTG_WEAPON, -1);
	clicked_tap(item_def::CTG_NEWHOT, item_def::MASKNEWHOT_ALL);
	clicked_menu_impl(item_def::CTG_NEWHOT);
	cb_update_cart_board();

	OnRevive(0);
	open_menu(item_def::CTG_NEWHOT);

	cb_update_preview_slot(); //preview btn�� ��Ȱ��ȭ �뵵�� ȣ���Ѵ�.
}

void	UIPhaseShop::OnUnloadAllScenes()
{
//	UI::remove_cb(this);
	m_SigConnection.clear();

	m_pPreview_slot->OnUnloadAllScenes();

	iMultiTap::OnUnloadAllScenes();
}

void UIPhaseShop::OnEntranceEnd() 
{
	iMultiTap::OnEntranceEnd();

	// �����ؾ��ϴ� ���� ���� ���� �ƴ� ���
	if(m_StartTab != 5)
	{
		iMultiTap::clicked_menu(m_StartTab);

		g_this->clicked_tap(m_StartTab, -1);
		SelectedEnteranceTab(m_StartTab);
		set_current_menu_idx(m_StartTab);
		m_StartTab = 5;
	}

	// �强�� ������ ���޿��� Ȯ��
	if(UserInfoContext::i()->IsGeneralRecvData())
	{
		if(UserInfoContext::i()->IsGeneralMsgBox())
			return;
		else
			UserInfoContext::i()->SetGeneralMsgBox();

		INVEN_BUFFER itemInfo = UserInfoContext::i()->GetGeneralRankInfo();

		// ���� üũ
		if(usf::GetItemStatus(itemInfo._ui32ItemID, itemInfo._TItemWareDBIdx) == ITEM_STATUS_EQUIPPED)
		{
			g_pFramework->EquipWeaponToMyInven(itemInfo._ui32ItemID, true);
		}

		GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PROMOTION4"));
	}

}

bool UIPhaseShop::OnRevive( i3RenderContext * pCtx)
{
	if( pCtx && iMultiTap::OnRevive( pCtx) == false) //OnLoadAllScenes���� OnRevive �� �Լ��� ȣ���Ҷ� �Ű����ڷ� 0�� �ִ´�. 
		return false;

	if (isEnable() == false) return false;

	m_pPreview_slot->OnRevive(pCtx);

	return true;
}

/*virtual*/ bool UIPhaseShop::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	iMultiTap::OnKeyInput(pInputMng);

	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();

	if(m_SearchEditBox->isFocused() == false && pKeyboard->GetStrokeState( I3I_KEY_SPACE) == true )
	{
//		UI::call_cb( "smenu_ability_close" );	//ability â�� ����.
		UISignal::SMenuAbilityClose();

		clicked_cart_board();	//��ٱ��� ���� â�� Ȱ��ȭ �Ѵ�.
		return true;
	}

#if defined(I3_DEBUG) && 0
	if( GameUI::IsKeyboardStroked(I3I_KEY_ENTER) )
	{
		if( GetMainFrame()->IsOpenedPopup(UPW_GACHA_WEAPON_DETAIL) == false )
		{
			T_ItemID item_id = ((ShopSlot*)m_pSlot)->get_get(); //AK Sopmod
			//T_ItemID item_id = 104011; //AK Sopmod
			GetMainFrame()->OpenPopup(UPW_GACHA_WEAPON_DETAIL, &item_id);
		}
		return true;
	}
#endif

	return false;
}

void UIPhaseShop::init_slide_impl()
{
	UI::tmSlideAttContainer list(m_menu_scn_info.scn);
	list.add("i3UIFrameWnd");
	m_tmSlide.add(list);
}

void UIPhaseShop::update_impl( REAL32 rDeltaSeconds)
{
	if (m_ppImgSet == nullptr)
		return;

	m_pPreview_slot->OnUpdate(rDeltaSeconds);
}

void UIPhaseShop::entrance_end_impl()
{
	m_pPreview_slot->MoveBackScene();
}

/*virtual*/ void UIPhaseShop::Enter(void)
{
}

/*virtual*/ void UIPhaseShop::Leave(void)
{
	if( CShopContext::i()->InServer())
		CShopContext::i()->LeaveServer();

	// leave �ÿ� �� �ʿ䰡 ���� ����. �̹� �ʿ��� ������ ���� ó���ǰ� �ִ�.
	// Shop -> Clan �̵��� �Ʒ� ������ ó���ϱ� ������ 15419 �� ���� �̽��� �߻���Ŵ.
	//SMenuAbility* ability = 
	//	static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
	//ability->EnableMainInfo(false);
	//ability->EnableExInfo(false);

	ItemConsumer::instance()->InitItemConsumer();

	UIMainFrame_V11 * pUIMainFrame_V11 = (UIMainFrame_V11*)g_pFramework->GetUIMainframe();
	if (pUIMainFrame_V11 != nullptr && !(i3::kind_of<UIMainFrame_V11*>(pUIMainFrame_V11)))
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REVERT_CHARA, REVERT_TYPE_ALL);
	}
}

void UIPhaseShop::enable_ability_main_info(bool enable)
{
	SMenuAbility* ability = 
		static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));

	if(ability == nullptr)
		return;

	ability->EnableMainDefaultInfoOnly();
	ability->EnableMainInfo(enable);
}

void UIPhaseShop::get_left_top_position(INT32& left, INT32 & top)
{
	i3UIFrameWnd* main_frame = (i3UIFrameWnd*)m_menu_scn_info.scn->FindChildByName( "i3UIFrameWnd");
	I3ASSERT( main_frame != nullptr);
	VEC3D* pos = main_frame->getAccumulatedPos();
	left = (INT32)pos->x; top = (INT32)pos->y;
}

void UIPhaseShop::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	iMultiTap::ProcessGameEvent(event, Arg, UserData);

	if( event == EVENT_CHANGE_MAINCHARA_FROM_ROOM)
	{
		I3_BOUNDCHK( Arg, S2MO_MULTI_SLOT_COUNT);
		m_pCharaInfoMgr->m_current_state.chara_idx = static_cast<UINT8>(Arg);
	}
}

void UIPhaseShop::SetListFrontItemByItemID(UINT32 ui32ItemID)
{
	static_cast<ShopSlot*>(m_pSlot)->set_list_front_item_by_ItemID(ui32ItemID);
}

void UIPhaseShop::SetEnteranceTab(INT32 idx)
{
	m_StartTab = idx;
}