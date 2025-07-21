#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaPlayer.h"
#include "StageBattle.h"
#include "Weapon.h"
#include "Crosshair_Cross.h"

//#define PHYSIX_CHARA_SLEEP_TIME		3.f
//#define CAMERA_CHARA_SLEEP_TIME		1.f
//#define PHYSIX_EFFECT_DUTY_FORCE	0.f
//#define HUD_CROSSNICKCHECK_TIME		0.0666f	// ũ�ν���� ǥ�� �˻�( �ʴ� 15ȸ )


//#define		SPROFILE(a)			( m_timeProfile[a].m_timeStart = m_pTimer->GetProfileTime())
//#define		EPROFILE(a)			{ m_timeProfile[a].m_timeEnd += m_pTimer->GetProfileTime() - m_timeProfile[a].m_timeStart; m_timeProfile[a].m_bValid = true; }

I3_CLASS_INSTANCE( CGameCharaPlayer, CGameCharaBase );

CGameCharaPlayer::CGameCharaPlayer( void)
{
	OnInitVariable();
}

CGameCharaPlayer::~CGameCharaPlayer( void)
{	
	//DetachCollisionNode();
	//Destroy();
}

//void CGameCharaPlayer::Destroy( void)
//{
//	INT32 i;
//	if( getWeaponContext() != NULL)
//	{
//		for( i = 0; i < MAX_ITEMSLOT_COUNT; i++)
//		{
//			getWeaponContext()->getControl()->OnDeleteWeapon( i);
//		}
//	}

//	if( getEquipContext() != NULL)
//	{
//		for( i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)
//		{
//			getEquipContext()->getControl()->OnDeleteEquip( (EQUIP_CLASS_TYPE) i);
//		}
//	}

	//for( i = 0; i < CHARA_SOUND_COUNT; i++)
	//{
	//	I3_SAFE_RELEASE( m_pCharaSoundRes[ i]);
	//}

	//if( m_pContactHitList != NULL)
	//{
	//	for( i = 0; i < m_pContactHitList->GetCount(); i++ )
	//	{
	//		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_pContactHitList->GetItem( i);
	//		I3_SAFE_RELEASE( pShapeSet);
	//	}
	//	I3_SAFE_RELEASE( m_pContactHitList);
	//}
//}

//void CGameCharaPlayer::OnInitVariable( void)
//{	
//	CGameCharaBase::OnInitVariable();

	//m_pDBInfo				= NULL;
	//m_pCharaInfo			= NULL;
	
	//for( i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)
	//{
	//	m_pCharaEquip[ i] = NULL;
	//	m_pEquipInfo[ i] = NULL;
	//}

//	m_Theta					= 0.0f;
//	m_Rho					= 0.0f;

//	m_RecoilData.Reset();
//	m_DeviationData.Reset();
//	m_ScopeWaveData.Reset();

//	m_bValidPivot			= false;		
//	m_PivotYaw				= 0.0f;
//	m_PivotYawTarget		= 0.0f;
//	m_timeYawBlend			= 0.0f;
//	m_timeTargetYawBlend	= 0.0f;
//	m_PivotRoll				= 0.0f;
//	i3Vector::Zero( &m_PivotAxis);

//	i3Matrix::Identity( &m_mtxPivot);
//	for( i = 0; i < BONE_MAX; i++)
//	{
//		m_idxBone[ i] = -1;
//		i3Matrix::Identity( &m_Bone[i]);
//	}

	//m_rIKHeight		= 0.0f;
	
//	m_CurFloorTerrainType		= (I3_TERRAIN_TYPE) 0;
//	i3Physix::HitResultClear( &m_CrossHitResult );
//	m_CrossHitLine.SetStart( i3Matrix::GetPos( GetMatrix() ) );
//	m_CrossHitLine.SetDir( i3Matrix::GetAt( GetMatrix() ) );
//	m_rCrossHitTimer			= 0.f;
//	m_pCrossHitChara			= NULL;

	//m_pContactHitList			= NULL;
	
//	m_rContactTime				= 0.f;
//	m_nContactReportCount		= 0;
//	m_nContactReportReadPos		= 0;
//	for( i = 0; i < PHYSIX_CONTACT_REPORT_COUNT; i++ )
//	{
//		i3Physix::ContactReportClear( &m_ContactReportBuf[ i ] );
//	}
//	for( i = 0; i < 6; i++)
//	{
//		m_bContactBones[ i ]	= FALSE;
	//	i3Vector::Zero( &m_vBonesForceDir[ i]);
	//}
	
	//m_nCollisionGroup			= 0;

	//m_pRoughHitTrans			= NULL;

	//m_DropCount = 0;

	//m_nScrEffectType	= SCR_EFFECT_NONE;
	//i3Color::Set( &m_EFFECT_ScreenColor, 0, 0, 0, 0);
	//m_fBlurStart		= 0.0f;
	//m_fBlurEnd			= 0.0f;

//#if defined( I3_DEBUG )
//	m_fDebugtimeStart	= 0;
//	m_fDebugtimeEnd		= 0;
//	i3Vector::Zero( &m_vDebugPos );
//	i3Vector::Zero( &m_vDebugOriginalPos );
//	i3Vector::Zero( &m_vDebugTargetPos);
//#endif

	//m_nAccessory		= 0;
	//m_fRunDuration		= 0.666f;

//	m_velShove = m_acclShove = m_tShove = 0.0f;
//	i3Matrix::Identity( &m_ShoveMatrix);
//	i3Quat::Identity( &m_quatShoveStart);
//	m_bShoveActive = false;
//}

/*
void CGameCharaPlayer::Create( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo)
{	
	__SLOG2( g_pGameContext->GetNickForSlot( pCharaInfo->_NetIdx));

	CGameCharaBase::Create( pRes, pCharaInfo, pDBInfo);
	//	Add after here

	m_pCharaInfo = pCharaInfo;
	m_pDBInfo = pDBInfo;

	//_InitBones();

	// Update Timing�� �����ϱ� ����, �������� Animation, World-Transform ����
	// �ϵ��� �Ѵ�.
	{
		// Animation Update�� World-Space Cache Matrix�� ���� �ð�����
		// ���ֱ� ����, �������� Update�� �Ѵ�.
		
		if( g_pGameContext->IsBattle() )
		{
			m_pBody->addStyle( I3SG_MANUAL_ANIM_UPDATE | I3SG_MANUAL_TRANSFORM);
			GetSceneObject()->AddFlag( I3_NODEFLAG_MANUAL_UPDATE);

			// �� ��, Animation Update�� World-Space Transform ���̿� LOD ��꿡��
			// Current Level�� �ٸ� �� �ֱ� ������...
			// ���۾����� LOD�� ����Ѵ�.
			m_pBody->removeStyle( I3SG_LOD_AUTO);

			//m_pContactHitList = i3List::NewObject();
			//I3ASSERT( m_pContactHitList != NULL);
		}
	}
			
	//_InitCharaSound();		//	���� �ε�	
	
	getCollisionContext()->AttachCollisionNode( GetSceneObject(), 0);

	// ���� ���	
	// ! praptor - getMySlotIdx -> GetMySlotIdxEx ��ü�߽��ϴ�.
	// �� �̿ܿ� ��ҿ����� ȣ��ɶ� idx�� 0������ ������ ������,
	// ��� ���(RoomInfo�ʱ�ȭ ��)�� -1�� �������������ϴ�.

	NET_CHARA_INFO * pNetCharaInfo;
	if( pCharaInfo->_NetIdx != MAX(0, g_pGameContext->GetMySlotIdxEx()) )		
		pNetCharaInfo  = g_pGameContext->getNetCharaInfo( pCharaInfo->_NetIdx );
	else		
		pNetCharaInfo = g_pGameContext->GetMyCharaInfo();

	g_pCharaManager->SetCharaWeaponSet( this, pNetCharaInfo);

	//	����� ����մϴ�.
	INT32 helmet = 0;

#if defined( I3_DEBUG)
	if( !g_pConfig->m_bNetworking)
	{
		switch( pCharaInfo->_ResType )
		{
		case CHARARES_REBEL_MALE :			helmet = EQUIPRES_NUM_HELMET_REBEL_M;		break;
		case CHARARES_SWAT_MALE :			helmet = EQUIPRES_NUM_HELMET_SWAT_M;		break;
		case CHARARES_REBEL_FEMALE :		helmet = EQUIPRES_NUM_HELMET_REBEL_F;		break;
		case CHARARES_SWAT_FEMALE :			helmet = EQUIPRES_NUM_HELMET_SWAT_F;		break;
		}
		m_pEquipContext->setEquipInfo( EQUIP_CLASS_TYPE_HELMET, g_pCharaInfoDataBase->getEquipInfo( EQUIPRES_TYPE_DEFAULT));
		if( m_pEquipContext->getControl()->OnCreateEquip( EQUIP_CLASS_TYPE_HELMET, helmet))
		{
			CGameCharaEquip * pEquip = m_pEquipContext->getControl()->getEquip( EQUIP_CLASS_TYPE_HELMET);
			if( getCharaTeam() & CHARACTER_TEAM_RED )
			{
				pEquip->SetCollisionGroup( CGRP_TEAM_RED, CGRP_MASK_TEAM_RED );
			}
			else
			{
				pEquip->SetCollisionGroup( CGRP_TEAM_BLUE, CGRP_MASK_TEAM_BLUE );
			}
		}
	}
#endif

	EQUIPRES_TYPE equipType = EQUIP_NUMBER( pNetCharaInfo->_EquipCharacter._nohair);
	switch( equipType )
	{
	case EQUIPRES_TYPE_DEFAULT :
		switch( pCharaInfo->_ResType )
		{
		case CHARARES_REBEL_MALE :			helmet = EQUIPRES_NUM_HELMET_REBEL_M;		break;
		case CHARARES_SWAT_MALE :			helmet = EQUIPRES_NUM_HELMET_SWAT_M;		break;
		case CHARARES_REBEL_FEMALE :		helmet = EQUIPRES_NUM_HELMET_REBEL_F;		break;
		case CHARARES_SWAT_FEMALE :			helmet = EQUIPRES_NUM_HELMET_SWAT_F;		break;
		}
		break;
	}

	m_pEquipContext->setEquipInfo( EQUIP_CLASS_TYPE_HELMET, g_pCharaInfoDataBase->getEquipInfo( equipType));
	
	if( m_pEquipContext->getControl()->OnCreateEquip( EQUIP_CLASS_TYPE_HELMET, helmet))
	{
		CGameCharaEquip * pEquip = m_pEquipContext->getControl()->getEquip( EQUIP_CLASS_TYPE_HELMET);
		if( getCharaTeam() & CHARACTER_TEAM_RED )
		{
			pEquip->SetCollisionGroup( CGRP_TEAM_RED, CGRP_MASK_TEAM_RED );
		}
		else
		{
			pEquip->SetCollisionGroup( CGRP_TEAM_BLUE, CGRP_MASK_TEAM_BLUE );
		}
	}

	
	//	Ŭ�� ��ũ ����
	SetCharaClanTexture( 0);

	ResetChara();		//	���� Ŭ�������� �ҷ��� �Ѵ�.

	__ELOG2( g_pGameContext->GetNickForSlot( pCharaInfo->_NetIdx));
}
*/

//void CGameCharaPlayer::CreateInLobby( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo)
//{
//	Create(pRes, pCharaInfo, pDBInfo);
	
//	m_pUpdateContext->OnChangeProfile();

	//	�κ� �ִϸ��̼����� �÷���
	//ResetLobbyAnim();
//}

//void CGameCharaPlayer::ResetChara()
//{
//	CGameCharaBase::ResetChara();

//	INT32 i;
	
//	m_Theta			= 0.0f;
//	m_Rho			= 0.0f;
	
//	m_PivotYaw		= 0.0f;
//	m_PivotRoll		= 0.0f;

	//m_fJumpDelayElapsedTime = 0.f;
	//m_rFlashElapseTime		= 0.f;
	//m_rFlashEffectiveTime	= 0.f;
	//m_rDamageElapseTime		= 0.f;
	//m_rRespawnElapseTime	= 0.f;
	//m_rInvincibleElapseTime	= 0.f;
	
	//m_rContactTime			= 0.f;
	//for( i = 0; i < 6; i++)
	//{
	//	m_bContactBones[ i] = FALSE;
	//	i3Vector::Zero( &m_vBonesForceDir[ i]);
	//}

	//m_fLeftStepTime			= 0.0f;
	//m_fRightSteptime		= 0.0f;

	//m_pPurchasePrimWeapon = NULL;

	//m_DropCount = 0;

	//	ĳ���Ϳ� ���̱� ���� ���� �پ� �ִ� ��� ���⸦ �����
	//for( i = 0; i < MAX_ITEMSLOT_COUNT; ++i)
	//{
	//	getWeaponContext()->getControl()->OnDetachWeapon( i);
	//}

	//	���� �ʱ�ȭ
	//getWeaponContext()->getControl()->OnResetWeapon();
	//--> WeaponControl
	/*
	{
		BOOL bCheck = FALSE;
		i = 0;

		while( !bCheck )
		{
			if( WEAPON_Select( i) )
			{
				break;
			}

			i++;
#if defined( I3_DEBUG )
			if( i >= MAX_ITEMSLOT_COUNT )
			{
				I3ASSERT_0;	// invalid server infomation for weapon list
			}
#endif
		}

		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->ReturnToReady();
		}

		m_nSwapWeaponSlotNum = m_nOldWeaponSlotNum = m_nCurWeaponSlotNum;

		for( i = 0; i < MAX_ITEMSLOT_COUNT; ++i)
		{
			WeaponBase * pWeapon = getWeaponContext()->getWeapon( i);

			if( pWeapon != NULL)
				pWeapon->Reset();
		}

		//	����� �⺻ �����տ� ����
		ChangeWeaponAttachHand( CHARA_WEAPON_ATTACH_RIGHT_HAND);

		getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), 1.0f);
	}

	//	��� �ʱ�ȭ 
	{
		for( i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)
		{
			EQUIP_AttachChara( (EQUIP_CLASS_TYPE) i, TRUE);
		}

		EQUIP_SetBlendAlphaAll( 1.0f, TRUE);
	}
	*/
	
	//ResetDefaultInfo();					//	Stat �ʱ�ȭ

	//ResetDefaultAnim( !is1PV());		//	�⺻ ����Ʈ Idle Anim���� �ʱ�ȭ
	//CALL_ACTION_COMMAND( ACTION_CMD_RESET, NULL);

	//	Meterial Alpha �ʱ�ȭ�ؾ� ���ɴϴ�.
	//SetBlendAlpha( 1.f );
	/*
	{
		i3Color::SetA( m_pMaterialAttr->GetAmbient(), 1.0f);
		i3Color::SetA( m_pMaterialAttr->GetDiffuse(), 1.0f);
		i3Color::SetA( m_pMaterialAttr->GetSpecular(), 1.0f);
	}
	*/

	//StopCharaSoundAll();

	//m_bRightStepSound = FALSE;
	//m_fRunElapseTime = 0.0f;

//	m_CurFloorTerrainType = (I3_TERRAIN_TYPE) 0;
	/*
	if( m_pController != NULL )
	{
		NxController * pNxController = m_pController->getNxController();
		if( pNxController == NULL )
		{
			I3WARN( "Controller is NULL." );
			return;
		}

		NxActor * pAct = pNxController->getActor();
		if( pAct == NULL )
		{
			I3WARN( "Actor is NULL." );
			return;
		}

		pAct->clearBodyFlag( NX_BF_DISABLE_GRAVITY );
	}
	*/

	//m_bClimbingLadder	= false;
	
//	m_nContactReportCount = 0;
//	m_nContactReportReadPos = 0;
//}

