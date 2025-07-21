#if !defined( __UI_HUD_BASE_H__)
#define __UI_HUD_BASE_H__

#include "UIBase.h"

class UIHUDBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIHUDBase, UIBase);

protected:
	/** \brief �ε�� �ʱ�ȭ
		\desc HUD�� ��� �̸� �ε��� �ϱ� ������ Texture�� �������� ���ƾ� �Ѵ�.
			�������� �ڵ����� ���������� �̸� �����ش�.*/
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

	/** \brief ��ε�� �ʱ�ȭ
		\desc Stage ������ Texture�� �ڵ����� ������ �� �ֵ��� ���ش�.*/
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIHUDBase();
	virtual ~UIHUDBase();

	virtual void	ProcessGameEvent( GAME_EVENT Event, INT32 Arg, const i3::user_data& UserData) override {}

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	ClearData() {}

};

#endif
