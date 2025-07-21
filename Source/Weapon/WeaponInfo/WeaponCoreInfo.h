/**
 * \file	Weapon\WeaponInfo\WeaponInfo.h
 *
 * Declares the weapon information class.
 */


#if !defined( _WEAPON_CORE_INFO_H__)
#define _WEAPON_CORE_INFO_H__

#include "../../../../Common/CommonSource/WeaponDefine.h"
#include "WeaponGlobal.h"

class CWeaponCoreInfo :	public i3ElementBase
{
	I3_CLASS_DEFINE( CWeaponCoreInfo, i3ElementBase);

public:
	T_ItemID				m_nItemID;					// ������ ���̵�
	WEAPON_ATTRIBUTE		m_Attribute;
	WEAPON::FIRE_TYPE		m_TypeFire;					//	���� ���
	WEAPON::TRIGGER_TYPE	m_TypeTrigger;				//	�浹 ���
	WEAPON::DAMAGE_TYPE		m_TypeDamage;				//	���� ���

	UINT16					m_nDamage;					//	���� ���ݷ�	(�⺻ ����)
	REAL32					m_nDamageOnlyUI;			//  �κ� ǥ�ÿ�
	UINT16					m_nDamageForDino;
	UINT16					m_nMaxDamage;
	UINT16					m_nMaxDamageForDino;

	INT32				m_nCriticalDamageChance;
	REAL32					m_fCriticalDamageRate;
	
	REAL32					m_rRange;					//  ��ȿ �����Ÿ� : ���⸦ �¾����� Ÿ���� ������ �ִ°Ÿ�
	REAL32					m_rExtDPD_Rate;				// �Ÿ��� ������ ������
	REAL32					m_rBulletWeight;			// Helmet penetrate
	REAL32					m_rWeight;					//	���� ����(������ ���¹����ΰ�?)
	REAL32					m_rFireDelay;				// �߻� ����
	REAL32					m_rHelmetPenetrate;			// Helmet penetrate
	REAL32					m_rHelmetPenetrate_Str;		// ranforce2 Helmet penetrate

	WEAPON_CLASS_TYPE		m_TypeClass;				//	���� Ŭ����
	WEAPON::RES_ID			m_nRealNumber;				// D ������ ��� ��¥ ���� ��ȣ.
	
	//Move( %)
	REAL32					m_rMoveNormalFire;			// ���� �߻� ���� �̵��ӵ� ���� ���� (Factor)
	REAL32					m_rMoveNormalSit;			// ���� ���¿����� �̵��ӵ� ���� ���� (Factor)

	REAL32					m_CrossHairSpeedFactor;		// CrossHair�� Ŀ���ų� �۾����� �ӵ��� ���� (���� �ӵ��� ��)
	REAL32					m_CrossHairExpandTime;
	REAL32					m_CrossHairContractTime;

	REAL32					m_rFireDelay2;				// �߻� ���� (���� ������ ��� ������ ����)
	REAL32					m_rMoveNormalStand;			// �� ���¿����� �̵��ӵ� ���� ���� (Factor)

	// KEYWORD : WEAPON_DUAL
	WEAPON_CLASS_TYPE		m_LeftHandWeaponClassType;
	INT32					m_nLeftHandWeaponNum;
	INT32					m_DualClassType;

	REAL32					m_rPhysixForceRate;			// ������ ���� ���� (�迭 ���밪 * Damage * Rate)
	
protected:
	UINT8 					m_u8EncryptKey;

	WEAPON_ATTRIBUTE		_getWeaponAttribute( void);

	virtual void			_ReadDamageKey( i3RegKey * pKey);
	virtual void			_ReadMovementKey( i3RegKey * pKey);
	virtual void			_ReadCrosshairKey( i3RegKey * pKey);

public:
	CWeaponCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	bool					isSame(WEAPON_CLASS_TYPE type, WEAPON::RES_ID nNumber)
	{
		return (GetTypeClass() == type && GetNumber() == nNumber);
	}

	// Fast Zoom
	bool					isEnableFastZoom(void);
	INT32					GetMaxDamage(void);
	INT32					GetDamageForDino();