/*
void CGameCharaPlayer::_ProcessOldWeapon( void)
{
	
	if( m_nSwapWeaponSlotNum == m_nCurWeaponSlotNum)
		return;

	BOOL bInStage = FALSE;
	i3Stage* pStage = g_pFramework->GetCurrentStage();
	if (g_pFramework->IsBattleStage())
	{
		bInStage = TRUE;
	}

	WeaponBase * pOldWeapon = getWeaponContext()->getWeapon( m_nCurWeaponSlotNum );
	pOldWeapon->ReturnToReady();	//	���� ��ü�� ���� ����� Ready ���·� ����
	pOldWeapon->GetSceneObject()->GetBody()->StopAnim();	// ���� ����� �ִϸ��̼��� �������Ѿ� �մϴ�.
	pOldWeapon->GetSceneObject()->AddFlag( I3_NODEFLAG_DISABLE );
	// ���� ����� �������� �����ݴϴ�.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)(getCharaNetIndex()) * 10.f, PHYSIX_HEIGHT_WEAPON, (REAL32)(m_nCurWeaponSlotNum) * 10.f );
	g_pWeaponManager->MoveShapePosition( pOldWeapon, &vec );
	
	//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.		
	WEAPON_Select( m_nSwapWeaponSlotNum);

	// HUD
	if( bInStage)
	{
		CHud * pHud = ((CStageBattle*)pStage)->getHud();
		if( pHud != NULL )
		{	
			pHud->setOldLoadedBullet( 0 );
		}
	}

	// �� ������ ��� ���ѿ� ���Դϴ�.
	if( !is1PV() )
	{
		WEAPON_AttachToBackpack( m_nOldWeaponSlotNum);
		WEAPON_AttachToSidepack( m_nOldWeaponSlotNum);
	}

	if( isNetwork() )
	{
		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->NET_WeaponChange();
		}
	}
	else
	{
		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->WeaponChange();		//	���� ��ü�� ���� ����� Change ���·�
		}
	}
}

//void CGameCharaPlayer::OnUpdate( REAL32 tm)
//{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	// ������ ó�� ������ �ſ� �߿��ϰ�, ������ ���� ���� �ʿ��ϴ�.
	//CGameCharaBase::OnUpdate( tm);
	//	Add code here

	//m_fDebugtimeStart = g_pViewer->GetTimer()->GetProfileTime();

 	//_ProcessChangePose( tm);	// Animation ���� ó��
	//m_pCameraContext->ProcessChangePose( tm);

	BOOL bInStage = FALSE;

	i3Stage* pStage = g_pFramework->GetCurrentStage();
	if (g_pFramework->IsBattleStage())
	{
		bInStage = TRUE;
	}

	if( !isNetwork() && bInStage)
	{
		m_pMoveContext->Move( tm);
		m_pMoveContext->UpdateSpeedReduction( tm);
	}

	i3Chara::OnUpdate( tm);		// �̵� �ӵ� �ݿ� �� AIState Drive
	
	//	1��Ī�� ��쿡 ��ü aistate�ǰ��� ȣ����� �����Ƿ� �ʿ��Ѱ�� ���� ȣ�����ֵ��� �ϴ� �Լ�
	if( is1PV())
	{
		if( !isNetwork())
		{	
			if( getActionContext()->isLower( CHARA_LOWER_JUMP) ||
				!isStateMask( CHARA_STATE_FLOOR) || isStateMask( CHARA_STATE_JUMP))
			{
				OnJumpStartRun( m_pAnimContext->getAILower(), tm);
			}
			else if( getActionContext()->isLower( CHARA_LOWER_MOVE) && isStateMask( CHARA_STATE_FLOOR))
			{
				OnMoveRun( m_pAnimContext->getAILower(), tm);
			}
			else if( getActionContext()->isLower( CHARA_LOWER_WALK) && isStateMask( CHARA_STATE_FLOOR))
			{	
				OnMoveRun( m_pAnimContext->getAILower(), tm);
			}
		}
		else
		{
			if( getActionContext()->isLower( CHARA_LOWER_JUMP) )
			{
				OnJumpStartRun( m_pAnimContext->getAILower(), tm);
			}
			else if( getActionContext()->isLower( CHARA_LOWER_MOVE))
			{
				OnMoveRun( m_pAnimContext->getAILower(), tm);
			}
			else if( getActionContext()->isLower( CHARA_LOWER_WALK))
			{	
				OnMoveRun( m_pAnimContext->getAILower(), tm);
			}
		}
	}

	getWeaponContext()->getControl()->OnUpdate( tm);		// ���Ŀ� ������Ʈ ������ �Űܾ� �մϴ�. dfly 20080130
	
	// Animation�� World-Space Transform ���̿� LOD ����� �޶��� �� ������,
	// �װ��� Visual �� ���� ������ �� �� �ֱ� ������
	// �������� LOD�� ����Ų��.
	if( g_pGameContext->IsP2PActivate() )	
	{
		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		
		if( is1PV() )//&& (!isNetwork() || bFPSView))
		{	// 1��Ī ĳ����
			m_pBody->setCurrentLevel( 0);

			if( pWeapon != NULL)
			{
				pWeapon->setCurrentLOD( 0);
			}
		}
		else
		{	// 3��Ī ĳ����
			//	Zoom�� FOV ���� ���
			REAL32 fRatio = g_pCamera->GetCemeraDefaultFOV() / g_pCamera->GetCameraFOV();

			//	�𵨸� LOD ���
			calcLOD( GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio); 
		}
	}

	{
		m_pBoneContext->UpdateAnim( tm);
		//_UpdateAnim( tm);			// Animation�� Key ���� ���� (�� Bone�� Cache Matrix�� ����)

		m_pBoneContext->CalcTheta( tm);
		//_calcTheta( tm);				// Position

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// ���Ͽ����� GetSceneObject()�� Matrix�� ����Ǿ �ȵȴ�.
		// ���� ����ȴٸ�, Bone ��꿡�� ����� ��ŭ�� ������ �� �� �ִ�.
		//

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 ���� World-Space Matrix�� ���
		// 

		m_pBoneContext->CalcSpineBones();
		//_calcSpineBones();

		//	Rho 
		m_pBoneContext->CalcRecoil( tm);
		m_pBoneContext->CalcRho( tm);
		//_calcRecoil( tm);			// ������
		//_calcRho( tm);				// IK Bone ( IK Cache Matrix)
		
		if( g_pGameContext->IsP2PActivate() )	
		{
			m_pBoneContext->TransformWorldSpace( tm);
			//_TransformWorldSpace( tm);
			m_pMaterialContext->DriveLightVolume( tm);
			//_DriveLightVolume( tm);
			//_CAMERA_UpdateState();
			m_pCameraContext->OnUpdate( tm);
		}
	}

	if( !isAlive() )
	{
		//_ProcessDownEffect();
		ProcessDownEffect();

		if( isStateMask( CHARA_STATE_PHYSIXCONTACT) )
		{
			m_rContactTime += tm;

			if( !isStateMask( CHARA_STATE_PHYSIXFREEZE) && m_rContactTime >= PHYSIX_CHARA_SLEEP_TIME )
			{	
				bool bAllSleep = true;

				for( INT32 i = 0; i < m_PhysixShapeSetList.GetCount(); i++)
				{
					i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_PhysixShapeSetList.GetItem( i);

					NxActor * pActor = pShapeSet->getNxActor();
					if( pActor != NULL && pActor->isDynamic() )
					{
						if( !pActor->isSleeping() )
						{
							bAllSleep = false;
							pActor->putToSleep();
						}
					}
				}
				
				if( bAllSleep )
				{
					addStateMask( CHARA_STATE_PHYSIXFREEZE);
				}
			}
		}
	}

	//	ĳ���� �𵨸� ���� ó��
	m_pMaterialContext->ProcessMaterial( tm);
	//_ProcessMaterial( tm);

	//	������� HUD ȭ�� ����Ʈ 
	//_ProcessScreenEffect( tm);

	//_UpdateCharaSound( tm);

	//char szTemp[ 256 ];
	//m_fDebugtimeEnd = g_pViewer->GetTimer()->GetProfileTime() - m_fDebugtimeStart;

	//sprintf( szTemp, "CharaUpdate %d: %.4f\n", this->getCharaNetIndex(), m_fDebugtimeEnd * i3Timer::GetProfileResolution() );
	//strcat( g_pFramework->GetViewer()->getDebugText(), szTemp );
//}
/*
void CGameCharaPlayer::_ProcessDownEffect( void)
{
	VEC3D vec, vUp;

	i3Vector::Set( &vUp, i3Math::Randf2(), i3Math::Randf()+ 0.1f, i3Math::Randf2());
	i3Vector::Normalize( &vUp, &vUp);

	i3Skeleton * pSkel = m_pBody->getFrameLOD()->getSkeleton();
	i3PhysixShapeSet * pShapeSet = NULL;
	REAL32 force = 0.f;
	REAL32 dot = 0.f;

	if( !m_bContactBones[ 0] )
	{	// Root
		pShapeSet = pSkel->getShapeSet( m_pBoneContext->getBoneIndex( BONE_ROOT) );
		pShapeSet->getLinearVelocity( &vec);

		force = i3Vector::LengthSq( &vec);

		if( force >0.f)
		{
			dot = i3Vector::Dot( &m_vBonesForceDir[ 0], &vec);

			if( dot < 0.3f && force < 0.2f)
			//if( force <= 0.01f && force > 0.f)
			{
				//pShapeSet->getShapeGlobalPose( &mat);
				//g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
				addStateMask( CHARA_STATE_PHYSIXCONTACT);
				m_bContactBones[ 0] = TRUE;
			}

			i3Vector::Copy( &m_vBonesForceDir[ 0], &vec);
		}
	}

	if( !m_bContactBones[ 1] )
	{	// Spine3
		pShapeSet = pSkel->getShapeSet( m_pBoneContext->getBoneIndex( BONE_SPINE3) );
		pShapeSet->getLinearVelocity( &vec);

		force = i3Vector::LengthSq( &vec);

		if( force > 0.f )
		{
			dot = i3Vector::Dot( &m_vBonesForceDir[ 1], &vec);
			if( dot < 0.3f && force < 0.2f)
			{
				//pShapeSet->getShapeGlobalPose( &mat);
				//g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
				addStateMask( CHARA_STATE_PHYSIXCONTACT);
				m_bContactBones[ 1] = TRUE;
			}

			i3Vector::Copy( &m_vBonesForceDir[ 1], &vec);
		}
	}

	
	if( !m_bContactBones[ 2] )
	{	// L forearm
		pShapeSet = pSkel->getShapeSet( m_idxBone[ BONE_LDARM ] );
		pShapeSet->getLinearVelocity( &vec);
		force = i3Vector::LengthSq( &vec);
		if( force <= 0.01f && force > 0.f)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
			m_bContactBones[ 2] = TRUE;
		}
	}

	if( !m_bContactBones[ 3] )
	{	// R forearm
		pShapeSet = pSkel->getShapeSet( m_idxBone[ BONE_RDARM ] );
		pShapeSet->getLinearVelocity( &vec);
		force = i3Vector::LengthSq( &vec);
		if( force <= 0.01f && force > 0.f)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
			m_bContactBones[ 3] = TRUE;
		}
	}

	if( !m_bContactBones[ 4] )
	{
		// R forearm
		pShapeSet = pSkel->getShapeSet( m_idxBone[ BONE_LFOOT ] );
		pShapeSet->getLinearVelocity( &vec);
		force = i3Vector::LengthSq( &vec);
		if( force <= 0.01f && force > 0.f)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
			m_bContactBones[ 4] = TRUE;
		}
	}

	if( !m_bContactBones[ 5] )
	{	// R forearm
		pShapeSet = pSkel->getShapeSet( m_idxBone[ BONE_RFOOT ] );
		pShapeSet->getLinearVelocity( &vec);
		force = i3Vector::LengthSq( &vec);
		if( force <= 0.01f && force > 0.f)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
			m_bContactBones[ 5] = TRUE;
		}
	}
}


#define LOD_SLOPE_VALUE		3.5f

void CGameCharaPlayer::calcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio)
{
	//	ĳ���� LOD
	m_pBody->CalcLOD( pPos, pCamPos, fFOVRatio);

	//	���� LOD
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon != NULL)
	{
		pWeapon->calcLOD( pPos, pCamPos, fFOVRatio);
	}

	// � �� ���� LOD�� ó��
	if( getWeaponContext()->getBackpack() != NULL)
	{
		getWeaponContext()->getBackpack()->calcLOD( pPos, pCamPos, fFOVRatio);
	}
	
	// ���̵� ������ �ް� �ִٸ�
	if( getWeaponContext()->getSidepack() != NULL )
	{
		getWeaponContext()->getSidepack()->calcLOD( pPos, pCamPos, fFOVRatio);
	}

	//	��� LOD
	for( INT32 i = 1; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		EQUIP_CLASS_TYPE type = (EQUIP_CLASS_TYPE) i;
		
		CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);
		if( pEquip != NULL)
		{
			if( m_pEquipContext->isAttachOnChara( type))
			{
				pEquip->calcLOD( pPos, pCamPos, fFOVRatio);
			}
		}
	}	
}

void CGameCharaPlayer::CheckCrossHit( )
{	
	VEC3D vAt;

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL)	return;	//	���Ⱑ ���� ���� �ִ�.

	CWeaponInfo * pInfo = pWeapon->getWeaponInfo();
	if( pInfo == NULL)		return;	//	���Ⱑ ���� ���� �ִ�.

	MATRIX mtx;
	VEC3D pos;
	m_pCameraContext->getVirtualCamPosition( &pos, &mtx);
	i3Vector::Scale( &vAt, i3Matrix::GetAt( &mtx), - 1000.f);

	m_CrossHitLine.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
	i3Vector::Add( &vAt, &vAt, &pos);
	m_CrossHitLine.SetEnd( &vAt );

	//World�� ���ؼ� �ѹ� �׽�Ʈ
	//UINT32 nResultCount = g_pFramework->raycast( &m_CrossHitLine, CGRP_ATTACK);
	//if( nResultCount > 0 )
	
	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &m_CrossHitLine, CGRP_ATTACK);
	if( pResult != NULL)
	{
		//I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->getRaycastResult();

		REAL32	rLength = 10000.f;

		//for( UINT32 i = 0; i < nResultCount; i++ )
		{
			//if( pResult[ i ].m_T < rLength )
			
			{
				i3Physix::HitResultCopyTo( &m_CrossHitResult, pResult );
				//rLength = pResult[ i].m_T;
			}
		}
	}

	// ��ź�� �ٶ󺸰� �ִ��� üũ
	if( g_pGameContext->IsBombMissionMode() && // ���ĸ������
		pMissionMng->getMissionState() == GAME_MISSION_STATE_INSTALLED &&	// ��ġ ������
		pMissionMng->getMyMissionObject() == MY_MISSION_OBJECT_DEFENSIVE )	// ���������
	{
		WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4 );
		if( pWeapon != NULL )
		{
			VEC3D vDir;

			i3Vector::Sub( &vDir, pWeapon->GetPos(), GetPos() );
			i3Vector::Normalize( &vDir, &vDir );

			if( i3Vector::Dot( &vDir, i3Matrix::GetAt( GetMatrix() ) ) > 0.6f )
			{
				g_pGameContext->getMissionManager()->setEnableCrossHit( TRUE);
			}
			else
			{
				g_pGameContext->getMissionManager()->setEnableCrossHit( FALSE);
			}
		}
		else
		{
			g_pGameContext->getMissionManager()->setEnableCrossHit( FALSE);
		}
	}
}


void CGameCharaPlayer::ReturnInstance( void)
{	
	{
		char szTemp[256] = "";
		sprintf( szTemp, "Remove Chara (%s)", g_pGameContext->getNickForSlot(  getCharaInfo()->_NetIdx));
		__MLOG( szTemp);
	}

	//DetachCollisionNode();

	//Destroy();

	CGameCharaBase::ReturnInstance();
}


// AI State�� ����� ������ ȣ��Ǵ� �Լ�.
// ���� �� �ֱ� ������ ����� Optimize�Ǿ�� �Ѵ�.
void CGameCharaPlayer::OnChangeAIState( i3AIContext * pCtx)
{
	i3AIState * pState = pCtx->getCurrentAIState();
	i3Animation * pAnim;
	i3TransformSequence * pTransSeq = NULL;

	I3ASSERT( pState != NULL);

	if( pState->getAnim() == NULL)
	{
		return;
	}

	pAnim = pState->getAnim();

	// ��ü Animation���� ��ü�� �Ͱ� ������ �ȸ��� �� �ִ�.
	// �̰��� �����ϱ� ����, Pivot Bone�� ���ԵǾ� ������,
	// Pivot�� �׻� ����(Z��)���� ���ؾ� �Ѵٴ� ������ �̿���
	// ��ü�� ��Ʋ���� ���� �����Ѵ�.

	m_pBoneContext->setTimeTargetYawBlend( pState->getBlendTime());
	m_pBoneContext->setTimeYawBlend( 0.f);

	// Pivot Keyframe�� �ִ��� Ȯ�� 
	if( i3String::Compare( pAnim->GetTrackBoneName( 0), "IK") == 0)
	{
		//  ��ü�� .
		if( i3String::Compare( pAnim->GetTrackBoneName( pAnim->GetTrackCount() - 1), "Pivot") == 0)
		{
			//pTransSeq = pAnim->GetTrack( 0);
			//pTransSeq->GetKeyMatrix( 0, &m_mtxPivot[0]);

			pTransSeq = pAnim->GetTrack( pAnim->GetTrackCount() - 1);
			pTransSeq->GetKeyMatrix( 0, m_pBoneContext->getPivotMatrix());

			m_pBoneContext->setValidPivot( true);
			return;
		}
		
		m_pBoneContext->setValidPivot( false);
	}


	m_pBoneContext->setPivotYawTarget( 0.f);
}


void CGameCharaPlayer::ChangeFPSView(BOOL bPlayAnimSync)
{
	//--> MaterialContext
	//m_pTexFilterAttr->SetMinFilter(I3G_TEXTURE_FILTER_LINEAR);
	
	ChangeAnimSet(CHARA_ANIMSET_1PV, bPlayAnimSync, FALSE);
	getSoundContext()->SetSoundOwner();
}

void CGameCharaPlayer::ChangeObserverView(BOOL bPlayAnimSync)
{
	ChangeAnimSet(CHARA_ANIMSET_3PV, bPlayAnimSync, FALSE);
	getSoundContext()->SetSoundOwner();
}

void CGameCharaPlayer::SetViewerChara( BOOL bEnable)
{	//	��� �Ǵ� GUI�� 3��Ī ĳ���ͷ� ����	
	if( bEnable)
	{
		m_pBody->removeStyle( I3SG_LOD_AUTO);
		m_pBody->setCurrentLevel( 1);

		//	���� LOD
		for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; i++)
		{
			WeaponBase * pWeapon = getWeaponContext()->getWeapon( i);

			if( pWeapon != NULL)
			{
				i3Body * pBody = pWeapon->GetSceneObject()->GetBody();
				I3ASSERT( pBody != NULL);

				pBody->removeStyle( I3SG_LOD_AUTO);
				pBody->setCurrentLevel( 0);	
			}
		}

		//	��� LOD
		for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
		{
			EQUIP_CLASS_TYPE type = (EQUIP_CLASS_TYPE) i;
			
			CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);
			if( pEquip != NULL)
			{
				i3Body * pBody = pEquip->GetSceneObject()->GetBody();
				I3ASSERT( pBody != NULL);

				pBody->removeStyle( I3SG_LOD_AUTO);
				pBody->setCurrentLevel( 0);	
			}
		}	

		//	�ؽ�ó ��ѱ� ������ ���ƿ�.
		m_pMaterialContext->getTextureFilterAttr()->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
		m_pMaterialContext->getTextureFilterAttr()->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
		//m_pTexFilterAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
		//m_pTexFilterAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		m_pBody->addStyle( I3SG_LOD_AUTO);

		//	���� LOD
		for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; i++)
		{
			WeaponBase * pWeapon = getWeaponContext()->getWeapon( i);

			if( pWeapon != NULL)
			{
				i3Body * pBody = pWeapon->GetSceneObject()->GetBody();
				I3ASSERT( pBody != NULL);

				pBody->addStyle( I3SG_LOD_AUTO);
			}
		}

		//	��� LOD
		for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
		{
			EQUIP_CLASS_TYPE type = (EQUIP_CLASS_TYPE) i;
			
			CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);
			if( pEquip != NULL)
			{
				i3Body * pBody = pEquip->GetSceneObject()->GetBody();
				I3ASSERT( pBody != NULL);

				pBody->addStyle( I3SG_LOD_AUTO);
			}
		}

		m_pMaterialContext->getTextureFilterAttr()->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		m_pMaterialContext->getTextureFilterAttr()->SetMinFilter( I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR);
	//	m_pTexFilterAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
	//	m_pTexFilterAttr->SetMinFilter( I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR);
	}
}


void CGameCharaPlayer::DumpChara( void)
{	
	I3ASSERT( m_pCharaInfo != NULL );
	INT32 nNetIdx = getCharaNetIndex();

	const char * pszName = g_pGameContext->getNickForSlot( nNetIdx);
	if( strlen( pszName) == 0)
		pszName = "null";

	I3NOTICE("---------------------------------------------------");
	I3NOTICE("<< Dump Character >>");	
	I3NOTICE( "- Chara ID : %s (%d)", pszName, nNetIdx );
	I3NOTICE( "- this : 0x%p", this );
	I3NOTICE( "- InvincibleElapseTime = %.5f", m_rInvincibleElapseTime );
	I3NOTICE( "- CharaTask = %d", g_pCharaManager->getCharaTask( nNetIdx ) );
	I3NOTICE( "- HP : %d", m_pCharaInfo->_Hp);

	//	ĳ���� ���� ����
	CGameCharaBase::DumpChara();

	//	���� ���� ����
	if( getWeaponContext()->getCurrent() != NULL)
	{
		getWeaponContext()->getCurrent()->DumpState();
	}
	else
	{
		I3NOTICE( "NO Weapon\n");
	}

	//	���ǰ ����
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		CGameCharaEquip * pEquip = m_pEquipContext->getEquip( (EQUIP_CLASS_TYPE) i);

		if( pEquip != NULL)
			pEquip->DumpState();
	}

	I3NOTICE("---------------------------------------------------");
}
/*

REAL32 CGameCharaPlayer::GetSyncTimeLower( void)
{
	I3ASSERT( m_pBody != NULL);

	REAL32 fSyncTime = 0.0f;
	if( m_pAnimContext->isAILower())
	{
		//	���� AI State�� ������. (�ٲ�� AI�� NextAI�� ��ϵǰ� ���� �����ӿ� ����Ǿ� AI�� ����˴ϴ�.)
		if( m_pAnimContext->getAILower()->getNextAIStateIndex() == -1)
		{
			//	��ü �ִϸ��̼ǰ� �´� Ÿ�� ��ũ�ؾ� �մϴ�.			
			fSyncTime = m_pBody->getPlayTime( m_pBoneContext->getBoneIndex( BONE_PELVIS));
		}
	}	

	return fSyncTime;
}

REAL32 CGameCharaPlayer::GetSyncTimeUpper( void)
{
	I3ASSERT( m_pBody != NULL);

	REAL32 fSyncTime = 0.0f;
	if( m_pAnimContext->isAIUpper())
	{	//	���� AI State�� ������. (�ٲ�� AI�� NextAI�� ��ϵǰ� ���� �����ӿ� ����Ǿ� AI�� ����˴ϴ�.)
		if( m_pAnimContext->getAIUpper()->getNextAIStateIndex() == -1)
		{
			//	��ü �ִϸ��̼ǰ� �´� Ÿ�� ��ũ�ؾ� �մϴ�.			
			fSyncTime = m_pBody->getPlayTime( m_pBoneContext->getBoneIndex( BONE_IK));
		}
	}	

	return fSyncTime;
}

void CGameCharaPlayer::ResetDefaultInfo( void)
{	//	ĳ���� Stat ���� ����

	I3ASSERT( m_pDBInfo != NULL);
	I3ASSERT( m_pCharaInfo != NULL );

	m_pCharaInfo->_Hp = (INT16) m_pDBInfo->m_nHP;				// HP
}

//#define		SLICE		0.033f
//#define		INVINCIBLE_MIN_ALPHA		0.4f


void CGameCharaPlayer::_DriveLightVolume( REAL32 tm)
{	
	m_timeVolLit += tm;

	if( m_timeVolLit < SLICE)
	{
		REAL32 val = i3Math::vsin_libm( m_timeVolLit * (1.0f / SLICE));
		COLORREAL diff, mtl;

		i3Color::Sub( &diff, &m_TargetDiffuse, &m_OldDiffuse);
		i3Color::Scale( &mtl, &diff, val);
		i3Color::Add( &mtl, &mtl, &m_OldDiffuse);

		SetMaterialColor( &mtl, &mtl );
		//m_pMaterialAttr->SetAmbient( bright * 0.8f, bright * 0.8f, bright * 0.8f, 1.0f);
		//m_pMaterialAttr->SetDiffuse( &mtl);
		//m_pMaterialAttr->SetSpecular( &mtl);

		getWeaponContext()->getControl()->SetMaterialColor( getWeaponContext()->getCurrentSlotNum(), &mtl, &mtl);

		EQUIP_SetMaterialColorAll( &mtl, &mtl, TRUE);
	}
	else
	{
		// �ݿ����� Collistion Test�� �Ѵ�. �� Frame���� �ϴ� ���� �ƴ϶�
		// ���������� �Ѵ�.
		i3CollideeSphere sphere;

		VEC3D pos;
		INT32 count, i;
		COLORREAL target;

		i3Vector::Copy( &pos, GetPos());
		i3Vector::AddY( &pos, m_rColRadius * 0.7f );
		sphere.SetCenter( &pos);
		sphere.SetRadius( m_rColRadius * 0.7f);

		//I3TRACE( "%f %f %f %f\n", pos.x, pos.y, pos.z, m_rColRadius);
		
		count = g_pFramework->getCollisionContext()->CollisionTest( &sphere, CGRP_CHARA_LIGHTVOL, 0); //I3_COLLISION_STYLE_NEAREST_ONE);

		I3_COLLISION_RESULT * pResult = g_pFramework->getCollisionContext()->GetResult();

		if( count > 0)
		{
			i3Color::Set( &target, 0.0f, 0.0f, 0.0f, 0.0f);

			for( i = 0; i < count; i++)
			{
				REAL32 r, g, b;

				r = (REAL32)pResult->m_pTri->m_Color[0] * (1.0f / 255.0f) * 2.0f;
				g = (REAL32)pResult->m_pTri->m_Color[1] * (1.0f / 255.0f) * 2.0f;
				b = (REAL32)pResult->m_pTri->m_Color[2] * (1.0f / 255.0f) * 2.0f;

				i3Color::Add( &target, r, g, b, 1.0f);
				pResult ++;
			}

			i3Color::Scale( &target, &target, (1.0f / count));
		}
		else
		{
			CStageBattle * pStage = (CStageBattle *) g_pFramework->GetCurrentStage();

			i3Color::Set( &target, pStage->getDefaultDiffuse());
		}

		{
			m_timeVolLit = 0.0f;
			i3Color::Set( &m_OldDiffuse, &m_TargetDiffuse);
			i3Color::Set( &m_TargetDiffuse, &target);
		}
	}
}
*/

