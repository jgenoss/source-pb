#if !defined( __UI_SHOP_FUNCTION_H__)
#define __UI_SHOP_FUNCTION_H__

enum ItemStatus;
enum ShopItemCategory;
enum UIShop_Item_Category;

namespace buy
{
	struct item;
	enum PurchaseRouteType;
}

namespace item_def
{
	enum PartsType_ShopPreview;
}

namespace usf
{
	namespace payment { }
}

namespace usf			// UIShopFunction ���ӽ����̽�..
{
	extern UINT8 g_price_idx;
	void SetPriceIdx(UINT8 price_idx);

	/***********************/
	i3::rc_wstring GetShopItemStatusString(ItemStatus status);
	bool IsSetItem( const SHOP_GOODS* pGoods);	
	
	/*********************/
	void		GetShopItemDescription( i3::rc_wstring& outName, const SHOP_GOODS_PACK* pack);
	void		GetShopItemDescription( i3::rc_wstring& outName, const SHOP_GOODS* goods);
	void		GetShopItemDescription( i3::rc_wstring& outName, T_ItemID itemID);

	/*********************/
	void		GetShopItemName( i3::rc_wstring& outName, const SHOP_GOODS_PACK* pack);
	void		GetShopItemName( i3::rc_wstring& outName, const SHOP_GOODS* goods);
	void		GetShopItemName( i3::rc_wstring& outName, T_ItemID itemID);
	void		GetShopItemName( i3::rc_wstring& outName, ITEM_INFO item_info);
	void		GetShopItemMultiCharaName( i3::rc_wstring& outName, ITEM_INFO item_info);

	/*********************/
	i3::rc_wstring GetItemClassName( T_ItemID itemID); //�ֹ���, ��������, ��������.., �����, ���̽����, ����, ����... (���� ��� "" ��ȯ)
	i3::rc_wstring GetItemUsageName_Only_PrimaryWeapon( T_ItemID itemID); //���ݼ���, SMG, ���ݼ���..(���� ��� "" ��ȯ)

	/*********************/
	i3::rc_wstring GetItemLineUpName_Only_Parts(item_def::PartsType_ShopPreview line_up);
	i3::rc_wstring GetItemLineUpName_Only_Parts( T_ItemID itemID);

	/*********************/
	void	GetShopPeriodCountString( T_ItemID itemID, i3::wstring& outstr);
	void	GetShopPeriodCountString( T_ItemID itemID, UINT8 authType, UINT32 authArg, i3::wstring& outStr);
	void	GetShopPeriodCountString(const SHOP_GOODS* pGoods, i3::wstring& outstr);
	void	GetShopPeriodCountString(SHOP_GOODS_PACK* pack, i3::wstring& outstr, const wchar_t* word= L";");
	void	GetShopPeriodCountStringByGoodsID(const T_GoodsID goodsID, i3::wstring& outstr);
	void GetShopPeriodCountString_only_Item( T_ItemID itemID, i3::wstring& outName); //������ �׸� �����ϴ�.

	/*********************/
	ShopItemCategory	GetShopItemCategory( T_ItemID itemID);
	ShopItemCategory	GetShopItemCategory(const SHOP_GOODS* goods);

	/*********************/
	ItemStatus	 GetItemStatus( T_ItemID itemID, T_ItemDBIdx ware_idx);	//�κ���
	ItemStatus	 GetItemStatus(const SHOP_GOODS_PACK* goods_pack); //������

	/*********************/
	bool IsExtensionItem( T_ItemID itemID);
	bool IsExtensionItem( const SHOP_GOODS* pGoods);

	/*********************/
	bool HasExtensionDescImage( T_ItemID itemID);
	bool HasExtensionDescImage( SHOP_GOODS* pGoods);


	/*********************/
	UIShop_Item_Category GetItemItemCategory( SHOP_GOODS* goods);
	UIShop_Item_Category GetItemItemCategory( T_ItemID itemID);

	/*********************/
	void	FormatHaveItemSecTermText(UINT32 SecTerm, i3::wstring& outDay);		// �ʴ����� ��� ����..
	
	void	GetGoodsUseDay(ItemStatus status, bool IsRepairItem, bool IsCountItem, UINT8 Type, UINT32 Arg, 
		i3::wstring& outStatus, bool includeState = true);
	void	GetItemUseDay(ItemStatus status, bool IsRepairItem, T_ItemID itemID, T_ItemDBIdx ware_idx, 
		i3::wstring& outStatus, bool includeState = true);	//inven ��

	void	GetGoodsUseDay_Zero(bool IsRepairItem, bool IsCountItem, i3::stack_wstring& outStatus);

	i3::rc_wstring GetCharacterItemCategoryString(INT32 characID);
	void	GetItemItemCategoryString(i3::wstring& out_wstr, UIShop_Item_Category cat, bool is_set_item);

	void GetShopGoodInfo(const SHOP_GOODS* pGoods, GOODS_PRICE_INFO & info);
	bool GetShopPriceString(SHOP_GOODS* pGoods, i3::wstring& outResult, GOODS_PRICE_TYPE t = GOODS_PRICE_ALL);
	bool GetShopPriceStringComma(SHOP_GOODS* pGoods, i3::wstring& inoutwstrResult, GOODS_PRICE_TYPE t = GOODS_PRICE_ALL);
	void	 GetShopPriceAndMyMoney(SHOP_GOODS* pGoods, UINT32& price, UINT32& mymoney, GOODS_PRICE_TYPE t = GOODS_PRICE_ALL);

