#include "pch.h"
#include "MainWeapon_MeatBomb.h"
#include "BattleObjContext.h"

#include "../UI/HUD/UIHudDomi_Indicator.h"
#include "../UI/HUD/UIHUD_Domination_Manager.h"

#include "WeaponFunction.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "GameCharaBoneContext.h"
#include "./StageBattle/UserContext.h"


namespace
{
	// WeaponC4.cpp���� �״�� ������.
	i3TimeSequenceInfo* MakeSoundObject(const char * pszSndName, REAL32 startTime, REAL32 duration)
	{
		i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( pszSndName);
		
		if(pRes == nullptr)
		{
			I3PRINTLOG(I3LOG_WARN, "Meat Bomb : Sound Object ���� : %s", pszSndName);
			I3ASSERT_RETURN(pRes != nullptr, nullptr);
		}

		i3SoundPlayInfo * pInfo = pRes->getSound();
		I3ASSERT( pInfo != nullptr);

		i3SoundPlayInfo * pNewInfo = i3SoundPlayInfo::new_object_ref();
		pInfo->CopyTo( pNewInfo, I3_COPY_INSTANCE);

		if( duration > 0.0f)
		{
			pNewInfo->setLoopMode( I3SND_RT_LOOPMODE_ONESHOT);
		}
		else
		{
			pNewInfo->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);
		}

		pNewInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);

		pNewInfo->setInsideConeAngle( 360);
		pNewInfo->setOutsideConeAngle( 360);
		pNewInfo->setOutsideVolume( 100);

		i3SoundNode * pNewSndNode = i3SoundNode::new_object_ref();
		pNewSndNode->setSoundPlayInfo( pNewInfo);
		i3TimeSequenceInfo * pTimeSeq = i3TimeSequenceInfo::new_object_ref();
		pTimeSeq->SetStartTime( startTime);
		pTimeSeq->SetNode( pNewSndNode);

		return pTimeSeq;
	}
}

//----------------------------------------------
// ��Ʈ�� ����� ó��
//----------------------------------------------
class ProcessMeatBombLight
{
public:
	explicit ProcessMeatBombLight(WeaponMeatBomb* mb);
	virtual ~ProcessMeatBombLight();

	void OnCreate();
	void OnInit();
	void OnStart();
	void OnRun();
	void OnEnd();

private:
	WeaponMeatBomb* m_meatbomb;
	i3Transform*	m_light;
	i3TimeSequence*	m_soundList;
};


//----------------------------------------------
// ��Ʈ�� ����� ó��
//----------------------------------------------
ProcessMeatBombLight::ProcessMeatBombLight(WeaponMeatBomb* mb) : m_meatbomb(mb), m_light(0), m_soundList(0) {}

ProcessMeatBombLight::~ProcessMeatBombLight() 
{
	m_light = nullptr; 
	I3_SAFE_RELEASE(m_soundList);

}

void ProcessMeatBombLight::OnInit()
{
	if (m_soundList)
	{
		m_soundList->RemoveFromParent();
		m_soundList->OnChangeTime(0.0f);
	}

	I3_SAFE_RELEASE(m_soundList);
}

void ProcessMeatBombLight::OnCreate()
{
	m_soundList = i3TimeSequence::new_object();

	REAL32 startTime = 0.0f;
	REAL32 duration = 1.0f;

	for (INT32 i=0; i<10; i++)
	{
		m_soundList->AddSequence(MakeSoundObject("WM_C4_Count_A.wav", startTime, duration));
		startTime += duration;
	}
}

void ProcessMeatBombLight::OnStart()
{
	if (m_light == nullptr)
	{
		VEC3D pos;
		m_light = g_pEffectManager->AddEnvironmentEffect(ENV_EFFECT_BOMB_LIGHT, &pos, &I3_YAXIS, m_meatbomb->getSceneObject()->GetCacheMatrix());
	}

	if (m_soundList)
	{
		m_meatbomb->GetNode()->AddChild(m_soundList);
		m_soundList->OnChangeTime(0.0f);
	}
}

