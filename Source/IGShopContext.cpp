#include "pch.h"
#include "IGShopContext.h"
#include "Shop.h"
#include "UI/UIPhaseShop.h"
#include "UI/UIShopFunction.h"
#include "ui/UIItemInfoUtil.h"
#include "MyRoomInfoContext.h"


I3_CLASS_INSTANCE(IGShopContext);//, i3GameNode);

IGShopContext::IGShopContext() :
m_bRequestOpen(false), m_bEntered(false)
{
	IGShopContext_Mode* mode = new IGShopContext_Mode;		// �⺻ ���� ������ ���� ��ġ..
	MEMDUMP_NEW( mode, sizeof( IGShopContext_Mode));

	m_vecStorage.push_back(mode);
	::memset(m_apContextMode, 0, sizeof(IGShopContext_Mode*) * STAGE_MODE_MAX);
	m_apContextMode[0] = mode;
}

// �������� ��� �����Ҵ絵 �䱸���� �ʱ⶧���� �Ҹ���ó���� Ư���� ����.
IGShopContext::~IGShopContext()
{
	struct fn_del
	{
		void operator()(const IGShopContext_Mode* p)
		{
			I3ASSERT( p != nullptr);
	#if defined( I3_DEBUG)
			NetworkDump_Free( const_cast<IGShopContext_Mode*>(p), __FILE__, __LINE__);
	#endif
			delete p;
		}
	};
	for_each(m_vecStorage.begin(), m_vecStorage.end(), fn_del());
}



void	IGShopContext::Allocate_StageID(UINT32 ui32FieldShopDataFlag)	// 32���̸��� ����������Ʈ������...
{
	{
		const size_t num = m_vecStorage.size();
		for ( size_t i = 1 ; i < num ; ++i )		//1���Ͷ�� ���� ����...
		{
			I3_SAFE_DELETE( m_vecStorage[i]);
		}
		m_vecStorage.resize(1);		// ������ ���� �������� ����...
	}

	for (INT32 i = 1; i < STAGE_MODE_MAX ; ++i)	//1���Ͷ�� ���� ����...
	{
		if( ( ui32FieldShopDataFlag & (0x01 << (i-1)) ) != 0 )	// �ش� ��� �������� ������
		{
			IGShopContext_Mode* mode = new IGShopContext_Mode;
			MEMDUMP_NEW( mode, sizeof( IGShopContext_Mode));

			m_vecStorage.push_back(mode);
			m_apContextMode[i] = mode;
		}
		else							// �⺻ �������� ������
		{
			m_apContextMode[i] = m_apContextMode[0];		
		}
	}
}

void	IGShopContext::SetMainShopChange()
{
	const size_t num = m_vecStorage.size();
	for (size_t i = 0 ; i < num ; ++i)
		m_vecStorage[i]->SetMainShopChange();
}

bool	IGShopContext::IsMainShopChange() const
{
	return m_apContextMode[m_curr_stage_type]->IsMainShopChange();
}

void	IGShopContext::UpdateGoodsList()
{
	return m_apContextMode[m_curr_stage_type]->UpdateGoodsList();
}

void	IGShopContext::ReceiveAndUpdateGoodsList(const FieldShopInfo& fsgi)
{
	//IGShopContext_Mode* mode = m_apContextMode[fsgi.m_ui8StageType];
	IGShopContext_Mode* mode = m_apContextMode[0];
	mode->ReceiveAndUpdateGoodsList(fsgi);
}

void	IGShopContext::SetEvent_GetInfo()
{
	GameEventSender::i()->SetEvent( EVENT_GETINFO_IGSHOP );
}

void	IGShopContext::SetEventOnce_Open()
{
	m_bRequestOpen = true;		// �ϴ� �տ� ���°� ���ڴ�.
	
#ifndef JuuL_Test_Code
	m_curr_stage_type = MyRoomInfoContext::i()->getStage()->GetStageMode();
	m_curr_stage_type = STAGE_MODE_NA; //�׽�Ʈ �ڵ�
#endif

	UINT32 ver = m_apContextMode[m_curr_stage_type]->GetVersion();	  
	GameEventSender::i()->SetEventOnce(EVENT_OPEN_IGSHOP, &ver);		// �������� ������ 3�������͸� �����µ�, �ٸ� �ϳ��� gamecontext�� ����ִ�..
}

void	IGShopContext::SetEvent_EquipItem(const ITEM_INFO & item_info)
{
	GameEventSender::i()->SetEvent(EVENT_INGAMESHOP_APPLY_ITEM, &item_info);
}

void	IGShopContext::EnteredServer()
{
	m_bRequestOpen = false;
	m_bEntered = true;
}

