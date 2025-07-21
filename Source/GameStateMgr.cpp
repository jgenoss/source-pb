#include "pch.h"
#include "GameStateMgr.h"

//#include "../../../Common/CommonSource/WeaponNumber.h"

#include "i3Base/string/ext/snprintf.h"

#include "i3Sound/i3SoundContextFMOD.h"

#include "StageBattle.h"
#include "HackDefense.h"
#include "ObserverBattle.h"
#include "UtilBattle.h"

#include "GameCharaWeaponContext.h"
#include "LogRespawn.h"

#include "IGShopItemController.h"
#include "CrosshairMgr.h"
#include "GameMode.h"
#include "GameMap.h"
#include "GameOrd.h"

#include "TutorialMode.h"
#include "ObjectManager.h"
#include "ObjExplosion.h"
#include "UpdateMapSector.h"
#include "ProfileLog.h"

#include "ChainKills.h"
#include "Crosshair.h"
#include "BattleHud.h"
#include "BattleGui.h"
#include "AwayInput.h"
#include "BattleHUD_Scope.h"
#include "BattleHUD_Revenge.h"
#include "BattleHUD_CountDown.h"

#include "GameDropItem.h"
#include "GameDropItemManager.h"
#include "GameWeaponManager.h"
#include "GameCharaCameraContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaMoveContext.h"
#include "Character/Fullbody/GameCharaFullbody.h"

#include "Network/GameNetworkManager.h"

#include "StageObject/GameControledObject.h"
#include "StageObject/TargetInTutorial.h"
#include "StageObject/ObjectBattleUseItem.h"

#include "Mode/Domination/DominationSkillSystem.h"
#include "Mode/Domination/GameMode_Domination.h"
#include "Mode/Domination/DomiUserSkillMgr.h"
#include "Mode/Domination/DomiSkillProperties.h"

#include "WeaponLRU_Manager.h"

#include "./StageBattle/AIModeContext.h"
#include "./StageBattle/UserContext.h"
#include "./StageBattle/NetworkContext.h"

#include "BattleServerContext.h"
#include "KickContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "BattleResultContext.h"
#include "UserInfoContext.h"
#include "HackContext.h"
#include "GameContextUtil.h"
#include "TutorialContext.h"
#include "GameMissionManager.h"
#include "MedalManager.h"
#include "GameMaster.h"
#include "BattleSyncTimer.h"
#include "Weapon/WeaponItemID.h"
#include "ChannelContext.h"
#include "ClanSeasonContext.h"
#include "AssistContext.h"
#include "GameCharaManager.h"
#include "GameCharaMultiWeaponContext.h"

#include "UI/PurchaseMgr.h"
#include "UI/MinimapUtil.h"
#include "UI/UIUtil.h"
#include "UI/UIHudUtil.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIHudManager.h"
#include "UI/UIHudNotice.h"
#include "UI/UIHUDBase.h"
#include "UI/UIHUDTabMinimap.h"
#include "UI/UIBattlePopupBase.h"
#include "UI/UIHudMissionBoard.h"
#include "UI/HUD/UI_HUD_TutorialMode.h"
#include "UI/hud/UIHUD_Domination_Manager.h"
#include "UI/HUD/DomiRevivalController.h"
#include "UI/UIHUDIngameChat.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "UI/UIBattleFrame.h"
#include "UI/UIHudObserver.h"
#include "BattleHUD_CountDown.h"

#include "MissionCard/McardUtil.h"

#include "ClientStageInfoUtil.h"

#include "../USG_Miscs.h"

#define OBJ_ASSIST_TIME		0.1f	// Object�� ���������̼� Ÿ��.. 0.1�� ������ ���������̼� �մϴ�.
#define OBJ_ASSIST_TIMEINV	10.f	//


namespace
{

	void CbFailStart(void* pThis,INT32 nParam)
	{
		CBattleGui::i()->SetEnableGui( false);
		i3GuiRoot::setGuiDisableInput(true);
		CGameFramework::JumpToReady();
	}

	void CbTimeOutHost(void* pThis,INT32 nParam)
	{
		if (g_pFramework->IsBattleStage())
		{
			I3PRINTF("CbTimeOutHost()\n");
			GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
		}
	}

	void CbTimeOutClient(void* pThis,INT32 nParam)
	{
		if (g_pFramework->IsBattleStage())
		{
			CBattleGui::i()->SetEnableGui( false);
			i3GuiRoot::setGuiDisableInput(true);
			CGameFramework::JumpToReady();
		}
	}

	void CbRoomKicked(void* pThis,INT32 nParam)
	{
		// ���� �� ȣ��ɶ� pThis�� ������ �ڴ�.
		// ���� pThis�� ����ؼ� �ȵȴ�.
		CBattleGui::i()->SetEnableGui( false);
		i3GuiRoot::setGuiDisableInput(true);
		CGameFramework::JumpToReady(true);
	}
	
	void CbExitNormal(void* pThis,INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			GOTO_EXIT_STAGE();
		}
	}

	void CbGMRoomKicked(void* pThis, INT32 nParam)
	{
		if( g_pFramework->IsBattleStage())
		{
			if(nParam == MBR_OK)
			{
				// GM�� ���� ���� ����ó��
				GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
			}
		}
	}
}

static CGameCharaBase * getMyChara( void)
{
	return g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
}

//
//  ����������Ʋ�� ���������ε����� ���� �б�� TCP���ú���Ŷ���� ������ ������ �ű⵵�� ����..
//

I3_CLASS_INSTANCE( gstatemgr_AvatarChange); //, i3GameNode);

gstatemgr_AvatarChange::gstatemgr_AvatarChange()
{

}

gstatemgr_AvatarChange::~gstatemgr_AvatarChange()
{

}

bool gstatemgr::GetChangeChara( INT32 idx)
{
	NET_CHARA_INFO * pSrcInfo	= BattleSlotContext::i()->setNetCharaInfo( idx);

	INT32 nSlot = CCharaInfoContext::i()->GetMainCharaIdx();
	I3ASSERT( nSlot != -1);

	pSrcInfo->setCurrentCharaIndex( (UINT8)nSlot);

	const CHARA_PARTS_INFO* pPartsInfo = g_pCharaManager->getCharaInfo(idx)->GetParts();
	return pPartsInfo->IsSame( pSrcInfo->getCharaParts());
}

void gstatemgr_AvatarChange::_ProcessEvent_Respawn_AvatarChange(INT32 idx, const i3::user_data& UserData)
{
	//GlobalFunc::PB_TRACE( "EVENT_RESPAWN_BATTLE");

	if( idx < 0
#if defined( DEF_OBSERVER_MODE)
		|| idx >= SLOT_MAX_COUNT
#endif
		)
	{
		return;
	}

 	if( g_pCharaManager->getCharaByNetIdx(idx))
	{
		gstatemgr::i()->_ProcessEvent_Respawn(idx, UserData);
	}
}

void gstatemgr_AvatarChange::sAfterLoadChara( void * pAvatar, INT32 idx, AI_BIND_DATA * pAIBind)
{
	if( idx == BattleSlotContext::i()->getMySlotIdx() )
	{
		if( g_pCharaManager->getCharaByNetIdx( idx) == nullptr )
		{
			// vv3 - check
			T_ItemID ItemID = BattleSlotContext::i()->getNetCharaInfo( idx)->getCharaParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

			CGameCharaBase * pChara  = g_pCharaManager->AddChara( idx, CHARA::ItemID2ResID( ItemID), false );
			I3ASSERT(pChara != nullptr);
		}

		UserContext::i()->Death[idx] =false;
		if( BattleServerContext::i()->IsP2PHost() )
		{
			CGameNetworkManager::i()->StartBattleUserProcess( idx);
		}
	}
	else
	{
		// vv3 - check
		CHARA_RES_ID CharaResID = CHARA::ItemID2ResID( BattleSlotContext::i()->getNetCharaInfo( idx)->getCharaParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));

		if( CharaResID == CHARA_RES_ID_UNKNOWN)
		{
			I3PRINTLOG(I3LOG_NOTICE, "intrude start chara type invalid. idx : %d, type : %d", idx, CharaResID);
		}

		CGameCharaBase * pChara  = g_pCharaManager->AddChara( idx, CharaResID, false );
		I3ASSERT(pChara != nullptr);
	}


	const NET_CHARA_INFO * pCharaInfo = BattleSlotContext::i()->getNetCharaInfo( idx);
	I3ASSERT( pCharaInfo->getCurrentCharaIndex() != 0xFF);

	GLOBAL_CHARA_INFO * pDestInfo = g_pCharaManager->setCharaInfo( idx);
	pCharaInfo->getCharaParts()->CopyTo( pDestInfo->SetParts());

	gstatemgr::i()->_ProcessEvent_Respawn(idx, i3::user_data());
	CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS);
}


gstatemgr::gstatemgr() : m_stage(nullptr), m_nStageState(GAMESTAGE_STATE_NONE), 
m_fEndTime(0.0f), m_fRoundTime(0.f), m_bRespawnEnable(false), m_fRespawnDelayTime(0.f), m_bNormalEnd(true), m_init_update_fn(&gstatemgr::_OnFirstSceneUpdate),
m_nSvrRoundWinnerTeam(-1), m_nRoundWinnerTeam(-1), m_ProfileLog(nullptr), m_bDriveAway(false), m_pgStatemgr_AvatarChange(nullptr), m_bLoadSuccess(false)
{
	i3mem::FillZero(m_aiCharacterRespawnTimer, sizeof(m_aiCharacterRespawnTimer));


	reg_evt(EVENT_BATTLE_ENTERANCE_USER,  &gstatemgr::_Process_EVENT_BATTLE_ENTERANCE_USER);
	reg_evt(EVENT_RENDEZVOUS_HOLEPUNCH_BATTLE, &gstatemgr::_Process_EVENT_RENDEZVOUS_HOLEPUNCH_BATTLE);
	reg_evt(EVENT_PRESTART_BATTLE, &gstatemgr::_Process_EVENT_PRESTART_BATTLE);
	reg_evt(EVENT_START_BATTLE , &gstatemgr::_Process_EVENT_START_BATTLE);
	reg_evt(EVENT_GIVEUP_BATTLE, &gstatemgr::_Process_EVENT_GIVEUP_BATTLE);
	reg_evt(EVENT_DEATH_BATTLE, &gstatemgr::_Process_EVENT_DEATH_BATTLE);
	reg_evt(EVENT_RESPAWN_BATTLE, &gstatemgr::_Process_EVENT_RESPAWN_BATTLE);
	reg_evt(EVENT_INGAMESHOP_APPLY_ITEM, &gstatemgr::_Process_EVENT_INGAMESHOP_APPLY_ITEM);
	reg_evt(EVENT_RESPAWN_BATTLE_FOR_AI, &gstatemgr::_Process_EVENT_RESPAWN_BATTLE_FOR_AI);
	reg_evt(EVENT_START_COUNTDOWN, &gstatemgr::_Process_EVENT_START_COUNTDOWN);
	
	reg_evt(EVENT_RESPAWN_DOMI_DINO, &gstatemgr::_Process_EVENT_RESPAWN_DOMI_DINO);
	reg_evt(EVENT_DEATH_DOMI_USER, &gstatemgr::_Process_EVENT_DEATH_DOMI_USER);
	//reg_evt(EVENT_WAVE_PRE_START, &gstatemgr::_Process_EVENT_WAVE_PRE_START);
	reg_evt(EVENT_WAVE_END, &gstatemgr::_Process_EVENT_WAVE_END);


	reg_evt(EVENT_M_END_BATTLE, &gstatemgr::_Process_EVENT_M_END_BATTLE);
	reg_evt(EVENT_ENTER_LOBBY, &gstatemgr::_Process_EVENT_ENTER_LOBBY);
	reg_evt(EVENT_TIMEOUT_HOST, &gstatemgr::_Process_EVENT_TIMEOUT_HOST);
	reg_evt(EVENT_TIMEOUT_CLIENT, &gstatemgr::_Process_EVENT_TIMEOUT_CLIENT);
	reg_evt(EVENT_ARMEDBOMB_BATTLE, &gstatemgr::_Process_EVENT_ARMEDBOMB_BATTLE);
	reg_evt(EVENT_DISARMBOMB_BATTLE, &gstatemgr::_Process_EVENT_DISARMBOMB_BATTLE);
	reg_evt(EVENT_OBJECT_USE_BATTLE, &gstatemgr::_Process_EVENT_OBJECT_USE_BATTLE);
	reg_evt(EVENT_M_PRE_ROUNDSTART_BATTLE, &gstatemgr::_Process_EVENT_M_PRE_ROUNDSTART_BATTLE);
	reg_evt(EVENT_M_ROUNDSTART_BATTLE, &gstatemgr::_Process_EVENT_M_ROUNDSTART_BATTLE);
	reg_evt(EVENT_M_ROUNDEND_BATTLE, &gstatemgr::_Process_EVENT_M_ROUNDEND_BATTLE);
	reg_evt(EVENT_M_KICK_USER, &gstatemgr::_Process_EVENT_M_KICK_USER);
	reg_evt(EVENT_KICK_VOTE_MY_KICKED, &gstatemgr::_Process_EVENT_KICK_VOTE_MY_KICKED);
	reg_evt(EVENT_M_HOSTLOST_BATTLE, &gstatemgr::_Process_EVENT_M_HOSTLOST_BATTLE);
	reg_evt(EVENT_M_DRIVE_AWAY, &gstatemgr::_Process_EVENT_M_DRIVE_AWAY);
	reg_evt(EVENT_M_GAMEGUARD_ERROR, &gstatemgr::_Process_EVENT_M_GAMEGUARD_ERROR);
	reg_evt(EVENT_M_ERROR, &gstatemgr::_Process_EVENT_M_ERROR);
	reg_evt(EVENT_M_DISCONNECTION, &gstatemgr::_Process_EVENT_M_DISCONNECTION);
	reg_evt(EVENT_M_SHUTDOWNTIME, &gstatemgr::_Process_EVENT_M_SHUTDOWN);
	reg_evt(EVENT_M_DESTROYED_NETWORK, &gstatemgr::_Process_EVENT_M_DESTROYED_NETWORK);
	reg_evt(EVENT_M_INVALID_TIMESYNC, &gstatemgr::_Process_EVENT_M_INVALID_TIMESYNC);
	reg_evt(EVENT_ITEM_AUTH, &gstatemgr::_Process_EVENT_ITEM_AUTH);
	reg_evt(EVENT_BATTLE_TOUCHDOWN, &gstatemgr::_Process_EVENT_BATTLE_TOUCHDOWN);
	reg_evt(EVENT_TUTORIAL_ROUND_END, &gstatemgr::_Process_EVENT_TUTORIAL_ROUND_END);
	reg_evt(EVENT_BATTLE_ESCAPEBLOCK, &gstatemgr::_Process_EVENT_BATTLE_ESCAPEBLOCK);
	reg_evt(EVENT_BATTLE_GM_PAUSE, &gstatemgr::_Process_EVENT_BATTLE_GM_PAUSE);
	reg_evt(EVENT_BATTLE_GM_RESUME, &gstatemgr::_Process_EVENT_BATTLE_GM_RESUME);
	reg_evt(EVENT_GM_EXIT_USER, &gstatemgr::_Process_EVNET_GM_USER_EXIT);
	reg_evt(EVENT_HACKTOOL_DETECTED, &gstatemgr::_Process_EVENT_HackToolDetected);
	
	reg_state(GAMESTAGE_STATE_NONE, &gstatemgr::_Process_GAMESTAGE_STATE_NONE);
	reg_state(GAMESTAGE_STATE_BATTLE,	 &gstatemgr::_Process_GAMESTAGE_STATE_BATTLE);			
	reg_state(GAMESTAGE_STATE_ENDROUND,  &gstatemgr::_Process_GAMESTAGE_STATE_ENDROUND);				
	reg_state(GAMESTAGE_STATE_SHOWSCORE,  &gstatemgr::_Process_GAMESTAGE_STATE_SHOWSCORE);				
	reg_state(GAMESTAGE_STATE_ENDGAME,  &gstatemgr::_Process_GAMESTAGE_STATE_ENDGAME);				
	reg_state(GAMESTAGE_STATE_PRESTART_ROUND,  &gstatemgr::_Process_GAMESTATE_STATE_PRESTART_ROUND);			

	reg_state(GAMESTAGE_STATE_WAIT_HOLEPUNCHING,  &gstatemgr::_Process_GAMESTATE_WAIT_HOLEPUNCHING);

}

gstatemgr::~gstatemgr()
{
	I3_SAFE_RELEASE(m_ProfileLog);
	
}

void gstatemgr::reg_evt( GAME_EVENT event, evt_fn f)
{
	m_evt_fn_map.insert(i3::unordered_map< GAME_EVENT, evt_fn >::value_type( event, f));
}

void gstatemgr::reg_state(GAMESTAGE_STATE state, state_fn f)
{
	m_state_fn[state] = f;
}

void gstatemgr::ProcessCreateStage(CStageBattle* sb)
{
	m_stage = sb;

	m_ProfileLog = CProfileLog::new_object();
	log_respawn::i()->ProcessCreateStage();
	//observer::Context::i()->ProcessCreateStage();
	hd::check_observer::i()->ProcessCreateStage();
	observer::Context::i()->Reset();
	hd::check_speedhack::i()->ProcessCreateStage();
	hd::check_mapdata::i()->ProcessCreateStage();
	hd::check_elapsedsynctimer::i()->ProcessCreateStage();

	m_nStageState = GAMESTAGE_STATE_NONE;
	m_fEndTime	= 0.0f;
	m_fRoundTime = 0.f;
	m_bRespawnEnable = false;
	m_fRespawnDelayTime = 0.f;
	m_bNormalEnd = true;
	m_bDriveAway = false;

	m_bLoadSuccess = true;

	m_nSvrRoundWinnerTeam = -1;
	m_nRoundWinnerTeam = -1;

	update_map_sector::i()->ProcessCreateStage();

	i3mem::FillZero(m_aiCharacterRespawnTimer, sizeof(m_aiCharacterRespawnTimer));

	obj_explosion::i()->ProcessCreateStage();

	m_init_update_fn = &gstatemgr::_OnFirstSceneUpdate;

	ISTEST_S
	{
		m_nStageState = GAMESTAGE_STATE_BATTLE;			// ��ݿ� �׽�Ʈ������� ��쿡 ���� BATTLE�� �Ѿ...
	}
	ISTEST_E

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	ChangeMyRoomInfoStageID_ForNonNetworkDebug();
#endif

	const STAGE_ID   StageID		= MyRoomInfoContext::i()->getStage()->m_ui32StageID;
	const STAGE_MODE StageType		= MyRoomInfoContext::i()->getStage()->GetStageMode();
	const STAGE_UID  StageUid		= MyRoomInfoContext::i()->getStage()->GetStageUID();

	create_gmode(StageType);

	gmode::i()->ProcessCreateStage( sb );

	create_gmap(StageUid);
	create_gord(StageUid);

	ScoreContext::i()->CreateBattle();
	BattleServerContext::i()->CreateBattle();
	BattleObjContext::i()->CreateBattle();
	BattleSlotContext::i()->CreateBattle();
	UserContext::i()->CreateBattle();

	m_pgStatemgr_AvatarChange = gstatemgr_AvatarChange::new_object();

}

void gstatemgr::ProcessDestroyStage()
{
	m_stage = nullptr;
	m_nStageState = GAMESTAGE_STATE_NONE;

	destroy_gord();
	destroy_gmap();
	destroy_gmode();

	obj_explosion::i()->ProcessDestroyStage();

	ScoreContext::i()->DestroyBattle();

	I3_SAFE_RELEASE(m_ProfileLog);
	I3_SAFE_RELEASE(m_pgStatemgr_AvatarChange);
}

