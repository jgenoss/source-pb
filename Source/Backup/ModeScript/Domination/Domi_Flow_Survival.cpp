#include "pch.h"
#include "Domi_Flow_Survival.h"

#include "../../Source/ui/hud/UIHUD_Domination_Manager.h"

CDomi_Flow_Survival::CDomi_Flow_Survival( gmode_domination* pmode) : CDominationFlowBase( pmode)
{
}

CDomi_Flow_Survival::~CDomi_Flow_Survival(void)
{
}

void CDomi_Flow_Survival::ProcessModeData( INT32 nIndex, INT32 nData)
{
	//if( nData > 999) nData = 999;

	//switch( nIndex)
	//{
	//case 110:
	//	m_iKillCount = 0;		//�ʱ�ȭ.
	//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData);
	//	break;

	//case 111:
	//	m_iKillCount = nData;	//�� kill Count �Դϴ�.
	//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, m_iKillCount);
	//	break;
	//}
}