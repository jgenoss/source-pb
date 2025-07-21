#if !defined( __UI_ItemInfo_Def_H__)
#define __UI_ItemInfo_Def_H__

namespace item_def {

/*
weapon : all, primary(combo), secondary, melee, throw, special
chara : all, man, woman, skinny, g2
parts : headgear, facegear, top, bottom, glove, belt, holster
item : all, enhanced, feature, reward, box, set
*/
enum Category
{
	CTG_NA = -1,		//�ʱⰪ

	CTG_WEAPON,
	CTG_CHARA,
	CTG_PARTS,
	CTG_SKIN,
	CTG_ITEM,
	CTG_NEWHOT,

	CTG_MAX,
};

enum SideCategory
{
	SCTG_NA = -1,		//�ʱⰪ

	SCTG_WEAPON,	//�����
	SCTG_PARTS,	//��ű���

	SCTG_MAX,
};

enum WeaponType
{
	WT_NA = -1,
	WT_ALL = -1,

	WT_PRIMARY,	//���� �޺��� ����.
	WT_SECONDARY,	
	WT_MELEE,	
	WT_THROWING,
	WT_ITEM,

	WT_MAX,
};

enum WeaponComboType
{
	WCT_NA = -1,

	WCT_PRIMARY,
	WCT_ASSAULT,	
	WCT_SMG,	
	WCT_SNIPER,	
	WCT_MACHINE,
	WCT_SHOTGUN,
	
	WCT_MAX,
};

enum MASKNEWHOT
{
	MASKNEWHOT_NA = -1,

	MASKNEWHOT_ALL,
	MASKNEWHOT_WEAPON,
	MASKNEWHOT_CHARA,
	MASKNEWHOT_PARTS,
	MASKNEWHOT_SKIN,
	MASKNEWHOT_ITEM,

	MASKNEWHOT_MAX,
};

enum CharaType
{
	CT_NA = -1,
	CT_ALL =-1,

	CT_MAN,
	CT_WOMAN,

	CT_MAX,
};

enum PartsType
{
	PT_NA = -1,
	PT_ALL = -1,
		
	PT_HEADGEAR = 0,
	PT_FACEGEAR,
	PT_UPPER,	
	PT_LOWER,
	PT_GLOVE,
	PT_BELT,
	PT_HOLSTER,
	PT_SKIN,
	PT_BERET,

	PT_MAX,

	PT_DINO = 0,	//0���� �ؾ� �ȴ�.
};

// ItemLineUp  --> ���̹��� ������ ���� enum ����Ѱɷ� �ٲߴϴ�..
// PartsType_ShopInven
enum PartsType_ShopPreview
{
	PartsType_ShopPreview_None = -1,
	PartsType_ShopPreview_Headgear,
	PartsType_ShopPreview_Facegear,
	PartsType_ShopPreview_Upper,
	PartsType_ShopPreview_Lower,
	PartsType_ShopPreview_Glove,
	PartsType_ShopPreview_Belt,
	PartsType_ShopPreview_Holster,
	PartsType_ShopPreview_Max
};

enum SKINTYPE
{
	ST_NA	= -1,
	ST_ALL	= -1,
	ST_COMMON,
	ST_MAN,
	ST_WOMAN,
	ST_MAX
};

enum ItemType
{
	IT_NA = -1,
	IT_ALL = -1,

	IT_ENHANCE,
	IT_FEATURE,
	IT_REWARD,
	IT_BOX,
	IT_SET,

	IT_MAX,
};



extern ITEM_INFO NULL_ITEM_INFO;

} //end of namespace  item_def


enum ItemStatus
{
	ITEM_STATUS_NOHAVE,			// �̺���.
	ITEM_STATUS_EQUIPPED,	//������ ������
	ITEM_STATUS_USED,	// �����( �Ⱓ���̸�....���������� �ƴ� ���)
	ITEM_STATUS_HAVE,	// ������ Ȥ�� �̻�� �Ⱓ��.. (�̹� �űԱ�ȹ�� �ǵ��� ����)	
	ITEM_STATUS_NOTUSED, //�� ���
	NUM_ITEM_STATUS,
};

enum ShopItemCategory
{
	SHOP_ITEM_CATEGORY_WEAPON,
	SHOP_ITEM_CATEGORY_CHARACTER,
	SHOP_ITEM_CATEGORY_ITEM,
	NUM_SHOP_ITEM_CATEGORY,
};

enum UIShop_Item_Category
{
	UISHOP_ITEM_INVALID = -1,
	UISHOP_ITEM_ENHANCED = 0,
	UISHOP_ITEM_FUNCTIONAL,
	UISHOP_ITEM_COMPENSATION,
	UISHOP_ITEM_BOX,
	UISHOP_ITEM_MAX,
};


#endif
