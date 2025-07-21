#include "pch.h"
#include "ObserverBattle.h"

#include "UtilBattle.h"
#include "HackDefense.h"
#include "BattleHud.h"
#include "BattleGui.h"
#include "ui/UIHudManager.h"

#include "./StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "BattleServerContext.h"

#include "GameMaster.h"
#include "BattleHUD_Scope.h"
#include "GameStateMgr.h"
#include "GameMissionManager.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIHudObserver.h"
#include "UI/UIHudDashBoard.h"
#include "UI/UIHudMissionBoard.h"
#include "UI/UIHUDIngameChat.h"


#define RESPAWN_COOL_TIME 3.0f
#define OBSERVER_CAMERA_INTERPOLATE_TIME 0.1f

namespace observer
{
	void ChangeObserverView(const INT32 idx)
	{
		UINT8 LockObserver = MyRoomInfoContext::i()->GetMyRoomLockObserver();

		//@eunil. 3��Ī ���� ������ ��� 3��Ī ȸ���� ����� �� ����, �� GM�� ���� ���� �ɷ�...
		BOOL bObserverCamera1PVOnly = TESTBIT(LockObserver, LOCK_OBSERVER_1PVONLY);
		if(bObserverCamera1PVOnly && UserInfoContext::i()->IsGM_Observer() == false)
			CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS, OBSERVER_CAMERA_INTERPOLATE_TIME);
		else
			CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_OBSERVER, OBSERVER_CAMERA_INTERPOLATE_TIME);

		observer::Context::i()->ChangeViewTarget(idx);	
	}

	bool ChangeViewToAnyoneAlive()
	{
		const INT32 idx = BattleSlotContext::i()->FindAnyoneAlive();
		if (idx != -1)
		{
			ChangeObserverView(idx);
			return true;
		}

		return false;
	}

	void ChangeViewTargetInTeam(const CHARACTER_TEAM_TYPE targetTeamType)
	{
		INT32 idx = observer::Context::i()->GetCurrIdx();
		CHARACTER_TEAM_TYPE currTeamType = CHARACTER_TEAM_NONE;

		if (idx == -1)
		{
			currTeamType = targetTeamType;
			idx = 0;
		}
		else
			currTeamType = BattleSlotContext::i()->getTeam(idx);

		if (targetTeamType != currTeamType)	// Ÿ������ ����.
			idx = BattleSlotContext::i()->FindAnyoneAlive(targetTeamType);
		else	// ���� �� �� ����.
			idx = BattleSlotContext::i()->FindNextPlayerAlive(targetTeamType, idx);

		if (idx != -1)
			observer::Context::i()->ChangeViewTarget(idx);
	}

	void ChangeViewToTopRanker(const TEAM_TYPE teamType)
	{
		INT32 idx = UserContext::i()->GetTopRankerIdx(teamType);
		if (idx != -1)
		{
			ChangeObserverView(idx);
			CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS);

			if(g_pCamera->getCurTarget())
			{
				CHUD_Scope::instance()->SetEnableSinperScope(false);
				g_pCamera->getCurTarget()->Cmd_Observer_Zoom();
			}
		}
	}

	void ChangeViewToTopview()
	{
		util_battle::SetToOldCameraIfOnObjectCamera();

		if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST )
			g_pFramework->SetNightVisionEnable(false);

		ub::toggle_topview_camera(); 
	}

	void ToggleFlyingCamera()
	{
#if defined( DEF_OBSERVER_MODE)
		// GM�� ����
		if( BattleSlotContext::i()->isObserverGM())
			ub::toggle_flying_camera_mode(); 
#else
#if !defined( I3_DEBUG)
		if(UserInfoContext::i()->IsGM_Observer())
			ub::toggle_flying_camera_mode(); 
#endif
#endif
	}

	bool CheckTeamMembersAllDead()
	{
		if (UserInfoContext::i()->IsGM_Observer())	// GM�������� ��� ��� �� �� ���� �����ϸ� ����ȭ���� ų���� ����.
		{
			if (!BattleSlotContext::i()->CheckAnyoneAlive(CHARACTER_TEAM_RED) ||
				!BattleSlotContext::i()->CheckAnyoneAlive(CHARACTER_TEAM_BLUE))
				return true;
		}
		else
		{
			if (!BattleSlotContext::i()->CheckAnyoneAlive(BattleSlotContext::i()->getMyTeam()))	// �Ϲ� ������ ��� �ڽ��� ���� ����� ����ȭ���� ų���� ����.
				return true;
		}

		return false;
	}

	void SetDotSight()
	{
		const INT32 currIdx = observer::Context::i()->GetCurrIdx();

		if (currIdx == -1) return;

		CGameCharaBase* pCharacter = g_pCharaManager->getCharaByNetIdx(currIdx);
		if (!pCharacter) return;

		WeaponBase* pWeapon = pCharacter->GetCurrentCharaWeapon();
		if(!pWeapon) return;

		if (pWeapon->GetCurrentExtensionType() == WEAPON::EXT_DOT_SIGHT ||
			pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SCOPE )
		{
			if (pCharacter->getViewType() == CHARA_VIEW_1PV)
				CHUD_Scope::instance()->SetEnableSinperScope(true);
			else 
			if (pCharacter->getViewType() == CHARA_VIEW_3PV)
				CHUD_Scope::instance()->SetEnableSinperScope(false);
		}
	}

	void SelectViewTargetDirectly_GMCommand(const CHARACTER_TEAM_TYPE teamType, const INT32 keyNumber)
	{
		INT32 idx = keyNumber - 1;	// Ű���� �ѹ�(1~8) => ���� �ε���(0~7).

		if (teamType == CHARACTER_TEAM_RED)
			idx = idx * 2;
		else
			idx = (idx * 2) + 1;

		if (MyRoomInfoContext::i()->IsAutoTeamChanged())	// �������밡 �ǽõǸ� ������ ����� ������ ���� �����Ѵ�.
			idx += (teamType == CHARACTER_TEAM_RED) ? 1 : -1;

		for (INT32 i=0; i<=idx; i++)	// GM ������ ��ŵ ó��.
		{
			if (BattleSlotContext::i()->getTeam(i) == teamType)
			{
				if (BattleSlotContext::i()->IsGMObserverForClient(i))
				{
					idx += 2;
					break;
				}
			}
		}

		//�ش� ���� GM�� �ִ� ��� �ε����� +2�� �Ǿ� 15�� �Ѿ� ũ���ð� ���� ���� ����
		if (idx > 15)
			return;

		if (ub::is_live_character(idx))
			observer::Context::i()->ChangeViewTarget(idx);
	}

	bool IsPreparedToObserveEnemy()
	{
		UINT8 lock = MyRoomInfoContext::i()->GetMyRoomLockObserver();	// ����â���� üũ�� ���溸�� ������ ����.
		if (TESTBIT(lock, LOCK_OBSERVER_ENEMY))
			return true;

		// GM �� ��� ������ ���� ���� ���, �������� ��庰�� ���.
		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) ||
