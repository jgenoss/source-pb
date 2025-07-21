#include "pch.h"
#include "ShopContext.h"

#include "GlobalVariables.h"
#include "ShopDef.h"
#include "EquipShapeManager.h"
#include "Shop.h"
#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"
#include "UI/UIPhaseShop.h"
#include "UI/UIPhaseGiftShop.h"
#include "UI/UIPhaseBase.h"
#include "UI/UIItemInfoUtil.h"
#include "i3Base/itl/algorithm/sort.h"


#include "UserInfoContext.h"
#include "MedalManager.h"
#include "UI/UICharaInfoMgr.h"

#if legacy_gui_function
	#include "GuiNotifyReceiver.h"
#endif


/*	��ǰ ����Ʈ ���� (komet)

	��ǰ �����ʹ� �������� �������� �����Ŷ� ������ �ȵ�����.
	�������� �޾ƿ� ��ǰ���� �������� �����ֱ����� ShopContext ����Ʈ(��ǰ��)�� �����Ѵ�. 	
	���������� ShopContext�� ��ǰ ����Ʈ�� �����Ͽ� ��ǰ���� ����ϰ� �ȴ�.
	������ ���� BuildGoodsList() �Լ��� ȣ��Ǽ� ��ǰ ����Ʈ�� ������.

	Server  ->	[UserInfoContext]  -> BuildGoodsList() -> [ShopContext]		��)

				Goods (1-1)							   GoodsPack(1)		: ����ġ 130% ���� (SHOP_GOODS_PACK)
				Goods (1-2)								- Goods (1-1)	: ����ġ 130% ���� 1��  (SHOP_GOODS)
				Goods (1-3)								- Goods (1-2)	: ����ġ 130% ���� 7��  (SHOP_GOODS)	
				Goods (1-4)								- Goods (1-3)	: ����ġ 130% ���� 30�� (SHOP_GOODS)	
				Goods (2-1)								- Goods (1-4)	: ����ġ 130% ���� 90�� (SHOP_GOODS)
				Goods (2-2)							   GoodsPack(2)
				Goods (2-3)								- Goods (2-1)
				Goods (2-4)								- Goods (2-2)
					:									- Goods (2-3)
					:									- Goods (2-4)
															:
*/


//	���� ��ǰ ���� �켱���� (���� ���氪�� ���� ���� ����˴ϴ�. ���ڰ� �������� �켱������ ����.)
#define SORT_WEAPON_ASSAULT			1
#define SORT_WEAPON_SMG				2
#define SORT_WEAPON_SNIPER			3
#define SORT_WEAPON_SHOTGUN			4
#define SORT_WEAPON_MACHINEGUN		5
#define SORT_WEAPON_SECONDARY		6
#define SORT_WEAPON_MELEE			7
#define SORT_WEAPON_THROWING		8
#define SORT_WEAPON_SPECIAL			9

//	ĳ���� ��ǰ ���� �켱���� (���� ���氪�� ���� ���� ����˴ϴ�.)
#define SORT_CHARA_CHARACTER		1
#define SORT_CHARA_EQUIP			2

//	������ ��ǰ ���� �켱���� (���� ���氪�� ���� ���� ����˴ϴ�.)
#define SORT_ITEM_MAINTENANCE		1
#define SORT_ITEM_EXPENDABLES		2
#define SORT_ITEM_CARD				3

INT32 __SortBySaleMark( const SHOP_GOODS_PACK * pGoodsPack, INT32 sortPriority)
{
	INT32 i;
	I3ASSERT( pGoodsPack != nullptr);

	INT32 iGoodsCount = pGoodsPack->GetGoodsCount();

	for( i = 0; i < iGoodsCount; i++)
	{
		SHOP_GOODS * pGoods = pGoodsPack->GetGoods( i);
		I3ASSERT( pGoods != nullptr);

		switch( pGoods->GetSaleMark(PRICE_TYPE_SHOP_REAL) )
		{
		case GOODS_MARK_HOT:		sortPriority -= 10310;		return sortPriority;	
		case GOODS_MARK_NEW:		sortPriority -= 10210;		return sortPriority;		
		case GOODS_MARK_EVENT:		sortPriority -= 10110;		return sortPriority;
		case GOODS_MARK_SALE:		sortPriority -= 10010;		return sortPriority;
		}
	}

	return sortPriority;
}

INT32 __CompareBySaleMark( const SHOP_GOODS * pShopGoodsSrc, SHOP_GOODS * pShopGoodsDest)
{
	I3ASSERT( pShopGoodsSrc != nullptr);
	I3ASSERT( pShopGoodsDest != nullptr);

	switch( pShopGoodsDest->GetSaleMark(PRICE_TYPE_SHOP_REAL) )
	{
	case GOODS_MARK_NO :				return -1;
	case GOODS_MARK_HOT :
		{
			switch( pShopGoodsSrc->GetSaleMark(PRICE_TYPE_SHOP_REAL) )
			{
			case GOODS_MARK_NO :		return 1;		// �Ϲ� ��ǰ�� HOT�̶�� �Ϲݻ�ǰ �켱
			case GOODS_MARK_HOT :		return -1;		// ��Ʈ��ǰ�� �켱
			case GOODS_MARK_NEW :		return 1;		// ��Ʈ��ǰ�� New�� �Ϲ� ��ǰ�� HOT�̶�� �Ϲݻ�ǰ �켱
			default :					return 1;
			}
		}
		break;

	case GOODS_MARK_NEW :
		{
			switch( pShopGoodsSrc->GetSaleMark(PRICE_TYPE_SHOP_REAL) )
			{
			case GOODS_MARK_NO :		return 1;		// �Ϲ� ��ǰ�� NEW��� �Ϲݻ�ǰ �켱
			case GOODS_MARK_HOT :		return -1;		// ��Ʈ��ǰ�� �켱
			case GOODS_MARK_NEW :		return -1;		// ��Ʈ��ǰ�� �켱
			default :					return 1;
			}
		}
		break;
	}

	return 0;
}

