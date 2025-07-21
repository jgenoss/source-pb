#include "pch.h"
#include "PurchaseMgr.h"

#include "Shop.h"
#include "UIUtil.h"

#include "UIItemInfoUtil.h"
#include "UIMainFrame.h"
#include "UIPopupItemBase.h"
#include "UIShopFunction.h"
#include "InvenUtil.h"
#include "UICharaInfoMgr.h"
#include "BuyUtil.h"
#include "UIFloatCharacterView.h"
#include "ItemConsumer.h"

#include "i3Base/string/ext/generic_string_ncopy.h"
#include "i3Base/string/ext/format_rcstring.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"
#include "ShopContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/ColorString.h"
#include "Designation/DesigUtil.h"

#include "UISignal.h"

// detail ���ӽ����̽��� ���� ���� �ƴϾ�����, ���� ���Ľ��Ŵ����� �������� �ؾ����� ���� ��Ȳ�̹Ƿ�, �ִ��� �ǹ��ִ� ���ܾ�� ��ü�մϴ�.  �������� �������̽��� �ִ��� ������ ����..
namespace buy
{	
	namespace detail
	{
		struct function_init
		{
			function_init() { i3::function<void(INVEN_BUFFER*)>::allow_global_variable(); }
		} _function_init_;
	}

	struct AuthResultInfo
	{
		i3::vector<bool> result_list;
		size_t			 request_count;
	};
	
	struct SelectedItemInfo
	{
		T_ItemDBIdx			ware_idx;
		const SHOP_GOODS*	goods;
		UINT32				item_id;
	};
	
	struct CartPurchaseResultInfo
	{
		i3::vector<INT32>				resultList;
		i3::vector<T_GoodsID>			GoodsIDList;
	};

	struct GlobalState
	{
		UIBase*					currUI;
		PurchaseRouteType		purchaseRoute;
		DisplayedGoodsList		purchaseGoodsList;
		
		GiftInfo				giftInfo;
		AuthResultInfo			authResultInfo;  // �������� ���ϴ�.. ����/����/������ �Ƚ��..
		SelectedItemInfo		selectedItemInfo;	// ���� ��ǰ ���ſ� ���� �����մϴ�...�κ�/���������� ���õ� ������ ����.(����/�Ⱓ ����/���� ���� ����) --> Ư�� ������ ó�� �� ���������Դϴ�..����..�̰Ž� �������Դϱ�.

		CartPurchaseResultInfo  cartPurchaseResultInfo;

		i3::function<void (INVEN_BUFFER*)>	addInvenBuffer;
		i3::vector<INVEN_BUFFER*> purchasedInvenItemList;	// �ֱ� ���ŵ� ���..(���� ���� ����ó������ ���ϴ�.) --> INVEN_BUFFER�� ���ٷ� ���°� �����Ǵ��Ͽ��� ����� ���̴µ�..�ð��� ��� �ϴ� ��ġ�մϴ�.
		
		UISecCount				sec_count;		// �ð���� ��ƿ..
		bool					working;
		bool					is_point_and_cash_try_purchase;
	};

	static GlobalState g_State;				// ������ ������ ������ �����Ƿ� PurchaseManager �ʱ�ȭ�� �ʿ��Ұ� ����...

	namespace detail
	{
		struct State_Init
		{
			State_Init() 
			{ 
				g_State.currUI = nullptr;
				g_State.purchaseRoute = PurchaseRoute_Immediate;
				g_State.working = false; 
				g_State.is_point_and_cash_try_purchase = false;
			}
		} _state_init;

	
		
		
		void AddInvenBuffer(INVEN_BUFFER* buffer)
		{
			buy::g_State.purchasedInvenItemList.push_back(buffer);
		}

		enum { ACK_TIME_OUT = 20 };

		void StartEvent()
		{
			buy::g_State.working = true;
			if (buy::g_State.currUI)
			{
				buy::g_State.currUI->SetInputDisable(TRUE);
			}

			buy::g_State.sec_count.SetBaseSec(detail::ACK_TIME_OUT);
			buy::g_State.sec_count.Start();
		}
		
		void EndEvent() 
		{
			if (buy::g_State.currUI)
			{
				//���� ������ �Է��� ���� ���¸� Ǯ����� �ϴµ�. �� ������ popup�� Ȱ��ȭ ���¶�� ���� �Է��� �� Ǭ��.
				//�Է��� ���� �����̱� ������ �˾��� Ȱ��ȭ �� �ҰŶ� ���������, Ȥ�� Ȱ��ȭ �� ��쿡 ������ �Ǳ⿡ �ּ�ó��
				//�ּ� ó���Ѵٰ� ������ ������ ������ ����.
				//if( g_pFramework->GetUIMainframe()->IsPopupFocused() == false ) //Ȥ�� ���� �ڵ带 �ּ�ó���Ѵ�.
				{
					buy::g_State.currUI->SetInputDisable(FALSE);
				}
			}

			buy::g_State.currUI = nullptr;
			buy::g_State.working = false;
			buy::g_State.sec_count.Stop();

			if (buy::g_State.addInvenBuffer.empty() == false)
				buy::g_State.addInvenBuffer.clear();
		}
		
		class SecCountCallback : public	UISecCountCallback
		{
			virtual void OnStart(INT32 result_time) {}
			virtual void OnSecChange(INT32 result_time) {}
			virtual void OnStop(INT32 result_time) {}

			virtual void OnDone(INT32 result_time)
			{
				buy::detail::EndEvent();

				switch (buy::g_State.sec_count.GetBaseSec())
				{
				case ACK_TIME_OUT:
#ifndef _DEBUG
					//	������ ���� Ȯ���� ������ �亯�� ���� ������ ��ٸ���. 
					//	���� ��� �ð��� �ʰ��� ��� ������ ���� �亯�� ���޾Ҵٰ� �ǴܵǹǷ� ������ ���� �����մϴ�.

					/*��Ʈ��ũ ������ ������ ���� �Ǿ����ϴ�.*/
					GameUI::ExitGamePopup(GAME_RCSTRING("STBL_IDX_EP_BATTLE_TIMEOUT_HOST_C"));
#else
					/*��Ʈ��ũ�� ������ �����߽��ϴ�.\n �ٽ� �õ��� �ֽʽÿ�.*/
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_FAILED_BY_NET"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
#endif
					break;
				}
			}

		} g_SecCountCB;

	}

}

//- PurchaseMgr ----------//
PurchaseMgr* PurchaseMgr::i()
{
	static PurchaseMgr instance;
	return &instance;
}

PurchaseMgr::PurchaseMgr()
{
	buy::g_State.addInvenBuffer.clear();
	buy::g_State.sec_count.SetReverse(true);
	buy::g_State.sec_count.SetCallback(&buy::detail::g_SecCountCB);

	RegisterGameEventFunc( EVENT_BUY_GOODS,			&PurchaseMgr::ReceiveEvent_Buy);
	RegisterGameEventFunc( EVENT_BUY_EXTEND_GOODS,	&PurchaseMgr::ReceiveEvent_Extend);
	RegisterGameEventFunc( EVENT_ITEM_REPAIR,		&PurchaseMgr::ReceiveEvent_Repair);
	RegisterGameEventFunc( EVENT_AUTH_DELETE,		&PurchaseMgr::ReceiveEvent_Auth_Delete);
	RegisterGameEventFunc( EVENT_CREATE_CHARA,		&PurchaseMgr::ReceiveEvent_Buy_Chara);		// �̰͵� ���ذ� �Ȱ���...(������ ����ؼ� ��� ������....OK)
	RegisterGameEventFunc( EVENT_SET_CHARA,			&PurchaseMgr::ReceiveEvent_Buy_Chara);		// ��..��..�̰� ���İ�...�� �� ���� ���°ǵ�...�˴� ���͸� ���..
	RegisterGameEventFunc( EVENT_DELETE_CHARA,		&PurchaseMgr::ReceiveEvent_Delete_Chara);
	RegisterGameEventFunc( EVENT_ITEM_AUTH,			&PurchaseMgr::ReceiveEvent_Item_Auth);
	RegisterGameEventFunc( EVENT_SEND_GIFT,			&PurchaseMgr::ReceiveEvent_Send_Gift);
}

PurchaseMgr::~PurchaseMgr()
{
	
}

void PurchaseMgr::OnNotify_NewInvenBufferAdded(INVEN_BUFFER* newInvenBuffer)
{
	buy::g_State.addInvenBuffer(newInvenBuffer);
}

void PurchaseMgr::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void PurchaseMgr::ProcessGameEvent( GAME_EVENT evt, INT32 arg, i3::user_data& UserData)
{
	// Update Receive Event
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & f = it->second;
		(this->*f)(arg, UserData);
	}
}

