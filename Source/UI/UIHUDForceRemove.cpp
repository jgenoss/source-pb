#include "pch.h"
#include "UIHUDForceRemove.h"

#include "UIBattleFrame.h"
#include "UIBattlePopupAccuseCrime.h"
#include "UIHUDIngameChat.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UIUtil.h"
#include "UIBattlePopupExit.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/format_string.h"

I3_CLASS_INSTANCE( UIHUDForceRemove);//, UIHUDBase);

static UIHUDForceRemove * GetThis()
{
	return (UIHUDForceRemove*) UIBattleFrame::i()->GetHUD(UIHUD_FORCE_REMOVE);
}

extern "C" 
{
	int	Close_ForceExit_Result( LuaState * L)
	{
		return 0;
	}
}

LuaFuncReg UIHUDForciblyRemoveResult_Glue[] = 
{
	{"Close_ForceExit_Result",	Close_ForceExit_Result	},
	{nullptr,						nullptr}
};

/************************/
namespace inner
{
	i3::rc_wstring get_force_remove_reason_str(FORCEREMOVE_CAUSE reason);

	REAL32 elapsed_time = 0.f;

	// ����ð�
	const REAL32 max_close_time = 5.f;
	bool is_close_time_check = false;

	// ��ǥ���� ���ð�
	bool is_start_time_check = false;
	const INT32 max_start_time = 3;

	// ��ǥ ���
	bool is_vote_time_check = false;

	// �����̵� ��� 
	REAL32 max_sliding_time = 1.0f; // �����̵尡 �ݸ� ���� ���� 0.5��
	bool is_open = false;
	REAL32 slide_time = 0.f;

	// reset ��
	REAL32 fmw_pos_x = 0.f;
}

/************************/
UIHUDForceRemove::UIHUDForceRemove()
{
	m_SecCount.SetReverse(true);
	m_SecCountCB.SetObj(this);
	m_SecCount.SetCallback(&m_SecCountCB);

	RegisterGameEventFunc( EVENT_KICK_VOTE_SUGGEST_RESULT,		&UIHUDForceRemove::ReceiveGameEvent_KickVote_SuggestResult);
	RegisterGameEventFunc( EVENT_KICK_VOTE_START,				&UIHUDForceRemove::ReceiveGameEvent_KickVote_Start);
	RegisterGameEventFunc( EVENT_KICK_VOTE,						&UIHUDForceRemove::ReceiveGameEvent_KickVote_Update);
	RegisterGameEventFunc( EVENT_KICK_VOTE_COUNT,				&UIHUDForceRemove::ReceiveGameEvent_KickVote_Count);
	RegisterGameEventFunc( EVENT_KICK_VOTE_CANCEL,				&UIHUDForceRemove::ReceiveGameEvent_KickVote_Cancel);
	RegisterGameEventFunc( EVENT_KICK_VOTE_RESULT,				&UIHUDForceRemove::ReceiveGameEvent_KickVote_Result);

	m_progressValue = 1.f; 
	m_bReporter = false;
	m_bUISTART_finished = false;
	m_NextUIState = -1;
	m_CurUIState = -1;
}

UIHUDForceRemove::~UIHUDForceRemove()
{
}

