#ifndef __SHOP_CONTEXT_H
#define __SHOP_CONTEXT_H

#include "UI_V11/ShopContextBasket.h"

namespace i3 { class user_data; }

enum SHOP_SORT_CATEGORY
{
	SHOP_SORT_CATEGORY_NEWHOT = 0,
	SHOP_SORT_CATEGORY_WEAPON,
	SHOP_SORT_CATEGORY_ITEM,
	SHOP_SORT_CATEGORY_MAX,
};
enum SHOP_SORT_TYPE
{
	SHOP_SORT_TYPE_NORMAL = 0,
	SHOP_SORT_TYPE_NEW,
	SHOP_SORT_TYPE_HOT,
	SHOP_SORT_TYPE_NAME,
	SHOP_SORT_TYPE_DAMAGE,
	SHOP_SORT_TYPE_RAPID,
	SHOP_SORT_TYPE_SPEED,
	SHOP_SORT_TYPE_POINT,
	SHOP_SORT_TYPE_CASH,
	SHOP_SORT_TYPE_MAX,
};

enum
{
	FILTER_CHARA_TYPE = 0,
	FILTER_CHARA_TYPE_NORMAL,
	FILTER_CHARA_TYPE_RED,
	FILTER_CHARA_TYPE_BLUE,
	FILTER_CHARA_TYPE_HEADGEAR = 7,
};

class CShopContext : public i3::shared_ginst<CShopContext>
{
private:
	UINT32 m_nUserSelectedGoodsId;

	//	�������� �޾ƿ� ��ǰ ����� Weapon, Chara, Item ���� �з��Ͽ� ������ �Ӵϴ�.
	//	�����κ��� ��ǰ ����� ����� ��� ��ǰ ����� �ٽ� ������ �մϴ�.
	//	������ ����GUI������ �� ������θ� ������ ó���� �մϴ�.
	//	SHOP_GOODS_PACK* �� �������.
	i3::vector<SHOP_GOODS_PACK*> m_pGoodsList[SHOP_GOODS_MAX];

	GAME_EVENT	m_nGameEvent;
	INT32 m_nEventArg;

	bool m_bEventLock;
	bool m_bInServer;
	
	bool m_bChangedChara;
	bool m_bChangedWeapon;

	SHOP_GOODS * m_pSelectedGoods;

	// ĸ�� ������ �ε���
	INT32				m_nCapsuleItemIdx;				// �������ؽ�Ʈ���� �ϴ� �̸��� �̵��Ѵ�..

	bool	m_bisBuildGoodsList;	// revision 53188 BuildGoodsList�� �ִ� static local ���� ����, ��� ������ ����

	// ���� �±� ���� ����.
	i3::vector<UINT8> m_tagPriorities;
	i3::vector<UINT8> m_tagPrioritiesType;
	SHOP_ITEM_TYPE m_ItemTagType;


#ifdef I3_DEBUG
	void	DumpAllGoodsInGameContext();
	void	DumpGoodsList(SHOP_GOODS_TYPE goodsType = SHOP_GOODS_UNKNOWN);
	void	_DumpGoodsList( SHOP_GOODS_TYPE goodsType);
#endif

public:
	CShopContext(void);
	virtual ~CShopContext(void);

	void EnterServer(void);
	void LeaveServer(void);
	bool InServer(void)	const		{ return m_bInServer; }

	void BuildGoodsList(void);
	void ClearGoodsList(void);
	void SetGoodsList( bool bBuild)	{ m_bisBuildGoodsList = bBuild; }	// revision 54070

	const i3::vector<SHOP_GOODS_PACK*>& GetShopList(SHOP_GOODS_TYPE type)	{ I3ASSERT(0 <= type && SHOP_GOODS_MAX > type); return m_pGoodsList[type]; }
	i3::vector<SHOP_GOODS_PACK*>& SetShopList(SHOP_GOODS_TYPE type)			{ I3ASSERT(0 <= type && SHOP_GOODS_MAX > type); return m_pGoodsList[type]; }