//	��ǰ�� ID�� �̿��Ͽ� �������� ���� (�� �Լ�)
bool _GoodsCompareFuncById(const SHOP_GOODS_PACK * p1, const SHOP_GOODS_PACK *p2)
{
	INT32 i;
	INT32 sortPriority[2];
	const SHOP_GOODS_PACK * pGoods[2] = { p1, p2 };
	SHOP_GOODS * pShopGoods[2] = {  pGoods[0]->GetGoods(0)  , pGoods[1]->GetGoods(0)  } ;

	if( pShopGoods[0] == nullptr || pShopGoods[1] == nullptr)	return false;
	
	sortPriority[ 0] = 0;
	sortPriority[ 1] = 0;
	
	bool IsPackageGoods[2] = { false, };
	CShop* shop = CShop::i();


	for ( i = 0 ; i < 2 ; ++ i )
	{
		IsPackageGoods[i] = shop->IsPackageGoods(pShopGoods[i]->_GoodsID);
	}


	if (IsPackageGoods[0] && IsPackageGoods[1])
	{
		sortPriority[0] = (INT32)pShopGoods[0]->_GoodsID;
		sortPriority[1] = (INT32)pShopGoods[1]->_GoodsID;

		sortPriority[ 0] = __SortBySaleMark( pGoods[ 0], sortPriority[ 0]);
		sortPriority[ 1] = __SortBySaleMark( pGoods[ 1], sortPriority[ 1]);

		return sortPriority[0] < sortPriority[1] ;
	}
	else if (IsPackageGoods[0])
	{
		__SortBySaleMark( pGoods[ 0], sortPriority[ 0]);
		return __CompareBySaleMark( pShopGoods[0], pShopGoods[1] ) < 0;
		// return -1;
	}
	else if (IsPackageGoods[1])
	{
		__SortBySaleMark( pGoods[ 1], sortPriority[ 1]);
		return __CompareBySaleMark( pShopGoods[1], pShopGoods[0] ) < 0;
		//return 1;
	}


	for( i=0; i<2; i++)
	{
		switch( GET_ITEM_TYPE(pShopGoods[i]->_ItemID))
		{
		case ITEM_TYPE_PRIMARY:
			{
			switch( WEAPON::ItemID2Class( pShopGoods[i]->_ItemID))
				{
				case WEAPON_CLASS_ASSAULT:		sortPriority[i] = SORT_WEAPON_ASSAULT;		break;
				case WEAPON_CLASS_SMG:			sortPriority[i] = SORT_WEAPON_SMG;			break;
				case WEAPON_CLASS_SNIPER:		sortPriority[i] = SORT_WEAPON_SNIPER;		break;
				case WEAPON_CLASS_SHOTGUN:		sortPriority[i] = SORT_WEAPON_SHOTGUN;		break;
				case WEAPON_CLASS_MG:			sortPriority[i] = SORT_WEAPON_MACHINEGUN;	break;
				}
			}
			break;
		case ITEM_TYPE_SECONDARY:		sortPriority[i] = SORT_WEAPON_SECONDARY;	break;
		case ITEM_TYPE_MELEE:			sortPriority[i] = SORT_WEAPON_MELEE;		break;
		case ITEM_TYPE_THROWING1:		sortPriority[i] = SORT_WEAPON_THROWING;		break;
		case ITEM_TYPE_THROWING2:		sortPriority[i] = SORT_WEAPON_SPECIAL;		break;
		case ITEM_TYPE_CHARA:			sortPriority[i] = SORT_CHARA_CHARACTER;		break;
		case ITEM_TYPE_HEADGEAR:		sortPriority[i] = SORT_CHARA_EQUIP;			break;
		case ITEM_TYPE_MAINTENANCE:		sortPriority[i] = SORT_ITEM_MAINTENANCE;	break;
		case ITEM_TYPE_WRAP_PERIOD:		sortPriority[i] = SORT_ITEM_EXPENDABLES;	break;
		case ITEM_TYPE_WRAP_COUNT:		sortPriority[i] = SORT_ITEM_EXPENDABLES;	break;
		case ITEM_TYPE_TEMP:			sortPriority[i] = SORT_ITEM_CARD;			break;
		}

		sortPriority[ i] = __SortBySaleMark( pGoods[ i], sortPriority[ i]);
	}

	
	//	���ʿ� �켱������ ������ ���������� �����Ѵ�. (�켱���� ����)
	if(sortPriority[0] == sortPriority[1])
	{
		return pShopGoods[0]->_ItemID < pShopGoods[1]->_ItemID;
	}

	return sortPriority[0] < sortPriority[1];
}


//	��ǰ �������� ���� (�� �Լ�) 
bool _GoodsCompareFuncOnPack(const SHOP_GOODS* pGoods1, const SHOP_GOODS* pGoods2)
{

	INT32 goodsClass = GET_ITEM_TYPE(pGoods1->_ItemID);

	switch((ITEM_TYPE) goodsClass)
	{
	case ITEM_TYPE_MAINTENANCE:		//	���Ӽ� ������
	case ITEM_TYPE_WRAP_PERIOD:		//	�Ҹ� ������(�Ⱓ���������� ������)
	case ITEM_TYPE_WRAP_COUNT:		//	�Ҹ� ������
		{
			// ��ǰ���������� ���Ӽ��������̶� �ص� ���Ǹ� ���Ӽ����� �ٲ����� ������ �ȵȰ��� �Ҹ� 1���� ��޵ǰ� �ִ�.
			// ���� �ȵ� ���Ӽ� �������� ��� arg���� �Ⱓ�� �ƴ� ������ ������ 1���� �� �ִ�.
			// �׷��Ƿ� ���Ӽ� �������� ��� ���� �Ⱓ�� �����;��� �������� ������ �� �� �ִ�.


			ITEM_TYPE itemUsage1 = (ITEM_TYPE) GET_ITEM_TYPE(pGoods1->_ItemID);
			ITEM_TYPE itemUsage2 = (ITEM_TYPE) GET_ITEM_TYPE(pGoods2->_ItemID);

			//	���Ӽ� �������� �ƴϴ�
			if( itemUsage1 != ITEM_TYPE_MAINTENANCE || itemUsage2 != ITEM_TYPE_MAINTENANCE)
				break;

			return pGoods1->_ItemID < pGoods2->_ItemID;
		}
		break;		
	}	

	//	�������� ���
	if( pGoods1->_AuthArg == pGoods2->_AuthArg)
	{	//	������ID�� ���� �������� ����
		return pGoods1->_ItemID < pGoods2->_ItemID;
	}
	else
	{	//	��ǰ�� ���Ⱓ�� ���� �������� ����
		return pGoods1->_AuthArg < pGoods2->_AuthArg;
	}
}

//	��ǰ�� Sale type�� ������������ �����մϴ�.
bool _GoodsSaleTypeSort(const SHOP_GOODS* pGoods1, const SHOP_GOODS* pGoods2)
{
	return pGoods1->_SaleType < pGoods2->_SaleType;
}

namespace
{
	bool g_bBuildGoodsList = false;

	::SHOP_GOODS_TYPE convert_type(item_def::Category category)
	{
		switch(category)
		{
		case item_def::CTG_WEAPON:	return ::SHOP_WEAPON_GOODS;
		case item_def::CTG_CHARA:	return ::SHOP_CHARACTER_GOODS;
		case item_def::CTG_PARTS:	return ::SHOP_CHARACTER_GOODS;
		case item_def::CTG_SKIN:	return ::SHOP_SKIN_GOODS;
		case item_def::CTG_ITEM:	return ::SHOP_CASH_ITEM_GOODS;
		}
		return ::SHOP_GOODS_MAX;
	}
	::SHOP_GOODS_TYPE convert_type( T_ItemID itemID)
	{
		return convert_type( item_def::get_category(itemID) );
	}
}

CShopContext::CShopContext() : 	m_nCapsuleItemIdx(0), m_bisBuildGoodsList(false)//revision 53188
{	
	m_nUserSelectedGoodsId = 0xFFFFFFFF;	

	m_bEventLock = false;
	m_pSelectedGoods = nullptr;
	m_nGameEvent = EVENT_NOTHING;
	
	m_bChangedChara = false;
	m_bChangedWeapon = false;
	m_ItemTagType = SHOP_ITEM_TYPE_NORMAL;
}