void ProcessMeatBombLight::OnRun()
{
}

void ProcessMeatBombLight::OnEnd()
{
	//	����� ����Ʈ ����
	if (m_light)
	{
		m_light->AddFlag( I3_NODEFLAG_DISABLE);
		m_light = nullptr;
	}

	if (m_soundList)
	{
		m_meatbomb->GetNode()->RemoveChild(m_soundList);
	}
}

MainWeapon_MeatBomb::~MainWeapon_MeatBomb()
{
	I3_SAFE_DELETE(m_cbLight);
}

MainWeapon_MeatBomb::MainWeapon_MeatBomb(WeaponBase* p) : MainWeapon_Grenade(p), 
m_rRotateY(0.f), m_bFirstBound(false)
{
	m_cbLight = new ProcessMeatBombLight(static_cast<WeaponMeatBomb*>(p) );
	MEMDUMP_NEW( m_cbLight, sizeof(ProcessMeatBombLight));
}

void	MainWeapon_MeatBomb::OnInitVariable() 
{
	MainWeapon_Grenade::OnInitVariable();

	m_rRotateY = 0.f;
	m_bFirstBound			= false;
	i3Vector::Zero(&m_vAttachNormal);

	m_cbLight->OnInit();
}

void	MainWeapon_MeatBomb::OnCreate( bool bCreateInWeapon)
{
	MainWeapon_Grenade::OnCreate( bCreateInWeapon);

	m_cbLight->OnCreate();
}
	
void	MainWeapon_MeatBomb::Reset()
{
	MainWeapon_Grenade::Reset();

	m_rRotateY = 0.f;
	m_bFirstBound			= false;
	
	i3Vector::Zero(&m_vAttachNormal);

}

