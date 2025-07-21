#include "pch.h"
#include "WeaponShotGunLauncherWithLaser.h"
#include "WeaponShotGunLauncher.h"
#include "GlobalVariables.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaActionContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "DotLazer.h"
#include "MyRoomInfoContext.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( WeaponShotGunLauncherWithLaser);//, WeaponShotGunLauncher);

namespace 
{
	bool Is1PVObserver(const CGameCharaBase* pOwner)
	{
		if (!pOwner) return false;

		return pOwner->isCharaStateMask(CHARA_STATE_OBSERVER) && pOwner->is1PV() ? true : false;
	}
}

#define LAZER_SIGHT_THIKNESS	0.5f

WeaponShotGunLauncherWithLaser::WeaponShotGunLauncherWithLaser(void) : m_fElapsedTime(0.f)
	, m_fLocalTime(10.0f), m_bReloaded(false), m_fRandX(0.0f), m_fRandY(0.0f), m_bCalcFirst(false)
	, m_fCalcTime(0.0f)
{
	m_pDotLazer = CDotLazer::new_object();
	I3ASSERT( m_pDotLazer != nullptr);

	i3mem::FillZero( m_vecRandPos, sizeof( m_vecRandPos));
	i3mem::FillZero( m_vecRandTangent, sizeof( m_vecRandTangent));
	i3mem::FillZero( m_vecRandSub, sizeof( m_vecRandSub));
}

WeaponShotGunLauncherWithLaser::~WeaponShotGunLauncherWithLaser(void)
{
	I3_SAFE_RELEASE( m_pDotLazer);
}


void WeaponShotGunLauncherWithLaser::_CreateLazerDot(void)
{
	if( m_pOwner != nullptr)
	{
		if (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			m_pDotLazer->CreateLazerDot();
	}	
}

void WeaponShotGunLauncherWithLaser::_UpdateLazerDot(void)
{
	if( m_pOwner != nullptr)
	{		
		if( (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			&& m_pOwner->GetCurrentCharaWeapon() == this)
		{
			if( GetCurrentExtensionType() == WEAPON::EXT_ZOOM && 
				(isWeaponStateMask(WSM_ZOOM) || m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM)))
			{	//	�� ������ ���
				m_pDotLazer->DotStampDisable();
			}
			else
			{
				// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.
				GetShotTargetPos( m_TargetPos);

				m_pDotLazer->UpdateDotStamp();
			}
		}		
		else
		{
			m_pDotLazer->DotStampDisable();
		}
	}
}

void WeaponShotGunLauncherWithLaser::DotStampDisable()
{
	if( m_pOwner != nullptr)
	{
		if( (m_pOwner->isLocal() && m_pOwner->GetCurrentCharaWeapon() == this) ||
			Is1PVObserver(m_pOwner))
		{
			m_pDotLazer->DotStampDisable();
		}
	}	
}

void WeaponShotGunLauncherWithLaser::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponShotGunLauncher::Create( pOwner, Itemid, bCreateInWeapon);

	m_pDotLazer->CreateLazerSight(getSceneObject());

	if( m_pOwner != nullptr)
	{
		if (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
		{
			m_pDotLazer->CreateLazerDot();
		}
	}
}

void WeaponShotGunLauncherWithLaser::Reset( void)
{
	WeaponShotGunLauncher::Reset();

	m_fElapsedTime = 0.0f;
	m_fLocalTime = 10.0f;

	m_fRandX = 0.0f;
	m_fRandY = 0.0f;
	m_bCalcFirst = false;
	m_fCalcTime = 0.0f;	

	if( m_pOwner != nullptr)
	{
		if ((m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			&& m_pOwner->GetCurrentCharaWeapon() == this)
		{
			m_pDotLazer->DotStampDisable();
			m_pDotLazer->CreateLazerDot();
		}
	}
}

void WeaponShotGunLauncherWithLaser::OnBindResource( void)
{
	WeaponShotGunLauncher::OnBindResource();
}

void WeaponShotGunLauncherWithLaser::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGunLauncher::OnIdle( pCtx, tm);
}

void WeaponShotGunLauncherWithLaser::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGunLauncher::OnFire( pCtx, tm);
}