namespace
{
	static void BuyResult_Sucess_Imp() // �ϴ� 1��¥��,��ٱ��� ��ο��� ���̵��� �մϴ�.. (���� �ڵ����Դϴ�. ���� ������� �����׿�.  ������ �� ��ġ�� ���ڴ�..)
	{

#if defined(USE_SALE_COUPON)
		// ���� ������ �κ����� ����
		CInvenList::i()->DeleteOnlyUsedCouponBuffer();
#endif 

		if (buy::g_State.purchaseRoute == buy::PurchaseRoute_Preview)
			return; //�̸����� ����

#if !defined(AUTH_FROM_BUY_GOODS)
					//	�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
		return;
#else // defined(AUTH_FROM_BUY_GOODS)

		if (buy::g_State.purchaseRoute == buy::PurchaseRoute_Cart)	//��ٱ��� ����
		{
			/*�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
			return;
		}

		const buy::DisplayedGoodsPtr& displayedGoods = buy::g_State.purchaseGoodsList[0];

		const SHOP_GOODS* buy_goods = displayedGoods->GetCurrentGoods();

		//��� ���� ��ƾ
		if (CInvenList::i()->HaveItemByItemID(buy_goods->_ItemID))
		{
			CShop* shop = CShop::i();

			//Set �������� ���� ����, �κ��� ����.
			if (shop->IsPackageGoods(buy_goods->_GoodsID))
			{
#if !defined(TEMP_ERASE_CODE)
				UINT32 point = 0;

				SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(buy_goods->_GoodsID);
				for (INT32 i = 0; i < pGoodsList->_ui8ItemCount; ++i)
				{
					//����Ʈ ��ǰ�� ���
					if (ITEM_TYPE_POINT == GET_ITEM_TYPE(pGoodsList->_ppItemData[i]->_ui32ItemID))
					{
						switch (GET_ITEM_NUMBER(itemId))
						{
						case 1: point += 1000;		break;
						case 2: point += 5000;		break;
						case 3: point += 10000;		break;
						case 4: point += 25000;		break;
						case 5: point += 50000;		break;
						case 6: point += 100000;	break;
						case 7: point += 20000;		break;
						}
					}
				}

				if (0 < point)
				{
					//	"%d ����Ʈ�� ���޵Ǿ�����\n \n�����Ͻ� �������� �κ��丮����\nȮ���Ͻ� �� �ֽ��ϴ�."
					i3::rc_wstring wstr_buyMessage;
					i3::sprintf(wstr_buyMessage, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_PRICE_SUCCESS"), point);
					GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_buyMessage, &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
				}
				else
#endif
				{
					if (item_def::get_category_goodsID(buy_goods->_GoodsID) != item_def::CTG_ITEM)
					{
						/*�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.*/
						GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
					}
				}
			}
		}

