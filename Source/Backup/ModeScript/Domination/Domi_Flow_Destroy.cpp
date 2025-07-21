#include "pch.h"
#include "Domi_Flow_Destroy.h"

#include "./GameMode_Domination.h"
#include "../../Source/ui/hud/UIHUD_Domination_Manager.h"

CDomi_Flow_Destroy::CDomi_Flow_Destroy( gmode_domination* pmode) : CDominationFlowBase( pmode)
{
}

CDomi_Flow_Destroy::~CDomi_Flow_Destroy(void)
{
}

void CDomi_Flow_Destroy::ProcessModeData( INT32 nIndex, INT32 nData)
{
	//switch( nIndex)
	//{
	//case 113:	//�ı� �̼ǿ��� ��� �ϴ� total ������Ʈ ��
	//	m_pmode->RegisterWaveMissionObjects( nIndex + 1, nData);
	//	m_pDestroyObj = NULL;
	//	break;

	//case 110:
	//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_MAX_COUNT, nData);
	//	break;

	//case 111: // �ı����� ������Ʈ �ε���
	//	{
	//		if( m_pDestroyObj != NULL)
	//			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_DELETE_STAGE_OBJECT, m_pDestroyObj->getNetIndex());

	//		m_pmode->SetCurrentMissionObjectIndex( nData);		//�ѹ��� �ϳ��� ������Ʈ��
	//		m_pDestroyObj = m_pmode->GetCurrentMissionObject();

	//		I3ASSERT( m_pDestroyObj != NULL);
	//		if( m_pDestroyObj != NULL)
	//		{
	//			m_pDestroyObj->SetEnable( TRUE);
	//			m_pDestroyObj->SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC);
	//			I3TRACE("  -- Recv Destroy Object Net_Index(%d) Name(%s)\n", m_pDestroyObj->getNetIndex(), m_pDestroyObj->GetName());
	//		}
	//		else
	//		{
	//			I3TRACE("  -- It is invalid Destroy object Idx(%d)\n", nData);
	//			return;
	//		}

	//		UIHUDDominationBase* pScoreBoard = UIHUDDominationMgr::i()->GetHUD(HUD_DMN_SCOREBOARD);
	//		PushScoreBoardEvent* pEvent = new PushScoreBoardEvent( m_pmode);
	//		pEvent->SetEvent( PushScoreBoardEvent::HUD, HUD_DMN_SCOREBOARD, HUD_DMN_EVT_SCOREBOARD_OBJECT_HP_ENABLE, m_pDestroyObj->getNetIndex());

	//		bool bSucess = pScoreBoard->PushEvent( pEvent);

	//		if( bSucess == false)
	//		{
	//			delete pEvent;
	//			UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_SCOREBOARD_OBJECT_HP_ENABLE, m_pDestroyObj->getNetIndex());
	//		}

	//		UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_INDICATOR, HUD_DMN_EVT_SET_STAGE_OBJECT, WAVE_MISSION_DESTROY);
	//	}
	//	break;

	//case 112:
	//	UIHUDDominationMgr::i()->SetHudEvent( HUD_DMN_SCOREBOARD, HUD_DMN_EVT_MISSION_CLEAR_COUNT, nData);
	//	break;
	//}
}

/*virtual*/	void CDomi_Flow_Destroy::ResetObject()
{
	//�ı� �̼� �ı� ������Ʈ ����(����).
	std::vector<i3Object*>& pMissionObjects = m_pmode->GetWaveMissionObjects();
	std::vector<i3Object*>::iterator itr;
	itr = pMissionObjects.begin();
	while( itr != pMissionObjects.end())
	{
		(*itr)->SetEnable(FALSE);
		(*itr)->SetDynamicState(I3GAME_DYNAMIC_STATE_NONE);
		++itr;
	}

	if( m_pDestroyObj != NULL)
	{//Reset �ϱ��� Ÿ�� ������Ʈ �ε����� �޾��� ���(BASE_EVENT_111)�� �ٷ� ����� �Ѵ�.
		m_pDestroyObj->SetEnable( TRUE);
		m_pDestroyObj->SetDynamicState(I3GAME_DYNAMIC_STATE_KINEMATIC);
	}
}
