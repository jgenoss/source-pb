#include "pch.h"
#include "UI_V11/ShopContextBasket.h"


CShopBasket::Basket::Basket(SHOP_GOODS_PACK * pac)
{
	pack = pac;
	packInx = 0;

	type = pac->GetGoods(0)->_SaleType;
	if( type == GOODS_PRICE_ALL )	//ALL�� ��쿣 Point�� ���� ó���Ѵ�.
		type = GOODS_PRICE_POINT;

	dpinx = 0;
}

CShopBasket::CShopBasket()
{
	i3mem::FillZero( m_nCouponDBIndex, sizeof( m_nCouponDBIndex));
}

CShopBasket::~CShopBasket()
{
	for(INT32 i = 0 ; i < USE_BASKET_COUNT ; i++)
	{
		m_nCouponDBIndex[i] = -1;	
	}
}

void CShopBasket::clear()
{
	m_BasketList.clear();
}

void CShopBasket::push(SHOP_GOODS_PACK* pGood)
{
	if( m_BasketList.size() >= USE_BASKET_COUNT )
	{
		pop(0);
	}

	m_BasketList.push_back( Basket(pGood) );
}

void CShopBasket::pop(size_t index)
{
	if( index >= m_BasketList.size() )
		return;

	BasketList::iterator it = m_BasketList.begin();

	std::advance( it, index );

	m_BasketList.erase( it );
}

void CShopBasket::make_packet(BUY_BASKET * stBasket) const
{
	// ��ٱ��� ����ü�ν� ����� GoodsID �� BuyType �� �ȿ� ����ֽ��ϴ�.
	memset( stBasket, 0, USE_BASKET_COUNT * sizeof( BUY_BASKET ) );

	for(size_t i = 0; i < m_BasketList.size(); ++i)
	{
		(*stBasket).m_ui32GoodsID = get_sel(i)->_GoodsID;
		(*stBasket).m_ui8BuyType = (UINT8)get_pack_type(i);

#if defined(USE_SALE_COUPON)
		T_ItemDBIdx n64CouponDBIdx = GetCouponIndex(i);

		// GetCouponIndex �޼����, �� ���� -1�� �����Ǳ� ������, 0���� ġȯ���ش�.
		if(n64CouponDBIdx <= 0)
			n64CouponDBIdx = 0;

		(*stBasket).m_ui32CouponID = CInvenList::i()->IsUsedCoupon(n64CouponDBIdx);
		(*stBasket).m_TCouponIdx = n64CouponDBIdx;
#endif
		++stBasket;
	}
}

bool CShopBasket::duplicate(SHOP_GOODS_PACK* pGood) const
{
	INT32 size = pGood->GetGoodsCount();
	for(size_t i = 0; i < m_BasketList.size(); ++i)
	{
		if( m_BasketList[i].pack->GetGoodsCount() != size )
			continue;

		for( INT32 j = 0; j < size; ++j)
		{
			if( *m_BasketList[i].pack->GetGoods( j ) == 
				*pGood->GetGoods( j ) )
			{
				return true;
			}
		}
	}

	return false;
}

bool CShopBasket::is_pack_price_dual(size_t pack)
{

	return (m_BasketList[ pack ].pack->GetGoods(0)->_SaleType
		== GOODS_PRICE_ALL);
}

void CShopBasket::set_pack_dpinx(size_t pack, int dpinx)
{
	m_BasketList[ pack ].dpinx = dpinx;
}

void CShopBasket::set_pack_inx(size_t pack, int packInx)
{
	m_BasketList[ pack ].packInx = packInx;
}

void CShopBasket::set_pack_type(size_t pack, int type)
{
	m_BasketList[ pack ].type = type;
}

SHOP_GOODS* CShopBasket::get_sel(size_t index) const
{
	if( index >= m_BasketList.size() )
		return 0;

	return m_BasketList[ index ].pack->GetGoods( m_BasketList[ index ].packInx );
}

SHOP_GOODS* CShopBasket::get(size_t index, size_t goods) const
{
	if( index >= m_BasketList.size() )
		return 0;

	if( m_BasketList[ index ].pack->GetGoodsCount() <= (INT32)goods )
		return 0;

	return m_BasketList[ index ].pack->GetGoods( goods );
}

SHOP_GOODS_PACK* CShopBasket::operator[](size_t index) const
{
	return m_BasketList[ index ].pack;
}