#include "pch.h"
#include "MainWeapon_Grenade.h"

#include "BattleObjContext.h"

#include "BattleSlotContext.h"

#include "MyRoomInfoContext.h"
#include "GameCharaCameraContext.h"
#include "BattleServerContext.h"
#include "GameContextUtil.h"

#include "HackContext.h"
#include "GameMode.h"
#include "TutorialMode.h"

#define GRENADE_POS_ASSISTTIME		0.2f
#define GRENADE_POS_ASSISTTIMEINV	5.0f

MainWeapon_Grenade::MainWeapon_Grenade(WeaponBase* p) : MainWeapon_Throw(p)
									, m_rElapsedExplosionTime(0.0f)
									, m_uCount(0)
									, m_bCheckFloor(false)
									, m_rBoundObjectLen(0.0f)
									, m_nFireEffectCount(0)
									, m_nStateFlag(-1)
									, m_bResetBullet(false)
{
	m_pAttachObjectTransform = nullptr;
	m_pAttachObject = nullptr;
}

void MainWeapon_Grenade::OnInitVariable()
{
	MainWeapon_Throw::OnInitVariable();

	m_rElapsedExplosionTime = 0.0f;

	i3Vector::Zero( &m_vObjectForce);

	m_bCheckFloor	= false;
	m_pAttachObjectTransform = nullptr;

	m_rBoundObjectLen = 0.f;

	m_uCount = (i3Math::Rand() & 0x0000FFFF);
	m_uCount |= (g_pRand->GetRand( m_uCount) & 0x00000FFF) << 20;

	m_pAttachObject	= nullptr;
}

void	MainWeapon_Grenade::OnCreate( bool bCreateInWeapon)
{
	m_nFireEffectCount = 0; 

	for(INT32 i=0; i<MAX_FIRE_EFFECT; i++)
	{
		if( m_pWeaponBase->getFireEffectIndex(i) != -1)
			m_nFireEffectCount++;
	}
	
}

void	MainWeapon_Grenade::Reset()
{
	MainWeapon_Throw::Reset();

	m_pWeaponBase->setWeaponStateMask( WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE);
//	m_StateMaskWeapon		= WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE;

	m_rElapsedExplosionTime = 0.0f;

	m_bResetBullet = true;
	m_bCheckFloor			= false;
	m_pAttachObjectTransform = nullptr;

	i3Vector::Zero( &m_vObjectForce);

	m_rBoundObjectLen = 0.f;
	m_pAttachObject = nullptr;

	m_uCount = (i3Math::Rand() & 0x0000FFFF);
	m_uCount |= (g_pRand->GetRand( m_uCount) & 0x00000FFF) << 20;

	m_pWeaponBase->SetEnable( true);

	m_nStateFlag = -1;
	m_vGrenadeStateFlag.clear();

	if (  m_pWeaponBase->getNetworkIdx() != -1)
	{
		GAMEINFO_THROW_WEAPON * pGrenadeInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx());
		pGrenadeInfo->_tNetData.resetPrevPos();
	}	
}

/*virtual*/ void MainWeapon_Grenade::OnEndFire(i3AIContext * pCtx, REAL32 tm)
{
	// ����� �ű�ϴ�.
	if(m_bResetBullet == false)
		CheckBullet();

	MainWeapon_Throw::OnEndFire(pCtx, tm);
}

/*virtual*/ void MainWeapon_Grenade::ChainExplosion(void)
{
	Explosion();
}

namespace
{
	void GrenadeHackDetection(UINT32 curCnt)
	{
		UINT32 ra = i3Math::Rand();
		curCnt = ((((curCnt & 0x000F0000)>>16) + 1) <<16) | (ra & 0x0000FFFF) | (g_pRand->GetRand(ra&0x0000FFFF)<<20);
		if( ((curCnt & 0x000F0000) >> 16) > (UINT32) LocaleValue::GetInt32("FireWall/GrenadeCount") ||
			((curCnt & 0xFFF00000)>>20) != (UINT32) (g_pRand->GetRand(curCnt&0x0000FFFF) & 0x00000FFF) )
		{
			HackContext::i()->HackingDetected( CC_MANY_GRENADE_COUNT * 1.f ,C_MANY_GRENADE_COUNT);
		}
	}
}

	
void	MainWeapon_Grenade::_ProcessTakeObject() // �ϴ� public����..(�Űܾ���)
{
	// ���߸� �ٴ��� �ѹ� üũ�մϴ�. Object ���� �ִ� ��� ������Ʈ matrix�� ������ �ʿ䰡 �ֽ��ϴ�.
	if(!m_bCheckFloor)
		_TakeObject();
	else
		_UpdateTakeObjectTransform();
}


