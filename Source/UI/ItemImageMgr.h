#if !defined( __PB_ITEM_IMAGES_H__)
#define __PB_ITEM_IMAGES_H__

#include "UIImgSetCtrl.h"

enum PBITEM_IMAGE_TYPE 
{ 
	PBITEM_WEAPON, 
	PBITEM_CHARACTER, 
	PBITEM_PARTS, 
	PBITEM_ITEM, 
	NUM_PBITEM, 
};

class ItemImageMgr_Init 
{
public:
	ItemImageMgr_Init();
	~ItemImageMgr_Init();

	void	AddWeaponTemplate( const char* szTemplate);

	// �Ʒ� sub_category�� pef�� UITemplate�� ������. 2014.05.30.
	void	AddCharacterTemplate(INT32 sub_category, INT32 looplood, const char* szTemplate);
	void	AddItemTemplate( INT32 sub_category, const char* szTemplate );
	void	AddPartsTemplate( INT32 sub_category, INT32 looplood, const char* szTemplate );

private:
	struct SubCategory	{	i3::vector<i3UITemplate*> m_tpl_list;	};	// �ܼ��� ������ �����̹Ƿ� ���ٴ� �����Ҵ��� ����..
	struct MainCategory	{	~MainCategory();	i3::vector<SubCategory*> m_sub_category_list; };

	void				_AddTemplate(MainCategory* main_category, INT32 sub_category, i3UITemplate* tpl);		 // �����ƾ..
	const MainCategory*	GetMainCategory(INT32 category) const { return &m_main_categorys[category];  }

	MainCategory		m_main_categorys[NUM_PBITEM];
	friend class ItemImageMgr;
};

class ItemImageMgr
{
public:
	ItemImageMgr();
	~ItemImageMgr();
	
	void				Initialize(const ItemImageMgr_Init& init);					// ���� �����Լ�..
	UIImgSetCtrl*		CreateImgSetCtrl(i3UIButtonImageSet* bound_rsc) const;
	UIImgSetCtrl*		CreateImgSetCtrl(i3UIScene* scn, const char* bound_rsc) const;
	void				SelectImage( UIImgSetCtrl* ctrl, const SHOP_GOODS* goods) const;	// ��Ʈ�������� itemID�� ������� �ʱ� ������ �Լ��� ����..
	void				SelectImage( UIImgSetCtrl* ctrl, T_ItemID itemID) const;		// itemID�ε� ó���� �ǵ��� �ٽ� �Լ��߰�.

	//���ø� Ÿ���� �̿��ؼ� �ƹ��� ó���Ѵ�.
	//SelectImage �� �Լ��� �������� usage(GET_ITEM_SUBTYPE) type�� ���� ���ø��� �ƴ� ����� ���� ���ø� ��밡��
	void SelectImage_UseTemplate(UIImgSetCtrl* ctrl, T_ItemID itemID, INT32 template_type) const;

private:
	void				CalcIndexes( INT32& outTplIdx, INT32& outImageIdx, const SHOP_GOODS* goods) const;
	void				CalcIndexes(  INT32& outTplIdx, INT32& outImageIdx, T_ItemID itemID) const;
	void				CalcIndexes_Impl(INT32& outTplIdx, INT32& outImageIdx, PBITEM_IMAGE_TYPE image_type, INT32 image_sub_type, INT32 shape_idx, INT32 tex_index) const;

	struct SubCategoryIdx {	i3::vector<INT32>	m_num_idx_list;		INT32 m_base_tpl_idx;	};
	struct MainCategoryIdx {	~MainCategoryIdx();	i3::vector<SubCategoryIdx*>	m_sub_category_idx_list;	};
	MainCategoryIdx		m_idx_offset_list[NUM_PBITEM];	// 
	UIImgSetTplGroup	m_tpl_group;		// 
};

/// �� ��Ű�� ������, �켱 ���̾ �̱��� ó��.. (UIMainFrame�� ����� ������, ���� ���Ǹ� �ʹ� �ø��� �ʴ°� ������ �ϴ�..)

inline ItemImageMgr*	GetItemImageMgr()			// �ζ��� Ű���带 �ֱ�� ������, �ּ��� static ����κ��� �ζ��εǼ��� �ȵȴٴ� ������ ���� �ִ�...
{
	static ItemImageMgr s_mgr;
	return &s_mgr;
}


#endif