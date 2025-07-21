#include "pch.h"
#include "MainWeapon_Claymore.h"

#include "BattleObjContext.h"
#include "BattleServerContext.h"
#include "GameCharaBoneContext.h"

#define CLAYMORE_POS_ASSISTTIME		0.2f
#define CLAYMORE_POS_ASSISTTIMEINV	5.0f

#define CLAYMORE_ROTATE_X			-0.75f

MainWeapon_Claymore::MainWeapon_Claymore(WeaponBase* p) : MainWeapon_Grenade(p),
m_rFloorCheckTime(0.0f),
m_rExplosionDelay(0.0f)
{
}

/*virtual*/ WeaponGrenade * MainWeapon_Claymore::_CreateThrowWepaon(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx)
{
	WeaponGrenade * pGrenade = MainWeapon_Grenade::_CreateThrowWepaon(vStart, vDir, rSpeed, nNetSlotIdx);

	if( pGrenade != nullptr )
	{
		_SetClaymoreMatrix(pGrenade);

		WeaponClaymore * pClaymore = i3::same_cast<WeaponClaymore*>(pGrenade);
		if(pClaymore != nullptr)
			pClaymore->SetWeaponLookDir(&m_vLookAt);
	}

	return pGrenade;
}

/*virtual*/ void MainWeapon_Claymore::_TakeObject(void)
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

		m_pAttachObject = (i3Object*) pReceiver;
		i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( m_pAttachObject->getSceneObject() );

		MATRIX mtxInv;

		if( pTrans != nullptr && pTrans->IsAnimated() )
		{
			// �ִϸ��̼� �ϴ� ������Ʈ�Դϴ�.
			m_pAttachObjectTransform = pTrans;

			i3SceneTracer * pTracer;
			MATRIX matTop;

			pTracer = g_pFramework->GetViewer()->GetSceneTracer();

			I3ASSERT(pTracer);

			pTracer->GetModelViewMatrixStack()->Reset();
			pTracer->SetTime( g_pViewer->getDeltaTime());
			pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

			m_pAttachObjectTransform->CalcUpdatedMatrix( &matTop, pTracer);

			i3Matrix::Inverse( &mtxInv, 0, m_pAttachObjectTransform->GetCacheMatrix() );
			i3Matrix::Mul( &m_AttachLocalMatrix, m_pWeaponBase->GetMatrix(), &mtxInv);
		}
		else
		{
			// �ִϸ��̼� ��ũ�� ������ �ʴ´ٸ� ������Ʈ�� ������ǥ�� �ٿ��ݴϴ�.
			i3Matrix::Inverse( &mtxInv, 0, m_pAttachObject->GetMatrix());
			i3Matrix::Mul( &m_AttachLocalMatrix, m_pWeaponBase->GetMatrix(), &mtxInv);
		}
	}
}