/************************/
/*virtual*/ void UIHUDForceRemove::_InitializeAtLoad(i3UIScene * pScene)
{
	UIHUDBase::_InitializeAtLoad(pScene);

	m_ProgressBar[0] = (i3UIProgressBar*)pScene->FindChildByName("start_progressbar");
	m_ProgressBar[1] = (i3UIProgressBar*)pScene->FindChildByName("guide_progressbar");
	m_ProgressBar[2] = (i3UIProgressBar*)pScene->FindChildByName("voted_progressbar");

	m_pMainFmw[0] = (i3UIFrameWnd*)pScene->FindChildByName("FmwStart");
	m_pMainFmw[1] = (i3UIFrameWnd*)pScene->FindChildByName("FmwVoteGuide");
	m_pMainFmw[2] = (i3UIFrameWnd*)pScene->FindChildByName("FmwVoted");
	m_pMainFmw[3] = (i3UIFrameWnd*)pScene->FindChildByName("FmwResult");
}
/*virtual*/ void UIHUDForceRemove::OnCreate( i3GameNode * pParent)
{
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true ) return; // Ʃ�丮�󿡼��� �ε����� �ʵ���

	UIHUDBase::OnCreate( pParent);

	m_wstrRemainTimeStr = GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REMAIN_TIME_SUBJECT");
	m_wstrRemainSecondStr = GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REMAIN_TIME_SECOND");

	m_VoteTime = LocaleValue::GetInt32("ForciblyRemoveTime"); //��ǥ ���� �ð�

	enum { DEFAULT_VOTE_TIME = 20, };
	if( m_VoteTime == 0 ) m_VoteTime = DEFAULT_VOTE_TIME; //Ȥ�� ���� ���� ���� ��쿣 20���� �ʱ�ȭ �Ѵ�.

	//AddScene("Scene/HUD/PBRe_ForceExit_Vote.i3UIs", UIHUDForciblyRemoveResult_Glue, true, false);
	AddScene("Scene/HUD/PBRe_ForceExit_Vote.i3UIs", UIHUDForciblyRemoveResult_Glue, true, false);
}

bool UIHUDForceRemove::OnEntranceStart(void * pArg1, void * pArg2)
{
	if( UIHUDBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	for (int i = 0; i < 3; ++i)
		m_ProgressBar[i]->setProgress(m_progressValue);

	return true;
}

void UIHUDForceRemove::OnLoadAllScenes()
{
	UIHUDBase::OnLoadAllScenes();

	m_numAgree = 1;
	m_numDisagree = 1;

	inner::is_close_time_check = false;
	m_bStartVote = false;
	m_bVoted = false;

	LuaState* L =0;

	L = _CallLuaFunction( "LoadAllScenes");
	_EndLuaFunction( L, 0);
	// �ػ� ������� �̴ϸ� �ٷ� ������ �����Ǿ�� �մϴ�. �̴ϸ�Width:163
	inner::fmw_pos_x = GameUI::AutoPosition::get_max_left() + 164.f;

	ResetVoteWnd();
	ChangeUIState(FORCE_REMOVE_UI_START);

	// ESC �˾��� Ȱ��ȭ�� ���¿��� ��ǥ�� ���۵Ǹ�, ESC �˾��� �ֻ�ܿ� �׸����� �մϴ�
	if(UIBattleFrame::i()->IsOpenPopup(UBP_EXIT))
	{
		UIBattlePopupExit* pPopupExit = static_cast<UIBattlePopupExit*>(GameUI::GetBattlePopup(UBP_EXIT));
		if(pPopupExit != nullptr )
		{
			pPopupExit->AttachToTopScreen_ForceRemove();
		}
	}

	m_SecCount.SetBaseSec(m_VoteTime);
	m_SecCount.Start();
}

/*virtual*/ bool UIHUDForceRemove::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true) return false; //Ʃ�丮�� ���� ������� ����
	if( UIHUDBase::OnKeyInput( pInputMng) )
		return true;

	if( m_bStartVote == true && m_bVoted == true ) return false; //�̹� ��ǥ ����.

	if( UIHUDIngameChat::instance()->GetEnableChat() ) return false; // ä�� �Է� �߿��� ��ǥ�� �Ұ���

	if(inner::is_start_time_check == true) return false;	// ��ǥ���� ����߿��� ��ǥ �Ұ���

	if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_9) == true ) // ���� ��ư
	{	
		Vote( true);
		return true;
	}
	else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_0) == true ) // �ݴ� ��ư
	{
		Vote( false);
		return true;
	}

	return false;
}

void UIHUDForceRemove::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == true) return; //Ʃ�丮�� ���� ��� �� �´�.

	UIHUDBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

