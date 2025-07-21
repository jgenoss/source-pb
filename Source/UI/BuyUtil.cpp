#include "pch.h"
#include "BuyUtil.h"

#include "UIItemInfoUtil.h"


#include "UIUtil.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include <functional>



#include "UIShopFunction.h"

#include "i3Base/itl/algorithm/remove_if.h"
#include "i3Base/itl/range/algorithm/find_if.h"

namespace buy
{
	namespace
	{
		bool BuildDisplayList(const SHOP_GOODS_PACK* pPack,
			i3::vector<INT32>&	outvecDisplayPackIndex, INT32& outnumPointItem)
		{
			if (pPack == NULL)
				return false;

			const INT32 numGood = pPack->GetGoodsCount();

			for (INT32 i = 0; i < numGood; ++i)
			{
				const SHOP_GOODS* pGood = pPack->GetGoods(i);
				if (pGood->_SaleType == GOODS_SALE_EVENT)
					continue;

				if (pGood->GetPriceType(usf::g_price_idx) & GOODS_PRICE_POINT)
					outvecDisplayPackIndex.push_back(i);
			}

			outnumPointItem = outvecDisplayPackIndex.size();

			for (INT32 i = 0; i < numGood; ++i)
			{
				const SHOP_GOODS* pGood = pPack->GetGoods(i);

				if (pGood->_SaleType == GOODS_SALE_EVENT)
					continue;

				if (pGood->GetPriceType(usf::g_price_idx) & GOODS_PRICE_CASH)
					outvecDisplayPackIndex.push_back(i);
			}

			return !outvecDisplayPackIndex.empty();
		}

	}

	DisplayedGoodsPtr	MakeDisplayedGoodsPtr(const SHOP_GOODS_PACK* pPack) 
	{
		if (pPack == NULL) return DisplayedGoodsPtr();
		DisplayedGoodsPtr new_ptr = i3::make_shared<DisplayedGoods>(pPack);
		if (new_ptr->GetDisplayCount() == 0)
			new_ptr.reset();
		return new_ptr;
	}
	
	DisplayedGoods::DisplayedGoods(const SHOP_GOODS_PACK* pPack) : m_Pack(pPack), m_currDisplayIndex(0),
		m_numPointItem(0), m_itemDBIndex(-1)
	{
		BuildDisplayList(pPack, m_vecDisplayPackIndex, m_numPointItem);
	}

	void	DisplayedGoods::SetCurrentDisplayIndexByPriceType(GOODS_PRICE_TYPE priceType, INT32 displayIndexByPriceType)
	{
		m_currDisplayIndex = (priceType == GOODS_PRICE_CASH) ? (m_numPointItem + displayIndexByPriceType) : displayIndexByPriceType;
	}

	INT32	DisplayedGoods::GetCurrentDisplayIndexByPriceType() const
	{
		return (m_currDisplayIndex >= m_numPointItem) ? (m_currDisplayIndex - m_numPointItem) : m_currDisplayIndex;
	}
	

}


//------------------------------////------------------------------//
namespace value
{


	typedef i3::vector< buy::DisplayedGoodsList > Chara_ItemList;

	buy::DisplayedGoodsPtr   immediate;			// �̰��� ��ȿ�� ������ �ʿ���..
	buy::DisplayedGoodsPtr	  preview[item_def::PartsType_ShopPreview_Max];	// �̰Ÿ� ���ġ �����ǰ�..��ȿ�� ���� �ʿ���..

	buy::DisplayedGoodsList	activePreviewList;		// 7�� �� ���� ��ȿ�� �͸� ����...�ǽð� ó���ϱ�? ������ ������ ��°�� ������Ʈ�ϴ� ���� ȿ����.



	value::Chara_ItemList	cart;				// ��ٱ��� �̳� ����...ĳ���Ͱ� ���������� V3���������� ���ߺ��ͷ� ��...

	struct init_value
	{
	//	enum { MAX_CART_ITEMS = 9 }; 
		init_value()
		{
			value::cart.resize(1);						// �ϴ� 1���� ������ �Ǹ�, �ʿ��Ҷ� ������¡ �ϵ��� �մϴ�.
		}
	} _init_value;
}

//------------------------------////------------------------------//
namespace util
{

	buy::DisplayedGoodsList& GetDisplayedGoodsList_Cart();
	
}

