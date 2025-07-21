#include "pch.h"
#include "UIShopFunction.h"

#include "UIUtil.h"
#include "UIInvenInfoMgr.h"
#include "BuyUtil.h"
#include "UIItemInfoUtil.h"
#include "InvenDef.h"
#include "PurchaseMgr.h"

#include "ClanGameContext.h"
#include "ShopContext.h"
#include <functional>


#include "MedalManager.h"
#include "UIMainFrame.h"
#include "Designation/DesigUtil.h"
#include "Designation/DesigSet.h"

//#include "GlobalVariables.h"
//#include "GameFramework.h"



namespace usf {

//- IsValidBuy
bool IsValidBuyPartsGoods(const SHOP_GOODS_PACK* pack)
{
	UINT32 chara_cnt = (UINT32)CCharaInfoContext::i()->GetHasCharacterCount();
	UINT32 part_cnt = CInvenList::i()->FindItemCount( pack->GetGoods(0)->_ItemID );
	return (chara_cnt <= part_cnt)	? false : true;
}


ValidBuyType IsValidBuyGoods_InCart(const SHOP_GOODS_PACK * pack)
{
	SHOP_GOODS* pGoods = pack->GetGoods(0);

	bool is_durable = (CShopContext::i()->IsDurabilityItem( pGoods->_ItemID ) == true) ? true : false;
	//UINT32 same_cnt = buy::op::get_same_count_in_cart(pack);

	if (is_durable)
	{
		bool isExistedPackInCart = buy::IsExistedShopGoodPackInCurrentCharaCart(pack);
		if (isExistedPackInCart)
			return vbt_Pushed_Perpetuity;	//������ ��ǰ�� �ߺ� ���� �� �� ����.
	}

	return IsValidBuyGoods(pack);
}




namespace
{
	size_t FindHaveItemCharaIdx( T_ItemID itemId)
	{
		item_def::Category ctg = item_def::get_category(itemId);
		UINT32 slot_idx = item_def::get_shop_slot_idx(itemId);

		inven_util::InvenInfoList list;
		list.list_up( ctg, slot_idx );

		return list.find_idx(itemId);
	}
}

ValidBuyType IsValidBuyGoods(const SHOP_GOODS_PACK * pack)
{
	SHOP_GOODS* pGoods = pack->GetGoods(0);
	if( pGoods == nullptr) return vbt_Invalid;

	if(usf::g_price_idx == PRICE_TYPE_GIFT_REAL) return vbt_Ok; /// ���� ���Ŵ� ���� �˻縦 ����.

	bool have_item = ( FindHaveItemCharaIdx(pGoods->_ItemID) != UINT_MAX ) ? true : false;
	bool durable = (CShopContext::i()->IsDurabilityItem( pGoods->_ItemID ) == true) ? true : false;

	if(have_item && durable)	return vbt_Perpetuity;	//������ ��ǰ�̸�, �̹� ���Ե� ����

	switch( GetShopItemCategory( pGoods->_ItemID ) )
	{
		case SHOP_ITEM_CATEGORY_WEAPON:	break;
		case SHOP_ITEM_CATEGORY_CHARACTER: 
			{
				switch( item_def::get_category(pGoods->_ItemID) )
				{
				// V3 ������ �ߺ� ���� ��Ģ �������� �ּ�ó�� �մϴ�.
				//case item_def::CTG_PARTS: 
				//case item_def::CTG_SKIN:
				//	if( IsValidBuyPartsGoods(pack) == false )
				//		return vbt_Disable_Buy_Parts_Any_more;	/*������ ���� ĳ���� �� ���� ���� ���� �� �� �����ϴ�.*/
				//	break;
				case item_def::CTG_CHARA:
					{
						// ĳ���� �ִ뺸�������� 9��
						int cnt = CCharaInfoContext::i()->GetHasCharacterCount();
						if( cnt > S2MO_MULTI_SLOT_COUNT )			// 9�� �ƴϱ���...����..�̰�...�� �׳� �Դ°ſ�?...
							return vbt_Exceed_MaxPurchaseableCharacters;
					}
					break;
				}	
			}
			break;

		case SHOP_ITEM_CATEGORY_ITEM:

			//	������ ���� ���Ž� ����ó��
			switch (pGoods->_ItemID)	//�� �κ��� �� �𸣰ڴ�.
			{
				//������ �����Ҷ� üũ�ϱ⶧���� �ּ� ó��
				//case CASHITEM_ITEMID_CLAN_PERSON_PLUS_50:
				//{
				//	//	Ŭ���� ���� �������� �ִ� ȸ������ ������ ���� �Ұ�
				//	CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();
				//	if (pClanInfo->m_maxPerson >= MAX_MEMBER_COUNT)
				//		return vbt_Limited_Clan_member;
				//}
				//break;

			//Ŭ������ �ʱ�ȭ �������� Ŭ�������Ͱ� �ƴϴ��� ���� ����
			case CASHITEM_ITEMID_CLR_CLAN_HISTORY:
				return vbt_Ok;

			default:
				break;
			}

			if( !g_pFramework->GetClanContext()->IsMaster())	//	�����͸� ���� ���� ���� �Ǵ� (���� ����)
			{
				//	ĳ�� �������� Ŭ�� �����͸� ����, ����� �����ϴ�.
				//	Ŭ�� �����Ϳ� ���������� ���� �Ǵ�.
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo( pGoods->_ItemID );
				if ( pInfo != nullptr && pInfo->GetPurchaseAuthority() == PURCHASE_AUTH_CLAN_MASTER )
					return vbt_Only_Clan_master;
			}
			break;
	}

	return vbt_Ok;
}

void Message_BuyResert(ValidBuyType fb, bool chat_msg)
{
	i3::rc_wstring wstrMsg;
	i3::rc_wstring wstrCaption;

	switch( fb )
	{
	case vbt_Ok: break;

	case vbt_Perpetuity:
		/*������*//*������ ������*/
		i3::sprintf(wstrMsg, L"%s : %s", GAME_RCSTRING("STBL_IDX_ITEM_HAVE"), GAME_RCSTRING("STR_SHOP_DURABILITY_ITEM"));
		wstrCaption = GAME_RCSTRING("STR_SHOP_CANNOT_ITEM_BUYING");
		break;
	case vbt_Only_Clan_master:
		/*������ ���� �Ұ�*/
		wstrMsg = GAME_RCSTRING("STR_SHOP_CANNOT_ITEM_BUYING");
		wstrCaption = GAME_RCSTRING("STR_SHOP_CANNOT_ITEM_BUYING");
		break;
	case vbt_Limited_Clan_member:
		/*�������� ������ �� �����ϴ�.\n \n Ŭ�� �ִ� �ο�����\n {col:255,153,51,255}%d{/col}������ ���ѵǾ� �ֽ��ϴ�.*/
		wstrMsg = GAME_RCSTRING("STR_SHOP_LIMITED_CLAN_MEMBER");
		wstrCaption = GAME_RCSTRING("STR_SHOP_CANNOT_ITEM_BUYING");
		break;
	case vbt_Pushed_Perpetuity:
		/*�������� ��ǰ�� �̹� �߰��Ǿ� �ֽ��ϴ�.*/
		wstrMsg = GAME_RCSTRING("STR2_INVALID_MORE_PERPETUITY");
		wstrCaption = GAME_RCSTRING("STR_SHOP_CANNOT_ITEM_BUYING");
		break;
	case vbt_Disable_Buy_Parts_Any_more:
		/*������ ���� ĳ���� �� ���� ���� ���� �� �� �����ϴ�.*/
		wstrMsg = GAME_RCSTRING("STR2_SHOP_DISABLE_BUY_PARTS_ANY_MORE");
		wstrCaption = GAME_RCSTRING("STR_SHOP_CANNOT_ITEM_BUYING");
		break;
	case vbt_Exceed_MaxPurchaseableCharacters:
		/*ĳ���� �ִ� ���� ����(9��) �̻����� ���Ű� �Ұ����մϴ�. */
		wstrMsg = GAME_RCSTRING("STR_BUY_CHARA_9OVER_MESSAGE");
		wstrCaption = GAME_RCSTRING("STR_SHOP_CANNOT_ITEM_BUYING");
		break;
	}
	
	if(chat_msg == false)
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg, &wstrCaption);	
	else
		g_pFramework->PutSystemChatting(wstrMsg);
}

}	//end of namespace usf