/*virtual*/ void MainWeapon_Claymore::_UpdateTakeObjectTransform(void)
{
	if( isBombState( WEAPON_BOMBSTATE_SETTLED ) == false)
		return;

	MATRIX matTop;
	VEC3D vDir;

	if( m_pAttachObjectTransform != nullptr)
	{
		// �ִϸ��̼� �Ǵ� ������Ʈ
		i3SceneTracer * pTracer = g_pFramework->GetViewer()->GetSceneTracer();
		I3ASSERT(pTracer);

		pTracer->GetModelViewMatrixStack()->Reset();
		pTracer->SetTime( g_pViewer->getDeltaTime());
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

		m_pAttachObjectTransform->CalcUpdatedMatrix( &matTop, pTracer);

		// �ִϸ��̼ǵ� ��ǥ���� ���� �������� ��ǥ�� ���� �����ǿ� �����ݴϴ�.
		i3Matrix::Mul( m_pWeaponBase->GetMatrix(), &m_AttachLocalMatrix, m_pAttachObjectTransform->GetCacheMatrix());


	}
	else if( m_pAttachObject != nullptr)
	{	
		// ���� ������Ʈ
		i3Matrix::Mul( m_pWeaponBase->GetMatrix(), &m_AttachLocalMatrix, m_pAttachObject->GetMatrix());
	}
	else
	{	
		if( m_pWeaponBase->IsDedicatedHost() &&	(isBombState( WEAPON_BOMBSTATE_FREEZE) == false) )
		{
			// �ٴڿ� �ִ� ���
			// �ٴ��� ����(������Ʈ �ı�)�� ������ ���������� �մϴ�.
			m_rFloorCheckTime += g_pViewer->getDeltaTime();
			if( m_rFloorCheckTime > 0.2f)
			{
				m_rFloorCheckTime = 0.f;
				i3Vector::Set( &vDir, 0.f, -1.f, 0.f);

				i3CollideeLine line;
				line.SetStart( m_pWeaponBase->GetPos());
				line.SetDir( &vDir);

				I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);

				if( pResult != nullptr)
				{
					if( pResult->m_pDestShapeSet->getShapeGroup() == CGRP_WORLD_ATTACK)
					{	
						addBombState( WEAPON_BOMBSTATE_FREEZE);
					}
					else if( pResult->m_T > 0.1f)
					{
						Throw( m_pWeaponBase->GetPos(), &vDir, 1.f, false, m_pWeaponBase->getNetworkIdx());
						removeBombState( WEAPON_BOMBSTATE_SETTLED);
					}
				}
			}
		}
	}

	if( isHost() && (m_pAttachObject != nullptr))
	{
		// ������Ʈ�� �پ� �ִ� ��� �ش� ������Ʈ�� �ı��ɽ� ũ����Ƹ� ��Ʈ���ϴ�.
		if( (m_pAttachObject->getStateCount() > 1) && (m_pAttachObject->getCurrentHP() == 0.f) )
		{
			m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
		}
	}
}

void MainWeapon_Claymore::_SetClaymoreMatrix(WeaponGrenade * pGrenade)
{
	I3ASSERT(pGrenade != nullptr);

	VEC3D vCross; 
	i3Vector::Cross(&vCross, &I3_YAXIS, &m_vLookAt);

	i3Matrix::SetUp( pGrenade->GetMatrix(), 0.f, 1.f, 0.f, 0.f);
	i3Matrix::SetAt( pGrenade->GetMatrix(), &m_vLookAt);
	i3Matrix::SetRight(pGrenade->GetMatrix(), &vCross);

	// �𵨸� ȸ�� (���� �߿�)
	{
		i3Matrix::PreRotateY( pGrenade->GetMatrix(), I3_PI );				// Y�� �������� 180�� ȸ��
		i3Matrix::PreRotateX( pGrenade->GetMatrix(), CLAYMORE_ROTATE_X );	// X�� �������� ȸ��
	}
}

void MainWeapon_Claymore::_CheckExplosionSensor(void)
{
	// Setted ���°� �ƴϸ� ����
	if( isSettled() == false)
		return;

	// �����ڰ� ������ ����
	CGameCharaBase * pOwner = m_pWeaponBase->getOwner();
	if( pOwner == nullptr )
		return;

	// ó�� ��ü(P2P - ����, Dedi - ��ô��)�� �ƴϸ� ����
	if( isHost() == false )
		return;

	REAL32 rRange = m_pWeaponBase->GetDetectedRange();

	for(INT32 idx = 0 ; idx < g_pCharaManager->getCharaCount(); ++idx)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara(idx);
		if( pChara == nullptr)
			continue;

		if( pChara->getCharaTeam() == pOwner->getCharaTeam() )
			continue;

		if( pChara->isCharaStateMask(CHARA_STATE_DEATH) )
			continue;

		bool bCheckSensor = false;

		if( m_pWeaponBase->IsValidExplosionArea(i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix() ), rRange, I3_PI4) )
		{
			bCheckSensor = true;
		}
		else if( m_pWeaponBase->IsValidExplosionArea(i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()), rRange, I3_PI4) )
		{
			bCheckSensor = true;
		}

		if( bCheckSensor )
		{
			m_rExplosionDelay = 0.0f;
			addBombState(WEAPON_BOMBSTATE_TIMESTART);
			m_nStateFlag = NET_GRENADE_STATE_CLICKFIRE;
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 0, m_pWeaponBase->GetPos(), false);
		}
	}
}