	// �Ӽ��� ����
public:
	WEAPON::RES_ID			GetNumber();			//	���� ��ȣ
	WEAPON::RES_ID			GetRealNumber();		//	Real ���� ��ȣ
	T_ItemID				GetItemID();

	WEAPON_SLOT_TYPE		GetTypeUsage();			//	���� �з�

	WEAPON_CLASS_TYPE		GetTypeClass();			//	���� Ŭ����
	WEAPON::FIRE_TYPE		GetTypeFire();			//	���� ���
	WEAPON::TRIGGER_TYPE	GetTypeTrigger();		//	�浹 ���	

	WEAPON::DAMAGE_TYPE		GetTypeDamage();			//	���� ���

	REAL32					GetWeight();				//	���� ����(������ ���¹����ΰ�?)
	REAL32					GetRange();					//  ��ȿ �����Ÿ� : ���⸦ �¾����� Ÿ���� ������ �ִ°Ÿ�

	REAL32					GetFireDelay();				// �߻� ����
	REAL32					GetFireDelay2();			// �߻� ���� (���� ����� ������ ����)

	REAL32					GetMoveNormalFire();		// ���� �߻� ���� �̵��ӵ� ���� ���� (Factor)
	REAL32					GetMoveNormalSit();			// ���� ���¿����� �̵��ӵ� ���� ���� (Factor)
	REAL32					GetMoveNormalStand();			// �� ���¿����� �̵��ӵ� ���� ���� (Factor)

	REAL32					GetCrossHairExpandTime();
	REAL32					GetCrossHairContractTime();
	REAL32					GetCrossHairSpeedFactor();		// CrossHair�� Ŀ���ų� �۾����� �ӵ��� ���� (���� �ӵ��� ��)

	REAL32					GetHelmetPenetrate();			// Helmet penetrate
	REAL32					GetHelmetPenetrate_Str();		// ranforce2 Helmet penetrate
	REAL32					GetBulletWeight();			// Helmet penetrate

	bool					AttachedSubWeapon();

	virtual WEAPON::ZOOM_TYPE	GetTypeZoom()				{ return WEAPON::ZOOM_UNKNOWN; }		//	�� ���

	//	�ϳ��� ���⳻�� �������� �ͽ��ټ��� ������ �����Ƿ� ������ m_pWeaponInfo->getExtensionType() ȣ���� �����ϼ���.
	//	������ ���� Ȱ��ȭ�� �ͽ��ټ��� �˱����ؼ� WeaponInfo�� �ִ� �Լ����� ���⿡ �ִ� getCurrentExtensionType() �Լ� �̿��ϼ���. komet
	virtual WEAPON::EXT_TYPE		getExtensionType( INT32 idx)	{ return WEAPON::EXT_NONE; }		//	���� Extension Ÿ�� (1~n)

	virtual INT32				GetFireBulletCount()	{ return 0; }			// �߻�ü ����
	virtual INT32				GetFireDecBulletCount() { return 1;	}		// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	virtual INT32				GetAutoFireCount()		{ return 0; }			// �߻� ���� (0:����, 1:�ܹ�, 2, 3, 4...:����)	���� ����
	virtual INT32				GetAutoFireCount2()		{ return 0; }			// �߻� ���� (0:����, 1:�ܹ�, 2, 3, 4...:����)  ������ ����
	virtual REAL32				GetAutoFireDelay()		{ return 0; }			// ���� ���� �߻� ����
	virtual INT32				GetAutoPointShotEnable(){ return 0; }		//�ű� ���� �۵� ���.

	virtual REAL32				GetLoadBulletTime()		{ return 0;	}		// ���� �����̸� ���� �ð�
	virtual REAL32				GetReloadTime()			{ return 0;	}			// �������� �ɸ��� �ð�
	virtual REAL32				GetReloadTimeDual()		{ return 0; }			// ��� źâ ���� �ð�

	virtual INT32				GetLoadBullet()			{ return 0; }				// ���� ���� ź�� ��
	virtual INT32				GetReloadBulletCount()	{ return 0; }		// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	virtual INT32				GetMaxBullet()			{ return 0; }				// �ִ� ���� ź�� ��
	virtual INT32				GetMaxScopeLevel()		{ return 0; }			// �ִ� ������ �ܰ� (WEAPON::ZOOM_SCOPE Ÿ�Ը� ����)
	virtual INT32				GetNeedToLoadBullet()	{ return 0;	}	// Load Magazine ��, Load Bullet�� �ؾ��ϴ°�?