namespace usf {

namespace inner
{
	bool IsUsingCashItem(UINT32* group, size_t group_size, UINT32 groupIdx)
	{
		struct Fn : std::binary_function<UINT32, UINT32, bool>
		{
			bool operator()(T_ItemID item_id, UINT32 groupIdx) const
			{
				if( GET_ITEM_NUMBER(item_id) == groupIdx) return false; //���� �������̸� �Ѿ��.
				return CInvenList::i()->IsUsingCashItem( item_id );
			}
		};

		UINT32* it = std::find_if(group, group + group_size, std::bind2nd( Fn(), groupIdx) );
		return std::distance(group, it) != (signed)group_size;
	}

	UINT32 FindUsingCashItem(UINT32* group, size_t group_size, UINT32 class_type)
	{
		struct Fn : std::binary_function<UINT32, UINT32, bool>
		{
			bool operator()(T_ItemID item_id, UINT32 cType) const
			{
				if( GET_ITEM_SUBTYPE(item_id) == cType ) return false; //���� �������̸� �Ѿ��.
				return CInvenList::i()->IsUsingCashItem( item_id );
			}
		};

		UINT32* it = std::find_if(group, group + group_size, std::bind2nd( Fn(), class_type) );
		if( std::distance(group, it) != (signed)group_size )
		{
			return *it;
		}
		return 0;
	}
}

UINT32 GetDisableDuplicateUseItemPair(T_ItemID item_id) //�ߺ� ����� �� ���� ������ itemID �ݴ��� �������� �˷��ش�.
{
	// Domination�̶� �ǹ̴� ������ �����Ѵ�.
	switch( GET_ITEM_TYPE(item_id))
	{
	case ITEM_TYPE_MAINTENANCE:
		{
			switch(item_id)
			{
			case CASHITEM_ITEMID_MEGA_HP5:
				if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_MEGA_HP10) == true ) return CASHITEM_ITEMID_MEGA_HP10;
				break;

			case CASHITEM_ITEMID_MEGA_HP10:
				if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_MEGA_HP5) == true ) return CASHITEM_ITEMID_MEGA_HP5;
				break;