/*virtual*/ void MainWeapon_Claymore::OnInitVariable(void)
{
	MainWeapon_Grenade::OnInitVariable();
	
	i3Vector::Zero(&m_vLookAt);
	i3Matrix::Identity( &m_AttachLocalMatrix);

	m_rFloorCheckTime = 0.0f;
	m_rExplosionDelay = 0.0f;
}

/*virtual*/ void MainWeapon_Claymore::Reset(void)
{
	MainWeapon_Grenade::Reset();

	i3Vector::Zero(&m_vLookAt);
	i3Matrix::Identity( &m_AttachLocalMatrix);

	m_rFloorCheckTime = 0.0f;
	m_rExplosionDelay = 0.0f;
}

/*virtual*/ void MainWeapon_Claymore::OnUpdate_Grenade( REAL32 rDeltaSeconds)
{
	if( isThrowing() )
	{	
		// ���忡 ������ ����ź�Դϴ�.
		// ������ �ִ��� üũ�ؼ� ������ ��ȯ�����ݴϴ�.
		if( _CheckProcessUserLeave() == false)
			return;

		m_nStateFlag = NET_GRENADE_STATE_FLY;

		REAL32 rExplosionRange = m_pWeaponBase->GetExplosionRange();

		UpdateFlying( rDeltaSeconds, rExplosionRange );

		_UpdateTakeObjectTransform();

		m_pWeaponBase->_TransformWorldSpace( rDeltaSeconds);

		i3Vector::Copy(&m_ThrowData.m_vOldPos , m_pWeaponBase->GetPos());

		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			if( isSettled() && m_pWeaponBase->getOwner()->isCharaStateMask(CHARA_STATE_DEATH) )
			{
				GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx());
				pInfo->Reset();
				m_pWeaponBase->_ResetGrenadeInfo();
				g_pWeaponManager->PushThrowWeapon(m_pWeaponBase); 
			}

			if( m_pWeaponBase->IsDedicatedHost() )
			{
				if( isBombState(WEAPON_BOMBSTATE_TIMESTART) )
				{
					m_rExplosionDelay += rDeltaSeconds;

					if(m_rExplosionDelay >= m_pWeaponBase->GetExplosionTime() )
						m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
				}
				else
				{
					_CheckExplosionSensor();
				}

				// ��Ʈ��ũ�� �����ݴϴ�.
				I3ASSERT( m_nStateFlag < 256);
				GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), &m_ThrowData.m_vOldPos, (UINT8)m_nStateFlag);

				throwInfo._pNormal				= GetThrowInfo_Normal();

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

/*virtual*/ void MainWeapon_Claymore::CheckWorldCollision( REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;

	VEC3D	vNew;
	i3Vector::Scale( &vNew, &m_ThrowData.m_vDirection , m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime);
	i3Vector::Add( m_pWeaponBase->GetPos(), &m_ThrowData.m_vStartPos , &vNew);
	addY( m_pWeaponBase->GetPos(), - 5.0f * m_ThrowData.m_rElapsedThrowTime  * m_ThrowData.m_rElapsedThrowTime );

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

	// World�� Object�� ���Ͽ� �׽�Ʈ�� �մϴ�.
	I3_PHYSIX_HIT_RESULT *	pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK );
	if( pResult == nullptr)
		return;

	g_pFramework->reserveRaycastResult( 1);

	{
		VEC3D	vecNew;

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

		// Settied ����
		{
			m_ThrowData.m_rSpeed = 0.0f;

			addBombState( WEAPON_BOMBSTATE_SETTLED);
			m_nStateFlag = NET_GRENADE_STATE_SETTLED;
		}
	}

	g_pFramework->resetRaycastResult();	
}

