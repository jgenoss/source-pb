#include "pch.h"
#include "GameCharaWeaponControl3PVLocal.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl3PVLocal, CGameCharaWeaponControl3PV);

CGameCharaWeaponControl3PVLocal::CGameCharaWeaponControl3PVLocal()
{
}

CGameCharaWeaponControl3PVLocal::~CGameCharaWeaponControl3PVLocal()
{
}

//void CGameCharaWeaponControl3PVLocal::OnUpdate( REAL32 rDeltatime)
//{
//	CGameCharaWeaponControl3PV::OnUpdate( rDeltatime);
//
//	//	���� ���涧 �������� ������ ���� ��������...���� �����ӿ� �ٲ�(�ٲ��� �����) ���⸦ ���� �����Ѵ�.
//	if( getSwapSlotNumber() != getCurrentSlotNumber())
//	{
//		if( getCurrentWeapon() != NULL)
//		{
//			// ���� ����� ������� �ʴ� ���·� �մϴ�.
//			getWeaponContext()->NoUseWeapon( getCurrentWeapon());
//		}
//		
//		//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.		
//		OnSelectWeapon( getSwapSlotNumber());
//
//		if( (getCurrentWeapon() != NULL) && g_pFramework->IsBattleStage())
//		{
//			getCurrentWeapon()->WeaponChange();		//	���� ��ü�� ���� ����� Change ���·�
//
//			// HUD
//			{
//				CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
//				CHud * pHud = pStage->getHud();
//				if( pHud != NULL )	pHud->setOldLoadedBullet( 0 );
//			}
//		}
//		
//		// ������ ����� ���⸦ �ش� ��ġ�� ���δ�.
//		OnAttachPurchasePrimWeapon( getOldSlotNumber());
//		OnAttachPurchaseSideWeapon( getOldSlotNumber());
//	}
//
//	if( getCurrentWeapon() != NULL)
//	{
//		//	�޼� ������ ���� �Ŀ��� ���Ⱑ ���ʿ� �پ� �ִ°��� ���´�.
//		if( getWeaponContext()->getAttachHand() == CHARA_WEAPON_BARREL_LEFT_HAND)
//		{
//			// ������ �׸� ����
//			//	�޼� ������ ���� �Ŀ��� ���Ⱑ ���ʿ� �پ� �ִ°��� ���´�.
//			if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) && 
//				!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET))
//			{	
//				//	������ �׸��̸� �ٽ� �����տ� ���̵��� �ǵ�����.
//				m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND);
//				getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_GRIP);
//			}
//		}
//		else if( getWeaponContext()->getAttachHand() == CHARA_WEAPON_BARREL_RIGHT_HAND)
//		{
//			// �޼� �׸� ����
//			// ������ ������ ���Ⱑ �����ʿ� �پ� �ִ� ���� ���´�.
//			if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) &&
//				!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET))
//			{
//				//
//				m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_GRIP_LEFT_HAND);
//				getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_GRIP);
//			}
//		}
//	}
//}

//bool CGameCharaWeaponControl3PVLocal::OnSwapWeapon( INT32 nSlot)
//{
//	if( !CGameCharaWeaponControl3PV::OnSwapWeapon( nSlot) )
//		return false;
//	
//	//	Weapon Change
//	WeaponBase * pCurWeapon = getCurrentWeapon();
//
//	if( pCurWeapon != NULL)
//	{	
//		//	ĳ���� �ִϸ��̼��� �����ϸ� ���� �����Ӻ��� ����Ǳ� ������ �̹� �����ӿ� ���⸦ �ٲ��� �ʰ� �ٸ� �����ӿ� 
//		//	���⸦ �ٲٵ��� �Ѵ�.
//		setOldSlotNumber( getCurrentSlotNumber());
//		setSwapSlotNumber( nSlot);				//	���� �����ӿ� �ٲ� ���� ��ȣ 
//	}
//	else
//	{	//	���� ���Ⱑ NULL�̸� ���� �ٲ۴�.		
//		//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.
//		if( !OnSelectWeapon( nSlot))
//			return false;
//
//		WeaponBase * pChangeWeapon = getCurrentWeapon();
//		if( pChangeWeapon == NULL )
//			return false;
//
//		pChangeWeapon->WeaponChange();		//	���� ��ü�� ���� ����� Change ���·�
//	}
//
//	//	����� �⺻ �����տ� ����	
//	ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND);
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnDropedWeaponSearching( REAL32 rDeltatime)
//{
//	return _ProcessDropedWeaponSearsing();
//}

//bool CGameCharaWeaponControl3PVLocal::OnFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	return _ProcessFireForLocal();
//}
//
//bool CGameCharaWeaponControl3PVLocal::OnSecFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	CGameCharaWeaponControl3PV::OnSecFire( pStartPos, pTargetPos);
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
//	I3ASSERT( pWeaponInfo != NULL);
//
//	//	���� ����
//	switch( pWeaponInfo->GetTypeClass())
//	{			
//	case WEAPON_CLASS_KNIFE:
//		{
//			if( pWeapon->IsExactTypeOf( WeaponKnife::GetClassMeta()))
//			{
//				WeaponKnife * pWeaponKnife = (WeaponKnife *) pWeapon;
//				pWeaponKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
//			}
//			else if( pWeapon->IsExactTypeOf( WeaponDualKnife::GetClassMeta()))
//			{
//				WeaponDualKnife * pWeaponKnife = (WeaponDualKnife *) pWeapon;
//				pWeaponKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
//			}
//		}
//		break;
//	}
//
//	if( !pWeapon->Fire( WEAPON_SECONDARY_FIRE) )
//		return false;
//	m_pOwner->ApplyCrosshair();
//
//	// ������ ���
//	getBoneContext()->_ProcessFire();
//
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnExtActivate( void)
//{
//	CGameCharaWeaponControl3PV::OnExtActivate();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	if( !pWeapon->IsExtensionWeapon())		return false;
//
//	if( pWeapon->GetCurrentExtensionType() == WEAPON_EXT_DUAL_MAG)
//	{// ��� źâ�� ��ü�� �� �ִ°�?
//		if( pWeapon->getTotalBulletCount() <= 0 && pWeapon->getLoadedBulletDualCount() <= 0)
//			return false;
//	}
//
//	//	���� Extension Ȱ��ȭ / ��Ȱ��ȭ �߿��� ����� �߻� ���Ѵ�.	
//	if( pWeapon->NET_ExtensionActivate( true, false))
//	{
//		pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
//
//		return true;
//	}
//
//	return false;
//}

