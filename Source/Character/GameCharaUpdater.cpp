#include "pch.h"
#include "GameCharaUpdater.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"
#include "StageBattle.h"
#include "GameStateMgr.h"

#include "Fullbody/GameCharaFullbody.h"

#include "./StageBattle/UserContext.h"

#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

#include "MainWeapon_RLShell.h"
//�������� UIIngameChatȰ��ȭ �Ǿ������� ĳ�����̵�����  16.11.01 �����
#include "UI/UIHUDIngameChat.h"		
#include "UI/UIBattleFrame.h"		


#define PHYSIX_CHARA_SLEEP_TIME		4.f
#define CAMERA_CHARA_SLEEP_TIME		0.6f
#define PHYSIX_CHARA_SHAPE_EPS_POS	0.1f

//#define PHYSIX_CHARA_RECALC_POS

I3_CLASS_INSTANCE( CGameCharaUpdater);

// UDP �����͸� ó���մϴ�.
void CGameCharaUpdater::ProcessLocalFromRecvPacket()
{	
	if( BattleServerContext::i()->IsP2PHost() )		return;

	INT32	idx = BattleSlotContext::i()->getMySlotIdx();
	if( idx == -1 ) return;

	GAMEINFO_CHARACTER * pCharaInfo	= BattleSlotContext::i()->setGameInfo_Chara( idx );

	if( !pCharaInfo->_bRecvFlag )			return;

	GAMEINFO_SUFFER * pSufferInfo	= &pCharaInfo->_tNetData._tSufferData;

	UINT32 iInfoFlag = pCharaInfo->_tNetData._iInfoFlag;

	UINT16 uNetHP = pCharaInfo->_tNetData._tHP.getHP();

	if( iInfoFlag & P2P_INFO_FLAG_CS_DEATH)
	{
		if( (uNetHP == 0) &&
			!m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) &&
			(pSufferInfo->getDeathType() != CHARA_DEATH_UN) &&
			!m_pOwner->isInvincible() )
			//!UserContext::i()->Death[idx] )
		{	// �ڽ��� ���� ���
			if( pSufferInfo->getDeathType() == CHARA_DEATH_B || pSufferInfo->getDeathType() == CHARA_DEATH_O )
			{
				VEC3D vTemp;
				i3Vector::Scale( &vTemp, pSufferInfo->getHitDirection(), -1.f );
				pSufferInfo->setHitDirection( &vTemp);
			}

			g_pCharaManager->DeathChara( m_pOwner, pSufferInfo->getShooterIndex(),
				const_cast<VEC3D*>(pSufferInfo->getHitDirection()),
				(CHARA_DEATH_TYPE) pSufferInfo->getDeathType(), (CHARA_HIT_PART) pSufferInfo->getHitPart() );
			
			UserContext::i()->Death[idx] = true;
			pSufferInfo->Reset();
		}
		pCharaInfo->_tNetData._iInfoFlag &= ~P2P_INFO_FLAG_CS_DEATH;
	}

	if( iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
	{
		// ����� ����ź �� ������Ʈ �ı��� ���� �������� ���ɴϴ�.
		if( (pSufferInfo->getHitPart() != CHARA_HIT_UNKNOWN) &&
			m_pOwner->getCharaInfo()->GetHP() != uNetHP )  // hp�� �ε��Ҽ������� �ٲٸ鼭 �� �κ��� ������ �� ������ ���� - �ڸ���
		{	// �ڽ��� ���� ���
			m_pOwner->Cmd_Only_DamageAction( (CHARA_HIT_PART) pSufferInfo->getHitPart(),
				const_cast<VEC3D*>(pSufferInfo->getHitDirection()) );
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_THROW, 0, m_pOwner->GetPos(), true);
			pSufferInfo->Reset();
		}

		pCharaInfo->_tNetData._iInfoFlag &= ~P2P_INFO_FLAG_CS_SUFFERING;
	}

	
	if( !m_pOwner->isInvincible() )
		m_pOwner->setCharaInfo()->SetHP((REAL32)uNetHP);
	pCharaInfo->_bRecvFlag				= false;
}

