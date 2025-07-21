#pragma once


#include "MainWeapon_Grenade.h"

// �ϴ� ���Ѵ�� ������ ����ϰ� ����ź�� ����Ѵ�..

// ���߿�, ������������ ��ƾ�� ���� �ű��, ����ź��� �������� ��ӹް� �����ؾ��Ѵ�.

class MainWeapon_MedicalKit : public MainWeapon_Grenade
{
public:
	MainWeapon_MedicalKit(WeaponBase* p); 
	virtual void	OnInitVariable() override;
	virtual void	OnCreate(bool bCreateInWeapon) override;
	virtual void	Reset() override;


	virtual void	CheckWorldCollision(REAL32 rDeltaSeconds) override;

	virtual bool	isPlayRadioSound(void) override { return false; }
	virtual bool	isUseTraceEffect(void) override { return false; }

	virtual VEC3D * GetThrowInfo_Normal(void) override { return &m_vAttachNormal; }

protected:
	virtual void	_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType = (I3_TERRAIN_TYPE)0) override;

protected:
	virtual void	UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange) override;
	virtual void	UpdateNetworkData(REAL32 rDeltatime) override;

protected:
	virtual void	OnUpdate_Grenade(REAL32 rDeltaSeconds) override;

	void			_TakeObject( I3_PHYSIX_HIT_RESULT * pResult );
	void			_TakeObject( i3Object * pObject);
	virtual void	Explosion( void ) override;

	bool RecoveryHP (CGameCharaBase * pChara);

protected:
	VEC3D			m_vAttachNormal;		// ���� �ٴ� ���

	MATRIX			m_AttachLocalMatrix;
	REAL32			m_rRotateY;
	REAL32			m_rThrowLen;			// �Ÿ��� ���� ���� �ٴ� ��� ����
	REAL32			m_rElapsedPickTime;		// ������ ��ô �� ȹ�� ���� ������

	bool			m_bFirstBound;
	bool			m_bCalcEllipsedTime;
};