REAL32 MainWeapon_Grenade::_BoundObject( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pDir)
{
	I3ASSERT( pHitResult != nullptr);
	I3ASSERT( pHitResult->m_pDestShapeSet != nullptr);
	
	if( !i3::kind_of<i3Object*>(pHitResult->m_pDestShapeSet->getEventReceiver()))
		return 0.f;

	i3Object * pObj = (i3Object*) pHitResult->m_pDestShapeSet->getEventReceiver();
	i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObj->getSceneObject() );

	if( pTrans == nullptr )
		return 0.f;

	if( pTrans->IsAnimated() )
	{
		// �ִϸ��̼��ϴ� ������Ʈ
		MATRIX	matTop;
		
		i3SceneTracer * pTracer;

		pTracer = g_pFramework->GetViewer()->GetSceneTracer();

		I3ASSERT( pTracer != nullptr);

		pTracer->GetModelViewMatrixStack()->Reset();
		pTracer->SetTime( g_pViewer->getDeltaTime());
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());
		
		pTrans->CalcUpdatedMatrix( &matTop, pTracer);

		VEC3D vNewPosDir;
		i3Vector::Sub( &vNewPosDir, i3Matrix::GetPos( &matTop ), i3Matrix::GetPos( pTrans->GetCacheMatrix() ) );

		if( i3Vector::Dot( &vNewPosDir, &pHitResult->m_Normal ) > 0)
		{
			i3Vector::Scale( &m_vObjectForce, &vNewPosDir, 1.3f );

			if( pDir != nullptr)
			{
				VEC3D vDir;
				i3Vector::Normalize( &vDir, pDir);
				REAL32 rDot = i3Vector::Dot( &pHitResult->m_Normal, &vDir) * I3_PI;
				REAL32 angle = i3Math::sin( I3_PI2 - rDot);

				if( angle > I3_EPS)
				{
					REAL32 rLen = i3Vector::Length( &vNewPosDir);
					REAL32 rBoundVal = rLen / angle;
					if( rBoundVal > rLen)
						rBoundVal = rLen;

					return rBoundVal;
				}
			}
		}
	}

	return 0.f;
}

void MainWeapon_Grenade::_ProcessBoundObjectForce()
{
	i3CollideeLine line;
	VEC3D vStartPos, vEndPos;

	if( getX( &m_vObjectForce) != 0.f || getY( &m_vObjectForce) != 0.f || getZ( &m_vObjectForce) != 0.f)
	{
		// �ѹ��� �ݴ������� �ø��� ������Ʈ�� �ִٸ� �о��.
		i3Vector::Scale( &vStartPos, &m_vObjectForce, 2.f );
		i3Vector::Add( &vStartPos, &vStartPos, &m_ThrowData.m_vOldPos );

		line.SetStart( &vStartPos);

		i3Vector::Scale( &vEndPos, &m_vObjectForce, -2.f);
		i3Vector::Add( &vEndPos, &m_ThrowData.m_vOldPos, &vEndPos);

		line.SetEnd( &vEndPos);
		
		I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);

		if( pHitResult != nullptr )
		{
			I3ASSERT( pHitResult->m_pDestShapeSet != nullptr);
			if( i3::kind_of<i3Object*>(pHitResult->m_pDestShapeSet->getEventReceiver()))
			{
				_BoundObject( pHitResult, nullptr );
				
				i3Vector::Add( m_pWeaponBase->GetPos(), m_pWeaponBase->GetPos(), &m_vObjectForce);
				i3Vector::Add( &m_ThrowData.m_vOldPos, &m_ThrowData.m_vOldPos, &m_vObjectForce);
				i3Vector::Add( &m_ThrowData.m_vStartPos, &m_ThrowData.m_vStartPos, &m_vObjectForce);

				return;
			}
		}

		i3Vector::Zero( &m_vObjectForce);
	}
}

void MainWeapon_Grenade::CheckBullet()
{
	INT32 total_count = m_pWeaponBase->getTotalBulletCount();
	INT32 load_count = m_pWeaponBase->getLoadedBulletCount();

	if (load_count > 0)
	{
		load_count -= m_pWeaponBase->getWeaponInfo()->GetFireDecBulletCount();
	}
	else if (total_count > 0)
	{
		m_pWeaponBase->setLoadedBulletCount(m_pWeaponBase->getWeaponInfo()->GetIncreaseCount());
		m_pWeaponBase->setTotalBulletCount(MIN(0, total_count - m_pWeaponBase->getWeaponInfo()->GetFireDecBulletCount()));
	}
}