buy::DisplayedGoodsList& util::GetDisplayedGoodsList_Cart()
{
	if (g_pEnvSet->IsV2Version() == false)
		return value::cart[0];

	INT32 idx =	CCharaInfoContext::i()->GetMainCharaSlotIdx(TEAM_NONE);			// �̰� ���� ����...�������̽���...

	if (idx >= INT32(value::cart.size()))
	{
		I3_BOUNDCHK(idx, MULTI_SLOT_COUNT);
		value::cart.resize(idx + 1);				// �ʿ��Ҷ��� ����..
	}

	return value::cart[idx];
}


void buy::ClearImmediate()
{
	value::immediate.reset();
}

void buy::ClearPreview()
{
	i3::for_each(value::preview, value::preview + item_def::PartsType_ShopPreview_Max,
		i3::mem_fun_ref<void, DisplayedGoodsPtr>(&buy::DisplayedGoodsPtr::reset));	// �����ε��Լ�����, ���ø� �Ű����� ���� ������ �䱸�Ǿ���.

	value::activePreviewList.clear();		
}

void buy::ClearCart()
{
	buy::DisplayedGoodsList& goodsList = util::GetDisplayedGoodsList_Cart();
	goodsList.clear();				// ������..�� ����...
}

void buy::Clear_ByPurchaseRoute(buy::PurchaseRouteType route)
{
	static void (*fn[])() = 
	{ 
		buy::ClearImmediate, buy::ClearImmediate, buy::ClearCart, buy::ClearPreview
	};

	(*fn[route])();
}

//-- push

void buy::PushImmediate(const SHOP_GOODS_PACK* pack)	// ��..�����̽��ε��� �μ�...�׵��� �������� �巴�� �Ѵ�...
{
	value::immediate = MakeDisplayedGoodsPtr(pack);
}

void buy::PushImmediate(const SHOP_GOODS_PACK* pack, INT32 displayIndex)
{
	value::immediate = MakeDisplayedGoodsPtr(pack);
	if (value::immediate)
		value::immediate->SetCurrentDisplayIndex(displayIndex);
}

void buy::PushImmediate(const SHOP_GOODS_PACK* pack, GOODS_PRICE_TYPE priceType, INT32 displayIndexByPriceType)
{
	value::immediate = MakeDisplayedGoodsPtr(pack);
	if (value::immediate)
		value::immediate->SetCurrentDisplayIndexByPriceType(priceType, displayIndexByPriceType);
}

void  buy::PushImmediateWithDBIndex(const SHOP_GOODS_PACK* pack, T_ItemDBIdx dbIndex)
{
	value::immediate = MakeDisplayedGoodsPtr(pack);
	if (value::immediate)
		value::immediate->SetItemDBIndex(dbIndex);
}

namespace
{
	void UpdateActivePreviewListWithPreviewArrayImp()			// ������������ ���Ǵٺ���..�μ��� ���� �ʵ��� �޽��ϴ�..
	{
		value::activePreviewList.clear();

		for (INT32 i = 0; i < item_def::PartsType_ShopPreview_Max; ++i)
		{
			const buy::DisplayedGoodsPtr& goods_ptr = value::preview[i];
			if (goods_ptr)
			{
				value::activePreviewList.push_back(goods_ptr);
			}
		}
	}

}


void buy::AddShopGoodPackToPreview(const SHOP_GOODS_PACK* pack)
{
	item_def::PartsType_ShopPreview partsType = item_def::get_shop_preview_parts_type(pack->GetGoods(0)->_ItemID);
	I3ASSERT_RETURN(partsType != item_def::PartsType_ShopPreview_None);
	
	DisplayedGoodsPtr newGood = buy::MakeDisplayedGoodsPtr(pack);
	value::preview[partsType] = newGood;									// �̷��� ������ ����...�������� �ϳ����ϻ��Դϴ�...

	UpdateActivePreviewListWithPreviewArrayImp();
}


namespace
{
	void AddShopGoodPackToCartImp(const buy::DisplayedGoodsPtr& newGood)
	{
		buy::DisplayedGoodsList& currGoodsList = util::GetDisplayedGoodsList_Cart();

		if (currGoodsList.size() >= USE_BASKET_COUNT)			// ��ٱ��� �ִ밹���� �о�Ⱑ �Ǿ�� �մϴ�. (��ٱ��� ���� ������)
			currGoodsList.erase(currGoodsList.begin());

		currGoodsList.push_back(newGood);
	}

}

void buy::AddShopGoodPackToCart(const SHOP_GOODS_PACK* pack)
{
	DisplayedGoodsPtr newGood = buy::MakeDisplayedGoodsPtr(pack);
	AddShopGoodPackToCartImp(newGood);
}

