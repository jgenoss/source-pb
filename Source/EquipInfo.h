#ifndef		__EQUIPINFO_H__
#define		__EQUIPINFO_H__

//#include "i3Base/itl/vector_map.h"

#define	MAX_SHARE_RES	10		// �ִ� 10���� Res�� ���ؼ� Sharing

namespace EQUIP {
	enum eABILITY {
		eABILITY_DAMAGE = 0,
		eABILITY_DAMAGERATE,
		eABILITY_CEBONUS,
		eABILITY_DEFENSE_ARM,
		eABILITY_DEFENSE_BODY,
		eABILITY_DEFENSE_LEG,
		eABILITY_DEFENSE_HAND,
		eABILITY_DEFENSE_FOOT,
		eABILITY_DETECTC5,				// C5 ����(�Ÿ���)
		eABILITY_FALLDAMAGE,
		eABILITY_PROTECTION_RATE,
		eABILITY_INVINCIBLE_TIME,
		eABILITY_MOVESPEED_FRONT,
		eABILITY_MOVESPEED_BACK,
		eABILITY_MOVESPEED_SIDE,
		eABILITY_MOVESPEED_DIAGONAL,
		eABILITY_RESPAWNTIME,
		eABILITY_STBONUS,
		eABILITY_MOVESOUND_VOL,
		eABILITY_ACCURACY,
		eABILITY_MAXBULLET,
		eABILITY_PENETRATE,
		eABILITY_RANGE,
		eABILITY_RELOADTIME,
		eABILITY_SWAPTIME,
		eABILITY_FIREDELAY,
		eABILITY_THROWSPEED,
		eABILITY_THROWCOUNT,
		eABILITY_HPINCREASE,			// HP ����
		eABILITY_C4SPEEDKIT,			// C4 ��ġ/��ü �ӵ� ����
		eABILITY_JUMPHEIGHT,
		eABILITY_CROSSHAIR_SPEEDFACTOR,
		eABILITY_RECOILHORZCOUNT,

		eABILITY_COUNT
	};
};

class CEquipTransform : public i3ElementBase
{
	I3_CLASS_DEFINE( CEquipTransform, i3ElementBase);
private:
	struct TRANSFORMINFO {
		VEC3D		m_Pos;
		REAL32		m_Scale = 0.0f;
	};

	i3::vector_map<i3::string, TRANSFORMINFO*>	m_listByChara;

protected:
	TRANSFORMINFO *	_FindInfo( const char * pszName);

public:
	virtual ~CEquipTransform();

	void	ReadInfo( i3RegKey * pKey);

	bool	GetPositionScale( const char * pszName, VEC3D & outVec, REAL32 & outScale);
};

////////////////////////////////////////////////////////////////////////////////////
//	��� ���̽� ����
////////////////////////////////////////////////////////////////////////////////////
class CEquipInfo : public i3NamedElement
{
	I3_CLASS_DEFINE( CEquipInfo, i3NamedElement );

private:
	INT32		m_nType = -1;				//	Ÿ��

	i3::rc_wstring	m_strName;				// �̸�	MAX_STRING_COUNT
	i3::rc_wstring	m_strResPath;			// ���ҽ� ��� (I3CHR)
	i3::rc_wstring	m_strDescription;		// ����
	i3::rc_wstring	m_strDiffuseTexture;	// Diffuse �ؽ�ó
	i3::rc_wstring	m_strNormalTexture;		// ��� �ؽ�ó
	i3::rc_wstring	m_strSpecularTexture;	// ����ŧ�� �ؽ�ó
	i3::rc_wstring	m_strResLocalPath;		// ���ҽ� I3CHR ���� ���
	i3::rc_wstring	m_strResPostName;		// ���ҽ� I3CHR ���� �ڿ� �ٴ� ���� (common, blue)
	i3::rc_wstring	m_strResName_Red;		// ���ҽ� I3CHR ���� �ڿ� �ٴ� ���� (Red)

	INT32		m_UIShapeIndex = -1;
	
	VAR_LR(REAL32,281)		m_Ability[EQUIP::eABILITY_COUNT] = { 0.0f };
	VAR_LR(INT32,282)		m_IsLimitedByDesigSlot = 0;		// ȣĪ�������� ���, ����UserTitle ��ǰ�϶� ȣĪ ���� ���� ������ ���� üũ..
											// 0���� ū ���, �������Կ� ������ ������񿡼� ���ܳ���.
	T_ItemID		m_ITEMID = 0;
	T_ItemID		m_ShareResItemID[ MAX_SHARE_RES];		// [initialize at constructor]

	INT32		m_i32CombinationType = 0;