	virtual INT32				GetDamage();			//	���� ���ݷ�	
	virtual REAL32				GetDamageOnlyUI();
	INT32						GetMaxDamageForDino(void);
	//��ô �����
	virtual REAL32				GetExplosion_Time()		{ return 0; }			// ���� �����ð�( Grenade�� ���)
	virtual REAL32				GetExplosion_Range()	{ return 0; }			// ���� ��ȿ ����
	virtual REAL32				GetExplosion_EffectiveSlope() { return 0; } // ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope
	virtual REAL32				GetThrowSpeed()			{ return 0;	}			// ������ �ӵ�
	virtual REAL32				GetEffectiveTime()		{ return 0; }			// ����Ʈ ��ȿ �ð�
	virtual REAL32				GetAttachLength()		{ return 0; }

	//Move( %)

	virtual REAL32				GetMoveNormalZoom()		{ return 0;	}		// ���� ���¿����� �̵��ӵ� ���� ���� (Factor)

	//
	virtual REAL32				GetCrossHairExpand()	{ return 4.f; }			// Crosshiar�� �о��� ��, ���̰� ������� ����
	virtual REAL32				GetCrossHairMin()		{ return 4.f; }		// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	virtual REAL32				GetCrossHairMax()		{ return 4.f; }
	virtual REAL32				GetCrossHairSizeDamage() { return 0; }		// Damage�� �Ծ��� ��, CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32				GetCrossHairSizeJump() { return 0; }		// Jump ���� �ÿ� CrossHair�� �о����� ���� (Factor)
	virtual REAL32				GetCrossHairSizeMove() { return 0; }		// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32				GetCrossHairSizeMoveSit() { return 0; }
	virtual REAL32				GetCrossHairSizeWalk() { return 0; }		// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32				GetCrossHairSizeWalkSit() { return 0; }
	virtual REAL32				GetCrossHairSizeStand() { return 0;	}	// Idle ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32				GetCrossHairSizeSit()	{ return 0;	}	// Sit ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)	
	virtual REAL32				GetCrossHairSizeZoom()	{ return 0; }		// ���� �ÿ� CrossHair�� �о����� ���� (Factor) ?
	virtual REAL32				GetCrossHairSizeNoZoom(void)	{ return 0; }

	virtual INT32				GetCrossHairInvisible() { return 0; }		// 0 : All Visible 2:Scope Invisible


	virtual REAL32				GetRecoilVertAngle() { return 0; }			// 1�ߴ� ���� �ݵ���
	virtual REAL32				GetRecoilVertMax() { return 0; }			// �ִ� ���� �ݵ���

	virtual REAL32				GetRecoilHorzAngle() { return 0; }			// �¿� ��鸲 ��
	virtual REAL32				GetRecoilHorzMax() { return 0; }			// �ִ� ���� �¿� ��鸲 ��
	virtual INT32				GetRecoilHorzCount() { return 0; }			// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����
	virtual INT32				GetOneHorzEnable() { return 0; }			// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����

	virtual REAL32				GetDeviation() { return 0; }				// ������
	virtual REAL32				GetDeviationMax() { return 0; }				// �ִ� ���� ������
	virtual REAL32				GetDeviationOnlyUI() { return 0; }
	virtual REAL32				GetDeviation_DiminutionDistance(void)		{ return 0.0f; }
	virtual REAL32				GetDeviation_Decrement(void)				{ return 0.0f; }

	virtual REAL32				GetZoomFOV() { return 0; }					// 1��Ī�϶� Zoom���� FOV��
	virtual REAL32				GetScopeFOV() { return 0;	}			// 1�� Scope���� FOV��
	virtual REAL32				GetScopeFOV2() { return 0; }				// 2�� Scope���� FOV��

	virtual REAL32				GetCamSyncRate() { return 0; }				// 0:�ѱ�ݵ��� ������   1:��������

	virtual REAL32				GetDPD_Rate() { return 0; }

	virtual I3COLOR*			GetBulletSmokeColor() { return nullptr;	}	//	�Ѿ� ���� ���� ����	
	virtual REAL32				GetBulletSmokeRepeatTexSize() const { return 1.f; }	//	�Ѿ� ���� ���� �̹����� Ÿ�ϸ��Ǵ� ���� (����: m)
	virtual REAL32				GetBulletSmokeGrowThickness() const { return 0;	}	//	�Ѿ� ���� ���� ���İ� ������� ���� ���Ʒ��� ������ �β� (����: m)
	virtual REAL32				GetBulletSmokeLifeTime() const { return 0; }		//	�Ѿ� ���� ���� ���� �ð� (����: ��) 0.0�̸� ��Ȱ��ȭ�ȴ�.
	virtual REAL32				GetBulletSmokeThickness() const { return 0;	}//	�Ѿ� ���� ���� �β� (����: m)
	virtual REAL32				GetBulletSmokeStartSpeed() const { return 0; }	//	�Ѿ� ���� ���� ������ �̵� �ӵ� (�ѱ�) (����: �ʴ� m)
	virtual REAL32				GetBulletSmokeEndSpeed() const { return 0; }		//	�Ѿ� ���� ���� ���� �̵� �ӵ� (Ÿ����) (����: �ʴ� m)

	virtual bool				GetPumpAction()	{ return 0;	}		//	�����׼� ��� ����	
	virtual INT32				GetFireDelayOnlyUI() const { return 0; }
	virtual	INT32				GetRecoilVertOnlyUI() const { return 0; }
	virtual	INT32				GetRecoilHorzOnlyUI() const { return 0; }
	virtual REAL32				GetRecoilVertAngle_Recovertime() const {return 1.0f;}
	virtual REAL32				GetRecoilHorzAngle_Recovertime() const {return 1.0f;}

	// KEYWORD : WEAPON_DUALKNIFE
	virtual REAL32				GetAttackTimeA( INT32 num)		{ return 0;}
	virtual REAL32				GetAttackTimeB( INT32 num)		{ return 0;}
	virtual REAL32				GetDamageA( INT32 num)			{ return 0;}
	virtual REAL32				GetDamageB( INT32 num)			{ return 0;}

	// KEYWORD : PHYSIX_CHARA_FORCE
	REAL32						GetPhysixForceRate();

	virtual bool				IsFastReloadAnimation( void)	{ return false; }
	virtual bool				IsAlphablendUsed( void);
	virtual bool				IsLoadBulletLeftBarrel( void)	{ return false; }
	virtual bool				IsLoadMagazineLeftBarrel( void)	{ return false; }
	virtual bool				IsLoadMagazineReady(void)		{ return false; }

	virtual INT32				GetScopeType( void)		{ return -1; }

private:

#if defined( I3_DEBUG )
	void						CheckRegDataType();	// ������Ʈ�� ������ Ÿ�� �˻�
#endif
	
};

class CMeleeWeaponCoreInfo : public CWeaponCoreInfo
{
	I3_CLASS_DEFINE(CMeleeWeaponCoreInfo, CWeaponCoreInfo);

private:
	REAL32					m_rAttackDamageA[ 1];					// ���� Ŭ�� ���� ������. (��: ù��° ������ �ε��� 0)
	REAL32					m_rAttackDamageA_ForDino[ 1];			// ���� Ŭ�� ���� ������. (��: ù��° ������ �ε��� 0)
	INT32					m_nAutoFireCount;				// ���� ���� �ִ� Ƚ��
	VEC3D					m_v3CollisionBoxExtent;			//  �и� ������ �� ���� �浹�� ���� �ڽ� ũ��
	INT32					m_nLoadBullet;				// ���� ���� ź�� ��
	INT32					m_nMaxBullet;				// �ִ� ���� ź�� ��	
	INT32					m_bEnableBounce;

public:
	CMeleeWeaponCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	virtual void			GetCollisionBoxExtent(VEC3D* pVec);
	virtual INT32			GetDamage();			//	���� ���ݷ�	
	virtual INT32			GetAutoFireCount();
	virtual INT32			GetLoadBullet();			// ���� ���� ź�� ��
	virtual INT32			GetMaxBullet();
	virtual bool			IsEnableBounce();
};