CShopContext::~CShopContext()
{
	ClearGoodsList();
	m_tagPriorities.clear();
}

void CShopContext::EnterServer(void)
{
	m_bInServer = true;

	m_pSelectedGoods = nullptr;
	m_bChangedChara = false;
	m_bChangedWeapon = false;
}

void CShopContext::LeaveServer(void)
{
	//ClearGoodsList();

	INT32 nArg = 0;

	if( m_bChangedChara)	//	�������� ĳ���͸� ������ ����ߴ�. (���� ���Խ��� ĳ���Ϳ� �����)
	{
		nArg |= 0x1;
		m_bChangedChara = false;
	}

	if( m_bChangedWeapon)	//	�������� ���⸦ ������ ����ߴ�. (���� ���Խ��� ����� �����)
	{
		nArg |= 0x2;
		m_bChangedWeapon = false;
	}

	GameEventSender::i()->SetEventOnce(EVENT_LEAVE_SHOP, &nArg);
	CharaInfoMgr::instance()->send_event_change_equip();

	m_bInServer = false;
}

void CShopContext::BuildGoodsList(void)
{
	//�ѹ� �� ��ǰ ���� �����ϵ��� ����
	if( m_bisBuildGoodsList )	// revision 53188
		return;

	// ������ ������ ���� �޴� ������ �ʿ��ѵ� �� �Լ��� ȣ��� ���������� �޴� ������ ������� ���� ����.
	// ���Ƿ� �ѹ� ȣ���ϰ� �մϴ�. �Ʒ� �Լ��� ���� ����ǰ� BuildGoodsList �� �ٽ� ����Ǹ� �ʹ� ������.
	CMedalManager::i()->MakeViewList(TAB_BASIC);

	//	��ǰ �����ʹ� �������� �������� �����Ŷ� ������ �ȵ�����.
	//	�������� �޾ƿ� ��ǰ���� �������� �����ֱ� ���� �ٽ� ����Ʈ�� �����Ѵ�. 	
	ClearGoodsList();


	CShop* shop = CShop::i();
	INT32 nGoodsCount = shop->GetGoodsCount();

	//	�����κ��� �޾ƿ� ��ǰ���� ������ �ٽ� ����Ʈ�� �����ϵ���
	for( INT32 index = 0; index < nGoodsCount ; index++)
	{
		UINT32 ui32GoodsID = shop->GetGoodsDataByIndex(index)->_ui32GoodsID;
		SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(ui32GoodsID);

		T_ItemID itemId = 0xFFFFFFFF;

		//	��ǰ ��Ű���� ������ǰ�� ������ (��Ʈ ��ǰ)
		if (1 < pGoodsList->_ui8ItemCount)
		{
			_AddGoodsToShopList(SHOP_PACKAGE_GOODS, ui32GoodsID);
			continue;
		}
		//	��ǰ ��Ű���� ������ǰ�� �ϳ� (���� ��ǰ)
		else if (1 == pGoodsList->_ui8ItemCount)
		{
			itemId		= pGoodsList->_ppItemData[0]->_ui32ItemID;
		}

		// ���� ���� ������ -> �޴޷� ���� ����
		CMedalManager::i()->SetItemRestriction( itemId );
		
		if( item_def::get_category(itemId) == item_def::CTG_NA )
		{
			I3TRACE( "[SHOP] ITEM_CLASS_UNKNOWN ��ǰ�� ������ �ֽ��ϴ�. (ITEM_ID: %d, GOODS_ID: %d, FLAG: 0x%x)\n", itemId, ui32GoodsID, 0/*ItemID*/);
			continue;
		}

		//	��ǰ ������ ���� �з��Ͽ� ��ǰ ��Ͽ� �߰��մϴ�.
		switch( item_def::get_category_goodsID( ui32GoodsID ) )
		{
		case item_def::CTG_WEAPON:
			_AddGoodsToShopList(SHOP_WEAPON_GOODS, ui32GoodsID);
			break;
		case item_def::CTG_CHARA:
			{
				UINT32 itemNumber = GET_ITEM_NUMBER(itemId);

				INT32 flag = 0;

				switch(itemNumber)
				{
					//	�� ���� ĳ���� ��ǰ�� ���� ID���θ� ���� �����ϹǷ� ���� ���� ĳ���Ͱ� �⺻ ���޵� ��� ��ǰ ��Ͽ��� ���ϴ�. komet
				case CHARA_RES_ID_TARANTULA:
					{
						flag = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_CHARA, GetCharaTeamType(itemNumber),	itemNumber);
					}
					break;
				case CHARA_RES_ID_SWAT_FEMALE:
					{
						flag = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_CHARA, GetCharaTeamType(itemNumber),	itemNumber);
					}
					break;
					//	�� ���� ĳ���� ��ǰ�� ���� ID���θ� ���� �����ϹǷ� ���� ���� ĳ���Ͱ� �⺻ ���޵� ��� ��ǰ ��Ͽ��� ���ϴ�. komet
				case CHARA_RES_ID_RED_BULLS:
					{
						flag = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_CHARA, GetCharaTeamType(itemNumber),	itemNumber);
					}
					break;
				case CHARA_RES_ID_ACID_POLE:
					{
						flag = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_CHARA, GetCharaTeamType(itemNumber),	itemNumber);
					}
					break;
				}

				_AddGoodsToShopList(SHOP_CHARACTER_GOODS, ui32GoodsID);
			}
			break;
		case item_def::CTG_PARTS:
			_AddGoodsToShopList(SHOP_CHARACTER_GOODS, ui32GoodsID);
			break;
		case item_def::CTG_SKIN:
			_AddGoodsToShopList(SHOP_SKIN_GOODS, ui32GoodsID);
			break;
			//	ĳ�� ������
		case item_def::CTG_ITEM:
			_AddGoodsToShopList(SHOP_CASH_ITEM_GOODS, ui32GoodsID);
			break;
		default:	
			{
				I3TRACE( "[SHOP] ITEM_CLASS_UNKNOWN ��ǰ�� ������ �ֽ��ϴ�. (ITEM_ID: %d, GOODS_ID: %d, FLAG: 0x%x)\n", itemId, ui32GoodsID, 0/*ItemID*/);
				continue;
			}
			break;
		}
	}


	//	��ǰ �켱���� ���� �� SaleType���� ����
	for(INT32 type=0; type<(INT32)SHOP_GOODS_MAX; type++)
	{
		i3::vector<SHOP_GOODS_PACK*>& ShopList = SetShopList((SHOP_GOODS_TYPE) type);

		//	ȭ�鿡 ������ ����Ʈ�� ��ǰ���� �����Ѵ�.
		i3::sort(ShopList.begin(), ShopList.end(), _GoodsCompareFuncById);

		//	In goods pack (��ǰ ���Գ����� ���Ⱓ�� ���� ���� ��ǰ���� �����Ѵ�.)
		for( size_t idx = 0; idx< ShopList.size(); idx++)
		{
			SHOP_GOODS_PACK * pGoodsPack = ShopList[idx];	

			// ������ �Ⱓ�� ���� ����
			pGoodsPack->Sort(_GoodsCompareFuncOnPack);

			// ���ĵ� �Ϳ��� SaleType���� ����
			pGoodsPack->Sort(_GoodsSaleTypeSort);
		}
	}

	m_bisBuildGoodsList = true;	// revision 53188
}

