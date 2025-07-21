#pragma once

#include "MainWeapon_Grenade.h"

// �ϴ� ���Ѵ�� ������ ����ϰ� ����ź�� ����Ѵ�..

// ���߿�, ������������ ��ƾ�� ���� �ű��, ����ź��� �������� ��ӹް� �����ؾ��Ѵ�.
class ProcessMeatBombLight;


class MainWeapon_MeatBomb : public MainWeapon_Grenade
{
public:
	MainWeapon_MeatBomb(WeaponBase* p); 
	virtual ~MainWeapon_MeatBomb();

	virtual void	OnInitVariable() override;
	virtual void	OnCreate( bool bCreateInWeapon) override;
	virtual void	Reset() override;

	virtual void	CheckWorldCollision(REAL32 rDeltaSeconds) override;
	virtual void	UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange) override;

	virtual void	UpdateNetworkData(REAL32 rDeltatime) override;

	virtual bool	isPlayRadioSound(void) override { return false; }
	virtual VEC3D * GetThrowInfo_Normal(void) override { return &m_vAttachNormal; }

protected:
	virtual void	_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType = (I3_TERRAIN_TYPE)0) override;

private:

	virtual void	OnUpdate_Grenade(REAL32 rDeltaSeconds) override;

	virtual void Explosion( void ) override;

	void _CheckExplosionCollision(REAL32 rRange);		// virtual����..���� �ȵȴ�..



	REAL32			m_rRotateY;	
	bool			m_bFirstBound;
	VEC3D			m_vAttachNormal;		// ���� �ٴ� ���

	ProcessMeatBombLight* m_cbLight;

};