class CGunCoreInfo : public CWeaponCoreInfo
{
	I3_CLASS_DEFINE(CGunCoreInfo, CWeaponCoreInfo);

private:
	INT32					m_nReloadBulletCount;		// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	INT32					m_nLoadBullet;				// ���� ���� ź�� ��
	INT32					m_nLoadBulletType;				// ���� ���� ź�� ��
	INT32					m_nMaxBullet;				// �ִ� ���� ź�� ��
	INT32					m_nFireBulletCount;			// �߻�ü ����
	INT32					m_nFireDecBulletCount;		// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����

	REAL32					m_rCrossHairExpand;			// Crosshiar�� �о��� ��, ���̰� ������� ����
	REAL32					m_rCrossHairMin;			// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	REAL32					m_rCrossHairMax;

	REAL32					m_rCrossHairSizeDamage;		// Damage�� �Ծ��� ��, CrossHair�� �Ͼ����� ���� (Factor)
	REAL32					m_rCrossHairSizeJump;		// Jump ���� �ÿ� CrossHair�� �о����� ���� (Factor)
	REAL32					m_rCrossHairSizeMove;		// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	REAL32					m_rCrossHairSizeMoveSit;
	REAL32					m_rCrossHairSizeWalk;		// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	REAL32					m_rCrossHairSizeWalkSit;

	REAL32					m_rCrossHairSizeStand;		// Idle ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	REAL32					m_rCrossHairSizeSit;		// Sit ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)	
	REAL32					m_rCrossHairSizeZoom;		// ���� �ÿ� CrossHair�� �о����� ���� (Factor) ?
	REAL32					m_rCrossHairSizeNoZoom;		// Sniper �ѿ� ����.

	REAL32					m_RecoilVertAngle;			// 1�ߴ� ���� �ݵ���
	REAL32					m_RecoilVertMax;			// �ִ� ���� �ݵ���

	REAL32					m_RecoilHorzAngle;			// �¿� ��鸲 ��
	REAL32					m_RecoilHorzMax;			// �ִ� ���� �¿� ��鸲 ��
	INT32					m_RecoilHorzCount;			// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����
	INT32					m_OneHorzEnable;			// ���� �ݵ� ������ 0: �� 1: ��

	INT32					m_nAutoFireCount;			// �߻� ���� (0:����, 1:�ܹ�, 2, 3, 4...:����)
	INT32					m_bNeedToLoadBullet;

	REAL32					m_rReloadTime;				// �������� �ɸ��� �ð�

	REAL32					m_DPD_Rate;

	REAL32					m_rReloadTimeDual;			// ��� źâ ���� �ð�

	REAL32					m_RecoilVertAngle_Recovertime;	//����ݵ� �����ӵ�
	REAL32					m_RecoilHorzAngle_Recovertime;	//����ݵ� �����ӵ�

protected:
	virtual void			_ReadCrosshairKey( i3RegKey * pKey);
	void					_ReadAccuracy( i3RegKey * pKey);
	virtual void			_ReadDamageKey( i3RegKey * pKey);

public:
	CGunCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	virtual INT32			GetLoadBullet();			// ���� ���� ź�� ��
	virtual INT32			GetLoadBulletType();		// ������ ź�� ��
	virtual INT32			GetReloadBulletCount();		// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	virtual INT32			GetMaxBullet();				// �ִ� ���� ź�� ��
	virtual INT32			GetFireBulletCount();		// �߻�ü ����
	virtual INT32			GetFireDecBulletCount();	// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����

	virtual REAL32			GetCrossHairExpand();			// Crosshiar�� �о��� ��, ���̰� ������� ����
	virtual REAL32			GetCrossHairMin();				// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	virtual REAL32			GetCrossHairMax();
	virtual REAL32			GetCrossHairSizeDamage();		// Damage�� �Ծ��� ��, CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeJump();			// Jump ���� �ÿ� CrossHair�� �о����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeMove();		// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeMoveSit();
	virtual REAL32			GetCrossHairSizeWalk();		// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeWalkSit();
	virtual REAL32			GetCrossHairSizeStand();	// Idle ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeZoom();		// ���� �ÿ� CrossHair�� �о����� ���� (Factor) ?
	virtual REAL32			GetCrossHairSizeNoZoom(void);