void buy::AddShopGoodPackToCart(const SHOP_GOODS_PACK* pack, INT32 displayIndex)
{
	DisplayedGoodsPtr newGood = buy::MakeDisplayedGoodsPtr(pack);
	AddShopGoodPackToCartImp(newGood);

	if (newGood)
		newGood->SetCurrentDisplayIndex(displayIndex);					// ����..���� ��������...
}

void buy::RemovePreview(item_def::PartsType_ShopPreview partsType)
{
	I3_BOUNDCHK_RETURN(INT32(partsType), INT32(item_def::PartsType_ShopPreview_Max));

	if (value::preview[partsType])
	{
		value::preview[partsType].reset();		// �� ���ָ� ����..
		UpdateActivePreviewListWithPreviewArrayImp();
	}
}

void buy::RemoveCart(size_t idx)				// �̰� �ʿ䰡 �ִ��� �𸣰ڴ�..�ϴ� �ۼ�..
{
	buy::DisplayedGoodsList& goodsList = util::GetDisplayedGoodsList_Cart();
	if (idx >= goodsList.size()) 
		return;			// �Ʈ ��� �׳� �����մϴ�.
	i3::vu::erase_index(goodsList, idx);
}

void buy::RemoveByPurchaseRoute(buy::PurchaseRouteType buy_type, size_t idx)
{
	switch (buy_type)
	{
	case buy::PurchaseRoute_Immediate:	value::immediate.reset();	break;
	case buy::PurchaseRoute_Extend:		value::immediate.reset();	break;
	case buy::PurchaseRoute_Cart:		buy::RemoveCart(idx);		break;
	case buy::PurchaseRoute_Preview:	buy::RemovePreview(item_def::PartsType_ShopPreview(idx));	break;
	}
}

//-- size

INT32 buy::GetCount_Immediate()
{
	return (value::immediate != nullptr) ? 1 : 0;
}

INT32 buy::GetCount_Preview()
{
	return INT32(value::activePreviewList.size());				// ������迭�� ���� �ȵ˴ϴ�.
}

INT32 buy::GetCount_CurrCharaCart()
{
	buy::DisplayedGoodsList& currCharaCartList = util::GetDisplayedGoodsList_Cart();
	return INT32(currCharaCartList.size());
}

INT32 buy::GetCount_ByPurchaseRouteType(buy::PurchaseRouteType route)
{
	static INT32(*fn[])() =
	{
		buy::GetCount_Immediate, buy::GetCount_Immediate, buy::GetCount_CurrCharaCart, buy::GetCount_Preview,
	};
	return (*fn[route])();
}

INT32 buy::GetNumSlot_Preview()
{
	return item_def::PartsType_ShopPreview_Max;				// ���� �������Դϴ�...�����۰����� �ƴ� ���԰����Դϴ�..
}

buy::DisplayedGoods* buy::GetDisplayedGoods_ByPreviewSlot(item_def::PartsType_ShopPreview slot) // �ǰ����� �ƴ� �����ε����Դϴ�..
{
	I3_BOUNDCHK_RETURN(slot, item_def::PartsType_ShopPreview_Max, NULL);
	return value::preview[slot].get();				// ��ȿ�� �ΰ��� �� ���;� �մϴ�..
}

buy::DisplayedGoods* buy::GetDisplayedGoods_Immediate()
{
	return value::immediate.get();
}

buy::DisplayedGoods* buy::GetDisplayedGoods_Immediate(INT32 idx)			// �ǹ̾��� �ε����Դϴ�. ������ ���ؼ��� ���ϴ�.
{
	return value::immediate.get();
}

buy::DisplayedGoods* buy::GetDisplayedGoods_ByPreviewActive(INT32 idxActive)
{
	return value::activePreviewList[idxActive].get();				//�ϴ� ���˻� ���մϴ�.  ������ �Ѿ�� �ʵ��� ���ݸ� �Ű澹�ô�..
}

buy::DisplayedGoods*	buy::GetDisplayedGoods_ByCurrCharaCart(INT32 idx)
{
	buy::DisplayedGoodsList& currCharaCartList = util::GetDisplayedGoodsList_Cart();
	return currCharaCartList[idx].get();
}

buy::DisplayedGoods* buy::GetDisplayedGoods_ByPurchaseRoute(buy::PurchaseRouteType routeType, INT32 idx)
{
	static buy::DisplayedGoods* (*fn[])(INT32) =
	{
		buy::GetDisplayedGoods_Immediate, buy::GetDisplayedGoods_Immediate, //�� �κ��� ��� �� ��
		buy::GetDisplayedGoods_ByCurrCharaCart, buy::GetDisplayedGoods_ByPreviewActive,
	};
	return (*fn[routeType])(idx);

}