/// ���� �� ��Ȯ�ϰ� �ϱ� ���� rDeltaSeconds�� ���� ����..
/// ���� �����ս�ī���� Ȥ�� timeGetTime������ ������...
void	UIHUDForceRemove::OnUpdate( REAL32 rDeltaSeconds)
{
	UIHUDBase::OnUpdate(rDeltaSeconds);
	m_SecCount.Update();

	SetProgressBar(rDeltaSeconds);
	SetPopupSlide(rDeltaSeconds);

	if( inner::is_close_time_check == true )
	{
		inner::elapsed_time += rDeltaSeconds;
		if(inner::max_close_time <= inner::elapsed_time)
		{
			UIBattleFrame::i()->EnableHUD(UIHUD_FORCE_REMOVE, false);
			inner::is_close_time_check = false;
			inner::elapsed_time = 0.f;
			m_progressValue = 1.f; 
		}
	}
}

// �˾� ���°� ����� �� �����̵� ����� �����մϴ�.
void UIHUDForceRemove::SetPopupSlide(REAL32 rDeltaSeconds)
{
	// Vote Window Open
	if(inner::is_open)
	{
		if(inner::slide_time < inner::max_sliding_time)
		{
			inner::slide_time += rDeltaSeconds;
			SetVoteWnd_Open(m_CurUIState, rDeltaSeconds); 
		}

		if(m_pMainFmw[m_CurUIState]->getPos()->y > 0)
		{
			inner::is_open = false;
			inner::slide_time = 0.f; 
		}
	}

	// Vote Window Close
	// ��ǥ ���۵ǰ� 3s ���Ŀ� ����
	if( m_bUISTART_finished )
	{
		if(inner::slide_time < inner::max_sliding_time)
		{
			inner::slide_time += rDeltaSeconds;
			SetVoteWnd_Close(m_CurUIState, rDeltaSeconds, false); 
		}
		else
		{
			m_bUISTART_finished = false;
			inner::slide_time = 0.f;

			if(m_bReporter)
				ChangeUIState(FORCE_REMOVE_UI_VOTE);
			else
				ChangeUIState(FORCE_REMOVE_UI_GUIDE);
		}
	}

	if(inner::is_close_time_check == true)
	{
		// elapsed_time�� OnUpdate �ܿ��� ó���Ѵ�.
		if(inner::max_close_time - inner::elapsed_time <= inner::max_sliding_time)
		{
			SetVoteWnd_Close(m_CurUIState, rDeltaSeconds, false); 
		}
	}

	// ��Ű���: ��ǥ�ȳ� â�� ���ִ� ���¿��� ������ ��ǥ�� ���� ���
	if( m_NextUIState == (INT32)FORCE_REMOVE_UI_VOTE )
	{
		inner::slide_time += rDeltaSeconds;
		INT32 ui_state = m_NextUIState - 1;
		
		SetVoteWnd_Close(ui_state, rDeltaSeconds, !m_bReporter); 

		if(inner::slide_time >= inner::max_sliding_time)
		{
			ChangeUIState(m_NextUIState);
			ResetVoteWnd();
		}
	}

	// Current:VOTED/GUIDE ���� â���� ���â�� ���;� ��
	if( m_NextUIState == (INT32)FORCE_REMOVE_UI_RESULT)
	{
		inner::slide_time += rDeltaSeconds;
		
		SetVoteWnd_Close(m_CurUIState, rDeltaSeconds, false);

		if(inner::slide_time >= inner::max_sliding_time)
		{
			ChangeUIState(m_NextUIState);
			ResetVoteWnd();
		}
	}

}