/*
void CGameCharaPlayer::_ProcessMaterial( REAL32 tm)
{	
	REAL32 fAlpha = 1.0f;

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( g_pGameContext->IsBattle() )
	{
		//////////////////////////////////////////////////////////////////////
		//	������ Ÿ��
		if( m_rRespawnElapseTime > 0.0f)	
		{
			if( pMissionMng->getStageType() == STAGE_TYPE_DEATHMATCH )
			{
				//	���� ĳ���ʹ� ������ ������� ȿ��
				I3ASSERT( g_pCharaInfoDataBase->getGlobal()->m_rRespawnTime > I3_EPS );

				fAlpha = m_rRespawnElapseTime / g_pCharaInfoDataBase->getGlobal()->m_rRespawnTime;

				if( !isNetwork() && g_pCamera->IsExactTypeOf( CGameCamera_Kill::GetClassMeta()))
				{	// �ڱ� �ڽ�
					CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
					
					if (pKillCam->isFPS())
					{
						SetZWriteEnable(FALSE);
						SetBlendAlpha(0.0f);
					}
					else
					{
						SetBlendAlpha( fAlpha);
					}
				}
				else
					SetBlendAlpha( fAlpha);

				EQUIP_SetBlendAlphaAll( fAlpha, TRUE);
				getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), fAlpha);

				if( getWeaponContext()->getBackpack() != NULL )
					getWeaponContext()->getBackpack()->SetBlendAlpha( fAlpha);
				if( getWeaponContext()->getSidepack() != NULL )
					getWeaponContext()->getSidepack()->SetBlendAlpha( fAlpha);
			}
		}
		else
		{
			if( isAlive())
			{
				//////////////////////////////////////////////////////////////////////////////
				//	���� Ÿ��
				if( m_rInvincibleElapseTime > 0.0f)
				{
					//	�����ð��ÿ��� �����̴� ȿ��		      
					fAlpha = i3Math::abs( i3Math::sin( m_rInvincibleElapseTime * 6.0f));

					//if( fAlpha < INVINCIBLE_MIN_ALPHA)		fAlpha = INVINCIBLE_MIN_ALPHA + (INVINCIBLE_MIN_ALPHA - fAlpha);
					//if( fAlpha < INVINCIBLE_MIN_ALPHA)			fAlpha = INVINCIBLE_MIN_ALPHA; 

					SetBlendAlpha( fAlpha);

					EQUIP_SetBlendAlphaAll( fAlpha, TRUE);
					getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), fAlpha);
					if( getWeaponContext()->getBackpack() != NULL )
						getWeaponContext()->getBackpack()->SetBlendAlpha( fAlpha);
					if( getWeaponContext()->getSidepack() != NULL )
						getWeaponContext()->getSidepack()->SetBlendAlpha( fAlpha);

					// debug ������ �� ĳ������ HP�� 0�̸� �����غ��ϴ�. by dfly
#if defined( I3_DEBUG)
					if( getCurHP() == 0)
					{
						DumpChara();
					}
#endif
				}
				else
				{
					//	������ �ƴҶ� ���� ���·� ����		
					SetBlendAlpha( 1.0f);
					EQUIP_SetBlendAlphaAll( 1.0f, TRUE);
					getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), 1.0f);
					if( getWeaponContext()->getBackpack() != NULL )
						getWeaponContext()->getBackpack()->SetBlendAlpha( 1.f );
					if( getWeaponContext()->getSidepack() != NULL )
						getWeaponContext()->getSidepack()->SetBlendAlpha( 1.f );
				}
			}
		}
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////////
		//	 ĳ���� Fadeȿ��
		if( m_rFadeElapseTime != 0.0f)
		{
			REAL32 fAlpha = m_rFadeElapseTime / 1.0f;

			I3ASSERT( m_pAnimContext->getAILower() != NULL);
			I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

			if( m_pAnimContext->getAILower()->getCurrentAIState() == NULL || m_pAnimContext->getAIUpper()->getCurrentAIState() == NULL)
				SetVisible( FALSE);
			else
				SetVisible( TRUE);

			SetBlendEnable( TRUE);

			if( m_rFadeElapseTime < 0.0f)
			{
				m_rFadeElapseTime += tm;

				if( m_rFadeElapseTime > 0.0f)	
				{
					m_rFadeElapseTime = 0.0f;
					SetBlendEnable( FALSE);
				}

				fAlpha = i3Math::abs( 1.0f + fAlpha);
			}
			else
			{
				m_rFadeElapseTime -= tm;

				if( m_rFadeElapseTime < 0.0f)	
				{
					m_rFadeElapseTime = 0.0f;
					SetBlendEnable( FALSE);
				}

				fAlpha = i3Math::abs( fAlpha);
			}

			SetBlendAlpha( fAlpha);

			EQUIP_SetBlendAlphaAll( fAlpha, TRUE);
			getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), fAlpha);
		}
	}
}
*/
/*
void CGameCharaPlayer::_ProcessScreenEffect( REAL32 tm)
{
	i3RasterizeChain_Blur * pRasterizeBlur = g_pFramework->GetRasterizeBlur();
	I3ASSERT( pRasterizeBlur != NULL);

	CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);
	CHud * pHud = pStage->getHud();
	
	BOOL	bActiveEffect = FALSE;
	INT32	Tex2DAlpha = 0;
	
	switch( m_nScrEffectType)
	{	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	������ ��ũ�� ����Ʈ
	case SCR_EFFECT_DAMAGE:
		{
			bActiveEffect = TRUE;
			m_rDamageElapseTime -= tm;

			if( m_rDamageElapseTime <= 0.0f)
			{	//	�� ȿ��(?)�� ������ �������.
				if( m_fBlurStart > m_fBlurEnd)
				{			
					m_fBlurStart -= 0.02f;
		
					//	������ ���İ� �������.
					INT32 a = (INT32) i3Color::GetA( &m_EFFECT_ScreenColor);
					a -= 2;
					if( a > 255)	a = 255; 
					else if( a < 0) a = 0;
					i3Color::SetA( &m_EFFECT_ScreenColor, (UINT8) a);

					//I3TRACE( "������ ������..(blur %.4f / alpha %d)\n", m_fBlurStart, i3Color::GetA( &m_EFFECT_ScreenColor));
				}
				else
				{
					bActiveEffect = FALSE;
				
					m_fBlurStart = m_fBlurEnd = 0.0f;
					m_rDamageElapseTime = 0.0f;

					m_nScrEffectType = SCR_EFFECT_NONE;

					//I3TRACE( "������ ����..\n");
				}
			}
			else
			{
				//I3TRACE( "������ ������..(blur %.4f / alpha %d)\n", m_fBlurStart, i3Color::GetA( &m_EFFECT_ScreenColor));
			}
		}
		break;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	���� ����Ʈ �ð� ��� (����ź���� ���)
	case SCR_EFFECT_FLASHBANG:
		{	
			Tex2DAlpha = 150;
			bActiveEffect = TRUE;
			m_rFlashElapseTime -= tm;

			if( m_rFlashElapseTime <= 0.f)
			{	
				Tex2DAlpha = (INT32) i3Color::GetA( &m_EFFECT_ScreenColor);		
				Tex2DAlpha -= (INT32) (tm * 255.0f);
				if( Tex2DAlpha < 0)	Tex2DAlpha = 0;

				//	����Ʈ�� ������ ������ ��������.
				if( Tex2DAlpha <= 0)
				{
					m_rFlashElapseTime = 0.0f;
					m_rFlashEffectiveTime = 0.0f;

					bActiveEffect = FALSE;
					m_nScrEffectType = SCR_EFFECT_NONE;

					//I3TRACE( "�ù� ����..\n");
				}
				else
				{
					i3Color::Set( &m_EFFECT_ScreenColor, 255, 255, 255, (UINT8) Tex2DAlpha);	

					//I3TRACE( "�ù� ������..(alpha %d)\n", Tex2DAlpha);
				}
			}
			else	//	����Ʈ ��
			{
				REAL32 fMinReduceAlpha = 150.0f;
				REAL32 fMinAlpha = 255.0f - fMinReduceAlpha;
				REAL32 fGapTime = m_rFlashEffectiveTime - m_rFlashElapseTime;
				INT32 alpha = 0;

				if( fGapTime <= 0.5f)		//	���ڱ� �Ͼ�ȭ������ ���̵� �ȴ�.
				{
					alpha = (INT32) (fGapTime * 2.0f * 255.f);
					if( alpha > 255) alpha = 255;
				}	
				else if( fGapTime <= 1.0f)	//	�Ͼ�ȭ���� ���ӵȴ�.
				{
					alpha = 255;
				}					 
				else						//	�Ͼ�ȭ���� �ﷷ(?) �Ÿ���.
				{			
					REAL32 fAlpha = i3Math::abs( i3Math::sin( m_rFlashElapseTime)) * fMinReduceAlpha;
					alpha = (INT32) (255.f - fAlpha);
					if( alpha < (INT32) fMinAlpha)		alpha = (INT32) fMinAlpha;
				}

				i3Color::Set( &m_EFFECT_ScreenColor, 255, 255, 255, (UINT8) alpha);	

				//I3TRACE( "�ù� ������..(alpha %d)\n", alpha);
			}			
		}
		break;
	default:
		{
			bActiveEffect = FALSE;

			i3Color::Set( &m_EFFECT_ScreenColor, 0, 0, 0, 0);
			m_fBlurStart		= 0.0f;
			m_fBlurEnd			= 0.0f;
		}
		break;
	};

	//	��Ʋ�߿����� üũ�մϴ�.
	if (g_pFramework->IsBattleStage())
	{
		if(g_pCamera->getCurTarget() == this)
		{
			//	1��Ī ī�޶󿡼��� �۵��� (3��Ī ī�޶�� ��ũ�� ����Ʈ�� ������ ���� �ʴ´�.)
			if( IsCamFPS() || IsCamKill())
			{	
				//	����Ʈ Ȱ��ȭ
				if( bActiveEffect)
				{
					if( m_nScrEffectType == SCR_EFFECT_DAMAGE)
					{
						//pRasterizeBlur->SetEnable( TRUE);
						//pRasterizeBlur->SetBlurAlpha( m_fBlurStart);
						//pRasterizeBlur->SetBlurAlphaMin( m_fBlurEnd);
						pRasterizeBlur->SetEnable( FALSE);					
					}
					else if( m_nScrEffectType == SCR_EFFECT_FLASHBANG)
					{			
						pHud->SetScreenTex2DEnable( 0, TRUE);
						pHud->SetScreenTex2DColor( 0, 255, 255, 255, (UINT8) Tex2DAlpha);
						
						for( INT32 i = 1; i < SCR_BACKBUFFER_COUNT; i++)
						{	//	ù��° ��ũ���� ���� �ȳ�����
							pHud->SetScreenTex2DEnable( i, FALSE);
						}
					}

					pHud->SetScreenEffectEnable( TRUE);
					pHud->SetScreenEffectColor( &m_EFFECT_ScreenColor);	
				}
				//	����Ʈ ��Ȱ��ȭ
				else
				{		
					//	ȭ�� ����Ʈ ��Ȱ��ȭ		
					pHud->SetScreenEffectEnable( FALSE);
					pHud->SetScreenTex2DEnable( FALSE);

					//	�� ��Ȱ��ȭ
					pRasterizeBlur->SetEnable( FALSE);
				} 
			}
			//	3��Ī ī�޶�
			else
			{
				//	ȭ�� ����Ʈ ��Ȱ��ȭ		
				pHud->SetScreenEffectEnable( FALSE);
				pHud->SetScreenTex2DEnable( FALSE);

				//	�� ��Ȱ��ȭ
				pRasterizeBlur->SetEnable( FALSE);
			}
		}
	}
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Process
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void CGameCharaPlayer::ProcessFirst( REAL32 tm)
//{	
//	CGameCharaBase::ProcessFirst( tm);
	//	Add code here
//}

/*
void CGameCharaPlayer::_ProcessRespawn( REAL32 tm)
{
	// Owner ĳ���ʹ� ���� �� KillCam �� �Ⱥ��̰�
	if( is1PV() && g_pCamera->IsExactTypeOf( CGameCamera_Kill::GetClassMeta()))
	{
		CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
		
		if (pKillCam->isFPS())
		{	
			m_pMaterialContext->SetBlendAlpha(0.0f);
			m_pMaterialContext->SetBlendEnable( TRUE);
		}
	}

	//	 ������ Ÿ�̸� üũ	
	if( m_rRespawnElapseTime > 0.0f)
	{	
		m_pMaterialContext->SetBlendEnable( TRUE);

		m_rRespawnElapseTime -= tm;

		//	�������� �ȴ�.
		if( m_rRespawnElapseTime <= 0.0f)
		{
			m_pMaterialContext->SetBlendEnable( FALSE);
			m_rRespawnElapseTime = 0.0f;
			
#if defined( I3_DEBUG )
			if( !g_pConfig->m_bNetworking )
			{	
				g_pCharaManager->RespawnChara( getCharaNetIndex() );
				i3Vector::Set( m_pMoveContext->getMoveDir(), (REAL32)(i3Math::Rand() % 2), 0.f, (REAL32)(i3Math::Rand() % 2) );

				MATRIX mat;
				VEC3D	vDir;
				i3Matrix::Identity( &mat);
				i3Matrix::SetRotateY( &mat, m_pBoneContext->getTheta() );

				i3Vector::Copy( &m_vDebugOriginalPos, GetPos() );
				i3Vector::TransformCoord( &vDir, m_pMoveContext->getMoveDir(), &mat );
				i3Vector::Add( &m_vDebugTargetPos, GetPos(), &vDir );
				m_rDebugElp = 1.f;

				//ACTION_MoveLower();

				if( i3Vector::IsSame( getMoveDir(), &I3_ZEROVECTOR) )
				{
					if(!STATE_IS_LOWER( CHARA_LOWER_IDLE))
						CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);;
				}
				else
				{
					ActionParameter * param = GET_PREPARE_ACTION_PARAM;
					i3Vector::Copy(&param->_dir, getMoveDir());

					CALL_ACTION_COMMAND(ACTION_CMD_RUNLOWER, param);
				}
			}
			else
#endif
			{	// ������ Ÿ�� ���� ������ ��ġ�� �ű�� ĳ���͸� ���ϴ�
				g_pCharaManager->DeathPosition( getCharaNetIndex() );
			}
		}
	}	
}
*/

