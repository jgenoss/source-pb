#include "pch.h"
#include "MainWeapon_GrenadeShell.h"

#include "MyRoomInfoContext.h"
#include "GameMode.h"
#include "TutorialMode.h"
#include "BattleServerContext.h"
#include "BattleObjContext.h"
#define GRENADESHELL_POS_ASSISTTIME		0.1f


#define NET_GRENADESHELL_OFFTIME		15.f				// ��Ʈ��ũ�� 15�ʰ� �� ������ ����
#define GRENADESHELL_POS_ASSISTTIMEINV	10.f


MainWeapon_GrenadeShell::MainWeapon_GrenadeShell(WeaponBase* p) : MainWeapon_Throw(p) 
{
	m_rElapsedExplosionTime = 0.f;
	m_bExplosion = false;
}
	
void	MainWeapon_GrenadeShell::OnInitVariable() 
{
	MainWeapon_Throw::OnInitVariable();

	m_rElapsedExplosionTime = 0.f;
	m_bExplosion = false;
	i3Vector::Zero( &m_vHitNormal);
}

void	MainWeapon_GrenadeShell::Reset()
{
	MainWeapon_Throw::Reset();
	
	m_pWeaponBase->setWeaponStateMask(0);

	m_rElapsedExplosionTime = 0.0f;
	m_bExplosion			= false;

	m_pWeaponBase->SetEnable(true);		//SetVisible( true);
}	

void	MainWeapon_GrenadeShell::Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin)
{
	addBombState( GRENADESHELL_STATE_LAUNCH);

	m_ThrowData.m_rElapsedThrowTime		= 0.f;
	m_ThrowData.m_rTracerUpdateTime		= 0.f;
	m_ThrowData.m_rSpeed				= rSpeed;

	i3Vector::Copy( &m_ThrowData.m_vStartPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vDirection, vDir);

	i3Vector::Copy( &m_ThrowData.m_vOldPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vTracerOldPos, vStart);
}


void MainWeapon_GrenadeShell::UpdateFlying( REAL32 rDeltaSeconds)
{
	if( rDeltaSeconds > 0.5f)
		rDeltaSeconds = 0.016f;

	m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;

	{
		if( m_pWeaponBase->IsDedicatedHost() )
			CheckWorldCollision( rDeltaSeconds);
		else
			UpdateNetworkData( rDeltaSeconds);
	}

		
	//	����ź ����(����)�� �����.
	if( (m_ThrowData.m_rElapsedThrowTime > 0.1f && m_ThrowData.m_rTracerUpdateTime <= 0.f) &&
		!i3Vector::IsSame( m_pWeaponBase->GetPos(), &m_ThrowData.m_vTracerOldPos) &&
		isBombState( GRENADESHELL_STATE_LAUNCH))
	{	
		REAL32 len;
		VEC3D vDir;

		CWeaponGlobal * pGlobalInfo = g_pWeaponInfoDataBase->getGlobal();

		GlobalFunc::PB_TRACE("UpdateFlying %f %f %f", m_pWeaponBase->GetPos()->x, m_pWeaponBase->GetPos()->y, m_pWeaponBase->GetPos()->z);

		i3Vector::Sub( &vDir, m_pWeaponBase->GetPos(), &m_ThrowData.m_vTracerOldPos); 
		len = i3Vector::Length( &vDir); 
		i3Vector::Normalize( &vDir, &vDir);

		//	���� ���� �ð� (�������� tracer�� ���� �����.)
		m_ThrowData.m_rTracerUpdateTime = pGlobalInfo->m_rTracerShellGrenadePieceCreateTime; 

		//	���� �߻�
		//if( m_rElapsedExplosionTime < m_pWeaponInfo->m_rExplosion_Time - 0.1f)	//	�����ϱ� ������ ������ ���ʿ��ϹǷ� ���´�.
		{
			TracerBulletInfo tracerInfo;
			tracerInfo._Type = TRACER_TYPE_SHELL_GRENADE;
			tracerInfo._LifeTime = pGlobalInfo->m_rTracerShellGrenadePieceLifeTime;
			i3Vector::Copy( &tracerInfo._vBeginPos, &m_ThrowData.m_vTracerOldPos);
			i3Vector::Copy( &tracerInfo._vDestPos, m_pWeaponBase->GetPos());
			tracerInfo._Length = len;
			tracerInfo._Thickness = pGlobalInfo->m_rTracerShellGrenadeThickness;
			tracerInfo._EndSpeed = 0;
			i3Color::Copy( &tracerInfo._color, &pGlobalInfo->m_TracerShellGrenadeColor);

			g_pTracerBulletManager->Fire( &tracerInfo);
		}

		i3Vector::Copy( &m_ThrowData.m_vTracerOldPos, m_pWeaponBase->GetPos());

		//I3TRACE( "Grenade Tracer : Update Time %.4f, Length : %.4f, Speed : %.4f\n", m_rTracerUpdateTime, len, speed);
	}
	
	m_ThrowData.m_rTracerUpdateTime -= rDeltaSeconds;
	
}


