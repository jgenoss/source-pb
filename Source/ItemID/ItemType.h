#ifndef __ITEMTYPE_H
#define	__ITEMTYPE_H

#include "ItemTypeWeaponClass.h"
#include "ItemTypeCharaClass.h"
#include "GoodsType.h"

//////////////////////////////////////////////////////////////////////////////////////
// ItemID 
// 429 49 67 295 : �ִ밪
// 428 99 99 999 : ����밪
// ItemID : 4byte
// ���� 1byte (FF) 
//  1��  -> Ŭ�����뿹��ó�����, 
//  10�� -> �̺�Ʈ ������ Ÿ��
//
// ���� 3byte(FFFFFFFF) : ���� ������ ���̵�
//  99 : ItemType : ������ ITEM_CLASS_TYPE + USAGE_TYPE (������ �ǹ̷� �����)
//  99 : ItemClass : ������ CLASS_TYPE
// 999 : ItemIdx

// ������ ���� ����
//MAKE_ITEM_FLAG(ITEM_EVENT_TYPE, ITEM_TYPE, WEAPON_CLASS_TYPE,    WEAPON_IDX)
//MAKE_ITEM_FLAG(ITEM_EVENT_TYPE, ITEM_TYPE, CHARACTER_CLASS_TYPE, Chara_IDX)
//MAKE_ITEM_FLAG(ITEM_EVENT_TYPE, ITEM_TYPE, CASHITEM_CLASS_TYPE,  CASHITEM_GROUP_TYPE )

#define MAKE_ITEM_FLAG(event,type,class,number)			(((event)*10000000)+((type)*100000)+((class)*1000)+(number))

inline UINT32 GET_ITEM_TIME( T_ItemID ItemID) //������ ���̵𿡼� ��¥(�ð�)�� �̾Ƴ���.
{
	return (((ItemID) % 100000) / 1000);
}

//
//	Item class
//
// ������ ���̵� ù��°�� ���ϴ�. ( 2 )
//
// ��ȿ �� ���� : 0~99
// ������ ���̵𿡼� ����Ǵ� �ڸ��� : 
// 9900000
//~ 100000
//
//////////////////////////////////////////////////////////////////////////////////////
// ���� ������
//
//enum EQMIPEMENT_SLOT
//{
//	EQMIPEMENT_SLOT_PRIM			= ITEM_TYPE_PRIMARY,
//	EQMIPEMENT_SLOT_SUB				= ITEM_TYPE_SECONDARY,
//	EQMIPEMENT_SLOT_MELEE			= ITEM_TYPE_MELEE,
//	EQMIPEMENT_SLOT_THROWING1		= ITEM_TYPE_THROWING1,
//	EQMIPEMENT_SLOT_THROWING2		= ITEM_TYPE_THROWING2,
//
//	EQMIPEMENT_SLOT_CHAR			= ITEM_TYPE_CHARA,			// ĳ����
//	EQMIPEMENT_SLOT_CHAR_HEADGEAR	= ITEM_TYPE_HEADGEAR,		// HeadGear
//	EQMIPEMENT_SLOT_CHAR_FACEGEAR	= ITEM_TYPE_FACEGEAR,		// FaceGear
//	EQMIPEMENT_SLOT_CHAR_UPPER		= ITEM_TYPE_UPPER,			// ����
//	EQMIPEMENT_SLOT_CHAR_LOWER		= ITEM_TYPE_LOWER,			// ����
//	EQMIPEMENT_SLOT_CHAR_GLOVE		= ITEM_TYPE_GLOVE,			// �尩
//	EQMIPEMENT_SLOT_CHAR_BELT		= ITEM_TYPE_BELT,			// ��Ʈ
//	EQMIPEMENT_SLOT_CHAR_HOLSTER	= ITEM_TYPE_HOLSTER,		// ������
//
//	EQMIPEMENT_SLOT_CHAR_ITEM		= ITEM_TYPE_ITEM,
//	EQMIPEMENT_SLOT_CHAR_DINO		= ITEM_TYPE_DINO,
//};

enum EQUIPMENT_SLOT
{
	EQUIPMENT_SLOT_NONE			= -1,

	// Weapon Slots
	EQUIPMENT_SLOT_PRIMARY		= 0,
	EQUIPMENT_SLOT_SECONDARY	,
	EQUIPMENT_SLOT_MELEE		,
	EQUIPMENT_SLOT_THROWING1	,
	EQUIPMENT_SLOT_THROWING2	,

	// Parts Slots
	EQUIPMENT_SLOT_CHARA		,
	EQUIPMENT_SLOT_HEAD			,
	EQUIPMENT_SLOT_FACE			,
	EQUIPMENT_SLOT_UPPER		,
	EQUIPMENT_SLOT_LOWER		,
	EQUIPMENT_SLOT_GLOVE		,
	EQUIPMENT_SLOT_BELT			,
	EQUIPMENT_SLOT_HOLSTER		,
	EQUIPMENT_SLOT_ITEM			,

	
	EQUIPMENT_SLOT_COUNT		,

	EQUIP_SLOT_WEAPON_COUNT		= EQUIPMENT_SLOT_THROWING2 - EQUIPMENT_SLOT_PRIMARY + 1,
	EQUIP_SLOT_PARTS_COUNT		= EQUIPMENT_SLOT_COUNT - EQUIP_SLOT_WEAPON_COUNT,
};

//////////////////////////////////////////////////////////////////////////
#endif