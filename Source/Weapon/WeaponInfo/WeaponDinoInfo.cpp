#include "pch.h"
#include "WeaponDinoInfo.h"

I3_CLASS_INSTANCE( CWeaponDinoInfo);//, CWeaponInfo);

CWeaponDinoInfo::CWeaponDinoInfo() : m_AssistKillImageIndex(-1), m_HitSoundType(-1), m_HitSoundIndex(-1)
	, m_rAttackFreezeTime(0.f), m_rDPD_Rate(0.f), m_nAutoFireCount(0), m_nFireBulletCount(0)
	, m_rDeviation(0.f), m_rExplosion_Range(0.f), m_rExplosion_EffectiveSlope(0.f)
	, m_rEffectiveTime(0.f), m_rSkillDuration(0.f), m_rSkillCoolTime(0.f), m_rSkillMovementRate(0.f)
	, m_rBulletSmokeRepeatTexSize(1.f), m_rBulletSmokeLifeTime(0.f), m_rBulletSmokeGrowThickness(0.f)
	, m_rBulletSmokeThickness(0.f), m_rBulletSmokeStartSpeed(0.f), m_rBulletSmokeEndSpeed(0.f)
	, m_TerrainHitSoundIndex(-1)
	, m_rCrossHairExpand(0.f), m_rCrossHairMin(0.f), m_rCrossHairMax(0.f)
{
	for(INT32 i = 0 ; i < ATTACK_NUM_MAX ; ++i)
	{
		m_rAttackTime[i] = 0.0f;
		m_rAttackDamage[i] = 0.0f;
	}

	i3Color::Set(&m_cBulletSmokeColor, (UINT8) 255, 255, 255, 255);
}

/*virtual*/ CWeaponDinoInfo::~CWeaponDinoInfo()
{

}

void CWeaponDinoInfo::_ReadBulletTracerKey( i3RegKey * pKey)
{
	FIND_REG_DATA( pKey, "BulletSmokeColor",			&m_cBulletSmokeColor);
	FIND_REG_DATA( pKey, "BulletSmokeRepeatTexSize",	&m_rBulletSmokeRepeatTexSize);
	FIND_REG_DATA( pKey, "BulletSmokeGrowThickness",	&m_rBulletSmokeGrowThickness);	
	FIND_REG_DATA( pKey, "BulletSmokeLifeTime",			&m_rBulletSmokeLifeTime);
	FIND_REG_DATA( pKey, "BulletSmokeThickness",		&m_rBulletSmokeThickness);
	FIND_REG_DATA( pKey, "BulletSmokeStartSpeed",		&m_rBulletSmokeStartSpeed);
	FIND_REG_DATA( pKey, "BulletSmokeEndSpeed",			&m_rBulletSmokeEndSpeed);
}

/*virtual*/void CWeaponDinoInfo::SetBulletSmokeInfo(TracerBulletInfo* pBulletInfo)
{
	I3ASSERT(pBulletInfo);

	pBulletInfo->_LifeTime = GetBulletSmokeLifeTime();
	pBulletInfo->_Thickness = GetBulletSmokeThickness();
	pBulletInfo->_GrowThickness = GetBulletSmokeGrowThickness();
	pBulletInfo->_StartSpeed = GetBulletSmokeStartSpeed();
	pBulletInfo->_EndSpeed = GetBulletSmokeEndSpeed();
	pBulletInfo->_RepeatTexSize = GetBulletSmokeRepeatTexSize();

	GetBulletSmokeColor( &pBulletInfo->_color);
}

void CWeaponDinoInfo::_ReadCrosshairKey( i3RegKey * pKey)
{
	FIND_REG_DATA( pKey, LS_STR("CrossHair_Expand"),		&m_rCrossHairExpand);				// ��鸰 ũ�ν� �� �����Ǵµ� �ɸ��� �ð�.
	FIND_REG_DATA( pKey, LS_STR("CrossHair_Min"),			&m_rCrossHairMin);					// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	FIND_REG_DATA( pKey, LS_STR("CrossHair_Max"),			&m_rCrossHairMax);					// CorssHair�� �ִ�� Ŭ ���� ũ�� (Clipping-Space������ ũ��)
}

void CWeaponDinoInfo::_ReadDamageKey( i3RegKey * pKey)
{
	CWeaponInfo::_ReadDamageKey( pKey);

	FIND_REG_DATA( pKey, LS_STR("DPD_Rate"),					&m_rDPD_Rate);
}

void CWeaponDinoInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	INT32 nTemp = 0;

	CWeaponInfo::ReadWeaponInfo(pKey);

	for(INT32 i=0; i<ATTACK_NUM_MAX; i++)
	{
		char temp[256] = "";

		i3::snprintf(temp, 256, LS_STR("AttackTimeA%d"), i+1);
		FIND_REG_DATA( pKey, temp,			&m_rAttackTime[i]);

		i3::snprintf(temp, 256, LS_STR("AttackDamageA%d"), i+1);
		FIND_REG_DATA( pKey, temp,			&m_rAttackDamage[i]);
	}

	FIND_REG_DATA( pKey, LS_STR("AttackFreezeTime"),			&m_rAttackFreezeTime);

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("AutoFireCount"),				&nTemp);
	I3_BOUNDCHK( nTemp, 255);
	m_nAutoFireCount = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("FireBulletCount"),				&nTemp);						// �߻� ����
	I3_BOUNDCHK( nTemp, 255);
	m_nFireBulletCount = (UINT8) nTemp;

	FIND_REG_DATA( pKey, LS_STR("Deviation"),					&m_rDeviation);					// ������

	FIND_REG_DATA( pKey, LS_STR("Explosion_Range"),				&m_rExplosion_Range);			// ���� ��ȿ ����
	FIND_REG_DATA( pKey, LS_STR("Explosion_EffectiveSlope"),	&m_rExplosion_EffectiveSlope);	// �Ÿ��� ������ ������

	FIND_REG_DATA( pKey, "EffectiveTime",				&m_rEffectiveTime);				// ����Ʈ ��ȿ �ð�

	FIND_REG_DATA( pKey, "Skill_Duration",				&m_rSkillDuration);				// ��ų ���ӽð�
	FIND_REG_DATA( pKey, "Skill_CoolTime",				&m_rSkillCoolTime);				// ��ų ��Ÿ��
	FIND_REG_DATA( pKey, "Skill_MovementRate",			&m_rSkillMovementRate);			// ��ų �̵��ӵ���

	FIND_REG_DATA( pKey, "AssistKillShape",				&m_AssistKillImageIndex);
	FIND_REG_DATA( pKey, "HitSoundType",				&m_HitSoundType);
	FIND_REG_DATA( pKey, "HitSoundIndex",				&m_HitSoundIndex);
	FIND_REG_DATA( pKey, "TerrainHitSound",				&m_TerrainHitSoundIndex);

	i3RegKey * pChildKey = Config::FindKey( "BulletTracer", pKey);
	if( pChildKey != nullptr)
		_ReadBulletTracerKey( pChildKey);

	i3RegKey * pCrosshair = Config::FindKey(LS_STR("Crosshair"), pKey);
	if( pCrosshair != nullptr)
		_ReadCrosshairKey( pChildKey);
}

/*virtual*/ void CWeaponDinoInfo::OnEncriptInfo( UINT8 u8Key)
{
	INT32 i;

	CWeaponInfo::OnEncriptInfo( u8Key);

#if defined( MEM_ENCRYPTION_LV1)
	for( i = 0; i < ATTACK_NUM_MAX; i++)
	{
		GlobalFunc::EncryptValue( &m_rAttackTime[i],		m_u8EncryptKey[VALUETYPE_REAL]);
	}

	GlobalFunc::EncryptValue( &m_nAutoFireCount,			m_u8EncryptKey[VALUETYPE_INT]);
	GlobalFunc::EncryptValue( &m_rExplosion_EffectiveSlope, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rEffectiveTime,			m_u8EncryptKey[VALUETYPE_REAL]);

	GlobalFunc::EncryptValue( &m_rSkillDuration,			m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rSkillCoolTime,			m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rSkillMovementRate,		m_u8EncryptKey[VALUETYPE_REAL]);

	GlobalFunc::EncryptValue( &m_cBulletSmokeColor,	sizeof(m_cBulletSmokeColor), m_u8EncryptKey[VALUETYPE_COLOR]);
	GlobalFunc::EncryptValue( &m_rBulletSmokeRepeatTexSize, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rBulletSmokeGrowThickness, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rBulletSmokeLifeTime, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rBulletSmokeThickness, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rBulletSmokeStartSpeed, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rBulletSmokeEndSpeed, m_u8EncryptKey[VALUETYPE_REAL]);
#endif

	for( i = 0; i < ATTACK_NUM_MAX; i++)
	{	
		GlobalFunc::EncryptValue( &m_rAttackDamage[i],		m_u8EncryptKey[VALUETYPE_REAL]);
	}

	GlobalFunc::EncryptValue( &m_nFireBulletCount,			m_u8EncryptKey[VALUETYPE_INT]);
	GlobalFunc::EncryptValue( &m_rDeviation,				m_u8EncryptKey[VALUETYPE_REAL]);

	GlobalFunc::EncryptValue( &m_rExplosion_Range,			m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rAttackFreezeTime,			m_u8EncryptKey[VALUETYPE_REAL]);

	GlobalFunc::EncryptValue( &m_rCrossHairExpand,			m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rCrossHairMin,				m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rCrossHairMax,				m_u8EncryptKey[VALUETYPE_REAL]);
}

/*virtual*/ REAL32 CWeaponDinoInfo::GetDamageA( INT32 num) const
{
	if(num >= ATTACK_NUM_MAX)
		return 0.f;

	return GlobalFunc::DecryptValue( &m_rAttackDamage[ num], m_u8EncryptKey[VALUETYPE_REAL]);
}

/*virtual*/ REAL32 CWeaponDinoInfo::GetAttackTimeA( INT32 num) const
{
	
#if defined( MEM_ENCRYPTION_LV1)
	REAL32 time = GlobalFunc::DecryptValue( &m_rAttackTime[ num], m_u8EncryptKey[VALUETYPE_REAL]);
#else
	REAL32 time = m_rAttackTime[ num];
#endif

	return ( (num < ATTACK_NUM_MAX) ? time : 0.f);
}

