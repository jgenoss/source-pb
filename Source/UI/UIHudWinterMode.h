#pragma once

#include "iHudBase.h"

//��忡�� �ش� ������ ���� ���� Ŭ����

class UIHudManager;

class UIHudWinterMode : public iHudBase
{

private:
	i3UIButtonImageSet*		Icon;
	i3UIButtonImageSet*		Icon1;
	i3UIButtonImageSet*		Icon2;

public:
	UIHudWinterMode(UIHudManager* p);
	virtual ~UIHudWinterMode();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

};