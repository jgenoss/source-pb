#include "pch.h"

struct disable_warning_lnk_4221 {} _;

#ifdef DOMI_USER_REVIVAL

#include "DomiRevivalController.h"
#include "UIHUD_Domination_Manager.h"

#include "../UIHudManager.h"
#include "../UIHudRespawnTimer.h"

#include "../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

namespace
{
	class Behavior
	{
	public:
		virtual void operator()() = 0;
	};

	class None : public Behavior
	{
	public:
		virtual void operator()() {}
	}g_none;

	// ���� : ��ü ��Ȱ HUD ���
	class TimeToUseWholeRevival : public Behavior
	{
	public:
		virtual void operator()()
		{
			iHudBase* hud = HUD::instance()->GetHud(HUD::RESPAWN_TIMER);
			UIHudRespawnTimer* respawnTimer = static_cast<UIHudRespawnTimer*>(hud);

			if( respawnTimer->is_working())
			{
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_HOLD);
			}

			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::revival);

			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_VISIBLE, dui::annihilation);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::annihilation);

		}
	}g_wholeRevival;

	class TimeToRespawn : public Behavior
	{
	public:
		virtual void operator()()
		{
			iHudBase* hud = HUD::instance()->GetHud(HUD::RESPAWN_TIMER);
			UIHudRespawnTimer* respawnTimer = static_cast<UIHudRespawnTimer*>(hud);

			if( respawnTimer->is_working())
			{
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_RELEASE_HOLD);
			}
			else
			{
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_START);
			}

			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::annihilation);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_VISIBLE, dui::revival);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::revival);
		}
	}g_respawn;

	class TimeToUseIndivisualRevival : public Behavior
	{
	public:
		virtual void operator()()
		{
			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::annihilation);

			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_VISIBLE, dui::revival);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::revival);		
		}
	}g_indivisualRevival;

	class ClearIfPrinted : public Behavior
	{
	public:
		virtual void operator()()
		{
			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::annihilation);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::revival);

			iHudBase* hud = HUD::instance()->GetHud(HUD::RESPAWN_TIMER);
			if (hud->IsVisible())
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, HUD::RT_RESET);
		}
	}g_clear;

	Behavior* g_behavior[RB_MAX] = 
	{
		&g_none, &g_wholeRevival, &g_respawn, &g_indivisualRevival, &g_clear
	};


	// ��Ʈ��ũ ���� ������� ����ִ��� ���� �Ǵ�.
	bool SomeoneAlive()
	{
		for (INT32 i=0; i<4; i++)
		{
			INT32 userIdx = i*2; // 0, 2, 4, 6
			if (userIdx == BattleSlotContext::i()->getMySlotIdx()) continue;

			bool isPlaying = UserContext::i()->IsPlayingSlot(userIdx) ? true : false;
			if (isPlaying)
			{
				CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(userIdx);
				if (chara && (chara->isCharaStateMask(CHARA_STATE_DEATH) == false))
					return true;
			}
		}

		return false;
	}

	bool NoTheOthers()
	{	
		for (INT32 i=0; i<4; i++)
		{
			INT32 userIdx = i*2;
			if (userIdx == BattleSlotContext::i()->getMySlotIdx()) continue;
			if ( UserContext::i()->IsPlayingSlot(userIdx))
				return false;
		}

		return true;
	}

	REVIVAL_BEHAVIOR What()
	{
		CGameCharaBase* local = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		if (local == nullptr) return RB_MAX;

		bool localDead = local->isCharaStateMask(CHARA_STATE_DEATH);
		bool local_intrusion = ( UserContext::i()->IsPlayingSlot( BattleSlotContext::i()->getMySlotIdx()) == false);
		 
		if (local_intrusion)				return RB_TIME_TO_USE_WHOLE_REVIVAL;	// ���͹̼��� ���� �� �ٷ� �������ϱ� ������ ���� ������ ���̺� �߿��� ����.
		if (!localDead)						return RB_COMPLETED_RESPAWN;				// ������ ���� ���
			
		if (localDead && NoTheOthers())		return RB_TIME_TO_USE_WHOLE_REVIVAL;		// ȥ�ڸ� �÷����ϰ� �ִ� ���
		if (localDead && !SomeoneAlive())	return RB_TIME_TO_USE_WHOLE_REVIVAL;		// ���� �÷��� �ϴٰ� ������ ���....
		if (localDead && SomeoneAlive())	return RB_TIME_TO_RESPAWN;
		
		
		I3PRINTLOG(I3LOG_NOTICE, "##revival log >>>>>>");
		I3PRINTLOG(I3LOG_NOTICE, "##revival log my slot(%d)", BattleSlotContext::i()->getMySlotIdx());
		for( INT32 i = 0; i <= 6; ++i)
		{
			CGameCharaBase* chara = nullptr;
			if( !(i%2))
			{
				chara = g_pCharaManager->getCharaByNetIdx( i, false);
				if( chara)
				{
					I3PRINTLOG(I3LOG_NOTICE,"  -- slot(%d), nick(%s), is playing(%d), state death(%d)", i, BattleSlotContext::i()->getNickForSlot(i), 
						UserContext::i()->IsPlayingSlot(i), chara->isCharaStateMask(CHARA_STATE_DEATH));
				}
			}
		}
		I3PRINTLOG(I3LOG_NOTICE, "##revival log <<<<<<");

		return RB_NONE;
	}
}

