#include "pch.h"
#include "AwayInput.h"
#include "UI/UIUtil.h"
#include "UtilBattle.h"

#include "BattleGui.h"

#include "ui/Batmanager.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

CAwayInput::CAwayInput() : m_rAwayInputTime(0.f), m_rCautionTime(0.f), m_bOffAwayInput(false), m_bOffCaution(false), m_pCautionMessageBox(nullptr)
{
		
}

CAwayInput::~CAwayInput()
{
	
	if( m_pCautionMessageBox )
	{
		GameUI::CloseMsgBox( m_pCautionMessageBox);
		m_pCautionMessageBox = nullptr;
	}
	
}

void	CAwayInput::ProcessCreateStage()
{
	m_bOffAwayInput = true;	m_bOffCaution = true;	
}

void	CAwayInput::SetAwayInputTime(REAL32 rAwayInputTime,REAL32 rCautionTime)
{
	if( BaTMaN::use_batman )
	{
		if( BaTMaN::enable_input_away_checker == false )
		{
			m_bOffAwayInput = true;	m_bOffCaution = true;	
			return;
		}
	}
	I3ASSERT(rAwayInputTime >= rCautionTime);
	m_rAwayInputTime = rAwayInputTime;
	m_rCautionTime = rCautionTime;
	m_bOffAwayInput = false;
	m_bOffCaution = false;
}

void	CAwayInput::Process_Update_OnCheckAwayInput(REAL32 rDeltaSeconds)
{

ISTEST_S
{
	// �̱� �׽�Ʈ����̸� ����
	return;
}

#if !defined(I3_DEBUG)
	if (LocaleValue::Enable("DisableCheckAwayInput")) return;

	if( UserInfoContext::i()->CheckAuthLevel() )	return; 
	if( getOffAwayInput() )	return;

	static REAL32 rCountDown = AWAY_INPUT_BATTLE_TIME - AWAY_INPUT_BATTLE_CAUTION;

#if defined	USE_AWAY_INPUT
	if( m_rAwayInputTime > 0.0f)
	{
		m_rAwayInputTime -= rDeltaSeconds;
		m_rCautionTime -= rDeltaSeconds;
	}
#endif

	bool is_live_chara = ub::is_live_character( BattleSlotContext::i()->getMySlotIdx() );
	bool enable_mode = MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION);

#if defined	USE_AWAY_INPUT
	if( g_pFramework->BeInput(false) ||	CBattleGui::i()->isEnableRespawnLimitMessage() ||
		(enable_mode	&& false == is_live_chara) )
#else
	if( g_pFramework->BeInput(false) || (enable_mode	&& false == is_live_chara) )
#endif
	{
		SetAwayInputTime(AWAY_INPUT_BATTLE_TIME, AWAY_INPUT_BATTLE_CAUTION);

		if( m_pCautionMessageBox )
		{
			GameUI::CloseMsgBox( m_pCautionMessageBox);
			m_pCautionMessageBox = nullptr;
		}
	}

	if( isAwayInput() )
	{
		setOffAwayInput();

		CBattleGui::i()->SetEnableGui( false);

		i3GuiRoot::setGuiDisableInput(true);

		INT32 nArg = 0;
		GameEventSender::i()->SetEvent(EVENT_GIVEUP_BATTLE, &nArg);

		// Ȧ��Ī �����Ͽ� ��Ŷ�� �� �� �� �ֱ� ������ �ڽ��� ��������� ��Ŷ�� �ȹ޾Ƶ� ������ ���ư���.
		setOffAwayInput();

		CGameFramework::JumpToReady();
	}
	else if( isCaution() )
	{
		setOffCaution();

		if( !m_pCautionMessageBox )
		{
			rCountDown = AWAY_INPUT_BATTLE_TIME - AWAY_INPUT_BATTLE_CAUTION;

			i3::rc_wstring wstrCount;
			i3::sprintf(wstrCount, GAME_RCSTRING("STBL_IDX_EP_GAME_AWAY_INPUT_BATTLE"), (INT32)rCountDown);
			m_pCautionMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, wstrCount);
		}
	}

	if (m_pCautionMessageBox)
	{
		rCountDown -= rDeltaSeconds;

		i3::rc_wstring wstrCount;
		i3::sprintf(wstrCount, GAME_RCSTRING("STBL_IDX_EP_GAME_AWAY_INPUT_BATTLE"), (INT32)rCountDown);

		m_pCautionMessageBox->ChangeMessage(wstrCount, nullptr);
	}

#else // defined(I3_DEBUG)
return;
#endif

}