#endif // defined(AUTH_FROM_BUY_GOODS)
	}
		
	static void BuyResult_Fail_Imp(INT32 Arg) // �ϴ� 1��¥�������� ���̵��� �մϴ�.. (���� �ڵ����Դϴ�. �ټ� �����׿�. ���� �ִ� BuyResult_Sucess_Imp���ٴ� �� �����ϴ�...)
	{
		i3::rc_wstring wstrErrorMsg;

		switch (Arg)
		{
		case EVENT_ERROR_INVALID_COUPON:
			/* �������� �ʴ� ���� �Դϴ�.*/
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)", GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_ERROR_INVALID_COUPON"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			break;

		case EVENT_ERROR_EXPIRY_DATE_COUPON:
			/*��� �Ⱓ�� �ùٸ��� �ʽ��ϴ�.*/
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)", GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL_REASON_LIMIT_TIME"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			break;

		case EVENT_ERROR_COUPON_DATA:
			/* �߸��� ���� �������̰ų� ���� ��� ���ǿ� ���� �ʽ��ϴ�. */
			i3::sprintf(wstrErrorMsg, L"%s\n(%s)", GAME_RCSTRING("STBL_IDX_COUPON_USE_FAIL"), GAME_RCSTRING("STBL_IDX_ERROR_COUPON_DATA"));
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL"));
			break;

		case 0X83000045:
			// �������� �ٲ�鼭 �̹� ������ �ִ� �������̱� ������ ���� ���� -- �����۾� �ʿ� -- (�� �װ��� ��������)
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_E"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
			break;

		default:
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
			break;
		}
	}

	// bool detail::param_util::result_buy_ack_and_msg()  ���� �̷� �Լ��ε�....�׳� ���� �׷����غ�����, �Լ� �̸����� ��� �����ϱⰡ ����ϴ�.. 
	// ������ ���������� �����Ҹ��� ������ ���˴ϴ�..

	static bool CheckResultBuyAck_Imp( const i3::vector<INT32>& resultList, const i3::vector<T_GoodsID>& resGoodsIDList)
	{
		i3::vector<INT32>& purchaseResultList =	buy::g_State.cartPurchaseResultInfo.resultList;
		i3::vector<T_GoodsID>& purchaseResultGoodsIDList = buy::g_State.cartPurchaseResultInfo.GoodsIDList;
		
		purchaseResultList.insert(purchaseResultList.end(), resultList.begin(), resultList.end());
		purchaseResultGoodsIDList.insert(purchaseResultGoodsIDList.end(), resGoodsIDList.begin(), resGoodsIDList.end());

		if (buy::g_State.is_point_and_cash_try_purchase)   // RECV�� �ѹ��� �޾ƾ� �Ѵٰ� �ϳ׿�...
			return false;
		
		bool is_success = true;
		bool inven_full_error = false;

		i3::stack_wstring wstr_fail_item_name;

		const i3::vector< buy::DisplayedGoodsPtr > & goodsList = buy::g_State.purchaseGoodsList;

		const size_t num_good_to_buy = goodsList.size();				// �̰��� ���� �����Դϴ�..
			
		for (size_t i = 0; i < num_good_to_buy; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = goodsList[i].get();
			const SHOP_GOODS* pCurrGoods = pDisplayedGoods->GetCurrentGoods();

			const size_t num_result = purchaseResultGoodsIDList.size();

			bool bSucessPurchaseEach = false;

			for (size_t j = 0; j < num_result; ++j)
			{
				T_GoodsID resGoodsID = purchaseResultGoodsIDList[j];

				if (pCurrGoods->_GoodsID == resGoodsID)
				{
					INT32 resultValue = purchaseResultList[j];

					if (EV_FAILED(resultValue) == false)		// ������ ���..
					{
						bSucessPurchaseEach = true;
					}
					else
					{
						if (resultValue == EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL_SHOP)
							inven_full_error = true;	//	�κ��丮�� ������ ���� ����.)

						i3::stack_string errString;
						i3::snprintf(errString, MAX_PATH, "BF[%d] : %x ", resGoodsID, resultValue);
						I3PRINTLOG(I3LOG_QA, errString);

					}
					
					i3::vu::erase_index(purchaseResultGoodsIDList, j);
					i3::vu::erase_index(purchaseResultList, j);
					break;			// �ݵ�� ������..
				}
			}

			if ( bSucessPurchaseEach == false)
			{
				is_success = false;
				i3::rc_wstring wstr_name;
				usf::GetShopItemName(wstr_name, (SHOP_GOODS_PACK*)goodsList[i]->GetGoodsPack());
				wstr_fail_item_name += wstr_name;
				wstr_fail_item_name += L',';
			}

		}
		
		/*
		const size_t num_result = resultList.size();					// �̰��� ��ȿ�� ������ ���ο��� ���ǰ˻�� �ɸ��ϴ�.
		
		for (size_t i = 0; i < num_good_to_buy; ++i)
		{
			if ( i < num_result )
			{
				INT32 resultValue = resultList[i];
				// ���� �α� ����մϴ�.

				if ( EV_FAILED(resultValue) )
				{
					i3::stack_string errString;
					i3::snprintf(errString, MAX_PATH, "BF[%d] : %x ", i, resultValue);
					I3PRINTLOG(I3LOG_QA, errString);

					if (resultValue == EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL_SHOP)	
						inven_full_error = true;	//	�κ��丮�� ������ ���� ����.
				
					i3::rc_wstring wstr_name;
					usf::GetShopItemName(wstr_name, (SHOP_GOODS_PACK*)goodsList[i]->GetGoodsPack());
					wstr_fail_item_name += wstr_name;
					wstr_fail_item_name += L',';

					is_success = false;
				}
			}
			else  // ������ ������ �����Դϴ�..
			{
				i3::rc_wstring wstr_name;
				usf::GetShopItemName(wstr_name, (SHOP_GOODS_PACK*)goodsList[i]->GetGoodsPack());
				wstr_fail_item_name += wstr_name;  
				wstr_fail_item_name += L',';
				is_success = false;
			}
		}
		*/
		
		if (is_success == false)
		{
			i3::stack_wstring fail_msg = GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_E");
			fail_msg += L'\n';

			wstr_fail_item_name.erase(wstr_fail_item_name.size() - 1); //�ǵ� "," ����
			fail_msg.append(wstr_fail_item_name.begin(), wstr_fail_item_name.end());

			/*���Խ���*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, fail_msg, &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_FAIL_CAPTION_E"));
		}

		if (inven_full_error)
			GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_POPUP_MESSAGE_INVENTORY_FULL_BUY_FAIL"));
		
		purchaseResultList.clear(); 
		purchaseResultGoodsIDList.clear();
		
		return is_success;
	}
	
}


// ������ ���ú� ��Ŷ�� �ѹ��� ��������, Ŭ������ ��ǰ������ �ɰ��� ���ú� ó���� �ϴ��� ó�� 1ȸ�� ���ƽ��ϴ�....( �̰� �� �������� ���̳� �̰���...)
// ĳ���ʹ� �̸��� ���� �ȵ˴ϴ�..

void PurchaseMgr::ReceiveEvent_Buy( INT32 arg, const i3::user_data &UserData)
{
	if( buy::g_State.working == false ) return;				// �������� �̸��� ���� �̻��� ���̽��� �̰����� �ɷ����������� ��������..(������ �ʳ�)
	
	bool ack_result;

	if (buy::g_State.purchaseRoute == buy::PurchaseRoute_Immediate)
	{
		ack_result = (EV_SUCCESSED(arg)); // 0�� �ε���.. 
		if (ack_result == true)
			BuyResult_Sucess_Imp();
		else
			BuyResult_Fail_Imp(arg);
	}
	else  // ��ٱ��� �� �̸������ ����˴ϴ�..
	{
		const RecvData_EVENT_BUY_GOODS& data = i3::value_cast<RecvData_EVENT_BUY_GOODS>(UserData);
		const i3::vector<INT32>& resList = data.resultList;
		const i3::vector<T_GoodsID>& resGoodsIDList = data.GoodsIDList;

		ack_result = CheckResultBuyAck_Imp(resList, resGoodsIDList);

		if (ack_result == true)
			BuyResult_Sucess_Imp();
		else
		if (buy::g_State.is_point_and_cash_try_purchase)
		{
			buy::g_State.is_point_and_cash_try_purchase = false;   // ������ �ѹ� �� �ޱ� ���� ����..
			return;
		}
	}
	
	buy::detail::EndEvent();
	
	UISignal::FinishBuyItem(ack_result, buy::g_State.purchaseGoodsList);			// ���������� ����.  ���������� ���� ó��..(������ �μ��� �ƿ� �޶����)
																					// ���������� ���� (��°�μ��� ���� ������ ������ �״�� ��ġ)
	I3TRACE("PurchaseMgr::ReceiveEvent_Buy \n");				
}

// ĳ���� ������ ���, ��� ���ſ� ���մϴ�.. �ٸ� ��η� ���� ���� ��� �̻��� �̴ϴ�...

void PurchaseMgr::ReceiveEvent_Buy_Chara( INT32 arg, const i3::user_data &UserData)
{
	if(buy::g_State.working == false ) return;

	if (buy::g_State.purchaseGoodsList.size() > 1)			// ��ٱ���, �̸����ⱸ�� �� 2���̻� ������ ���� ���Ž� �������� ���ƿ´ٸ� ���� ƨ�ܳ�..
		return;
	
//	I3ASSERT_RETURN(buy::g_State.purchaseRoute == buy::PurchaseRoute_Immediate);

	bool ack_result = ( EV_SUCCESSED( arg ) );

	if( ack_result == true )
		BuyResult_Sucess_Imp();
	else
		BuyResult_Fail_Imp( arg );
		
	buy::detail::EndEvent();

	UISignal::FinishBuyItem(ack_result, buy::g_State.purchaseGoodsList);			// ���������� ����.  ���������� ���� ó��..(������ �μ��� �ƿ� �޶����)
																					// ���������� ���� (��°�μ��� ���� ������ ������ �״�� ��ġ)

	I3TRACE( "PurchaseMgr::ReceiveEvent_Buy_Chara \n");				// ����μ� �˸��� ���� Ʈ���̽�. �Լ��� ū ������ ������ ���߿� ����..
}

// ĳ���ʹ� �̸��� ���� �ȵ˴ϴ�..
void PurchaseMgr::ReceiveEvent_Auth_Delete( INT32 arg, const i3::user_data &UserData)
{
	if(buy::g_State.working == false ) return;

	ReceiveEvent_Delete_Chara( arg, UserData);				// �̷� ��ģ...���̹�...
}

void PurchaseMgr::ReceiveEvent_Delete_Chara( INT32 arg, const i3::user_data &UserData)
{
	if(buy::g_State.working == false ) return;

	bool ack_result = ( EV_SUCCESSED( arg ) ) ? true : false;
	if( ack_result )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_INVEN_DELETE_ITEM_SUCCESS"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));

		ITEM_INFO info;
		info.m_TItemID = buy::g_State.selectedItemInfo.item_id;			
		info.m_TItemDBIdx = buy::g_State.selectedItemInfo.ware_idx;		

		buy::detail::EndEvent();

		UISignal::FinishRemoveItem(true, &info);
	//	detail::param_util::call_finish_cb_string(true, &info);
			
	}
	else
	{
		const RecvData_EVENT_DELETE_CHARA &data = i3::value_cast<RecvData_EVENT_DELETE_CHARA>(UserData);
		CCharaInfoContext::i()->SetMainCharaBySlotIdx( data._DeleteSlotIdx );
		UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));
		if( pCharaViewWin != nullptr)
			pCharaViewWin->SetChangeChara(0, true);

		/*��ǥ ĳ���� ������ �� �� �Ǿ����ϴ�.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INCORRECT_MAIN_CHARACTER"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));

		buy::detail::EndEvent();

		UISignal::FinishRemoveItem(false, nullptr);
	//	detail::param_util::call_finish_cb_string(false);

	
	}
}

namespace
{
	// ������ ��Ŷ�� ������ ������ �ϰ� ��...( �������Ŵ� �ϰ� ���� ������ �Ǿ� �ִµ�, ������ �׳� ������ �����°ɷ� �ϴ� ��ɱ�..)

	static void SendAuthPacket_Imp(INT32 currRequestIndex)
	{
		const i3::vector<INVEN_BUFFER*>& catched_inven_buffer_list = buy::g_State.purchasedInvenItemList;
		
		if (currRequestIndex >= (INT32)catched_inven_buffer_list.size()) return;

		INVEN_BUFFER* buffer = catched_inven_buffer_list[currRequestIndex];

		buy::g_State.selectedItemInfo.ware_idx = buffer->_TItemWareDBIdx;
		buy::g_State.selectedItemInfo.item_id = buffer->_ui32ItemID;

		GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, &buy::g_State.selectedItemInfo.ware_idx, 
														&buy::g_State.selectedItemInfo.item_id);

	}

	// ����Ǹ� false�� ����Դϴ�... ���ϰ��� �Լ������� ���߰� �ȵȴ�. (�ټ� �����ٴ� �̴ϴ�..)...

	static bool TrySendAuth_Imp(bool ack_result)
	{
		buy::g_State.authResultInfo.result_list.push_back(ack_result);

		size_t ack_size = buy::g_State.authResultInfo.result_list.size();

		if (buy::g_State.authResultInfo.request_count <= ack_size)		// ����� ������ �����ų ���..�ƴ϶�� ��� �ݺ�...
			return false;

		SendAuthPacket_Imp(ack_size);					// ������ ��û ��Ŷ ��ȣ �����Դϴ�..
		return true;
	}



}

namespace
{
	
	void PrintMsgIfAuthResultError_Imp()
	{
		bool is_success = true;

		i3::stack_wstring wstr_fail_item_name;

		const i3::vector<INVEN_BUFFER*>& catched_inven_buffer_list = buy::g_State.purchasedInvenItemList;
			
		size_t size = buy::g_State.authResultInfo.result_list.size();

		for (size_t i = 0; i < size; ++i)
		{
			if (buy::g_State.authResultInfo.result_list[i] == false)
			{
				is_success = false;

				i3::rc_wstring wstr_name;
				usf::GetShopItemName(wstr_name, catched_inven_buffer_list[i]->_ui32ItemID);
				wstr_fail_item_name.append(wstr_name.begin(), wstr_name.end());
				wstr_fail_item_name += L',';
			}
		}

		if (is_success == false)
		{
			i3::stack_wstring fail_msg = GAME_RCSTRING("STR_POPUP_MESSAGE_FAILED_AUTH");
			fail_msg += L'\n';
			wstr_fail_item_name.erase(wstr_fail_item_name.size() - 1); //�ǵ� "," ����
			fail_msg.append(wstr_fail_item_name.begin(), wstr_fail_item_name.end());

			/*������ ��� ����*/ /*������ ��� ����*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, fail_msg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
		}
	}
}

void PurchaseMgr::ReceiveEvent_Item_Auth( INT32 arg, const i3::user_data &UserData)
{
	if( buy::g_State.working == false ) return;

	bool ack_result = ( EV_SUCCESSED( arg ) ) ? true : false;
	bool is_auth_send = TrySendAuth_Imp(ack_result);

	if(is_auth_send == true) 
		return;			// ��� ��û...

	PrintMsgIfAuthResultError_Imp();

	buy::detail::EndEvent();

	UISignal::CurrentFinishAuthItem(ack_result, buy::g_State.selectedItemInfo.goods); 	// �����䵵 �ֱ� ������ �Ű��� �� ����Ѵ�..�������� ��� �� �μ��� ���� ����δ�..

}

namespace
{

	// ��...����� ��� ���µ�...���� �ϴ� ���� ���� ����...

	static bool  CheckRepairAck_Imp( const i3::vector<INT32>& resultList)
	{
		bool is_success = true;

		const size_t size = resultList.size();		// ��û�� ������ ������ �ȸ������� �ְٽ��ϴ�...�ּ��� �Ѱ��� �����޼����� ����ֱ� ������ �ϴ� �̷��� ��ġ�մϴ�..

		for (size_t i = 0; i < size; ++i)
		{
			if (EV_FAILED(resultList[i] ) )
			{
				i3::stack_string strErrorLog;
				i3::snprintf(strErrorLog, MAX_PATH, "RF[%d] : %x ", i, resultList[i]);
				I3PRINTLOG(I3LOG_QA, strErrorLog);
				is_success = false; 
			}
		}

		i3::rc_wstring wstrMsg;

		if ( buy::g_State.selectedItemInfo.ware_idx != 0 )			// ���� ǰ�� ����.. ��ü������ wareDBIdx���� 0���� �����߽��ϴ�..
		{
			// �Ʒ� �޼����� ���� ������ �ش��� ���Դϴ�.  buy::g_State.selectedItemInfo�� ���� 1��ǰ�� �ش�Ǵ� ��Ȳ������...���� ĳ������ ��ü������ ���� ���� �ʽ��ϴ�...
			if (item_def::is_dino_category(buy::g_State.selectedItemInfo.item_id) == false)
			{
				wstrMsg = is_success ? GAME_RCSTRING("STR_POPUP_REPAIR_SUCCESS") : GAME_RCSTRING("STR_POPUP_REPAIR_FAIL");
			}
			else
			{
				wstrMsg = is_success ? GAME_RCSTRING("STR_POPUP_RECOVER_SUCCESS") : GAME_RCSTRING("STR_POPUP_RECOVER_FAIL");
			}
		}
		else 
		{
			wstrMsg = is_success ? GAME_RCSTRING("STR_POPUP_REPAIR_SUCCESS") : GAME_RCSTRING("STR_POPUP_REPAIR_FAIL");
		}

		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrMsg, &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));

		return is_success;
	}

}


void PurchaseMgr::ReceiveEvent_Repair( INT32 arg, const i3::user_data &UserData)
{
	if( EV_SUCCESSED( arg))
	{
		if(buy::g_State.working == false ) return;

		static const bool ack_result_sucess = true;

		const RecvData_EVENT_ITEM_REPAIR& resData = i3::value_cast<RecvData_EVENT_ITEM_REPAIR>(UserData);
				
		bool  ack_result = CheckRepairAck_Imp(resData.resultList);

		buy::detail::EndEvent();

		UISignal::CurrentFinishRepairItem(ack_result);
	}
}

// �Ⱓ ���� ���Ŵ� 1�����̷��ϴ�...���߿� �������� �ٲ�� �̰����� ��ġ�� �Ǵ°ų�...

void PurchaseMgr::ReceiveEvent_Extend( INT32 arg, const i3::user_data &UserData)
{
	if( buy::g_State.working == false ) return;

	bool ack_result = ( EV_SUCCESSED( arg ) ) ? true : false;

	const i3::rc_wstring& wstr_msg = ack_result ? GAME_RCSTRING("STR_POPUP_EXTEND_SUCCESS") : GAME_RCSTRING("STR_POPUP_EXTEND_FAIL");
	GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_msg, &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));

	buy::detail::EndEvent();
	UISignal::FinishBuyExtendItem(ack_result);

}

// �������Ŵ� �ٸ� ������� �����°Ŷ�..���ú�� ���� �޴°� ����..
namespace
{
	static bool CheckGiftAck_Imp( const i3::vector<INT32>& resultList )
	{
		bool ack_result = true;

		const size_t numResult = resultList.size();

		for (size_t i = 0; i < numResult; ++i)
		{
			INT32 res = resultList[i];
			if (EV_FAILED(res))
			{
				// ���� �α� ����մϴ�.
				/*EVENT_ERROR_SUCCESSED
				EVENT_ERROR_EVENT_GIFT_GOODS_NOT_ENOUGH_POINT	����Ʈ ����
				EVENT_ERROR_EVENT_GIFT_GOODS_WRONG_RECEIVE_USER ���� ������
				EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_GOODS	��Ʈ��ũ ����
				EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_USER	��Ʈ��ũ ����
				EVENT_ERROR_EVENT_CHEKC_GIFT_PARAMETER_GOODS	���� ��ǰ
				EVENT_ERROR_FAIL	��Ʈ��ũ ����*/

				i3::stack_string strErrorLog;
				i3::snprintf(strErrorLog, MAX_PATH, "AGF[%d] : %x ", i, res);
				I3PRINTLOG(I3LOG_QA, strErrorLog);
				ack_result = false;
			}
		}

		i3::rc_wstring wstr_result;
		i3::rc_wstring wstr_caption;

		if (ack_result)
		{
			i3::sprintf(wstr_result, L"[%s]%s", buy::g_State.giftInfo.wstr_receiver_nick, GAME_RCSTRING("STBL_IDX_EP_SHOP_GIFT_SUCCESS"));
			wstr_caption = GAME_RCSTRING("STBL_IDX_EP_SHOP_GIFT_SUCCESS_CAPTION");
		}
		else
		{
			wstr_result = GAME_RCSTRING("STBL_IDX_EP_SHOP_GIFT_FAIL_E");
			wstr_caption = GAME_RCSTRING("STBL_IDX_EP_SHOP_GIFT_FAIL_CAPTION");
		}

		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_result, &wstr_caption);

		return ack_result;
	}
}

