#pragma once

#include "MainWeapon.h"

struct WeaponThrowData
{
	VEC3D			m_vTracerOldPos;
	VEC3D			m_vOldPos;		//���� ��ġ
	VEC3D			m_vStartPos;	//���� ��ġ
	VEC3D			m_vDirection;	//����

	REAL32			m_rSpeed;		//�ӵ�
	REAL32			m_rElapsedThrowTime; // �������� �� ��� �ð�
	REAL32			m_rTracerUpdateTime;

	UINT32			m_BombState;

	bool			m_bCheckThrow;	// ���������� üũ
};

void	Reset_WeaponThrowData(WeaponThrowData* outData);


class MainWeapon_Throw : public MainWeapon 
{
public:
	MainWeapon_Throw(WeaponBase* p);




////
	UINT32			getBombState(void) const								{ return m_ThrowData.m_BombState; }
	void			addBombState( UINT32 mask)								{ m_ThrowData.m_BombState |= mask; }
	void			removeBombState( UINT32 mask)							{ m_ThrowData.m_BombState &= ~mask; }
	bool			isBombState( UINT32 mask) const							{ return (m_ThrowData.m_BombState & mask) != 0; }
	
	bool			isThrowing(void) const									{ return isBombState( WEAPON_BOMBSTATE_THROWING); }
	bool			isSettled(void)	const									{ return isBombState( WEAPON_BOMBSTATE_SETTLED); }

	bool			isLaunching(void)										{ return isBombState( GRENADESHELL_STATE_LAUNCH); }
	bool			isMissing(void)											{ return isBombState( GRENADESHELL_STATE_MISFIRE); }

	virtual void	OnInitVariable() override;
	virtual void	Reset() override;

	// �ϴ� �Ʒ� �Լ� 2���� ����ź������ �����ϵ��� ȣ����ġ�� ����ź������ �̵��� �����̴�..
	// �׷����� ���⿡ ������ �ȵɰ� ����...�̵��̾����� Grenade�� ���̾�� ���δ�..
	void			OnFire_Grenade( i3AIContext * pCtx, REAL32 tm);

	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;

	void			CheckChainExplosion(VEC3D * pStartPos, REAL32 rRange, REAL32 rRadian = I3_PI);
	virtual void	ChainExplosion(void)	{ }
	virtual VEC3D*	GetThrowInfo_Normal()	{ return nullptr; }

protected:

	WeaponThrowData		m_ThrowData;

	void			UpdateTraceEffect(REAL32 fElapsedTime);

private:		
	virtual	bool	_CheckThrow(REAL32 LocalTime) { return false; }
	

	INT32				m_nTraceEffectTickCount;

};