void CGameCharaUpdater::ProcessRespawn( REAL32 rDeltatime)
{
	// Owner ĳ���ʹ� ���� �� KillCam �� �Ⱥ��̰�
	if( m_pOwner->is1PV() && i3::same_of<CGameCamera_Kill*>(g_pCamera))
	{
		CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
		
		if (pKillCam->isFPS())
		{
			m_pOwner->Cmd_SetColor( nullptr, nullptr, 0.f);
		}
	}

	//	 ������ Ÿ�̸� üũ	
	REAL32 rRespawnElapseTime = m_pOwner->getRespawnElapseTime();

	if( rRespawnElapseTime > 0.0f)
	{	
		getMaterialContext()->SetBlendEnable( true);

		rRespawnElapseTime -= rDeltatime;
		m_pOwner->setRespawnElapseTime( rRespawnElapseTime);

		//	�������� �ȴ�.
		if( rRespawnElapseTime <= 0.0f)
		{
			getMaterialContext()->SetBlendEnable( false);
			m_pOwner->setRespawnElapseTime( 0.f);
			
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			if( g_pConfig != nullptr && !g_pConfig->m_bNetworking)
			{	
				const NET_CHARA_INFO * pMyCharaInfo = BattleSlotContext::i()->getNetCharaInfo( BattleSlotContext::i()->getMySlotIdx() );
				NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( m_pOwner->getCharaNetIndex());

				//	���� ���ΰ��� ���� ���⸦ �״�� ����Ų��.
				pNetCharaInfo->SetWeaponList( pMyCharaInfo->getWeaponList( WEAPON_SLOT_PRIMARY));
			
				if( m_pOwner->getCharaInfo()->GetAIToClient())
				{
					//util_battle::AddAiChara( m_pOwner->getCharaNetIndex());
					//g_pCharaManager->RespawnCharaForAI( m_pOwner->getCharaNetIndex() );
				}
				else
				{
					g_pCharaManager->RespawnChara( m_pOwner->getCharaNetIndex() );
				}

				i3Vector::Set( getMoveContext()->getMoveDir(), (REAL32)(i3Math::Rand() % 2), 0.f, (REAL32)(i3Math::Rand() % 2) );

				MATRIX mat;
				VEC3D	vDir;
				
				i3Matrix::SetRotateY( &mat, getBoneContext()->getTheta() );
				i3Vector::Copy( &m_pOwner->m_vDebugOriginalPos, m_pOwner->GetPos() );
				i3Vector::TransformCoord( &vDir, getMoveContext()->getMoveDir(), &mat );
				i3Vector::Add( &m_pOwner->m_vDebugTargetPos, m_pOwner->GetPos(), &vDir );
				m_pOwner->m_rDebugElp = 1.f;

				if( i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR) )
					m_pOwner->Cmd_Idle_Lower();
				else
					m_pOwner->Cmd_Run( getMoveContext()->getMoveDir());
			}
			else
#endif
			{	// ������ Ÿ�� ���� ������ ��ġ�� �ű�� ĳ���͸� ���ϴ�
				g_pCharaManager->DeathPosition( m_pOwner->getCharaNetIndex() );
			}
		}
	}
}

void CGameCharaUpdater::DoFirstUpdate( REAL32 rDeltatime)
{
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !BattleServerContext::i()->IsP2PActivate() )		return;

	if (i3::kind_of<CGameCharaFullbody*>((CGameCharaBase*)(m_pOwner)))
		OnFirstUpdateFullbody(rDeltatime);
	else
		OnFirstUpdate(rDeltatime);
}

