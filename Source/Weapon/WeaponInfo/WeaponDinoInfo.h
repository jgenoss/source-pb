#if !defined( __WEAPON_DINO_INFO_H)
#define __WEAPON_DINO_INFO_H

#include "WeaponInfo.h"
#include "MeleeWeaponInfo.h"

class CWeaponDinoInfo :	public CWeaponInfo
{
	I3_CLASS_DEFINE(CWeaponDinoInfo, CWeaponInfo);

private:
	REAL32		m_rAttackTime[ATTACK_NUM_MAX];
	REAL32		m_rAttackDamage[ATTACK_NUM_MAX];
	REAL32		m_rAttackFreezeTime;					

	REAL32		m_rDPD_Rate;							// �Ÿ��� ��������

	UINT8		m_nAutoFireCount;						// �ڵ� �߻� Ƚ��
	UINT8		m_nFireBulletCount;						// �߻�ü ����

	REAL32		m_rDeviation;							// ������

	REAL32		m_rExplosion_Range;						// ���� ��ȿ ����
	REAL32		m_rExplosion_EffectiveSlope;			// �Ÿ��� ������ ������ Slop

	REAL32		m_rEffectiveTime;						// ����Ʈ ��ȿ �ð�

	REAL32		m_rSkillDuration;						// ��ų ���� �ð�
	REAL32		m_rSkillCoolTime;						// ��ų ��Ÿ��
	REAL32		m_rSkillMovementRate;					// ��ų �̵��ӵ���

	INT32		m_AssistKillImageIndex;			// Assist Kill Image Index
	INT32		m_HitSoundType;					// ���� ��Ʈ ���� ����
	INT32		m_HitSoundIndex;				// ��Ʈ ���� �ε���
	INT32		m_TerrainHitSoundIndex;			// ���� ��Ʈ ����

	REAL32		m_rCrossHairExpand;			// Crosshiar�� �о��� ��, ���̰� ������� ����
	REAL32		m_rCrossHairMin;			// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	REAL32		m_rCrossHairMax;
	//--------------------------------------------------------------------------------------//
	//										���� ����										//
	//--------------------------------------------------------------------------------------//
private:
	I3COLOR		m_cBulletSmokeColor;			//	�Ѿ� ���� ���� ����	
	REAL32		m_rBulletSmokeRepeatTexSize;	//	�Ѿ� ���� ���� �̹����� Ÿ�ϸ��Ǵ� ���� (����: m)
	REAL32		m_rBulletSmokeGrowThickness;	//	�Ѿ� ���� ���� ���İ� ������� ���� ���Ʒ��� ������ �β� (����: m)
	REAL32		m_rBulletSmokeLifeTime;			//	�Ѿ� ���� ���� ���� �ð� (����: ��) 0.0�̸� ��Ȱ��ȭ�ȴ�.
	REAL32		m_rBulletSmokeThickness;		//	�Ѿ� ���� ���� �β� (����: m)
	REAL32		m_rBulletSmokeStartSpeed;		//	�Ѿ� ���� ���� ������ �̵� �ӵ� (�ѱ�) (����: �ʴ� m)
	REAL32		m_rBulletSmokeEndSpeed;			//	�Ѿ� ���� ���� ���� �̵� �ӵ� (Ÿ����) (����: �ʴ� m)

protected:
	void				_ReadBulletTracerKey( i3RegKey * pKey);
	virtual void		_ReadDamageKey( i3RegKey * pKey) override;
	virtual void		_ReadCrosshairKey( i3RegKey * pKey);

public:
	CWeaponDinoInfo();
	virtual ~CWeaponDinoInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	virtual REAL32			GetDamageA( INT32 num) const override;				// ������
	virtual REAL32			GetAttackTimeA( INT32 num) const override;			// ���� ���� �ӵ�
	
	virtual INT32			GetAutoFireCount(void) const override;				// �ڵ� �߻� Ƚ��
	virtual INT32			GetFireBulletCount(void) const override;			// �߻�ü ����
	virtual REAL32			GetDeviation(void) const override;					// ������

	virtual REAL32			GetExplosion_Range(void) const override;			// ���� ��ȿ ����
	virtual REAL32			GetExplosion_EffectiveSlope(void) const override;	// ���� �Ÿ��� ������ ������

	virtual REAL32			GetEffectiveTime(void) const override;				// ����Ʈ ��ȿ �ð�

	virtual REAL32			GetAttackFreezeTime(void) const override;

	virtual REAL32			GetSkillDuration(void) const override;				// ��ų ���ӽð�
	virtual REAL32			GetSkillCoolTime(void) const override;				// ��ų ��Ÿ��
	virtual REAL32			GetSkillSkillMovementRate(void) const override;	// ��ų �̵��ӵ���

	virtual WEAPON::ATTACK_TYPE	GetAttackType(WEAPON::FIRE_ORDER fireOrder, INT32 i32FireCount) const override;

	virtual void			SetBulletSmokeInfo(TracerBulletInfo* pBulletInfo) override;

	//	�Ѿ� ���� ���� ����	
	virtual void			GetBulletSmokeColor( I3COLOR * pOut);

	//	�Ѿ� ���� ���� �̹����� Ÿ�ϸ��Ǵ� ���� (����: m)
	virtual REAL32			GetBulletSmokeRepeatTexSize() const override;
	virtual REAL32			GetBulletSmokeGrowThickness() const override;	//	�Ѿ� ���� ���� ���İ� ������� ���� ���Ʒ��� ������ �β� (����: m)
	virtual REAL32			GetBulletSmokeLifeTime() const override;			//	�Ѿ� ���� ���� ���� �ð� (����: ��) 0.0�̸� ��Ȱ��ȭ�ȴ�.
	virtual REAL32			GetBulletSmokeThickness() const override;		//	�Ѿ� ���� ���� �β� (����: m)
	virtual REAL32			GetBulletSmokeStartSpeed() const override;		//	�Ѿ� ���� ���� ������ �̵� �ӵ� (�ѱ�) (����: �ʴ� m)
	virtual REAL32			GetBulletSmokeEndSpeed() const override;			//	�Ѿ� ���� ���� ���� �̵� �ӵ� (Ÿ����) (����: �ʴ� m)

	INT32					GetAssistKillImageIndex( void) const	{ return m_AssistKillImageIndex; }
	INT32					GetHitSoundType( void) const			{ return m_HitSoundType; }
	INT32					GetHitSoundIndex( void) const			{ return m_HitSoundIndex; }
	INT32					GetTerrainHitSoundIndex( void) const	{ return m_TerrainHitSoundIndex; }

	virtual REAL32			GetCrossHairExpand() const override;			// Crosshiar�� �о��� ��, ���̰� ������� ����
	virtual REAL32			GetCrossHairMin() const override;				// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	virtual REAL32			GetCrossHairMax() const override;

	virtual REAL32			GetDPD_Rate() const override { return m_rDPD_Rate; }
};

#endif // __WEAPON_DINO_INFO_H