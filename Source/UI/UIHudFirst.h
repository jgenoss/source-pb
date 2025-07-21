#pragma once

#include "iHudBase.h"

class UIHudManager;

//�� Ŭ������ �ƹ��� �ൿ�� ���� �ʴ´�..
//�ٸ� ��� hud �� �������� ȣ�� �Ǵ� Ŭ���� �̴�.
class UIHudFirst : public iHudBase
{
public:
	UIHudFirst(UIHudManager* p);
	virtual ~UIHudFirst();

private:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;

private:
	void event_start_battle(INT32 arg, const i3::user_data& UserData);
	void event_m_roundstart_battle(INT32 arg, const i3::user_data& UserData);
	void event_respawn_battle(INT32 arg, const i3::user_data& UserData);
};