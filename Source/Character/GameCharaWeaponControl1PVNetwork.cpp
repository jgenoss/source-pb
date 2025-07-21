#include "pch.h"
#include "GameCharaWeaponControl1PVNetwork.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl1PVNetwork, CGameCharaWeaponControl1PV);

CGameCharaWeaponControl1PVNetwork::CGameCharaWeaponControl1PVNetwork()
{
}

CGameCharaWeaponControl1PVNetwork::~CGameCharaWeaponControl1PVNetwork()
{
}

//void CGameCharaWeaponControl1PVNetwork::OnUpdate( REAL32 rDeltatime)
//{
//	CGameCharaWeaponControl1PV::OnUpdate( rDeltatime);
//
//	//	���� ���涧 �������� ������ ���� ��������...���� �����ӿ� �ٲ�(�ٲ��� �����) ���⸦ ���� �����Ѵ�.
//	if( getSwapSlotNumber() != getCurrentSlotNumber())
//	{
//		if( getCurrentWeapon() != NULL)
//		{
//			// ���� ����� ������� �ʴ� ����
//			getWeaponContext()->NoUseWeapon( getCurrentWeapon());
//		}
//		
//		//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.		
//		OnSelectWeapon( getSwapSlotNumber());
//
//		if( (getCurrentWeapon() != NULL) && g_pFramework->IsBattleStage())
//		{
//			getCurrentWeapon()->NET_WeaponChange( CHARA_ANIMSET_1PV, true);		//	���� ��ü�� ���� ����� Change ���·�
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
//
//		// Animation�� World-Space Transform ���̿� LOD ����� �޶��� �� ������,
//		// �װ��� Visual �� ���� ������ �� �� �ֱ� ������
//		// �������� LOD�� ����Ų��.
//		getCurrentWeapon()->setCurrentLOD( 0);
//	}
//}

//bool CGameCharaWeaponControl1PVNetwork::OnSwapWeapon( INT32 nSlot)
//{
//	if( !CGameCharaWeaponControl1PV::OnSwapWeapon( nSlot) )
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
//		if( !OnSelectWeapon( nSlot))	return false;
//		if( getCurrentWeapon() == NULL )	return false;
//		if( g_pGameContext->IsP2PActivate() )
//			getCurrentWeapon()->NET_WeaponChange( CHARA_ANIMSET_1PV, true);	//	���� ��ü�� ���� ����� Change ���·�
//	}
//
//	//	����� �⺻ �����տ� ����	
//	ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND);
//
//	return true;
//}
//
//bool CGameCharaWeaponControl1PVNetwork::OnFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	return _ProcessFireForNetwork( pStartPos, pTargetPos);
//}
//
//bool CGameCharaWeaponControl1PVNetwork::OnSecFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	return _ProcessFireForNetwork( pStartPos, pTargetPos, WEAPON_SECONDARY_FIRE);
//}

//bool CGameCharaWeaponControl1PVNetwork::OnExtActivate( void)
//{
//	CGameCharaWeaponControl1PV::OnExtActivate();
//
//	if( getCurrentWeapon() == NULL )				return false;
//	if( !getCurrentWeapon()->IsExtensionWeapon())	return false;
//
//	//	���� Extension Ȱ��ȭ / ��Ȱ��ȭ �߿��� ����� �߻� ���Ѵ�.	
//	getCurrentWeapon()->NET_ExtensionActivate( true);
//
//	return true;
//}

//bool CGameCharaWeaponControl1PVNetwork::OnExtDeactivate( void)
//{
//	CGameCharaWeaponControl1PV::OnExtDeactivate();
//
//	if( getCurrentWeapon() == NULL )				return false;
//	if( !getCurrentWeapon()->IsExtensionWeapon())	return false;
//
//	//	���� Extension Ȱ��ȭ / ��Ȱ��ȭ �߿��� ����� �߻� ���Ѵ�.	
//	getCurrentWeapon()->NET_ExtensionActivate( false);
//
//	return true;
//}
//
//bool CGameCharaWeaponControl1PVNetwork::OnLoadMagazine( void)
//{
//	if( getCurrentWeapon() == NULL )		return false;
//	
//	I3ASSERT( getCurrentWeapon()->getWeaponInfo() != NULL);
//	//	�ѱ� �迭�� ����
//	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;
//
//	//	KEYWORD : WEAPON_SHOTGUN_LAUNCHER
//	if( getCurrentWeapon()->GetCurrentExtensionType() == WEAPON_EXT_SHOTGUN_LAUNCHER && getCurrentWeapon()->isExtensionActivate() )
//	{		
//		WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*) getCurrentWeapon();
//		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON))
//			return false;
//
//		if( pLauncher->getTotalShellCount() <= 0)
//		{	
//			return false;
//		}
//		
//		pLauncher->LoadShell();
//	}
//	else
//	{
//		// KEYWORD : WEAPON_DUMMY
//		if( getCurrentWeapon()->IsLoadMagazineLeftBarrel() )
//		{
//			m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//			getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//		}
//
//		getCurrentWeapon()->SetEnable( true);
//		getCurrentWeapon()->NET_LoadMagazine(CHARA_ANIMSET_1PV);
//	}
//
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//
//	return true;
//}

//bool CGameCharaWeaponControl1PVNetwork::OnLoadMagazineDual( void)
//{
//	CGameCharaWeaponControl1PV::OnLoadMagazineDual();
//
//	if( getCurrentWeapon() == NULL )		return false;
//	I3ASSERT( getCurrentWeapon()->getWeaponInfo() != NULL);
//	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;
//	if( getCurrentWeapon()->GetCurrentExtensionType() != WEAPON_EXT_DUAL_MAG )	return false;
//
//	getCurrentWeapon()->SetEnable( true);
//	getCurrentWeapon()->ExtensionActivate( true);	//	���� Extension ���� 
//
//	return true;
//}

//bool CGameCharaWeaponControl1PVNetwork::OnLoadBullet( void)
//{
//	CGameCharaWeaponControl1PV::OnLoadBullet();
//
//	if( getCurrentWeapon() == NULL )	return false;
//
//	//	�ѱ� �迭�� ����
//	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;
//
//	// KEYWORD : WEAPON_DUMMY
//	if( getCurrentWeapon()->IsLoadBulletLeftBarrel() )
//	{	// MG�� ź�� �����ÿ��� �޼� ����
//		m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//		getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//	}
//
//	getCurrentWeapon()->SetEnable( true);
//
//	// 2006. 9. 23 by ������
//	// desc : ��Ʈ��ũ �κ� �߰�
//	getCurrentWeapon()->NET_LoadBullet( CHARA_ANIMSET_1PV);
//
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//	
//	return true;
//}

//bool CGameCharaWeaponControl1PVNetwork::OnLoadMagazine_Ready( void)
//{	
//	bool bRv = CGameCharaWeaponControl::OnLoadMagazine_Ready();
//
//	if( bRv)
//	{
//		bRv = _ActionWeapon_LoadMagazine_Ready_ForNetwork();
//	}
//
//	return bRv;
//}