	const SHOP_GOODS_PACK * GetGoodsPack(SHOP_GOODS_TYPE type, INT32 index);
	const SHOP_GOODS_PACK * GetGoodsPack( T_ItemID itemID);	
	const SHOP_GOODS * GetSelectedGoods(void) const				{ return m_pSelectedGoods; }
	const SHOP_GOODS * GetGoods( T_ItemID itemID);

	//1.1 ������ ���Ͽ� const �Ⱥپ� �ִ� �Լ��� ����ϴ�.
	//

	INT32	GetGoodsCount(SHOP_GOODS_TYPE type);
	INT32	GetGoodsPackCount(SHOP_GOODS_TYPE type);

	//	��ǰ�� �����մϴ�.
	bool	CheckBuy(UINT32 GoodsID, UINT32 PriceType, UINT8 price_idx, GOODS_PRICE_TYPE tabPrice = GOODS_PRICE_ALL);
	
	/** \brief ��ǰ ���
		\return 1 : Chara, 2 : Weapon*/
	EQUIP::GOODS	EquipGoods(const SHOP_GOODS * pGoods, bool bUpdateViewModel = true);

	void AddTagPriority(const UINT8 priority);
	void SetTagPriorityType(const SHOP_ITEM_TYPE priority);
	UINT8 GetTagPriority(const SHOP_ITEM_TYPE type) const;

	// ���� ����
public:
	bool	IsDurabilityItem( T_ItemID ItemID) const;

	bool	GetDurability( T_ItemID ItemID, REAL32& DurableRate) const;
	bool	GetDurability( T_ItemID ItemID, T_ItemDBIdx ware_idx, REAL32& DurableRate) const;
	// ������ ���������� ���ο��� ������ִ� ����� ����ؾ���
	bool GetDurability( T_ItemID ItemID, UINT32 & Current, UINT32 & Max) const; //Parts�� �ΰ� �̻��� ��� ��ȿ�� ���� ���� �� ��
	bool GetDurability( T_ItemID ItemID,T_ItemDBIdx ware_idx, UINT32 & Current, UINT32 & Max) const; //�κ���

	bool	GetRepairPrice( T_ItemID ItemID, UINT32 & Price, GOODS_PRICE_TYPE & PriceType) const;
	bool	GetRepairPrice( T_ItemID ItemID, T_ItemDBIdx ware_idx, UINT32 & Price, GOODS_PRICE_TYPE & PriceType) const; //�κ���

	REPAIR_TYPE	CheckCanRepair( T_ItemID ItemID) const;
	REPAIR_TYPE	CheckCanRepair( T_ItemID ItemID, T_ItemDBIdx ware_idx) const;

	bool RepairItem( T_ItemID ItemID);
	bool RepairItem( T_ItemID ItemID, T_ItemDBIdx ware_idx);

	void OnGameEvent(INT32 event,INT32 arg, const i3::user_data& UserData);
	void PopGameEvent(GAME_EVENT * pOutEvent, INT32 * pOutArg)	
	{ 
		*pOutEvent = m_nGameEvent; 
		*pOutArg = m_nEventArg;

		m_nGameEvent = EVENT_NOTHING; 
		m_nEventArg = 0;
	}

	INT32				getCapsuleItemIdx(void)								{ return m_nCapsuleItemIdx;	}
	void				setCapsuleItemIdx(INT32 idx)						{ m_nCapsuleItemIdx = idx;	}

	/** \brief ������ ���̵� ������̵� ������ �̸����� ����Ʈ�մϴ�. */
	void GetGoodsName(i3::rc_wstring& Name, UINT32 nGoodsid, T_ItemID nItemid);
	/** \brief ������ ���̵� ������̵� ������ �̸����� ����Ʈ�մϴ�. */
	INT32 GetGoodsUIShapeIdx(UINT32 nGoodsid, T_ItemID nItemid);
	/** \brief ���� ���̵� ������ ���̵�� ����Ʈ�մϴ�. */
	INT32	ConvertGoodsID(UINT32 nGoodsId);
	INT32	FindItemCountOrPeriod(const UINT32 goodsID) const;

protected:
	void _AddGoodsToShopList(SHOP_GOODS_TYPE type, UINT32 nAddGoodsId);

private:
	void getClanInfo();

};


#endif