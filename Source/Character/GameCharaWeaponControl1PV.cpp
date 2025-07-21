#include "pch.h"
#include "GameCharaWeaponControl1PV.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl1PV);

bool CGameCharaWeaponControl1PV::OnCreateWeapon( WEAPON_SLOT_TYPE nSlot, T_ItemID Itemid, UINT8 nMultiWeaponSync)
{
	bool bRv = CGameCharaWeaponControl::OnCreateWeapon( nSlot, Itemid, nMultiWeaponSync);

	// 1��Ī���� �ٿ�� �ڽ� ����
	WeaponBase* pWeapon = nullptr;

	pWeapon = getWeapon( nSlot);
	if (pWeapon == nullptr) return false;

	pWeapon->OnChangeFPSView();

	return bRv;
}

WeaponBase * CGameCharaWeaponControl1PV::OnAttachWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode)
{
	WeaponBase * pWeapon = CGameCharaWeaponControl::OnAttachWeapon( nSlot, pSceneNode, pGameNode);

	// 1��Ī�� ����� Shadow�� ������ �ʵ��� �����Ǿ� �ִ�.
	if( pWeapon != nullptr)
		pWeapon->getSceneObject()->GetBody()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);

	if (pWeapon)
	{
		CGameCharaBase* pOwner = pWeapon->getOwner();
		if (pOwner)
			pWeapon->EnableRenderAABB(pOwner->IsEnabledRenderAABB());
		else
			pWeapon->EnableRenderAABB(true);
	}

	return pWeapon;
}

WeaponBase * CGameCharaWeaponControl1PV::OnDetachWeapon( WEAPON_SLOT_TYPE nSlot)
{
	WeaponBase * pWeapon = CGameCharaWeaponControl::OnDetachWeapon( nSlot);

	if( pWeapon != nullptr )
	{
		// 1��Ī�� ����� Shadow�� ������ �ʵ��� �����Ǿ� �ִ�.
		pWeapon->getSceneObject()->GetBody()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	}

	if (pWeapon)
	{
		pWeapon->EnableRenderAABB(false);
	}
	
	return pWeapon;
}

bool CGameCharaWeaponControl1PV::OnChangeViewWeapon( void)
{
	CGameCharaWeaponControl::OnChangeViewWeapon();
	// Add code here

	for( UINT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		WeaponBase * pWeapon = getWeapon( static_cast<WEAPON_SLOT_TYPE>(i) );
		if( pWeapon != nullptr )
		{
			pWeapon->OnChangeFPSView();
			if( pWeapon != getCurrentWeapon() )
			{	// ageia ���� ��� �ִ� ����� �ٸ��ٸ� ��� ���� ������ �ű�ϴ�.
				_NoUseWeapon( pWeapon);
			}
		}
	}

	return true;
}

/*virtual*/ bool CGameCharaWeaponControl1PV::OnAttachPurchasePrimWeapon( WEAPON_SLOT_TYPE nSlot)
{
	if( CGameCharaWeaponControl::OnAttachPurchasePrimWeapon( nSlot))
	{
		WeaponBase * pWeapon = getWeapon( nSlot);
		if( pWeapon != nullptr)
			pWeapon->SetEnable( false);
		return true;
	}

	return false;
}

/*virtual*/ bool CGameCharaWeaponControl1PV::OnAttachPurchaseSideWeapon( WEAPON_SLOT_TYPE nSlot)
{	
	if( CGameCharaWeaponControl::OnAttachPurchaseSideWeapon( nSlot))
	{
		WeaponBase * pWeapon = getWeapon( nSlot);
		if( pWeapon != nullptr)
			pWeapon->SetEnable( false);
		return true;
	}

	return false;
}

/*virtual*/ bool CGameCharaWeaponControl1PV::OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound /*= true*/)
{
	if( CGameCharaWeaponControl::OnSelectWeapon( nSlot, bPlaySound))
	{
		// Animation�� World-Space Transform ���̿� LOD ����� �޶��� �� ������,
		// �װ��� Visual �� ���� ������ �� �� �ֱ� ������
		// �������� LOD�� ����Ų��.
		getCurrentWeapon()->setCurrentLOD( 0);
		return true;
	}

	return false;
}

bool CGameCharaWeaponControl1PV::OnZoom( void)
{
	if( getCurrentWeapon() == nullptr )
		return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr );

	m_pOwner->Cmd_ApplyCrosshair();

	WEAPON::ZOOM_TYPE type = pWeaponInfo->GetTypeZoom();

	//	���� ����� �ִ� ����
	switch( type)
	{
		case  WEAPON::ZOOM_ZOOM:
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				//	���� ī�޶� ����
				getCameraContext()->SetZoom();
				return true;
			}
			break;

		case WEAPON::ZOOM_SCOPE :
			//	������ ����� �ִ� ����
			{
				INT32 nLvl = getBoneContext()->getScopeWaveData()->m_nCurScopeLevel + 1;

				//	�������� ī�޶� ����	
				if( getCameraContext()->SetScopeLevel( nLvl))
				{
					return true;
				}
			}
			break;

		case WEAPON::ZOOM_STEADY_AIM :
			// �ٸ� � Animation �� Crosshair�� ������� FOV���� ����
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				//	���� ī�޶� ����
				getCameraContext()->SetZoom();
				return true;
			}
			break;
		default :
			return false;
	}
	
	return false;
}

bool CGameCharaWeaponControl1PV::OnExtActivate( void)
{
	m_pOwner->Cmd_ApplyCrosshair();

	return CGameCharaWeaponControl::OnExtActivate();
}

bool CGameCharaWeaponControl1PV::OnExtDeactivate( void)
{
	m_pOwner->Cmd_ApplyCrosshair();

	return CGameCharaWeaponControl::OnExtDeactivate();
}

bool CGameCharaWeaponControl1PV::OnRemoveShell( void)
{
	if( getCurrentWeapon() == nullptr )							return false;
	if( getCurrentWeapon()->GetCurrentExtensionType() != WEAPON::EXT_GLAUNCHER)
		return false;

	WeaponGrenadeLauncher * pGLauncher = (WeaponGrenadeLauncher*) getCurrentWeapon();

	if( !m_pOwner->isNetwork())
	{
		if( !pGLauncher->isLoadBulletEnabled())
			return false;
	}

	pGLauncher->SetEnable( true);
	pGLauncher->RemoveShell();
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
		m_pOwner->Cmd_Idle_Camera();

	return true;
}


bool CGameCharaWeaponControl1PV::OnProcessRespawn( void)
{	
	if( !CGameCharaWeaponControl::OnProcessRespawn())
		return false;
	
	if( !m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY))
	{
		// �ֹ��� ���ѽ� ��������� ����
		m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_SECONDARY);
	}

	return true;
}


