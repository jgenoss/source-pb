#if !defined( __GUN_INFO_H)
#define __GUN_INFO_H

#include "WeaponInfo.h"
#include "GameCharaBoneDefine.h"

class CGameCharaBase;

struct TracerBulletInfo;
struct KILL_INFO;

class CGunInfo : public CWeaponInfo
{
	I3_CLASS_DEFINE( CGunInfo, CWeaponInfo);

private:
	VAR_LR(WEAPON::ZOOM_TYPE,130)			m_TypeZoom = WEAPON::ZOOM_UNKNOWN;					//	�� ���
	VAR_LR(REAL32, 131)					m_fThrowSpeed = 0.0f;
	VAR_LR(REAL32, 132)					m_rAutoFireDelay = 0.0f;			// ���� ���� �߻� ����

	VAR_LR(REAL32, 133)					m_rLoadBulletTime = 0.0f;			// ���� �����̸� ���� �ð�
	VAR_LR(REAL32, 134)					m_rReloadTime = 0.0f;				// �������� �ɸ��� �ð�

	VAR_LR(REAL32, 135)					m_RecoilVertAngle = 0.0f;			// 1�ߴ� ���� �ݵ���
	VAR_LR(REAL32, 136)					m_RecoilVertMax = 0.0f;			// �ִ� ���� �ݵ���

	VAR_LR(REAL32, 137)					m_RecoilHorzAngle = 0.0f;			// �¿� ��鸲 ��
	VAR_LR(REAL32, 138)					m_RecoilHorzMax = 0.0f;			// �ִ� ���� �¿� ��鸲 ��

	VAR_LR(REAL32, 139)					m_Deviation = 0.0f;				// ������
	VAR_LR(REAL32, 140)					m_DeviationMax = 0.0f;				// �ִ� ���� ������
	REAL32									m_DeviationOnlyUI = 0.0f;			// �κ��丮�� ������ ǥ��Ǵ� Hit Rate ������ �⺻ 0 �̻��� �����Ǿ� �ִ� ��츸 ����Ѵ�. 

	VAR_LR(REAL32, 142)					m_CamSyncRate = 0.0f;				// 0:�ѱ�ݵ��� ������   1:��������
	VAR_LR(REAL32, 143)					m_DPD_Rate = 0.0f;					// �Ÿ��� ��������
	VAR_LR(REAL32, 144)					m_rBulletWeight = 100.0f;			// Helmet penetrate

	VAR_LR(UINT16, 145)					m_RecoilHorzCount = 0;			// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����
	VAR_LR(UINT16, 146)					m_RecoilVertCount = 0;			// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����

	VAR_LR(UINT32, 147)					m_nLoadBullet = 0;				// ���� ���� ź�� ��
	VAR_LR(UINT32, 148)					m_nLoadBulletType = 0;			// ���� ���� ź�� ��
	VAR_LR(UINT32, 149)					m_nGaugeReload = 0;				// Gauge�� �̿��� Reload Ÿ��
	VAR_LR(UINT16, 150)					m_nReloadBulletCount = 0;		// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	VAR_LR(UINT32, 151)					m_nMaxBullet = 0;				// �ִ� ���� ź�� ��

	VAR_LR(UINT8, 152)						m_nFireBulletCount = 0;			// �߻�ü ����
	VAR_LR(UINT8, 153)						m_nFireDecBulletCount = 1;		// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	VAR_LR(UINT8, 154)						m_nAutoFireCount = 0;			// �߻� ���� (0:����, 1:�ܹ�, 2, 3, 4...:����)

	VAR_LR(INT32, 155)						m_OneHorzEnable = 0;			// ���� �ݵ� ���� ��� ��ȯ ��: false �� : true (��Ŭ�� ������/����)
	VAR_LR(INT32, 156)						m_OneVertEnable = 0;			// ���� �ݵ� ���� ��� ��ȯ ��: false �� : true (��Ŭ�� ������/����)

	VAR_LR(REAL32, 157)					m_RecoilVertAngle_Recovertime = 1.0f;	//����ݵ� �����ӵ�
	VAR_LR(REAL32, 158)					m_RecoilHorzAngle_Recovertime = 1.0f;	//����ݵ� �����ӵ�

	INT32					m_FireDelayOnlyUI = 0;			// �κ��丮�� ������ ǥ��Ǵ� ����� ������ �⺻ 0 �̻��� �����Ǿ� �ִ� ��츸 ����Ѵ�. 
	INT32					m_RecoilVertOnlyUI = 0;
	INT32					m_RecoilHorzOnlyUI = 0;

