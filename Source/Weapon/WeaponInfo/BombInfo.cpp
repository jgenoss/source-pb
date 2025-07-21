#include "pch.h"
#include "BombInfo.h"

I3_CLASS_INSTANCE( CBombInfo);//, CWeaponInfo);


void CBombInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	INT32 nTemp = 0;
	CWeaponInfo::ReadWeaponInfo(pKey);

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("FireBulletCount"),			&nTemp);				// �߻� ����
	I3_BOUNDCHK( nTemp, 255);
	m_nFireBulletCount = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("FireDecBulletCount"),		&nTemp);				// �߻� ����
	I3_BOUNDCHK( nTemp, 255);
	m_nFireDecBulletCount = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("LoadBullet"),				&nTemp);				// źâ�� �ִ� ź��
	I3_BOUNDCHK( nTemp, 65535);
	m_nLoadBullet = (UINT16) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("MaxBullet"),				&nTemp);					// źâ ��
	I3_BOUNDCHK( nTemp, 65535);
	m_nMaxBullet = (UINT16) nTemp;

	REAL32 rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Explosion_Time"),			 &rTemp);				// ���� �����ð�( Grenade�� ���)
	m_rExplosion_Time = rTemp;
	
	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Explosion_Range"),			 &rTemp);			// ���� ��ȿ ����
	m_rExplosion_Range = rTemp;
	
	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Explosion_EffectiveSlope"), &rTemp);			// 3�� ���� ��ȿ ����
	m_rExplosion_EffectiveSlope = rTemp;

	FIND_REG_DATA( pKey, "UseBombTrigger",			&m_bUseBombTrigger);
}

/*virtual*/ void CBombInfo::OnEncriptInfo( UINT8 u8Key)
{
	CWeaponInfo::OnEncriptInfo( u8Key);
}

/*virtual*/ INT32 CBombInfo::GetFireBulletCount() const
{
	return m_nFireBulletCount;
}

/*virtual*/ INT32 CBombInfo::GetFireDecBulletCount() const
{
	return m_nFireDecBulletCount;
}

/*virtual*/ INT32 CBombInfo::GetLoadBullet() const
{
	return m_nLoadBullet;
}
/*virtual*/ INT32 CBombInfo::GetMaxBullet() const
{
	return m_nMaxBullet;
}

//��ô �����
/*virtual*/ REAL32 CBombInfo::GetExplosion_Time() const
{
	return m_rExplosion_Time;
}
/*virtual*/ REAL32 CBombInfo::GetExplosion_Range() const
{
	return m_rExplosion_Range;
}
/*virtual*/ REAL32 CBombInfo::GetExplosion_EffectiveSlope() const
{
	return m_rExplosion_EffectiveSlope;
}

/*virtual*/ bool CBombInfo::GetUseBombTrigger() const
{
	return m_bUseBombTrigger;
}