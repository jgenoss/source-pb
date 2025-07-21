#include "pch.h"

#include "UI/UIShopFunction.h"
#include "UI/UIClanFunction.h"

#include "UI_V11/UITabShopItem.h"
#include "UI_V11/UIToolTipShop.h"
#include "UI_V11/UIPhaseShop_V11.h"

#include "InvenUtil.h"
#include "ShopContext.h"
#include "UserInfoContext.h"

UITabShopItem::UITabShopItem(UIPhaseSubBase* phase, UIToolTipShop* pTT) : UITabShopBase(phase, pTT), 
	m_category(UISHOP_SUBTAB_ITEM_ALL), m_currFilterType(SISFT_NEW)
{

}

UITabShopItem::~UITabShopItem()
{
}

void	UITabShopItem::ChangeItemCategory( UIShop_SubTab_Item  category)
{
	if (m_category == category)
		return;

	// ��ǰ ����� �ٽ� �����.
	m_category = category;
	m_phaseShop->ChangeSubCategory();
}

void	UITabShopItem::OnLoadAllScenes()
{
	m_category = UISHOP_SUBTAB_ITEM_ALL;
	m_currFilterType = SISFT_NEW;
}

void	UITabShopItem::_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList)
{
	//	���͸� ��ģ ��ǰ�� ����Ʈ�� �ִ´�.
	refGoodsList.clear();

	CShopContext * pShopContext = CShopContext::i();
	INT32 goodsPackCount = pShopContext->GetGoodsPackCount(SHOP_CASH_ITEM_GOODS);
	i3::rc_wstring keyword = m_phaseShop->GetKeywordString();

	for(INT32 i=0; i<goodsPackCount; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = const_cast<SHOP_GOODS_PACK*>(pShopContext->GetGoodsPack(SHOP_CASH_ITEM_GOODS, i));
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;
		
		switch (m_category)
		{
		case UISHOP_SUBTAB_ITEM_ALL:
		break;

		case UISHOP_SUBTAB_ITEM_ENHANCED:
			if ( usf::GetItemItemCategory(pGoods) != UISHOP_ITEM_ENHANCED) 
				continue;			
		break;
		case UISHOP_SUBTAB_ITEM_FUNCTIONAL:
			if ( usf::GetItemItemCategory(pGoods) != UISHOP_ITEM_FUNCTIONAL) 
				continue;		
		break;
		case UISHOP_SUBTAB_ITEM_COMPENSATION:
			if ( usf::GetItemItemCategory(pGoods) != UISHOP_ITEM_COMPENSATION) 
				continue;	
		break;
		case UISHOP_SUBTAB_ITEM_BOX:
			if ( usf::GetItemItemCategory(pGoods) != UISHOP_ITEM_BOX) 
				continue;				//DB�� �غ���� �ʾ� ������� ��..
		break;
		default:
		break;
		}
		
		if( pGoods->_SaleType == GOODS_SALE_EVENT )
			continue;

		if ( keyword.size() != 0 )
		{
			i3::rc_wstring strItemName;
			usf::GetShopItemName(strItemName, pGoods);
			i3::to_lower_copy(strItemName, strItemName.c_str()); i3::to_lower_copy(keyword, keyword.c_str());
			if( i3::contain_string(strItemName,keyword) < 0 )		
				continue;
		}

		refGoodsList.push_back(pGoodsPack);
	}
	
	goodsPackCount = pShopContext->GetGoodsPackCount(SHOP_PACKAGE_GOODS);					// ��Ű�� �������� ������ �׸����� ���Ƴִ´�.
	
	for(INT32 i=0; i<goodsPackCount; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = const_cast<SHOP_GOODS_PACK*>(pShopContext->GetGoodsPack(SHOP_PACKAGE_GOODS, i));
		if( pGoodsPack == NULL)
			continue;
		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		switch (m_category)
		{
		case UISHOP_SUBTAB_ITEM_ALL:
			break;

		case UISHOP_SUBTAB_ITEM_ENHANCED:
			if ( usf::GetItemItemCategory(pGoods) != UISHOP_ITEM_ENHANCED) 
				continue;			
			break;
		case UISHOP_SUBTAB_ITEM_FUNCTIONAL:
			if ( usf::GetItemItemCategory(pGoods) != UISHOP_ITEM_FUNCTIONAL) 
				continue;		
			break;
		case UISHOP_SUBTAB_ITEM_COMPENSATION:
			if ( usf::GetItemItemCategory(pGoods) != UISHOP_ITEM_COMPENSATION) 
				continue;	
			break;
		case UISHOP_SUBTAB_ITEM_BOX:
			if ( usf::GetItemItemCategory(pGoods) != UISHOP_ITEM_BOX) 
				continue;				//DB�� �غ���� �ʾ� ������� ��..
			break;
		default:
			break;
		}

		if ( keyword.size() != 0 )
		{
			i3::rc_wstring strItemName;
			usf::GetShopItemName(strItemName, pGoods);
			if( i3::contain_string(strItemName,keyword) <= 0 )		
				continue;
		}

		refGoodsList.push_back(pGoodsPack);
	}

	switch (m_currFilterType)
	{
		case SISFT_NAME:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByName); break;
		case SISFT_POINT:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByPoint); break;
		case SISFT_CASH:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByCash); break;
		default: break;
	}
	
}

