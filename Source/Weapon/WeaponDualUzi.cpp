#include "pch.h"
#include "WeaponDualUzi.h"
#include "GlobalVariables.h"
#include "GameCharaWeaponContext.h"

#include "MainWeapon_DualUzi.h"

I3_CLASS_INSTANCE( WeaponDualUzi);//, WeaponDualSMG);

WeaponDualUzi::WeaponDualUzi()
{
}


WeaponDualUzi::~WeaponDualUzi()
{
}

MainWeapon*	WeaponDualUzi::CreateMainWeapon()
{
	MainWeapon_DualUzi * p = new MainWeapon_DualUzi(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualUzi));
	return p;
}

// ���⼭, ReturnInstance�Լ��� ���ŵɼ��� ����.. (WeaponDual�� �Լ��� �������� �ȵ�)

/*virtual*/ void WeaponDualUzi::ReturnInstance()
{
	WeaponBase::ReturnInstance();						// WeaponDual::ReturnInstance�� �ǳʶڴ�..(�߸��� �������)
}