	CEquipTransform *	m_pTransformInfo = nullptr;
	CEquipTransform *	m_pTransformInfo_Red = nullptr;	// Red����

	bool		m_bIsDefaultEquipment = false;


protected:										
	UINT8		m_u8EncryptKey = 0;

public:
	CEquipInfo();
	virtual ~CEquipInfo();

	virtual void	ReadInfo( i3RegKey * pKey);
	virtual void	ReadCommonInfo( i3RegKey * pKey);
	virtual void	ReadVersionInfo( i3RegKey * pKey, bool isV2Info = false);

	virtual void	OnEncryptInfo( UINT8 u8Key);

	void	getName( i3::rc_wstring& out) const;
	void	getResPath(  i3::rc_wstring& out) const;
	void	getDescription(  i3::rc_wstring& out) const;
	void	getDiffuseTexture(  i3::rc_wstring& out) const;
	void	getNormalTexture(  i3::rc_wstring& out) const;
	void	getSpecularTexture(  i3::rc_wstring& out) const;
	void	GetResLocalPath( i3::rc_wstring& out) const;

	INT32	GetUIShapeIndex() const;

	virtual void	GetResPostName( i3::rc_wstring& out, TEAM_TYPE team = TEAM_NONE, INT32 res_subtype = EQUIP::eBERET_NONE,  bool bFemale = false) const;

	T_ItemID		GetITEMID() const;
	EQUIP::RES_ID	GetResID( void)	const;

	bool		IsSharedUniq( void) const;
	T_ItemID	GetShareResItemID( UINT32 idx) const;

	INT32			GetCombinationType( void) const;

	// Ability
	REAL32	GetAbilityValue( EQUIP::eABILITY ability) const;

	// Transform
	CEquipTransform * GetTransform( TEAM_TYPE team) const;

	bool	IsDefaultEquipment( void) const		{ return m_bIsDefaultEquipment; }
	bool	IsLimitedByDesigSlot() const;

};



////////////////////////////////////////////////////////////////////////////////////
//	��� ����
////////////////////////////////////////////////////////////////////////////////////
class CHeadInfo : public CEquipInfo
{
	I3_CLASS_DEFINE( CHeadInfo , CEquipInfo);
protected:
	REAL32		m_rProtectionTime = 1.0f;
	REAL32		m_rDefenseRateForDino = 1.0f;		// ����� 1.f default

	AVATAR::HEAD_TYPE	m_AvatarHeadType = AVATAR::HEAD_TYPE_NORMAL;	// common, blue
	AVATAR::HEAD_TYPE	m_AvatarHeadType_Red = AVATAR::HEAD_TYPE_NORMAL;
	bool		m_bFacegearShow = true;			// common, blue
	bool		m_bFacegearShow_Red = true;

public:
	virtual void	ReadInfo( i3RegKey * pKey) override;
	virtual void	OnEncryptInfo( UINT8 u8Key) override;

	REAL32	getDefenseRate( void) const;
	REAL32	getProtectionTime( void) const;
	AVATAR::HEAD_TYPE	getAvatarHeadType( TEAM_TYPE team) const;
	bool	getFacegearShow( TEAM_TYPE team) const;
};

////////////////////////////////////////////////////////////////////////////////////
//	������ ����
////////////////////////////////////////////////////////////////////////////////////
class	CBeretInfo : public CHeadInfo
{
	I3_CLASS_DEFINE(CBeretInfo, CHeadInfo);

private:
	i3::rc_wstring	m_strMixResPostName[EQUIP::eBERET_COUNT];		// ���+������ ���� ���ҽ� I3CHR ���� ���� (common, blue)
	i3::rc_wstring	m_strMixResName_Red[EQUIP::eBERET_COUNT];		// ���+������ ���� ���ҽ� I3CHR ���� ���� (Red)
	
public:
	virtual void	ReadVersionInfo( i3RegKey * pKey, bool isV2Info = false) override;
	virtual void	OnEncryptInfo( UINT8 u8Key) override;

	virtual void	GetResPostName( i3::rc_wstring& out, TEAM_TYPE team = TEAM_NONE, INT32 res_subtype = EQUIP::eBERET_NONE,  bool bFemale = false) const override;
};


////////////////////////////////////////////////////////////////////////////////////
//	����ũ ����
////////////////////////////////////////////////////////////////////////////////////
class	CFacegearInfo : public CHeadInfo
{
	I3_CLASS_DEFINE(CFacegearInfo, CHeadInfo);
public:
};

