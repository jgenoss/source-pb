#pragma once

#include "iHudBase.h"

class UIHudManager;

//�� Ŭ������ �ƹ��� �ൿ�� ���� �ʴ´�..
class UIHudNone : public iHudBase
{
public:
	UIHudNone(UIHudManager* p) : iHudBase(p) {}
	virtual ~UIHudNone() {}

	virtual bool IsEnable() const override { return false; }
	virtual bool IsVisible() const override { return false; }

private:
	virtual void InitializeAtLoad( i3UIScene * pScene) override {}
};