void MainWeapon_GrenadeShell::CheckWorldCollision( REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult = nullptr;

	VEC3D	vNew;
	
	i3Vector::Scale( &vNew, &m_ThrowData.m_vDirection, m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime);
	i3Vector::Add( &vNew, &m_ThrowData.m_vStartPos, &vNew);
	addY( &vNew, -9.81f * m_pWeaponBase->getWeaponInfo()->GetWeight() * m_ThrowData.m_rElapsedThrowTime * m_ThrowData.m_rElapsedThrowTime);
	//i3Matrix::PreRotateX( GetMatrix(), - I3_DEG2RAD( rDeltaSeconds * 360.f));
	
	ColLine.SetStart( &m_ThrowData.m_vOldPos);
	i3Vector::Sub( &vTemp,  &vNew, &m_ThrowData.m_vOldPos);
	rLength = i3Vector::Normalize( &vTemp, &vTemp);

	REAL32 rTheta = i3Math::atan2( getX( &vTemp), getZ( &vTemp));
	REAL32 rRho = i3Math::acos( getY( &vTemp) );
	i3Matrix::SetRotateY( m_pWeaponBase->GetMatrix(), rTheta);
	i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), rRho);
	i3Matrix::SetPos( m_pWeaponBase->GetMatrix(), &vNew);

	if( rLength <= 0.0f )
	{	// �̵� �ӵ��� 0
		return;
	}

	ColLine.SetEnd( m_pWeaponBase->GetPos());
	ColLine.SetDir( &vTemp);

	REAL32 ColLength = 0.f;
	// �켱������ �̰� World�� �׽�Ʈ �ؾ� �˴ϴ�. (����:PBRU-1000)
	I3_PHYSIX_HIT_RESULT * pWorldResult = nullptr;
	pWorldResult = g_pFramework->raycastClosest(&ColLine, CGRP_MASKGROUP_ATTACK);
	if ((pWorldResult != nullptr))
	{
		ColLength = pWorldResult->m_T;
		// ����� �켱 ���带 �ִ´�.
		pResult = pWorldResult;
	}

	// ĳ���� �浹 ó��
	I3_PHYSIX_HIT_RESULT * pCharaResult = nullptr;

	UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
	if (m_pWeaponBase->getOwner()->getCharaTeam() & CHARACTER_TEAM_BLUE)
		nColGroup = CGRP_MASKGROUP_CHARA_RED;

	pCharaResult = g_pFramework->raycastClosest(&ColLine, nColGroup);

	if (pCharaResult != nullptr)
	{
		i3EventReceiver * pRecv = pCharaResult->m_pDestShapeSet->getEventReceiver();
		if (i3::kind_of<CGameCharaBase*>(pRecv))
		{
			if (!((CGameCharaBase *)pRecv)->isCharaStateMask(CHARA_STATE_DEATH))
			{
				// ĳ���Ͱ� �� �����ٸ� ĳ���͸� ����� �ִ´�.
				if (ColLength == 0.f || ColLength > pCharaResult->m_T)
				{
					pResult = pCharaResult;
				}
			}
		}
	}

	//����, ĳ���� �Ѵ� �ε�ġ�� �ʾҴٸ� ���� ��ƾ��(���� ��ƾ�̹Ƿ�) ���� �ʽ��ϴ�.
	if (pResult == nullptr)
	{
		g_pFramework->reserveRaycastResult(1);
		return;
	}

	{
		VEC3D	vecNew;
		//�̵� ���� ���� �� �̵��Ÿ�
		i3Vector::Sub( &vecNew, &pResult->m_Intersect, &m_ThrowData.m_vOldPos);
		REAL32 rTestLen = i3Vector::Normalize( &vecNew, &vecNew);

		if( rLength + 0.05f < rTestLen )
		{	
			//��ȿ�� �Ÿ����� �浹�� ����..
			return;
		}

		VEC3D vecRevert;
		i3Vector::Scale( &vecRevert, &vecNew, -0.1f);
		i3Vector::Add( m_pWeaponBase->GetPos(), &pResult->m_Intersect, &vecRevert);

		m_bExplosion = true;
		i3Vector::Copy( &m_vHitNormal, &pResult->m_Normal);
	}

	g_pFramework->resetRaycastResult();
}


