#include "pch.h"
#include "UIFloatPlayTimeAlarm.h"
#include "UIMainFrame.h"


#include "GameContext.h"
#include "ShutDownContext.h"

I3_CLASS_INSTANCE( UIFloatPlayTimeAlarm);//, UIFloatingBase);

extern "C"{
}

LuaFuncReg UIFloatPlayTimeAlarm_Glue[] = {
	{nullptr ,				nullptr}
};

UIFloatPlayTimeAlarm::UIFloatPlayTimeAlarm() : m_FloatLifeTime(0.f)
{
	m_AlignedPos = UIFLOATING_POS_LEFTBOTTOM;
	
	m_bFixedWindow = true;

#if defined( I3_DEBUG)
	SetName( "PlayTimeAlarm");
#endif

	RegisterGameEventFunc( EVENT_PLAYTIME_ALARM,			&UIFloatPlayTimeAlarm::ReceiveGameEvent_PlayTimeNotice);
	RegisterGameEventFunc( EVENT_SHUTDOWNTIME_ALARM,		&UIFloatPlayTimeAlarm::ReceiveGameEvent_ShutDownTimeNotice);
}

UIFloatPlayTimeAlarm::~UIFloatPlayTimeAlarm()
{
}

/*virtual*/ void UIFloatPlayTimeAlarm::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Main/PointBlankRe_Time_Alarm.i3UIs", UIFloatPlayTimeAlarm_Glue, true, false);
}

/*virtual*/ bool UIFloatPlayTimeAlarm::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	LuaState * L = _CallLuaFunction("__init");
	if( L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}
	
	return true;
}

/*virtual*/ void UIFloatPlayTimeAlarm::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate( rDeltaSeconds);

	if (m_FloatLifeTime > 5.0f)
	{
		ShutDownContext::i()->SetAlarmState(EMPTY_ALARM);
		GetMainFrame()->DisableFloatWin(UFW_PLAYTIMEALARM);
		m_FloatLifeTime = 0.f;
	}

	m_FloatLifeTime += rDeltaSeconds;
}

/*virtual*/ void UIFloatPlayTimeAlarm::_InitializeAtLoad(i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad(pScene);

	m_pText = (i3UIStaticText*)pScene->FindChildByName("i3UIStaticText");
}


/*virtual*/ void UIFloatPlayTimeAlarm::_ClearAtUnload(i3UIScene * pScene)
{
	UIFloatingBase::_ClearAtUnload(pScene);

	ShutDownContext::i()->SetAlarmState(EMPTY_ALARM);
}


/*virtual*/ bool UIFloatPlayTimeAlarm::OnExitStart( void)
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	// ���� �� ������� �˸��� ������ �̺�Ʈ�� �ٽ� ȣ���Ŵ
	if (ShutDownContext::i()->GetAlarmState() == WAIT_PLAYTIME_ALARM)
	{
		ShutDownContext::i()->SetAlarmState(EMPTY_ALARM);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_PLAYTIME_ALARM);
	}
	else if (ShutDownContext::i()->GetAlarmState() == WAIT_SHUTDOWN_ALARM)
	{
		ShutDownContext::i()->SetAlarmState(EMPTY_ALARM);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SHUTDOWNTIME_ALARM);
	}

//	UI::remove_cb(this);			// insert_cb�� ��� �ּ��̴ϴ�..

	return true;
}

/*virtual*/ void UIFloatPlayTimeAlarm::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIFloatingBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

//////////////////////////////////////////////////////////////////////////////

void UIFloatPlayTimeAlarm::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}


void UIFloatPlayTimeAlarm::ReceiveGameEvent_PlayTimeNotice( INT32, const i3::user_data&)
{
	// �÷���Ÿ�� �̺�Ʈ�� ���� ��
	// �˸� �޽����� ���� ��� �ٷ� ���
	if (ShutDownContext::i()->GetAlarmState() == EMPTY_ALARM)
		PlayTimeNotice();
	// ���� �˴ٿ� �˸��� ���� ��� ��� �˸����� Push
	else if(ShutDownContext::i()->GetAlarmState() != WAIT_SHUTDOWN_ALARM)
		ShutDownContext::i()->SetAlarmState(WAIT_PLAYTIME_ALARM);

}
void UIFloatPlayTimeAlarm::ReceiveGameEvent_ShutDownTimeNotice( INT32, const i3::user_data&)
{
	// �˴ٿ�Ÿ�� �̺�Ʈ�� ���� ��
	// �˸� �޽����� ���� ��� �ٷ� ���
	m_FloatLifeTime = 0;
	ShutDownTimeNotice();
	// �˴ٿ�Ÿ���� �켱������ ���� �����Ƿ� ������ ����ش�
}

void UIFloatPlayTimeAlarm::PlayTimeNotice()
{
	ShutDownContext::i()->SetAlarmState(PRINT_PLAYTIME_ALARM);

	i3::stack_wstring wstrTemp;
	i3::sprintf( wstrTemp, GAME_RCSTRING("STR_SHUTDOWN_INGAME_HUD_GAMECLOSE_TEXT2"), ShutDownContext::i()->GetPlayTime());

	LuaState * L = _CallLuaFunction("SetAlarmText");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrTemp);
		_EndLuaFunction(L, 1);
	}
}

void UIFloatPlayTimeAlarm::ShutDownTimeNotice()
{
	ShutDownContext::i()->SetAlarmState(PRINT_SHUTDOWNTIME_ALARM);

	i3::stack_wstring wstrTemp;
	i3::sprintf(wstrTemp, GAME_RCSTRING("STR_SHUTDOWN_INGAME_HUD_GAMECLOSE_TEXT"), ShutDownContext::i()->GetFnishTime());

	LuaState * L = _CallLuaFunction("SetAlarmText");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrTemp);
		_EndLuaFunction(L, 1);
	}
}