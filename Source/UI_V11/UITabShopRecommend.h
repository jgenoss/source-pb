#if !defined( __UI_TAB_SHOPRECOMMEND_H__)
#define __UI_TAB_SHOPRECOMMEND_H__

#include "UI_V11/UITabShopBase.h"

//���ʿ� ������ �ȵǾ� �ִ� ������ �ϴ� ������ �Ϸ� 2015-05-21 ��뿵

class UIToolTipShop;

class UITabShopRecommend : public UITabShopBase
{

public:
	UITabShopRecommend(UIPhaseSubBase* phase, UIToolTipShop* pTTWeapon, UIToolTipShop* pTTNonWeapon);
	virtual ~UITabShopRecommend();
	
};

#endif