	virtual REAL32			GetRecoilVertAngle();		// 1�ߴ� ���� �ݵ���
	virtual REAL32			GetRecoilVertMax();			// �ִ� ���� �ݵ���
	
	virtual REAL32			GetRecoilHorzAngle();		// �¿� ��鸲 ��
	virtual REAL32			GetRecoilHorzMax();			// �ִ� ���� �¿� ��鸲 ��
	virtual INT32			GetRecoilHorzCount();		// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����
	virtual INT32			GetOneHorzEnable();

	virtual INT32			GetAutoFireCount();			// �߻� ���� (0:����, 1:�ܹ�, 2, 3, 4...:����)
	virtual INT32			GetNeedToLoadBullet();	// Load Magazine ��, Load Bullet�� �ؾ��ϴ°�?

	virtual REAL32			GetReloadTime();			// �������� �ɸ��� �ð�
	virtual REAL32			GetReloadTimeDual();		// ��� źâ ���� �ð�
	virtual REAL32			GetDPD_Rate();

	virtual REAL32			GetRecoilVertAngle_Recovertime() const;
	virtual REAL32			GetRecoilHorzAngle_Recovertime() const;
};

class CGrenadeCoreInfo :	public CWeaponCoreInfo
{
	I3_CLASS_DEFINE( CGrenadeCoreInfo, CWeaponCoreInfo);

private:
	COnlyGrenadeInfo*		m_pOnlyGreandeInfo;

public:
	CGrenadeCoreInfo();
	virtual ~CGrenadeCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	// �Ӽ��� ����.
	virtual INT32			GetFireBulletCount(); 			// �߻�ü ����
	virtual INT32			GetFireDecBulletCount(); 	// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	virtual INT32			GetLoadBullet();				// ���� ���� ź�� ��
	virtual INT32			GetReloadBulletCount();		// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	virtual INT32			GetMaxBullet();				// �ִ� ���� ź�� ��

	//��ô �����
	virtual REAL32			GetExplosion_Time(); 		// ���� �����ð�( Grenade�� ���)
	virtual REAL32			GetExplosion_Range();			// ���� ��ȿ ����
	virtual REAL32			GetExplosion_EffectiveSlope(); // ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope
	virtual REAL32			GetThrowSpeed(); 			// ������ �ӵ�
	virtual REAL32			GetEffectiveTime(); 			// ����Ʈ ��ȿ �ð�

	virtual REAL32			GetAttachLength();

	virtual REAL32			GetDamageTerm();
};

class CBombCoreInfo : public CWeaponCoreInfo
{
	I3_CLASS_DEFINE(CBombCoreInfo, CWeaponCoreInfo);
private:
	INT32					m_nFireBulletCount;			// �߻�ü ����
	INT32					m_nFireDecBulletCount;		// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����

	INT32					m_nLoadBullet;				// ���� ���� ź�� ��
	INT32					m_nMaxBullet;				// �ִ� ���� ź�� ��

	REAL32					m_rExplosion_Time;			// ���� �����ð�( Grenade�� ���)
	REAL32					m_rExplosion_Range;			// ���� ��ȿ ����
	REAL32					m_rExplosion_EffectiveSlope;// ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope

public:
	CBombCoreInfo();
	virtual ~CBombCoreInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey, INT32 iShift);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	// �Ӽ��� ����.
	virtual INT32			GetFireBulletCount();			// �߻�ü ����
	virtual INT32			GetFireDecBulletCount();	// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	virtual INT32			GetLoadBullet();				// ���� ���� ź�� ��
	virtual INT32			GetMaxBullet();				// �ִ� ���� ź�� ��

	//��ô �����
	virtual REAL32			GetExplosion_Time();		// ���� �����ð�( Grenade�� ���)
	virtual REAL32			GetExplosion_Range();			// ���� ��ȿ ����
	virtual REAL32			GetExplosion_EffectiveSlope(); // ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope
};

#endif