			case CASHITEM_ITEMID_FULLMETALJACKETAMMO:
			case CASHITEM_ITEMID_HOLLOW_POINT_AMMO:
			case CASHITEM_ITEMID_JACKETED_HELLOW_POINT_AMMO:
			case CASHITEM_ITEMID_HOLLOW_POINT_AMMO_PLUS:
				{
					UINT32 group[] = { 
						CASHITEM_ITEMID_FULLMETALJACKETAMMO, 
						CASHITEM_ITEMID_HOLLOW_POINT_AMMO, 
						CASHITEM_ITEMID_JACKETED_HELLOW_POINT_AMMO,
						CASHITEM_ITEMID_HOLLOW_POINT_AMMO_PLUS, };
					INT32 size = GET_ARRAY_COUNT(group);
					return inner::FindUsingCashItem(group, size, 0);
				}
				break;

			case CASHITEM_ITEMID_BULLET_PROOF_VEST:
			case CASHITEM_ITEMID_BULLET_PROOF_VEST_PLUS:
			case CASHITEM_ITEMID_BULLET_PROOF_VEST_GM:
			case CASHITEM_ITEMID_BULLET_PROOF_VEST_METAL:
				{
					UINT32 group[] = { 
						CASHITEM_ITEMID_BULLET_PROOF_VEST,
						CASHITEM_ITEMID_BULLET_PROOF_VEST_PLUS,
						CASHITEM_ITEMID_BULLET_PROOF_VEST_GM,
						CASHITEM_ITEMID_BULLET_PROOF_VEST_METAL, };
					INT32 size = GET_ARRAY_COUNT(group);
					return inner::FindUsingCashItem(group, size, 0);
				}
				break;
			}
		}
		break;
	case ITEM_TYPE_WRAP_PERIOD: break;
	case ITEM_TYPE_WRAP_COUNT: break;
	}
	return 0;
}

