#include "pch.h"
#include "WeaponGrenadeLauncherWithLaser.h"
#include "WeaponGrenadeLauncher.h"
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
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( WeaponGrenadeLauncherWithLaser);//, WeaponGrenadeLauncher);

namespace
{
	bool Is1PVObserver(const CGameCharaBase* pOwner)
	{
		if (!pOwner) return false;

		return pOwner->isCharaStateMask(CHARA_STATE_OBSERVER) && pOwner->is1PV() ? true : false;
	}

}

#define LAZER_SIGHT_THIKNESS	0.5f

WeaponGrenadeLauncherWithLaser::WeaponGrenadeLauncherWithLaser(void)
{
	m_fElapsedTime = 0.0f;
	m_fLocalTime = 10.0f;
	
	m_bReloaded = false;

	m_fRandX = 0.0f;
	m_fRandY = 0.0f;
	m_bCalcFirst = false;
	m_fCalcTime = 0.0f;

	m_pDotLazer = CDotLazer::new_object();

	INT32 i;

	for( i = 0; i < 3; i++)
		i3Vector::Zero( &m_vecRandPos[ i]);

	for( i = 0; i < 2; i++)
		i3Vector::Zero( &m_vecRandSub[ i]);

	for( i = 0; i < 2; i++)
		i3Vector::Zero( &m_vecRandTangent[ i]);
}

WeaponGrenadeLauncherWithLaser::~WeaponGrenadeLauncherWithLaser(void)
{
	I3_SAFE_RELEASE(m_pDotLazer);
}

void WeaponGrenadeLauncherWithLaser::_CreateLazerDot(void)
{
	if( m_pOwner != nullptr)
	{
		if (m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			m_pDotLazer->CreateLazerDot();
	}	
}

void WeaponGrenadeLauncherWithLaser::_UpdateLazerDot( bool bEnableByOption)
{
	if( m_pOwner != nullptr)
	{
		if( GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER && isExtensionActivate())
		{	//	��ź�߻� ����̰� �߻�Ⱑ Ȱ��ȭ �Ǿ� ������ ����.
			m_pDotLazer->DotStampDisable();
		}
		else if ((m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
			&& m_pOwner->GetCurrentCharaWeapon() == this &&
			(bEnableByOption || isWeaponStateMask(WSM_ZOOM)))
		{
			// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.
			GetShotTargetPos( m_TargetPos);

			m_pDotLazer->UpdateDotStamp();
		}		
		else
		{
			m_pDotLazer->DotStampDisable();
		}
	}
}

void WeaponGrenadeLauncherWithLaser::DotStampDisable()
{
	if( m_pOwner != nullptr)
	{
		if( m_pOwner->isLocal() && m_pOwner->GetCurrentCharaWeapon() == this)
		{
			m_pDotLazer->DotStampDisable();
		}
	}	
}

void WeaponGrenadeLauncherWithLaser::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	WeaponGrenadeLauncher::Create( pOwner, Itemid, bCreateInWeapon);

	m_pDotLazer->CreateLazerSight(getSceneObject());

	if( m_pOwner != nullptr)
	{
		if( m_pOwner->isLocal()|| Is1PVObserver(m_pOwner))
		{
			m_pDotLazer->CreateLazerDot();
		}
	}
}

void WeaponGrenadeLauncherWithLaser::Reset( void)
{
	WeaponGrenadeLauncher::Reset();

	m_fElapsedTime = 0.0f;
	m_fLocalTime = 10.0f;

	m_fRandX = 0.0f;
	m_fRandY = 0.0f;
	m_bCalcFirst = false;
	m_fCalcTime = 0.0f;	

	if( m_pOwner != nullptr)
	{
		if( (m_pOwner->isLocal() && m_pOwner->GetCurrentCharaWeapon() == this)
			|| Is1PVObserver(m_pOwner))
		{
			m_pDotLazer->DotStampDisable();
			m_pDotLazer->CreateLazerDot();
		}
	}
}

void WeaponGrenadeLauncherWithLaser::OnBindResource( void)
{
	WeaponGrenadeLauncher::OnBindResource();
}

void WeaponGrenadeLauncherWithLaser::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponGrenadeLauncher::OnIdle( pCtx, tm);
}

void WeaponGrenadeLauncherWithLaser::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponGrenadeLauncher::OnFire( pCtx, tm);
}

bool WeaponGrenadeLauncherWithLaser::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bRet = WeaponGrenadeLauncher::OnLoadMag( pCtx, tm);

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

void WeaponGrenadeLauncherWithLaser::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	WeaponGrenadeLauncher::OnEndLoadMag( pCtx, tm);

	m_bReloaded = false;
}