	bool CheckGoodsPackBothPrices(SHOP_GOODS_PACK* pack);
	void	 CalcGoodsPackMapByPriceType( SHOP_GOODS_PACK* pack, INT32& out_num, INT32 (&out_array)[4], GOODS_PRICE_TYPE t = GOODS_PRICE_ALL );
	bool CalcGoodsPackFirstIdxByPriceType(  SHOP_GOODS_PACK* pack, INT32& out_idx, GOODS_PRICE_TYPE t = GOODS_PRICE_ALL);

	// ������ �� ����/��¥�� ����ī�װ��� �� �����ڿ�..
	INT32		GetGoodsCount(SHOP_GOODS_PACK* pack);
	SHOP_GOODS*	GetGoodsFromPack(SHOP_GOODS_PACK* pack, INT32 goods_idx);

/*
	void GetWeaponSpecificValues( T_ItemID itemID, 
		INT32& Damage, INT32& Rapid, INT32& Accuracy, INT32& Recoil_V, 
		INT32& Recoil_H, INT32& Speed, char* strRange, const size_t sizeRangeMax, 
		char* strBulletCount, const size_t sizeBulletCountMax);
	void GetWeaponSpecificValues( SHOP_GOODS_PACK* pack, 
		INT32& Damage, INT32& Rapid, INT32& Accuracy, INT32& Recoil_V, 
		INT32& Recoil_H, INT32& Speed, char* strRange, const size_t sizeRangeMax,
		char* strBulletCount, const size_t sizeBulletCountMax );
*/	
/*
	void SetShopPointInfo(SHOP_GOODS * pGoods, GOODS_PRICE_TYPE price_type, 
		i3::rc_wstring& out_wstrSubjectPointCash,
		char* szHavePointCash, size_t sizeHavePointCash,
		char* szBuyPointCash, size_t sizeBuyPointCash,
		char* szRemainPointCash, size_t sizeRemainPointCash,
		const char* szMoneySuffix = "",
		const INT32	nCouponIdx = -1);
*/

	bool GetDurability( T_ItemID itemID, T_ItemDBIdx ware_idx, REAL32& rate );	// T_ItemDBIdx ( DB �ε��� ) �� �𸣸� 0�� �־��ָ� �ȴ�.

	//Goods.pef�� ��ϵ��� �ʴ� �������� �������� ���Ÿ��ϰ� �Ѵ�.
	bool CheckValidGoods_in_Pef( T_ItemID itemID);
	bool CheckValidGoods_in_Pef( const SHOP_GOODS_PACK * pack);

	//�������� �������� �����ϰ� ���� ��츦 �˻��Ѵ�.
	enum ValidBuyType
	{
		vbt_Invalid = -1,
		
		vbt_None= 0,
		vbt_Ok = vbt_None,
		vbt_Perpetuity,	//������ ��ǰ�̸�, �̹� ���Ե� ����
		vbt_Only_Clan_master, //Ŭ�� �����͸� ���� ������ ������
		vbt_Limited_Clan_member, //Ŭ���� ���� �������� �ִ� ȸ������ ������ ���� �Ұ� 
		vbt_Pushed_Perpetuity,	//������ ��ǰ�� īƮ�� �ϳ��� ���� �� �ִ�.
		vbt_Disable_Buy_Parts_Any_more,	//������ ���� ĳ���� �� ���� ���� ���� �� �� �����ϴ�.
		vbt_Exceed_MaxPurchaseableCharacters, // ĳ���� �ִ뺸������(9��) �̻����� ���Ű� �Ұ����մϴ�.
	};
	bool IsValidBuyPartsGoods(const SHOP_GOODS_PACK* pack);
	ValidBuyType IsValidBuyGoods(const SHOP_GOODS_PACK * pack);


	ValidBuyType IsValidBuyGoods_InCart(const SHOP_GOODS_PACK * pack);

	INT32 GetValidPurchaseCount(const SHOP_GOODS_PACK * pack, INT32 count);

	void Message_BuyResert(ValidBuyType fb, bool chat_msg = false);

	//������ ��� ���� ���θ� �˻��Ѵ�.
	enum ValidUseType
	{
		vut_None= 0,
		vut_Ok = vut_None,
		vut_Duplicate,	//�ߺ� ����� �� ����.
	};
	ValidUseType IsUseableItem( T_ItemID itemID );
	void Message_UseResert(ValidUseType fb, bool chat_msg = false);

	UINT32 GetDisableDuplicateUseItemPair( T_ItemID item_id); //�ߺ� ����� �� ���� ������ itemID �ݴ��� �������� �˷��ش�. ���� ��� 0����
	
	bool CheckBuyItemPeriod( const SHOP_GOODS_PACK* pack, UINT32 buySlot );		// �κ������� ���ѵ� ���ᳯ¥ 16.11.11 �����


	bool PopupRestriction(UINT32 ui32ItemID);				// buyutil.h�� ����ġ �����Ƿ� �������� �ϴ� �Ű���ϴ�..
}

namespace usf
{
	namespace payment
	{
		//SHOP_GOODS_PACK���� ���� ������ �ݾ��� ��ȯ�Ѵ�.
		//Cash String + Point String ������ ���´�.
		//��) POINT
		i3::wstring get_price_combo_string( const SHOP_GOODS_PACK* packs );
	
		//buy::item ���� �ݾ��� �����Ѵ�.
		
	}


	bool CheckValidGoods(const SHOP_GOODS_PACK * pack, bool chat_msg);	
	bool CheckValidGoods_InCart(const SHOP_GOODS_PACK * pack);

}

#endif