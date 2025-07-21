#if !defined( __ONLY_GRENADE_INFO_H)
#define __ONLY_GRENADE_INFO_H

/*
�ڸ��� 2008. 6. 25
GrenadeShell�� �ѱ���� �Ӽ��� ������, ��ô���� �Ӽ��� ������ �ֱ� ������
�ΰ��� �Ӽ��� �� �����ؾ� �ϱ� ������ ��ô���� �Ӽ��� CGrenadeInfo���� ���� Ŭ����
*/

#define  GRENADE_ATTACK_NUM_MAX		1		//	 �ִ� �ѹ� Ŭ���� ���� ��ȣ

class COnlyGrenadeInfo : public i3NamedElement
{
	I3_CLASS_DEFINE( COnlyGrenadeInfo, i3NamedElement);

private:
	//��ô �����
	VAR_LR(REAL32, 181)			m_rExplosion_Time = 0.0f;			// ���� �����ð�( Grenade�� ���)
	VAR_LR(REAL32, 182)			m_rExplosion_Range = 0.0f;			// ���� ��ȿ ���� : �Ϲ� ����
	VAR_LR(REAL32, 183)			m_rExplosion_NearRange = 0.0f;		// ���� ��ȿ ���� : �ֺ� ���� (ex. Claymore)
	VAR_LR(REAL32, 184)			m_rExplosion_EffectiveSlope = 0.0f;// ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope
	VAR_LR(REAL32, 185)			m_rDetected_Range = 0.0f;			// ���� ���� (Claymore)
	VAR_LR(REAL32, 186)			m_rThrowSpeed = 0.0f;				// ������ �ӵ�
	VAR_LR(REAL32, 187)			m_rExThrowSpeed = 0.0f;			// ���� �ͽ��ټ� ������ �ӵ�
	VAR_LR(REAL32, 188)			m_rEffectiveTime = 0.0f;			// ����Ʈ ��ȿ �ð�  ���� �÷��������� ���

	VAR_LR(REAL32, 189)			m_rAttachLength = 0.0f;			// KEYWORD : WEAPON_C5	���� �ٴ� �Ÿ�

	VAR_LR(REAL32, 190)			m_rCheckDamageTerm = 0.0f;			// WP_SMOKE : ������ ��� ��

	VAR_LR(REAL32, 191)			m_rAttackTimeA[GRENADE_ATTACK_NUM_MAX] = { 0.0f, };	// ���� Ŭ�� ���ݽ� ���� Ÿ�̹�. (��: ù��° ������ �ε��� 0)
	VAR_LR(REAL32, 192)			m_rAttackTimeB[GRENADE_ATTACK_NUM_MAX] = { 0.0f, };	// ������ Ŭ�� ���ݽ� ���� Ÿ�̹�. (��: ù��° ������ �ε��� 0)

	// KEYWORD : DECOY_SOUND
	i3::rc_wstring	m_strDefaultDecoySound;


	VAR_LR(UINT8, 193)			m_nFireBulletCount = 0;			// �߻�ü ����
	VAR_LR(UINT8, 194)			m_nFireDecBulletCount = 1;		// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����

	VAR_LR(UINT16, 195)		m_nLoadBullet = 0;				// ���� ���� ź�� ��
	VAR_LR(UINT16, 196)		m_nReloadBulletCount = 0;
	VAR_LR(UINT16, 197)		m_nMaxBullet = 0;				// �ִ� ���� ź�� ��

	VAR_LR(UINT8, 198)			m_bDual = 0;					// Dual ����ź ���� (K-413)
	VAR_LR(UINT8, 199)			m_bUseThrowDelay = 0;

	VAR_LR(INT32, 200)			m_nIncreaseCount = 0;

	UINT8			m_bUseBombTrigger = 0;		// BombTirgger ��� ���� (C5 �迭)
	INT32			m_nBombtriggerItemID = 0;

protected:
	UINT8 			m_u8EncryptKey = 0;

public:
	void			ReadWeaponInfo( i3RegKey * pKey);
	virtual void	OnEncriptInfo( UINT8 u8Key);

	// �Ӽ��� ����.
	INT32			GetFireBulletCount() const;			// �߻�ü ����
	INT32			GetFireDecBulletCount() const;		// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	INT32			GetLoadBullet() const;				// ���� ���� ź�� ��
	INT32			GetReloadBulletCount() const;			// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	INT32			GetMaxBullet() const;					// �ִ� ���� ź�� ��

	//��ô �����
	REAL32			GetExplosion_Time() const;			// ���� �����ð�( Grenade�� ���)
	REAL32			GetExplosion_Range() const;			// ���� ��ȿ ����
	REAL32			GetExplosion_NearRange() const;			// ���� ��ȿ ����
	REAL32			GetExplosion_EffectiveSlope() const; // ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope
	REAL32			GetDetected_Range() const;				// ���� �Ÿ� (Claymore) 
	REAL32			GetThrowSpeed() const;				// ������ �ӵ�
	REAL32			GetExThrowSpeed();				// ������ �ӵ�
	REAL32			GetEffectiveTime() const;				// ����Ʈ ��ȿ �ð�

	REAL32			GetAttachLength() const;

	REAL32			GetDamageTerm( void) const;

	INT32			GetIncreaseCount();

	// ���� Ŭ�� ���ݽ� ���� Ÿ�̹�. ���ڴ� ���� Ƚ��° �ε��� (��: ù��° ������ �ε��� 0)
	REAL32			GetAttackTimeA( INT32 num) const;

	// ������ Ŭ�� ���ݽ� ���� Ÿ�̹�.
	REAL32			GetAttackTimeB( INT32 num) const;

	bool			GetDual( void) const;
	bool			GetUseThrowDelay( void) const;


	// KEYWORD : DECOY_SOUND
	void			GetDefaultDecoySound(i3::rc_wstring& out);
	
	UINT32			GetBombtriggerItemID() const;
	bool			GetUseBombTrigger( void) const;
};

#endif