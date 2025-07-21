#if !defined( __SCOPE_GUN_INFO_H)
#define __SCOPE_GUN_INFO_H

#include "GunInfo.h"

class CScopeGunInfo :	public CGunInfo
{
	I3_CLASS_DEFINE( CScopeGunInfo, CGunInfo);
private:
	REAL32					m_rScopeFOV;				// 1�� Scope���� FOV��
	REAL32					m_rScopeFOV2;				// 2�� Scope���� FOV��
	
	REAL32					m_fThrowSpeed;

	UINT8					m_nMaxScopeLevel;			// �ִ� ������ �ܰ� (WEAPON::ZOOM_SCOPE Ÿ�Ը� ����)
	UINT8					m_nScopeType;

protected:
	virtual void		_ReadExtensionKey( i3RegKey * pKey, INT32 idx);
	
public:
	CScopeGunInfo();
	virtual ~CScopeGunInfo();

	virtual void			ReadWeaponInfo( i3RegKey * pKey);
	virtual void			OnEncriptInfo( UINT8 u8Key);

	// �Ӽ��� ����
	virtual INT32			GetMaxScopeLevel();			// �ִ� ������ �ܰ� (WEAPON::ZOOM_SCOPE Ÿ�Ը� ����)

	virtual REAL32			GetScopeFOV();			// 1�� Scope���� FOV��
	virtual REAL32			GetScopeFOV2();				// 2�� Scope���� FOV��

	virtual REAL32			GetThrowSpeed();

	virtual INT32			GetScopeType( void);
};

#endif