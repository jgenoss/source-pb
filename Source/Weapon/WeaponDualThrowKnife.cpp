#include "pch.h"
#include "WeaponDualThrowKnife.h"

#include "GameCharaAnimContext.h"
#include "MainWeapon_DualThrowKnife.h"

#include "MeleeWeaponInfo.h"

I3_CLASS_INSTANCE( WeaponDualThrowKnife);//, WeaponDualKnife);

MainWeapon* WeaponDualThrowKnife::CreateMainWeapon()
{
	MainWeapon_DualThrowKnife * p = new MainWeapon_DualThrowKnife(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualThrowKnife));
	return p;
}

WeaponDualThrowKnife::WeaponDualThrowKnife() : m_iAttackSoundPlayCount(0), m_DoThrowKnife(false)
{

}

WeaponDualThrowKnife::~WeaponDualThrowKnife()
{

}

void WeaponDualThrowKnife::OnFire( i3AIContext * pCtx, REAL32 tm )
{
	// ������ �Ǵ� �����ʸ� ó���ϰ� ���̷� ���Ǵ� ������ ó���� �ʿ� ����.
	MainWeapon_DualThrowKnife* mainWeapon = static_cast<MainWeapon_DualThrowKnife*>(GetMainWeapon());

	if( mainWeapon->GetHandType() == WEAPON_DUAL_HANDTYPE_LEFT)
		return;		

	if(m_FireOrder == WEAPON::PRIMARY_FIRE)
	{
		// ���� ó���� ���� �ʿ���.
		INT32 nCollisionCount = 1;
		REAL32 fAttackTiming = 0.f;
		INT32 autoFireCount = getAutoFireCount();

		nCollisionCount = m_pWeaponInfo->GetAutoFireCount();
		fAttackTiming = m_pWeaponInfo->GetAttackTimeA(autoFireCount);

		if (m_iAttackSoundPlayCount < 2 && pCtx->getLocalTime() > fAttackTiming )
		{
			m_iAttackSoundPlayCount++;
			_OnPlayFireSound();
		}

		// ���� ó�� �켱
		WeaponDualKnife::OnFire(pCtx, tm);
	}
	else
	{
		if (m_iAttackSoundPlayCount < 1)
		{
			m_iAttackSoundPlayCount++;
			_PlayFireSound(WEAPON::FIRE_SOUND_NORMAL_2);
		}
		WeaponDualThrowKnife::OnSecondaryFire(pCtx, tm);
	}
}

// ���⼭ �ð��� ����ؼ� Net_Throw�� �ϸ� �˴ϴ�.
void WeaponDualThrowKnife::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm )
{
	// ���⿡ ���� ���� �̹� Left�� �Ÿ��� ���� �����
	INT32 autoFireCount = getAutoFireCount();
	INT32 nCollisionCount = 1;
	REAL32 fAttackTiming = 0.f;
	REAL32 fAttackDelay = m_pOwner->getAnimContext()->getUpperDuration();

	nCollisionCount = m_pWeaponInfo->GetAutoFireCount2();
	fAttackTiming = m_pWeaponInfo->GetAttackTimeB( autoFireCount);

	//	���� ���ۺ��� Ÿ�� �ð��� �Ǹ� ���������� �ѹ� �ø��� �Ѵ�.
	if( (autoFireCount < nCollisionCount) && pCtx->getLocalTime() > fAttackTiming )
	{
		// ��Ʈ��ũ�� ��Ŷ���� ó�� �մϴ�.
		if (m_pOwner->isLocal())
		{
			VEC3D target;
			GetShotTargetPos(&target);

			NET_Throw(nullptr, &target, -1);

			autoFireCount++;

			// ���⼭ �������� ������ ���� Ÿ�� �ؾ� �մϴ�.
			m_DoThrowKnife = true;
		}
	}

	setAutoFireCount( autoFireCount);

	// ���� �����ð��� ����� �����ٸ�...Ready ���·�
	if( pCtx->getLocalTime() + tm > fAttackDelay )
	{
		m_DoThrowKnife = false;
		ReturnToReady();
	}
}

void WeaponDualThrowKnife::NET_Fire(VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	m_iAttackSoundPlayCount = 0;

	if (type == WEAPON::PRIMARY_FIRE)
		setAutoFireCount(0);

	WeaponKnuckle::NET_Fire(pStartPos, pTargetPos, type);
}

// ���� Throw
void WeaponDualThrowKnife::NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx )
{
	MainWeapon_DualThrowKnife* mainWeapon = static_cast<MainWeapon_DualThrowKnife*>(GetMainWeapon());
	mainWeapon->NET_Throw(pStartPos, pTargetPos, nThrowNetIdx);
}

bool WeaponDualThrowKnife::Fire( WEAPON::FIRE_ORDER type /*= WEAPON::PRIMARY_FIRE*/ )
{
	SetFireOrder(type);

	// 1�� ���� ���¿��� ��ô �Ұ���.
	if( (getLoadedBulletCount() <= 2) && 
		(getFireOrder() == WEAPON::SECONDARY_FIRE) && 
		((CMeleeWeaponInfo*)getBaseWeaponInfo())->DoNotThrowLast())
	{
		return false;
	}

	return WeaponDualKnife::Fire(type);
}

void WeaponDualThrowKnife::Reset(void)
{
	WeaponBase::Reset();

	m_DoThrowKnife = false;
}