void	MainWeapon_MeatBomb::CheckWorldCollision(REAL32 rDeltaSeconds)
{

	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult = nullptr;
	bool	bCharaCollision = false;

	INT32	nBoundType = 0;

	VEC3D	vNew;
	i3Vector::Scale( &vNew, &m_ThrowData.m_vDirection , m_ThrowData.m_rSpeed  * m_ThrowData.m_rElapsedThrowTime ); 
	i3Vector::Add( m_pWeaponBase->GetPos(), &m_ThrowData.m_vStartPos , &vNew);
	REAL32	rGravity = -5.0f * m_pWeaponBase->getWeaponInfo()->GetWeight() * m_ThrowData.m_rElapsedThrowTime * m_ThrowData.m_rElapsedThrowTime;
	addY( m_pWeaponBase->GetPos(), rGravity);

	m_rRotateY -= I3_DEG2RAD( rDeltaSeconds * 360.f);
	i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), - I3_DEG2RAD( rDeltaSeconds * 360.f));


	if(!m_bFirstBound)
	{
		i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), - I3_DEG2RAD( rDeltaSeconds * 360.f));
	}

	ColLine.SetStart( &m_ThrowData.m_vOldPos );
	i3Vector::Sub( &vTemp,  m_pWeaponBase->GetPos(), &m_ThrowData.m_vOldPos );
	rLength = i3Vector::Normalize( &vTemp, &vTemp);

	if( rLength <= 0.f )
	{
		this->_ProcessBoundObjectForce();
		return;
	}


	ColLine.SetEnd( m_pWeaponBase->GetPos());
	ColLine.SetDir( &vTemp);

	REAL32 rMinLen = 10000.f;
	// World collision
	UINT32 nWorldCount = g_pFramework->raycast( &ColLine, CGRP_MASKGROUP_ATTACK );
	if( nWorldCount >0)
	{
		I3_PHYSIX_HIT_RESULT * pWorldResult = g_pFramework->getRaycastResult();
		g_pFramework->reserveRaycastResult( nWorldCount);

		for( UINT32 i = 0; i < nWorldCount; i++)
		{
			if( pWorldResult[ i].m_pDestShapeSet->getShapeGroup() != CGRP_ITEM_HIT)
			{	
				if( pWorldResult[ i].m_T < rMinLen )
				{
					pResult = &pWorldResult[ i];
					rMinLen = pResult->m_T;
				}
			}
		}
	}

	if( pResult == nullptr)
	{
		this->_ProcessBoundObjectForce();
		return;
	}

	// ù��° raycast�� Chara���� Rough Collision�� �˻��մϴ�.
	UINT32 nRoughCount = g_pFramework->raycast( &ColLine, CGRP_MASKGROUP_GRENADE );
	if( nRoughCount > 0)
	{
		I3_PHYSIX_HIT_RESULT * pRoughResult = g_pFramework->getRaycastResult();

		g_pFramework->reserveRaycastResult( nRoughCount + nWorldCount);

		for( UINT32 i = 0; i < nRoughCount; i++)
		{
			i3EventReceiver * pRecv = pRoughResult[ i].m_pDestShapeSet->getEventReceiver();

			if( !i3::kind_of<CGameCharaBase*>(pRecv))
				continue;

			UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
			CGameCharaBase * pTarget = (CGameCharaBase *)pRecv;

			if( pTarget == m_pWeaponBase->getOwner() )
				continue;

			if( pTarget->getCharaTeam() & CHARACTER_TEAM_RED)
			{
				nColGroup = CGRP_MASKGROUP_CHARA_RED;
			}

			UINT32 nCount = g_pFramework->raycast( &ColLine, nColGroup );
			if( nCount <= 0)
				continue;

			I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->getRaycastResult();
			g_pFramework->reserveRaycastResult( nCount + nRoughCount + nWorldCount);

			for( UINT32 j = 0; j < nCount; j++)
			{
				pRecv = pHitResult[ j].m_pDestShapeSet->getEventReceiver();
				if( !i3::kind_of<CGameCharaBase*>(pRecv))
					continue;

				pTarget = (CGameCharaBase *) pRecv;
				if( pTarget == m_pWeaponBase->getOwner() )
					continue;

				if( !pTarget->isCharaStateMask( CHARA_STATE_DEATH) &&
					(rMinLen > pHitResult[ j].m_T) )
				{
					pResult = &pHitResult[ j];
					rMinLen = pHitResult[ j].m_T;
					bCharaCollision = true;

					//I3TRACE( "CharaCol %d %.4f\n", pTarget->getCharaNetIndex(), rMinLen);
				}
			}
		}
	}

	VEC3D	vecNew, vecNewPos, vecNormal;
	REAL32	rDot1 = 0.0f;

	//�̵� ���� ���� �� �̵��Ÿ�
	i3Vector::Sub( &vecNew, &pResult->m_Intersect, &m_ThrowData.m_vOldPos); //&m_vOldPos);
	REAL32 rTestLen = i3Vector::Normalize( &vecNew, &vecNew);

	// �����̴� ������Ʈ��� ���� ����ؼ� �ݻ緮�� ��������մϴ�. by dfly79 080117
	m_rBoundObjectLen = _BoundObject( pResult, ColLine.GetDir());

	if( (rLength < rTestLen - 0.03f + m_rBoundObjectLen) || (rTestLen == 0.f) )
	{	
		//��ȿ�� �Ÿ����� �浹�� ����..
		_ProcessBoundObjectForce();
		return;
	}

	// �� ��ŭ �ڷ� ������.
	// Step1. ����ź�� ��ġ�� �浹�� �Ͼ ��ġ�� �̵�( ����ź�� Radius�� ����)
	VEC3D vecRevert;
	i3Vector::Scale( &vecRevert, &vecNew, - 0.03f);

	if( getX( &m_vObjectForce) != 0.f || getY( &m_vObjectForce) != 0.f || getZ( &m_vObjectForce) != 0.f)
	{
		VEC3D vAddForce;
		REAL32 rHeightGravity = -rGravity * getY( &m_vObjectForce);
		i3Vector::Scale( &vAddForce, &m_vObjectForce, 4.f);
		i3Vector::Add( &vecRevert, &vecRevert, &vAddForce);
		i3Vector::AddY( &vecRevert, rHeightGravity);
	}

	REAL32 rDot = 1.f;

	UINT16 shapeGroup = pResult->m_pDestShapeSet->getShapeGroup();
	if( ( shapeGroup== CGRP_WORLD_ATTACK) || OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup ) )
	{
		rDot = i3Vector::Dot( &pResult->m_Normal, &I3_YAXIS);
	}

	m_bFirstBound = true;

	VEC3D vGrenadeSize;
	i3Vector::Scale( &vGrenadeSize, &pResult->m_Normal, 0.03f);

	i3Vector::Add( &vecRevert, &vecRevert, &vGrenadeSize);
	i3Vector::Add( m_pWeaponBase->GetPos(), &pResult->m_Intersect, &vecRevert);	//	���� �ݴ� �������� ������.
	i3Vector::Add( &m_ThrowData.m_vOldPos ,  &m_ThrowData.m_vOldPos, &vecRevert);

	i3Vector::Add( &vecNewPos, &pResult->m_Intersect, &vecNew);	//NewPosition
	rDot1 = i3Vector::Dot( &vecNew, &pResult->m_Normal);

	i3Vector::Scale( &vecNormal, &pResult->m_Normal, - 2.0f * rDot1);
	i3Vector::Add( &vecNewPos, &vecNewPos, &vecNormal);

	i3Vector::Sub( &vecNew, &vecNewPos, &pResult->m_Intersect);
	i3Vector::Normalize( &vecNew, &vecNew);


	VEC3D vPos;
	i3Vector::Copy( &vPos, m_pWeaponBase->GetPos());
	i3Matrix::Identity( m_pWeaponBase->GetMatrix());
	i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), m_rRotateY);

	m_pWeaponBase->SetPos( &vPos);

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

	//�� ��ô
	NET_Bound( m_pWeaponBase->GetPos(), &vecNew, Speed, nBoundType );

	i3Vector::Set( &m_vAttachNormal, 0.f, 1.f, 0.f);

	g_pFramework->resetRaycastResult();

}