void MainWeapon_Grenade::UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange)
{
	if( m_ThrowData.m_rSpeed < 0.15f)
	{
		_ProcessTakeObject();
		return;
	}

	if( rDeltaSeconds > 0.5f)
		rDeltaSeconds = 0.016f;

	if( m_pWeaponBase->IsDedicatedHost() )
	{
		this->CheckWorldCollision( rDeltaSeconds);
	}
	else
	{	// ��Ʈ��ũ ó���Դϴ�.
		this->UpdateNetworkData( rDeltaSeconds);
	}

#if !defined( NO_GRENADE_TRACE_EFFECT)
	//	����ź ������ ����ϴ� ��쿡 ����ź ����(����)�� �����.
	if( isUseTraceEffect() && m_pWeaponBase->isVisible() &&
		false == i3Vector::IsSame( m_pWeaponBase->GetPos(), &m_ThrowData.m_vTracerOldPos ) )
	{
		if( m_rElapsedExplosionTime < rExplosionRange - 0.1f && //	�����ϱ� ������ ������ ���ʿ��ϹǷ� ���´�.
			m_ThrowData.m_rElapsedThrowTime > 0.1f &&
			m_ThrowData.m_rTracerUpdateTime <= 0.f )
		{
			UpdateTraceEffect(m_ThrowData.m_rElapsedThrowTime);
		}
	}

	m_ThrowData.m_rTracerUpdateTime -= rDeltaSeconds;

#endif
	
	m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;	
}


