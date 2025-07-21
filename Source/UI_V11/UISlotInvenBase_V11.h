/**
 * \file	UI_V11\UISlotInvenBase_V11.h
 *
 * Declares the user interface slot inven base class.
 */


#if !defined( __UI_SLOT_INVENITEM_H__)
#define __UI_SLOT_INVENITEM_H__

#include "UI/UISlotBase.h"

// ������ Template�� Shape index�Դϴ�.
// shape�� ����Ǹ� ���⵵ ����Ǿ� ��..
enum UI_INVEN_CATEGORY
{
	UI_INVEN_CATEGORY_NONE = -1,
	UI_INVEN_CATEGORY_PRIM,
	UI_INVEN_CATEGORY_SEC,
	UI_INVEN_CATEGORY_MELEE,
	UI_INVEN_CATEGORY_THROW,
	UI_INVEN_CATEGORY_SPECIAL,
	UI_INVEN_CATEGORY_CHARA,
	UI_INVEN_CATEGORY_HEAD,
	UI_INVEN_CATEGORY_TERMITEM,
	UI_INVEN_CATEGORY_COUNTITEM,
	UI_INVEN_CATEGORY_DINOSOUR,

	UI_INVEN_CATEGORY_ASSAULT,
	UI_INVEN_CATEGORY_SMG,
	UI_INVEN_CATEGORY_SNIPER,
	UI_INVEN_CATEGORY_SHOTGUN,
	UI_INVEN_CATEGORY_MACHINGUN,

	UI_INVEN_CATEGORY_MAX
};

enum UI_INVEN_USAGE
{
	UI_INVEN_USAGE_NONE = -1,
	UI_INVEN_USAGE_USE,
	UI_INVEN_USAGE_NOTUSE,
	UI_INVEN_USAGE_EQUIPED,

	UI_INVEN_USAGE_MAX
};

enum UI_INVEN_NEWHOT
{
	UI_INVEN_NEWHOT_NONE = -1,
	UI_INVEN_NEWHOT_HOT,
	UI_INVEN_NEWHOT_NEW,
	UI_INVEN_NEWHOT_EVENT,
	UI_INVEN_NERHOT_PCCAFE,

	UI_INVEN_NEWHOT_MAX
};

enum UI_INVEN_EVENT
{
	UI_INVEN_EVENT_NONE = -1,
	UI_INVEN_EVENT_EVENT,
	UI_INVEN_EVENT_SET,
	UI_INVEN_EVENT_BOX,

	UI_INVEN_EVENT_MAX
};

class UIPhaseInventory_V11;
class UIImgSetCtrl;

class UISlotInvenBase_V11 : public UISlotBase
{
	I3_ABSTRACT_CLASS_DEFINE( UISlotInvenBase_V11, UISlotBase);

protected:
	i3::vector<i3UIButtonImageSet*>	m_ControlList;
	UIImgSetCtrl * m_pImageCtrl;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
	virtual void	_ClearAtUnload( i3UIScene * pScene);

public:
	UISlotInvenBase_V11();
	virtual ~UISlotInvenBase_V11();

	virtual void	OnCreate( i3GameNode * pParent);

	/** \brief Mouse Enter event */
	virtual bool	OnEnterMouse( i3UIControl * pControl);

	/** \brief Mouse leave event */
	virtual void	OnLeaveMouse( i3UIControl * pControl);

	virtual void	OnWheel( i3UIControl * pControl, INT32 zDelta);

	virtual bool	OnClick( i3UIControl * pControl);

	virtual bool	OnSelect( bool bSound = true);
	virtual void	OnUnselect( void);

	UIPhaseInventory_V11 *	GetInventory( void);

	virtual void	SetEmptySlot( void);

	/** \brief Inven ����
		\param[in] pszItemName ������ �̸�
		\param[in] category UI_INVEN_CATEGORY
		\param[in] endurance ������
		\param[in] ImageIdx Image Index
					category�� ���� Index�� �ǹ̴� �ٸ��� ���� �� �ֽ��ϴ�.
		\param[in] pszDay �Ⱓ
		\param[in] extension Extension ��ũ����
		\param[in] usage UI_INVEN_USAGE
		\param[in] newhot UI_INVEN_NEWHOT
		\param[in] event UI_INVEN_EVENT
		*/
	virtual void	SetInvenData( const char * pszItemName, UI_INVEN_CATEGORY category, bool is_repair_item, REAL32 repair_ratio, 
						INT32 ItemID,	const char * pszDay = NULL, bool extension = false,
						UI_INVEN_USAGE usage = UI_INVEN_USAGE_NONE,
						UI_INVEN_NEWHOT newhot = UI_INVEN_NEWHOT_NONE,
						UI_INVEN_EVENT event = UI_INVEN_EVENT_NONE);

	void			SetSlotEnable(bool bEnable);		// ���� enable�� �ƴ� �󽽷� �̹������� ��ü���η� ó��...

};

#endif
