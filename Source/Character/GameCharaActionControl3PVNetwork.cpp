#include "pch.h"
#include "GameCharaActionControl3PVNetwork.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaActionControl3PVNetwork, CGameCharaActionControl3PV);

CGameCharaActionControl3PVNetwork::CGameCharaActionControl3PVNetwork()
{
}

CGameCharaActionControl3PVNetwork::~CGameCharaActionControl3PVNetwork()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Action

//bool CGameCharaActionControl3PVNetwork::OnActionStandup( void)
//{
//	return _OnCmdActionStandUpForNetwork();
//}

//bool CGameCharaActionControl3PVNetwork::OnActionJumpStart( void)
//{
//	/// Death ���¿��� Jump packet�� �޴´ٸ�?? ������ ���� �� �ִ�. 2010.08.25 ����
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )	return false;
//	
//	m_pOwner->addCharaStateMask( CHARA_STATE_JUMP);
//	getMoveContext()->setElapsedTakeOffTime( 0.f);
//	OnCmdAnimPlayJump();
//	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTBT_JUMP_START, m_pOwner->GetPos(), m_pOwner->is1PV());
//
//	//I3TRACE( "Jump....\n");
//
//	return true;
//}