void MainWeapon_MeatBomb::UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange)
{

	if( m_ThrowData.m_rSpeed  < 0.15f)
	{
		this->_ProcessTakeObject();
		return;
	}

	if( rDeltaSeconds > 0.5f)
		rDeltaSeconds = 0.016f;

	if( m_pWeaponBase->IsDedicatedHost() )
	{
		CheckWorldCollision( rDeltaSeconds);
	}
	else
	{	// ��Ʈ��ũ ó���Դϴ�.
		UpdateNetworkData( rDeltaSeconds);
	}

	m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;
}

void MainWeapon_MeatBomb::UpdateNetworkData(REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vTemp, vDir, vAxis;

	I3ASSERT( m_pWeaponBase->IsDedicatedHost() == false);

	if( m_pWeaponBase->getNetworkIdx() < 0 || 
		m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)	return;

	GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pInfo->_tNetData;

	VEC3D vNetPos = pNetInfo->GetPos();
	VEC3D vNetDir = pNetInfo->GetDir();

	if( (this->isBombState( WEAPON_BOMBSTATE_SETTLED) == false) && (pNetInfo->GetState() & NET_GRENADE_STATE_FLY) )
	{
		// ���߿��� ���󰡰� �ִ� ����
		// ����������Ʈ �մϴ�.
		pAssistInfo->_fInterpolateTime += rDeltatime;
		flerp	= pAssistInfo->_fInterpolateTime;

		REAL32 rInterpolTime = pInfo->_fBroadcastCheckSyncTime + pInfo->_fBroadcastCheckSyncTime * 0.2f;

		if( flerp > rInterpolTime)
			flerp = rInterpolTime;
		flerp /= rInterpolTime;

		// Ŭ���̾�Ʈ �󿡼� ���Ƿ� ȸ�������ݴϴ�.
		m_rRotateY -= I3_DEG2RAD( rDeltatime * 360.f);

		i3Vector::Lerp( m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vNetPos, flerp );
		i3Vector::Sub( &vTemp, &pAssistInfo->_vPostPos, &vNetPos );
		if( i3Vector::LengthSq( &vTemp ) > I3_EPS )
		{
			i3Matrix::PreRotateX(  m_pWeaponBase->GetMatrix(), -I3_DEG2RAD( rDeltatime * 360.f));
			i3Matrix::PreRotateY(  m_pWeaponBase->GetMatrix(), -I3_DEG2RAD( rDeltatime * 360.f));
		}
	}

	if( pInfo->_bRecv )
	{
		if( pNetInfo->GetState() & NET_GRENADE_STATE_FLY)
			 m_pWeaponBase->SetVisible( true );

		if( (this->isBombState( WEAPON_BOMBSTATE_SETTLED) == false) )
		{
			// ���忡 �����˴ϴ�.
			if( (pNetInfo->GetState() & NET_GRENADE_STATE_SETTLED) )
			{
				m_pWeaponBase->SetVisible( true );
				this->addBombState( WEAPON_BOMBSTATE_SETTLED);

				// ���� ����ϴ�.
				i3Vector::Set( &vDir, getX( &vNetDir), getY( &vNetDir) , getZ( &vNetDir));
				i3Matrix::MakeWithAt( m_pWeaponBase->GetMatrix(), &vDir, &I3_YAXIS);
				i3Vector::Copy( &vAxis, i3Matrix::GetRight( m_pWeaponBase->GetMatrix()));
				if( getY( &vNetDir) < 0.5f)
				{	// �𵨸��� Z�������� ȸ������ ���� ���̸� ��������մϴ�.
					i3Vector::Copy( &vAxis, i3Matrix::GetAt( m_pWeaponBase->GetMatrix()));
					i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), I3_PI2);
				}
				else
				{	// �ٴڿ� �����Ǵ� ����Դϴ�.
					i3Matrix::Identity( m_pWeaponBase->GetMatrix());
					i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), m_rRotateY);
				}

				// ��ǥ�� ����ϴ�.
				 m_pWeaponBase->SetPos( &vNetPos);

				m_cbLight->OnStart();

				CUIHudDomi_Indicator* ui = (CUIHudDomi_Indicator*)UIHUDDominationMgr::i()->GetHUD( HUD_DOMI_INDICATOR);
				tmp_uu::CIndicateObject* ui_obj	= ui->InsertIndication( m_pWeaponBase, WAVE_MISSION_BOMB, tmp_uu::MEATBOMB);
				static_cast<UI_Util::CIndicateMeatBomb*>(ui_obj)->SetNetIdx(m_pWeaponBase->getNetworkIdx() );
			}
		}
		else
		{
			if( this->isBombState( WEAPON_BOMBSTATE_ATTACHOBJECT) &&
				!(pNetInfo->GetState() & NET_GRENADE_STATE_TAKEOBJECT))
			{
				this->removeBombState( WEAPON_BOMBSTATE_SETTLED);
			}
		}

		if( pNetInfo->GetState() & NET_GRENADE_STATE_BOUND )
		{// �ٿ�� ����
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);
			// �ٿ�� �� �ö��� ���� �ٲ���� �մϴ�.
			UINT8 tState = pNetInfo->GetState();
			tState	&= ~NET_GRENADE_STATE_BOUND;
			pNetInfo->SetState(tState);
		}

		pInfo->_bRecv = false;

		if( pNetInfo->GetState() & NET_GRENADE_STATE_EXPLOSION )
		{
			m_pWeaponBase->SetPos( &vNetPos);
			if( this->isBombState( WEAPON_BOMBSTATE_EXPLOSION) == false)
			{
				UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_INDICATOR, HUD_DMN_EVT_DELETE_MEATBOMB, m_pWeaponBase->getNetworkIdx() );
				Explosion();
				m_cbLight->OnEnd();
			}
		}
	}
}

