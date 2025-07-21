#include "pch.h"
#include "GameCharaWeaponControl3PV.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl3PV);

bool CGameCharaWeaponControl3PV::OnChangeViewWeapon( void)
{
	for( UINT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		WEAPON_SLOT_TYPE slot = static_cast<WEAPON_SLOT_TYPE>(i);
		if( getWeapon( slot) != nullptr )
		{
			getWeapon( slot)->OnChangeObserverView();
			if( getWeapon( slot) != getCurrentWeapon() && slot < WEAPON_SLOT_MELEE)
			{	// ���� ��� �ִ� ����� �ٸ� ��� �ش� ������ġ�� ���δ�.
				OnAttachPurchasePrimWeapon( slot);
				OnAttachPurchaseSideWeapon( slot);
			}
		}
	}

	return true;
}

/*virtual*/ bool CGameCharaWeaponControl3PV::OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound /*= true*/)
{
	if( CGameCharaWeaponControl::OnSelectWeapon( nSlot, bPlaySound))
	{
		if( g_pFramework->IsBattleStage())
		{	// InGame������ � ���Դϴ�.
			// ������ ����� ���⸦ �ش� ��ġ�� ���δ�.
			OnAttachPurchasePrimWeapon( (WEAPON_SLOT_TYPE) getOldSlotNumber());
			OnAttachPurchaseSideWeapon( (WEAPON_SLOT_TYPE) getOldSlotNumber());
		}
		return true;
	}

	return false;
}


bool CGameCharaWeaponControl3PV::OnZoom( void)
{
	if( getCurrentWeapon() == nullptr )
		return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr );

	WEAPON::ZOOM_TYPE type = pWeaponInfo->GetTypeZoom();

	//	���� ����� �ִ� ����
	switch( type)
	{
		case  WEAPON::ZOOM_ZOOM:
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				m_pOwner->Cmd_ApplyCrosshair();
				return true;
			}
			break;

		case WEAPON::ZOOM_SCOPE :
			//	������ ����� �ִ� ����
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				INT32 nScopeLevel = getBoneContext()->getScopeWaveData()->m_nCurScopeLevel + 1;
				getBoneContext()->getScopeWaveData()->m_nCurScopeLevel = nScopeLevel;
				m_pOwner->Cmd_ApplyCrosshair();
				
				return true;
			}
			break;

		case WEAPON::ZOOM_STEADY_AIM :
			// �ٸ� � Animation �� Crosshair�� ������� FOV���� ����
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				m_pOwner->Cmd_ApplyCrosshair();
				return true;
			}
			break;
	}

	m_pOwner->Cmd_ApplyCrosshair();
	return false;
}


bool CGameCharaWeaponControl3PV::OnProcessRespawn( void)
{
	if( !CGameCharaWeaponControl::OnProcessRespawn())
		return false;
	
	
	OnAttachPurchasePrimWeapon( WEAPON_SLOT_PRIMARY);
	OnAttachPurchaseSideWeapon( WEAPON_SLOT_SECONDARY);

	if( !m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY))
	{
		// �ֹ��� ���ѽ� ��������� ����
		m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_SECONDARY);
	}

	return true;
}



