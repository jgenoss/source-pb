#if !defined( __UI_POPUP_REMOVEFRIEND_H__)
#define __UI_POPUP_REMOVEFRIEND_H__

#include "UIPopupInteractBase.h"

// �� Ŭ���� ���� ���̰� ���� ������ �ϴ�...(2014.09.01.����)

class UIPopupRemoveFriend : public UIPopupInteractBase
{
	I3_CLASS_DEFINE( UIPopupRemoveFriend, UIPopupInteractBase);

public:
	UIPopupRemoveFriend();
	virtual ~UIPopupRemoveFriend();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnOKButton( void) override;
};

#endif
