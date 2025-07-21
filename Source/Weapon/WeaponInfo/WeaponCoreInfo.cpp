#include "pch.h"
#include "GlobalVariables.h"
#include "WeaponCoreInfo.h"
#include "MyRoomInfoContext.h"
#include "OnlyGrenadeInfo.h"

I3_CLASS_INSTANCE( CWeaponCoreInfo); //, i3ElementBase);
I3_CLASS_INSTANCE( CMeleeWeaponCoreInfo); //, CWeaponCoreInfo);
I3_CLASS_INSTANCE( CGunCoreInfo); //, CWeaponCoreInfo);
I3_CLASS_INSTANCE( CGrenadeCoreInfo); //, CWeaponCoreInfo);
I3_CLASS_INSTANCE( CBombCoreInfo); //, CWeaponCoreInfo);

//===========================================================================================================================================//

CWeaponCoreInfo::CWeaponCoreInfo()
{
	m_Attribute = 0;
	m_TypeFire			= (WEAPON::FIRE_TYPE)0;				//	���� ���
	m_TypeTrigger		= (WEAPON::TRIGGER_TYPE)0;			//	�浹 ���
	m_TypeDamage		= (WEAPON::DAMAGE_TYPE)0;			//	���� ���

	m_nDamage			= 0;							//	���� ���ݷ�	
	m_nDamageOnlyUI		= 0;
	m_nMaxDamage		= 0;

	m_nCriticalDamageChance = 0;
	m_fCriticalDamageRate = 0.f;

	m_rWeight = 0.0f;									//	���� ����(������ ���¹����ΰ�?)
	m_rFireDelay = 0.0f;								// �߻� ����
	m_rFireDelay2 = 0.0f;
	m_rRange = 0.0f;		
	m_nItemID			= 0;

	m_CrossHairContractTime = 0.f;
	m_CrossHairExpandTime = 0.f;
	m_CrossHairSpeedFactor = 0.f;
	
	m_nDamageForDino = 0;
	m_nLeftHandWeaponNum = 0;
	m_nMaxDamageForDino = 0;
	
	m_nRealNumber = 0;
	m_rBulletWeight = 0.f;
	m_rHelmetPenetrate_Str = 0.f;
	m_rHelmetPenetrate = 0.f;
	m_rMoveNormalFire = 0.f;
	m_rMoveNormalSit = 0.f;
	m_rMoveNormalStand = 0.f;
	m_rPhysixForceRate = 0.f;
	m_u8EncryptKey = 0;
}

void CWeaponCoreInfo::_ReadDamageKey( i3RegKey * pKey)
{
	INT32  nTemp;
	REAL32 rTemp;

	FIND_REG_DATA( pKey, "_DamageType",				(INT32*)&m_TypeDamage);			//	���� ���

	FIND_REG_DATA( pKey, "HelmetPenetrate",			&m_rHelmetPenetrate);
	FIND_REG_DATA( pKey, "HelmetPenetrate_Str",		&m_rHelmetPenetrate_Str);

	nTemp = 0;
	FIND_REG_DATA( pKey, "Damage",					&nTemp);
	I3_BOUNDCHK( nTemp, 65535);
	m_nDamage = (UINT16) nTemp;

	nTemp = m_nDamage;
	FIND_REG_DATA( pKey, "DamageForDino",			&nTemp);	
	I3_BOUNDCHK( nTemp, 65535);
	m_nDamageForDino = (UINT16) nTemp;
	
	rTemp = 0.0f;
	FIND_REG_DATA(pKey, "Damage_OnlyUI", &rTemp);
	I3_BOUNDCHK(rTemp, 65535);
	m_nDamageOnlyUI = (UINT16)rTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, "DamageMax",				&nTemp);
	I3_BOUNDCHK( nTemp, 65535);
	m_nMaxDamage = (UINT16) nTemp;

	nTemp = m_nMaxDamage;
	FIND_REG_DATA( pKey, "DamageMaxForDino",		&nTemp);
	I3_BOUNDCHK( nTemp, 65535);
	m_nMaxDamageForDino = (UINT16) nTemp;

	FIND_REG_DATA( pKey, "CriticalDamageChance",	&m_nCriticalDamageChance);
	FIND_REG_DATA( pKey, "CriticalDamageRate",		&m_fCriticalDamageRate);
}

void CWeaponCoreInfo::_ReadMovementKey( i3RegKey * pKey)
{
	FIND_REG_DATA( pKey, "MoveNormal_Fire",			&m_rMoveNormalFire);			// ���� ������ ���������� �⺻ �̵� �ӵ�
	FIND_REG_DATA( pKey, "MoveNormal_Sit",			&m_rMoveNormalSit);				// ���� ������ ���������� �̵��ӵ�
	FIND_REG_DATA( pKey, "MoveNormal_Stand",		&m_rMoveNormalStand);			// ���� ����+�߻�� ���������� �̵��ӵ�
}

void CWeaponCoreInfo::_ReadCrosshairKey( i3RegKey * pKey)
{
	FIND_REG_DATA( pKey, "CrossHair_SpeedFactor",	&m_CrossHairSpeedFactor);
	FIND_REG_DATA( pKey, "CrossHair_ExpandTime",	&m_CrossHairExpandTime);
	FIND_REG_DATA( pKey, "CrossHair_ContractTime",	&m_CrossHairContractTime);
}

