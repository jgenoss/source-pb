#include "pch.h"
#include "MainWeapon_Smoke.h"
#include "MyRoomInfoContext.h"
#include "GameMode.h"
#include "TutorialMode.h"
#include "BattleObjContext.h"


MainWeapon_Smoke::MainWeapon_Smoke(WeaponBase* p) : MainWeapon_Grenade(p), m_pSmokeStamp(nullptr)
{
}

MainWeapon_Smoke::~MainWeapon_Smoke() {}

void	MainWeapon_Smoke::Reset()
{
	MainWeapon_Grenade::Reset();

	m_vReceiveStateFlag.clear();
}


void	MainWeapon_Smoke::Explosion()
{
	//WeaponGrenade::Explosion();
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
			m_pSmokeStamp = g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex( nFireIndex), m_pWeaponBase->GetPos(), &I3_YAXIS, pTrans->GetCacheMatrix(), 0.f, pObj);
		else
			m_pSmokeStamp = g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex( nFireIndex), m_pWeaponBase->GetPos(), &I3_YAXIS);

		//ȣ��Ʈ�� ��츸 ���⼭ ����
		//g_pWeaponManager->PushThrowWeapon(this);
	}

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		if(gmode::i()->GetTutorialMode() != nullptr)
		{
			VEC3D v3Pos;

			i3Vector::Set(&v3Pos,	m_ThrowData.m_vOldPos.x, 
									m_ThrowData.m_vOldPos.y, 
									m_ThrowData.m_vOldPos.z);
			gmode::i()->GetTutorialMode()->CheckGrenadeInTutorial(v3Pos, m_pWeaponBase->getWeaponInfo()->GetNumber());
		}
	}
	addBombState(WEAPON_BOMBSTATE_EXPLOSION);

}

void	MainWeapon_Smoke::OnUpdate_Grenade(REAL32 rDeltaSeconds)
{
	if( isThrowing())
	{	
		// ���忡 ������ ����ź�Դϴ�.
		// ������ �ִ��� üũ�ؼ� ������ ��ȯ�����ݴϴ�.
		if( _CheckProcessUserLeave() == false)
			return;

		m_nStateFlag = NET_GRENADE_STATE_FLY;

		bool	bHost = m_pWeaponBase->IsDedicatedHost();

		if( bHost )
		{
			if( m_ThrowData.m_rSpeed < 0.2f && 
				!isBombState( WEAPON_BOMBSTATE_SETTLED) && 
				!isBombState( WEAPON_BOMBSTATE_EXPLOSION))
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

		i3Vector::Copy( &m_ThrowData.m_vOldPos , m_pWeaponBase->GetPos());

		if( m_pWeaponBase->getNetworkIdx() == -1)
			return;

		if( m_rElapsedExplosionTime > rExplosionRange)
		{
			m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
		}

		if( m_ThrowData.m_rElapsedThrowTime >  15.0f)
		{
			//��ƼŬ�� �����մϴ�~
			this->StopSmokeParticle(true);
			m_nStateFlag = 0;

			// ��Ʈ���� �޴� ��� 15�ʱ��� ��Ŷ�� �� ������ �׳� �����ݴϴ�.
			GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
			pInfo->Reset();

			if( !bHost )
			{
				m_pWeaponBase->_ResetGrenadeInfo();
			}

			g_pWeaponManager->PushThrowWeapon(m_pWeaponBase);
		}

		if( bHost )
		{
			// ��Ʈ��ũ�� �����ݴϴ�.
			I3ASSERT( m_nStateFlag < 256);
			GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(),&m_ThrowData.m_vOldPos,(UINT8)m_nStateFlag);
			
			if (SuccessiveStateFlagCheck(m_nStateFlag))
			{
				//���� �Ȱ�� �ٽ� settled�� ���¸� �ٲ��ݴϴ�.
				if (isBombState(WEAPON_BOMBSTATE_EXPLOSION))
				{
					m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;

					m_nStateFlag = NET_GRENADE_STATE_SETTLED;

					GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo2(m_pWeaponBase->getWeaponInfo()->GetItemID(), m_pWeaponBase->GetPos(), (UINT8) m_nStateFlag);
					
					throwInfo2._ucRemainingTime	= (UINT8) (m_ThrowData.m_rElapsedThrowTime  * 10.0f);
					
					if (BattleObjContext::i()->IsInterEnterSmokeSend())
					{
						I3TRACE("Inter Enter Smoke Send false CharaSlot Idx%d\n",  m_pWeaponBase->getNetworkIdx() );
						BattleObjContext::i()->SetInterEnterSmokeSend(false);
						GAMEEVENT::Write_ThrowingWeapon(  m_pWeaponBase->getNetworkIdx(), &throwInfo2 );
					}
				}
				else
				{
					GAMEEVENT::Write_ThrowingWeapon(  m_pWeaponBase->getNetworkIdx(), &throwInfo );
				}
			}

			if( throwInfo._nAddStateFlag == NET_GRENADE_STATE_EXPLOSION &&
				!isBombState( WEAPON_BOMBSTATE_EXPLOSION) )
			{
				Explosion();
			}

		}
	}


}


void	MainWeapon_Smoke::SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo)
{
	VEC3D vNetPos = pNetInfo->GetPos();

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
			if (m_vReceiveStateFlag.empty())
			{
				//addBombState( WEAPON_BOMBSTATE_SETTLED);
				m_pWeaponBase->SetPos( &vNetPos);
				Explosion();

				m_vReceiveStateFlag.push_back(pNetInfo->GetState());
			}

			addBombState( WEAPON_BOMBSTATE_SETTLED);
			// Ŭ���̾�Ʈ�� ������Ʈ ���� �ִ� ��츦 ����մϴ�.
			_ProcessTakeObject();

			m_ThrowData.m_rElapsedThrowTime = pGrenadeInfo->_tNetData.getRemainingTime() / 10.0f;

			I3TRACE("NET_GRENADE_STATE_SETTLED �ð�%d\n", pGrenadeInfo->_tNetData.getRemainingTime());

		}
		break;
	case NET_GRENADE_STATE_TAKEOBJECT:
		break;
	case NET_GRENADE_STATE_CLICKFIRE:
		break;
	case NET_GRENADE_STATE_EXPLOSION:
		{
			m_pWeaponBase->SetPos( &vNetPos);
			if( isBombState( WEAPON_BOMBSTATE_EXPLOSION) == false)
				Explosion();
		}
		break;
	}

	pGrenadeInfo->_bRecv = false;

}


void MainWeapon_Smoke::StopSmokeParticle(bool bStop)
{

	if (m_pSmokeStamp && bStop)
	{
		i3Node* pNode = m_pSmokeStamp->GetChild(0);
		if (i3::kind_of<i3TimeSequence*>(pNode))
		{
			for (int nParticle = 0; nParticle < pNode->GetChildCount(); nParticle++)
			{
				i3Node* pChildNode = pNode->GetChild(nParticle);
				if (i3::kind_of<i3Particle*>(pChildNode))
				{
					i3Particle* pParticleNode = (i3Particle*)pChildNode;
					if (pParticleNode)
						pParticleNode->SetStop(true);
				}
			}
		}

		m_pSmokeStamp = nullptr;
	}
}