/*virtual*/ void MainWeapon_MeatBomb::_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType)
{
	I3ASSERT( g_pViewer != nullptr);
	g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);
}

void	MainWeapon_MeatBomb::OnUpdate_Grenade(REAL32 rDeltaSeconds)
{
	if( this->isThrowing())
	{	// ���忡 ������ ����ź�Դϴ�.
		// ������ �ִ��� üũ�ؼ� ������ ��ȯ�����ݴϴ�.
		if( _CheckProcessUserLeave() == false)
			return;

		WF::PlayWeaponAnim(m_pWeaponBase, m_pWeaponBase->getAIIndex(WEAPON::AI_LOADBULLET));

		bool	bSettled = false;
		bool	bExplosion = false;
		bool	bHost = m_pWeaponBase->IsDedicatedHost();

		if (bHost)
		{
			if( m_ThrowData.m_rSpeed < 0.2f && ! this->isBombState( WEAPON_BOMBSTATE_SETTLED) )
			{
				 this->addBombState( WEAPON_BOMBSTATE_SETTLED);
				bSettled = true;
				m_cbLight->OnStart();

				// ��Ʈ�� ��� �� ���� 0���� �����Ͽ� ��ų������ �ٽ� ������ �� �ֵ���.
				DomiUserSkillMgr::i()->SetLevel(m_pWeaponBase->getOwner()->getCharaNetIndex(), USER_SKILL_MEATBOMB, 0);

				CUIHudDomi_Indicator* ui = (CUIHudDomi_Indicator*)UIHUDDominationMgr::i()->GetHUD( HUD_DOMI_INDICATOR);
				tmp_uu::CIndicateObject* ui_obj	= ui->InsertIndication( m_pWeaponBase, WAVE_MISSION_BOMB, tmp_uu::MEATBOMB);
				static_cast<UI_Util::CIndicateMeatBomb*>(ui_obj)->SetNetIdx(m_pWeaponBase->getNetworkIdx() );
			}
		}

		if(  this->isSettled())
		{
		//	m_rElapsedExplosionTime += rDeltaSeconds;
			this->SetElapsedExplosionTime( this->GetElapsedExplosionTime() + rDeltaSeconds);
			m_cbLight->OnRun();
		}

		REAL32 rExplosionRange = m_pWeaponBase->GetExplosionTime();

		this->UpdateFlying( rDeltaSeconds, rExplosionRange );

		m_pWeaponBase->_TransformWorldSpace( rDeltaSeconds);

		i3Vector::Copy( &m_ThrowData.m_vOldPos , m_pWeaponBase->GetPos());

		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			if( this->GetElapsedExplosionTime() > rExplosionRange)
			{
				bExplosion = true;
			}

			if( m_ThrowData.m_rElapsedThrowTime > 15.f/*NET_GRENADE_OFFTIME*/)
			{// ��Ʈ���� �޴� ��� 15�ʱ��� ��Ŷ�� �� ������ �׳� �����ݴϴ�.
				if( bHost )
				{
					bExplosion = true;
				}
				else
				{
					GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
					pInfo->Reset();

					m_pWeaponBase->_ResetGrenadeInfo();
					g_pWeaponManager->PushThrowWeapon(m_pWeaponBase); 
				}
			}

			if( bHost )
			{
				// ��Ʈ��ũ�� �����ݴϴ�.
				GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), &m_ThrowData.m_vOldPos, NET_GRENADE_STATE_FLY);
				throwInfo._pNormal				= GetThrowInfo_Normal();
				if( bExplosion )	throwInfo._nAddStateFlag	= NET_GRENADE_STATE_EXPLOSION;
				else if( bSettled )		throwInfo._nAddStateFlag	= NET_GRENADE_STATE_SETTLED;

				if( this->isBombState( WEAPON_BOMBSTATE_ATTACHOBJECT) && (m_pAttachObject != nullptr) )
				{
					throwInfo._nAddStateFlag	|= NET_GRENADE_STATE_TAKEOBJECT;
					throwInfo._nBoundType		= m_pAttachObject->getNetIndex();
				}

				GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo );
			}

			if( bExplosion && !this->isBombState( WEAPON_BOMBSTATE_EXPLOSION))
			{
				UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_INDICATOR, HUD_DMN_EVT_DELETE_MEATBOMB, 
					m_pWeaponBase->getNetworkIdx() );
				Explosion();
				m_cbLight->OnEnd();
			}
		}
	}
	
	
}