void	buy::GatherDisplayedGoods_ByPurchaseRoute(buy::PurchaseRouteType routeType,
	i3::vector< buy::DisplayedGoodsPtr>& outList)
{
	outList.clear();			// 
	switch (routeType)
	{
	case buy::PurchaseRoute_Immediate:	if (value::immediate)	outList.push_back(value::immediate);	break;
	case buy::PurchaseRoute_Extend:		if (value::immediate)	outList.push_back(value::immediate);	break;
	case buy::PurchaseRoute_Cart:		outList = util::GetDisplayedGoodsList_Cart();					break;
	case buy::PurchaseRoute_Preview:	outList = value::activePreviewList;								break;
		break;
	}
}

void		buy::UpdateItemInfoListWithPreview(i3::vector< ITEM_INFO >& item_info_list)
{
	const INT32 numPreviewSlot = buy::GetNumSlot_Preview();

	for (INT32 i = 0; i < numPreviewSlot; ++i)
	{
		const DisplayedGoods* goods = buy::GetDisplayedGoods_ByPreviewSlot((item_def::PartsType_ShopPreview)i);

		if (goods != NULL)			// ������ ��� Ȯ���� �ΰ��Դϴ�.
		{
			item_info_list[i].m_TItemID = goods->GetGoodsPack()->GetGoods(0)->_ItemID;
			//GetWareDBIdx_ByID �Լ��� ������ ��� �ùٸ� m_TItemDBIdx �� ���� ���Ѵ�. 
			//������ chara_view���� �ùٸ� m_TItemDBIdx�� ������ �����Ŷ� �����Ǳ� ������(m_TItemID �� ���� �߿�)
			//GetWareDBIdx_ByID �� �Լ��� ����Ѵ�. 
			item_info_list[i].m_TItemDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID(item_info_list[i].m_TItemID);
		}
	}
}



//-- only cart
bool  buy::IsExistedShopGoodPackInCurrentCharaCart(const SHOP_GOODS_PACK* pPack)
{
	buy::DisplayedGoodsList& list = util::GetDisplayedGoodsList_Cart();
	
	struct fn : i3::binary_function< DisplayedGoodsPtr, const SHOP_GOODS_PACK*, bool>
	{
		bool operator()(const DisplayedGoodsPtr& lhs, const SHOP_GOODS_PACK* rhs) const
		{
			const SHOP_GOODS_PACK* pack_Lhs = lhs->GetGoodsPack();
			return *pack_Lhs == *rhs;					// ������ ���ε�, �������� ������ �ʿ��Ѱ����� �ƴ��� ���� ���������� ����..
		}
	};

	buy::DisplayedGoodsList::const_iterator it = i3::find_if(list.begin(), list.end(), i3::bind2nd(fn(), pPack));

	return it != list.end();
}

INT32 buy::RemoveDisplayedGoodsInCart(const SHOP_GOODS* pGoods)
{

	INT32 eraseCount = 0;

	const size_t numChara = value::cart.size();			// �� ����ĳ���͵��� ��ٱ��� �ϳ��� ��� �ִ� ����..
	
	for (size_t i = 0; i < numChara; ++i)
	{
		buy::DisplayedGoodsList& goodsList = value::cart[i];

		struct fn : i3::binary_function< DisplayedGoodsPtr, const SHOP_GOODS*, bool>
		{
			bool operator()(const DisplayedGoodsPtr& goodsPtr, const SHOP_GOODS* pGoods) const
			{
				const SHOP_GOODS_PACK* pPack_Lhs = goodsPtr->GetGoodsPack();			// ������ ���̴�...

				const INT32 numGood = pPack_Lhs->GetGoodsCount();

				for (INT32 i = 0; i < numGood; ++i)					// ���� ���� �ƴ� ��ü�� �� ���ƾ� �ϴ°� ���� ����������..
				{
					const SHOP_GOODS* pGood_Lhs = pPack_Lhs->GetGoods(i);

					if (*pGood_Lhs != *pGoods)	// ������ �����̴�... �ӵ��� �ణ ������ �񱳱����̴�.. �ϳ��� �ٸ��� false��µ�....��������...�񱳱����� �� �Žñ��ϴ�....
						return false;
				}

				return true;					// ��ü�� �� ���ƾ� true�ε��ѵ�....�̰� �����񶧸���...�������� �񱳱����ϼ� ������, ȣȯ�������� ���� �����Ѵ�..
			}									// ���� ������ �������� �ߺ��������� ���̴ϱ�..ū������ ������, �Լ���ü�� �������� ������� ���� �ִ�...
		};										// �ƹ����� �ʹ� �� ���Ƽ� �� �����ؾߵɵ� �ʹ�....�ֺ� �ڵ尡 ��� ���� �����Ǹ� �� ������ ���� ������ �����ϰڴ�...

		buy::DisplayedGoodsList::iterator it = i3::remove_if(goodsList.begin(), goodsList.end(),
			i3::bind2nd(fn(), pGoods));

		if (it != goodsList.end())
		{
			eraseCount += i3::distance(it, goodsList.end());
			goodsList.erase(it, goodsList.end());
		}
	}

	return eraseCount;
}

