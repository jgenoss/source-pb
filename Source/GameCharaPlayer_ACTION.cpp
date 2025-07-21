#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaBase.h"
#include "StageBattle.h"
#include "Weapon.h"

//#define NO_SCALE_ANIMATION

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ACTION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���� Character�� Ư�� �ø��� �׷�� �浹������ �ƴ����� ���θ� �׽�Ʈ �ϴ� �Լ��Դϴ�.
/*
bool CGameCharaPlayer::_IsOverlappedBy( UINT32 nGroupIdx)
{
	if( isNetwork() )
		return TRUE;

	VEC3D vec;
	i3Vector::Copy( &vec, GetPos() );
	i3Vector::Add( &vec, 0.f, RADIUS+0.05f, 0.f );
	NxVec3 v1( getX(&vec), getY(&vec), getZ(&vec) );
	i3Vector::Add( &vec, 0.f, TALL-0.05f, 0.f );
	NxVec3 v2( getX(&vec), getY(&vec), getZ(&vec) );
	NxSegment	seg( v1, v2 );
	NxCapsule Capsule( seg, RADIUS );
	
	NxScene * pScene = g_pPhysixCtx->getScene();
 	NxU32	activeGrp = g_pPhysixCtx->getGroupMask( nGroupIdx );

	activeGrp &= ~( 1 << CGRP_TRIGGER );
	//activeGrp = 0;
	
	return pScene->checkOverlapCapsule( Capsule, NX_ALL_SHAPES, activeGrp, NULL);
}

BOOL CGameCharaPlayer::ACTION_Stand( void)
{
	if( getActionContext()->isBody( CHARA_BODY_CROUCH) )
	{
		if( !isNetwork() )
		{	
			if( _IsOverlappedBy( CGRP_MYCHARA_BOUND) )
			{	
				return FALSE;
			}

			//m_pMoveContext->ChangeControllerHeight( TALL );
		}

		_ANIM_playBodyLowerSync( CHARA_BODY_STAND, !is1PV());
		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_Crouch( void)
{
	if( getActionContext()->isBody( CHARA_BODY_STAND))
	{
//		if( !isNetwork() )
//			m_pMoveContext->ChangeControllerHeight( HALFTALL );

		_ANIM_playBodyLowerSync( CHARA_BODY_CROUCH, !is1PV());
		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_ToggleIdle( void)
{	
	if( getActionContext()->isBody2( CHARA_BODY_ATTACKIDLE))
	{
		_ANIM_playBody2( CHARA_BODY_IDLE, !is1PV());
	}
	else if( getActionContext()->isBody2( CHARA_BODY_IDLE))
	{
		_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV());
	}

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_Respawn( VEC3D * pStartPos)
{
	I3ASSERT( pStartPos != NULL);

	ResetChara();	//	ĳ���� �ʱ�ȭ
	
	//	ó�� �������� 0�� ����� ��ü�ϴ� �����Ѵ�.
	{
		getWeaponContext()->getControl()->OnSelectWeapon( 2);	// �������ѿ� ���Ե��� �ʴ� ��������� �ʱ� ����
		//ACTION_apply( !is1PV());
		getAnimContext()->getControl()->AnimSetApply();

		SyncWeapon();
		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->ReturnToReady();	//	���� ��ü�� ���� ����� Ready ���·� ����
		}

		//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.
		if( isNetwork() )
		{
			getWeaponContext()->getControl()->OnAttachPurchasePrimWeapon( 0);
			getWeaponContext()->getControl()->OnAttachPurchaseSideWeapon( 1);
		}

		CommandParameter * pParam = getCommandContext()->getParameter();
		pParam->_nSlotIdx = 0;
		
		//if (FALSE == ACTION_SwapWeapon( 0))
		if( !getCommandContext()->Command( CHARA_CMD_SWAPWEAPON))
		{
			// �ֹ��� ���ѽ� ��������� ����
			pParam->_nSlotIdx = 1;
			getCommandContext()->Command( CHARA_CMD_SWAPWEAPON);
			//ACTION_SwapWeapon(1);
		}

		SyncWeapon();

		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->WeaponChange();		//	���� ��ü�� ���� ����� Change ���·�
		}

		//	��ü�� ������ �ִϸ��̼��� �Ѵ�.
		ANIM_playUpper( CHARA_UPPER_SWAPWEAPON, ID_UPPER_SWAP_WEAPON);

		addEnableStateMask( CHARA_STATE_DISABLE_SHOT );//| CHARA_STATE_DISABLE_CROUCH);
		removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL | CHARA_STATE_DISABLE_MOVE );

		//removeStateMask( CHARA_STATE_DEFAULT);
	}

	if( !isNetwork() && (g_pCameraManager != NULL) && g_pCamera != NULL )
	{	//	Default ī�޶��
		g_pCameraManager->setTargetCharacter(this);
		g_pCameraManager->ChangeCamera( "FPS", 0.0f);
		
		//I3PRINTF("FPS idx = %d, Nick = %s\n", getCharaInfo()->_NetIdx, g_pGameContext->GetNickForSlot(getCharaInfo()->_NetIdx));

		//_CAMERA_SetIdle();		//	�Ϲ� ī�޶� ����
		m_pCameraContext->SetIdleCamera();

		// 1��Ī ��忡 ���� �� ó��
		ChangeFPSView( FALSE);
	}

	// �ɾƼ� ���� ��� ���� ������ ��Ʈ�ѷ��� ���õǾ� �ֽ��ϴ�.
	// �������ҽÿ� ������ �� �ִ� ���·� �������ݴϴ�.
	m_pMoveContext->ChangeControllerHeight( TALL );

	// ����Ʈ�� ����Ʈ �ڽ��� Kinematic���� ����
	getCollisionContext()->SetContactShapeSetEnable( FALSE);
	
	forceSetPos( pStartPos);		//	������ ��ġ ���� (������ ���� ��ǥ)

	{
		//m_rElapsedTakeOffTime	= 0.0f;
		//m_rCurJumpHeight		= m_pDBInfo->m_rJumpHeight;
		//m_rCurJumpTime			= m_pDBInfo->m_rJumpTime * 0.5f;

		//i3Vector::Copy( &m_TakeOffPos, pStartPos);
	}
#if defined( I3_DEBUG )
	MATRIX mat;
	VEC3D	vDir;
	i3Matrix::Identity( &mat);
	i3Matrix::SetRotateY( &mat, m_pBoneContext->getTheta() );

	i3Vector::Copy( &m_vDebugOriginalPos, GetPos() );
	i3Vector::TransformCoord( &vDir, m_pMoveContext->getMoveDir(), &mat );
	i3Vector::Add( &m_vDebugTargetPos, GetPos(), &vDir );
	m_rDebugElp = 1.f;
#endif

	ApplyCrosshair();

//	m_DropCount = 0;
	
	//I3TRACE("StartPos: %f, %f, %f\n", getX( pStartPos), getY( pStartPos), getZ( pStartPos));

	//I3TRACE( "Chara Respawn : %d ( %.4f, %.4f, %.4f)\n", getCharaNetIndex(), getX( pStartPos), getY( pStartPos), getZ( pStartPos));

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_Invincible( BOOL bVal )
{
	if( bVal )
	{
		m_rInvincibleElapseTime = g_pCharaInfoDataBase->getGlobal()->m_rInvincibleTime;
		getMaterialContext()->SetBlendEnable( TRUE);
	}
	else
	{
		m_rInvincibleElapseTime = 0.f;
		getMaterialContext()->SetBlendEnable( FALSE );
	}

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_NoneBattleIdle( void)
{
	REAL32 fSyncTime = 0.f;
	if( m_pAnimContext->isAIUpper() && getActionContext()->isUpper( CHARA_UPPER_NONEBATTLEIDLE))
	{	
		fSyncTime = getAnimContext()->getUpperAniTimer();

		//if( getWeaponContext()->getCurrent() != NULL)
		{
		//	CWeaponInfo * pWeaponInfo = getWeaponContext()->getCurrent()->getWeaponInfo();
		//	INT32 nAnimSet = (INT32) ANIM_getAnimSet();
		//	INT32 nWeaponType = (INT32) pWeaponInfo->m_TypeClass;
		//	INT32 nWeaponNum = (INT32) pWeaponInfo->m_nNumber;
		//	INT32 nBody2 = (INT32) STATE_getBody2();
			
		//	i3AIState * pNewAIState = m_pAnimContext->getAIUpper()->getAIState( m_nAIUpperId[ nAnimSet ][ nWeaponType ][ nWeaponNum][ nBody2][ ID_UPPER_NONEBATTLE_IDLE] );
		//	if( pNewAIState != NULL)
			{
		//		if( fSyncTime > pNewAIState->getAnimDuration() )
		//			fSyncTime -= pNewAIState->getAnimDuration();
			}
		}
	}

	ANIM_playUpper( CHARA_UPPER_NONEBATTLEIDLE, ID_UPPER_NONEBATTLE_IDLE, fSyncTime);

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_NoneBattleWeaponAction( void)
{
	REAL32 fSyncTime = 0.f;
	if( m_pAnimContext->isAIUpper() && getActionContext()->isUpper( CHARA_UPPER_NONEBATTLEWEAPONSHOW))
	{
		//if( getWeaponContext()->getCurrent() != NULL)
		{
		//	CWeaponInfo * pWeaponInfo = getWeaponContext()->getCurrent()->getWeaponInfo();
		//	INT32 nWeaponNum = (INT32) pWeaponInfo->m_nNumber;
		//	INT32 nID = m_pAnimContext->getAIUpperID( getAnimSet(), pWeaponInfo->m_TypeClass, nWeaponNum, getActionContext()->getBody2(), ID_UPPER_NONEBATTLE_IDLE);
		//	i3AIState * pNewAIState = m_pAnimContext->getAIUpper()->getAIState(	nID );
		//	if( pNewAIState != NULL)
			{
		//		if( fSyncTime > pNewAIState->getAnimDuration() )
		//			fSyncTime -= pNewAIState->getAnimDuration();
			}
		}
		

		fSyncTime = getAnimContext()->getUpperAniTimer();
	}
	else
		getAnimContext()->setUpperAniTimer( 0.f);

	ANIM_playUpper( CHARA_UPPER_NONEBATTLEWEAPONSHOW, ID_UPPER_NONEBATTLE_SHOWWEAPON, fSyncTime );
	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_MoveLower( VEC3D * pMoveDir)
{	
	if( pMoveDir == NULL )
		pMoveDir	= m_pMoveContext->getMoveDir();

	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_AI nId = _ANIM_getMoveId( pMoveDir);
	CHARA_ACTION_LOWER state = CHARA_LOWER_MOVE;
	
	if( i3Vector::IsSame( pMoveDir, &I3_ZEROVECTOR) && !getActionContext()->isLower( CHARA_LOWER_IDLE) )
		return ACTION_IdleLower();

	if( nId != ID_LOWER_IDLE )
	{
#if !defined( NO_SCALE_ANIMATION )
		REAL32 fVel = i3Vector::Length( m_pMoveContext->getVelocity() );

		if( fVel > I3_EPS )
		{	
			if( g_pViewer->getDeltaTime() > I3_EPS )
			{
				fVel /= g_pViewer->getDeltaTime();
				
				if( getActionContext()->isBody( CHARA_BODY_STAND ) )
				{
					switch( nId )
					{
					case ID_LOWER_MOVE_FRONT :
					case ID_LOWER_MOVE_RIGHT :
					case ID_LOWER_MOVE_FRONT_LEFT :
						fScaleTime = (fVel * 0.2222222222222222222222222222222f);
						break;

					case ID_LOWER_MOVE_LEFT :
					case ID_LOWER_MOVE_BACK :
						fScaleTime = (fVel * 0.3030303030303030303030303030303f);
						break;
						
					case ID_LOWER_MOVE_FRONT_RIGHT :
						fScaleTime = (fVel * 0.25641025641025641025641025641026f);
						break;

					case ID_LOWER_MOVE_BACK_LEFT :
					case ID_LOWER_MOVE_BACK_RIGHT :
						fScaleTime = (fVel * 0.33333333333333333333333333333333f);
						break;
					}
				}
				else
				{
					switch( nId )
					{
					case ID_LOWER_MOVE_FRONT :
					case ID_LOWER_MOVE_BACK :
					case ID_LOWER_MOVE_BACK_RIGHT :
						fScaleTime = (fVel * 0.88888888888888888888888888888889f);
						break;
					
					case ID_LOWER_MOVE_FRONT_LEFT :
						fScaleTime = fVel;
						break;

					case ID_LOWER_MOVE_LEFT :
					case ID_LOWER_MOVE_RIGHT :
					case ID_LOWER_MOVE_BACK_LEFT :
						fScaleTime = (fVel * 0.8f);
						break;
						
					case ID_LOWER_MOVE_FRONT_RIGHT :
						fScaleTime = (fVel * 0.72727272727272727272727272727273f);
						break;
					}
				}
			}

			if( fScaleTime < 0.05f )
			{
				fScaleTime = 0.05f;
			}
		}
#endif

		if( m_pAnimContext->isAILower())
		{		
			//	���� �̵� ���̴�. ( �̵� �ִϸ��̼� ��ü ���۽ð��� ���� �÷���)
			if( getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK ))
			{
				fSyncTime = m_pAnimContext->getAILower()->getLocalTime();
				if( m_pAnimContext->getOldLowerID() != nId &&
					((m_pAnimContext->getOldLowerID() > ID_LOWER_IDLE) &&
					(m_pAnimContext->getOldLowerID() <= ID_LOWER_WALK_BACK_RIGHT)) )
				{	
					INT32 nID = m_pAnimContext->getAILowerID( getActionContext()->getBody(), (ID_LOWER_AI)m_pAnimContext->getOldLowerID() );
					i3AIState * pOldAIState = m_pAnimContext->getAILower()->getAIState( nID );
					nID = m_pAnimContext->getAILowerID( getActionContext()->getBody(), nId );
					i3AIState * pNewAIState = m_pAnimContext->getAILower()->getAIState( nID );

					if( pOldAIState->getAnimDuration() > I3_EPS )
						fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
				}

				//I3TRACE( "SyncTime:%.4f,ScaleTime:%.4f\n", fSyncTime, fScaleTime );
			}
		}
	}

	if( (m_pAnimContext->getOldLowerID() != nId) )
	{
		ANIM_playLower( state, nId, fSyncTime, fScaleTime );
	}
	else if( m_pAnimContext->getAILower() != NULL)
	{
		REAL32 rRet = m_pMoveContext->getSpeed() - fScaleTime;
		if( rRet < -0.05f || rRet > 0.05f )
		{
			SetLowerTimeScale( fScaleTime );
			m_pMoveContext->setSpeed( fScaleTime);
		}
	}

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_MoveUpper( VEC3D * pMoveDir)
{	
	if( pMoveDir == NULL )
		pMoveDir	= m_pMoveContext->getMoveDir();

	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_UPPER_AI	nId = ID_UPPER_MOVE;	//	��ü�� ���� ������� "Move"��
	CHARA_ACTION_UPPER state = CHARA_UPPER_MOVE;

	ACTION_SyncWeapon();
	
	if( i3Vector::IsSame( pMoveDir, &I3_ZEROVECTOR))
	{
		nId = ID_UPPER_IDLE;
		state = CHARA_UPPER_IDLE;
	}

	if( nId != ID_UPPER_IDLE )
	{
		if( m_pAnimContext->isAIUpper())
		{		
			//	���� �̵� ���̴�. ( �̵� �ִϸ��̼� ���۽ð��� ���� �÷���)
			fSyncTime = m_pAnimContext->getAIUpper()->getLocalTime();
		}

		switch( getActionContext()->getLower() )
		{
		case CHARA_LOWER_MOVE :
			fScaleTime = 1.f;
			break;

		case CHARA_LOWER_JUMP :
		case CHARA_LOWER_DEATH :
			break;

		case CHARA_LOWER_WALK :
			fScaleTime = m_pDBInfo->m_rWalkFrontMove / m_pDBInfo->m_rFrontMoveSpeed;
			break;
		}

		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		if( pWeapon != NULL )
		{	
			CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();*/

			/*
			fScaleTime *= pWeaponInfo->m_rMoveNormalStand;
			

			if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_PRIMARY )
			{
				
			}
			else if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_SECONDARY )
			{
				fScaleTime *= 0.8f;
			}
			else if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_MELEE )
			{
			}
			else if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_THROWING || pWeaponInfo->m_TypeUsage == WEAPON_USAGE_ITEM )
			{
				fScaleTime *= 0.95f;
			}
			else if( pWeaponInfo->m_TypeUsage == WEAPON_USAGE_MISSION )
			{
				fScaleTime *= 0.75f;
			}*/

			//CWeaponInfo * pDefaultInfo = g_pWeaponInfoDataBase->getWeaponInfo( pWeaponInfo->m_TypeClass, 1 );

			/*REAL32	rScaleRate = 0.0f;
			if( getActionContext()->isBody( CHARA_BODY_STAND))			//	�⺻ ���ִ� ���¿���
				//fScaleTime *= (1.f - ((pDefaultInfo->m_rMoveNormalStand - pWeaponInfo->m_rMoveNormalStand) * 0.01f) );
				rScaleRate =  pWeaponInfo->m_rMoveNormalStand * 0.01f;
			else if( getActionContext()->isBody( CHARA_BODY_CROUCH))		//	�⺻ �ɾ��ִ� ���¿���
				//fScaleTime *= (0.6f - ((pDefaultInfo->m_rMoveNormalSit - pWeaponInfo->m_rMoveNormalSit)  * 0.01f));
				rScaleRate = pWeaponInfo->m_rMoveNormalSit * 0.01f;

			if( isStateMask( CHARA_STATE_ZOOM))
			{
				if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) &&
					!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET))	//	���� �������� �����ÿ��� ����ȵǰ�...
					rScaleRate *= pWeaponInfo->m_rMoveNormalZoom * 0.01f;
			}

			if( getActionContext()->isUpper( CHARA_UPPER_FIRE) ||
				getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE))			//	�߻� �����϶�
			{
				rScaleRate *= pWeaponInfo->m_rMoveNormalFire * 0.01f;
			}

			fScaleTime *= rScaleRate * 1.25f;
		}
	}

	if( (m_pAnimContext->getOldUpperID() != nId) )
	{
		ANIM_playUpper( state, nId, fSyncTime, fScaleTime );
	}
	else if( m_pAnimContext->getAIUpper() != NULL)
	{
		i3AIState * pAI = m_pAnimContext->getAIUpper()->getCurrentAIState();
		if( pAI != NULL)
		{
			pAI->setTimeScale( fScaleTime);
		}

		SetUpperTimeScale( fScaleTime );
	}

//	ANIM_playUpper( state, nId, fSyncTime, fScaleTime );

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_WalkLower( VEC3D * pMoveDir)
{	
	if( pMoveDir == NULL )
		pMoveDir	= m_pMoveContext->getMoveDir();

	REAL32 fSyncTime = 0.f;
	REAL32 fScaleTime = 1.f;
	ID_LOWER_AI nId = _ANIM_getWalkId( pMoveDir);
	CHARA_ACTION_LOWER state = CHARA_LOWER_WALK;	//	�ȱ� ����
	
	if( i3Vector::IsSame( pMoveDir, &I3_ZEROVECTOR) && !getActionContext()->isLower( CHARA_LOWER_IDLE) )
		CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);
		//return ACTION_IdleLower();
	
	if( nId != ID_LOWER_IDLE )
	{
#if !defined( NO_SCALE_ANIMATION )
		REAL32 fVel = i3Vector::Length( m_pMoveContext->getVelocity() );

		if( fVel > 0.f )
		{
			if( g_pViewer->getDeltaTime() > I3_EPS )
			{
				fVel /= g_pViewer->getDeltaTime();
				
				if( getActionContext()->isBody( CHARA_BODY_STAND ) )
				{
					switch( nId )
					{
					case ID_LOWER_WALK_FRONT :
					case ID_LOWER_WALK_LEFT :
					case ID_LOWER_WALK_BACK_LEFT:
					case ID_LOWER_WALK_BACK_RIGHT :
					case ID_LOWER_WALK_FRONT_LEFT :
						fScaleTime = fVel * 0.59259259259259259259259259259259f;
						break;
					case ID_LOWER_WALK_RIGHT :
						fScaleTime = fVel * 0.44444444444444444444444444444444f;
						break;
					case ID_LOWER_WALK_BACK :
						fScaleTime = fVel * 0.66666666666666666666666666666667f;
						break;
					case ID_LOWER_WALK_FRONT_RIGHT :
						fScaleTime = fVel * 0.53333333333333333333333333333333f;
						break;
					}
				}
				else
				{
					switch( nId )
					{
					case ID_LOWER_WALK_FRONT :
					case ID_LOWER_WALK_BACK :
					case ID_LOWER_WALK_LEFT :
					case ID_LOWER_WALK_BACK_LEFT:
					case ID_LOWER_WALK_BACK_RIGHT :
						fScaleTime = fVel * 3.5555555555555555555555555555556f;
						break;
					case ID_LOWER_WALK_FRONT_LEFT :
						fScaleTime = fVel * 4.f;
						break;
					case ID_LOWER_WALK_RIGHT :
						fScaleTime = fVel * 3.2f;
						break;
					case ID_LOWER_WALK_FRONT_RIGHT :
						fScaleTime = fVel * 2.9090909090909090909090909090909f;
						break;
					}
				}
			}

			if( fScaleTime < 0.01f )
			{
				fScaleTime = 0.01f;
			}
		}
#endif

		if( m_pAnimContext->isAILower())
		{		
			//	���� �̵� ���̴�. ( �̵� �ִϸ��̼� ��ü ���۽ð��� ���� �÷���)
			if( getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK ))
			{
				fSyncTime = m_pAnimContext->getAILower()->getLocalTime();
				
				if( m_pAnimContext->getOldLowerID() != nId && (m_pAnimContext->getOldLowerID() > ID_LOWER_IDLE && m_pAnimContext->getOldLowerID() <= ID_LOWER_WALK_BACK_RIGHT) )
				{	
					INT32 nID = m_pAnimContext->getAILowerID( getActionContext()->getBody(), nId );
					i3AIState * pNewAIState = m_pAnimContext->getAILower()->getAIState( nID );
					nID = m_pAnimContext->getAILowerID( getActionContext()->getBody(), (ID_LOWER_AI) m_pAnimContext->getOldLowerID());
					i3AIState * pOldAIState = m_pAnimContext->getAILower()->getAIState( nID );
					if( pOldAIState->getAnimDuration() > I3_EPS )
						fSyncTime *= pNewAIState->getAnimDuration() / pOldAIState->getAnimDuration();
				}

				//I3TRACE( "SyncTime:%.4f,ScaleTime:%.4f\n", fSyncTime, fScaleTime );
			}
		}
	}

	if( m_pAnimContext->getOldLowerID() != nId )
		ANIM_playLower( state, nId, fSyncTime, fScaleTime );
	else
	{
		REAL32 rRet = m_pMoveContext->getSpeed() - fScaleTime;
		if( rRet < -0.01f || rRet > 0.01f )
		{
			SetLowerTimeScale( fScaleTime );
			m_pMoveContext->setSpeed( fScaleTime);
		}
	}

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_IdleLower( void)
{
	ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_IDLE);
	
	m_fRunElapseTime = 0.0f;
	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_IdleUpper( void)
{
	REAL32 fSyncTime = 0.f;

	if( getActionContext()->isUpper( CHARA_UPPER_IDLE ))
		return FALSE;

	if( m_pAnimContext->isAILower())
	{
		//	��ü �ִϸ��̼ǰ� �´� Ÿ�� ��ũ�ؾ� �մϴ�.
		fSyncTime = GetSyncTimeLower();
	}

	ACTION_SyncWeapon();

	ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_IDLE, fSyncTime);

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_Jump( void)
{
	//setClimbingLadderEnable( false);
	if( !isNetwork() )
	{
		// ĳ������ �ٿ�� üũ
		//	���� ������ ���� ���� ������ �ð��� �ش�.
		if( m_fJumpDelayElapsedTime <= m_pDBInfo->m_rJumpDelay || _IsOverlappedBy( CGRP_MYCHARA_BOUND) )
			return FALSE;
	}

	addStateMask( CHARA_STATE_JUMP);

	m_pMoveContext->setElapsedTakeOffTime( 0.f);

	I3ASSERT( m_pDBInfo->m_rJumpPenaltyTime > I3_EPS );
	REAL32	rRate = m_fJumpDelayElapsedTime / m_pDBInfo->m_rJumpPenaltyTime;
	REAL32  rValidRate = 1.0f;

	if( rRate < 0.3f)
	{
		rValidRate = MINMAX( 0.1f, 1.0f - ( m_pDBInfo->m_rJumpPenaltyRate * 3.0f), 1.0f);
	}
	else if( (rRate >= 0.3f) && ( rRate < 0.6f))
	{
		rValidRate = MINMAX( 0.1f, 1.0f - ( m_pDBInfo->m_rJumpPenaltyRate * 2.0f), 1.0f);
	}
	else if(( rRate > 1.0f) && ( rRate >= 0.6f))
	{
		rValidRate = MINMAX( 0.1f, 1.0f - ( m_pDBInfo->m_rJumpPenaltyRate * 1.0f), 1.0f);
	}

	m_pMoveContext->setCurJumpHeight( m_pDBInfo->m_rJumpHeight * rValidRate);
	m_pMoveContext->setCurJumpTime( m_pDBInfo->m_rJumpTime * rValidRate * 0.5f);

	if( getActionContext()->isBody( CHARA_BODY_CROUCH))	//	�ɾ� ������
	{
		_ANIM_playBody( CHARA_BODY_STAND, !is1PV());
	}

	ANIM_playLower( CHARA_LOWER_JUMP, ID_LOWER_JUMP_START);

	if( getActionContext()->isUpper( CHARA_UPPER_IDLE) ||
		getActionContext()->isUpper( CHARA_UPPER_MOVE) )// || (STATE_getUpper() == CHARA_UPPER_JUMP))
	{
		ACTION_SyncWeapon();
		ANIM_playUpper( CHARA_UPPER_JUMP, ID_UPPER_JUMP_START);
	}

	// �̵��� �ɱ⸦ ���´�.
	addEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTBT_JUMP_START, GetPos(), is1PV());

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_JumpEndUpper( void )
{
	ACTION_SyncWeapon();
	ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_JUMP_END);

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_JumpEndLower( void )
{
	ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_JUMP_END);

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_Damage( CHARA_HIT_PART part, VEC3D * pvHitDir)
{
	if( isInvincible() )
		return FALSE;

	if( !isNetwork() )
	{
		BOOL bCheck = FALSE;
		CHARA_ACTION_UPPER state = getActionContext()->getUpper();

		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		if( (pWeapon!=NULL && pWeapon->getWeaponInfo()->m_TypeClass==WEAPON_CLASS_MISSION) &&
			state == CHARA_UPPER_FIRE)
			bCheck = TRUE;

		if( !bCheck && (state != CHARA_UPPER_IDLE) && (state != CHARA_UPPER_MOVE) && (state != CHARA_UPPER_JUMP ) )
		{
			return FALSE;	// 1��Ī�̰� ĳ���Ͱ� �߻� �����̸� ��ŵ
		}
	}

	ACTION_SyncWeapon();

	INT32 nType = i3Math::Rand() % 2;

	VEC3D * pAt = i3Matrix::GetAt( GetMatrix());

	//	�չ��� ����
	BOOL bFront = (i3Vector::Dot( pAt, pvHitDir) < 0.0f);

	ID_UPPER_AI id;
		
	switch( part)
	{	
	case CHARA_HIT_HEAD		:
	case CHARA_HIT_NECK		:
	case CHARA_HIT_BODY1	:
	case CHARA_HIT_BODY2	:
	case CHARA_HIT_BODY3	:		//	������ �¾Ҵ�.
		if( bFront)
		{	
			if( nType == 0)
			{
				if( i3Math::Rand() % 2 == 0)
					id = ID_UPPER_DAMAGE_FRONT_LEFT_A;
				else
					id = ID_UPPER_DAMAGE_FRONT_LEFT_B;
			}
			else
			{
				if( i3Math::Rand() % 2 == 0)
					id = ID_UPPER_DAMAGE_FRONT_RIGHT_A;
				else
					id = ID_UPPER_DAMAGE_FRONT_RIGHT_B;
			}
		}
		else
		{
			if( (i3Math::Rand() % 2) == 0)
				id = ID_UPPER_DAMAGE_BACK_A;
			else
				id = ID_UPPER_DAMAGE_BACK_B;
		}
		break;	

	//	���� ������
	case CHARA_HIT_LUARM	:		//	���� ���� �¾Ҵ�.
	case CHARA_HIT_LDARM	:
	case CHARA_HIT_LHAND	:
	case CHARA_HIT_LCLAVICLE	:
		if( bFront)
		{
			if( nType == 0)
			{
				id = ID_UPPER_DAMAGE_FRONT_LEFT_A;
			}
			else
			{
				id = ID_UPPER_DAMAGE_FRONT_LEFT_B;
			}
		}
		else
		{
			if( nType == 0)
				id = ID_UPPER_DAMAGE_BACK_A;
			else
				id = ID_UPPER_DAMAGE_BACK_B;
		}
		break;

	//	������ ������
	case CHARA_HIT_RUARM	:
	case CHARA_HIT_RDARM	:
	case CHARA_HIT_RHAND	:
	case CHARA_HIT_RCLAVICLE	:
		if( bFront)
		{
			if( nType == 0)
			{
				id = ID_UPPER_DAMAGE_FRONT_RIGHT_A;
			}
			else
			{
				id = ID_UPPER_DAMAGE_FRONT_RIGHT_B;
			}
		}
		else
		{
			if( nType == 0)
				id = ID_UPPER_DAMAGE_BACK_A;
			else
				id = ID_UPPER_DAMAGE_BACK_B;
		}
		break;
	//	�ٸ�
	case CHARA_HIT_ROOT		:
	case CHARA_HIT_LULEG	:
	case CHARA_HIT_LDLEG	:
	case CHARA_HIT_RULEG	:
	case CHARA_HIT_RDLEG	:
	case CHARA_HIT_LFOOT	:
	case CHARA_HIT_RFOOT	:
		if( nType == 0)
		{
			id = ID_UPPER_DAMAGE_BACK_B;
		}
		else
			id = ID_UPPER_DAMAGE_BACK_A;
		break;

	default :	I3TRACE( "[%s] : WARNING! Hit part(%d) is Invalid!\n", __FUNCTION__, part ); 
		ANIM_playUpper( CHARA_UPPER_DAMAGE, ID_UPPER_DAMAGE_BACK_B);
		return FALSE;
	}
	
	BOOL	bCheckAction = FALSE;

	if( m_pAnimContext->getOldUpperID() != id )
	{
		if( getActionContext()->isUpper( CHARA_UPPER_DAMAGE ) )
		{
			if( m_pAnimContext->getAIUpper() != NULL && m_pAnimContext->getAIUpper()->getElapsedTime() > _ANIM_getUpperDuration() * 0.2f )
				bCheckAction = TRUE;
		}
		else
			bCheckAction = TRUE;
	}
	else if( m_pAnimContext->getAIUpper() != NULL && m_pAnimContext->getAIUpper()->getCurrentAIState() != NULL)
	{
		if( m_pAnimContext->getAIUpper()->getElapsedTime() > _ANIM_getUpperDuration() * 0.5f)
		{
			bCheckAction = TRUE;
		}
	}

	if( bCheckAction )
	{
		ANIM_playUpper( CHARA_UPPER_DAMAGE, id );
	}

	//	������ �����Ҹ� �÷���~	�ܺο��� ȣ�����ݴϴ�.
	//PlayCharaDamageSound( part, TRUE);


	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_Death( VEC3D * pHitDir, CHARA_DEATH_TYPE type, INT32 nKillerIdx)
{	
	//ACTION_IdleUpper();
	CALL_ACTION_COMMAND(ACTION_CMD_STEADYUPPER, NULL);

	m_pAnimContext->getAIUpper()->SetAIStateImmediately( g_pViewer->getDeltaTime());

	//	Death �����̹Ƿ� NONE ���·� ����� �ش�.
	getActionContext()->setBody2( CHARA_BODY_NONE);
	getActionContext()->setUpper( CHARA_UPPER_NONE);

	if( is1PV())
	{
		//_CAMERA_SetIdle();			//	�Ϲ� ī�޶� ���� (ĳ���� Visible On, ScopeHUD off, Cam FOV default)
		m_pCameraContext->SetIdleCamera();
	}	

	ApplyCrosshair();
	
	m_pMoveContext->setVelocity( 0.0f, 0.0f, 0.0f);		// �̵����� 0����
	m_pMoveContext->setAccel( 0.0f, 0.0f, 0.0f);

	ID_LOWER_AI nId = ID_LOWER_DEATH_BACK_A;

	if( is1PV())		//	�ڽ��� ��� Kill Camera�� ����
	{
		g_pCameraManager->ChangeCamera( "Kill", 0.0f);
		g_pCameraManager->setTargetCharacter(this);
		// ų ī�޶� ���۽� 1��Ī ĳ���ʹ� �Ⱥ��̵��� �մϴ�.
		SetVisible( FALSE );	
		
		if( g_pCamera->IsExactTypeOf( CGameCamera_Kill::GetClassMeta()))
		{
			CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
			pKillCam->SetEnemyIdx( nKillerIdx);
		}

		//	Screen Effect �ʱ�ȭ
		//{
		//	CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		//	I3ASSERT( pStage != NULL);

		//	i3RasterizeChain_Blur * pRasterizeBlur = g_pFramework->GetRasterizeBlur();
		//	I3ASSERT( pRasterizeBlur != NULL);

		//	CHud * pHud = pStage->getHud();
		//	if( pHud)		
		//	{
		//		pHud->SetScreenEffectEnable( FALSE);
				//pRasterizeBlur->SetEnable( FALSE);	//	<- �׾��� ��쵵 ��� ��� �� �����Ϸ��� �ּ� ó�� �Ұ�!
				
				//	�׾��� ��쵵 ��� ��� �� �����Ϸ��� Ȱ��ȭ �Ұ�!
		//		if( m_rFlashElapseTime > 0.0f)		//	���� ȿ���� �������̸� ��� �� ����Ʈ�� ����.
		//		{
		//			pRasterizeBlur->SetEnable( FALSE);

		//			pHud->SetScreenTex2DEnable( FALSE);
		//			m_rDamageElapseTime = 0.0f;	
		//		}

		//		m_rFlashElapseTime = m_rFlashEffectiveTime = 0.0f;
				//m_rDamageElapseTime = 0.0f;			//	<- �׾��� ��쵵 ��� ��� �� �����Ϸ��� �ּ� ó�� �Ұ�!
		//	}
		}
	}
	else				//	�ڱ�� ĳ����
	{
		ChangeAnimSet( CHARA_ANIMSET_3PV, FALSE, !is1PV());
	}

	if( EFFECT_IsAvailable())
	{
		EFFECT_Clear();
	}

	_ANIM_stop();

	m_pAnimContext->getAIUpper()->SetEnable( false, true);
	m_pAnimContext->setAIUpper( NULL);

	ANIM_playLower( CHARA_LOWER_DEATH, nId);

	// ��� ����� ���ϴ�.
	addEnableStateMask( CHARA_STATE_DISABLE_CONTROL | CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE |
		CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE );

	addStateMask( CHARA_STATE_USED_COMBINE_ANIM);

	//EnableCollisionGroup( FALSE );

	//	���⵵ ��� ������ �����ϰ� Ready ���·�
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon != NULL)
	{
		pWeapon->ReturnToReady();
		pWeapon->SetVisible( TRUE);
	}

	//I3TRACE( "Death!!  - %d\n", getCharaNetIndex() );

	return TRUE;
}

void CGameCharaPlayer::ACTION_GhostFire( WEAPON_FIRE_ORDER type)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);

	CHARA_ACTION_UPPER	state = CHARA_UPPER_NONE;
	ID_UPPER_AI nId = ID_UPPER_UNKNOWN;

	//	�� ����
	if( type == WEAPON_PRIMARY_FIRE)
	{
		state = CHARA_UPPER_FIRE;

		switch( pWeaponInfo->m_TypeClass)
		{			
		case WEAPON_CLASS_KNIFE:
			{	//	�������� ��� ���� ó��
				I3ASSERT( pWeapon->IsExactTypeOf( WeaponKnife::GetClassMeta()));
				WeaponKnife * pWeaponKnife = (WeaponKnife *) pWeapon;

				pWeaponKnife->SetAttackType( WEAPON_KNIFE_ATTACK );
				nId = ID_UPPER_ATTACK;
			}
			break;
		case WEAPON_CLASS_HANDGUN:
		case WEAPON_CLASS_ASSAULT:
		case WEAPON_CLASS_SMG:
		case WEAPON_CLASS_MG:
		case WEAPON_CLASS_SNIPER:
		case WEAPON_CLASS_SHOTGUN:	
		case WEAPON_CLASS_THROWING:
		case WEAPON_CLASS_MISSION:
		case WEAPON_CLASS_ITEM:
			{
				nId = ID_UPPER_ATTACK;
			}
			break;
		default:	I3ASSERT_0;
		}
	}
	//	���� ����
	else if( type == WEAPON_SECONDARY_FIRE)
	{
		state = CHARA_UPPER_SECONDARY_FIRE;
		nId = ID_UPPER_SECONDARY_ATTACK;

		if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_KNIFE )
		{
			I3ASSERT( pWeapon->IsExactTypeOf( WeaponKnife::GetClassMeta()));
			WeaponKnife * pWeaponKnife = (WeaponKnife *) pWeapon;

			pWeaponKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
		}
	}

	I3ASSERT( state != CHARA_UPPER_NONE && nId != ID_UPPER_UNKNOWN);

	// �߻�!!		
	ANIM_playUpper( state, nId);
}

BOOL CGameCharaPlayer::ACTION_FireReady( void)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);
	
	pWeapon->SetVisible( TRUE);

	// (�ӽ�) �߻� �غ�. ���� ��ô ���⸸ ������ ����....����� �ִϸ��̼� �߰� �ʿ���.
	if( pWeaponInfo->m_TypeClass != WEAPON_CLASS_THROWING)		return FALSE;
	
	pWeapon->FireReady();

	ANIM_playUpper( CHARA_UPPER_FIRE_READY, ID_UPPER_ATTACK_READY);	

	addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	�߻� �غ��߿� �߻����� ���ϵ���. �ִϸ��̼��� ������ �߻� ����

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_Fire( VEC3D * pTargetPos, WEAPON_FIRE_ORDER type)
{
	ACTION_SyncWeapon();
	bool bApplyExt = true;

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

	if( !isNetwork() )
	{
		ACTION_GhostFire( type);
		pWeapon->Fire( type);

		// �̰�	�����̳� ��ȹ���� ��û���� ������� �ʽ��ϴ�. �߻��߿� ���ҾȵǴ� ����Դϴ�.
		//addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
		// ����ź �迭�� ���׷� ������ ���ϰ� ������ �� �ֽ��ϴ�.
		if( pWeapon->getWeaponInfo()->m_TypeClass == WEAPON_CLASS_THROWING )
		{
			addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );
		}

		ApplyCrosshair();
	}

	if( pWeapon->getExtensionType() & WEAPON_EXT_MASK_NEED_TO_ACTIVATE)
	{
		if( pWeapon->isExtensionActivate() == false)
		{
			bApplyExt = false;
		}
	}

	if( !(pWeapon->getExtensionType() & WEAPON_EXT_MASK_DIRECT_USE))
	{
		if( !isStateMask( CHARA_STATE_ZOOM))
			bApplyExt = false;
	}

	// Extension�� Ȱ��ȭ �Ǿ� ���� ���� ó��..

	// ���� �ݵ�
	CHARAPLAYER_RECOIL * pRecoilData = m_pBoneContext->getRecoilData();

	{
		REAL32 factor = 1.0f;

		if( bApplyExt)		factor = pWeaponInfo->m_ExtRecoilVertFactor;

		pRecoilData->m_RecoilAngle +=	pWeapon->getRecoil() * factor;
		if( pRecoilData->m_RecoilAngle > pWeapon->getRecoilMax())
			pRecoilData->m_RecoilAngle = pWeapon->getRecoilMax();

		pRecoilData->m_RecoilTargetAngle = pRecoilData->m_RecoilAngle;
		pRecoilData->m_RecoilTargetTime = log10f( pRecoilData->m_RecoilAngle);
		pRecoilData->m_RecoilTime = 0.0f;
	}

	// ���� �ݵ�
	{
		INT32 limitCount;

		if( bApplyExt)
			limitCount = pWeaponInfo->m_ExtRecoilHorzCount;
		else
			limitCount = pWeapon->getRecoilHorzCount();

		// ���� ���� ������ ���翡���� ���� �ݵ��� �������� �ʴ´�.
		if( limitCount < pWeapon->getAutoFireCount())
		{
			REAL32 factor = 1.0f;

			if( bApplyExt)	factor = pWeaponInfo->m_ExtRecoilHorzFactor;

			pRecoilData->m_RecoilHorzAngle = i3Math::Randf2() * pWeapon->getRecoilHorz() * factor;

			// ���� ����� Theta�� ���� ������ ������ ����ٸ�
			// �ݴ�� �������� ����ǵ��� �Ѵ�.
			{
				REAL32 diff = i3Math::abs( (I3_RAD2DEG( m_pBoneContext->getTheta()) + pRecoilData->m_RecoilHorzAngle) - I3_RAD2DEG(pRecoilData->m_HorzStartAngle));

				if( diff > pWeapon->getRecoilHorzMax())
				{
					//I3TRACE( "Turn Angle : %f, %f, %f\n", I3_RAD2DEG(m_Theta), pRecoilData->m_RecoilHorzAngle, I3_RAD2DEG( pRecoilData->m_HorzStartAngle));

					pRecoilData->m_RecoilHorzAngle = -pRecoilData->m_RecoilHorzAngle;		// ���� ������
				}
			}

			pRecoilData->m_RecoilHorzTime = 0.0f;
			pRecoilData->m_RecoilHorzTimeTarget = MIN( pWeapon->getWeaponInfo()->m_rFireDelay, 0.5f);

			if( pRecoilData->m_RecoilHorzTimeTarget > I3_EPS )
				pRecoilData->m_rRecoilHorzTimeTarget = 1.0f / pRecoilData->m_RecoilHorzTimeTarget;
			else
				pRecoilData->m_rRecoilHorzTimeTarget = 0.0f;
		}
	}

	// ����
	CHARAPLAYER_DEVIATION * pDeviationData = m_pBoneContext->getDeviationData();
	{
		REAL32 dev, factor = 1.0f;

		if( bApplyExt)	factor = pWeaponInfo->m_ExtDeviationFactor;

		dev = pWeapon->getDeviation() * factor;

		pDeviationData->m_FireDeviation = pDeviationData->m_DeviationAngle;
		pDeviationData->m_DeviationAngle = pDeviationData->m_DeviationTargetAngle = pDeviationData->m_DeviationAngle + dev;
		pDeviationData->m_DeviationStartAngle = pDeviationData->m_DeviationAngle;

		if( pDeviationData->m_DeviationTargetAngle > pWeaponInfo->m_rCrossHairMax)
			pDeviationData->m_DeviationTargetAngle = pWeaponInfo->m_rCrossHairMax;

		pDeviationData->m_DeviationTargetTime = pWeapon->getCrosshairSpeedFactor() * pWeaponInfo->m_CrossHairContractTime;
		pDeviationData->m_DeviationTime = 0.0f;
		pDeviationData->m_DeviationState = DEVIATION_CHANGE;
	}

	return TRUE;	
}

BOOL CGameCharaPlayer::ACTION_ToggleExtensionWeapon( void)
{
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	BOOL bActivate = pWeapon->isExtensionActivate();
	
	return ACTION_ExtensionWeapon( !bActivate);
}

BOOL CGameCharaPlayer::ACTION_ExtensionWeapon( BOOL bActivate)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	//	���� Extension ����
	if( pWeapon->ExtensionActivate( bActivate) == FALSE)
		return FALSE;

	pWeapon->SetVisible( TRUE);

	//	���� Extension ��� Ȱ��ȭ
	if( bActivate)
	{	
		ANIM_playUpper( CHARA_UPPER_EXTENSION_ACTIVATE, ID_UPPER_EXTENSION_ACTIVATE);

#if defined( I3_DEBUG )
		if( !isNetwork() )
			g_pFramework->getChatBox()->PutSystemChatting( "Extension On");
#endif
	}
	else
	{	
		ANIM_playUpper( CHARA_UPPER_EXTENSION_DEACTIVATE, ID_UPPER_EXTENSION_DEACTIVATE);

#if defined( I3_DEBUG )
		if( !isNetwork() )
			g_pFramework->getChatBox()->PutSystemChatting( "Extension Off");
#endif
	}

	if( !isNetwork() )
		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	�߻� �غ��߿� �߻����� ���ϵ���. �ִϸ��̼��� ������ �߻� ����

	return TRUE;
}*/