void	MainWeapon_Grenade::CheckWorldCollision( REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult;

	INT32	nBoundType = 0;

	VEC3D	vNew;
	i3Vector::Scale( &vNew, &m_ThrowData.m_vDirection , m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime);
	i3Vector::Add( m_pWeaponBase->GetPos(), &m_ThrowData.m_vStartPos , &vNew);
	addY( m_pWeaponBase->GetPos(), - 5.0f * m_ThrowData.m_rElapsedThrowTime  * m_ThrowData.m_rElapsedThrowTime );
	i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), - I3_DEG2RAD( rDeltaSeconds * 360.f));

	ColLine.SetStart(&m_ThrowData.m_vOldPos );
	i3Vector::Sub( &vTemp,  m_pWeaponBase->GetPos(),&m_ThrowData.m_vOldPos );
	rLength = i3Vector::Normalize( &vTemp, &vTemp);

	if( rLength <= 0.0f )
	{
		_ProcessBoundObjectForce();
		return;
	}

	ColLine.SetEnd( m_pWeaponBase->GetPos());
	ColLine.SetDir( &vTemp);

	// ù��° raycast�� Chara���� Rough Collision�� �˻��մϴ�.
	pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_GRENADE );
	if( pResult != nullptr)
	{
		i3EventReceiver * pRecv = pResult->m_pDestShapeSet->getEventReceiver();

		g_pFramework->reserveRaycastResult( 1);

		if( i3::kind_of<CGameCharaBase*>(pRecv))
		{
			UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
			CGameCharaBase * pTarget = (CGameCharaBase *)pRecv;

			if( pTarget == m_pWeaponBase->getOwner() ) //m_pOwner )
			{
				pResult = nullptr;
			}
			else
			{
				if( pTarget->getCharaTeam() & CHARACTER_TEAM_RED)
				{
					nColGroup = CGRP_MASKGROUP_CHARA_RED;
				}

				pResult = g_pFramework->raycastClosest( &ColLine, nColGroup );
				if( pResult != nullptr)
				{
					pRecv = pResult->m_pDestShapeSet->getEventReceiver();
					if( i3::kind_of<CGameCharaBase*>(pRecv))
					{
						pTarget = (CGameCharaBase *) pRecv;
						if( pTarget->isCharaStateMask( CHARA_STATE_DEATH))
							pResult = nullptr;
					}
				}
			}
		}
	}

	if( pResult == nullptr)
	{
		// Rough Hit Collision�� Chara Hit����� ������ World�� Object�� ���Ͽ� �׽�Ʈ�� �մϴ�.
		pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK );
		if( pResult == nullptr)
			return;

		g_pFramework->reserveRaycastResult( 1);
	}

	{
		VEC3D	vecNew, vecNewPos, vecNormal;
		REAL32	rDot1 = 0.0f;

		//�̵� ���� ���� �� �̵��Ÿ�
		i3Vector::Sub( &vecNew, &pResult->m_Intersect, &m_ThrowData.m_vOldPos );
		REAL32 rTestLen = i3Vector::Normalize( &vecNew, &vecNew);

		// �����̴� ������Ʈ��� ���� ����ؼ� �ݻ緮�� ��������մϴ�. by dfly79 080117
		m_rBoundObjectLen = _BoundObject( pResult, ColLine.GetDir());

		if( rLength + 0.05f < rTestLen + m_rBoundObjectLen) //rTestLen * rTestLen)
		{	
			//��ȿ�� �Ÿ����� �浹�� ����..
			_ProcessBoundObjectForce();
			return;
		}

		// �� ��ŭ �ڷ� ������.
		// Step1. ����ź�� ��ġ�� �浹�� �Ͼ ��ġ�� �̵�( ����ź�� Radius�� ����)
		VEC3D vecRevert;
		i3Vector::Scale( &vecRevert, &vecNew, - 0.05f);

		if( getX( &m_vObjectForce) != 0.f || getY( &m_vObjectForce) != 0.f || getZ( &m_vObjectForce) != 0.f)
		{
			VEC3D vAddForce;
			i3Vector::Scale( &vAddForce, &m_vObjectForce, 3.f);
			i3Vector::Add( &vecRevert, &vecRevert, &vAddForce);
		}

		VEC3D vGrenadeSize;
		i3Vector::Scale( &vGrenadeSize, &pResult->m_Normal, 0.05f);
		
		i3Vector::Add( &vecRevert, &vecRevert, &vGrenadeSize);
		i3Vector::Add( m_pWeaponBase->GetPos(), &pResult->m_Intersect, &vecRevert);	//	���� �ݴ� �������� ������.
		i3Vector::Add( &m_ThrowData.m_vOldPos , &m_ThrowData.m_vOldPos, &vecRevert);

		i3Vector::Add( &vecNewPos, &pResult->m_Intersect, &vecNew);	//NewPosition
		rDot1 = i3Vector::Dot( &vecNew, &pResult->m_Normal);

		i3Vector::Scale( &vecNormal, &pResult->m_Normal, - 2.0f * rDot1);
		i3Vector::Add( &vecNewPos, &vecNewPos, &vecNormal);

		i3Vector::Sub( &vecNew, &vecNewPos, &pResult->m_Intersect);
		i3Vector::Normalize( &vecNew, &vecNew);

		//�ݻ� �ӵ� ���
		REAL32	Speed = m_ThrowData.m_rSpeed;

		rDot1 = i3Vector::Dot( &vecNew, &m_ThrowData.m_vDirection ); 
		if( i3Vector::Dot( &I3_YAXIS, &pResult->m_Normal) > 0.5f)
		{
			Speed = ((rDot1 + 1.0f) * Speed * 0.1f) + (Speed * 0.165f);
		}
		else
		{
			Speed = ((rDot1 + 1.0f) * Speed * 0.11f) + (Speed * 0.165f);
		}

		if( pResult->m_pTerrain != nullptr )
			nBoundType = pResult->m_pTerrain->m_Terrain;

		//�� ��ô (��Ʈ��ũ ó���˴ϴ�. by ������ 2006. 10. 16 )
		NET_Bound( m_pWeaponBase->GetPos(), &vecNew, Speed, nBoundType );
	}

	g_pFramework->resetRaycastResult();	
	
	
}



void MainWeapon_Grenade::Explosion()
{
	//BOOM
	m_pWeaponBase->_OnCheckCollisionByExplosion();
	
	{
		i3Object * pObj = nullptr;
		i3Transform * pTrans = nullptr;
		I3_PHYSIX_HIT_RESULT * pResult;
		i3CollideeLine line;
		VEC3D vDir;

		i3Vector::Set( &vDir, 0.f, -1.f, 0.f);
		line.SetStart( m_pWeaponBase->GetPos());
		line.SetDir( &vDir);
		pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
		if( pResult != nullptr)
		{
			UINT16 shapeGroup = pResult->m_pDestShapeSet->getShapeGroup();
				
			if( OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup ) )
			{
				i3EventReceiver * pReceiver = pResult->m_pDestShapeSet->getEventReceiver();
			
				I3ASSERT( i3::kind_of<i3Object*>(pReceiver));

				pObj = (i3Object*) pReceiver;
				pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObj->getSceneObject() );
			}
		}

		INT32 nFireIndex = 0;	//	default
		if( m_nFireEffectCount > 1)
		{
			// ������ ����Ʈ�� �ִ� ��� �������� ���� �÷����Ѵ�.
			nFireIndex = (i3Math::Rand() % 3);
			if( nFireIndex < 0)						nFireIndex = 0;
			if( nFireIndex >= m_nFireEffectCount)	nFireIndex = m_nFireEffectCount-1;
		}

		// ���� ����Ʈ
		if( pTrans != nullptr)
			g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex( nFireIndex), m_pWeaponBase->GetPos(), &I3_YAXIS, pTrans->GetCacheMatrix(), 0.f, pObj);
		else
			g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex( nFireIndex), m_pWeaponBase->GetPos(), &I3_YAXIS);
		//I3TRACE("WeaponGrenade::Explosion \n "); 

		g_pWeaponManager->PushThrowWeapon(m_pWeaponBase);
	}

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		if( (g_pFramework->IsBattleStage() != false) && (gmode::i()->GetTutorialMode() != nullptr) )
		{
			VEC3D v3Pos;

			i3Vector::Set(&v3Pos,	m_ThrowData.m_vOldPos.x, 
									m_ThrowData.m_vOldPos.y,
									m_ThrowData.m_vOldPos.z);
							
			gmode::i()->GetTutorialMode()->CheckGrenadeInTutorial(v3Pos, m_pWeaponBase->getWeaponInfo()->GetNumber());
		}
	}

}

