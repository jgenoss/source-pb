/**
 * \file	Weapon\WeaponInfo\WeaponInfo.h
 *
 * Declares the weapon information class.
 */


#if !defined( _WEAPON_INFO_H__)
#define _WEAPON_INFO_H__

#include "WeaponGlobal.h"
#include "GameCharaBoneDefine.h"

#define MAX_FIRE_EFFECT		3
#define MAX_WEAPON_EXTENSION	4

struct CompatibleWeaponInfo
{
	T_ItemID	_nItemID;	// ȣȯ ������ ItemID
	bool		_bUseShape;	// ȣȯ ���� Shape ��� Flag

	void Reset(void)
	{
		_nItemID = 0;
		_bUseShape = false;
	}

	void SetCompatibleInfo(T_ItemID ItemID, bool _useCurshape)
	{
		_nItemID = ItemID;
		_bUseShape = _useCurshape;
	}
};

class CGameCharaBase;
class WeaponBase;
class COnlyGrenadeInfo;
class CWeaponExtInfoBase;

struct TracerBulletInfo;
struct KILL_INFO;

extern const char g_chBlank;

class CWeaponInfo :	public i3NamedElement
{
	I3_CLASS_DEFINE( CWeaponInfo, i3NamedElement);

private:
	VAR_LR(WEAPON::DAMAGE_TYPE,203)		m_TypeDamage = WEAPON::DAMAGE_UNKNOWN;	//	���� ���
	VAR_LR(WEAPON::FIRE_TYPE,204)			m_TypeFire = WEAPON::FIRE_UNKNOWN;		//	���� ���
	VAR_LR(WEAPON::TRIGGER_TYPE,205)		m_TypeTrigger = WEAPON::TRIGGER_UNKNOWN;	//	�浹 ���

	VAR_LR(UINT32,206)				m_nRealNumber = 0;	// D ������ ��� ��¥ ���� ��ȣ.
															// �ε��׽þƿ� ��� ���� ���������� �ǸŹ�� ������ D ���Ⱑ �߰��˴ϴ�.
															// * RealNumber�� �����̳� �ǸŹ�ĸ� �ٸ� �������� �ڵ� �ߺ��� �ذ��Ϸ� ������.
															// ���� ������ RealNumber�� ���� ��ü�� ��� ������ �߻��� �� �ֱ� ������ (��Ŷ)
															// Number�� RealNumber �����ؼ� ����մϴ�.

	i3::rc_wstring			m_strWeaponName;		//	���� �̸�
	i3::rc_wstring			m_strLinkedToCharaAI;	//	ĳ������ AI���� ����Ǵ� �̸�
	i3::rc_wstring			m_strFireSound;			// �߻� ����
	i3::rc_wstring			m_strSwapSound;
	i3::rc_wstring			m_strIdleSound;			// Idle Sound

	// 2015-01-20 Ư�� �ǰ� HUD ó�� - ��뿵
	INT32					m_nSpecialDamageHUDIndex = -1;	// Ư�� ������ ó�� ��� �ε��� �Դϴ�. ( �ʱ�ȭ : -1 )
	INT32					m_nSpecialDieHUDIndex = -1;		// Ư�� ���� ó�� ��� �ε��� �Դϴ�. ( �ʱ�ȭ : -1 )
														// WeaponInfoDataBase�� �ؽ��� �н��� �����ϰ� �ش� �ε����� ���⿡ �����մϴ�. 

	// 2008.4.22 GrenadeShell fire sound
	
	i3::rc_wstring			m_strFireSoundExt;		// �ͽ��ټ� ��ɿ����� �߻� ����
	
	bool					m_bExtFireEffectOffset = false;		// �ͽ��ټ� ��ɿ����� �߻� ����Ʈ ������ ��� ����
	VEC3D					m_vPosExtFireEffectOffset;	// �ͽ��ټ� ��ɿ����� �߻� ����Ʈ ������ ��

	i3::rc_wstring			m_strClassMetaName;		// ���� �ν��Ͻ� ������ Ŭ���� ��Ÿ ����.
	
	REAL32					m_nDamageOnlyUI = 0.0f;			//  �κ� ǥ�ÿ�

	VAR_LR(UINT16, 207)	m_nDamage = 0;					//	���� ���ݷ�	(�⺻ ����)
	VAR_LR(UINT16,208)		m_nDamageForDino = 0;			//	���� ���ݷ�	(����  ����� �� ����)

	VAR_LR(UINT16,209)		m_nMaxDamage = 0;
	VAR_LR(UINT16,210)		m_nMaxDamageForDino = 0;

	VAR_LR(INT32,211)		m_nCriticalDamageChance = 0;
	VAR_LR(REAL32,212)		m_fCriticalDamageRate = 0.0f;

	VAR_LR(UINT32,213)		m_WeaponOriginalItemID = 0;

	REAL32					m_rWeight = 0.0f;					//	���� ����(������ ���¹����ΰ�?)
	VAR_LR(REAL32,214)		m_rFireDelay = 0.0f;				// �߻� ����
	VAR_LR(REAL32,215)		m_rFireDelay2 = 0.0f;				// �߻� ���� (���� ������ ��� ������ ����)
	VAR_LR(REAL32,216)		m_rRange = 0.0f;					//  ��ȿ �����Ÿ� : ���⸦ �¾����� Ÿ���� ������ �ִ°Ÿ�
	