void PurchaseMgr::ReceiveEvent_Send_Gift( INT32 arg, const i3::user_data &UserData)
{
	if( buy::g_State.working == false ) return;

	const RecvData_EVENT_SEND_GIFT& data = i3::value_cast<RecvData_EVENT_SEND_GIFT>(UserData);
	
	bool ack_result = CheckGiftAck_Imp(data.resultList);

	buy::detail::EndEvent();

	UISignal::FinishBuyGift(ack_result);
	
}

void	PurchaseMgr::OnUpdate( REAL32 rDeltaSeconds)
{
	buy::g_State.sec_count.Update();
}


namespace
{
	// �̷� �Լ��� �׳� ����� �Ϲ��Լ��� ������ѵ� �ɵ�..?

	static bool IsPeriodGoods_Imp(const SHOP_GOODS* goods)
	{
		SHOP_GOODS_LIST * pGoodsList = CShop::i()->GetGoodsListData(goods->_GoodsID);
		if (!pGoodsList) return false;

		UINT8 Type = 0; UINT32 Arg = 0;
		INVEN::DB_TYPE dbType = INVEN::ItemID2DBType(goods->_ItemID);
		if (CInvenList::i()->GetItemTypeByItemID(dbType, goods->_ItemID, &Type, &Arg) == false) return false;

		if (pGoodsList->_ui8ItemCount > 1) return false;	//��Ʈ �������̴�.

		if (pGoodsList->_ui8ItemCount == 1 && pGoodsList->_ppItemData[0]->_ui8AuthType == ITEM_TYPE_TIME)
			return true;

		return false;
	}

