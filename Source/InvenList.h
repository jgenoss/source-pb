#ifndef _InvenList_h
#define _InvenList_h

#include "InvenDef.h"

class CInvenList: public i3ElementBase
{
	I3_CLASS_DEFINE(CInvenList, i3ElementBase);

	friend class iInvenDB;
	friend class CheckerEquipments;		// get_main_list
	friend class UIBattlePopupSelectWeapon;


public:
	CInvenList();
	virtual ~CInvenList();

public:
	static CInvenList* i();

private:
	const char *	__getDBTypeString( INVEN::DB_TYPE type);

protected:
	const i3::vector<INVEN_BUFFER*>& get_main_list(INVEN::DB_TYPE db_type) const;

public:
	const i3::vector<INVEN_BUFFER*>& get_ref_list( INVEN::DB_TYPE db_type, INT32 usage) const;

	bool IsValidItem( INVEN::DB_TYPE db_type, T_ItemID ItemId) const;
	bool IsValidItem( INVEN::DB_TYPE db_type, T_ItemDBIdx ware_idx) const;

	void			AddItem(INVEN_BUFFER* pBuffer,INT32 nCount);
	const INVEN_BUFFER* GetItem( INVEN::DB_TYPE db_type, INT32 idx, INT32 usage = -1) const;
	UINT32			GetItemCount( INVEN::DB_TYPE dbType, INT32 usage = -1);

	bool			GetItemTypeByItemID( INVEN::DB_TYPE dbType, T_ItemID itemID, UINT8 * pOutType, UINT32 * pOutArg);
	bool			GetItemTypeByDBIdx( INVEN::DB_TYPE dbType, T_ItemDBIdx ware_idx, UINT8* pOutType, UINT32* pOutArg);
	bool			GetItemTypeByIdx( INVEN::DB_TYPE dbType, INT32 idx, UINT8* pOutType, UINT32* pOutArg);
	UINT32			GetItemArg( INVEN::DB_TYPE dbType, INT32 idx, INT32 usage = -1);
	T_ItemID		GetItemID( INVEN::DB_TYPE dbType, INT32 idx, INT32 usage = -1);
	UINT32			FindItemCount( T_ItemID itemID);
	INT32			FindItemIndexByItemID( INVEN::DB_TYPE dbType, T_ItemID itemID, INT32 usage = -1);
	INT32			FindItemIndexByDBIdx( INVEN::DB_TYPE dbType, T_ItemDBIdx ware_idx, INT32 usage = -1);
	T_ItemID		FindItemIDByDBIdx( INVEN::DB_TYPE dbType, T_ItemDBIdx ware_idx);

	T_ItemDBIdx		FindWareDBIdx_ByItemID( T_ItemID ItemId);	//������ ������ ��� ���� �� ��..
	T_ItemDBIdx		GetDBIdxByIndex( INVEN::DB_TYPE dbType, INT32 idx, INT32 usage = -1);	//������ ������ ��� ���� �� ��..

	T_ItemDBIdx		FindWareDBIdx_ByItemID_ForBattle( T_ItemID ItemId); //��Ʋ ��, ���� "����"�� ���� ����Ͽ� DB �ε����� �������� ���� �Լ�	


	bool			HaveItemByItemID( T_ItemID itemID);
	bool			HaveItemByDBIdx( INVEN::DB_TYPE dbType, T_ItemDBIdx ware_idx);
	bool			HaveItemBySubtype( INVEN::DB_TYPE dbType, INT32 type);

	INVEN::DB_TYPE	FindInvenDBTypeByDBIdx( T_ItemDBIdx WareDBIdx);

	void		UpdateInvenBuffer(INVEN_BUFFER* pBuffer);
	void		DeleteInvenBuffer(T_ItemDBIdx WareDBIdx);
	void		DeleteInvenBuffer_only_DefaultParts( T_ItemID ItemId);
	void		DeleteInvenBuffer_only_Chara( T_ItemDBIdx WareDBIdx);
	void		DeleteInvenAllClear();
	
	INT32		GetCharaterSlotCount_V11();	//1.1�� ���Ͽ� ����ϴ�. 1.5�� �ٸ��� ����� ĳ���� ĳ���� ������ �־ �׷����ϴ�.
public:

public:
	//	ĳ�� ������
	INVEN_BUFFER* FindCashItem(INT32 ItemID);

	//	���� ����

private:
	const INVEN_BUFFER *	__GetCoupon(INT32 idx, bool bCheckAllData = false) const;

public:
	INT32		GetDiscountCouponCount();

	bool		IsUsedCoupon(T_ItemDBIdx CouponDBIdx);
	void		SetUsedCoupon(T_ItemDBIdx CouponDBIdx, bool bUsed);

	// ���� ���� �κ����� ����
	void		DeleteAllCouponBuffer();
	void		DeleteOnlyUsedCouponBuffer();
	void		DeleteCouponBuffer(T_ItemDBIdx CouponDBIdx);
	void		DeleteTimeOverCouponBuffers();

public:
	//	������� key �������� ã�´�.
	//	(1��,7��,30�� �̻�� ���� ���� �������� ����(���)�ϰ� �Ǹ� key �������� �Ǿ� �κ��丮�� ����ִ�.
	//	
	//	��) �̻�� ������ : CASHITEM_MAINTENANCE_XP_PLUS_130_1D, CASHITEM_MAINTENANCE_XP_PLUS_130_7D, CASHITEM_MAINTENANCE_XP_PLUS_130_30D
	//		key ������ : CASHITEM_MAINTENANCE_XP_PLUS_130

	bool IsUsingCashItem( T_ItemID ItemID); //MAKE_CASH_ITEM_FLAG �� ���� ������ ���̵�
	//bool IsUsingCashItem_Maintenance(CASHITEM_GROUP_TYPE type); //ITEM_TYPE_MAINTENANCE
	//bool IsUsingCashItem_Period(CASHITEM_GROUP_TYPE type); //ITEM_TYPE_WRAP_PERIOD
	//bool IsUsingCashItem_Count(CASHITEM_GROUP_TYPE type); //ITEM_TYPE_WRAP_COUNT

	void	SetLatestUsedCashItemFlag( T_ItemID ItemID) { m_latestUsedCashItemID = ItemID; }
	INT32	GetLatestUsedCashItemFlag(void) const { return m_latestUsedCashItemID; }

	void	SetUseBattleGiveupCashItem(bool bVal);
	bool IsUseBattleGiveupCashItem(void) const { return m_bUseBattleGiveupCashItem; }

private:
	INT32	m_latestUsedCashItemID;
	bool	m_bUseBattleGiveupCashItem;		//	�����̵�, �����̵� �������� �̿��Ͽ� �ΰ��ӿ��� ������ ���

public:
	void	SetWeaponListDummy(void);		// ����� �׽�Ʈ�� ����
	void	SetCharacterListDummy(void);	// ����� �׽�Ʈ�� ����
	void	SetCashItemListDummy(void);		// ����� �׽�Ʈ�� ����

private:
	class iInvenDB * m_InvenDB[INVEN::DB_MAX_COUNT];
	class CouponBufferDB* m_CouponDB;
};

#endif //_InvenList_h
