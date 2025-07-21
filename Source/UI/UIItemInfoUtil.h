#if !defined( __UI__Item_Info_Util_H__)
#define __UI__Item_Info_Util_H__

#include "UIItemInfo_Def.h"
#include "ShopDef.h"

namespace item_def {

/*********************************/
//�Ϲ� ��ȯ
item_def::Category get_category_tab(T_ItemID item_id);
item_def::Category get_category(T_ItemID item_id);
item_def::Category get_category_goodsID(T_GoodsID goods_id);
item_def::SideCategory get_side_category(T_ItemID item_id);
bool is_chara_category(T_ItemID item_id);
bool is_dino_category(T_ItemID item_id);
item_def::PartsType_ShopPreview get_shop_preview_parts_type(T_ItemID item_id);

WEAPON_SLOT_TYPE get_weapon_slot_type(T_ItemID item_id);
WEAPON_CLASS_TYPE get_weapon_class_type(T_ItemID item_id);
ITEM_TYPE get_weapon_type(T_ItemID item_id);
ITEM_TYPE get_chara_type(T_ItemID item_id);
ITEM_TYPE get_parts_type(T_ItemID item_id);
ITEM_TYPE get_item_type(T_ItemID item_id);	//���� �Ҿ���
ITEM_TYPE get_type(T_ItemID item_id);

WEAPON_CLASS_TYPE get_weapon_usage(T_ItemID item_id);
CHARACTER_CLASS_TYPE get_chara_usage(T_ItemID item_id);
CHARACTER_CLASS_TYPE get_parts_usage(T_ItemID item_id);
ITEM_TYPE get_item_usage(T_ItemID item_id);
INT32 get_item_number(T_ItemID item_id); //������ �׸��� �ƴѰ�� -1�� �����Ѵ�.
UINT get_usage(T_ItemID item_id);

/*********************************/
//Ÿ�� ��ȯ
item_def::Category convert(item_def::SideCategory category);
item_def::SideCategory convert(item_def::Category category);
CHAR_EQUIPMENT_PARTS convert(item_def::PartsType type);

item_def::WeaponType convert_weapon_type(ITEM_TYPE type);
UINT32 convert_chara_type(ITEM_TYPE type);
item_def::PartsType convert_parts_type(ITEM_TYPE type);
item_def::ItemType convert_item_type(ITEM_TYPE type);

//InvenList Ŭ�������� ���� ��� ���ؼ� ���ȴ�.
SHOP_GOODS_TYPE convert_goods_type(item_def::Category category);	
WEAPON_SLOT_TYPE convert_weapon_type(item_def::WeaponType type);
UINT32 convert_chara_type(UINT32 type);
CHARACTER_CLASS_TYPE convert_parts_type(item_def::PartsType type);
ITEM_TYPE convert_item_type(item_def::ItemType type);

// ������ ��ȯ.
item_def::PartsType	get_beret_parts_type(void);
CHARACTER_CLASS_TYPE get_beret_class_type(void);

/*********************************/
//�Ϲ� �Լ�
bool is_default_item(T_ItemID item_id);
bool is_default_parts(T_ItemID item_id, T_ItemID chara_item_id);
bool is_shared_parts_item(T_ItemID item_id, T_ItemID chara_item_id);

ITEM_INFO get_default_item(T_ItemID item_id); //weapon
ITEM_INFO get_default_item(UINT32 idx, WEAPON_CLASS_TYPE type); //weapon

ITEM_INFO get_default_item(T_ItemID chara_item_id, INT32 idx); //parts
ITEM_INFO get_default_dino(void);

UINT get_shop_slot_idx(T_ItemID item_id);
UINT get_char_slot_idx(T_ItemID item_id);

bool is_locked_weapon(UINT8 mode_flag, T_ItemID item_id); //mode�� ���� lock(����� �� ����)�� ���� ���� �˷��ش�. (true: lock �� ����)
bool is_locked_weapon(UINT8 mode_flag, WEAPON_CLASS_TYPE classType, WEAPON_SLOT_TYPE usage);

bool is_direct_use_item(T_ItemID item_id);

}	//end of namespace item_def




