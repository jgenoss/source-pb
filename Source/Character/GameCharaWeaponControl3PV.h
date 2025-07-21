#if !defined( __GAME_CHARA_WEAPON_CONTROL_3PV_H__)
#define __GAME_CHARA_WEAPON_CONTROL_3PV_H__

#include "GameCharaWeaponControl.h"

class CGameCharaWeaponControl3PV : public CGameCharaWeaponControl
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl3PV, CGameCharaWeaponControl);
public:
	virtual bool	OnChangeViewWeapon( void) override;

	// �ش� ���� ����� ��ü(���� �����ӿ��� ��ü�˴ϴ�.)
	// return true �ش� ����� ��ü ����
	virtual bool	OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound = true) override;

	virtual bool	OnZoom( void) override;

	virtual bool	OnProcessRespawn( void) override;
};

#endif