#if defined( DEF_OBSERVER_MODE)
			BattleSlotContext::i()->isObserverMe())
#else
			UserInfoContext::i()->IsGM_Observer())
#endif
		{
			return true;
		}

		return false;
	}

	bool IsPreparedToObserveEnemyHP()
	{
		UINT8 lock = MyRoomInfoContext::i()->GetMyRoomLockObserver();	// ����â���� üũ�� ���溸�� ������ ����.
		if ( TESTBIT( lock, LOCK_OBSERVER_ENEMY_HP))
			return true;

#if defined( DEF_OBSERVER_MODE)
		if (BattleSlotContext::i()->isObserverMe())
#else
		if (UserInfoContext::i()->IsGM_Observer())
#endif
		{
			return true;
		}

		return false;
	}

	bool IsObserverState()
	{
		return (observer::Context::i()->GetCurrIdx() != -1) ? true : false;
	}

	bool UpdateObserverCamera(observer::Context* ctx, REAL32& tmAccumulated, const REAL32 tm)
	{
		if (i3::same_of<CGameCamera_FLY*>(g_pCamera))
		{
			if(CHud::i() && ctx->IsGMObserverHUDOff() == false)
				CHud::i()->SetHudMode(HUD_MODE_MISSION_OBSERVER);
				//CHud::i()->SetHudMode(HUD_MODE_MISSION_FLY);

			return false;	// ���� ��ȯ ó���� ���� �ʰڴ�.
		}

		g_pCamera->AdjustTargetByCamera();

		const INT32 currIdx = ctx->GetCurrIdx();
		if (currIdx == -1) return false;

		if (!ub::is_live_character(currIdx))
		{
			tmAccumulated += tm;

			if (tmAccumulated > RESPAWN_COOL_TIME)
			{
				INT32 idx = 0;
				if (UserInfoContext::i()->IsGM_Observer())	// ���� ���� ĳ������ �� ���Ḧ ������� �����Ѵ�.
				{	
					const CHARACTER_TEAM_TYPE currTeam = BattleSlotContext::i()->getTeam(currIdx);
					idx = BattleSlotContext::i()->FindNextPlayerAlive(currTeam, currIdx);
				}
				else
				{
					idx = BattleSlotContext::i()->FindNextPlayerAlive(BattleSlotContext::i()->getMyTeam(), currIdx);
				}

				if (idx == -1)
					idx = BattleSlotContext::i()->FindAnyoneAlive();

				if (idx != -1)
					ctx->ChangeViewTarget(idx);
			}
		}

		//PBBUG-17127 ������Ʈ���� �� Ű�� �ɱ��
		if(CHud::i() && ctx->IsGMObserverHUDOff() == false)
			CHud::i()->SetHudMode(HUD_MODE_MISSION_OBSERVER);

		return true;
	}


	struct ViewState
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm) = 0;
	};

	struct ViewState_OnReady : ViewState				// ų ī�޶� �Ǵ� �� ���� ���·� ��ȯ. 
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
	} g_viewstate_ready;

	struct ViewState_OnTeamMembersAllDead : ViewState	// �� ���� => ų�� ������� Ÿ�� ����.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
	} g_viewstate_teamdead;

	struct ViewState_OnKillCamera : ViewState			// ų ī�޶� ���� ��(���� �Ǵ� ���� ĳ���Ͱ� �׾��� ��), ���� ���� �� ����ȭ������ ��ȯ.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
	} g_viewstate_killcam;

	struct ViewState_OnObserverCamera : ViewState		// ���� ȭ�� ���� �� : Ÿ�� �����ֱ�, Ÿ�� �����ϱ�, �� ���� ó��.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
		REAL32 m_tm;
	} g_viewstate_observercam;

	struct ViewState_OnRedTeamDeadAfterInstallingBomb : ViewState	// �������� ��ź ��ġ �� ������ ����. ��ź�� �۵� ���� ����� ����� ���� �� �� ������.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
		REAL32 m_tm;
	} g_viewstate_redteamdead_after_installbomb;

	struct ViewState_OnIntrude : ViewState	// ����.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
	} g_viewstate_intrude;



	void ViewState_OnReady::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		if(CHud::i())
		{
			if (CHud::i()->getHudMode() != HUD_MODE_DEFAULT && MyRoomInfoContext::i()->getStage()->IsHudOff() == false)
				CHud::i()->SetHudMode(HUD_MODE_DEFAULT);
		}
		if (CheckTeamMembersAllDead())
		{
			owner->SetViewState(&g_viewstate_teamdead);
			return;
		}

		if (i3::same_of<CGameCamera_Kill*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_OBSERVER, OBSERVER_CAMERA_INTERPOLATE_TIME);
			owner->SetViewState(&g_viewstate_killcam);
			return;
		}

		if (BattleSlotContext::i()->IsLocalPlayerDead())
		{
			if (ChangeViewToAnyoneAlive())
			{
				g_viewstate_observercam.m_tm = 0.0f;
				owner->SetViewState(&g_viewstate_observercam);
			}
			return;
		}
	}

	void ViewState_OnTeamMembersAllDead::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		// �������� �����Ͽ����� ��ź�� ��ġ�Ǿ���. ��ź�� �����ų� ��ü�� ������ ���� �÷��� ����.
		const bool isRedTeamAllDead = 
			(BattleSlotContext::i()->FindAnyoneAlive(CHARACTER_TEAM_RED) == -1) ? true : false;

		const bool isBombInstalled = 
			(CGameMissionManager::i()->getMissionState() == GAME_MISSION_STATE_INSTALLED) ? true : false;

		//@eunil. �������� ������! �ٽ� ���½����ش�.��, ���̴� ���� ����.
		CGameMissionManager * pMissionManager = CGameMissionManager::i();
		if(pMissionManager->getRespawnMode() != GAME_RESPAWN_MODE_1LIFE)
		{
			const bool isBlueTeamAllDead =
				(BattleSlotContext::i()->FindAnyoneAlive(CHARACTER_TEAM_BLUE) == -1) ? true : false;

			if (!isRedTeamAllDead && !isBlueTeamAllDead) {
				owner->SetViewState(&g_viewstate_ready);
				return;
			}
		}

		if (isRedTeamAllDead && isBombInstalled)
		{
			g_viewstate_redteamdead_after_installbomb.m_tm = 0.0f;
			owner->SetViewState(&g_viewstate_redteamdead_after_installbomb);
			return;
		}

		INT32 idx = BattleSlotContext::i()->GetKillerIdxMostRecent();

		if (idx == -1 || !ub::is_live_character(idx))	// �ڻ쿡 ���� ó���� ��ȿ���� �����Ƿ� �ƹ� �÷��̾ Ÿ������.
			idx = BattleSlotContext::i()->FindAnyoneAlive();

		if (idx != -1)
		{
			if(CHud::i())
				CHud::i()->ResetKillMessage();

			if (IsObserverState())
				owner->ChangeViewTarget(idx);
			else
				ChangeObserverView(idx);
		}
	}

	void ViewState_OnKillCamera::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		if (CheckTeamMembersAllDead())	// ųī�޶� ���߿� ���� ���� ��� => ��� �÷��̾ Ÿ������ ����.
		{
			owner->SetViewState(&g_viewstate_teamdead);
			return;
		}

		if (!i3::same_of<CGameCamera_Kill*>(g_pCamera))	// ųī�޶� �ƴϹǷ� Ÿ�� ���� / ���� ��ȯ.
		{
			if (ChangeViewToAnyoneAlive())
			{
				UINT8 LockObserver = MyRoomInfoContext::i()->GetMyRoomLockObserver();
				//@eunil. 3��Ī ���� ������ ��� 3��Ī ȸ���� ����� �� ����, �� GM�� ���� ���� �ɷ�...
				BOOL bObserverCamera1PVOnly = TESTBIT(LockObserver, LOCK_OBSERVER_1PVONLY);
				if (bObserverCamera1PVOnly && UserInfoContext::i()->IsGM_Observer() == false)
					CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS, OBSERVER_CAMERA_INTERPOLATE_TIME);

				g_viewstate_observercam.m_tm = 0.0f;
				owner->SetViewState(&g_viewstate_observercam);
			}
			return;
		}

		if (((CGameCamera_Kill*)g_pCamera)->isFreeze()) // ����ȭ������ ��ȯ. ųī�޶� ���� ��������.
		{
			if (!IsObserverState())
			{
				if (ChangeViewToAnyoneAlive())
				{
					UINT8 LockObserver = MyRoomInfoContext::i()->GetMyRoomLockObserver();
					//@eunil. 3��Ī ���� ������ ��� 3��Ī ȸ���� ����� �� ����, �� GM�� ���� ���� �ɷ�...
					BOOL bObserverCamera1PVOnly = TESTBIT(LockObserver, LOCK_OBSERVER_1PVONLY);
					if (bObserverCamera1PVOnly && UserInfoContext::i()->IsGM_Observer() == false)
						CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS, OBSERVER_CAMERA_INTERPOLATE_TIME);

					g_viewstate_observercam.m_tm = 0.0f;
					owner->SetViewState(&g_viewstate_observercam);
				}
				return;
			}
		}
	}

	void ViewState_OnObserverCamera::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		if (UpdateObserverCamera(owner, m_tm, tm))
		{
			if (CheckTeamMembersAllDead())
				owner->SetViewState(&g_viewstate_teamdead);
		}
	}

	void ViewState_OnRedTeamDeadAfterInstallingBomb::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		const INT32 idx = BattleSlotContext::i()->FindAnyoneAlive(CHARACTER_TEAM_BLUE);
		if (idx != -1)
		{
			if (!IsObserverState())
				ChangeObserverView(idx);	

			UpdateObserverCamera(owner, m_tm, tm);
		}
	}

	void ViewState_OnIntrude::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		INT32 idx = BattleSlotContext::i()->FindAnyoneAlive(BattleSlotContext::i()->getMyTeam());

		I3_BOUNDCHK_RETURN( idx, SLOT_MAX_COUNT);

		if ( idx != -1 && BattleSlotContext::i()->IsGMObserverForClient(idx))	// GM�������� �÷��� ���� �� �ٷ� �ױ� ������ ���õǾ�� �� �ȴ�.
			idx = BattleSlotContext::i()->FindNextPlayerAlive(BattleSlotContext::i()->getMyTeam(), idx);

		if (idx == -1)
			idx = BattleSlotContext::i()->FindAnyoneAlive(BattleSlotContext::i()->getEnemyTeamType());

		if (idx != -1)
		{
			ChangeObserverView(idx);

			g_viewstate_observercam.m_tm = 0.0f;
			owner->SetViewState(&g_viewstate_observercam);
		}
	}

	void SetIntrudeViewState()
	{
		observer::Context::i()->SetViewState(&g_viewstate_intrude);
	}



	Context::Context()
	{
		Reset();
	}

	void Context::Reset()
	{
		m_currIdx = -1;
		m_currViewState = &g_viewstate_ready;
		m_bGMHUDOff = false;
	}

	bool Context::ChangeViewTarget(const INT32 idx)
	{
		hd::check_observer::i()->check();	// ��ŷ ��� ó��. SPION ���� ROOM_INFO_ADD::_LockObserver�� ��ŷ�ϴ� ������ Ȯ���Ѵ�.

		if (idx < 0 || SLOT_MAX_COUNT <= idx) return false;
		if (!ub::is_live_character(idx)) return false;

		CGameCharaBase* pCharacter = g_pCharaManager->getCharaByNetIdx(idx);

		if (pCharacter == nullptr) return false;
		if (g_pCamera->getCurTarget() == pCharacter) return false;

		if (g_pCamera->getCurTarget())	// ���� ĳ���͸� 3��Ī��� ó��
			g_pCamera->getCurTarget()->Cmd_Observer(false);

		CGameCameraManager::i()->setTargetCharacter(pCharacter);	// �ٲ�� ĳ���͸� ī�޶� ���
		pCharacter->Cmd_Observer(true);

		m_currIdx = idx;

		CHud::i()->SetEnableObserver(true);
		observer_hud::Context::i()->Notify(observer_hud::EVT_TARGET_CHANGED, idx);

		for (INT32 i=0; i<g_pCharaManager->getCharaCount(); i++)	// TeamTag��Ȱ��ȭ
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara(i);
			if (pChara == nullptr)	continue;
			pChara->Cmd_Enable_TeamTag( false);

			if (pChara->same_of(Avatar::static_meta()))	// �� ��� ����
#if defined (LOCALE_RUSSIA)		
				((Avatar*)pChara)->DisableTeamBand();	// PBRU-1064 ���þ� ��û���� ����� ����
#else
				((Avatar*)pChara)->SetTeamBand(true);	// ���� ���� �������� ����ؾ��մϴ�.	
#endif
		}

		return true;
	}

	void Context::SetViewState(ViewState* state)
	{
		m_currViewState = state;
	}

	void Context::OnUpdate(REAL32 tm)
	{
		m_currViewState->OnUpdate(this, tm);

		if (IsObserverState())
		{
			_UpdateInput();
		}
	}

	INT32 Context::GetCurrIdx() const
	{
		return m_currIdx;
	}

	void Context::_UpdateInput()
	{
		if(CHud::i() == nullptr) return;

		i3InputDeviceManager* pInputMgr = g_pViewer->GetInputDeviceManager();
		i3InputKeyboard* pKey = pInputMgr->GetKeyboard();
		i3InputMouse* pMouse = pInputMgr->GetMouse();

		// revision 61410 [IngameUI] ESC �� ���� �޴� Ȱ��ȭ �� ��������� ��ǲ ����
		if (CBattleGui::i()->GetEnableGui(GAME_GUI_RESPAWN)) return;
		if (CHud::i()->GetEnableChatScroll()) return;
		if (CBattleGui::i()->GetEnablePopup()) return;

		if (!CHud::i()->GetChatEnable())
		{
			if (UserInfoContext::i()->IsGM_Observer())
				_UpdateInput_GMObserver(pKey, pMouse);

			if (pKey->GetStrokeState(I3I_KEY_SPACE))
				ub::toggle_normal_observe_mode();
			else 
			if (pKey->GetStrokeState(I3I_KEY_P))
				ToggleFlyingCamera();
		}

		if (!i3::same_of<CGameCamera_FLY*>(g_pCamera) && !CGameMaster::i()->IsPaused())
			_UpdateInput_ViewChange(pKey, pMouse);
	}

	void Context::_UpdateInput_GMObserver(i3InputKeyboard* pKey, i3InputMouse* pMouse)
	{
		CHud* pHUD = CHud::i();

		if (pKey->GetPressState(I3I_KEY_LSHIFT) || pKey->GetPressState(I3I_KEY_RSHIFT))
		{
			if (pKey->GetStrokeState(I3I_KEY_1))		
				ChangeViewToTopRanker(TEAM_RED);
			else 
			if (pKey->GetStrokeState(I3I_KEY_2))	
				ChangeViewToTopRanker(TEAM_BLUE);
			else 
			if (pKey->GetStrokeState(I3I_KEY_3))	
				ub::toggle_flying_camera_mode();
			else 
			if (pKey->GetStrokeState(I3I_KEY_4))
				ChangeViewToTopview();
		}
		else if (g_pFramework->getKeyboard()->GetPressState(I3I_KEY_LCTRL) || g_pFramework->getKeyboard()->GetPressState(I3I_KEY_RCTRL))
		{
			if (i3::same_of<CGameCamera_FLY*>(g_pCamera)) // ���� ���. �ö��� ī�޶��� ���� ���� ����.
			{
				for (INT32 i=0; i<=9; i++) // 0 ~ 9 number key
				{
					if (pKey->GetStrokeState(static_cast<I3I_KEY>(i)))
						FlyCameraUtil::SaveFlyCameraLocation(static_cast<I3I_KEY>(i));
				}
			}

			if (pKey->GetPressState(I3I_KEY_LALT))
			{
				if (pKey->GetStrokeState(I3I_KEY_P))
				{
					if (CGameMaster::i()->IsPaused())	//GM Observer Pause ���
						GameEventSender::i()->SetEventOnce(EVENT_BATTLE_GM_RESUME);
					else
						GameEventSender::i()->SetEventOnce(EVENT_BATTLE_GM_PAUSE);
				}
			}
		}
		else if (pKey->GetStrokeState(I3I_KEY_NUMPADPLUS) || pKey->GetStrokeState(I3I_KEY_NUMPADMINUS))
		{
			if(pKey->GetStrokeState(I3I_KEY_NUMPADPLUS))
				pHUD->ChangeTextNameSize(true);
			else 
			if(pKey->GetStrokeState(I3I_KEY_NUMPADMINUS))
				pHUD->ChangeTextNameSize(false);
			else
				I3ASSERT_0;
		}
		else
		{
			if (pKey->GetStrokeState(I3I_KEY_F5))	// �ΰ��� HUD ON/OFF(����׸�忡�� ä��â "/test 21"�� ����)
			{
				if (IsGMObserverHUDOff() == false) //����
				{
					SetGMObserverHUDOff(true);
					pHUD->SetHudMode(HUD_MODE_NOTHING);

					//���ھ� ����� �׻� On
					UIHudDashBoard* pBoard = static_cast<UIHudDashBoard*>(HUD::instance()->GetHud(HUD::DASH_BOARD));
					if(pBoard) pBoard->SetVisible(true);

					UIHUDIngameChat* chat = (UIHUDIngameChat*)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
					if (chat) chat->SetVisible(false);
				}
				else
				{ // ����
					SetGMObserverHUDOff(false);

					pHUD->SetHudMode(HUD_MODE_MISSION_OBSERVER);

					//���ھ� ����� �׻� On
					UIHudDashBoard* pBoard = static_cast<UIHudDashBoard*>(HUD::instance()->GetHud(HUD::DASH_BOARD));
					pBoard->SetVisible(true);

				}
			}
			else 
				if (pKey->GetStrokeState(I3I_KEY_GRAVE))	// �г��� ON/OFF
					UserInfoContext::i()->ToggleViewHudNick();
				else 
				if (pKey->GetStrokeState(I3I_KEY_EQUAL))	// �г��� ������ ���� : I3I_KEY_NUMPADPLUS -> WHEEL UP, I3I_KEY_NUMPADMINUS -> WHEEL DOWN
					pHUD->ChangeTextNameSize(true);
				else 
				if (pKey->GetStrokeState(I3I_KEY_MINUS))
					pHUD->ChangeTextNameSize(false);

			for (INT32 i=0; i<=9; i++) // 0 ~ 9 number key, ī�޶� ��ġ �̵�
			{
				if (pKey->GetStrokeState(static_cast<I3I_KEY>(i)))
					FlyCameraUtil::MoveFlyCameraLocation(static_cast<I3I_KEY>(i));
			}

			if (pKey->GetPressState(I3I_KEY_LBRACKET))	// ���� ���� ����(���� ����) ���. RED TEAM [
			{
				for (INT32 i=1; i<=MAX_TEAM_SLOT_COUNT; i++)	// 1 ~ 8 number Key
				{
					if (pKey->GetStrokeState(static_cast<I3I_KEY>(i)))
						SelectViewTargetDirectly_GMCommand(CHARACTER_TEAM_RED, i);
				}
			}
			else if (pKey->GetPressState(I3I_KEY_RBRACKET))	// BLUE TEAM ] 
			{
				for (INT32 i=1; i<=MAX_TEAM_SLOT_COUNT; i++)	// 1 ~ 8 number Key
				{
					if (pKey->GetStrokeState(static_cast<I3I_KEY>(i)))
						SelectViewTargetDirectly_GMCommand(CHARACTER_TEAM_BLUE, i);
				}
			}
		}
	}

	void Context::_UpdateInput_ViewChange(i3InputKeyboard* pKey, i3InputMouse* pMouse)
	{
		if (pMouse->GetStrokeButtonState() & I3I_MOUSE_LBUTTON)	// ������ Ÿ�� ����.
		{
			if (IsPreparedToObserveEnemy())
			{
				ChangeViewTargetInTeam(CHARACTER_TEAM_RED);
			}
			else
			{
				if (BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_RED)
					ChangeViewTargetInTeam(CHARACTER_TEAM_RED);
			}
		}
		else if (pMouse->GetStrokeButtonState() & I3I_MOUSE_RBUTTON)	// ����� Ÿ�� ����.
		{
			if (IsPreparedToObserveEnemy())
			{
				ChangeViewTargetInTeam(CHARACTER_TEAM_BLUE);
			}
			else
			{
				if (BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_BLUE)
					ChangeViewTargetInTeam(CHARACTER_TEAM_BLUE);
			}
		}
	}
}