#ifdef DOMI_NEW_FLOW
void gstatemgr::ProcessCreateRoundJumpStage(CStageBattle* sb)
{
	m_stage = sb;

	if (!m_ProfileLog)
		m_ProfileLog = CProfileLog::new_object();
	log_respawn::i()->ProcessCreateStage();
	//observer_battle::i()->ProcessCreateStage(); // ROOM_INFO_ADD�� ���µǸ� check_observer::check()���� ��ŷ Ž����.
	hd::check_speedhack::i()->ProcessCreateStage();
	hd::check_mapdata::i()->ProcessCreateStage();
	hd::check_elapsedsynctimer::i()->ProcessCreateStage();

	m_nStageState = GAMESTAGE_STATE_NONE;
	m_fEndTime	= 0.0f;
	m_fRoundTime = 0.f;
	m_bRespawnEnable = false;
	m_fRespawnDelayTime = 0.f;
	m_bNormalEnd = true;
	m_bDriveAway = false;

	m_bLoadSuccess = true;

	m_nSvrRoundWinnerTeam = -1;
	m_nRoundWinnerTeam = -1;

	update_map_sector::i()->ProcessCreateStage();

	i3mem::FillZero(m_aiCharacterRespawnTimer, sizeof(m_aiCharacterRespawnTimer));

	obj_explosion::i()->ProcessCreateStage(); 

	m_init_update_fn = &gstatemgr::_OnFirstSceneUpdate;

	ISTEST_S
	{
		m_nStageState = GAMESTAGE_STATE_BATTLE;			// ��ݿ� �׽�Ʈ������� ��쿡 ���� BATTLE�� �Ѿ...
	}
	ISTEST_E

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	ChangeMyRoomInfoStageID_ForNonNetworkDebug();
#endif

	const STAGE_ID   StageID = MyRoomInfoContext::i()->getStage()->m_ui32StageID;
	const STAGE_MODE StageType = MyRoomInfoContext::i()->getStage()->GetStageMode();
	const STAGE_UID  StageUid = MyRoomInfoContext::i()->getStage()->GetStageUID();

	create_gmode(StageType);

	gmode::i()->ProcessCreateStage( sb );
	create_gmap(StageUid);
	create_gord(StageUid);
}

void gstatemgr::ProcessDestroyRoundJumpStage()
{
	m_stage = nullptr;
	m_nStageState = GAMESTAGE_STATE_DOMI_NEW_FLOW;

	destroy_gord();
	destroy_gmap();
	destroy_gmode();

	obj_explosion::i()->ProcessDestroyStage();

	I3_SAFE_RELEASE(m_ProfileLog);
	I3_SAFE_RELEASE(m_pgStatemgr_AvatarChange);
}
#endif

void gstatemgr::Process_InitObject( void)				// stagebattle::_InitObject ���θ� ���� �ű�..
{
		
	CGameObjectManager::i()->DestroyObject();
	// Object�� GameContext�� �˷��ݴϴ�.
	StageObjectListPtr pObjectList	= m_stage->GetStageObjectList();
	if( pObjectList == nullptr)
		return;

	size_t	nCnt = pObjectList->size();
	if( nCnt <= 0)
		return;

	obj_explosion::i()->Process_InitObject(nCnt);

	for( size_t i = 0; i < nCnt; i++ )
	{
		i3Object * pObj = (*pObjectList)[i];
		BattleObjContext::i()->AddObject( pObj );

		if( i3::kind_of<CGameControledObject*>(pObj))
			((CGameControledObject*)pObj)->InitObject();

		bool bTransparentCheck = false;
		for( INT32 j = 0; j < pObj->getAttachNode()->GetChildCount(); j++)
		{
			i3Node * pNode = pObj->getAttachNode()->GetChild( j);

			if( pNode->IsFlag( I3_NODEFLAG_TRANSPARENT))
			{
				bTransparentCheck = true;
				break;
			}
		}

		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		{
			CTutorialMode* mod = gmode::i()->GetTutorialMode();
			if(mod != nullptr && pObj->getNetIndex() >= 0x1b && pObj->getNetIndex() <= 0x20)
			{
				mod->PushDrum(pObj);
			}
			if(i3::same_of<CTargetInTutorial*>(pObj))
			{
				((CTargetInTutorial*)pObj)->ResetObjectOnInit();
			}
		}

		if( bTransparentCheck )
		{
			pObj->getSceneObject()->AddFlag( I3_NODEFLAG_TRANSPARENT);
		}

		gmap::i()->Process_InitObject(pObj);
	}

	_ChangeObjectSpecificName();
}

void gstatemgr::_ChangeObjectSpecificName()
{
	if( GetCodePage() == I3_LANG_CODE_KOREAN)
		return;

	// Object�� GameContext�� �˷��ݴϴ�.
	StageObjectListPtr pObjectList	= m_stage->GetStageObjectList();
	if( pObjectList == nullptr)
		return;

	size_t	nCnt = pObjectList->size();
	if( nCnt <= 0)
		return;

	for(size_t i = 0; i < nCnt; i++ )
	{
		i3Object * pObj = (*pObjectList)[i];
		if( ! pObj->hasName())
			continue;
		gord::i()->ChangeObjectSpecificName(pObj);
	}
}

void gstatemgr::ProcessPreUpdate(REAL32 fDeltaTime)
{
	(this->*m_init_update_fn)();			// ù������Ʈ���� ���� ����..
	
	ub::ApplyLRHandFromEnvSet();			// �̰��� �ִ°� ����ġ �ʴµ�...�ǽð�������Ʈ���ٴ� �ʿ��� ������ ���������� ���߿� �����ϴ°� ����..

	obj_explosion::i()->ProcessUpdate(m_stage);
	
	gstatemgr::_ProcessGameContext(fDeltaTime);

}

void gstatemgr::ProcessUpdate(REAL32 fDeltaTime)
{
	// ���뷱�� �޼��� ���	:::: ���� �ø��� ������ �ø��ϴ�.		// �̰� �� ���� ����?? (12.01.17.����) .... �����ؼ� �����Ұ�...
	if (MyRoomInfoContext::i()->IsTeamBalanceMessageFlag())
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_ACTIVE_TEAM_BALANCE"));/*{col:255,120,0,255}�� �뷱�� �ɼǿ� ���� �� �� �ο����� ���߱� ���� �ڵ����� �����Ǿ����ϴ�.{/col}*/
		
		MyRoomInfoContext::i()->SetTeamBalanceMessageFlag(false);
	}

	// Key : Clan Season End Notice
	if( ChannelContext::i()->GetBattleChannel() )
	{
		i3::rc_wstring wstr_notice; 
		if( ClanSeasonContext::i()->CheckToSeasonEndNotice(wstr_notice) )
		{
			CHud::i()->SetMissionNoticeString(wstr_notice);
		}
	}

#if !defined( I3_NO_PROFILE)
	g_pFramework->UpdateCharaState();					// ��������.....
#endif
	
	if( CHud::i() != nullptr)
		CHud::i()->ShowHUDNick();
	
	gmode::i()->ProcessUpdate(fDeltaTime);

	bool bEnableShowScore = true;		// ����ǥ�� �Һ����� ������Ʈ���ݿ��� ����ǰ�, ���߿� ���ǺбⰡ �ݿ��˴ϴ�..(12.01.13.����)
	gstatemgr::_ProcessUpdateState(fDeltaTime, bEnableShowScore);
	gstatemgr::_UpdateRespawnUI(fDeltaTime);
	gstatemgr::_UpdateShowScore(fDeltaTime, bEnableShowScore);
	gstatemgr::_UpdateWeaponLRU(fDeltaTime);

	CAwayInput::i()->Process_Update_OnCheckAwayInput(fDeltaTime);

	update_map_sector::i()->call(m_stage);

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)//����� Ű
	gstatemgr::_ToggleUpdateInput();
	log_respawn::i()->AddDeathAckTime(fDeltaTime);
#endif
	// AI ��� NPC ������
	gstatemgr::_AutoRespawnAiCharacter( fDeltaTime);			// ���и��ݿ������ʹ� �������� �����Ͽ� ��������..
	//
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) && gmode::i()->GetTutorialMode() != nullptr)
	{
		gmode::i()->GetTutorialMode()->UpdateTutorialState(fDeltaTime);
	}
	else if(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
	{
		// Madness ��� ������Ʈ

	}

	m_ProfileLog->UpdateFrame( fDeltaTime);
}

void	gstatemgr::_UpdateRespawnUI(REAL32 fDeltaTime)
{
	if (m_bRespawnEnable == false) return;

	m_fRespawnDelayTime += fDeltaTime;

	log_respawn::i()->AddRespawnTime(fDeltaTime);

	CHUD_Scope::instance()->SetEnableSinperScope( false );

	if( m_fRespawnDelayTime > 3.0f)
	{
		if( isPlayingBattle())
			CBattleGui::i()->SetEnableGui( true, GAME_GUI_RESPAWN, true);
		else
		{// revision 50214 
			//GAMESTAGE_STATE_BATTLE �� �ƴ� ���¿��� ������ �Ǵ� ���� ����. (hansoft.1293)
			m_bRespawnEnable = false;
			m_fRespawnDelayTime = 0.0f;
		}

		{// ���й̼� �� �ʱ�ȭ �ȵǴ� �κ��� �־� ������ �Ѵ�. 
			if( HUD::instance())
			{
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_RELEASE_HOLD);
				HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, (INT32)HUD::RT_START);
			}
		}
	}
}

void	gstatemgr::_UpdateShowScore(float fDeltaTime, bool bEnableShowScore)
{

	UIBattleFrame* bf =	UIBattleFrame::i();
	if( bf != nullptr)
	{
		if ( bf->IsOpenPopup(UBP_INGAME_SHOP) )
			bEnableShowScore = false;
	}

	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		bEnableShowScore = false;


	if (LocaleValue::Enable("TabMinimap"))
	{
		// �� �̴ϸ� ���� : 2011.11.04. ugeun.ji
		if (bEnableShowScore)
			tu::ProcessInput();
	}
	else
	{
		if( bEnableShowScore)
		{
			UINT64 nPressKey	= g_pFramework->getKeyPress();

			// ���� ���� �߿��� ǥ������ ����.
			if( (nPressKey & GAME_KEY_CMD_SHOWSCORE) || (g_pFramework->getSysKeyPress() & GAME_KEY_TAB)) {
				_UpdateScore( true, fDeltaTime);
			}
			else {
				_UpdateScore( false, fDeltaTime);
			}
		}
	}

}

void	gstatemgr::_ToggleUpdateInput(void)
{
	
	UIBattleFrame* frame = UIBattleFrame::i();
	if (frame)
	{
		if (!frame->IsPopupClosedAll())		// �ű�UI�˾��� ��µǴ� ������ �ϴ� ���´�..
			return;
	}

	i3InputDeviceManager* pInputMgr = g_pViewer->GetInputDeviceManager();
	i3InputKeyboard* pKey = pInputMgr->GetKeyboard();
//	char szString[MAX_STRING_COUNT] = "";

	//// 1.5 Check Debug - �����е�� ĳ���� �����ϴºκ� �ּ�ó��
	/***********************************************
	if( pKey->GetStrokeState( I3I_KEY_NUMPAD1))
	{
		_ToggleChara(1);
	}
	else if (pKey->GetStrokeState(I3I_KEY_NUMPAD2))
		_ToggleChara(2);
	{
	}
	else if (pKey->GetStrokeState(I3I_KEY_NUMPAD3))
	{
		_ToggleChara(3);
	}
	else if (pKey->GetStrokeState(I3I_KEY_NUMPAD4))
	{
		_ToggleChara(4);
	}
	else if (pKey->GetStrokeState(I3I_KEY_NUMPAD5))
	{
		_ToggleChara(5);
	}
	else if (pKey->GetStrokeState(I3I_KEY_NUMPAD6))
	{
		_ToggleChara(6);
	}
	else if (pKey->GetStrokeState(I3I_KEY_NUMPAD7))
	{
		_ToggleChara(7);
	}
	else if (pKey->GetStrokeState(I3I_KEY_NUMPAD8))
	{
		_ToggleChara(10);
	}
	else if (pKey->GetStrokeState(I3I_KEY_NUMPAD9))
	{
		_ToggleChara(15);
	}

	if( pKey->GetPressState( I3I_KEY_LCTRL))
	{
		if( pKey->GetStrokeState( I3I_KEY_NUMPAD1))
		{
			_ToggleChara(16);
		}
		else if (pKey->GetStrokeState(I3I_KEY_NUMPAD2))
		{
			_ToggleChara(8);
		}
		else if (pKey->GetStrokeState(I3I_KEY_NUMPAD3))
		{
			_ToggleChara(9);
		}
		else if (pKey->GetStrokeState(I3I_KEY_NUMPAD4))
		{
			_ToggleChara(11);
		}
		else if (pKey->GetStrokeState(I3I_KEY_NUMPAD5))
		{
			_ToggleChara(12);
		}
		else if (pKey->GetStrokeState(I3I_KEY_NUMPAD6))
		{
			_ToggleChara(13);
		}
		else if (pKey->GetStrokeState(I3I_KEY_NUMPAD7))
		{
			_ToggleChara(14);
		}
	}
	********************************************************/

	if (pKey->GetStrokeState(I3I_KEY_NUMPAD0))
	{
		ub::toggle_3p_camera();
	}

	if (pKey->GetStrokeState(I3I_KEY_HOME))
	{
		getMyChara()->getMoveContext()->EnableSpeedReduction(2.0f, 10.0f);
	}

	if (pKey->GetStrokeState(I3I_KEY_END))
	{	
		if (getMyChara()->getMoveContext()->isGravity() == false)
		{
			getMyChara()->getMoveContext()->SetGravity(true);
			i3::rc_wstring wstrString;
			i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", L"Gravity ON");

			g_pFramework->getChatBox()->PutSystemChatting( wstrString);
		}
		else
		{
			i3::rc_wstring wstrString;
			i3::sprintf( wstrString, L"{col:255,120,0,255}%s{/col}", L"Gravity OFF");

			g_pFramework->getChatBox()->PutSystemChatting( wstrString);
			getMyChara()->getMoveContext()->SetGravity(false);
		}
	}

	if (pKey->GetStrokeState(I3I_KEY_DEL))
	{
	}

	if (pKey->GetStrokeState(I3I_KEY_F8))
	{
		i3TextNodeDX2* skillview = g_pFramework->getCommandSet()->GetSkillView();
		g_pFramework->getCommandSet()->SkillDebugView();
		if( skillview->GetEnable() == true )
			skillview->SetEnable(false);
		else
			skillview->SetEnable(true);
	}

	if (pKey->GetStrokeState(I3I_KEY_F9))
	{
		VEC3D pos;
		gmap::i()->GetPos_Stroke_F9(pos);
		getMyChara()->forceSetPos( &pos );
	}
		
}

void	gstatemgr::_ToggleChara(UINT32 Arg)
{	
	CHARA_RES_ID charaType = (CHARA_RES_ID) ((UINT32)Arg);
	if( charaType != CHARA_RES_ID_UNKNOWN && charaType < CHARA_RES_ID_HUMAN_E)
	{
		UINT32	nCharaUseMap[ SLOT_MAX_COUNT];
		INT32	nUseCharaCount = 0;

		i3mem::FillZero( nCharaUseMap, sizeof( nCharaUseMap));

		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara( i);
			if( pChara == nullptr )	continue;
			if( pChara->isAIToClient()) continue;

			nCharaUseMap[ nUseCharaCount] = pChara->getCharaNetIndex();
			nUseCharaCount++;
		}

		if( nUseCharaCount > 0)
		{
			g_DebugChara[ 0] = charaType;
			g_DebugChara[ 1] = charaType;

			g_pCharaManager->RemoveAllChara();

			for(INT32 i = 0; i < nUseCharaCount; i++)
			{
				CGameCharaBase * pChara = g_pCharaManager->AddChara( nCharaUseMap[ i], g_DebugChara[ nCharaUseMap[ i] %2]);
				if( pChara != nullptr)
				{
					g_pCharaManager->RespawnChara( pChara->getCharaNetIndex(), true, true);
				}
			}
		}
	}
}


