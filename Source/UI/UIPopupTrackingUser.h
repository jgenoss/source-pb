#if !defined( __UI_POPUP_TRACKINGUSER_H__)
#define __UI_POPUP_TRACKINGUSER_H__

#include "UIPopupInteractBase.h"

// �� Ŭ���� ���� ���̰� ���� ������ �ϴ�.. (2014.09.01.����)

class UIPopupTrackingUser : public UIPopupInteractBase
{
	I3_CLASS_DEFINE( UIPopupTrackingUser, UIPopupInteractBase);

public:
	UIPopupTrackingUser();
	virtual ~UIPopupTrackingUser();

	virtual void	OnCreate( i3GameNode * pParent) override;
};

#endif