void CGameCharaUpdater::OnFirstUpdate( REAL32 rDeltatime)
{
	if( m_pOwner->isLocal() || m_pOwner->isAI() )
	{
		// Local�� ��Ŷ ó��
		ProcessLocalFromRecvPacket();
	
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		{
			// �̵� ó��
			getMoveContext()->Move( rDeltatime);
			getMoveContext()->UpdateSpeedReduction( rDeltatime);
		}
	}

	if( m_pOwner->is3PV())
	{
		// Animation�� World-Space Transform ���̿� LOD ����� �޶��� �� ������,
		// �װ��� Visual �� ���� ������ �� �� �ֱ� ������
		// �������� LOD�� ����Ų��.
		//	Zoom�� FOV ���� ���
		if(g_pCamera)
		{
			REAL32 fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();

			//	�𵨸� LOD ���
			m_pOwner->calcLOD( m_pOwner->GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio);
		}
	}

	getBoneContext()->UpdateAnim( rDeltatime);		// Animation�� Key ���� ���� (�� Bone�� Cache Matrix�� ����)

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		if( m_pOwner->isNetwork())
			getBoneContext()->_CalcThetaNetwork( rDeltatime);
		else
			getBoneContext()->_CalcTheta( rDeltatime);

		if( m_pOwner->is3PV())
		{
			// ��, ��ü �� ������.
			getBoneContext()->_CalcAimBone( rDeltatime);

			// �Ӹ��� ���� IK
			if( !m_pOwner->IsDino())
				getBoneContext()->_CalcHeadBone( rDeltatime);
		}
		
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// ���Ͽ����� getSceneObject()�� Matrix�� ����Ǿ �ȵȴ�.
		// ���� ����ȴٸ�, Bone ��꿡�� ����� ��ŭ�� ������ �� �� �ִ�.

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 ���� World-Space Matrix�� ���
		// 
		getBoneContext()->_CalcSpineBones();

		//	Rho 
		getBoneContext()->CalcRecoil( rDeltatime);		// ������

		if( m_pOwner->is1PV())
		{
			getBoneContext()->_CalcRho( rDeltatime);			// IK Bone ( IK Cache Matrix)
		}
		else
		{
			// ��ü�� ������.
			getBoneContext()->_CalcRho3PV( rDeltatime);
		}
	}

	getBoneContext()->TransformWorldSpace( rDeltatime);
}

void CGameCharaUpdater::OnFirstUpdateFullbody(REAL32 tm)
{
	REAL32 ratio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();
	m_pOwner->calcLOD(m_pOwner->GetPos(), i3Matrix::GetPos(g_pCamera->getCamMatrix()), ratio);

	getBoneContext()->UpdateAnim(tm);
	if (!m_pOwner->isCharaStateMask(CHARA_STATE_DEATH))
	{
		if( m_pOwner->isNetwork())
			getBoneContext()->_CalcThetaNetwork(tm);
		else
			getBoneContext()->_CalcTheta(tm);
	}
	getBoneContext()->TransformWorldSpace(tm);
}


void CGameCharaUpdater::DoUpdate( REAL32 rDeltatime)
{
	if( m_pOwner->getCharaNetIndex() == -1 )	return;
	if( !BattleServerContext::i()->IsP2PActivate() )		return;

	OnProcessJump( rDeltatime);
	OnUpdate( rDeltatime);
}

void CGameCharaUpdater::OnUpdate( REAL32 rDeltatime)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		ProcessDeathState( rDeltatime);

	getMaterialContext()->DriveLightVolume( rDeltatime);
	getCameraContext()->OnUpdate( rDeltatime);

	//	ĳ���� �𵨸� ���� ó��
	getMaterialContext()->ProcessMaterial( rDeltatime);

	CHARA_TASK task = CHARA_TASK_NONE;
	task = g_pCharaManager->getCharaTask( m_pOwner->getCharaNetIndex());

	if( task == CHARA_TASK_RESPAWN)	//	���� ������ �����̸�
	{
		if( gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE)
			OnProcessInvincible( rDeltatime);
		else
		{
			if( !m_pOwner->isEnableStateMask( CHARA_STATE_DISABLE_CONTROL ) )
			{	
				m_pOwner->Cmd_Reset();
				m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_CONTROL);
			}
		}
	}
	else
	{	//	ĳ���Ͱ� �׾�����
		ProcessRespawn( rDeltatime);
	}
}

void CGameCharaUpdater::DoLastUpdate( REAL32 rDeltatime)
{
	if( !BattleServerContext::i()->IsP2PActivate()  )	return;

	OnLastUpdate( rDeltatime);
}

void CGameCharaUpdater::OnLastUpdate( REAL32 rDeltatime)
{
	
}