void	gstatemgr::_AutoRespawnAiCharacter( REAL32 fDeltaTime)
{
	CGameCharaBase * pChara;

	if( false == (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
		return;

	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		pChara = g_pCharaManager->getCharaByNetIdx(i);

		if( pChara == nullptr) continue;

		if( pChara->isAI() && (pChara->getCurHP() == 0) )
		{
			m_aiCharacterRespawnTimer[i] += fDeltaTime;

			AI_ENTRY* aiEntry = &AIModeContext::i()->AIEntry[i];

			if (aiEntry->_respawnTime < m_aiCharacterRespawnTimer[i])
			{
				I3TRACE("Ai(%d:%d) respawn %f / %f\n", aiEntry->_level, aiEntry->_number, m_aiCharacterRespawnTimer[i], aiEntry->_respawnTime);

				util_battle::AddAiChara(i);
				m_aiCharacterRespawnTimer[i] = 0.0f;
			}
		}
	}	
		
}


namespace
{
	FMOD_RESULT F_CALLBACK  EndSoundProc( FMOD_CHANNEL *  channel, FMOD_CHANNEL_CALLBACKTYPE  type, int  command, unsigned int  commanddata1, unsigned int  commanddata2)
	{
		i3SoundContextFMOD * pCtx = (i3SoundContextFMOD *) g_pSndMng->GetSndCtx();
		if( pCtx == nullptr)
		{
			return FMOD_OK;
		}

		
		FMOD::Channel * pChannel = (FMOD::Channel *) channel;

		i3SoundPlayStateFMOD * pState;
		pChannel->getUserData( (void **) &pState);
		I3ASSERT( pState != nullptr);

		INT32 idx;
		size_t sidx;

		sidx = i3::vu::index_of(pCtx->m_UsedPlayStateList, pState);
		
		idx = (sidx != pCtx->m_UsedPlayStateList.size() ) ? INT32(sidx) : -1;

		if( idx != -1)
		{
			pCtx->m_UsedPlayStateList.erase( pCtx->m_UsedPlayStateList.begin() + idx);
		}

		sidx = i3::vu::index_of( pCtx->m_FreePlayStateList, pState);

		idx = (sidx != pCtx->m_FreePlayStateList.size() ) ? INT32(sidx) : -1;

		if( idx == -1)
		{
			pCtx->m_FreePlayStateList.push_back( pState);
		}

		pState->Reset();

		// ���ӹ濡�� ���� �÷��� �Ǵ� �� ���� ����
		if(!MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && g_pFramework->GetState() == i3Framework::STATE_SCENE)
		{
			g_pSndMng->StopBGM(1.f);
			g_pSndMng->StartBGM( "Sound/BackGround/BreedingNest_loop.ogg", true);
		}
		else if(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && g_pFramework->GetState() == i3Framework::STATE_SCENE)
		{
			g_pSndMng->StopBGM(1.f);
			g_pSndMng->StartBGM( "Sound/BackGround/Madness_BGM.ogg", true);
		}

		return FMOD_OK;
	}
	
}


void	gstatemgr::_OnFirstSceneUpdate()
{
	
	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
	{
		g_pSndMng->StartBGM( "Sound/BackGround/BreedingNest.ogg", true, false, 2.0f, EndSoundProc);				// ���������� ���� �� ����� �ڵ�..
	}
	if (MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
	{
		g_pSndMng->StartBGM( "Sound/BackGround/Madness_BGM.ogg", true, false, 2.0f, EndSoundProc);
	}

	hd::check_elapsedsynctimer::i()->ResetTimer();
	m_init_update_fn = &gstatemgr::_Function_Null;
}

void gstatemgr::_ProcessTabminimapEvent(GAME_EVENT nEvent, INT32 Arg, const i3::user_data& UserData)
{

	if (LocaleValue::Enable("TabMinimap"))
	{
		UIBattleFrame* pFrame = UIBattleFrame::i();/*g_pFramework->GetUIBattleFrame();*/

		if (pFrame)
		{
			UIHUDTabMinimap* pTapMinimap = static_cast<UIHUDTabMinimap*>(pFrame->GetHUD(UIHUD_TABMINIMAP));
			
			if (pTapMinimap)
			{
				if (pTapMinimap->isEnable())
					pTapMinimap->ProcessGameEvent(nEvent, Arg, UserData);
			}
		}
	}

}

void gstatemgr::_ProcessGameContext(REAL32 fDeltaTime)
{
	GAME_EVENT nEvent;
	INT32 Arg;
	i3::user_data	UserData;

	// UDP�� �� ���� ��� TCP�� ó�����ݴϴ�. �׽� ������Ʈ �Ǿ���մϴ�.
	g_pCharaManager->UpdateReserveDeath( fDeltaTime );

	i3::unordered_map< GAME_EVENT, evt_fn >::iterator it;

	while ( GameEventReceiver::i()->PopRecvData(nEvent, Arg, UserData) )
	{
	
		if (LocaleValue::Enable("TabMinimap"))
		{
			tu::ProcessGameEvent(nEvent, Arg, UserData);
		}

		it = m_evt_fn_map.find(nEvent);

		if ( it != m_evt_fn_map.end() )
		{
			evt_fn& mf = it->second;	
			(this->*mf)(Arg, UserData);
			if( HUD::instance() != nullptr)
				HUD::instance()->OnEvent(nEvent, Arg, UserData);
		}
		else
		{
			_Process_EVENT_default(nEvent, Arg, UserData);
			if( HUD::instance() != nullptr)
				HUD::instance()->OnEvent( EVENT_NOTHING, Arg, UserData);	//default �� ȣ���ϱ� ���ؼ� EVENT_NOTHING �� ����
		}
	}
}

// ������ gstatemgr�� �̺�Ʈ�� �����ϰ� �Ҷ� ���� (�������� �������� �ʿ�)
void gstatemgr::ForceProcessEvent( GAME_EVENT nEvent, INT32 Arg , const i3::user_data& UserData)
{
	i3::unordered_map< GAME_EVENT, evt_fn >::iterator it;

	it = m_evt_fn_map.find(nEvent);

	if ( it != m_evt_fn_map.end() )
	{
		evt_fn& mf = it->second;	
		(this->*mf)(Arg, UserData);
		if( HUD::instance() != nullptr)
			HUD::instance()->OnEvent(nEvent, Arg, UserData);
	}
	else
	{
		_Process_EVENT_default(nEvent, Arg, UserData);
	}
}


void gstatemgr::_ProcessUpdateState(REAL32 fDeltaTime, bool& bEnableShowScore)
{
	(this->*m_state_fn[m_nStageState])(fDeltaTime, bEnableShowScore);
}


void gstatemgr::_Process_EVENT_BATTLE_ENTERANCE_USER(INT32, const i3::user_data& UserData)
{
	// Observer�� ������ ��忡���� �̸� �� �ִ� ������ ������ �Ѵ�.
	BattleServerContext::i()->Battle_EntranceAnotherUser( CStageInfoUtil::IsObserverModeStage(*MyRoomInfoContext::i()->getStage()) );
}

void	gstatemgr::_Process_EVENT_RENDEZVOUS_HOLEPUNCH_BATTLE(INT32, const i3::user_data& UserData)
{
	NetworkContext::i()->RendezvousHolePunch();
}

void	gstatemgr::_Process_EVENT_PRESTART_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
	UIBattleFrame* bf =	UIBattleFrame::i();
	bf->EnableHUD(UIHUD_MANAGER, true);

	if (UserInfoContext::i()->IsGM_Observer())
	{
		bf->EnableHUD(UIHUD_MANAGER, true);
		UIHudMissionBoard* MissionBoard = (UIHudMissionBoard*)HUD::instance()->GetHud(HUD::MISSION_BOARD);
		if (MissionBoard) MissionBoard->CloseMissionBoard();
	}

	if( Arg > - 1 && Arg < USER_SLOT_COUNT )
	{	// �������� ó�� ���� ������ �˴ϴ�.

		//������ �÷��������� Ȯ���ؾ��մϴ�.
		// ���������Ŀ��� ������ �ȵ�..
		if( BattleServerContext::i()->IsP2PActivate() )
		{
			BattleServerContext::i()->P2PPreStartBattleUser( Arg );
			CAwayInput::i()->SetAwayInputTime(AWAY_INPUT_BATTLE_TIME, AWAY_INPUT_BATTLE_CAUTION);
		}
	}
	else
	{
		if (EV_IS_ERROR(Arg, EVENT_ERROR_PRESTART_BATTLE_ALEADY_END))
		{
			/*������ ����Ǿ� �������� ���Ͽ����ϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_GAME_END"), nullptr, MAKE_CALLBACK(CbFailStart), this);
		}
		else
		{
			// Ȧ��Ī ����
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_GAME_FAIL_START_C"), nullptr, MAKE_CALLBACK(CbFailStart), this);
			I3PRINTF("Fail EVENT_PRESTART_BATTLE\n");
		}
	}

}

void	gstatemgr::_Process_EVENT_START_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
	g_mGameInfoBulletCon.clear();
	g_mGameInfoCharaCon.clear();

	hd::check_observer::i()->init_roominfo();
	
	i3::stack_wstring wstrTemp;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);


	hd::check_PhysixDll::i()->check();

	if( Arg > -1)
	{// ���� �÷��̾� �߰�
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		I3PRINTLOG(I3LOG_NOTICE, "EVENT_START_BATTLE");
		GlobalFunc::PB_TRACE("!!!!!!!! EVENT_START_BATTLE");
#endif

		if( Arg == BattleSlotContext::i()->getMySlotIdx())
		{
			if( CHud::i() != nullptr)
				CHud::i()->ResetHud();
			UIBattleFrame::i()->EnableHUD(UIHUD_INGAMECHATING, true);

			// revision 40646 ���Ĺ̼� �糭�Խ� ������ �ȵǰ� ��Ŀ�ؼ� �Ǵ� ���� ����
			NET_CHARA_INFO * pNetChara = BattleSlotContext::i()->setNetCharaInfo(Arg);
			pNetChara->setRespawnIndex( 0);
			pNetChara->setMyRespawnCount( 0);

			if (UserInfoContext::i()->IsGM_Observer())
			{
				UIHUDIngameChat* chat = (UIHUDIngameChat*)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
				if (chat) chat->SetVisible(false);
			}

		}

		// AI���� ���� Client ó��
		util_battle::StartAiForClient( Arg);

		bool Rv = false;

		Rv = BattleServerContext::i()->P2PStartBattleUser( Arg, true );	// ĳ���͸� ����մϴ�.

		if( Rv && IntrudeContext::i()->IsIntrusionMessage() && (CHud::i() != nullptr))
		{
			// ���� �޽���
			i3::wstring wstrNick;
			CHud::i()->EditCharaNick( wstrNick, BattleSlotContext::i()->getNickForSlot(Arg), 160);
			i3::sprintf( wstrTemp, L"%s%s", wstrNick, GAME_RCSTRING("STBL_IDX_GAME_INTRODUCE_PLAYER"));

			CHud::i()->CreateInGameMessage(wstrTemp);
			if( HUD::instance() != nullptr)
				HUD::instance()->SetParameter(HUD::GAME_RESULT, HUD::GRS_UserRoomIn, Arg);

			if( UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_CHK_VISIBLE, dui::intermission))
			{//���͹̼� �� ������ ������ ��� ��� ǥ��.
				domi_state_info* info = domi_state_info::i();
				info->grade_state = DOMI_REWARD_GRADE_STATE_USER_ENGER;
				info->grade_user_nick = BattleSlotContext::i()->getNickForSlot( Arg);
				UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_GRADE);
				UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_USER_STATE, dus::USER_STATE_E_SYNC);
			}

			// keyword : Star Player
			MyRoomInfoContext::i()->PrintStarPlayerEntranceMsg(Arg);
		}

		//
		HackContext::i()->resetHackUserSlot( Arg);

		if( Arg == BattleSlotContext::i()->getMySlotIdx() )
		{
			if( CStageInfoUtil::IsObserverModeStage(*MyRoomInfoContext::i()->getStage()) && 
				( UserContext::i()->IsPlayingSlot( Arg) == 0 ) )
			{
				//observer_battle::i()->init_observer(0);//, false);	// �����ϸ� �������� ��ȯ
				observer::SetIntrudeViewState();

				if(CHud::i() != nullptr)
					CHud::i()->SetEnableMissionNotice( false);

				// �ٸ� ������ �÷��� ���¿� ���� Visible�� Ų��. 
				// @eunil. GM�������� visible ������
				for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
				{
					if( Arg != i && UserContext::i()->IsPlayingSlot( i))
					{
						CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( i);
						
						if( pChara != nullptr)
						{
							// ���� ���� ��, ���� ������ ���� �ʾ�, ������ ���� �Ҷ� ���������� �������ݴϴ�.
							if( pChara->isCharaStateMask(CHARA_STATE_FIRSTRESPAWN) == false )
							{
								CHARA::TYPE type = BattleSlotContext::i()->getCharaType(i);
								const NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->getNetCharaInfo(i, CHARA::Type2BufferType(type) );
								g_pCharaManager->SetCharaWeaponSet(pChara, pNetCharaInfo->getWeaponList(WEAPON_SLOT_PRIMARY));
							}

							pChara->addCharaStateMask( CHARA_STATE_FIRSTRESPAWN);

							if (BattleSlotContext::i()->IsGMObserverForClient(i))
								pChara->Cmd_Visible(false);
							else
							{
								pChara->Cmd_Visible(true);
								pChara->Cmd_Idle();
							}
						}
					}
				}
			}
			else
			{
				//observer_battle::i()->set_my_idx_iterator(); 

				if(CHud::i() != nullptr )
					CHud::i()->SetHudMode( HUD_MODE_DEFAULT);
				hd::check_elapsedsynctimer::i()->ResetTimer();
			}

			// Start Round�� ������ �� �ð����� ������ �� ���忡���� ��ġ����
			// ���� ���忡���� ���������� �ʾƼ� ���ӿ� ������ �־
			// �������� �� GAMESTAGE_STATE_BATTLE�� �����ϴ� ������ �����Ͽ��µ�w
			// �����Ͽ��� ���� GAMESTAGE_STATE_BATTLE�� �����ʴ� ���װ� �߻��Ͽ� 
			if(IntrudeContext::i()->IsIntrusionMessage())
			{
				m_nStageState = GAMESTAGE_STATE_BATTLE;
			}
			

			// �̼Ǹ�带 �ʱ�ȭ����� �մϴ�.
			pMissionMng->setMissionState( GAME_MISSION_STATE_INBATTLE);

			// Ʈ���̴׸�� ���۽� ��� �޼��� ���
			if ( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) )
			{
				if( Arg == BattleSlotContext::i()->getMainSlotIdx())
				{
					util_battle::AddAiCharacter();
				}
			}
			CBattleGui::i()->SetEnableGui( false, GAME_GUI_BATTLE_START);
		}
		else
		{
			//���԰� ���ÿ� �÷��� ������ ��������(���)�� GM OBSERVER �� �۵��� �� ������ ���ö� ����.
			if (UserInfoContext::i()->IsGM_Observer()
				&& !CStageInfoUtil::IsObserverModeStageByOnlyMode(*MyRoomInfoContext::i()->getStage()))
			{
				if (CHud::i() && observer::Context::i()->IsGMObserverHUDOff() == false)
				{
					observer_hud::Context::i()->Notify(observer_hud::EVT_ENABLE, true);
				}
			}
		}
	}
	else
	{
		if (EV_IS_ERROR(Arg,EVENT_ERROR_START_BATTLE_ALEADY_END)) 
		{
			/*������ ����Ǿ� �������� ���Ͽ����ϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_GAME_END"), nullptr, MAKE_CALLBACK(CbFailStart), this);
		}
		else
		{
			//���ӽ��� ����
			if( BattleServerContext::i()->IsP2PHost() == false )
			{
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_FAIL_START_C"), nullptr, MAKE_CALLBACK(CbFailStart), this);
			}
		}
	}

	if ( hd::check_mapdata::i()->is_hacked() ) 
	{
		// LogD �߰� �Ǹ� �����޽��� �������� ó���ϱ�.
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_ERROR_MAP_DATA_HACKING"), nullptr, MAKE_CALLBACK(CbFailStart), this);
	}

//	check_elapsedsynctimer::i()->InitMagnet();
}

void	gstatemgr::_Process_EVENT_GIVEUP_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
		// Ȧ��Ī ����� ������ ��쿡�� Arg�� ������ ���ֵȴ�.
	INT32 slot = Arg;

	if( Arg > -1 )
	{	
		if( Arg != BattleSlotContext::i()->getMySlotIdx() )
		{
			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( Arg);
			CGameCameraManager::i()->InitCameraByGiveupChara( pChara);
		}
		else
		{
			UIBattleFrame* frame = UIBattleFrame::i();
			if (frame)
			{
				frame->SetGameEnd();			// ��Ʋ����ÿ��� ����� ����..
			}

			CAwayInput::i()->getOffAwayInput();
		}
	}
	else
		slot = BattleSlotContext::i()->getMySlotIdx();

	// Object Event�� ������ UserPointer�� �����ؾ� �մϴ�. 2014.10.29 swoongoo.jung
	if( CGameMissionManager::i()->getMissionState() != GAME_MISSION_STATE_ENDBATTLE)
	{
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( slot);
		BattleObjContext::i()->ResetEventHandler( pChara);
	}

	BattleServerContext::i()->P2PGiveupBattleUser( slot );

	// User�� ������ Mission�� Data�� �ʱ�ȭ���ش�.
	// �߰��� ���� �ڸ��� Slot���� �����ϴ� ��� ������ �� �� �ֱ⿡ �ʱ�ȭ
	if( slot != BattleSlotContext::i()->getMySlotIdx())
	{
		CGameMissionManager::i()->GiveupBattle( slot);
		NetworkContext::i()->GiveupBattle( slot);
	}

	UINT16 flag = 1 << slot;

	// Playing ���¸� ����
	UserContext::i()->PlayingSlotFlag &= ~flag;

	// ����ÿ� ���� ī�޶� �ʱ�ȭ �ؾ� �մϴ�.
	//observer::Context::i()->ProcessEventGiveupBattle( slot);
	if ( CStageInfoUtil::IsObserverModeStageByOnlyMode(*MyRoomInfoContext::i()->getStage()) )
	{
		if (observer::Context::i()->GetCurrIdx() == slot)
			observer::Context::i()->ChangeViewTarget(BattleSlotContext::i()->FindAnyoneAlive());

		observer_hud::Context::i()->Notify(observer_hud::EVT_GIVEUP_BATTLE, slot);
	}
	
	//���԰� ���ÿ� �÷��� ������ ��������(���)�� GM OBSERVER �� �۵��� �� ������ ������ ����.
	if (UserInfoContext::i()->IsGM_Observer()
		&& !CStageInfoUtil::IsObserverModeStageByOnlyMode(*MyRoomInfoContext::i()->getStage()))
	{
		if (CHud::i() && observer::Context::i()->IsGMObserverHUDOff() == false)
		{
			observer_hud::Context::i()->Notify(observer_hud::EVT_GIVEUP_BATTLE, slot);
		}
	}

	// ���� �޽���
	if(CHud::i() != nullptr)
	{
		i3::stack_wstring wstrTemp;
		i3::wstring wstrNick;

		CHud::i()->EditCharaNick( wstrNick, BattleSlotContext::i()->getNickForSlot( slot), 150);
		i3::sprintf( wstrTemp, L"%s%s", wstrNick, GAME_RCSTRING("STBL_IDX_GAME_BYE_PLAYER"));
		CHud::i()->CreateInGameMessage(wstrTemp);

		CHUD_Revenge::instance()->DeleteRecordRevenge( slot);		// 1:1 �������� ��� ����
		CRevengeManager::i()->ResetRevengeUser( slot);
		if( HUD::instance() != nullptr)
			HUD::instance()->SetParameter(HUD::GAME_RESULT, HUD::GRS_UserRoomOut, slot);
	}

	ScoreContext::i()->ResetIndividualScore(slot);

	HackContext::i()->resetHackUserSlot( slot);
	
	gmode::i()->ProcessEventGiveUpBattle( Arg);

	I3TRACE("[NOTICE] EVENT_GIVEUP_BATTEL\n");
}

void	gstatemgr::_Process_EVENT_DEATH_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
	const RecvData_EVENT_DEATH_BATTLE& data = i3::value_cast<RecvData_EVENT_DEATH_BATTLE>(UserData);
	
	INT32 KillIdx		= data.KillIdx;
	INT32 nPreKillCount	= data.MyPrevKillCount;		// ���� ���� �������� ���� ī��Ʈ
	INT32 nNowKillCount	= data.MyKillCount;			// ���� �������� �ѹ��� ���� ī��Ʈ
	INT32 nKillMessage	= data.MyKillMsg;
	INT32 AssistIdx		= data.DeathCharaInfo[0]._ui8AssistIdx;
	
	bool bHeadShot = false;

	// AI ��� ���� ���� ���� â - ���� ų ��
	static INT32 chainKillCount = 0;

	CGameMissionManager * pMissionMng = CGameMissionManager::i();

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( data._KillWeaponFlag, true );

	UINT8 ui8ExtState = (UINT8) GET_ITEM_EXT_STATE(data._KillWeaponFlag);	// ������ Extension State 
	if(ui8ExtState > 0)
	{
		//  ������ Shape�� ����ϴ� ��쿡�� ������ ������
		if( pWeaponInfo->isUseCompatibleShape(ui8ExtState - 1) )
		{
			CWeaponInfo * pExtInfo = g_pWeaponInfoDataBase->getWeaponInfo( pWeaponInfo->GetCompatibleItemID(ui8ExtState - 1) );
			if( pExtInfo != nullptr)
				pWeaponInfo = pExtInfo;
		}
	}

	if( pWeaponInfo == nullptr )
	{
		// Error �Դϴ�.
		I3ASSERT( GAME_STRING("STR_TBL_INGAME_HUD_ERROR_I3TRACE_WRONG_WEAPON_INFO") );/*Event_Death_Battle. �߸��� ���� �����Դϴ�.\n*/
		return;
	}

	CChainKills::instance()->Set_ChainKill_WeaponInfo(pWeaponInfo);
	UINT32 WeaponSlot = pWeaponInfo->GetTypeUsage();

	//�ӹ�ī�带 ó�������� ����
	bool bProcessQuest = true;
	if( (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
	{
		bProcessQuest = false;
	}

#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
		bProcessQuest = false;
#endif

	// Kill ���� ó�� ( | chain kills )
	if (BattleSlotContext::i()->getMySlotIdx() == KillIdx)
	{
		// AI ��� ���� ���� ���� - ���� ų�� �ʱ�ȭ
		const SCORE_INFO* pScoreInfo = ScoreContext::i()->getIndividualScore(KillIdx);

		if (pScoreInfo->m_KillCount == nNowKillCount)
		{
			// ���ݱ��� ų ���� ��� ų ���� ���ٸ�, ���� ���� ų
			chainKillCount = 0;
		}

		//ų�޽������� ó���ϰ�, �ӹ�ī�带 ó���ؾ� ������ �½��ϴ�.
		{ //ų�޽��� ó��
			INT32 chainHeadshotCount = 0;
			INT32 chainStopperCount = 0;
			
			const i3::vector<DEATH_INFO_CHARA>& vecDeathInfo = data.DeathCharaInfo;
			const size_t nDeathCount = vecDeathInfo.size();

			for( UINT32 j = 0; j < nDeathCount; j++)
			{
				const DEATH_INFO_CHARA * pInfo = &vecDeathInfo[j];
				INT32 DeathIdx = GET_DEATHINFO_IDX_TYPE( pInfo->_DeathInfo );

				CGameCharaBase * pKiller = g_pCharaManager->getCharaByNetIdx( KillIdx);
				CGameCharaBase * pDeath = g_pCharaManager->getCharaByNetIdx( DeathIdx);

				if( pKiller == nullptr) continue;
				if( pDeath == nullptr) continue;
				if( pKiller == pDeath) continue;	// 54546  ��ų�� �޽��� ���� ����

				CChainKills::instance()->Set_Killer_Victim_Chara(pKiller, pDeath);
				if( KillIdx != DeathIdx && HUD::instance() ) //�ڻ��� ������
				{
					HUD::instance()->SetParameter(HUD::GAME_RESULT, HUD::GRS_UserKilled, DeathIdx);
				}

				if( LocaleValue::Enable("EnableRevengeMode") == true)
				{
					I3TRACE("DeathIdx : %d\n", DeathIdx);
					CRevengeManager* m = CRevengeManager::i();
					m->SetDeathUserNick(BattleSlotContext::i()->getNickForSlot(DeathIdx));
					m->AddKillDeath(DeathIdx);

					if( m->isLoveShot(DeathIdx) == true && pKiller->isCharaStateMask(CHARA_STATE_DEATH) == true)// �ϴ� ���� ���� �� ���� �������� Ȯ��
					{
						if(CHud::i() != nullptr)
							CHud::i()->AddChainKills(REVENGE_TYPE_LOVESHOT, 0, 1, 0, 0, 0, true);
					}

					m->SetLoveShotIdxAfter(DeathIdx);		// ���� ������ ���̸� ���꼦�� ���
				}

				if( pKiller->getCharaTeam() != pDeath->getCharaTeam())
				{
					if (0 != nNowKillCount)
					{
						if (TESTBIT(vecDeathInfo[j]._DeathInfo, KILLING_MESSAGE_CHAINHEADSHOT))
						{
							chainHeadshotCount++;
						}

						if (TESTBIT(vecDeathInfo[j]._DeathInfo, KILLING_MESSAGE_HEADSHOT))
						{
							if( HUD::instance() != nullptr)
								HUD::instance()->SetParameter(HUD::GAME_RESULT, HUD::GRS_HeadShot);
						}

						if (TESTBIT(vecDeathInfo[j]._DeathInfo, KILLING_MESSAGE_CHAINSTOPPER))
						{
							chainStopperCount++;
						}
					}
				}
			}

			// AI ��� ���� ���� ���� â - ų ī��Ʈ
			{
				INT32 tmChainKillCount = chainKillCount + nNowKillCount;

				if (5 < (chainKillCount + nNowKillCount) && chainKillCount < 5)
				{
					tmChainKillCount = 5;
				}
				else if (20 < (chainKillCount + nNowKillCount) && chainKillCount < 20)
				{
					tmChainKillCount = 20;
				}

				chainKillCount += nNowKillCount;

				if( HUD::instance() != nullptr)
				{
					switch(tmChainKillCount)
					{
					case 5:		HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Challenge, HUD::CHM_5_CHAIN_KILL); break;
					case 20:	HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Challenge, HUD::CHM_20_CHAIN_KILL); break;
					}
				}
			}

			//chain headshot�̳� chainstopper���ǿ� ������ chainkills�߰�
			if( (0 != nNowKillCount) || (chainHeadshotCount > 0) || (chainStopperCount > 0))
			{
				CRevengeManager* m = CRevengeManager::i();
				if (CHud::i() != nullptr)
				{
					I3TRACE("Its Special KillShot.\n");
					CHud::i()->AddChainKills(nKillMessage, nPreKillCount, nNowKillCount, WeaponSlot, chainHeadshotCount, chainStopperCount);
					//ī���� �޼���
					if (MyRoomInfoContext::i()->getStage()->IsHudOff())
						CHud::i()->AddChainKills(REVENGE_TYPE_AWESOME, 0, 1, 0, 0, 0, true);

					CHud::i()->AddChainKills(m->GetCurrentMessage(), 0, 1, 0, 0, 0, true);
				}

				if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_KNUCKLE )
				{
					if( pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::PUMPKIN_KNUCKLE) )
					{
						GSND_TYPE_ANNOUNCE announce = (GSND_TYPE_ANNOUNCE)0;

						if( (nKillMessage & KILLING_MESSAGE_CHAINSTOPPER) ||
							(nKillMessage & KILLING_MESSAGE_CHAINHEADSHOT) ||
							(nKillMessage & KILLING_MESSAGE_CHAINSLUGGER) )
						{
							announce = GTA_PUMPKIN_KNUCKLE_CHAIN_KILLER;
						}
						else if( getMyChara()->getDBInfo()->GetCharaSexTypeDISPLAY() )
						{
							announce = GTA_PUMPKIN_KNUCKLE_FEMALE;
						}
						else
						{
							announce = GTA_PUMPKIN_KNUCKLE_MALE;
						}

						g_pSndMng->StartAnnounceMessage( announce );
					}
					else if(pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::GARENA_KNUCKLE))
					{
						GSND_TYPE_ANNOUNCE announce = (GSND_TYPE_ANNOUNCE)0;
						announce = GTA_GARENA_KNUCKLE_HIT;
						g_pSndMng->StartAnnounceMessage( announce );
					}
				}
				
				// Kill Sound �߰�
				if (pWeaponInfo->GetKillSoundIndex() != -1)
				{
					g_pSndMng->StartWeaponKillSound(pWeaponInfo->GetKillSoundIndex());
				}
				//else if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SHOTGUN && pWeaponInfo->GetNumber() == GetItemIndex("SHOTGUN_ZOMBIE_SLAYER"))//���񽽷��̾�
				//{
				//	GSND_TYPE_ANNOUNCE announce = (GSND_TYPE_ANNOUNCE)0;
				//	announce = GTA_HALLOWEEN_KILL;
				//	g_pSndMng->StartAnnounceMessage( announce );
				//}
				//else if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_KNIFE && pWeaponInfo->GetNumber() == GetItemIndex("KNIFE_KUNAI"))//�������̵�
				//{
				//	GSND_TYPE_ANNOUNCE announce = (GSND_TYPE_ANNOUNCE)0;
				//	announce = GTA_HALLOWEEN_KILL;
				//	g_pSndMng->StartAnnounceMessage( announce );
				//}
				//else if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SHOTGUN && pWeaponInfo->GetNumber() == GetItemIndex("SHOTGUN_CERBERUS"))//�������̵�
				//{
				//	GSND_TYPE_ANNOUNCE announce = (GSND_TYPE_ANNOUNCE)0;
				//	announce = GTA_CAREBERUS_KILL;
				//	g_pSndMng->StartAnnounceMessage( announce );
				//}

				// AI ��� ���� ���� ���� â - ų �޽���
				if( HUD::instance() != nullptr)
				{
					if (nKillMessage & KILLING_MESSAGE_PIERCINGSHOT)
					{
						HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Challenge, HUD::CHM_PIERCING_KILL);
					}
					else if (nKillMessage & KILLING_MESSAGE_MASSKILL)
					{
						HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Challenge, HUD::CHM_MASS_KILL);
					}
					else if (nKillMessage & KILLING_MESSAGE_CHAINSLUGGER)
					{
						HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Challenge, HUD::CHM_CHAIN_SLUGGER);
					}
					else if (nKillMessage & KILLING_MESSAGE_CHAINSTOPPER)
					{
						HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Challenge, HUD::CHM_CHAIN_STOPPER);
					}
				}
			}
		}
	}
	else if (BattleSlotContext::i()->getMySlotIdx() == AssistIdx)
	{
		CHud::i()->AddChainKills( KILLING_MESSAGE_ASSIST, 0, 1, 0, 0, 0);
	}

	// �ѹ��� ����ĳ���� ����ŭ �ݺ� ó��

	const i3::vector<DEATH_INFO_CHARA>& vecDeathInfo = data.DeathCharaInfo;
	const size_t nDeathCount = vecDeathInfo.size();

	for( UINT32 j = 0; j < nDeathCount; j++)
	{
		//UINT16* pInfo = infoAdd._nDeathIdx;
		const DEATH_INFO_CHARA * pInfo = &vecDeathInfo[j];

		//	// 0x00	0F		FF		F		F
		//	//		defence	message	type	index
		INT32 DeathSupprot	= GET_DEATHINFO_ENEMY_TYPE( pInfo->_DeathInfo );
		INT32 DeathType		= GET_DEATHINFO_DEATH_TYPE( pInfo->_DeathInfo );
		INT32 DeathIdx		= GET_DEATHINFO_IDX_TYPE(pInfo->_DeathInfo);
		INT32 AssistUserIdx = (INT32)pInfo->_ui8AssistIdx;

		bool bDefenceObj = false;
		//Key:EscapeMission ų�޽��� ��Ʈ�� �����ϸ鼭 ���̼� ��Ʈ���� ���ڸ� ����÷Ƚ��ϴ�.
		if( CHARA_DEATH_ENEMY_STATE_DEFENCE == DeathSupprot )
		{
			bDefenceObj = true;
		}

		if( CHARA_DEATH_H == DeathType)
			bHeadShot = true;

		const GAMEINFO_CHARACTER * pCharaInfo = BattleSlotContext::i()->getGameInfo_Chara( DeathIdx );

		// TCP�� ĳ���͸� ���̵��� �մϴ�. UDP�� ��Ŷ�� �� �޾��� ��쿡 ó����.
		
		//HitDir���� ������ ĳ���Ͱ� �� ���� ����ó���س�ϴ�. 2015-02-13 ��뿵
		CGameCharaBase * pKiller = g_pCharaManager->getCharaByNetIdx( KillIdx);
		CGameCharaBase * pVictim = g_pCharaManager->getCharaByNetIdx( DeathIdx);
		VEC3D vHitDir;
	
		i3Vector::Copy( &vHitDir, const_cast<VEC3D*>(pCharaInfo->_tNetData._tSufferData.getHitDirection()));

		if( pKiller != nullptr && pVictim != nullptr)
		{
			i3Vector::Sub( &vHitDir, pVictim->GetPos(), pKiller->GetPos());
			i3Vector::Normalize(&vHitDir, &vHitDir);
		}

		g_pCharaManager->ReserveDeath( DeathIdx, KillIdx,	&vHitDir, (CHARA_DEATH_TYPE ) DeathType );
		//HitDir���� ������ ĳ���Ͱ� �� ���� ����ó���س�ϴ�. 2015-02-13 ��뿵

		if( (KillIdx == DeathIdx) && (DeathType == CHARA_DEATH_F) )
		{	// �ڻ�
			COLOR col;

			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( KillIdx);

			if(BattleSlotContext::i()->IsRedTeam(DeathIdx))
			{
				// Red
				i3Color::Set( &col, (UINT8) 252, 88, 88, 255);
			}
			else
			{
				// Blue
				i3Color::Set( &col, (UINT8) 67, 182, 243, 255);
			}
			
			if(pChara->IsAcid())
			{
				CGameCharaDinoAcid * pAcid = (CGameCharaDinoAcid*)pChara;
				pAcid->Effect();

			}
#ifdef CC_DINO
			else if(pChara->IsCCAcid() || pChara->IsAcidVulcan())
			{
				CGameCharaDinoCCAcid * pCCAcid = (CGameCharaDinoCCAcid*)pChara;
				pCCAcid->Effect();
			}
#endif
			
			if (!BattleSlotContext::i()->IsGMObserverForClient(KillIdx))
			{
				i3::stack_wstring wstrText;
				i3::sprintf( wstrText, L"%s %s", 
					BattleSlotContext::i()->getNickForSlot(KillIdx), GAME_RCSTRING("STBL_IDX_GAME_PLAYER_SUICIDE") );	// �ڻ�
				if (CHud::i() != nullptr)
					CHud::i()->CreateInGameMessage(wstrText, &col, AssistUserIdx, DeathIdx);
			}
		}
		else
		{	
			CGameCharaBase * pKiller2 = g_pCharaManager->getCharaByNetIdx( KillIdx);
			if( LocaleValue::Enable("EnableRevengeMode") == true)
				CRevengeManager::i()->SetDeathWeaponNum(-1);		// ����â �ʱ�ȭ
			// ų �޼��� ����
			if( DeathType == CHARA_DEATH_O)
			{
				// ������Ʈ�� ���� ��� ������ ���� �������� ����
				CRevengeManager::i()->SetDeathWeaponNum(-1);	// revision 57596 ���� ������Ʈ�� ������ 1:1����â�� ���� �̹����� ���� (hansoft.5118)
				if( CHud::i() != nullptr)
					CHud::i()->CreateInGameMessage(KillIdx, DeathIdx, AssistUserIdx, nullptr, bHeadShot, bDefenceObj);
			}
			else if( DeathType == CHARA_DEATH_FAST_OBJECT)
			{
				CRevengeManager::i()->SetDeathWeaponNum(-1);	// revision 57596 ���� ������Ʈ�� ������ 1:1����â�� ���� �̹����� ���� (hansoft.5118)
				if( CHud::i() != nullptr)
					CHud::i()->CreateInGameMessage(KillIdx, DeathIdx, AssistUserIdx, nullptr, bHeadShot, false, data._nByObjectIdx);
			}
			else if( DeathType == CHARA_DEATH_TRAMPLED)
			{	

				CWeaponInfo* pCurWeaponInfo = nullptr;
				//��ũ�� Ƽ������ ũ������ ����Ÿ���� �����ؼ� ����ϹǷ�
				//����ó���մϴ�.
				if (pKiller2->IsTank())
					pCurWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_DINO, WEAPON::getItemIndex( WEAPON::DINO_TANK_CRASH), true );
				else
					pCurWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_DINO, WEAPON::getItemIndex( WEAPON::DINO_TREX_CRASH), true );

				if( CHud::i() != nullptr)
					CHud::i()->CreateInGameMessage(KillIdx, DeathIdx, AssistUserIdx, pCurWeaponInfo, bHeadShot, bDefenceObj);
				CGameMissionManager::i()->AddUserDamage(KillIdx);
			}
			else if( DeathType == CHARA_DEATH_HOWL)
			{
				CWeaponInfo * pCurWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_DINO, WEAPON::getItemIndex(WEAPON::DINO_TREX_HOWL), true );

				if(CHud::i() != nullptr)
					CHud::i()->CreateInGameMessage(KillIdx, DeathIdx, AssistUserIdx, pCurWeaponInfo, bHeadShot, bDefenceObj);
				CGameMissionManager::i()->AddUserDamage(KillIdx);
			}
			else if (((pWeaponInfo != nullptr) && (pWeaponInfo->isNotMission())) || DeathType != CHARA_DEATH_B)
			{

				if( DeathIdx == BattleSlotContext::i()->getMySlotIdx() && KillIdx != DeathIdx  && pKiller2 != nullptr)
				{
					CRevengeManager* m = CRevengeManager::i();

					switch(pWeaponInfo->GetTypeUsage())
					{
						case WEAPON_SLOT_PRIMARY :
						case WEAPON_SLOT_SECONDARY :
						case WEAPON_SLOT_MELEE :
						case WEAPON_SLOT_THROWING1 :
						case WEAPON_SLOT_THROWING2 :

							if(pKiller2->IsDino() == false)
							{
								WEAPON_CLASS_TYPE _classType = WEAPON::ItemID2Class( pWeaponInfo->GetItemID());

								switch(_classType)
								{
								case WEAPON_CLASS_OBJECT_GUN	:			break;
								default:
									{
										T_ItemID itemid = pWeaponInfo->GetOriginalWeaponItemID();	// GrenameShell ���� �ͽ��ټ� ���⿡�� �۵�
										if(itemid > 0)
										{
											CWeaponInfo * pNewWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( itemid, true);
											m->SetDeathWeaponNum( pNewWeaponInfo->GetItemID());
										}
										else
										{
											m->SetDeathWeaponNum( pWeaponInfo->GetItemID());
										}
									}
									
									break;
								}

							}
							break;
						default :
							break;
					}

					m->SubKillDeath(KillIdx);	//������ �� ���ϰ��
					i3::rc_wstring strWeaponName;

					{
						T_ItemID itemid = pWeaponInfo->GetOriginalWeaponItemID();
						if (itemid > 0)
						{
							CWeaponInfo * pNewWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(itemid, true);
							pNewWeaponInfo->GetWeaponName(strWeaponName);
						}
						else
						{
							pWeaponInfo->GetWeaponName(strWeaponName);
						}
						m->SetDeathWeaponName(strWeaponName);
					}

					if (LocaleValue::Enable("EnableRevengeMode") == TRUE)
					{
						m->SetLoveShotIdx(KillIdx);
						if( m->isLoveShotAfter(KillIdx) == true && pKiller2->isCharaStateMask(CHARA_STATE_DEATH) == true)	//���� ������ ���� �׿��� ��� ���꼦 �˻�
						{
							if(CHud::i() != nullptr)
								CHud::i()->AddChainKills(REVENGE_TYPE_LOVESHOT, 0, 1, 0, 0, 0, true);
						}
					}
				}

				// @eunil. Die HUD ó��. ���⼭ HUD�� ���ɴϴ�!
				if (DeathIdx == BattleSlotContext::i()->getMySlotIdx())
				{
					if (UserInfoContext::i()->IsGM_Observer() == false)
					{
						if (pKiller && pKiller->IsDino() == false)
						{
							INT32 filter_index = pWeaponInfo->GetWeaponSpecialDieHudIndex();
							if (CHud::i() != nullptr)
								CHud::i()->OnDieFilter(filter_index);
						}
					}
				}

				// �̼� �� ��� ���⿡ ���� ���� ó��
				// �̼� C4�� ������ �״� ����� ų�޼����� ����� �ʽ��ϴ�.
				if(CHud::i() != nullptr)
					CHud::i()->CreateInGameMessage(KillIdx, DeathIdx, AssistUserIdx, pWeaponInfo, bHeadShot, bDefenceObj);
				CGameMissionManager::i()->AddUserDamage(KillIdx);
			}
			else
			{
				I3TRACE("[NOTICE] Skip CreateInGameMessage() - DeathType(%d), weaponClass(%d)\n", DeathType, pWeaponInfo->GetTypeClass());
			}
		}

		if (g_pCharaManager->getCharaByNetIdx(KillIdx))
			observer_hud::Context::i()->Notify(observer_hud::EVT_KILL, KillIdx);

		if (g_pCharaManager->getCharaByNetIdx(AssistIdx))
			observer_hud::Context::i()->Notify(observer_hud::EVT_ASSIST, AssistIdx);

		if (g_pCharaManager->getCharaByNetIdx(DeathIdx))
			observer_hud::Context::i()->Notify(observer_hud::EVT_DEATH, DeathIdx);

		// AI ��� ���� ���� ���� â - death count
		if (DeathIdx == BattleSlotContext::i()->getMySlotIdx())
		{
			chainKillCount = 0;

			const SCORE_INFO* pScore = ScoreContext::i()->getIndividualScore(DeathIdx);

			if( HUD::instance() != nullptr)
			{
				switch(pScore->m_DeathCount)
				{
				case 5:		HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Challenge, HUD::CHM_5_DEATH); break;
				case 20:	HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Challenge, HUD::CHM_20_DEATH); break;
				}
			}

			// ����� �ڽ��� �׾������ ������ �÷��� Ȱ��ȭ
			if( pMissionMng->getRespawnMode() == GAME_RESPAWN_MODE_NOLIMIT ||
				(pMissionMng->getTotalRespawnCount() > pMissionMng->getRespawnCount( DeathIdx)) )
			{

#if defined( DEF_OBSERVER_MODE)			// ���� ����� �ڱ� �ڽ��� ��� ������ �ٱ��� �̹� �����ϹǷ�, ���� ���� ����..(11.11.21.����)
				if( BattleSlotContext::i()->isObserverMe() == false)
#else
				if(UserInfoContext::i()->IsGM_Observer() == false)
#endif
				{	// revision 50214
					m_bRespawnEnable = true;
				}

				CBattleGui::i()->SetEnableGui(false, GAME_GUI_RESPAWN);  

				// ���� ����� �ڱ� �ڽ��� ���..
				UIBattleFrame* battle_frame = UIBattleFrame::i();

				if (battle_frame)
					battle_frame->SetDeath();
				//
			}
			else	// ������ �÷��װ� Ȱ��ȭ���� �ʴ´ٸ�, 1������ ��ٷ������� ó��..(���� �� �׾��� ���, ������ ���°� ���� ����)
			{
				// ���� ����� �ڱ� �ڽ��� ���..
				UIBattleFrame* battle_frame = UIBattleFrame::i();
				if (battle_frame)
					battle_frame->SetDeathDelay();
			}

		}	// DeathIdx == BattleSlotContext::i()->getMySlotIdx()
	}

}
///////////////////////////////////////////////