void CWeaponCoreInfo::ReadWeaponInfo( i3RegKey * pKey, INT32 iShift)
{
	INT32 nTemp = 0;
	I3ASSERT( pKey != nullptr);

	FIND_REG_DATA( pKey, "_NumberReal",				(INT32*)&m_nRealNumber);
	FIND_REG_DATA( pKey, "ITEMID",					(INT32*)&m_nItemID);			//	�����۾��̵�

	// RealNumber�� ������ _Number�� RealNumber�Դϴ�.
	if ( m_nRealNumber == 0 )	m_nRealNumber = WEAPON::ItemID2ResID( m_nItemID);

	FIND_REG_DATA( pKey, "_FireType",				(INT32*)&m_TypeFire);			//	���� ���
	FIND_REG_DATA( pKey, "_TriggerType",			(INT32*)&m_TypeTrigger);		//	�浹 ���
	
	FIND_REG_DATA( pKey, "Weight",					&m_rWeight);					//	���� ����

	i3RegKey * pDamage = Config::FindKey( "Damage", pKey);
	if( pDamage != nullptr)
		_ReadDamageKey( pDamage);

	i3RegKey * pMovement = Config::FindKey( "Movement", pKey);
	if( pMovement != nullptr)
		_ReadMovementKey( pMovement);

	i3RegKey * pCrosshair = Config::FindKey( "Crosshair", pKey);
	if( pCrosshair != nullptr)
		_ReadCrosshairKey( pCrosshair);

	FIND_REG_DATA( pKey, "FireDelay",				&m_rFireDelay);					// �߻� ����
	FIND_REG_DATA( pKey, "FireDelay2",				&m_rFireDelay2);				// �߻� ����
	
	FIND_REG_DATA( pKey, "Range",					&m_rRange);					//  ��ȿ �����Ÿ� : ���⸦ �¾����� Ÿ���� ������ �ִ°Ÿ�
	
	nTemp = 0;
	FIND_REG_DATA( pKey, "FastZoom",				&nTemp);
	if( nTemp > 0)
		m_Attribute |= WEAPON_ATTRIBUTE_FASTZOOM;

	FIND_REG_DATA( pKey, "PhysixForceRate",			&m_rPhysixForceRate);

	nTemp = 0;
	FIND_REG_DATA( pKey, "AttachedSubWeapon",		&nTemp);
	if( nTemp > 0)
		m_Attribute |= WEAPON_ATTRIBUTE_ATTACHEDSUB;
	

// issue : BUG_PB #8765
// �о���� ������ Ÿ�� ��ȿ�� �˻�. 2011-09-20. ugeun.ji 
#if defined( I3_DEBUG )
	CheckRegDataType();
#endif

#if defined( FIREDELAY_COST)
	// 60fps�� ������ ��Ȯ�� �ð��� ����ϱ� ���� �������� ���Ѵ�.
	REAL32 rRemain;

	if( m_rFireDelay > 0.f)
	{
		m_rFireDelay += 0.00001f;
		rRemain = i3Math::fmod( m_rFireDelay, 0.016f);

		if( rRemain > 0.f)
		{
			rRemain = 0.016f - rRemain;
			m_rFireDelay += rRemain;
		}
	}

	if( m_rFireDelay2 > 0.f)
	{
		m_rFireDelay2 += 0.00001f;
		rRemain = i3Math::fmod( m_rFireDelay2, 0.016f);

		if( rRemain > 0.f)
		{
			rRemain = 0.016f - rRemain;
			m_rFireDelay2 += rRemain;
		}
	}

#endif
}

#if defined( I3_DEBUG )
void CWeaponCoreInfo::CheckRegDataType()
{
	I3ASSERT( WEAPON::FIRE_UNKNOWN <= m_TypeFire && m_TypeFire < WEAPON::FIRE_COUNT );
	I3ASSERT( WEAPON::TRIGGER_UNKNOWN <= m_TypeTrigger && m_TypeTrigger < WEAPON::TRIGGER_COUNT );
	I3ASSERT( WEAPON::DAMAGE_UNKNOWN <= m_TypeDamage && m_TypeDamage < WEAPON::DAMAGE_COUNT );
}
#endif

void CWeaponCoreInfo::OnEncriptInfo( UINT8 u8Key)
{
	m_u8EncryptKey = u8Key;

	{	
#if defined( MEM_ENCRYPTION_LV1)
		GlobalFunc::EncryptValue( (INT32*)&m_nItemID, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rPhysixForceRate,		m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_CrossHairExpandTime,	m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_CrossHairContractTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_CrossHairSpeedFactor,	m_u8EncryptKey);
		
		GlobalFunc::EncryptValue( &m_nRealNumber,			m_u8EncryptKey);

		GlobalFunc::EncryptValue( (INT32*)&m_LeftHandWeaponClassType, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nLeftHandWeaponNum,	m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rWeight,				m_u8EncryptKey);
#endif

		GlobalFunc::EncryptValue( (INT32*) &m_Attribute,	m_u8EncryptKey);

		GlobalFunc::EncryptValue( (INT32*)&m_TypeTrigger,	m_u8EncryptKey);
		GlobalFunc::EncryptValue( (INT32*)&m_TypeFire,		m_u8EncryptKey);
		GlobalFunc::EncryptValue( (INT32*)&m_TypeDamage,	m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_nDamage,				m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nDamageOnlyUI,			m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nDamageForDino,		m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rRange,				m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rHelmetPenetrate,		m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rBulletWeight,			m_u8EncryptKey);
		
		GlobalFunc::EncryptValue( &m_nMaxDamage,			m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nMaxDamageForDino,		m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_nCriticalDamageChance,	m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_fCriticalDamageRate,	m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rFireDelay,			m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rMoveNormalFire,		m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rMoveNormalSit,		m_u8EncryptKey);
		
		GlobalFunc::EncryptValue( &m_rExtDPD_Rate,			m_u8EncryptKey);
		
		GlobalFunc::EncryptValue( &m_rFireDelay2,			m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rMoveNormalStand,		m_u8EncryptKey);
		
		GlobalFunc::EncryptValue( &m_rHelmetPenetrate_Str,	m_u8EncryptKey);
	}
}