	INT32					m_nAutoPointShotEnable = 0;		// �ű� ���� ��� �۵� ( ���� ���� )
	// �Ӽ��� ����
public:
	//	�� ���
	virtual WEAPON::ZOOM_TYPE	GetTypeZoom() const override;
	
	virtual INT32			GetFireBulletCount() const override;		// �߻�ü ����
	virtual INT32			GetFireDecBulletCount() const override;	// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	virtual INT32			GetAutoFireCount() const override;		// �߻� ���� (0:����, 1:�ܹ�, 2, 3, 4...:����)	
	virtual REAL32			GetAutoFireDelay() const override;		// ���� ���� �߻� ����
	virtual INT32			GetAutoPointShotEnable() const override; // �ű� ���� ��� �۵�

	virtual REAL32			GetLoadBulletTime() const override;		// ���� �����̸� ���� �ð�
	virtual REAL32			GetReloadTime() const override;			// �������� �ɸ��� �ð�
	
	virtual INT32			GetLoadBullet() const override;			// ���� ���� ź�� ��
	virtual INT32			GetLoadBulletType() const override;		// ������ Ÿ��
	virtual INT32			GetGaugeReload() const override;			// Gauge�� ���ε� �ϴ� Ÿ��(WaterGun)
	virtual INT32			GetReloadBulletCount() const override;	// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	
	virtual INT32			GetMaxBullet() const override;			// �ִ� ���� ź�� ��
	virtual INT32			GetNeedToLoadBullet() const override;	// Load Magazine ��, Load Bullet�� �ؾ��ϴ°�?
	virtual REAL32			GetThrowSpeed() const override;
	
	virtual REAL32			GetRecoilVertAngle() const override;		// 1�ߴ� ���� �ݵ���
	virtual REAL32			GetRecoilVertMax() const override;		// �ִ� ���� �ݵ���
	virtual INT32			GetRecoilVertCount() const override;		// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����

	virtual REAL32			GetRecoilVertAngle_Recovertime() const override;
	virtual REAL32			GetRecoilHorzAngle_Recovertime() const override;
	
	virtual REAL32			GetRecoilHorzAngle() const override;		// �¿� ��鸲 ��
	virtual REAL32			GetRecoilHorzMax() const override;		// �ִ� ���� �¿� ��鸲 ��
	virtual INT32			GetRecoilHorzCount() const override;		// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����
	
	virtual REAL32			GetDeviation() const override;			// ������
	virtual REAL32			GetDeviationMax() const override;		// �ִ� ���� ������
	virtual REAL32			GetDeviationOnlyUI() const override;		// �κ��丮�� ������ ǥ��Ǵ� Hit Rate ������ �⺻ 0 �̻��� �����Ǿ� �ִ� ��츸 ����Ѵ�.
	virtual INT32			GetFireDelayOnlyUI() const override;		// �κ��丮�� ������ ǥ��Ǵ� FireDelay ������ �⺻ 0 �̻��� �����Ǿ� �ִ� ��츸 ����Ѵ�.
	virtual	INT32			GetRecoilVertOnlyUI() const override;
	virtual	INT32			GetRecoilHorzOnlyUI() const override;
		
	virtual REAL32			GetCamSyncRate() const override;			// 0:�ѱ�ݵ��� ������   1:��������
	
	virtual INT32			GetOneHorzEnable() const override;		// ���� �ݵ� ��� ��� 0: �� 1: ��
	virtual INT32			GetOneVertEnable() const override;		// ���� �ݵ� ��� ��� 0: �� 1: ��

	// ���� üũ..
	virtual bool			isNeedToActivate(void) const override
	{
		return ( ( getExtensionType(0) & WEAPON_EXT_MASK_NEED_TO_ACTIVATE) != 0 );
	}

	virtual bool			isDirectUse(void) const override
	{
		return ( ( getExtensionType(0) & WEAPON_EXT_MASK_DIRECT_USE) != 0 );
	}

