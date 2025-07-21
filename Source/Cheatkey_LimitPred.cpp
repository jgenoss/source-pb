#include "pch.h"
#include "Cheatkey_LimitPred.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "../UI/UIMainFrame.h"

namespace Cheatkey
{
	//----------------------------------------------
	// ��� ���� ����
	//----------------------------------------------
	bool LPred_Anywhere::operator()() { return true; }

	//----------------------------------------------
	// �ΰ��� ����
	//----------------------------------------------
	bool LPred_IngameOnly::operator()()
	{
		ISTEST_S
		{
			i3Framework* frmWork = i3Framework::getCurrentFramework();
			i3StageInfo* startStageInfo = frmWork->GetStartStageInfo(); // �ٷΰ���� �ʷε��׽�Ʈ�� ����Ȼ����Դϴ�.

			if(startStageInfo)
				return true;
		}

		return	(BattleSlotContext::i()->getSlotInfo(BattleSlotContext::i()->getMySlotIdx())->_State == SLOT_STATE_BATTLE) ? true : false;
	}	

	//----------------------------------------------
	// �ƿ����� ���� : �κ�, �����, �� ���...
	//----------------------------------------------
	bool LPred_OutgameOnly::operator()()
	{
		UINT8 state = BattleSlotContext::i()->getSlotInfo(BattleSlotContext::i()->getMySlotIdx())->_State;

		if (SLOT_STATE_LOAD == state)			return false;
		if (SLOT_STATE_RENDEZVOUS == state)		return false;
		if (SLOT_STATE_PRESTART == state)		return false;
		if (SLOT_STATE_BATTLE_LOADOK == state)	return false;
		if (SLOT_STATE_BATTLE_READY == state)	return false;
		if (SLOT_STATE_BATTLE == state)			return false;

		return true;
	}

	//----------------------------------------------
	// �κ� ����
	//----------------------------------------------
	bool LPred_LobbyOnly::operator()()
	{
		UIMainFrame* stage = g_pFramework->GetUIMainframe();
		if (stage)
		{
			if (stage->GetCurrentPhaseType() == UIPHASE_LOBBY) return true;
		}

		return false;
	}

	//----------------------------------------------
	// Ŭ�� �κ� ����
	//----------------------------------------------
	bool LPred_ClanLobbyOnly::operator()()
	{
		UIMainFrame* stage = g_pFramework->GetUIMainframe();
		if (stage)
		{
			if (stage->GetCurrentPhaseType() == UIPHASE_CLANLOBBY) return true;
		}

		return false;
	}

	//----------------------------------------------
	// ���ڷ�����
	//----------------------------------------------
	ILimitPredDecorator::ILimitPredDecorator(ILimitPred* p) : m_pred(p) {}
	ILimitPredDecorator::~ILimitPredDecorator()
	{
		I3_SAFE_DELETE(m_pred);
	}

	bool ILimitPredDecorator::operator()()
	{
		 return m_pred->operator()();
	}
	//----------------------------------------------
	// GM ����
	//----------------------------------------------
	bool LPred_GMOnly::operator()()
	{
		return ILimitPredDecorator::operator()() && UserInfoContext::i()->IsGM_Observer();
	}

	//----------------------------------------------
	// ġƮ ��� ���� ���� ����
	//----------------------------------------------
	bool LPred_PermittedAccountOnly::operator()()
	{
#if !defined(I3_DEBUG) && !defined(BUILD_RELEASE_QA_VERSION)
		return ILimitPredDecorator::operator()() && UserInfoContext::i()->CheckSpecialAbility(SA_CHEAT);
#else
		return ILimitPredDecorator::operator()();
#endif
	}
}