void MainWeapon_GrenadeShell::Explosion()
{
	//BOOM
	m_pWeaponBase->_OnCheckCollisionByExplosion();

	{
		// ���� ����Ʈ
		g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex(), m_pWeaponBase->GetPos(), &I3_YAXIS);
		I3TRACE("WeaponGrenadeShell::Explosion \n "); 

		g_pWeaponManager->PushGrenadeShellWeapon( static_cast<WeaponGrenadeShell*>(m_pWeaponBase) );
	}

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		if( (g_pFramework->IsBattleStage() != false) && (gmode::i()->GetTutorialMode() != nullptr) )
		{
			VEC3D v3Pos;

			i3Vector::Set(&v3Pos, m_ThrowData.m_vOldPos.x, m_ThrowData.m_vOldPos.y, m_ThrowData.m_vOldPos.z);
			 gmode::i()->GetTutorialMode()->CheckGrenadeInTutorial(v3Pos, m_pWeaponBase->getWeaponInfo()->GetNumber());
		}
	}
}

void	MainWeapon_GrenadeShell::OnUpdate_GrenadeShell( REAL32 rDeltaSeconds)
{
	if( isLaunching())
	{
		// ������ �������
		bool bOwnerInvalid = true;
		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			if( g_pCharaManager->getChara( i) == nullptr)
				continue;
			if( g_pCharaManager->getChara( i) == m_pWeaponBase->getOwner() )
			{
				bOwnerInvalid = false;
				break;
			}
		}
		if( m_pWeaponBase->getOwner() == nullptr || bOwnerInvalid )
		{
			g_pWeaponManager->PushGrenadeShellWeapon( static_cast<WeaponGrenadeShell*>(m_pWeaponBase));
			return;
		}

		m_rElapsedExplosionTime += rDeltaSeconds;

		UpdateFlying( rDeltaSeconds );

		m_pWeaponBase->_TransformWorldSpace( rDeltaSeconds);

		i3Vector::Copy( &m_ThrowData.m_vOldPos, m_pWeaponBase->GetPos());

		{
			if( m_pWeaponBase->IsDedicatedHost() && m_pWeaponBase->getNetworkIdx() != -1)
			{
				GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), &m_ThrowData.m_vOldPos, NET_GRENADE_STATE_FLY);

				if( i3::kind_of<WeaponGrenadeLauncher*>(m_pWeaponBase->getPairWeapon()))
				{// ��ó�� ������ ���� �ͽ��ټ� ������ �Ѵ�.
					throwInfo._bExtension = true;
				}
				
				if( m_bExplosion )
					throwInfo._nAddStateFlag	= NET_GRENADE_STATE_EXPLOSION;

				GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo );
			}
			//I3ASSERT(m_NetIdx != -1);
		}
		
		if( m_ThrowData.m_rElapsedThrowTime >= NET_GRENADESHELL_OFFTIME)
		{// 15�ʱ��� �浹�� ���ų� ��Ŷ�� �� ������ �׳� �����ݴϴ�.
			m_pWeaponBase->_ResetGrenadeInfo();
			g_pWeaponManager->PushGrenadeShellWeapon(static_cast<WeaponGrenadeShell*>(m_pWeaponBase) ); 
		}

		if( m_bExplosion)
		{	
			Explosion();
		}
	}

}