void	IGShopContext::LeaveServer()
{
	m_bEntered = false;	
	m_bRequestOpen = false;			// �̷� ��쵵 �����Ѵ�..(���Խõ���ü���� ����õ�...)
	// ������ �ݴ� ������ ���� ��Ʈ�� ��Ŷ�� �������� �ʱ� ������ �̰����� ����..

}

INT32				IGShopContext::GetNumCategory() const
{
	return m_apContextMode[m_curr_stage_type]->GetNumCategory();
}

const TTCHAR*			IGShopContext::GetCategoryString(INT32 idx) const
{
	return m_apContextMode[m_curr_stage_type]->GetCategoryString(idx);
}

INT32				IGShopContext::GetNumShopGoodsPack(INT32 cat) const
{
	return m_apContextMode[m_curr_stage_type]->GetNumShopGoodsPack(cat);
}

SHOP_GOODS_PACK*	IGShopContext::GetShopGoodsPack( INT32 cat, INT32 idx) const
{
	return m_apContextMode[m_curr_stage_type]->GetShopGoodsPack(cat, idx);
}


////////////////////

IGShopContext_Mode::IGShopContext_Mode() : m_num_category(0), m_bChangeMainShopContext(false) 
{	
	::memset(m_num_items, 0, sizeof(INT32)* MAX_FIELDSHOP_CATEGORYCOUNT);
	::memset(m_pstrCategoryName, 0, sizeof(TTCHAR*) * MAX_FIELDSHOP_CATEGORYCOUNT);
	::memset(m_aapGoodsPack, 0, sizeof(SHOP_GOODS_PACK*) * MAX_FIELDSHOP_CATEGORYCOUNT * MAX_FIELDSHOP_GOODSLISTCOUNT );
}

IGShopContext_Mode::~IGShopContext_Mode()		// ����ó���� ���� �ʴ´�..
{
	IGShopContext_Mode::ClearInfo();
}

void	IGShopContext_Mode::ClearInfo()
{
	const INT32 num_cat = m_num_category;

	for (INT32 i = 0 ; i < num_cat ; ++i )
	{
		const INT32 num_pack = m_num_items[i];
		
		for (INT32 j = 0 ; j < num_pack; ++j )
		{
			SHOP_GOODS_PACK*& pack = m_aapGoodsPack[i][j];
			I3_SAFE_DELETE( pack);
		}

		m_pstrCategoryName[i] = nullptr;
		m_num_items[i] = 0;
	}
	
	m_num_category = 0;
}


void	IGShopContext_Mode::ReceiveAndUpdateGoodsList(const FieldShopInfo& fsgi)
{
	m_info_from_server = fsgi;
	IGShopContext_Mode::UpdateGoodsList();
}

void	IGShopContext_Mode::UpdateGoodsList()
{
	IGShopContext_Mode::ClearInfo();

	for (INT32 i = 0; i < MAX_FIELDSHOP_CATEGORYCOUNT ; ++i)				// ��ȿ�Ѱ͸� �޾Ƶ��δ�...
	{
		Category* cat = m_info_from_server.m_CategoryList + i;

		for (INT32 j = 0 ; j < MAX_FIELDSHOP_GOODSLISTCOUNT ; ++j)
		{
			IGShopContext_Mode::BuildShopGoodsPack( cat->m_ui32GoodsIDList[j] );
		}

		const INT32 num_curr_items = m_num_items[ m_num_category ];

		if ( num_curr_items > 0)		// ��ȿ�ϴٰ� �Ǵ�...�� ��쿡�� ī�װ��� ��ȿ�ϰ� ����...
		{
			m_pstrCategoryName[m_num_category] = cat->m_strCategoryName;
			++m_num_category;
		}

	}
	
	m_bChangeMainShopContext = false;	//

}


