#include "pch.h"
#include "GameCharaMoveControlLocal.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

#define	PHYSIX_FLOORCHECK_TIME			0.2f	// �ٴڸ��� ���� �˻�( �ʴ� 5ȸ )

I3_CLASS_INSTANCE( CGameCharaMoveControlLocal, CGameCharaMoveControl);

CGameCharaMoveControlLocal::CGameCharaMoveControlLocal()
{
}

CGameCharaMoveControlLocal::~CGameCharaMoveControlLocal()
{
}

void CGameCharaMoveControlLocal::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaMoveControl::OnUpdate( rDeltatime);
}

//void CGameCharaMoveControlLocal::OnProcessWorldCollision( REAL32 rDeltatime)
//{
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) ) return;
//
//	m_pOwner->getMoveContext()->CheckWorldCollision( rDeltatime);
//}

//void CGameCharaMoveControlLocal::OnChangeHeight( REAL32 rHeight)
//{
//	m_pOwner->getMoveContext()->ChangeControllerHeight( rHeight);
//}

//void CGameCharaMoveControlLocal::OnProcessFloorCheck( REAL32 rDeltatime)
//{
//	CGameCharaMoveControl::OnProcessFloorCheck( rDeltatime);
//
//	if( m_pOwner->getActionContext()->isLower( CHARA_LOWER_JUMP ))
//		return;
//
//	REAL32 rElapsed = m_pOwner->getMoveContext()->getFloorCheckTimer();
//	rElapsed += rDeltatime;
//	m_pOwner->getMoveContext()->setFloorCheckTimer( rElapsed);
//
//	if( rElapsed > PHYSIX_FLOORCHECK_TIME )		// �ʴ� 4ȸ�� üũ�մϴ�.
//	{	
//		m_pOwner->getMoveContext()->CheckFloorTerrain();
//		m_pOwner->getMoveContext()->setFloorCheckTimer( 0.f);
//	}
//
//}