void WeaponGrenadeLauncherWithLaser::OnUpdate( REAL32 rDeltaSeconds)
{	
	m_fElapsedTime = rDeltaSeconds;

	WeaponGrenadeLauncher::OnUpdate( rDeltaSeconds);	
	
	if( m_pOwner != nullptr)
	{
		static const bool	bEnableLaserByOption = true;		// Ȯ���ϰ� �صα� ���� static const�� �ھƵӴϴ�..16.08.01.����

		if( m_pOwner->isLocal())
		{
			if( m_pOwner->GetCurrentCharaWeapon() == this)
			{
				// bool bEnableLaserByOption = (g_pEnvSet->m_bEnableLaserSight==true);
				// ������ ������ ����Ʈ off �Ұ����� ����� �׻� �ѵд�. (��. Ŀ���� M4)
				//if( isPossibleTurnOffWithDotLaser() == false)	// �ּ�ó��. WeaponGrenadeLauncherWithLaser ������ �׻� �������� ���� �־�� �մϴ�. 140507 Gyungju.kim
				//	bEnableLaserByOption = true;

				if (MyRoomInfoContext::i()->getStage()->IsHudOff())
				{
					// ������ ����Ʈ ǥ���ϱ�
					if (isWeaponStateMask(WSM_ZOOM))
					{
						if (_isInvisibleLazerScopeType() == true)
						{
							m_pDotLazer->SetEnableDotLaser(false);
						}
						else
						{
							m_pDotLazer->SetEnableDotLaser(true);
							_UpdateLazerDot(bEnableLaserByOption);
						}

					}
					else
					{
						m_pDotLazer->SetEnableDotLaser(false);
					}
				}
				else
				{
					// bEnableLaserByOption�� ���� Ŀ����Ƽ������ ���� ���� ���Ǻб�� ������ �ּ�ó���մϴ�.  (16.08.01.����)
					// ������ ������ ����Ʈ off �Ұ����� ����� �׻� �ѵд�. (��. Ŀ���� M4)
//					if (isPossibleTurnOffWithDotLaser() == false)
//						bEnableLaserByOption = true;

					// ������ ����Ʈ ǥ���ϱ�
//					if (bEnableLaserByOption || isWeaponStateMask(WSM_ZOOM))
					{
						if (_isInvisibleLazerScopeType() == true)
						{
							m_pDotLazer->SetEnableDotLaser(false);
						}
						else
						{
							m_pDotLazer->SetEnableDotLaser(true);
							_UpdateLazerDot(bEnableLaserByOption);
						}
					}
//					else
//					{
//						m_pDotLazer->SetEnableDotLaser(false);
//					}
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
						if (_isInvisibleLazerScopeType() == true)
						{
							m_pDotLazer->SetEnableDotLaser(false);
						}
						else
						{
							m_pDotLazer->SetEnableDotLaser(true);
							_UpdateLazerDot(true);
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

void WeaponGrenadeLauncherWithLaser::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponGrenadeLauncher::AttachNode( pSceneNode, pOwner);

	_CreateLazerDot();
}

void WeaponGrenadeLauncherWithLaser::DetachNode( void )
{
	WeaponGrenadeLauncher::DetachNode();

	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponGrenadeLauncherWithLaser::OnDrop( void)
{
	WeaponGrenadeLauncher::OnDrop();

	m_pDotLazer->SetEnableDotLaser(false);
}

void WeaponGrenadeLauncherWithLaser::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart)
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

void WeaponGrenadeLauncherWithLaser::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	if( m_pOwner == nullptr)
		return;

	if( m_pOwner->isLocal() && m_pOwner->GetCurrentCharaWeapon() == this)
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
	else
	{
		WeaponGrenadeLauncher::FireCollision( pStartPos, pTargetPos);
	}
}

bool WeaponGrenadeLauncherWithLaser::isBeing(VEC2D* pVec)
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

bool WeaponGrenadeLauncherWithLaser::isBeing(REAL32 fVal1, REAL32 fVal2)
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

bool WeaponGrenadeLauncherWithLaser::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	m_pDotLazer->OnLostDevice(bLostDevice);
	return true;
}

bool WeaponGrenadeLauncherWithLaser::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	m_pDotLazer->OnRevive(pCtx);
	return true;
}

void WeaponGrenadeLauncherWithLaser::ReturnInstance( void)
{
	//	��� ���� (�������� ���ӳ�忡�� ���ŵȴ�.)
	WeaponGrenadeLauncher::ReturnInstance();

	m_pDotLazer->ReturnInstance();
}

// scope �߿� lazer sight �� ������ �ϴ� �͵� �߰� �ʿ��մϴ�.
bool WeaponGrenadeLauncherWithLaser::_isInvisibleLazerScopeType()
{
	if (isWeaponStateMask(WSM_ZOOM) == false) return false;

	CWeaponInfo * pWeaponInfo = GetCurrentWeaponInfo();
	I3ASSERT(pWeaponInfo != nullptr);

	const SUB_SCOPE_INFO* subscopeinfo = g_pWeaponInfoDataBase->GetSubScopeInfo(pWeaponInfo->GetScopeSubType());
	bool invisible = false;
	if (subscopeinfo != nullptr)
	{
		invisible = subscopeinfo->m_bOffDotLaser;
	}

	return invisible;
}