#pragma once

//
//  �� Ŭ������ ����Ŭ���� WeaponDualUzi�� ���缭 �ϴ� �ۼ��Ǿ�����...���߿� Ŭ������ü�� ���ŵǾ���Ѵ�.
//

#include "MainWeapon_DualSMG.h"

class MainWeapon_DualUzi : public MainWeapon_DualSMG
{
public:
	MainWeapon_DualUzi(WeaponBase* p);

protected:
	virtual void		OnCreate(bool bCreateInWeapon) override;
	// ������ �𵨸��� ���� ����ó����
	i3::vector<i3Node*>	m_SilencerMaterialList;
private:
	virtual void		ReturnInstance() override;

};