void	UITabShopItem::Update_ToolTip_ByShopCategory(SHOP_GOODS_PACK* pGoodsPack, 
													i3::rc_wstring& outItemName, i3::rc_wstring& outDesc, i3::rc_wstring& outCategory, i3::rc_wstring& outStatus, i3::rc_wstring& outRequired)
{
	SHOP_GOODS* pGoods = pGoodsPack->GetGoods(0);
	if (usf::IsSetItem(pGoods))
	{
		outStatus.clear();
		outRequired.clear();

		INT32 GoodsID = pGoods->_GoodsID;

		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
		if (pInfo)
		{
			i3::wstring temp_outcategory;
			usf::GetItemItemCategoryString(temp_outcategory, UIShop_Item_Category(pInfo->GetCatagory()), true);	
			outCategory = temp_outcategory;
			outItemName =  pInfo->GetScriptName();
			outDesc     =  pInfo->GetDescription();
		}
		else
		{
			i3::sprintf(outItemName, L"%s\nGoods:%d\nItem:%d", INVALID_TEXT, pGoods->_GoodsID, pGoods->_ItemID); 
			outDesc.clear();
		}
		
		return;
	}

	T_ItemID itemID		= pGoods->_ItemID;
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(itemID);
	if( pInfo )
	{
		i3::wstring temp_outcategory;
		usf::GetItemItemCategoryString(temp_outcategory, UIShop_Item_Category(pInfo->GetCatagory()), false);
		g_pShopItemInfoDataBase->GetItemName(itemID, outItemName);
		outCategory = temp_outcategory;
		outDesc     =  pInfo->GetDescription();
	}
	else
	{
		i3::sprintf(outItemName, L"%s\nGoods:%d\nItem:%d", INVALID_TEXT, pGoods->_GoodsID, pGoods->_ItemID); 
		outDesc.clear();
	}

	// ���� ����
	INT32 itemUsage = CashItem::GetUsageFromItemType(itemID);
	UINT8 itemType = 0;
	UINT32 itemArg = 0;
	UINT32 item_sec_term = 0;
	BOOL bUsed = FALSE;
	i3::wstring strDay;

//	outCategory[0] = 0;

	switch(itemUsage)
	{
	case CASHITEM_USAGE_MAINTENANCE:
		{
			// ���Ӽ� �ϰ�� ������ ���� �迭 �������� ��¥�� ���Ѵ�. (�����Ⱓ �� = (1�� * 1�ϱ� ����) + (7�� * 7�ϱ� ����) + (30�� * 30�ϱ� ����))
			for(INT32 i = 0; i < pGoodsPack->GetGoodsCount(); i++)
			{
				SHOP_GOODS * pGoodsInLoop = pGoodsPack->GetGoods(i);

				if( CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM,  itemID, &itemType, &itemArg) )
				{
					INT32 Day = GET_ITEM_NUMBER(pGoodsInLoop->_ItemID);
					//	������ ���� ���Ⱓ ����
					item_sec_term += Day * itemArg ;
				}
			}
			// �ڱⰡ ���� ������� �������� ���� ���� �Ⱓ�� �����ؾ� �˴ϴ�.
			INT32 ItemKeyID = GetCashItemByGroup(itemID);
			if( ItemKeyID != 0)
			{
				if( CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM, ItemKeyID, &itemType, &itemArg) )
				{
					//	���� ������� ���� �Ⱓ ����
					UINT32 sec_term = 0;
					if( itemArg > 0)
					{
						if( ITEM_ATTR_TYPE_BUY == itemType)
						{
							sec_term = itemArg; 
						}
						else
						{
							sec_term = UserInfoContext::i()->GetInventorySecTime(itemArg); 
						}

						if( sec_term > 0)
						{
							//	�輺�� ����� ��Ź���� �ڵ� �߰��մϴ�. �Ⱓ ���� - komet
							if( sec_term >= ITEM_TERM_ADJUST_VALUE)
								sec_term -= ITEM_TERM_ADJUST_VALUE;

							item_sec_term += sec_term;
						}
					}
					usf::FormatHaveItemSecTermText(item_sec_term, strDay);
					i3::sprintf(outStatus, L"(%s) %s : %s", usf::GetShopItemStatusString(CalcItemStatus(ItemKeyID)), GAME_STRING("STR_SHOP_REMAIN_DAY"), strDay);
				}
				else
				{
					//	������ : %d��
					//					StringCchPrintf(outDurable, MAX_STRING_COUNT, 
					//						"{col:255,255,0,255}%s{/col} ", GAME_STRING("STBL_IDX_ITEM_HAVE_TERM"));
					UINT32 day_term = ConvertSecToUseDay(item_sec_term);
					i3::sprintf(strDay, L"{col:255,255,255,255}%d%s{/col}", day_term, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT")); 
					i3::sprintf(outStatus, L"(%s) %s : %s", GAME_STRING("STBL_IDX_ITEM_HAVE"), GAME_STRING("STR_SHOP_REMAIN_DAY"), strDay);
				}
			}
			else
			{
				if(item_sec_term > 0)
				{
					//	������ : %d��
					UINT32 day_term = ConvertSecToUseDay(item_sec_term);
					i3::sprintf(strDay, L"{col:255,255,255,255}%d%s{/col}", day_term, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT")); 
					i3::sprintf(outStatus, L"(%s) %s : %s", GAME_STRING("STBL_IDX_ITEM_HAVE"), GAME_STRING("STR_SHOP_REMAIN_DAY"), strDay);
				}
			}
			if(item_sec_term > 0)	bUsed = TRUE;
		}
		break;
	case CASHITEM_USAGE_EXPENDABLES:
		{
			if( CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM,  itemID, &itemType, &itemArg) )		// �ڽ��������� ��� �� �Լ��� �˸��� �ʴ�..PB�ڵ尡 ���� �̷��ѵ� �켱 ��ŵ. 11.09.01.����
			{
				item_sec_term = itemArg;	//	������ ����
			}
			if(item_sec_term > 0)
			{
				i3::sprintf(strDay, L"{col:255,255,255,255}%d%s{/col}", item_sec_term, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT")); 
				i3::sprintf(outStatus, L"(%s) %s : %s", usf::GetShopItemStatusString(CalcItemStatus(itemID)), GAME_STRING("STR_SHOP_REMAIN_COUNT"), strDay);

				bUsed = TRUE;
			}
		}
		break;
	}
	outRequired.clear();

	if (!bUsed)
	{
		//	�̺����� ������,�Ⱓ�� �����ϴ� �󼼼����� �ִ´�..
		bool IsCounted = (pGoods->_AuthType == ITEM_TYPE_COUNT);
		i3::stack_wstring temp_outstatus;
		usf::GetGoodsUseDay_Zero(false , IsCounted, temp_outstatus);
		outStatus = temp_outstatus;
	}
}