void CGameCharaUpdater::OnProcessInvincible( REAL32 rDeltatime)
{
	if( m_pOwner == nullptr )	return;

	REAL32 rInvincible = m_pOwner->getInvincibleTime();

	if( g_pFramework->IsBattleStage() == false)
		return;

	if( !m_pOwner->isNetwork())
	{
#if !defined( DEF_OBSERVER_MODE)
		if(UserInfoContext::i()->IsGM_Observer())
		{
			if( m_bObserverStart )
			{
				CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
				if( pMyChara != nullptr && pMyChara->getCurHP() > 0)
				{
					pMyChara->Cmd_DamageSelf( 255);
				}
			}
			else					m_bObserverStart	= true;
		}
#endif			
		//	������ ���� �����ð��� ����� �ش�.
		if( rInvincible > 0.0f)
		{
			rInvincible -= rDeltatime;
			m_pOwner->setInvincibleTime( rInvincible);
			
			REAL32 limitTime = m_pOwner->getAbility( CHARA::eABILITY_INVINCIBLE_TIME);

			//	�������� �� �� �����ð� ������ Ű�Է��� Ǯ���ش�. (���� ��ü GUI���� Ű�Է��� �������Ƿ�)
			if (rInvincible < limitTime - 0.5f)
			{
				//�������� �� ��(������ ����) �ΰ��� ä��â�� ��Ÿ�� �ִٸ� ĳ���� ��Ʈ�� ����
				UIHUDIngameChat* chat = (UIHUDIngameChat*)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
				if ( nullptr != chat && chat->GetEnableChat() )
				{
					if (false == m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_CONTROL))
					{
						m_pOwner->addEnableStateMask(CHARA_STATE_DISABLE_CONTROL);
					}
				}
				else
				{
					m_pOwner->removeEnableStateMask(CHARA_STATE_DISABLE_CONTROL);
				}
			}

			getMaterialContext()->SetBlendEnable( true);
		}		
		else if( rInvincible < 0.0f)		
		{	
			//�������� �� ��(��������) �ΰ��� ä��â�� ��Ÿ�� �ִٸ� ĳ���� ��Ʈ�� ����
			UIHUDIngameChat* chat = (UIHUDIngameChat*)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
			if ( nullptr != chat && chat->GetEnableChat() )
			{
				if ( false == m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_CONTROL) )
				{
					m_pOwner->addEnableStateMask(CHARA_STATE_DISABLE_CONTROL);
				}
			}
			else
			{
				m_pOwner->removeEnableStateMask(CHARA_STATE_DISABLE_CONTROL);
			}

			rInvincible -= rDeltatime;
			m_pOwner->setInvincibleTime( rInvincible);

			CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

			if( (pMyChara != nullptr) && pMyChara->IsDino())
			{
				getMaterialContext()->setFaceCull(false);
			}
			else
			{
				getMaterialContext()->SetBlendEnable( false);
			}
			if(rInvincible < -0.5f) 
			{
				m_pOwner->setInvincibleTime( -0.5f);
			}

		}
	}
	else
	{
		if( rInvincible > 0.f )
		{
			rInvincible -= rDeltatime;
			m_pOwner->setInvincibleTime( rInvincible);

			getMaterialContext()->SetBlendEnable( true);
		}		
		else if( rInvincible < 0.f )
		{	
			m_pOwner->setInvincibleTime( 0.f);

			getMaterialContext()->SetBlendEnable( false);
		}
	}
}

void CGameCharaUpdater::OnProcessJump( REAL32 rDeltatime)
{
	/*
	//	���� ������ üũ
	if( m_fJumpDelayElapsedTime > 0.0f)
	{
		m_fJumpDelayElapsedTime -= tm;
	}
	else if( m_fJumpDelayElapsedTime < 0.0f)		m_fJumpDelayElapsedTime = 0.0f;
	*/

	m_pOwner->setJumpDelayElapsedTime( m_pOwner->getJumpDelayElapsedTime() + rDeltatime);
}

void CGameCharaUpdater::ProcessPhysixSleep( REAL32 rDeltatime)
{
#if defined( PHYSIX_CHARA_RECALC_POS)
	ProcessRecalcPhysixPos( rDeltatime);
#endif

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXCONTACT) )
		return;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXFREEZE))
		return;

	REAL32 rContactTime = m_pOwner->getContactTime();

	rContactTime += rDeltatime;
	m_pOwner->setContactTime( rContactTime);

	if( rContactTime >= CAMERA_CHARA_SLEEP_TIME)
		m_pOwner->addCharaStateMask( CHARA_STATE_CAMFREEZE);

	if( rContactTime > PHYSIX_CHARA_SLEEP_TIME )
	{	
		for( INT32 i = 0; i < CHARA_HIT_MAXCOUNT; i++)
		{
			PHYSIX_SHAPESET_INFO * info = m_pOwner->setPhysixShapeSetInfo( (CHARA_HIT_PART) i);
			if( info->_pShapeSet != nullptr)
				info->_pShapeSet->setDynamicState(  I3_PHYSIX_STATE_DISABLE);
		}

		m_pOwner->addCharaStateMask( CHARA_STATE_PHYSIXFREEZE);
	}
}