	REAL32					m_rValue = 0.0f;					// ������ ����ġ

		
	i3::rc_wstring			m_strUiPath;				// GUI resource path
	i3::rc_wstring			m_strDescription;			// ���� ����
	INT32					m_nUIShapeIndex = 0;			// UIShapeIndex
	
	// Damage
	VAR_LR(REAL32,217)		m_rHelmetPenetrate = 0.0f;			// Helmet penetrate
	VAR_LR(REAL32,218)		m_rHelmetPenetrate_Str = 0.0f;		// ranforce2 Helmet penetrate

	INT32					m_nLevel = 0;
	INT32					m_nMaxLevel = 0;

	i3::rc_wstring			m_strQuestPostfix_Use;	//Quest���� ���Ǵ� ���̾�, ��/���� ���й��ڿ�. ��) mp7����, k-2�� ���� ����϶�.
	i3::rc_wstring			m_strQuestPostfix_Do;	//Quest���� ���ȴ� ���̾�, ��/�� ���� ���ڿ�. ��) c4��, m4a1�� �ٴڿ� ������!!

	REAL32					m_rPhysixForceRate = 1.0f;			// ������ ���� ���� (�迭 ���밪 * Damage * Rate)
		
	WEAPON_INDICATOR_TYPE	m_nIndicatorType = WEAPON_INDICATOR_UNKNOWN;			// ������ ������ Ÿ�� (���� HUD ����ź ǥ��� ���Ŀ� ���)
	REAL32					m_rIndicateDistance = 0.0f;		// ǥ�ñ� �Ÿ�

	VAR_LR(T_ItemID,219)		m_nItemID = 0;

	TRACER_TYPE				m_TracerType = TRACER_TYPE_BULLET;

	// ����ȿ��
	VAR_LR(bool,220)	m_bParalysis = false;
	VAR_LR(REAL32,221)	m_rJumpParalysis = 0.0f;
	VAR_LR(REAL32,222)	m_rWalkParalysis = 0.0f;
	VAR_LR(REAL32,223)	m_rRunParalysis = 0.0f;
	VAR_LR(REAL32,224)	m_rCrouchParalysis = 0.0f;

	// revision 61755 KEYWORD : ROTATE_GRENADE
	INT32					m_RotateAngleTable[4] = { 0, 0, 0, 0 };		// ��X , ��Y , �ٴ�X , �ٴ�Y

	INT32					m_TerrainHitSoundIndex = -1;		// ���� ��Ʈ ���� �ε���
	INT32					m_CharaHitSoundIndex = -1;		// ĳ���� ��Ʈ ���� �ε���
	INT32					m_CharaHitSoundIndex2 = -1;		// ĳ���� ��Ʈ ���� �ε���

	bool					m_bModeWeapon = false;				// Ư�� ���� ����

	QUATERNION				m_Shop3DViewObjQuat = { 0.0f, 0.0f, 0.0f, 1.0f };
	VEC3D					m_Shop3DViewObjPos;
	INT32					m_Shop3DViewSilencer = 0;
	REAL32					m_Shop3DViewDist = 0.0f;

	INT32					m_IsLimitedByDesigSlot = 0;

	INT32					m_KillSoundIndex = -1;

protected:

	//--------------------------------------------------------------------------------------//
	//										��ȣȭ											//
	//--------------------------------------------------------------------------------------//
protected:
	// Shift �� (�ܺο��� �����Ǵ� ��)
	UINT8 					m_u8EncryptKey[VALUETYPE_COUNT] = { 0, };

	// "Damage" field�� load
	virtual void			_ReadDamageKey( i3RegKey * pKey);

	//--------------------------------------------------------------------------------------//
	//										Movement										//
	//--------------------------------------------------------------------------------------//
private:
	REAL32					m_rMoveNormalFire = 0.0f;			// ���� �߻� ���� �̵��ӵ� ���� ���� (Factor)
	REAL32					m_rMoveNormalSit = 0.0f;			// ���� ���¿����� �̵��ӵ� ���� ���� (Factor)
	REAL32					m_rMoveNormalStand = 0.0f;			// �� ���¿����� �̵��ӵ� ���� ���� (Factor)

protected:
	// "Movement" field�� load
	virtual void			_ReadMovementKey( i3RegKey * pKey);

	//--------------------------------------------------------------------------------------//
	//										Extension ����									//
	//--------------------------------------------------------------------------------------//
protected:
	CWeaponExtInfoBase *	m_pExtInfo[MAX_WEAPON_EXTENSION] = { nullptr, };

	i3::rc_wstring				m_strExtName;		// �ͽ��ټ� ��
	i3::rc_wstring				m_strExtFeatures;	// �ͽ��ټ� ���

protected:
	virtual void		_ReadExtensionKey( i3RegKey * pKey, INT32 idx);