void	gstatemgr::_Process_EVENT_RESPAWN_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
	m_pgStatemgr_AvatarChange->_ProcessEvent_Respawn_AvatarChange(Arg, UserData);

	return;
}

void	gstatemgr::_ProcessEvent_RespawnForAI(INT32 Arg, const i3::user_data& UserData)
{
	
	GlobalFunc::PB_TRACE( "_ProcessEvent_RespawnForAI");

	INT32 i;
	if( Arg < 0
#if defined( DEF_OBSERVER_MODE)
		|| Arg >= SLOT_MAX_COUNT
#endif
		)
		return;

	g_pCharaManager->RespawnCharaForAI( Arg );

	if( Arg == BattleSlotContext::i()->getMySlotIdx() )
	{
		m_nStageState = GAMESTAGE_STATE_BATTLE;

		UIBattleFrame* frame = UIBattleFrame::i();
		if (frame)
			frame->SetRespawn();

		CGameCameraManager::i()->RespawnCameraMgr();

		CBattleGui::i()->SetEnableGui( false);

		if(CHud::i() != nullptr)
			CHud::i()->ResetKillMessage();

		if( !getMyChara()->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN) )
		{
			// ó�� �������Դϴ�.
			// ���� �޼����� �˷��ְ� waiting �޼����� ���ϴ�.
			{	// �ڽ��� �����ϸ� ���� ���带 ���

				INT32 sndidx = GTA_START_MISSION_1 + i3Math::Rand()%3;
				if( getMyChara() != nullptr)
				{
					if( getMyChara()->IsRex()) sndidx = GTA_START_MISSION_TREX;
					else if( getMyChara()->IsRaptorBase()) sndidx = GTA_START_MISSION_RAPTOR;
				}

				g_pSndMng->StartAnnounceMessage( sndidx);
			}

			if(CHud::i() != nullptr)
				CHud::i()->SetEnableMissionNotice( false);

			for( i = 0; i < g_pCharaManager->getCharaCount(); i++)
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( i);
				if( pChara == nullptr )	continue;
				if( pChara->getCharaNetIndex() == -1)
					continue;

				if( UserContext::i()->IsPlayingSlot( pChara->getCharaNetIndex()) == false)
					continue;

				// ĳ���͸� ���̵��� ����
				pChara->Cmd_Visible( true);
			}

			getMyChara()->addCharaStateMask( CHARA_STATE_FIRSTRESPAWN);
		}

		// ������ �ϸ� �ʱ�ȭ�ϴ� �Լ�
		_MyCharaRespawnInit();
	}

	//observer_battle::i()->ProcessEventRespawnForAI();
	BattleServerContext::i()->P2PRespawnChara( Arg);
	
}