void CShopContext::_AddGoodsToShopList(SHOP_GOODS_TYPE type, UINT32 AddGoodsID)
{
	//	���� ������������ ���Ⱓ�� ���� ���� ��ǰ�� ���� ��� �ϳ��� ��ǰ Pack���� �����Ͽ� ó���մϴ�.
	SHOP_GOODS_PACK * pFindGoodsPack = nullptr;

	i3::vector<SHOP_GOODS_PACK*>& ShopList = SetShopList(type);

	UINT32 nAddGameItemId = 0xFFFFFFFF;
	UINT8 nAuthType = 0;
	UINT32	nAuthArg = 0;				// _AuthType�� 1�� ��� ��, 2�� ��� ����

	//	#define GOODS_MARK_NO					0					// ��ũ ����
	//	#define GOODS_MARK_NEW					1					// New ��ũ
	//	#define GOODS_MARK_HOT					2					// Hot ��ũ
	UINT8	nSaleMark = GOODS_MARK_NO;
	UINT8	nSaleType = GOODS_SALE_EVENT;
	
	//	DESIGNATION_TYPE
	UINT8	nSaleDesignation = DESIGNATION_TYPE_NONE;

	CShop* shop = CShop::i();

	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(AddGoodsID);
	if ( pGoodsList )
	{
		nAddGameItemId = pGoodsList->_ppItemData[0]->_ui32ItemID;
		nAuthType = pGoodsList->_ppItemData[0]->_ui8AuthType;
		nAuthArg = pGoodsList->_pui32ItemArg[0];
		nSaleMark = pGoodsList->_pGoodsData->GetMark();
		nSaleType = pGoodsList->_pGoodsData->_ui8SaleType;

		nSaleDesignation = pGoodsList->_ppItemData[0]->_ui8Restriction;
	}

	//	��ǰ ����Ʈ���� �߰��� ��ǰ�� ���� ��ǰ Pack�� ã�´�. (��. ���� �������� 1��, 7�� 30�� ���� ��ǰ�� �ϳ��� ��ǰpack���� ���� ����)
	for( size_t i = 0; i < ShopList.size() ; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = ShopList[i];
		if( pGoodsPack == nullptr) continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == nullptr) continue;

		UINT32 nFindGameItemId = pGoods->_ItemID;
		UINT32 FindGoodsID = pGoods->_GoodsID;

		//GOODS_TYPE �� ���ƾ��� ��ģ��.
		ITEM_TYPE find_goods_type = (ITEM_TYPE)GET_GOODS_FLAG_TYPE(FindGoodsID);		if(find_goods_type == ITEM_TYPE_UNKNOWN) continue;
		ITEM_TYPE add_goods_type = (ITEM_TYPE)GET_GOODS_FLAG_TYPE(AddGoodsID);		if(add_goods_type == ITEM_TYPE_UNKNOWN) continue;

		if( find_goods_type != add_goods_type ) continue;

		//	ĳ�� �������� ��� ���� ��ǰ������ GameItemId�� �ٸ��Ƿ� ����ó���Ѵ�. komet
		if( type == SHOP_CASH_ITEM_GOODS)
		{
			//���� Ÿ��, ĳ�� Ÿ���� ������ ��ģ��.
			INT32 nGroupOnFindGoods = nFindGameItemId;
			if (nGroupOnFindGoods == 0) continue;
			INT32 nGroupOnAddGoods = nAddGameItemId;
			if (nGroupOnAddGoods == 0) continue;

			if (GET_ITEM_TYPE(nAddGameItemId) != ITEM_TYPE_WRAP_COUNT)
			{
				nGroupOnFindGoods = nFindGameItemId - (GET_ITEM_SUBTYPE(nFindGameItemId) * 1000);
				nGroupOnAddGoods = nAddGameItemId - (GET_ITEM_SUBTYPE(nAddGameItemId) * 1000);;
			}

			//	ĳ�� �������� ��� �׷� idx�� ��ǰ pack�� �������� �Ѵ�.
			if (nGroupOnFindGoods == nGroupOnAddGoods)
			{
				pFindGoodsPack = pGoodsPack;
				break;
			}
		}
		else if (type == SHOP_PACKAGE_GOODS)
		{
			SHOP_GOODS_LIST * pFindGoodsList = shop->GetGoodsListData(FindGoodsID);
			SHOP_GOODS_LIST * pAddGoodsList = shop->GetGoodsListData(AddGoodsID);

			if( pFindGoodsList == 0 || pAddGoodsList == 0 ) continue;
			if (pFindGoodsList->_ui8ItemCount != pAddGoodsList->_ui8ItemCount) continue;

			bool isSame = true;

			INT32 ItemCount = pFindGoodsList->_ui8ItemCount;
			for( INT32 k = 0; k < ItemCount ; k++)
			{
				INT32 findItemId = pFindGoodsList->_ppItemData[k]->_ui32ItemID;
				INT32 addItemId = pAddGoodsList->_ppItemData[k]->_ui32ItemID;
				if (findItemId == addItemId) continue;

				// Set �������� �ȿ� Goods�� �ִ� ItemID�� �˻�
				// Ÿ������ Cash item �� ��츸 üũ��.
				ITEM_TYPE find_item_type = (ITEM_TYPE) GET_ITEM_TYPE(findItemId);
				ITEM_TYPE add_item_type = (ITEM_TYPE) GET_ITEM_TYPE(addItemId);

				if ((ITEM_TYPE_MAINTENANCE == find_item_type || ITEM_TYPE_WRAP_PERIOD == find_item_type) &&
					(ITEM_TYPE_MAINTENANCE == add_item_type || ITEM_TYPE_WRAP_PERIOD == add_item_type) )
				{
					// Item ID���� SubType�� ���ϴ�. Maintenance�� WrapCount�� ��쿡�� SubType�� �Ⱦ��µ�
					// Wrap Period �� Maintenance�� �������� 17���� SubType�� Day�̱� ������ Day�� ����
					// üũ�� �մϴ�. �� Wrap Period ����� �Ȱ��ٴ� �Ҹ�.
					INT32 nGroupOnFindGoods = findItemId - (GET_ITEM_SUBTYPE(findItemId)*1000);
					INT32 nGroupOnAddGoods = addItemId - (GET_ITEM_SUBTYPE(addItemId)*1000);

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
			{
				pFindGoodsPack = pGoodsPack;
			}
		}
		else //	���⳪ ĳ���ʹ� ��ǰ ���̵�� �ٸ����� GameItemId�� ����. (�����۹׻�ǰ ����Ʈ ���� ����)
		{
			//	�̹� ����� ����Ʈ �ȿ��� ���� ������ ��ǰ Pack�� ã�Ҵ�.
			if(nFindGameItemId == nAddGameItemId)
			{
				pFindGoodsPack = pGoodsPack;
			}
		}
	}

	//	��ǰ ����Ʈ�� �߰��� ��ǰ�� ��ǰ Pack�� ������ ���ο� ��ǰ Pack���� ����
	if( pFindGoodsPack == nullptr)
	{		
		pFindGoodsPack = new SHOP_GOODS_PACK;
		MEMDUMP_NEW( pFindGoodsPack, sizeof(SHOP_GOODS_PACK));

		//	��ǰ ����Ʈ�� ��ǰ Pack �߰�
		ShopList.push_back(pFindGoodsPack);
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
		I3MEM_SAFE_FREE(pNewGoods);
	}
}


void CShopContext::ClearGoodsList(void)
{
	for( INT32 type=0; type<SHOP_GOODS_MAX; type++)
	{
		i3::vector<SHOP_GOODS_PACK*>& ShopList = SetShopList((SHOP_GOODS_TYPE) type);

		//	��ǰ ����Ʈ�� ��ǰ Pack ����
		for( size_t packCnt = 0; packCnt< ShopList.size(); packCnt++)
		{
			SHOP_GOODS_PACK * pGoodsPack = ShopList[packCnt];
				//	��ǰ Pack�ȿ� ��ǰ ����
			I3_SAFE_DELETE( pGoodsPack);
		}

		ShopList.clear();		
	}

	m_bisBuildGoodsList = false; // revision 53188
}

INT32 CShopContext::GetGoodsCount(SHOP_GOODS_TYPE type)
{	 
	//	���� �������� �Ϸ�¥��, ����¥��, �Ѵ�¥���� ���� ��� ��ǰ�� 3���� �Ǵ��Ѵ�.
	I3ASSERT(0 <= type && SHOP_GOODS_MAX > type);

	INT32 count = 0;
	const i3::vector<SHOP_GOODS_PACK*>& ShopList = GetShopList((SHOP_GOODS_TYPE) type);
		
	for(size_t cnt = 0; cnt< ShopList.size(); cnt++)
	{
		SHOP_GOODS_PACK * pGoodsPack = ShopList[cnt];

		count += pGoodsPack->GetGoodsCount();
	}

	return count;
}

INT32 CShopContext::GetGoodsPackCount(SHOP_GOODS_TYPE type)
{
	//	���� �������� �Ϸ�¥��, ����¥��, �Ѵ�¥���� ���� ��� ���� �������̹Ƿ� 1���� �Ǵ��Ѵ�.
	I3ASSERT(0 <= type && SHOP_GOODS_MAX > type);

	return (INT32)GetShopList((SHOP_GOODS_TYPE) type).size();
}

const SHOP_GOODS_PACK * CShopContext::GetGoodsPack(SHOP_GOODS_TYPE type, INT32 index)
{
	I3ASSERT(0 <= type && SHOP_GOODS_MAX > type);	

	const i3::vector<SHOP_GOODS_PACK*>& ShopList = GetShopList((SHOP_GOODS_TYPE) type);

	I3ASSERT(0 <= index && (INT32)ShopList.size() > index);	

	return ShopList[index];
}

bool CShopContext::CheckBuy(UINT32 GoodsID, UINT32 PriceType, UINT8 price_idx, GOODS_PRICE_TYPE tabPrice)
{
	//tabPrice�� ��/�� UI ȣȯ�� ���ؼ� �ʿ��� �Ű������̴�.	
	//�� UI : GOODS_PRICE_ALL
	//�� UI : GOODS_PRICE_ALL, GOODS_PRICE_POINT, GOODS_PRICE_CASH �� ��� �´�.
	if ( !(PriceType & tabPrice) )
		return false;

	//tabPrice ���� ���� ���� ���� ������ Ÿ���� ���Ѵ�.
	//�� UI ���� :	tabPrice == GOODS_PRICE_ALL �� ��� PriceType ���� �����Ѵ�.
	//						tabPrice != GOODS_PRICE_ALL �� ��� tabPrice ���� �����Ѵ�.
	//�� UI ���� :	tabPrice == GOODS_PRICE_ALL �� ��� PriceType ���� �����Ѵ�.
	//						tabPrice != GOODS_PRICE_ALL �� ��� tabPrice ���� �����Ѵ�.
	UINT32 realPriceType = tabPrice;
	if( realPriceType == GOODS_PRICE_ALL )
	{
		realPriceType = PriceType;
		if (realPriceType == GOODS_PRICE_ALL)
			realPriceType = GOODS_PRICE_POINT;
	}

	CShop *	pShop = CShop::i();

	GOODS_PRICE_INFO info;
	i3mem::FillZero(&info, sizeof(GOODS_PRICE_INFO));
	pShop->GetGoodsPrice(GoodsID, info, price_idx);

	switch(realPriceType & info._Type)
	{
	case GOODS_PRICE_POINT	:
		{
			UINT32 MyPoint = UserInfoContext::i()->GetMyPoint();
			if (MyPoint >= info._Point)
				return true;
		}
		break;
	case GOODS_PRICE_CASH	:
		{
			UINT32 MyCash = UserInfoContext::i()->GetMyCash();
			if (MyCash >= info._Cash)
				return true;
		}
		break;

	case GOODS_PRICE_ALL	:	// All�̸� ����Ʈ�� üũ�Ѵ�.
		{
			UINT32 MyPoint = UserInfoContext::i()->GetMyPoint();
			UINT32 MyCash = UserInfoContext::i()->GetMyCash();
			if (MyPoint >= info._Point || MyCash >= info._Cash)
				return true;
		}
		break;
	}

	return false;
}

EQUIP::GOODS CShopContext::EquipGoods(const SHOP_GOODS * pGoods, bool bUpdateViewModel /*= true*/)
{
	EQUIP::GOODS flag = EQUIP::EquipGoods( pGoods, bUpdateViewModel);

	switch( flag)
	{
	case EQUIP::GOODS_CHARA :	m_bChangedChara = true;		break;
	case EQUIP::GOODS_WEAPON :	m_bChangedWeapon = true;	break;
	}

	return flag;
}

void CShopContext::AddTagPriority(const UINT8 prior)
{
	m_tagPriorities.push_back(prior);
}

void CShopContext::SetTagPriorityType(const SHOP_ITEM_TYPE prior)
{
	m_ItemTagType = prior;
	m_tagPrioritiesType = m_tagPriorities;
	for (size_t i = 0; i < m_tagPriorities.size(); i++)
	{
		if (m_tagPrioritiesType[i] < m_tagPrioritiesType[prior])
		{
			m_tagPrioritiesType[i]++;
		}
	}
	m_tagPrioritiesType[prior] = 0;
}

UINT8 CShopContext::GetTagPriority(const SHOP_ITEM_TYPE type) const
{
	if (m_tagPrioritiesType.empty())
		return static_cast<UINT8>(type);

	if (m_ItemTagType == SHOP_ITEM_TYPE_NORMAL)
		return m_tagPriorities.at(type);
	else
		return m_tagPrioritiesType.at(type);
}

/***************************************************/
//Repair
bool CShopContext::IsDurabilityItem( T_ItemID ItemID) const
{
	SHOP_ITEM_DATA * pItemData = CShop::i()->GetItemData(ItemID);
	if ( pItemData )
		return (pItemData->_ui8AuthExpireType == ITEM_EXPIRE_LEAVE);

	return false;
}

/*********//*********/
bool CShopContext::GetDurability( T_ItemID ItemID, UINT32 & Current, UINT32 & Max) const
{
	//Parts�� �ΰ� �̻��� ��� ��ȿ�� ���� ���� �� ��
	return GetDurability(ItemID, 0, Current, Max);
}

bool CShopContext::GetDurability( T_ItemID ItemID, T_ItemDBIdx ware_idx, UINT32 & Current, UINT32 & Max) const
{
	INT32 ItemIdx = -1;

	INVEN::DB_TYPE dbType = INVEN::ItemID2DBType( ItemID);

	if( (dbType == INVEN::DB_WEAPON) || (dbType == INVEN::DB_CHARA) )
		ItemIdx = CInvenList::i()->FindItemIndexByItemID( dbType, ItemID);
	else if( (dbType == INVEN::DB_PARTS) || (dbType == INVEN::DB_SKIN) )
		ItemIdx = CInvenList::i()->FindItemIndexByDBIdx( dbType, ware_idx);

	if( ItemIdx != -1)
		Current	= CInvenList::i()->GetItemArg( dbType, ItemIdx);

	SHOP_ITEM_DATA* pItemData	= CShop::i()->GetItemData(ItemID);
	if ( pItemData == nullptr )	return false;

	Max = pItemData->_ui32Durability;
	if ( Max == 0 ) Max = 100;

	return true;
}

/*********//*********/
bool	CShopContext::GetDurability( T_ItemID ItemID, REAL32& DurableRate ) const
{
	return GetDurability(ItemID, 0, DurableRate);
}

bool	CShopContext::GetDurability( T_ItemID ItemID, T_ItemDBIdx ware_idx, REAL32& DurableRate ) const
{
	UINT32	Current = 100, Max = 100;
	if( GetDurability(ItemID, ware_idx, Current, Max) == false )
		return false;

	DurableRate	= static_cast<REAL32>(Current) / Max;
	DurableRate	= i3Math::Clamp(DurableRate, 0.f, 1.f);

	if( UserInfoContext::i()->IsNewRepair() )
	{
		if( DurableRate > 0.99f && DurableRate < 1.0f )
			DurableRate	= 0.99f;
		else if( DurableRate < 0.01f && DurableRate > 0.0f )
			DurableRate	= 0.01f;
	}

	return true;
}

/*********//*********/
bool	CShopContext::GetRepairPrice( T_ItemID ItemID, UINT32 & Price, GOODS_PRICE_TYPE & PriceType) const
{
	return GetRepairPrice(ItemID, 0, Price, PriceType);
}

bool	CShopContext::GetRepairPrice( T_ItemID ItemID, T_ItemDBIdx ware_idx, UINT32 & Price, GOODS_PRICE_TYPE & PriceType) const
{
	SHOP_ITEM_DATA*	pItemData	= CShop::i()->GetItemData(ItemID);
	if ( pItemData == nullptr )
		return false;

	PriceType = GOODS_PRICE_UNKNOWN;

	if ( pItemData->_ui32RepairCash > 0 )
	{
		PriceType = GOODS_PRICE_CASH;
	}
	else if ( pItemData->_ui32RepairPoint > 0 )
	{
		PriceType = GOODS_PRICE_POINT;
	}
	else
	{
		return false;
	}

	if( UserInfoContext::i()->IsNewRepair() )
	{
		REAL32 DurableRate	= 1.0f;
		if( GetDurability( ItemID, ware_idx, DurableRate ) == false )
			return false;

		REAL32		RepairRate	= 1.0f - DurableRate;
		REAL32		Weight		= 1.0f;

		switch (PriceType)
		{
		case GOODS_PRICE_CASH:	Price	= pItemData->_ui32RepairCash;		break;
		case GOODS_PRICE_POINT:	Price	= pItemData->_ui32RepairPoint;		break;
		}

		if( RepairRate >= 0.9f )		Weight	= 1.25f;
		else if( RepairRate >= 0.5f )	Weight	= 1.1f;

		Price	= static_cast<UINT32>( Price * RepairRate * Weight );
	}
	else
	{
		UINT32 CurrnetDurability = 0, MaxDurability = 0;

		if( GetDurability(ItemID, ware_idx, CurrnetDurability, MaxDurability) == false )
			return false;

		INT32	RepairedDurability = MaxDurability - CurrnetDurability;
		I3ASSERT(RepairedDurability >= 0);
		if ( RepairedDurability < 0 )
			RepairedDurability = 0;

		switch (PriceType)
		{
		case GOODS_PRICE_CASH:	Price	= RepairedDurability * pItemData->_ui32RepairCash;	break;
		case GOODS_PRICE_POINT:	Price	= RepairedDurability * pItemData->_ui32RepairPoint;	break;
		}
	}

	return true;
}

/*********//*********/
REPAIR_TYPE	CShopContext::CheckCanRepair( T_ItemID ItemID) const
{
	return CheckCanRepair(ItemID, 0);
}

REPAIR_TYPE	CShopContext::CheckCanRepair( T_ItemID ItemID, T_ItemDBIdx ware_idx) const
{
	UINT32				Price		= 0;
	GOODS_PRICE_TYPE	PriceType	= GOODS_PRICE_UNKNOWN;

	if ( ! GetRepairPrice(ItemID, ware_idx, Price, PriceType) )
		return REPAIR_TYPE_UNKOWN;

	UINT32 CurrnetDurability = 0, MaxDurability = 0;
	if ( ! GetDurability(ItemID, ware_idx, CurrnetDurability, MaxDurability) )
		return REPAIR_TYPE_UNKOWN;

	if ( CurrnetDurability == MaxDurability )
		return REPAIR_TYPE_NEEDLESS;

	// ������ ������ ������ ������ ��
	UINT32		MyMoney = 0;
	switch ( PriceType )
	{
	case GOODS_PRICE_POINT:	MyMoney = UserInfoContext::i()->GetMyPoint();		break;
	case GOODS_PRICE_CASH:		MyMoney = UserInfoContext::i()->GetMyCash();		break;
	default:		I3ASSERT_0;		break;
	}

	REPAIR_TYPE	Rv = REPAIR_TYPE_CANNOT;

	if ( MyMoney == 0 ) Rv = REPAIR_TYPE_CANNOT;	// ������ �� �� ���� ��.
	else if ( MyMoney >= Price ) Rv = REPAIR_TYPE_CAN_FULL;	// ���� ������ ������ ���
	else Rv = REPAIR_TYPE_CAN_SOME; // ������ �Ϻθ� ���� �� ���

	return Rv;
}

/*********//*********/
bool	CShopContext::RepairItem( T_ItemID ItemID)
{
	// DB �ε��� üũ
	T_ItemDBIdx	WeapinWereDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID( ItemID); //���� ��ȹ �������δ� ���⸸ �Ѵ�. ������ T_ItemDBIdx�� �˰� �ֱ� �����̴�.
	if ( INVALID_WAREDB_INDEX == WeapinWereDBIdx )
		return false;

	return RepairItem(ItemID, WeapinWereDBIdx);
}

bool	CShopContext::RepairItem( T_ItemID ItemID, T_ItemDBIdx ware_idx)
{
	// DB �ε��� üũ
	if ( INVALID_WAREDB_INDEX == ware_idx )
		return false;

	// ������ �� �ִ��� Ȯ���Ѵ�.
	REPAIR_TYPE	Rv = CheckCanRepair(ItemID, ware_idx);
	if ( Rv == REPAIR_TYPE_UNKOWN || Rv == REPAIR_TYPE_CANNOT )
		return false;

	// ���� �̺�Ʈ�� �߻���Ų��.
	return (GameEventSender::i()->SetEvent(EVENT_ITEM_REPAIR, &ware_idx) == true) ? true : false;
}


void CShopContext::OnGameEvent(INT32 event,INT32 arg, const i3::user_data& UserData)
{
	m_nGameEvent = (GAME_EVENT) event;
	m_nEventArg = arg;

	switch(event)
	{
		case EVENT_ENTER_SHOP:
		case EVENT_ENTER_GIFTSHOP:
			{
				EnterServer();

				if( EV_FAILED(arg))			
				{
					GameUI::ExitGamePopup( GAME_RCSTRING("STBL_IDX_EP_SHOP_ENTER_FAIL_E") );
				}
				//	������ ���� ���� ���Կ� �����ϸ� ��ǰ����Ʈ�� ��ǰ�� ä���.
				BuildGoodsList();
				
				// Clan ������ �޾ƿɴϴ�.
				getClanInfo();

				//2014-08-27 ������������ �ٽ� ��Ŀ�� ������ �ش� UI�� �ٽ� Ȱ��ȭ�ϵ��� �߰��մϴ�. ENG_��뿵
				if(i3::same_of<UIMainFrame*>(g_pFramework->GetUIMainframe()))
				{
					UIMainFrame* main_frame = g_pFramework->GetUIMainframe();

					if(main_frame->GetCurrentSubPhaseType() == USP_SHOP)
					{
						UIPhaseSubBase* sub_phase = main_frame->GetCurrentSubPhase();
						UIPhaseShop* shop_phase = (UIPhaseShop*)sub_phase;
						shop_phase->enable_ability_main_info(true);
					}
					else if (main_frame->GetCurrentSubPhaseType() == USP_GIFTSHOP)
					{
						UIPhaseSubBase* sub_phase = main_frame->GetCurrentSubPhase();
						UIPhaseGiftShop* shop_phase = (UIPhaseGiftShop*)sub_phase;
						shop_phase->enable_ability_main_info(true);
					}
				}

			}
			break;
		case EVENT_LEAVE_SHOP:
			{
				if( EV_FAILED(arg))
				{
					GameUI::ExitGamePopup( GAME_RCSTRING("STBL_IDX_EP_SHOP_LEAVE_FAIL_E") );
				}
				// �������� ���� ��� ����Ʈ�� Ŭ�����Ѵ�.
				//ClearGoodsList();
			}
			break;
		case EVENT_BUY_GOODS:
			{
				m_bEventLock = false;
			}
			break;
		case EVENT_SEND_GIFT:		//	�����ϱ�
			{
				m_bEventLock = false;
			}
			break;
		case EVENT_ENTER_GACHA :
			EnterServer();
			break;
	}
}

const SHOP_GOODS_PACK * CShopContext::GetGoodsPack( T_ItemID itemID)
{
	if( !g_bBuildGoodsList )	BuildGoodsList();

	const i3::vector<SHOP_GOODS_PACK*>& ShopList = GetShopList( convert_type(itemID) );

	const size_t size = ShopList.size();
	for(size_t i=0; i<size; ++i)
	{
		SHOP_GOODS_PACK * pGoodsPack = ShopList[i];
		if( pGoodsPack == nullptr)	continue;

		for(INT32 j = 0; j < pGoodsPack->GetGoodsCount(); ++j)
		{
			SHOP_GOODS* pGoods = pGoodsPack->GetGoods(j);
			if( pGoods == nullptr)
				continue;

			if( pGoods->_ItemID == itemID )
			{
				return pGoodsPack;
			}
		}
	}

	return nullptr;
}

const SHOP_GOODS * CShopContext::GetGoods( T_ItemID itemID)
{
	SHOP_GOODS * pGoods = nullptr; 
	for(size_t type = SHOP_WEAPON_GOODS; type < SHOP_GOODS_MAX; ++type)
	{
		INT32 goodsPackCount = GetGoodsPackCount( (SHOP_GOODS_TYPE)type );

		for(INT32 i = 0 ; i < goodsPackCount ; i++)
		{
			const SHOP_GOODS_PACK * pGoodsPack = GetGoodsPack( (SHOP_GOODS_TYPE)type, i);

			if( pGoodsPack == nullptr)
				continue;

			for(INT32 j = 0; j < pGoodsPack->GetGoodsCount(); ++j)
			{
				pGoods = pGoodsPack->GetGoods(j);
				if( pGoods == nullptr)
					continue;

				if( pGoods->_ItemID == itemID )
				{
					return pGoods;
				}
			}
		}
	}

	return nullptr;
}







#ifdef I3_DEBUG
void CShopContext::DumpGoodsList(SHOP_GOODS_TYPE goodsType/*= SHOP_GOODS_UNKNOWN*/)
{
	I3TRACE("============================================\n");
	I3TRACE("GOODS LIST - top\n");

	if( goodsType == SHOP_GOODS_UNKNOWN)	//	��ǰ ������ ������� ��� ��ǰ ����
	{
		for(INT32 i=0; i<SHOP_GOODS_MAX; i++)
		{
			_DumpGoodsList( (SHOP_GOODS_TYPE) i);
		}
	}
	else	//	������ ��ǰ�� ����
	{
		_DumpGoodsList( goodsType);
	}

	I3TRACE("GOODS LIST - end\n");
	I3TRACE("============================================\n");	
}

void CShopContext::_DumpGoodsList( SHOP_GOODS_TYPE goodsType)
{
	char temp[MAX_STRING_COUNT] = "";
	const i3::vector<SHOP_GOODS_PACK*>& ShopList = GetShopList((SHOP_GOODS_TYPE) goodsType);

	//	��ǰ ����Ʈ�� ��ǰ Pack
	for( size_t packCnt = 0; packCnt < ShopList.size(); packCnt++)
	{
		SHOP_GOODS_PACK * pGoodsPack = ShopList[packCnt];

		//	��ǰ Pack�ȿ� ��ǰ
		for( INT32 goodsCnt = 0; goodsCnt<pGoodsPack->GetGoodsCount(); goodsCnt++)
		{
			SHOP_GOODS * pGoods = pGoodsPack->GetGoods(goodsCnt);

			//	���� ���
			sprintf_s( temp, "SHOP GOODS [ GoodsId:%u / ItemID:%u ] = CLASS %u : NUM %u\n", 
				pGoods->_GoodsID,
				pGoods->_ItemID, 
				GET_ITEM_TYPE(pGoods->_ItemID), 
				GET_ITEM_NUMBER(pGoods->_ItemID));
			I3TRACE( temp);
		}
	}
}
void CShopContext::DumpAllGoodsInGameContext(void)
{
	TEMP_ERASE_CODE;

	//////	������ ���� �޾ƿ� ��ǰ
	CShop* shop = CShop::i();
	UINT32 nGoodsCount = shop->GetGoodsCount();
	SHOP_GOODS_EXPANSION* pGoodsDataArray = shop->GetGoodsAllData();
	i3::string strTemp;

	I3TRACE("============================================\n");
	I3TRACE("GOODS LIST in GameContext - top\n");

	for( UINT32 i = 0; i < nGoodsCount; i++)
	{
		SHOP_GOODS_EXPANSION * pGoodsData = &pGoodsDataArray[i];
		SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(pGoodsData->_ui32GoodsID);

		i3::sprintf( strTemp, "SHOP GOODS (%d) = name:%s, cash:%d, point:%d, count:%d\n", 
									pGoodsData->_ui32GoodsID,
									"(null)",		//(char*)shop->GetGoodsName(index),
									pGoodsData->GetCash(),
									pGoodsData->GetPoint(),
									shop->GetGoodsListData(pGoodsData->_ui32GoodsID)->_ui8ItemCount );

		I3TRACE( strTemp);

		//	��ǰ��Ű���ȿ� ���� ��ǰ
		INT32 ItemCount = pGoodsList->_ui8ItemCount;
		for( INT32 sub = 0; sub < ItemCount ; sub++)
		{
			UINT8 type = pGoodsList->_ppItemData[sub]->_ui8AuthType;

			switch( type)
			{
			case ITEM_TYPE_TIME :			//	�Ⱓ
				{
					i3::sprintf(strTemp, "              (%d) + id:%d, type:%s, seconds:%d\n", 
									sub,
									pGoodsList->_ppItemData[sub]->_ui32ItemID,
									"ITEM_TYPE_TIME",
									pGoodsList->_ppItemData[sub]->_ui8AuthExpireType);

					I3TRACE( strTemp);
				}
				break;
			case ITEM_TYPE_COUNT :		//	����
				{
					i3::sprintf( strTemp, "              (%d) + id:%d, type:%s, quantity:%d\n", 
									sub,
									pGoodsList->_ppItemData[sub]->_ui32ItemID,
									"ITEM_TYPE_COUNT",
									pGoodsList->_ppItemData[sub]->_ui8AuthExpireType);

					I3TRACE( strTemp);
				}
				break;
			}			
		}
	}

	I3TRACE("GOODS LIST in GameContext - end\n");
	I3TRACE("============================================\n");	
}
#endif

void CShopContext::GetGoodsName(i3::rc_wstring& Name, UINT32 nGoodsid, T_ItemID nItemid)
{
	T_ItemID pitemid = 0;
	
	if( nGoodsid != 0)
		pitemid = CShopContext::ConvertGoodsID( nGoodsid) ;
	else
		pitemid = nItemid;

	switch((ITEM_TYPE) GET_ITEM_TYPE(pitemid))
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		{
			CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(pitemid);
			I3ASSERT( pInfo != nullptr);
			pInfo->GetWeaponName( Name);
		}
		break;
	case ITEM_TYPE_CHARA:			// ĳ���� ��ü
	case ITEM_TYPE_HEADGEAR:		// HeadGear
	case ITEM_TYPE_FACEGEAR:		// FaceGear
	case ITEM_TYPE_UPPER:			// ����
	case ITEM_TYPE_LOWER:			// ���� 10
	case ITEM_TYPE_GLOVE:			// �尩
	case ITEM_TYPE_BELT:			// ��Ʈ
	case ITEM_TYPE_HOLSTER:			// ������
	case ITEM_TYPE_BERET :			// ������
		{
			const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo( pitemid);
			if( pEquipInfo)
			{
				g_pCharaInfoDataBase->GetCharacterName(pitemid, Name); //sizeof(outItemName));
			}		
			else
			{
				i3::sprintf(Name, L"%s\nGoods:%d\nItem:%d", INVALID_TEXTW,  nGoodsid, pitemid);
			}
		}
		break;
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:
	case ITEM_TYPE_GACHA_TICKET:
		{
			ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(pitemid);
			if( pInfo )
			{
				g_pShopItemInfoDataBase->GetItemName(pitemid, Name);
			}
			else
				i3::sprintf(Name, L"%s\nGoods:%d\nItem:%d", INVALID_TEXTW, nGoodsid, pitemid);
		}
		break;
	case ITEM_TYPE_POINT:	// ����Ʈ�� ���. 2012.01.13 ���ؿ�.
		{
			UINT32 ui32Point = GET_POINTITEM( pitemid );
			i3::sprintf(Name, L"%d POINT", ui32Point);
		}
		break;
	}
}

