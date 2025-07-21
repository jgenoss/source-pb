#pragma once

namespace item_def
{
	enum PartsType_ShopPreview;
}

namespace buy
{

	class DisplayedGoods
	{
	public:
		DisplayedGoods(const SHOP_GOODS_PACK* pPack);
	
		const SHOP_GOODS_PACK*  GetGoodsPack() const { return m_Pack;  }

		void					SetCurrentDisplayIndex(INT32 displayIndex) { m_currDisplayIndex = displayIndex; }
		void					SetCurrentDisplayIndexByPriceType(GOODS_PRICE_TYPE priceType, INT32 displayIndexByPriceType);
		
		INT32					GetCurrentDisplayIndex() const { return m_currDisplayIndex; }
		INT32					GetCurrentDisplayIndexByPriceType() const;

		INT32					GetCurrentPackIndex() const { return  m_vecDisplayPackIndex[m_currDisplayIndex]; }
		const SHOP_GOODS*		GetCurrentGoods() const {  return  m_Pack->GetGoods(GetCurrentPackIndex()); }
		GOODS_PRICE_TYPE		GetCurrentPriceType() const { return (m_currDisplayIndex < m_numPointItem) ? GOODS_PRICE_POINT : GOODS_PRICE_CASH; }
		const SHOP_GOODS*		GetPackByDisplayIndex(INT32 displayIdx) const { return m_Pack->GetGoods(m_vecDisplayPackIndex[displayIdx]); }
		INT32					GetDisplayCount() const { return m_vecDisplayPackIndex.size(); }
		
		T_ItemDBIdx				GetItemDBIndex() const { return m_itemDBIndex;  }
		void					SetItemDBIndex(T_ItemDBIdx dbIndex) { m_itemDBIndex = dbIndex;  }
		bool					IsItemDBIndexValid() const { return m_itemDBIndex != -1;  }
	private:
		const SHOP_GOODS_PACK*	m_Pack;
		i3::vector<INT32>		m_vecDisplayPackIndex;
		INT32					m_numPointItem;				// UIǥ�ýÿ� ����Ʈ->ĳ�� ������ ǥ�õǸ�, ����Ʈ�����۰����� �ʿ��ϱ� ��..
		INT32					m_currDisplayIndex;	// �����Ͱ� �޸�, price_type�� ������ m_numPointItem�� ��ȿ���� ���η� ������� �մϴ�.

		T_ItemDBIdx				m_itemDBIndex;		// �κ��丮������ ���ϵ��ѵ�... �������������� �볳�� �ȵǼ�, �������� �ֽ��ϴ�..

	};
	
	typedef i3::shared_ptr<DisplayedGoods>			DisplayedGoodsPtr;
	typedef i3::vector< buy::DisplayedGoodsPtr >	DisplayedGoodsList;

	DisplayedGoodsPtr			MakeDisplayedGoodsPtr(const SHOP_GOODS_PACK* pPack);

}

namespace buy
{
	enum PurchaseRouteType 
	{ 
		PurchaseRoute_Immediate, 
		PurchaseRoute_Extend, 
		PurchaseRoute_Cart, 
		PurchaseRoute_Preview, 
		PurchaseRoute_Max 
	}; //��� ����, �Ⱓ ���� ����, īƮ ����, ���� �̸����� ����

	struct PurchaseInfo
	{
		i3::vector< DisplayedGoodsPtr > vecGoods;	
		PurchaseRouteType		route;
	};

	void ClearPreview(); //������� -> ���� �׵��� ����...�򰥸��� �ڵ���� ���� �׸�...
	void ClearCart();
	void ClearImmediate();
	void Clear_ByPurchaseRoute(buy::PurchaseRouteType routeType);

	void PushImmediate(const SHOP_GOODS_PACK* pack);		// ��..�����̽��ε��� �μ�...�׵��� �������� �巴�� �Ѵ�...
	void PushImmediate(const SHOP_GOODS_PACK* pack, INT32 displayIndex);
	void PushImmediate(const SHOP_GOODS_PACK* pack, GOODS_PRICE_TYPE priceType, INT32 displayIndexByPriceType);
	void PushImmediateWithDBIndex(const SHOP_GOODS_PACK* pack, T_ItemDBIdx dbIndex);


	void AddShopGoodPackToPreview(const SHOP_GOODS_PACK* pack);
	void AddShopGoodPackToCart(const SHOP_GOODS_PACK* pack);
	void AddShopGoodPackToCart(const SHOP_GOODS_PACK* pack, INT32 displayIndex);
	
	void RemovePreview(item_def::PartsType_ShopPreview partsType);
	void RemoveCart(size_t idx);				// �̰� �ʿ䰡 �ִ��� �𸣰ڴ�..�ϴ� �ۼ�..

	void RemoveByPurchaseRoute(buy::PurchaseRouteType buy_type, size_t idx);


	INT32 RemoveDisplayedGoodsInCart(const SHOP_GOODS* pGoods);			// ȣȯ�������� ���󰡴� ������ ������, ���α����� �� ������...�񱳱��� ������ �ְ����̴�..

	bool  IsExistedShopGoodPackInCurrentCharaCart(const SHOP_GOODS_PACK* pPack);

	INT32 GetCount_Immediate();
	INT32 GetCount_Preview();
	INT32 GetCount_CurrCharaCart();
		
	INT32 GetCount_ByPurchaseRouteType(buy::PurchaseRouteType route);
	INT32 GetNumSlot_Preview();
	
	DisplayedGoods* GetDisplayedGoods_Immediate();
	DisplayedGoods* GetDisplayedGoods_Immediate(INT32 idx);
	DisplayedGoods* GetDisplayedGoods_ByPreviewSlot(item_def::PartsType_ShopPreview slot); // �ǰ����� �ƴ� �����ε����Դϴ�..
	DisplayedGoods* GetDisplayedGoods_ByPreviewActive(INT32 idxActive);
	DisplayedGoods*	GetDisplayedGoods_ByCurrCharaCart(INT32 idx);
	DisplayedGoods* GetDisplayedGoods_ByPurchaseRoute(buy::PurchaseRouteType routeType, INT32 idx);

	// UI�󿡼� ��ŷ�� Ȥ�� ��Ƴ��� ��ǰ ����� �����Ͽ� ���ɴϴ�. ( ��������� ó���صӴϴ�...)
	void			GatherDisplayedGoods_ByPurchaseRoute(buy::PurchaseRouteType routeType, 
														i3::vector< buy::DisplayedGoodsPtr>& outList);

	void			UpdateItemInfoListWithPreview(i3::vector< ITEM_INFO >& item_info_list);

		
	//  �̻��� ��ǰ�� �����մϴ�.  (ĳ���� ���� ��� ���� item���� ������ �Ѿ�� ��Ͽ� �ȵ����� ������...)
	void			RemoveInvalidDisplayedGoods_CartPreview(buy::PurchaseRouteType routeType, bool& out_is_invalid);

	
}