void	MainWeapon_GrenadeShell::NET_Bound_GrenadeShell( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
	
	{
		if( m_pWeaponBase->IsDedicatedHost() && m_pWeaponBase->getNetworkIdx() != -1)
		{
			// ��Ʈ��ũ�� �����ݴϴ�.
			GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), pStartPos, NET_GRENADE_STATE_BOUND);

			if( i3::kind_of<WeaponGrenadeLauncher*>(m_pWeaponBase->getPairWeapon()))
			{// ��ó�� ������ ���� �ͽ��ټ� ������ �Ѵ�.
				throwInfo._bExtension = true;
			}

			throwInfo._nBoundType		= nBoundType;

			GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo);
		}
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

	Throw( pStartPos, pDir, fSpeed , false );
	
}


void MainWeapon_GrenadeShell::UpdateNetworkData( REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vTemp, vDir;

	I3ASSERT( BattleServerContext::i()->IsP2PHost() == false);

	if( m_pWeaponBase->getNetworkIdx() < 0 || m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)	return;

	GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pInfo->_tNetData;

	VEC3D vNetPos = pNetInfo->GetPos();
	flerp = pAssistInfo->_fInterpolateTime += rDeltatime;


	//GlobalFunc::PB_TRACE(" pInfo->_tNetData %f %f %f", pInfo->_tNetData._vPos.x, pInfo->_tNetData._vPos.y, pInfo->_tNetData._vPos.z);


	if( pInfo->_bRecv )
	{
		i3Vector::Sub( &pAssistInfo->_vDir, &vNetPos, &pAssistInfo->_vPostPos);

		if( i3Vector::Length( &pAssistInfo->_vDir) > I3_EPS)
		{	
			i3Vector::Normalize( &vDir, &pAssistInfo->_vDir);
			i3Matrix::MakeWithAt( m_pWeaponBase->GetMatrix(), &vDir, &I3_YAXIS);
			i3Matrix::PreRotateX(  m_pWeaponBase->GetMatrix(), I3_PI2);
		}
		else
		{
			if( m_pWeaponBase->getOwner() != nullptr)
			{

				m_pWeaponBase->SetMatrix( m_pWeaponBase->getOwner()->GetMatrix());
				i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), I3_PI2);
			}
		}
	}

	if( flerp > GRENADESHELL_POS_ASSISTTIME )
	{	
		if( (pNetInfo->GetState() & 0x0F) == NET_GRENADE_STATE_FLY )
		{
			i3Vector::Copy( &pAssistInfo->_vPostPos, m_pWeaponBase->GetPos());
			i3Vector::Add( &vTemp, &pAssistInfo->_vPostPos, &pAssistInfo->_vDir );
			pNetInfo->SetPos( vTemp );
			vNetPos = vTemp;
			
			flerp = rDeltatime;
		}
		else
			flerp = GRENADESHELL_POS_ASSISTTIME;
	}

	flerp *= GRENADESHELL_POS_ASSISTTIMEINV;

	i3Vector::Lerp( m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vNetPos, flerp );
	
	if( pInfo->_bRecv )
	{
		pInfo->_bRecv = false;

		if( pNetInfo->GetState() & NET_GRENADE_STATE_FLY)
			m_pWeaponBase->SetVisible( true );

		if( pNetInfo->GetState() & NET_GRENADE_STATE_EXPLOSION )
		{
			m_pWeaponBase->SetPos( &vNetPos );
			Explosion();
		}
	}
}

void MainWeapon_GrenadeShell::_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType)
{
	I3ASSERT( g_pViewer != nullptr);
	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nTerrainType, GTBT_BOUND_THROW, m_pWeaponBase->GetPos(), false);
}