	virtual bool			isExtensible(void) const override { return ( getExtensionType(0) != WEAPON::EXT_NONE); }
	// ī�޶� ���� ���� ������
	virtual	bool			isCameraZoomType() const override
	{
		WEAPON::ZOOM_TYPE type = GetTypeZoom();
		return (type == WEAPON::ZOOM_ZOOM || type == WEAPON::ZOOM_SCOPE || type == WEAPON::ZOOM_STEADY_AIM);
	}
	// �ִϸ��̼� ���� ���� �ΰ���, WEAPON::ZOOM_STEADY_AIM���� ���� idle�� ���� �ִϸ��̼��� ����.
	virtual	bool			isAnimZoomType() const override
	{
		WEAPON::ZOOM_TYPE type = GetTypeZoom();
		return (type == WEAPON::ZOOM_ZOOM || type == WEAPON::ZOOM_SCOPE);
	}

	//--------------------------------------------------------------------------------------//
	//										Crosshair ����									//
	//--------------------------------------------------------------------------------------//
private:
	VAR_LR(REAL32, 159)			m_rCrossHairExpand = 0.0f;			// Crosshiar�� �о��� ��, ���̰� ������� ����
	VAR_LR(REAL32, 160)			m_rCrossHairMin = 0.0f;			// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	VAR_LR(REAL32, 161)			m_rCrossHairMax = 0.0f;

	VAR_LR(REAL32, 162)			m_rCrossHairSizeDamage = 0.0f;		// Damage�� �Ծ��� ��, CrossHair�� �Ͼ����� ���� (Factor)
	VAR_LR(REAL32, 163)			m_rCrossHairSizeJump = 0.0f;		// Jump ���� �ÿ� CrossHair�� �о����� ���� (Factor)
	VAR_LR(REAL32, 164)			m_rCrossHairSizeMove = 0.0f;		// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	VAR_LR(REAL32, 165)			m_rCrossHairSizeMoveSit = 0.0f;
	VAR_LR(REAL32, 166)			m_rCrossHairSizeWalk = 0.0f;		// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	VAR_LR(REAL32, 167)			m_rCrossHairSizeWalkSit = 0.0f;

	VAR_LR(REAL32, 168)			m_rCrossHairSizeStand = 0.0f;		// Idle ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	VAR_LR(REAL32, 169)			m_rCrossHairSizeSit = 0.0f;		// Sit ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)	
	VAR_LR(REAL32, 170)			m_rCrossHairSizeZoom = 0.0f;		// ���� �ÿ� CrossHair�� �о����� ���� (Factor) ?
	VAR_LR(REAL32, 171)			m_rCrossHairSizeNoZoom = 0.0f;		// Sniper �ѿ� ����.
	VAR_LR(INT32,	172)			m_CrossHairInvisible = 0;		// 0 : All Visible 2:Scope Invisible

	VAR_LR(REAL32, 173)			m_CrossHairExpandTime = 0.3f;
	VAR_LR(REAL32, 174)			m_CrossHairContractTime = 0.4f;
	VAR_LR(REAL32, 175)			m_CrossHairSpeedFactor = 1.0f;		// CrossHair�� Ŀ���ų� �۾����� �ӵ��� ���� (���� �ӵ��� ��)

protected:
	void				_ReadCrosshairKey( i3RegKey * pKey);
	void				_ReadAccuracyKey( i3RegKey * pKey);

public:
	virtual REAL32		GetCrossHairExpand() const override;				// Crosshiar�� �о��� ��, ���̰� ������� ����
	virtual REAL32		GetCrossHairMin() const override;				// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	virtual REAL32		GetCrossHairMax() const override;
	virtual REAL32		GetCrossHairSizeDamage() const override;			// Damage�� �Ծ��� ��, CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32		GetCrossHairSizeJump() const override;			// Jump ���� �ÿ� CrossHair�� �о����� ���� (Factor)
	virtual REAL32		GetCrossHairSizeMove() const override;			// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32		GetCrossHairSizeMoveSit() const override;
	virtual REAL32		GetCrossHairSizeWalk() const override;			// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32		GetCrossHairSizeWalkSit() const override;
	virtual REAL32		GetCrossHairSizeStand() const override;			// Idle ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32		GetCrossHairSizeSit() const override;
	virtual REAL32		GetCrossHairSizeZoom() const override;			// ���� �ÿ� CrossHair�� �о����� ���� (Factor) ?
	virtual REAL32		GetCrossHairSizeNoZoom(void) const override;
	virtual INT32		GetCrossHairInvisible() const override;			// 0 : All Visible 2:Scope Invisible

	virtual REAL32		GetCrossHairExpandTime( void) const override;
	virtual REAL32		GetCrossHairContractTime( void) const override;
	virtual REAL32		GetCrossHairSpeedFactor( void) const override;