WEAPON_ATTRIBUTE CWeaponCoreInfo::_getWeaponAttribute( void)
{
	return GlobalFunc::DecryptValue( &m_Attribute, m_u8EncryptKey);
}

bool CWeaponCoreInfo::isEnableFastZoom(void)
{
	return (_getWeaponAttribute() & WEAPON_ATTRIBUTE_FASTZOOM) == WEAPON_ATTRIBUTE_FASTZOOM;
}

INT32 CWeaponCoreInfo::GetMaxDamage(void)
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{
		return GlobalFunc::DecryptValue( &m_nMaxDamageForDino, m_u8EncryptKey);
	}
	else
	{
		return GlobalFunc::DecryptValue( &m_nMaxDamage, m_u8EncryptKey);
	}
}

/*virtual*/ T_ItemID CWeaponCoreInfo::GetItemID()
{
#if defined( MEM_ENCRYPTION_LV1)
	return (WEAPON::ZOOM_TYPE) GlobalFunc::DecryptValue( (INT32*) &m_nItemID, m_u8EncryptKey);
#else
	return m_nItemID;
#endif
}


INT32 CWeaponCoreInfo::GetMaxDamageForDino(void)
{
	return m_nMaxDamageForDino;
}

WEAPON::RES_ID CWeaponCoreInfo::GetNumber()
{
	return WEAPON::ItemID2ResID( GetItemID());
}

WEAPON::RES_ID CWeaponCoreInfo::GetRealNumber()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_nRealNumber, m_u8EncryptKey);
#else
	return m_nRealNumber;
#endif
}

WEAPON_SLOT_TYPE CWeaponCoreInfo::GetTypeUsage()
{
	return (WEAPON_SLOT_TYPE)WEAPON::ItemID2Slot( GetItemID());
}

WEAPON_CLASS_TYPE CWeaponCoreInfo::GetTypeClass()
{
	return WEAPON::ItemID2Class( GetItemID());
}

WEAPON::FIRE_TYPE CWeaponCoreInfo::GetTypeFire()
{
	return (WEAPON::FIRE_TYPE) GlobalFunc::DecryptValue( (INT32*) &m_TypeFire, m_u8EncryptKey);
}

WEAPON::TRIGGER_TYPE CWeaponCoreInfo::GetTypeTrigger()
{
	return (WEAPON::TRIGGER_TYPE) GlobalFunc::DecryptValue( (INT32*) &m_TypeTrigger, m_u8EncryptKey);
}

WEAPON::DAMAGE_TYPE CWeaponCoreInfo::GetTypeDamage()
{
	return (WEAPON::DAMAGE_TYPE) GlobalFunc::DecryptValue( (INT32*) &m_TypeDamage, m_u8EncryptKey);
}

REAL32 CWeaponCoreInfo::GetWeight()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rWeight, m_u8EncryptKey);
#else
	return m_rWeight;
#endif
}

REAL32 CWeaponCoreInfo::GetRange()
{
	return GlobalFunc::DecryptValue( &m_rRange, m_u8EncryptKey);
}

REAL32 CWeaponCoreInfo::GetFireDelay()
{
	return GlobalFunc::DecryptValue( &m_rFireDelay, m_u8EncryptKey);
}

REAL32 CWeaponCoreInfo::GetFireDelay2()
{
	return GlobalFunc::DecryptValue( &m_rFireDelay2, m_u8EncryptKey);
}

REAL32 CWeaponCoreInfo::GetMoveNormalFire()
{
	return GlobalFunc::DecryptValue( &m_rMoveNormalFire, m_u8EncryptKey);
}

REAL32 CWeaponCoreInfo::GetMoveNormalSit()
{
	return GlobalFunc::DecryptValue( &m_rMoveNormalSit, m_u8EncryptKey);
}

REAL32 CWeaponCoreInfo::GetMoveNormalStand()
{
	return GlobalFunc::DecryptValue( &m_rMoveNormalStand, m_u8EncryptKey);
}

REAL32 CWeaponCoreInfo::GetCrossHairExpandTime()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_CrossHairExpandTime, m_u8EncryptKey);
#else
	return m_CrossHairExpandTime;
#endif
}

REAL32 CWeaponCoreInfo::GetCrossHairContractTime()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_CrossHairContractTime, m_u8EncryptKey);
#else
	return m_CrossHairContractTime;
#endif
}