/*
void CGameCharaPlayer::EQUIP_CheckVisible( void)
{

	if (g_pFramework->IsBattleStage())
	{
		if( g_pCameraManager != NULL && g_pCamera->getCurTarget() == this)
		{
			//	1��Ī ī�޶󿡼��� �۵��� (3��Ī ī�޶�� ��ũ�� ����Ʈ�� ������ ���� �ʴ´�.)
			if( IsCamFPS() || IsCamKill())
			{	//	1��Ī �ϰ�� ���� ���� �ʿ䰡 ����.
				EQUIP_SetVisibleAll( FALSE, TRUE);	
				return;
			}
		}
	}

	EQUIP_SetVisibleAll( TRUE, TRUE);
}

<<<<<<< .mine

void CGameCharaPlayer::ChangeAnimSet( CHARA_ANIMSET_TYPE type, BOOL bAnimSync, BOOL isFixed3PV)
{
	CGameCharaBase::ChangeAnimSet( type, bAnimSync, isFixed3PV);
=======
//void CGameCharaPlayer::ChangeAnimSet( CHARA_ANIMSET_TYPE type, BOOL bAnimSync, BOOL isFixed3PV)
//{
//	CGameCharaBase::ChangeAnimSet( type, bAnimSync, isFixed3PV);
>>>>>>> .r10962
	
	//EQUIP_CheckVisible();
//}

//void CGameCharaPlayer::ChangeAnimSetLowerSync( CHARA_ANIMSET_TYPE type, BOOL isFixed3PV)
//{
//	CGameCharaBase::ChangeAnimSetLowerSync( type, isFixed3PV);

	//EQUIP_CheckVisible();
//}


void CGameCharaPlayer::ProcessMovement( REAL32 tm )
{
	VEC3D	vDist;
	VEC3D	vDistAt;
	VEC3D	vDistRight;
	
	REAL32	fMoveVelocity	= 0.f;
	MATRIX * pCharaMat = GetSceneObject()->GetMatrix();

	i3Vector::Zero( &vDist);

	if( (m_pMoveContext->getMoveDir()->x != 0.0f) || (m_pMoveContext->getMoveDir()->y != 0.0f) || (m_pMoveContext->getMoveDir()->z != 0.0f))
	{
		CHARA_ACTION_BODY	stateBody = getActionContext()->getBody();
		CHARA_ACTION_UPPER	stateUpper = getActionContext()->getUpper();

		//	ĳ���� �ٱ�
		if( getActionContext()->isLower( CHARA_LOWER_MOVE))
		{
			if( m_pMoveContext->getMoveDir()->z > 0.0f)
			{
				if( m_pMoveContext->getMoveDir()->x > 0.0f || m_pMoveContext->getMoveDir()->x < 0.0f)
					fMoveVelocity = getDBInfo()->m_rDiagonalMoveSpeed;	//	�밢������ �̵���
				else
					fMoveVelocity = getDBInfo()->m_rFrontMoveSpeed;		//	������ �̵���
			}
			else if( m_pMoveContext->getMoveDir()->z < 0.0f)
			{
				if( m_pMoveContext->getMoveDir()->x > 0.0f || m_pMoveContext->getMoveDir()->x < 0.0f)
					fMoveVelocity = getDBInfo()->m_rDiagonalMoveSpeed;	//	�밢������ �̵���
				else
					fMoveVelocity = getDBInfo()->m_rBackMoveSpeed;		//	�ڷ� �̵���
			}
			else
				fMoveVelocity = getDBInfo()->m_rSideMoveSpeed;		//	�¿�� �̵���
		}
		//	ĳ���� �ȱ�
		else if( getActionContext()->isLower( CHARA_LOWER_WALK))
		{
			if( m_pMoveContext->getMoveDir()->z > 0.0f)
			{
				if( m_pMoveContext->getMoveDir()->x > 0.0f || m_pMoveContext->getMoveDir()->x < 0.0f)
					fMoveVelocity = getDBInfo()->m_rWalkDiagonalMove;	//	�밢������ �̵���
				else
					fMoveVelocity = getDBInfo()->m_rWalkFrontMove;		//	������ �̵���
			}
			else if( m_pMoveContext->getMoveDir()->z < 0.0f)
			{
				if( m_pMoveContext->getMoveDir()->x > 0.0f || m_pMoveContext->getMoveDir()->x < 0.0f)
					fMoveVelocity = getDBInfo()->m_rWalkDiagonalMove;	//	�밢������ �̵���
				else
					fMoveVelocity = getDBInfo()->m_rWalkBackMove;		//	�ڷ� �̵���
			}
			else
				fMoveVelocity = getDBInfo()->m_rWalkSideMove;		//	�¿�� �̵���
		}

		//	���� ���ۿ� ���� �̵� ��ġ�� ������ �޽��ϴ�.
		{
			WeaponBase * pWeapon = getWeaponContext()->getCurrent();
			I3ASSERT( pWeapon != NULL);
			CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();				
			
			if( stateBody == CHARA_BODY_STAND)			//	�⺻ ���ִ� ���¿���
				fMoveVelocity *= pWeaponInfo->m_rMoveNormalStand * 0.01f;
			else if( stateBody == CHARA_BODY_CROUCH)		//	�⺻ �ɾ��ִ� ���¿���
				fMoveVelocity *= pWeaponInfo->m_rMoveNormalSit  * 0.01f;
			
			if( isStateMask( CHARA_STATE_ZOOM))
			{//	���� �Ǵ� �� �����϶�
				if( stateUpper != CHARA_UPPER_LOADMAGAZINE && stateUpper != CHARA_UPPER_LOADBULLET)	//	���� �������� �����ÿ��� ����ȵǰ�...
					fMoveVelocity *= pWeaponInfo->m_rMoveNormalZoom * 0.01f;
			}

			if( stateUpper == CHARA_UPPER_FIRE ||
				stateUpper == CHARA_UPPER_SECONDARY_FIRE)			//	�߻� �����϶�
				fMoveVelocity *= pWeaponInfo->m_rMoveNormalFire * 0.01f;
		}

		//	�����߿��� �̵������� �����Ҽ� ����.
		if( !getActionContext()->isLower( CHARA_LOWER_JUMP))
		{
			i3Vector::Copy( m_pMoveContext->getMoveAt(), i3Matrix::GetAt( pCharaMat));
			i3Vector::Copy( m_pMoveContext->getMoveRight(), i3Matrix::GetRight( pCharaMat));

			m_pMoveContext->setMaxSpeed( fMoveVelocity);
		}

		i3Vector::Normalize( &vDistAt, m_pMoveContext->getMoveAt());
		i3Vector::Scale( &vDistAt, &vDistAt, m_pMoveContext->getMoveDir()->z);

		i3Vector::Normalize( &vDistRight, m_pMoveContext->getMoveRight());
		i3Vector::Scale( &vDistRight, &vDistRight, m_pMoveContext->getMoveDir()->x);

		i3Vector::Add( &vDist, &vDistAt, &vDistRight);

		i3Vector::Normalize( &vDist, &vDist);
		//i3Vector::Scale( &vDist, &vDist, fMoveVelocity);
	}
	else
	{
		//	�̵����� �ƴϴ�.
		//m_fRunElapseTime = 0.0f;		
		getAnimContext()->setRunElapseTime( 0.f);
		removeStateMask( CHARA_STATE_FOOTSTEPRIGHT);
	}

	m_pMoveContext->setAccel( &vDist);
}



void CGameCharaPlayer::ProcessTimer( REAL32 tm)
{
	//	������ ���� �����ð��� ����� �ش�.
	if( m_rInvincibleElapseTime > 0.0f)
	{
		m_rInvincibleElapseTime -= tm;
		
		//	�������� ���� �����ð� ������ Ű�Է��� Ǯ���ش�. (���� ��ü GUI���� Ű�Է��� �������Ƿ�)
		if( g_pCharaInfoDataBase->getGlobal()->m_rInvincibleTime - 0.5f > m_rInvincibleElapseTime)	
		{	
			EnableControl( TRUE);
		}

		m_pMaterialContext->SetBlendEnable( TRUE);
	}		
	else if( m_rInvincibleElapseTime < 0.0f)		
	{	
		EnableControl( TRUE);
		m_rInvincibleElapseTime = 0.0f;

		m_pMaterialContext->SetBlendEnable( FALSE);
	}

	
	//	���� ������ üũ
	//if( m_fJumpDelayElapsedTime > 0.0f)
	//{
	//	m_fJumpDelayElapsedTime -= tm;
	//}
	//else if( m_fJumpDelayElapsedTime < 0.0f)		m_fJumpDelayElapsedTime = 0.0f;
	
	m_fJumpDelayElapsedTime += tm;
}

void CGameCharaPlayer::_ProcessChangePose( RT_REAL32 rDeltaSeconds)
{
	m_rIKHeight = -0.15f;

	if( isStateMask( CHARA_STATE_CHANGEPOSE) )
	{	// �ڼ� �������̴�.
		REAL32 rDelta	= m_pDBInfo->m_vCamIKHeight.vec[m_nChangeFromState] - m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam];

		if( m_rChangePoseAccTime >= m_rChangePoseTime)
		{// �ڼ� ������ �Ϸ�Ǿ���.
			removeStateMask( CHARA_STATE_CHANGEPOSE);
			//m_bChangePoseEnable = FALSE;
			m_rChangePoseAccTime= 0.0f;
			m_rIKHeight			+= m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam] - m_pDBInfo->m_vCamIKHeight.vec[0];
			return ;
		}

		REAL32 rDiv				= i3Math::div( m_rChangePoseAccTime, m_rChangePoseTime);
		REAL32 rRatio			= MINMAX( 0.0f, ( rDelta > 0.0f) ? i3Math::vsin_libm( rDiv) : i3Math::vcos_libm( rDiv), 1.0f);
		
		REAL32 rHeight			= (rDelta > 0.0f) ? rDelta * ( 1.0f - rRatio) - m_pDBInfo->m_vCamIKHeight.vec[0] : 
									rDelta * (rRatio) - m_pDBInfo->m_vCamIKHeight.vec[0];

		m_rIKHeight				+= m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam] + rHeight;
		m_rChangePoseAccTime	+= rDeltaSeconds;
	}
	else
	{
		m_rIKHeight				+= m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam] - m_pDBInfo->m_vCamIKHeight.vec[0];
	}
}


INT32 CGameCharaPlayer::CalcFallDamage( VEC3D * pTakeOffPos)
{
	I3ASSERT( pTakeOffPos != NULL);
	I3ASSERT( g_pCharaInfoDataBase->getGlobal() != NULL);

	INT32 nDamage = 0;

	VEC3D vLen, vDiffPos;
	REAL32 fTakeoffHeight = 0.f;

	i3Vector::Copy( &vDiffPos, GetPos());
	i3Vector::SetX( &vDiffPos, i3Vector::GetX( pTakeOffPos));
	i3Vector::SetZ( &vDiffPos, i3Vector::GetZ( pTakeOffPos));

	i3Vector::Sub( &vLen, &vDiffPos, pTakeOffPos);		
	fTakeoffHeight = i3Vector::Length( &vLen);		//	������ ���� ���

	//	���� ������ �ּ� ���̺��� ���� ������ ��������
	if( fTakeoffHeight > g_pCharaInfoDataBase->getGlobal()->m_rFallDamageMin)
	{	
		INT32 nMinDamage = g_pCharaInfoDataBase->getGlobal()->m_nFallDamage;	//	�ּ� ������

		//	���� ���� ���̷� ��� (���� ������ = �ּ� ������ + (���� ���� * ���� Slope))
		nDamage = nMinDamage + (INT32) ( fTakeoffHeight * g_pCharaInfoDataBase->getGlobal()->m_rFallDamageSlope);

		if( nMinDamage > nDamage)	nDamage = nMinDamage;
		if( nDamage > 100)			nDamage = 100;

		if( nDamage > 0 && g_pCamera != NULL && is1PV())
		{
			REAL32 TrembAmpRate = ((REAL32)nDamage) / 100.0f;
			g_pCamera->SetTremble( 2.5f + TrembAmpRate, 1.0f, 0.2f, 0.5f);
		}
	}

	return nDamage;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	EQUIPMENT
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaPlayer::EQUIP_SetVisibleAll( BOOL bEnable, BOOL bAttachedOnChara)
{
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		EQUIP_CLASS_TYPE type = (EQUIP_CLASS_TYPE) i;
		
		CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);
		if( pEquip != NULL)
		{
			if( bAttachedOnChara)
				if( !m_pEquipContext->isAttachOnChara( type))		continue;

			pEquip->SetVisible( bEnable);
		}
	}	
}

void CGameCharaPlayer::EQUIP_SetBlendAlphaAll( REAL32 fVal, BOOL bAttachedOnChara)
{	
	//	ĳ���� ���İ��� ������ ��� ������ �޴´�.
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		EQUIP_SetBlendAlpha( (EQUIP_CLASS_TYPE) i, fVal, bAttachedOnChara);
	}
}

BOOL CGameCharaPlayer::EQUIP_SetBlendAlpha( EQUIP_CLASS_TYPE type, REAL32 fVal, BOOL bAttachedOnChara)
{
	if( bAttachedOnChara == TRUE)
		if( m_pEquipContext->isAttachOnChara( type) == FALSE)	
			return FALSE;

	CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);

	if( pEquip != NULL)
	{
		pEquip->SetBlendAlpha( fVal);
		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::EQUIP_SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular, BOOL bAttachedOnChara)
{	
	//	ĳ���� ���İ��� ������ ��� ������ �޴´�.
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		EQUIP_SetMaterialColor( (EQUIP_CLASS_TYPE) i, pfDiffuse, pfSpecular, bAttachedOnChara);
	}
}

BOOL CGameCharaPlayer::EQUIP_SetMaterialColor( EQUIP_CLASS_TYPE type, COLORREAL * pfDiffuse, COLORREAL * pfSpecular, BOOL bAttachedOnChara)
{
	if( bAttachedOnChara == TRUE)
		if( m_pEquipContext->isAttachOnChara( type) == FALSE)	
			return FALSE;

	CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);

	if( pEquip != NULL)
	{
		pEquip->SetMaterialColor( pfDiffuse, pfSpecular);
		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::m_pEquipContext->DropToWorld( EQUIP_CLASS_TYPE equipType, VEC3D * pHitDir )
{
	I3ASSERT( pHitDir != NULL);

	VEC3D vHitDir;
	
	CGameCharaEquip * pEquip = m_pEquipContext->getEquip( equipType );
	if( pEquip != NULL )
	{		
		i3Vector::Scale( &vHitDir, pHitDir, -3.f );
		i3Vector::Add( &vHitDir, 0.f, 3.5f, 0.f );
		
		i3Scene::SetDynamicVelocity( pEquip->GetSceneObject(), NULL, &vHitDir, &vHitDir );
		
		if( ACTION_DropEquip( EQUIP_CLASS_TYPE_HELMET, GetSceneObject()->GetParent(), getParent(), i3Matrix::GetPos( getHeadMatrix()) ) )
		{
			pEquip->setDynamicActiveState( true, false);
			pEquip->SetDynamicState( I3GAME_DYNAMIC_STATE_DYNAMIC, true);
			pEquip->EnableCollisionGroup( FALSE);
		}

		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::EQUIP_setInfo( EQUIP_CLASS_TYPE equipType, CEquipInfo * pInfo)
{
	m_pEquipInfo[ equipType ] = pInfo;
}

CEquipInfo * CGameCharaPlayer::EQUIP_getInfo( EQUIP_CLASS_TYPE equipType)
{
	return (CEquipInfo*) m_pEquipInfo[ equipType];
}

BOOL CGameCharaPlayer::EQUIP_Add( EQUIP_CLASS_TYPE type, INT32 number)
{	
	BOOL ret = FALSE;

	//	Resource
	i3GameResChara * pRes = (i3GameResChara *) g_pCharaManager->QueryEquipResource( type, number);
	I3ASSERT( pRes != NULL);

	//	Create
	CGameCharaEquip * pEquip = CGameCharaEquip::NewObject();	//	���� ĳ���Ϳ� ���ӵȴ�.
	I3ASSERT( pEquip != NULL);
    
	pEquip->Create( pRes, type, number);

	MATRIX mat;
	i3Body * pBody = pEquip->GetSceneObject()->GetBody();
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	i3Matrix::Identity( &mat );
	i3Matrix::SetPos( &mat, (REAL32)(getCharaNetIndex()) * 10.f, PHYSIX_HEIGHT_EQUIP, (REAL32)(type) * 10.f, 1.f );
	for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
	{
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
		if( pShapeSet != NULL )
		{
			pShapeSet->setShapeGlobalPose( &mat );
		}
	}

	pEquip->Reset();

	ret = EQUIP_Set( pEquip);
	if( !ret)
	{
		I3TRACE( "[EQUIP_Add] Warning! Failed to add equipment (type: %d, num: %d)\n", type, number);

		//	�޸� ����
		I3_SAFE_RELEASE( pEquip);
	}

	return ret;
}

BOOL CGameCharaPlayer::EQUIP_Set( CGameCharaEquip * pEquip)
{	//	�������� �����ϰ� ĳ���Ϳ� �����Ѵ�.
	I3ASSERT( pEquip != NULL);

	BOOL ret = FALSE;
	EQUIP_CLASS_TYPE type = pEquip->GetType();

	EQUIP_Delete( type);	//	���� ��� ����

	ret = EQUIP_Attach( pEquip);	//	���� ��� ����
	if( !ret)
	{
		I3TRACE( "[EQUIP_Set] Warning! Failed to set equipment (type: %d, num: %d)\n", pEquip->GetType(), pEquip->GetNumber());
	}

	//	1��Ī �ϰ�� ���� ���� �ʿ䰡 ����.
	if( m_AnimSet == CHARA_ANIMSET_1PV)
	{
		pEquip->SetVisible( FALSE);	
	}
	else
	{
		pEquip->SetVisible( TRUE);	
	}

	return ret;
}

BOOL CGameCharaPlayer::EQUIP_AttachChara( EQUIP_CLASS_TYPE type, BOOL bReset)
{
	CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);

	if( pEquip != NULL)
	{
		if( bReset)	
		{
			pEquip->Reset();
		}

		if( pEquip->GetOwner() != this)
		{
			EQUIP_Detach( type);
			
			//pEquip->SetPos( 0.f, 0.f, 0.f);

			return EQUIP_Attach( pEquip);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::EQUIP_Attach( CGameCharaEquip * pEquip)
{	//	ĳ���Ϳ� Attach�� �Ѵ�.
	i3Node * pAttachBone = NULL;
	INT32 nIdx = (INT32) pEquip->GetType();
	CHARA_EQUIP_PART nEquipPart = CHARA_EQUIP_PART_UNKNOWN;

	I3ASSERT( nIdx < EQUIP_CLASS_TYPE_COUNT );

	if( m_pCharaEquip[ nIdx] != NULL)
	{
		if( m_pCharaEquip[ nIdx]->IsAttached())
		{
			I3FATAL( "Failed! Please detach or delete previous equipmenet. (type: %d)\n", pEquip->GetType());
			return FALSE;
		}
	}

	switch( pEquip->GetType())
	{
	case EQUIP_CLASS_TYPE_HELMET:		//	ĳ���� �Ӹ�
		pAttachBone = m_pDummyHelmet;
		nEquipPart = CHARA_EQUIP_PART_HEAD;
		break;
	default:
		I3FATAL( "Failed! Unknown equipmenet type. (type: %d)\n", pEquip->GetType());
		return FALSE;
	}			

	I3ASSERT( pAttachBone != NULL);

	//	ĳ���Ϳ� ���δ�.
	pEquip->Attach( pAttachBone, this, TRUE);
	pEquip->Reset();
	pEquip->SetEquipPart( nEquipPart);

	m_pCharaEquip[ nIdx] = pEquip;

	return TRUE;
}

CGameCharaEquip * CGameCharaPlayer::EQUIP_Detach( EQUIP_CLASS_TYPE type)
{	//	ĳ���Ϳ��� Detach�� �Ѵ�.
	CGameCharaEquip * pEquip = NULL;
	INT32 nIdx = (INT32) type;

	I3ASSERT( nIdx < EQUIP_CLASS_TYPE_COUNT );

	if( m_pCharaEquip[ nIdx] != NULL)
	{
		if( m_pCharaEquip[ nIdx]->IsAttached() )
		{
			if( m_pCharaEquip[ nIdx ]->GetOwner() == this )
				pEquip = m_pCharaEquip[ nIdx];

			m_pCharaEquip[ nIdx]->Detach();	//	�θ𿡼� �и�
		}
	}

	return pEquip;
}

BOOL CGameCharaPlayer::EQUIP_Delete( EQUIP_CLASS_TYPE type)
{	//	ĳ���Ϳ��� Detach�� �޸� �����Ѵ�.
	INT32 nIdx = (INT32) type;

	I3ASSERT( nIdx < EQUIP_CLASS_TYPE_COUNT );

	if( m_pCharaEquip[ nIdx] != NULL)
	{	
		EQUIP_Detach( type);	//	ĳ���Ϳ��� �����

		//	�޸� ����
		I3_SAFE_RELEASE( m_pCharaEquip[ nIdx]);

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::m_pEquipContext->isAttachOnChara( CGameCharaEquip * pEquip)
{
	if( pEquip != NULL)
	{
		//	���� ĳ���Ϳ� �پ� �ִ��� ����
		if( pEquip->IsAttached() && (pEquip->GetOwner() == this))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CGameCharaPlayer::m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE type)
{
	INT32 nIdx = (INT32) type;

	I3ASSERT( nIdx < EQUIP_CLASS_TYPE_COUNT );

	if( m_pCharaEquip[ nIdx] == NULL)	return FALSE;

	//	���� ĳ���Ϳ� �پ� �ִ��� ����
	if( m_pCharaEquip[ nIdx]->IsAttached() && (m_pCharaEquip[ nIdx]->GetOwner() == this))
	{
		return TRUE;
	}

	return FALSE;
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	WEAPON
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
BOOL CGameCharaPlayer::getWeaponContext()->getControl()->SetBlendAlpha( INT32 nSlot, REAL32 fVal)
{	
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);

	if( pWeapon != NULL)
	{
		pWeapon->SetBlendAlpha( fVal);
		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::WEAPON_SetBlendAlphaAll( REAL32 fVal)
{
	for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; ++i)
	{
		getWeaponContext()->getControl()->SetBlendAlpha( i, fVal);
	}
}

BOOL CGameCharaPlayer::WEAPON_SetMaterialColor( INT32 nSlot, COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);

	if( pWeapon != NULL)
	{
		pWeapon->SetMaterialColor( pfDiffuse, pfSpecular);
		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::WEAPON_SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; ++i)
	{
		WEAPON_SetMaterialColor( i, pfDiffuse, pfSpecular);
	}
}


BOOL CGameCharaPlayer::WEAPON_Select( INT32 nSlot)
{	//	ĳ���Ͱ� ��� �ִ� ���� ���� ��ü

	//--> WeaponControl		dfly	20080130
	WeaponBase * pSwapWeapon = getWeaponContext()->getWeapon( nSlot );
	if( pSwapWeapon != NULL )
	{
		// ���� ����� �����.
		WEAPON_Detach( m_nCurWeaponSlotNum );

		//	�ٲ� ����� ���δ�.
		WEAPON_Attach( nSlot, getWeaponAttachRoot(), this );

		m_nCurWeaponSlotNum = m_nSwapWeaponSlotNum = nSlot;

		ApplyCrosshair();

		{
			VEC3D vec;

			pSwapWeapon->SetVisible( TRUE);
			i3Vector::Zero( &vec );
			pSwapWeapon->SetPos( &vec );
		}

		return TRUE;
	}
	return FALSE;
}

void CGameCharaPlayer::WEAPON_Attach( INT32 nSlot, i3Node * pSceneNode, i3GameNode * pGameNode )
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot );

	if( pWeapon != NULL )
	{	
		pWeapon->SetVisible( TRUE);

		pWeapon->ChangeGunDummy( WEAPON_ATTACH_RIGHT_GRIP);	//	�⺻ Root�� ����
		pWeapon->AttachNode( pSceneNode, pGameNode );

		if( is1PV())
		{
			// 1��Ī�� ����� Shadow�� ������ �ʵ��� �����Ǿ� �ִ�.
			pWeapon->GetSceneObject()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
		}

		ApplyCrosshair();
	}
}

void CGameCharaPlayer::WEAPON_Detach( INT32 nSlot )
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot );

	if( pWeapon != NULL )
	{
		pWeapon->DeatchNode();

		if( is1PV())
		{
			// 1��Ī�� ����� Shadow�� ������ �ʵ��� �����Ǿ� �ִ�.
			pWeapon->GetSceneObject()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
		}
	}
}

void CGameCharaPlayer::WEAPON_Delete( INT32 nSlot)
{	
	//--> WeaponControl		dfly	20080130
	
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);

	if( pWeapon != NULL )
	{
		if( nSlot == m_nCurWeaponSlotNum)
		{// ���� ��� �ִ� ������ ������մϴ�.
			WEAPON_Detach( nSlot );
		}

		// � �Ű� �ִٸ� �����.
		if( pWeapon == m_pPurchasePrimWeapon )
		{
			WEAPON_DetachFromBackpack();
		}
		// �������� �ִٸ� �����
		else if( pWeapon == m_pPurchaseSideWeapon )
		{
			WEAPON_DetachFromSidepack();
		}

		//	���� �Ŵ������� ���⸦ �����ϵ��� �Ѵ�.
		g_pWeaponManager->RemoveCharaWeapon( m_pWeapon[nSlot]);
		m_pWeapon[ nSlot] = NULL;
	}
}

//��� ������ �ٽ� �غ��ô�. �� �������ִ°Ŷ�. ���̴°Ŷ�..�������
BOOL CGameCharaPlayer::WEAPON_Add(INT32 nSlot, WEAPON_CLASS_TYPE classType, INT32 number)
{	
	//--> WeaponControl		dfly	20080130
	I3ASSERT( number > 0 && (INT32) classType > 0 && (INT32) classType < WEAPON_CLASS_COUNT);
	I3ASSERT( nSlot < MAX_ITEMSLOT_COUNT );

	//	�˼� ���� ������ ��� ����
	//if( number <= 0 || (INT32) classType <= 0)	return FALSE;

	//	���� �Ŵ����� ���� Instance�� �޾ƿ´�.
	if( m_pWeapon[nSlot] == NULL)
	{
		m_pWeapon[nSlot] = g_pWeaponManager->QueryWeapon( this, classType, number);
	}
	else
	{
		CWeaponInfo * pInfo = m_pWeapon[nSlot]->getWeaponInfo();
			
		//	���� �ٸ� ������ ��� �ٽ� �޾ƿ´�.
		if( pInfo->m_TypeClass == classType)
		{
			if( pInfo->m_nNumber != number)
			{
				WEAPON_Delete( nSlot);

				m_pWeapon[ nSlot] = g_pWeaponManager->QueryWeapon( this, classType, number);
			}
		}
		else
		{
			WEAPON_Delete( nSlot);

			m_pWeapon[ nSlot] = g_pWeaponManager->QueryWeapon( this, classType, number);
		}
	}

	I3ASSERT( m_pWeapon[nSlot] != NULL);

	// ���� �� ���߿� ����ݴϴ�.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)(getCharaNetIndex()) * 10.f, PHYSIX_HEIGHT_WEAPON, (REAL32)(nSlot) * 10.f );
	g_pWeaponManager->MoveShapePosition( m_pWeapon[nSlot], &vec );

	m_pWeapon[nSlot]->SetOwner( this);
	m_pWeapon[nSlot]->Reset();

	// ���� ĳ���ʹ� ������ �ٿ��ڽ� ����
	if( is1PV() )
	{
		m_pWeapon[ nSlot ]->ChangeFPSView();
	}

	return TRUE;
}


BOOL CGameCharaPlayer::WEAPON_AttachToBackpack( INT32 nSlot)
{	
	//--> WeaponControl		dfly	20080130
	
	WeaponBase * pAttachWeapon = getWeaponContext()->getWeapon( nSlot);
	if( pAttachWeapon == NULL )
		return FALSE;

	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	if( pCurWeapon == NULL)
		return FALSE;

	//	���� ���⸦ ��� �����Ƿ� ��� �����.
	if( pCurWeapon == m_pPurchasePrimWeapon)
	{
		WEAPON_DetachFromBackpack();
	}
		
	//	���Ⱑ �ֹ���� ��¦�� ���δ� (� �Ǵ�)
	if( pAttachWeapon->getWeaponInfo()->m_TypeUsage == WEAPON_USAGE_PRIMARY)
	{
		I3ASSERT( m_pDummyPrimPurchase != NULL);
		pAttachWeapon->SetBlendAlpha( 1.f );
		m_pPurchasePrimWeapon = pAttachWeapon;
		WEAPON_Attach( nSlot, m_pDummyPrimPurchase, this);
		//m_pPurchasePrimWeapon->AttachNode( m_pDummyPrimPurchase, this);

		return TRUE;
	}
	else
	{
		if( m_pPurchasePrimWeapon != NULL)
		{
			if( m_pPurchasePrimWeapon->getWeaponInfo() != NULL )
			{
				//	�Ⱥپ� ������ �ٽ� ���δ�.
				if( !m_pPurchasePrimWeapon->IsAttachedScene())
					m_pPurchasePrimWeapon->AttachNode( m_pDummyPrimPurchase, this);
			}
			else
			{// �ش� ����� ���� ����̴�.(���忡 ���� ���)
				m_pPurchasePrimWeapon = NULL;
			}
		}
	}
	

	return FALSE;
}


void CGameCharaPlayer::WEAPON_DetachFromBackpack()
{
	//--> WeaponControl		dfly		20080130
	
	if(  m_pPurchasePrimWeapon != NULL )
	{
		if( m_pPurchasePrimWeapon->getParentScene() == m_pDummyPrimPurchase)
			m_pPurchasePrimWeapon->DeatchNode();

		m_pPurchasePrimWeapon = NULL;
	}
}

BOOL CGameCharaPlayer::WEAPON_AttachToSidepack( INT32 nSlot)
{
	//--> WeaponControl		dfly	20080130
	WeaponBase * pAttachWeapon = getWeaponContext()->getWeapon( nSlot);
	if( pAttachWeapon == NULL)
		return FALSE;

	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	if( pCurWeapon == NULL)
		return FALSE;

	//	���� ���⸦ ��� �����Ƿ� �����.
	if( pCurWeapon == m_pPurchaseSideWeapon)
	{
		WEAPON_DetachFromSidepack();
	}
		
	//	���Ⱑ �����ֹ���� ���δ� 
	if( pAttachWeapon->getWeaponInfo()->m_TypeUsage == WEAPON_USAGE_SECONDARY )
	{
		I3ASSERT( m_pDummySidePurchase != NULL);
		pAttachWeapon->SetBlendAlpha( 1.f );
		m_pPurchaseSideWeapon = pAttachWeapon;
		WEAPON_Attach( nSlot, m_pDummySidePurchase, this);

		return TRUE;
	}
	else
	{
		if( m_pPurchaseSideWeapon != NULL)
		{
			if( m_pPurchaseSideWeapon->getWeaponInfo() != NULL )
			{
				//	�Ⱥپ� ������ �ٽ� ���δ�.
				if( !m_pPurchaseSideWeapon->IsAttachedScene())
					m_pPurchaseSideWeapon->AttachNode( m_pDummySidePurchase, this);
			}
			else
			{// �ش� ����� ���� ����̴�.(���忡 ���� ���)
				m_pPurchaseSideWeapon = NULL;
			}
		}
	}

	return FALSE;
}


void CGameCharaPlayer::WEAPON_DetachFromSidepack( void)
{
	//--> WeaponControl		dfly		20080130
	
	if(  m_pPurchaseSideWeapon != NULL )
	{
		if( m_pPurchaseSideWeapon->getParentScene() == m_pDummySidePurchase)
			m_pPurchaseSideWeapon->DeatchNode();

		m_pPurchaseSideWeapon = NULL;
	}
}



BOOL CGameCharaPlayer::WEAPON_IsSame( WeaponBase * pSrc, WeaponBase * pDst)
{
	if( pSrc == NULL || pDst == NULL) return FALSE;

	CWeaponInfo * pSrcWeaponInfo = pSrc->getWeaponInfo();
	CWeaponInfo * pDstWeaponInfo = pDst->getWeaponInfo();

	//	���� ������ �������� �˻�
	return ( pSrcWeaponInfo->m_TypeClass == pDstWeaponInfo->m_TypeClass &&
			 pSrcWeaponInfo->m_nNumber == pDstWeaponInfo->m_nNumber);
}


BOOL CGameCharaPlayer::WEAPON_DropToWorld( INT32 nSlot, VEC3D * pPos)
{
	//--> WeaponControl		dfly	20080130
	//	SceneNode -> World Scene Node
	//	GameNode -> World Game Node
	
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);
	if( pWeapon != NULL)
	{
		// ������ Extension ���¸� �ʱ�ȭ���ش�.
		pWeapon->setExtensionActivate( FALSE );
		pWeapon->ResetEmptyCatridgeSound();
		pWeapon->GetSceneObject()->GetBody()->StopAnim();
		pWeapon->ReturnToReady();
		//	����(ĳ���� �θ�)�� �ٴ´�.
		pWeapon->Transmission( GetSceneObject()->GetParent(), this->getParent(), pPos, TRUE);

		//	�������� ���� �Ŵ����� �����̸� �����ȴ�. �ش� ĳ���ʹ� �������� ����.
		g_pWeaponManager->MoveWeaponCharaToDrop( pWeapon);	

		//	������, ���� ĳ���ͷν� �����ڸ� ������ �� �ִ�.
		pWeapon->SetOwner( this);

		m_pWeapon[ nSlot] = NULL;

		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::WEAPON_Take( WeaponBase * pWeapon )
{
	INT32	nWeaponSlot = 0;
	I3ASSERT( pWeapon != NULL );

	switch( pWeapon->getWeaponInfo()->m_TypeUsage )
	{
	case WEAPON_USAGE_UNKNOWN	:	break;
	case WEAPON_USAGE_PRIMARY	:	nWeaponSlot = 0; break;
	case WEAPON_USAGE_SECONDARY	:	nWeaponSlot = 1; break;
	case WEAPON_USAGE_MELEE		:
	case WEAPON_USAGE_THROWING	:
	case WEAPON_USAGE_ITEM		:
	case WEAPON_USAGE_MISSION	:
	default :
		I3ASSERT_0;
		break;
	}

	//	���ο� ���⸦ �����ϱ� ���ؼ��� �ش� ���� ������ �ݵ�� ��� �־�� �մϴ�. ���⼭ �ɸ��� ������...komet
	if( getWeaponContext()->getWeapon( nWeaponSlot ) != NULL )
	{
		return;
	}


	{
		I3TRACE( "[TakeWeapon] chara : %d, id : %d, idx : %d\n", getCharaNetIndex(), pWeapon->getNetworkID(), pWeapon->getIdx() );

		pWeapon->EnableCollisionGroup( TRUE );		// Raycast������ ��ȯ

		// Ageia ���⸦ Kinematic���� �����ϱ����� �ø����� �ȵǵ��� ����(������ �־�Դϴ�.)
		VEC3D vec;

		CWeaponInfo *pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);
		i3Vector::Set( &vec, (REAL32)(getCharaNetIndex()) * 10.f, PHYSIX_HEIGHT_WEAPON, (REAL32)(pWeaponInfo->m_TypeClass) * 1.f );
		g_pWeaponManager->MoveShapePosition( pWeapon, &vec );

		pWeapon->setDynamicActiveState( true, false );
		pWeapon->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true );

		if( pWeapon->getContactHitBox() != NULL)
		{
			pWeapon->getContactHitBox()->modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION, 0);
		}

		pWeapon->DeatchNode();
		pWeapon->SetOwner( this);
		pWeapon->setIdx( -1 );
		WEAPON_set( nWeaponSlot, pWeapon);
		//m_pWeapon[ nWeaponSlot ] = pWeapon;

		// 1��Ī �� ĳ���Ͷ��
		if( is1PV())
		{	// LOD�� �����ϰ� �ٿ�� �ڽ��� �����Ѵ�.
			getWeaponContext()->getWeapon( nWeaponSlot )->ChangeFPSView();
		}

		if( g_pGameContext->IsBattle() )
		{
			MATRIX mat;
			i3Matrix::Identity( &mat );
			pWeapon->GetSceneObject()->SetMatrix( &mat );
			pWeapon->GetSceneObject()->AddFlag( I3_NODEFLAG_MANUAL_UPDATE, TRUE );
			if( pWeapon->isExtensionActivate() )
				pWeapon->SetVisibleExtension( TRUE );
		}

		//	���忡�� ĳ���ͷ� �ű��.
		pWeapon->Transmission( GetSceneObject(), this, NULL, FALSE);			// SceneGraph ��� ����, ĳ���� ���� ���Դϴ�.
		pWeapon->ReturnToReady();
		pWeapon->ChangeGunDummy( WEAPON_ATTACH_RIGHT_GRIP);						// �⺻ ������ �׸�
		WEAPON_Attach( m_nCurWeaponSlotNum, getWeaponAttachRoot(), this );		// ĳ���Ϳ� GameNode�� ���Դϴ�.
		g_pWeaponManager->MoveWeaponDropToChara( pWeapon);
		
		// 
		if( (nWeaponSlot == 0) || (m_pWeapon[ 0 ] == NULL) )
		{	// �ֹ��Ⱑ ��� �ְų� �ֹ���� ���� ���� �����ݴϴ�.
			ACTION_SwapWeapon( nWeaponSlot );

			ACTION_apply( !is1PV());
		}
		else if( nWeaponSlot == 1 )
		{	
			// �������⸦ ������ ��� 3��Ī ���� �������� �ٿ��ݴϴ�.
			WEAPON_AttachToSidepack( 1 );

			if( is1PV())
			{	
				pWeapon->GetSceneObject()->GetBody()->StopAnim();
				pWeapon->SetVisible( FALSE);//>GetSceneObject()->AddFlag( I3_NODEFLAG_NO_VISABLE);
				//pWeapon->GetSceneObject()->AddFlag( I3_NODEFLAG_DISABLE );
			}
		}
	}
}

void CGameCharaPlayer::WEAPON_CheckTaking( REAL32 rDeltaTime )
{
	INT32	i;
	VEC3D	vDir;
	REAL32	rLength;
	WeaponBase * pWeapon;

	i3CollideeLine ColLine;

	if( g_pWeaponManager == NULL )	return;

	if( !isAlive() ) return;

	if( g_pWeaponManager->GetDropListCount() == 0 ) return;

	m_rDropedWeaponCheckTime += rDeltaTime;
	if( m_rDropedWeaponCheckTime > WEAPON_CHECKTAKING_TIME )
		m_rDropedWeaponCheckTime = 0.f;
	else
		return;

	ColLine.SetStart( i3Matrix::GetPos( getIKMatrix() ) );

	// �ֹ��� ������ ����ִ�. üũ
	if( m_pWeapon[ 0 ] == NULL )
	{	
		for( i = 0; i < g_pWeaponManager->GetDropListCount(); i++ )
		{	
			pWeapon = g_pWeaponManager->GetDropWeapon( i );
			if( pWeapon->IsDroped() && pWeapon->getWeaponInfo()->m_TypeUsage == WEAPON_USAGE_PRIMARY)
			{
				if( pWeapon->getTotalBulletCount() > 0 || pWeapon->getLoadedBulletCount() > 0 || pWeapon->getLoadedBulletDualCount() > 0 )
				{	
					i3Vector::Sub( &vDir, pWeapon->GetPos(), GetPos() );//i3Matrix::GetPos( getIKMatrix() ) );
					rLength = i3Vector::LengthSq( &vDir );
					if( rLength < 1.f )//2.25f )
					{	
						// Pickup weapon
						g_pGameContext->WriteMyTakeWeapon( pWeapon, pWeapon->getIdx(), 0 );
						break;
					}
				}
			}
		}
	}

	// ���� ���� ������ ����ִ�. üũ
	if( m_pWeapon[ 1 ] == NULL )
	{
		for( i = 0; i < g_pWeaponManager->GetDropListCount(); i++ )
		{
			pWeapon = g_pWeaponManager->GetDropWeapon( i );
			if( pWeapon->IsDroped() && pWeapon->getWeaponInfo()->m_TypeUsage == WEAPON_USAGE_SECONDARY)
			{
				if( pWeapon->getTotalBulletCount() > 0 || pWeapon->getLoadedBulletCount() > 0 )
				{
					i3Vector::Sub( &vDir, pWeapon->GetPos(), GetPos() );//i3Matrix::GetPos( getIKMatrix() ) );
					rLength = i3Vector::LengthSq( &vDir );
					if( rLength < 1.f ) //2.25f )
					{	
						// Pickup weapon
						g_pGameContext->WriteMyTakeWeapon( pWeapon, pWeapon->getIdx(), 1 );
						break;
					}
				}
			}
		}
	}
}


void CGameCharaPlayer::WEAPON_validCheck( WEAPON_CLASS_TYPE nClassType, INT32 WeaponNum )
{
	BOOL bCheck = FALSE;

	INT32 nSlot = 0;
	switch( nClassType )
	{
	case WEAPON_CLASS_KNIFE : nSlot = 2; break;
	case WEAPON_CLASS_HANDGUN : nSlot = 1; break;
	case WEAPON_CLASS_ASSAULT :
	case WEAPON_CLASS_SMG :
	case WEAPON_CLASS_SNIPER :
	case WEAPON_CLASS_SHOTGUN :
	case WEAPON_CLASS_MG :
		nSlot = 0; break;
	case WEAPON_CLASS_THROWING : nSlot = 3;
		if( WeaponNum > 1 )		// ����ũ, �ù��� ������ Ÿ�������� ������ ���������� ��� 4�� �������� ���
			nSlot = 4;
		break;
	case WEAPON_CLASS_ITEM :	nSlot = 4; break;
	case WEAPON_CLASS_MISSION : nSlot = 5; break;
	default :
		I3TRACE( "invalid weapon.\n" );
		return;
	}

	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot );
	
	if( pWeapon != NULL )
	{	
		if( pWeapon->getWeaponInfo()->m_TypeClass != nClassType || pWeapon->getWeaponInfo()->m_nNumber != WeaponNum )
		{
			bCheck = TRUE;
			//pWeapon->ReturnInstance();
			WEAPON_Delete( nSlot );
			//
			I3TRACE( "wrong weapon. recreate.\n" );
		}
	}
	else
	{
		bCheck = TRUE;
	}

	if( bCheck )
	{	
		WEAPON_Add( nSlot, nClassType, WeaponNum );
		WEAPON_Select( nSlot );
	}
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CAMERA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void CGameCharaPlayer::_CAMERA_UpdateState(void)
{
	CHARA_STATE_FOR_CAM nNewTargetState = CSFC_UNKNOWN;

	if( getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK))
	{
		if( getActionContext()->isBody( CHARA_BODY_STAND))	//	�� ������
		{
			nNewTargetState = CSFC_STAND;
		}
		else if( getActionContext()->isBody( CHARA_BODY_CROUCH))	//	�ɾ� ������
		{
			nNewTargetState = CSFC_SITMOVE;
		}
	}
	else if( getActionContext()->isLower( CHARA_LOWER_JUMP))
	{
		nNewTargetState = CSFC_JUMP;
	}
	else
	{
		if( getActionContext()->isBody( CHARA_BODY_STAND))	//	�� ������
		{
			nNewTargetState = CSFC_STAND;
		}
		else if( getActionContext()->isBody( CHARA_BODY_CROUCH))	//	�ɾ� ������
		{
			nNewTargetState = CSFC_SIT;
		}
	}

	if( nNewTargetState != m_StateForCam)
	{
		//�ڼ� ������� �Ҹ�
		if( (nNewTargetState == CSFC_STAND) && (m_StateForCam == CSFC_SIT))
		{
			INT32 nRand = i3Math::Rand()%2;

			g_pSndMng->StartCharacterActionSnd( GTBCA_ON_STANDUP, nRand, GetPos(), is1PV());
		}
		else if( (m_StateForCam == CSFC_STAND) && (nNewTargetState == CSFC_SIT))
		{
			INT32 nRand = i3Math::Rand()%2;

			g_pSndMng->StartCharacterActionSnd( GTBCA_ON_SITDOWN, nRand, GetPos(), is1PV());
		}


		BOOL bTrue1PV = FALSE;
		if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{	
			CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
			if( pCharacter == this ) bTrue1PV = TRUE;
		}
		
		if( bTrue1PV )
		{
			if((m_StateForCam == CSFC_STAND) && (nNewTargetState != CSFC_JUMP))
			{
 				_setStandOn1PV( nNewTargetState, 0.1f);
			}
			else
			{
				_setStandOn1PV( nNewTargetState, 0.3f);
			}
		}
		else
		{
			if( is1PV() )
			{
				if((g_pCamera != NULL) && (nNewTargetState != CSFC_JUMP) && m_StateForCam != CSFC_JUMP)
				{
					g_pCamera->SetInterPolate( 0.2f);
				}
			}
		}
	}
	else
	{

	}

	m_StateForCam = nNewTargetState;

	I3ASSERT( m_StateForCam != CSFC_UNKNOWN);
}

void CGameCharaPlayer::_CAMERA_SetIdle( void)
{
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pWeapon != NULL);

	BOOL bViewer = FALSE;
	if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bViewer = TRUE;
	}	
	else if( g_pCamera->IsExactTypeOf(CGameCamera_Kill::GetClassMeta()))
	{
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bViewer = TRUE;
	}
	
	SetVisible( TRUE);
	pWeapon->SetVisible( TRUE);
	pWeapon->setZoom( false);

	if( bViewer )
	{
		//	�Ϲ� ī�޶� ����
		g_pCamera->SetDefaultFOV( 0.15f);			

		CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		I3ASSERT( pStage != NULL);

		CHud * pHud = pStage->getHud();

		if( m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel > 0)
		{
			if( pHud->IsEnabledSniperScope())
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_SCOPE, 2, GetPos(), is1PV());
			}
			else
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_ZOOM, 2, GetPos(), is1PV());
			}
		}

		pHud->SetSniperScope( FALSE);

		m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel = 0;
	}

	removeStateMask( CHARA_STATE_ZOOM );

	ApplyCrosshair();
}

void CGameCharaPlayer::_CAMERA_SetZoom( void)
{
	BOOL bViewer = FALSE;
	if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bViewer = TRUE;
	}	

	if( bViewer )
	{
		//	���� ī�޶� ����
		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		I3ASSERT( pWeapon != NULL);

		if( g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{
			g_pCamera->ZoomIn( pWeapon->getWeaponInfo()->m_rZoomFOV, 0.15f);		
		}
		else
		{
			g_pCamera->ZoomIn( pWeapon->getWeaponInfo()->m_rZoomFOV3PV, 0.15f);		
		}

		m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel = 1;

		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_ZOOM, 0, GetPos(), is1PV());
	}
}

BOOL CGameCharaPlayer::_CAMERA_SetScope( INT32 nScopeLevel)
{	
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL );

	BOOL bViewer = FALSE;
	if( g_pCamera != NULL && g_pCamera->IsExactTypeOf( CGameCamera_FPS::GetClassMeta()) )
	{	
		//	Zoom�� ĳ���� �Ȱ� �ѱ��� ȭ�鿡 ���������� ���� �Ⱥ��̰� �ϱ� ���� Visible Off ��Ų��.
		//	Scope �����̸� ȭ�� HUD�� ���̵��� ĳ���Ϳ� ���� �𵨸��� Scene�� off, DynamicState�� On �Ǿ�� �Ѵ�.
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this )
		{
			bViewer = TRUE;
			SetVisible( FALSE);
		}
		else
		{
			SetVisible(TRUE);
		}
	}
	else
	{
		SetVisible(TRUE);
	}

	pWeapon->setZoom( true);
	
	//	�������� ī�޶� ���� (������ �ܰ谡 ������ �����̸�)
	if( nScopeLevel <= pWeaponInfo->m_nMaxScopeLevel)
	{
		if( bViewer )
		{
			REAL32 fScopeFOV = 0.0f;
			switch( nScopeLevel)
			{
			case 1:		fScopeFOV = pWeaponInfo->m_rScopeFOV;		break;		//	1�� Scope
			case 2:		fScopeFOV = pWeaponInfo->m_rScopeFOV2;		break;		//	2�� Scope
			default:	I3ASSERT_0;	//	������ �̻��� Scope�� �Ҽ� ����.
			}	

			g_pCamera->ZoomIn( fScopeFOV, 0.15f);

			CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
			I3ASSERT( pStage != NULL);

			CHud * pHud = pStage->getHud();
			pHud->SetSniperScope( TRUE, pWeaponInfo);

			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_SCOPE, 0, GetPos(), is1PV());
		}

		m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel = nScopeLevel;
		//I3TRACE("ScopeLevel : %d\n", m_nCurScopeLevel);

		return TRUE;
	}

	return FALSE;
}
*/