	//--------------------------------------------------------------------------------------//
	//										��ȿ ���� ����									//
	//--------------------------------------------------------------------------------------//
protected:
	CompatibleWeaponInfo	m_stCompatibleInfo[MAX_WEAPON_EXTENSION];

public:
	void					SetCompatibleItemID(INT32 idx, T_ItemID nItemID, bool bUseExtShape);
	T_ItemID				GetCompatibleItemID(INT32 idx) const;
	INT32					GetCompatibleWeaponIdx(T_ItemID nItemID) const;
	bool					isUseCompatibleShape(INT32 idx) const
	{
		if (idx < 0 || idx >= MAX_WEAPON_EXTENSION)
		{
			I3_BOUNDCHK(idx, MAX_WEAPON_EXTENSION);
			return false;
		}
		return m_stCompatibleInfo[idx]._bUseShape; 
	}

protected:
	bool				_FindAttributeKey( i3RegKey * pKey, const char * pszFieldName);

public:
	//	�ϳ��� ���⳻�� �������� �ͽ��ټ��� ������ �����Ƿ� ������ m_pWeaponInfo->getExtensionType() ȣ���� �����ϼ���.
	//	������ ���� Ȱ��ȭ�� �ͽ��ټ��� �˱����ؼ� WeaponInfo�� �ִ� �Լ����� ���⿡ �ִ� getCurrentExtensionType() �Լ� �̿��ϼ���. komet
	WEAPON::EXT_TYPE	getExtensionType(INT32 idx) const;
	T_ItemID			GetExtensionItemID( INT32 idx ) const;
	bool				isUseExtShape(INT32 idx) const;

	bool				isExtScopeType( INT32 idx) const
	{
		WEAPON::EXT_TYPE type = getExtensionType( idx);
		return type == WEAPON::EXT_DOT_SIGHT || type == WEAPON::EXT_SCOPE;
	}

	bool				isExtFireType( INT32 idx) const
	{
		WEAPON::EXT_TYPE type = getExtensionType( idx);
		return (type & WEAPON_EXT_MASK_FIRE) == WEAPON_EXT_MASK_FIRE;
	}