REAL32 CWeaponCoreInfo::GetCrossHairSpeedFactor()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_CrossHairSpeedFactor, m_u8EncryptKey);
#else
	return m_CrossHairSpeedFactor;
#endif
}

REAL32 CWeaponCoreInfo::GetHelmetPenetrate()
{
	return GlobalFunc::DecryptValue( &m_rHelmetPenetrate, m_u8EncryptKey);
}

REAL32 CWeaponCoreInfo::GetHelmetPenetrate_Str()
{
	return GlobalFunc::DecryptValue( &m_rHelmetPenetrate_Str, m_u8EncryptKey);
}

REAL32 CWeaponCoreInfo::GetBulletWeight()
{
	return GlobalFunc::DecryptValue( &m_rBulletWeight, m_u8EncryptKey);
}

bool CWeaponCoreInfo::AttachedSubWeapon()
{
	return (_getWeaponAttribute() & WEAPON_ATTRIBUTE_ATTACHEDSUB) == WEAPON_ATTRIBUTE_ATTACHEDSUB;
}


REAL32 CWeaponCoreInfo::GetPhysixForceRate()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rPhysixForceRate, m_u8EncryptKey);
#else
	return m_rPhysixForceRate;
#endif
}

/*virtual*/ bool CWeaponCoreInfo::IsAlphablendUsed( void)
{
	return (_getWeaponAttribute() & WEAPON_ATTRIBUTE_ALPHABLEND) == WEAPON_ATTRIBUTE_ALPHABLEND;
}

/*virtual*/ INT32 CWeaponCoreInfo::GetDamage()
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{
		return GlobalFunc::DecryptValue( &m_nDamageForDino, m_u8EncryptKey);
	}
	else
	{
		return GlobalFunc::DecryptValue( &m_nDamage, m_u8EncryptKey);
	}
}

/*virtual*/ REAL32 CWeaponCoreInfo::GetDamageOnlyUI()
{
	return GlobalFunc::DecryptValue( &m_nDamageOnlyUI, m_u8EncryptKey);
}


/*virtual*/ INT32 CWeaponCoreInfo::GetDamageForDino()
{
	return m_nDamageForDino;
}


//===========================================================================================================================================//

CMeleeWeaponCoreInfo::CMeleeWeaponCoreInfo()
{
	m_nAutoFireCount	= 1;
	i3Vector::Set(&m_v3CollisionBoxExtent, 0.f, 0.f, 0.f);
	m_nLoadBullet = 0;				// ���� ���� ź�� ��
	m_nMaxBullet = 0;				// �ִ� ���� ź�� ��	

	m_rAttackDamageA_ForDino[0] = 0.f;
	m_rAttackDamageA[0] = 0.f;
}

void CMeleeWeaponCoreInfo::ReadWeaponInfo( i3RegKey * pKey, INT32 iShift)
{
	CWeaponCoreInfo::ReadWeaponInfo(pKey, iShift);

	FIND_REG_DATA( pKey, "AttackDamageA1",				&m_rAttackDamageA[0]);

	m_rAttackDamageA_ForDino[0] = m_rAttackDamageA[0];
	FIND_REG_DATA( pKey, "AttackDamageA1ForDino",		&m_rAttackDamageA_ForDino[0]);

	FIND_REG_DATA( pKey, "AutoFireCount",				&m_nAutoFireCount);

	FIND_REG_DATA( pKey, "CollisionBoxExtent",			&m_v3CollisionBoxExtent);

	FIND_REG_DATA( pKey, "LoadBullet",					&m_nLoadBullet);					// źâ�� �ִ� ź��
	FIND_REG_DATA( pKey, "MaxBullet",					&m_nMaxBullet);						// źâ ��
	FIND_REG_DATA( pKey, "EnableBounce",				&m_bEnableBounce);
}

/*virtual*/ void CMeleeWeaponCoreInfo::OnEncriptInfo( UINT8 u8Key)
{
	CWeaponCoreInfo::OnEncriptInfo( u8Key);

	{
		GlobalFunc::EncryptValue( m_rAttackDamageA, m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_rAttackDamageA_ForDino, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_v3CollisionBoxExtent, sizeof(VEC3D), m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nLoadBullet, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nMaxBullet, m_u8EncryptKey);

#if defined( MEM_ENCRYPTION_LV1)
		GlobalFunc::EncryptValue( &m_nAutoFireCount, m_u8EncryptKey);
#endif
	}
}

/*virtual*/ void CMeleeWeaponCoreInfo::GetCollisionBoxExtent(VEC3D* pVec)
{
	return GlobalFunc::DecryptValue( pVec, &m_v3CollisionBoxExtent, m_u8EncryptKey);
}

/*virtual*/ INT32 CMeleeWeaponCoreInfo::GetDamage()
{
	REAL32 rDm = 0.f;

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{
		rDm = GlobalFunc::DecryptValue( &m_rAttackDamageA_ForDino[0], m_u8EncryptKey);
	}
	else
	{
		rDm = GlobalFunc::DecryptValue( &m_rAttackDamageA[0], m_u8EncryptKey);
	}

	return static_cast<INT32>( rDm );
}

/*virtual*/ INT32 CMeleeWeaponCoreInfo::GetAutoFireCount()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_nAutoFireCount, m_u8EncryptKey);
#else
	return m_nAutoFireCount;
