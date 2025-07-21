#if !defined( __GRENADE_INFO_H)
#define __GRENADE_INFO_H

#include "WeaponInfo.h"

class COnlyGrenadeInfo;

class CGrenadeInfo :	public CWeaponInfo
{
	I3_CLASS_DEFINE( CGrenadeInfo, CWeaponInfo);

private:
	COnlyGrenadeInfo*		m_pOnlyGrenadeInfo;
	TRACER_TYPE				m_TraceType;

private:
	void					_SetTracerType(i3RegKey* pKey);

public:
	CGrenadeInfo();
	virtual ~CGrenadeInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	virtual void			FillBulletCountString(i3::stack_wstring& out_wstrBulletCount) override;

	// �Ӽ��� ����.
	virtual INT32			GetFireBulletCount() const override; 			// �߻�ü ����
	virtual INT32			GetFireDecBulletCount() const override; 	// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	virtual INT32			GetLoadBullet() const override;				// ���� ���� ź�� ��
	virtual INT32			GetReloadBulletCount() const override;		// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	virtual INT32			GetMaxBullet() const override;				// �ִ� ���� ź�� ��
	
	//��ô �����
	virtual REAL32			GetExplosion_Time() const override; 				// ���� �����ð�( Grenade�� ���)
	virtual REAL32			GetExplosion_Range() const override;				// ���� ��ȿ ���� : �Ϲ� ����
	virtual REAL32			GetExplosion_NearRange() const override;			// ���� ��ȿ ���� : �ֺ� ���� (ex. Claymore)
	virtual REAL32			GetExplosion_EffectiveSlope() const override;	// ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope
	virtual REAL32			GetDetected_Range() const override;				// ���� �Ÿ� (Claymore)
	virtual REAL32			GetThrowSpeed() const override; 					// ������ �ӵ�
	virtual REAL32			GetExThrowSpeed() override; 						// ���� �ͽ��ټ� ������ �ӵ�
	virtual REAL32			GetEffectiveTime() const override; 				// ����Ʈ ��ȿ �ð�

	virtual REAL32			GetAttachLength() const override;

	virtual REAL32			GetDamageTerm() const;

	virtual INT32			GetIncreaseCount() override;

	// ���� Ŭ�� ���ݽ� ���� Ÿ�̹�. ���ڴ� ���� Ƚ��° �ε��� (��: ù��° ������ �ε��� 0)
	virtual REAL32			GetAttackTimeA( INT32 num) const override;

	// ������ Ŭ�� ���ݽ� ���� Ÿ�̹�.
	virtual REAL32			GetAttackTimeB( INT32 num) const override;

	// KEYWORD : DECOY_SOUND
	virtual void			GetDefaultDecoySound(i3::rc_wstring& out) override;

	virtual bool			GetDual( void) const override;

	virtual bool			GetUseThrowDelay( void) const override;

	virtual UINT32			GetBombtriggerItemID() const override;

	virtual bool			GetUseBombTrigger( void) const override;
};

#endif