ValidUseType IsUseableItem(T_ItemID item_id)
{
	// 17�� ���۵Ǵ� Wrap Period �� Maintenance�� �ٲ��ݴϴ�.
	// �׷��� �ٸ� ITEMID�� �ȸ��� üũ�� ����.
	T_ItemID ItemID = GetCashItemByGroup(item_id);
	switch( GET_ITEM_TYPE(ItemID))
	{
	case ITEM_TYPE_MAINTENANCE:
		{
			switch(ItemID)
			{
			case CASHITEM_ITEMID_MEGA_HP5:
				if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_MEGA_HP10) == true ) return vut_Duplicate;
				break;

			case CASHITEM_ITEMID_MEGA_HP10:
				if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_MEGA_HP5) == true ) return vut_Duplicate;
				break;

			case CASHITEM_ITEMID_FULLMETALJACKETAMMO:
			case CASHITEM_ITEMID_HOLLOW_POINT_AMMO:
			case CASHITEM_ITEMID_JACKETED_HELLOW_POINT_AMMO:
			case CASHITEM_ITEMID_HOLLOW_POINT_AMMO_PLUS:
				{
					UINT32 group[] = { 
						CASHITEM_ITEMID_FULLMETALJACKETAMMO, 
						CASHITEM_ITEMID_HOLLOW_POINT_AMMO, 
						CASHITEM_ITEMID_JACKETED_HELLOW_POINT_AMMO, 
						CASHITEM_ITEMID_HOLLOW_POINT_AMMO_PLUS, };
					INT32 size = GET_ARRAY_COUNT(group);
					for (INT32 i = 0; i < size; i++)
					{
						if (CInvenList::i()->IsUsingCashItem(group[i]) == true) 
							return vut_Duplicate;
					}
				}
				break;

			case CASHITEM_ITEMID_BULLET_PROOF_VEST:
			case CASHITEM_ITEMID_BULLET_PROOF_VEST_PLUS:
			case CASHITEM_ITEMID_BULLET_PROOF_VEST_GM:
			case CASHITEM_ITEMID_BULLET_PROOF_VEST_METAL:
				{
					UINT32 group[] = { 
						CASHITEM_ITEMID_BULLET_PROOF_VEST,
						CASHITEM_ITEMID_BULLET_PROOF_VEST_PLUS,
						CASHITEM_ITEMID_BULLET_PROOF_VEST_GM,
						CASHITEM_ITEMID_BULLET_PROOF_VEST_METAL, };
					INT32 size = GET_ARRAY_COUNT(group);
					for (INT32 i = 0; i < size; i++)
					{
						if (CInvenList::i()->IsUsingCashItem(group[i]) == true)
							return vut_Duplicate;
					}
				}
				break;
			}
		}
		break; // end of ITEM_TYPE_MAINTENANCE

	case ITEM_TYPE_WRAP_PERIOD: break;
	case ITEM_TYPE_WRAP_COUNT: 
		switch (item_id)
		{
		case CASHITEM_ITEMID_CHANGE_NICK:
			if (CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_DISGUISE_NICK) == true) return vut_Duplicate;
			break;
		}
		break;
	}

	return vut_Ok;
}


void Message_UseResert(ValidUseType fb, bool chat_msg)
{
	i3::rc_wstring wstrMsg;
	i3::rc_wstring wstrCaption;

	switch( fb )
	{
	case vut_Ok: break;
	case vut_Duplicate:
		/*������*//*������ ������*/
		wstrMsg = GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_BY_DUPLICATE");
		wstrCaption = GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM");
		break;
	}

	if(chat_msg == false)
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg, &wstrCaption);	
	else
		g_pFramework->PutSystemChatting(wstrMsg);
}

} //end of namespace usf