#endif
}

// ���� ���� ź�� ��
/*virtual*/ INT32 CMeleeWeaponCoreInfo::GetLoadBullet()
{
	return GlobalFunc::DecryptValue( &m_nLoadBullet, m_u8EncryptKey);
}

// �ִ� ���� ź�� ��
/*virtual*/ INT32 CMeleeWeaponCoreInfo::GetMaxBullet()
{
	return GlobalFunc::DecryptValue( &m_nMaxBullet, m_u8EncryptKey);
}

bool CMeleeWeaponCoreInfo::IsEnableBounce()
{
	return GlobalFunc::DecryptValue( &m_bEnableBounce, m_u8EncryptKey) != 0;
}

//===========================================================================================================================================//

CGunCoreInfo::CGunCoreInfo()
{
	m_nFireBulletCount = 0;
	m_nFireDecBulletCount = 1;

	m_rCrossHairExpand = 0.0f;							// Crosshiar�� �о��� ��, ���̰� ������� ����
	m_rCrossHairMin = 0.0f;								// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	m_rCrossHairMax = 0.0f;

	m_rCrossHairSizeDamage = 0.0f;						// Damage�� �Ծ��� ��, CrossHair�� �Ͼ����� ���� (Factor)
	m_rCrossHairSizeJump = 0.0f;						// Jump ���� �ÿ� CrossHair�� �о����� ���� (Factor)
	m_rCrossHairSizeMove = 0.0f;						// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	m_rCrossHairSizeMoveSit = 0.0f;
	m_rCrossHairSizeWalk = 0.0f;						// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	m_rCrossHairSizeWalkSit = 0.0f;

	m_rCrossHairSizeStand = 0.0f;						// Idle ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	m_rCrossHairSizeSit = 0.f;
	m_rCrossHairSizeZoom = 0.0f;						// ���� �ÿ� CrossHair�� �о����� ���� (Factor) ?
	m_rCrossHairSizeNoZoom = 0.0f;

	m_RecoilHorzAngle = 0.0f;
	m_RecoilHorzMax		= 0.0f;
	m_RecoilHorzCount	= 0;

	m_RecoilVertAngle	= 0.0f;
	m_RecoilVertMax		= 0.0f;

	m_nAutoFireCount = 0;
	m_bNeedToLoadBullet = false;

	m_rReloadTime = 0.0f;								// �������� �ɸ��� �ð�
	m_rReloadTimeDual = 0.f;

	m_DPD_Rate = 0.0f;

	m_nLoadBullet = 0;
	m_nLoadBulletType = 0;
	m_nMaxBullet = 0;
	m_nReloadBulletCount = 0;

	m_OneHorzEnable = 0;
	m_RecoilVertAngle_Recovertime = 1.0f;
	m_RecoilHorzAngle_Recovertime = 1.0f;
}

void CGunCoreInfo::_ReadCrosshairKey( i3RegKey * pKey)
{
	CWeaponCoreInfo::_ReadCrosshairKey( pKey);

	FIND_REG_DATA( pKey, "CrossHair_Expand",		&m_rCrossHairExpand);				// ��鸰 ũ�ν� �� �����Ǵµ� �ɸ��� �ð�.
	FIND_REG_DATA( pKey, "CrossHair_Min",			&m_rCrossHairMin);					// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	FIND_REG_DATA( pKey, "CrossHair_Max",			&m_rCrossHairMax);					// CorssHair�� �ִ�� Ŭ ���� ũ�� (Clipping-Space������ ũ��)
	FIND_REG_DATA( pKey, "CrossHair_Size_Damage",	&m_rCrossHairSizeDamage);			// ǥ�ػ��¿����� ARũ��
	FIND_REG_DATA( pKey, "CrossHair_Size_Jump",		&m_rCrossHairSizeJump);				// ������   "
	FIND_REG_DATA( pKey, "CrossHair_Size_Move",		&m_rCrossHairSizeMove);				// ���ؽ�   "
	FIND_REG_DATA( pKey, "CrossHair_Size_MoveSit",	&m_rCrossHairSizeMoveSit);			// �ɾƼ� ���ؽ�   "
	FIND_REG_DATA( pKey, "CrossHair_Size_Walk",		&m_rCrossHairSizeWalk);				// ���ؽ�   "
	FIND_REG_DATA( pKey, "CrossHair_Size_WalkSit",	&m_rCrossHairSizeWalkSit);			// �ɾƼ� ���ؽ�   "
	FIND_REG_DATA( pKey, "CrossHair_Size_Stand",	&m_rCrossHairSizeStand);			// ������� "
	FIND_REG_DATA( pKey, "CrossHair_Size_Sit",		&m_rCrossHairSizeSit);				// ������� "
	FIND_REG_DATA( pKey, "CrossHair_Size_Zoom",		&m_rCrossHairSizeZoom);				// ������� "
	FIND_REG_DATA( pKey, "CrossHair_Size_NoZoom",	&m_rCrossHairSizeNoZoom);			// ������� "
}