void	gstatemgr::_ProcessEvent_Respawn(INT32 Arg, const i3::user_data& UserData)
{
	//GlobalFunc::PB_TRACE( "EVENT_RESPAWN_BATTLE");

	INT32 i;
	if( Arg < 0
#if defined( DEF_OBSERVER_MODE)
		|| Arg >= SLOT_MAX_COUNT
#endif
		)
		return;

	// ��ų �ɷ�ġ���� ����Ǿ�� �Ѵ�.
	util_battle::ApplySkillsystem(Arg);	

	g_pCharaManager->RespawnChara( Arg, true, false );

	hd::check_speedhack::i()->init_pos();

	if( Arg == BattleSlotContext::i()->getMySlotIdx() )
	{
		if( HUD::instance() != nullptr)
			HUD::instance()->SetParameter(HUD::GAME_RESULT, HUD::GRS_UpdateKillCount);

		m_nStageState = GAMESTAGE_STATE_BATTLE;

		UIBattleFrame* frame = UIBattleFrame::i();
		if (frame)
			frame->SetRespawn();

		CGameCameraManager::i()->RespawnCameraMgr();

		CBattleGui::i()->SetEnableGui( false);

		///������ UI Ȱ��ȭ ����.
		m_bRespawnEnable = false;
		m_fRespawnDelayTime = 0.0f;

		if(CHud::i() != nullptr)
			CHud::i()->ResetKillMessage();

		if(getMyChara() && !getMyChara()->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN) )
		{
			// ó�� �������Դϴ�.
			// ���� �޼����� �˷��ְ� waiting �޼����� ���ϴ�.
			{	// �ڽ��� �����ϸ� ���� ���带 ���

				INT32 sndidx = GTA_START_MISSION_1 + i3Math::Rand()%3;
				if( getMyChara() != nullptr)
				{
					if( getMyChara()->IsRex()) sndidx = GTA_START_MISSION_TREX;
					else if( getMyChara()->IsRaptorBase()) sndidx = GTA_START_MISSION_RAPTOR;
				}

				g_pSndMng->StartAnnounceMessage( sndidx);
			}

			if(CHud::i() != nullptr)
				CHud::i()->SetEnableMissionNotice( false);

			for( i = 0; i < g_pCharaManager->getCharaCount(); i++)
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( i);
				if( pChara == nullptr )	continue;
				if( pChara->getCharaNetIndex() == -1)
					continue;

				if( UserContext::i()->IsPlayingSlot(pChara->getCharaNetIndex()) == false)
					continue;

				// ĳ���͸� ���̵��� ����
				pChara->Cmd_Visible( true);
			}

			getMyChara()->addCharaStateMask( CHARA_STATE_FIRSTRESPAWN);
		}
		_MyCharaRespawnInit();
	}

	//observer_battle::i()->ProcessEventRespawn();

	BattleServerContext::i()->P2PRespawnChara( Arg);

	gmode::i()->ProcessEventRespawn( Arg);

	if (IntrudeContext::i()->IsIntrusionMessage())
	{
		//�ϴ� ĳ���Ͱ� �������� �Ϸ���� ������..
		BattleObjContext::i()->SetInterEnterSmokeSend(true);
		I3TRACE("Inter Enter Smoke Send true\n");
	}
}


void	gstatemgr::_Process_EVENT_INGAMESHOP_APPLY_ITEM(INT32, const i3::user_data& UserData)
{
	IGShopItemController::GetInstance()->Apply();
}

void	gstatemgr::_Process_EVENT_RESPAWN_BATTLE_FOR_AI(INT32 Arg, const i3::user_data& UserData)
{
	I3TRACE( "EVENT_RESPAWN_BATTLE_FOR_AI %d\n", Arg);

	if ( Arg >= 0 && Arg <	SLOT_MAX_COUNT )
	{	
		g_pCharaManager->RespawnCharaForAI(Arg);
	}
	else
	{
		I3_BOUNDCHK(Arg, SLOT_MAX_COUNT);		// ���и�� �Ⱦ��ϱ�....  MAX_COUNT_DOMI�����Ŵ� �ϴ� �����ؼ� Ŀ����Ƽ ó��..
	}
}

void	gstatemgr::_Process_EVENT_START_COUNTDOWN(INT32, const i3::user_data& UserData)
{
	//count down hud 
	if(CHUD_CountDown::instance() != nullptr)
		CHUD_CountDown::instance()->start_count_down();

	//ĳ���� ������ ��ġ �̵�
	g_pCharaManager->MoveRespawnPoint();

	//ĳ���� �ֹ���� ���� �� �Ѿ� ����
	CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	pChara->Cmd_ChangeWeapon(WEAPON_SLOT_PRIMARY, true);
	WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();
	if (pWeapon != nullptr)
		pWeapon->setTotalBulletCount( pWeapon->getWeaponInfo()->GetMaxBullet() );
}

void	gstatemgr::_Process_EVENT_RESPAWN_DOMI_DINO(INT32 Arg, const i3::user_data& UserData)
{
	if( UserContext::i()->PlayingSlotFlag & (0x01 << BattleSlotContext::i()->getMySlotIdx()))
	{//���� ������ �ƴ� ��쿡 �� UI ǥ��.
		const NET_CHARA_INFO* pnet_chara_info = BattleSlotContext::i()->getNetCharaInfo( Arg, CHARA::BUFFER_NEW_AI);

		//obj ui
		CUIHudDomi_Indicator* ui = (CUIHudDomi_Indicator*)UIHUDDominationMgr::i()->GetHUD( HUD_DOMI_INDICATOR);

		bool find_dot = false;
		i3::vector_map<INT32, VEC3D>::iterator itr;
		for( itr = ui->m_dot_idx_list.begin(); itr != ui->m_dot_idx_list.end(); ++itr)
		{
			VEC3D pos = itr->second;
			VEC3D dir;

			i3Vector::Sub( &dir, &pos, pnet_chara_info->getRespawnPos());
			REAL32 len = i3Vector::LengthSq( &dir);
			if( len < 1.f)
			{		
				find_dot = true;
				break;
			}
		}

		INT32 dot_idx = -1;
		tmp_uu::CIndicateObject* ui_obj = nullptr;

		if( find_dot == false)
		{//new
			dot_idx = ui->m_dot_count++;
			ui->m_dot_idx_list.insert( i3::vector_map<INT32, VEC3D>::value_type(dot_idx, *pnet_chara_info->getRespawnPos()));

			ui_obj = ui->InsertIndication( nullptr, WAVE_MISSION_BLOCK, tmp_uu::DOT_OBJECT);
			if( ui_obj)
			{
				tmp_uu::CIndicateDot* ui_dot = static_cast<tmp_uu::CIndicateDot*>(ui_obj);
				ui_dot->SetDotIdx( dot_idx);
				i3Vector::Copy( ui_dot->GetObjPos(), const_cast<VEC3D*>(pnet_chara_info->getRespawnPos()));

				dui::member_call_back<CUIHudDomi_Indicator> * pCb = new dui::member_call_back<CUIHudDomi_Indicator>( ui, &CUIHudDomi_Indicator::cb_time_event, (void*)ui_obj);
				MEMDUMP_NEW( pCb, sizeof(dui::member_call_back<CUIHudDomi_Indicator>)); 

				dui::call_back_ptr cb( pCb);
				dui::time_event_detail * pEvt = new dui::time_event_detail( nullptr, 7.f, cb);
				MEMDUMP_NEW( pEvt, sizeof( dui::time_event_detail));

				dui::time_event_ptr ev( pEvt);
				ui_obj->event_ptr = ev;

				ui->m_time_event_list.push_event( ev);
			}
		}
	}

	CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx( Arg, CHARA::BUFFER_NEW_AI);
	const GLOBAL_CHARA_INFO* pInfo = chara->getCharaInfo();

	//name
	const CCharaInfo* chara_db_info = (const CCharaInfo*)g_pCharaInfoDataBase->GetInfo( EQUIP::ePART_CHARA, pInfo->GetResType());
	i3::rc_wstring _name = L"no name";
	if( chara_db_info)
		chara_db_info->getName( _name);

	BattleSlotContext::i()->setNickForSlot(chara->getCharaNetIndex(), chara->getBufferType(), _name);

	//
	if( chara->IsDomiMutantRex())
	{
		domi_state_info* info = domi_state_info::i();
		{	
			info->boss_list.push_back( DOMI_BOSS_HP( pInfo->GetHP(), chara));
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_BOSS_HP);

			I3PRINTLOG(I3LOG_NOTICE,"push boss info idx(%d) hp(%d)", chara->getCharaNetIndex(), pInfo->GetHP());
		}
	}
	else if( chara->IsDomiAcidItem())
	{
		CUIHudDomi_Indicator* ui = (CUIHudDomi_Indicator*)UIHUDDominationMgr::i()->GetHUD( HUD_DOMI_INDICATOR);
		ui->InsertIndication( chara, WAVE_MISSION_ELIMINATE, tmp_uu::CHARACTER_OBJECT);
	}

	// ������ ������ ������Ʈ
	{
		REAL32 _rStartPos[ ][ 3 ] = {
			{0, -1010, 0}, {0, -1020, 0}, {0, -1030, 0}, {0, -1040, 0},
			{0, -1050, 0}, {0, -1060, 0}, {0, -1070, 0}, {0, -1080, 0},
			{0, -1090, 0}, {0, -1100, 0}, {0, -1110, 0}, {0, -1120, 0},
			{0, -1130, 0}, {0, -1140, 0}, {0, -1150, 0}, {0, -1160, 0}
		};

		VEC3D pos;
		i3Vector::Set( &pos, _rStartPos[ rand()%16 ][ 0 ], g_pConfigEx->GetPhysix().HeightChara, _rStartPos[ rand()%16 ][ 2 ] );

		MATRIX mtx;
		VEC3D vec;

		i3SceneObject * pSceneObj = chara->getSceneObject();
		i3Body * pBody = pSceneObj->GetBody();
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
		for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			if( pShapeSet != nullptr )
			{
				pShapeSet->getShapeGlobalPose( &mtx );
				i3Vector::Sub( &vec, i3Matrix::GetPos( &mtx ), i3Matrix::GetPos( chara->getBoneContext()->getBoneMatrix(BONE_IK) ) );
				i3Vector::Add( &vec, &vec, &pos );
				i3Vector::Add( &vec, (REAL32)i, 0.f, 0.f );

				i3Matrix::SetPos( &mtx, &vec );

				pShapeSet->setShapeGlobalPose( &mtx );
			}
		}
	}

	// �ִϸ��̼� ����
	chara->setHpTagMaxHp( pInfo->GetHP());
	fb_fsm::StateMgr* sm = static_cast<CGameCharaFullbody*>(chara)->GetFBStateMgr();
	sm->Transition(sm->GetState(1)); // IDLE

	//respawn flag
	if( !chara->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN))
		chara->addCharaStateMask(CHARA_STATE_FIRSTRESPAWN);
}

void	gstatemgr::_Process_EVENT_DEATH_DOMI_USER(INT32 Arg, const i3::user_data& UserData)
{
	const RecvData_EVENT_DEATH_DOMI_USER& data = i3::value_cast<RecvData_EVENT_DEATH_DOMI_USER>(UserData);
	
	const DEATH_INFO_CHARA* pInfo = nullptr;
	INT32 nDeathSupprot	= 0;
	INT32 nDeathType	= 0;
	INT32 nDeathIdx		= 0;

	//INT32 KillIdx		= (Arg >> 4)	& 0x0000000F;
	//-> Killer Type ���� �ʿ�.
	
	const i3::vector<DEATH_INFO_CHARA>& vecInfo = data.DeathCharaInfo;
	const size_t nDeathCount = vecInfo.size();

	for( UINT32 cnt = 0; cnt < nDeathCount ; ++cnt)
	{
		pInfo = &vecInfo[cnt];

		nDeathSupprot	= GET_DEATHINFO_ENEMY_TYPE( pInfo->_DeathInfo );
		nDeathType		= GET_DEATHINFO_DEATH_TYPE( pInfo->_DeathInfo );
		nDeathIdx		= GET_DEATHINFO_IDX_TYPE( pInfo->_DeathInfo );

		//Reserve Death �ʿ�
		//shooter
		const GAMEINFO_CHARACTER * pCharaInfo = BattleSlotContext::i()->getGameInfo_Chara( nDeathIdx);
		g_pCharaManager->ReserveDeath( nDeathIdx, -1,
			const_cast<VEC3D*>(pCharaInfo->_tNetData._tSufferData.getHitDirection()), CHARA_DEATH_A);

		if( nDeathIdx == BattleSlotContext::i()->getMySlotIdx())
		{//���� �̼� Respawn Time Penalty
			REAL32 respawn_time = MyRoomInfoContext::i()->GetRespawnTime( nDeathIdx);
			respawn_time += DOMI_USER_ADD_RESPAWN_TIME;

			respawn_time = MINMAX( DOMI_USER_FIRST_RESPAWN_TIME, respawn_time, DOMI_USER_MAX_RESPAWN_TIME);
			CGameMissionManager::i()->setRespawnTime( respawn_time);

			//������ ��忡 ��� ���� ������ �� �� �ֵ��� ����.
			m_bRespawnEnable = true;
			CBattleGui::i()->SetEnableGui(false, GAME_GUI_RESPAWN);

			//sp UI ����.
			CUIHudDomi_Skill* ui_skill = (CUIHudDomi_Skill*)UIHUDDominationMgr::i()->GetHUD( HUD_DOMI_SKILL);
			ui_skill->VisibleGauge( false);
		}
	}
}

void	gstatemgr::_Process_EVENT_WAVE_END(INT32 Arg, const i3::user_data& UserData)
{
	if (Arg) // round result
		UIHUDDominationMgr::i()->EnableHUD(HUD_DOMI_ROUND_RESULT, true);
	else // wave result
		UIHUDDominationMgr::i()->EnableHUD(HUD_DOMI_WAVE_RESULT, true);

	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_INDICATOR, HUD_DMN_EVT_CLEAR_INDICATIONS, tmp_uu::RESPAWN_OBJECT);
}

void	gstatemgr::_Process_EVENT_M_END_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
	
	CAwayInput::i()->setOffAwayInput();

	UIBattleFrame* frame = UIBattleFrame::i();
	if (frame)
	{
		frame->SetGameEnd();		// ���� ���â�� ������ ��Ÿ���ٰ� ����.
	}

	if( m_nStageState == GAMESTAGE_STATE_NONE)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_GAME_END"), nullptr, MAKE_CALLBACK(CbFailStart), this);
	}
	m_nStageState = GAMESTAGE_STATE_ENDGAME;

	// HudGameResult�� �߰����â�� ���� ���µ� �߰����â�� ������ ������
	// Update�� ���Ƽ� ESC�� EnterŰ�� �����ԵǸ� EndTime�� 100�̵Ǿ� �ٷ� �����ϴ�.
	// �׷��� End Battle ������ EndTime�� �ʱ�ȭ �Ͽ� ������ �ʰ� �մϴ�.
	m_fEndTime = 0.0f;

	//���� : OnStageEnd()

	// chunjong.song 2014-08-29 CloseAll()���� ��� HUD�� ���⿡ ��ġ�� ���� �ű�
	if( LocaleValue::Enable( "ForciblyRemove" ) )
	{
		if (UIBattleFrame::i())
			UIBattleFrame::i()->CloseAll();			// ������� UI���� ���δµ� ���ϴ�. 11.06.23.����.
	}

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) && gmode::i()->GetTutorialMode()->IsEndByTimeOut() == false)
	{
		CrosshairMgr::i()->getCrosshair()->SetEnable(false);

		g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);			
		gmode::i()->GetTutorialMode()->GetHudTutorial()->TurnOnResultHUD(
			BattleResultContext::i()->GetInfo().m_BattleExp[0], 
			BattleResultContext::i()->GetInfo().m_BattlePoint[0], 
			TutorialContext::i()->GetRoundPlayTime() );		
	}
	else		// ���� ����
	{
		if( HUD::instance() != nullptr)
			HUD::instance()->SetParameter( HUD::GAME_RESULT, HUD::GRS_WinTeamState, Arg );
	}

	if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		for (INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara(i);
			if (pChara == nullptr)	continue;

			pChara->getMultiWeaponContext()->ClearWeaponSlotList();
		}
	}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	I3PRINTLOG(I3LOG_NOTICE, "EVENT_M_END_BATTLE");
#endif
}

void	gstatemgr::_Process_EVENT_ENTER_LOBBY(INT32 Arg, const i3::user_data& UserData)
{
	if( Arg > -1)
	{
		// �κ�� �̵�OK
		UIBattleFrame* frame = UIBattleFrame::i();
		if (frame)
		{
			frame->SetGameEnd();
		}

		CBattleGui::i()->SetEnableGui( false);
		i3GuiRoot::setGuiDisableInput(true);

		CGameFramework::JumpToReady( true);
	}
}

void	gstatemgr::_Process_EVENT_TIMEOUT_HOST(INT32, const i3::user_data& UserData)	// ���� ���� (Host)
{
	CAwayInput::i()->setOffAwayInput();
	
	// �ð� �ʰ��� ����Ż�� ����Ʈ �Ϸ� ����ϴ� �Լ�..
	MCardUtil::Update_Process_GiveupBattle();

	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_BATTLE_TIMEOUT_HOST_C"), nullptr, MAKE_CALLBACK(CbTimeOutHost), this);
}

void	gstatemgr::_Process_EVENT_TIMEOUT_CLIENT(INT32, const i3::user_data& UserData)
{
	CAwayInput::i()->setOffAwayInput();

	// �ð� �ʰ��� ����Ż�� ����Ʈ �Ϸ� ����ϴ� �Լ�..
	MCardUtil::Update_Process_GiveupBattle();

	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_BATTLE_TIMEOUT_CLIENT_C"), nullptr, MAKE_CALLBACK(CbTimeOutClient), this);
}