/*virtual*/ INT32 CWeaponDinoInfo::GetAutoFireCount(void) const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_nAutoFireCount, m_u8EncryptKey[VALUETYPE_INT]);
#else
	return m_nAutoFireCount;
#endif
}

// �߻�ü ����
/*virtual*/ INT32 CWeaponDinoInfo::GetFireBulletCount(void) const
{
	return GlobalFunc::DecryptValue( &m_nFireBulletCount, m_u8EncryptKey[VALUETYPE_INT]);
}

// ������
/*virtual*/ REAL32 CWeaponDinoInfo::GetDeviation(void) const
{
	return GlobalFunc::DecryptValue( &m_rDeviation, m_u8EncryptKey[VALUETYPE_REAL]);
}

// ���� ��ȿ ����
/*virtual*/ REAL32 CWeaponDinoInfo::GetExplosion_Range(void) const
{
	return GlobalFunc::DecryptValue( &m_rExplosion_Range, m_u8EncryptKey[VALUETYPE_REAL]);
}

// �Ÿ��� ������ ������ Slop
/*virtual*/ REAL32 CWeaponDinoInfo::GetExplosion_EffectiveSlope(void) const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rExplosion_EffectiveSlope, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rExplosion_EffectiveSlope;
#endif
}

// ����Ʈ ��ȿ �ð�
/*virtual*/ REAL32 CWeaponDinoInfo::GetEffectiveTime(void) const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rEffectiveTime, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rEffectiveTime;
#endif
}

// ���� ��ȿ ����
/*virtual*/ REAL32 CWeaponDinoInfo::GetAttackFreezeTime(void) const
{
	return GlobalFunc::DecryptValue( &m_rAttackFreezeTime, m_u8EncryptKey[VALUETYPE_REAL]);
}

// ��ų ���ӽð�
/*virtual*/ REAL32 CWeaponDinoInfo::GetSkillDuration(void) const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rSkillDuration, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rSkillDuration;
#endif
}

// ��ų ��Ÿ��
/*virtual*/ REAL32 CWeaponDinoInfo::GetSkillCoolTime(void) const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rSkillCoolTime, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rSkillCoolTime;
#endif
}

// ��ų �̵��ӵ���
/*virtual*/ REAL32 CWeaponDinoInfo::GetSkillSkillMovementRate(void) const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rSkillMovementRate, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rSkillMovementRate;
#endif
}

/*virtual*/WEAPON::ATTACK_TYPE CWeaponDinoInfo::GetAttackType(WEAPON::FIRE_ORDER fireOrder, INT32 i32FireCount)  const
{ 	
	return (WEAPON::ATTACK_TYPE)(WEAPON::ATTACK_TYPE_L1 + i32FireCount);
}

//	�Ѿ� ���� ���� ����	
/*virtual*/ void CWeaponDinoInfo::GetBulletSmokeColor( I3COLOR * pOut)
{
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::DecryptValue( pOut, &m_cBulletSmokeColor, m_u8EncryptKey[VALUETYPE_COLOR]);
#else
	i3Color::Copy( pOut, &m_cBulletSmokeColor);
#endif
}	

//	�Ѿ� ���� ���� �̹����� Ÿ�ϸ��Ǵ� ���� (����: m)
/*virtual*/ REAL32 CWeaponDinoInfo::GetBulletSmokeRepeatTexSize() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rBulletSmokeRepeatTexSize, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rBulletSmokeRepeatTexSize;
#endif
}
/*virtual*/ REAL32 CWeaponDinoInfo::GetBulletSmokeGrowThickness() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rBulletSmokeGrowThickness, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rBulletSmokeGrowThickness;
#endif
}
/*virtual*/ REAL32 CWeaponDinoInfo::GetBulletSmokeLifeTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rBulletSmokeLifeTime, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rBulletSmokeLifeTime;
#endif
}
/*virtual*/ REAL32 CWeaponDinoInfo::GetBulletSmokeThickness() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rBulletSmokeThickness, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rBulletSmokeThickness;
#endif
}

/*virtual*/ REAL32 CWeaponDinoInfo::GetBulletSmokeStartSpeed() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rBulletSmokeStartSpeed, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rBulletSmokeStartSpeed;
#endif
}
/*virtual*/ REAL32 CWeaponDinoInfo::GetBulletSmokeEndSpeed() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rBulletSmokeEndSpeed, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rBulletSmokeEndSpeed;
#endif
}

/*virtual*/ REAL32 CWeaponDinoInfo::GetCrossHairExpand() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rCrossHairExpand, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rCrossHairExpand;
#endif
}

/*virtual*/ REAL32 CWeaponDinoInfo::GetCrossHairMin() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rCrossHairMin, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rCrossHairMin;
#endif
}

/*virtual*/ REAL32 CWeaponDinoInfo::GetCrossHairMax() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rCrossHairMax, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rCrossHairMax;
#endif
}