/*virtual*/ void MainWeapon_Claymore::UpdateNetworkData( REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vTemp;

	I3ASSERT( m_pWeaponBase->IsDedicatedHost() == false);

	if( m_pWeaponBase->getNetworkIdx() < 0 || m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)		return;

	GAMEINFO_THROW_WEAPON * pGrenadeInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx());
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pGrenadeInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pGrenadeInfo->_tNetData;

	VEC3D vNetPos = pNetInfo->GetPos();

	pAssistInfo->_fInterpolateTime += rDeltatime;
	flerp	= pAssistInfo->_fInterpolateTime;

	if( flerp > CLAYMORE_POS_ASSISTTIME )
	{	
		if( pNetInfo->GetState() & NET_GRENADE_STATE_FLY )
		{
			VEC3D vDir;

			i3Vector::Sub( &vDir, &vNetPos, &pAssistInfo->_vPostPos);
			i3Vector::Copy( &pAssistInfo->_vPostPos, m_pWeaponBase->GetPos() );

			i3Vector::Add( &vTemp, &pAssistInfo->_vPostPos, &vDir );
			pNetInfo->SetPos(vTemp);

			flerp = rDeltatime;
		}
		else
			flerp = CLAYMORE_POS_ASSISTTIME;
	}

	flerp *= CLAYMORE_POS_ASSISTTIMEINV;

	i3Vector::Lerp( m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vNetPos, flerp );

	if( pGrenadeInfo->_bRecv )
		SetReceiveStates(pGrenadeInfo, pNetInfo);
}

/*virtual*/ void MainWeapon_Claymore::SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo)
{
	VEC3D vNetPos = pNetInfo->GetPos();
	VEC3D vNetDir = pNetInfo->GetDir();

	switch(pNetInfo->GetState())
	{
	case NET_GRENADE_STATE_FLY:
		{
			m_nStateFlag = NET_GRENADE_STATE_FLY;
			m_pWeaponBase->SetVisible( true );
		}
		break;
	case NET_GRENADE_STATE_BOUND:
	case NET_GRENADE_STATE_SETTLED:
		{
			if( !isBombState(WEAPON_BOMBSTATE_SETTLED) )
			{
				addBombState( WEAPON_BOMBSTATE_SETTLED);
				m_pWeaponBase->SetVisible( true );

				// ���� �� �ٶ󺸴� ���� ����
				{
					m_ThrowData.m_rSpeed = 0.0f;
					i3Vector::Copy(m_pWeaponBase->GetPos(), &vNetPos);
					i3Vector::Copy(&m_vLookAt, &vNetDir);
					WeaponGrenade * pGrenade = i3::kind_cast<WeaponGrenade*>(pGrenadeInfo->_pWeaponBase);
					_SetClaymoreMatrix(pGrenade);					
				}

				// Ŭ���̾�Ʈ�� ������Ʈ ���� �ִ� ��츦 ����մϴ�.
				_TakeObject();
			}
		}
		break;
	case NET_GRENADE_STATE_CLICKFIRE:
		{
			if( !isBombState(WEAPON_BOMBSTATE_TIMESTART) )
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 0, m_pWeaponBase->GetPos(), false);
				addBombState(WEAPON_BOMBSTATE_TIMESTART);
			}
		}
		break;
	case NET_GRENADE_STATE_EXPLOSION:
		{
			m_pWeaponBase->SetPos( &vNetPos);
			if( !isBombState( WEAPON_BOMBSTATE_EXPLOSION) )
				Explosion();
		}
		break;
	default:
		break;
	}

	pGrenadeInfo->_bRecv = false;
}

void MainWeapon_Claymore::UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange)
{
	if( rDeltaSeconds > 0.5f)
		rDeltaSeconds = 0.016f;

	if( m_pWeaponBase->IsDedicatedHost() )
	{
		if( m_ThrowData.m_rSpeed < 0.15f)
		{
			if(!m_bCheckFloor)
				_TakeObject();					

			return;
		}

		CheckWorldCollision( rDeltaSeconds);
	}
	else
	{
		UpdateNetworkData( rDeltaSeconds);	// ��Ʈ��ũ ó���Դϴ�.
	}

	m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;	
}

bool MainWeapon_Claymore::isHost(void) const
{
	bool bHost = BattleServerContext::i()->IsP2PHost();

	if( BattleServerContext::i()->IsUseDedicationMode())
		bHost = m_pWeaponBase->getOwner()->isLocal();

	return bHost;
}