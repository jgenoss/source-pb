#include "pch.h"
#include "GlobalVariables.h"
#include "GameObject.h"
#include "GameCharaContexts.h"

#include "./Character/GameCharaMoveControl.h"
#include "PBLog.h"
#include "GlobalFunc.h"
#include "StageBattle.h"
#include "GameWorld.h"
#include "TutorialMode.h"
#include "HackDefense.h"
#include "GameMode.h"
#include "StageBattle/AIModeContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"
#include "Weapon/WeaponItemID.h"
#include "GameMaster.h"
#include "BattleObjContext.h"

#include "i3Base/string/ext/contain_string_head.h"

#define	RADIUS_NETWORK	0.45f
#define	TALL_NETWORK	1.5f//0.78f

#define CHARASKINWIDTH 0.085f


I3_CLASS_INSTANCE( PlayerController);

PlayerController::~PlayerController(void)
{
	I3_SAFE_RELEASE( m_pOwner);
}

void PlayerController::FlushHitShape( void)
{
	for( INT32 i = 0; i < getHitShapeCount(); i++)
	{
		CHitInfo * pHitInfo = GetHitInfo( i);

		if( pHitInfo->GetHitShape() != nullptr)
		{
			i3PhysixShapeSet * pShapeSet = reinterpret_cast<i3PhysixShapeSet*>(pHitInfo->GetHitShape()->userData);
			I3ASSERT( pShapeSet != nullptr);
			pShapeSet->setReleaseCallback( nullptr, nullptr);
		}
	}
}

void PlayerController::setOwner( CGameCharaBase * pPlayer)
{	
	I3_REF_CHANGE( m_pOwner, pPlayer); 
	setEventReceiver( pPlayer);
}

void PlayerController::releaseShape( i3ElementBase * pReceiver, NxShape * pShape)
{
	I3ASSERT( i3::same_of<PlayerController*>(pReceiver));
	PlayerController * pCtrl = static_cast<PlayerController*>(pReceiver);
	// shapeset�� �����Ǵ� ��� ���ۿ� �ִ� ���� ��������� �Ѵ�.
	for( INT32 i = 0; i < pCtrl->getHitShapeCount(); i++)
	{
		CHitInfo * pHitInfo = pCtrl->GetHitInfo( i);
		if( pHitInfo->GetHitShape() == pShape)
			pHitInfo->SetHitInfo( nullptr, nullptr);
	}
}

//#define		CHECK_SM_TRESPASS

/*virtual*/ NxControllerAction PlayerController::OnShapeHit( const NxControllerShapeHit * pHit)
{
	#if defined( CHECK_SM_TRESPASS)
	// ������
	// SM Collision�� �հ� �� ��츦 �˻��ϱ� ����,
	// ������ �߻����� ��, Capsule Controller���� �Ÿ��� Ȯ��.
	{
		NxController * pCtrl = pHit->controller;
		REAL32 distViolate;
		VEC3D diff;
		INT32 type;

		if( pCtrl->getType() == NX_CONTROLLER_CAPSULE)
		{
			NxCapsuleController * pCapsule = (NxCapsuleController *) pCtrl;
			VEC3D vmin, vmax, hitPos;
			NxExtendedVec3 pos;

			pos = pCapsule->getPosition();

			vmin.x = vmax.x = (REAL32) pos.x;
			vmin.z = vmax.z = (REAL32) pos.z;
			vmin.y = (REAL32) pos.y - (pCapsule->getHeight() * 0.5f);
			vmax.y = (REAL32) pos.y + (pCapsule->getHeight() * 0.5f);

			hitPos.x = (REAL32) pHit->worldPos.x;
			hitPos.y = (REAL32) pHit->worldPos.y;
			hitPos.z = (REAL32) pHit->worldPos.z;

			// ������ vmin.y���� �۰ų�, vmax���� ũ�ٸ� Capsule�� ���Ʒ� Sphere �������� �Ÿ��� ��
			if( pHit->worldPos.y > vmax.y)
			{
				// ��� Sphere
				i3Vector::Sub( &diff, &vmax, &hitPos);
				type = 0;
			}
			else if( pHit->worldPos.y < vmin.y)
			{
				// �ϴ� Sphere
				i3Vector::Sub( &diff, &vmin, &hitPos);
				type = 1;
			}
			else
			{
				REAL32 t;

				// ���뿡 Hit �� ���̱� ������ �� ������ �ش� �������� �Ÿ��� ��
				i3Math::GetPerpendicularPoint( &vmin, &vmax, &hitPos, &diff, &t);

				i3Vector::Sub( &diff, &diff, &hitPos);

				type = 2;
			}

			distViolate = pCapsule->getRadius() - i3Vector::Length( &diff);

			if( i3Math::abs( distViolate) > pCapsule->getRadius())
			{
				I3TRACE( "VIOLATE(%d) : %f\n", type, distViolate);
			}
		}
	}
	#endif

	// ������Ʈ�� �ø��� �Ǹ� ������Ʈ�� ��ġ�� ���ߴ��� Ȯ���մϴ�.
	if( pHit->shape != nullptr && pHit->shape->userData != nullptr)
	{
		i3ElementBase * pOwner = (i3ElementBase *) pHit->shape->userData;

		if( !i3::same_of<i3PhysixShapeSet*>(pOwner))
			return NX_ACTION_NONE;

		((i3PhysixShapeSet *)pOwner)->setReleaseCallback( this, releaseShape);
		
		m_HitNormal.x = pHit->worldNormal.x; m_HitNormal.y = pHit->worldNormal.y; m_HitNormal.z = pHit->worldNormal.z;

		{
			m_HitDir.x = pHit->dir.x; m_HitDir.y = pHit->dir.y; m_HitDir.z = pHit->dir.z;

			//m_rLength = pHit->length;
		}

		if( m_nHitShapeCount < CONTROLLER_HIT_INFO_COUNT)
		{
			m_HitInfo[m_nHitShapeCount].SetHitInfo(pHit->shape, &m_HitNormal);
			m_nHitShapeCount++;
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN, "m_nHitShapeCount >= CONTROLLER_HIT_INFO_COUNT Group %d id %d", pHit->shape->getGroup(), pHit->id);
		}

		m_HitPos.x = (REAL32)pHit->worldPos.x; m_HitPos.y = (REAL32)pHit->worldPos.y; m_HitPos.z = (REAL32)pHit->worldPos.z;

		if( m_nHitWorldPosCount < CONTROLLER_WORLD_HIT_POS_COUNT)
		{
			m_HitWorldPos[m_nHitWorldPosCount].SetWorldHitInfo(&m_HitPos, &m_HitNormal);
			m_nHitWorldPosCount++;
		}
		else
		{
			m_nHitWorldPosCount = 0;
			InitWorldHitInfoArray();
		}
	}

	return NX_ACTION_NONE;
}

/*virtual*/	NxControllerAction PlayerController::OnControllerHit( const NxControllersHit * pHit)
{
	// ��Ʈ�ѷ� ���� ��Ʈ�� �Ǹ� �о���մϴ�.
	// �浹 ó�� �� ��ġ�� �ٽ� ����� �ʿ䰡 �ֽ��ϴ�..
	/*
	if( pHit->controller != nullptr )
	{
		NxExtendedVec3 pos;

		pos = pHit->controller->getPosition();

		VEC3D vecOther;
		VEC3D vDir;
		VEC3D * pVecMyPos = g_pPlayer->m_pOwner->GetPos();

		i3PhysixController * pController = (i3PhysixController*) pHit->controller->getUserData();

		setX( &vecOther, (REAL32) pos.x);
		setY( &vecOther, (REAL32) pos.y - pController->getHeight() );
		setZ( &vecOther, (REAL32) pos.z);

		i3Vector::Sub( &vDir, pVecMyPos, &vecOther );
		REAL32 rLength = i3Vector::Normalize( &vDir, &vDir );
		if( rLength < 0.7f )
		{
			rLength = (1.f - rLength);
			i3Vector::Scale( &vDir, &vDir, rLength );
			i3Vector::Add( &vDir, &vDir, pVecMyPos );
			g_pPlayer->forceSetPos( &vDir );
		}
	}
	*/
	//I3TRACE( "CONTROLLER HIT\n");

	m_pHitUser = nullptr;

	if( ( getOwner()->isLocal() || getOwner()->isAI()) &&
		pHit->controller != nullptr)
	{
		if( i3::same_of<PlayerController*>((i3EventReceiver*)pHit->controller->getUserData()))
		{
			PlayerController * pController = (PlayerController*)pHit->controller->getUserData();
			if( getOwner() != pController->getOwner() )
				m_pHitUser = pController->getOwner();
		}
	}

	return NX_ACTION_NONE;
}


I3_CLASS_INSTANCE( CGameCharaMoveContext);

CGameCharaMoveContext::CGameCharaMoveContext() 
{
	addMoveFlag( CHARA_MOVE_FLAG_TOUCHDOWN); //EscapeMission ���ӵ� �浹üũ�� ���� ���ھ� �ߺ�üũ�� ���� ���� �����մϴ�.

}

CGameCharaMoveContext::~CGameCharaMoveContext()
{
	OnDestroy();
}

INT32 CGameCharaMoveContext::getCharaNetIndex( void)
{
	if( m_pOwner)
		return m_pOwner->getCharaNetIndex();

	return -1;
}

void CGameCharaMoveContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	m_pCurrentMoveControl = nullptr;
	m_pMoveControl = CGameCharaMoveControl::new_object();
	m_pMoveControl->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

void CGameCharaMoveContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	ReleaseController();

	m_pCurrentMoveControl = nullptr;
	I3_SAFE_RELEASE( m_pMoveControl );

	I3_SAFE_DELETE(m_Accel);
	I3_SAFE_DELETE(m_VelCharacter);
	I3_SAFE_DELETE(m_OldVel);
	I3_SAFE_DELETE(m_TakeOffPos);				// Jump start position
	I3_SAFE_DELETE(m_RideLocalPos);
	I3_SAFE_DELETE(m_RideSyncPos);
	I3_SAFE_DELETE(m_vOldPos);
	I3_SAFE_DELETE(m_vPostPosition);
	I3_SAFE_DELETE(m_vRecvPosition);
	I3_SAFE_DELETE(m_matRideObjectPose);
}

void CGameCharaMoveContext::OnReset( void)
{
	CGameCharaContextBase::OnReset();

	i3Vector::Zero( &m_MoveDir);
	i3Vector::Copy( &m_MoveAt, i3Matrix::GetAt( m_pOwner->getSceneObject()->GetMatrix()));
	i3Vector::Copy( &m_MoveRight, i3Matrix::GetRight( m_pOwner->getSceneObject()->GetMatrix()));
	i3Vector::Zero( m_Accel);
	i3Vector::Zero( m_VelCharacter);
	i3Vector::Zero( m_OldVel );

	m_rVelocity			= 0.f;
	m_tmStopingElapsed  = 0.f;
	m_tmAccelElapsed	= 0.f;
	m_rMaxMoveSpeed		= 0.f;

	m_rElapsedSpeedReduction = 0.f;
	m_rSpeedReductionRate = 0.f;
	m_bSpeedReductionEternal = false;

	i3Vector::Zero( m_TakeOffPos);			// Jump start position
	i3Vector::Zero( &m_ForceJumpDir);			// Jump start speed
	i3Vector::Zero(&m_ForceJumpCharaVel);			// Jump start speed
	m_rForceJumpSpeed = 0.f;
	m_rFallOldPosY = 0.f;
	m_rJumpUpOldTime = 0.f;
	m_rElapsedTakeOffTime	= 0.f;

	if( m_pOwner->getDBInfo() != nullptr)
	{
		m_rCurJumpHeight		= m_pOwner->getDBInfo()->GetJumpHeight();
		m_rCurJumpTime			= m_pOwner->getDBInfo()->GetJumpTime() * 0.5f;
		if( m_rCurJumpTime > 0.f)
			m_rHalfAccelOfGravity	= m_rCurJumpHeight / (m_rCurJumpTime * m_rCurJumpTime);
		else
			m_rHalfAccelOfGravity = 0.f;
	}
	else
	{
		m_rCurJumpHeight = 0.f;
		m_rCurJumpTime = 0.f;
		m_rHalfAccelOfGravity = 0.f;
	}

	m_CurFloorTerrainType	= I3_TERRAIN_NA;
	m_rFloorCheckTimer		= 0.f;

	m_rFloorLength			= 0.f;

	m_pRideObject			= nullptr;
	m_pOldRideObject		= nullptr;
	
	m_pPrevJumpHold			= nullptr;

	m_iRideObjectIndex		= -1;
	i3Matrix::Identity( m_matRideObjectPose);
	//g_pLog->Log(g_pFramework->GetAccTime(), "CGameCharaMoveContext::OnReset()\n");

	ResetController();

	m_rDamageTime = 0.f;
	i3Vector::Zero(&m_v3ContactObjectVelToChara);

	m_bGravityRecovery = false;
}

void CGameCharaMoveContext::OnChangeLocal()
{
	CGameCharaContextBase::OnChangeLocal();

	m_pCurrentMoveControl = m_pMoveControl;
}

void CGameCharaMoveContext::OnChangeNetwork()
{
	CGameCharaContextBase::OnChangeNetwork();

	m_pCurrentMoveControl = m_pMoveControl;
}

void CGameCharaMoveContext::OnChangeNPC( void)
{
	CGameCharaContextBase::OnChangeNPC();
}

void CGameCharaMoveContext::OnChangeAI( void)
{
	m_pCurrentMoveControl = m_pMoveControl;
	
	if( IsNetworkController() )
	{
		ReleaseController();	// ��Ʈ��ũ�� Control���� AI������ ����
		CreateController();
	}
}

void CGameCharaMoveContext::OnChangeFullbody( void)
{
	CGameCharaContextBase::OnChangeFullbody();

	m_pCurrentMoveControl = m_pMoveControl;
}

void CGameCharaMoveContext::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaContextBase::OnUpdate( rDeltatime);

	if( m_pCurrentMoveControl == nullptr)
		return;

	m_pCurrentMoveControl->OnUpdate( rDeltatime);

	//if( m_pController != nullptr)
	//	((PlayerController*)m_pController)->FlushHitShape();
}