	static bool CheckPeriod_Imp()
	{
		const size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			if (IsPeriodGoods_Imp(pGoods))
			{
				return true;
			}
		}
		return false;
	}

	static bool IsDuplicablePurchaseGoods_Imp(const SHOP_GOODS* goods)
	{
		UINT8 Type = 0; UINT32 Arg = 0;
		INVEN::DB_TYPE dbType = INVEN::ItemID2DBType(goods->_ItemID);

		if (item_def::get_category_goodsID(goods->_GoodsID) == item_def::CTG_PARTS)	//parts�� �ߺ� ���� ����
		{
			if (goods->_AuthType < 1 || goods->_AuthType > 2)			// ������������ ��� _AuthType�� �Ⱓ��1, ������2 �ϰ�츸 �ߺ����Ű���
				return false;
		}

		if (g_pEnvSet->IsV2Version())
		{
			if (item_def::get_category_goodsID(goods->_GoodsID) == item_def::CTG_CHARA && //ĳ���� �ߺ� ���� ����
				item_def::get_type(goods->_ItemID) != ITEM_TYPE_DINO)
			{
				if (goods->_AuthType < 1 || goods->_AuthType > 2)			// ĳ���;������� ��� _AuthType�� �Ⱓ��1, ������2 �ϰ�츸 �ߺ����Ű���
					return false;
			}
		}

		return CInvenList::i()->GetItemTypeByItemID(dbType, goods->_ItemID, &Type, &Arg);	//�������̸� True
	}

	static bool CheckDuplicablePurchase_Imp()
	{
		size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			if (IsDuplicablePurchaseGoods_Imp(pGoods))
				return true;
		}
		return false;
	}
	
	static bool CheckMoney_Imp()
	{
		UINT32 tot_point = 0, tot_cash = 0;
		const size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			GOODS_PRICE_INFO info;
			usf::GetShopGoodInfo(pGoods, info);

			if (pDisplayedGoods->GetCurrentPriceType() == GOODS_PRICE_CASH)
				tot_cash += info._Cash;
			else
				tot_point += info._Point;
		}

		UINT32 nHaveCash = UserInfoContext::i()->GetMyCash();
		UINT32 nHavePoint = UserInfoContext::i()->GetMyPoint();

		if (tot_point == 0) return (INT32(nHaveCash) - INT32(tot_cash)) >= 0 ? true : false;
		if (tot_cash == 0) return (INT32(nHavePoint) - INT32(tot_point)) >= 0 ? true : false;

		return ((INT32(nHavePoint) - INT32(tot_point)) >= 0 && (INT32(nHaveCash) - INT32(tot_cash)) >= 0);
	}

	static bool CheckClanMaxPerson_Imp()
	{
		const UINT8 PLUS_10 = 10; const UINT8 PLUS_50 = 50;
		UINT8 plus_person = 0;

		const size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			if (item_def::get_category_goodsID(pGoods->_GoodsID) == item_def::CTG_ITEM)
			{
				if (pGoods->_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10 || pGoods->_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_50)
				{
					CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();

					if (pGoods->_ItemID == CASHITEM_ITEMID_CLAN_PERSON_PLUS_10) plus_person = PLUS_10;
					else plus_person = PLUS_50;

					//	Ŭ�����������������ִ�ȸ�����������û��Ұ�
					if (pClanInfo->m_maxPerson + plus_person > MAX_MEMBER_COUNT)
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	static bool CheckAnnounceUseOnlyClanMaster_Imp()
	{
		size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			const SHOP_GOODS* pGoods = pDisplayedGoods->GetCurrentGoods();

			if (item_def::get_category_goodsID(pGoods->_GoodsID) == item_def::CTG_ITEM)
			{
				if (pGoods->_ItemID == CASHITEM_ITEMID_CLR_CLAN_HISTORY)
					return true;
			}
		}
		return false;
	}

	static void 	UpdatePointAndCashTryPurchase()
	{
		buy::g_State.cartPurchaseResultInfo.resultList.clear();
		buy::g_State.cartPurchaseResultInfo.GoodsIDList.clear();
		

		bool isPointPurchase = false;
		bool isCashPurchase = false;
		const size_t size = buy::g_State.purchaseGoodsList.size();

		for (size_t i = 0; i < size; ++i)
		{
			buy::DisplayedGoods* pDisplayedGoods = buy::g_State.purchaseGoodsList[i].get();
			GOODS_PRICE_TYPE priceType = pDisplayedGoods->GetCurrentPriceType();

			if (priceType == GOODS_PRICE_POINT)
				isPointPurchase = true;
			else
			if (priceType == GOODS_PRICE_CASH)
				isCashPurchase = true;
		}

		if (isPointPurchase && isCashPurchase)
		{
			buy::g_State.is_point_and_cash_try_purchase = true;
		}
	}


}

namespace MsgBoxCB
{

	static void BuyMoreCB(void*, INT32 param)
	{
		if (param == MBR_OK)
		{
			buy::detail::StartEvent();
			buy::g_State.purchasedInvenItemList.clear();
			buy::g_State.addInvenBuffer = buy::detail::AddInvenBuffer;

			// Set ���������� üũ�մϴ�.
			const buy::DisplayedGoodsPtr& displayedGoods = buy::g_State.purchaseGoodsList[0];

			const SHOP_GOODS* pGoods = displayedGoods->GetCurrentGoods();

			if (GET_GOODS_TYPE(pGoods->_GoodsID) == ITEM_TYPE_SET)
			{
				// Set Item�� ��쿡�� Buy Goods�θ� �����ϴ�.

				UpdatePointAndCashTryPurchase();

				GameEventSender::i()->SetEvent(EVENT_BUY_GOODS);
			}
			else
			{
				T_ItemID item_id = pGoods->_ItemID;

				if (item_def::is_chara_category(item_id) == true)
				{
					GameEventSender::i()->SetEvent(EVENT_BUY_CHAR_GOODS);
				}
				else
				{
					UpdatePointAndCashTryPurchase();
					GameEventSender::i()->SetEvent(EVENT_BUY_GOODS);
				}
			}
		}
	}

}

namespace
{

	static void BuyItemPeriodExtension_Imp(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList)
	{
		buy::g_State.currUI = pUIBase;
		buy::g_State.purchaseRoute = routeType;
		buy::g_State.purchaseGoodsList = goodsList;
		buy::g_State.is_point_and_cash_try_purchase = false;

		buy::detail::StartEvent();
		buy::g_State.purchasedInvenItemList.clear();
		buy::g_State.addInvenBuffer = buy::detail::AddInvenBuffer;

		GameEventSender::i()->SetEvent(EVENT_BUY_EXTEND_GOODS);
	}

	static void Buy_Imp(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList)
	{
		buy::g_State.currUI = pUIBase;
		buy::g_State.purchaseRoute = routeType;
		buy::g_State.purchaseGoodsList = goodsList;
		buy::g_State.is_point_and_cash_try_purchase = false;

		if (CheckMoney_Imp() == false)
		{
			/*�ܾ��� �����Ͽ� �������� ������ �� �����ϴ�.*/
			GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_BASE_NO_POINT_TO_GET_ITEM"));
			return;
		}

		// Ŭ�� ���� �ο��� ���� �� ���� �Ұ����ϵ���
		if (CheckClanMaxPerson_Imp())
		{
			/*�������� ����� �� �����ϴ�.\n \n Ŭ�� �ִ� �ο�����\n {col:255,153,51,255}%d{/col}������ ���ѵǾ� �ֽ��ϴ�.*/
			i3::rc_wstring wstrErrorMsg = i3::format_rcstring(GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrErrorMsg, &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			return;
		}

		if (CheckPeriod_Imp())	//�Ⱓ ����
		{
			/*�������� �������� ���� ��\n�������� �Ⱓ �� ������ �����մϴ�.\n \n���� �Ͻðڽ��ϱ�?*//*����Ȯ��*/
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY,
				GAME_RCSTRING("STR_POPUP_Q_MESSAGE_BUY_SET_GOODS"),
				&GAME_RCSTRING("STR_POPUP_CAPTION_CONFIRM_BUY"),
				MAKE_CALLBACK(MsgBoxCB::BuyMoreCB), PurchaseMgr::i() );
			return;
		}

		if (CheckDuplicablePurchase_Imp())		//�ߺ� ����
		{
			/*�������� �������� ���� ��\n�������� �Ⱓ �� ������ �����մϴ�.\n \n���� �Ͻðڽ��ϱ�?*//*����Ȯ��*/
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY,
				GAME_RCSTRING("STR2_SHOP_DUPLICATED_BUYING"),
				&GAME_RCSTRING("STR_POPUP_CAPTION_CONFIRM_BUY"),
				MAKE_CALLBACK(MsgBoxCB::BuyMoreCB), PurchaseMgr::i() );
			return;
		}

		if (CheckAnnounceUseOnlyClanMaster_Imp())	//Ŭ�� �����͸� ����� �� �ִ� ������ �˸�
		{
			/*Ŭ�� �����͸� ����� ������ �������Դϴ�\n ���� �Ͻðڽ��ϱ�?*//*����Ȯ��*/
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY,
				GAME_RCSTRING("STR_SHOP_ITEM_BUYING_CLANMASTER"),
				&GAME_RCSTRING("STR_POPUP_CAPTION_CONFIRM_BUY"),
				MAKE_CALLBACK(MsgBoxCB::BuyMoreCB), PurchaseMgr::i());
			return;
		}

		buy::detail::StartEvent();
		buy::g_State.purchasedInvenItemList.clear();
		buy::g_State.addInvenBuffer = buy::detail::AddInvenBuffer;

		// Set ���������� üũ�մϴ�.
		const buy::DisplayedGoodsPtr& displayedGoods = buy::g_State.purchaseGoodsList[0];

		const SHOP_GOODS* pGoods = displayedGoods->GetCurrentGoods();

		if (GET_GOODS_TYPE(pGoods->_GoodsID) == ITEM_TYPE_SET)
		{
			// Set Item�� ��쿡�� Buy Goods�θ� �����ϴ�.
			UpdatePointAndCashTryPurchase();

			GameEventSender::i()->SetEvent(EVENT_BUY_GOODS);
		}
		else
		{
			T_ItemID item_id = pGoods->_ItemID;

			if (item_def::is_chara_category(item_id) == true)
			{
				GameEventSender::i()->SetEvent(EVENT_BUY_CHAR_GOODS);
			}
			else
			{
				// ���� ��Ŷ�� ������ ���� ���� ���Ž� ����Ʈ�� ĳ�ø� ���ÿ� ����ϴ��� �˾ƾ� �Ѵ�..
				UpdatePointAndCashTryPurchase();

				GameEventSender::i()->SetEvent(EVENT_BUY_GOODS);
			}
		}
	}
	


}


void	PurchaseMgr::Buy(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList)
{
	if (buy::g_State.working == true) return;

	if (routeType == buy::PurchaseRoute_Extend)
		BuyItemPeriodExtension_Imp(pUIBase, routeType, goodsList);
	else
		Buy_Imp(pUIBase, routeType, goodsList);
}


void PurchaseMgr::Gift(UIBase* pUIBase, buy::PurchaseRouteType routeType, const buy::DisplayedGoodsList& goodsList,
	const i3::rc_wstring& wstr_nick, const i3::rc_wstring& wstr_msg)
{
	if(buy::g_State.working == true ) return;

	buy::g_State.currUI = pUIBase;
	buy::g_State.purchaseRoute = routeType;
	buy::g_State.purchaseGoodsList = goodsList;
	buy::g_State.giftInfo.wstr_receiver_nick = wstr_nick;
	buy::g_State.giftInfo.wstr_gift_message = wstr_msg;
	buy::g_State.is_point_and_cash_try_purchase = false;

	if(CheckMoney_Imp() == false )
	{
		/*�ܾ��� �����Ͽ� ��ǰ�� ������ �� �����ϴ�.*/
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_GIFT") );
		return;
	}

	buy::detail::StartEvent();
	GameEventSender::i()->SetEvent(EVENT_SEND_GIFT);
}

namespace MsgBoxCB
{
	static void AuthCommonCB_Imp(void* arg, INT32 nParam)
	{
		ItemConsumer::instance()->m_ItemID = 0; //ItemConsumer�� process_game_event �Լ��� ó���ϱ� ������ Item(category)�� �ƴҰ�츦 �Ǵ��ϱ� ����

		if (nParam == MBR_OK)
		{
			buy::detail::StartEvent();
			GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, &buy::g_State.selectedItemInfo.ware_idx, &buy::g_State.selectedItemInfo.item_id);
		}
	}

	static void AuthInvenCB(void* arg, INT32 nParam)
	{
		AuthCommonCB_Imp(arg, nParam);
	}

}

