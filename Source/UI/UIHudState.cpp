#include "pch.h"
#include "UIHudState.h"

#include "UIHudDefine.h"
#include "UIHudUtil.h"
#include "MyRoomInfoContext.h"
//#define i3_profile_enable
#include "i3Base/profile/profile.h"

//- UIHudState ------------------------------------//
UIHudState::UIHudState(UIHudManager* p) : iHudBase(p)
{
}

UIHudState::~UIHudState()
{
}

/*********************************/
void UIHudState::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	HUD::MISSION_MODE mode = static_cast< HUD::MISSION_MODE >( arg1 );

	if( m_setting_mission_mode == mode ) return;
	m_setting_mission_mode = mode;

	//dino�� dino_state_hud �� ������ ó���� �Ѵ�.
	//dino�� dino_state_hud �� �����ش�.
	if(s_pMyChara != nullptr && hu_ut::is_dino(s_pMyChara) == m_is_dino_state_hud )
	{
		iHudBase::SetEnable( mode != HUD::MISSION_OBSERVER );
	}
	else
	{
		iHudBase::SetEnable( false );
	}
}


/*********************************/
/*virtual*/ void UIHudState::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	iHudBase::SetVisible(false);

	event_start_battle_impl(arg);
}

/*virtual*/ void UIHudState::event_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	m_setting_mission_mode = HUD::MISSION_NONE;

	//���� �� ĳ���͸� ���� -> ���� �ϱ� ������ �ٽ� ���� ��.
	//dino�� dino_state_hud �� �����ش�.
	if(s_pMyChara != nullptr 
		&& hu_ut::is_dino(s_pMyChara) == m_is_dino_state_hud 
		&& MyRoomInfoContext::i()->getStage()->IsHudOff() == false)
	{
		iHudBase::SetEnable( true );
		event_respawn_battle_impl(arg);
	}
	else
	{
		iHudBase::SetEnable( false );
	}
}

void UIHudState::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetEnable( hu_ut::is_domination_mode() );
}