bool WeaponShotGunLauncherWithLaser::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bRet = WeaponShotGunLauncher::OnLoadMag( pCtx, tm);

	if( pCtx->getLocalTime() == 0.0f)
	{
		m_bReloaded = true;
	}
	else if( bRet)
	{
		m_bReloaded = false;
	}

	return true;
}

void WeaponShotGunLauncherWithLaser::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGunLauncher::OnEndLoadMag( pCtx, tm);

	m_bReloaded = false;
}

void WeaponShotGunLauncherWithLaser::OnUpdate( REAL32 rDeltaSeconds)
{	
	m_fElapsedTime = rDeltaSeconds;

	WeaponShotGunLauncher::OnUpdate( rDeltaSeconds);	
	
	if( m_pOwner != nullptr)
	{
		bool bEnableLaserByOption = g_pEnvSet->m_bEnableLaserSight;

		if( m_pOwner->isLocal())
		{
			if( m_pOwner->GetCurrentCharaWeapon() == this)
			{
				// ������ ������ ����Ʈ off �Ұ����� ����� �׻� �ѵд�. (��. Ŀ���� M4)
				if( isPossibleTurnOffWithDotLaser() == false)	
					bEnableLaserByOption = true;

				if (MyRoomInfoContext::i()->getStage()->IsHudOff())
				{
					// ������ ����Ʈ ǥ���ϱ�
					if (isWeaponStateMask(WSM_ZOOM))
					{
						m_pDotLazer->SetEnableDotLaser(true);
						_UpdateLazerDot();

					}
					else
					{
						m_pDotLazer->SetEnableDotLaser(false);
					}
				}
				else
				{
					// ������ ����Ʈ ǥ���ϱ�
					if (bEnableLaserByOption || isWeaponStateMask(WSM_ZOOM) ||
						(GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate()))
					{
						m_pDotLazer->SetEnableDotLaser(true);
						_UpdateLazerDot();
					}
					else
					{
						m_pDotLazer->SetEnableDotLaser(false);
					}
				}
	
			}
			else
			{
				m_pDotLazer->SetEnableDotLaser(false);
			}
		}
		else
		{
			if( m_pOwner->GetCurrentCharaWeapon() == this)
			{
				m_pDotLazer->SetEnableLaserBar(true);
			}	

			if (CGameCameraManager::i())
			{
				CGameCharaBase* pChara = CGameCameraManager::i()->getTargetCharacter();
				if (pChara && pChara == m_pOwner)
				{
					//������ 1��Ī���¿��� �ߵ�!!
					if (Is1PVObserver(m_pOwner))
					{
						if (isWeaponStateMask(WSM_ZOOM) ||
							(GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate()))
						{
							m_pDotLazer->SetEnableDotLaser(true);
							_UpdateLazerDot();
						}
						else
						{
							m_pDotLazer->SetEnableDotLaser(false);
						}
					}

					if (UserInfoContext::i()->IsGM_Observer() && m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM))
					{
						m_pDotLazer->DotStampDisable();
					}
				}
			}
		}
	}
	else
	{
		// ǥ������ �ʱ�
		m_pDotLazer->SetEnableDotLaser(false);
	}	
}

void WeaponShotGunLauncherWithLaser::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponShotGunLauncher::AttachNode( pSceneNode, pOwner);
	_CreateLazerDot();
}

