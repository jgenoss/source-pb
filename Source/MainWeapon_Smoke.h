#pragma once

#include "MainWeapon_Grenade.h"

class MainWeapon_Smoke : public MainWeapon_Grenade 
{
	friend class MainWeapon_SmokeShell;

public:
	MainWeapon_Smoke(WeaponBase* p); 
	virtual ~MainWeapon_Smoke();
	
	virtual void	Reset() override;
	virtual void	Explosion() override;

private:
	virtual void	OnUpdate_Grenade(REAL32 rDeltaSeconds) override;
	virtual void	SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo) override;

private:

	void			StopSmokeParticle(bool bStop);

	i3TimeStamp*	m_pSmokeStamp;
	typedef i3::vector<INT32> vStateFlag;		// �ӽ÷� �ٽ� �־...(����źŬ���� ������)
	vStateFlag		m_vReceiveStateFlag;
};

