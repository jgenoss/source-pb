#include "pch.h"
#include "GameCharaInputControlAI.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaCameraContext.h"
#include "BattleObjContext.h"

#define		THROW_DIST			27.0f

REAL32 CGameCharaInputControlAI::CalcRhoForThrow( const VEC3D * pTargetPos, VEC3D * pLookPos)
{
	VEC3D diff;
	REAL32 dist;
	REAL32 rho, h;

	i3Vector::Sub( &diff, pTargetPos, m_pOwner->GetPos());

	dist = i3Vector::Length( &diff) * 0.5f;

	if( dist > THROW_DIST)
	{
		// ��ǥ �Ÿ��� �߰����� ������ �Ÿ��� ������ �ʴ´�.
		// �� ������ ������ �Ǵ��ϰ� ��ȯ.
		return 0.0f;
	}

	i3Vector::Normalize( pLookPos, &diff);
	i3Vector::Scale( pLookPos, pLookPos, dist);

	if( dist > I3_EPS)
	{
		rho = i3Math::acos( dist / THROW_DIST);

		//I3TRACE( "THROW : %.2fm  %f(degree)\n", dist, I3_RAD2DEG(rho));
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		I3ASSERT( i3Math::isValid( rho));
#endif

		if( (dist >= (THROW_DIST * 0.3333f)) && (rho < I3_DEG2RAD( 60.0f)))
		{
			h = i3Math::tan( rho) * dist;
		}
		else
		{
			//  ���� ���� �̻� ���� ������ �ϴ� ��쿡�� 
			// �׳� ������ ������.
			h = 0.0f;
		}

		pLookPos->y += h;
	}

	return dist;
}

bool CGameCharaInputControlAI::isWeaponExist( WEAPON_SLOT_TYPE type)
{	
	if( getWeaponContext() == nullptr)
		return false;

	WeaponBase * pWeapon = getWeaponContext()->getWeapon( type);
	if( pWeapon == nullptr)
		return false;

	// ������ ������ 0�̶�� �̹� ���� ����.
	if( pWeapon->getLoadedBulletCount() <= 0)
		return false;

	return true;
}

INT32 CGameCharaInputControlAI::IsThrowWasSafeZone(INT32 * pIdx)	//�ڽ��� ���� ����ź�� �ڽſ��� ������ ��ġ�� ���������� üũ
{
	INT32 i, cnt = 0;
	INT32 idx = m_pOwner->getCharaNetIndex();
	*pIdx = -1;

	// ������ ���� ����ź ���� ����
	for( i = 0; i < MAX_THROWWEAPON_SLOP; i++)
	{
		const GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->getGameInfo_ThrowWeapon( idx, i);
		I3ASSERT( pInfo != nullptr );

		if( pInfo->getWeaponBase() == nullptr)
			continue;

		WeaponBase * pWeapon = const_cast<WeaponBase*>(pInfo->getWeaponBase());
		if( pWeapon->isEnable())
		{
			if( pWeapon->getWeaponInfo() == nullptr)
				continue;

			cnt++;

			switch(pWeapon->getWeaponInfo()->GetTypeClass())
			{
			case WEAPON_CLASS_THROWING_GRENADE:
			case WEAPON_CLASS_THROWING_CONTAIN:
			case WEAPON_CLASS_THROWING_ATTACK:
			case WEAPON_CLASS_THROWING_HEAL:
				{
					VEC3D vDiff;
					i3Vector::Sub( &vDiff, pWeapon->GetPos(), m_pOwner->GetPos());

					REAL32 length = i3Vector::Length( &vDiff);

					if( length < 8.0f)
					{
						*pIdx = i;
						m_bForceEvadeSelfThrow = true;
					}
				}
				break;
			}
		}
	}

	return cnt;
}