//NxCCDSkeleton* CreateCCDSkeleton(REAL32 rSize, REAL32 rHeight)
//{
//	NxU32 triangles[3 * 12] = { 
//		0,1,3,
//		0,3,2,
//		3,7,6,
//		3,6,2,
//		1,5,7,
//		1,7,3,
//		4,6,7,
//		4,7,5,
//		1,0,4,
//		5,1,4,
//		4,0,2,
//		4,2,6
//	};
//
//	NxVec3 points[8];
//
//	// Static mesh
//	points[0].set( size, -size, -size);
//	points[1].set( size, -size,  size);
//	points[2].set( size,  size, -size);
//	points[3].set( size,  size,  size);
//
//	points[4].set(-size, -size, -size);
//	points[5].set(-size, -size,  size);
//	points[6].set(-size,  size, -size);
//	points[7].set(-size,  size,  size);
//
//	NxSimpleTriangleMesh stm;
//	stm.numVertices = 8;
//	stm.numTriangles = 6*2;
//	stm.pointStrideBytes = sizeof(NxVec3);
//	stm.triangleStrideBytes = sizeof(NxU32)*3;
//
//	stm.points = points;
//	stm.triangles = triangles;
//	stm.flags |= NX_MF_FLIPNORMALS;
//	return gPhysicsSDK->createCCDSkeleton(stm);
//}
//
//NxActor* CGameCharaMoveContext::CreateCCDBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density, bool doDynamicCCD)
//{
//	// Add a single-shape actor to the scene
//	NxActorDesc actorDesc;
//	NxBodyDesc bodyDesc;
//
//	// The actor has one shape, a box
//	NxBoxShapeDesc boxDesc;
//	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
//	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
//
//	// Set box up with CCD skeleton
//	boxDesc.ccdSkeleton = CreateCCDSkeleton(boxDim.x*0.8f);
//	if (doDynamicCCD)  boxDesc.shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD;  // Activate dynamic-dynamic CCD for this body
//
//	actorDesc.shapes.pushBack(&boxDesc);
//
//	if (density)
//	{
//		actorDesc.body = &bodyDesc;
//		actorDesc.density = density;
//	}
//	else
//	{
//		actorDesc.body = nullptr;
//	}
//	actorDesc.globalPose.t = pos;
//	return gScene->createActor(actorDesc);	
//}


//
void CGameCharaMoveContext::CreateCCD(VEC3D* pv3Pos, VEC3D* pv3Size, int iCGRP)
{
	/*m_pCCD_Box = i3PhysixShapeSet::new_object();
	m_pCCD_Box->CreateBoxShapeCCD( pv3Pos, pv3Size, iCGRP, true);
	m_pCCD_Box->getNxActor()->setGroup( iCGRP);
	m_pCCD_Box->SetName( "test" );
	m_pCCD_Box->getNxShape(0)->setName( "test22");*/
}

bool CGameCharaMoveContext::IsNetworkController( void)
{
	if( m_pController != nullptr)
	{
		NxCapsuleController * pCtrl = (NxCapsuleController*) m_pController->getNxController();
		REAL32 rad = pCtrl->getRadius() - RADIUS_NETWORK;
		if( rad < 0.001f && rad > -0.001f )
		{
			return true;
		}
	}

	return false;
}

void CGameCharaMoveContext::CreateController()
{
	NxCapsuleControllerDesc capsuledesc;

	capsuledesc.setToDefault();

	capsuledesc.stepOffset = 0.27f;	// ���� ���̰� 40��Ƽ �̻��̸� ���� ���մϴ�.
	capsuledesc.slopeLimit = i3Math::cos( I3_DEG2RAD( 45));	// ��簢�� 45�̻��̸� �̵����� ���ϵ��� �մϴ�.
	
	if( m_pOwner->IsDino())
	{
		capsuledesc.radius = m_pOwner->getDBInfo()->GetDinoCCRadius();
		capsuledesc.height = max( 0.1f, m_pOwner->getDBInfo()->GetDinoCCTall());
		capsuledesc.stepOffset = m_pOwner->getDBInfo()->GetDinoCCStepOffset();
		capsuledesc.position.set( 0.0f, 0.0f, 0.0f);
		capsuledesc.skinWidth = CHARASKINWIDTH;
	}
	else if(m_pOwner->isLocal()  || m_pOwner->isAI())
	{
		capsuledesc.radius = RADIUS;
		capsuledesc.height = max( 0.1f, CAPSULE_FULL_HEIGHT);
		capsuledesc.skinWidth = CHARASKINWIDTH;
		capsuledesc.stepOffset = 0.4f;	// ���� ���̰� 40��Ƽ �̻��̸� ���� ���մϴ�.
		capsuledesc.position.set( 0.0f, 0.0f, 0.0f);
	}
	else if( m_pOwner->isNetwork() )
	{
		capsuledesc.radius = RADIUS_NETWORK;
		capsuledesc.height = max( 0.1f, TALL_NETWORK);
		capsuledesc.skinWidth = 0.4f;
		capsuledesc.position.set( 0.0f, 0.3f, 0.0f);
	}
	else
	{
		capsuledesc.radius = 0.7f;
		capsuledesc.height = 1.8f;
		capsuledesc.skinWidth = 0.4f;
		capsuledesc.position.set( 0.0f, 0.3f, 0.f);
	}

	if( m_pOwner->isAI())
	{
		capsuledesc.stepOffset = 0.25f;						// ai�� �ΰɸ� ������ �� �����ϰ��Ͽ� ������ �����մϴ�.
	}

	I3ASSERT( capsuledesc.isValid());

	m_pController = PlayerController::new_object();	//g_pCharaManager->AllocPhysXController( m_pOwner);
	I3ASSERT( m_pController != nullptr);

	if(m_pOwner->isLocal()  || m_pOwner->isAI())
	{
		if( m_pOwner->IsRex() == false && m_pOwner->IsDomiMutantRex() == false )
		{
			m_pController->Create( CGRP_MYCHARA_BOUND, (NxControllerDesc*)&capsuledesc);
		}
		else
		{
			m_pController->Create( CGRP_TREX_BOUND, (NxControllerDesc*)&capsuledesc);
		}
	}
	else
	{
		if( m_pOwner->IsRex()  || m_pOwner->IsDomiMutantRex())
		{
			m_pController->Create( CGRP_TREX_BOUND, (NxControllerDesc*)&capsuledesc);
		}
		else
		{
			m_pController->Create( CGRP_CHARA_BOUND, (NxControllerDesc*)&capsuledesc);
		}
	}

	((PlayerController*)(i3PhysixController*)m_pController)->setOwner( m_pOwner);

	NxActor * pAct = m_pController->getActor();
	if( pAct == nullptr )
	{
		I3PRINTLOG(I3LOG_WARN,  "Actor is nullptr." );
		return;
	}
	
	if(m_pOwner->isLocal() || m_pOwner->isAI())
	{
		m_pController->MakeCapsuleTrigger();
	}

	pAct->raiseBodyFlag( NX_BF_DISABLE_GRAVITY );
}

void CGameCharaMoveContext::ReleaseController()
{
	if( m_pController != nullptr)
	{
		((PlayerController*)(i3PhysixController*)m_pController)->setOwner(nullptr);	// CharaBase�� ����
	}
	I3_SAFE_RELEASE( m_pController);
}

void CGameCharaMoveContext::ResetController()
{
	if( m_pController == nullptr )
		return;

	((PlayerController*)(i3PhysixController*)m_pController)->setHitShapeCount( 0);
	((PlayerController*)(i3PhysixController*)m_pController)->SetHitWorldShapeCount(0);

	NxActor * pAct = m_pController->getActor();
	if( pAct == nullptr )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Actor is nullptr." );
		return;
	}

	pAct->clearBodyFlag( NX_BF_DISABLE_GRAVITY );
}

void CGameCharaMoveContext::EnableController( bool bVal)
{
#ifdef DOMI_OPTIMIZE
	if (m_pController == nullptr) return;
#endif

	if( bVal )
	{
		if( m_pOwner->IsRex() == false &&  m_pOwner->IsDomiMutantRex() == false)
		{
			if(m_pOwner->isLocal()  || m_pOwner->isAI())
				m_pController->setGroup( CGRP_MYCHARA_BOUND );
			else
				m_pController->setGroup( CGRP_CHARA_BOUND );
		}
		else
		{
			m_pController->setGroup( CGRP_TREX_BOUND );
		}
	}
	else
		m_pController->setGroup( CGRP_DUMMY );
}


void CGameCharaMoveContext::ChangeControllerHeight( REAL32 rHeight )
{	
	I3ASSERT( m_pController != nullptr );

	REAL32 rTall = (rHeight * 0.5f) + RADIUS;

	if( m_pController->getHeight() == rTall )
		return;

	m_pController->SetCenterHeightAndHeight((rHeight * 0.5f) + RADIUS, rHeight);
}

void CGameCharaMoveContext::CheckWorldCollision( REAL32 rDeltatime)
{	
	CStageBattle* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();

	if( m_pController == nullptr )
	{
		I3PRINTLOG(I3LOG_WARN,  "m_pController is nullptr." );
		return;
	}

	REAL32	y		= m_VelCharacter->y;
	m_VelCharacter->y			= 0.0f;

	bool force_process_disable = false;
	if (CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE) && m_pOwner->isCharaStateMask(CHARA_STATE_JUMP))
		force_process_disable = true;
		
	if ( i3Vector::LengthSq(m_Accel) > I3_EPS )
	{
		//�ӵ� ���� ���μ����� ó�� �κ�.
		VEC3D accl, vVel, vAccel;
		REAL32 rVel, rDrive, delta_offset_increas, maxSpeed, fSpeed;

		maxSpeed = delta_offset_increas = rDrive = rVel = fSpeed = 0.0f;
		i3Vector::Normalize(&vAccel, m_Accel);

		if( !m_pOwner->isCharaStateMask(CHARA_STATE_FORCEJUMP) )
		{
			//���� ���� �̵�, ���� �̵� ���� ���μ���.

			rVel = i3Vector::Length(m_VelCharacter);
			if (rVel > I3_EPS)	i3Vector::Normalize(&vVel, m_VelCharacter);
			else				i3Vector::Copy(&vVel, m_VelCharacter);

			rDrive = i3Vector::Dot(&vVel, &vAccel);
			delta_offset_increas = rDeltatime;

			if (pStage != nullptr && CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE))
				delta_offset_increas *= pStage->GetStageAddAcceleration_Up_Speed_late();

			if (rVel > I3_EPS)//�̵� �ϴ� ���̾��ٸ�
			{
				m_tmAccelElapsed += delta_offset_increas * (rDrive - m_pOwner->getDBInfo()->GetAccelRateBackVsFront());		//���� GetAccelRateBackVsFront �̰Ŵ� �۵� ���ϴ°� Ȯ��������, �ϴ� ���� ��.
				if (rDrive < 0.0f) m_tmAccelElapsed -= delta_offset_increas;
			}
			else
			{
				m_tmAccelElapsed += delta_offset_increas;	//�ӵ��� �ſ� �������� ���ӵ��� �ſ� ������츦 ���� += �� ���.
			}

			m_tmAccelElapsed = MINMAX(0.0f, m_tmAccelElapsed, m_pOwner->GetAccelerationTimeAddOffset());

			i3Vector::Scale(&accl, m_Accel, delta_offset_increas);

			if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB))
				i3Vector::Copy(m_VelCharacter, &accl);
			else
				i3Vector::Add(m_VelCharacter, m_VelCharacter, &accl);

			i3Vector::Normalize(m_VelCharacter, m_VelCharacter);
			maxSpeed = m_rMaxMoveSpeed;
			if (rVel > I3_EPS && rDrive <= 0.0f && m_rOldMaxMoveSpeed > 0.0f)
				maxSpeed = MIN((REAL32)m_rOldMaxMoveSpeed, (REAL32)m_rMaxMoveSpeed);

			if (isEnabledSpeedReduction())
				maxSpeed *= m_rSpeedReductionRate;

			fSpeed = maxSpeed * i3Math::sin((m_tmAccelElapsed / m_pOwner->GetAccelerationTimeAddOffset()) * I3_PI2);
			i3Vector::Scale(m_VelCharacter, m_VelCharacter, fSpeed * delta_offset_increas);
		}
		else
		{
			//���� ���� �̵� ���μ���. 
			//m_ForceJumpCharaVel�� ����ϰ� �����ؼ� �߰������� ���� �ӵ� �����ִ� �������� ����.
			rVel = i3Vector::Length(&m_ForceJumpCharaVel);
			if (rVel > I3_EPS) i3Vector::Normalize(&vVel, &m_ForceJumpCharaVel);
			else			   i3Vector::Copy(&vVel, &m_ForceJumpCharaVel);

			rDrive = i3Vector::Dot(&vVel, &vAccel);

			if (rVel > I3_EPS)//�̵� �ϴ� ���̾��ٸ�
			{
				m_tmAccelElapsed += rDeltatime * rDrive * 0.5f;
				if (rDrive < 0.0f) m_tmAccelElapsed -= rDeltatime; //�а�
			}
			else
			{
				m_tmAccelElapsed += rDeltatime;	//�ӵ��� �ſ� �������� ���ӵ��� �ſ� ������츦 ���� += �� ���.
			}

			m_tmAccelElapsed = MINMAX(0.0f, m_tmAccelElapsed, 1.0f);	//�ϴ��� ���� �̵� �� ���� ������ ���.
			i3Vector::Scale(&accl, m_Accel, rDeltatime);
			i3Vector::Add(&m_ForceJumpCharaVel, &m_ForceJumpCharaVel, &accl);

			i3Vector::Normalize(&m_ForceJumpCharaVel, &m_ForceJumpCharaVel);
			maxSpeed = m_rMaxMoveSpeed;
			if (rVel > I3_EPS && rDrive <= 0.0f && m_rOldMaxMoveSpeed > 0.0f)
				maxSpeed = MIN((REAL32)m_rOldMaxMoveSpeed, (REAL32)m_rMaxMoveSpeed);
			
			fSpeed = maxSpeed * i3Math::sin((m_tmAccelElapsed / 1.0f) * I3_PI2);			//�����ӿ� ���� ���� �̵� �ӵ� ��� �κ�.
			i3Vector::Scale(&m_ForceJumpCharaVel, &m_ForceJumpCharaVel, fSpeed * rDeltatime);
			
			REAL32 rFactor = rDeltatime * 60.0f;
			VEC3D force_speed;
			i3Vector::Scale(&force_speed, &m_ForceJumpDir, (m_rForceJumpSpeed * 0.017f * I3_PI2) * rFactor);

			i3Vector::Add( m_VelCharacter, &force_speed, &m_ForceJumpCharaVel);	//���� ĳ���� �ӵ��� ���� �κ�.
		}
	}
	else
	{
		if (!force_process_disable)
		{
			//�ӵ� ���� ���μ����� ó�� �κ�.
			REAL32 delta_offset_decreas = rDeltatime;

			if (pStage != nullptr && CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE))
				delta_offset_decreas *= pStage->GetStageAddAcceleration_Down_Speed_late();

			m_tmAccelElapsed -= delta_offset_decreas;
			if (m_pOwner->IsRex()) m_tmAccelElapsed -= delta_offset_decreas;

			if (m_tmAccelElapsed > 0.0f)
			{
				i3Vector::Scale(m_VelCharacter, m_VelCharacter, i3Math::sin((m_tmAccelElapsed / m_pOwner->GetAccelerationTimeAddOffset()) * I3_PI2));

				REAL32 len = i3Vector::Length(m_VelCharacter);
				if (len > I3_EPS)
				{
					i3Vector::Normalize(m_VelCharacter, m_VelCharacter);
					//���� �̵� �ӵ� ������ ������ �̷��� ����ó���� ���ܵӴϴ�. �ΰ��ؼ� �Ȱǵ帮�ڽ��ϴ�.

					if (CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE) || pStage->GetStageAddAcceleration_Down_Speed_late() == 1.0f)
						len = MIN(len, m_rMaxMoveSpeed * rDeltatime);

					i3Vector::Scale(m_VelCharacter, m_VelCharacter, len);
				}

				if (len < 0.001)
					m_tmAccelElapsed = 0.0f;
			}
			else
			{
				i3Vector::Zero(m_VelCharacter);
				m_tmAccelElapsed = 0.0f;
			}
		}
	}

	if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB) == false )
	{
		m_VelCharacter->y = y;
	}
	else
	if ( IsJumpingOnLadder(m_ladder_info) )
	{
		m_VelCharacter->y = y;
	}
}