ItemStatus	UITabShopItem::CalcItemStatus(INT32 itemID)
{
	T_ItemDBIdx ware_idx = CInvenList::i()->FindWareDBIdx_ByItemID(itemID);
	return usf::GetItemStatus(itemID, ware_idx);
}
void UITabShopItem::OnClickMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	m_toolTip->EnableCurrentSlotToolTip(SlotIndex, pGoodsPack);
}

void UITabShopItem::OnEnterMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	m_toolTip->EnableCurrentSlotToolTip(SlotIndex, pGoodsPack);
}

void UITabShopItem::OnLeaveMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	m_toolTip->DisableToolTip(SlotIndex, pGoodsPack);
}


void UITabShopItem::CloseTooltip()
{
	m_toolTip->DisableToolTip_Forced();
}


void	UITabShopItem::_MakeFilterList_ByShopCategory()
{
	ucf::ComboString str_combo;
	str_combo	<< GAME_RCSTRING("STR_SHOP_FILTER_NEW") 
		<< GAME_RCSTRING("STR_SHOP_FILTER_NAME") 
		<< GAME_RCSTRING("STR_SHOP_FILTER_POINT") 
		<< GAME_RCSTRING("STR_SHOP_FILTER_CASH");

	LuaState* L = UILua::CallLuaFunction(m_phaseShop->GetMenuScene(), "SetFilterList");
	const i3::wstring cs_wstr = str_combo;
	i3Lua::PushStringUTF16To8(L, cs_wstr.c_str(), cs_wstr.size());
	i3Lua::PushInteger(L, m_currFilterType);
	UILua::EndLuaFunction(L, 2);
}