//�κ� ����
void PurchaseMgr::Auth_Inven(UIBase* pUIBase, T_ItemID item_id, T_ItemDBIdx ware_idx) //�κ���
{
	if( buy::g_State.working == true ) return;
		
	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = 0;					// ���� �����ϸ� �׻� �ΰ��̱���...
	buy::g_State.selectedItemInfo.item_id = item_id;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;
		
	UISignal::CurrentFinishAuthItem = UISignal::FinishAuthItem; //	SelectFinishAuthItem_Normal();

	buy::g_State.authResultInfo.result_list.clear();
	buy::g_State.authResultInfo.request_count = 1;		//1�� �����̴�.
	
	item_def::Category ctg = item_def::get_category( item_id );
	if( item_def::get_type(item_id) == ITEM_TYPE_DINO )
		ctg = item_def::CTG_WEAPON;
	
	switch( ctg )
	{
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:
		{
			// vv3 - check
			bool ignore_authenticate = true;
			for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
			{
				const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo(static_cast<TEAM_TYPE>(idx)); 
				int chara_itemid = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
				ignore_authenticate &= (item_def::is_shared_parts_item(item_id, chara_itemid) == false);
			}

			if( ignore_authenticate )
				return;
		}		
	case item_def::CTG_WEAPON:
	case item_def::CTG_CHARA:
		{
			if( CharaInfoMgr::instance()->is_chara_equipped(item_id, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()) == false) //������� �ƴϸ�
			{
				/*����� �������� ���� �������Դϴ�.\n��� �ú��� ���� �Ⱓ��ŭ ����� �����մϴ�.\n����Ͻðڽ��ϱ�?*/
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_INVEN_STARTING_AUTH"), nullptr, MAKE_CALLBACK(MsgBoxCB::AuthInvenCB), this);
				return;
			}
		}
		break;
	case item_def::CTG_ITEM:
		{
			bool bMaster = g_pFramework->GetClanContext()->IsMaster() ? true:false;

			// ��뿩��
			UINT8 itemAttr = 0; UINT32 itemArg = 0;
			if( CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM,  item_id, &itemAttr, &itemArg ) )
			{
				if( ITEM_ATTR_TYPE_BUY == itemAttr ||
					ITEM_ATTR_TYPE_AUTH < itemAttr)	//�̺�Ʈ �������� ��쿡�� ItemID�� Shop���� ���� �� ���� ������ �ٷ� ����ư�� Ȱ��ȭ ó���մϴ�.
				{
					ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(item_id);
					if( pInfo == nullptr) return;	//	Goods.pef�� ��ϵ��� ���� ������

					if ( pInfo->GetPurchaseAuthority() != PURCHASE_AUTH_ANYONE )
					{
						if ( pInfo->GetPurchaseAuthority() == PURCHASE_AUTH_CLAN_MASTER && !bMaster )
						{
							// ����� �� ���� �޽���
							GameUI::MsgBox( MSG_TYPE_GAME_OK, pInfo->GetCertificationAuthorityDesc(), &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));	//������ ��� �Ұ�
							return;
						}
					}

					if ( pInfo->GetUsePosition() != USE_POS_ANYWHERE )
					{
							// ����� �� �ִ� ��ġ �޽���
						
						GameUI::MsgBox( MSG_TYPE_GAME_OK, pInfo->GetUsePositionDesc(), &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));	//������ ��� �Ұ�
						return;
					}

					//	������ ���� ���� ����zó��
					if( GET_ITEM_TYPE( item_id ) == ITEM_TYPE_WRAP_COUNT)		//	�Ҹ�
					{
						CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();
						switch( item_id )
						{
						case CASHITEM_ITEMID_CLAN_PERSON_PLUS_10:
						case CASHITEM_ITEMID_CLAN_PERSON_PLUS_50:
							{
								//	Ŭ���� ���� �������� �ִ� ȸ������ ������ ��� �Ұ�
								if( pClanInfo->m_maxPerson >= MAX_MEMBER_COUNT)
								{
									/*�������� ����� �� �����ϴ�.\n \n Ŭ�� �ִ� �ο�����\n {col:255,153,51,255}%d{/col}������ ���ѵǾ� �ֽ��ϴ�.*/
									GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_INVEN_LIMITED_CLAN_MEMBER"), &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));/*������ ��� �Ұ�*/
									return;
								}
							}
							break;
						case CASHITEM_ITEMID_CHANGE_NICK:		//	�г��� ����
							{
								if(CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_DISGUISE_NICK) == true)
								{
									/*�ӽ� �г��� ���� �߿��� �г��� ������� ����� �� �����ϴ�.\n�ӽ� �г��� ������ ���� �� �г����� �����Ͻñ� �ٶ��ϴ�.*/
									GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_Q_MESSAGE_MODIFY_NICK_WITHOUT_DISUISE_NICK"), &GAME_RCSTRING("STR_INVEN_DO_NOT_USE"));
									return;
								}
							}
							break;
						}
					}
				}
			}
		}
		break;
	}
}

namespace MsgBoxCB
{
	
	static void AuthShopCB_ShopItemCategory()
	{
		CShop* shop = CShop::i();
		if (shop->IsPackageGoods(buy::g_State.selectedItemInfo.goods->_GoodsID))
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
				&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.*/

			return;
		}

		ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(buy::g_State.selectedItemInfo.item_id);
		if (pInfo != NULL && pInfo->GetUsePosition() == USE_POS_ANYWHERE)
		{
			//�������� ������� ����
			ITEM_QUERY_STYLE style = IQS_USE;

			UIBase* ui_base = buy::g_State.currUI;

			UIFramework * pParent = i3::same_cast<UIFramework*>(ui_base->getParent());
			I3ASSERT(pParent != NULL);

			if (buy::g_State.selectedItemInfo.item_id == CASHITEM_ITEMID_MAIN_SKILL_ALLOPEN)
				pParent->GetUIMainframe()->OpenPopup(UPW_SKILLOPEN_ITEM, &buy::g_State.selectedItemInfo.item_id, &style);
			else
				pParent->GetUIMainframe()->OpenPopup(UPW_QUERY_ITEM, &buy::g_State.selectedItemInfo.item_id, &style);
		}
		else
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
				&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));/*�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.*/
		}

	}

	static void AuthShopCB(void* arg, INT32 nParam)
	{
		ItemConsumer::instance()->m_ItemID = 0; //ItemConsumer�� process_game_event �Լ��� ó���ϱ� ������ Item(category)�� �ƴҰ�츦 �Ǵ��ϱ� ����

		if (nParam == MBR_OK)
		{
			switch (item_def::get_category_goodsID(buy::g_State.selectedItemInfo.goods->_GoodsID))
			{
			case item_def::CTG_CHARA:
			case item_def::CTG_WEAPON:
			case item_def::CTG_PARTS:
			case item_def::CTG_SKIN:
				MsgBoxCB::AuthCommonCB_Imp(0, MBR_OK); //auth_inven ����ص� ������ ( �̷��� ���ֱ̹����ؼ� ó������ ����...�����Ѱ� �ƴ϶�..�������� ��������..)
				break;

			case item_def::CTG_ITEM:
				MsgBoxCB::AuthShopCB_ShopItemCategory();
				break;
			}
		}
		else
		{
			switch (item_def::get_category_goodsID(buy::g_State.selectedItemInfo.goods->_GoodsID))
			{
			case item_def::CTG_WEAPON:
			case item_def::CTG_PARTS:
				/*�����Ͻ� �������� �κ��丮����\nȮ���Ͻ� �� ������ ������ �ѹ���\n������� ���� �̻�� �����\n���� ���߿� ��ü�� �� �����ϴ�.*/
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_ITEM_BUY_COMPLETE_1"),
					&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
				break;

			case item_def::CTG_CHARA:
			case item_def::CTG_ITEM:
			case item_def::CTG_SKIN:
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
					&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
				break;
			}
		}
	}


	static void EquipShopCB(void* arg, INT32 nParam)
	{
		//	�������� �κ��� ���
		if (nParam != MBR_OK)
		{
			/*�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
		}

		buy::detail::EndEvent();

		if (nParam == MBR_OK)
		{
			UISignal::CurrentFinishAuthItem(true, buy::g_State.selectedItemInfo.goods);
		}
	}
	
}

T_ItemDBIdx PurchaseMgr::GetTargetItemDBIndex() const
{
	return buy::g_State.selectedItemInfo.ware_idx;
}

const i3::vector<INVEN_BUFFER*>&	PurchaseMgr::GetPurchasedInvenBufferList() const
{
	return buy::g_State.purchasedInvenItemList;
}

const buy::DisplayedGoodsList&		PurchaseMgr::GetPurchaseGoodsList() const
{
	return buy::g_State.purchaseGoodsList;
}

const buy::GiftInfo&		PurchaseMgr::GetGiftInfo() const
{
	return buy::g_State.giftInfo;
}

//���� ����
void PurchaseMgr::Auth_Shop( UIBase* pUIBase, const SHOP_GOODS* goods, bool isExtensionPopup)
{
	if( buy::g_State.working == true ) return;

	INT32	itemid	= goods->_ItemID;

	T_ItemDBIdx ware_idx = INVALID_WAREDB_INDEX;
	if (buy::g_State.purchasedInvenItemList.empty() == false)
		ware_idx = buy::g_State.purchasedInvenItemList.back()->_TItemWareDBIdx;
	
	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = goods;
	buy::g_State.selectedItemInfo.item_id = itemid;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;
	
	UISignal::CurrentFinishAuthItem = (isExtensionPopup) ? UISignal::FinishExtensionBuyAuthItem : UISignal::FinishAuthItem;

	buy::g_State.authResultInfo.result_list.clear();
	buy::g_State.authResultInfo.request_count = 1;		//1�� �����̴�.


	item_def::Category ctg = item_def::get_category_goodsID( goods->_GoodsID );
	if( item_def::get_type(itemid) == ITEM_TYPE_DINO )	//���� ���� ����� ���� ó�� �ϸ� �� ��
		ctg = item_def::CTG_WEAPON;

	

	switch( ctg )
	{
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
		{
			// vv3 - check
			bool ignore_authenticate = true;
			for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
			{
				const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo(static_cast<TEAM_TYPE>(idx)); 
				int chara_itemid = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);				
				ignore_authenticate &= (item_def::is_shared_parts_item(itemid, chara_itemid) == false);
			}

			if( ignore_authenticate )
				break;
		}						// ���⿡ break���� ������ �ڵ�� ������̴�...�ּ����� �����ϴ� �͸� ����� �ȵǰ�...�̷� �ڵ带 ¥�� �ȵɵ��ϴ�..
	case item_def::CTG_CHARA:
	case item_def::CTG_WEAPON: 
		{
			bool bWearable = true;

			bWearable = DesigUtil::CheckWearable_MyItemByDesig(itemid);

			if(bWearable && CharaInfoMgr::instance()->is_chara_equipped(itemid, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()) == false) //������� �ƴϸ�
			{
				const i3::rc_wstring& wstrMessage = GAME_RCSTRING("STR_POPUP_Q_MESSAGE_EQUIP");	/*�ٷ� ��� �Ͻðڽ��ϱ�?*/
				SHOP_ITEM_DATA* pItem = CShop::i()->GetItemData( itemid );
				switch( pItem->_ui8AuthType )
				{
				case ITEM_TYPE_COUNT: case ITEM_TYPE_ETERNITY:	// ������, ������
					GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(MsgBoxCB::EquipShopCB), this);
					return;
					break;
				case ITEM_TYPE_TIME:	// �Ⱓ��
					{
						UINT8 itemAttr = 0;
						UINT32 itemArg = 0;
						inven::GetItemTypeArg(itemid, ware_idx, &itemAttr, &itemArg);
						if(ITEM_ATTR_TYPE_BUY == itemAttr)
						{
							if(	buy::g_State.selectedItemInfo.ware_idx != INVALID_WAREDB_INDEX )
							{
								GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(MsgBoxCB::AuthShopCB), this);
								return;
							}
						}
						else if( ctg == item_def::CTG_WEAPON )
						{
							GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(MsgBoxCB::EquipShopCB), this);
							return;
						}
						else if( ctg == item_def::CTG_CHARA )
						{
							const i3::rc_wstring& wstrMessage2 = GAME_RCSTRING("STR2_Q_ENABLE_MAIN_CHARA");	/*���� ĳ���ͷ� �����Ͻðڽ��ϱ�?*/
							GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage2, nullptr, MAKE_CALLBACK(MsgBoxCB::EquipShopCB), this);
							return;
						}
					}
					break;
				}
			}
		} 
		break; 
	case item_def::CTG_ITEM:
		{
			ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(itemid);

			if( pInfo != nullptr && pInfo->GetUsePosition() != USE_POS_ANYWHERE ) break; //Ư�� ��ġ���� ��� ������ �������̸� �������� �ʴ´�.
			if( item_def::is_direct_use_item(itemid) == false ) break;

			INVEN::DB_TYPE InvenItemType = CInvenList::i()->FindInvenDBTypeByDBIdx(ware_idx);
			if( InvenItemType == INVEN::DB_CASHITEM)
			{
				MsgBoxCB::AuthShopCB( this, MBR_OK);	//	�ٷ� ��뿩�θ� �����.
				return;
			}
		}
		break;
	}

	//�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"), 
		&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
}