/*
BOOL CGameCharaPlayer::EFFECT_IsAvailable( void)
{	//	���� ��ũ�� ����Ʈ�� ������ �������� ����
	if (g_pFramework->IsBattleStage())
	{				
		if( g_pCameraManager != NULL && g_pCamera->getCurTarget() == this)
		{
			if( IsCamFPS() || IsCamKill())
				return TRUE;
		}
	}

	return FALSE;
}

void CGameCharaPlayer::EFFECT_Clear( void)
{
	i3Color::Set( &m_EFFECT_ScreenColor, 0, 0, 0, 0);
	m_fBlurStart		= 0.0f;
	m_fBlurEnd			= 0.0f;

	m_rFlashElapseTime = m_rFlashEffectiveTime = 0.0f;
	m_rDamageElapseTime = 0.0f;

	m_nScrEffectType	= SCR_EFFECT_NONE;

	if (FALSE == g_pFramework->IsBattleStage()) return;
	
	//	ȭ�� ����Ʈ ��Ȱ��ȭ	
	CStageBattle	* pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);

	CHud * pHud = pStage->getHud();
	if( pHud == NULL)
		return;

	i3RasterizeChain_Blur * pRasterizeBlur = g_pFramework->GetRasterizeBlur();
	I3ASSERT( pRasterizeBlur != NULL);

	pHud->SetScreenEffectEnable( FALSE);
	pHud->SetScreenTex2DEnable( FALSE);

	pRasterizeBlur->SetEnable( FALSE);
}

BOOL CGameCharaPlayer::EFFECT_FlashBang( WeaponBase * pWeapon, REAL32 rDamagedLength)
{
	I3ASSERT( pWeapon != NULL);

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);

	return EFFECT_FlashBang( pWeapon->GetPos(),
							pWeaponInfo->m_rEffectiveTime,
							pWeaponInfo->m_rExplosion_Range,
							pWeaponInfo->m_rExplosion_EffectiveSlope,
							rDamagedLength);
}

BOOL CGameCharaPlayer::EFFECT_FlashBang( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength)
{
	I3ASSERT( pDamageInfo != NULL);
	
	return EFFECT_FlashBang( pDamageInfo->_pVecPos,
							pDamageInfo->_rEffectiveTime,
							pDamageInfo->_rExplosionRange,
							pDamageInfo->_rExplosionSlope,
							rDamagedLength);
}

BOOL CGameCharaPlayer::EFFECT_FlashBang( VEC3D * pVecWeaponPos, REAL32 rEffectiveTime, REAL32 rExplosionRange, REAL32 rExplosionSlope, REAL32 rDamagedLength)
{
	I3ASSERT( pVecWeaponPos != NULL);

	CStageBattle	* pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);

	CHud * pHud = pStage->getHud();

	if( rEffectiveTime == 0.0f) return FALSE;

	i3Color::Set( &m_EFFECT_ScreenColor, 255, 255, 255, 255);

	if (g_pFramework->IsBattleStage())
	{
		if( IsCamFPS() && g_pCamera->getCurTarget() == this)
		{
			for( INT32 i = 0; i < SCR_BACKBUFFER_COUNT; i++)
				pHud->SetCurrentScreenImage( i, 255, 255, 255, 255);
		}
	}
	
	// �Ÿ��� ���� ȿ�� ������( 1.0f�� Default�Դϴ�.)
	REAL32  rValidRate	= 1.0f - ((1.0f / rExplosionRange) * rDamagedLength) * rExplosionSlope;
	if( rValidRate < I3_EPS)	rValidRate = I3_EPS;

	//	�ٶ󺸴� ������ ���� ������ �޴´�.
	{					
		VEC3D vCharaDir, vFlashDir;					
		i3Vector::Copy( &vCharaDir, i3Matrix::GetAt( GetMatrix()));
		i3Vector::Sub( &vFlashDir, pVecWeaponPos, GetPos());
		i3Vector::Normalize( &vFlashDir, &vFlashDir);

		REAL32 fCharaFlashAngle = i3Vector::Dot( &vCharaDir, &vFlashDir);
		fCharaFlashAngle = I3_RAD2DEG( fCharaFlashAngle);
		if( fCharaFlashAngle <= 0.0f)
			fCharaFlashAngle = i3Math::abs( fCharaFlashAngle) + 90.f; 					
							
		//	������ ���� �޴� ���� 0 ~ 70�� 
		if( fCharaFlashAngle > 110.0f)		// 1/4 ���� �޴� ���� 110 ~ 180
		{
			rValidRate *= 0.25f;
		}
		else if( fCharaFlashAngle > 70.f)	// 1/2 ���� �޴� ���� 70 ~ 110
		{
			rValidRate *= 0.5f;
		}
	}	

	//	���� �ð� ���
	m_rFlashElapseTime = m_rFlashEffectiveTime = rEffectiveTime * rValidRate;
	
	//I3TRACE("EFFECT_FlashBang : cur %.4f / src %.4f (Rate: %.2f%)\n", m_rFlashEffectiveTime, pWeaponInfo->m_rEffectiveTime, rValidRate);

	m_nScrEffectType = SCR_EFFECT_FLASHBANG;

	return TRUE;	
}

BOOL CGameCharaPlayer::EFFECT_Damage( WeaponBase * pWeapon)
{
	I3ASSERT( pWeapon != NULL);

	//	����ź�� �켱 (���� ����Ʈ �������̸� ��ŵ)
	if( m_nScrEffectType == SCR_EFFECT_FLASHBANG)	return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

	if( pWeaponInfo->m_rEffectiveTime == 0.0f) return FALSE;

	CWeaponGlobal * pWeaponGlobal = g_pWeaponInfoDataBase->getGlobal();
	
	return EFFECT_Damage( pWeaponInfo->m_rEffectiveTime, 
							&pWeaponGlobal->m_DamageEffectColor, 
							pWeaponGlobal->m_DamageEffectBlurStart,
							pWeaponGlobal->m_DamageEffectBlurEnd);
}

BOOL CGameCharaPlayer::EFFECT_Damage( REAL32 fDamageTime, I3COLOR * pColor, REAL32 fBlurStart, REAL32 fBlurEnd)
{
	// �ǰ� ���� �����մϴ� - praptor
	// 2007.05.17praptor

	////	Local�̵� Network ĳ���̵� ó���Ǿ� �Ѵ�.	

	////	����ź�� �켱 (���� ����Ʈ �������̸� ��ŵ)
	//if( m_nScrEffectType == SCR_EFFECT_FLASHBANG)	return FALSE;

	//if( fDamageTime > 0.0f)
	//{
	//	m_nScrEffectType = SCR_EFFECT_DAMAGE;
	//	i3Color::Copy( &m_EFFECT_ScreenColor, pColor);
	//	m_fBlurStart = i3Math::Clamp( fBlurStart, 0.f, 1.f);
	//	m_fBlurEnd = i3Math::Clamp( fBlurEnd, 0.f, 1.f);
	//	
	//	m_rDamageElapseTime = fDamageTime;	

	//	return TRUE;
	//}

	return FALSE;
}

void CGameCharaPlayer::EFFECT_FadeChara( REAL32 fFadeTime)
{	//	fFadeTIme�� ������ FadeIn, ����� FadeOut ȿ���� ���´�.

	m_rFadeElapseTime = fFadeTime;

	if( m_rFadeElapseTime > 0.5f)			m_rFadeElapseTime = 0.5f;
	else if( m_rFadeElapseTime < -0.5f)		m_rFadeElapseTime = -0.5f;

	I3ASSERT( m_pAnimContext->getAILower() != NULL);
	I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

	if( m_pAnimContext->getAILower()->getCurrentAIState() == NULL || m_pAnimContext->getAIUpper()->getCurrentAIState() == NULL)
		SetVisible( FALSE);

	SetBlendEnable( TRUE);

	if( m_rFadeElapseTime < 0.0f)
		SetBlendAlpha( 0.0f);
	else
		SetBlendAlpha( 1.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Hit ó��
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT32 CGameCharaPlayer::OnHit( CHARA_DAMAGEINFO * pDamageInfo)
{
	I3ASSERT( pDamageInfo != NULL);
	
	if( isInvincible())		return 0;		// ���� �����̴�.

	CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
	if( pStage->getStageState() != GAMESTAGE_STATE_BATTLE &&
		pStage->getStageState() != GAMESTAGE_STATE_INTRUDE )
		return 0;

	I3_PHYSIX_HIT_RESULT *	pHitResult = pDamageInfo->_pHitResult;
	i3PhysixShapeSet*		pDestShapeSet = pHitResult->m_pDestShapeSet;
	
	CHARA_HIT_PART	part		= CHARA_HIT_UNKNOWN;	

	REAL32	rDamagedLength		= _CalcDamageLength( pDamageInfo);

	switch( pDamageInfo->_DamageType)
	{
	////////////////////////////////////////////////////////	����
	case WEAPON_DAMAGE_FLASH:
		{
			EFFECT_FlashBang( pDamageInfo, rDamagedLength);
		}
		break;	
	////////////////////////////////////////////////////////	HP	����
	case WEAPON_DAMAGE_HP_LOSS:
		{	
			// Character�� �¾Ҵ�.
			part = GetHitPart( pDestShapeSet);
						
			if( pDamageInfo->_WeaponType == WEAPON_CLASS_THROWING || pDamageInfo->_DeathType == CHARA_DEATH_O )
			{
				if( isAlive() )
				{
					REAL32 rVaildDamageRate = _CalcDamageRate( pDamageInfo, rDamagedLength);

					if( g_pGameContext->IsP2PHost() )
					{	// ����ź�� ������ ���

						NET_Hit( part, pDamageInfo, rVaildDamageRate);
					}
					else
					{
						if( rVaildDamageRate > 0.f)
						{
							//	��� üũ
							if( part == CHARA_HIT_HEAD && m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) )
							{
								// ���κп� �Ҳ��� Ƣ�� ȿ���� ����
								g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, pDamageInfo->_WeaponType, &pDamageInfo->_pHitResult->m_Intersect, 
									&pDamageInfo->_pHitResult->m_Normal, is1PV());
							}
							else
							{
								ACTION_Hit( pDamageInfo);
							}
						}
					}
				}
				else
				{
					ACTION_Hit( pDamageInfo);
	
					// ���� ĳ���͸� ���� ������ �ش�.
					//_PHYSIX_SetLinearVelocity( pDamageInfo );
				}

				return TRUE;
			}
			else if( pDamageInfo->_WeaponType == WEAPON_CLASS_MISSION )
			{// C4�� ��� Ŭ���̾�Ʈ�� ���
				// Death chara & Physix set
				g_pCharaManager->DeathChara( getCharaNetIndex(), pDamageInfo->_nKillerIdx, pDamageInfo->_pVecDir, pDamageInfo->_DeathType, part );
				return TRUE;
			}

			////////////////////////////////////////////////////////////////////////
			// for Network  --- GameContext���� �ڵ尡 �ֽ��ϴ�.
			// desc : �ڽ��� �߻��� ���� ��Ʈ��ũ�� CS�� �����ϴ�.
			
			if( pDamageInfo->_IsOwner )
			{
				if( isAlive() )
				{
					g_pGameContext->WriteMyFireHit( getCharaNetIndex(), (UINT32) part, pDamageInfo->_WallShotReduceRate );
				}
				
				if( g_pGameContext->IsP2PHost() )
				{
					REAL32 rVaildDamageRate = 1.f;
					// ������ ���� �β� ��� �߰� by dfly79 20071227
					if( pDamageInfo->_WallShotReduceRate < 100 && pDamageInfo->_WallShotReduceRate > 0)
					{	
						rVaildDamageRate = 1.f - ((REAL32)pDamageInfo->_WallShotReduceRate * 0.01f);
						if( rVaildDamageRate < 0.f)
							rVaildDamageRate = 0.f;
					}

					NET_Hit( part, pDamageInfo, rVaildDamageRate);
				}
				else
				{	//	��� üũ
					if( part == CHARA_HIT_HEAD && m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) )
					{
						//	�Ҳ��� Ƥ��.
						g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, pDamageInfo->_WeaponType, &pDamageInfo->_pHitResult->m_Intersect, 
							&pDamageInfo->_pHitResult->m_Normal, is1PV());
					}
					else
					{
						ACTION_Hit( pDamageInfo);
					}
				}

				// ��� ������ ���
				if( pDamageInfo->_pHitResult != NULL )
				{
					i3PhysixShapeSet * pShapeSet = pDamageInfo->_pHitResult->m_pDestShapeSet;
					i3EventReceiver * pReceiver = pShapeSet->getEventReceiver();
					if( pReceiver != NULL && pReceiver->IsExactTypeOf( CGameCharaEquip::GetClassMeta() ) && m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ))
					{
						VEC3D vLin, vTorque, vRoll;
						MATRIX mat;

						i3Vector::Scale( &vLin, pDamageInfo->_pVecDir, pDamageInfo->_rDamage * 0.3f );
						
						pShapeSet->getShapeGlobalPose( &mat );
						i3Vector::Sub( &vRoll, &pDamageInfo->_pHitResult->m_Intersect, i3Matrix::GetPos( &mat ) );
						i3Vector::Cross( &vTorque, &vRoll, &vLin );
						pShapeSet->setLinearVelocity( &vLin );
						pShapeSet->setAngularVelocity( &vTorque );
					}
				}
			}
		}	//	End of case (WEAPON_DAMAGE_HP_LOSS)
		break;
	default:
		{
			I3TRACE("[%s] WARNING! Unknown damage type! (%d)\n", __FUNCTION__, (INT32) pDamageInfo->_DamageType);
		}
	}	//	End of switch

	return 1;
}

BOOL CGameCharaPlayer::CalcHelmetPenetrate( CHARA_DAMAGEINFO * pDamageInfo )
{
	BOOL	bPenetrate = TRUE;
	REAL32 rPenetrateHead = 0.f;

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( pDamageInfo->_WeaponType, pDamageInfo->_nWeaponNumber);
	if( pWeaponInfo != NULL )
		rPenetrateHead = pWeaponInfo->m_rHelmetPenetrate;

	if( pDamageInfo->_DeathType == CHARA_DEATH_O )
		rPenetrateHead = 0.5f;

	// ��� ����� ���
	CEquipInfo * pEquipInfo = (CEquipInfo*) m_pEquipContext->getEquipInfo( EQUIP_CLASS_TYPE_HELMET);
	if( pEquipInfo != NULL )
	{
		if( pEquipInfo->getDefenseRate() > 0.f)
		{
			rPenetrateHead /= pEquipInfo->getDefenseRate();
		}
	}
	
	if( rPenetrateHead < i3Math::Randf() )
	{
		bPenetrate = FALSE;
	}

	return bPenetrate;
}

BOOL CGameCharaPlayer::NET_Hit( CHARA_HIT_PART part, CHARA_DAMAGEINFO * pDamageInfo, REAL32 rValidDamageRate )
{	// Only Host
	BOOL	bDamaged = TRUE;
	BOOL	bDeath = FALSE;
	BOOL	bHelmetDrop = FALSE;
	VEC3D	vHeadDir;

		
	if (g_pFramework->IsBattleStage())
	{	
		i3Stage* pStage = g_pFramework->GetCurrentStage();

		GAMESTAGE_STATE stageState = ((CStageBattle*)pStage)->getStageState();
		if( (stageState != GAMESTAGE_STATE_BATTLE) && (stageState != GAMESTAGE_STATE_INTRUDE))
		{
			return FALSE;
		}

		// ���� �����̸� ó���մϴ�.
	}

	GAMEINFO_RECV * pGameInfo = g_pGameContext->getRecvGameInfo( getCharaNetIndex() );

	I3ASSERT( pDamageInfo->_nHarmerIdx != WEAPON_USAGE_UNKNOWN );
	I3ASSERT( pDamageInfo->_nWeaponNumber != -1 );

	i3Vector::Scale( &vHeadDir, pDamageInfo->_pVecDir, -1.f );

	if( isAlive())
	{
		///
		if( part == CHARA_HIT_HEAD && m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) )
		{	
			if( pDamageInfo->_WeaponType != WEAPON_CLASS_THROWING)
			{
				//	����� ���� �ִٸ�
				bHelmetDrop = TRUE;
				// ����� ���� ���� ���
				if( !CalcHelmetPenetrate( pDamageInfo ) )
				{	// ������ �� �Ǿ����ϴ�. �������� ������� �ʽ��ϴ�.
					rValidDamageRate = 0.f;
					bDamaged = FALSE;
				}
			}
		}

		if( bDamaged )
		{
			//	���� �������� ���� HP�� ���(CS)
			INT32 nHP	 = getCurHP();
			INT32 nCurHP = nHP;
			
			// P2P Server �� ����մϴ�. C4�� ��� Ŭ���̾�Ʈ�� ����մϴ�.
			if( g_pGameContext->IsP2PHost() || pDamageInfo->_WeaponType == WEAPON_CLASS_MISSION)
			{
				// DPD_Rate�� (Damage per Distance) ����
				{
					VEC3D vLen;
					REAL32 rate = 1.0f;
					CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( pDamageInfo->_WeaponType, pDamageInfo->_nWeaponNumber );
					CGameCharaPlayer * pHitUser = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx );

					if( pHitUser != NULL )
					{
						i3Vector::Sub( &vLen, GetPos(), pHitUser->GetPos() );

						REAL32 rDistance = i3Vector::Length( &vLen);

						rate = rDistance / pWeaponInfo->m_rRange;
					}

					if( rate > 1.0f)	rate = 1.0f;

					rValidDamageRate *= (1.0f - (rate * pWeaponInfo->m_DPD_Rate));
				}

				//����ź�� �Ӹ��� ������� ��������길 ROOT�� ���� �������� ����� �մϴ�. --1214��ȹ����û, RAJA����
				if( pDamageInfo->_WeaponType == WEAPON_CLASS_THROWING && part == CHARA_HIT_HEAD)
				{		
					nCurHP = _CalcHit( CHARA_HIT_ROOT, pDamageInfo->_rDamage, rValidDamageRate);
				}
				else
				{
					nCurHP = _CalcHit( part, pDamageInfo->_rDamage, rValidDamageRate);
				}
			}
			
			if( nHP != nCurHP)
			{
				if( nCurHP == 0)	//	������
				{	
					//	Hit Effect
					ACTION_Hit( pDamageInfo);

					// Network Sync
					if( pDamageInfo->_WeaponType != WEAPON_CLASS_MISSION )
					{	// C4�� ��Ʈ��ũ ��ũ�� ������ �ʽ��ϴ�.
						g_pGameContext->WriteCharaDeath( pDamageInfo->_nKillerIdx, getCharaNetIndex(), pDamageInfo->_nHarmerIdx,
							pDamageInfo->_WeaponType, pDamageInfo->_nWeaponNumber, pDamageInfo->_DeathType, part, pDamageInfo->_pVecDir );
					}

					bDeath = TRUE;
				}
				else if( nCurHP > 0 && !isInvincible())		//	������
				{
					//	������ ����Ʈ
					ACTION_Hit( pDamageInfo);

					//	��ü�� ������ ������ ������ �����̸�
					//ACTION_Damage( part, pDamageInfo->_pVecDir);

					ActionParameter * param = GET_PREPARE_ACTION_PARAM;
					param->_part = part;
					i3Vector::Copy( &param->_dir, pDamageInfo->_pVecDir);

					CALL_ACTION_COMMAND(ACTION_CMD_DAMAGE, param);

					//	������ ������ ���Ҷ��� ��� �Ҹ��� ���� �Ѵ�.
					m_pSoundContext->PlayCharaDamageSound( part, TRUE);
				}
				else
					bHelmetDrop = FALSE;
			}
			else
				bHelmetDrop = FALSE;
		}
		else
		{	// �������� ��������� ������ �ִϸ��̼��� ���մϴ�.
			//	��ü�� ������ ������ ������ �����̸�
			//ACTION_Damage( part, pDamageInfo->_pVecDir );

			ActionParameter * param = GET_PREPARE_ACTION_PARAM;
			param->_part = part;
			i3Vector::Copy( &param->_dir, pDamageInfo->_pVecDir);

			CALL_ACTION_COMMAND(ACTION_CMD_DAMAGE, param);

			m_pSoundContext->PlayCharaDamageSound( part, TRUE);
		}
	}
	else
	{	
		ACTION_Hit( pDamageInfo);

		// ���� ĳ���͸� ���� ������ �ش�.
		//_PHYSIX_SetLinearVelocity( pDamageInfo );
	}

	if( bDamaged )
	{
		// network sync
		if( pDamageInfo->_WeaponType != WEAPON_CLASS_MISSION &&
			(pDamageInfo->_DeathType == CHARA_DEATH_B || pDamageInfo->_DeathType == CHARA_DEATH_O) )
		{
			CMulticastSystem * pMulticast = g_pGameContext->getMulticast();
			
			{
				N_PCINFO_DEF Packet;
				if( bHelmetDrop )
					pGameInfo->_tDefData._nHelmet = HELMET_STATE_DROP;
				g_pGameContext->BuildDefPacket( &Packet, &pGameInfo->_tDefData);
				pMulticast->setOriginalPacketChara_Def( getCharaNetIndex(), &Packet );
			}

			{
				N_PCINFO_HIT Packet;
				g_pGameContext->BuildHitPacket( &Packet, (UINT8) pDamageInfo->_nKillerIdx, (UINT8) part, 0 );
				pMulticast->setOriginalPacketChara_Hit( getCharaNetIndex(), &Packet );
			}

			pMulticast->setOriginalPacketChara_Type( getCharaNetIndex(), CHARA_PACKETTYPE_HIT );
			pMulticast->plusWritePos_Chara( getCharaNetIndex() );
		}
	}

	if( bHelmetDrop )
	{
		m_pEquipContext->DropToWorld( EQUIP_CLASS_TYPE_HELMET, &vHeadDir );

		VEC3D * pVecPos = NULL;
		VEC3D * pNormal = NULL;

		if( pDamageInfo->_pHitResult != NULL)
		{
			pVecPos = &pDamageInfo->_pHitResult->m_Intersect;
			pNormal = &pDamageInfo->_pHitResult->m_Normal;
		}
		else
		{
			pVecPos = i3Matrix::GetPos( m_pBoneContext->getHeadMatrix());
			pNormal = pDamageInfo->_pVecDir;
		}

		//	�Ҳ��� Ƥ��.
		g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, pDamageInfo->_WeaponType, pVecPos, 
					pNormal, is1PV());
	}

	if( bDeath )
	{
		// Death chara & Physix set
		g_pCharaManager->DeathChara( getCharaNetIndex(), pDamageInfo->_nKillerIdx, pDamageInfo->_pVecDir, pDamageInfo->_DeathType, part );
	}

	return bDeath;
}


void CGameCharaPlayer::_PHYSIX_SetLinearVelocity( CHARA_DAMAGEINFO * pInfo )
{
	VEC3D vForce;
	VEC3D vDir;

	i3Vector::Normalize( &vDir, pInfo->_pVecDir );

	if( pInfo->_DeathType == CHARA_DEATH_B || pInfo->_DeathType == CHARA_DEATH_O )
	{
		i3PhysixShapeSet * pShapeSet;
		i3Body * pBody = GetSceneObject()->GetBody();
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();

		i3Vector::Scale( &vForce, &vDir, (REAL32)(pInfo->_rDamage) * 0.05f );
		
		for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
		{
			pShapeSet = pSkel->getShapeSet( i );

			if( pShapeSet == NULL )		continue;

			CHARA_HIT_PART part = GetHitPart( pShapeSet );

			if( part == CHARA_HIT_BODY1 )
			{
				VEC3D vAddLin;
				i3Vector::Scale( &vAddLin, &vForce, 1.5f );
				pShapeSet->setLinearVelocity( &vForce );
			}
			else if( part != CHARA_HIT_UNKNOWN )
			{
				pShapeSet->setLinearVelocity( &vForce );
			}
		}
		return;
	}

	if( pInfo->_pHitResult != NULL )
	{	
		i3Vector::Scale( &vForce, &vDir, (REAL32)(pInfo->_rDamage) * 0.5f );
		pInfo->_pHitResult->m_pDestShapeSet->setLinearVelocity( &vForce );
	}
}
*/