INT32 CShopContext::GetGoodsUIShapeIdx(UINT32 nGoodsid, T_ItemID nItemid)
{
	T_ItemID pitemid = 0;

	if( nGoodsid != 0)
		pitemid = CShopContext::ConvertGoodsID( nGoodsid) ;
	else
		pitemid = nItemid;

	switch((ITEM_TYPE) GET_ITEM_TYPE(pitemid))
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		{
			CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo( pitemid);
			I3ASSERT( pInfo != nullptr);
			return g_pFramework->GetWeaponOrdIdx( pInfo->GetTypeClass(), pInfo->GetNumber());
		}
		break;
	case ITEM_TYPE_CHARA:
	case ITEM_TYPE_HEADGEAR:
		{
			const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo( pitemid);
			if( pEquipInfo)
			{
				return pEquipInfo->GetUIShapeIndex();
			}		
		}
		break;
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:
	case ITEM_TYPE_GACHA_TICKET:
		{
			ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(pitemid);
			if( pInfo )
			{
				pInfo->GetShapeIndex();
			}
		}
		break;
	}

	return -1;
}

INT32 CShopContext::ConvertGoodsID(UINT32 nGoodsId)
{
	CShop* shop = CShop::i();

	return shop->ConvertGoodsID(nGoodsId);

}

void CShopContext::getClanInfo()
{
	CClanContext * pClanCxt = g_pFramework->GetClanContext();

	// �ڽ��� Clan������ �����ɴϴ�.
	USER_INFO_BASIC Info;
	UserInfoContext::i()->GetMyUserInfoBasic(&Info);

	pClanCxt->SendClanDetailInfoNEvent(Info.m_ui32Clanidx, 0);
}

INT32 CShopContext::FindItemCountOrPeriod(const UINT32 goodsID) const
{
	SHOP_MATCHING_DATA* dataList = CShop::i()->GetMatchingAllData();
	if (dataList)
	{
		for (size_t idx=0; idx<CShop::i()->GetMatchingCount(); idx++)
		{
			SHOP_MATCHING_DATA* data = (dataList+idx);
		
			if (goodsID == data->_ui32GoodsID)
				return data->_ui32ItemArg;
		}
	}

	return -1;
}