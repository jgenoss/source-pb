#ifndef _SHOP_DEF_H
#define _SHOP_DEF_H

enum SHOP_ITEM_TYPE
{
	SHOP_ITEM_TYPE_NOTSETTING = -1,
	SHOP_ITEM_TYPE_NORMAL = 0,
	SHOP_ITEM_TYPE_NEW,			//	�Ż�
	SHOP_ITEM_TYPE_HOT,			//	����
	SHOP_ITEM_TYPE_EVENT,		//	�̺�Ʈ ����
	SHOP_ITEM_TYPE_PCROOM,		//	PC�� ����
	SHOP_ITEM_TYPE_SALE,		//	SALE

	SHOP_ITEM_TYPE_COUNT
};

// CommonDef.h�� GOODS_PRICE_TYPE��� ����Ǿ� ������.
// �� Enum�� ��Ʈ ������ �ȵǱ� ������
// ���μ����Ͽ���.
typedef			UINT32		GOODS_PRICE_TYPE;

#define			GOODS_PRICE_UNKNOWN		(0x00000000)
#define			GOODS_PRICE_POINT		(0x00000001)
#define			GOODS_PRICE_CASH		(0x00000002)
#define			GOODS_PRICE_ALL			(GOODS_PRICE_POINT | GOODS_PRICE_CASH)


void	CopyPriceTypeText(GOODS_PRICE_TYPE PriceType, i3::rc_wstring& outText);

struct GOODS_PRICE_INFO
{
	UINT32	_Cash;		// ĳ�� ����
	UINT32	_Point;		// ����Ʈ ����
	UINT32	_Type;		// ���� ��� (Cash / Point / All)
	UINT8	_SaleMark;	// ��ǰ ��ũ 0 - ����, 1 - NEW, 2 - Hot	

	void	BuildPriceInfo(UINT32 ui32Cash, UINT32 ui32Point, UINT8 ui8Mark)
	{
		_Cash = ui32Cash;
		_Point = ui32Point;
		_SaleMark = ui8Mark;

		_Type = GOODS_PRICE_UNKNOWN;

		if ( ui32Point > 0 )
			_Type |= GOODS_PRICE_POINT;

		if ( ui32Cash > 0 )
			_Type |= GOODS_PRICE_CASH;

		if (ui32Cash == 0 && ui32Point == 0)
			_Type = GOODS_PRICE_POINT;
	}

	bool CheckPrice()
	{
		if (_Cash == 0 && _Point == 0)
			return false;

		return true;
	}


} ;

bool operator==(const GOODS_PRICE_INFO & lhs, const GOODS_PRICE_INFO & rhs);
bool operator!=(const GOODS_PRICE_INFO & lhs, const GOODS_PRICE_INFO & rhs);

struct SHOP_GOODS
{
	UINT32				_GoodsID;			// ��ǰ ���̵�
	UINT8				_AuthType;			// 1:�Ⱓ��, 2:������
	UINT32				_AuthArg;			// _AuthType�� 1�� ��� ��(����), 2�� ��� ����
	UINT32				_ItemID;
	UINT8				_SaleDesignation;	//	���Ž� ȣĪ ����	(DESIGNATION_TYPE)
	UINT8				_SaleType;
	GOODS_PRICE_INFO	_PriceInfo[PRICE_TYPE_MAX]; // ���� ����

	UINT32			GetPriceCash(INT32 idx)	 const	{ return _PriceInfo[idx]._Cash; }
	UINT32			GetPricePoint(INT32 idx) const	{ return _PriceInfo[idx]._Point; }
	UINT32			GetPriceType(INT32 idx)	 const	{ return _PriceInfo[idx]._Type; }
	UINT8			GetSaleMark(INT32 idx)	 const	{ return _PriceInfo[idx]._SaleMark; }

	bool			isShopGoods(void) const { return (_SaleType == GOODS_SALE_ALL || _SaleType == GOODS_SALE_BUY); }
	bool			isGiftGoods(void) const { return (_SaleType == GOODS_SALE_ALL || _SaleType == GOODS_SALE_GIFT); }
	
} ;

bool operator==(const SHOP_GOODS & lhs, const SHOP_GOODS & rhs);
bool operator!=(const SHOP_GOODS & lhs, const SHOP_GOODS & rhs);

struct	SHOP_GOODS_PACK
{
public:
	SHOP_GOODS_PACK();
	~SHOP_GOODS_PACK();

	INT32				GetGoodsCount(void) const;
	bool				AddGoods(SHOP_GOODS * pAddGoods);
	SHOP_GOODS *		GetGoods(INT32 index) const;
	void				Sort(bool (*pProc)( const SHOP_GOODS*, const SHOP_GOODS*) );
	void				ClearAll();
	INT32				GetDisplayShopGoodsIndex(bool bigger = false);									// ���� ���Կ��� ǥ���� goods �̱�.
	GOODS_PRICE_TYPE	GetPriceTypeAll(INT32 price_idx);												// �ѿ� ���Ե� ��� ��ǰ�� ���������� ��Ʈ�����ؼ� ��ȯ�մϴ�.
	
private:
	i3::vector<SHOP_GOODS*> _pGoodsList;		//	SHOP_GOODS�� �������. 1�� or n��
};