/*
"", "H_Head", "B_Neck", "B_L Clavicle", "B_R Clavicle",
	"B_Spine3", "B_Spine2", "B_Spine1", "B_Root", "AU_L UpperArm", "AD_L ForeArm",
	"AU_R UpperArm", "AD_R ForeArm", "HN_L Hand", "HN_R Hand",
	"LU_L Thigh", "LD_L Calf", "LU_R Thigh", "LD_R Calf",
	"F_L Foot", "F_R Foot" };
	*/

/*
CHARA_HIT_PART CGameCharaPlayer::GetHitPart( i3PhysixShapeSet * pDestShapeSet)
{	
	I3ASSERT( pDestShapeSet != NULL);

	CHARA_HIT_PART	part = CHARA_HIT_UNKNOWN;
	const char * pszCollName = pDestShapeSet->GetName();
	if( pszCollName == NULL ) return part;

	switch( pszCollName[0])
	{
		case 'M' : //Model_Helmet
			if( pszCollName[2] == 'd')
				part = CHARA_HIT_HEAD;
			break;
		case 'H' :
			if( pszCollName[1] == 'N' )
			{
				if( pszCollName[3] == 'L' )		part = CHARA_HIT_LHAND;		// Left Hand
				else							part = CHARA_HIT_RHAND;		// Right Hand
			}
			else
			{
				if( pszCollName[1] == 'E' && !m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) )
					part = CHARA_HIT_HEAD;
				else
					part = CHARA_HIT_HEAD;									// Head
			}
			break;
		case 'B' :
			if( pszCollName[2] == 'N' )			part = CHARA_HIT_NECK;		// Neck
			else if( pszCollName[2] == 'S' )
			{
				switch( pszCollName[ 7 ] )
				{
				case '3' :	part = CHARA_HIT_BODY1;		break;				// Body1
				case '2' :	part = CHARA_HIT_BODY2;		break;				// Body2
				case '1' :	part = CHARA_HIT_BODY3;		break;				// Body3
				default : part = CHARA_HIT_UNKNOWN;		break;
				}
			}
			else if( pszCollName[4] == 'C' )
			{
				if( pszCollName[2] == 'L' ) part = CHARA_HIT_LCLAVICLE;		// Left Clavicle
				else						part = CHARA_HIT_RCLAVICLE;		// Right Clavicle
			}
			else							part = CHARA_HIT_ROOT;			// Root
			break;
		case 'A' :
			if( pszCollName[1] == 'U' )
			{
				if( pszCollName[3] == 'L' ) part = CHARA_HIT_LUARM;		// Left UpperArm
				else						part = CHARA_HIT_RUARM;		// Right UpperArm
			}
			else
			{
				if( pszCollName[3] == 'L' )	part = CHARA_HIT_LDARM;		// Left ForeArm
				else						part = CHARA_HIT_RDARM;		// Right ForeArm
			}
			break;
		case 'L' :
			if( pszCollName[1] == 'U' )
			{
				if( pszCollName[3] == 'L' )	part = CHARA_HIT_LULEG;		// Left Thigh
				else						part = CHARA_HIT_RULEG;		// Right Thigh
			}
			else
			{
				if( pszCollName[3] == 'L' )	part = CHARA_HIT_LDLEG;		// Left Calf
				else						part = CHARA_HIT_RDLEG;		// Right Calf
			}
			break;
		case 'F' :
			if( pszCollName[2] == 'L' )		part = CHARA_HIT_LFOOT;		// Left Foot
			else							part = CHARA_HIT_RFOOT;		// Right Foot
			break;
		default:
			{
				//I3TRACE( "[%s] WARNING! Invalid Hit Part (%s)\n", __FUNCTION__, pszCollName);
			}
			break;
	}

		
	//I3TRACE( "HIT NAME : %s\n", pszCollName);

	return part;
}

REAL32 CGameCharaPlayer::_CalcDamageLength( CHARA_DAMAGEINFO * pDamageInfo)
{	
	I3ASSERT( pDamageInfo != NULL);

	VEC3D InvVecDir, vShapePos;

	I3_PHYSIX_HIT_RESULT *	pHitResult = pDamageInfo->_pHitResult;
	i3PhysixShapeSet*		pDestShapeSet = pHitResult->m_pDestShapeSet;
	
	*((NxVec3 *) &vShapePos) = pDestShapeSet->getNxActor()->getGlobalPosition();

	i3Vector::Sub( &InvVecDir, &vShapePos, pDamageInfo->_pVecPos );

	return i3Vector::Length( &InvVecDir);
}

REAL32 CGameCharaPlayer::_CalcDamageRate( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength)
{
	I3ASSERT( pDamageInfo != NULL);

	REAL32  rValidDamageRate = 1.0f;

	// �Ÿ��� ���� �������� ������( 1.0f�� Default�Դϴ�.)
	rValidDamageRate	= 1.0f - ((1.0f / pDamageInfo->_rExplosionRange) * rDamagedLength) * pDamageInfo->_rExplosionSlope;
 	if( rValidDamageRate < I3_EPS)	rValidDamageRate = I3_EPS;

	return rValidDamageRate;
}

INT32 CGameCharaPlayer::_CalcHit( CHARA_HIT_PART part, REAL32 nDamage, REAL32 rValidDamageRate)
{	
	I3ASSERT( g_pCharaInfoDataBase->getGlobal() != NULL);
	I3ASSERT( m_pCharaInfo != NULL );

#if defined ( I3_DEBUG)
//	char szPart[256] = "\0";
#endif

	if( m_rInvincibleElapseTime > 0.0f + I3_EPS)		//	���� �ð��̹Ƿ� �������� ���� �ʴ´�. 
		return -1;

	REAL32 rCurrentDamage = nDamage * rValidDamageRate;
	REAL32 rRateDamage = 0;

	//	�������� ���� ������ ������ �մϴ�.
	switch( part)
	{
	case CHARA_HIT_HEAD:			//	�Ӹ� ������
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateHead;
		}
		break;
	case CHARA_HIT_NECK	:
	case CHARA_HIT_LCLAVICLE :
	case CHARA_HIT_RCLAVICLE :		// ���
	case CHARA_HIT_BODY1 :
	case CHARA_HIT_BODY2 :
	case CHARA_HIT_BODY3 :
	case CHARA_HIT_ROOT	:			//	���� ������
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateBody;
		}
		break;

	case CHARA_HIT_RUARM:
	case CHARA_HIT_RDARM:
	case CHARA_HIT_LUARM:
	case CHARA_HIT_LDARM:			//	�� ������
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateArm;
		}
		break;
	case CHARA_HIT_LHAND:
	case CHARA_HIT_RHAND:			//	�� ������
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateHand;
		}
		break;
	case CHARA_HIT_LULEG:
	case CHARA_HIT_LDLEG:
	case CHARA_HIT_RULEG:
	case CHARA_HIT_RDLEG:			//	�ٸ� ������
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateLeg;
		}
		break;
	case CHARA_HIT_LFOOT:
	case CHARA_HIT_RFOOT:			//	�� ������
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateFoot;
		}
		break;
	default: rRateDamage = 0.f;
	}

	//	���� �������� ����
	rCurrentDamage = (rRateDamage) * rCurrentDamage;

	//	HP ����	
	m_pCharaInfo->_Hp -= (INT32) (rCurrentDamage + 0.5f);
	if( m_pCharaInfo->_Hp <= 0)		m_pCharaInfo->_Hp = 0;

	//I3TRACE( "HP : %d\n", m_pCharaInfo->_Hp);

	GAMEINFO_RECV * pGameInfo	= g_pGameContext->getRecvGameInfo( getCharaNetIndex() );

	{//Hit�Ȱ�� �̵��ӵ� �������� ȿ��
		if( !isNetwork() && ( pGameInfo->_tDefData._nHP > (UINT8) m_pCharaInfo->_Hp))
		{
			m_pMoveContext->EnableSpeedReduction( pGameInfo->_tDefData._nHP - (UINT8) m_pCharaInfo->_Hp);
		}
	}

	pGameInfo->_tDefData._nHP	= (UINT8) m_pCharaInfo->_Hp;

#if defined( I3_DEBUG)
	//I3TRACE("%d %f \n",nCurrentDamage, rRateDamage); 
	//I3TRACE( "Hit! (Part: %s (%d) / HP: %d / Damage %d)\n", szPart, part, m_pCharaInfo->_Hp, nCurrentDamage);
#endif

	return m_pCharaInfo->_Hp;		//	���� ���� HP ��ȯ
}


void CGameCharaPlayer::DeleteBoundBox( void)
{
	// ĳ������ �ø��� ���ϴ�.
	if( m_pBody->GetBound() != NULL )
		m_pBody->SetBound( NULL );

	// ������ �ø��� ���ϴ�.
	WeaponBase * pWeapon = NULL;
	for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; i++)
	{
		pWeapon = getWeaponContext()->getWeapon( i);

		if( pWeapon != NULL)
			pWeapon->DeleteBoundBox();
	}

	//	����� �ø��� ���ϴ�.
	CGameCharaEquip * pEquip = NULL;	
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)
	{
		pEquip = m_pEquipContext->getEquip( (EQUIP_CLASS_TYPE) i);

		if( pEquip != NULL)
			pEquip->DeleteBoundBox();
	}	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void CGameCharaPlayer::EnableControl( BOOL bVal )
//{
//	if( bVal)
//	{
//		removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);
//	}
//	else
//	{
//		addEnableStateMask( CHARA_STATE_DISABLE_CONTROL);
//	}
//}

void CGameCharaPlayer::EnableCollision( BOOL bVal )
{
	if( bVal )
	{
		if( !isNetwork() )
			m_pController->setGroup( CGRP_MYCHARA_BOUND );
		else
			m_pController->setGroup( CGRP_CHARA_BOUND );
	}
	else
		m_pController->setGroup( CGRP_DUMMY );
}

void CGameCharaPlayer::_CalcClimbingLadder( VEC3D * vMoveDir)
{
	VEC3D vTemp;
	VEC3D vVelNormal;
	
	i3Vector::Sub( &vTemp, getLadderContactPosition(), GetPos());

	setY( &vTemp, 0.0f);
	i3Vector::Normalize( &vTemp, &vTemp);

	i3Vector::Copy( &vVelNormal, vMoveDir);
	setY( &vVelNormal, 0.0f);
	REAL32 rLength = i3Vector::Normalize(&vVelNormal, &vVelNormal);

	REAL32 rMove = i3Vector::Dot( &vVelNormal, &vTemp);

	if( i3Math::abs( rMove) > 0.707f)
	{
		i3Vector::Set( vMoveDir, 0.0f, rMove * rLength, 0.0f);
	}
	else
	{
		setY( vMoveDir, 0.0f);
	}
}


void CGameCharaPlayer::_CalcTakeObject( VEC3D * pFirstPos )
{
	if( m_pController->getHitShape() != NULL )
	{	// ������Ʈ�� �浹, ������Ʈ�� �����δٸ�..?
		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_pController->getHitShape()->userData;
		i3EventReceiver * pReceiver = pShapeSet->getEventReceiver();
		if( pReceiver == NULL )
			return;

		VEC3D vUp;
		i3Vector::Set( &vUp, 0.f, 1.f, 0.f );

		if( pReceiver->IsExactTypeOf( i3Object::GetClassMeta() ) )
		{
			i3Object * pObj = ((i3Object*)pReceiver);
			
			i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( pObj->GetSceneObject(), i3Transform2::GetClassMeta() );

			if( pTrans == NULL )
				return;

			if( pTrans->IsAnimated() )
			{	
				I3MATRIXINFO Env;
				MATRIX	matTop, matObj;

				VEC3D	vNewPosDir;

				if( i3Vector::Dot( m_pController->getHitNormal(), &vUp ) > I3_EPS )
					m_bTakeObject = TRUE;
				else
					m_bTakeObject = FALSE;

				i3Node * pParent = pTrans->GetParent();
				i3Matrix::Identity( &matTop );
				i3Matrix::Copy( &matObj, pTrans->GetMatrix() );

				// �ش� �ִϸ��̼��� ����մϴ�. ���� ����� Matrix�� �����;� �մϴ�.
				while( pParent != NULL )
				{	
					if( pParent->IsTypeOf( i3Transform::GetClassMeta() ) )
					{
						i3Matrix::Mul( &matTop, &matTop, ((i3Transform*)pParent)->GetMatrix() );
					}

					pParent = pParent->GetParent();
				}

				Env.m_pIn	= &matObj;
				Env.m_pOut	= &matObj;
				Env.m_Time	= g_pViewer->getDeltaTime();
				Env.m_pModelViewMatrix = &matTop;
				pTrans->GetAnimatedMatrix( &Env );

				// �ִϸ��̼��� �ϰ� �ٽ� ���� Ÿ���� ���������ϴ�.
				pTrans->AddPlayTime( -g_pViewer->getDeltaTime() );

				i3Matrix::Mul( &matTop, &matObj, &matTop );

				// �ִϸ��̼ǵ� ��ǥ���� ���� �������� ��ǥ�� ���� ĳ���� �����ǿ� �����ݴϴ�.
				i3Vector::Sub( &vNewPosDir, i3Matrix::GetPos( &matTop ), i3Matrix::GetPos( pTrans->GetCacheMatrix() ));
				
				if( m_bTakeObject )
				{
					// �ش� ������Ʈ�� ���� ���� ��ǥ�� �����ɴϴ�.
					MATRIX mInv;
					VEC3D vLocalPos;
					i3Matrix::Inverse( &mInv, 0, pTrans->GetCacheMatrix() );
					i3Vector::TransformCoord( &vLocalPos, GetPos(), &mInv );
					g_pGameContext->WriteMyCharaPos( &vLocalPos, getTheta(), getRho(), m_bTakeObject, pObj->getNetIndex() );
				}
				else
				{
					// �𷺼��� üũ
					// �浹�ϸ� �̵��������� ���ݴ� �о���ϴ�. (������Ʈ�� �ɸ��� ������ ����)
					if( i3Vector::Dot( m_pController->getHitNormal(), &vNewPosDir ) > 0.f )
					{
						REAL32 stepOff = i3Vector::Normalize( &vNewPosDir, &vNewPosDir);
						if( stepOff < 0.04f )
						{
							i3Vector::Scale( &vNewPosDir, &vNewPosDir, 0.04f );
						}
						else
						{
							i3Vector::Scale( &vNewPosDir, &vNewPosDir, stepOff );
						}
					}
					else
					{
						REAL32 stepOff = i3Vector::Normalize( &vNewPosDir, &vNewPosDir);
						if( stepOff < 0.04f )
						{
							i3Vector::Scale( &vNewPosDir, &vNewPosDir, -0.04f );
						}
						else
						{
							i3Vector::Scale( &vNewPosDir, &vNewPosDir, -stepOff );
						}
					}

					if( getActionContext()->isLower( CHARA_LOWER_JUMP) )
					{	// �������� ��� �ӵ��� 0���� �մϴ�.
						setX( m_pMoveContext->getVelocity(), 0.f);
						setZ( m_pMoveContext->getVelocity(), 0.f);
					}
				}
				
				VEC3D vNewPos;
				i3Vector::Add( &vNewPos, &vNewPosDir, pFirstPos );
				forceSetPos( &vNewPos );
			}
		}
	}
	else
	{
		m_bTakeObject = FALSE;
	}
}


void CGameCharaPlayer::CheckFloorTerrain( void )
{
	VEC3D vPos;
	i3CollideeLine ColLine;

	i3Vector::Copy( &vPos, GetPos());
	i3Vector::AddY( &vPos, 0.5f);
	ColLine.SetStart( &vPos);

	i3Vector::AddY( &vPos, - 1.0f);
	ColLine.SetEnd( &vPos);

	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &ColLine, CGRP_ATTACK );
	if((pResult != NULL) && (pResult->m_pTerrain != NULL))
	{
		m_CurFloorTerrainType = (I3_TERRAIN_TYPE)pResult->m_pTerrain->m_Terrain;
	}
}
*/

