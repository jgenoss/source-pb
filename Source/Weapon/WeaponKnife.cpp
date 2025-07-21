#include "pch.h"
#include "WeaponKnife.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaWeaponContext.h"
#include "WeaponItemID.h"
#include "MainWeapon_Knife.h"
#include "WeaponFunction.h"
#include "WeaponInfo/MeleeWeaponInfo.h"

#define WEAPON_KNIFE_SEC_ATTACK_TIME	0.31f

I3_CLASS_INSTANCE( WeaponKnife);//, WeaponBase);

WeaponKnife::WeaponKnife( void)
{

}

WeaponKnife::~WeaponKnife( void)
{
	if((getOwner() != nullptr) && (getOwner()->isLocal()))
	{
		// 1��Ī�� Į�� ������ �־��ٸ� ������ ���� ������ �ؾ���.
//		g_pEdgeTrail->Reset();
	}
}


MainWeapon*	WeaponKnife::CreateMainWeapon() 
{
	MainWeapon_Knife * p = new MainWeapon_Knife(this);	
	MEMDUMP_NEW( p, sizeof( MainWeapon_Knife));
	return p;
}

void WeaponKnife::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}

void WeaponKnife::Reset( void)
{
	WeaponBase::Reset();	// revision 54702 ������ ���� �� Knife �� ��� �� Reset (hansoft.4544)
	ReturnToReady();
}

void WeaponKnife::ReturnToReady( void)
{
	WeaponBase::ReturnToReady();

	m_StateMaskWeapon	= WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE;

	setAutoFireCount( 0);

}

void WeaponKnife::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/)
{
	// �ӽ� ����...(���߿� �� ��ü �ؾ��Ұ��̴�..)
	// �����Լ� ������ �ƴ� ��ü �Լ� ȣ���� ���� �������� ����..
	((MainWeapon_Knife*)m_mainWeapon.get())->MainWeapon_Knife::OnPlayChangeAni(rTimeScale);
	//

/*
	if( (m_pPairWeapon != nullptr) && (m_aiiChangeLeft != -1) )
	{// paired weapon�̸� �޼����̿� �ִϸ��̼��� �Ѵ�.
		if( g_pFramework->IsBattleStage() )
			_PlayWeaponAnim( m_aiiChangeLeft , rTimeScale );
	}
	else
	{
		WeaponBase::OnPlayChangeAniImpl( rTimeScale);
	}
*/
}

bool WeaponKnife::Fire( WEAPON::FIRE_ORDER type)
{
	m_FireOrder = type;

	// ������ ���ϰ� �ִ� ��ǥ ������ ���Ѵ�.

	//	Head�� IK �߰� ������ ����
	NET_Fire( nullptr, nullptr, m_FireOrder );

	addWeaponStateMask( WSM_WAIT_AUTOFIREDELAY | WSM_NEED_SEND_FIREACTION);

	setAutoFireCount( 0);

	return true;
}

