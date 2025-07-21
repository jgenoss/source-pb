#include "pch.h"
#include "GameCharaUpdater1PV.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater1PV);

void CGameCharaUpdater1PV::Process1PVOnly( REAL32 rDeltatime)
{
	if( !m_pOwner->isNetwork())
	{
		const LadderStateInfo* ladderInfo = m_pOwner->getMoveContext()->getLadderStateInfo();

		if( getActionContext()->isLower( CHARA_LOWER_JUMP) || m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) ||
		 ( ladderInfo == nullptr && !m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) ) )
		{
			getAnimContext()->CallbackAnim_JumpStartRun( nullptr, rDeltatime);
		}
		else if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) || ladderInfo != nullptr ) 
		{
			if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
			{
				getAnimContext()->CallbackAnim_MoveRun( nullptr, rDeltatime);
			}
		}
	}
	else
	{
		if( getActionContext()->isLower( CHARA_LOWER_JUMP) )
		{
			getAnimContext()->CallbackAnim_JumpStartRun( nullptr, rDeltatime);
		}
		else if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
		{
			getAnimContext()->CallbackAnim_MoveRun( nullptr, rDeltatime);
		}
	}
	
}

void CGameCharaUpdater1PV::OnUpdate( REAL32 rDeltatime)
{
	//	Add code here
	//	1��Ī�� ��쿡 ��ü aistate�ǰ��� ȣ����� �����Ƿ� �ʿ��Ѱ�� ���� ȣ�����ֵ��� �ϴ� �Լ�
	Process1PVOnly( rDeltatime);

	CGameCharaUpdater::OnUpdate( rDeltatime);

	//	������� HUD ȭ�� ����Ʈ 
	getHUDContext()->OnUpdate( rDeltatime);

	// RPG7 ���� ���� �÷���
	CheckGrazeSoundPlay();
}