//BOOL CGameCharaPlayer::ACTION_Hit( WeaponBase * pShooterWeapon)
/*
BOOL CGameCharaPlayer::CharaHit( WeaponBase * pShooterWeapon)
{
	I3ASSERT( pShooterWeapon != NULL);

	CWeaponInfo * pWeaponInfo = pShooterWeapon->getWeaponInfo();

	CHARA_DAMAGEINFO DamageInfo;							
	CGameCharaPlayer * pKiller = pShooterWeapon->getOwner();
	if( pKiller == NULL )
		return FALSE;

	DamageInfo._WeaponType		= pWeaponInfo->m_TypeClass;
	DamageInfo._DamageType		= pWeaponInfo->m_TypeDamage;
	DamageInfo._rEffectiveTime	= pWeaponInfo->m_rEffectiveTime;
	DamageInfo._rExplosionRange = pWeaponInfo->m_rExplosion_Range;
	DamageInfo._rExplosionSlope = pWeaponInfo->m_rExplosion_EffectiveSlope;
	DamageInfo._rDamage			= pShooterWeapon->getAP();	
	DamageInfo._IsOwner			= !pKiller->isNetwork();
	DamageInfo._nKillerIdx		= pKiller->getCharaNetIndex();
	DamageInfo._nHarmerIdx		= pWeaponInfo->m_TypeUsage;
	DamageInfo._pHitResult		= NULL;
	DamageInfo._DeathType		= CHARA_DEATH_UN;	
	DamageInfo._pVecDir			= NULL;
	DamageInfo._pWallHitResult	= NULL;
	DamageInfo._nWeaponNumber	= pWeaponInfo->m_nNumber;

	{
		VEC3D diff;

		i3Vector::Sub( &diff, pKiller->GetPos(), GetPos());
		DamageInfo._rDistance	= i3Vector::Length( &diff);
	}
	
	if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING || pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION)
		DamageInfo._pVecPos		= pShooterWeapon->GetPos();
	else
		DamageInfo._pVecPos		= pKiller->GetPos();

	//I3ASSERT( DamageInfo.isValid() );

	//return ACTION_Hit( &DamageInfo);
	return CharaHit( &DamageInfo);
}
*/
//BOOL CGameCharaPlayer::ACTION_Hit( CHARA_DAMAGEINFO * pDamageInfo)
/*
BOOL CGameCharaPlayer::CharaHit( CHARA_DAMAGEINFO * pDamageInfo)
{
	I3_PHYSIX_HIT_RESULT * pResult;
	I3ASSERT( pDamageInfo != NULL);

	INT32 effectCount = 1;

	// Last Attacker�� ���� ����
	{		
		m_pLastAttacker = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx);
	}
	
	switch( pDamageInfo->_HitPart )
	{
	case CHARA_HIT_HEAD :	effectCount = 6;	break;
	case CHARA_HIT_NECK :
	case CHARA_HIT_LCLAVICLE :
	case CHARA_HIT_RCLAVICLE :
	case CHARA_HIT_BODY1 :
	case CHARA_HIT_BODY2 :
	case CHARA_HIT_BODY3 :
	case CHARA_HIT_ROOT :
		effectCount = 3;	break;
	}
	
	{
		//	1��Ī �ڽ��� �ǰ� ������ ȭ�� �� ����Ʈ ó���մϴ�.
		if( is1PV()) 
		{
			//	�ǰ� ������ ������ �չ��� �밢�� ���� �����ϰ� ���������� �ڽ��� ������ �Ѵ�.
			VEC3D vPos;
			MATRIX Temp;
			i3Matrix::Identity( &Temp);
			i3Matrix::SetRotateAxisDeg( &Temp, i3Matrix::GetRight( g_pCamera->getCamMatrix()), -45.0f); 		
			i3Vector::Scale( &vPos, i3Matrix::GetAt( g_pCamera->getCamMatrix()), 0.5f);
			vPos.x = -vPos.x;	//	ī�޶��� At�� �Ųٷ�?
			vPos.y = -vPos.y;
			vPos.z = -vPos.z;
			i3Vector::Add( &vPos, i3Matrix::GetPos( m_pBoneContext->getIKMatrix()), &vPos);

			//	Hit Effect
			g_pEffectManager->AddCharaHitEffect( WEAPON_CLASS_UNKNOWN, &vPos, i3Matrix::GetUp( &Temp), pDamageInfo->_rDistance);
		    
			//	Hit Sound
			//g_pEffectManager->PlayCharaHitSound( i3Matrix::GetPos( getHeadMatrix()), pDamageInfo->_WeaponType, TRUE);
			if( pDamageInfo->_WeaponType == WEAPON_CLASS_KNIFE)
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNIFE_HUMAN, 0, &vPos, TRUE);
			}
			else
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_HUMAN, 2, &vPos, TRUE);
			}
		}
		//	3��Ī ���
		else
		{
			if( pDamageInfo->_pHitResult != NULL)
			{	
				MATRIX mat;
				VEC3D vPos, vRand;
				VEC3D * pPos, * pNormal;
				REAL32 rRandFactor = 0.05f;

				pPos = &pDamageInfo->_pHitResult->m_Intersect;
				pNormal = &pDamageInfo->_pHitResult->m_Normal;
				i3Vector::Zero( &vRand);

				if( pDamageInfo->_WeaponType == WEAPON_CLASS_THROWING )
				{	// ����ź ���� ó��
					for( INT32 i = 0; i < m_PhysixShapeSetList.GetCount(); i++ )
					{
						i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_PhysixShapeSetList.GetItem( i);
						if( getCollisionContext()->GetHitPart( pShapeSet) == CHARA_HIT_NECK)
						{
							pShapeSet->getShapeGlobalPose( &mat);

							pPos = i3Matrix::GetPos( &mat);
							break;
						}
					}

					//pNormal = &vNormal;
					//i3Vector::Set( &vNormal, 0.f, 1.f, 0.f);
					effectCount = 3;
					rRandFactor = 0.5f;
				}

				for( INT32 i = 0; i < effectCount; i++ )
				{
					i3Vector::Add( &vPos, pPos, &vRand);
					g_pEffectManager->AddCharaHitEffect( pDamageInfo->_WeaponType, &vPos, 
														&pDamageInfo->_pHitResult->m_Normal, pDamageInfo->_rDistance);
					i3Vector::Set( &vRand, i3Math::Randf2() * rRandFactor, i3Math::Randf2() * rRandFactor, i3Math::Randf2() * rRandFactor);
				}

				BOOL bListener = (pDamageInfo->_nKillerIdx == g_pGameContext->getMySlotIdx()) ? TRUE : FALSE;

				if( bListener)
				{
					if( pDamageInfo->_WeaponType == WEAPON_CLASS_KNIFE)
					{
						g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNIFE_HUMAN, 0, &pDamageInfo->_pHitResult->m_Intersect, TRUE);
					}
					else
					{
						g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_HUMAN, i3Math::Rand()%2, &pDamageInfo->_pHitResult->m_Intersect, TRUE);
					}
				}
				else
				{
					if( pDamageInfo->_WeaponType == WEAPON_CLASS_KNIFE)
					{
						g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_HIT_KNIFE_HUMAN, 0, &pDamageInfo->_pHitResult->m_Intersect, FALSE);
					}
					else
					{
						g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_HIT_HUMAN, i3Math::Rand()%2, &pDamageInfo->_pHitResult->m_Intersect, FALSE);
					}
				}
			}
		}
	}

	// Wall effect
	if( g_pEnvSet->m_bEnableBlood )
	{
		if( pDamageInfo->_rDamage < I3_EPS )
			pDamageInfo->_rDamage = I3_EPS;

		REAL32	rHeight = 1.f /(REAL32)pDamageInfo->_rDamage;
		BLOOD_EFFECT_TYPE effectType = BLOOD_EFFECT_ASSAULT;

		switch( pDamageInfo->_WeaponType )
		{
		case WEAPON_CLASS_ASSAULT :		effectType = BLOOD_EFFECT_ASSAULT;	rHeight *= 21.f;	break;
		case WEAPON_CLASS_SMG :			effectType = BLOOD_EFFECT_SMG;		rHeight *= 25.f;	break;
		case WEAPON_CLASS_HANDGUN :		effectType = BLOOD_EFFECT_HANDGUN;	rHeight *= 20.f;	break;
		case WEAPON_CLASS_SHOTGUN :		effectType = BLOOD_EFFECT_SHOTGUN;	rHeight *= 15.f;	break;
		case WEAPON_CLASS_KNIFE :		effectType = BLOOD_EFFECT_KNIFE;	rHeight *= 80.f;	break;
		case WEAPON_CLASS_THROWING :	effectType = BLOOD_EFFECT_GRENADE;	rHeight *= 70.f;	break;
		case WEAPON_CLASS_SNIPER :		effectType = BLOOD_EFFECT_RIFLE;	rHeight *= 50.f;	break;
		case WEAPON_CLASS_MG :			effectType = BLOOD_EFFECT_ASSAULT;	rHeight *= 20.f;	break;
		}

		// ���鿡 �Ǹ� ���Դϴ�.
		if( pDamageInfo->_pWallHitResult != NULL )
		{
			pResult = pDamageInfo->_pWallHitResult;
			if( pResult->m_pTerrain != NULL )
			{
				i3Object * pObj = NULL;
				if( pResult->m_pDestShapeSet->getEventReceiver()->IsExactTypeOf( i3Object::GetClassMeta()) )
				{
					pObj = (i3Object*) pResult->m_pDestShapeSet->getEventReceiver();
				}

				if( effectType == BLOOD_EFFECT_KNIFE )
					g_pEffectManager->AddBloodEffect( BLOOD_TERRAIN_FLOOR, effectType, &pResult->m_Intersect, &pResult->m_Normal, pDamageInfo->_pVecDir, 0.f, pObj );
				else
					g_pEffectManager->AddBloodEffect( BLOOD_TERRAIN_WALL, effectType, &pResult->m_Intersect, &pResult->m_Normal, pDamageInfo->_pVecDir, 0.f, pObj );
			}
		}
		else
		{
			// �ٴڿ� �Ǹ� ���Դϴ�.
			VEC3D	vDir, vRand;
			i3CollideeLine Line;

			if( pDamageInfo->_WeaponType == WEAPON_CLASS_KNIFE )
			{
				i3Vector::Set( &vDir, -getX( pDamageInfo->_pVecDir ), -getY( pDamageInfo->_pVecDir ) - rHeight, -getZ( pDamageInfo->_pVecDir ) );
			}
			else
			{
				i3Vector::Set( &vDir, getX( pDamageInfo->_pVecDir ), -getY( pDamageInfo->_pVecDir ) - rHeight, getZ( pDamageInfo->_pVecDir ) );
			}

			if( pDamageInfo->_pHitResult != NULL )
				Line.SetStart( &pDamageInfo->_pHitResult->m_Intersect );
			else
			{
				Line.SetStart( i3Matrix::GetPos( m_pBoneContext->getHeadMatrix() ) );
				//i3Vector::Set( &vDir, -getX( &vDir), -getY(&vDir), -getZ(&vDir) );
			}

			i3Vector::Set( &vRand, (REAL32)((i3Math::Rand() % 100)-50)*0.001f, (REAL32)((i3Math::Rand() % 100)-50)*0.001f, (REAL32)((i3Math::Rand() % 100)-50)*0.001f );
			i3Vector::Add( &vDir, &vDir, &vRand );
			Line.SetDir( &vDir );

			UINT32 nResultCount = g_pFramework->raycast( &Line, CGRP_ATTACK );
			if( nResultCount > 0 )
			{
				pResult = g_pFramework->getRaycastResult();
				for( UINT32 i = 0; i < nResultCount; i++ )
				{
					i3Object * pObj = NULL;
					if( pResult[ i ].m_pTerrain != NULL && pResult[ i ].m_T < 5.f )
					{
						I3ASSERT( pDamageInfo->_nKillerIdx < SLOT_MAX_COUNT );

						CGameCharaPlayer * pChara = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx );
						REAL32	rRotate = 0.f;
						if( pChara != NULL )
						{
							rRotate = -pChara->getBoneContext()->getTheta();
						}

						if( pResult[ i ].m_pDestShapeSet->getEventReceiver()->IsExactTypeOf( i3Object::GetClassMeta() ) )
						{
							pObj = (i3Object*) pResult[ i ].m_pDestShapeSet->getEventReceiver();
						}

						VEC3D vDir;
						i3Vector::Sub( &vDir, &pResult[ i ].m_Intersect, Line.GetStart() );
						i3Vector::Normalize( &vDir, &vDir );
						 
						g_pEffectManager->AddBloodEffect( BLOOD_TERRAIN_FLOOR, effectType, &pResult[ i ].m_Intersect, &pResult[ i ].m_Normal, &vDir, rRotate, pObj );
					}	
				}
			}
		}
	}

	//	�ǰ� ��ũ�� ����Ʈ�� �Ѵ�. (Onwer�� �ƴѰ�� �ؾ� �Ѵ�.)
	{		
		CWeaponGlobal * pWeaponGlobal = g_pWeaponInfoDataBase->getGlobal();
		I3ASSERT( pWeaponGlobal != NULL);

		EFFECT_Damage( pDamageInfo->_rEffectiveTime, 
						&pWeaponGlobal->m_DamageEffectColor, 
						pWeaponGlobal->m_DamageEffectBlurStart,
						pWeaponGlobal->m_DamageEffectBlurEnd);
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_ActiveZoom( void)
{
	BOOL bResult = FALSE;

	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL );

	WEAPON_ZOOM_TYPE type = pWeaponInfo->m_TypeZoom;

	//	���� ����� �ִ� ����
	switch( type)
	{
		case  WEAPON_ZOOM_ZOOM:
			if( ! isStateMask( CHARA_STATE_ZOOM))
			{
				_ANIM_playBody2( CHARA_BODY_ZOOM, !is1PV());

				//_CAMERA_SetZoom();		//	���� ī�޶� ����
				m_pCameraContext->SetZoom();
				
				bResult = TRUE;
			}
			break;

		case WEAPON_ZOOM_SCOPE :
			//	������ ����� �ִ� ����
			{
				INT32 nLvl = m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel + 1;

				if( ! isStateMask( CHARA_STATE_ZOOM))
					_ANIM_playBody2( CHARA_BODY_ZOOM, !is1PV());

				//bResult = _CAMERA_SetScope( nLvl);		//	�������� ī�޶� ����	
				bResult = m_pCameraContext->SetScope( nLvl);
			}
			break;

		case WEAPON_ZOOM_STEADY_AIM :
			// �ٸ� � Animation �� Crosshair�� ������� FOV���� ����
			if( ! isStateMask( CHARA_STATE_ZOOM))
			{
				//_CAMERA_SetZoom();		//	���� ī�޶� ����
				m_pCameraContext->SetZoom();

				bResult = TRUE;
			}
			break;
	}

	addStateMask( CHARA_STATE_ZOOM);

	ApplyCrosshair();

	return bResult;
}

BOOL CGameCharaPlayer::ACTION_CancelZoom( void)
{	//	Zoom ���

	BOOL bResult = FALSE;

	if( isStateMask( CHARA_STATE_ZOOM))
	{
		_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV());

		//_CAMERA_SetIdle();			//	�Ϲ� ī�޶� ����
		m_pCameraContext->SetIdleCamera();

		bResult = TRUE;
	}

	removeStateMask( CHARA_STATE_ZOOM);

	ApplyCrosshair();

	return bResult;
}

BOOL CGameCharaPlayer::ACTION_ToggleZoom( void)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL );

	WEAPON_ZOOM_TYPE type = pWeaponInfo->m_TypeZoom;

	//	���� ����� �ִ� ����
	switch( type)
	{
		case WEAPON_ZOOM_ZOOM :
			if( isStateMask( CHARA_STATE_ZOOM))
				return ACTION_CancelZoom();		//	���� �����϶� ���
			else
				return ACTION_ActiveZoom();		//	���� �����϶� ����
			break;

		case WEAPON_ZOOM_SCOPE :
			//	������ ����� �ִ� ����
			if( isStateMask( CHARA_STATE_ZOOM))
			{
				if( m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel < pWeaponInfo->m_nMaxScopeLevel )
					return ACTION_ActiveZoom();
				else
					return ACTION_CancelZoom();		//	���� �����϶� ���
			}
			else
				return ACTION_ActiveZoom();		//	���� �����϶� ����
			break;

		case WEAPON_ZOOM_STEADY_AIM :
			if( ! isStateMask( CHARA_STATE_ZOOM))
				ACTION_ActiveZoom();
			else
				ACTION_CancelZoom();
			break;

	}

	return FALSE;
}

void CGameCharaPlayer::ACTION_SetObserveZoom(void)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();

	if( pWeapon == NULL )
		return;

	I3ASSERT( pWeapon->getWeaponInfo() != NULL);

	WEAPON_ZOOM_TYPE type = pWeapon->getWeaponInfo()->m_TypeZoom;
	BOOL	bCheckZoom = FALSE;

	if( isAlive() )
	{
		if( type == WEAPON_ZOOM_ZOOM || type == WEAPON_ZOOM_SCOPE )
		{
			if( getActionContext()->isBody2( CHARA_BODY_ZOOM ))
			{
				if( type == WEAPON_ZOOM_ZOOM)
				{
					//_CAMERA_SetZoom();
					m_pCameraContext->SetZoom();
					bCheckZoom = TRUE;
				}
				else if( type == WEAPON_ZOOM_SCOPE)
				{
					//_CAMERA_SetScope( m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel);
					m_pCameraContext->SetScope( m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel);
					bCheckZoom = TRUE;
				}
			}
		}
	}

	if( !bCheckZoom )
	{
		//_CAMERA_SetIdle();
		m_pCameraContext->SetIdleCamera();
	}
}*/