void	gstatemgr::_Process_EVENT_ARMEDBOMB_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
	// HUD�� �����Ǿ��ٴ� �޽���
	if( Arg < -2 || Arg >= SLOT_MAX_COUNT)
		return;

	{
		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB))
		{
			if( CHud::i() != nullptr)
				CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_BOMB_MISSION_ATTACH"));	// ��ź�� ��ġ�Ǿ����ϴ�.			
			g_pSndMng->StartAnnounceMessage( GTA_BOMB_PLATED);
		}

		CGameCharaBase * pChara = nullptr;

		// ���и�忡�� ��ġ�� ��ų�������� �����κ��� -1������ ó���ϰ� �ڱ� �ڽ��� ó���ϴ� ������� ����
		if (Arg == -1)
		{
#if defined( I3_DEBUG )
			I3TRACE(" _Process_EVENT_ARMEDBOMB_BATTLE ��ġ�� \n");
#endif
			pChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
		}
		else
			pChara = g_pCharaManager->getCharaByNetIdx( Arg);

		if( pChara != nullptr )
		{
			VEC3D vPos;

			// ��ź�� ���忡 ������.
			i3Vector::Copy( &vPos, C4Context::i()->getInstalledPos());
			i3Vector::Add( &vPos, 0.f, 0.1f, 0.f );

			// revision 54538 2013. 3. 6 �ڱ⼺
			// ���� ĳ���� ��ź�� 2�� ����� ���� ����
			// ���� ĳ���ʹ� UDP, TCP �� ������ ��ź ��ġ Ŀ�ǵ尡 ���ƿ��Ƿ� �� ���� �����־�� �Ѵ�.
			// ��ź�� �̹� ��ġ�Ǿ� �ִٸ� ��ġ���� �ʰ� �Ѿ��.
			WeaponBase* pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
			if( pWeapon != nullptr)
			{
				I3ASSERT( i3::same_of<WeaponC4*>(pWeapon));
				WeaponC4 * pBomb = (WeaponC4 *) pWeapon;
				pBomb->SetPos(&vPos);
			}
			else
			{
				pChara->Cmd_DropWeapon( WEAPON_SLOT_MISSION, &vPos);
			}
		}

		if( getMyChara() != nullptr)
		{
			// �ڽ��� ��ġ���̶��
			if( getMyChara()->isCharaStateMask( CHARA_STATE_ARMINGBOMB) )
			{ 
				// ���
				getMyChara()->Cmd_Install( false);
				// Kriss�� �߻�Ǵ� ������ �߻� ������ ������ �Ϸ�Ǹ� �ڵ����� Ǯ���Ƿ� �ʿ䰡 ����. 2010.08.25 ����
				getMyChara()->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				getMyChara()->removeCharaStateMask( CHARA_STATE_ARMINGBOMB);
			}

			// �ڽ��� C4�� ��� ������
			if( getMyChara()->getWeaponContext()->getCurrentSlotNum() == WEAPON_SLOT_MISSION)
			{
				getMyChara()->Cmd_ChangeWeapon( getMyChara()->getWeaponContext()->getOldSlotNum());
			}
		}

		gmap::i()->Process_Event_ArmedBomb_Battle();

		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		{
			gmode::i()->GetTutorialMode()->ChangeNextState(TS_REMOVE_C4);
		}

	}
}

void	gstatemgr::_Process_EVENT_DISARMBOMB_BATTLE(INT32, const i3::user_data& UserData) // HUD�� ��ü�Ǿ��ٴ� �޽���
{
	
	if( getMyChara() != nullptr &&
		getMyChara()->isCharaStateMask( CHARA_STATE_DISARMINGBOMB) )
	{	// �ڽ��� ��ü���ΰ�� ���
		getMyChara()->Cmd_Uninstall( false);

		getMyChara()->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
		getMyChara()->removeCharaStateMask( CHARA_STATE_DISARMINGBOMB);
	}

	WeaponBase * pWeapon = nullptr;

	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		if(CHud::i() != nullptr)
			CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_BOMB_MISSION_DETTACH"));	// ��ź�� ��ü�Ǿ����ϴ�.
		g_pSndMng->StartAnnounceMessage( GTA_BOMB_REMOVED);
	
#if defined( USE_EVENT_SHUTTLEX)
		pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SHUTTLEX));
#else
		pWeapon = g_pWeaponManager->FindDropWeapon( nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
#endif
		if( pWeapon != nullptr)
		{
			I3ASSERT( i3::same_of<WeaponC4*>(pWeapon));
			WeaponC4 * pBomb = (WeaponC4 *) pWeapon;

			pBomb->Uninstall();			//	��ź ����
		}
		else
		{
			// ������ ����� ��ź�� ��ġ�Ǿ����� �� �� �����ϴ�.

			I3PRINTF(GAME_STRING("STR_TBL_INGAME_HUD_I3PRINTF_C4_WARNING1"), __FILE__, __LINE__);/*�������� ���Ͽ� C4 ��ü ���θ� Ȯ���� �� �����ϴ�. - ����� �鼺��, %s(%d)\n*/

			//I3ASSERT_0;
		}
	}
	//��ź �������� ����
	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	pMissionMng->ClearUninstallingSound();

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		gmode::i()->GetTutorialMode()->ChangeNextState(TS_PREPARE_MELEE);
	}
}

void	gstatemgr::_Process_EVENT_OBJECT_USE_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
}

void	gstatemgr::_Process_EVENT_M_PRE_ROUNDSTART_BATTLE(INT32, const i3::user_data& UserData)
{
	if(IsLoadSuccess() == false)
		return;

	gmode::i()->ProcessEventPreStartRound();

	m_nStageState = GAMESTAGE_STATE_PRESTART_ROUND;
	CHud::i()->SetFirstHUD(false);

	if (CHud::i() != nullptr)
	{
		if (MyRoomInfoContext::i()->getStage()->IsHudOff())
			CHud::i()->SetHudMode(HUD_MODE_CHAOS);
		else
			CHud::i()->SetHudMode(HUD_MODE_MISSION_ROUND_START);
	}
		

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT)) {
		if(getMyChara()) {
			ub::AddDinoChara();
		}
		else {
			I3ASSERT_0;
		}
	}
}

void	gstatemgr::_Process_EVENT_M_ROUNDSTART_BATTLE(INT32, const i3::user_data& UserData)
{
	gmode::i()->ProcessEventRoundStart();

	INT32 i;

	if (CGameMaster::i()->IsPaused())
		GameEventSender::i()->SetEventOnce(EVENT_BATTLE_GM_RESUME);

	if(IntrudeContext::i()->IsIntrusionMessage())
	{
		m_nStageState = GAMESTAGE_STATE_BATTLE;
	}

	//GlobalFunc::PB_TRACE("_ProcessEvent_RoundStart");

	m_fRoundTime = 0.0f;
	//m_nStageState = GAMESTAGE_STATE_BATTLE;
	hd::check_elapsedsynctimer::i()->ResetTimer();

	//observer_battle::i()->ProcessEventRoundStart();

	// Effect�� �ʱ�ȭ �մϴ�.
	g_pEffectManager->Reset();
	g_pTracerBulletManager->ResetAll();

	// Object Init
	if( !IntrudeContext::i()->IsInterEnter() )
	{// revision 62691 ������ �ƴ� ��
		gmode::i()->ResetObjectStatus();
	}

	// ������Ʈ ���� ���� �ʱ�ȭ
	if (m_stage) // �Ѽ���Ʈ 3978
		obj_explosion::i()->Process_ResetObjectExploded(m_stage);

	if( CHud::i() != nullptr)
	{
		CHud::i()->ResetHud();
		// Kill Message�� �ʱ�ȭ �մϴ�.
		CHud::i()->ResetKillMessage();
	}

	INT32 sndidx = GTA_START_MISSION_1 + i3Math::Rand()%3;
	if( getMyChara() != nullptr)
	{
		if( getMyChara()->IsRex()) sndidx = GTA_START_MISSION_TREX;
		else if( getMyChara()->IsRaptorBase()) sndidx = GTA_START_MISSION_RAPTOR;
	}

	g_pSndMng->StartAnnounceMessage( sndidx);

	// HUD�� ��ǥ �޼���
	gmode::i()->Process_Event_RoundStart_MissionNoticeString();

	//ĳ���� ������
	//// Chara Respawn
	for( i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( UserContext::i()->IsPlayingSlot(i) == false) continue;

		// AI Respawn ���� ����
		if( (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
		{
			if( (BattleSlotContext::i()->getMySlotIdx() % 2) != (i %2) && (i / 2) >= AIModeContext::i()->EnemyCount)
			{
				continue;
			}
		}

		if( BattleSlotContext::i()->getSlotInfo( i)->_State != SLOT_STATE_BATTLE)	continue;

		BattleServerContext::i()->P2PStartBattleUser( i );

		// 880 �غ����� ��� ���� ����� ���´�
		if( i == BattleSlotContext::i()->getMySlotIdx() )
		{
			if (getMyChara())
				getMyChara()->addCharaStateMask( CHARA_STATE_FIRSTRESPAWN);
			
			IntrudeContext::i()->End();
			
			// ������ �ϸ� �ʱ�ȭ�ϴ� �Լ�
			_MyCharaRespawnInit();
		}
	}

	// HUD reset
	if( CHud::i() != nullptr)
	{
		CHud::i()->SetEnableMissionNotice(false);
		CHud::i()->SetEnableMissionResult( false);
		CHud::i()->SetEnableMissionMessage( false);
	}
	CHUD_Scope::instance()->SetEnableSinperScope( false);
	CBattleGui::i()->SetEnableGui(false, GAME_GUI_ROUND_START);

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		CTutorialMode* mode = gmode::i()->GetTutorialMode();
		if(TutorialContext::i()->IsFinishTutorial())
		{
			mode->SetOneEndCount();
		}
		mode->SetTutorialState(TS_START);

	}

	//���� ���� ��, ��Ʋ �������� ���������� ���� �ݴ´�.
	if(!UIBattleFrame::i()->IsPopupClosedAll())
	{
		UIBattleFrame::i()->CloseAll();
	}

	// �ΰ��ӽ��۽ñ����� ���̿� ESC�������� BattlePopup�������Ͽ��ٸ� ���ʿ� �����ԵǴµ�
	// CloseAll()�� ����� INGAMECHATING HUD�� GameNode State�� Disable���°��Ǿ� ä�ø���
	{
		UIBattleFrame::i()->EnableHUD(UIHUD_INGAMECHATING, true);
		UIHUDIngameChat* chat = (UIHUDIngameChat*)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
		if( chat)
			chat->SetEnableChat(false);
	}

	bool bIsRound = CStageInfoUtil::IsModeRoundType(*MyRoomInfoContext::i()->getStage());
	bool b = UserContext::i()->IsPlayingSlot(BattleSlotContext::i()->getMySlotIdx());

	if ( bIsRound && b == false )
	{
		if( IntrudeContext::i()->GetInterEnter() == MY_INTRUDE_STATE_PAUSED)
		{
			CGameMaster::i()->SetPause(true);
			if(CHud::i() != nullptr)
				CHud::i()->OnPauseMessage();
		}
	}
	else
	{
		if( IntrudeContext::i()->GetInterEnter() == MY_INTRUDE_STATE_PAUSED)
		{
			CGameMaster::i()->SetPause(true);
			if(CHud::i() != nullptr)
				CHud::i()->OnPauseMessage();
		}
		GameEventSender::i()->SetEventOnce( EVENT_RESPAWN_BATTLE);
	}

	if (CHud::i() != nullptr)
	{
		if (MyRoomInfoContext::i()->getStage()->IsHudOff())
			CHud::i()->SetHudMode(HUD_MODE_CHAOS);
		else
			CHud::i()->SetHudMode(HUD_MODE_MISSION_ROUND_START);
	}
}

void	gstatemgr::_Process_EVENT_M_ROUNDEND_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
	if( Arg <= -1 ) return;
	
	CGameMaster::i()->SetPause(false);
	// ��Ʋ�� �����µ� ���� ��Ŷ�� �ް� Ǯ���� �ʿ䰡 �ֳ�?
	// ��Ŷ ������ �ʰ� �׳� Paused�� Ǯ�� �ݴϴ�. 
	/*if (CGameMaster::i()->IsPaused())
		GameEventSender::i()->SetEventOnce(EVENT_BATTLE_GM_RESUME);*/

	gmode::i()->ProcessEventRoundEnd();

	// �������� ���� �����.....SetDeath���¸� �������� ���·� ������..
	UIBattleFrame* battle_frame = UIBattleFrame::i();
	if (battle_frame)
	{
		battle_frame->SetRoundEnd();
	}
	//

	INT32	i;
	INT32	nWinnerTeam = (Arg & 0x0000FF00) >> 8;
	m_nSvrRoundWinnerTeam = nWinnerTeam; // �����ʿ��� ���� �¸������� ����صӴϴ�.

	CGameMissionManager * pMissionMng = CGameMissionManager::i();

	if (MyRoomInfoContext::i()->IsAutoTeamChanged())
		nWinnerTeam = (INT32)SwapTeamResultType((const TEAM_RESULT_TYPE)nWinnerTeam); // �����������Ǿ����� �ٲ��ݴϴ�.

	m_nRoundWinnerTeam = nWinnerTeam;


	//EscapeMission : ����̼��� ���� ���� �޽����������ʵ��� ������ 
	// ç���� ��忡���� ���� ���� HUD�� ���� �ʵ��� ����.
	if (!MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) && !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT)
		&& !(MyRoomInfoContext::i()->getStage()->IsAiMode()))
	{
		if (CHud::i() != nullptr)
			CHud::i()->SetMissionRoundResult(nWinnerTeam);
	}

	if (g_pSndMng != nullptr)
	{
		g_pSndMng->StopBGM(2.0f);
		g_pSndMng->FlushTerrainSound(false);

		//��ź �������� ����
		pMissionMng->ClearUninstallingSound();

		m_bNormalEnd = true;
	}

	switch (Arg & 0x000000FF)
	{
	case MISSION_END_TYPE_NONE:
		break;
	case MISSION_END_TIMEOUT:
	{
		if (CHud::i() != nullptr)
			CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_BOMB_MISSION_DEFENCE"));	// ���ѽð��� ����Ǿ����ϴ�.
		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		{
			CTutorialMode* mode = gmode::i()->GetTutorialMode();
			mode->GetHudTutorial()->OnOffTutorialHUD(TH_GAME_OVER, true);
			mode->SetEndByTimeOut();
		}
	}
	break;

	case MISSION_END_BOMBFIRE:
	{
		gmode::i()->Process_Event_RoundEnd_MissionEndBombFire(m_bNormalEnd);
	}
	break;	// ��ǥ�� ���ĵǾ����ϴ�
	case MISSION_END_UNINSTALL:
	{
		m_bNormalEnd = false;
	}
	break;
	case MISSION_END_ALLDEATH:
		if (CHud::i() != nullptr)
		{
			bool bRed = 0;

			if (nWinnerTeam == RS_RED_WIN)
				bRed = true;

			if (bRed == true)
				CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_BLUE_ELEMENATE"));	// ����� ����
			else
				CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_RED_ELEMENATE"));	// ������ ����
		}
		break;

	case MISSION_END_GENERATOR_DESTROY:
	{
		//������ ���ķ� ���� ����� ������ �й����� ������Ʈ�� ���Ľ�Ų��.
		gmode::i()->Process_Event_RoundEnd_MissionEnd_ObjectExplosion(m_nRoundWinnerTeam);
	}
	break;	// ��ǥ�� ���ĵǾ����ϴ�

	break;

	case MISSION_END_DEFENCE_DESTROY:
		gmode::i()->Process_Event_RoundEnd_MissionEnd_ObjectExplosion(m_nRoundWinnerTeam);
		break;
	case MISSION_END_TUTORIAL_COMPLETE:
		break;
	case MISSION_END_MAX_KILL:
		break;
	case MISSION_END_USURPATION:
		break;
	default:
		I3PRINTLOG(I3LOG_FATAL, "StageBattle::ProcessGameContext Error Unknown Mission End Type %d", Arg & 0x000000FF);
		break;
	}

	//���� �̼ǿ���, ���� �����, ��ź�� ��ġ�Ǿ��ٰ�, ���ŵ��� �������¿��� ���尡 ����� ��쿣, c4�� ���½����־�� �մϴ�.
	gmode::i()->Process_Event_RoundEnd_IfNoBombFire_ResetBomb((MISSION_END_TYPE)(Arg & 0x000000FF));

	// ����� ����鿡 ���� ó��
	g_pWeaponManager->StageRoundEnd();

	m_nStageState = GAMESTAGE_STATE_ENDROUND;

	// ��� ĳ������ ���¸� Idle�� �������ݴϴ�.
	for (i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara(i);
		if (pChara == nullptr)	continue;
		if (pChara->isCharaStateMask(CHARA_STATE_DEATH) == false)
			pChara->Cmd_Reset();

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			pChara->getMultiWeaponContext()->ClearWeaponSlotList();
		}
	}

	// revision 62691 ���Խ� �ı�������Ʈ ó��. ������ �ƴ϶�� �ʱ�ȭ, ���� ���� �� ���Ի��¸� �ٽ� �ʱ�ȭ��Ŵ.
	if( IntrudeContext::i()->IsInterEnter() )
	{
		IntrudeContext::i()->InterEnter( MY_INTRUDE_STATE_NONE );
	}
	
	{
		//Hazeȿ�� ��� ����
		if (g_pFramework->GetHazePtr())
			g_pFramework->GetHazePtr()->InitEffect();

		observer_hud::Context::i()->Notify(observer_hud::EVT_HAZEMODE, false);
	}
}

void	gstatemgr::_Process_EVENT_M_KICK_USER(INT32 Arg, const i3::user_data& UserData)
{
	if( EV_SUCCESSED(Arg))
	{
		//
		// 2�������� �������� 2�� �����ǵ�.... �׳� 2�� ǥ���ϴ� ���׸� ���°� ����...
		//
		CAwayInput::i()->setOffAwayInput();

		// GM�� ��� ���� ó��
		bool IsGM = Arg > 0 ? true:false;
		if(IsGM)
		{
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_GM_GAME_KICK_EXIT"), nullptr, MAKE_CALLBACK(CbGMRoomKicked), this);
			return;
		}

		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_KICKED"), nullptr, MAKE_CALLBACK(CbRoomKicked), this);

		
		UIBattleFrame* frame = UIBattleFrame::i();
		if (frame)
		{
			frame->SetGameEnd();			// űŷ�������ÿ��� ����� ����..
		}

		// �κ���̵�
		GameEventSender::i()->SetEventOnce( EVENT_ENTER_LOBBY); // �κ�� ����
	}
	else
	{
		if( EV_IS_ERROR(Arg, EVENT_ERROR_FIRST_MAINLOAD) )
		{
			if (SLOT_STATE_BATTLE == BattleSlotContext::i()->getMySlotInfo()->_State)
			{
				// ���� �� ���� ���� ��
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_BATTLE_FRIST_MAINLOAD_BATTLE"), nullptr, MAKE_CALLBACK(CbFailStart), this);
			}
			else
			{
				// �뿡�� ���� ���� �� 
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_BATTLE_FRIST_MAINLOAD"), nullptr, MAKE_CALLBACK(CbFailStart), this);
			}
		} 
		else if( EV_IS_ERROR(Arg, EVENT_ERROR_FIRST_HOLE) )
		{
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_BATTLE_FRIST_HOLE"), nullptr, MAKE_CALLBACK(CbFailStart), this);
		}
		else
		{
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_BATTLE_FRIST_DEFAULT"), nullptr, MAKE_CALLBACK(CbFailStart), this);
		}
	}
}