	bool				availableExtType( WEAPON::EXT_TYPE type) const {
		for( INT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
			if( getExtensionType(i) == type)
				return true;
		return false;
	}

	void				GetExtensionName( i3::rc_wstring& out) const;		// �ͽ��ټ� ��
	void				GetExtensionFeatures( i3::rc_wstring& out) const; // { return &g_chBlank; }	// �ͽ��ټ� ���

	// �ͽ��ټ� ����� �ִ� ������ Ȱ��ȭ ������ ������ �������� ����
	T_ItemID				GetSubExtensionItemID( WEAPON::EXT_TYPE ExtType = WEAPON::EXT_NONE ) const;

	// addable info
	REAL32				GetExtRange( void) const;
	REAL32				GetExtDamage( void) const;
	REAL32				GetExtAttackTime( void) const;
	REAL32				GetExt_DPD_Rate( void) const;

	// Gun
	REAL32				GetExtRecoilHorzFactor(void) const;
	INT32				GetExtRecoilHorzCount(void) const;
	REAL32				GetExtRecoilVertFactor(void) const;
	INT32				GetExtRecoilVertCount(void) const;

	REAL32				GetExtDeviationFactor(void) const;
	INT32				GetExtAutoFireCount(void) const;
	REAL32				GetExtFireDelay(void) const;
	INT32				GetExtOneHorzEnable(void) const;
	INT32				GetExtOneVertEnable(void) const;
	INT32				GetExtAutoPointShotEnable(void) const;

	// Zoom
	REAL32				GetZoomFOV(void) const;		// 1��Ī�϶� Zoom���� FOV��

	// Scope
	INT32				GetScopeType( void) const;
	INT32				GetScopeSubType(void) const;
	REAL32				GetScopeFOV(void) const;
	REAL32				GetScopeFOV2(void) const;
	INT32				GetMaxScopeLevel(void) const;			// �ִ� ������ �ܰ� (WEAPON::ZOOM_SCOPE Ÿ�Ը� ����)

	// Silencer
	REAL32				GetExtSilencerDmgRate() const;	// Extension : Damage Rate


	// Dual Magazine
	REAL32				GetReloadTimeDual() const;		// ��� źâ ���� �ð�

	INT32				GetTerrainHitSoundIndex( void) const			{ return m_TerrainHitSoundIndex; }
	INT32				GetCharaHitSoundIndex( void) const				{ return m_CharaHitSoundIndex; }
	INT32				GetCharaHitSoundIndex2( void) const				{ return m_CharaHitSoundIndex2; }

	bool				isNoExjectShell( void) const					{ return _isAttribute(WEAPON_ATTRIBUTE_NO_EXJECTSHELL); }
	bool				isNoOffExtAtChange( void) const					{ return _isAttribute(WEAPON_ATTRIBUTE_NOTOFF_EXTENSION_AT_CHANGE); }
	
	bool				GetLoadMagToLoadBullet() const					{ return _isAttribute(WEAPON_ATTRIBUTE_LOADMAG_TO_LOADBULLET);	}

	// �Ӽ��� ����
public:
	WEAPON::RES_ID			GetNumber() const;	//	���� ��ȣ
	WEAPON::RES_ID			GetRealNumber() const;//	Real ���� ��ȣ
	T_ItemID				GetItemID() const;

	T_ItemID				GetLeftHandWeaponItemID() const;	// �޼� ����

	//	���� �̸�
	void					GetWeaponName( i3::rc_wstring& strOut) const;


	//---------------------------------------------------------------------------------//
	// 2015. 01. 13 EFFECT ó�� - ������
protected:
	i3::rc_wstring	m_strFireEffect[ MAX_FIRE_EFFECT];			// �߻� ����Ʈ
	i3::rc_wstring	m_strFireEffectExt;				// �ͽ��ټ� ��ɿ����� �߻� ����Ʈ
	i3::rc_wstring	m_strObjHitEffect;			// ����/������Ʈ ��Ʈ ����Ʈ
	i3::rc_wstring	m_strCharaHitEffect;		// ĳ���� �ǰ� ����Ʈ
	i3::rc_wstring	m_strCharaHitEffect1PV;	// ĳ���� 1��Ī �ǰ� ����Ʈ
	UINT16			m_UnDecalEffFlag = 0;
	UINT16			m_UnBlowEffFlag = 0;
	i3::rc_wstring	m_strDecalEffectExt;
	UINT16			m_UnColorEffectFlag = 0;		// Color Fire Muzzle Flash ��뿩��. ������� �ʴ� ��츦 üũ��(ColorFireEffect)
	bool			m_bUseHazeEffect = false;

	i3::rc_wstring	m_strIdleEffect;			// Idle ����Ʈ
	REAL32			m_rIdleEffectLoopTime = 0.0f;		// Idle ����Ʈ�� �����Ǵ� �ð�

	void		_ReadEffectKey( i3RegKey * pRootKey, i3RegKey * pEffRegKey);

public:
	// �߻� ����Ʈ
	void		GetFireEffect( i3::rc_wstring& strOut, INT32 idx = 0) const;
	// ��Ʈ ����Ʈ
	void		GetObjHitEffect( i3::rc_wstring& strOut ) const;
	void		GetCharaHitEffect( i3::rc_wstring& strOut ) const;
	void		GetCharaHitEffect1PV( i3::rc_wstring& strOut ) const;
	UINT16		GetUnDecalEffFlag() const;
	UINT16		GetUnBlowEffFlag() const;
	void		GetDecalEffectExt( i3::rc_wstring& strOut ) const;
	UINT16		GetUnColorEffectFlag() const;
	//���̵� ����Ʈ
	void		GetIdleEffect(i3::rc_wstring& strOut) const;
	REAL32		GetIdleEffectLoopTime() const;

	bool		IsHazeEffect() { return m_bUseHazeEffect; }
	//--------------------------------------------------------------------------------//

	// �߻� ����
	void					GetFireSound( i3::rc_wstring& strOut) const;
	// ��ü ����
	void					GetSwapSound( i3::rc_wstring& strOut) const;
	// Idle ����
	void					GetIdleSound( i3::rc_wstring& strOut) const;

	void					GetLinkedToCharaAI( i3::rc_wstring& strOut) const;
	
	// 2008.4.22 GrenadeShell fire sound
	// �ͽ��ټ� ��ɿ����� �߻� ����Ʈ
	void					GetFireEffectExt( i3::rc_wstring& strOut) const;
	// �ͽ��ټ� ��ɿ����� �߻� ����
	void					GetFireSoundExt( i3::rc_wstring& strOut) const;

	void					GetClassMetaName( i3::rc_wstring& strOut) const;
	
	bool					GetEnableExtFireEffectOffSet() const		{ return m_bExtFireEffectOffset; }
	void					GetPosExtFireEffectOffSet( VEC3D* vecOut );

	WEAPON_SLOT_TYPE		GetTypeUsage() const;				//	���� �з�
	WEAPON_CLASS_TYPE		GetTypeClass() const;				//	���� Ŭ����
	WEAPON::FIRE_TYPE		GetTypeFire() const;			//	���� ���
	WEAPON::TRIGGER_TYPE	GetTypeTrigger() const;		//	�浹 ���	
	WEAPON::DAMAGE_TYPE		GetTypeDamage() const;		//	���� ���
		
	
	// Kill Sound
	INT32					GetKillSoundIndex() { return m_KillSoundIndex; }

	//	���� ����(������ ���¹����ΰ�?)
	REAL32					GetWeight() const;
	//  ��ȿ �����Ÿ� : ���⸦ �¾����� Ÿ���� ������ �ִ°Ÿ�
	REAL32					GetRange() const;

	// �߻� ����
	REAL32					GetFireDelay() const;
	// �߻� ���� (���� ����� ������ ����)
	REAL32					GetFireDelay2() const;

	// ���� �߻� ���� �̵��ӵ� ���� ���� (Factor)
	REAL32					GetMoveNormalFire() const;
	// ���� ���¿����� �̵��ӵ� ���� ���� (Factor)
	REAL32					GetMoveNormalSit() const;
	// �� ���¿����� �̵��ӵ� ���� ���� (Factor)
	REAL32					GetMoveNormalStand() const;

	virtual REAL32					GetCrossHairExpandTime() const		{ return 0.f;}
	virtual REAL32					GetCrossHairContractTime() const	{ return 0.f;}
	// CrossHair�� Ŀ���ų� �۾����� �ӵ��� ���� (���� �ӵ��� ��)
	virtual REAL32					GetCrossHairSpeedFactor() const		{ return 0.f;}

	// GUI resource path (MAX_PATH)
	void					GetUiPath( i3::rc_wstring& strOut) const;
	
	// UI shape
	INT32					GetUIShapeIndex() const;

	// ������ ����ġ
	REAL32					GetValue() const;
	// ���� ���� (2048bytes)
	void					GetDescription( i3::rc_wstring& strOut) const;

	// �󼼼����̹��� ���翩��
	bool					isDescriptionImage() const;

	// Helmet penetrate
	REAL32					GetHelmetPenetrate() const;
	// ranforce2 Helmet penetrate
	REAL32					GetHelmetPenetrate_Str() const;
	// Helmet penetrate
	virtual REAL32			GetBulletWeight() const		{ return 100.f; }

	void					GetQuestPostfix_Use( i3::rc_wstring& strOut) const;
	void					GetQuestPostfix_Do( i3::rc_wstring& strOut) const;

	bool					UsePairData(void) const 			{ return _isAttribute(WEAPON_ATTRIBUTE_USINGPAIRWEAPON); }
	
	bool					AttachedSubWeapon();

	WEAPON_INDICATOR_TYPE	GetIndicatorType( void) const;
	REAL32					GetIndicateDistance( void) const;

	REAL32					GetRotateAngle( INT32 nIndex ) const;

	bool					IsThrowWeapon( void) const			{ return _isAttribute(WEAPON_ATTRIBUTE_THROW_WEAPON); }
	bool					IsExceptionEndurance( void) const	{ return _isAttribute(WEAPON_ATTRIBUTE_EXCEPTION_ENDURANCE); }
	bool					IsWorldWeapon( void) const			{ return _isAttribute(WEAPON_ATTRIBUTE_WORLD_WEAPON); }

	virtual WEAPON::ZOOM_TYPE	GetTypeZoom() const				{ return WEAPON::ZOOM_UNKNOWN; }		//	�� ���
	
	virtual bool			isMultipleExtension(void) const		{ return false;	}	//	�ΰ��� Extension ��� ������ �ִ� ����
	
	virtual INT32			GetFireBulletCount() const			{ return 0; }	// �߻�ü ����
	virtual INT32			GetFireDecBulletCount() const		{ return 1;	}	// �߻����� ��, �Ҹ��ϴ� �Ѿ� ����
	virtual INT32			GetAutoFireCount() const			{ return 0; }	// �߻� ���� (0:����, 1:�ܹ�, 2, 3, 4...:����)	���� ����
	virtual INT32			GetAutoFireCount2() const			{ return 0; }	// �߻� ���� (0:����, 1:�ܹ�, 2, 3, 4...:����)  ������ ����
	virtual REAL32			GetAutoFireDelay() const			{ return 0; }	// ���� ���� �߻� ����
	virtual INT32			GetAutoPointShotEnable() const		{ return 0; }	// �ű� ���� �۵� ���.

	virtual REAL32			GetLoadBulletTime() const			{ return 0;	}	// ���� �����̸� ���� �ð�
	virtual REAL32			GetReloadTime() const				{ return 0;	}	// �������� �ɸ��� �ð�

	virtual INT32			GetLoadBullet() const				{ return 0; }	// ���� ���� ź�� ��
	virtual INT32			GetLoadBulletType() const			{ return 0; }	// ���� ���� ź�� ��
	virtual INT32			GetGaugeReload() const				{ return 0; }	// Gauge�� ���ε� �ϴ� Ÿ��(WaterGun)
	virtual INT32			GetReloadBulletCount() const		{ return 0; }	// 1�� ������ ä������ �Ѿ� ���� (������ Load Bullet ����)
	virtual INT32			GetMaxBullet() const				{ return 0; }	// �ִ� ���� ź�� ��
	virtual INT32			GetNeedToLoadBullet() const			{ return 0;	}	// Load Magazine ��, Load Bullet�� �ؾ��ϴ°�?
	
	//	���� ���ݷ�	
	virtual INT32			GetDamage() const;
	virtual REAL32			GetDamageOnlyUI() const;
	virtual INT32			GetDamageForDino() const;

	INT32					GetMaxDamage(void) const;
	INT32					GetMaxDamageForDino(void) const;

	INT32					GetCriticalDamageChance() const;
	REAL32					GetCriticalDamageRate() const;

	
	UINT32					GetOriginalWeaponItemID(void) const;

	//��ô �����
	virtual REAL32			GetExplosion_Time() const				{ return 0; }			// ���� �����ð�( Grenade�� ���)
	virtual REAL32			GetExplosion_Range() const				{ return 0; }			// ���� ��ȿ ���� : �Ϲ� ����
	virtual REAL32			GetExplosion_NearRange() const			{ return 0; }			// ���� ��ȿ ���� : �ֺ� ���� (ex. Claymore)
	virtual REAL32			GetExplosion_EffectiveSlope() const		{ return 0; }			// ���� �Ÿ��� ���� Damage�� �����ϴ� ���� Slope
	virtual REAL32			GetDetected_Range() const				{ return 0; }			// ���� ���� (Claymore)
	virtual REAL32			GetThrowSpeed() const					{ return 0;	}			// ������ �ӵ�
	virtual REAL32			GetExThrowSpeed()			{ return 0;	}			// ���� �ͽ��ټ� ������ �ӵ�
	virtual REAL32			GetEffectiveTime() const				{ return 0; }			// ����Ʈ ��ȿ �ð�
	virtual REAL32			GetAttachLength() const					{ return 0; }
	virtual bool			GetUseBombTrigger( void) const			{ return false;}
	virtual bool			GetDual( void) const					{ return false; }
	virtual bool			GetUseThrowDelay( void) const			{ return false; }		// �����⿡ ���� ������ ����
	virtual UINT32			GetBombtriggerItemID() const			{ return 0; }

	virtual INT32			GetIncreaseCount() { return 0; }

	//Move( %)
	
	virtual REAL32			GetMoveNormalZoom() const				{ return 0;	}		// ���� ���¿����� �̵��ӵ� ���� ���� (Factor)

	//
	virtual REAL32			GetCrossHairExpand() const				{ return 4.f; }// Crosshiar�� �о��� ��, ���̰� ������� ����
	virtual REAL32			GetCrossHairMin() const		 			{ return 4.f; }	// CorssHair�� �ּҷ� ���� ���� ũ�� (Clipping-Space������ ũ��)
	virtual REAL32			GetCrossHairMax() const		 			{ return 4.f; }
	virtual REAL32			GetCrossHairSizeDamage() const			{ return 0; }	// Damage�� �Ծ��� ��, CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeJump() const	 		{ return 0; }	// Jump ���� �ÿ� CrossHair�� �о����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeMove() const	 		{ return 0; }	// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeMoveSit() const			{ return 0; }
	virtual REAL32			GetCrossHairSizeWalk() const			{ return 0; }	// �̵� ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeWalkSit() const			{ return 0; }
	virtual REAL32			GetCrossHairSizeStand() const		 	{ return 0;	}	// Idle ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)
	virtual REAL32			GetCrossHairSizeSit() const				{ return 0;	}	// Sit ���� �ÿ� CrossHair�� �Ͼ����� ���� (Factor)	
	virtual REAL32			GetCrossHairSizeZoom() const			{ return 0; }	// ���� �ÿ� CrossHair�� �о����� ���� (Factor) ?
	virtual REAL32			GetCrossHairSizeNoZoom(void) const		{ return 0; }
	
	virtual INT32			GetCrossHairInvisible() const			{ return 0; }		// 0 : All Visible 2:Scope Invisible
	

	virtual REAL32			GetRecoilVertAngle() const				{ return 0; }			// 1�ߴ� ���� �ݵ���
	virtual REAL32			GetRecoilVertMax() const				{ return 0; }			// �ִ� ���� �ݵ���
	virtual INT32			GetRecoilVertCount() const				{ return 0; }			// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����
	virtual INT32			GetOneVertEnable()	const				{ return 0; }			// ���� �ݵ� ��� ��� 0: �� 1: ��

	virtual REAL32			GetRecoilHorzAngle() const				{ return 0; }			// �¿� ��鸲 ��
	virtual REAL32			GetRecoilHorzMax() const				{ return 0; }			// �ִ� ���� �¿� ��鸲 ��
	virtual INT32			GetRecoilHorzCount() const				{ return 0; }			// ���� �ݵ��� ����Ǳ� �����ϴ� ���� �Ѿ� ����
	virtual INT32			GetOneHorzEnable()	const				{ return 0; }			// ���� �ݵ� ��� ��� 0: �� 1: ��

	virtual REAL32			GetDeviation() const					{ return 0; }			// ������
	virtual REAL32			GetDeviationMax() const					{ return 0; }			// �ִ� ���� ������
	virtual REAL32			GetDeviationOnlyUI() const				{ return 0; }
	virtual REAL32			GetDeviation_DiminutionDistance(void) const	{ return 0.0f; }
	virtual REAL32			GetDeviation_Decrement(void) const			{ return 0.0f; }
	
	virtual REAL32			GetCamSyncRate() const					{ return 0; }				// 0:�ѱ�ݵ��� ������   1:��������

	virtual INT32			GetFireDelayOnlyUI() const				{ return 0; }
	virtual	INT32			GetRecoilVertOnlyUI() const				{ return 0; }
	virtual	INT32			GetRecoilHorzOnlyUI() const				{ return 0; }
	virtual REAL32			GetRecoilVertAngle_Recovertime() const	{ return 1.0f; }
	virtual REAL32			GetRecoilHorzAngle_Recovertime() const	{ return 1.0f; }

	virtual REAL32			GetDPD_Rate() const						{ return 0; }

	virtual I3COLOR*		GetBulletSmokeColor() const				{ return nullptr;	}	//	�Ѿ� ���� ���� ����	
	virtual REAL32			GetBulletSmokeRepeatTexSize() const		{ return 1.f; }	//	�Ѿ� ���� ���� �̹����� Ÿ�ϸ��Ǵ� ���� (����: m)
	virtual REAL32			GetBulletSmokeGrowThickness() const		{ return 0;	}	//	�Ѿ� ���� ���� ���İ� ������� ���� ���Ʒ��� ������ �β� (����: m)
	virtual REAL32			GetBulletSmokeLifeTime() const			{ return 0; }		//	�Ѿ� ���� ���� ���� �ð� (����: ��) 0.0�̸� ��Ȱ��ȭ�ȴ�.
	virtual REAL32			GetBulletSmokeThickness() const			{ return 0;	}//	�Ѿ� ���� ���� �β� (����: m)
	virtual REAL32			GetBulletSmokeStartSpeed() const			{ return 0; }	//	�Ѿ� ���� ���� ������ �̵� �ӵ� (�ѱ�) (����: �ʴ� m)
	virtual REAL32			GetBulletSmokeEndSpeed() const			{ return 0; }		//	�Ѿ� ���� ���� ���� �̵� �ӵ� (Ÿ����) (����: �ʴ� m)

	bool					GetPumpAction() const					{ return (_getAttribute() & WEAPON_ATTRIBUTE_PUMPACTION) == WEAPON_ATTRIBUTE_PUMPACTION;	}		//	�����׼� ��� ����	

	virtual INT32			GetLevel() const					{ return m_nLevel; }
	virtual INT32			GetMaxLevel() const					{ return m_nMaxLevel; }

	// KEYWORD : WEAPON_DUALKNIFE
	virtual REAL32			GetAttackTimeA( INT32 num) const		{ return 0;}
	virtual REAL32			GetAttackTimeB( INT32 num) const		{ return 0;}
	virtual REAL32			GetDamageA( INT32 num) const			{ return 0;}
	virtual REAL32			GetDamageB( INT32 num) const			{ return 0;}

	// KEYWORD : DECOY_SOUND
	virtual void			GetDefaultDecoySound(i3::rc_wstring& out) {};

	// KEYWORD : PHYSIX_CHARA_FORCE
	REAL32			GetPhysixForceRate() const;

	TRACER_TYPE		GetTracerType( void) const						{ return m_TracerType; }

	// KEYWORD : WEAPON_DINO
	virtual REAL32			GetAttackFreezeTime(void) const			{ return 0.0f; }
	virtual REAL32			GetSkillDuration(void) const			{ return 0.0f; }
	virtual REAL32			GetSkillCoolTime(void) const			{ return 0.0f; }
	virtual REAL32			GetSkillSkillMovementRate(void) const	{ return 0.0f; }

	// revision 53433 ����ð�
	bool					IsParalysis( void) const				{ return m_bParalysis; }
	REAL32					GetJumpParalysis() const;
	REAL32					GetWalkParalysis() const;
	REAL32					GetRunParalysis() const;
	REAL32					GetCrouchParalysis() const;

	QUATERNION				GetShop3DViewObjQuat()		{ return m_Shop3DViewObjQuat; }
	VEC3D					GetShop3DViewObjPos()		{ return m_Shop3DViewObjPos; }
	INT32					IsShop3DViewSilencer()		{ return m_Shop3DViewSilencer; }
	REAL32					GetShop3DViewDist()			{ return m_Shop3DViewDist; }
	bool					IsShop3DViewDualWeapon()	{ return _isAttribute( WEAPON_ATTRIBUTE_DUAL_WEAPON); }
	
	// Ư�� �ǰ� HUD
	INT32					GetWeaponSpecialHitHudIndex( void ) const;
	INT32					GetWeaponSpecialDieHudIndex( void ) const;

	virtual WEAPON::ATTACK_TYPE	GetAttackType(WEAPON::FIRE_ORDER fireOrder, INT32 i32FireCount) const;

	//			Gatcha2
private:
	REAL32					m_rScaleRate = 0.0f;
	REAL32					m_rXAxis = 0.0f;
	REAL32					m_rYAxis = 0.0f;
	REAL32					m_rZAxis = 0.0f;
	REAL32					m_rBrightness = 1.0f;				//	�̱����2 �𵨸� ��� ����

	void		__ReadGatchaKey( i3RegKey * pKey);

public:
	REAL32					GetWeaponBrightness(void) const;
	REAL32					GetWeaponScale(void) const;
	REAL32					GetWeaponXRotate(void) const;
	REAL32					GetWeaponYRotate(void) const;
	REAL32					GetWeaponZRotate(void) const;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//											���� üũ..													//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	WEAPON_ATTRIBUTE		m_Attribute = 0;			// ���� �Ӽ�
	WEAPON_ATTRIBUTE		_getAttribute( void) const;
	bool					_isAttribute( WEAPON_ATTRIBUTE attr) const	{ return (_getAttribute() & attr) == attr; }

public:

	/** \brief Extension ���� ���� */
	virtual bool			isExtensible(void) const			{ return false; }
	virtual bool			isNeedToActivate(void) const		{ return false; }
	virtual bool			isDirectUse(void) const				{ return false; }
	
	bool					isBombTypeClass() const;
	bool					isUseExplosionRange() const;
	bool					hasAction() const;
	bool					isRange() const;
	bool					isMelee() const;
	bool					isThrowing() const;
	bool					isItem() const;
	bool					isThrowingClass() const;
	bool					isMission() const;
	bool					isNotMission() const;
	bool					isEnableHeadShot() const;
	bool					isEnableCrossHairEffect() const;
	bool					isEnableDrop() const;
	bool					isNotNeedContactBox() const;
	bool					isNotPenetrate() const;
	bool					isExplosionPhysics() const;
	bool					isValidUiPath() const;
	bool					isUnconditionalNeutralColorCrossHair() const;		// ������ �ڿ� Į�� �����Ǵ� ����
	bool					isUnableSwapIfZero() const;
	bool					isGun() const;
	bool					isCompoundBow() const;
	bool					isC4() const;
	bool					isOverFireDelay(REAL32 rDelay) const;
	bool					hasExtensionInfo() const;
	bool					hasFireEffect() const;
	bool					hasFireEffectExt() const;
	bool					hasIdleEffect() const;
	bool					isModeWeapon( void) const				{ return _isAttribute( WEAPON_ATTRIBUTE_MODE_WEAPON); }

	/** \brief Fast Zoom ���� ����*/
	bool					isEnableFastZoom(void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_FASTZOOM); }
	/** \brief Shotgun Maser C �ִϸ��̼��� �ϴ� �ѱ� */
	bool					isShotgunMasterC( void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_SHOTGUNMASTERC); }
	/** \brief CIC ���� �Ǻ� */
	bool					isCIC( void) const						{ return _isAttribute( WEAPON_ATTRIBUTE_CIC); }
	/** \brief DotLaser�� �׽� On */
	bool					isLockedDotLaser( void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_LOCKEDDOTLASER); }
	virtual	bool			isCameraZoomType() const				{ return false; }

	// �ִϸ��̼� ���� ���� �ΰ���, WEAPON::ZOOM_STEADY_AIM���� ���� idle�� ���� �ִϸ��̼��� ����.
	virtual	bool			isAnimZoomType() const					{ return false; }

	bool					IsReloadLoopAnimation(void) const		{ return _isAttribute( WEAPON_ATTRIBUTE_RELOAD_LOOP_ANIMATION); }
	bool					IsFastReloadAnimation( void) const		{ return _isAttribute( WEAPON_ATTRIBUTE_FASTARELOAD); }
	bool					IsAlphablendUsed( void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_ALPHABLEND); }
	bool					IsLoadBulletLeftBarrel( void) const		{ return _isAttribute( WEAPON_ATTRIBUTE_LOADBULLETLEFTBARREL); }
	bool					IsLoadMagazineLeftBarrel( void) const	{ return _isAttribute( WEAPON_ATTRIBUTE_LOADMAGLEFTBARREL); }
	bool					IsLoadMagazineReady(void) const			{ return _isAttribute( WEAPON_ATTRIBUTE_LOADMAGREADY); }

	bool					IsDelayedThrowingType() const			{ return _isAttribute( WEAPON_ATTRIBUTE_DELAYED_THROWING); }

	bool					isSame(WEAPON_CLASS_TYPE type, WEAPON::RES_ID nNumber) const;

	bool					IsLimitedByDesigSlot() const;