/*
void CGameCharaPlayer::Move( REAL32 tm )
{
	VEC3D	vVel;
	VEC3D	newPos;
	UINT32	hitMask;

	VEC3D vPos;

	if( isNetwork() )
		return;
	
	_UpdateJumping( tm);

	_CheckWorldCollision( tm);

//	if( isStateMask( CHARA_STATE_CLIMB))
//	{
//		setClimbingLadderEnable( _IsOverlappedBy( CGRP_OBJECT_LADDER));
//	}

	m_pController->getPosition( &vPos );
	
	if( isAlive() )
	{
		// ������Ʈ�� �ö������� ����մϴ�.
		_CalcTakeObject( &vPos );
	}

	i3Vector::Copy( &vVel, m_pMoveContext->getVelocity());
	i3Vector::Copy( m_pMoveContext->getOldVelocity(), m_pMoveContext->getVelocity() );

	// ĳ���� �ø��� üũ
	if( isInvincible() )		// ������ �ÿ��� �մϴ�.
	{
		VEC3D vOtherDir;
		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++ )
		{
			CGameCharaPlayer * pChara = g_pCharaManager->getChara( i );
			if( pChara != this && pChara->isAlive() )
			{
				i3Vector::Sub( &vOtherDir, this->GetPos(), pChara->GetPos() );
				REAL32 rLength = i3Vector::Length( &vOtherDir);//i3Vector::Normalize( &vOtherDir, &vOtherDir );

				if( rLength < 0.45f )
				{
					//rLength = 1.f - rLength;
					//i3Vector::Scale( &vOtherDir, &vOtherDir, rLength );
					i3Vector::Add( &vVel, &vVel, &vOtherDir );
					i3Vector::Add( m_pMoveContext->getVelocity(), m_pMoveContext->getVelocity(), &vOtherDir);
				}
			}
		}
	}

	// �̵� ���� �ִ��� üũ
	if( (vVel.x != 0.0f) || (vVel.y != 0.0f) || (vVel.z != 0.0f))
	{
		REAL32	rSharpness = 0.3f * ( g_pViewer->getDeltaTime() / 0.016666f);
		REAL32	stepOff = i3Vector::Length( &vVel) * 0.2f;
		if( stepOff > 0.3f)
			stepOff = 0.3f;
		else if( stepOff < 0.01f)
			stepOff = 0.01f;
		
		m_pController->Move( &vVel, stepOff, &hitMask, rSharpness);

		m_rFloorCheckTimer += g_pViewer->getDeltaTime();

		if( hitMask & NXCC_COLLISION_DOWN)
		{	
			if( !isStateMask( CHARA_STATE_FLOOR) || m_rFloorCheckTimer > PHYSIX_FLOORCHECK_TIME )
			{	
				CheckFloorTerrain();

				m_rFloorCheckTimer = 0.f;
			}

			// �ٴڿ� ��Ҵٸ�....
			addStateMask( CHARA_STATE_FLOOR);

			// �ٴ��� ���鿡 ���� ĳ���͸� �̲���Ʈ���ϴ�.
			//NxShape * pShape = m_pController->getHitShape();
			//if( pShape != NULL )
			{
				VEC3D * pFloorNormal = m_pController->getHitNormal();
				REAL32 rDot = i3Vector::Dot( &I3_YAXIS, pFloorNormal );
				if( rDot < 0.5f && rDot > I3_EPS)
				{	
					setX( &vVel, getX( pFloorNormal) * 0.02f );
					setZ( &vVel, getZ( pFloorNormal) * 0.02f );
					setY( &vVel, -0.04f );

					stepOff = i3Vector::Length( &vVel) * 0.1f;
					if( stepOff > 0.3f)
						stepOff = 0.3f;

					m_pController->Move( &vVel, stepOff, &hitMask);

					addStateMask( CHARA_STATE_SLOPESLIDE); //m_bSlide = TRUE;
				}
				else
				{
					removeStateMask( CHARA_STATE_SLOPESLIDE); //m_bSlide = FALSE;
				}

				
				// ��Ʈ�� ��ġ�� �Ʒ����� Sphere���� �������� ����Ͽ� ĳ���͸� ������ ����߷��� �մϴ�.
				// �����ÿ� ���� �ذ�
				VEC3D * pHitWorldPos = m_pController->getHitPos();
				VEC3D	vDownSpherePos;

				setX( &vDownSpherePos, getX( i3Matrix::GetPos( getIKMatrix())) ); 
				setY( &vDownSpherePos, getY( i3Matrix::GetPos( getIKMatrix())) - (TALL*0.5f) );
				setZ( &vDownSpherePos, getZ( i3Matrix::GetPos( getIKMatrix())) );
				
				i3Vector::Sub( &vDownSpherePos, &vDownSpherePos, pHitWorldPos);
				i3Vector::Normalize( &vDownSpherePos, &vDownSpherePos);
				REAL32 rDotAngle = i3Vector::Dot( &vDownSpherePos, &I3_YAXIS);
				if( rDotAngle < 0.7f && rDotAngle > 0.f)
				{
					setX( &vVel, getX( &vDownSpherePos) * 0.2f);
					setZ( &vVel, getZ( &vDownSpherePos) * 0.2f);


					m_pController->Move( &vVel, stepOff, &hitMask);

					//_ResetJumping();
				}
				
			}
		}
		else
		{
			// �ٴڿ��� �������ٸ�
			//m_bCollidedFloor = FALSE;
			removeStateMask( CHARA_STATE_FLOOR | CHARA_STATE_SLOPESLIDE);

			if( !isStateMask( CHARA_STATE_JUMP))
			{
		//		I3TRACE("or:%f, vel: %f, new:%f\n", getY( &newPos), vPos.y, vVel.y);
				setY( &newPos, getY( &vPos) + vVel.y);
			}
		}

		m_pController->getPosition( &newPos);

		if( isStateMask( CHARA_STATE_JUMP) && (hitMask & NXCC_COLLISION_UP))
		{
			_ResetJumping();

			i3Vector::Add( &newPos, 0.f, -getY( &vVel), 0.f );
		}
	#if 1
		i3Vector::Copy( GetPos(), &newPos);
	#else
		i3Vector::Sub( m_pMoveContext->getVelocity(), &newPos, GetPos());

		i3Vector::Copy( GetPos(), &newPos);
	#endif	
	}

	//m_rMoveTime -= 0.016f;
	//if( m_rMoveTime >= 0.016f)
		//m_rMoveTime = 0.f;
	
}



void CGameCharaPlayer::MoveStop( void)
{
	i3Vector::Zero( m_pMoveContext->getMoveDir());

	m_pMoveContext->setAccel( 0.0f, 0.0f, 0.0f);
	m_pMoveContext->setVelocity( 0.0f, 0.0f, 0.0f);	
}


void CGameCharaPlayer::forceSetPos( VEC3D * pPos)
{	
	CGameCharaBase::forceSetPos( pPos);
	m_pMoveContext->SetPosition( pPos);
}

void CGameCharaPlayer::GetVirtualCamPosition( VEC3D * pPos, MATRIX * pMat, bool bDump)
{
	I3ASSERT( g_pCameraManager != NULL);

	REAL32	Theta;
	VEC3D	vAt, vPos;
	MATRIX	mtx;

	i3Matrix::Identity( &mtx);
	i3Vector::Copy( &vAt, i3Matrix::GetAt( GetMatrix()));

	Theta = atan2f( getX( &vAt), getZ( &vAt));
	i3Matrix::SetRotateY( &mtx, Theta + I3_DEG2RAD( 180.0f));

	//Rotate
	i3Matrix::PostRotateAxis( &mtx, i3Matrix::GetRight( GetMatrix()), - getRho());
	i3Vector::TransformCoord( &vAt, &m_pDBInfo->m_vFPSCamPos, &mtx);
	
	if( is1PV())
	{
		i3Vector::Copy( &vPos, i3Matrix::GetPos( getBoneMatrix( BONE_NECK)));
	}
	else
	{
		i3Vector::Copy( &vPos, i3Matrix::GetPos( getBoneMatrix( BONE_IK)));
		i3Vector::SetY( &vPos, getY( GetPos()) + m_pCameraContext->getIKHeightForCam());
	}
	//i3Vector::SetY( &vPos, getY( GetPos()) + getIKHeightForCam());
	i3Vector::Add( pPos, &vAt,&vPos);

	if( pMat != NULL )
		i3Matrix::Copy( pMat, &mtx );
}
*/