RevivalController::RevivalController() : m_keyInput(new DomiRevivalKeyInput::KeyInput)
{
	MEMDUMP_NEW( m_keyInput, sizeof( DomiRevivalKeyInput::KeyInput));
	DomiRevivalKeyInput::CustomKeyInput * pCb = new DomiRevivalKeyInput::CustomKeyInput;
	MEMDUMP_NEW( pCb, sizeof( DomiRevivalKeyInput::CustomKeyInput));
	m_keyInput->SetCallback( pCb);
}
RevivalController::~RevivalController()
{
	I3_SAFE_DELETE( m_keyInput);
}

void RevivalController::Show()
{
	INT32 what = static_cast<INT32>(What());
	(*g_behavior[what])();
}

void RevivalController::ImmediateRespawn()
{
	//����� ������ �ٷ� ������ ��ų�� �ֵ��� �մϴ�.
	iHudBase* hud = HUD::instance()->GetHud(HUD::RESPAWN_TIMER);
	UIHudRespawnTimer* respawnTimer = static_cast<UIHudRespawnTimer*>(hud);
	if( respawnTimer->is_working())
		HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_IMMEDIATE);
	else
		GameEventSender::i()->SetEventOnce( EVENT_RESPAWN_BATTLE);
}

void RevivalController::ProcessKeyInput()
{
	m_keyInput->ProcessKeyInput();
}



using namespace DomiRevivalKeyInput;

void CustomKeyInput::OnKeyInput()
{
	// U ��ư : ���κ�Ȱ
	if (g_pFramework->getKeyboard()->GetStrokeState(I3I_KEY_U))
	{
		if (domi_state_info::i()->single_respawn_count > 0)
		{
			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INPUT, I3I_KEY_U);
		}
		else
		{
			//domi_util::buy_domi_item(CASHITEM_GROUP_INDIVIDUAL_REVIVAL);
		}
	}

	// I ��ư : ��ü��Ȱ
	if (g_pFramework->getKeyboard()->GetStrokeState(I3I_KEY_I))
	{
		if (domi_state_info::i()->everyone_respawn_count > 0)
		{
			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INPUT, I3I_KEY_I);
		}
		else
		{
			//domi_util::buy_domi_item(CASHITEM_GROUP_WHOLE_REVIVAL);
		}
	}
}

KeyInput::~KeyInput()
{
	I3_SAFE_DELETE(m_cb);
}

void KeyInput::ProcessKeyInput()
{
	m_cb->OnKeyInput();
}

void KeyInput::SetCallback(KeyInputCallback* cb)
{
	m_cb = cb;
}

#endif