//bool CGameCharaWeaponControl3PVLocal::OnExtDeactivate( void)
//{
//	CGameCharaWeaponControl3PV::OnExtDeactivate();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	if( !pWeapon->IsExtensionWeapon())		return false;
//
//	if( pWeapon->GetCurrentExtensionType() == WEAPON_EXT_DUAL_MAG)
//	{// ��� źâ�� ��ü�� �� �ִ°�?
//		if( pWeapon->getTotalBulletCount() <= 0 && pWeapon->getLoadedBulletDualCount() <= 0)
//			return false;
//	}
//
//	//	���� Extension Ȱ��ȭ / ��Ȱ��ȭ �߿��� ����� �߻� ���Ѵ�.	
//	if( pWeapon->NET_ExtensionActivate( false, false))
//	{
//		pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
//		return true;
//	}
//
//	return false;
//}

//bool CGameCharaWeaponControl3PVLocal::OnLoadMagazine( void)
//{
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
//	I3ASSERT( pWeaponInfo != NULL);
//
//	//	�ѱ� �迭�� ����
//	if(pWeaponInfo->isGun() == false)
//		return false;
//	
//	//	KEYWORD : WEAPON_SHOTGUN_LAUNCHER	
//	if( pWeapon->GetCurrentExtensionType() == WEAPON_EXT_SHOTGUN_LAUNCHER && pWeapon->isExtensionActivate() )
//	{
//		WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*) pWeapon;
//		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON))	// ���� �����߿��� ���ε� ����
//			return false;
//
//		// KEYWORD : WEAPON_DUMMY
//		if( pWeapon->IsLoadMagazineLeftBarrel() )
//		{
//			m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//			pWeapon->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//		}
//
//		if( pLauncher->getTotalShellCount() <= 0)
//		{
//			return false;
//		}
//		
//		//	LSS ����
//		pLauncher->LoadShell();
//	}
//	else
//	{
//		//	���� �Ѿ��� ������ ��� ������ �� �� ����.
//		if( pWeapon->getTotalBulletCount() <= 0)	
//			return false;
//
//		if( (pWeapon->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) &&
//			pWeapon->getLoadedBulletCount() == pWeapon->GetActualLoadBullet() )
//			return false;
//
//		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON ))	// ���� �����߿��� ���ε� ����
//			return false;
//
//		// KEYWORD : WEAPON_DUMMY
//		if( pWeapon->IsLoadMagazineLeftBarrel() )
//		{
//			m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//			pWeapon->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//		}
//
//		pWeapon->SetEnable( true); //SetVisible( true);
//		pWeapon->LoadMagazine(CHARA_ANIMSET_3PV);
//	}
//
//	if( pWeaponInfo->GetTypeTrigger() != WEAPON_TRIGGER_RELOADING_SHOT )
//		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
//
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnLoadMagazineDual( void)
//{
//	CGameCharaWeaponControl3PV::OnLoadMagazineDual();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
//	I3ASSERT( pWeaponInfo != NULL);
//
//	if(pWeaponInfo->isGun() == false)
//		return false;
//
//	if( pWeapon->GetCurrentExtensionType() != WEAPON_EXT_DUAL_MAG )
//		return false;
//
//	//	���� �Ѿ��� ������ ��� ������ �� �� ����.
//	if( pWeapon->getTotalBulletCount() <= 0)	
//		return false;
//
//	pWeapon->SetEnable( true); //SetVisible( true);
//
//	//	���� Extension ���� 
//	pWeapon->ExtensionActivate( true);
//	g_pFramework->getChatBox()->PutSystemChatting( "Extension On");
//
//	if( pWeaponInfo->GetTypeTrigger() != WEAPON_TRIGGER_RELOADING_SHOT )
//		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	�߻� �غ��߿� �߻����� ���ϵ���. �ִϸ��̼��� ������ �߻� ����
//
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnLoadBullet( void)
//{
//	CGameCharaWeaponControl3PV::OnLoadBullet();
//
//	if( getCurrentWeapon() == NULL )	return false;
//
//	//	�ѱ� �迭�� ����
//	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;
//
//	if( !getCurrentWeapon()->isLoadBulletEnabled())	return false;
//
//	// KEYWORD : WEAPON_DUMMY
//	if( getCurrentWeapon()->IsLoadBulletLeftBarrel() )
//	{	// MG�� ź�� �����ÿ��� �޼� ����
//		m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//		getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//	}
//
//	getCurrentWeapon()->SetEnable( true);
//	getCurrentWeapon()->LoadBullet();
//
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnLoadMagazine_Ready( void)
//{
//	bool bRv = CGameCharaWeaponControl::OnLoadMagazine_Ready();
//
//	if( bRv)
//	{
//		bRv = _ActionWeapon_LoadMagazine_Ready_ForLocal();
//	}
//
//	return bRv;
//}