namespace usf {

i3::wstring payment::get_price_combo_string( const SHOP_GOODS_PACK* packs )
{
	i3::wstring point, cash;

	i3::wstring price_str;
	i3::wstring perido_str;
	i3::stack_wstring tmp_str;

	INT32 size = packs->GetGoodsCount();
	for(INT32 i = 0; i < size; ++i)
	{
		SHOP_GOODS* pack = packs->GetGoods( i );

		// ������ ���� ����Ʈ�� EVENT�������� �����մϴ�.
		if(pack->_SaleType == GOODS_SALE_EVENT)
			continue;

		usf::GetShopPeriodCountString(pack, perido_str);

		if( pack->GetPriceType(usf::g_price_idx) == GOODS_PRICE_ALL )
		{
			usf::GetShopPriceStringComma(pack, price_str, GOODS_PRICE_POINT );
			i3::sprintf(tmp_str, L"%s - %s", perido_str, price_str);
			point += tmp_str;	point.append(1, L';' );

			usf::GetShopPriceStringComma(pack, price_str, GOODS_PRICE_CASH );
			i3::sprintf(tmp_str, L"%s - %s", perido_str, price_str);
			cash += tmp_str;	cash.append(1, L';' );
		}
		else
		{
			bool bCheck = usf::GetShopPriceStringComma(pack, price_str, pack->GetPriceType(usf::g_price_idx) );

			if (bCheck == true)
			{
				if (pack->GetPriceType(usf::g_price_idx) == GOODS_PRICE_POINT)
				{
					i3::sprintf(tmp_str, L"%s - %s", perido_str, price_str);
					point += tmp_str;	point.append(1, L';');
				}
				else
				{
					i3::sprintf(tmp_str, L"%s - %s", perido_str, price_str);
					cash += tmp_str;	cash.append(1, L';');
				}
			}

		}
	}

	//point String �ڷ� cash String �� �ִ´�.
	point.append( cash );

	if(point.size() > 0)
		point.erase( point.size() -1 );

	return i3::move(point);
}



// �� �� �Լ� �����̳�...�ǵ��ľ� �� ��ģ��...  �ǵ��� ������ ���� �� �ǵ����� �����ϰ� ¥�����ɷ� �����ϰڴ�..
/*
void payment::getgoodstype(buy::item & item, INT32 display_inx, bool displaysetting, int pridata)
{
	struct GOODSSELECTINFO
	{
		INT32 nPackIndex;
		INT32 nPriceType;
	};

	INT32 cnt = item.pack->GetGoodsCount();

	GOODSSELECTINFO* pPointIndex = new GOODSSELECTINFO[cnt];
	GOODSSELECTINFO* pCashIndex = new GOODSSELECTINFO[cnt];
	
	int PointCount = 0;
	int CashCount = 0;
	for (INT32 nIndex = 0; nIndex < cnt; nIndex++)
	{
		SHOP_GOODS * pGoods = item.pack->GetGoods(nIndex);

		if (pGoods->_SaleType == GOODS_SALE_EVENT)
			continue;

		if (pGoods->GetPriceType(usf::g_price_idx) == GOODS_PRICE_ALL)
		{
			pPointIndex[PointCount].nPriceType = GOODS_PRICE_POINT;
			pCashIndex[CashCount].nPriceType = GOODS_PRICE_CASH;

			pPointIndex[PointCount++].nPackIndex = nIndex;
			pCashIndex[CashCount++].nPackIndex = nIndex;
		}
		else if (pGoods->GetPriceType(usf::g_price_idx) == GOODS_PRICE_POINT)
		{
			pPointIndex[PointCount].nPriceType = GOODS_PRICE_POINT;
			pPointIndex[PointCount++].nPackIndex = nIndex;
		}
		else
		{
			pCashIndex[CashCount].nPriceType = GOODS_PRICE_CASH;
			pCashIndex[CashCount++].nPackIndex = nIndex;
		}

	}


	GOODSSELECTINFO* pTotalIndex;
	pTotalIndex = new GOODSSELECTINFO[PointCount + CashCount];

	for (INT32 nIndex = 0; nIndex < PointCount + CashCount; nIndex++)
	{
		if (nIndex >= PointCount)
		{
			pTotalIndex[nIndex].nPackIndex = pCashIndex[nIndex - PointCount].nPackIndex;
			pTotalIndex[nIndex].nPriceType = pCashIndex[nIndex - PointCount].nPriceType;
		}
		else
		{
			pTotalIndex[nIndex].nPackIndex = pPointIndex[nIndex].nPackIndex;
			pTotalIndex[nIndex].nPriceType = pPointIndex[nIndex].nPriceType;
		}
	}

	if (displaysetting)
	{
		display_inx = pridata + PointCount;
		item.display_inx = display_inx;
	}

	item.select_inx = pTotalIndex[display_inx].nPackIndex;
	item.price_type = pTotalIndex[display_inx].nPriceType;

	delete[]pTotalIndex;
	delete[]pPointIndex;
	delete[]pCashIndex;
}
*/
// ...�� �� ���� �Լ� ���̹�...
/*
void payment::adjust_payment( buy::item & item, INT32 display_inx )
{
	


	INT32 select_inx = 0;

	//���õ� goods ��  point/cash �� �� ��쿡 �ٸ� ó���� �ؾ� �ȴ�.
	if( buy::item_util::is_dual_price(item, usf::g_price_idx) == true )
	{
		getgoodstype(item, display_inx);

		select_inx = item.select_inx;
	}
	else if( buy::item_util::is_plural_price(item) == true )
	{
		item.price_type = item.pack->GetGoods(display_inx)->GetPriceType(usf::g_price_idx);

		select_inx = display_inx;
	}

	item.select_inx = select_inx;
	item.display_inx = display_inx;
}
void payment::adjust_payment(buy::PurchaseRouteType route, INT32 index, INT32 tab)
{
	buy::item & item = buy::op::get_item(route, index);
	payment::adjust_payment(item, tab);
}
*/

bool PopupRestriction(UINT32 ui32ItemID)
{
#if defined(LOCALE_KOREA) && !defined(BUILD_RELEASE_QA_VERSION)			// �ӽ��ּ��̴�..�� Ǯ��ߵ�..
	return true;
#else
	bool bRestriction = true;
	if (g_pEnvSet->IsV2Version() == true)
	{
		SHOP_ITEM_DATA* pItemBase = CShop::i()->GetItemData(ui32ItemID);

		UINT8 lv = pItemBase->_ui8Restriction - 1;
		UINT16 idx = pItemBase->_ui16Condition;

		//INT32 imaxsize = CMedalManager::i()->getMedalTotalCount(MEDAL_SET_TYPE_NORMAL);

		MedalViewInfo* pMedalInfo = CMedalManager::i()->GetMedalViewIdx(pItemBase->_ui16Condition);

		if (idx == 0)
			return true;

		if (pMedalInfo == NULL)
		{
			I3PRINTLOG(I3LOG_FATAL, "MedalList Purchase restrictions \n(ITEMID:%d)(pMedalInfo:NULL)( Condition:%d)", ui32ItemID, pItemBase->_ui16Condition);
			return false;
		}

		if (pItemBase->_ui8RestrictionType == TYPE_ITEM_RESTRICTION_MEDAL)
		{
			INT32 iLevel = pMedalInfo->getCurrentLevel();
			if (iLevel < lv + 1)
				bRestriction = false;
		}
		if (bRestriction == false)
		{
			g_pFramework->GetUIMainframe()->OpenPopup(UPW_MEDAL_ITEM, (void*)pMedalInfo, (void*)&lv);
		}
	}
	else
	{
		const i3::vector<UINT8> *outNeedDesigSetType;
		if (!DesigUtil::CheckPurchasable_MyItemByDesig(ui32ItemID, &outNeedDesigSetType))
		{
			i3::rc_wstring wszUserTitle;
			for (size_t i = 0; i < outNeedDesigSetType->size(); i++)
			{
				const DesigSet*	desigSet = DesigUtil::FindDesigSetByType(outNeedDesigSetType->at(i));
				if (desigSet)
					wszUserTitle += desigSet->wstrName;
			}
			i3::rc_wstring wszMessage;
			i3::sprintf(wszMessage, GAME_RCSTRING("STR_SHOP_REQUIRE_DESIGNATION_TO_BUY"), wszUserTitle);/*�ش� �������� {col:255,0,0,255}%s{/col} ȣĪ��\nȹ���Ͽ��� ������ �� �ֽ��ϴ�.\n{col:255,120,0,255}(ȣĪ ȹ�� �� ���� ����){/col}*/

			GameUI::MsgBox(MSG_TYPE_GAME_OK, wszMessage);
			bRestriction = false;
		}
	}

	return bRestriction;
#endif
}



} //end of namespace usf