//�ΰ��� ������ - ������� ���������, ����ڿ��� ��� ���θ� ���� �ʰ�, ������ ����Ѵ�.
void PurchaseMgr::Auth_InGame( UIBase* pUIBase, const SHOP_GOODS* goods)
{
	if( buy::g_State.working == true ) return;

	INT32	itemid	= goods->_ItemID;
	T_ItemDBIdx ware_idx = INVALID_WAREDB_INDEX;
	if (buy::g_State.purchasedInvenItemList.empty() == false)
		ware_idx = buy::g_State.purchasedInvenItemList.back()->_TItemWareDBIdx;
	
	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = goods;
	buy::g_State.selectedItemInfo.item_id = itemid;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;
		
	UISignal::CurrentFinishAuthItem = UISignal::FinishAuthItem;

	buy::g_State.authResultInfo.result_list.clear();
	buy::g_State.authResultInfo.request_count = 1;		//1�� �����̴�.


	item_def::Category ctg = item_def::get_category( itemid );
	if( item_def::get_type(itemid) == ITEM_TYPE_DINO )	//���� ���� ����� ���� ó�� �ϸ� �� ��
		ctg = item_def::CTG_WEAPON;

	switch( ctg )
	{
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
		{
			// vv3 - check
			bool ignore_authenticate = true;
			for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
			{
				const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetMainCharaPartsInfo(static_cast<TEAM_TYPE>(idx)); 
				int chara_itemid = pPartsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);				
				ignore_authenticate &= (item_def::is_shared_parts_item(itemid, chara_itemid) == false);
			}

			if( ignore_authenticate )
				break;
		}
	case item_def::CTG_WEAPON: 
		{
			if( CharaInfoMgr::instance()->is_chara_equipped(itemid, ware_idx, CCharaInfoContext::i()->GetMainCharaSlotIdx()) == false) //������� �ƴϸ�
			{
				SHOP_ITEM_DATA* pItem = CShop::i()->GetItemData( itemid );
				switch( pItem->_ui8AuthType )
				{
				case ITEM_TYPE_COUNT:	// ������
					{
						//������ �ʿ���� �������̴�.
						buy::detail::EndEvent();
						UISignal::CurrentFinishAuthItem(true, buy::g_State.selectedItemInfo.goods);
					}
					return;
					break;
				case ITEM_TYPE_TIME:	// �Ⱓ��
					{
						UINT8 itemAttr = 0; UINT32 itemArg = 0;
						inven::GetItemTypeArg(itemid, ware_idx, &itemAttr, &itemArg);
						if(ITEM_ATTR_TYPE_BUY == itemAttr)
						{
							if(buy::g_State.selectedItemInfo.ware_idx != INVALID_WAREDB_INDEX )
							{
								//������ �ʿ��� �������̴�.
								ItemConsumer::instance()->m_ItemID = 0;
								buy::detail::StartEvent();
								GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, 
									&buy::g_State.selectedItemInfo.ware_idx, &buy::g_State.selectedItemInfo.item_id);
								return;
							}
						}
						else
						{
							buy::detail::EndEvent();
							UISignal::CurrentFinishAuthItem(true, buy::g_State.selectedItemInfo.goods);
							return;
						}
					}
					break;
				}
			}
		} 
		break; 
	case item_def::CTG_CHARA:
		{
			//������ �ʿ���� �������̴�.
			buy::detail::EndEvent();
			UISignal::CurrentFinishAuthItem(true, buy::g_State.selectedItemInfo.goods);
			return;
		}
		break; 

	case item_def::CTG_ITEM:
		{
			ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(itemid);

			if( pInfo != nullptr && pInfo->GetUsePosition() != USE_POS_ANYWHERE ) break; //Ư�� ��ġ���� ��� ������ �������̸� �������� �ʴ´�.
			if( item_def::is_direct_use_item(itemid) == false ) break;

			INVEN::DB_TYPE InvenItemType = CInvenList::i()->FindInvenDBTypeByDBIdx(ware_idx);
			if( InvenItemType == INVEN::DB_CASHITEM)
			{
				//������ �ʿ��� �������̴�.
				buy::detail::StartEvent();
				GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, &buy::g_State.selectedItemInfo.ware_idx, &buy::g_State.selectedItemInfo.item_id);
				return;
			}
		}
		break;
	}

	g_pFramework->PutSystemChatting( GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS") ); /*�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.*/
}

//���� - preview ��

namespace MsgBoxCB
{
	static void AuthShopPreviewCB(void* arg, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			buy::detail::StartEvent();
			SendAuthPacket_Imp(0);				// ����..
		}
		else
		{
			//�����Ͻ� ��ǰ�� �κ��丮����\n Ȯ���Ͻ� �� �ֽ��ϴ�.
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS"),
				&GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION"));
		}
	}
}

void PurchaseMgr::Auth_Preview( UIBase* pUIBase)
{
	if( buy::g_State.working == true ) return;

	const i3::vector<INVEN_BUFFER*>& catched_inven_buffer_list = buy::g_State.purchasedInvenItemList;
	
	buy::g_State.currUI = pUIBase;
	buy::g_State.authResultInfo.result_list.clear();
	buy::g_State.authResultInfo.request_count = catched_inven_buffer_list.size();		//���� �� �����̴�.
	
	UISignal::CurrentFinishAuthItem = UISignal::FinishPreviewAuthItem;

	/*�ٷ� ��� �Ͻðڽ��ϱ�?*/
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_POPUP_Q_MESSAGE_EQUIP"), 
		nullptr, MAKE_CALLBACK(&MsgBoxCB::AuthShopPreviewCB), this);
}


namespace MsgBoxCB
{

