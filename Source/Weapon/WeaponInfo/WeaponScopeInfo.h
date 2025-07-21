#if !defined( __SCOPE_INFO_H__)
#define __SCOPE_INFO_H__

#include "WeaponExtGunInfo.h"

class CWeaponScopeInfo : public CWeaponExtGunInfo
{
	I3_CLASS_DEFINE( CWeaponScopeInfo, CWeaponExtGunInfo);
private:
	VAR_LR(REAL32, 119)					m_rScopeFOV = 0.0f;				// 1�� Scope���� FOV��
	VAR_LR(REAL32, 120)					m_rScopeFOV2 = 0.0f;				// 2�� Scope���� FOV��
	
	UINT8					m_nMaxScopeLevel = 0;			// �ִ� ������ �ܰ� (WEAPON::ZOOM_SCOPE Ÿ�Ը� ����)
	INT32					m_nScopeType = -1;				// �űԷ� �ٲ����.
	INT32					m_nSubScopeType = -1;			// ������ TEXTURE�� �ʿ��� �ΰ������� �ε��� (INFODATABASE ��)

public:
	virtual void	OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType) override;

	virtual void	OnEncrypt( UINT8 * pCode) override;

	// �Ӽ��� ����
	INT32		GetMaxScopeLevel( UINT8 code) const;			// �ִ� ������ �ܰ� (WEAPON::ZOOM_SCOPE Ÿ�Ը� ����)

	REAL32		GetScopeFOV( UINT8 code) const;			// 1�� Scope���� FOV��
	REAL32		GetScopeFOV2( UINT8 code) const;				// 2�� Scope���� FOV��

	INT32		GetScopeType( UINT8 code) const;
	INT32		GetSubScopeType() const;
};

#endif