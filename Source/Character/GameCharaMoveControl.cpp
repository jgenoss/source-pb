#include "pch.h"
#include "GameCharaMoveControl.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"
#include "HackDefense.h"
#include "MyRoomInfoContext.h"


#define	PHYSIX_FLOORCHECK_TIME			0.2f	// �ٴڸ��� ���� �˻�( �ʴ� 5ȸ )
#define NET_PHYSIX_FLOORCHECK_TIME		0.25f

I3_CLASS_INSTANCE( CGameCharaMoveControl);

void CGameCharaMoveControl::OnUpdate( REAL32 rDeltatime)
{
	if (getActionContext() && getActionContext()->isLower(CHARA_LOWER_JUMP))
		return;

	OnProcessFloorCheck( rDeltatime);
}

void CGameCharaMoveControl::OnChangeHeight( REAL32 rHeight)
{
	if( !m_pOwner->isNetwork())
		getMoveContext()->ChangeControllerHeight( rHeight);
}

void CGameCharaMoveControl::OnProcessFloorCheck( REAL32 rDeltatime)
{
	REAL32 rElapsed = getMoveContext()->getFloorCheckTimer();
	rElapsed += rDeltatime;
	getMoveContext()->setFloorCheckTimer( rElapsed);

	REAL32 rCheckTime = PHYSIX_FLOORCHECK_TIME;

	if( m_pOwner->isNetwork())
		rCheckTime = NET_PHYSIX_FLOORCHECK_TIME;

	if( rElapsed > rCheckTime )
	{	
		CheckFloorTerrain();
	}
}

void CGameCharaMoveControl::CheckFloorTerrain( void)
{
	VEC3D vPos;
	i3CollideeLine ColLine;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		return;

	i3Vector::Copy( &vPos, m_pOwner->GetPos());
	i3Vector::AddY( &vPos, 0.5f);
	ColLine.SetStart( &vPos);

	i3Vector::AddY( &vPos, - 1.0f);
	i3Vector::AddX( &vPos, 0.01f);
	ColLine.SetEnd( &vPos);

	getMoveContext()->setStandOnMovingObject( false);

//	if( LocaleValue::Enable( "CheckMapCollision" ) )
	{
		hd::check_UnknwonPos::i()->check(&ColLine);
	}



	// ���ڱ� ���带 ���� Raycasting...
	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK );
	if((pResult != nullptr) && (pResult->m_pTerrain != nullptr))
	{
		getCollisionContext()->setFloorHitResult( pResult);

		getMoveContext()->setFloorTerrainType( (I3_TERRAIN_TYPE)pResult->m_pTerrain->m_Terrain);
		getMoveContext()->setFloorLength( pResult->m_T);

		if( m_pOwner->isLocal() || m_pOwner->isAI())
		{
			// ������Ʈ
			i3EventReceiver * pReceiver = pResult->m_pDestShapeSet->getEventReceiver();

			if( i3::kind_of<i3Object*>(pReceiver))
			{
				if( getMoveContext()->getRideObject() == pReceiver)
				{
					// ������ �ؼ� ���߿� �� ���� �ϴ���, Object�� �ö� Ÿ �ִ� �����
					// CountTime�� ������Ű�� �ʾƾ� �Ѵ�.
					getMoveContext()->setStandOnMovingObject( true);
				}
			}
		}
	}
	else
	{
		getMoveContext()->setFloorLength( 2.f);
	}

	getMoveContext()->setFloorCheckTimer( 0.f);
}