void CGameCharaMoveContext::UpdateTouchDown(REAL32 rDeltatime)
{
//EscapeMission ��ġ�ٿ �����̸� �־� �������� ������ ������
	if(GetTouchdown()) //��ġ�ٿ��� �� �� �ִ� �����̸� ���� ��Ų��.
		return;
	
	m_fDelayTime = m_fDelayTime - rDeltatime;

	if( m_fDelayTime <= 0.0f)
	{
		SetTouchdown( true);
		SetDeleyTime( 10.0f);
	}

}
void CGameCharaMoveContext::ResetJumping(void)
{
	m_pController->getPosition( m_TakeOffPos);
	i3Vector::Zero(&m_ForceJumpDir);			// Jump start speed
	i3Vector::Zero(&m_ForceJumpCharaVel);		// Jump start speed

	m_rForceJumpSpeed		= 0.f;
	m_rFallOldPosY			= 0.f;
	m_rJumpUpOldTime		= 0.f;
	m_rElapsedTakeOffTime	= 0.f;

	m_pOwner->removeCharaStateMask( CHARA_STATE_JUMP);
	m_pOwner->removeCharaStateMask(	CHARA_STATE_JUMPING_UP);
	m_pOwner->removeCharaStateMask(	CHARA_STATE_JUMPING_DOWN);
	m_pOwner->removeCharaStateMask( CHARA_STATE_FORCEJUMP);
	
	if (m_pOwner->isLocal() && (m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->GetMySlotIdxEx()))
		m_pPrevJumpHold = nullptr;
}

void CGameCharaMoveContext::UpdateFallDamage( REAL32 rDeltatime)
{
	//	���� ������ ���
	if(EnableKill() && (m_rElapsedTakeOffTime > rDeltatime))
	{
		if( m_rElapsedTakeOffTime > 0.1f)
		{
			INT32 nDamage = _CalcFallingDamage( m_TakeOffPos);

			if( m_pOwner != nullptr )
			{
				if(m_pOwner->isAI() && MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
					nDamage = 0;

				// �� ��ġ���� Flag�� ������.
				if (MyRoomInfoContext::i()->getStage()->IsSpace() && m_bGravityRecovery == false)
					nDamage = 0;
			}

			if( nDamage > 0)
				m_pOwner->Cmd_DamageSelf( nDamage, 0xFFFF, true);
		}
	}

	if( m_rElapsedTakeOffTime > rDeltatime)
	{
		getSoundContext()->_PlayJumpEndSound();
	}

	ResetJumping();
	
	//I3TRACE( "in Floor..%.4f\n", g_pViewer->getDeltaTime());
}

REAL32 CGameCharaMoveContext::CalrFallVel(VEC3D& newPos, REAL32 rFac, REAL32 rDeltatime)
{
	REAL32 rLen = 0.f;
	REAL32 rUp = 0.f;

	if (m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_GRAVITY) )		// �ϴ� ��ٸ� Ʈ���� ���ο����� �߷��۵��� �����ϵ��� ó��...(2014.12.04.����)
	{
		rLen = 0;
		rUp = 0.f;
	}
	else
	{
		rLen = m_rHalfAccelOfGravity * m_rElapsedTakeOffTime * m_rElapsedTakeOffTime;
		rUp = m_rFallOldPosY - rLen;
	}

	if(rUp > 0) 
	{
		I3ASSERT_0;
	}

	m_rFallOldPosY = rLen;
	
	return rUp;
}

void CGameCharaMoveContext::UpdateJumping(REAL32 rDeltatime)
{
	I3ASSERT( m_pOwner->getDBInfo() != nullptr);

	REAL32 rNextHeight = 0.0f;
	REAL32 rFac = 0.f;
	VEC3D  newPos;
	m_pController->getPosition(&newPos);
	REAL32 rUp = 0.0f;

	if ( !(MyRoomInfoContext::i()->getStage()->IsSpace()) )
	{
		if (m_pOwner->isAI() && MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD))
		{
			m_rCurJumpHeight = g_pTempConfig->m_JumpHeight;
			m_rCurJumpTime = g_pTempConfig->m_JumpTime * 0.5f;
			m_rHalfAccelOfGravity = m_rCurJumpHeight / (m_rCurJumpTime * m_rCurJumpTime);
		}
		else if (m_pOwner->isAI() && MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
		{
			INT32 charaNetIndex = getCharaNetIndex();
			if (charaNetIndex < 0 || charaNetIndex >= SLOT_MAX_COUNT)
			{
				I3_BOUNDCHK(charaNetIndex, SLOT_MAX_COUNT);
			}
			else
			{
				AI_ENTRY *		pAiEntry = &AIModeContext::i()->AIEntry[charaNetIndex];
				AI_BIND_DATA *  pBindData = g_pCharaManager->getAiBindData(pAiEntry->_number);

				m_rCurJumpHeight = pBindData->_Jump_Height;
				m_rCurJumpTime = pBindData->_Jump_Time * 0.5f;
				m_rHalfAccelOfGravity = m_rCurJumpHeight / (m_rCurJumpTime * m_rCurJumpTime);
			}
		}

		if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && (m_rElapsedTakeOffTime == 0.0f))
		{
			m_VelCharacter->y = i3Math::sin((rDeltatime / m_rCurJumpTime) * I3_PI2) * m_rCurJumpHeight;

			GetPosition(m_TakeOffPos);

			m_rElapsedTakeOffTime = rDeltatime;

			return;
		}

		if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && ((m_rElapsedTakeOffTime + rDeltatime) > m_rCurJumpTime))
		{
			ResetJumping();
		}
		else
		{
			if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH) == false)
			{
				m_rElapsedTakeOffTime = m_rElapsedTakeOffTime + rDeltatime;
			}
		}

		/////////////////////////////////////////////////////////////////////////////////
		// Process ����
		if (m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR))
		{
			UpdateFallDamage(rDeltatime);

			setY(m_VelCharacter, -0.1f);
		}
		else
		{
			//���� �����ӿ��� ����ߴ� ���� m_pOwner->GetPos()���� ����������� �ݿ����� ���ϴ� ��찡 ���� ������
			//���� getPosition( &newPos)������ ����vel�������� �̵�ġ�� �ݿ��Ǿ��ٰ� ����Ҽ��� �����ϴ�.
			rFac = (m_rElapsedTakeOffTime / m_rCurJumpTime) * I3_PI2;

			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP))
			{
				rNextHeight = getY(m_TakeOffPos) + (i3Math::sin(rFac) * m_rCurJumpHeight);
				rUp = MAX(0.0f, rNextHeight - getY(&newPos));
			}
			else
			{
				rUp = CalrFallVel(newPos, rFac, rDeltatime);
			}

			setY(m_VelCharacter, rUp);

			if (EnableKill())
			{
				if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST)
				{
					// �ʿ��� ������ ��� ����.
					if (m_pOwner->GetPos()->y < -170.f)
					{
						m_pOwner->Cmd_DamageSelf(255);
					}
				}
				else
				{
					// �ʿ��� ������ ��� ����.
					if (m_pOwner->GetPos()->y < -30.f)
					{
						m_pOwner->Cmd_DamageSelf(255);
					}
				}
			}
		}
	}
	else
	{
		//�� ���� ���μ���. �����̽� ��� ������ ���Ͽ� ���� ����ϴ�.
		//������ �߿��Ͽ� ���� �ٲ� �� ���� ������ �ٲ��ּ���.

		CStageBattle* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		if (pStage == nullptr) return;

		m_rHalfAccelOfGravity = pStage->GetStageGravity();
		REAL32 Chara_Up_Gravity_Speed_late = pStage->GetStage_Chara_Up_Gravity_Speed_late();
		REAL32 Chara_Down_Gravity_Speed_late = pStage->GetStage_Chara_Down_Gravity_Speed_late();
		REAL32 chara_fall_gravity_late = pStage->GetStage_Chara_Fall_Gravity_late();

		// �߷º��� ������ ���� �ÿ��� �ٲ��ݴϴ�.
		if (m_bGravityRecovery)
		{
			m_rHalfAccelOfGravity = pStage->GetGrageGravityRestore();//m_rCurJumpHeight / (m_rCurJumpTime * m_rCurJumpTime);
			Chara_Up_Gravity_Speed_late = 1.0f;
			Chara_Down_Gravity_Speed_late = 1.0f;
			chara_fall_gravity_late = 1.0f;
		}

		{// ���� ���� ������. �� ���ǿ� ������ �ؿ� ��ƾ�� ���� ����ϰ� ���� �����Ӻ��� ����.
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && (m_rElapsedTakeOffTime == 0.0f))
			{
				REAL32 rFactor = rDeltatime * 60.0f;

				if (m_pOwner->isCharaStateMask(CHARA_STATE_FORCEJUMP))
				{
					//�����ӿ� ���� ���� ���� �̵� �ӵ� ��� �κ�.
					VEC3D force_speed;
					i3Vector::Scale(&force_speed, &m_ForceJumpDir, (m_rForceJumpSpeed * 0.017f * I3_PI2) * rFactor);
					i3Vector::Copy(m_VelCharacter, &force_speed);
				}

				m_VelCharacter->y = (m_rCurJumpHeight * 0.017f * I3_PI2) * rFactor;
				m_rElapsedTakeOffTime = rDeltatime;
				GetPosition(m_TakeOffPos);
				m_pOwner->addCharaStateMask(CHARA_STATE_JUMPING_UP);
					
				return;
			}
		}

		{// ���� ���� �˻� �� ������Ʈ �ð� ���� �κ�.
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR)) 
			{
				// ���� ����ÿ� ������ ó�� �� �� �� �մϴ�....
				UpdateFallDamage(rDeltatime);
				m_bGravityRecovery = false;

				ResetJumping();
			}
			else if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH) == false)									
				m_rElapsedTakeOffTime = m_rElapsedTakeOffTime + rDeltatime;
		}

		// Process ���� , Process ����, Process ���� �� ���� �Ϸ�
		REAL32 gravity_delta = 0.0f;		//�߷� ����
		REAL32 jump_delta = 0.0f;		//�� ����
		if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP)) //�����ΰ� ���� �� ��츦 ������ ���� �ؼ� �ش� �ൿ ���μ��� ó��
		{
			//rFac�� ��꿡 �� �ð��̶�� ��������.
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMPING_UP))
			{
				rFac = (m_rElapsedTakeOffTime)* I3_PI2 * Chara_Up_Gravity_Speed_late;
			}
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMPING_DOWN))
			{
				REAL32 only_down_rFac = (m_rElapsedTakeOffTime - m_rJumpUpOldTime) * I3_PI2 * Chara_Down_Gravity_Speed_late;
				rFac = (m_rJumpUpOldTime)* I3_PI2 * Chara_Up_Gravity_Speed_late + only_down_rFac;
			}

			gravity_delta	= ((m_rHalfAccelOfGravity * rFac * rFac) * 0.5f);
			jump_delta		= (rFac * m_rCurJumpHeight);

			rNextHeight = getY(m_TakeOffPos) - (gravity_delta)+(jump_delta);
			rUp = rNextHeight - getY(&newPos);
			setY(m_VelCharacter, rUp);

			if ( m_pOwner->isCharaStateMask(CHARA_STATE_JUMPING_UP) && 
				 (gravity_delta >= (jump_delta*0.5f)) )
				{//���� ��� ���������� �ϰ� �������� �ٲٴ� �κ�.
					m_pOwner->removeCharaStateMask(CHARA_STATE_JUMPING_UP);
					m_pOwner->addCharaStateMask(CHARA_STATE_JUMPING_DOWN);
					m_rJumpUpOldTime = m_rElapsedTakeOffTime;
				}
		}
		else if (!m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR)) //�ܼ��� �ٴ��� ���� �������� ���� ó��
		{
			//rFac�� ��꿡 �� �ð��̶�� ��������.
			rFac = (m_rElapsedTakeOffTime) * I3_PI2 * chara_fall_gravity_late;

			gravity_delta = (((m_rHalfAccelOfGravity * rFac * rFac)) * 0.5f);

			rNextHeight = getY(m_TakeOffPos) - gravity_delta;
			rUp = rNextHeight - getY(&newPos);
			setY(m_VelCharacter, rUp);
		}
		else //�Ϸ� ó��. ������ �ƴϰ� ���ϵ� �ƴϴϱ��.
		{
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMPING_DOWN)) m_pOwner->removeCharaStateMask(CHARA_STATE_JUMPING_DOWN);
			UpdateFallDamage(rDeltatime);
			// �׷���Ƽ �����մϴ�.
			m_bGravityRecovery = false;
			setY(m_VelCharacter, -0.1f);
		}

		// �ʿ��� ������ ��� ���̴� ��ƾ. ��� ��쿡�� �ش� ��ƾ�� �˻��ؾ������ ����.
		if (EnableKill())
		{
			if ( MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST )
			{
				if (m_pOwner->GetPos()->y < -170.f) m_pOwner->Cmd_DamageSelf(255);
			}
			else
			{
				if (m_pOwner->GetPos()->y < -30.f) m_pOwner->Cmd_DamageSelf(255);
			}
		}

	}

	//I3TRACE("Half Gravity %f\n", m_rHalfAccelOfGravity);
}