bool MainWeapon_Grenade::SuccessiveStateFlagCheck(INT32 nState)
{
	if (false == m_vGrenadeStateFlag.empty())
	{
		UINT32 nSize = m_vGrenadeStateFlag.size();

		//����, ���� ���� ���Ŀ� ������ ��Ȳ�� ���ü� ����.
		if (nState == NET_GRENADE_STATE_FLY || nState == NET_GRENADE_STATE_BOUND)
		{
			//�ٷξ� ���¸� ã�´�.
			UINT8 usPrevState = (UINT8)m_vGrenadeStateFlag[nSize - 1];

			switch( usPrevState )
			{
			case NET_GRENADE_STATE_EXPLOSION	:
			case NET_GRENADE_STATE_SETTLED		:
			case NET_GRENADE_STATE_CLICKFIRE	:
				{
					//��Ŷ�� ������ �ʴ´�.
					return false;
				}
			}
		}
	}

	m_vGrenadeStateFlag.push_back(nState);
	return true;
}

void	MainWeapon_Grenade::OnUpdate_Grenade( REAL32 rDeltaSeconds)
{
	if( isThrowing() )
	{	// ���忡 ������ ����ź�Դϴ�.
		// ������ �ִ��� üũ�ؼ� ������ ��ȯ�����ݴϴ�.
		if( _CheckProcessUserLeave() == false)
			return;

		m_nStateFlag = NET_GRENADE_STATE_FLY;

		bool	bHost = m_pWeaponBase->IsDedicatedHost();

		if( bHost )
		{
			if( m_ThrowData.m_rSpeed < 0.2f && !isBombState( WEAPON_BOMBSTATE_SETTLED) )
			{
				addBombState( WEAPON_BOMBSTATE_SETTLED);
				m_nStateFlag = NET_GRENADE_STATE_SETTLED;
			}
		}

		if( isSettled())
			m_rElapsedExplosionTime += rDeltaSeconds;

		REAL32 rExplosionRange = m_pWeaponBase->GetExplosionTime();

		this->UpdateFlying( rDeltaSeconds, rExplosionRange );

		m_pWeaponBase->_TransformWorldSpace( rDeltaSeconds);

		i3Vector::Copy(&m_ThrowData.m_vOldPos , m_pWeaponBase->GetPos());

		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			if( m_rElapsedExplosionTime > rExplosionRange)
			{
				m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
				m_pWeaponBase->setSkillOnce_Jump(false);
				m_pWeaponBase->setSkillOnce_Sit(false);
			}

			if( m_ThrowData.m_rElapsedThrowTime > NET_GRENADE_OFFTIME)
			{// ��Ʈ���� �޴� ��� 15�ʱ��� ��Ŷ�� �� ������ �׳� �����ݴϴ�.
				if( bHost )
				{
					m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
				}
				else
				{
					GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx());
					pInfo->Reset();

					m_pWeaponBase->_ResetGrenadeInfo();
					g_pWeaponManager->PushThrowWeapon(m_pWeaponBase); 
				}
			}

			if( bHost )
			{
				// ��Ʈ��ũ�� �����ݴϴ�.
				I3ASSERT( m_nStateFlag < 256);
				GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), &m_ThrowData.m_vOldPos, (UINT8)m_nStateFlag);
				
				if (SuccessiveStateFlagCheck(m_nStateFlag))
				{
					GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo );
				}
			}

			if( m_nStateFlag == NET_GRENADE_STATE_EXPLOSION  && !isBombState( WEAPON_BOMBSTATE_EXPLOSION) )
				this->Explosion();
		}
	}

}