void UIHUDForceRemove::SetProgressBar(REAL32 rDeltaSeconds)
{
	if (inner::is_start_time_check == true &&
		m_CurUIState == FORCE_REMOVE_UI_START)
	{
		inner::elapsed_time += rDeltaSeconds;
		m_progressValue = 1.f - (inner::elapsed_time / inner::max_start_time);

		if(m_progressValue <= 0)				m_progressValue = 0.f;
		m_ProgressBar[m_CurUIState]->setProgress(m_progressValue);

		if (inner::max_start_time <= inner::elapsed_time)
		{
			inner::is_start_time_check = false;
			inner::elapsed_time = 0.f;			
			m_progressValue = 1.f;
		}
	}
	else if( inner::is_vote_time_check == true &&
		(m_CurUIState == FORCE_REMOVE_UI_GUIDE || m_CurUIState == FORCE_REMOVE_UI_VOTE))
	{
		inner::elapsed_time += rDeltaSeconds;
		// 12792 : ������ǥ UI�� ������ ��µǴ� �ð��� UI��� ���Ŀ� �����̵� �˾��� ������ �ð��̹Ƿ� -2.f 
		m_progressValue = 1.f - (inner::elapsed_time / (m_VoteTime-inner::max_start_time-2.f));

		if(m_progressValue <= 0) 			m_progressValue = 0.f;
		m_ProgressBar[m_CurUIState]->setProgress(m_progressValue);

		if((m_VoteTime-inner::max_start_time-2.f) <= inner::elapsed_time )
		{
			inner::is_vote_time_check = false;
			inner::elapsed_time = 0.f;			
			m_progressValue = 1.f; 
		}
	}

}

/************************/
void UIHUDForceRemove::OnSecCount_Start(INT32 result_time)
{
}

void	UIHUDForceRemove::OnSecCount_Change(INT32 result_time)
{
	if ( m_VoteTime - result_time >= inner::max_start_time &&
		m_CurUIState == FORCE_REMOVE_UI_START)
	{
		m_bUISTART_finished = true; 
	}
}

void	UIHUDForceRemove::OnSecCount_DoneOrStop(INT32 result_time)
{
	//EndVote();
}

/************************/
void UIHUDForceRemove::Vote( bool bAgree)
{
	// ������ event�� ������.
	m_bMyVoteResult = bAgree;
	GameEventSender::i()->SetEvent( EVENT_KICK_VOTE, &bAgree);	// ������ ���� �ٸ�..

	m_bVoted = true;
}

void UIHUDForceRemove::EndVote()
{
	m_bStartVote = false;
	m_bVoted = false;

	inner::elapsed_time = 0.f;
	inner::is_close_time_check = true;
	inner::is_vote_time_check = false;
	inner::is_start_time_check = false;
	m_progressValue = 1.f;
	m_bReporter = false;
}

/************************/
void UIHUDForceRemove::to_Lua_Switch(FORCE_REMOVE_UI_TYPE type)
{
	LuaState* 	L = _CallLuaFunction( "Switch");
	i3Lua::PushInteger(L, type);
	_EndLuaFunction( L, 1);
}