void CGameCharaMoveContext::EnableSpeedReduction( REAL32 rRate, REAL32 rTime)
{
	m_rElapsedSpeedReduction = rTime;

	if (rTime == -1)
		m_bSpeedReductionEternal = true;

	m_rSpeedReductionRate = rRate;
}

void CGameCharaMoveContext::EnableSpeedReduction( UINT8 nHitPart)
{
	CHARA_HIT_PART_SIMPLE eCHPS = PartKind(nHitPart);
	const CSpeedReduction* pReDuction = m_pOwner->getDBInfo()->GetSpeedReduction(eCHPS);

	if(pReDuction == nullptr)
	{
		return;
	}

	EnableSpeedReduction( ((100.f - pReDuction->GetRate()) / 100.f), pReDuction->GetTime());
}

CHARA_HIT_PART_SIMPLE CGameCharaMoveContext::PartKind(UINT8 nHitPart)
{
	switch(nHitPart)
	{
	case CHARA_HIT_NECK	:
	case CHARA_HIT_LCLAVICLE :
	case CHARA_HIT_RCLAVICLE :		// ���
	case CHARA_HIT_BODY1 :
	case CHARA_HIT_BODY2 :
	case CHARA_HIT_BODY3 :
	case CHARA_HIT_ROOT	:			//	���� ������
		{
			return CHPS_BODY;
		}
		break;

	case CHARA_HIT_RUARM:
	case CHARA_HIT_RDARM:
	case CHARA_HIT_LUARM:
	case CHARA_HIT_LDARM:			//	�� ������
		{
			return CHPS_ARM;
		}
		break;
	case CHARA_HIT_LHAND:
	case CHARA_HIT_RHAND:			//	�� ������
		{
			return CHPS_HAND;
		}
		break;
	case CHARA_HIT_LULEG:
	case CHARA_HIT_LDLEG:
	case CHARA_HIT_RULEG:
	case CHARA_HIT_RDLEG:			//	�ٸ� ������
		{
			return CHPS_LEG;
		}
		break;
	case CHARA_HIT_LFOOT:
	case CHARA_HIT_RFOOT:			//	�� ������
		{
			return CHPS_FOOT;
		}
		break;
	default: return CHPS_UNDKNOWN;
	}
	
}

void CGameCharaMoveContext::UpdateSpeedReduction(REAL32 rDeltaSeconds)
{
	if (m_bSpeedReductionEternal) return;

	m_rElapsedSpeedReduction -= rDeltaSeconds;
}

// ������Ʈ Ÿ�� ���
void CGameCharaMoveContext::CheckObjectByController( void)
{
	bool bRide = false;
	bool bTouchWorld = false;

	I3ASSERT_RETURN( m_pController != nullptr && m_pOwner != nullptr );

	PlayerController * pCtrl = (PlayerController *)(i3PhysixController*)m_pController;

	for( INT32 i = 0; i < pCtrl->getHitShapeCount(); i++)
	{
		CHitInfo* pHitInfo = pCtrl->GetHitInfo( i);

		NxShape * pShape = pHitInfo->GetHitShape();

		if( pShape == nullptr)
			continue;	// pShapeSet�� ������ ��� ���� �����ӿ��� ��Ʈ�� �����̴�. ��ŵ

		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pShape->userData;
		I3ASSERT( pShapeSet != nullptr);
		if( pShapeSet == nullptr)
		{	// �̰�찡 ���´ٸ� ������ ��ü�� �ǽ��غ��� �Ѵ�.
			continue;
		}

		pShapeSet->setReleaseCallback( nullptr, nullptr);

		i3EventReceiver * pReceiver = pShapeSet->getEventReceiver();
		if( pReceiver == nullptr )
			continue;

		if( i3::same_of<CGameWorld*>(pReceiver))
		{
			bTouchWorld = true;

			FromObjDamageToChara( pShape, pShapeSet);
		}
		else if( i3::kind_of<i3Object*>(pReceiver))
		{
			i3Object * pObj = ((i3Object*)pReceiver);

			FromObjDamageToChara( pShape, pShapeSet);

			if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
			{
				if( g_pFramework->IsBattleStage() )
				{
					// "BN_arrow_Green" ���ڿ� �˻����� ��� ȭ��ǥ���� �ĺ�
					// Obj type���� �� �־ ���� �񱳷� ������ ������ ����
					bool head_contain = i3::contain_string_head(pObj->GetName(), "BN_arrow_Green");

					if( head_contain )
					{
						char szTemp[256];
						strncpy(szTemp, pObj->GetName(), 256);
						INT32 iNum = atoi(&szTemp[14]); // BN_arrow_Green�� �����ϰ� ��ȣ�� int ������ �ٲ۴�.
						GlobalFunc::PB_TRACE("iNum %d", iNum);
						if(gmode::i()->GetTutorialMode() != nullptr && gmode::i()->GetTutorialMode()->ChangeNextStateByArrowInTutorial(iNum))
						{
							g_pSndMng->StartCharacterActionSnd( GTBCA_ON_WARPGATE, 0, nullptr, true);
							pObj->setDynamicActiveState(false);
							pObj->InvisibleScene();
						}
					}
				}
			}
			
			if( i3::same_of<CGameObjectJumper*>(pObj))
			{
				if (m_pOwner->IsDino())
				{
					continue;
				}
				if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE)
					&& !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)
					&& !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY))
				{
					m_pOwner->WarpChara(((CGameObjectJumper*)pObj)->getJumpPos());
				
				}
				if( m_pOwner->isLocal())
				{	
					//Key:EscapeMission �浹üũ�� ��� �̺�Ʈ�� �����ϴ� �κ��Դϴ�.
					if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
					{
						I3ASSERT( pObj->getNetIndex() < 65536);
						UINT16 ObjectIdx = (UINT16) pObj->getNetIndex();
					
						if(GetTouchdown())
						{
							GAMEEVENT::Write_Chara_TouchDown( m_pOwner, ObjectIdx );
							//GameEventSender::i()->SetEvent(EVENT_BATTLE_TOUCHDOWN,&SlotIdx);
							SetTouchdown(false);
						
						}
					}
					else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY))
					{
						I3ASSERT( pObj->getNetIndex() < 65536);

						if( MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
						{
							CGameCharaWeaponContext * pWeaponCtx = m_pOwner->getWeaponContext();
							if( pWeaponCtx->getCurrentSlotNum() == WEAPON_SLOT_MISSION)
							{
								UINT16 ObjectIdx = (UINT16) pObj->getNetIndex();
								GAMEEVENT::Write_Chara_TouchDown( m_pOwner, ObjectIdx );
							}
						}
						else
						{
							if(CGameMissionManager::i()->isMyUsurpatio() == true)
							{
								UINT16 ObjectIdx = (UINT16) pObj->getNetIndex();
								GAMEEVENT::Write_Chara_TouchDown( m_pOwner, ObjectIdx );
							}
						}
					}
					else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY))
					{
						if(CGameMissionManager::i()->isMyUsurpatio() == true)
						{
							UINT16 ObjectIdx = (UINT16) pObj->getNetIndex();
							GAMEEVENT::Write_Chara_TouchDown( m_pOwner, ObjectIdx );
						}
					}
					else
					{
						m_pOwner->forceSetPos( ((CGameObjectJumper*)pObj)->getJumpPos());

						//pBattle->InitCheckDataPos();
						hd::check_speedhack::i()->init_pos();
						g_pSndMng->StartCharacterActionSnd( GTBCA_ON_WARPGATE, 0, nullptr, true);

					}
		
				
					
				}
				continue; 
			}
			else if( pObj->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_ANIM)
			{
				// ������ ����Ǵ� AnimationObject�� ��� ����������������� ����ϸ� �ȵ˴ϴ�.
				// ���� ��������� HP�� 0�� ����� ������Ʈ�Դϴ�.
				if( m_pOwner->isLocal() )
				{
					if( (pObj->getStateCount() > 1) && (pObj->getCurrentHP() == 0.f) )
					{
						ResetRideObject();
						continue;
					}

					VEC3D vUp;
					i3Vector::Set( &vUp, 0.f, 1.f, 0.f );
					REAL32 rDot = i3Vector::Dot( pHitInfo->GetHitNormal(), &vUp );

					if( rDot > 0.5f )
					{
						// �ö� ź ���¸� ����.
						m_pOwner->AddOnObjectState();
						SetRideObject( pObj);

						bRide = true;

						m_iRideObjectIndex = pObj->getNetIndex();
					}
				}
			}
			else if( i3::same_of<CGameObjectEvent*>(pObj))
			{
				CGameObjectEvent * ObjEvent = (CGameObjectEvent*)pObj;
				BattleObjContext::i()->ObjectHitWriteFunction( ObjEvent->getEventType(), pObj, m_pOwner );
			}
			else if( i3::same_of<CGameObjectWeaponBox*>(pObj))
			{
				// RPG7 Box�� �浹��
				if( m_pOwner->isLocal() && (m_pOwner->IsDino() == false))
				{
					CGameObjectWeaponBox * pWeaponBox = (CGameObjectWeaponBox*)pObj;

					if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY))
					{
						if(MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
						{
							CGameCharaWeaponContext * pWeaponCtx = m_pOwner->getWeaponContext();
							if( pWeaponCtx->getCurrentSlotNum() == WEAPON_SLOT_MELEE)
							{
								WeaponBase * pWeapon = pWeaponCtx->getWeapon( WEAPON_SLOT_MELEE);
								// �̹� ��Į�� ��� ������ ��ŵ
								if( (pWeapon != nullptr) &&
									(pWeapon->getWeaponInfo()->GetNumber() == WEAPON::getItemIndex( WEAPON::CHINESECLEAVER_SEIZED)
										&& (pWeaponBox->GetWeaponType() != WT_CROWNWATERMELON)) )
								{
									continue;
								}

								// �����̶�� �������� üũ
								if( (pWeaponBox->GetWeaponType() == WT_CROWNWATERMELON) && (m_pOwner->getTeam() != TEAM_RED))
									continue;
							}
							else
								continue;
						}
						else
						{
							if(pWeaponBox->GetWeaponType() != WT_CROWNWATERMELON)
								continue;
						}
					}
					else
					{
						if( ( pWeaponBox->getShellCount() < 1) ||
							( !MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && getWeaponContext()->getWeapon( WEAPON_SLOT_PRIMARY) != nullptr) ||
							( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && m_pOwner->getMultiWeaponContext()->CheckEmptySlot() == false))
						{
							continue;
						}
					}

					if( BattleServerContext::i()->IsP2PHost())
					{	// For Host
						pWeaponBox->HoldToChara( m_pOwner);
					}

					GAMEEVENT::Write_Chara_UseObject( m_pOwner, pObj, NET_ACTIONKEY_USEOBJECT);
				}
			}
			else if (i3::same_of<CGameObjectGiftbox*>(pObj))
			{
				CGameObjectGiftbox* giftbox = (CGameObjectGiftbox*)pObj;
				if (giftbox && m_pOwner->isLocal())
					giftbox->Collide(m_pOwner);
			}
			else if (i3::same_of<CGameObjectJumpHold*>(pObj))
			{
				//�ӵ� ���̷� 2���̻� �浹 �Ǵ� ��찡 ���� <- �ش� ��츦 �����Ѵ�.
				CGameObjectJumpHold* jumphold = (CGameObjectJumpHold*)pObj;
				if ( jumphold && 
					 !( (m_pOwner->isCharaStateMask(CHARA_STATE_FORCEJUMP)) && jumphold == m_pPrevJumpHold ) 
				   )
				{
					if (m_pOwner->isLocal() && (m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->GetMySlotIdxEx()))
					{
						GAMEEVENT::Write_Chara_UseObject(m_pOwner, pObj, NET_ACTIONKEY_USE_MOVE_CHARA_OBJECT);
						jumphold->JumpToChara(pShape, m_pOwner);
						jumphold->PlayJumpAnim();
						g_pSndMng->StartObjectSnd(GSND_OBJECT_JUMPHOLD, jumphold->GetPos());
						m_pPrevJumpHold = jumphold;
					}
				}
			}
			else if (i3::same_of<CGameObjectBattleUseItem*>(pObj))
			{
				// Enable �˻� �� object Action Key�� �����ϴ�.
				if (
					pObj->isEnable() == TRUE && 
					m_pOwner
					)
				{
					// ����� UDP�� ������ �� 3���̸� Enable�� ���ϴ�.
					if (m_pOwner->isLocal() && (m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->GetMySlotIdxEx()))
					{
						GAMEEVENT::Write_Chara_UseObject(m_pOwner, pObj, NET_ACTIONKEY_USEITEMBOX);
						pObj->SetEnable(FALSE);
					}
					else
					{
						pObj->SetEnable(FALSE);
					}
				}
			}
		}
	}

	// CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ ������ CheckFloorTerrain���� Raycast�� �ؼ�
	// ���� �����̴� Object�� ���� �� �ִ����� Ȯ���� ����� ����̴�.
	//
	// �ƹ��Ϳ��� ���� �ʾ����鼭, Raycast ������� Object�� ���� �ö� ���� ����
	// ��쿡�� Ride Object ���¸� �����Ѵ�.
	//
	// CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ�� ���� ����� �ݿ����� ���� ���,
	// �ö� Ÿ �����鼭 Jump���� ��, Ride Object State�� Ǯ��
	// ������ ������� �ʰ� �ȴ�.
	if( bRide == false)
	{
		// �����̴� Object�� ��� ���� �ʴ�.

		// Floor Terrain �˻簡 ���������� �̷������ ������ CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ ������
		// ���� �ݿ��ؼ� �ȵǸ�, ��濡 ���� ����� �׻� �ö�Ÿ ������ �������־�� �Ѵ�.
		if( bTouchWorld == true)
		{
			ResetRideObject();
		}
		else if( isMoveFlag( CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ) == false)
		{
			// �ƹ��� Object���� �ö�Ÿ�� ���� ���� ����̱� ������
			// OnObject ���¸� �����Ѵ�.
			ResetRideObject();
		}
	}

	((PlayerController*)(i3PhysixController*)m_pController)->setHitShapeCount( 0);
}

