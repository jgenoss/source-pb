#pragma once

#include "MainWeapon_Throw.h"

class MainWeapon_RLShell : public MainWeapon_Throw
{
public:
	MainWeapon_RLShell(WeaponBase* p);

	void			Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin);
	bool			isPlayedGrazeSound(void)								{ return m_bGrazeSoundPlayed;	}
	void			PlayGrazeSound(void);

	void			OnUpdate_RLShell( REAL32 rDeltaSeconds);
	
	void			NET_Bound_RLShell( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType );


private:
	virtual void	OnInitVariable() override;
	virtual void	Reset() override;

	void			UpdateFlying( REAL32 rDeltaSeconds);
	void			CheckWorldCollision( REAL32 rDeltaSeconds);
	bool			isBeing(VEC2D* pVec);

	void			UpdateNetworkData( REAL32 rDeltatime);			// ������ �ʿ䰡 ��� ����..
	void			Explosion();									// ������ �ʿ䰡 ��� ����..

	void			_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType = (I3_TERRAIN_TYPE)0);


	REAL32			m_rElapsedExplosionTime;
	REAL32			m_fSmokeLocalTime;
	REAL32			m_rElapsedFlyingTime;			// ������ �߻������ ����ð�.

	VEC2D			m_vRandDirection[4];			//��������(������)
	VEC2D			m_vRandTangent[2];				//��������(ź��Ʈ)
	VEC2D			m_vRandSub[2];					//��������

	bool			m_bExplosion;
	INT32			m_nFlyingStep;					// ������ �߻��� ����
	bool			m_bGrazeSoundPlayed;
};