void WeaponKnife::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{	//	���� Local, ��ũ��ũ ������� TargetPos�� �����մϴ�. ��Ʈ��ũ�� ��� �ø��� ������ �����ؾ� �մϴ�.

	setWeaponState( WEAPON::STATE_FIRE);		//	����!!
	m_FireOrder = type;

	setAutoFireCount( 0);

	if( m_pPairWeapon == nullptr)
	{	// �Ϲ����� ������ �Դϴ�.
		WeaponBase::NET_Fire( nullptr, nullptr, type );
	}
	else
	{
		// KEYWORD : WEAPON_CIC
		// CIC�� �������� ȣ������� �մϴ�.
		//	CIC �ѱ� �߰��Ǹ� �ݵ�� �ε��� �߰� �ʿ��մϴ�.
		if( m_pPairWeapon->getWeaponInfo()->isGun() 
			|| i3::kind_of<WeaponMiniAxe*>(m_pPairWeapon) )	// CIC�� �޼��� �и� �������� �ݵ�� �ǽ�����̴�
			WeaponBase::NET_Fire( nullptr, nullptr, type );
	}

	if( pStartPos != nullptr )
	{
		i3Vector::Copy( m_StartPos, pStartPos );
		m_bSetStart	= true;
	}
	else
	{
		m_bSetStart	= false;
	}
	
	if( pTargetPos != nullptr )
		i3Vector::Copy( m_TargetPos, pTargetPos);

	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()) )
		addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
	removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);

	if( ((CMeleeWeaponInfo*)m_pWeaponInfo)->isDelayedFireSound() == false)
	{
		_OnPlayFireSound();
	}

	MainWeapon_Knife* mainWeapon = static_cast<MainWeapon_Knife*>(m_mainWeapon.get());

	if( type == WEAPON::PRIMARY_FIRE)
	{
		// KEYWORD : WEAPON_DUALKNIFE
		if( (m_pPairWeapon != nullptr) && (mainWeapon->GetAttackA_Left() != -1)
			&& (i3::kind_of<WeaponMiniAxe*>(m_pPairWeapon) == false) )
			WF::PlayWeaponAnim(this, mainWeapon->GetAttackA_Left() );
	}
	else if( type == WEAPON::SECONDARY_FIRE)
	{
		// KEYWORD : WEAPON_DUALKNIFE
		if( (m_pPairWeapon != nullptr) && (mainWeapon->GetAttackB_Left() != -1)
			&& (i3::kind_of<WeaponMiniAxe*>(m_pPairWeapon) == false) )
			WF::PlayWeaponAnim(this, mainWeapon->GetAttackB_Left() );
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_SECFIRE));
	}
}

void WeaponKnife::FireEnd(void)
{	
}

void WeaponKnife::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos )
{	
	INT32 collGroup;

	if( getOwner() == nullptr )		return;
	
	collGroup = CGRP_MASKGROUP_ATTACK;
	if( getOwner()->getCharaTeam() & CHARACTER_TEAM_RED)	collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	else													collGroup = CGRP_MASKGROUP_CHARA_RED;
		
	if( pStartPos == nullptr )
	{
		if( m_pOwner->is1PV())
		{
			// 1��Ī������ ����� Camera�� ����
			m_pOwner->getCameraContext()->getVirtualCamPosition( m_StartPos );
		}
		else
		{
			//	3��Ī������ ĳ���� ��ǥ(��Ȯ�ϰԴ� ĳ������ IK Bone)�� ��ġ�� ������������ ī�޶� ��������
			// ��ŸƮ ������
			i3Vector::Sub( m_StartPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getHeadMatrix()), i3Matrix::GetPos( m_pOwner->getBoneContext()->getIKMatrix()));
			i3Vector::Scale( m_StartPos, m_StartPos, 0.5f);
			i3Vector::Sub( m_StartPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getHeadMatrix()), m_StartPos);
		}
	}
	else
	{
		i3Vector::Copy( m_StartPos, pStartPos );
	}

	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
	{
		GetShotTargetPos( m_TargetPos, m_StartPos);
	}
	else
	{
		i3Vector::Copy( m_TargetPos, pTargetPos );
	}

	// Network Random Sync
	// KEYWORD : WEAPON_CIC
	if( i3::same_of<WeaponDualCIC*>(getPairWeapon()) )
	{	// CIC�� ������ �����̸� ������ �����Ͱ� �ƴ� CIC������ ��Ʈ��ũ�� �����ϴ�.
		getPairWeapon()->NET_FireSync( m_TargetPos, WEAPON::SECONDARY_FIRE);
	}
	else
	{
		NET_FireSync( m_TargetPos, getFireOrder() );
	}
	
	/*GlobalFunc::PB_TRACE("m_StartPos %f %f %f m_TargetPos %f %f %f GetPos %f %f %f", m_StartPos.x, m_StartPos.y, m_StartPos.z, m_TargetPos.x, m_TargetPos.y, m_TargetPos.z,
												m_pOwner->GetPos()->x, m_pOwner->GetPos()->y, m_pOwner->GetPos()->z);*/
	//	collision line
	{	
		i3CollideeLine col;

		col.SetStart( m_StartPos);
		col.SetEnd( m_TargetPos );
		
		DoFireCollision( &col, collGroup);
	}
}