void CGameCharaMoveContext::UpdateRideObject( MATRIX * pNewTx, REAL32 tm, bool bPhysixUpdate)
{
	I3ASSERT( m_pRideObject != nullptr);

	i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( m_pRideObject->getSceneObject() );

	if( pTrans != nullptr )
	{
	
		I3MATRIXINFO Env;
		MATRIX	matTop, matObj;

		i3Node * pParent = pTrans->GetParent();
		
		i3Matrix::Copy( &matObj, pTrans->GetMatrix() );

		// �ش� �ִϸ��̼��� ����մϴ�. ���� ����� Matrix�� �����;� �մϴ�.
		while( pParent != nullptr )
		{	
			if( i3::kind_of<i3Transform*>(pParent))
			{
				i3Matrix::Mul( &matTop, &matTop, ((i3Transform*)pParent)->GetMatrix() );
			}

			pParent = pParent->GetParent();
		}

		if( pTrans->IsAnimated())
		{
			i3SceneTracer * pTracer = g_pViewer->GetSceneTracer();
			Env.m_FrameID = pTracer->getFrameID();
			Env.m_pIn	= &matObj;
			Env.m_pOut	= &matObj;
			Env.m_Time	= g_pViewer->getDeltaTime();
			Env.m_pModelViewMatrix = &matTop;
			pTrans->GetAnimatedMatrix( &Env );
		}

		// ���� �ִϸ��̼ǵ� matrix
		i3Matrix::Mul( pNewTx, &matObj, &matTop );
	}
	else
	{
		I3ASSERT_0;
	}
}

void CGameCharaMoveContext::MoveCharaByObject( VEC3D * pPos)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ONOBJECT) == true)
	{
		if ( !CGameMaster::i()->IsPaused() )
		{
			REAL32 SyncPosRatio = g_pViewer->getDeltaTime() * 60.f;		// 60Hz ��� ��ʽ����� �����Ѵ�.. 60Hz�϶� 1..60������ �� ������.

			if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST)
			{
				m_RideSyncPos->y += 0.04f * SyncPosRatio;
			}
			else
			{
				m_RideSyncPos->y += 0.01f * SyncPosRatio;
			}

			SetPosition(m_RideSyncPos);
		}
	}
}


VEC3D * CGameCharaMoveContext::TransformByRideObject( MATRIX * pAnimatedMtx, VEC3D * pPos, bool bUpdate)
{
	MATRIX inv;
	// �ӷ��� ����ϱ����� ���� ��ǥ�� ����
	VEC3D vVel, vOldLocalPos;
	VEC3D vRealVel;
		
	if( m_pOldRideObject != m_pRideObject)
	{
		// ���ο� Object�� �ö�����.
		// ���� Frame���κ��� Local-Pos�� ���Ѵ�.
		m_pOldRideObject = m_pRideObject;

		i3Matrix::Inverse( &inv, nullptr, pAnimatedMtx);	

		// local position
		i3Vector::TransformCoord( m_RideLocalPos, pPos, &inv);
	}
	else
	{
		// Controller�κ��� ����� Position(vPos)��
		// ���� Frame������ Local-Pos�� �����Ѵ�.
		i3Matrix::Inverse( &inv, nullptr, m_matRideObjectPose);
		
		i3Vector::Copy( &vOldLocalPos, m_RideLocalPos);

		// local position
		i3Vector::TransformCoord( m_RideLocalPos, pPos, &inv);
		
		vRealVel.x = pAnimatedMtx->m30 - m_matRideObjectPose->m30;
		vRealVel.y = pAnimatedMtx->m31 - m_matRideObjectPose->m31;
		vRealVel.z = pAnimatedMtx->m32 - m_matRideObjectPose->m32;

		// �ִϸ��̼ǿ� ����� �ӷ� ���
		i3Vector::Sub( &vVel, &vOldLocalPos, m_RideLocalPos);
		i3Vector::SetY( &vVel, 0.f);
	}

	if( m_pOwner->isLocal() || m_pOwner->isAI())
	{
		REAL32 speed = i3Vector::Length( &vVel);
		setSpeed( speed);
	}

	// �ö� ź Object�� Local-Space Position�� �̹� Animation Transform��
	// ���� World-Space Position���� ��ȯ�Ѵ�.
	i3Vector::TransformCoord( m_RideSyncPos, m_RideLocalPos, pAnimatedMtx);

	// Animation�� Object��ŭ Controller�� ������ ��ġ �̵���Ų��.
	if( bUpdate)
	{
		MoveCharaByObject( &vRealVel);
		RotateThetaByRideObejct( pAnimatedMtx, &inv);
	}

	i3Matrix::Copy( m_matRideObjectPose, pAnimatedMtx);

	return m_RideSyncPos;
}

void CGameCharaMoveContext::setRideObjectLocalPos(VEC3D* pPos)
{
	VEC3D vPos;
	i3Vector::Copy(m_RideLocalPos, pPos);
	i3Vector::TransformCoord( &vPos, m_RideLocalPos, m_matRideObjectPose);

	SetPosition(&vPos);
}

void CGameCharaMoveContext::RotateThetaByRideObejct( MATRIX * pAnimatedMtx, MATRIX * pInvMtx)
{
	MATRIX matAt;
	i3Matrix::Copy( &matAt, m_pOwner->GetMatrix());
	i3Matrix::PostRotateAxis( &matAt, i3Matrix::GetRight( &matAt), getBoneContext()->getRhoValue());
	i3Matrix::Mul( &matAt, &matAt, pInvMtx);
	i3Matrix::Mul( &matAt, &matAt, pAnimatedMtx);
	REAL32 rTheta = i3Math::atan2( getX( i3Matrix::GetAt( &matAt)), getZ( i3Matrix::GetAt( &matAt)));
	getBoneContext()->setTheta( rTheta + getInputContext()->getInputMoveTheta() );
}

void CGameCharaMoveContext::SetRideObject( i3Object * pObj)
{
	m_pRideObject			= pObj;
	m_iRideObjectCountTime	= 0;

 	m_pOwner->setAttachedObject( m_pRideObject);
}

void CGameCharaMoveContext::ResetRideObject( void)
{
	m_pRideObject			= nullptr;
	m_pOldRideObject		= nullptr;
	m_iRideObjectCountTime = 0;
	m_pOwner->setAttachedObject( nullptr);
	m_pOwner->RemoveOnObjectState();
}

void CGameCharaMoveContext::_TestCollisionAtRespawn()
{
	VEC3D vOtherDir;
	for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++ )
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i );
		
		if (pChara == nullptr)
			continue;

		if( (pChara != m_pOwner) && !pChara->isCharaStateMask( CHARA_STATE_DEATH))
		{
			i3Vector::Sub( &vOtherDir, m_pOwner->GetPos(), pChara->GetPos() );
			REAL32 rLength = i3Vector::Length( &vOtherDir);

			if( rLength < 0.45f )
			{
				if (m_bPushingRandomDir)
				{
					VEC3D randomDir;
					i3Vector::GetRandVector(&randomDir);
					i3Vector::Normalize(&randomDir, &randomDir);
					i3Vector::ScaleAdd(&vOtherDir, &randomDir, rLength);
				}

				i3Vector::Add( m_VelCharacter, m_VelCharacter, &vOtherDir); 
			}
		}
	}

	if (m_bPushingRandomDir == true)
		m_bPushingRandomDir = false;
}

REAL32	CGameCharaMoveContext::_GetMoveVelocity(VEC3D * pVecDir, EMoveSpeedType MoveType)
{
	I3ASSERT(pVecDir != nullptr);
	I3ASSERT(MoveType == MST_RUN || MoveType == MST_WALK || MoveType == MST_JUMP_RUN || MoveType == MST_JUMP_WALK );

	EMoveDirectionType	MoveDirection;

	if( pVecDir->z > 0.0f)
	{
		if( pVecDir->x > 0.0f || pVecDir->x < 0.0f)
			MoveDirection = MDT_DIAGONAL;
		else
			MoveDirection = MDT_FRONT;
	}
	else if( pVecDir->z < 0.0f)
	{
		if( pVecDir->x > 0.0f || pVecDir->x < 0.0f)
			MoveDirection = MDT_DIAGONAL;
		else
			MoveDirection = MDT_BACK;
	}
	else
	{
		MoveDirection = MDT_SIDE;
	}

	return m_pOwner->GetMoveSpeedAddOffset(MoveType, MoveDirection);
}

INT32 CGameCharaMoveContext::_CalcFallingDamage( VEC3D * pTakeOffPos)
{
	I3ASSERT( pTakeOffPos != nullptr);
	I3ASSERT( g_pCharaInfoDataBase->getGlobal() != nullptr);

	INT32 nDamage = 0;

	VEC3D vLen, vDiffPos;
	REAL32 fTakeoffHeight = 0.f;

	i3Vector::Copy( &vDiffPos, m_pOwner->GetPos());
	i3Vector::SetX( &vDiffPos, i3Vector::GetX( pTakeOffPos));
	i3Vector::SetZ( &vDiffPos, i3Vector::GetZ( pTakeOffPos));

	i3Vector::Sub( &vLen, &vDiffPos, pTakeOffPos);		
	
	//// + ���� ������ ����� ���� �ʴ´�.
	//if(vLen.y > 10.f)
	//{
	//	return 0;
	//}

	fTakeoffHeight = i3Vector::Length( &vLen);		//	������ ���� ���

	//	���� ������ �ּ� ���̺��� ���� ������ ��������
	if( fTakeoffHeight >  m_pOwner->GetFallDamageMinHeight() + m_pOwner->GetFallDamageMinOffset() )
	{	
		INT32 nFullHP = m_pOwner->getFullHP();

		INT32 nMinDamage = m_pOwner->GetFallMinDamage();	//	�ּ� ������

		//	���� ���� ���̷� ��� (���� ������ = �ּ� ������ + (���� ���� * ���� Slope damage))
		REAL32 fallDmg = m_pOwner->GetFallDamageSlope();
		fallDmg += m_pOwner->getAbility( CHARA::eABILITY_FALLDAMAGE);
		nDamage = nMinDamage + (INT32) ( fTakeoffHeight * fallDmg);

		nDamage = MINMAX( nMinDamage, nDamage, nFullHP);
	}

	return nDamage;
}

void CGameCharaMoveContext::GetNxCapsule(NxCapsule* pCapsule)
{
	VEC3D vec;

	// i3Vector::Copy( &vec, m_pOwner->GetPos() );
	m_pController->getPosition( &vec );
	// i3Vector::Copy( &vec, m_pOwner->GetPos() );
	i3Vector::Add( &vec, 0.f, RADIUS+0.05f, 0.f );
	NxVec3 v1( getX(&vec), getY(&vec), getZ(&vec) );
	i3Vector::Add( &vec, 0.f, CAPSULE_FULL_HEIGHT-0.05f, 0.f );
	NxVec3 v2( getX(&vec), getY(&vec), getZ(&vec) );
	pCapsule->radius = RADIUS;
	pCapsule->p0 = v1;
	pCapsule->p1 = v2;
	
}

void CGameCharaMoveContext::PushByRaycastResult( const I3_PHYSIX_HIT_RESULT * pResult, const NxCapsule * pCapsule, NxVec3 &vDir, REAL32 skinWidth)
{
	//I3TRACE( "raycast %3f\n", pResult->m_T);
	I3ASSERT( pResult != nullptr);
	I3ASSERT( pCapsule != nullptr);

	REAL32 damp = (pCapsule->radius + skinWidth) - pResult->m_T + 0.01f;
	if( damp > 0.f)
	{
		m_v3ContactObjectVelToChara.x = vDir.x * damp;
		m_v3ContactObjectVelToChara.y = vDir.y * damp;
		m_v3ContactObjectVelToChara.z = vDir.z * damp;
		//I3TRACE( "Damp %3f\n", damp);
	}
}