void CGameCharaInputControlAI::ProcessGrenadeForBusyPattern( REAL32 tm, WEAPON_SLOT_TYPE type)
{
	// ��ô ���Ⱑ ���� �� �ִ�.
	if( isWeaponExist( type) == false)
		return;

	m_timeGrenadeThrow += tm;
	if( m_timeGrenadeThrow < 0.8f)
		return;

	VEC3D	OwnerPos;
	INT32	prio;

	// ���� �������� ���Ѵ�.
	m_pOwner->getCameraContext()->getVirtualCamPosition( &OwnerPos);

	m_timeGrenadeThrow = 0.0f;

	if( type == WEAPON_SLOT_THROWING1)
		prio = 50;
	else
		prio = 70;

	// ���� ��ǥ ������ ã�´�.
	I3_POINTIDX idxTarget = m_pGamePath->FindClosestPointByPriority( &OwnerPos, prio);
	i3::pack::PATH_POINT_INFO * pPoint;
	VEC3D targetDir;

	if( idxTarget == -1)
		return;

	pPoint = m_pGamePath->getPoint( idxTarget);

	VEC3D pos(pPoint->m_Pos.x, pPoint->m_Pos.y, pPoint->m_Pos.z);
	REAL32 dist = CalcRhoForThrow( &pos, &targetDir);
	if( dist == 0.0f)
		return;

	//I3TRACE( "Look pos : %f, %f, %f\n", m_throwTarget.x, m_throwTarget.y, m_throwTarget.z);

	// Raycast�� �ؼ� �ش� �������� ���� �� �ִ����� Ȯ���Ѵ�.
	{
		I3_PHYSIX_HIT_RESULT * pResult;
		i3CollideeLine line;
		VEC3D targetPos;

		line.SetStart( m_pOwner->GetPos());

		i3Vector::Add( &targetPos, &targetDir, &OwnerPos);
		line.SetEnd( &targetPos);

		pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK, nullptr);

		if((pResult != nullptr) && (pResult->m_T < dist))
		{
			// ���� ��ֹ��� �ִ�. ������ �ʴ´�.
			//I3TRACE( "--��ֹ�\n");
			return;
		}
	}

	i3Vector::Copy( &m_vTargetDir, &targetDir);

	// ������.
	m_nAIState = AI_STATE_THROW;

	m_ThrowingWeapon = type;

	i3Vector::Copy( &m_vThrowPosition, m_pOwner->GetPos());
}

bool CGameCharaInputControlAI::ProcessGrenadeForLazyPattern( VEC3D * pLastPos)
{
	if( m_GrenadePattern != AI_GRENADE_LAZY)
		return false;

	// ����ź�� ���� �� �ִ�.
	if( isWeaponExist( WEAPON_SLOT_THROWING1) == false)
		return false;

	VEC3D targetDir, OwnerPos;

	REAL32 dist = CalcRhoForThrow( pLastPos, &targetDir);
	if( dist == 0.0f)
		return false;

	m_pOwner->getCameraContext()->getVirtualCamPosition( &OwnerPos);

	//I3TRACE( "Look pos : %f, %f, %f\n", m_throwTarget.x, m_throwTarget.y, m_throwTarget.z);

	// Raycast�� �ؼ� �ش� �������� ���� �� �ִ����� Ȯ���Ѵ�.
	{
		I3_PHYSIX_HIT_RESULT * pResult;
		i3CollideeLine line;
		VEC3D targetPos;

		line.SetStart( m_pOwner->GetPos());

		i3Vector::Add( &targetPos, &targetDir, &OwnerPos);
		line.SetEnd( &targetPos);

		pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK, nullptr);

		if((pResult != nullptr) && (pResult->m_T < dist))
		{
			return false;
		}
	}

	i3Vector::Copy( &m_vTargetDir, &targetDir);

	// ������.
	m_nAIState = AI_STATE_THROW;
	m_ThrowingWeapon = WEAPON_SLOT_THROWING1;

	i3Vector::Copy( &m_vThrowPosition, m_pOwner->GetPos());

	return true;
}