/*virtual*/ void CGunCoreInfo::OnEncriptInfo( UINT8 u8Key)
{
	CWeaponCoreInfo::OnEncriptInfo( u8Key);

	{
#if defined( MEM_ENCRYPTION_LV1)
		GlobalFunc::EncryptValue( &m_bNeedToLoadBullet, m_u8EncryptKey);
#endif

#if defined( MEM_ENCRYPTION_LV2)
		GlobalFunc::EncryptValue( &m_RecoilHorzCount, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_RecoilVertMax, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_RecoilHorzMax, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_OneHorzEnable, m_u8EncryptKey);
#endif

		GlobalFunc::EncryptValue( &m_nReloadBulletCount, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nLoadBullet, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nLoadBulletType, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nMaxBullet, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nFireBulletCount, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nFireDecBulletCount, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_nAutoFireCount, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_DPD_Rate, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rCrossHairExpand, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairMin, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairMax, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rCrossHairSizeDamage, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairSizeJump, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairSizeMove, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairSizeMoveSit, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairSizeWalk, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairSizeWalkSit, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rCrossHairSizeStand, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairSizeSit, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairSizeZoom, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rCrossHairSizeNoZoom, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rReloadTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rReloadTimeDual, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_RecoilVertAngle, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_RecoilHorzAngle, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_RecoilVertAngle_Recovertime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_RecoilHorzAngle_Recovertime, m_u8EncryptKey);
	}
}

void CGunCoreInfo::_ReadAccuracy( i3RegKey * pKey)
{
	FIND_REG_DATA( pKey, "RecoilHorzAngle",			&m_RecoilHorzAngle);				// ���ؽ�   "
	FIND_REG_DATA( pKey, "RecoilHorzMax",			&m_RecoilHorzMax);					// �߻��   "
	FIND_REG_DATA( pKey, "RecoilHorzCount",			&m_RecoilHorzCount);				// �߻��   "

	FIND_REG_DATA( pKey, "RecoilVertAngle",			&m_RecoilVertAngle);				// ������� "
	FIND_REG_DATA( pKey, "RecoilVertMax",			&m_RecoilVertMax);					// ������� "

	FIND_REG_DATA( pKey, "OneHorzEnable",			&m_OneHorzEnable);			

	FIND_REG_DATA( pKey, "RecoilVertAngle_Recovertime",		&m_RecoilVertAngle_Recovertime);
	FIND_REG_DATA( pKey, "RecoilHorzAngle_Recovertime",		&m_RecoilHorzAngle_Recovertime);
}

void CGunCoreInfo::_ReadDamageKey( i3RegKey * pKey)
{
	CWeaponCoreInfo::_ReadDamageKey( pKey);

	FIND_REG_DATA( pKey, "DPD_Rate",				&m_DPD_Rate);						// ���⺰ ������ �� (DistanceDamageSlope ���...)
	FIND_REG_DATA( pKey, "BulletWeight",			&m_rBulletWeight);
}

void CGunCoreInfo::ReadWeaponInfo( i3RegKey * pKey, INT32 iShift)
{
	CWeaponCoreInfo::ReadWeaponInfo(pKey, iShift);

	FIND_REG_DATA( pKey, "LoadBullet",				&m_nLoadBullet);				// źâ�� �ִ� ź��
	FIND_REG_DATA( pKey, "LoadBulletType",			&m_nLoadBulletType);				// ������ Ÿ��
	FIND_REG_DATA( pKey, "ReloadBulletCount",		&m_nReloadBulletCount);			// źâ�� �ִ� ź��
	FIND_REG_DATA( pKey, "MaxBullet",				&m_nMaxBullet);					// źâ ��
	FIND_REG_DATA( pKey, "FireBulletCount",			&m_nFireBulletCount);			// �߻�ü ����
	FIND_REG_DATA( pKey, "FireDecBulletCount",		&m_nFireDecBulletCount);		

	FIND_REG_DATA( pKey, "AutoFireCount",			&m_nAutoFireCount);				
	FIND_REG_DATA( pKey, "NeedToLoadBullet",		&m_bNeedToLoadBullet);			

	FIND_REG_DATA( pKey, "ReloadTime",				&m_rReloadTime);				// �������� �ɸ��� �ð�

	i3RegKey * pAccurarcy = Config::FindKey( "Accuracy", pKey);
	if( pAccurarcy != nullptr)
		_ReadAccuracy( pAccurarcy);

	REAL32 rValue = 0.f;
	i3RegKey * pExtension = Config::FindKey( "Extension1", pKey);
	if( pExtension != nullptr)
		FIND_REG_DATA( pExtension, "ReloadTimeDual",		&m_rReloadTimeDual);
	pExtension = Config::FindKey( "Extension2", pKey);
	if( pExtension != nullptr)
		FIND_REG_DATA( pExtension, "ReloadTimeDual",		&rValue);
	if( rValue > 0.f)
		m_rReloadTimeDual = rValue;
}

/*virtual*/ INT32 CGunCoreInfo::GetLoadBullet()
{
	return GlobalFunc::DecryptValue( &m_nLoadBullet, m_u8EncryptKey);
}

/*virtual*/ INT32 CGunCoreInfo::GetLoadBulletType()
{
	return GlobalFunc::DecryptValue( &m_nLoadBulletType, m_u8EncryptKey);
}

/*virtual*/ INT32 CGunCoreInfo::GetReloadBulletCount()
{
	return GlobalFunc::DecryptValue( &m_nReloadBulletCount, m_u8EncryptKey);
}

/*virtual*/ INT32 CGunCoreInfo::GetMaxBullet()
{
	return GlobalFunc::DecryptValue( &m_nMaxBullet, m_u8EncryptKey);
}

