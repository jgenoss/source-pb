#pragma once

//
// ���� �׸����̵�����, �̰� ����ź����� ���� �ʴ´�..
//
#include "MainWeapon_Throw.h"

class MainWeapon_GrenadeShell : public MainWeapon_Throw
{
public:
	MainWeapon_GrenadeShell(WeaponBase* p);
	virtual ~MainWeapon_GrenadeShell() {}
	
	virtual void	Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin);			// �������⼭ ���� ���̴°� ����..

	virtual void	OnUpdate_GrenadeShell( REAL32 rDeltaSeconds);
	void			NET_Bound_GrenadeShell( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );

protected:
	virtual void	OnInitVariable() override;
	virtual void	Reset() override;

	void			UpdateFlying( REAL32 rDeltaSeconds);
	void			CheckWorldCollision( REAL32 rDeltaSeconds);

	virtual void	Explosion();					// virtual�� ��..
	void			UpdateNetworkData( REAL32 rDeltatime);		// virtual ��...������ �ʿ䰡 ����..
	void			_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType = (I3_TERRAIN_TYPE)0);


	REAL32			m_rElapsedExplosionTime;
	bool			m_bExplosion;
	VEC3D			m_vHitNormal;
};