// x, z�ุ �о� ����.
// �ö� ���� ���� ������� �ʴ´�.
void CGameCharaMoveContext::CalcOverlapLength(NxShape* pShape, NxCapsule* pCapsule, REAL32 rSkinWidth, NxVec3 &vDir)
{
	/*if( m_pRideObject != nullptr)
	{
		return;
	}*/

	I3ASSERT(pShape);
	I3ASSERT(pCapsule);

	
	NxActor* pActor = nullptr;
	pActor = &(pShape->getActor());
	I3ASSERT(pActor);

	if(i3::generic_is_iequal(pActor->getName(), "__FastObject") )
	{
		return;
	}

	if( isMoveFlag(CHARA_MOVE_FLAG_PENETRATEWALL))
	{
		return;
	}
	
	i3Vector::Zero(&m_v3ContactObjectVelToChara);

	NxShapeType nxType = pShape->getType();

	if( nxType == NX_SHAPE_BOX )
	{
		REAL32 len = vDir.normalize();

		if( len > I3_EPS || len < -I3_EPS)
		{
			i3CollideeLine line;
			VEC3D start, end;

			i3Vector::Set( &start, pCapsule->p0.x, pCapsule->p0.y, pCapsule->p0.z);

			line.SetStart( &start);
			
			end.x = -vDir.x + start.x;
			end.y = -vDir.y + start.y;
			end.z = -vDir.z + start.z;

			line.SetEnd( &end);

			// ������Ʈ ���ǵ� ��ŭ �ðܳ���.
			rSkinWidth += len * g_pFramework->GetViewer()->getDeltaTime();

			I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_OBJECT);

			if( pResult != nullptr)
				PushByRaycastResult( pResult, pCapsule, vDir, rSkinWidth);
			else
			{
				start.y -= pCapsule->radius;
				line.SetStart( &start);
				end.y -= pCapsule->radius;
				line.SetEnd( &end);

				pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_OBJECT);
				if( pResult != nullptr)
					PushByRaycastResult( pResult, pCapsule, vDir, rSkinWidth);
				else
				{
					start.y = pCapsule->p1.y;
					line.SetStart( &start);
					end.y = pCapsule->p1.y;
					line.SetEnd( &end);

					pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_OBJECT);
					if( pResult != nullptr)
						PushByRaycastResult( pResult, pCapsule, vDir, rSkinWidth);
					else
					{
						I3TRACE( "�ɸ��� �ø����� ����...\n");
					}
				}
			}
		}
		else
		{
			NxBoxShape* pBoxShape =	pShape->isBox();
			NxVec3 nxExtent = pBoxShape->getDimensions();
			NxVec3 nxPos = pBoxShape->getGlobalPosition();
			NxMat33 nxMat33 = pBoxShape->getGlobalOrientation();

			nxExtent.x = abs(nxExtent.x);
			nxExtent.y = abs(nxExtent.y);
			nxExtent.z = abs(nxExtent.z);
			
			NxVec3 v3CapsulePos;
			v3CapsulePos.x = pCapsule->p0.x;
			v3CapsulePos.z = pCapsule->p0.z;
			v3CapsulePos.y = 0;//pCapsule->p0.y;

			NxMat33 nxInvMat33;
			nxMat33.getInverse(nxInvMat33);
			//nxPos.y = 0;

			v3CapsulePos = v3CapsulePos - nxPos;

			nxInvMat33.multiply(v3CapsulePos, v3CapsulePos);

			nxPos.zero();
			pCapsule->p0.x = v3CapsulePos.x;
			pCapsule->p0.y = v3CapsulePos.y;
			pCapsule->p0.z = v3CapsulePos.z;

			//GlobalFunc::PB_TRACE("nxExtent %f %f %f \n", nxExtent.x, nxExtent.y, nxExtent.z);

			CalcContactObjectVel(nxExtent, nxPos, pCapsule, rSkinWidth);

			nxExtent.x =  m_v3ContactObjectVelToChara.x;
			nxExtent.y =  m_v3ContactObjectVelToChara.y;
			nxExtent.z =  m_v3ContactObjectVelToChara.z;

			//GlobalFunc::PB_TRACE("processed nxExtent %f %f %f \n", nxExtent.x, nxExtent.y, nxExtent.z);

			nxMat33.multiply(nxExtent, nxExtent);
			
			m_v3ContactObjectVelToChara.x = nxExtent.x;
			m_v3ContactObjectVelToChara.y = 0;
			m_v3ContactObjectVelToChara.z = nxExtent.z;
		}

		GlobalFunc::PB_TRACE("INV_ContactObjectVelToChara %f %f %f \n", m_v3ContactObjectVelToChara.x, m_v3ContactObjectVelToChara.y, m_v3ContactObjectVelToChara.z);


	}
	else if(nxType == NX_SHAPE_CONVEX)
	{
		NxBounds3 nxBound3;
		pShape->getWorldBounds(nxBound3);
		
		NxVec3 nxExtent;
		NxVec3 nxPos;
		nxBound3.getCenter(nxPos);
		nxBound3.getExtents(nxExtent);

		CalcContactObjectVelConvex(nxExtent, nxPos, pCapsule, rSkinWidth);
	}
}

void CGameCharaMoveContext::CalcContactObjectVel(NxVec3& nxExtent, NxVec3& nxPos, NxCapsule* pCapsule, REAL32 rSkinWidth)
{
	VEC3D v3BoxPos;
	VEC3D v3CapsulePos;
	VEC3D v3GapPos;

	v3BoxPos.x = nxPos.x;
	v3BoxPos.z = nxPos.z;
	v3BoxPos.y = nxPos.y;

	v3CapsulePos.x = pCapsule->p0.x;
	v3CapsulePos.z = pCapsule->p0.z;
	v3CapsulePos.y = pCapsule->p0.y;

	i3Vector::Sub(&v3GapPos, &v3CapsulePos, &v3BoxPos);

	REAL32 rLen = i3Math::abs(v3GapPos.x);
	REAL32 rGapX = rLen - (pCapsule->radius + nxExtent.x + rSkinWidth);
	VEC3D v3MoveVel;
	
	// GlobalFunc::PB_TRACE("CalcContactObjectVel() rGapX %f rSkinWidth %f \n", rGapX, rSkinWidth);

	if(rGapX < 0)
	{
		if(v3GapPos.x < 0)
		{
			v3MoveVel.x += rGapX;
		}
		else 
		{
			v3MoveVel.x -= rGapX;
		}
	}

	rLen = i3Math::abs(v3GapPos.z);
	REAL32 rGapZ = rLen - (pCapsule->radius + nxExtent.z + rSkinWidth);

	if(rGapZ < 0)
	{
		if(v3GapPos.z < 0)
		{
			v3MoveVel.z += rGapZ;
		}
		else 
		{
			v3MoveVel.z -= rGapZ;
		}
	}

	rLen = i3Math::abs(v3GapPos.y);
	REAL32 rGapY = rLen - (pCapsule->radius + nxExtent.y + rSkinWidth);

	if(rGapY < 0)
	{
		if(v3GapPos.y < 0)
		{
			v3MoveVel.y += rGapY;
		}
		else 
		{
			v3MoveVel.y -= rGapY;
		}
	}

	REAL32 rAbsGapX, rAbsGapZ, rAbsGapY;

	rAbsGapX = abs(rGapX);
	rAbsGapZ = abs(rGapZ);
	rAbsGapY = abs(rGapY);

	m_v3ContactObjectVelToChara.x = rAbsGapX;
	m_v3ContactObjectVelToChara.y = rAbsGapY;
	m_v3ContactObjectVelToChara.z = rAbsGapZ;

	if(m_v3ContactObjectVelToChara.x > m_v3ContactObjectVelToChara.y)
	{
		m_v3ContactObjectVelToChara.x = 0;
		if( m_v3ContactObjectVelToChara.y > m_v3ContactObjectVelToChara.z)
		{
			m_v3ContactObjectVelToChara.y = 0;
			m_v3ContactObjectVelToChara.z = v3MoveVel.z;
		}
		else
		{
			m_v3ContactObjectVelToChara.z = 0;
			m_v3ContactObjectVelToChara.y = v3MoveVel.y;
		}
	}
	else
	{
		m_v3ContactObjectVelToChara.y = 0;
		if( m_v3ContactObjectVelToChara.x > m_v3ContactObjectVelToChara.z)
		{
			m_v3ContactObjectVelToChara.x = 0;
			m_v3ContactObjectVelToChara.z = v3MoveVel.z;
		}
		else
		{
			m_v3ContactObjectVelToChara.z = 0;
			m_v3ContactObjectVelToChara.x = v3MoveVel.x;
		}
	}
}

void CGameCharaMoveContext::CalcContactObjectVelConvex(NxVec3& nxExtent, NxVec3& nxPos, NxCapsule* pCapsule, REAL32 rSkinWidth)
{
	VEC3D v3BoxPos;
	VEC3D v3CapsulePos;
	VEC3D v3GapPos;

	v3BoxPos.x = nxPos.x;
	v3BoxPos.z = nxPos.z;
	v3BoxPos.y = 0;

	v3CapsulePos.x = pCapsule->p0.x;
	v3CapsulePos.z = pCapsule->p0.z;
	v3CapsulePos.y = 0;
	
	i3Vector::Sub(&v3GapPos, &v3CapsulePos, &v3BoxPos);

	REAL32 rLen = i3Math::abs(v3GapPos.x);
	REAL32 rGapX = rLen - (pCapsule->radius + nxExtent.x + rSkinWidth);
	VEC3D v3MoveVel;
	
	// GlobalFunc::PB_TRACE("CalcContactObjectVel() rGapX %f rSkinWidth %f \n", rGapX, rSkinWidth);

	if(rGapX < 0)
	{
		if(v3GapPos.x < 0)
		{
			v3MoveVel.x += rGapX;
		}
		else 
		{
			v3MoveVel.x -= rGapX;
		}
	}

	rLen = i3Math::abs(v3GapPos.z);
	REAL32 rGapZ = rLen - (pCapsule->radius + nxExtent.z + rSkinWidth);

	if(rGapZ < 0)
	{
		if(v3GapPos.z < 0)
		{
			v3MoveVel.z += rGapZ;
		}
		else 
		{
			v3MoveVel.z -= rGapZ;
		}
	}

	rLen = i3Math::abs(v3GapPos.y);
	REAL32 rGapY = rLen - (pCapsule->radius + nxExtent.y + rSkinWidth);

	if(rGapY < 0)
	{
		if(v3GapPos.y < 0)
		{
			v3MoveVel.y += rGapY;
		}
		else 
		{
			v3MoveVel.y -= rGapY;
		}
	}

	REAL32 rAbsGapX, rAbsGapZ, rAbsGapY;

	rAbsGapX = abs(rGapX);
	rAbsGapZ = abs(rGapZ);
	rAbsGapY = abs(rGapY);

	if(rAbsGapX > rAbsGapZ)
	{
		// m_v3ContactObjectVelToChara.z += v3MoveVel.z;
		m_v3ContactObjectVelToChara.z = v3MoveVel.z;
	}
	else 
	{
		// m_v3ContactObjectVelToChara.x += v3MoveVel.x;
		m_v3ContactObjectVelToChara.x = v3MoveVel.x;
	}

	//I3TRACE("%f CalcContactObjectVel() m_v3ContactObjectVelToChara %f %f %f rAbsGap %f %f %f v3MoveVel %f %f %f\n", g_pFramework->GetAccTime(), m_v3ContactObjectVelToChara.x, 
	//	m_v3ContactObjectVelToChara.y, m_v3ContactObjectVelToChara.z, rAbsGapX, rAbsGapY, rAbsGapZ, v3MoveVel.x, v3MoveVel.y, v3MoveVel.z);
}


//void CGameCharaMoveContext::FindDamageObjectAndDamageCharaSelf(NxShape* pShape, i3PhysixShapeSet* pi3Shape)
//{
//	// I3TRACE("FindDamageObjectAndDamageCharaSelf \n");
//
//	if(EnableKill() == false) 
//	{
//		return;
//	}
//
//	I3ASSERT(pShape);
//	I3ASSERT(pi3Shape);
//
//	const char* pChar = nullptr;
//	NxActor* pActor = nullptr;
//	pActor = &(pShape->getActor());
//	
//	I3ASSERT(pActor);
//	pChar = pActor->getName();
//	
//	if(pChar == nullptr) 
//	{
//		return;
//	}
//
//	
//}

void CGameCharaMoveContext::DamageCharaSelf_IfOverlap(NxShape* pShape)
{
	if(EnableKill() == false) 
	{
		return;
	}

	I3ASSERT(pShape);

	const char* pChar = nullptr;
	NxActor* pActor = nullptr;
	pActor = &(pShape->getActor());
	
	I3ASSERT(pActor);
	pChar = pActor->getName();

	if(pChar == nullptr) 
	{
		return;
	}

	//I3TRACE("Overlap %s Actor Flag %d\n", pChar, pActor->readActorFlag(NX_AF_DISABLE_COLLISION));
	NxBoxShape* pBox = pShape->isBox();
	if(pBox)
	{
		//I3TRACE("Overlap Box pos %f %f %f half extent %f %f %f \n", pBox->getGlobalPosition().x, pBox->getGlobalPosition().y, pBox->getGlobalPosition().z, pBox->getDimensions().x, pBox->getDimensions().y, pBox->getDimensions().z);
	}
}

void CGameCharaMoveContext::FromObjDamageToChara(NxShape* pShape, i3PhysixShapeSet* pi3Shape)
{
	if(EnableKill() == false) 
	{
		return;
	}

    I3ASSERT(pShape);

	i3EventReceiver * pReceiver = pi3Shape->getEventReceiver();
	if( pReceiver == nullptr )
		return;

	REAL32 rSpeed = 0.0f;
	
	const char* pChar = nullptr;
	NxActor* pActor = nullptr;
	pActor = &(pShape->getActor());

	VEC3D v3Vel;

	if( pActor->isDynamic())
	{
		// World ���� Static Actor�� ��쿡�� getLinearVelocity() �Լ��� ȣ������ �� Warning�� �߻��Ѵ�.
	
		pi3Shape->getLinearVelocity(&v3Vel);
		
		rSpeed = i3Vector::Length(&v3Vel);
	}
	
	I3ASSERT(pActor);
	pChar = pActor->getName();

	if(pChar == nullptr) 
	{
		return;
	}
	
	if(i3::generic_is_iequal(pChar, "__FastObject") ) 
	{
		if(rSpeed < 0.05f || rSpeed > 100.f)
		{
			//GlobalFunc::PB_TRACE("__FastObject rSpeed %f \n", rSpeed);
			return;
		}

		#if defined( I3_DEBUG)
		if( !i3::kind_of<i3Object*>(pReceiver))
		{
			I3ASSERT_0;
			return;
		}
		#endif

		VEC3D v3ObjectPos, v3CharaPos, v3DirFromObjToChara;
		MATRIX matTemp;
		REAL32 rDot;

		pi3Shape->getShapeGlobalPose(&matTemp);
	
		GetPosition(&v3CharaPos);
		v3ObjectPos.x = matTemp.m30;
		v3ObjectPos.y = matTemp.m31;
		v3ObjectPos.z = matTemp.m32;

		i3Vector::Sub(&v3DirFromObjToChara, &v3CharaPos, &v3ObjectPos);

		i3Vector::Normalize(&v3DirFromObjToChara, &v3DirFromObjToChara);
		i3Vector::Normalize(&v3Vel, &v3Vel);

		rDot = i3Vector::Dot(&v3DirFromObjToChara, &v3Vel);

        
		/*I3TRACE("__FastObject rSpeed %f rDot %f v3DirFromObjToChara %f %f %f v3Vel %f %f %f \n", rSpeed, rDot, v3DirFromObjToChara.x, v3DirFromObjToChara.y, v3DirFromObjToChara.z, 
				v3Vel.x, v3Vel.y, v3Vel.z);*/

		bool bDeath = false;
		I3_BOUNDCHK( ((i3Object*)pReceiver)->getNetIndex(), 65536);

		// �ƿ� ����Ʈ�̰�
		if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST)
		{
			// ��⿡ Ÿ�� ���� �ʴ� ���¶�� 
			if( ( i3::same_of<CGameObjectRidingHelicopter*>(pReceiver)
				&& m_pOwner->isCharaStateMask(CHARA_STATE_ONOBJECT)) == false)
			{
				// ���δ�.
				bDeath = true;
			}
		}
		else if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_HELISPOT)
		{
			// ��⿡ Ÿ�� ���� �ʴ� ���¶�� 
			if( (i3::same_of<CGameObjectRespawnHelicopter*>(pReceiver)
				&& m_pOwner->isCharaStateMask(CHARA_STATE_ONOBJECT)) == false)
			{
				// ���δ�.
				bDeath = true;
			}
		}
		else if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE)
		{
			bDeath = true;
		}
		else
		{
			// �������� ���⼺�� ���� ó��
			if(rDot > 0.f)
			{
				bDeath =true;
			}
		}

		if( bDeath)
			m_pOwner->Cmd_DamageSelf( 255, (UINT16) ((i3Object*)pReceiver)->getNetIndex() );
	}
	else if(i3::generic_is_iequal(pChar, "__DamageObject") ) 
	{
		VEC3D vDir;
		i3Vector::Set( &vDir, 1.f, 0.f, 0.f );

		VEC3D v3Vel2;
		pi3Shape->getLinearVelocity(&v3Vel2);
		REAL32 rMass = pi3Shape->getMass();
		REAL32 rSpeed2 = i3Vector::Length(&v3Vel2);
		//REAL32 rDamage = rMass * rSpeed * 0.005f;
		REAL32 rDamage = rMass * rSpeed2 * 0.1f;
		
		//if(rSpeed < 0.05f)
		if(rSpeed2 < 0.5f)
		{
			//GlobalFunc::PB_TRACE("__DamageObject rSpeed %f \n", rSpeed);
			return;
		}

		if(g_pFramework->GetAccTime() - m_rDamageTime > 1.f)
		{
			if(rDamage < 0.f)
			{
				rDamage = 0.f;
			}
			// revision 60567 ����Ʃ�� B�� ��ź ��ġ �� ����� �������� �׾�� ��
			else if(MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE)
			{
				rDamage = 255.f;
			}
			else if(rDamage > 25.f)
			{
				rDamage = 25.f;
			}

			I3_BOUNDCHK( ((i3Object*) pi3Shape->getEventReceiver())->getNetIndex(), 65536);
			m_pOwner->Cmd_DamageSelf((INT32)rDamage, (UINT16) ((i3Object*) pi3Shape->getEventReceiver())->getNetIndex() );

			m_rDamageTime = g_pFramework->GetAccTime();
		}

	}
	else if(i3::generic_is_iequal(pChar, "__DeathObject") ) 
	{
		m_pOwner->Cmd_DamageSelf( 255);
	}
	
}