void	MainWeapon_Grenade::SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo)
{
	
	switch(pNetInfo->GetState())
	{
	case NET_GRENADE_STATE_FLY:
		{
			m_nStateFlag = NET_GRENADE_STATE_FLY;
			m_pWeaponBase->SetVisible( true );
		}
		break;
	case NET_GRENADE_STATE_BOUND:
		{
			if( pNetInfo->GetBoundType() < I3_TERRAIN_TYPE_COUNT )
			{
				_PlayBoundSound( (I3_TERRAIN_TYPE) pNetInfo->GetBoundType() );
			}
			// �ٿ�� �� �ö��� ���� �ٲ���� �մϴ�.
			pNetInfo->SetState( NET_GRENADE_STATE_FLY );
		}
		break;
	case NET_GRENADE_STATE_C5_SETTLED:
		break;
	case NET_GRENADE_STATE_SLEEP:	
		break;
	case NET_GRENADE_STATE_SETTLED:
		{
			addBombState( WEAPON_BOMBSTATE_SETTLED);

			// Ŭ���̾�Ʈ�� ������Ʈ ���� �ִ� ��츦 ����մϴ�.
			_ProcessTakeObject();
		}
		break;
	case NET_GRENADE_STATE_TAKEOBJECT:
		break;
	case NET_GRENADE_STATE_CLICKFIRE:
		break;
	case NET_GRENADE_STATE_EXPLOSION:
		{
			VEC3D vPos = pNetInfo->GetPos();
			m_pWeaponBase->SetPos( &vPos);
			if( isBombState( WEAPON_BOMBSTATE_EXPLOSION) == false)
				Explosion();
		}
		break;
	}

	pGrenadeInfo->_bRecv = false;
}

void	MainWeapon_Grenade::NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
	bool bHost = m_pWeaponBase->IsDedicatedHost();

	if( bHost && m_pWeaponBase->getNetworkIdx() != -1)
	{
		// ��Ʈ��ũ�� �����ݴϴ�.
		GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), pStartPos, NET_GRENADE_STATE_BOUND);

		throwInfo._nBoundType			= nBoundType;

		if (SuccessiveStateFlagCheck(throwInfo._nAddStateFlag))
			GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo);
	}

	if( fSpeed > 0.5f)	//	���� �ٿ��� ������ �ȳ��´�.
	{
#if !defined( NO_GRENADE_TRACE_EFFECT)

		m_ThrowData.m_rTracerUpdateTime = 0.0f;
#endif
		_PlayBoundSound( (I3_TERRAIN_TYPE) nBoundType );
	}
	else
	{
#if !defined( NO_GRENADE_TRACE_EFFECT)
		m_ThrowData.m_rTracerUpdateTime = 0.2f;
#endif
	}

	Throw( pStartPos, pDir, fSpeed , false, -1 );
	
	
}


void MainWeapon_Grenade::UpdateNetworkData( REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vTemp;

	REAL32	rRotate = 360.0f;

	I3ASSERT( m_pWeaponBase->IsDedicatedHost() == false);

	if( m_pWeaponBase->getNetworkIdx() < 0 || m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)		return;

	GAMEINFO_THROW_WEAPON * pGrenadeInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx());
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pGrenadeInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pGrenadeInfo->_tNetData;

	VEC3D vNetPos = pNetInfo->GetPos();

	pAssistInfo->_fInterpolateTime += rDeltatime;
	flerp	= pAssistInfo->_fInterpolateTime;

	if( flerp > GRENADE_POS_ASSISTTIME )
	{	
		if( (pNetInfo->GetState() & 0x0F) == NET_GRENADE_STATE_FLY )
		{
			VEC3D vDir;

			i3Vector::Sub( &vDir, &vNetPos, &pAssistInfo->_vPostPos);
			i3Vector::Copy( &pAssistInfo->_vPostPos, m_pWeaponBase->GetPos() );
			
			i3Vector::Add( &vTemp, &pAssistInfo->_vPostPos, &vDir );
			pNetInfo->SetPos( vTemp );
			vNetPos = vTemp;
			
			flerp = rDeltatime;
		}
		else
			flerp = GRENADE_POS_ASSISTTIME;
	}

	flerp *= GRENADE_POS_ASSISTTIMEINV;

	i3Vector::Lerp( m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vNetPos, flerp );
	i3Vector::Sub( &vTemp, &pAssistInfo->_vPostPos, &vNetPos );

	if (i3Vector::LengthSq(&vTemp) > 0.0001f)
	{
		if (flerp >= 1.0f)
		{
			//pAssistInfo�� ���� �������� �־�, ���ŵ��� ���� ������ ���� vTemp���� ���� ��� ȸ���ϴ°��� ����
			i3Vector::Copy(&pAssistInfo->_vPostPos, m_pWeaponBase->GetPos());
		}

		i3Matrix::PreRotateX(m_pWeaponBase->GetMatrix(), -I3_DEG2RAD(rDeltatime * rRotate));
	}
	
	if( pGrenadeInfo->_bRecv )
	{
		SetReceiveStates(pGrenadeInfo, pNetInfo);		// �� �Լ��� ������ ȣ����ġ��..
	}
}


