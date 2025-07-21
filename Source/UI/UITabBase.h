#if !defined( __UI_TAB_BASE_H__)
#define __UI_TAB_BASE_H__

#include "UIBase.h"

class UITabBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UITabBase, UIBase);

public:
	UITabBase();
	virtual ~UITabBase();

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override {  }		// �� �Լ��� ���⼭���� ����..UIBase���� �������� �ʴ´�..

	UIMainFrame *	GetMainFrame( void);


};

#endif