/*virtual*/ INT32 CGunCoreInfo::GetFireBulletCount()
{
	return GlobalFunc::DecryptValue( &m_nFireBulletCount, m_u8EncryptKey);
}

/*virtual*/ INT32 CGunCoreInfo::GetFireDecBulletCount()
{
	return GlobalFunc::DecryptValue( &m_nFireDecBulletCount, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairExpand()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairExpand, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairMin()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairMin, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairMax()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairMax, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairSizeDamage()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairSizeDamage, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairSizeJump()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairSizeJump, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairSizeMove()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairSizeMove, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairSizeMoveSit()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairSizeMoveSit, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairSizeWalk()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairSizeWalk, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairSizeWalkSit()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairSizeWalkSit, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairSizeStand()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairSizeStand, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairSizeZoom()
{
	return GlobalFunc::DecryptValue( &m_rCrossHairSizeZoom, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetCrossHairSizeNoZoom(void)
{
	return GlobalFunc::DecryptValue( &m_rCrossHairSizeNoZoom, m_u8EncryptKey);
}

/*virtual*/ INT32 CGunCoreInfo::GetAutoFireCount()
{
	return GlobalFunc::DecryptValue( &m_nAutoFireCount, m_u8EncryptKey);
}

/*virtual*/ INT32 CGunCoreInfo::GetNeedToLoadBullet()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_bNeedToLoadBullet, m_u8EncryptKey);
#else
	return m_bNeedToLoadBullet;
#endif
}

/*virtual*/ REAL32 CGunCoreInfo::GetReloadTime()
{
	return GlobalFunc::DecryptValue( &m_rReloadTime, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetReloadTimeDual()
{
	return GlobalFunc::DecryptValue( &m_rReloadTimeDual, m_u8EncryptKey);
}

/*virtual*/ REAL32 CGunCoreInfo::GetDPD_Rate()
{
	return GlobalFunc::DecryptValue( &m_DPD_Rate, m_u8EncryptKey);
}

// 1�ߴ� ���� �ݵ���
/*virtual*/ REAL32 CGunCoreInfo::GetRecoilVertAngle()
{
	return GlobalFunc::DecryptValue( &m_RecoilVertAngle, m_u8EncryptKey);
}
// �ִ� ���� �ݵ���
/*virtual*/ REAL32 CGunCoreInfo::GetRecoilVertMax()
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( &m_RecoilVertMax, m_u8EncryptKey);
#else
	return m_RecoilVertMax;
#endif
}

// �¿� ��鸲 ��
/*virtual*/ REAL32 CGunCoreInfo::GetRecoilHorzAngle()
{
	return GlobalFunc::DecryptValue( &m_RecoilHorzAngle, m_u8EncryptKey);
}
// �ִ� ���� �¿� ��鸲 ��
/*virtual*/ REAL32 CGunCoreInfo::GetRecoilHorzMax()
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( &m_RecoilHorzMax, m_u8EncryptKey);
#else
	return m_RecoilHorzMax;
#endif
}
// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����
/*virtual*/ INT32 CGunCoreInfo::GetRecoilHorzCount()
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( &m_RecoilHorzCount, m_u8EncryptKey);
#else
	return m_RecoilHorzCount;
#endif
}

/*virtual*/ INT32 CGunCoreInfo::GetOneHorzEnable()
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( &m_OneHorzEnable, m_u8EncryptKey);
#else
	return m_OneHorzEnable;
#endif
}

REAL32 CGunCoreInfo::GetRecoilVertAngle_Recovertime() const
{
	return GlobalFunc::DecryptValue( &m_RecoilVertAngle_Recovertime, m_u8EncryptKey);
}
REAL32 CGunCoreInfo::GetRecoilHorzAngle_Recovertime() const
{
	return GlobalFunc::DecryptValue( &m_RecoilHorzAngle_Recovertime, m_u8EncryptKey);
}
//===========================================================================================================================================//

CGrenadeCoreInfo::CGrenadeCoreInfo()
{
	m_pOnlyGreandeInfo = COnlyGrenadeInfo::new_object();

	//OnEncriptInfo(0);
}

/*virtual*/ CGrenadeCoreInfo::~CGrenadeCoreInfo()
{
	I3_SAFE_RELEASE(m_pOnlyGreandeInfo);
}

void CGrenadeCoreInfo::ReadWeaponInfo( i3RegKey * pKey, INT32 iShift)
{
	CWeaponCoreInfo::ReadWeaponInfo(pKey, iShift);

	I3ASSERT(m_pOnlyGreandeInfo);

	m_pOnlyGreandeInfo->ReadWeaponInfo(pKey);
}

/*virtual*/ void CGrenadeCoreInfo::OnEncriptInfo( UINT8 u8Key)
{
	CWeaponCoreInfo::OnEncriptInfo( u8Key);

	m_pOnlyGreandeInfo->OnEncriptInfo( u8Key);
}

/*virtual*/INT32 CGrenadeCoreInfo::GetFireBulletCount()			
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetFireBulletCount(); 
}

/*virtual*/INT32 CGrenadeCoreInfo::GetFireDecBulletCount()
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetFireDecBulletCount();	
}

/*virtual*/INT32 CGrenadeCoreInfo::GetLoadBullet()
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetLoadBullet(); 
}

