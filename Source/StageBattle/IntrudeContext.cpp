#include "pch.h"
#include "IntrudeContext.h"

#include "./UserContext.h"
#include "../StageBattle.h"
#include "BattleSlotContext.h"


IntrudeContext::IntrudeContext() : m_eState(MY_INTRUDE_STATE_NONE), m_ui8InterEnter(0) , m_bIntrusionMsgFlag(false)
{

}

IntrudeContext::~IntrudeContext() 
{

}



void IntrudeContext::InterEnter( UINT8 intrude)
{
	m_ui8InterEnter = intrude;
}

void IntrudeContext::ProcessIntrude()
{
	if( !IsIntrudeStart()) return;

	INT32 i;
	VEC3D vec;

	// ������ ���
	i3Vector::Set( &vec, 0.f, 0.f, -1.f );

	for( i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( i == BattleSlotContext::i()->getMySlotIdx()) continue;

		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( i);
		const GAMEINFO_CHARA_NETDATA * pCharaNetData = &BattleSlotContext::i()->getGameInfo_Chara( i)->_tNetData;

		if( pChara == nullptr )	
		{
			//I3PRINTLOG(I3LOG_NOTICE,"_ProcessIntrude #1 %d", i);
			continue;
		}
		if( UserContext::i()->Death[i])								
		{
			//I3PRINTLOG(I3LOG_NOTICE,"_ProcessIntrude #2 %d", i);
			continue;
		}
		if( const_cast<GAMEINFO_CHARA_NETDATA*>(pCharaNetData)->_tHP.getHP() > 0 )			
		{
			//I3PRINTLOG(I3LOG_NOTICE,"_ProcessIntrude #3 %d", i);
			continue;
		}
		if( UserContext::i()->IsPlayingSlot(i) == false)
		{
			//I3PRINTLOG(I3LOG_NOTICE,"_ProcessIntrude #4 %d", i);
			continue;
		}
		if( pChara->isInvincible() )						
		{
			//I3PRINTLOG(I3LOG_NOTICE,"_ProcessIntrude #5 %d", i);
			continue;
		}

		//�׾��ִ� ��츸 ���´�.
		pChara->SetEnable( true);		// SetVisible();
		pChara->SetVisible(false);
		
		// �������� �� �� ������Ʈ ������մϴ�.
		pChara->forceSetPos( pCharaNetData->_tPosRotData.getPos() );
		pChara->OnUpdate( 0.016f);	// �ѹ� ������Ʈ ����� �ش� ��ġ�� �̵��� �˴ϴ�.
		
		g_pCharaManager->DeathChara( pChara, i, &vec, CHARA_DEATH_F );
		UserContext::i()->Death[i] = true;
	}
}
