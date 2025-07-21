#include "pch.h"
#include "GameCharaWeaponControl1PVLocal.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl1PVLocal, CGameCharaWeaponControl1PV);

CGameCharaWeaponControl1PVLocal::CGameCharaWeaponControl1PVLocal()
{
}

CGameCharaWeaponControl1PVLocal::~CGameCharaWeaponControl1PVLocal()
{
}
//
//void CGameCharaWeaponControl1PVLocal::OnUpdate( REAL32 rDeltatime)
//{
//	CGameCharaWeaponControl1PV::OnUpdate( rDeltatime);
//
//	//	���� ���涧 �������� ������ ���� ��������...���� �����ӿ� �ٲ�(�ٲ��� �����) ���⸦ ���� �����Ѵ�.
//	if( getSwapSlotNumber() != getCurrentSlotNumber())
//	{
//		if( getCurrentWeapon() != NULL )
//		{
//			// ���� ����� ������� �ʴ� ����
//			getWeaponContext()->NoUseWeapon( getCurrentWeapon());
//		}
//		
//		//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.		
//		OnSelectWeapon( getSwapSlotNumber());
//
//		// �ٲ� ����
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
//	}
//	
//	if( getCurrentWeapon() != NULL)
//	{
//		// KEYWORD : WEAPON_DUMMY
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
//
//		
//	}
//}

//bool CGameCharaWeaponControl1PVLocal::OnSwapWeapon( INT32 nSlot)
//{
//	if( !CGameCharaWeaponControl::OnSwapWeapon( nSlot) )
//		return false;
//
//	//	Weapon Change
//	if( getCurrentWeapon() != NULL)
//	{	
//		//	ĳ���� �ִϸ��̼��� �����ϸ� ���� �����Ӻ��� ����Ǳ� ������ �̹� �����ӿ� ���⸦ �ٲ��� �ʰ� �ٸ� �����ӿ� 
//		//	���⸦ �ٲٵ��� �Ѵ�.
//		setOldSlotNumber( getCurrentSlotNumber());
//		setSwapSlotNumber( nSlot);		//	���� �����ӿ� �ٲ� ���� ��ȣ 
//	}
//	else
//	{	//	���� ���Ⱑ NULL�̸� ���� �ٲ۴�.		
//		//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.
//		if( !OnSelectWeapon( nSlot))			return false;
//		if( getCurrentWeapon() == NULL )		return false;
//		if( g_pGameContext->IsP2PActivate() )
//			getCurrentWeapon()->WeaponChange();		//	���� ��ü�� ���� ����� Change ���·�
//	}
//
//	if( g_pEdgeTrail != NULL)
//	{
//		g_pEdgeTrail->Reset();
//	}
//
//	//	����� �⺻ �����տ� ����	
//	ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND);
//	return true;
//}

//bool CGameCharaWeaponControl1PVLocal::OnDropedWeaponSearching( REAL32 rDeltatime)
//{
//	return _ProcessDropedWeaponSearsing();
//}

//bool CGameCharaWeaponControl1PVLocal::OnFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	return _ProcessFireForLocal();
//}

//bool CGameCharaWeaponControl1PVLocal::OnSecFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	//CGameCharaWeaponControl1PV::OnSecFire( pStartPos, pTargetPos);
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )		return false;
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
//	case WEAPON_CLASS_CIC:
//		{
//			WeaponKnife * pWeaponKnife = (WeaponKnife*) ((WeaponDualCIC*)pWeapon)->getLeftWeapon();
//			{
//				pWeaponKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
//				if( (g_pCamera != NULL) && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
//				{
//					((CGameCamera_FPS*)g_pCamera)->SetNeckAnim(true);
//				}
//			}
//		}
//		break;
//	}
//
//	if( !pWeapon->Fire( WEAPON_SECONDARY_FIRE) )
//		return false;
//
//	// �̰�	�����̳� ��ȹ���� ��û���� ������� �ʽ��ϴ�. �߻��߿� ���ҾȵǴ� ����Դϴ�.
//	//addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
//
//	if( pWeapon->IsExactTypeOf( WeaponBombTrigger::GetClassMeta() ) ||
//		pWeapon->IsTypeOf( WeaponGrenadeLauncher::GetClassMeta()) )
//	{
//		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
//	}
//
//	m_pOwner->ApplyCrosshair();
//
//	// ������ ���
//	getBoneContext()->_ProcessFire();
//
//	return true;
//}
//
//bool CGameCharaWeaponControl1PVLocal::OnExtActivate( void)
//{
//	CGameCharaWeaponControl1PV::OnExtActivate();
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
//	if( pWeapon->isWeaponStateMask( WSM_EXT_DISABLE ))
//		return false;
//
//	//	���� Extension Ȱ��ȭ / ��Ȱ��ȭ �߿��� ����� �߻� ���Ѵ�.	
//	if( pWeapon->NET_ExtensionActivate( true))
//	{			
//		pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
//
//		return true;
//	}
//
//	return false;
//}
//
//bool CGameCharaWeaponControl1PVLocal::OnExtDeactivate( void)
//{
//	CGameCharaWeaponControl1PV::OnExtActivate();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	if( !pWeapon->IsExtensionWeapon())		return false;
//
//	if( pWeapon->isWeaponStateMask( WSM_EXT_DISABLE) )
//		return false;
//
//	if( pWeapon->GetCurrentExtensionType() == WEAPON_EXT_DUAL_MAG)
//	{// ��� źâ�� ��ü�� �� �ִ°�?
//		if( pWeapon->getTotalBulletCount() <= 0 && pWeapon->getLoadedBulletDualCount() <= 0)
//			return false;
//	}
//
//	//	���� Extension Ȱ��ȭ / ��Ȱ��ȭ �߿��� ����� �߻� ���Ѵ�.	
//	if( pWeapon->NET_ExtensionActivate( false))
//	{
//		pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
//
//		return true;
//	}
//
//	return false;
//}

