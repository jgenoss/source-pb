#if !defined( __WEAPON_DUAL_H__)
#define __WEAPON_DUAL_H__

#include "WeaponBase.h"

class CGameCharaBase;

class WeaponDual : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponDual, WeaponBase);
protected:
	WeaponBase *	m_pLeftHandWeapon;
public:
	WeaponBase *	getLeftWeapon( void)					{ return m_pLeftHandWeapon; }
	WeaponDual();
public:

	virtual ~WeaponDual();

	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;			// WeaponDualUzi�� ��� dual �⺻Ŭ���� Create()�� ���� ����, WeaponDualUziSilencer�� ���, WeaponDualUzi ����� �⺻Ŭ�����Լ���� ó��
																			// ��Ŭ�� ��쵵 dual�⺻Ŭ������ ���� ����...
	virtual void	OnSetBlendAlpha( REAL32 val) override;
	virtual void	OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) override;

	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;

	virtual void	ReturnInstance( void) override;					// WeaponDualUzi�� ��� dual �⺻Ŭ���� ReturnInstance�� ���� ����...��Ŭ�� ��� dual�⺻Ŭ���� ���� ����...

	virtual void	OnChangeFPSView( void) override;
	virtual void	OnChangeObserverView( void) override;

	virtual void	ReturnToReady( void) override;

	virtual void	OnDrop( void) override;
	virtual void	SetOwner( CGameCharaBase * pOwner) override;

	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f) override;

protected:
	void	NET_LoadMagazineDualGun( CHARA_VIEW_TYPE eCAT);
	void	NET_ChangeDualGun( CHARA_VIEW_TYPE eCAT, REAL32 rTimeScale );

	// UI weapon animation
	virtual void	PlayAnim_UI_IdleA( void) override { Reset();	}
	virtual void	PlayAnim_UI_IdleB( void) override { Reset();	}				// WeaponDualKnife�� ��� ������ ������̹Ƿ� ���߿� �ݿ��ؾ���..
};

#endif

