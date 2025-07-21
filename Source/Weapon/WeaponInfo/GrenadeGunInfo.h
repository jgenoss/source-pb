#if !defined( __GRENADE_GUN_INFO_H)
#define __GRENADE_GUN_INFO_H

#include "GunInfo.h"
#include "GrenadeInfo.h"

class COnlyGrenadeInfo;

class CGrenadeGunInfo :	public CGrenadeInfo
{
	I3_CLASS_DEFINE( CGrenadeGunInfo, CGrenadeInfo);

private:
	COnlyGrenadeInfo*		m_pOnlyGreandeInfo;

	REAL32					m_Homing;

public:
	CGrenadeGunInfo();
	virtual ~CGrenadeGunInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	REAL32					getHoming(void) const;

	// �Ӽ��� ����.
	virtual INT32			GetFireBulletCount() const override; 			// �߻�ü ����
	virtual INT32			GetFireDecBulletCount() const override; 	// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	virtual INT32			GetLoadBullet() const override;				// ���� ���� ź�� ��
	virtual INT32			GetReloadBulletCount() const override;		// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	virtual INT32			GetMaxBullet() const override;				// �ִ� ���� ź�� ��

	//��ô �����
	virtual REAL32			GetExplosion_Time() const override; 		// ���� �����ð�( Grenade�� ���)
	virtual REAL32			GetExplosion_Range() const override;			// ���� ��ȿ ����
	virtual REAL32			GetExplosion_EffectiveSlope() const override; // ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope
	virtual REAL32			GetThrowSpeed() const override; 			// ������ �ӵ�
	virtual REAL32			GetEffectiveTime() const override; 			// ����Ʈ ��ȿ �ð�
	
	virtual REAL32			GetAttachLength() const override;				// KEYWORD : WEAPON_C5	���� �ٴ� �Ÿ�

	virtual REAL32			GetDamageTerm() const override;

};

#endif