bool operator==(const SHOP_GOODS_PACK & lhs, const SHOP_GOODS_PACK & rhs);
bool operator!=(const SHOP_GOODS_PACK & lhs, const SHOP_GOODS_PACK & rhs);

enum SHOP_GOODS_TYPE
{
	SHOP_GOODS_UNKNOWN = -1,
	SHOP_WEAPON_GOODS = 0,
	SHOP_CHARACTER_GOODS,
	SHOP_SKIN_GOODS,
	SHOP_CASH_ITEM_GOODS,
	SHOP_PACKAGE_GOODS,

	SHOP_GOODS_MAX
};


enum PCCAFE_TYPE
{
	PCCAFE_NONE		= 0,		//	�Ϲ�
	PCCAFE_EVENT	= 1,		//	�̺�Ʈ ����
	PCCAFE_NORMAL	= 2,		//	PC�� ����
	PCCAFE_PREMIUM	= 3,		//	PREMIUM PC�� ����
};


enum SLOT_SALE_TEXT
{
	SALE_TEXT_FIRST,
	SALE_TEXT_SECOND,

	SALE_TEXT_MAX
};

//	Ȱ��ȭ �ɽ� ������� ���� ���θ� �����. ��Ȱ��ȭ�� ���Խ� ������ ���� �ʴ´�.
#define AUTH_FROM_BUY_GOODS

namespace EQUIP 
{
	enum GOODS 
	{
		GOODS_NONE = 0,
		GOODS_CHARA,
		GOODS_WEAPON,
	};

	//	��� ����� ��� GOODS_EQUIP_CHARA �Ǵ� GOODS_EQUIP_WEAPON�� �÷��� ������ ��ȯ�Ѵ�. 0�� ��� ����� ��� ����.
	GOODS	EquipGoods( const SHOP_GOODS * pGoods, bool bUpdateViewModel = true);
}

//	�����ϱ�ÿ� ��� ����
#define SHOP_GIFT_REQUIREMENT_RANK		4

#define	GOODS_DAY_1				1
#define	GOODS_DAY_3				3
#define	GOODS_DAY_7				7
#define	GOODS_DAY_30			30

// �κ��丮�� �������� ��ǰ�� ǥ�õǴ� ���� ����.
#define			SLOT_HORI_COUNT_WEAPON			2
#define			SLOT_HORI_COUNT_CHARA			3
#define			SLOT_HORI_COUNT_ITEM			3
#define			SLOT_HORI_COUNT_PACKAGE			3		// ���������� ���˴ϴ�.

#define			SLOT_VERT_COUNT_WEAPON			3
#define			SLOT_VERT_COUNT_CHARA			2
#define			SLOT_VERT_COUNT_ITEM			2
#define			SLOT_VERT_COUNT_PACKAGE			2

// ���Կ� ǥ�õǴ� ��ǰ ����.
#define			MAX_SLOT_FOR_SHOP_WEAPON		6
#define			MAX_SLOT_FOR_SHOP_CHARACTER		6
#define			MAX_SLOT_FOR_SHOP_ITEM			6
#define			MAX_SLOT_FOR_SHOP_PACKAGE		6



#define			ITEM_TERM_ADJUST_VALUE		120


enum	REPAIR_TYPE
{
	REPAIR_TYPE_UNKOWN,
	REPAIR_TYPE_CAN_FULL,
	REPAIR_TYPE_CAN_SOME,
	REPAIR_TYPE_CANNOT,
	REPAIR_TYPE_NEEDLESS,
};

bool				CheckYourRank( UINT8 bCheckRank);			//	��� �̻� �ڰ� �Ǵ�
bool				IsCashBuyingFromGoods( SHOP_GOODS * pGoods, UINT8 price_idx);		//	��ǰ�� ĳ������ ����

// �Ⱓ�� �������� �Ǹ� �Ⱓ�� ����մϴ�.
INT32 GetMaintenanceItemDay( T_ItemID ItemID, UINT8 AuthType, UINT32 AuthArg);
INT32 GetMaintenanceItemHour( T_ItemID ItemID, UINT8 AuthType, UINT32 AuthArg);




bool ExcuteSNDAWeb(void);

//	���� ���� ���� ���������� ����.
bool ExcuteCashWeb(void);

//	���ͳ��ͽ��÷η� ����
bool ExecuteInternetExplorer(LPCWSTR url, LPCSTR postData /*= nullptr*/, bool isPopup /*= false*/);

// "������ : %d��"�� ���� �ؽ�Ʈ�� �����.
void	FormatHaveItemTermText(i3::stack_wstring& out, UINT32 Term);

// �������� 0���� Ȯ���մϴ�. (�Ⱓ�� ����Ǿ����� �Ǵ� ���������� 0������ üũ�մϴ�.);
// �ű� ������ �ý����� �̰� ���� �ȵ� - �űԴ� arg / max < 0.01 �϶� ����
bool	GetEnduranceZero(UINT8 InvenItemAttr, UINT32 InvenItemArg);

bool	CanUse_ViewOtherUserItemInfo();



#endif