void CGameCharaUpdater::ProcessPhysixSleepForOnObject( REAL32 rDeltatime)
{
#if defined( PHYSIX_CHARA_RECALC_POS)
	ProcessRecalcPhysixPos( rDeltatime);
#endif

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXCONTACT) )
		return;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_PHYSIXFREEZE ) )
		return;

	REAL32 rContactTime = m_pOwner->getContactTime();

	rContactTime += rDeltatime;
	m_pOwner->setContactTime( rContactTime);

	if( rContactTime >= CAMERA_CHARA_SLEEP_TIME)
		m_pOwner->addCharaStateMask( CHARA_STATE_CAMFREEZE);

	if( rContactTime > PHYSIX_CHARA_SLEEP_TIME )
	{
		REAL32 rHideTime = getUpdateContext()->getHideTimer();

		rHideTime -= rDeltatime;
		getUpdateContext()->setHideTimer( rHideTime);
		
		if( rHideTime > 0.f)
		{
			I3ASSERT( getUpdateContext()->getToHideTime() > I3_EPS);
			REAL32 fAlpha = rHideTime / getUpdateContext()->getToHideTime();

			m_pOwner->Cmd_SetColor( nullptr, nullptr, fAlpha);
		}
		else
		{	
			getUpdateContext()->setToHideTime( 0.f);
			getUpdateContext()->setHideTimer( 0.f);

			m_pOwner->Cmd_SetColor( nullptr, nullptr, 1.f);

			m_pOwner->getSceneObject()->AddFlag( I3_NODEFLAG_INVISIBLE);

			for( INT32 i = 0; i < CHARA_HIT_MAXCOUNT; i++)
			{
				PHYSIX_SHAPESET_INFO * info = m_pOwner->setPhysixShapeSetInfo( (CHARA_HIT_PART) i);
				if( info->_pShapeSet != nullptr)
					info->_pShapeSet->setDynamicState(  I3_PHYSIX_STATE_DISABLE);
			}

			m_pOwner->addCharaStateMask( CHARA_STATE_PHYSIXFREEZE);
		}
	}
}

void CGameCharaUpdater::DebugProcess( REAL32 rDeltatime)
{	
#if defined( I3_DEBUG) | defined( BUILD_RELEASE_QA_VERSION)
	if( g_pConfig != nullptr && !g_pConfig->m_bNetworking)
	{
		if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))
			return;

		m_pOwner->m_rDebugMoveTime += ( rDeltatime * m_pOwner->m_rDebugElp * 3.f);
		if( m_pOwner->m_rDebugMoveTime > 3.f || m_pOwner->m_rDebugMoveTime < -3.f)
		{
			m_pOwner->m_rDebugElp *= -1.f;
			if( m_pOwner->m_rDebugMoveTime > 3.f )
				m_pOwner->m_rDebugMoveTime = 3.f;
			else if( m_pOwner->m_rDebugMoveTime < -3.f)
				m_pOwner->m_rDebugMoveTime = -3.f;
			i3Vector::Scale( getMoveContext()->getMoveDir(), getMoveContext()->getMoveDir(), -1.f );
		}

		i3Vector::Lerp( &m_pOwner->m_vDebugPos, &m_pOwner->m_vDebugOriginalPos, &m_pOwner->m_vDebugTargetPos, m_pOwner->m_rDebugMoveTime );
		i3Vector::Sub( getMoveContext()->getOldVelocity(), &m_pOwner->m_vDebugPos, m_pOwner->GetPos() );
		getMoveContext()->setSpeed( i3Vector::Length( getMoveContext()->getOldVelocity() ) );
		
		m_pOwner->Cmd_Run( getMoveContext()->getMoveDir());

		m_pOwner->forceSetPos( &m_pOwner->m_vDebugPos );
	}
#endif
}