/*
#define SIT_HEIGHT -0.6f

void CGameChara::_setStandOn1PV( BOOL bStand, REAL32 rTime)
{
	m_rChangePoseTime		= rTime;	//�ڼ� ����� �ɸ��� �ð�
	m_rChangePoseAccTime	= 0.0f;		//�ڼ� ������ ���� �ð�
	m_bChangePoseEnable		= TRUE;		//�ڼ� ������ ���۵Ǿ����� �˸��� �÷���
	m_bChangeToStand		= bStand;	//���ڼ��� �������� �����ڼ��� ��������.
}
*/

/*
void CGameCharaPlayer::_setStandOn1PV( CHARA_STATE_FOR_CAM nState, REAL32 rTime)
{
	if( nState == m_StateForCam)
		return;

	if( isStateMask( CHARA_STATE_CHANGEPOSE))
	{// �ڼ� �������̴�.
		m_rChangePoseAccTime	= rTime * ( (m_rChangePoseTime - m_rChangePoseAccTime) / m_rChangePoseTime);		//�ڼ� ������ ���� �ð�
	}
	else
	{
		m_rChangePoseAccTime	= 0.0f;
		addStateMask( CHARA_STATE_CHANGEPOSE);
		//m_bChangePoseEnable		= TRUE;								//�ڼ� ������ ���۵Ǿ����� �˸��� �÷���
	}

	if( m_nChangeFromState == CSFC_JUMP || nState == CSFC_JUMP)
	{
		m_rChangePoseAccTime = rTime * 0.1f;
	}

	m_rChangePoseTime		= rTime;								//�ڼ� ����� �ɸ��� �ð�
	m_nChangeFromState		= m_StateForCam;
	m_StateForCam			= nState;

	//�ڼ� ����� ���ۺ��� ��Ʈ�ѷ��� �ٲ۴�.
	{
		if( m_StateForCam == 2)
		{
			m_pMoveContext->ChangeControllerHeight( HALFTALL);
		}
		else
		{
			m_pMoveContext->ChangeControllerHeight( TALL);
		}
	}
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HUD
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
BOOL CGameCharaPlayer::ApplyCrosshair( void)
{
	CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);

	BOOL bEnableCross = TRUE;			// CH�� ��Ÿ���� ������ ����
	BOOL bScopeCH = FALSE;				// Scope�� CH�� ��Ÿ������ ����

	if( isNetwork())	return FALSE;

	if( g_pCamera != NULL)
	{
		if( g_pCamera->getCurTarget() != this)		return FALSE;	
	}

	if( pStage->getCrosshair() == NULL)			return FALSE;

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
	{
		bEnableCross = FALSE;
	}
	else
	{
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);

		//	Crosshair
		if( isAlive())
		{
			switch( pWeaponInfo->getExtensionType())
			{
				case WEAPON_EXT_SCOPE :
					// Ext.�� Ȱ��ȭ �Ǵ� �͸����� Crosshair�� ������� ��.
					bEnableCross = !pWeapon->isExtensionActivate();
					bScopeCH = isStateMask( CHARA_STATE_ZOOM);
					break;

				case WEAPON_EXT_SCOPE2 :
					bEnableCross = TRUE;
					bScopeCH = isStateMask( CHARA_STATE_ZOOM);
					break;

				case WEAPON_EXT_ZOOM :
				case WEAPON_EXT_DOT_SIGHT :
					bEnableCross = ! isStateMask( CHARA_STATE_ZOOM);
					break;

				//case WEAPON_EXT_DUAL_MAG :
				//case WEAPON_EXT_SILENCER :
				//case WEAPON_EXT_STEADY_AIM :
				//case WEAPON_EXT_NONE :
			}

			if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_SNIPER)
			{
				bEnableCross = isStateMask( CHARA_STATE_ZOOM);
				bScopeCH = TRUE;
			}
		}
		else
		{
			bEnableCross = FALSE;	//	ĳ���Ͱ� ���� ���
		}
		
		if( pWeaponInfo->m_CrossHairInvisible & 0x00000002)
		{
			// �� �Ŀ� ������ Scope CH�� ������ٰ� �ϴ���, ���� ScopeCH�� ON�̶�� ����
			// Scope ����̱� ������ �Ϲ� CH�� ��Ÿ���� �ʾƾ� �Ѵ�.
			if( bScopeCH == TRUE)
				bEnableCross = FALSE;

			// Scope Crosshair
			bScopeCH = FALSE;
		}

		if( pWeaponInfo->m_CrossHairInvisible & 0x00000001)
		{
			// Normal Crosshair
			if( bScopeCH == FALSE)
				bEnableCross = FALSE;
		}
	}

	if( g_pCamera->IsExactTypeOf( CGameCamera_Observer::GetClassMeta()))
	{
		// ���� �ÿ��� Crosshair�� �׻� Disable ��Ų��.
		bEnableCross = FALSE;
	}

	if( g_pGameContext->getUserDeath( g_pPlayer->getCharaNetIndex()))
		bEnableCross = FALSE;

	//bEnableCross = TRUE;

	// Dragonov�� SSG69�� ���� ������ �Ǵ� ������ ��쿣 Scope_CH�� ��Ÿ���� �ʴ´�.
	bScopeCH = bScopeCH && pWeapon->getScopeCrosshairEnable();

	//	Apply Crosshair	
	pStage->setScopeCrosshair( bScopeCH == TRUE);
	pStage->enableCrossHair( bEnableCross);

	return TRUE;
}

void CGameCharaPlayer::SetCharaNick(BOOL bFriendly)
{
	CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);

	CHud * pHud = pStage->getHud();

	REAL32 rPosX, rPosY;

    if( _CalcProjectionPos( &rPosX, &rPosY, GetPos() ))
	{
 		rPosX *= 0.5f;		rPosX += 0.5f;
		rPosY *= -0.5f;		rPosY += 0.5f;
		
		VEC3D vTemp;
		char szTemp[MAX_STRING_COUNT] = {0,};
		i3Vector::Sub( &vTemp, g_pPlayer->GetPos(), GetPos());
		INT32 nLength = (INT32)i3Vector::Length(&vTemp);
		if( nLength > 9000 || nLength < -9000) nLength = 0;
		
		//if( g_pCharaManager->getCharaByNetIdx( getCharaNetIndex()] != NULL )
		{
			if( g_pPlayer->isAlive() )
			{
#if defined(I3_DEBUG)
				sprintf( szTemp, "%s [%dm]-%dHP", g_pGameContext->getNickForSlot(getCharaNetIndex()), nLength, getCurHP());
#else
				sprintf( szTemp, "%s", g_pGameContext->getNickForSlot(getCharaNetIndex()) );
#endif
			}
			else
			{
#if defined(I3_DEBUG)
				sprintf( szTemp, "%s -%dHP", g_pGameContext->getNickForSlot(getCharaNetIndex()), getCurHP());
#else
				sprintf( szTemp, "%s", g_pGameContext->getNickForSlot(getCharaNetIndex()) );
#endif
			}
		}

		// Friendly
		pHud->SetCharaNick( szTemp, bFriendly, rPosX, rPosY, (REAL32) nLength);
	}
}


void CGameCharaPlayer::SetNickOnTarget()
{	
	BOOL	bNeutralColor = TRUE;

	// ĳ������ �� �ε����� -1�� ���� �ش� ĳ���Ͱ� ��������̴�.
	// ������Ʈ ������ ������ üũ�� ĳ�����̹Ƿ� -1�̸� ������� ���ƾ��մϴ�. -by dfly 2007.12.20
	if( m_pCrossHitChara != NULL && m_pCrossHitChara->getCharaNetIndex() != -1 && m_pCrossHitChara->isNetwork() )
	{
		m_pCrossHitChara->SetCharaNick( FALSE);
	}

	m_rCrossHitTimer += g_pViewer->getDeltaTime();
	// �ʴ� 15ȸ�� üũ
	if( m_rCrossHitTimer < HUD_CROSSNICKCHECK_TIME )
		return;
		
	CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);
	Crosshair * pCrosshair = pStage->getCrosshair();
	
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )	return;
	CWeaponInfo * pInfo = pWeapon->getWeaponInfo();
	if( pInfo == NULL )	return;

	// ���� �ø���
	CheckCrossHit();

	if( pInfo->m_TypeClass == WEAPON_CLASS_THROWING || pInfo->m_TypeClass == WEAPON_CLASS_ITEM ||
		pInfo->m_TypeClass == WEAPON_CLASS_MISSION )
	{
		bNeutralColor = TRUE;
		m_pCrossHitChara = NULL;
	}
	else
	{
		UINT32 roughGroup, collGroup;
		I3_PHYSIX_HIT_RESULT * pRoughResult, * pResult;
		
		if( getCharaTeam() & CHARACTER_TEAM_BLUE )
		{
			roughGroup = CGRP_TEAM_BLUE_ROUGH;
			collGroup = CGRP_TEAM_BLUE_ATTACK;
		}
		else
		{
			roughGroup = CGRP_TEAM_RED_ROUGH;
			collGroup = CGRP_TEAM_RED_ATTACK;
		}

		REAL32 rMinLength = 10000.f;

		// ���� �׽�Ʈ
		UINT32 nRoughCount = g_pFramework->raycast( &m_CrossHitLine, roughGroup);
		if( nRoughCount > 0)
		{
			pRoughResult = g_pFramework->getRaycastResult();
			g_pFramework->reserveRaycastResult( nRoughCount );

			for( UINT32 i = 0; i < nRoughCount; ++i)
			{
				i3EventReceiver * pObj = pRoughResult[ i ].m_pDestShapeSet->getEventReceiver();

				if( pObj == NULL)	continue;
				
				if( m_CrossHitResult.m_T < pRoughResult[ i ].m_T || pRoughResult[ i].m_T >= rMinLength)
				{	
					m_pCrossHitChara = NULL;
					continue;
				}

				UINT32 nResultCount = g_pFramework->raycast( &m_CrossHitLine, collGroup);
				if( nResultCount == 0 )
				{
					m_pCrossHitChara = NULL;
					continue;
				}
				pResult = g_pFramework->getRaycastResult();

				for( UINT32 j = 0; j < nResultCount; j++ )
				{
					if( pResult[ j ].m_pDestShapeSet->getEventReceiver() != pObj )
						continue;

					if( m_CrossHitResult.m_T < pResult[ j ].m_T )// || pResult[ j].m_T >= rMinLength)
					{
						m_pCrossHitChara = NULL;
						continue;
					}

					if( pResult[ j ].m_T > pInfo->m_rRange )
					{
						m_pCrossHitChara = NULL;
						continue;
					}

					if( !pObj->IsExactTypeOf( CGameCharaBase::GetClassMeta() ) )
					{
						m_pCrossHitChara = NULL;
						continue;
					}

					if( pResult[ j].m_T < rMinLength )
					{
						m_pCrossHitChara = (CGameCharaPlayer *) pObj;
						if( m_pCrossHitChara->isAlive())
						{
							bNeutralColor = FALSE;
						}

						rMinLength = pResult[ j].m_T;
					}
				}
			}

			g_pFramework->resetRaycastResult();
		}
		else
			m_pCrossHitChara = NULL;
	}

	if( bNeutralColor)//�����
	{
		// ���ص� ���� �׾����� ���� �������� ����
		pCrosshair->SetCrosshairColor( 0, 255, 0);
	}
	else			  //���ǻ�
	{	
		pCrosshair->SetCrosshairColor( 255, 0, 0);
	}

	m_rCrossHitTimer = 0.f;
}

BOOL CGameCharaPlayer::_CalcProjectionPos( REAL32 * pOutX, REAL32 * pOutY, VEC3D * pIn )
{
	VEC3D	vTemp;
	VEC3D	vTempIn;
	MATRIX	CamMat;
	VEC3D*	pvCamAt;
	VEC3D	vSub;
	REAL32	Height = 1.9f;

	if( getActionContext()->isLower( CHARA_LOWER_DEATH ) )	return FALSE;

	pvCamAt = i3Matrix::GetAt( g_pCamera->getCamMatrix());

	i3Vector::Sub( &vSub, g_pPlayer->GetPos(), pIn );
	i3Vector::Normalize( &vSub, &vSub );

	if( i3Vector::Dot( pvCamAt, &vSub) < 0.f ) 
		return FALSE;
	
	i3Matrix::Mul( &CamMat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

//	if( STATE_getBody() == CHARA_BODY_CROUCH )
//		Height = 1.25f;

	i3Vector::Set( &vTempIn, i3Vector::GetX( pIn ), i3Vector::GetY( pIn ) + Height, i3Vector::GetZ( pIn ) );
	
	i3Vector::TransformCoord( &vTemp, &vTempIn, &CamMat );

	*pOutX	= i3Vector::GetX( &vTemp );
	*pOutY	= i3Vector::GetY( &vTemp );

	return TRUE;
}

void CGameCharaPlayer::_QueContactReport( I3_PHYSIX_CONTACT_REPORT * pReport )
{
	UINT32 nCount = m_nContactReportCount - m_nContactReportReadPos;
	if( nCount > PHYSIX_CONTACT_REPORT_COUNT )
	{
		I3TRACE( "CGameCharaPlayer::_QueContactReport() - buffer over flow.\n" );
		return;
	}

	UINT32 i = m_nContactReportCount % PHYSIX_CONTACT_REPORT_COUNT;

	i3Physix::ContactReportCopyTo( &m_ContactReportBuf[ i ], pReport );

	m_nContactReportCount++;
}

void CGameCharaPlayer::_ParseContactReport( void )
{
	UINT32 nCount = m_nContactReportCount - m_nContactReportReadPos;
	if( nCount == 0 )
		return;

	UINT32 i;

	BOOL	bCheck = FALSE;
	INT32	nEffAndSndCnt	= 0;
	VEC3D	vUp;

	i3Vector::Set( &vUp, 0.f, 1.f, 0.f );

	for( i = 0; i < nCount; i++ )
	{
		UINT32 nPos = (m_nContactReportReadPos + i) % PHYSIX_CONTACT_REPORT_COUNT;
		I3_PHYSIX_CONTACT_REPORT * pReport = &m_ContactReportBuf[ nPos ];

		i3PhysixShapeSet * pShapeSet = pReport->m_pOwnShapeSet;
		const char * pszName = pShapeSet->GetName();

		if( pszName[12] == 'R' && pszName[15] == 't')
		{
			bCheck = TRUE;
		}

		if( pReport->m_Events == NX_NOTIFY_ON_END_TOUCH )
		{
			if( bCheck )
			{
				removeStateMask( CHARA_STATE_PHYSIXCONTACT);
			}
		}
		else if( (pReport->m_Events == NX_NOTIFY_ON_START_TOUCH) )	//pReport->m_rNormalForce > PHYSIX_EFFECT_DUTY_FORCE
		{
			for( UINT32 j = 0; j < pReport->m_nPointCount; j++ )
			{	
				if( !isStateMask( CHARA_STATE_PHYSIXCONTACT) )
				{
					if( i3Vector::Dot( &pReport->m_Normal[ j ], &vUp ) > I3_EPS + 0.9f )
					{
						addStateMask( CHARA_STATE_PHYSIXCONTACT);
					}
				}

				{
					if( pszName[0] == 'C' && (pszName[12] == 'L' || pszName[12] == 'R') && isAlive() )
						g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_WALK_COMMON, &pReport->m_Intersect[ j ], &pReport->m_Normal[ j ] );
					else
						g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, &pReport->m_Intersect[ j ], &pReport->m_Normal[ j ] );

					if( bCheck )
					{
						//if( pReport->m_rNormalForce[ j ] > 0.f && nEffAndSndCnt  <= 0 && pReport->m_pDestShapeSet != NULL)
						if( nEffAndSndCnt  <= 0 && pReport->m_pDestShapeSet != NULL && (m_DropCount == 0 || pReport->m_rNormalForce[ j ] > 0.f) )
						{
							I3_PHYSIX_TERRAIN * pTri = pReport->m_pDestShapeSet->getTerrain(0);
							if( pTri != NULL)
							{
								g_pSndMng->StartCharacterActionSnd( GTBCA_ON_THUD, i3Math::Rand()%2, &pReport->m_Intersect[ j ], is1PV() );
								m_DropCount++;
								//I3TRACE("�;���~~~��~~~��~~~��~~~..!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
							}

							nEffAndSndCnt++;
						}
					}
				}
			}
		}
	}

	m_nContactReportReadPos += nCount;
}


BOOL CGameCharaPlayer::OnEvent( UINT32 event, i3ElementBase *pObj, UINT32 param )
{
	switch( event )
	{
	case I3_EVT_CONTACT :
		if( pObj->IsExactTypeOf( i3PhysixShapeSet::GetClassMeta()) )
		{
			i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pObj;
			
			for( INT32 i = 0; i < pShapeSet->getContactCount(); i++ )
			{
				I3_PHYSIX_CONTACT_REPORT * pReport = pShapeSet->getContactReport( i );

				getCollisionContext()->QueContactReport( pReport );
			}

			return TRUE;
		}
		break;
	}

	return i3GameObj::OnEvent( event, pObj, param );
}


void CGameCharaPlayer::ProcessMissionEnd( void )
{
	removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	removeEnableStateMask( CHARA_STATE_DISABLE_MOVE);
	removeEnableStateMask( CHARA_STATE_DISABLE_CROUCH);
}


REAL32 CGameCharaPlayer::AnimUpperScale( ID_UPPER_AI ai, CWeaponInfo * pWeaponInfo, REAL32 time)
{
	REAL32 rTimeScale;

	if( m_pAnimContext->getAIUpper() == NULL )
		return 1.f;

	if( time <= 0.f)
		return 1.f;

	I3ASSERT( pWeaponInfo != NULL);
	
	INT32 nID = m_pAnimContext->getAIUpperID( getAnimSet(), pWeaponInfo->m_TypeClass, pWeaponInfo->m_nNumber, getActionContext()->getBody2(), ai);
	i3AIState * pAIState = m_pAnimContext->getAIUpper()->getAIState( nID );

	rTimeScale = pAIState->getDuration() / time;

	return rTimeScale;
}

void CGameCharaPlayer::SetMoveDir( UINT64 key)
{
	if( key & GAME_KEY_MOVE_FORWARD )			setZ( m_pMoveContext->getMoveDir(), 1.f);
	else if( key & GAME_KEY_MOVE_BACKWARD)		setZ( m_pMoveContext->getMoveDir(), -1.f);
	else													setZ( m_pMoveContext->getMoveDir(), 0.f);

	if( key & GAME_KEY_MOVE_LEFT )				setX( m_pMoveContext->getMoveDir(), 1.f);
	else if( key & GAME_KEY_MOVE_RIGHT )		setX( m_pMoveContext->getMoveDir(), -1.f);
	else													setX( m_pMoveContext->getMoveDir(), 0.f);

}

*/

