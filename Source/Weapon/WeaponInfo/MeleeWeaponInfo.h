#if !defined( __KNIFE_INFO_H)
#define __KNIFE_INFO_H

#include "WeaponInfo.h"

#define  ATTACK_NUM_MAX		4		//	 �ִ� �ѹ� Ŭ���� ���� ��ȣ

class CMeleeWeaponInfo :	public CWeaponInfo
{
	I3_CLASS_DEFINE(CMeleeWeaponInfo, CWeaponInfo);

private:
	REAL32					m_rExplosion_Time = 0.0f;			// ���� �����ð�( Grenade�� ���)

	VAR_LR(UINT8,177)		m_nAutoFireCount = 1;			// ���� ���� �ִ� Ƚ��
	VAR_LR(UINT8,178)		m_nAutoFireCount2 = 1;			// ������ ���� �ִ� Ƚ��

	VAR_LR(REAL32, 179)	m_rAttackTimeA[ATTACK_NUM_MAX] = { 0.0f, };	// ���� Ŭ�� ���ݽ� ���� Ÿ�̹�. (��: ù��° ������ �ε��� 0)
	VAR_LR(REAL32, 180)	m_rAttackTimeB[ATTACK_NUM_MAX] = { 0.0f, };	// ������ Ŭ�� ���ݽ� ���� Ÿ�̹�. (��: ù��° ������ �ε��� 0)

	REAL32					m_rAttackDamageA[ATTACK_NUM_MAX] = { 0.0f, };	// ���� Ŭ�� ���� ������. (��: ù��° ������ �ε��� 0)
	REAL32					m_rAttackDamageB[ATTACK_NUM_MAX] = { 0.0f, };	// ������ Ŭ�� ���� ������. (��: ù��° ������ �ε��� 0)

	REAL32					m_rAttackDamageA_ForDino[ATTACK_NUM_MAX] = { 0.0f, };	// ���� Ŭ�� ���� ������. (��: ù��° ������ �ε��� 0)
	REAL32					m_rAttackDamageB_ForDino[ATTACK_NUM_MAX] = { 0.0f, };	// ������ Ŭ�� ���� ������. (��: ù��° ������ �ε��� 0)

	REAL32					m_rThrowSpeed = 0.0f;				// ������ �ӵ�							

	INT32					m_iInstanceCount = 16;			// �������� �ν��Ͻ� �ִ� ����
	REAL32					m_rHideTime = 5.0f;				// ������ �ν��Ͻ��� ������� �ð�

	VEC3D					m_v3CollisionBoxExtent;		//  �и� ������ �� ���� �浹�� ���� �ڽ� ũ��

	UINT16					m_nLoadBullet = 0;				// ���� ���� ź�� ��
	UINT16					m_nMaxBullet = 0;				// �ִ� ���� ź�� ��

	REAL32					m_rThrowDelayTime = 0.0f;			// ������ �������� ���� �ð�

	REAL32					m_RotateFront = -1.0f;				// ������ ���� Weapon ����
	REAL32					m_WallBetweenDistance = 0.12f;		// ���� ���� ������ �Ÿ�

public:
	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	
	void			GetCollisionBoxExtent(VEC3D* pVec) const;

	// �Ӽ���
	virtual REAL32			GetExplosion_Time() const override;	// ���� �����ð�( Grenade�� ���)

	// ���� Ŭ�� ���ݽ� ���� Ÿ�̹�. ���ڴ� ���� Ƚ��° �ε��� (��: ù��° ������ �ε��� 0)
	virtual REAL32			GetAttackTimeA( INT32 num) const override;

	// ������ Ŭ�� ���ݽ� ���� Ÿ�̹�.
	virtual REAL32			GetAttackTimeB( INT32 num) const override;
	
	//	���� ���ݷ�
	virtual INT32			GetDamage() const override;

	// ���� Ŭ�� ���� ������
	virtual REAL32			GetDamageA( INT32 num) const override;

	// ������ Ŭ�� ���� ������
	virtual REAL32			GetDamageB( INT32 num) const override;

	// ������ �ӵ�
	virtual REAL32			GetThrowSpeed() const override;

	virtual INT32			GetLoadBullet() const override;			// ���� ���� ź�� ��

	REAL32			GetRotateFront();
	REAL32			GetWallBetweenDistance();

	virtual INT32			GetAutoFireCount() const override;
	virtual INT32			GetAutoFireCount2() const override;

	virtual INT32			GetMaxBullet() const override;
	
	virtual WEAPON::ATTACK_TYPE	GetAttackType(WEAPON::FIRE_ORDER fireOrder, INT32 i32FireCount) const override;

	INT32					GetInstanceCount( void) const		{ return m_iInstanceCount; }
	REAL32					GetHideTime( void) const			{ return m_rHideTime; }

	bool					IsEnableBounce() const				{ return _isAttribute(WEAPON_ATTRIBUTE_BOUNCED_ATTACK); }

	REAL32					GetThrowDelayTime( void) const		{ return m_rThrowDelayTime; }

	bool					isDelayedFireSound( void) const		{ return _isAttribute(WEAPON_ATTRIBUTE_DELAYED_FIRE_SOUND); }
	bool					NoMoreSecSound( void) const			{ return _isAttribute(WEAPON_ATTRIBUTE_NOMORE_SECONDARY_SOUND); }
	bool					DoNotThrowLast( void) const			{ return _isAttribute(WEAPON_ATTRIBUTE_DO_NOT_THROW_LAST); }
	bool					UseHitedAnimation( void) const		{ return _isAttribute(WEAPON_ATTRIBUTE_HITED_ANIMATION); }
	BOOL					IsSpraySecAttack( void) const		{ return _isAttribute(WEAPON_ATTRIBUTE_SPRAY_SECONDARYATTACK); }
};

#endif