void CGameCharaUpdater::ProcessRecalcPhysixPos( REAL32 rDeltatime)
{
	VEC3D vDist;
	MATRIX Mtx, destMtx;
	PHYSIX_SHAPESET_INFO * pCurInfo = nullptr;
	
	for( INT32 i = 1; i < CHARA_HIT_MAXCOUNT; i++)
	{
		pCurInfo = m_pOwner->setPhysixShapeSetInfo( (CHARA_HIT_PART)i);
		if( pCurInfo->_pShapeSet == nullptr)
			continue;
		
		if( (pCurInfo->_pShapeSet->getNxJoint() != nullptr) && (pCurInfo->_iParentShapeIdx != (INT32)CHARA_HIT_UNKNOWN) )
		{
			pCurInfo->_pShapeSet->getShapeGlobalPose( &Mtx);

			const PHYSIX_SHAPESET_INFO * pLinkInfo = m_pOwner->getPhysixShapeSetInfo( pCurInfo->_iParentShapeIdx);
			I3ASSERT( pLinkInfo != nullptr);
			
			if( pLinkInfo->_pShapeSet != nullptr)
			{
				pLinkInfo->_pShapeSet->getShapeGlobalPose( &destMtx);
				
				VEC3D * pPos = i3Matrix::GetPos( &Mtx);
				i3Vector::Sub( &vDist, i3Matrix::GetPos( &destMtx), pPos);
				REAL32 rLen = i3Vector::Normalize( &vDist, &vDist);

				if( pCurInfo->_rDistanceToParentBone + PHYSIX_CHARA_SHAPE_EPS_POS < rLen )
				{
					//I3TRACE( "[%d] %s : %.4f, %s : %.4f\n", i, pCurInfo->_pShapeSet->GetName(), pCurInfo->_rDistanceToParentBone, pLinkInfo->_pShapeSet->GetName(), rLen );
					i3Vector::Scale( &vDist, &vDist, (rLen - pCurInfo->_rDistanceToParentBone) * 0.5f );
					i3Vector::Add( pPos, pPos, &vDist);
					pCurInfo->_pShapeSet->setShapeGlobalPose( &Mtx);
				}
			}
		}
	}
}

void CGameCharaUpdater::ProcessDeathState( REAL32 rDeltatime)
{
	if( g_pEnvSet->m_bEnablePhysX == false || m_pOwner->IsRex() || m_pOwner->IsDomiMutantRex())
	{
		REAL32 rOriginalHeight = getUpdateContext()->getDeathOriginalHeight();
		REAL32 rBlendTime = getUpdateContext()->getDeathBlendTime();
		rBlendTime += rDeltatime;
		if( rBlendTime > 0.5f)
			rBlendTime = 0.5f;

		getUpdateContext()->setDeathBlendTime( rBlendTime);

		setY( m_pOwner->GetPos(), rOriginalHeight + (getUpdateContext()->getDeathBlendHeight() - rOriginalHeight) * rBlendTime * 2.f);
	}

	getCollisionContext()->ProcessCollisionEffect();

	if( g_pFramework->IsBattleStage())
	{
		if( m_pOwner->isCharaStateMask( CHARA_STATE_ONOBJECT))
			ProcessPhysixSleepForOnObject( rDeltatime);
		else
			ProcessPhysixSleep( rDeltatime);

		getUpdateContext()->UpdateDeathTime( rDeltatime );
	}
}

void CGameCharaUpdater::CheckGrazeSoundPlay(void)
{
	const i3::vector<WeaponRocketLauncherShell*>&	List = g_pWeaponManager->getRPG7ShellList();
	GrazeSoundPlay(List);

	GrazeSoundPlay(List);		// ���� ������ ���� 2��ȣ���..(�ǵ����ε�?) 2012.05.30.����.
}

void CGameCharaUpdater::GrazeSoundPlay(const i3::vector<WeaponRocketLauncherShell*>& List)
{
	for( size_t i = 0; i < List.size(); i++)
	{
		WeaponRocketLauncherShell * pShell = List[i];
		if( pShell->isEnable() && pShell->getOwner() != m_pOwner)
		{		
			MainWeapon_RLShell* mainWeapon_Shell = static_cast<MainWeapon_RLShell*>(pShell->GetMainWeapon());

			if( mainWeapon_Shell->isBombState( GRENADESHELL_STATE_LAUNCH) && !(mainWeapon_Shell->isPlayedGrazeSound()) )
			{
				// �Ÿ� üũ
				VEC3D vDir;
				i3Vector::Sub( &vDir, m_pOwner->GetPos(), pShell->GetPos());
				REAL32 rLen = i3Vector::Length( &vDir);
				if( rLen < 10.f)
				{
					 mainWeapon_Shell->PlayGrazeSound();
				}
			}
		}
	}
}