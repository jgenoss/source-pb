#pragma once

#include "MainWeapon_Grenade.h"

//
// ���Ӽ��� �и��� �����ϴٸ�, �� Ŭ������ �����Ұ���
// �ϴ��� ���ι��� �и� �� �з������� �� Ŭ���� �ۼ���.
//

class MainWeapon_DualGrenade : public MainWeapon_Grenade
{
public:
	MainWeapon_DualGrenade(WeaponBase* p);


	void OnBindResource_DualGrenade();
	void SetOwner_LeftGrenade(CGameCharaBase* pOwner);
	void ReturnInstance_LeftGrenade();
	void	PlayFireAnim( void);
	void	PlayIdleAnim( void);
	WeaponBase* GetLeftWeapon(){ return m_pLeftGrenade; }

	bool	getThrowFirstGrenade() { return m_bCheckThrowFirst; }

private:

	virtual void	OnCreate( bool bCreateInWeapon) override;
	virtual void	OnUpdate_Grenade( REAL32 rDeltaSeconds) override;
	virtual void	Reset() override;
	virtual void	ReturnToReady() override;
	virtual void	FireReady() override;
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner ) override;
	virtual void	DetachNode() override;
	virtual void	OnSetBlendAlpha( REAL32 val) override;
	virtual void	OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) override;
	virtual	bool	_CheckThrow(REAL32 LocalTime) override;




	// ������ ������, ������ ����
	enum GRENADE_TYPE { 
		GRENADE_LEFT = 0, 
		GRENADE_RIGHT = 1,
		GRENADE_MAX
	};

	void	setGrenadeType( GRENADE_TYPE type)						{ m_TypeGrenade = type;		}
	void	AttachToLeftHand(void);
	void	AttachToRightHand(void);

	bool		m_bCheckThrowFirst;

	INT8		m_aiiGrenadeIdle[ GRENADE_MAX];
	INT8		m_aiiGrenadeFire[ GRENADE_MAX];

	GRENADE_TYPE m_TypeGrenade;

	WeaponDualGrenade *			m_pLeftGrenade;	//	������ ���� �𵨸����θ� ���
	
};