void MainWeapon_MeatBomb::Explosion(void)
{
	m_pWeaponBase->ResetCommand();

	_CheckExplosionCollision( m_pWeaponBase->GetExplosionRange() );

	// ���� ����Ʈ
	g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex(), 
		m_pWeaponBase->GetPos(), &I3_YAXIS);

	// ���� ��ź�� �����մϴ�.
	g_pWeaponManager->PushThrowWeapon( m_pWeaponBase);
} 

void MainWeapon_MeatBomb::_CheckExplosionCollision( REAL32 rRange)
{
	if( m_pWeaponBase->getOwner() == nullptr)
		return;

	INT32 i;
	VEC3D vLength;

	//------------------------------------------------------------------------------------------//
	//										Character Collision	
	bool bHost = BattleServerContext::i()->IsP2PHost();

	UINT32 nColGroup = CGRP_MASKGROUP_ATTACK;

	if( BattleServerContext::i()->IsUseDedicationMode())
	{	// Dedicated�� �ڽŸ� �ϸ� �ȴ�.
		if (m_pWeaponBase->getOwner()->isLocal())
			bHost = true;
		else
			bHost = false;
	}

	for( i = 0; i < g_pCharaManager->getCharaCount(); ++i)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if (pChara == nullptr) continue;
		if (pChara->IsDomiDino() == false && pChara->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx())
		{
			i3Vector::Sub( &vLength, m_pWeaponBase->GetPos(), i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()));
			REAL32 rLength = i3Vector::Length( &vLength );

			//����ź�� ������ ��ó�� Owner�� ��ġ���ִ°�� ī�޶� ��鸲 ȿ��
			if( (g_pCamera != nullptr) && m_pWeaponBase->getOwner()->is1PV() && !m_pWeaponBase->getOwner()->isCharaStateMask( CHARA_STATE_DEATH) )
			{	
				REAL32 MaxLength	= rRange * 2.0f;
				if( rLength < MaxLength)
				{
					REAL32 TrembAmpRate = 1.0f - rLength/MaxLength;
					g_pCamera->SetTremble( 15.0f * TrembAmpRate * TrembAmpRate, 10.0f * TrembAmpRate, 0.2f, 1.0f *	TrembAmpRate);
				}
			}
			continue;
		}
		if (pChara->IsDomiDino() == false) continue;

		if (pChara->getCharaTeam() & CHARACTER_TEAM_RED)
			nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
		else
			nColGroup = CGRP_MASKGROUP_CHARA_RED;

		i3Vector::Sub( &vLength, m_pWeaponBase->GetPos(), i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()));
		REAL32 rLength = i3Vector::Length( &vLength );

		// ���常 ĳ���� �ø����� �ϵ��� �մϴ�.
		if( rLength < rRange)
		{
			if( bHost )
			{
				m_pWeaponBase->_OnCheckCharaCollisionByExplosion( pChara, nColGroup, rRange);
			}
		}
	}

	if( BattleServerContext::i()->IsP2PHost() )
	{
		// ����ź���� ���� ĳ���͸� �켱������ �����ݴϴ�.
		UserContext::i()->SendDeathChara();
	}

	m_pWeaponBase->_CheckExplosionWeapon( rRange);

	//------------------------------------------------------------------------------------------//
	//										Object Collision
	m_pWeaponBase->_CheckExplosionObjectCollision( rRange);
	//------------------------------------------------------------------------------------------//
}

