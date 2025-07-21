#if !defined( __BOMB_INFO_H)
#define __BOMB_INFO_H


#include "WeaponInfo.h"

class CBombInfo :	public CWeaponInfo
{
	I3_CLASS_DEFINE(CBombInfo, CWeaponInfo);
private:
	VAR_LR(UINT8,123)		m_nFireBulletCount = 0;			// �߻�ü ����
	VAR_LR(UINT8,124)		m_nFireDecBulletCount = 1;		// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����

	VAR_LR(UINT16,125)		m_nLoadBullet = 0;				// ���� ���� ź�� ��
	VAR_LR(UINT16,126)		m_nMaxBullet = 0;				// �ִ� ���� ź�� ��

	VAR_LR(REAL32,127)		m_rExplosion_Time = 0.0f;			// ���� �����ð�( Grenade�� ���)
	VAR_LR(REAL32,128)		m_rExplosion_Range = 0.0f;			// ���� ��ȿ ����
	VAR_LR(REAL32,129)		m_rExplosion_EffectiveSlope = 0.0f;// ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope

	bool					m_bUseBombTrigger = false;
public:
	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	// �Ӽ��� ����.
	virtual INT32			GetFireBulletCount() const override;			// �߻�ü ����
	virtual INT32			GetFireDecBulletCount() const override;	// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	virtual INT32			GetLoadBullet() const override;	// ���� ���� ź�� ��
	virtual INT32			GetMaxBullet() const override;	// �ִ� ���� ź�� ��

	//��ô �����
	virtual REAL32			GetExplosion_Time() const override;		// ���� �����ð�( Grenade�� ���)
	virtual REAL32			GetExplosion_Range() const override;			// ���� ��ȿ ����
	virtual REAL32			GetExplosion_EffectiveSlope() const override; // ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope

	virtual bool			GetUseBombTrigger() const override;
};

#endif