bool WeaponKnife::DoFireCollision( i3CollideeLine * pCol, INT32 idxGroup)
{
	bool	bWarningOwner = false;

	I3_PHYSIX_HIT_RESULT * pLastHitResult;

	pLastHitResult = _DoFireCollisionKnife( pCol, idxGroup, &bWarningOwner);
	if( pLastHitResult == nullptr )
	{
		// �ƹ����� �浹���� �ʾҴ�.
		return false;
	}

	if (m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex( WEAPON::CANDYCANE))
	{
		MainWeapon_Knife* mainWeapon = static_cast<MainWeapon_Knife*>(m_mainWeapon.get());

		// revision 47817 ĵ������ ����Ʈ ������ ��ġ ���� -> Ÿ�� �������� ���� - hansoft.3362
		MATRIX mtx;
		
		i3Matrix::SetUp(&mtx, &pLastHitResult->m_Normal);
		i3Matrix::SetPos(&mtx, &pLastHitResult->m_Intersect);

		_FireEffect( &mtx, mainWeapon->GetCandyEffectIdx(), true);
	}

	I3ASSERT( m_pOwner != nullptr);

	//GlobalFunc::PB_TRACE("DoFireCollision");

	if( pLastHitResult->m_pDestShapeSet != nullptr)
	{
		if( (m_FireOrder == WEAPON::PRIMARY_FIRE) &&
			((CMeleeWeaponInfo*)m_pWeaponInfo)->UseHitedAnimation() )
		{
			m_pOwner->Cmd_Attack( WEAPON::HITED_FIRE);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void WeaponKnife::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	setAutoFireCount( 0);

	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
	{
		addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		removeWeaponStateMask( WSM_FIRE_DISABLE);
	}
}

void WeaponKnife::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
	{
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_FIRING | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	}

	REAL32 fAttackTiming = 0.f;

	if( m_pPairWeapon != nullptr)
	{
		// KEYWORD : WEAPON_DUALKNIFE
		// ��� �������� ���� �������� WeaponDualKnife���� �ø����� �մϴ�.
		if( i3::same_of<WeaponDualKnife*>(m_pPairWeapon))
		{
			if( pCtx->getLocalTime() + tm > pCtx->getCurrentAIState()->getActualDuration())
				ReturnToReady();

			return;
		}
	}

	{
		//	���� ��Ŀ� ���� �浹�ð��� ���Ѵ�.
		MainWeapon_Knife* mainWeapon = static_cast<MainWeapon_Knife*>(m_mainWeapon.get());

		WEAPON_KNIFE_ATTACK_TYPE attack_type = mainWeapon->GetKnifeAttackType();

		switch( attack_type)
		{
		case WEAPON_KNIFE_ATTACK:		fAttackTiming = m_pWeaponInfo->GetAttackTimeA(0);		break;
		case WEAPON_KNIFE_ATTACK2:		fAttackTiming = m_pWeaponInfo->GetAttackTimeA(1);		break;	
		default:	I3PRINTLOG(I3LOG_FATAL,  "AttackType is invalid.");
		}
	}
	
	if( ((CMeleeWeaponInfo*)m_pWeaponInfo)->isDelayedFireSound())
	{
		if( !isWeaponStateMask( WSM_PLAYED_FIRE_SOUND) && pCtx->getLocalTime() + tm > fAttackTiming - 0.1f )
		{
			_OnPlayFireSound();
			addWeaponStateMask( WSM_PLAYED_FIRE_SOUND);
		}
	}

	INT32 autoFireCount = getAutoFireCount();

	//	���� ���ۺ��� Ÿ�� �ð��� �Ǹ� ���������� �ѹ� �ø��� �Ѵ�.
	if( autoFireCount < m_pWeaponInfo->GetAutoFireCount() && pCtx->getLocalTime() > fAttackTiming)
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
		{
			FireCollision( nullptr, nullptr );
		}
		autoFireCount++;
	}
	
	
	setAutoFireCount( autoFireCount);

	// ���� �����ð��� ����� �����ٸ�...Ready ���·�
	//if( pCtx->getLocalTime() + tm > (m_pWeaponInfo->GetFireDelay()))
	REAL32 rDelay = pCtx->getLocalTime() + tm;
	if( rDelay > GetFireDelay())
	{
		_OnProcessFireEndAnim( rDelay);
	}
}