INT32 CGameCharaInputControlAI::GetClosestDirectionPoint( INT32 idxStart, VEC3D *pvDir)
{
	if( m_pGamePath == nullptr)
		return -1;

	INT32 i,idx;
	I3_NEXTPOINTINFO Next[10];
	REAL32 dot;
	REAL32 maxDot = 0.f;
	INT32 idxCnt = 0;
	VEC3D vDiff;

	INT32 cnt = m_pGamePath->GetLinkedPoint( idxStart, Next, 10);

	i3::pack::PATH_POINT_INFO * pCurPointInfo = m_pGamePath->getPoint( idxStart);

	idx = -1;
	for( i = 0;i < cnt; ++i)
	{
		i3::pack::PATH_POINT_INFO * pPointInfo = m_pGamePath->getPoint( Next[i].m_idxNext);

		i3Vector::Sub( &vDiff, &pPointInfo->m_Pos, &pCurPointInfo->m_Pos);
		i3Vector::Normalize( &vDiff, &vDiff);

		dot = i3Vector::Dot( &vDiff, pvDir);
		if( idxCnt == 0)
		{
			maxDot = dot;
			idx = Next[i].m_idxNext;
		}
		else
		{
			if( maxDot < dot)
			{
				maxDot = dot;
				idx = Next[i].m_idxNext;
			}
		}

		idxCnt++;
	}

	return idx;
}

void CGameCharaInputControlAI::OnThrow( REAL32 tm, bool bFirst)
{
	// ����
	if( bFirst)
	{
		m_nActionState &= ~AI_ACTION_MOVE;
		m_nMoveState = 0;

		{
			// ������ �� ����
	
			// ���� ��ü
			if( m_ThrowingWeapon == WEAPON_SLOT_THROWING1)
				m_nActionState |= AI_ACTION_SWAPWEAPON3;
			else
				m_nActionState |= AI_ACTION_SWAPWEAPON4;
		}
	}
	else if((m_rAIUpdateElapsedTime > 0.2f) && (m_rAIUpdateElapsedTime < 1.2f))
	{
		if( m_ThrowingWeapon == WEAPON_SLOT_THROWING1)
				m_nActionState &= ~AI_ACTION_SWAPWEAPON3;
			else
				m_nActionState &= ~AI_ACTION_SWAPWEAPON4;

		// �Ⱓ ���� ������ ��� ���ش�.
		// ����ź�� ��� �ִ� �Ͱ� ������ ȿ��.
		Attack( &m_vTargetDir, tm);		
	}
	else
	{
		// 1�� �ĺ��� ��ư�� ���� ����ź�� ������.
		m_nActionState &= ~(AI_ACTION_FIRE);
	}

	//
	if( m_rAIUpdateElapsedTime > 2.4f)
	{
		// ������ �������� �����ش�.
		m_vTargetDir.y = 0.0f;
		SeeTarget( &m_vTargetDir);

		// 1.2�ʰ� �����ٸ� ������ ������ �Ϸ��� ������ �����ϰ�
		// �̵� �������� ���ư���.
		m_nAIState = AI_STATE_MOVE;

		SelectWeapon();

		//����ź�� ���� ��쿣 ����ź�� �� �������� �̵� ������ ��´�.
		if( m_ThrowingWeapon == WEAPON_SLOT_THROWING1)
		{
			m_rAfterThrowTimer = 0.5f;

			VEC3D vInvDir;
			i3Vector::Scale( &vInvDir, &m_vTargetDir, -1.0f);
			i3Vector::Normalize( &vInvDir, &vInvDir);

			INT32 cur = GetClosestDirectionPoint( m_pOwner->GetPos(), &vInvDir);
			if( cur != -1)
			{
				INT32 idx = GetClosestDirectionPoint( cur, &vInvDir);
				if( idx != -1)
				{
					m_idxOldTargetPoint = m_idxCurTargetPoint;
					m_idxCurTargetPoint	= cur;

					m_WayPoint[0] = cur;
					m_WayPoint[1] = idx;

					m_CurWayPointIndex = 1;

					m_WayPointCount = 2;

					m_rElapsedTraceTime = 0.0f;
				}
			}
		}
	}
}
