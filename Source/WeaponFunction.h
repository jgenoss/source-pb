#pragma once

//
//  WeaponBase�� ����Լ��� �ʹ����� ������, ���߿� �Ϲ��Լ��� ���� �ִٰ� �ǴܵǴ� ���� 
//  �Ϲ� �Լ��� �и��ϰ�, ��ƿȭ�Ѵ�..
//

namespace WeaponFunction 
{
	INT8	FindAIStateIndex(WeaponBase* pWeapon, const char* aiName);	

	bool	PlayWeaponAnim( WeaponBase* pWeapon, INT8 aiStateIdx, REAL32 rTimeScale = 1.f);
	bool	PlayWeaponAnimDirect( WeaponBase* pWeapon, INT8 aiStateIdx, REAL32 rTimeScale = 1.f);
	
	void	PlayWeaponAnim(WeaponBase* pWeapon, const char * pszName);
	
	void	SetCollideeGroup_NoUseCollidee(WeaponBase* pWeapon);
	void	SetCollideeGroup_NormalWeapon(WeaponBase* pWeapon);
	void	SetCollideeGroup_HitWeapon(WeaponBase* pWeapon);
	void	SetCollideeGroup_DinoDummy(WeaponBase* pWeapon);
	void	SetCollideeGroup_SentryGun(WeaponBase* pWeapon);
	void	SetCollideeGroup_Supply(WeaponBase* pWeapon);

}

namespace WF = WeaponFunction;