void	IGShopContext_Mode::BuildShopGoodsPack(UINT32* goodsIDs)
{
	CShop* shop = CShop::i(); 
	
	for ( INT32 i = 0 ; i < MAX_FIELDSHOP_GOODSPERIODCOUNT; ++i)
	{
		UINT32 goodsID = goodsIDs[i];

		SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(goodsID);		// �����׸�..
		if( pGoodsList == nullptr ) continue;

		//pGoodsList->_ui8ItemCount == 1 �� ��츸 ��ȿ�ϴ�.
		if( pGoodsList->_ui8ItemCount == 0 ) continue; //�̷� ���� ��������...
		if( pGoodsList->_ui8ItemCount > 1) // ��Ʈ �������� �����մϴ�..
		{
			I3TRACE("[INGAME SHOP] Can't Insert Set Item Because Of Shop Convention [Set Item] (GOODS_ID: %d)\n", goodsID);
			continue;
		}

		SHOP_GOODS_TYPE t = SHOP_GOODS_UNKNOWN;

		T_ItemID itemId =  pGoodsList->_ppItemData[0]->_ui32ItemID;
		switch( item_def::get_category(itemId) )
		{
		case item_def::CTG_WEAPON: t = SHOP_WEAPON_GOODS; break;
		case item_def::CTG_CHARA: t = SHOP_CHARACTER_GOODS; break;
		case item_def::CTG_PARTS: t = SHOP_CHARACTER_GOODS; break;
		case item_def::CTG_SKIN : t = SHOP_CHARACTER_GOODS; break;
		case item_def::CTG_ITEM:
			{
				UIShop_Item_Category cat = usf::GetItemItemCategory(itemId);
				if (cat != UISHOP_ITEM_ENHANCED && cat != UISHOP_ITEM_COMPENSATION)
				{
					I3TRACE("[INGAME SHOP] Can't Insert Item Because Of Shop Convention (ITEM_ID: %d, GOODS_ID: %d)\n", itemId, goodsID);
					continue;
				}

				t = SHOP_CASH_ITEM_GOODS; 
			}
		break;
		}

		IGShopContext_Mode::BuildShopGoodsPack( t, goodsID, ( i > 0) );
	}
}


void	IGShopContext_Mode::BuildShopGoodsPack(SHOP_GOODS_TYPE type, UINT32 AddGoodsID, bool bHint)
{
	if (type == SHOP_GOODS_UNKNOWN)
		return;
	
	SHOP_GOODS_PACK * pFindGoodsPack = nullptr;

	SHOP_GOODS_PACK** ppPacks = m_aapGoodsPack[m_num_category];

	const INT32	 curr_num_item = m_num_items[m_num_category];
	
	if ( bHint && curr_num_item > 0)
	{
		SHOP_GOODS_PACK* pGoodsPack = ppPacks[ curr_num_item - 1];			// ���� �ֱ� ��..
		
		if (IGShopContext_Mode::IsExistedPack(type, pGoodsPack, AddGoodsID) )
		{
			pFindGoodsPack = pGoodsPack;
		}
	}
	
	if (pFindGoodsPack == nullptr)			// �Ⱓ�ε������� �����ѵ��� ��ġ�� ����� ó��...
	{
		for (INT32 i = 0 ; i < curr_num_item ; ++i )
		{
			SHOP_GOODS_PACK* pGoodsPack = ppPacks[i];
			
			if ( IGShopContext_Mode::IsExistedPack(type, pGoodsPack, AddGoodsID) )
			{
				pFindGoodsPack = pGoodsPack;
				break;
			}
		}
	}

	if ( pFindGoodsPack == nullptr)
	{
		if ( curr_num_item == MAX_FIELDSHOP_GOODSLISTCOUNT)		// �� ��� ��ġ�� ������ �ߴ��ؾ��Ѵ�..
			return;
		
		pFindGoodsPack = new SHOP_GOODS_PACK; 
		MEMDUMP_NEW( pFindGoodsPack, sizeof( SHOP_GOODS_PACK));

		ppPacks[curr_num_item] = pFindGoodsPack;

		++m_num_items[m_num_category];
	}

	UINT32  nAddGameItemId = 0xFFFFFFFF;
	UINT8	nAuthType = 0;
	UINT32	nAuthArg = 0;		//  _AuthType�� 1�� ��� ��, 2�� ��� ���� 
	UINT8	nSaleMark = GOODS_MARK_NO;	// GOODS_MARK_NO, GOODS_MARK_NEW, GOODS_MARK_HOT
	UINT8	nSaleDesignation = DESIGNATION_TYPE_NONE;
	UINT8	nSaleType = GOODS_SALE_EVENT;

	CShop* shop = CShop::i(); 
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(AddGoodsID);

	if (pGoodsList)
	{
		nAddGameItemId =  pGoodsList->_ppItemData[0]->_ui32ItemID;
		nAuthType = pGoodsList->_ppItemData[0]->_ui8AuthType;
		nAuthArg = pGoodsList->_pui32ItemArg[0];
		nSaleMark = pGoodsList->_pGoodsData->GetMark();
		nSaleDesignation = pGoodsList->_ppItemData[0]->_ui8Restriction;
		nSaleType = pGoodsList->_pGoodsData->_ui8SaleType;
	}

	//	����
	GOODS_PRICE_INFO PriceInfo[PRICE_TYPE_MAX];
	i3mem::FillZero(PriceInfo, sizeof(PriceInfo));
	shop->GetGoodsPrice(AddGoodsID, PriceInfo);

	//	��ǰ ��Ͽ� ä�� �ִ´�.
	SHOP_GOODS * pNewGoods = (SHOP_GOODS*) i3MemAlloc(sizeof(SHOP_GOODS));
	i3mem::FillZero(pNewGoods, sizeof(SHOP_GOODS));

	pNewGoods->_GoodsID		= AddGoodsID;
	pNewGoods->_ItemID		= nAddGameItemId;
	pNewGoods->_AuthType	= nAuthType;
	pNewGoods->_AuthArg		= nAuthArg;
	pNewGoods->_SaleDesignation = nSaleDesignation;
	pNewGoods->_SaleType	= nSaleType;
	i3mem::Copy(pNewGoods->_PriceInfo, PriceInfo, sizeof(PriceInfo));

	//	��ǰ Pack�� ��ǰ �߰�	
	if ( ! pFindGoodsPack->AddGoods(pNewGoods) )
	{
		I3PRINTLOG(I3LOG_NOTICE,"Failed to AddGoods. %d", pNewGoods->_GoodsID);
	}		
}