// ��ǥ�� ���� ����� ����ó������ GIVEUPBATTLE_ACK�� ��ٸ��� ��..
// ����ÿ� �޼����ڽ��� '����'�� �˷��־���մϴ�. (11.11.18.����)
void	gstatemgr::_Process_EVENT_KICK_VOTE_MY_KICKED(INT32, const i3::user_data& UserData)
{	
	CBattleGui::i()->SetEnableGui( false);
	i3GuiRoot::setGuiDisableInput(true);
	KickContext::i()->SetKickedByVote();
}

void	gstatemgr::_Process_EVENT_M_HOSTLOST_BATTLE(INT32 Arg, const i3::user_data& UserData)
{
	BattleServerContext::i()->P2PHostChange( Arg);

	// �Ʒ��� ��, AI ĳ���� �Ѱܹޱ�
	if ((MyRoomInfoContext::i()->getStage()->IsAiMode()) &&
		BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx())
	{
		util_battle::ChangeAiCharacter();
	}
}

void	gstatemgr::_Process_EVENT_M_DRIVE_AWAY(INT32 Arg, const i3::user_data& UserData)
{
	CAwayInput::i()->setOffAwayInput();

	switch(Arg)
	{
	case ACCOUNT_KICK_LOGIN:
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DRIVE_AWAY_LOGIN_E"), nullptr, EXIT_CALLBACK(nullptr, true), this);
		break;
	case ACCOUNT_KICK_GM_TOOL:
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DRIVE_AWAY_TOOL_E"), nullptr, EXIT_CALLBACK(nullptr,true), this);
		break;
	case ACCOUNT_KICK_OTHER:
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DRIVE_AWAY_OTHER_E"), nullptr, EXIT_CALLBACK(nullptr,true), this);
		break;
	default:
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DRIVE_AWAY_OTHER_E"), nullptr, EXIT_CALLBACK(nullptr,true), this);
		break;
	}

	m_bDriveAway = true;	// DISCONNECTION_M �����˾��� ���´�
}

void	gstatemgr::_Process_EVENT_M_GAMEGUARD_ERROR(INT32, const i3::user_data& UserData)
{
	CAwayInput::i()->setOffAwayInput();
	CBattleGui::i()->SetEnableGui( false);
	i3GuiRoot::setGuiDisableInput(true);

	// ���� - DRIVE_AWAY_M�� ���� ����� �ߺ� �޽����� ���ϱ� ���� �˻��Ѵ�
	if (false == m_bDriveAway)
	{
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_EP_GAMEGUARD_ERROR"), nullptr, EXIT_CALLBACK(nullptr,true), this);
		m_bDriveAway = true;
	}
}

void	gstatemgr::_Process_EVENT_M_ERROR(INT32 Arg, const i3::user_data& UserData)
{
	gstatemgr::_Process_EVENT_M_DISCONNECTION(Arg, UserData);		// �� �Լ��� ��ü��..
}

void	gstatemgr::_Process_EVENT_M_DISCONNECTION(INT32, const i3::user_data& UserData)
{
	CAwayInput::i()->setOffAwayInput();
	CBattleGui::i()->SetEnableGui( false);
	i3GuiRoot::setGuiDisableInput(true);
	// ���� - DRIVE_AWAY_M�� ���� ����� �ߺ� �޽����� ���ϱ� ���� �˻��Ѵ�
	if( false == m_bDriveAway)
	{
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_ASSERT_E"), nullptr, EXIT_CALLBACK(nullptr,true), this);
		m_bDriveAway = true;
	}
}

void	gstatemgr::_Process_EVENT_M_SHUTDOWN(INT32 Arg, const i3::user_data& UserData)
{
	if (false == m_bDriveAway)
	{
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STR_SHUTDOWN_POPUP_ACCESSPROHIBITION"), nullptr, EXIT_CALLBACK(nullptr,true), this);
		m_bDriveAway = true;
	}

	GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
}

void	gstatemgr::_Process_EVENT_M_DESTROYED_NETWORK(INT32, const i3::user_data& UserData)	// ��Ʈ��ũ�� ��������
{
	CAwayInput::i()->setOffAwayInput();
	CBattleGui::i()->SetEnableGui( false);
	i3GuiRoot::setGuiDisableInput(true);

	if( m_bDriveAway == false)
	{
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_EP_GAME_DESTROYED_NETWORK_E"), nullptr, EXIT_CALLBACK(nullptr,true), this);
		m_bDriveAway = true;
	}
}

void	gstatemgr::_Process_EVENT_M_INVALID_TIMESYNC(INT32, const i3::user_data& UserData)
{
	CAwayInput::i()->setOffAwayInput();
	CBattleGui::i()->SetEnableGui( false);
	i3GuiRoot::setGuiDisableInput(true);

	// ���� - DRIVE_AWAY_M�� ���� ����� �ߺ� �޽����� ���ϱ� ���� �˻��Ѵ�
	if (false == m_bDriveAway)
	{
		m_bDriveAway = true;
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_EP_GAME_INVALID_USE_E"), nullptr, EXIT_CALLBACK(nullptr,true), this);
	}
}

void	gstatemgr::_Process_EVENT_ITEM_AUTH(INT32 Arg, const i3::user_data& UserData)// ������ ������ ����
{
	INT32 itemBattleMove = CASHITEM_ITEMID_BATTLEFIELD_MOVE;
	UIBattleFrame* battle_frame = UIBattleFrame::i();

	if( EV_SUCCESSED(Arg))
	{
		//	������ ���� �̵��� ��� ����� �������� �������
		if( CInvenList::i()->GetLatestUsedCashItemFlag() == itemBattleMove)
		{
		
			i3::stack_wstring wstrRemainTeam = INVALID_TEXTW;
			UINT32 itemTerm = 0;
			UINT8 itemAttr = 0;
			UINT32 itemArg = 0;

			INT32 cashItemKeyFlag = GetCashItemByGroup(itemBattleMove);

			if( cashItemKeyFlag == 0)	cashItemKeyFlag = itemBattleMove;

			i3::rc_wstring wstrName;
			g_pShopItemInfoDataBase->GetItemName(cashItemKeyFlag, wstrName);

			if( CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM, cashItemKeyFlag, &itemAttr, &itemArg))
			{
				itemTerm = itemArg;	//	������ ����

				i3::sprintf(wstrRemainTeam, GAME_RCSTRING("STR_TBL_GUI_READY_REMAINING_COUNT"), itemTerm);/*���� ���� : {col:255,255,255,255}%d��{/col}*/
			}

			/*{col:255,153,51,255}%s{/col} �������� ����Ǿ����ϴ�.\n \n {col: 51,204,255,255}����ġ �� ����Ʈ�� ȹ���߽��ϴ�.\n ��Ż �г�Ƽ�� ������� �ʽ��ϴ�.\n ���� �� ų/������ ��ϵ��� �ʽ��ϴ�.{/col}\n \n %s*/

			i3::rc_wstring wstrMessage;

			i3::sprintf(wstrMessage, GAME_RCSTRING("STR_TBL_GUI_READY_ITEM_APPLYED_LEAVE_GAME"), wstrName, wstrRemainTeam);

			GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMessage);

			CInvenList::i()->SetUseBattleGiveupCashItem(false);
		}
		else
		{
			// ������� �����ϸ� ��� ���������� ���������� ���̴�...
			if (battle_frame)
				battle_frame->ProcessGameEvent( EVENT_ITEM_AUTH, Arg, UserData);
		}
	}
	else
	{
		if( CInvenList::i()->GetLatestUsedCashItemFlag() == itemBattleMove)
		{
			/*��Ʈ��ũ ������ ������ ������ �����߽��ϴ�.\n �ٽ� �õ��� �ּ���.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_ITEM_IDENTYFY_FAIL_BY_NETWORK_PROBLEM"));
		}
		else
		{
			// ������� �����ϸ� ��� ���������� ���������� ���̴�...
			if (battle_frame)
				battle_frame->ProcessGameEvent( EVENT_ITEM_AUTH, Arg, UserData);
		}
	}

	//PurchaseMgr::i()->cb_receive_auth_packet( Arg);
}

	//Key:EscapeMission ȭ�� ���� ����� �߰� ����ġ�� ���� �κ��Դϴ�.
void	gstatemgr::_Process_EVENT_BATTLE_TOUCHDOWN(INT32, const i3::user_data& UserData)
{
	// Key:EscapeMission ȭ���������� �߰� ����ġ�� ���� �����Դϴ�.

	if (BattleSlotContext::i()->getMySlotIdx() == DinoModeContext::i()->TouchDownSlotIdx)
	{
		if( CHud::i() != nullptr)
			CHud::i()->AddTouchDown( DinoModeContext::i()->TouchDownCount, DinoModeContext::i()->TouchDownSlotIdx);
	}
	if( CHud::i() != nullptr)
		CHud::i()->CreateInGameMessage(DinoModeContext::i()->TouchDownSlotIdx, 0, 0);
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	I3PRINTLOG(I3LOG_NOTICE, "EVENT_RESPAWN_BATTLE");
#endif

	if( DinoModeContext::i()->TouchDownSlotIdx < 0 || DinoModeContext::i()->TouchDownSlotIdx >= SLOT_MAX_COUNT)
		return;

	if( (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  == false) )
	{
		g_pCharaManager->RespawnChara( DinoModeContext::i()->TouchDownSlotIdx);
		hd::check_speedhack::i()->init_pos();

		if( DinoModeContext::i()->TouchDownSlotIdx == BattleSlotContext::i()->getMySlotIdx() )
		{
			CGameCameraManager::i()->RespawnCameraMgr();
			CBattleGui::i()->SetEnableGui( false);
			// ������ �ϸ� �ʱ�ȭ�ϴ� �Լ�
			_MyCharaRespawnInit();
			UserInfoContext::i()->SetMyTouchDownCnt( UserInfoContext::i()->GetMyTouchDownCnt() + 1 );
		}

		//observer_battle::i()->ProcessEventEscapeRespawn();
		BattleServerContext::i()->P2PRespawnChara( DinoModeContext::i()->TouchDownSlotIdx);
	}
}

void	gstatemgr::_Process_EVENT_TUTORIAL_ROUND_END(INT32, const i3::user_data& UserData)
{
	gmode::i()->GetTutorialMode()->PlusOneEndCount();
}

//Key:EscapeMission 

void	gstatemgr::_Process_EVENT_BATTLE_ESCAPEBLOCK(INT32, const i3::user_data& UserData)
{
	TOUCHDOWN_BLOCK_STATE eState = DinoModeContext::i()->eEscapeBlock;
	
	switch( eState)
	{
	case TOUCHDOWN_BLOCK_STATE_INIT :
		break;

	case TOUCHDOWN_BLOCK_STATE_FULL:
		if(CHud::i() != nullptr)
			CHud::i()->AddEscapeBlock();
		break;
	}
}

void	gstatemgr::_Process_EVENT_BATTLE_GM_PAUSE(INT32 Arg, const i3::user_data& UserData)
{
	//
	if ( EV_SUCCESSED(Arg) )
	{
		CGameMaster::i()->SetPause(true);
		hu::set_time_pause(true);
		BattleSyncTimer::i()->Pause();
		if(CHud::i() != nullptr)
			CHud::i()->OnPauseMessage();
	}
	else
	{	
		switch (Arg)
		{
		case EVENT_ERROR_GM_PAUSE_FAIL_REASON_C4_INSTALL:
			g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_GM_PAUSE_FAIL_REASON_C4_INSTALL"));
			break;
		case EVENT_ERROR_GM_PAUSE_FAIL_REASON_ALREADY_PAUSE:
			g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_GM_PAUSE_FAIL_REASON_ALREADY_PAUSE"));
			break;
		case EVENT_ERROR_GM_PAUSE_FAIL_REASON_U_R_NOT_GM:
			g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_GM_PAUSE_FAIL_REASON_U_R_NOT_GM"));
			break;
		case EVENT_ERROR_GM_PAUSE_FAIL_REASON_TIME_LIMIT:
			g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_GM_PAUSE_FAIL_REASON_TIME_LIMIT"));
			break;
		default:
			break;
		}
	}
}
void	gstatemgr::_Process_EVENT_BATTLE_GM_RESUME(INT32 Arg, const i3::user_data& UserData)
{
	if ( EV_SUCCESSED(Arg) )
	{
		CGameMaster::i()->SetPause(false);
		hu::set_time_pause(false);
		BattleSyncTimer::i()->Resume();
		if(CHud::i() != nullptr)
			CHud::i()->ClearPauseMessage();
	}
	else
	{
		switch(Arg)
		{
		case EVENT_ERROR_GM_RESUME_FAIL_REASON_NOT_PAUSE:
			g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_GM_RESUME_FAIL_REASON_NOT_PAUSE"));
			break;
		case EVENT_ERROR_GM_RESUME_FAIL_REASON_U_R_NOT_LOCKER:
			g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_GM_RESUME_FAIL_REASON_U_R_NOT_LOCKER"));
			break;
		default:
			break;
		}
	}
}

void	gstatemgr::_Process_EVNET_GM_USER_EXIT(INT32 Arg, const i3::user_data& UserData)
{
	if ( false == m_bDriveAway)
	{
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_GM_GAME_KICK_LOGOUT"), nullptr, MAKE_CALLBACK(CbExitNormal), this);
		m_bDriveAway = true;
	}
}

void	gstatemgr::_Process_EVENT_HackToolDetected(INT32 Arg, const i3::user_data& UserData)
{
	I3PRINTLOG(I3LOG_NOTICE, "[UlsanGuard] Hacking tool detected. Close Game.");

	GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_HACKUSER"), &GAME_RCSTRING("STBL_IDX_COMMON_MSGBOX_CAPTION"),
		MAKE_CALLBACK(&usg_misc::HackToolDetectionCtx::OnMsgboxOKButtonClicked), this);
}

void	gstatemgr::_Process_EVENT_default( GAME_EVENT nEvent, INT32 Arg, const i3::user_data& UserData)
{
	UIBattleFrame* frame = UIBattleFrame::i();
	if (frame != nullptr)
	{
		frame->ProcessGameEvent( nEvent, Arg, UserData);
	}
	else
	{
		NETTRACE( "StageBattle::ProcessGameContext() - invalid Event. %d\n", nEvent );
	}

	gmode*	pMode	= gmode::i();
	if( pMode != nullptr)
	{
		pMode->ProcessEventDefault( nEvent, Arg, UserData);
	}
}

void	gstatemgr::_MyCharaRespawnInit( void)
{
	//observer::Context::i()->ProcessMyCharaRespawnInit();
	observer::Context::i()->Reset();
	CHud::i()->SetEnableObserver(false);
	log_respawn::i()->Reset();
}

void	gstatemgr::_Process_GAMESTAGE_STATE_NONE(REAL32 fDeltaTime, bool& bEnableShowScore)
{
	// empty
}

void	gstatemgr::_Process_GAMESTAGE_STATE_BATTLE(REAL32 fDeltaTime, bool& bEnableShowScore)
{
	g_pWeaponManager->OnUpdate( fDeltaTime );				//���忡 �ִ� ���� ó��

	_UpdateObject( fDeltaTime );							// ������Ʈ ��Ʈ��ũ ó��

	if ( CStageInfoUtil::IsObserverModeStage(*MyRoomInfoContext::i()->getStage()) )
		observer::Context::i()->OnUpdate(fDeltaTime);

	// Process KillCam Filter
	if(CHud::i() != nullptr)
	{
		if(  i3::same_of<CGameCamera_Kill*>(g_pCamera))
		{
			CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;

			if (pKillCam->isFPS())
			{
				CHud::i()->EnableDeathSprite( true);
			}
			else
			{
				CHud::i()->EnableDeathSprite(false);
				CHud::i()->ClearDamageFilter();
			}
		}
		else
		{
			CHud::i()->EnableDeathSprite( false);
		}
	}
}

void	gstatemgr::_Process_GAMESTAGE_STATE_ENDROUND(REAL32 fDeltaTime, bool& bEnableShowScore)
{
	gstatemgr::_Process_EndRound_ShowScore_Common(fDeltaTime);
	
	static bool RoundEndSoundPlay;

	if (m_fRoundTime >= MISSION_FREE_TIME)
	{
		if ( !RoundEndSoundPlay )
		{
			//���� ����� ���� ó�� �κ�.
			if (getMyChara() != nullptr && m_nRoundWinnerTeam < TEAM_DRAW)
			{
				//ĳ���� Ÿ���� 0�� NONE�̶� �̷��� 1�� �÷��� �ϴµ� �ϴ�.
				if ((m_nRoundWinnerTeam + 1) == (INT32)getMyChara()->getCharaTeam())//MISSION SUCCESS MESSAGE
				{
					g_pSndMng->StartAnnounceMessage(GTA_MISSION_SUCCESS);
				}
				else
					g_pSndMng->StartAnnounceMessage(GTA_MISSION_FAIL);
			}

			RoundEndSoundPlay = true;
		}

		if (CHud::i() != nullptr)
			CHud::i()->ResetKillMessage();

		// ��Ÿ���� ������ ��� ����â�� �����ݴϴ�.

		CBattleGui::i()->SetEnableGui(false);

		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) == false)
		{
			if (CHud::i() != nullptr)
				CHud::i()->SetHudMode(HUD_MODE_MISSION_SCORE);
		}
		else
			gstatemgr::_UpdateScore(false, fDeltaTime);
	}
	else
	{
		RoundEndSoundPlay = false;
	}

	bEnableShowScore = false;			// ����â ��Ȱ��ȭ�� �Ұ����ϵ���...
}

void	gstatemgr::_Process_GAMESTAGE_STATE_SHOWSCORE(REAL32 fDeltaTime, bool& bEnableShowScore)
{
	gstatemgr::_Process_EndRound_ShowScore_Common(fDeltaTime);

	if (m_fRoundTime >= MISSION_FREE_TIME)
	{
		if (CHud::i() != nullptr)
			CHud::i()->ResetKillMessage();

		// ��Ÿ���� ������ ��� ����â�� �����ݴϴ�.

		CBattleGui::i()->SetEnableGui(false);

		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) == false)
		{
			if (CHud::i() != nullptr)
				CHud::i()->SetHudMode(HUD_MODE_MISSION_SCORE);
		}
		else
			gstatemgr::_UpdateScore(false, fDeltaTime);

	}

	if (getMyChara() != nullptr)
	{
		static const bool bPlayEndSound = false;

		struct PlayEndSoundStruct
		{
			static void call(CGameCharaBase* getMyChara, INT32 m_nRoundWinnerTeam)
			{
				if ((m_nRoundWinnerTeam + 1) == (INT32)getMyChara->getCharaTeam())//MISSION SUCCESS MESSAGE
				{
					g_pSndMng->StartAnnounceMessage(GTA_MISSION_SUCCESS);
				}
				else
				{
					if (m_nRoundWinnerTeam + 1 != 3)
					{
						g_pSndMng->StartAnnounceMessage(GTA_MISSION_FAIL);
					}
				}
			}
		};

		struct NoEndSoundStruct
		{
			static void call(CGameCharaBase* getMyChara, INT32 m_nRoundWinnerTeam) {}
		};

		typedef mpl::if_c<bPlayEndSound, PlayEndSoundStruct, NoEndSoundStruct>::type  PlaySoundOrNot;
		PlaySoundOrNot::call(getMyChara(), m_nRoundWinnerTeam);
	}

	bEnableShowScore = false;			// ����â ��Ȱ��ȭ�� �Ұ����ϵ���...
}

void	gstatemgr::_UpdateScore(bool bVisible, REAL32 tm)
{
	if(bVisible == true && CBattleGui::i()->IsResultState()) 
		return;
	
	//EscapeMission HUD���ھ� â�� ����մϴ�.
	if( CHud::i() != nullptr)
		CHud::i()->UpdateScore( bVisible == true);
}

#include "ui/UIHudGameResult.h"
void	gstatemgr::_Process_GAMESTAGE_STATE_ENDGAME(REAL32 fDeltaTime, bool& bEnableShowScore)
{
	bool PlayEndSound = false;
	if (m_fEndTime == 0.0f)
		PlayEndSound = true;

	m_fEndTime += fDeltaTime;

	if (m_nStageState == GAMESTAGE_STATE_NONE)
		bEnableShowScore = false;

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		if( m_fEndTime > BATTLE_RESULT_TIME)
		{
			m_fEndTime = 0.0f;

			CGameFramework::JumpToReady();

			m_nStageState = GAMESTAGE_STATE_NONE;
			bEnableShowScore = false;	
		}

	}
	else
	{
		if( LocaleValue::Enable( "ForciblyRemove" ) )
		{
			UIBattleFrame* pUIFrame = UIBattleFrame::i();

			if (pUIFrame)
				pUIFrame->CloseAll();			// ������� UI���� ���δµ� ���ϴ�. 11.06.23.����.
		}

		{
			CBattleGui::i()->SetEnableGui(true, GAME_GUI_RESULT);
			if(CHud::i() != nullptr)
				CHud::i()->SetGameEnd();
			if( HUD::instance() != nullptr)
			{
				UIHudMissionBoard* MissionBoard = (UIHudMissionBoard*)HUD::instance()->GetHud(HUD::MISSION_BOARD);
				MissionBoard->CloseMissionBoard();

				HUD::instance()->SetParameter(HUD::DASH_BOARD, HUD::DBS_SetGameTimeZero);
				HUD::instance()->SetParameter(HUD::GAME_RESULT, HUD::GRS_EndShow);

				if (getMyChara() != nullptr)
				{
					UIHudGameResult* GameResult = (UIHudGameResult*)HUD::instance()->GetHud(HUD::GAME_RESULT);
					if (PlayEndSound && GameResult != nullptr )
					{
						if (GameResult->result_state == TEAM_BLUE_WIN)
							g_pSndMng->StartAnnounceMessage(GTA_BLUETEAM_WIN);
						else
							if (GameResult->result_state == TEAM_RED_WIN)
								g_pSndMng->StartAnnounceMessage(GTA_REDTEAM_WIN);
					}
				}

			}
		}

		// 8�ʰ� �����Ŀ� ��������� �̵��մϴ�.
		if( m_fEndTime > BATTLE_RESULT_TIME &&
			HUD::instance() != nullptr && HUD::instance()->IsEnableHud(HUD::GAME_RESULT) == true )
		{
			// ����Ʈ ������ ������ ���ش�.
			g_pFramework->SetNightVisionEnable(false);
			m_fEndTime = 0.0f;

			//���â
			CBattleGui::i()->SetEnableGui( false, GAME_GUI_RESULT);
			_EnableHUD(false, GAME_GUI_RESULT);
			HUD::instance()->EnableHud(HUD::GAME_RESULT, false);
			HUD::instance()->EnableHud(HUD::DEFAUTOCHANGE, false);

			//���¡ ���⼭??�ϵ���
			CGameFramework::JumpToReady();

			m_nStageState = GAMESTAGE_STATE_NONE;
			bEnableShowScore = false;			// ����â ������~

		}

	}
}

void	gstatemgr::_Process_GAMESTATE_STATE_PRESTART_ROUND(REAL32 fDeltaTime, bool& bEnableShowScore)
{
	CBattleGui::i()->SetEnableGui( true, GAME_GUI_PRESTART_ROUND, bEnableShowScore);
}

void	gstatemgr::_Process_GAMESTATE_WAIT_HOLEPUNCHING(REAL32 fDeltaTime, bool& bEnableShowScore)
{
	if( NetworkContext::i()->relayServerState == UDP_STATE_DONE)
	{
		// EVENT_M_PRE_ROUNDSTART_BATTLE ó��
		//g_pWeaponManager->PreStartRound();
		CGameMissionManager::i()->ProcessMissionPreStart();

		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
		{
			DinoModeContext::i()->eEscapeBlock	= TOUCHDOWN_BLOCK_STATE_INIT;
			DinoModeContext::i()->rDelayTime		= 0.0f;
		}

		_Process_EVENT_M_PRE_ROUNDSTART_BATTLE( 0 , i3::user_data() );
		BattleServerContext::i()->MoveStageEnd();
	}
}

void	gstatemgr::_Process_EndRound_ShowScore_Common(REAL32 fDeltaTime)
{
	m_fRoundTime += fDeltaTime;

	if( m_fRoundTime < MISSION_FREE_TIME)
	{
		if( !CBattleGui::i()->GetEnableGui( GAME_GUI_RESPAWN)  )
		{
			GAME_KEY_STRUCT* pKeyMap = g_pEnvSet->getGameKeyMap();

			// ���� ����� �ǹ̴ϸ��� Ȱ��ȭ�Ǿ� ������ �����մϴ�.

			if (LocaleValue::Enable("TabMinimap"))
			{
				UIBattleFrame* frame = UIBattleFrame::i();/*g_pFramework->GetUIBattleFrame();*/
				frame->EnableHUD(UIHUD_TABMINIMAP, false);
			}

			if(CHud::i() != nullptr)
			{
				CHud::i()->SetEnableMiniMap( false);
				CHud::i()->SetEnableMissionResult( true);

				if((MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) == false)
					&& MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  == false
					&& UserInfoContext::i()->IsGM_Observer() == false)
				{
					i3::stack_wstring wstrWeaponChange;
					i3::sprintf(wstrWeaponChange, L"{col:253, 167,   0, 255}%s{/col} %s", 
						GameUI::GetKeyboardString( &pKeyMap[GAME_KEY_MAP_CHANGEWEAPON]), 
						GAME_RCSTRING("STBL_IDX_MISSION_WEAPONCHANGE"));
					CHud::i()->SetMissionMessageString(wstrWeaponChange);
				}
			}

			// ���� ����� �ǹ̴ϸ��� Ȱ��ȭ�Ǿ� ������ �����մϴ�. 2011.11.14. ugeun.ji
			if (LocaleValue::Enable("TabMinimap"))
			{
				tu::ProcessRoundEnd();
			}
		}
		else
		{
			if(CHud::i() != nullptr)
			{
				CHud::i()->SetEnableMissionResult( false);
				CHud::i()->SetEnableMissionMessage( false);
			}
		}

		// ����鿡 ���� ������Ʈ
		g_pWeaponManager->OnUpdate( fDeltaTime );
	}

}



