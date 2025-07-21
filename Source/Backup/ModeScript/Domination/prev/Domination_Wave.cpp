#include "pch.h"

#ifdef	DOMI_FLOW

#include "Domination_Wave.h"
#include "..\..\UI\HUD\UIHUD_Domination_Manager.h"
#include "GameMode_Domination.h"


CDominationWave::CDominationWave( gmode_domination* pmode) : 
	CDominationFlowBase( pmode)
{

}

CDominationWave::~CDominationWave(void)
{
}

//--------------------------------------
// �̺�Ʈ ó�� �Լ���
void CDominationWave::ProcessEventPreStartRound(void)
{
	m_pmode->ChangeWaveLayer();
}

void CDominationWave::ProcessEventRoundStart(void)
{
}

void CDominationWave::ProcessEventRoundEnd(void)
{
	// ���̺� ����� ��� AI�� �����մϴ�.
	g_pCharaManager->RemoveAllAIChara();
}


//--------------------------------------
// ������Ʈ �Լ���
void CDominationWave::OnUpdate( REAL32 rDeltaSeconds)
{
	//UINT64 keyStroke = g_pFramework->getKeyStroke();

	//if( keyStroke & GAME_KEY_SPECIAL_MISSION)
	//{
	//	if( g_pGameContext->IsSkillCooling() == false )
	//	{
	//		g_pGameContext->PushKeyNetwork( GAME_KEY_MAP_SPECIAL_MISSION);
	//		g_pGameContext->SetEvent( EVENT_KEY_PROCESS_NET );
	//	}
	//	else
	//	{
	//		g_pGameContext->AddSkillCoolNotice();
	//	}
	//}
}

#endif