bool	IGShopContext_Mode::IsExistedPack( SHOP_GOODS_TYPE type, SHOP_GOODS_PACK* test_pack,	UINT32 AddGoodsID)
{
	if (test_pack == nullptr) return false;			// ������ �̷� ���� �߻����� ���� ���̴�...

	SHOP_GOODS* pGoods = test_pack->GetGoods(0);
	if (pGoods == nullptr) return false;

	UINT32 nFindGameItemId = pGoods->_ItemID;
	UINT32 FindGoodsID = pGoods->_GoodsID;
	
	UINT32  nAddGameItemId = 0xFFFFFFFF;

	CShop* shop = CShop::i(); 

	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(AddGoodsID);
	if (pGoodsList)
	{
		nAddGameItemId =  pGoodsList->_ppItemData[0]->_ui32ItemID;
	}

	switch (type)
	{
	case SHOP_CASH_ITEM_GOODS:
		{
			//���� Ÿ��, ĳ�� Ÿ���� ������ ��ģ��.
			INT32 nGroupOnFindGoods = nFindGameItemId - (GET_ITEM_SUBTYPE(nFindGameItemId) * 1000);
			INT32 nGroupOnAddGoods = nAddGameItemId - (GET_ITEM_SUBTYPE(nAddGameItemId) * 1000);;

			if ( nGroupOnFindGoods != 0 && nGroupOnAddGoods != 0 && nGroupOnFindGoods == nGroupOnAddGoods)
				return true;
		}
		break;

	case SHOP_PACKAGE_GOODS:
		{
			SHOP_GOODS_LIST * pFindGoodsList = shop->GetGoodsListData(FindGoodsID);
			SHOP_GOODS_LIST * pAddGoodsList = pGoodsList;			//shop->GetGoodsListData(AddGoodsID);	// �ߺ�..

			if (pFindGoodsList && pAddGoodsList)
			{
				if (pFindGoodsList->_ui8ItemCount == pAddGoodsList->_ui8ItemCount)
				{						
					bool isSame = true;

					INT32 ItemCount = pFindGoodsList->_ui8ItemCount;
					for( INT32 k = 0; k < ItemCount ; k++)
					{
						INT32 findItemId = pFindGoodsList->_ppItemData[k]->_ui32ItemID;
						INT32 addItemId = pAddGoodsList->_ppItemData[k]->_ui32ItemID;
						if (findItemId == addItemId) continue;

						ITEM_TYPE find_item_type = (ITEM_TYPE)GET_ITEM_TYPE(findItemId);
						ITEM_TYPE add_item_type = (ITEM_TYPE)GET_ITEM_TYPE(addItemId);

						if ((ITEM_TYPE_MAINTENANCE == find_item_type || ITEM_TYPE_WRAP_PERIOD == find_item_type || ITEM_TYPE_WRAP_COUNT == find_item_type) &&
							(ITEM_TYPE_MAINTENANCE == add_item_type || ITEM_TYPE_WRAP_PERIOD == add_item_type || ITEM_TYPE_WRAP_COUNT == add_item_type))
						{
							INT32 nGroupOnFindGoods = findItemId - (GET_ITEM_SUBTYPE(findItemId) * 1000);
							INT32 nGroupOnAddGoods = addItemId - (GET_ITEM_SUBTYPE(addItemId) * 1000);

							//	ĳ�� �������� ��� �׷� idx�� ��ǰ pack�� �������� �Ѵ�.
							if( nGroupOnFindGoods == 0 || nGroupOnAddGoods == 0 ||
								nGroupOnFindGoods != nGroupOnAddGoods)
							{
								isSame = false;
								break;
							}
						}
						else
						{
							isSame = false;
							break;
						}
					}

					if (isSame)
						return true;
				}
			}
		}
		break;

	default:
		if (nFindGameItemId == nAddGameItemId)
			return true;
		break;
	}

	return false;
			
}
