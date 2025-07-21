#if !defined( __WEAPON_EXT_INFO_BASE_H__)
#define __WEAPON_EXT_INFO_BASE_H__

#include "../../../../Common/CommonSource/WeaponDefine.h"

class CWeaponExtInfoBase : public i3ElementBase
{
	I3_CLASS_DEFINE( CWeaponExtInfoBase, i3ElementBase);

private:
	WEAPON::EXT_TYPE	m_ExtType;		// Extension Type. (1 ~ n)
	T_ItemID			m_ItemID;		// �ͽ��ټ� ����� �ִ� ������ Ȱ��ȭ ������ ������ �������� ����

	bool				m_bUseExtShape;	// ExtShape ��� Flag (false : �������� ShapeIdx ���, true : Ȯ�幫�� ShapeIdx ���)

public:
	CWeaponExtInfoBase();
	virtual ~CWeaponExtInfoBase();

	virtual void	OnReadInfo( i3RegKey * pKey, WEAPON::EXT_TYPE extType);
	virtual void	OnEncrypt( UINT8 * pCode);

	WEAPON::EXT_TYPE	GetExtType( UINT8 code) const;
	T_ItemID			GetItemID( UINT8 code) const;

	bool				IsUseExtShape(void) const		{ return m_bUseExtShape; }
};

#endif
