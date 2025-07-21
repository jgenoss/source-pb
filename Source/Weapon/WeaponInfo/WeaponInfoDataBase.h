#if !defined( _WEAPON_INFO_DATABASE_H__)
#define _WEAPON_INFO_DATABASE_H__

#include "WeaponGlobal.h"

class CWeaponGlobal;
class CWeaponInfo;

#if ENABLE_UNIT_TEST
namespace inner_hud { void change_test_factor(); }
#endif

struct SUB_SCOPE_INFO
{
	INT32	m_nIdx = -1;

	bool	m_bOffDotLaser = false;
	bool	m_bOnScopeSound = false;
	VEC2D 	m_vec2DotStartPos;
	VEC2D	m_vec2DotWidthHeight;
};

//WeaponInfo들을 담아두고있는 정보 클래스입니다.
//WeaponInfo는 하나의 i3RegKey값과 대응 합니다.
//Index를 이용하여 무기를 얻는 경우는 무기의 인덱스를 이용하고
//name을 이용하여 얻는 경우는 weaponinfo의 무기 이름이 아닌
//i3RegKey에서 얻어온 이름을 사용합니다.(i3NamedElement에 GetName())
class WeaponInfoDataBase : public i3GameNode
{
	I3_CLASS_DEFINE(WeaponInfoDataBase, i3GameNode);

#if ENABLE_UNIT_TEST
	friend void inner_hud::change_test_factor();
#endif

protected:
	CWeaponGlobal* m_pGlobalInfo;

	i3::vector<CWeaponInfo*> 		m_WeaponInfoList[WEAPON_CLASS_COUNT];

	i3::vector<UINT8*>			m_ShiftKeyList;
	i3::vector<CUITemplateInfo_Weapon*> m_UITemplateWeaponInfos;

	i3::vector<i3::rc_wstring>		m_SpecialDamageHudTexturePath;
	i3::vector<i3::rc_wstring>		m_SpecialDieHudTexturePath;
	i3::vector<i3::rc_wstring>		m_ScopeHudTexturePath;

	i3::vector<i3::rc_wstring>		m_vSndByWeaponKillFileName;

	i3::vector<SUB_SCOPE_INFO*>		m_vecSubScopeInfo;

protected:
	UINT8* _CreateShiftKeyTable(INT32 count);

	void	_InitExtWeaponTable();
	INT32	_AddExtWeaponTable(WEAPON_CLASS_TYPE OwnerClass, INT32 OwnerNumber, WEAPON_CLASS_TYPE ExtClass, INT32 ExtNumber, WEAPON::EXT_TYPE ExtType);

	void	_SetCompatibleWeaponID(void);
	void	_RegisterCompatibleWeaponID(CWeaponInfo* pDestInfo, INT32 idx, T_ItemID ItemID);

	CWeaponInfo* CreateWeaponInfo(WEAPON_CLASS_TYPE ClassType);
	void	_ReadUITemplateInfo();
	void	_ReadSubScopeInfo();

public:
	WeaponInfoDataBase();
	virtual ~WeaponInfoDataBase();

	bool	Create();
	void	CreateWeapon(i3RegKey* pRoot);

	//단순히 리스트의 인덱스를 이용하여 얻어오는 값이아닌 DEFINE된 weapon의 값을 찾아서 그 것을 리턴합니다.
	//	classType = 클래스 타입, number = 무기 번호
	CWeaponInfo* getWeaponInfo(WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number, bool check = false);
	void _PrintAvailableWeaponsInClass(WEAPON_CLASS_TYPE classType);
	CWeaponInfo* getWeaponInfo(T_ItemID itemID, bool check = false);

	CWeaponGlobal* getGlobal(void) { return m_pGlobalInfo; }

	bool IsUsedWeapon(const USER_INFO_EQUIPMENT_WEAPON* pEquipWeapon, INT32 nWeaponFlag);
	bool BeDescriptionWeapon(CWeaponInfo* pInfo);
	bool IsExtensionWeapon(CWeaponInfo* pInfo);
	i3::rc_wstring GetWeaponCategory(CWeaponInfo* pInfo);
	i3::rc_wstring GetWeaponClass(CWeaponInfo* pInfo);
	i3::rc_wstring GetWeaponClass(WEAPON_CLASS_TYPE type);

	void GetWeaponCategoryEx(CWeaponInfo* pInfo, i3::rc_wstring& out);

#if !defined( I3_NO_PROFILE)
	virtual void	OnReloadProperty() override;
#endif

	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	size_t GetUITemplateInfoCount() const;
	const CUITemplateInfo_Weapon* GetUITemplateInfo(const size_t i) const;

	INT32 InsertSpecialDamageHUDTexturePath(const i3::rc_wstring texture_path);
	const UINT GetSpecialDamageHUDVolume() const { return m_SpecialDamageHudTexturePath.size(); }
	bool GetSpecialDamageHUDTexturePath(INT32 index, i3::rc_wstring& out);

	INT32 InsertSpecialDieHUDTexturePath(const i3::rc_wstring texture_path);
	const UINT GetSpecialDieHUDVolume() const { return m_SpecialDamageHudTexturePath.size(); }
	bool GetSpecialDieHUDTexturePath(INT32 index, i3::rc_wstring& out);

	INT32 InsertScopeHUDTexturePath(const i3::rc_wstring texture_path);
	const UINT GetScopeHUDTextureVolume() const { return m_SpecialDamageHudTexturePath.size(); }
	bool GetScopeHUDTexturePath(INT32 index, i3::rc_wstring& out);

	const SUB_SCOPE_INFO* GetSubScopeInfo(INT32 index);

	INT32				InsertWeaponKillSound(const i3::rc_wstring soundname);
	INT32				GetWeaponKillSoundCount();
	i3::string			GetWeaponSkillSoundFileName(INT32 index);
};

#endif