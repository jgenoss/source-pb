/**
 * \file	Character\GameCharaWeaponControl1PV.h
 *
 * Declares the game chara weapon control 1 pv class.
 */


#if !defined( __GAME_CHARA_WEAPON_CONTROL_1PV_H__)
#define __GAME_CHARA_WEAPON_CONTROL_1PV_H__

#include "GameCharaWeaponControl.h"

class CGameCharaWeaponControl1PV : public CGameCharaWeaponControl
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl1PV, CGameCharaWeaponControl);
public:
	virtual bool	OnCreateWeapon( WEAPON_SLOT_TYPE nSlot, T_ItemID Itemid, UINT8 nMultiWeaponSync = 0) override;
	virtual WeaponBase * OnAttachWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode) override;
	virtual WeaponBase * OnDetachWeapon( WEAPON_SLOT_TYPE nSlot) override;

	virtual bool	OnChangeViewWeapon( void) override;

	/** \brief � �ֹ��⸦ ���Դϴ�.
		\note 1��Ī������ ���Ⱑ ���̸� �ȵ˴ϴ�. */
	virtual bool	OnAttachPurchasePrimWeapon( WEAPON_SLOT_TYPE nSlot) override;

	/** \brief ���� ���⸦ �������� ���Դϴ�. */
	virtual bool	OnAttachPurchaseSideWeapon( WEAPON_SLOT_TYPE nSlot) override;

	virtual bool	OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound = true) override;

	virtual bool	OnZoom( void) override;
	virtual bool	OnExtActivate( void) override;
	virtual bool	OnExtDeactivate( void) override;
	virtual bool	OnRemoveShell( void) override;
	

	virtual bool	OnProcessRespawn( void) override;
};

#endif