namespace
{
	bool HasMaximumItemCount(const SHOP_GOODS_PACK * pack)
	{
		SHOP_GOODS* pGoods = pack->GetGoods(0);

		ShopItemCategory catShop = (usf::GetShopItemCategory(pGoods->_ItemID));
		if (catShop != SHOP_ITEM_CATEGORY_CHARACTER)
			return false;

		item_def::Category catItem = item_def::get_category(pGoods->_ItemID);

		if (catItem != item_def::CTG_PARTS && catItem != item_def::CTG_SKIN)
			return false;
		
		return true;
	}
	
}


// ������ ������ �Լ�3���� �����ϰ� �����ϸ� �� �Լ��� �����մϴ�...(�� ������ �����Ÿ� �������ϰ� �������� ���ô�...���躸�ٴ� �������� �� ������ ��Ĩ�ϴ�..)

void	buy::RemoveInvalidDisplayedGoods_CartPreview(buy::PurchaseRouteType routeType, bool& out_is_invalid)
{
	out_is_invalid = false;

	buy::DisplayedGoodsList* pGoodsList = NULL;
	switch (routeType)
	{
	case buy::PurchaseRoute_Cart:		pGoodsList = &util::GetDisplayedGoodsList_Cart();	break;
	case buy::PurchaseRoute_Preview:	pGoodsList = &value::activePreviewList; break;
	default:			return;
	}

	UINT32 chara_cnt = (UINT32)CCharaInfoContext::i()->GetHasCharacterCount();			// �� �Լ� �������� �������̴�.

	buy::DisplayedGoodsList& goodsList = *pGoodsList;

	struct SHOP_GOODS_PACK_Available { const SHOP_GOODS_PACK* pack;	INT32	availableCount; };

	i3::vector<SHOP_GOODS_PACK_Available>	vecGoodsPack;	// ������ ��쿡 �ְ�, �ߺ�üũ�� �ϸ� �˴ϴ�.
	
	for (INT32 i = 0; i < INT32(goodsList.size()); ++i)
	{
		const buy::DisplayedGoodsPtr& goods = goodsList[i];

		const SHOP_GOODS_PACK* pGoodPack = goods->GetGoodsPack();

		if (HasMaximumItemCount(pGoodPack) == false)
			continue;
		
		struct fn : i3::binary_function< SHOP_GOODS_PACK_Available, const SHOP_GOODS_PACK*, bool>
		{
			bool operator()(const SHOP_GOODS_PACK_Available& lhs, const SHOP_GOODS_PACK* rhs) const { return *lhs.pack == *rhs; }
		};
		i3::vector<SHOP_GOODS_PACK_Available>::iterator it = i3::range::find_if(vecGoodsPack, i3::bind2nd(fn(), pGoodPack));
			
		INT32 availableCount = 0;

		if (it == vecGoodsPack.end())
		{
			SHOP_GOODS* pGoods = pGoodPack->GetGoods(0);
			INT32 invenCount = CInvenList::i()->FindItemCount(pGoods->_ItemID);

			SHOP_GOODS_PACK_Available p;
			p.pack = pGoodPack;
			p.availableCount = chara_cnt - invenCount;				// ���� ���밹��..
			availableCount = p.availableCount;

			--p.availableCount;										// �ϳ��� ���δ�..
			vecGoodsPack.push_back(p);
		}
		else
		{
			availableCount = (*it).availableCount;
			--(*it).availableCount;									// ��� ���δ�.. ��� ���ڸ��� Ȯ�� ����..
		}

		if (availableCount < 1)										// ĳ���Ͱ������� ���� ������ �����Ƿ�, ������ �Ѵٰ� �մϴ�..
		{
			out_is_invalid = true;
			i3::vu::erase_index(goodsList, i);		--i;			// ����ó��..continue�� ���� ����..
		}
	}
	
}
