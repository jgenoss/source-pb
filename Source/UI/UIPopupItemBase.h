#if !defined( __UI_POPUP_ITEMBASE_H__)
#define __UI_POPUP_ITEMBASE_H__

#include "UIPopupBase.h"
#include "Useable_Item_Def.h"

class UIUseItem;

class UIPopupItemBase : public UIPopupBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPopupItemBase, UIPopupBase);

public:
	UIPopupItemBase();
	virtual ~UIPopupItemBase();

	/*
	param[in] pArg1 INT32*
	param[in] pArg2 USE_ITEM_POPUP_TYPE*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

protected:
	USE_ITEM_POPUP_TYPE m_Type;

	T_ItemID			m_ItemID;
	T_ItemDBIdx			m_AuthWareDBIndex;
	
protected:
	/** \brief EntranceStart�� ȣ��Ǹ� ItemID�� ���� ������ �����ϰ� �˴ϴ�.
		\desc UI�� ������ �����Ϸ��� �̰��� define�Ͻʽÿ�.
			LuaStack count 1�� ������ ����(ItemName�� ������)
			*/
	virtual LuaState * _SetItemInfo( void);
};

#endif