void UIHUDForceRemove::to_Lua_VoteStart()
{
	i3::wstring reason_str = i3::format_string(L"%s", inner::get_force_remove_reason_str(m_Reason));

	i3::wstring nick_str = i3::format_string(L"%s",	m_wstrCandidateNickName);
	i3::wstring guide_str = i3::format_string(GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_NOTIFY"), nick_str); 

	LuaState* L = _CallLuaFunction("SetStartContext");
	i3Lua::PushStringUTF16To8( L, reason_str );
	i3Lua::PushStringUTF16To8( L, nick_str );
	i3Lua::PushStringUTF16To8( L, guide_str);
	UILua::EndLuaFunction(L , 3);
}

void UIHUDForceRemove::to_Lua_VoteCount()
{
	LuaState* L = _CallLuaFunction("SetVoteCount");

	i3::stack_wstring wstr_stack;

	i3Lua::PushInteger(L, m_CurUIState);

	i3::itoa( m_numAgree, wstr_stack);
	i3Lua::PushStringUTF16To8( L, wstr_stack );

	i3::itoa( m_numDisagree, wstr_stack);
	i3Lua::PushStringUTF16To8( L, wstr_stack );

	UILua::EndLuaFunction(L , 3);
}


void	UIHUDForceRemove::to_Lua_VoteResult(BATTLE_FORCIBLYREMOVE_RESULT Res)
{
	i3::stack_wstring wstrMsgFinal;	// �˾� UI�� ����� ����� ����
	i3::stack_wstring wstrMsgChat;	// ä��â�� ����� ����

	switch (Res)
	{
	case BATTLE_FORCIBLYREMOVE_SUCCESS:
		{
			i3::sprintf(wstrMsgChat, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_KICKED_BY_VOTE"), m_wstrCandidateNickName);
			i3::sprintf(wstrMsgFinal, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_EXIT_NOTIFY")); /*���� ���� �Ǿ����ϴ�*/
			break;
		}
	case BATTLE_FORCIBLYREMOVE_SHORTAGES:
	case BATTLE_FORCIBLYREMOVE_NOALLY:
	case BATTLE_FORCIBLYREMOVE_NOENEMY:
		{
			i3::sprintf(wstrMsgChat, GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_RESULT_REJECT"));
			i3::sprintf(wstrMsgFinal, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_REJECT_NOTIFY") );/*���� ������ �ΰ�Ǿ����ϴ�*/
			break;
		}
	case BATTLE_FORCIBLYREMOVE_UNKNOWN_ERROR:
		{
			wstrMsgFinal = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_UNKNOWN_ERROR"); /*�� �� ���� �����Դϴ�.*/
		}	
		break;
	}

	// �Ű����� ��쿡�� ä��â�� ����� ��µ��� ����
	//if(m_bReporter)
		//g_pFramework->PutSystemChatting(wstrMsgChat);

	LuaState* L2 = _CallLuaFunction( "SetResultContext");
	i3Lua::PushInteger(L2, m_numAgree);
	i3Lua::PushInteger(L2, m_numDisagree);
	i3Lua::PushStringUTF16To8( L2, wstrMsgFinal);
	_EndLuaFunction( L2, 3);
}

void UIHUDForceRemove::to_Lua_VoteCancel()
{
	LuaState* L = 0;

	//��ǥ ��� ȭ�� ��ȯ
	to_Lua_Switch((FORCE_REMOVE_UI_TYPE)m_CurUIState);

	/* ����ڰ� ��Ż�Ͽ� ��ǥ�� �ߴܵ˴ϴ�.*/
	i3::stack_wstring wstrMsg;
	i3::sprintf(wstrMsg, GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_CANCEL_VOTE"));

	L = _CallLuaFunction( "SetResultContext");
	i3Lua::PushStringUTF16To8( L, wstrMsg);
	_EndLuaFunction( L, 2);
}

void UIHUDForceRemove::to_lua_VoteGuide()
{
	i3::wstring reason_str = i3::format_string(L"%s", inner::get_force_remove_reason_str(m_Reason));
	LuaState* L = _CallLuaFunction("SetGuideContext");
	i3Lua::PushStringUTF16To8( L, m_wstrCandidateNickName);
	i3Lua::PushStringUTF16To8( L, reason_str);
	UILua::EndLuaFunction(L, 2);

	to_Lua_VoteCount();
}

/************************/
i3::rc_wstring inner::get_force_remove_reason_str(FORCEREMOVE_CAUSE reason)
{
	switch(reason)
	{
	case FORCEREMOVE_CURSE: return GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REASON_NOMANNER"); break;
	case FORCEREMOVE_ILLEGALPROGRAM: return GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REASON_ILLEGALPROGRAM"); break;
	case FORCEREMOVE_ABUSE: return GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REASON_ABUSE"); break;

	case FORCEREMOVE_ETC:
	default:
		break;
	}

	return GAME_RCSTRING("STR_HUD_FORCIBLYREMOVE_REASON_ETC");
}

void UIHUDForceRemove::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

//��ǥ ����(�Ű��ڿ�)
void UIHUDForceRemove::ReceiveGameEvent_KickVote_SuggestResult( INT32 arg, const i3::user_data&)
{
	if (arg != EVENT_ERROR_SUCCESS) return; //UIBattleFrame::ProcessGameEvent_Kick ���� �̸� ó���ȴ�.

	UIBattlePopupAccuseCrime * pAccuseCrimePopup = static_cast<UIBattlePopupAccuseCrime*>(GameUI::GetBattlePopup(UBP_ACCUSE_CRIME) );
	if( pAccuseCrimePopup != nullptr)
	{
		m_Reason = pAccuseCrimePopup->GetSelected_Cause();
		m_wstrCandidateNickName = pAccuseCrimePopup->GetSelected_NickName();
	}

	//��ǥ�� ���۵Ǿ���, �� �������� ��ǥ�� �ߴ�.
	m_bStartVote = true; 
	m_bVoted = true;
	m_bMyVoteResult = true;
	m_bReporter = true;

	m_SecCount.SetBaseSec(m_VoteTime);
	m_SecCount.Start();
	
	// CHANGEUI �Լ��� ���� ȣ��Ǳ� ������ Lua ������ ���� �ʿ䰡 ����
	to_Lua_VoteStart();
}

//��ǥ ����(�� �Ű��ڿ�)
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Start( INT32 arg, const i3::user_data&)
{
	INT32 app_slot, cand_slot;
	DisassembleValue(arg, app_slot, cand_slot, (INT32&)m_Reason);

	//app_slot ������ 1.1 �ҽ��� ���������, 1.5 �ҽ��� ������� �ʴ´�.
	//const char*  pszAppNick  = BattleSlotContext::i()->getNickForSlot(app_slot);
	m_wstrCandidateNickName = BattleSlotContext::i()->getNickForSlot( cand_slot);

	//��ǥ�� ���۵Ǿ���, �� ���� ��ǥ�� ���� �ʾҴ�.
	m_bStartVote = true;
	m_bVoted = false;
	m_bReporter = false;

	m_SecCount.SetBaseSec(m_VoteTime);
	m_SecCount.Start();

	// CHANGEUI �Լ��� ���� ȣ��Ǳ� ������ Lua ������ ���� �ʿ䰡 ����
	to_Lua_VoteStart();
}

//��ǥ�� �Ŀ� ȣ��ȴ�.
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Update( INT32 arg, const i3::user_data&)
{	 
	if (arg != EVENT_ERROR_SUCCESS) return; //UIBattleFrame::ProcessGameEvent_Kick ���� �̸� ó���ȴ�.

	if (m_bMyVoteResult)
		++m_numAgree;
	else
		++m_numDisagree;

	m_NextUIState = FORCE_REMOVE_UI_VOTE;
}

//���� ��ǥ ���� ��Ȳ ǥ�� - �ٸ� ������ ��ǥ�ϸ� ȣ�� �ȴ�.
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Count( INT32 arg, const i3::user_data&)
{
	DisassembleValue(arg, m_numAgree, m_numDisagree);
	to_Lua_VoteCount();
}


// �����ĺ��ڰ� ���� ���..
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Cancel( INT32 arg, const i3::user_data&)
{
	m_CurUIState = FORCE_REMOVE_UI_RESULT;
	inner::is_open = true;
	to_Lua_VoteCancel();
	m_SecCount.Stop();
	EndVote();
}

//��ǥ ��� - �����ڸ� ������ ������ �޽��ϴ�...
void UIHUDForceRemove::ReceiveGameEvent_KickVote_Result( INT32 arg, const i3::user_data&)
{
	//1.1 �ҽ��� ��/�� ���ھ ǥ���Ѵ�. ������ 1.5 �ҽ��� ǥ�� �� �ʿ䰡 ����.
	INT32 slot;
	BATTLE_FORCIBLYREMOVE_RESULT res;
	DisassembleValue(arg, slot, m_numAgree, m_numDisagree, (INT32&)res);

	//�� 2���� ���������� ���� ���̾���ϴµ�, ������ �޶����� �������ؿ��� ��Ŷ�� 
	//CANCEL�� ������ �����Դϴ�...��ġ�����θ� ����� �ʿ䰡 ����.
	//szNickName�� slot�� �г��ӹ��ڿ��� �����ؾ���... �񱳰˻縦 �ϰ� assert�� �̴ϴ�..
	//strncmp��ƾ ��ü�� �Ʈ�� ���ϴ�..
	I3ASSERT(i3::generic_is_equal(m_wstrCandidateNickName, BattleSlotContext::i()->getNickForSlot(slot)) );

	m_VoteResult = res;
	m_NextUIState = FORCE_REMOVE_UI_RESULT;
	m_SecCount.Stop();
	EndVote();
}

void UIHUDForceRemove::ChangeUIState(INT32 State)
{
	if(m_CurUIState == State) return;
	if(State < 0 ) return;

	m_CurUIState = State;

	to_Lua_Switch((FORCE_REMOVE_UI_TYPE)m_CurUIState);

	switch (m_CurUIState)
	{
	case FORCE_REMOVE_UI_START:
		inner::is_start_time_check = true;
		inner::is_open = true;
		to_Lua_VoteStart();
		break;
	case FORCE_REMOVE_UI_GUIDE:
		inner::is_start_time_check = false;
		inner::is_vote_time_check = true;
		inner::is_open = true;
		to_lua_VoteGuide();
		break;
	case FORCE_REMOVE_UI_VOTE:
		if(m_bReporter) // �Ű��ڴ� Guide�� ��ġ�� �ʰ� �ٷ� Vote�� �̵���
		{
			inner::is_start_time_check = false;
			inner::is_vote_time_check = true;
			inner::is_open = true;
		}
		to_Lua_VoteCount();
		break;
	case FORCE_REMOVE_UI_RESULT:
		inner::is_open = true;
		to_Lua_VoteResult(m_VoteResult);
		break;
	default:
		break;
	}
}

void UIHUDForceRemove::SetVoteWnd_Close(INT32 ui_type, REAL32 rDeltaSeconds, bool half)
{
	if(ui_type < 0 || ui_type >= MAX_SLIDE_WND)	 return;

	float fmw_size_y	= m_pMainFmw[ui_type]->getSize()->y;
	if(half)		fmw_size_y /= 2;
	VEC3D* fmw_pos		= m_pMainFmw[ui_type]->getPos();
	inner::fmw_pos_x	= fmw_pos->x;

	float distance	= fmw_pos->y - (fmw_size_y * rDeltaSeconds) / inner::max_sliding_time;
	m_pMainFmw[ui_type]->setPos(fmw_pos->x, distance);
}

void UIHUDForceRemove::SetVoteWnd_Open(INT32 ui_type, REAL32 rDeltaSeconds)
{
	if(ui_type < 0 || ui_type >= MAX_SLIDE_WND)	 return;

	float fmw_size_y	= m_pMainFmw[ui_type]->getSize()->y;
	VEC3D* fmw_pos		= m_pMainFmw[ui_type]->getPos();

	float distance =  fmw_pos->y + (fmw_size_y * rDeltaSeconds) / inner::max_sliding_time ;
	m_pMainFmw[ui_type]->setPos(fmw_pos->x, distance);
}

void UIHUDForceRemove::ResetVoteWnd()
{
	inner::slide_time = 0.f;
	m_NextUIState = -1;

	for(int i=0; i<MAX_SLIDE_WND; i++)
	{
		float fmw_size_y	= m_pMainFmw[i]->getSize()->y;
		// Slide�� �������� �ϱ� ������ Height��ŭ ���� �ö� �־�� ��
		m_pMainFmw[i]->setPos(inner::fmw_pos_x, 0-fmw_size_y);
	}

	// ��Ű����� ��� Guide ���°� �������� ���̴� �� ó�� �ؾ��ϱ� ������
	// Voted ���°� �����̵� ���� �� �ʿ䰡 ����
	if(!m_bReporter)
		m_pMainFmw[FORCE_REMOVE_UI_VOTE]->setPos(inner::fmw_pos_x, 0);
}