//bool CGameCharaWeaponControl1PVLocal::OnLoadMagazine( void)
//{
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )		return false;
//
//	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
//	I3ASSERT( pWeaponInfo != NULL);
//
//	//	�ѱ� �迭�� ����
//	if(pWeaponInfo->isGun() == false)
//		return false;
//
//	pWeapon->SetEnable( true); //SetVisible( true);
//	
//	if( pWeapon->GetCurrentExtensionType() == WEAPON_EXT_GLAUNCHER && pWeapon->isExtensionActivate() )
//	{
//		WeaponGrenadeLauncher * pGLauncher = (WeaponGrenadeLauncher*) pWeapon;
//		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON))
//			return false;
//
//		if( pGLauncher->getTotalShellCount() <= 0)
//			return false;
//		
//		pGLauncher->LoadShell();
//	}
//	//	KEYWORD : WEAPON_SHOTGUN_LAUNCHER	
//	else if( pWeapon->GetCurrentExtensionType() == WEAPON_EXT_SHOTGUN_LAUNCHER && pWeapon->isExtensionActivate() )
//	{
//		WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*) pWeapon;
//		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON))	// ���� �����߿��� ���ε� ����
//			return false;
//
//		if( pLauncher->getTotalShellCount() <= 0)
//			return false;
//
//		if( (pLauncher->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) &&
//			pLauncher->getLoadedShellCount() == pLauncher->getExtensionWeaponInfo()->GetLoadBullet() )
//			return false;
//
//		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON ))	// ���� �����߿��� ���ε� ����
//			return false;
//
//		// KEYWORD : WEAPON_DUMMY
//		if( pLauncher->IsLoadMagazineLeftBarrel() )
//		{
//			m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//			pLauncher->ChangeGunDummy( WEAPON_ATTACH_BARREL);
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
//		pWeapon->LoadMagazine(CHARA_ANIMSET_1PV);
//	}
//	
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//
//	return true;
//}

//bool CGameCharaWeaponControl1PVLocal::OnLoadMagazine_Ready( void)
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

//bool CGameCharaWeaponControl1PVLocal::OnLoadMagazineDual( void)
//{
//	CGameCharaWeaponControl1PV::OnLoadMagazineDual();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//	I3ASSERT( pWeapon->getWeaponInfo() != NULL);
//
//	if(pWeapon->getWeaponInfo()->isGun() == false)
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
//	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	�߻� �غ��߿� �߻����� ���ϵ���. �ִϸ��̼��� ������ �߻� ����
//
//	return true;
//}

//bool CGameCharaWeaponControl1PVLocal::OnLoadBullet( void)
//{
//	CGameCharaWeaponControl1PV::OnLoadBullet();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	//	�ѱ� �迭�� ����
//	if(pWeapon->getWeaponInfo()->isGun() == false)
//		return false;
//
//	if(pWeapon->isLoadBulletEnabled() == false)
//		return false;
//
//	// KEYWORD : WEAPON_DUMMY
//	if( pWeapon->IsLoadBulletLeftBarrel() )
//	{	// MG�� ź�� �����ÿ��� �޼� ����
//		m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//		pWeapon->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//	}
//
//	pWeapon->SetEnable( true); //SetVisible( true);
//	pWeapon->LoadBullet();
//	
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//
//	return true;
//}

//bool CGameCharaWeaponControl1PVLocal::OnRemoveShell( void)
//{
//	CGameCharaWeaponControl1PV::OnRemoveShell();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	if( pWeapon->GetCurrentExtensionType() != WEAPON_EXT_GLAUNCHER)
//		return false;
//
//	WeaponGrenadeLauncher * pGLauncher = (WeaponGrenadeLauncher*) pWeapon;
//
//	if( !pGLauncher->isLoadBulletEnabled())
//		return false;
//
//	pGLauncher->SetEnable( true); //SetVisible( true);
//	pGLauncher->RemoveShell();
//	
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//
//	return true;
//}