//void CGameCharaPlayer::ACTION_SyncWeapon( void)
/*
void CGameCharaPlayer::SyncWeapon( void)
{
	//	���� ��ü��(SwapWeapon)�� ���� �����ӿ� ����Ǳ� ������ ���� �����ӿ��� ��ü ���⸦ ������ �ʿ䰡 ���� ���
	//	ȣ���ؾ� �մϴ�. (��Ʈ��ũ ���� ...)
	//	
	//	��.)
	//	Frame			Command			Current Weapon
	//=====================================================================
	//	(1) :	SwapWeapon (No.1)	CurWeapon(No.0) <= current (before swap)
	//			Fire (No.1)					:
	//	(2) :		:				CurWeapon(No.1)	<= Apply swap 

	//if( m_nSwapWeaponSlotNum != m_nCurWeaponSlotNum)
	INT32 nSwapSlot = getWeaponContext()->getSwapSlotNum();
	INT32 nCurrentSlot = getWeaponContext()->getCurrentSlotNum();
	INT32 nOldSlot = getWeaponContext()->getOldSlotNum();

	if( nSwapSlot != nCurrentSlot )
	{
		//	���� ��� ����
		getWeaponContext()->getControl()->OnSelectWeapon( nSwapSlot);
		if( isNetwork() )	// invalid using
		{
			getWeaponContext()->getControl()->OnAttachPurchasePrimWeapon( nOldSlot);
			getWeaponContext()->getControl()->OnAttachPurchaseSideWeapon( nOldSlot);
		}
		
		//ACTION_apply( !is1PV());
		getAnimContext()->getControl()->AnimSetApply();

		ApplyCrosshair();
	}
	/*
	if( getWeaponContext()->getWeaponSwapSlotNum() != getWeaponContext()->getCurrentSlotNum() )
	{
		//	���� ��� ����
		WEAPON_Select( m_nSwapWeaponSlotNum);
		if( isNetwork() )	// invalid using
		{
			WEAPON_AttachToBackpack( m_nOldWeaponSlotNum);
			WEAPON_AttachToSidepack( m_nOldWeaponSlotNum);
		}
		
		ACTION_apply( !is1PV());

		ApplyCrosshair();
	}
	*/
}
/*
BOOL CGameCharaPlayer::ACTION_SwapWeapon( INT32 slot)
{
	// ���� ���� �˻�
	if (TRUE == g_pGameContext->IsLockedWeaponSlot(slot))
	{
		return FALSE;
	}

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( slot == 5)
	{
		if( (pMissionMng->getStageType() == STAGE_TYPE_BOMB) &&
			(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) )
			return FALSE;	// ��ź�� ��� ��ġ���� �ƴ϶�� �ٲ� �� �����ϴ�.
	}

	WeaponBase * pSwapWeapon = getWeaponContext()->getWeapon( slot);
	INT32	nCurSlot = getWeaponContext()->getCurrentSlotNum();
	
	if( pSwapWeapon != NULL && nCurSlot != slot)
	{
		CWeaponInfo * pWeaponInfo = pSwapWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);

		if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING ||
			pWeaponInfo->m_TypeClass == WEAPON_CLASS_ITEM || 
			pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION)
		{
			//	�Ҹ� ������ Throwing �Ǵ� Item�� ���� ������ 0�̸� ��ü�� �� ����.
			if( pSwapWeapon->getLoadedBulletCount() <= 0)
				return FALSE;
		}

		//	Weapon Change
		getWeaponContext()->getControl()->OnSwapWeapon( slot);*/

		//--> WeaponControl
		/*
		WeaponBase * pOldWeapon = getWeaponContext()->getCurrent();
		if( pOldWeapon != NULL)
		{	
			//	ĳ���� �ִϸ��̼��� �����ϸ� ���� �����Ӻ��� ����Ǳ� ������ �̹� �����ӿ� ���⸦ �ٲ��� �ʰ� �ٸ� �����ӿ� 
			//	���⸦ �ٲٵ��� �Ѵ�.
			m_nOldWeaponSlotNum = m_nCurWeaponSlotNum;
			m_nSwapWeaponSlotNum = slot;		//	���� �����ӿ� �ٲ� ���� ��ȣ 
		}
		//	���� ���Ⱑ NULL�̸� ���� �ٲ۴�.
		else
		{			
			//	�ִϸ��̼� �ϱ� ���� ���� ���� ��ü(�𵨸�)�� �ؾ� �Ѵ�.
			WEAPON_Select( slot);			

			WeaponBase * pChangeWeapon = getWeaponContext()->getCurrent();
			if( pChangeWeapon != NULL )
			{
				if( isNetwork() )
					pChangeWeapon->NET_WeaponChange();
				else
					pChangeWeapon->WeaponChange();		//	���� ��ü�� ���� ����� Change ���·�
			}
		}

		//	����� �⺻ �����տ� ����	
		ChangeWeaponAttachHand( CHARA_WEAPON_ATTACH_RIGHT_HAND);

		pSwapWeapon->SetVisible( TRUE);

		*/

		/*getActionContext()->setBody2( CHARA_BODY_ATTACKIDLE);

		if( isStateMask( CHARA_STATE_ZOOM))
		{	//	���� ���¿����� �Ϲ� ī�޶�� ���ư��ϴ�
			//_CAMERA_SetIdle();
			m_pCameraContext->SetIdleCamera();
		}

		if( m_pAnimContext->getAIUpper() != NULL)
		{
			m_pAnimContext->getAIUpper()->StopAI( g_pViewer->getDeltaTime());
		}

		//ACTION_apply( !is1PV());	//	�̹� ĳ���ʹ� ���� ������ �������� �����Ѵ�.
		getAnimContext()->getControl()->AnimSetApply();

		//	��ü�� ������ �ִϸ��̼��� �Ѵ�.
		ANIM_playUpper( CHARA_UPPER_SWAPWEAPON, ID_UPPER_SWAP_WEAPON);
		
		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		//addStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE);
		
		// ��Ʈ��ũ�� �����ݴϴ�.
		//if( !isNetwork() )
		//	g_pGameContext->WriteMyWeaponChange( m_nCurWeaponSlotNum );		//-->WeaponControlLocal

		//I3TRACE("Changing! Weapon (%d)\n", slot);

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_SwapNextWeapon( void)
{
	INT32 nIdx = getWeaponContext()->getCurrentSlotNum() + 1;

	if( nIdx > MAX_ITEMSLOT_COUNT - 1)		nIdx = 0;

	if( nIdx == 5)
	{
		CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
		if( (pMissionMng->getStageType() == STAGE_TYPE_BOMB) &&
			(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) )
			nIdx = 0;	// ��ź�� ��� ��ġ���� �ƴ϶�� �ٲ� �� �����ϴ�.
	}

	//	���� ��ȣ�� ����� ��ü
	while( !ACTION_SwapWeapon( nIdx))
	{
		if( nIdx < MAX_ITEMSLOT_COUNT - 1)
			nIdx++;
		else	
			nIdx = 0;

		if( nIdx == getWeaponContext()->getCurrentSlotNum())	//	�ѹ��� ����( �ٲ� ���Ⱑ ���ٸ�)
			return FALSE;
	}

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_SwapPrevWeapon( void)
{
	INT32 nIdx = getWeaponContext()->getCurrentSlotNum() - 1;

	if( nIdx < 0)		nIdx = MAX_ITEMSLOT_COUNT - 1;

	if( nIdx == 5)
	{
		CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
		if( (pMissionMng->getStageType() == STAGE_TYPE_BOMB) &&
			(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) )
			nIdx = 4;	// ��ź�� ��� ��ġ���� �ƴ϶�� �ٲ� �� �����ϴ�.
	}

	//	���� ��ȣ�� ����� ��ü
	while( !ACTION_SwapWeapon( nIdx))
	{
		if( nIdx > 0)
			nIdx--;
		else
			nIdx = MAX_ITEMSLOT_COUNT - 1;

		if( nIdx == getWeaponContext()->getCurrentSlotNum())	//	�ѹ��� ����( �ٲ� ���Ⱑ ���ٸ�)
			return FALSE;
	}

	return TRUE;
}

BOOL CGameCharaPlayer::ACTION_SwapOldWeapon( void)
{
	if( getWeaponContext()->getCurrentSlotNum() != getWeaponContext()->getOldSlotNum())
	{
		//	�ٷ� ���� ����� ��ü
		return ACTION_SwapWeapon( getWeaponContext()->getOldSlotNum());
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_LoadMagazine( void)
{
	REAL32 rTimeScale = 1.f;
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	if( pWeaponInfo == NULL )
		return FALSE;

	if( pWeapon->IsLoadMagazineLeftGrip() )
	{
		ChangeWeaponAttachHand( CHARA_WEAPON_ATTACH_LEFT_HAND);
		pWeapon->ChangeGunDummy( WEAPON_ATTACH_LEFT_GRIP);
	}
	
	{	
		INT32 nID = m_pAnimContext->getAIUpperID( getAnimSet(), pWeaponInfo->m_TypeClass, pWeaponInfo->m_nNumber, getActionContext()->getBody2(), ID_UPPER_LOAD_MAGAZINE);
		i3AIState * pNewAIState = m_pAnimContext->getAIUpper()->getAIState( nID );

		if( pNewAIState != NULL && pWeaponInfo->m_rReloadTime > I3_EPS )
			rTimeScale = pNewAIState->getAnimDuration() / pWeaponInfo->m_rReloadTime;
	}
	
	pWeapon->SetVisible( TRUE);

	//	�ѱ� �迭�� ����
	if( pWeaponInfo->m_TypeFire == WEAPON_FIRE_RANGE)
	{
		if( isNetwork() )
		{
			ANIM_playUpper( CHARA_UPPER_LOADMAGAZINE, ID_UPPER_LOAD_MAGAZINE, 0.f, rTimeScale );
			pWeapon->NET_LoadMagazine();

			// ������� 1��Ī �����϶� ������ ����
			if( g_pCamera && g_pCamera->IsExactTypeOf( CGameCamera_FPS::GetClassMeta()) )
			{	
				if( isStateMask( CHARA_STATE_ZOOM))
				{
					//_CAMERA_SetIdle();
					m_pCameraContext->SetIdleCamera();
				}
			}
		}
		else
		{
			//	���� �Ѿ��� ������ ��� ������ �� �� ����.
			if( pWeapon->getTotalBulletCount() <= 0)	
				return FALSE;

			if( (pWeapon->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) && pWeapon->getLoadedBulletCount() == pWeapon->getWeaponInfo()->m_nLoadBullet )
				return FALSE;

			if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON ))	// ���� �����߿��� ���ε� ����
				return FALSE;

			ANIM_playUpper( CHARA_UPPER_LOADMAGAZINE, ID_UPPER_LOAD_MAGAZINE, 0.f, rTimeScale);

			pWeapon->LoadMagazine();

			//	���� �Ǵ� �������� ī�޶� ���¿����� �����ÿ� �Ϲ� ���� ���·� ���ƿɴϴ�.
			if( isStateMask( CHARA_STATE_ZOOM))
			{
				//_CAMERA_SetIdle();
				m_pCameraContext->SetIdleCamera();
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_LoadMagazineDual( void )
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	pWeapon->SetVisible( TRUE);

	if( pWeapon->getWeaponInfo()->m_TypeFire == WEAPON_FIRE_RANGE)
	{
		//	���� �Ѿ��� ������ ��� ������ �� �� ����.
		if( pWeapon->getTotalBulletCount() <= 0)	
			return FALSE;
		//	���� Extension ���� 
		pWeapon->ExtensionActivate( TRUE);

		//	���� Extension ��� Ȱ��ȭ
		ANIM_playUpper( CHARA_UPPER_EXTENSION_ACTIVATE, ID_UPPER_EXTENSION_ACTIVATE);

		g_pFramework->getChatBox()->PutSystemChatting( "Extension On");

		addStateMask( CHARA_STATE_DISABLE_SHOT);	//	�߻� �غ��߿� �߻����� ���ϵ���. �ִϸ��̼��� ������ �߻� ����

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_LoadBullet( void)
{
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	// MG�� ź�� �����ÿ��� �޼� ����
	if( pWeapon->IsLoadBulletLeftGrip() )
	{
		ChangeWeaponAttachHand( CHARA_WEAPON_ATTACH_LEFT_HAND);
		pWeapon->ChangeGunDummy( WEAPON_ATTACH_LEFT_GRIP);
	}

	pWeapon->SetVisible( TRUE);

	//	�ѱ� �迭�� ����
	if( pWeapon->getWeaponInfo()->m_TypeFire == WEAPON_FIRE_RANGE)
	{
		if( isNetwork() )
		{	// 2006. 9. 23 by ������
			// desc : ��Ʈ��ũ �κ� �߰�
			ANIM_playUpper( CHARA_UPPER_LOADBULLET, ID_UPPER_LOAD_BULLET);
			pWeapon->NET_LoadBullet();
			
			// ������� 1��Ī �����϶� ������ ����
			if( g_pCamera && g_pCamera->IsExactTypeOf( CGameCamera_FPS::GetClassMeta()) )
			{
				if( isStateMask( CHARA_STATE_ZOOM))
				{
					//_CAMERA_SetIdle();
					m_pCameraContext->SetIdleCamera();
				}
			}
		}
		else
		{
			if( pWeapon->isLoadBulletEnabled())
			{
				ANIM_playUpper( CHARA_UPPER_LOADBULLET, ID_UPPER_LOAD_BULLET);

				pWeapon->LoadBullet();

				//	���� �Ǵ� �������� ī�޶� ���¿����� �����ÿ� �Ϲ� ���� ���·� ���ƿɴϴ�.
				if( isStateMask( CHARA_STATE_ZOOM))
				{
					//_CAMERA_SetIdle();
					m_pCameraContext->SetIdleCamera();
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::ACTION_FallDamage( void)
{
	//	��ü�� ������ ������ ������ �����̸�
	ACTION_Damage( CHARA_HIT_ROOT, i3Matrix::GetAt( GetMatrix()));
	//	������ ������ ���Ҷ��� ��� �Ҹ��� ���� �Ѵ�.
	m_pSoundContext->PlayCharaDamageSound( CHARA_HIT_BODY1, TRUE);

	//	Post Effect
	if( is1PV() )
	{
		CWeaponGlobal * pWeaponGlobal = g_pWeaponInfoDataBase->getGlobal();

		//	Screen Effect 
		EFFECT_Damage( 1.0f, 
					   &pWeaponGlobal->m_DamageEffectColor, 
					   pWeaponGlobal->m_DamageEffectBlurStart,
					   pWeaponGlobal->m_DamageEffectBlurEnd);
	}

	//	Damage Direction
	if( !isNetwork() )
	{
		ShowDamageHUD( this->GetPos(), this->GetPos(), I3_PI);
	}
}

BOOL CGameCharaPlayer::ACTION_DropEquip( EQUIP_CLASS_TYPE type, i3Node * pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos )
{	
	//	ĳ���� ��� �����
	CGameCharaEquip * pCharaEquip = m_pEquipContext->getControl()->OnDetach( type); 

	//	������ ��� �ִٸ�
	if( pCharaEquip != NULL)
	{	
		pCharaEquip->SetVisible( TRUE);

		//	���ο� �θ� �ٴ´�.
		pCharaEquip->Attach( pNewSceneNode, pNewOwner, FALSE);
		
		//	Ż�� ��ġ ����
		if( pPos != NULL )
		{
			pCharaEquip->SetPos( pPos );

			if( is1PV() )
			{
				MATRIX mat;
				i3Skeleton *pSkel = pCharaEquip->GetSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
				for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
				{
					i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
					if( pShapeSet == NULL ) continue;

					pShapeSet->getShapeGlobalPose( &mat );
					i3Matrix::SetPos( &mat, pPos );
					pShapeSet->setShapeGlobalPose( &mat );
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_ReturnToReady( BOOL bWithCurrentWeapon)
{	
	ACTION_SyncWeapon();

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	BOOL ret = FALSE;

	//	Idle ���·� ���ư���.
	if( bWithCurrentWeapon)
		pWeapon->ReturnToReady();

	//	��ü�� ���� ���·� ���ư���.	
	ret = ANIM_playUpperReturn( !is1PV() );

	return ret;
}

BOOL CGameCharaPlayer::ACTION_DropWeapon( INT32 nSlot, VEC3D * pDropPos)
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);
	if( pWeapon != NULL)
	{
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);

		//	������ ���忡 ������.
		getWeaponContext()->getControl()->OnWeaponDropToWorld( nSlot, pDropPos);

		//	���� ó��
		switch( pWeaponInfo->m_TypeClass)
		{
		case WEAPON_CLASS_MISSION:	
			{
				switch( pWeaponInfo->m_nNumber)
				{
					case WEAPON_NUM_MISSION_C4:
						{
							removeEnableStateMask( CHARA_STATE_DISABLE_MOVE);
							removeEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

							((WeaponC4*)pWeapon)->m_bEnableActive = TRUE;

							//	�ֹ���� �ٷ� ��ü
							CommandParameter * pParam = getCommandContext()->getParameter();
							pParam->_nSlotIdx = 0;
							//if( !ACTION_SwapWeapon( 0) )
							if( !getCommandContext()->Command( CHARA_CMD_SWAPWEAPON))
							{
								//ACTION_SwapOldWeapon();

								if( getWeaponContext()->getCurrentSlotNum() != getWeaponContext()->getOldSlotNum())
								{
									//	�ٷ� ���� ����� ��ü
									pParam->_nSlotIdx = getWeaponContext()->getOldSlotNum();
									getCommandContext()->Command( CHARA_CMD_SWAPWEAPON);
								}
							}
						}
						break;
				}
			}
			break;
		default:
			{
				//	���� ����� �ٷ� ��ü
				//ACTION_SwapNextWeapon();	

				CommandParameter * pParam = getCommandContext()->getParameter();
				pParam->_nSlotIdx = getWeaponContext()->getNextUsableWeaponSlot();

				if(pParam->_nSlotIdx != -1)
				{
					getCommandContext()->Command( CHARA_CMD_SWAPWEAPON);
				}
			}
			break;
		}			
		
		return TRUE;
	}

	return FALSE;
}


void CGameCharaPlayer::ACTION_TurnBack( void)
{
	REAL32 rTheta = m_pBoneContext->getTheta();
	rTheta		+= I3_PI; 
	m_pBoneContext->setTheta( i3Math::NormalizeAngle( rTheta));

	m_pMoveContext->setVelocity( 0.0f, 0.0f, 0.0f);
	m_pMoveContext->setAccel( 0.0f, 0.0f, 0.0f);

	m_pMoveContext->setAccelElapsed( 0.f);

	MATRIX * pCharaMat = GetSceneObject()->GetMatrix();
	i3Matrix::PreRotateAxisDeg( pCharaMat, &I3_YAXIS, 180.0f);
}

BOOL CGameCharaPlayer::ACTION_ActiveMission( WeaponBase * pWeapon)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

#if defined( I3_DEBUG)
	if( g_pConfig->m_bNetworking )
	{
#endif
	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// ���ĸ�尡 �ƴѰ��
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// ��ġ���� �ƴѰ��
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// �������� �ƴѰ��
		return FALSE;
#if defined( I3_DEBUG)
	}
#endif

	I3ASSERT( pWeapon != NULL);
	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);
	
	CGameMissionManager * pMissionManager = g_pGameContext->getMissionManager();
	
	if( (pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION) && (pWeaponInfo->m_nNumber == WEAPON_NUM_MISSION_C4 ) )
	{		
		if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
			(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
		{	
			switch( pMissionManager->getMissionState() )
			{
			case GAME_MISSION_STATE_INBATTLE :					//	�̼ǿ� ��ź�� ��ġ�Ǿ� ���� �ʴٸ�
				if( pMissionManager->IsEnableMy())
				{//	��ź ��ġ�� �����ϸ�
					//	�̵� �Ұ�. 
					i3Vector::Zero( m_pMoveContext->getMoveDir());

					if( !getActionContext()->isLower( CHARA_LOWER_IDLE))
						CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);
						//ACTION_IdleLower();

					//ACTION_Crouch();	// ��ġ�� ������ �ɴ´�.
					CALL_ACTION_COMMAND(ACTION_CMD_CROUCH, NULL);		// ��ġ�� ������ �ɴ´�.
					getCommandContext()->Command(CHARA_CMD_PRIMATTACK);
					//ACTION_Fire();		// ��ź ��ġ!!

					addEnableStateMask( CHARA_STATE_DISABLE_MOVE);
					addEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

					addStateMask( CHARA_STATE_ARMINGBOMB);

					g_pGameContext->WriteMyInstallingMission( TRUE );

					return TRUE;
				}
				else
				{
					CStageBattle * pBattle = (CStageBattle*)g_pFramework->GetCurrentStage();
					I3ASSERT( pBattle != NULL);
					pBattle->getHud()->SetMissionNotice( STBL_IDX_BOMB_MISSION_NONAREA);			// ���������� ��ġ�Ҽ� �ֽ�.
				}
				break;

			case GAME_MISSION_STATE_INSTALLED :					//	�̹� ��ġ �ƴٸ�
				{
					removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				}
				
			default :
				{
					//CStageBattle * pBattle = (CStageBattle*)g_pFramework->GetCurrentStage();
					//I3ASSERT( pBattle != NULL);
					//pBattle->getHud()->SetMissionNotice( STBL_IDX_BOMB_MISSION_NONAREA);			// ���������� ��ġ�Ҽ� �ֽ�.
				}
				
				break;
			}
		}
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_ActiveDisarm( void)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// ���ĸ�尡 �ƴѰ��
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||			// ��ġ �İ� �ƴѰ��
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )		// ������� �ƴѰ��
	{
		return FALSE;
	}
	
	//	��ź ��ü	
	if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
		(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
	{	
		switch( pMissionMng->getMissionState() )
		{
		case GAME_MISSION_STATE_INSTALLED :					//	�̼ǿ� ��ź�� ��ġ�Ǿ� �ִٸ�
			if( pMissionMng->IsEnableMy() && pMissionMng->IsEnableCrossHit() )
			{
				WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4);

				if( pWeapon != NULL)
				{
					I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
					WeaponC4 * pBomb = (WeaponC4 *) pWeapon;

					addStateMask( CHARA_STATE_DISARMINGBOMB);

					pBomb->ActiveDisarm( TRUE);
				}
				else
				{
					removeStateMask( CHARA_STATE_DISARMINGBOMB);
				}
			}
			else
			{
				removeStateMask( CHARA_STATE_DISARMINGBOMB);
			}
			break;
		}
	}

	if( isStateMask( CHARA_STATE_DISARMINGBOMB) )
	{	// ��ü��
		//	�̵� �Ұ�. 
		i3Vector::Zero( m_pMoveContext->getMoveDir());

		if( !getActionContext()->isLower( CHARA_LOWER_IDLE))
			CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);
			//ACTION_IdleLower();

		if( !getActionContext()->isUpper( CHARA_UPPER_IDLE))
			CALL_ACTION_COMMAND(ACTION_CMD_STEADYUPPER, NULL);
			//ACTION_IdleUpper();

		//ACTION_Crouch();	// ��ü�� ������ �ɴ´�.
		CALL_ACTION_COMMAND(ACTION_CMD_CROUCH, NULL);	// ��ü�� ������ �ɴ´�.

		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		addEnableStateMask( CHARA_STATE_DISABLE_MOVE);
		addEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->WriteMyUninstallingMission( TRUE );		//	��ź ��ü

		return TRUE;
	}
	else
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->WriteMyUninstallingMission( FALSE );

		//I3TRACE( "-- ��ź ��ü���...\n");

		return FALSE;
	}
}

BOOL CGameCharaPlayer::ACTION_CancelMission( void)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// ���ĸ�尡 �ƴѰ��
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// ��ġ ���� �ƴѰ��
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// �������� �ƴѰ��
		return FALSE;
	
	if( pMissionMng->IsInstalling( g_pGameContext->getMySlotIdx() ) )
	{	//	�ڽ��� ��ź ��ġ ���̸� ���
		g_pGameContext->WriteMyInstallingMission( FALSE );	//	��ź ��ġ ���
		
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		//if( getActionContext()->isUpper( CHARA_UPPER_FIRE) || getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE))
			//ACTION_ReturnToReady( TRUE);
			getCommandContext()->CmdReturnToReady( TRUE);

		//I3TRACE("-- ��ź ��ġ ���!!\n");

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::ACTION_CancelDisarm( void)
{	//	��ź ��ü ���̸�
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// ���ĸ�尡 �ƴѰ��
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||			// ��ġ �İ� �ƴѰ��
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )		// ������� �ƴѰ��
		return FALSE;

	INT32 mySlotIdx = g_pGameContext->getMySlotIdx();

	if( isStateMask( CHARA_STATE_DISARMINGBOMB) )
	{
		//	�ڽ��� ��ź ���� ���̸� ���
		//I3TRACE("isUnInstalling() : %d, WhoUnInstall() : %d\n", pMissionData->IsUninstalling( g_pGameContext->getMySlotIdx() ), pMissionData->WhoUninstall( nMyNetIdx));
		if( pMissionMng->IsUninstalling( mySlotIdx ))
		{			
			g_pGameContext->WriteMyUninstallingMission( FALSE );		//	��ź ��ü ���

			removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

			removeStateMask( CHARA_STATE_DISARMINGBOMB);

			WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4);

			if( pWeapon != NULL)
			{
				I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
				WeaponC4 * pBomb = ( WeaponC4*)pWeapon;

				pBomb->ActiveDisarm( FALSE);
			}

			//I3TRACE("-- ��ź ��ü ���!!\n");

			return TRUE;
		}
	}

	return FALSE;
}
*/