////////////////////////////////////////////////////////////////////////////////////
//	��Ʈ ����
////////////////////////////////////////////////////////////////////////////////////
class	CBeltInfo	: public CEquipInfo
{
	I3_CLASS_DEFINE(CBeltInfo, CEquipInfo);
public:
};

////////////////////////////////////////////////////////////////////////////////////
//	�۷��� ����
////////////////////////////////////////////////////////////////////////////////////
class	CGloveInfo	: public CEquipInfo
{
	I3_CLASS_DEFINE(CGloveInfo, CEquipInfo);
public:
};

////////////////////////////////////////////////////////////////////////////////////
//	Ȧ���� ����
////////////////////////////////////////////////////////////////////////////////////
class	CHolsterInfo	: public CEquipInfo
{
	I3_CLASS_DEFINE(CHolsterInfo, CEquipInfo);
public:
};

////////////////////////////////////////////////////////////////////////////////////
//	���� ����
////////////////////////////////////////////////////////////////////////////////////
class	CJacketInfo	: public CEquipInfo
{
	I3_CLASS_DEFINE(CJacketInfo, CEquipInfo);
protected:
	bool		m_bHeadgearShow = true;			// ���� �Ⱥ��̰� �ϴ� ���.
	AVATAR::HEAD_TYPE	m_AvatarHeadType = AVATAR::HEAD_TYPE_NORMAL;	// ������� headtype���� �켱 ������ ����.

	bool		m_bHeadgearShow_Red = true;			// ���� �Ⱥ��̰� �ϴ� ���.
	AVATAR::HEAD_TYPE	m_AvatarHeadType_Red = AVATAR::HEAD_TYPE_NORMAL;	// ������� headtype���� �켱 ������ ����.

public:
	virtual void	ReadInfo( i3RegKey * pKey) override;

	bool			getHeadgearShow( TEAM_TYPE team) const;
	AVATAR::HEAD_TYPE	getAvatarHeadType( TEAM_TYPE team) const;
};

////////////////////////////////////////////////////////////////////////////////////
//	���� ����
////////////////////////////////////////////////////////////////////////////////////
class	CPoketInfo	: public CEquipInfo
{
	I3_CLASS_DEFINE(CPoketInfo, CEquipInfo);
public:
};

////////////////////////////////////////////////////////////////////////////////////
//	�ǻ� ��Ʈ
////////////////////////////////////////////////////////////////////////////////////
class CSkinInfo : public CEquipInfo
{
	I3_CLASS_DEFINE( CSkinInfo, CEquipInfo);

private:
	AVATAR::HEAD_TYPE	m_AvatarHeadType = AVATAR::HEAD_TYPE_NONE;	// ���� ���� �켱������ HeadType

	T_ItemID			m_HeadgearItemID = 0;	// ���� ������ ĳ���Ϳ� ���õ� ����� ��ü�Ѵ�. ������ ���õ� ����� �Ⱥ��̰� �Ѵ�.
	T_ItemID			m_FacegearItemID = 0;
	T_ItemID			m_BeltItemID = 0;
	T_ItemID			m_HolsterItemID = 0;

	bool				m_bCommonStyle = false;				// ��,�� ���� ��Ų�̸� true

	i3::rc_wstring			m_strResName_Man_Red;
	i3::rc_wstring			m_strResName_Man_Blue;
	i3::rc_wstring			m_strResName_Woman_Red;
	i3::rc_wstring			m_strResName_Woman_Blue;

public:
	virtual void	ReadInfo( i3RegKey * pKey) override;
	virtual void	ReadCommonInfo(i3RegKey * pKey) override;
	virtual void	OnEncryptInfo( UINT8 u8Key) override;

	AVATAR::HEAD_TYPE	GetHeadType( void) const;
	T_ItemID			GetHeadgearItemID( void) const;
	T_ItemID			GetFacegearItemID( void) const;
	T_ItemID			GetBeltItemID( void) const;
	T_ItemID			GetHolsterItemID( void) const;

	virtual void		GetResPostName( i3::rc_wstring& out, TEAM_TYPE team = TEAM_NONE, INT32 res_subtype = EQUIP::eBERET_NONE,  bool bFemale = false) const override;
};


class CUITemplateInfo_Parts : public i3NamedElement
{
	I3_CLASS_DEFINE(CUITemplateInfo_Parts, i3NamedElement);

public:	
	void ReadInfo(i3RegKey* key);

	INT32 GetType() const;
	INT32 GetLoopLoad() const;
	const i3::rc_wstring& GetName() const;

private:
	INT32 m_type = -1;
	INT32 m_LoopLoad = 0;
	i3::rc_wstring m_name;
};

#endif	// __EQUIPINFO_H__