void WeaponShotGunLauncherWithLaser::DetachNode( void )
{
	WeaponShotGunLauncher::DetachNode();

	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponShotGunLauncherWithLaser::OnDrop( void)
{
	WeaponShotGunLauncher::OnDrop();
	
	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponShotGunLauncherWithLaser::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart)
{
	if( m_pOwner == nullptr)
		return;

	if( (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
		&& m_pOwner->GetCurrentCharaWeapon() == this)
	{
		REAL32	fRange = 400.f;

		if( m_pWeaponInfo->GetTypeFire() == WEAPON::FIRE_MELEE )
			fRange = 400.f;
		else
			fRange = GetRange();

		m_pDotLazer->GetShotTargetPos(pDest, pStart, m_bReloaded, fRange, m_pOwner, getFXMatrix());
		if(isWeaponStateMask( WSM_ZOOM))
		{
			//m_pDotLazer->DotStampDisable();				
		}
	}
	else
	{
		WeaponBase::GetShotTargetPos( pDest, pStart);			
	}
}

void WeaponShotGunLauncherWithLaser::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if( m_pOwner == nullptr)
		return;

	if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate())
	{
		WeaponShotGunLauncher::FireCollision( pStartPos, pTargetPos);
	}
	else
	{
		VEC3D target, start;
		INT32 i, collGroup;

		i3Vector::Copy( &target, pTargetPos);

		if( pStartPos == nullptr )
		{
			// �Ѿ��� �߻� �������� (1��Ī�� ��쿡��) Camera�� Position����,
			// (3��Ī�� ��쿡��) �ѱ� �������� �Ѵ�.
			if( m_pOwner->is1PV())
			{
				// ����� Camera�� ����
				m_pOwner->getCameraContext()->getVirtualCamPosition( &start );
			}
			else
			{
				i3Vector::Copy( &start, i3Matrix::GetPos( getFXMatrix()));
			}
		}
		else
		{
			i3Vector::Copy( &start, pStartPos );
		}

		collGroup = CGRP_MASKGROUP_ATTACK;
		if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)
			collGroup = CGRP_MASKGROUP_CHARA_BLUE;
		else
			collGroup = CGRP_MASKGROUP_CHARA_RED;

		// ��Ʈ��ũ�� �����ݴϴ�.
		NET_FireSync( &target );

		for( i = 0; i < m_pWeaponInfo->GetFireBulletCount(); i++)
		{
			DoLineCollision( &start, pTargetPos, collGroup);
		}

		INT32 autoFireCount = getAutoFireCount();

		autoFireCount++;
		setAutoFireCount( autoFireCount);
	}
}

bool WeaponShotGunLauncherWithLaser::isBeing(VEC2D* pVec)
{
	bool bRet = false;
	INT32 i = 0;

	for( i=0; i<3; i++)
	{
		if( (i3Vector::GetX( &m_vecRandPos[i]) == i3Vector::GetX( pVec)) &&	(i3Vector::GetY( &m_vecRandPos[i]) == i3Vector::GetY( pVec)) )
		{
			bRet = true;
		}
	}

	return bRet;
}

bool WeaponShotGunLauncherWithLaser::isBeing(REAL32 fVal1, REAL32 fVal2)
{
	bool bRet = false;
	INT32 i = 0;

	for( i=0; i<3; i++)
	{
		if( (i3Vector::GetX( &m_vecRandPos[i]) == fVal1) &&	(i3Vector::GetY( &m_vecRandPos[i]) == fVal2) )
		{
			bRet = true;
		}
	}

	return bRet;
}

void WeaponShotGunLauncherWithLaser::OnProcessExtension( bool bVal)
{
	// LSS�� ��� ������ ���� ������ �����ϹǷ�, LSS�� Extension Ȱ��ȭ�� ������ ���Ǵ� m_pWeaponInfo�� �����ϱ� ���� m_pExtWeaponInfo ������ ��ü�ȴ�.	
	if( m_pExtWeaponInfo != nullptr && (GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER || GetCurrentExtensionType() == WEAPON::EXT_ZOOM))
	{
		if( bVal)
			m_pWeaponInfo = m_pExtWeaponInfo;		//	Extension ���Ⱑ �ֹ��� ������ �ȴ�.
		else
			m_pWeaponInfo = m_pAbsoluteWeaponInfo;	//	���� �ֹ��� ������ �ٽ� ��ȯ
	}
}


bool WeaponShotGunLauncherWithLaser::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	m_pDotLazer->OnLostDevice(bLostDevice);
	return true;
}

bool WeaponShotGunLauncherWithLaser::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	m_pDotLazer->OnRevive(pCtx);
	return true;
}

void WeaponShotGunLauncherWithLaser::ReturnInstance( void)
{
	//	��� ���� (�������� ���ӳ�忡�� ���ŵȴ�.)
	WeaponShotGunLauncher::ReturnInstance();

	m_pDotLazer->ReturnInstance();
}
