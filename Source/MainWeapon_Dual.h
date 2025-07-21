#pragma once

// Dual�� ���ؼ��� �ͽ��ټ� �Ӽ����� �и��Ϸ��� ������, ���� Dual�Ļ� Ŭ�������� WeaponBase �����Լ� ���յ��� ���� ���ؼ�
// �ϴ���, MainWeapon�� �Ļ� ī�װ��� ���� �۾��Ѵ�..

// WeaponDualCIC , WeaponDualHandGun , WeaponDualKnife , WeaponDualSMG �� �� WeaponDual�� ���� �Ļ�Ŭ�����̰�

// WeaponCrossFire�� WeaponDualHandGun�� �Ļ�.
// WeaponKnuckle�� WeaponDualKnife�� �Ļ�.
// WeaponDualUzi�� WeaponDualSMG�� �Ļ�.

//
// WeaponKrissSuperV�� ���, ������ü�� �̱��ε�....���ο��� ��� ������ ���� �����ϴ� ���̽���....
// ����, �����Ϸ���, WeaponKrissSuperV�� �Բ� �����ؼ� �����ϰ�, WeaponKrissSuperV�� ���� ��κ� ���� �����ؾ��Ѵ�.
//

#include "MainWeapon.h"

class MainWeapon_Dual : public MainWeapon 
{
public:
	WeaponBase *	getLeftWeapon( void)					{ return m_pLeftHandWeapon; }
	void			setLeftWeapon_Null()					{ m_pLeftHandWeapon = nullptr; }			// �ӽ÷� �ۼ���..(���߿� �ٽ� �����丵�Ұ�)
protected:
	MainWeapon_Dual(WeaponBase* p);
	virtual void	ReturnToReady() override;
	virtual void	OnCreate( bool bCreateInWeapon) override;							// �Ļ�Ŭ�����θ� �����ɼ� �ְ� ����..
private:

	virtual void	ReturnInstance() override;
	virtual void	OnSetBlendAlpha( REAL32 val) override;
	virtual void	OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) override;

	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;

	virtual void	OnChangeFPSView( void) override;
	virtual void	OnChangeObserverView( void) override;


	virtual void	SetOwner( CGameCharaBase * pOwner) override;

	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f) override;
	
	// ȣ����� �ʽ��ϴ�. 2017.03.16. soon9
	/*virtual void	PlayAnim_UI_IdleA( void);
	virtual void	PlayAnim_UI_IdleB( void);*/

	virtual bool	IsWeaponDualDerivedClass() const override { return true; }

public:
	void	NET_LoadMagazineDualGun( CHARA_VIEW_TYPE eCAT);
	void	NET_ChangeDualGun( CHARA_VIEW_TYPE eCAT, REAL32 rTimeScale );

protected:
	WeaponBase*		m_pLeftHandWeapon;
};