public:
	CWeaponInfo(void);
	virtual ~CWeaponInfo(void);

	virtual void			ReadWeaponInfo( i3RegKey * pKey);
	void					ReadShop3DViewKey( i3RegKey * pKey );
	/** \brief ��ȣȭ �մϴ�.
		\param[in] pShiftCode Bit Shift count�Դϴ�. 0 ~ 7*/
	virtual void			OnEncriptInfo( UINT8 u8Key);
	virtual void			FillBulletCountString(i3::stack_wstring& out_wstrBulletCount );
	virtual INT32			GetPercentage( WEAPON::INFO_TYPE nWeaponInfoType) const;
	void					MakeBulletCountString(i3::stack_wstring& out_wstrBulletCount); 
	void					MakeWeaponRangeString(i3::stack_wstring& out_wstrRange);

	void					SetKillInfo(KILL_INFO* pKillInfo);

	// �ٸ� Ŭ���� �� ����..
	virtual void			GetDamageInfo(CHARA_DAMAGEINFO * pDamageInfo);
	UINT8					GetWeaponFireState(T_ItemID itemID) const;

	virtual void			SetRecoilVert(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase* pWeapon, REAL32 fRate) { return; }
	virtual void			SetRecoilHorz(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase* pWeapon, REAL32 rTheta, REAL32 fRate) { return; }
	//virtual void			SetDropWeaponInfo(N_PCINFO_DROPWEAPON* pIn) { return; }
	virtual void			SetBulletSmokeInfo(TracerBulletInfo* pBulletInfo) { return; }
	
	// ���..
	virtual REAL32			CalcDeviationByAction(CGameCharaBase* pOwner, WeaponBase * pWeapon, REAL32 fAddDeviation = 0.f) { return 0; }

	static void				InitializeAttributeKey( void);

private:

#if defined( I3_DEBUG )
	void					CheckRegDataType();	// ������Ʈ�� ������ Ÿ�� �˻�
#endif
};


#endif