// ������ ��쿡�� ó�� - �ڸ��� 2008. 7. 23
void CGameCharaMoveContext::CheckOverlapObject()
{
	INT32 i = 0;
	
	NxScene * pNxScene = g_pPhysixCtx->getScene();
	NxShape* pShapeList[32];
	
	NxCapsule nxCapsule;
	GetNxCapsule(&nxCapsule);
	INT32 iActiveGrp = (1<<CGRP_OBJECT_WHOLE) | (1<<CGRP_OBJECT) | (1<<CGRP_OBJECT_ALL);
	

	INT32 nOverlapCount = pNxScene->overlapCapsuleShapes( nxCapsule, NX_DYNAMIC_SHAPES, 32, pShapeList, nullptr, iActiveGrp);//, collisionGroup);//idxGroup, nullptr, true );

	// GlobalFunc::PB_TRACE("CheckOverlapObject p0 %f %f %f nOverlapCount %d\n", nxCapsule.p0.x, nxCapsule.p0.y, nxCapsule.p0.z, nOverlapCount);

	if( nOverlapCount > 0)
	{
		for( i = 0; i < nOverlapCount; ++i)
		{
			i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pShapeList[i]->userData;
			if( pShapeSet == nullptr)
				continue;

			if( i3::same_of<i3PhysixShapeSet*>(pShapeSet) == false)
				continue;
			
			//GlobalFunc::PB_TRACE("CheckOverlapObject %s \n", pShapeList[i]->getActor().getName());

			FromObjDamageToChara(pShapeList[i], pShapeSet);
		}			
	}
}

bool CGameCharaMoveContext::EnableKill()
{
	GAME_MISSION_STATE missionState = CGameMissionManager::i()->getMissionState();

	if( BattleServerContext::i()->IsP2PActivate() &&
		(m_pOwner->isLocal()  || m_pOwner->isAI())&&
		!m_pOwner->isInvincible() &&	// ���� Ÿ�� üũ
		(( missionState != GAME_MISSION_STATE_ENDBATTLE) &&		// ���� ���ᰡ �ƴ� ��� üũ
			(missionState != GAME_MISSION_STATE_GAMEOVER) &&	// ���� ������ �ƴ� ��� üũ
			(missionState != GAME_MISSION_STATE_PREBATTLE) ))	// ���� �غ� �ܰ谡 �ƴ� ��� üũ
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CGameCharaMoveContext::MoveController( VEC3D * pMoveVec, REAL32 minDist, UINT32 * pHintFlag, REAL32 sharpness)
{
	m_pController->Move( pMoveVec, minDist, pHintFlag, sharpness);
	// GlobalFunc::PB_TRACE("MoveController pMoveVec %f %f %f \n", pMoveVec->x, pMoveVec->y, pMoveVec->z);

	if( isMoveFlag( CHARA_MOVE_FLAG_PENETRATEWALL))
	{
		if( (*pHintFlag & NXCC_COLLISION_SIDES) == NXCC_COLLISION_SIDES)
		{
			VEC3D vPos;
			m_pController->getPosition( &vPos);
			i3Vector::SetY( pMoveVec, 0.f);
			i3Vector::Add( &vPos, &vPos, pMoveVec);
			m_pController->setPosition( &vPos);
		}
	}
}

void CGameCharaMoveContext::OverlapObjectPush(VEC3D& vVel, REAL32 minDist, UINT32& hitMask)
{
	VEC3D v3Right;
	VEC3D v3Temp;
	VEC3D v3Result;

	if( i3Vector::Normalize(&v3Temp, &m_v3ContactObjectVelToChara) > I3_EPS)
	{
		i3Vector::Cross(&v3Right, &I3_YAXIS, &v3Temp);
		REAL32 rDotResult = i3Vector::Dot(&vVel, &v3Right);
		i3Vector::Scale(&v3Right, &v3Right, rDotResult);
		
		i3Vector::Add(&v3Result, &m_v3ContactObjectVelToChara, &v3Right);
		v3Result.y = vVel.y;
	}
	else
	{
		i3Vector::Copy( &v3Result, &vVel);
	}

	// ����� �̵�
	MoveController( &v3Result, minDist, &hitMask, 1.f);
}

void CGameCharaMoveContext::PostUpdate( REAL32 rDeltatime)
{
	i3Vector::Zero(&m_v3ContactObjectVelToChara);
}


void CGameCharaMoveContext::Move( REAL32 rDeltatime)
{
	VEC3D	vVel;
	VEC3D	vPos;
	UINT32	hitMask = 0;
	MATRIX	mtxCurRideWS;

	hitMask = 0;

	//GlobalFunc::PB_TRACE("getCharaStateMask %x \n", m_pOwner->getCharaStateMask());
	//GlobalFunc::PB_TRACE("Move m_Vel %f %f %f \n", m_Vel.x, m_Vel.y, m_Vel.z);
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		return;
	
	// ����.. 2008. 5. 15
	// ��ŸŸ���� ũ�� �̵��Ÿ��� ������ �ݴϴ�.. ���� �ð��� �Ѱ� �Ǹ� ������ �ð��� ���ؼ� ��ŵ�ؼ� �������� ����մϴ�.
	// �̵� �Լ����� �����մϴ�.
	if( rDeltatime > 0.1f)
		rDeltatime = 0.016f;

	// revision 59757 ���� ������ �񽺵��� ������ �� �ӵ��� ������ ���ǵ��� �߻��ϴ� ���� ����
	//i3Vector::Copy( &m_vOldPos, m_pOwner->GetPos());

	if (!m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_GRAVITY))
		UpdateJumping( rDeltatime);
	else
		ResetJumping();

	CheckWorldCollision( rDeltatime);

	UpdateTouchDown(rDeltatime);

	m_pController->getPosition( &vPos );



	// ��Ʈ�� ������Ʈ �˻�
	m_rRideObjectElapseTime = m_rRideObjectElapseTime - rDeltatime;
	if( m_rRideObjectElapseTime < 0.f )
		CheckObjectByController();

	CheckOverlapObject();	

	REAL32 minDist = 0.0001f;
	VEC3D vTest;
	if( m_VelCharacter->y < 0 )
	{
		i3Vector::Set( &vTest, 0.f, -0.01f, 0.f);
		MoveController( &vTest, minDist, &hitMask, 1.f);
	}
	else
	{
		hitMask = 0;
	}

	if( m_pOwner->isCharaStateMask( CHARA_STATE_ONOBJECT) == true 
		&& IsUpTownMoveTrainArea() == false 
		&& (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_UPTOWN))
	{
		m_pOwner->removeCharaStateMask( CHARA_STATE_ONOBJECT);
	}

	if( m_pRideObject != nullptr)
	{
		// �̹� Frame�� �̵��� ��ġ�� Object�κ��� ���´�.
		// ���ο��� ���� PhysixConvex�� ��ġ�� Update�ȴ�.
		UpdateRideObject( &mtxCurRideWS, rDeltatime, true);

		// ĳ������ ��ǥ�� �ִϸ��̼ǵ� ��ġ�� ����ϴ�.
		TransformByRideObject( &mtxCurRideWS, &vPos, true);
	}
	else
	{
		// �ִϸ��̼ǿ� ����� �ӷ� ���
		if ( !m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB) )
		{
			i3Vector::Sub( &vVel, &vPos, m_pOwner->GetPos());
			i3Vector::SetY( &vVel, 0.f);
		}
		else
		{
			VEC3D vPos_Ground = vPos;
			vPos_Ground.y -= CHARASKINWIDTH;		// �Լ� �ϴܿ� CHARASKINWIDTH�� ���� �ڵ尡 �־ ���⼭ �ɼ��� ����..	
			i3Vector::Sub(&vVel, &vPos_Ground, m_pOwner->GetPos());		// �ִϸ��̼� ���� �ӷ°���� ���� �ڵ�� ��ġ�� ������ ������ �ʴ�.
		}																// (2014.12.03.����)

		//I3TRACE("SpeedCal Check 2 : %f\n", i3Vector::Length( &vVel));
		setSpeed( i3Vector::Length( &vVel) );
	}
	
	// ĳ���� �ø��� üũ
	if( m_pOwner->isInvincible() )		// ������ �ÿ��� �մϴ�.
	{
		_TestCollisionAtRespawn();
	}

	i3Vector::Copy( &vVel, m_VelCharacter);
	i3Vector::Copy( m_OldVel, m_VelCharacter );

	

	UINT32	hitMask2;
	PushProcessFromAnimObj(vVel, minDist, hitMask2);
	
	UINT32	hitResult = hitMask2 | hitMask;
	
	SlipSlope(hitResult, minDist, vVel);

	// revision 54918 2013. 3. 12 �ڱ⼺
	//�밢�� �̵��� �ɸ��� ���� ����
	//if( !(i3Vector::isAlike(&m_vOldPos, &vPos, 1)))	// revision 59757 ���� ������ �񽺵��� ������ �� �ӵ��� ������ ���ǵ��� �߻��ϴ� ���� ����
	//{
	//	CheckCollisionException(hitMask, vVel);
	//}
	//���ڵ尡 �̵��� ������ ó���� ���ʿ��� �κ����� �ǴܵǾ� �ּ�ó�� �մϴ�. 16.10.19 �����

	CheckCollisionException(hitMask, vVel);

	// Move�Լ� ȣ���� �ణ�� �̵��� ���� �� �ֽ��ϴ�.(Physix�� �ùķ��̼��� ���ư� �����ε� �մϴ�..)
	m_pController->getPosition( &vPos );

	// ���� ���¿��� õ���� �ε����� �������¸� ����
	if( m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) && (hitResult & NXCC_COLLISION_UP))
	{
		ResetJumping();
		i3Vector::Add( &vPos, 0.f, -getY( &vVel), 0.f );
	}
	//// skinwidth �������� ���� ��ġ�� ���� - �ڸ��� 2008. 11. 10
	//// skinwidth 0.025�������� ������ 0
	vPos.y -= CHARASKINWIDTH;
	//// ������� (skinwidth �������� ���� ��ġ�� ���� - �ڸ��� 2008. 11. 10)
	i3Vector::Copy( m_pOwner->GetPos(), &vPos);
	i3Vector::Zero(&m_v3ContactObjectVelToChara);
}

void CGameCharaMoveContext::CheckCollisionException(UINT32& hitMask, VEC3D& vVel)
{
	//�������̶�� ����
	if(!m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) ) 
	{
		return;
	}

	UINT32 tempMask = 0;
	PlayerController * pCtrl = (PlayerController *)(i3PhysixController*)m_pController;
	VEC3D * pIKPos = i3Matrix::GetPos( getBoneContext()->getIKMatrix());

	for( INT32 i = 0; i < pCtrl->GetHitWorldShapeCount(); i++)
	{
		SWorldHitInfo* pHitInfo = pCtrl->GetHitWorld(i);
		VEC3D* pHitWorldPos = pHitInfo->GetHitWorldPos();

		REAL32 tempHeightCalc = getY(pHitWorldPos) - (getY( pIKPos) - (CAPSULE_FULL_HEIGHT*0.5f) - 0.4f);
		
		if(tempHeightCalc >= 0.11f && tempHeightCalc <= 0.4f)
		{
			//I3TRACE("Height! : %f\n", tempHeightCalc);
			//�ɸ��� �ø�����.
			i3Vector::Set(&vVel, getX(&vVel) * 0.1f, getY(&vVel) * 0.1f, 0.f);
			pCtrl->InitWorldHitInfoArray();
			MoveController(&vVel, 0.003f, &tempMask);
		}
	}
}