bool MainWeapon_Grenade::_CheckProcessUserLeave( void)
{	
	bool	bOwnerInvalid = true;

	// ������ ������� üũ	
	for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);

		if( pChara == nullptr)
			continue;
		if( g_pCharaManager->getChara( i) == m_pWeaponBase->getOwner() )
		{
			bOwnerInvalid = false;
			break;
		}
	}

	if( bOwnerInvalid )
	{
		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
			pInfo->Reset();
		}

		g_pWeaponManager->PushThrowWeapon( m_pWeaponBase);
		return false;
	}

	return true;
}

void MainWeapon_Grenade::_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType)
{
	I3ASSERT( g_pViewer != nullptr);
	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nTerrainType, GTBT_BOUND_THROW, m_pWeaponBase->GetPos(), false);
}

/*virtual*/ WeaponGrenade * MainWeapon_Grenade::_CreateThrowWepaon(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx)
{
	I3ASSERT( m_pWeaponBase->getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_THROWING);

	WeaponGrenade * pNewWeapon = (WeaponGrenade*) g_pWeaponManager->GetFreeThrowWeapon(
		m_pWeaponBase->getWeaponInfo()->GetTypeClass(), m_pWeaponBase->getWeaponInfo()->GetNumber() );
	if( pNewWeapon == nullptr )
	{
		// �ν��Ͻ��� �����ϴ�..?
		//I3PRINTLOG(I3LOG_FATAL,  "no instance resource." );
		return nullptr;
	}
	// Reset �ϸ� Bullet�� ������ �ȵȴ�.
	m_bResetBullet = false;

	pNewWeapon->SetOwner(  m_pWeaponBase->getOwner());	// ���� �����ؾߵ˴ϴ�.	
	pNewWeapon->Reset();
	pNewWeapon->Throw( vStart, vDir, rSpeed, false, nNetSlotIdx);

	// Network ID
	pNewWeapon->setNetworkIdx( (INT16)nNetSlotIdx);
	if( nNetSlotIdx == -1)
	{
		const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara(  m_pWeaponBase->getOwner()->getCharaNetIndex());
		pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());
	}

	VEC3D	vTemp;
	m_pWeaponBase->getOwner()->getCameraContext()->getVirtualCamPosition( &vTemp );
	GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, &vTemp );

	if( m_pWeaponBase->IsDedicatedHost() == false )
	{	
		pNewWeapon->SetPos( &vTemp);			
		pNewWeapon->SetVisible( false );
	}

	if( m_pWeaponBase->getOwner() != nullptr && m_pWeaponBase->getOwner()->isLocal() )
	{
		// Radio Sound ��� �ϴ� ��쿡�� ó��.
		if( isPlayRadioSound() )  
		{
			if( BattleServerContext::i()->IsP2PHost())
				GameContextUtil::PlayRadioMessageSound( RADIOCHAT_TYPE_AUTO, 1, m_pWeaponBase->getOwner());

			GAMEEVENT::Write_Chara_RadioChat( m_pWeaponBase->getOwner(), 31);
		}

		// ��ŷ ���� count
		// KEYWORD : Hack Protect
		GrenadeHackDetection(m_uCount);
	}

	if ( ! i3::same_of<WeaponDualGrenade*>(m_pWeaponBase))
		m_pWeaponBase->SetVisible( false); // �ڱ� �տ� �ִ� ����ź 

	return pNewWeapon;
}

/*virtual*/ void MainWeapon_Grenade::_SetThrowData(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed)
{
	addBombState( WEAPON_BOMBSTATE_THROWING);

	m_ThrowData.m_rElapsedThrowTime	= 0.0f;

#if !defined( NO_GRENADE_TRACE_EFFECT)
	m_ThrowData.m_rTracerUpdateTime	= 0.0f;
#endif

	m_ThrowData.m_rSpeed = rSpeed;

	i3Vector::Copy( &m_ThrowData.m_vStartPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vDirection, vDir);

	i3Vector::Copy( &m_ThrowData.m_vOldPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vTracerOldPos, vStart);

	m_pWeaponBase->SetHitCollideeGroup();
}

