#if !defined( __GAME_CHARA_WEAPON_CONTROL_3PV_NPC_H__)
#define __GAME_CHARA_WEAPON_CONTROL_3PV_NPC_H__

#include "GameCharaWeaponControl3PV.h"

class CGameCharaWeaponControl3PVNPC : public CGameCharaWeaponControl3PV
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl3PVNPC, CGameCharaWeaponControl3PV);
protected:
public:
	CGameCharaWeaponControl3PVNPC();
	virtual ~CGameCharaWeaponControl3PVNPC();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void	OnUpdate( REAL32 rDeltatime) {}

	// ���� ����(WeaponManager���� �޾ƿɴϴ�.)
	virtual bool	OnCreateWeapon( INT32 nSlot, INT32 Itemid) { return false;}
	// ���� ����(WeaponManager�� ��ȯ�մϴ�.)
	virtual void	OnDeleteWeapon( INT32 nSlot) {}
	// ���⸦ ĳ���Ϳ� ���Դϴ�.
	virtual WeaponBase * OnAttachWeapon( INT32 nSlot, i3Node * pSceneNode, i3GameNode * pGameNode) { return NULL;}
	// ���⸦ ĳ���Ϳ��� ������ϴ�.
	virtual WeaponBase * OnDetachWeapon( INT32 nSlot) { return NULL;}

	// ���� �ʱ�ȭ
	virtual void	OnResetWeapon( void)													{}

	// � �ֹ��⸦ ���Դϴ�.
	virtual bool	OnAttachPurchasePrimWeapon( INT32 nSlot)								{ return false; }
	// � ���� �ֹ��⸦ ������ϴ�.
	virtual void	OnDetachPurchasePrimWeapon( void)										{}
	// �������� �������⸦ ���Դϴ�.
	virtual bool	OnAttachPurchaseSideWeapon( INT32 nSlot)								{ return false; }
	// �������� ���� �������⸦ ������ϴ�.
	virtual void	OnDetachPurchaseSideWeapon( void)										{}

	// �ش� ���� ����� ��ü(���� �����ӿ����� ��ü���� �ʽ��ϴ�.)
	virtual bool	OnPreSelectWeapon( INT32 nSlot)		{ return false; }
	// �ش� ���� ����� ��ü(���� �����ӿ��� ��ü�˴ϴ�.)
	// return true �ش� ����� ��ü ����
	virtual bool	OnSelectWeapon( INT32 nSlot, bool bPlaySound = true)				{ return false; }

	// 1/3��Ī�� ���� ���� ���μ��� ó��
	virtual bool	OnChangeViewWeapon( void)												{ return false; }

	// Drop
	virtual bool	OnWeaponDropToWorld( INT32 nSlot, VEC3D * pPos, bool bChangeNextWeapon = true)	{ return false; }
	virtual bool	OnWeaponTake( WeaponBase * pWeapon)										{ return false; }
	virtual bool	OnDropedWeaponSearching( REAL32 rDeltatime)								{ return false; }

	virtual bool	OnFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL)			{ return false; }
	virtual bool	OnSecFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL)			{ return false; }
	virtual bool	OnFireReady( void)														{ return false; }

	virtual bool	OnZoom( void)															{ return false; }
	virtual bool	OnExtActivate( void)													{ return false; }
	virtual bool	OnExtDeactivate( void)													{ return false; }
	virtual bool	OnLoadMagazine( void)													{ return false; }
	virtual bool	OnLoadMagazineDual( void)												{ return false; }
	virtual bool	OnLoadBullet( void)														{ return false; }

	virtual bool	OnRemoveShell( void)													{ return false; }

	// ĳ���� �������� ���� ȣ��˴ϴ�.
	// �������� 0�� ����� ��ü�ϴ� �����Ѵ�.
	virtual bool	OnProcessRespawn( void)													{ return false; }

};

#endif