/* ITEM_INFO Container
	���� WeaponType / PartsType �� ��Ī �Ǹ�, fuct_idx_to_slot_idx�� �ش� idx�� ��´�.
*/
class ItemInfoList
{
	typedef i3::vector< ITEM_INFO > List;

public:
	ItemInfoList() {}
	~ItemInfoList() {}

public:
	void clear()	{	m_list.clear();	}
	void resize(size_t size)	{	m_list.resize( size ); }
	size_t size() const { return m_list.size(); }
	
	void push( const ITEM_INFO & info);		//m_list�� resize() �Լ� ȣ�� �Ŀ� ����� ��� �߰��� ���� ���� �� ����. �ǵ��� clear() �Ŀ� �������.
	void push( const ItemInfoList& list);

	void	remove( size_t idx);
	
	size_t find_idx( const ITEM_INFO & info ) const;
	size_t find_idx( const T_ItemID & itemId ) const;
	size_t find_idx( const T_ItemID & itemId, const T_ItemDBIdx & ware_idx ) const;
	size_t find_idx_by_ware_idx(const T_ItemDBIdx & ware_idx) const;

	void remove_unshared(const T_ItemID & charaID);
	void remove_duplicate_default_item();
	
	void default_part_id_make_to_mainchara();	//���� �κ�����Ʈ�� ������ ����Ʈ ������ ����µ� ĳ���ͺ��� ���� ������ �ٸ� ���� �� �� ����ϴ� �Լ� ( 1.5 ������ )
	void default_skin_id_make_to_mainchara();	// ���� �κ�����Ʈ���� Skin�� ������ ���� ��� Default�� ��쿡�� ���� ��Ų��.
	void default_part_id_make_to_union();		//���� �κ�����Ʈ�� ������ ����Ʈ ������ ����µ� ĳ���Ͱ� ��� ������ �����ϴ� ���� �� �� ����ϴ� �Լ� ( 1.0, 1.1 ������ )

	void SearchKeyword(const WCHAR* wstr);
	
	const ITEM_INFO & operator[](size_t idx) const { return m_list[idx]; }
	ITEM_INFO & operator[](size_t idx) { return m_list[idx]; }

	void operator=( const ItemInfoList & rhs );

	const List& get_list() const { return m_list; }

private:
	List m_list;
};








/*********************************/
//�ڵ� ��ȯ
struct ItemInfoConvert
{
	::ITEM_INFO m_info;

	/*********************************/
	ItemInfoConvert(const ::ITEM_INFO & info)	{ m_info = info;	}
	ItemInfoConvert(::T_ItemID  itemid)	{	m_info.m_TItemID = itemid;	m_info.m_TItemDBIdx = 0; }

	/*********************************/
	ItemInfoConvert& operator= (const ::T_ItemID itemid)	{	m_info.m_TItemID = itemid;	m_info.m_TItemDBIdx = 0;	}
	ItemInfoConvert& operator= (const ::ITEM_INFO & info)	{	m_info = info;	}

	/*********************************/
	item_def::Category get_category() const;
	item_def::SideCategory get_side_category() const;

	item_def::WeaponType get_weapon_type() const;
	item_def::WeaponComboType get_weapon_combo_type() const;

	UINT32 get_chara_type() const;

	item_def::SKINTYPE	get_skin_type() const;

	item_def::PartsType get_parts_type() const;
	item_def::PartsType get_beret_type() const;

	item_def::ItemType get_item_type() const;


	/*********************************/
	UINT32 get_shop_slot_idx() const;
	UINT32 get_char_slot_idx() const;

	bool is_default_item() const;
	bool is_default_item_to_DBidx(const T_ItemDBIdx itemdbidx ) const;
	bool is_default_parts(T_ItemID chara_item_id) const;	//���� ĳ���� ���� �⺻ ���� ������ �Ǵ��Ѵ�.
};

#endif