	//--------------------------------------------------------------------------------------//
	//										Movement										//
	//--------------------------------------------------------------------------------------//
private:
	VAR_LR(REAL32, 176)			m_rMoveNormalZoom = 0.0f;			// ���� ���¿����� �̵��ӵ� ���� ���� (Factor)

protected:
	virtual void		_ReadMovementKey( i3RegKey * pKey) override;

public:
	virtual REAL32		GetMoveNormalZoom() const override;		// ���� ���¿����� �̵��ӵ� ���� ���� (Factor)

	//--------------------------------------------------------------------------------------//
	//										���� ����										//
	//--------------------------------------------------------------------------------------//
private:
	I3COLOR				m_cBulletSmokeColor = { 255, 255, 255, 255 };		//	�Ѿ� ���� ���� ����	
	REAL32				m_fBulletSmokeRepeatTexSize = 1.0f;	//	�Ѿ� ���� ���� �̹����� Ÿ�ϸ��Ǵ� ���� (����: m)
	REAL32				m_fBulletSmokeGrowThickness;		//	�Ѿ� ���� ���� ���İ� ������� ���� ���Ʒ��� ������ �β� (����: m)
	REAL32				m_fBulletSmokeLifeTime;		//	�Ѿ� ���� ���� ���� �ð� (����: ��) 0.0�̸� ��Ȱ��ȭ�ȴ�.
	REAL32				m_fBulletSmokeThickness;	//	�Ѿ� ���� ���� �β� (����: m)
	REAL32				m_fBulletSmokeStartSpeed;	//	�Ѿ� ���� ���� ������ �̵� �ӵ� (�ѱ�) (����: �ʴ� m)
	REAL32				m_fBulletSmokeEndSpeed;		//	�Ѿ� ���� ���� ���� �̵� �ӵ� (Ÿ����) (����: �ʴ� m)

protected:
	void				_ReadBulletTracerKey( i3RegKey * pKey);

public:
	//	�Ѿ� ���� ���� ����	
	virtual void			GetBulletSmokeColor( I3COLOR * pOut) const;

	//	�Ѿ� ���� ���� �̹����� Ÿ�ϸ��Ǵ� ���� (����: m)
	virtual REAL32			GetBulletSmokeRepeatTexSize() const override;
	virtual REAL32			GetBulletSmokeGrowThickness() const override;	//	�Ѿ� ���� ���� ���İ� ������� ���� ���Ʒ��� ������ �β� (����: m)
	virtual REAL32			GetBulletSmokeLifeTime() const override;			//	�Ѿ� ���� ���� ���� �ð� (����: ��) 0.0�̸� ��Ȱ��ȭ�ȴ�.
	virtual REAL32			GetBulletSmokeThickness() const override;		//	�Ѿ� ���� ���� �β� (����: m)
	virtual REAL32			GetBulletSmokeStartSpeed() const override;		//	�Ѿ� ���� ���� ������ �̵� �ӵ� (�ѱ�) (����: �ʴ� m)
	virtual REAL32			GetBulletSmokeEndSpeed() const override;			//	�Ѿ� ���� ���� ���� �̵� �ӵ� (Ÿ����) (����: �ʴ� m)

	//					���� ������
protected:
	virtual void			_ReadDamageKey( i3RegKey * pKey) override;

public:
	virtual REAL32			GetDPD_Rate() const override;

	// Helmet penetrate
	virtual REAL32			GetBulletWeight() const override;

public:
	virtual void			FillBulletCountString(i3::stack_wstring& out_wstrBulletCount) override;
	virtual void			ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void			OnEncriptInfo( UINT8 u8Key) override;

	virtual INT32			GetPercentage( WEAPON::INFO_TYPE nWeaponInfoType) const override;

	virtual void			SetRecoilVert(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase* pWeapon, REAL32 fRate) override;
	virtual void			SetRecoilHorz(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase* pWeapon, REAL32 rTheta, REAL32 fRate) override;
	// �ٸ� Ŭ���� �� ����..
	//virtual void			SetDropWeaponInfo(N_PCINFO_DROPWEAPON* pIn);
	virtual void			SetBulletSmokeInfo(TracerBulletInfo* pBulletInfo) override;
	// ���..
	virtual REAL32			CalcDeviationByAction(CGameCharaBase* pOwner, WeaponBase * pWeapon, REAL32 fAddDeviation = 0.f) override;
	
	const char*				GetStringExtensionType(WEAPON::EXT_TYPE type) const;
};

#endif