void CGameCharaMoveContext::SlipSlope(UINT32& hitMask, REAL32& minDist, VEC3D& vVel)
{
	UINT32 tempMask = 0;

	if( hitMask & NXCC_COLLISION_DOWN)
	{	
		// ���� ���߿� ��ġ�� ���¿��� ������ ���
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) )
		{
			// �ٴ� ������ �˻��մϴ�.
			m_pCurrentMoveControl->CheckFloorTerrain();
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
		}

		// �ٴ��� ���鿡 ���� ĳ���͸� �̲���Ʈ���ϴ�.
		{
			bool bReApplyMovePos = false;
			VEC3D * pFloorNormal = m_pController->getHitNormal();
			REAL32 rDot = i3Vector::Dot( &I3_YAXIS, pFloorNormal );

			if( rDot < 0.6f && rDot > I3_EPS)
			{	
				REAL32 rFactor = g_pViewer->getDeltaTime() * 60.f;		// 60mhz���� ���� (�츣�갡 ���� ������ϼ��� ��ġ�� ���� �������)
				i3Vector::Set(&vVel, getX(pFloorNormal) * 0.02f, -0.04f, getZ(pFloorNormal) * 0.02f);
				m_pOwner->addCharaStateMask(CHARA_STATE_SLOPESLIDE);

				bReApplyMovePos = true;
				vVel *= rFactor;
				minDist = i3Vector::Length(&vVel) * 0.1f;
			}
			else
				m_pOwner->removeCharaStateMask( CHARA_STATE_SLOPESLIDE);

			if( ((PlayerController*)(i3PhysixController*)m_pController)->getHitUser() == nullptr)
			{
				// ��Ʈ�� ��ġ�� �Ʒ����� Sphere���� �������� ����Ͽ� ĳ���͸� ������ ����߷��� �մϴ�.
				// �����ÿ� ���� �ذ�
				VEC3D * pHitWorldPos = m_pController->getHitPos();
				VEC3D	vDownSpherePos;
				VEC3D * pIKPos = i3Matrix::GetPos( getBoneContext()->getIKMatrix());

				i3Vector::Set( &vDownSpherePos, getX( pIKPos), getY( pIKPos) - (CAPSULE_FULL_HEIGHT*0.5f), getZ( pIKPos) );
				i3Vector::Sub( &vDownSpherePos, &vDownSpherePos, pHitWorldPos);
				i3Vector::Normalize( &vDownSpherePos, &vDownSpherePos);
				REAL32 rDotAngle = i3Vector::Dot( &vDownSpherePos, &I3_YAXIS);

				if( !m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR)) 
				{	// ���� �������� ��� üũ
					if( rDotAngle < 0.7f && rDotAngle > 0.f)
					{
						i3Vector::Set( &vVel, getX( &vDownSpherePos) * 0.2f, 0.f, getZ( &vDownSpherePos) * 0.2f);
						ResetJumping();
						m_pOwner->addCharaStateMask( CHARA_STATE_JUMPPUSH);

						bReApplyMovePos = true;
						minDist = i3Vector::Length( &vVel) - 0.005f;
					}
				}
				else
				{
					if( rDotAngle < 0.45f && rDotAngle > 0.f)
					{	// ��簢�� 45�̻��̸� �о��
						i3Vector::Set( &vVel, getX( &vDownSpherePos) * 0.2f, 0.f, getZ( &vDownSpherePos) * 0.2f);

						bReApplyMovePos = true;
						minDist = i3Vector::Length( &vVel) - 0.005f;
					}
					else if( m_pOwner->isCharaStateMask( CHARA_STATE_JUMPPUSH))
					{	// ������ �о�� ����̸� �ѹ� �о�� ��簢�� 45�̻��̵ǵ����Ͽ� �ڵ����� �и����� �Ѵ�.
						VEC3D * pCharaPos = m_pOwner->GetPos();
						if( m_rFloorLength > (getY( pIKPos) - getY( pCharaPos)) * 0.5f + 0.2f)
						{
							i3Vector::Set( &vVel, getX( &vDownSpherePos) * 0.3f, 0.f, getZ( &vDownSpherePos) * 0.3f);
							m_pOwner->removeCharaStateMask( CHARA_STATE_JUMPPUSH);

							bReApplyMovePos = true;
							minDist = i3Vector::Length( &vVel) - 0.005f;
						}
					}
				}
			}
			else
			{
				CGameCharaBase * pHitUser = ((PlayerController*)(i3PhysixController*)m_pController)->getHitUser();
				i3Vector::Sub( &vVel, m_pOwner->GetPos(), pHitUser->GetPos());
				setY( &vVel, 0.f);

				bReApplyMovePos = true;
				minDist = (RADIUS + RADIUS_NETWORK) - i3Vector::Length( &vVel);
			}

			// �ٽ� �̵�ó���� ������ ó��
			if( bReApplyMovePos)
			{	
				if( minDist > 0.3f)						minDist = 0.3f;
				else if( minDist < 0.0001f)				minDist = 0.0001f;

				if(m_pOwner->IsDino() == false) {
					MoveController( &vVel, minDist, &tempMask);
				}
			}
		}

		// �ٴڿ� ��Ҵٸ�....
		m_pOwner->addCharaStateMask( CHARA_STATE_FLOOR);

		ResetLadderJump(m_ladder_info);
	}
	else
	{
		// �ٴڿ��� �������ٸ�
		m_pOwner->removeCharaStateMask( CHARA_STATE_FLOOR );
		m_pOwner->removeCharaStateMask( CHARA_STATE_SLOPESLIDE );
	}
}

void CGameCharaMoveContext::OverlapPush()
{
	REAL32 rObjectSpeed = i3Vector::Length(&m_v3ContactObjectVelToChara);

	if( MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE &&
		MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) )
	{
		if(rObjectSpeed > I3_EPS)
		{
			VEC3D vPos;
			m_pController->getPosition(&vPos);
			i3Vector::Add(&vPos, &vPos, &m_v3ContactObjectVelToChara);

			SetPosition(&vPos);
		}
	}
}

void CGameCharaMoveContext::PushProcessFromAnimObj(VEC3D& vVel, REAL32 minDist, UINT32& hitMask)
{
	REAL32 rObjectSpeed = i3Vector::Length(&m_v3ContactObjectVelToChara);
	
	if( (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE)  ||
		(MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE)  ||
		(MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_UPTOWN) )
	{
		// ���� ���
		if(rObjectSpeed > I3_EPS)
		{
			OverlapObjectPush(vVel, minDist, hitMask);
			i3Vector::Zero( m_VelCharacter);	// �̵����� �ʱ�ȭ
		}
		else 
		{
			MoveController( &vVel, minDist, &hitMask, 1.f);
		}
	}
	else
	{
		bool bAddableObjectVel = false;
		if ((MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_PORT_AKABA))
		{
			if (rObjectSpeed > I3_EPS)
				bAddableObjectVel = true;
		}
		else
		{
			if (rObjectSpeed > I3_EPS && m_pRideObject == nullptr)
			{
				bAddableObjectVel = true;
			}
		}

		if (bAddableObjectVel)
		{
			// set���δ� ���带 �հ� �� �� �־� move���� ó���ǵ��� �ӵ��� ���Ѵ�. swoongoo.jung 2014.03.14
			i3Vector::Add(&vVel, &vVel, &m_v3ContactObjectVelToChara);

			if (i3Vector::Length(&m_v3ContactObjectVelToChara) > I3_EPS)
			{	// ĳ������ �̵��ӵ��� ���ش�.
				i3Vector::Zero(m_VelCharacter);
			}
		}

		MoveController(&vVel, minDist, &hitMask, 1.f);
	}
}

void CGameCharaMoveContext::SetPosition( const VEC3D * pPos)
{
	if (m_pController)
		m_pController->setPosition( const_cast<VEC3D*>(pPos) );
}

void CGameCharaMoveContext::GetPosition( VEC3D * pPos)
{
	if (m_pController)
		m_pController->getPosition( pPos);
}


void CGameCharaMoveContext::ProcessAccel( void)
{
	if(m_pOwner->IsDino() == false)
	{
		if( getCurrentWeapon() == nullptr)
		{
			return;
		}
	}

	VEC3D	vDist;
	VEC3D	vDistAt;
	VEC3D	vDistRight;

	REAL32	fMoveVelocity	= 0.f;
	MATRIX * pCharaMat = m_pOwner->GetMatrix();
	VEC3D *	pVecDir = getMoveDir();

	if( (pVecDir->x != 0.0f) ||	(pVecDir->y != 0.0f) ||	(pVecDir->z != 0.0f))
	{
		CHARA_ACTION_BODYLOWER	stateBody = getActionContext()->getBodyLower();
		CHARA_ACTION_UPPER	stateUpper = getActionContext()->getUpper();

		//	ĳ���� �ٱ�
		if( getActionContext()->isLower( CHARA_LOWER_RUN))
		{
			fMoveVelocity = _GetMoveVelocity(pVecDir, MST_RUN);
		}
		//	ĳ���� �ȱ�
		else if( getActionContext()->isLower( CHARA_LOWER_WALK))
		{
			fMoveVelocity = _GetMoveVelocity(pVecDir, MST_WALK);
		}

		//���� �� ���� �̵��ӵ� ���ϴ� ��.
		if ( CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE) &&	//��� �˻�
			 ( (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP)  ) || !m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR) ) //���� �˻�
		   )
		{
			if (getActionContext()->isLower(CHARA_LOWER_RUN))
			{
				fMoveVelocity = _GetMoveVelocity(pVecDir, MST_JUMP_RUN);
			}
			//	ĳ���� �ȱ�
			else if (getActionContext()->isLower(CHARA_LOWER_WALK))
			{
				fMoveVelocity = _GetMoveVelocity(pVecDir, MST_JUMP_WALK);
			}
		}

		//	���� ���ۿ� ���� �̵� ��ġ�� ������ �޽��ϴ�.
		if(m_pOwner->IsDino() == false)
		{
			WeaponBase * pWeapon = getCurrentWeapon();

			if( pWeapon != nullptr )
			{	
				if( stateBody == CHARA_BODYLOWER_STAND)			//	�⺻ ���ִ� ���¿���
				{
					fMoveVelocity *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_NORMAL) * 0.01f;
				}
				else if( stateBody == CHARA_BODYLOWER_CROUCH)		//	�⺻ �ɾ��ִ� ���¿���
				{
					fMoveVelocity *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_SIT) * 0.01f;
				}
				
				if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
				{//	���� �Ǵ� �� �����϶�
					//	���� �������� �����ÿ��� ����ȵǰ�...
					if( stateUpper != CHARA_UPPER_LOADMAGAZINE && stateUpper != CHARA_UPPER_LOADBULLET &&
						stateUpper != CHARA_UPPER_FIREBULLET)	
					{
						fMoveVelocity *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_ZOOM) * 0.01f;
					}

				}

				if( stateUpper == CHARA_UPPER_FIRE || stateUpper == CHARA_UPPER_SECONDARY_FIRE)			//	�߻� �����϶�
				{
					fMoveVelocity *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_FIRE) * 0.01f;
				}
			}
		}

		if( !getActionContext()->isLower( CHARA_LOWER_JUMP) )
		{
			i3Vector::Copy( getMoveAt(), i3Matrix::GetAt( pCharaMat));
			i3Vector::Copy( getMoveRight(), i3Matrix::GetRight( pCharaMat));

//			FIXED32 test = i3Vector::Dot(getMoveAt(), getMoveRight());

			if( fMoveVelocity != getMaxSpeed())
			{
				m_rOldMaxMoveSpeed = getMaxSpeed();
			}

			if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) )
			{
				INT32 charaNetIndex = getCharaNetIndex();

				if (charaNetIndex < 0 || charaNetIndex >= SLOT_MAX_COUNT)
				{
					I3_BOUNDCHK(charaNetIndex, SLOT_MAX_COUNT);
				}
				else
				if( m_pOwner->isAI())
				{
					AI_ENTRY* pAiEntry = &AIModeContext::i()->AIEntry[charaNetIndex];
					I3_BOUNDCHK(pAiEntry->_level,MAX_AI_COUNT+1);
					fMoveVelocity = fMoveVelocity * g_pTempConfig->m_Speed[pAiEntry->_level];				
				}
			}else if(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
			{
				INT32 charaNetIndex = getCharaNetIndex();

				if (charaNetIndex < 0 || charaNetIndex >= SLOT_MAX_COUNT)
				{
					I3_BOUNDCHK(charaNetIndex, SLOT_MAX_COUNT);
				}
				else
				if( m_pOwner->isAI())
				{
					AI_ENTRY* pAiEntry = &AIModeContext::i()->AIEntry[charaNetIndex];
					AI_BIND_DATA * pBindData = g_pCharaManager->getAiBindData( pAiEntry->_number );
					fMoveVelocity = fMoveVelocity * pBindData->_Speed;
				}
			}

			setMaxSpeed( fMoveVelocity);
		}

		if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB))									
		{
			vDistAt = *GetCurrLadderAccAxis(m_ladder_info);			// ��ٸ� �ö�ź ���¿��� �̵����� ����.
			i3Vector::Scale( &vDistAt, &vDistAt, pVecDir->z);
			vDistRight = *GetCurrLadderRightAxis(m_ladder_info);
			i3Vector::Scale( &vDistRight, &vDistRight, pVecDir->x);
		}
		else
		{
			i3Vector::Normalize( &vDistAt, getMoveAt());			// �ٶ󺸴� ����...
			i3Vector::Scale( &vDistAt, &vDistAt, pVecDir->z);
			i3Vector::Normalize( &vDistRight, getMoveRight());
			i3Vector::Scale( &vDistRight, &vDistRight, pVecDir->x);
		}

		i3Vector::Add( &vDist, &vDistAt, &vDistRight);

		i3Vector::Normalize( &vDist, &vDist);

		//i3Vector::Scale( &vDist, &vDist, fMoveVelocity);
	}
	else
	{
		//	�̵����� �ƴϴ�.
		getAnimContext()->setRunElapseTime( 0.f);
		m_pOwner->removeCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);
	}

	setAccel( &vDist);
}



void	CGameCharaMoveContext::setLadderStateInfo(const LadderStateInfo* info)
{
	m_ladder_info = info;
}

const LadderStateInfo* CGameCharaMoveContext::getLadderStateInfo() const
{
	return m_ladder_info;
}

bool		CGameCharaMoveContext::isJumpingOnLadder() const
{
	return ::IsJumpingOnLadder(m_ladder_info);
}