	static void RemoveCB(void* arg, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			bool chara_category = item_def::is_chara_category(buy::g_State.selectedItemInfo.item_id);

			buy::detail::StartEvent();
			if (chara_category == true)
			{
				//INT32 main_chara_slot_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();
				T_ItemID itemid_for_searching = buy::g_State.selectedItemInfo.item_id;
				INT32 idx = CCharaInfoContext::i()->FindCharaInfoIndexByItemID(itemid_for_searching);
				if (idx != -1)
				{
					const CHARA_INFO* info = CCharaInfoContext::i()->GetCharaInfo(idx);
					GameEventSender::i()->SetEvent(EVENT_DELETE_CHARA, &buy::g_State.selectedItemInfo.ware_idx, &info->m_CharaBasicInfo.m_ui8CharaSlotIdx);
				}
			}
			else
			{
				GameEventSender::i()->SetEvent(EVENT_AUTH_DELETE, &buy::g_State.selectedItemInfo.ware_idx);
			}
		}
	}

}

void PurchaseMgr::Remove( UIBase* pUIBase, T_ItemID item_id, T_ItemDBIdx ware_idx) //�κ���
{
	if( buy::g_State.working == true ) return;

	if( INVALID_WAREDB_INDEX == ware_idx )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REMOVE_REQ_FAIL"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
		return;
	}

	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = 0;
	buy::g_State.selectedItemInfo.item_id = item_id;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;

	ItemInfoConvert item_info_convert(item_id);

	//�⺻ ����/�������� �Ǵ��Ѵ�.
	switch( item_info_convert.get_category() )
	{
	case item_def::CTG_WEAPON:	
		if( item_info_convert.is_default_item() )
		{
			/*�⺻ ���� ���� �Դϴ�.\n������ �� �����ϴ�.*/	/*������ ����*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_DEFAULT_WEAPON"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
			return;
		}
		break;

	case item_def::CTG_PARTS:
		if( item_info_convert.is_default_item() )
		{
			/*�⺻ ���� ���� �Դϴ�.\n������ �� �����ϴ�.*/	/*������ ����*/
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_DEFAULT_PARTS"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
			return;
		}
		break;
	
	case item_def::CTG_CHARA:
		//ĳ���ʹ� 1�� ���Ϸ� ���� �� �� ����
		if( item_def::is_chara_category(item_id) )
		{
			if( CCharaInfoContext::i()->GetHasCharacterCount() <= 1 )
			{
				/*�� �̻� ĳ���͸� ������ �� �����ϴ�.*/	/*������ ����*/
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_CHARA"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
				return;
			}
		}
		else
		{
			//2014-10-07 ���� 10213 �ذ��� ���Ͽ� ���� �κи� is_default_item_to_DBidx �Լ��� ��ü �մϴ�. ENG_��뿵
			if( item_info_convert.is_default_item_to_DBidx(ware_idx) )
			{
				/*�⺻ ���� ���� �Դϴ�.\n������ �� �����ϴ�.*/	/*������ ����*/
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INVALID_REMOVE_DEFAULT_DINO"), &GAME_RCSTRING("STR_POPUP_CAPTION_DELETE_ITEM") );
				return;
			}
		}
		break;

	case item_def::CTG_ITEM:
		if( usf::GetItemStatus( item_id, ware_idx  ) == ITEM_STATUS_USED )

		{
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_INVEN_MESSAGE_DELETE_ITEM"), nullptr, MAKE_CALLBACK(MsgBoxCB::RemoveCB), this);
			return;
		}
		break;

	case item_def::CTG_SKIN :
		break;
	}

	MsgBoxCB::RemoveCB(0, MBR_OK); //�ٷ� �����Ѵ�.
}


namespace MsgBoxCB
{
	static void RepairCB(void* arg, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			buy::g_State.authResultInfo.result_list.clear();
			buy::g_State.authResultInfo.request_count = 1;			

			UISignal::CurrentFinishRepairItem = UISignal::FinishRepairItem;

			buy::detail::StartEvent();
			GameEventSender::i()->SetEvent(EVENT_ITEM_REPAIR, &buy::g_State.selectedItemInfo.ware_idx);
		}

	}
}

void PurchaseMgr::Repair( UIBase* pUIBase, T_ItemID item_id, T_ItemDBIdx ware_idx)
{
	if( buy::g_State.working == true ) return;

	if( INVALID_WAREDB_INDEX == buy::g_State.selectedItemInfo.ware_idx )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REPAIR_REQ_FAIL"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
		return;
	}

	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = 0;
	buy::g_State.selectedItemInfo.item_id = item_id;
	buy::g_State.selectedItemInfo.ware_idx = ware_idx;

	// ������ �� �ִ��� Ȯ���Ѵ�.
	REPAIR_TYPE	Rv = CShopContext::i()->CheckCanRepair(item_id);
	if ( Rv == REPAIR_TYPE_UNKOWN || Rv == REPAIR_TYPE_CANNOT )
	{
		/*������ �� �� �����ϴ�.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REPAIR_REQ_FAIL"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
		return;
	}

	if ( Rv == REPAIR_TYPE_CAN_SOME )
	{
		/*���� �ݾ��� �����Ͽ� ������ �� �����ϴ�.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REPAIR_3"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
		return;
	}

	UINT32 Price = 0;
	GOODS_PRICE_TYPE	PriceType	= GOODS_PRICE_UNKNOWN;
	CShopContext::i()->GetRepairPrice(item_id, Price, PriceType);

	i3::rc_wstring wstr;

	if( item_def::is_dino_category(item_id) == false )
	{
		//���� ��� %d(%s)\n�����Ͻðڽ��ϱ�?
		i3::sprintf(wstr, L"%s %d(%s)\n%s", GAME_RCSTRING("STR_POPUP_REPAIR_PRICE"), Price,
			(PriceType == GOODS_PRICE_CASH) ? GAME_RCSTRING("STR_SHOP_INFO_CASH") : GAME_RCSTRING("STR_SHOP_INFO_POINT"),
			GAME_RCSTRING("STR_POPUP_REPAIR_1") );
	}
	else 
	{
		//ü�� ȹ�� ��� %d(%s)\nȸ���Ͻðڽ��ϱ�?
		i3::sprintf(wstr, L"%s %d(%s)\n%s", 	GAME_RCSTRING("STR_POPUP_RECOVER_PRICE"), Price,
			(PriceType == GOODS_PRICE_CASH) ? GAME_RCSTRING("STR_SHOP_INFO_CASH") : GAME_RCSTRING("STR_SHOP_INFO_POINT"),
			GAME_RCSTRING("STR_POPUP_REPAIR_5") );
	}

	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstr, &GAME_RCSTRING("STR_POPUP_REPAIR_CAPTION"), MAKE_CALLBACK(MsgBoxCB::RepairCB), this);
}

// �ι�° �μ��� ���� �ʿ�� ���� �ʾƾ� �Ѵ�..
void PurchaseMgr::RepairAll( UIBase* pUIBase) //���� ĳ���� ���� ��� ���ؼ� ��� ����(ĳ���� ���)
{
	if( buy::g_State.working == true ) return;

	buy::g_State.currUI = pUIBase;
	buy::g_State.selectedItemInfo.goods = nullptr;
	buy::g_State.selectedItemInfo.item_id = 0;
	buy::g_State.selectedItemInfo.ware_idx = 0;				// �ʱ�ȭ�� ���ؼ� 1�� ������ �����ϵ��� �ϴ� ó���صӴϴ�..  (���� ����� �ƴ��ٵ�...)
		
	UISignal::CurrentFinishRepairItem = UISignal::FinishRepairAllItem;   // �̰ɷ� �����ص� �Ǳ� �Ǵµ�...

	buy::detail::StartEvent();

	// RepairAll�� ���� ��Ŷ ������ ó���� �̰��� �ִ°� �ϰ��� �����Ƿ� UIFloatCharacterView���� �������� �ű�ϴ�...
	UINT32 chara_idx = static_cast<UINT32>(CCharaInfoContext::i()->GetMainCharaSlotIdx());
	GameEventSender::i()->SetEvent(EVENT_REPAIR_EQUIPMENT, &chara_idx);
	
}

bool PurchaseMgr::IsAllRepairPossible(AllRepairInfo* outInfo)
{
	UINT32 total_point_cost = 0;
	UINT32 total_cash_cost = 0;
	INT32 repair_item_count = 0;

	GOODS_PRICE_TYPE	PriceType = GOODS_PRICE_UNKNOWN;
	UINT32 Price;

	UINT32 chara_idx = (UINT32)CCharaInfoContext::i()->GetMainCharaSlotIdx();
	for (INT32 i = 0; i < MAX_WEAPON_CHECK_SLOT_COUNT; i++)
	{
		ITEM_INFO info = CharaInfoMgr::instance()->find(chara_idx, item_def::SCTG_WEAPON, i);
		if (info.m_TItemID == 0) continue;

		REAL32	durability_ratio = 0.0f;
		if (usf::GetDurability(info.m_TItemID, info.m_TItemDBIdx, durability_ratio) == false) continue; //������ �������� �ƴϴ�.
		if (durability_ratio == 1.f) continue; //���� �� �ʿ䰡 ����.

		if (CShopContext::i()->GetRepairPrice(info.m_TItemID, Price, PriceType) == false) continue;

		if (PriceType == GOODS_PRICE_POINT)
			total_point_cost += Price;
		else if (PriceType == GOODS_PRICE_CASH)
			total_cash_cost += Price;

		++repair_item_count;
	}

	if (outInfo != NULL)
	{
		outInfo->total_point_cost = total_point_cost;
		outInfo->total_cash_cost = total_cash_cost;
		outInfo->repair_item_count = repair_item_count;
	}

	if (total_point_cost > UserInfoContext::i()->GetMyPoint()) return false; //���� ����.
	if (total_cash_cost > UserInfoContext::i()->GetMyCash()) return false; //���� ����.

	return true;
}