void WeaponKnife::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm)
{
	INT32 autoFireCount = getAutoFireCount();
	REAL32 fAttackTiming = m_pWeaponInfo->GetAttackTimeB( autoFireCount);

	if( ((CMeleeWeaponInfo*)m_pWeaponInfo)->isDelayedFireSound() )
	{
		if( !isWeaponStateMask(WSM_PLAYED_FIRE_SOUND) && pCtx->getLocalTime() + tm > fAttackTiming - 0.2f )
		{
			_OnPlayFireSound();
			addWeaponStateMask( WSM_PLAYED_FIRE_SOUND);
		}
	}

	//	���� ���ۺ��� Ÿ�� �ð��� �Ǹ� ���������� �ѹ� �ø��� �Ѵ�.
	if( autoFireCount < m_pWeaponInfo->GetAutoFireCount2() && (pCtx->getLocalTime() + tm > fAttackTiming) )
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))// ��Ʈ��ũ�� ��Ŷ���� ó���մϴ�.
		{
			FireCollision( nullptr, nullptr );
		}

		autoFireCount++;
	}

	setAutoFireCount( autoFireCount);

	// ���� �����ð��� ����� �����ٸ�...Ready ���·�	
	if( pCtx->getLocalTime() >= (GetFireDelay2() - tm))
	{
		ReturnToReady();
	}
}

void WeaponKnife::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{	//	�ִϸ��̼��� ���� ������ �������� ȣ������� �մϴ�. ĳ���Ϳ��� Fire ����� �ڵ����� ȣ���մϴ�.
	if( getWeaponState() == WEAPON::STATE_FIRE)
		return;

	ReturnToReady();
}

void WeaponKnife::OnUI_ShowWeapon( i3AIContext * pCtx, REAL32 tm)
{
	REAL32 localTime = pCtx->getLocalTime();
	
	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);

	if( localTime + tm > pAIState->getDuration())
	{
		_OnPlayWeaponIdleAnim();
		setWeaponState( WEAPON::STATE_READY);
	}
}


void WeaponKnife::OnDrop()
{
	 I3ASSERT_0;
}

void WeaponKnife::PlayAnim_UI_IdleA( void)
{
	if( m_pOwner != nullptr)
	{
		((MainWeapon_Knife*)m_mainWeapon.get())->PlayAnim_UI_Swat_Common();
	}
}

void WeaponKnife::PlayAnim_UI_IdleB( void)
{
	if( m_pOwner != nullptr)
	{
		((MainWeapon_Knife*)m_mainWeapon.get())->PlayAnim_UI_Swat_Common();
	}
}


void WeaponKnife::_OnProcessFireEndAnim( REAL32 rDelay)
{	
	if( (m_pOwner != nullptr) && (m_pOwner->getAnimContext()->getAIUpper() != nullptr) )
	{ 
		i3AIState * pAIState = m_pOwner->getAnimContext()->getAIUpper()->getCurrentAIState();
		if( pAIState != nullptr )
		{
			if( rDelay > pAIState->getActualDuration() )
				ReturnToReady();
			else
				setWeaponState( WEAPON::STATE_FIRING);

			return;
		}
	}
	
	ReturnToReady();
}