/*virtual*/ void MainWeapon_Grenade::_TakeObject(void)
{
	I3_PHYSIX_HIT_RESULT * pResult;
	i3CollideeLine line;
	VEC3D vDir;
	VEC3D vPos;

	m_bCheckFloor = true;

	i3Vector::Set( &vDir, 0.f, -1.f, 0.f);
	i3Vector::Copy( &vPos, m_pWeaponBase->GetPos());
	i3Vector::AddY( &vPos, 0.05f);
	line.SetStart( &vPos);
	line.SetDir( &vDir);
	pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
	if( pResult == nullptr)
		return;

	UINT16 shapeGroup = pResult->m_pDestShapeSet->getShapeGroup();

	if( OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup ) )
	{
		i3EventReceiver * pReceiver = pResult->m_pDestShapeSet->getEventReceiver();
		I3ASSERT( i3::kind_of<i3Object*>(pReceiver));

		i3Object * pObj = (i3Object*) pReceiver;
		i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObj->getSceneObject() );

		if( pTrans != nullptr )
		{
			// �ִϸ��̼� �ϴ� ������Ʈ�Դϴ�.
			if( pTrans->IsAnimated() )
				m_pAttachObjectTransform = pTrans;

			m_pAttachObject = pObj;
		}
	}
}

/*virtual*/ void MainWeapon_Grenade::_UpdateTakeObjectTransform(void)
{
	// �ִϸ��̼� �Ǵ� Transform�Դϴ�. Matrix�� ���� �����ݴϴ�.
	if( m_pAttachObject && m_pAttachObjectTransform)
	{	
		for(INT32 i = 0 ; i < m_pAttachObject->getStateCount(); i++)
		{
			if( m_pAttachObject->getHP(i) == 0)
				return;
		}

		MATRIX	matTop;
		VEC3D	vNewPosDir;

		i3SceneTracer * pTracer;

		pTracer = g_pFramework->GetViewer()->GetSceneTracer();

		I3ASSERT(pTracer);

		pTracer->GetModelViewMatrixStack()->Reset();
		pTracer->SetTime( g_pViewer->getDeltaTime());
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

		m_pAttachObjectTransform->CalcUpdatedMatrix( &matTop, pTracer);

		// �ִϸ��̼ǵ� ��ǥ���� ���� �������� ��ǥ�� ���� �����ǿ� �����ݴϴ�.
		i3Vector::Sub( &vNewPosDir, i3Matrix::GetPos( &matTop ), i3Matrix::GetPos( m_pAttachObjectTransform->GetCacheMatrix() ) );

		i3Vector::Add( &vNewPosDir, &vNewPosDir, m_pWeaponBase->GetPos());
		m_pWeaponBase->SetPos( &vNewPosDir);
	}
}

void	MainWeapon_Grenade::Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin, INT32 nNetSlotIdx)
{
	////////////////////////////////////////////////////////////////////////////////////////
	//	������ ���� (ȭ�鿡 ���󰡴� ��ü�� �ƴϴ�. ĳ���Ͱ� ���� ��� ���� ��� �ν��Ͻ��� �����ؼ� ���忡 �����ϴ�.)
	if( bOrigin)
	{
		_CreateThrowWepaon(vStart, vDir, rSpeed, nNetSlotIdx);
	}
	////////////////////////////////////////////////////////////////////////////////////////
	//	�������� ��ü (�ν��Ͻ��� �����Ǿ� ���ư��� �༮�̴�.)
	else
	{
		_SetThrowData(vStart, vDir, rSpeed);
	}
}


bool MainWeapon_Grenade::_CheckThrow( REAL32 LocalTime )
{
	if( (BattleServerContext::i()->IsP2PHost() && LocalTime > m_pWeaponBase->getWeaponInfo()->GetAttackTimeA(0) ) ||
		(!BattleServerContext::i()->IsP2PHost() && LocalTime > (m_pWeaponBase->getWeaponInfo()->GetAttackTimeA(0) - 0.05f) ) )
	{
		if( !m_pWeaponBase->getOwner()->isNetwork() )// ��Ʈ��ũ�� ��Ŷ���� ó���մϴ�.
		{
			//�տ��� �������� �ð�
			// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.
			VEC3D target;
			m_pWeaponBase->GetShotTargetPos( &target);
			m_pWeaponBase->NET_Throw( nullptr, &target, -1 );
		}

		return true;
	}

	return false;
}