/*virtual*/INT32 CGrenadeCoreInfo::GetReloadBulletCount()
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetReloadBulletCount(); 
}

/*virtual*/INT32 CGrenadeCoreInfo::GetMaxBullet()
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetMaxBullet(); 
}

/*virtual*/REAL32 CGrenadeCoreInfo::GetExplosion_Time()
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetExplosion_Time(); 
}	

/*virtual*/REAL32 CGrenadeCoreInfo::GetExplosion_Range()
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetExplosion_Range(); 
}

/*virtual*/REAL32 CGrenadeCoreInfo::GetExplosion_EffectiveSlope()
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetExplosion_EffectiveSlope(); 
}

/*virtual*/REAL32 CGrenadeCoreInfo::GetThrowSpeed()
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetThrowSpeed();	
}

/*virtual*/REAL32 CGrenadeCoreInfo::GetEffectiveTime()
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetEffectiveTime(); 
}

/*virtual*/REAL32 CGrenadeCoreInfo::GetAttachLength()
{
	I3ASSERT( m_pOnlyGreandeInfo != nullptr);
	return m_pOnlyGreandeInfo->GetAttachLength();
}

/*virtual*/REAL32 CGrenadeCoreInfo::GetDamageTerm()
{
	I3ASSERT( m_pOnlyGreandeInfo != nullptr);
	return m_pOnlyGreandeInfo->GetDamageTerm();
}

//===========================================================================================================================================//

CBombCoreInfo::CBombCoreInfo()
{
	m_nFireBulletCount = 0;
	m_nFireDecBulletCount = 1;
	m_nLoadBullet = 0;									// ���� ���� ź�� ��
	m_nMaxBullet = 0;			

	//��ô �����
	m_rExplosion_Time = 0.0f;							// ���� �����ð�( Grenade�� ���)
	m_rExplosion_Range = 0.0f;							// ���� ��ȿ ����
	m_rExplosion_EffectiveSlope = 0.0f;					// ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope

	//OnEncriptInfo(0);
}

/*virtual*/ CBombCoreInfo::~CBombCoreInfo()
{

}


void CBombCoreInfo::ReadWeaponInfo( i3RegKey * pKey, INT32 iShift)
{
	CWeaponCoreInfo::ReadWeaponInfo(pKey, iShift);

	FIND_REG_DATA( pKey, "FireBulletCount",			&m_nFireBulletCount);				// �߻� ����
	FIND_REG_DATA( pKey, "FireDecBulletCount",		&m_nFireDecBulletCount);				// �߻� ����
	FIND_REG_DATA( pKey, "LoadBullet",				&m_nLoadBullet);				// źâ�� �ִ� ź��
	FIND_REG_DATA( pKey, "MaxBullet",				&m_nMaxBullet);					// źâ ��

	FIND_REG_DATA( pKey, "Explosion_Time",			&m_rExplosion_Time);				// ���� �����ð�( Grenade�� ���)
	FIND_REG_DATA( pKey, "Explosion_Range",			&m_rExplosion_Range);			// ���� ��ȿ ����
	FIND_REG_DATA( pKey, "Explosion_EffectiveSlope", &m_rExplosion_EffectiveSlope);			// 3�� ���� ��ȿ ����
}

/*virtual*/ void CBombCoreInfo::OnEncriptInfo( UINT8 u8Key)
{
	CWeaponCoreInfo::OnEncriptInfo( u8Key);

	{
#if defined( MEM_ENCRYPTION_LV1)
		GlobalFunc::EncryptValue( &m_rExplosion_Time, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rExplosion_Range, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rExplosion_EffectiveSlope, m_u8EncryptKey);
#endif

		GlobalFunc::EncryptValue( &m_nFireBulletCount, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nFireDecBulletCount, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_nLoadBullet, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nMaxBullet, m_u8EncryptKey);
	}
}

/*virtual*/ INT32 CBombCoreInfo::GetFireBulletCount()
{
	return GlobalFunc::DecryptValue( &m_nFireBulletCount, m_u8EncryptKey);
}

/*virtual*/ INT32 CBombCoreInfo::GetFireDecBulletCount()
{
	return GlobalFunc::DecryptValue( &m_nFireDecBulletCount, m_u8EncryptKey);
}

/*virtual*/ INT32 CBombCoreInfo::GetLoadBullet()
{
	return GlobalFunc::DecryptValue( &m_nLoadBullet, m_u8EncryptKey);
}

/*virtual*/ INT32 CBombCoreInfo::GetMaxBullet()
{
	return GlobalFunc::DecryptValue( &m_nMaxBullet, m_u8EncryptKey);
}

//��ô �����
/*virtual*/ REAL32 CBombCoreInfo::GetExplosion_Time()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rExplosion_Time, m_u8EncryptKey);
#else
	return m_rExplosion_Time;
#endif
}

/*virtual*/ REAL32 CBombCoreInfo::GetExplosion_Range()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rExplosion_Range, m_u8EncryptKey);
#else
	return m_rExplosion_Range;
#endif
}

/*virtual*/ REAL32 CBombCoreInfo::GetExplosion_EffectiveSlope()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rExplosion_EffectiveSlope, m_u8EncryptKey);
#else
	return m_rExplosion_EffectiveSlope;
#endif
}