void	gstatemgr::_UpdateObject( REAL32 rDeltaTime )
{

	StageObjectListPtr	pObjectList = m_stage->GetStageObjectList();

	if( pObjectList == nullptr )return;

	if( CGameMissionManager::i()->getMissionState() == GAME_MISSION_STATE_ENDBATTLE )
		return;

	for(size_t i = 0; i < pObjectList->size(); i++ )
	{
		i3Object * pGameObject = (*pObjectList)[i];

		INT32 idx = pGameObject->getNetIndex();
		if (idx < 0)
		{
			I3ASSERT(idx >= 0);		continue;
		}

		GAMEINFO_OBJECT * pInfo = BattleObjContext::i()->setGameInfo_Object( idx);
		if( pInfo == nullptr )
			continue;

		bool bMissionObject = CGameMissionManager::i()->isMissionObject( pGameObject);

		if( i3::kind_of<CGameControledObject*>(pGameObject))
		{
			if( i3::same_of<CGameObjectWeaponBox*>(pGameObject))
			{
				CGameObjectWeaponBox * pWeaponbox = static_cast<CGameObjectWeaponBox*>(pGameObject);
				if( bMissionObject )
				{
					if( 0xFF == pInfo->_tNetData.getEnterCharaIndex() ) 
					{
						pWeaponbox->EnableObjectInBox();
						CGameMissionManager::i()->CopyMissionObjPos( pWeaponbox->GetPos());
					}
					else
					{
						pWeaponbox->DisableObjectInBox();
					}
				}
				else
				{
					if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
					{
						// ? �� Process�� �ȵ�����?
						pWeaponbox->ProcessObject(rDeltaTime);
					}
					else
					{
						// WeaponBox�� respawn timer�� �����.
						if (pInfo->_tNetData.getShellCount() == 0)
							pWeaponbox->DisableObjectInBox();
						else
							pWeaponbox->EnableObjectInBox();

						pWeaponbox->setRespawnTimer(pInfo->_tNetData.getRespawnTime());
						//pWeaponbox->setMaxShellCount( pInfo->_tNetData.getMaxShellCount());
					}
					
					pInfo->_bIntrudeCheckForControledObject = true;
				}
			}
			else if( i3::same_of<CGameObjectEvent*>( pGameObject))
			{
				CGameObjectEvent * pEvent = (CGameObjectEvent*) pGameObject;

				BattleObjContext::i()->ObjectHitReceveFunction( pEvent->getEventType(), pGameObject);

				pInfo->_bIntrudeCheckForControledObject = true;
			}
			else if (i3::same_of<CGameObjectBattleUseItem*>(pGameObject))
			{
				CGameObjectBattleUseItem * pitemBox = (CGameObjectBattleUseItem*)pGameObject;
				if (pitemBox != nullptr)
				{
					pitemBox->ProcessObject(rDeltaTime);
				}

				pInfo->_bIntrudeCheckForControledObject = TRUE;
			}

			// Ư�� ������Ʈ�Դϴ�.
			if( IntrudeContext::i()->IsInterEnter() )
			{
				// ���Կ� ���� Ư�� ������Ʈ ó��
				((CGameControledObject*)pGameObject)->ProcessIntrude( &pInfo->_tNetData);
			}
			

			if( ((CGameControledObject*)pGameObject)->isManualUpdate())
			{	// �������� ó���ÿ��� �ش� �Լ� ó���� ���� ������Ʈ�� ������Ʈ�Ѵ�.
				((CGameControledObject*)pGameObject)->ProcessObject( rDeltaTime);
				pInfo->_tNetData.setObjectType( 0);
				continue;
			}
		}

		

		if( pGameObject->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_NONE )
			continue;	// ��Ʈ��ũ�� ���� �ʿ䰡 ������ ��ŵ

		if( BattleServerContext::i()->IsP2PHost() )
		{	// ������ ������Ʈ �����͸� ������ �����մϴ�.(for P2P)

			INT32 EventSender = BattleSlotContext::i()->getMySlotIdx();

			if( i3::kind_of<CGameCharaBase*>(pGameObject->getEventSender()))
			{
				CGameCharaBase * pES = (CGameCharaBase*) pGameObject->getEventSender();
				EventSender = pES->getCharaNetIndex();
			}

			switch( pGameObject->getNetworkSyncMode() )
			{
			case I3_NETWORK_GAMEOBJECT_TYPE_NONE :		break;
			case I3_NETWORK_GAMEOBJECT_TYPE_STATIC :	GAMEEVENT::Write_Object( idx, EventSender, (UINT16) pGameObject->getCurrentHP() );	break;
			case I3_NETWORK_GAMEOBJECT_TYPE_MOVE :		GAMEEVENT::Write_Object( idx, EventSender, (UINT16) pGameObject->getCurrentHP(), pGameObject->GetPos() );	break;
			case I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC :
				{
					MATRIX mat;
					QUATERNION Quat;
					i3Matrix::Copy( &mat, pGameObject->getSceneObject()->GetMatrix());
					i3Matrix::GetRotateQuat( &Quat, &mat );
					GAMEEVENT::Write_Object( idx, EventSender, (UINT16) pGameObject->getCurrentHP(), pGameObject->GetPos(), &Quat );
				}
				break;
			case I3_NETWORK_GAMEOBJECT_TYPE_ANIM :
				{
					REAL32 rAnimLocalTime = 0.f;
					REAL32 rAnimDuration = 0.f;
					bool bIncludeAction = false;
					I3_OBJECT_STATE * pState = pGameObject->getState( pGameObject->getCurState());
					for( UINT32 j = 0; j < pState->m_pTimeEvent->getEventCount(); j++)
					{
						i3TimeEventInfo * pEventInfo = pState->m_pTimeEvent->getEvent( j);
						i3Action * pAction = pEventInfo->getAction();
						if( pAction == nullptr) continue;
						if( i3::same_of<i3ActionPlayAnim*>(pAction))
						{
							rAnimLocalTime = pState->m_pTimeEvent->getLocalTime();
							rAnimDuration = pState->m_pTimeEvent->getDuration();
							bIncludeAction = true;
							break;
						}
					}

					if( bIncludeAction == false)
					{
						i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pGameObject->getSceneObject() );
						if( pTrans != nullptr)
						{
							rAnimLocalTime = pTrans->getPlayTime();
							rAnimDuration = pTrans->GetDuration();
							if( pInfo->_fBroadcastTime == 0.f)
								GAMEEVENT::Write_Object_Animate( idx, EventSender, (UINT16) pGameObject->getCurrentHP(), pTrans->getPlayTime());

							pInfo->_fBroadcastTime += rDeltaTime;
							if( pInfo->_fBroadcastTime > pTrans->GetDuration() )
								pInfo->_fBroadcastTime = 0.f;
						}
					}
				}
				break;
			default :	break;
			}
		}
		else
		{	// Client(for Dedicated)
			T_OBJ objType = pInfo->_tNetData.getObjectType();

			if( objType & UDP_OBJECT_TYPE_HP )
			{
				REAL32 rDamage = pGameObject->getCurrentHP() - (REAL32) pInfo->_tNetData.getHP();

				if( rDamage > 0.f )
				{
					WeaponBase * pEventWeapon = nullptr;
					if( pInfo->_tNetData.getEventSender() != 0xFF)
					{// revision 37299  ������Ʈ ������ ��Ʈ��ũ ó���� ������ ����ó��
						CGameCharaBase * pDestroyer = g_pCharaManager->getCharaByNetIdx( pInfo->_tNetData.getEventSender());

						if( pDestroyer == nullptr )
						{
							I3PRINTLOG(I3LOG_NOTICE, "Invalid Destoryer : %d", pInfo->_tNetData.getEventSender() );
						}
						else
						{
							pEventWeapon	= pDestroyer->GetCurrentCharaWeapon();
						}
					}

					pGameObject->Damage( rDamage, pEventWeapon, nullptr);
				}
			}

			if( objType & UDP_OBJECT_TYPE_STATE)
			{	// ���Խÿ��� ó���Ǹ� ������Ʈ ���� ��ũ�� �����.
				// ����Ʈ, ���� ���� ó������ ������ Scene state�� ���ߵ��� �Ѵ�.
				if( IntrudeContext::i()->IsInterEnter() &&
					(CGameMissionManager::i()->isMissionObject( pGameObject) == false) )
				{
					// Netstate�� �ƴ� Object�� ���� State ��ȣ�Դϴ�.
					if( (INT32) pInfo->_tNetData.getState() < pGameObject->getStateCount() &&
						pInfo->_tNetData.getState() > pGameObject->getCurState() )
					{
						I3_OBJECT_STATE * pState = pGameObject->getState( pInfo->_tNetData.getState() );
						REAL32 rDamage = (1.f - pState->m_HPRatio) * pGameObject->getMaxHP();

						GlobalFunc::ChangeObjState_( pGameObject, pInfo->_tNetData.getState(), 0.f);

						pGameObject->setCurrentHP( pGameObject->getMaxHP() - rDamage);
						pInfo->_tNetData.setHP( (UINT16) pGameObject->getCurrentHP());
					}
				}
			}

			if( ((objType & UDP_OBJECT_TYPE_POSITION) > 0) ||
				((objType & UDP_OBJECT_TYPE_ROTATION) > 0) )
			{
				if( pGameObject->getDynamicState() == I3GAME_DYNAMIC_STATE_DYNAMIC)
					pGameObject->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC);

				if( pGameObject->getDynamicState() == I3GAME_DYNAMIC_STATE_KINEMATIC )
				{
					VEC3D vPos;
					REAL32	fSlerp;
					pInfo->_tAssistData._fAssistTime += rDeltaTime;
					if( pInfo->_tAssistData._fAssistTime >= OBJ_ASSIST_TIME )
						pInfo->_tAssistData._fAssistTime = OBJ_ASSIST_TIME;
					fSlerp = pInfo->_tAssistData._fAssistTime * OBJ_ASSIST_TIMEINV;

					if( objType & UDP_OBJECT_TYPE_POSITION)
					{
						VEC3D vecTemp;
						UDP_UTIL::ConvertNet3DtoVec3D( &vecTemp, pInfo->_tNetData.getPosition() );
						i3Vector::Lerp( &vPos, &pInfo->_tAssistData._vPostPos, &vecTemp, fSlerp );
						pGameObject->SetPos( &vPos);
					}
					else
					{
						i3Vector::Copy( &vPos, pGameObject->GetPos());
					}

					if( objType & UDP_OBJECT_TYPE_ROTATION)
					{
						i3Matrix::SetRotateQuat( pGameObject->getSceneObject()->GetMatrix(), pInfo->_tNetData.getRotation() );
						i3Matrix::PostTranslate( pGameObject->getSceneObject()->GetMatrix(), &vPos );
					}
				}
				else if(pGameObject->getDynamicState() == I3GAME_DYNAMIC_STATE_STATIC)
				{
					if( objType & UDP_OBJECT_TYPE_POSITION)
					{
						VEC3D vecTemp;
						UDP_UTIL::ConvertNet3DtoVec3D( &vecTemp, pInfo->_tNetData.getPosition() );
						pGameObject->SetPos( &vecTemp);
					}
				}
			}

			if( objType & UDP_OBJECT_TYPE_ANIMKEY)
			{
				// �׼��� ���� �ִ� ������Ʈ�� key�� ������ �ʰ� �׼� ������Ʈ �������� �����.
				bool bIncludeAction = false;

				I3_OBJECT_STATE * pState = pGameObject->getState( pGameObject->getCurState());
				for(UINT32 j = 0; j < pState->m_pTimeEvent->getEventCount(); j++)
				{
					i3TimeEventInfo * pEventInfo = pState->m_pTimeEvent->getEvent( j);
					i3Action * pAction = pEventInfo->getAction();
					if( pAction == nullptr) continue;
					if( i3::same_of<i3ActionPlayAnim*>(pAction))
					{
						bIncludeAction = true;
						break;
					}
				}

				if( bIncludeAction == false)
				{
					// 1�� �̻� ���� ���� ����
					INT32 maxBone = 0;

					if( pGameObject->getSceneObject()->GetBody() != nullptr)
					{
						i3AnimationContext * pCtx = pGameObject->getSceneObject()->GetBody()->getAnimationContext();
						maxBone = pCtx->getCombinerCount();
					}
					else
					{
						maxBone = pGameObject->getSceneObject()->GetBoneCount();
					}

					for( INT32 j = 0; j < maxBone; j++)
					{
						if( pGameObject->getSceneObject()->getAnimPlayCount( j) > 0)
						{
							REAL32 localTime = pGameObject->getSceneObject()->getAnimPlayTime( j);

							if( i3Math::abs( localTime - pInfo->_tNetData.getKeyFrame()) > 0.999f)
							{
								pGameObject->getSceneObject()->SetPlayTime( pInfo->_tNetData.getKeyFrame());
								I3TRACE( "AnimObject %d - %.4f\n", idx, pInfo->_tNetData.getKeyFrame());
							}
							break;
						}
					}
				}
			}

			pInfo->_tNetData.setObjectType( 0);
		}
	}
}

void	gstatemgr::_EnableHUD(bool bEnable, GAME_GUI_STATUS nState, bool bDeath)
{
	if( !bEnable)
	{
		m_bRespawnEnable = false;
		m_fRespawnDelayTime = 0.0f;
	}

	if(CHud::i() != nullptr)
		CHud::i()->SetEnableWithoutNotice( bEnable, nState, bDeath);
}

void gstatemgr::_UpdateWeaponLRU( REAL32 fDeltaTime )
{
	CGameCharaBase * pPlayer = nullptr;
	WeaponBase * pWeaponBase = nullptr;
	static REAL32 pRespawnLRUCountTime = 0.0f;
	pRespawnLRUCountTime += fDeltaTime;

	if( pRespawnLRUCountTime > 20.0f)
	{
		for(INT32 i=0; i< SLOT_MAX_COUNT; i++)
		{
			pPlayer = g_pCharaManager->getCharaByNetIdx(i);
			if(pPlayer)
			{
				pWeaponBase = pPlayer->GetCurrentCharaWeapon();
				if( pWeaponBase != nullptr )
				{
					WeaponLRU_Manager::i()->addCount(pWeaponBase->getWeaponInfo()->GetItemID());
				}
			}
			
		}
		pRespawnLRUCountTime = 0.0f;
	}
}
