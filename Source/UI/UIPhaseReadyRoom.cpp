#include "pch.h"
#include "UIPhaseReadyRoom.h"

#include "UIMainFrame.h"

#include "UIPopup.h"
#include "UIFloatReadyRoomInfo.h"
#include "UIFloatMiniProfile.h"
#include "UIFloatChat.h"
#include "UIFloatUserList.h"
#include "UIFloatPlayTimeAlarm.h"
#include "UIFloatCharacterView.h"
#include "UICharaView.h"
#include "UI/UITopMenu_V15.h"

#include "../CommunityContext.h"
#include "../AwayChecker.h"
#include "../TextSet.h"

#include "BreakIntoGame.h"

#include "Mode/Domination/Domination_Defines.h"
#include "Mode/Domination/DomiStageProperties.h"
#include "Mode/Domination/DomiSkillProperties.h"
#include "GameContextUtil.h"

#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/ColorString.h"

#include "InviteContext.h"
#include "RandomMapContext.h"
#include "ClientStageInfoContext.h"
#include "ClanGameContext.h"
#include "ChannelContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "QuickJoinContext.h"
#include "ClanSeasonContext.h"
#include "KickContext.h"
#include "StageBattle/AIModeContext.h"

#include "../StageBattle/NetworkContext.h"
#include "UIPhaseProfile.h"

#include "MedalManager.h"
#include "../RoomMasterKickManager.h"
#include "UIShopFunction.h"

#include "ClientStageInfoUtil.h"

#define SLOT_UPDATETIME	1.f




I3_CLASS_INSTANCE( UIPhaseReadyRoom);//, UIPhaseBase);

UIPhaseReadyRoom::UIPhaseReadyRoom() :
	m_pBeHostMessageBox(nullptr), 	m_bIsHost(false), m_OldHostIndex(-1), 
	m_transferHostSlotIdx(-1), m_rSearchHostDelay(0.0f), m_rInviteLobbyUserTimer(0.f), 
	m_pWaitInviteMB(nullptr), m_bInviteMessage(false), m_bInviteResult(false),
	m_nSelectedSlotIdx(-1), m_bEnableUpdate(false),
	m_bKickingState(false), m_curr_room(nullptr), m_pBtnReady(nullptr), m_pBtnStart(nullptr),
	m_bReadyEnable(false), m_bQueJumpToBattle( false)
{
	m_bLeaveTeamState = false;


	i3mem::FillZero(m_room_detail, sizeof(m_room_detail));
	

	i3mem::FillZero(m_pCountdownSound, sizeof(m_pCountdownSound));

	i3mem::FillZero(m_cbBox, sizeof(m_cbBox));
	

	// EXPORT_AVALIABLE_CODE_VALUE


	m_iRemainCount = -1;
	m_rRemainTime = 1.f;

	m_pPwEditBox		= nullptr;

	RegisterGameEventFunc( EVENT_READY_BATTLE,			&UIPhaseReadyRoom::ReceiveEvent_Ready_Battle);
	RegisterGameEventFunc( EVENT_START_GAME,			&UIPhaseReadyRoom::ReceiveEvent_Start_Game);
	RegisterGameEventFunc( EVENT_BATTLE_COUNTDOWN,		&UIPhaseReadyRoom::_EventProcess_BattlCountDown);
	RegisterGameEventFunc( EVENT_GET_INVITE_LOBBY_USER,	&UIPhaseReadyRoom::ReceiveEvent_GetLobbyUserInfo);
	RegisterGameEventFunc( EVENT_ENTER_LOBBY,			&UIPhaseReadyRoom::ReceiveEvent_EnterLobby);
	RegisterGameEventFunc( EVENT_M_KICK_USER,			&UIPhaseReadyRoom::ReceiveEvent_KickUser);
	RegisterGameEventFunc( EVENT_CHANGE_ROOM_OPTION_N,	&UIPhaseReadyRoom::_ModifyRoomOption);
	RegisterGameEventFunc( EVENT_UNREADY_4VS4_N,		&UIPhaseReadyRoom::ReceiveEvent_Unready_4vs4);
	RegisterGameEventFunc( EVENT_GET_ROOM_ADDINFO_N,	&UIPhaseReadyRoom::ReceiveEvent_GetRoomAddInfo);
	RegisterGameEventFunc( EVENT_M_DESTROYED_NETWORK,	&UIPhaseReadyRoom::ReceiveEvent_DestroyNetwork);
	RegisterGameEventFunc( EVENT_SEARCH_HOST,			&UIPhaseReadyRoom::ReceiveEvent_SearchHost);
	RegisterGameEventFunc( EVENT_REQUEST_HOST_CHANGE,	&UIPhaseReadyRoom::ReceiveEvent_Request_HostChange);
	RegisterGameEventFunc( EVENT_TRANSFER_HOST,			&UIPhaseReadyRoom::ReceiveEvent_Transfer_Host);
	RegisterGameEventFunc( EVENT_ITEM_AUTH,				&UIPhaseReadyRoom::ReceiveEvent_Item_Auth);
	RegisterGameEventFunc( EVENT_ITEM_AUTH_DATA,		&UIPhaseReadyRoom::ReceiveEvent_Item_Auth_Data);
	RegisterGameEventFunc( EVENT_M_RECEIVE_USER_SLOT,	&UIPhaseReadyRoom::ReceiveEvent_User_Slot);
	if(g_pEnvSet->IsV2Version())
		RegisterGameEventFunc( EVENT_GET_USER_EQUIPMENT_INFO, &UIPhaseReadyRoom::ReceiveEvent_Get_UserEquipmentInfo);
	else
		RegisterGameEventFunc( EVENT_GET_USER_EQUIPMENT_INFO, &UIPhaseReadyRoom::ReceiveEvent_Get_UserEquipmentInfo_V10);
	RegisterGameEventFunc( EVENT_CHANGE_READYROOM_INFO, &UIPhaseReadyRoom::ReceiveEvent_Change_ReadyRoomInfo);
	RegisterGameEventFunc( EVENT_DOMI_ENTER,			&UIPhaseReadyRoom::ReceiveEvent_Enter_Domination);
	RegisterGameEventFunc( EVENT_GET_ROOM_SCORE,		&UIPhaseReadyRoom::ReceiveEvent_RoomScore);
	RegisterGameEventFunc( EVENT_GIVEUP_BATTLE,			&UIPhaseReadyRoom::ReceiveEvent_Giveup_Battle);	
	RegisterGameEventFunc( EVENT_PLAYTIME_ALARM,			&UIPhaseReadyRoom::ReceiveEvent_PlayTimeAlarm);
	RegisterGameEventFunc( EVENT_SHUTDOWNTIME_ALARM,		&UIPhaseReadyRoom::ReceiveEvent_ShutDownTimeAlarm);
	RegisterGameEventFunc( EVENT_PCCAFE_CHANGED_STATUS,		&UIPhaseReadyRoom::ReceiveEvent_PCCafeChangedStatus);
	RegisterGameEventFunc( EVENT_FORCE_LEAVE_MATCHROOM,		 &UIPhaseReadyRoom::ReceiveEvent_ForceLeaveMatchroom);
}

UIPhaseReadyRoom::~UIPhaseReadyRoom()
{
	for( INT32 i = 0 ; i < ROOM_T_MAX; ++i)
	{
		I3_SAFE_DELETE( m_room_detail[i]);
	}
		
	for( INT32 i = 0; i < COUNTDOWN_COUNT; i++)
		I3_SAFE_RELEASE(m_pCountdownSound[i]);

}


READY_ROOM_TYPE UIPhaseReadyRoom::get_room_type( STAGE_ID e)
{
	return ROOM_T_NORMAL;
}

void UIPhaseReadyRoom::change_room_detail( STAGE_ID e)
{
	for( INT32 i = 0; i < ROOM_T_MAX; ++i)
		if(m_room_detail[i]) m_room_detail[i]->hide_frame();

	READY_ROOM_TYPE room_type = get_room_type( e);

	//���� �̼�
	if( room_type == ROOM_T_DOMI)
	{
		domi_state_info::i()->destroy();
		domi::StageProp::i()->Clear();
		domi::SkillProp::i()->Clear();
	}


	m_curr_room = m_room_detail[room_type];
	m_curr_room->show_frame();
}

/*virtual*/ void UIPhaseReadyRoom::_InitializeAtLoad( i3UIScene * pScene)
{

}

/*virtual*/ void UIPhaseReadyRoom::_ClearAtUnload( i3UIScene * pScene)
{
	UIPhaseBase::_ClearAtUnload( pScene);

	if( m_curr_room)
		m_curr_room->exit_start();

	for(size_t i=0;i<UserListMaxFrm;++i)
	{
		m_pMainWnd[i] = 0;
	}

	m_pBtnReady = nullptr;
	m_pBtnStart = nullptr;
}


void UIPhaseReadyRoom::_DoSearchHost( void)
{
	if (0.0f < m_rSearchHostDelay)								// ���� �Է� ����
		return;

	//[domi room]
	//_SetButtonState();
	m_curr_room->set_button_state();

	m_rSearchHostDelay = 3.0f;									// 3��

	if( _IsHost())												// ���忩�ο� ���� ����� �޶�����.
		GameEventSender::i()->SetEvent(EVENT_SEARCH_HOST);			// ������ �����ĺ��� ã�´�.
	else
		GameEventSender::i()->SetEvent(EVENT_REQUEST_HOST_CHANGE);	// ���� �����û �޽����� �����Ѵ�.
}

void UIPhaseReadyRoom::_DoInviteFromLobby(void)
{
	if( m_pWaitInviteMB == nullptr)								// ��� �޽��� -> Ack������ �ΰ�ó��
	{
		/*���ǿ� ��ġ�� ����ڵ��� ����� �ް� *//*�ֽ��ϴ�. ��ø� ��ٷ� �ֽʽÿ�.*/
		m_pWaitInviteMB = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, GAME_RCSTRING("STR_TBL_GUI_READY_GET_USERLIST_FROM_LOBBY1"));
		m_rInviteLobbyUserTimer = 0.0f;

		GameEventSender::i()->SetEvent(EVENT_GET_INVITE_LOBBY_USER);			// ������ ��û
	}
}

void UIPhaseReadyRoom::RunRandomMap(void)
{
	// ������ ���� �ý����� ������� ������ ���� ������ ����� ����.
	if (true == ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		if (!_IsHost())
			return;

		ROOM_INFO_ALL info = GetCurRoonInfo();

		// ���� �������� ������ ���.
		// ���� ���� �������� ����. ����Ʈ ��� �ο� ������ �ѱ��.
		CSI_STAGE_LIST temp_list = CStageInfoUtil::GetStageList(CStageInfoUtil::GetStageData(info._Basic._StageID)->m_i32RuleIdx);
		CSI_STAGE* stage = temp_list.GetStageData(0);

		// ���� ������ �ο� ���� �÷��� ����(�����ٲٸ� �ȵſ�).
		// 0x0000 0100 -- 8�� (4:4)
		// 0x0000 0010 -- 10�� (5:5)
		// 0x0000 0001 -- 16�� (8:8)
		const UINT8 _MaxIndex = 3;
		UINT8 _maxPersonNum[_MaxIndex] = { 16, 10, 8 }; // �������� bit shift.
		UINT8 _maxPersonFlag = 0x01;

		for (int i = 0; i < _MaxIndex; ++i)
		{
			if (stage->m_ui8MaxPerson == _maxPersonNum[i])
				break;

			_maxPersonFlag <<= 1;
		}

		info._Basic._StageID = stage->m_ui32StageID;
		info._Basic._OptionFlag = _maxPersonFlag;

		_UpdateMapInfo(&info);
	}
	else
	{
		if (!_IsHost() &&
			RandomMapContext::i()->IsStageListEmpty())
			return;

		ROOM_INFO_ALL info = GetCurRoonInfo();

		INT32 RandomMapPrev = RandomMapContext::i()->getPrevIndex();
		INT32 RandomMapCount = RandomMapContext::i()->GetStageListSize();
		INT32 RandomNumber = rand() % RandomMapCount;

		if (RandomMapPrev == RandomNumber && RandomMapCount > 1) //���� �ϳ� �̻� üũ�ɰ�츸 ������ �ݺ����� �ʵ��� ����
		{
			while (RandomMapPrev == RandomNumber)
			{
				RandomNumber = rand() % RandomMapCount;
			}
		}

		CSI_STAGE* stage = RandomMapContext::i()->GetStage(RandomNumber);
		info._Basic._StageID = stage->m_ui32StageID;

		RandomMapContext::i()->setPrevIndex(RandomNumber);

		_UpdateMapInfo(&info);
	}
	
}

ROOM_INFO_ALL UIPhaseReadyRoom::GetCurRoonInfo()
{
	ROOM_INFO_ALL info;

	MyRoomInfoContext::i()->getMyRoomInfo(&info._Basic);
	MyRoomInfoContext::i()->getMyRoomInfoAdd(&info._Add);

	return info;
}

void UIPhaseReadyRoom::_UpdateMapInfo( ROOM_INFO_ALL * pInfo)
{
	ROOM_INFO_ALL Info;
	i3mem::Copy( &Info._Basic,	&pInfo->_Basic, sizeof( ROOM_INFO_BASIC ));
	i3mem::Copy( &Info._Add,	&pInfo->_Add,	sizeof( ROOM_INFO_ADD ));

	// ����� ��忡 ���� �⺻ ���� ����
	STAGE_MODE type = StageID::GetStageMode(Info._Basic._StageID);


	// ������ ���� �ý����� ������� ������ ���� ������ ����� ����.
	if (true == ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		switch (type)
		{
		case STAGE_MODE_ANNIHILATION:
		case STAGE_MODE_BOMB:
		case STAGE_MODE_CONVOY:
		case STAGE_MODE_RUN_AWAY:
			Info._Add._SubType = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;
			break;
		case STAGE_MODE_ESCAPE:

			Info._Add._SubType = BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_2;
			break;

		case STAGE_MODE_CROSSCOUNT:
			Info._Add._SubType = BATTLE_TIME_TYPE_5 | BATTLE_ROUND_TYPE_2;
			break;

		case STAGE_MODE_DESTROY:
			Info._Add._SubType = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;
			break;

		case STAGE_MODE_DEFENCE:	// ������� �ֹ���� �⺻���� ����
			/*Info._Basic._WeaponFlag = */SETBIT(Info._Basic._WeaponFlag, LOCK_PRIMARY);
			Info._Add._SubType = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;
			break;

		default:
			Info._Add._SubType = BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;
			break;
		}
	}
	else
	{
		bool bProcess = TRUE;

		//�������� ��� ��Ÿ���� �ٲ����ʴ��� �ʼ����� �ʱ�ȭ ���� �ʵ��� �մϴ�.
		if (!MyRoomInfoContext::i()->IsRandomMap() || RandomMapContext::i()->GetPrevStageType() != type && MyRoomInfoContext::i()->IsRandomMap())
			bProcess = TRUE;
		else
			bProcess = FALSE;

		if (bProcess)
		{
			switch (type)
			{
			case STAGE_MODE_ANNIHILATION:
			case STAGE_MODE_BOMB:
			case STAGE_MODE_CONVOY:
			case STAGE_MODE_RUN_AWAY:
				Info._Add._SubType = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;
				break;
			case STAGE_MODE_ESCAPE:

				Info._Add._SubType = BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_2;
				break;

			case STAGE_MODE_CROSSCOUNT:
				Info._Add._SubType = BATTLE_TIME_TYPE_5 | BATTLE_ROUND_TYPE_2;
				break;

			case STAGE_MODE_DESTROY:
				Info._Add._SubType = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;
				break;

			case STAGE_MODE_DEFENCE:	// ������� �ֹ���� �⺻���� ����
				/*Info._Basic._WeaponFlag = */SETBIT(Info._Basic._WeaponFlag, LOCK_PRIMARY);
				Info._Add._SubType = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;
				break;

			default:
				Info._Add._SubType = BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;
				break;
			}
			RandomMapContext::i()->SetPrevStageType(type);
		}
	}
	

	// RPG7 ��� ����
	if( CStageInfoUtil::GetSlotMode(*CStageInfoUtil::GetStageData(Info._Basic._StageID)) != STAGE_SLOT_MODE_4VS4)
	{
		if( g_pConfigEx->GetGame().Enable_RPG7 == false || type == STAGE_MODE_DESTROY || type == STAGE_MODE_DEFENCE ||
			(g_pConfigEx->GetGame().Enable_RPG7 == true && g_pConfigEx->GetGame().UnlimitedForRPG7 == false))
		{
			if(TESTBIT(Info._Basic._WeaponFlag ,	LOCK_RPG7))
			{
				Info._Basic._WeaponFlag |= LOCK_PRIMARY;
				Info._Basic._WeaponFlag &= ~LOCK_RPG7;
			}
		}
	}

	GameEventSender::i()->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &Info );
}

void UIPhaseReadyRoom::_ModifyRoomOption(INT32, const i3::user_data&)
{
	//	�������� ��� ���� ���⸦ ����
	g_pFramework->EquipWeaponInGameMode();

	/*���� ��޿ɼ��� ����Ǿ����ϴ�.*/
	GameUI::PrintSystemChat( GAME_RCSTRING("STR_TBL_GUI_READY_CHANGED_OPTION"));

	// ������ �� ���� �����츦 ����.
	UIFloatReadyRoomInfo * pRoomInfo = static_cast<UIFloatReadyRoomInfo *>(GameUI::GetFloating( UFW_READYROOMINFO));
	if( pRoomInfo != nullptr)
		pRoomInfo->UpdateMapInfo();

	if (_IsHost())	return;		// ������ ó������ �ʴ´�

	/*���� ���������� ����Ǿ�\n�غ�Ϸ� ���¸� �����մϴ�.*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_CHANGED_WEAPON_LIMIT"));
}

void UIPhaseReadyRoom::_InitGMObserverEnable( void)
{
	LuaState* L = _CallLuaFunction("EnableGMObserver");

	if(L != nullptr)
	{	
		i3Lua::PushBoolean(L , UserInfoContext::i()->CheckAuthLevel() ); 
		i3Lua::PushInteger(L , UserInfoContext::i()->IsGM_Observer() ? 1 : 0); 
		_EndLuaFunction(L ,2);
	}
}

void UIPhaseReadyRoom::_UpdateGMObserverControlState()
{
	// revision 44121
	if( UserInfoContext::i()->CheckAuthLevel() && m_pGMObserver != nullptr)
	{
		ROOM_INFO_BASIC Info;
		MyRoomInfoContext::i()->getMyRoomInfo( &Info);

		switch( Info._State)
		{
		case ROOM_STATE_READY:
			{
				m_pGMObserver->setInputDisable( MyRoomInfoContext::i()->IsManualReady() );
			}
			break;

		case ROOM_STATE_COUNTDOWN_R:
			{
				m_pGMObserver->setInputDisable( true);
			}
			break;
		}	
	}
}

void UIPhaseReadyRoom::_ProcessInviteLobbyUser( REAL32 rDeltaSeconds)
{
	//�ʴ� �ϱ�
	if (m_pWaitInviteMB)
	{
		if (m_bInviteMessage && 1.0f <= m_rInviteLobbyUserTimer)		// �����̿� �� �ʱ�ȭ
		{
			GameUI::CloseMsgBox( m_pWaitInviteMB);
			m_pWaitInviteMB = nullptr;

			m_rInviteLobbyUserTimer = 0.0f;
			m_bInviteMessage = false;

			if( m_bInviteResult)										// �ʴ��ϱ� ��� ó��
			{
				GetMainFrame()->TogglePopup( UPW_INVITELOBBYUSER);
			}
			else														// ��Ȱ��ȭ Ǯ��
			{	/*���ǿ� ����ڰ� �����ϴ�.*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NO_USER_AT_LOBBY"));
			}
		}
		else
		{
			m_rInviteLobbyUserTimer += rDeltaSeconds;
		}
	}
}

void UIPhaseReadyRoom::_GetMissionRoundType(UINT8& subtype, STAGE_MODE mode)
{
	//�̷����� �Ͼ�� �ȵȴ�.
	if (mode == STAGE_MODE_NA) return;

	switch (mode)
	{
	case STAGE_MODE_ANNIHILATION:
#if defined( LOCALE_NORTHAMERICA )
		subtype = BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_5;
		break;
#endif
	case STAGE_MODE_BOMB:
	case STAGE_MODE_CONVOY:
	case STAGE_MODE_RUN_AWAY:
	case STAGE_MODE_DESTROY:		subtype = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break;
	case STAGE_MODE_DEFENCE:		subtype = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break;
	case STAGE_MODE_ESCAPE:			subtype = BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_2;		break;
	case STAGE_MODE_CROSSCOUNT:		subtype = BATTLE_TIME_TYPE_5 | BATTLE_ROUND_TYPE_2;				break;
	default:	// �⺻��ü�� ��Ÿ��
		subtype = BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;
		break;
	}
}

void UIPhaseReadyRoom::_SetAiModeConditon(STAGE_ID stageid)
{
}

void UIPhaseReadyRoom::_SetButtonTooltip( void)
{
	LuaState * L = _CallLuaFunction( "SetButtonToolTip");
	if( L != nullptr)
	{
		_EndLuaFunction( L, 0);
	}
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//

void UIPhaseReadyRoom::ProcessGameEvent( GAME_EVENT evt,INT32 arg, const i3::user_data& UserData)
{
	UIPhaseBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}

	//[domi room]
	if(evt != EVENT_ENTER_LOBBY)
		m_curr_room->process_update_slot();
}

void UIPhaseReadyRoom::Set_bGameStart(bool bFlag)
{ 
	m_bGameStart = bFlag;
}

void UIPhaseReadyRoom::StartCount(INT32 iCount)
{ 
	m_rRemainTime = 1.f;
	m_iRemainCount = iCount;
}

void UIPhaseReadyRoom::SetRemainCount(INT32 iCount)
{ 
	m_iRemainCount = iCount; 
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_BATTLE_COUNTDOWN, m_iRemainCount);
}

//------------------------------------------------------------------------------//
//							PhaseReadyRoom OnUpdate								//
//------------------------------------------------------------------------------//

void UIPhaseReadyRoom::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate( rDeltaSeconds);

	if (ChannelContext::i()->GetConnectedChannelType() == CHANNEL_TYPE_CLAN &&
		UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN &&
		!UserInfoContext::i()->CheckAuthLevel())								//MOD,GM�� Ŭ�� ������ ������� �������� �ʴ´�
	{
		//��ġ����ŷ�� �������
		if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
		{
			if (ClanGameContext::i()->getMyMercenaryState() == false &&
				GetMainFrame()->getDeprotationClanChannel() == false)
			{
				i3UI::setDisableUIInput(true);

				GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
				m_bMyPhase = false;
				return;
			}
		}
		else
		{
			OnGotoBackPhase();
	}
	}

	if(m_iRemainCount > 0)
	{
		m_rRemainTime -= rDeltaSeconds;
		if(m_rRemainTime <= 0)
		{
			m_rRemainTime = 1.f;
			DecreaseRemainCount();
		}
	}

	m_curr_room->set_button_state();

	//���� �˻�
	m_rSearchHostDelay = MAX(m_rSearchHostDelay - rDeltaSeconds, 0.f);

	// �ʴ��ϱ� �˸��˾� �����ð� ����
	_ProcessInviteLobbyUser( rDeltaSeconds);

	//[domi room]
	m_curr_room->update( rDeltaSeconds);

	_UpdateGMObserverControlState();

	// Ready ���� ���� Ȯ��
	I3ASSERT( m_pBtnReady != nullptr);
	I3ASSERT( m_pBtnStart != nullptr);

	// ������ ���� �ý����� ������� ������ ���� ������ ����� ����.
	if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		bool ready = true;

		if (MyRoomInfoContext::i()->IsRandomMap())
		{
			// �����κ��� ���� ���� �������� ����Ʈ ����.
			RandomMapContext::i()->TakeOverStageList();
			// �������� ���� �ִµ� ���� 0����� ��Ȱ��
			//if( RandomMapContext::i()->GetOrdListSize() == 0)
			if (RandomMapContext::i()->IsStageListEmpty())
			{
				ready = false;
			}
		}

		if (ready && !m_bReadyEnable)
		{
			m_pBtnReady->removeState(I3UI_STATE_DEACTIVATED);
			m_pBtnReady->setModifyState();
			m_pBtnStart->removeState(I3UI_STATE_DEACTIVATED);
			m_pBtnStart->setModifyState();

			m_bReadyEnable = true;
		}
		else if (!ready && m_bReadyEnable)
		{
			m_pBtnReady->addState(I3UI_STATE_DEACTIVATED);
			m_pBtnReady->setModifyState();
			m_pBtnStart->addState(I3UI_STATE_DEACTIVATED);
			m_pBtnStart->setModifyState();

			m_bReadyEnable = false;
		}
	}

	if( m_bQueJumpToBattle )
	{
		if( !CharaViewMgr::i()->isLoadWaiting() )	// �ƹ�Ÿ �ε����� �ƴ� ���
		{
			g_pFramework->JumpToBattle();
			SetInputDisable( false);
			m_bQueJumpToBattle = false;
		}
	}

	// ��й�ȣ ���� ���� üũ. ������ �ƴ� ��쿡�� ������Ʈ ���ݴϴ�.
	ROOM_INFO_ALL Info = GetCurRoonInfo();
	if((!i3::generic_is_equal(pw, MyRoomInfoContext::i()->getMyRoomPW())|| !i3::generic_is_equal(m_strTitle, Info._Basic._Title))
		&& !m_bIsHost)
	{
		SetLuaText();
	}
}


/*virtual*/ void UIPhaseReadyRoom::OnCreate( i3GameNode * pParent)
{

}

/*virtual*/ bool UIPhaseReadyRoom::OnKey_Escape( void)
{
	if (m_pWaitInviteMB) return false;	// �ʴ� �˾��� �������� ��� ����.

	if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) )
	{
		if( ClanGameContext::i()->getMyMercenaryState() == true )
		{
			Departured();
			return true;
		}
	}

	GameEventSender::i()->SetEventOnce( EVENT_ENTER_LOBBY);
	m_bMyPhase = false;
	
	return true;
}

/*virtual*/ void UIPhaseReadyRoom::OnGotoBackPhase( void)
{
	if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) )
	{
		if( ClanGameContext::i()->getMyMercenaryState() == true )
		{
			Departured();
			return; 
		}
	}

	// �κ�� ������.
	GameEventSender::i()->SetEventOnce( EVENT_ENTER_LOBBY);
	m_bMyPhase = false;
}

/*virtual*/ bool UIPhaseReadyRoom::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPhaseBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	// ReadyRoom�� ���ö� Add�Ѵ�
	GetMainFrame()->GetRoomMasterAwayCheckerManager()->AddAwayChekcer();

	//room type ����...
	change_room_detail( (STAGE_ID)MyRoomInfoContext::i()->GetMyRoomStageID());

	// �� ����� �ڽ��� �������� Ȯ���Ѵ�
	m_bIsHost = _IsHost() ? true : false;
	m_OldHostIndex = BattleSlotContext::i()->getMainSlotIdx();

	// Button�� Ű�� ����
	m_curr_room->set_button_state();

	// Button Tooltip ����
	_SetButtonTooltip();

	m_curr_room->empty_slot( 0); m_curr_room->empty_slot( 1);

	//[domi room]
	m_curr_room->process_update_slot();
	m_curr_room->grade_domi_room(false);

	_InitGMObserverEnable();

	m_nSelectedSlotIdx = -1;

	// Room Title ��� [�����̸� �����ѹ� >> ���ȣ : �� ����]
	SetRoomTitle();

	// ��й�ȣ On/Off ���� ���
	SetLuaText();

	SetFrmWndPos();

	// �强�� ������ ���޿��� Ȯ��
	if(UserInfoContext::i()->IsGeneralRecvData())
	{
		if(UserInfoContext::i()->IsGeneralMsgBox())
			return true;
		else
			UserInfoContext::i()->SetGeneralMsgBox();

		INVEN_BUFFER itemInfo = UserInfoContext::i()->GetGeneralRankInfo();

		// ���� üũ
		if(usf::GetItemStatus(itemInfo._ui32ItemID, itemInfo._TItemWareDBIdx) == ITEM_STATUS_EQUIPPED)
		{
			g_pFramework->EquipWeaponToMyInven(itemInfo._ui32ItemID, true);
		}

		GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PROMOTION4"));
	}

	m_bLeaveTeamState = false;

	CheckBeHost();

	//@eunil. PBBUG-17128 ����� ���� �� �������� �ʱ�ȭ.
	ROOM_INFO_ALL info;
	MyRoomInfoContext::i()->getMyRoomInfo(&info._Basic);
	STAGE_MODE type = StageID::GetStageMode(info._Basic._StageID);
	RandomMapContext::i()->SetPrevStageType(type);

	return true;
}

void UIPhaseReadyRoom::SetFrmWndPos()
{
	enum { Right_Dec = 8 };

	for (size_t idx = 0; idx < UserListMaxFrm; ++idx)
	{
		if (m_pMainWnd[idx] == nullptr)
			continue;

		REAL32 offset = 0.0f;

		// GM Observer Frame�� right ��ǥ�� �� ���� �ƴϱ� ������ ���� �� �߰�- sanghun.han(2014. 12. 19)
		if (idx == 2)
			offset = 193.0f;

		VEC3D* pos = m_pMainWnd[idx]->getPos();
		VEC2D* size = m_pMainWnd[idx]->getSize();

		m_pMainWnd[idx]->setPos((REAL32)(GameUI::AutoPosition::get_max_right() - size->x - Right_Dec - offset), pos->y);
	}
}


void UIPhaseReadyRoom::OnEntranceEnd()
{
	UIPhaseBase::OnEntranceEnd();

	m_curr_room->entrance_end();

	if( Domination_Util::g_renewal_best)
	{
		GetMainFrame()->OpenPopup( UPW_DOMI_SCORE);
		Domination_Util::g_renewal_best = false;
	}

	// ������ üũ
	if( 0 < UserInfoContext::i()->GetRankUpRank())
	{
		GetMainFrame()->OpenPopup(UPW_RANK_UP);
		UserInfoContext::i()->ResetRankUp();

		CCharaInfoContext::i()->SetOneCheckLevel(true);
	}

	// ����, �н����� �ʱ� ǥ��
	//_UpdateTitlePassword(true);

	// ������ ���� �ý����� ������� ������ ���� ������ ����� ����.
	if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		if (!_IsHost() && MyRoomInfoContext::i()->IsRandomMap())
		{
			m_curr_room->add_all_ramdom_map();
		}
	}

	if (UserInfoContext::i()->isEventReawardFlage())
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_INFORM_SUCCESS_GIFT"));
		UserInfoContext::i()->SetEventReawardFlag(false);
	}

	//���� ���� ä�� �޼����� ����.
	_ApplyMsgReadyBox();

	//�ð� ���� â�� ����.
	CheckTSReward();

	// ������ üũ
	if( UserInfoContext::i()->IsNewRepair() )
		GameUI::CheckWeaponEndurance();

	// Battle���� �����˾� �� ���� �� ó��
	if( g_pFramework->IsMessageBoxEnabled() )
	{
		// �ʴ� �޼����� �ƴҰ�쿡�� Inupt Disable
		if( !CCommunityContext::i()->IsReceivedInvite() )
			i3UI::setDisableUIInput( true);
	}

	//	�������� ��� ���� ���⸦ ����
	g_pFramework->EquipWeaponInGameMode();

	if (CCommunityContext::i()->IsMovingToUser())
	{
		OnInviteMove();
	}

	// �ʺ�/��� ä���̵� �˾�
	ChannelType channelType = ChannelContext::i()->GetConnectedChannelType();
	if (!ChannelContext::i()->IsValidChannelType(channelType))
	{
		switch(channelType)
		{
		case CHANNEL_TYPE_BEGIN1:
			/*Kill / Death ������ 40%�� �Ѿ� ���� ���� ȭ������ �̵��մϴ�. (K/D ���� 39%���� ���� ����)*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_LEAVE_SERVER_BY_KILL_RATE"), nullptr, MAKE_CALLBACK(CbMoveChannel), this);
			return;
		case CHANNEL_TYPE_BEGIN2:
			/*�ʺ� ������ ���� ������ ��ް� ���� �ʾ� ���� ���� ȭ������ �̵��մϴ�. (�� ��ޱ����� ���� ����)*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_WRONG_GRADE_FOR_BEGINNER"), nullptr, MAKE_CALLBACK(CbMoveChannel), this);
			return;
		case CHANNEL_TYPE_EXPERT:
			/*��� ���� ���� ������ ������Ű�� ���Ͽ� �������� ȭ������ �̵��մϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_WRONG_GRADE_FOR_VETERAN"), nullptr, MAKE_CALLBACK(CbMoveChannel), this);
			return;
		default:
			break;
		}
	}

	// Ȱ��ȭ�� �˾��� �ִ� ���, �˾����� ��Ŀ���� �ݴϴ�.
	if( GetMainFrame()->IsPopupFocused() )
		g_pFramework->SetFocus( GetMainFrame()->GetCurrentPopup() );
	else
		g_pFramework->SetFocus(nullptr);

	// ���뷱�� �޼��� ���
	if (MyRoomInfoContext::i()->IsTeamBalanceMessageFlag())
	{
		/*{col:255,120,0,255}�� �뷱�� �ɼǿ� ���� �� �� �ο����� ���߱� ���� �ڵ����� �����Ǿ����ϴ�.{/col}*/
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_AUTO_BALANCED"));
		MyRoomInfoContext::i()->SetTeamBalanceMessageFlag(false);
	}

	if( bing::is_break_into_game() == true )
	{
		bing::print_room_info();
	}

	if( UserInfoContext::i()->IsDefaultItemChanged() == true )
	{
		/*�⺻ ���� ����Ǿ����ϴ�.*/ /*�Ⱓ����*/
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_CHANGE_DEFAULT_ITEM"), &GAME_RCSTRING("STR2_EXPIRE_ITEM") );
		UserInfoContext::i()->SetDefaultItemChanged(false);
	}

	//��ų �޼��� �˾�
	if( CCharaInfoContext::i()->GetOneCheckLevel() == true)
	{
		if( CCharaInfoContext::i()->CheckAllCharaData() == true )
		{
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_SKILL_GETNEWTYPE"), nullptr, MAKE_CALLBACK(PopupTabSkill), this);
		}
	}

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}

	// keyword : Star Player
	if ( !MyRoomInfoContext::i()->getInReadyRoom() )
	{
		INT32 i32MySlotIdx = BattleSlotContext::i()->getMySlotIdx();
		MyRoomInfoContext::i()->PrintStarPlayerEntranceMsg(i32MySlotIdx);
		MyRoomInfoContext::i()->setInReadyRoom(true);
	}

	// ī��Ʈ �ٿ� Ȯ��
	if( MyRoomInfoContext::i()->GetCountDown() > 0)
	{
		m_bGameStart = true;
		GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(true);
		StartCount( MyRoomInfoContext::i()->GetCountDown());
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_BATTLE_COUNTDOWN, MyRoomInfoContext::i()->GetCountDown() );
		MyRoomInfoContext::i()->SetCountDown( 0);
	}

	if (QuickJoinContext::i()->wasPassingthroughLobby())
	{
		GetMainFrame()->InputDisableMainUI(false);
	}

	// ����뿡 ���ö� Location ���� �ֽ�ȭ
	GetMainFrame()->UpdateLocation();

	// �޴� ���� ���� â�� ���
	CheckQuickMedalReward();
	
	// �� �ϴ� UI ComboBox ���
	for(UINT8 i=COMBO_RULE; i<COMBO_MAX; i++)
		UpdateCBBoxInfo((COMBO_TYPE)i);

	CheckBeHost();

	//2014-08-18 ���� 9402 ����뿡 ���� �� �̴�����â�� ������ �����ϵ��� ����. //ENG_��뿵.
	GameEventSender::i()->SetEvent(EVENT_GET_REMAIN_MONEY);

	if(KickContext::i()->IsKickedByVote())
		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);

}
void UIPhaseReadyRoom::PopupTabSkill(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		if( g_pFramework->GetUIMainframe() == nullptr)
			return;

		UITopMenu * pMenu = g_pFramework->GetUIMainframe()->GetTopMenu();//OpenPopup( UPW_PROFILE);

		pMenu->Clicked(4);	// 4���� Profile

		UIPhaseProfile * pProfilePopup = static_cast<UIPhaseProfile*>(GameUI::GetPopup( UPW_PROFILE));
		if( pProfilePopup != nullptr)
			pProfilePopup->SetCurrentTab(UIPhaseProfile::TAB_SKILL);
	}
	else
	{
		CCharaInfoContext::i()->SetAllCharaData();
	}
}

bool UIPhaseReadyRoom::OnExitStart( void)
{
//	UI::remove_cb(this);

	if( UIPhaseBase::OnExitStart() == false)
		return false;

	if( GetMainFrame()->IsOpenedPopup(UPW_TS_REWARD))
	{
		GetMainFrame()->ClosePopup( UPW_TS_REWARD);
	}

	if( GetMainFrame()->IsOpenedPopup(UPW_USERPROFILE) )
	{
		GetMainFrame()->ClosePopup(UPW_USERPROFILE);
	}

	if( m_pBeHostMessageBox)
	{
		GameUI::CloseMsgBox( m_pBeHostMessageBox);
		m_pBeHostMessageBox = nullptr;
	}

	m_bGameStart = false;
	GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(false);

	m_curr_room->exit_start();

	return true;
}

/*virtual*/ void UIPhaseReadyRoom::OnExitEnd( void)
{
	UIPhaseBase::OnExitEnd();
	
	//g_pFramework->getChatBox()->setWhisperEnable( false);

	InviteContext::i()->ClearInviteInfo();
}

/*virtual*/ bool UIPhaseReadyRoom::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	switch(event)
	{
	case I3_EVT_INPUT:
		{
			i3InputDeviceManager * pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard * pKeyboard = pMgr->GetKeyboard();
		
			bool ready = true;

			// ������ ���� �ý����� ������� ������ ���� ������ ����� ����.
			if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
			{
				if (MyRoomInfoContext::i()->IsRandomMap())
				{	// �������� ���� �ִµ� ���� 0����� ��Ȱ��
					if (RandomMapContext::i()->IsStageListEmpty())
					{
						ready = false;
					}
				}
			}

			if (ready)
			{
				// F5 Ű �Է½� ����/��ŸƮ
				if (pKeyboard->GetStrokeState(I3I_KEY_F5))
				{
					// SubPhase == NONE , ���� �˾��� ������ ����
					if (GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE &&
						GetMainFrame()->IsPopupFocused() == false)
					{
						if (bing::is_break_into_game() == false) //���� ������ �ƴϸ� �ٷ� ready �Ѵ�.
						{
							//[domi room]
							//_DoReadyGame();
							m_curr_room->do_ready_game();
						}
						else
							GameEventSender::i()->SetEventOnce(EVENT_GET_ROOM_SCORE); //�� ���ھ� ������ ��û�Ѵ�.
					}
				}
			}
			
		}
		break;


	case UIEVENT_SET_FOCUS_POPUP:
		{
			if( code == I3_EVT_CODE_ACTIVATE)
			{
				m_curr_room->event_set_focus_popup();
			}
		}
		break;

	case UIEVENT_KILL_FOCUS_POPUP:
		{
			if( code == I3_EVT_CODE_ACTIVATE)
			{
				m_curr_room->event_kill_focus_popup();
			}
		}
		break;
	case ACE_AWAY:			
		//if(m_bGameStart) 
		//{
		//	__StopRoomMasterKick();
		//}
		GetMainFrame()->setProcessLeaveRoom(true);			
		GetMainFrame()->CloseAwayMessageBox();
		break;
	case ACE_CAUTION:
		{
			GetMainFrame()->OpenAwayMessageBox(GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_GET_AWAY"));/*��ð� �������� �����Ƿ� 5�� �� �κ�� �̵��մϴ�.*/
		}
		break;
	}

	return UIPhaseBase::OnEvent(event, pObj, param2, code);
}

/*virtual*/ bool UIPhaseReadyRoom::OnPlayClickSound(i3UIControl * pCtrl)
{
	// ������ �ƴ� ������ Ready ��ư Ŭ���� ����ó��
	if(!_IsHost() && i3::same_of<i3UIButton*>(pCtrl) && 
		i3::generic_is_iequal(pCtrl->GetName(), "Btn_Ready") )
	{
		// ������� -> OK , ���� ��� -> Cancel ���� ���
		if (SLOT_STATE_READY == BattleSlotContext::i()->getMySlotInfo()->_State)
		{
			return PlaySound(UISND_ITEM_CLICK_CANCEL);
		}
		else 
		{
			return PlaySound(UISND_ITEM_CLICK_OK);
		}
	}

	return UIBase::OnPlayClickSound(pCtrl);
}

void UIPhaseReadyRoom::ClickSlot( INT32 idx)
{
	if( idx < 0 || idx >= SLOT_MAX_COUNT)
		return;

	if( m_nSelectedSlotIdx == idx)
		return;

	m_nSelectedSlotIdx = idx;

	const SLOT_INFO* pSlotInfo = BattleSlotContext::i()->getSlotInfo( idx);

	if( SLOT_STATE_EMPTY == pSlotInfo->_State)
	{
		if( !g_pFramework->GetClanContext()->InBattleTeam())				//	Ŭ������ �ƴ� ��츸 �����ϵ���
		{
			INT32 nTeam;

			if (0 == idx % 2)
				nTeam = TEAM_RED;
			else
				nTeam = TEAM_BLUE;

			//[domi room]
			//_ChangeTeam(nTeam);
			m_curr_room->change_team( nTeam);
		}

	}
	else if( SLOT_STATE_CLOSE != pSlotInfo->_State)
	{
		// �� UI�� �ٷ� ĳ���� ������ ��û������..
		// Context �޴��� ����Ǿ ���� ��û�ϴ� ������� ����
		// Index�� ��������!!
		UIFloatCharacterView * pCharaView = static_cast<UIFloatCharacterView *>(GameUI::GetFloating( UFW_CHARACTER));
		if( pCharaView != nullptr)
			pCharaView->ChangeOtherUserChara(idx);
	}
}

void UIPhaseReadyRoom::_GetUserInfo(INT32 idxGameSlot)
{
	if( BattleSlotContext::i()->getMySlotIdx() != idxGameSlot)
	{
		GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_N, &idxGameSlot);
	}
}

void UIPhaseReadyRoom::ClickRSlot( INT32 idx)
{
	if( (idx >= 0 && idx <= SLOT_MAX_COUNT) == false ) //0���� ���� ���� ��ƿ��� �Ѿ� ���� ��찡 �ִ�.
		return; //�ùٸ� idx �� �ƴϹǷ� �����Ѵ�.

	m_nSelectedSlotIdx = idx;

	const SLOT_INFO* pSlotInfo = BattleSlotContext::i()->getSlotInfo( idx);

	if (SLOT_STATE_EMPTY != pSlotInfo->_State &&
		SLOT_STATE_CLOSE != pSlotInfo->_State)
	{
		ClickSlot( idx);

		i3::rc_wstring wstrUser = BattleSlotContext::i()->getNickForSlot(idx);
		if( i3::generic_string_size(wstrUser) > 0 )
		{
			GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_ROOM, &idx);

			// ���õ� ���� �� �ʱ�ȭ�մϴ�.
			{
				UIFloatUserList * pFrame = i3::same_cast<UIFloatUserList*>(GameUI::GetFloating(UFW_USERLIST));

				if(pFrame != nullptr)
					pFrame->ResetUserTab();
			}
		}
		PlaySound( UISND_ITEM_SELECTING);
	}
}

void UIPhaseReadyRoom::ClickCloseButton( INT32 idx)
{
	if( !_IsHost() || IsBattleCountDown() )
		return;

	// �ڱ� �ڽ��� �������� �ʵ��� �Ѵ�.
	if( BattleSlotContext::i()->getMainSlotIdx() == idx)
		return;

	// �Ʒ��� �� Ư�� �����̻��� ���� ���� �ʵ��� �մϴ�.
	if ((MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) ) )
	{
		if (8 <= idx)
		{
			return;
		}
	}

	if ( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) && !( idx <= 6 && idx % 2 == 0 ) )
	{
		return;
	}

	if( idx >= SLOT_MAX_COUNT)	// ���� ���� ������ ��� ���մϴ�.
		return;

	INT32 slotFlag = 0;
	const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo( idx);

	//	Ŭ������ �ƴ� ��츸 �����ϵ���
#if defined( TEST_CLAN_BATTLE)
	if(idxSlot >= 0)
		return;
#else
	if( g_pFramework->GetClanContext()->InBattleTeam())
		return;
#endif

#if defined( BUILD_RELEASE_QA_VERSION ) && defined( TEMP_QUEST_REWARD)
	// TODO: 4��4 ��忡���� �Ʒ� 8�� ������ �ݾƾ��ϳ�, �Ϲݸ�忡�� ������
	// ����ڰ� �Ʒ� ������ �����ϰ� ���� ��� ������ ���ǵǾ� ���� �ʴ�.
	// ���� ���ö����� ������ ���´�.

	// �Ʒ��� & 4vs4�̸� ���� 8�� ������ �۵� ����
	UINT32 StageID = MyRoomInfoContext::i()->GetMyRoomStageID();

	if (STAGE_SLOT_MODE_4VS4 == getStageSlot(StageID) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE))
	{
		if (8 <= idx)
		{
			return;
		}
	}
#endif

	// ���������� ���� �÷��� ����
	if (SLOT_STATE_CLOSE == pInfo->_State)
	{
		slotFlag = 0x10000000;
	}

	slotFlag |= idx;

	GameEventSender::i()->SetEvent(EVENT_CHANGE_SLOT_N, &slotFlag);
}

namespace
{

}

void UIPhaseReadyRoom::ClickButton( INT32 BtnIdx)
{
	// ���� ���̸� ��ư ������ �����սô�.
	if( MyRoomInfoContext::i()->GetMyRoomStageID() == STAGE_ID_NA )
		return;

	I3_BOUNDCHK( BtnIdx, RBS_MAX);

	switch(BtnIdx)
	{
	case RBS_SEARCH_HOST:	_DoSearchHost();						break;
	case RBS_INVITE :		_DoInviteFromLobby();					break;
	case RBS_ROOMSET :		m_curr_room->do_config_room();			break;
	case RBS_TEAMCHANGE :	m_curr_room->do_swap_team();			break;
	case RBS_READY :				
	case RBS_START :
		{
			if( bing::is_break_into_game() == false ) //���� ������ �ƴϸ� �ٷ� ready �Ѵ�.
			{
				m_curr_room->do_ready_game();
			}
			else
				GameEventSender::i()->SetEventOnce( EVENT_GET_ROOM_SCORE ); //�� ���ھ� ������ ��û�Ѵ�.
		}
		break;
	default :
		I3PRINTLOG(I3LOG_FATAL,  "not yet setting this function!!!");
		break;
	}
}

void UIPhaseReadyRoom::OpenPopupUsingCashItem(void)
{
	if( ! CInvenList::i()->IsUseBattleGiveupCashItem())
		return;

	UINT32 itemTerm = 0;
	UINT8 itemAttr = 0;
	UINT32 itemArg = 0;


	//	���� '���� �̵�' �������� ����ϰ� ������ ������ ��� �˸� �˾�â�� ����.
	INT32 itemFreeMove = CASHITEM_ITEMID_FREE_MOVE;
	INT32 itemBattleMove = CASHITEM_ITEMID_BATTLEFIELD_MOVE;

	//	'���� �̵�' ������ �켱
	if( CInvenList::i()->IsUsingCashItem(itemFreeMove))
	{
		T_ItemID ItemID = GetCashItemByGroup(itemFreeMove);

		CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM, ItemID, &itemAttr, &itemArg);

		//	���� ������� ���� �Ⱓ�� �˾ƿ´�.
		if( itemArg > 0)
		{
			itemTerm = UserInfoContext::i()->GetInventorySecTime(itemArg);
			if( itemTerm > 0)
			{
				//	�Ⱓ ���� - komet
				if( itemTerm >= 120)
					itemTerm -= 120;

				itemTerm = ConvertSecToUseDay(itemTerm);
			}
		}
		else
		{
			itemTerm = 0;
		}

		i3::stack_wstring wstrRemainTeam;

		if (1 < itemTerm)
		{
			//	���� �Ⱓ : %d��
			i3::sprintf(wstrRemainTeam, GAME_RCSTRING("STR_TBL_GUI_READY_REMAINING_DAYS1"),
				itemTerm, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));/*���� �Ⱓ : {col:255,255,255,255}%d%s{/col}*/
		}
		else if( itemTerm == 0)
		{
			//	���� �Ⱓ : %d��
			i3::sprintf(wstrRemainTeam, GAME_RCSTRING("STR_TBL_GUI_READY_REMAINING_DAYS2"),
				itemTerm, GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"));/*���� �Ⱓ : {col:255,255,255,255}%d%s{/col}*/
		}
		else
		{
			//	���� �Ⱓ : 1�� �̸�
			i3::sprintf(wstrRemainTeam, GAME_RCSTRING("STR_TBL_GUI_READY_REMAINING_DAYS2"),
				GAME_RCSTRING("STBL_IDX_ITEM_TERM_UNIT"), GAME_RCSTRING("STBL_IDX_ITEM_LESS_THEN"));/*���� �Ⱓ : {col:210,0,0,255}1%s %s{/col}*/
		}


		i3::rc_wstring wstrName; 
		g_pShopItemInfoDataBase->GetItemName(ItemID, wstrName);

		/*{col:255,153,51,255}%s{/col} �������� ����Ǿ����ϴ�.\n \n {col: 51,204,255,255}����ġ �� ����Ʈ�� ȹ���߽��ϴ�.\n ��Ż �г�Ƽ�� ������� �ʽ��ϴ�.\n ���� �� ų/������ ��ϵ��� �ʽ��ϴ�.{/col}\n \n %s*/
		i3::rc_wstring wstrMsg;
		i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_GUI_READY_ITEM_APPLYED_LEAVE_GAME"), wstrName, wstrRemainTeam);

		GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg);

		CInvenList::i()->SetUseBattleGiveupCashItem(false);
	}
	//	'���� �̵�' �������� ��� ����� �������
	//	�ΰ��ӿ��� '���� �̵�' �������� ����Ͽ� ������ ��� ����뿡 ������ ��� ����� �����κ��� �ް� �˸�â�� ����. komet
	else if( CInvenList::i()->GetLatestUsedCashItemFlag() == itemBattleMove)
	{
		T_ItemID ItemID = GetCashItemByGroup(itemBattleMove);

		i3::stack_wstring wstrRemainTeam;

		if( CInvenList::i()->GetItemTypeByItemID( INVEN::DB_CASHITEM, ItemID, &itemAttr, &itemArg))
		{
			itemTerm = itemArg;	//	������ ����

			i3::sprintf(wstrRemainTeam, GAME_RCSTRING("STR_TBL_GUI_READY_REMAINING_COUNT"), itemTerm);/*���� ���� : {col:255,255,255,255}%d��{/col}*/
		}

		i3::rc_wstring wstrName;
		g_pShopItemInfoDataBase->GetItemName(ItemID, wstrName);

		i3::rc_wstring wstrMsg;
		/*{col:255,153,51,255}%s{/col} �������� ����Ǿ����ϴ�.\n \n {col: 51,204,255,255}����ġ �� ����Ʈ�� ȹ���߽��ϴ�.\n ��Ż �г�Ƽ�� ������� �ʽ��ϴ�.\n ���� �� ų/������ ��ϵ��� �ʽ��ϴ�.{/col}\n \n %s*/
		i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_GUI_READY_ITEM_APPLYED_LEAVE_GAME"), wstrName, wstrRemainTeam);

		GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg);

		CInvenList::i()->SetUseBattleGiveupCashItem(false);
	}
}

void UIPhaseReadyRoom::CheckTSReward()
{
	//���� ���� �ڿ��.
	if( UserInfoContext::i()->IsTSRewardFlag() == true )
	{
		GetMainFrame()->OpenPopup( UPW_TS_REWARD );
		UserInfoContext::i()->SetTSRewardFlag(false);
		return;
	}

	if (UserInfoContext::i()->IsDailyGiftGauge() == false || UserInfoContext::i()->IsUserInfoDaily() == false)
		return;

	if (UserInfoContext::i()->GetUserInfoDaily()->_ui8GetItem == DAILY_GIFT_TYPE_MAX) 
		return;

	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_TS_EVENT) == false)
		return;

	if (UserInfoContext::i()->IsShowTSRewardMsg() == false)
		return;

	UserInfoContext::i()->SetShowTSRewardMsg(false);

	INT32 idx = UserInfoContext::i()->GetUserInfoDaily()->_ui8GetItem;
	INT32 max_gauge = UserInfoContext::i()->GetDailyGiftGauge(idx);

	i3::stack_wstring playTime;
	UINT32 minutes = (max_gauge - UserInfoContext::i()->GetUserInfoDaily()->_DGPlaytime) / 60;

	i3::sprintf(playTime, GAME_STRING("STR_POPUP_WHAT_MINUTE"), minutes);		//%d��

	i3::stack_wstring str(playTime);
	::ColorString(str, 255, 0, 0);

	i3::sprintf(playTime, GAME_STRING("STR_TBL_UI_TS_TIME"), str.c_str());		//%s �� �÷����Ͻø� �������� ������ �� �ֽ��ϴ�.

	g_pFramework->getChatBox()->PutSystemChatting(playTime);
}

void UIPhaseReadyRoom::CheckQuickMedalReward()
{
	// ���� ���� �޴��� ������, ���
	if( CMedalManager::i()->getQuickRewardCount() > 0)
	{
		GetMainFrame()->OpenPopup(UPW_MEDAL_QUICK_REWARD);
	}
}

void UIPhaseReadyRoom::ChangeGMObserver(bool bChecked)
{
	// üũ�� �ȵ��ִٸ� Observer ��� ���� �� Check ����
	// Check ���ִ� ���, Observer ��� ���� �� Check ����
	UserInfoContext::i()->SetEnable_GMObserver(bChecked != false);
}

void UIPhaseReadyRoom::ApplyLua_cbbox(COMBO_TYPE _cType, const i3::wliteral_range& wstrCondition, INT32 _cel)
{

}

void UIPhaseReadyRoom::ApplyLua_auto_team_change_enable(UINT32 stageID)
{

}

void UIPhaseReadyRoom::Apply_Password()
{
	LuaState* L = _CallLuaFunction("getPassword");
	const char* pszPassword = _ReturnLuaString(L, 0);

	if( i3::generic_is_iequal( MyRoomInfoContext::i()->getMyRoomPW(), pszPassword) == false)
	{
		INT32 Length = i3::generic_string_size( pszPassword);
		if(Length <= 0 )
		{
			MyRoomInfoContext::i()->ClearMyRoomPW();
		}
		else
		{
			//�ϴ� ���� ������ �н����带 ���� Clear �� �缳��
			MyRoomInfoContext::i()->SetMyRoomPW(pszPassword, Length);
		}
		GameEventSender::i()->SetEvent( EVENT_CHANGE_ROOM_PASSWORD_N);
	}
	SetLuaText();
}

void UIPhaseReadyRoom::AdjustRandomMapData(ROOM_INFO_ALL  &Info) const
{
	CSI_STAGE_LIST mapList;

	if (m_CurStageRule.m_i32RuleIdx == RULE_ID_EVENT)
		mapList = CStageInfoUtil::GetStageListofEvent();
	else
		mapList = CStageInfoUtil::GetStageList(m_CurStageRule.m_i32RuleIdx);

	if (MyRoomInfoContext::i()->IsRandomMap())
	{
		// ������ ���� �ý����� ������� ������ ���� ������ ����� ����.
		if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
		{
			RandomMapContext::i()->BackupStageList();
			RandomMapContext::i()->ClearStageList();

			for (INT32 i = 0; i < MAX_OPEN_STAGE; ++i)
				Info._Add._ui64OpenStage[i] = 0;

			for (size_t i = 0; i < mapList.GetStageListSize(); ++i)
			{
				CSI_STAGE* stage = mapList.GetStageData(i);
				RandomMapContext::i()->AddStage(stage);

				SET_ENABLE_RANDOM_MAP(Info._Add._ui64OpenStage, stage->GetStageUID(), TRUE);
			}

			Info._Basic._InfoFlag = SET_ROOM_INFO_RANDOM_MODE(Info._Basic._InfoFlag, true);
		}
	}
}

void UIPhaseReadyRoom::ChangeGameMode(INT32 _mode, INT32 _celidx)
{

}

void UIPhaseReadyRoom::UpdateCBBoxInfo(COMBO_TYPE BoxType)
{

}

void UIPhaseReadyRoom::SetLuaText()
{
	SetLuaPasswordOpt();
}

void UIPhaseReadyRoom::SetLuaPasswordOpt()
{
	i3::string_ncopy_nullpad(pw, MyRoomInfoContext::i()->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
	bool usePassword = ('\0' != pw[0]);

	LuaState* L = _CallLuaFunction("SetPasswordOpt");
	if(L != nullptr)
	{
		if(!usePassword)
		{
			i3Lua::PushInteger(L, 1);
			i3Lua::PushString(L, L"");
		}
		else
		{
			i3Lua::PushInteger(L, 2);
			i3Lua::PushString(L, pw);
		}
		_EndLuaFunction(L, 2);
	}
}

void UIPhaseReadyRoom::SetRoomTitle()
{
	ROOM_INFO_ALL info = GetCurRoonInfo();

	i3::stack_wstring _strTitle;

	INT32 serverIdx = g_pServerInfo->GetCurGameServerIndex();
	INT32 chIdx = g_pServerInfo->GetCurChannelNumber();
	ChannelType _channelType = ChannelContext::i()->GetChannelType(serverIdx, chIdx);
	
	i3::rc_wstring wstrTitle ;//= m_aawstrServerText[mainType][SS_SUBTYPE_CATEGORY_SLOT_TEXT];
	ChannelContext::i()->GetServerChannelText(wstrTitle, _channelType, SS_SUBTYPE_CATEGORY_SLOT_TEXT);

	i3::wstring wstrBuf;

	// ä������ [ä���̸� 00 >> 000����]
	i3::sprintf(wstrBuf, L" %02d", chIdx+1);
	i3::generic_string_cat(wstrTitle, wstrBuf);
	i3::generic_string_cat(wstrTitle, L" >> "); 
	i3::sprintf(wstrBuf, GAME_STRING("STBL_IDX_ROOM_TEXT1"), (info._Basic._Idx+1));
	i3::generic_string_cat(wstrTitle, wstrBuf);

	LuaState* L = _CallLuaFunction("SetRoomTitle");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrTitle);
		i3Lua::PushStringUTF16To8(L, info._Basic._Title);
		_EndLuaFunction(L ,2);
	}

	m_strTitle = info._Basic._Title;
}


//------------------------------------------------------------------------------//
//								Error Handler									//
//------------------------------------------------------------------------------//
void UIPhaseReadyRoom::EhKicked(bool isGM)
{
	if( m_bKickingState == false)
	{	
		// Top Menu�� �����ִ� �˾����� ��Ȱ��ȭ ��ŵ�ϴ�.
		GetMainFrame()->GetTopMenu_V15()->CloseEnterPopup();

		// Kick ���߽��ϴ�.
		// �κ�� �̵�
		m_bKickingState = true;

		if( isGM)	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_GM_GAME_KICK_EXIT"));
		else		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_KICKED"));
		
		i3UI::setDisableUIInput(true);
		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
		m_bEnableUpdate = false;
		m_bMyPhase = false;
	}
}


void UIPhaseReadyRoom::CbBeHost(void* pThis,INT32 nParam)
{
	((UIPhaseReadyRoom*)pThis)->m_pBeHostMessageBox = nullptr;
}

void UIPhaseReadyRoom::CbSearchHost(void* pThis,INT32 nParam)
{
	UIPhaseReadyRoom * pReadyRoom = static_cast<UIPhaseReadyRoom*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
	if( pReadyRoom != nullptr && MBR_OK == nParam)
	{
		INT32 slotIdx = pReadyRoom->m_transferHostSlotIdx;
		GameEventSender::i()->SetEvent(EVENT_TRANSFER_HOST, &slotIdx);
	}
}

void UIPhaseReadyRoom::cbQueryLeaveClanBattle(void * pThis, INT32 nParam)
{
	//	Ŭ�������� ������ ó��
	if( nParam == MB_OK)
	{
		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
		i3UI::setDisableUIInput( true);

		UIPhaseReadyRoom * pPhase = (UIPhaseReadyRoom *)pThis;
		pPhase->m_bMyPhase = false;
	}
}

void UIPhaseReadyRoom::CbMoveChannel(void* pThis,INT32 nParam)
{
	// ���/�ʺ� ä�� ������ �������� ���Ͽ� ä�� �̵�
	if (nParam == MB_OK)
	{
		ChannelContext::i()->SetMoveChannel(); 

		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
		i3UI::setDisableUIInput( true);

		UIPhaseReadyRoom * pPhase = (UIPhaseReadyRoom *)pThis;
		pPhase->m_bMyPhase = false;
	}
}

void UIPhaseReadyRoom::CbBlockUser(void* pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
	}
}

void UIPhaseReadyRoom::CbFinalComplateConfirm(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
	}
}

void UIPhaseReadyRoom::CbMoveShop(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		UIPhaseReadyRoom* pReadyRoom = (UIPhaseReadyRoom*)pThis;
		pReadyRoom->GetMainFrame()->ChangeSubPhase( USP_SHOP);
	}
}

void UIPhaseReadyRoom::CbDomiEnter(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
		if(pFrame->GetCurrentSubPhaseType() == USP_NONE && pFrame->IsPopupFocused() == false )
		{
			if( bing::is_break_into_game() == false ) //���� ������ �ƴϸ� �ٷ� ready �Ѵ�.
			{
				UIPhaseReadyRoom* pReadyRoom = (UIPhaseReadyRoom*)pThis;
				pReadyRoom->getCurrRoom()->do_ready_game();
			}
			else
				GameEventSender::i()->SetEventOnce( EVENT_GET_ROOM_SCORE ); //�� ���ھ� ������ ��û�Ѵ�.
		}
	}
}

void UIPhaseReadyRoom::Departured()
{
	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) &&
		ChannelContext::i()->GetBattleChannel() && m_bGiveupBattle)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_LEAVE_NOTICE_POPUP"), nullptr, MAKE_CALLBACK(UIPhaseReadyRoom::cbQueryLeaveClanBattle), this);

		return;
	}

	//g_pFramework->GetClanContext()->MatchTeamLeave();
	GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
	m_bMyPhase = false;
}

bool	UIPhaseReadyRoom::OnInviteMove( void)
{
	// �� UI�� �����ؼ� �ۼ��ؾ��Ѵ�..(���� �����Ѵ�.)
	bool bMove = false;

	if ( CCommunityContext::i()->GetMoveTargetMatch() > 0 )
		bMove = true;
	else
	{
		const CUserLocationInfo& UserLocInfo = CCommunityContext::i()->GetMoveTargetInfo();
		bMove = (gcu::IsSameRoom(UserLocInfo) == false); 
	}
	
	if (bMove)
	{
		GetMainFrame()->TogglePopup(UPW_NULL);		// ���� �ߺ�ó���ε� �ϴ�.. �ʿ���ٰ� �ǴܵǸ� ���߿� ����..
		
		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);	// ChangeStep or ChangePhase���� ���� �ʰ� �����ϰ� �̰����� ó���ϴ°� ���ڴ�....
															// MovePhase�� ��Ŷ�� ���� ������ �̺�Ʈó���Լ����� ȣ���..
		m_bMyPhase = false;

		return true;
	}

	return false;	
}

#include "UIUtil.h"
#include "UIUtil.h"
bool UIPhaseReadyRoom::OnRevive( i3RenderContext * pCtx)
{
	if( UIPhaseBase::OnRevive( pCtx) == false)
		return false;

	enum { Right_Dec = 8 };

	for (size_t idx = 0; idx < UserListMaxFrm; ++idx)
	{
		if (m_pMainWnd[idx] == nullptr)
			continue;

		REAL32 offset = 0.0f;

		// GM Observer Frame�� right ��ǥ�� �� ���� �ƴϱ� ������ ���� �� �߰�- sanghun.han(2014. 12. 19)
		if (idx == 2)
			offset = 193.0f;

		VEC3D* pos = m_pMainWnd[idx]->getPos();
		VEC2D* size = m_pMainWnd[idx]->getSize();

		m_pMainWnd[idx]->setPos((REAL32)(GameUI::AutoPosition::get_max_right() - size->x - Right_Dec - offset), pos->y);
	}

	for( UINT32 i = 0; i < ROOM_T_MAX; i++)
	{
		if( m_room_detail[i] != nullptr)
		{
			m_room_detail[i]->revive();
		}
	}

	return true;
}

void UIPhaseReadyRoom::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPhaseReadyRoom::ReceiveEvent_Ready_Battle( INT32 result, const i3::user_data&)
{
	GetMainFrame()->GetTopMenu_V15()->SetInputDisable(false);
		
	if (EV_IS_ERROR( result, EVENT_ERROR_NO_REAL_IP))
		g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_REAL_IP_S"));
	else if (EV_IS_ERROR( result, EVENT_ERROR_NO_READY_TEAM))
		g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_READY_TEAM_S"));
	else if (EV_IS_ERROR( result, EVENT_ERROR_NO_START_FOR_UNDER_NAT))
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_UNDER_NAT"));
	else if ( EV_IS_ERROR(  result, EVENT_ERROR_NO_CLAN_TEAM ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_NO_CLAN_TEAM"));
	else if ( EV_IS_ERROR(  result, EVENT_ERROR_NO_CLAN_TEAM_NOTFULL ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_TEAM_NOTFULL"));
	else if ( EV_IS_ERROR( result, EVENT_ERROR_NO_CLAN_TEAM_NOT_ALL_READY ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_NOT_ALL_READY"));
	else if ( EV_IS_ERROR( result, EVENT_ERROR_EVENT_READY_WEAPON_EQUIP ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_ERROR_READY_WEAPON_EQUIP"));
	else if ( EV_IS_ERROR( result, EVENT_ERROR_NO_CLAN_DUPLE_CLAN_TEAM ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_DUPLE_CLAN_TEAM"));
	else if( EV_IS_ERROR( result, EVENT_ERROR_PBV15_DOMI_LAST_WAVE_NANIB_FAIL) )
		GetMainFrame()->TogglePopup(UPW_DOMI_ENTER);
	else
		GameUI::ErrorHandleStage( EVENT_READY_BATTLE, result);

	//PlaySound( UISND_ERROR);
}


void UIPhaseReadyRoom::ReceiveEvent_Start_Game( INT32 result, const i3::user_data&)
{
	m_curr_room->exit_start();

	// stage ���� ���� ���� ������ ĳ���� ���ҽ��� ŵ����!
	Avatar * pAvatar = (Avatar*) g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	if( pAvatar != nullptr)
		pAvatar->KeepResource();

	// �������� �����ÿ��� �ݹ��� �����մϴ�.
//	UI::remove_cb(this); 

	if(  CharaViewMgr::i()->isLoadWaiting() )
	{	// ĳ���Ͱ� �ε����� ���� �ε��� �Ϸ� �ǰ� ������ �� �ֵ��� �Ѵ�.
		m_bQueJumpToBattle = true;
		SetInputDisable( true);
	}
	else		// �ش� stage�� ����
		g_pFramework->JumpToBattle();
}

void UIPhaseReadyRoom::_EventProcess_BattlCountDown( INT32 arg, const i3::user_data&)
{
	if ( GetMainFrame()->GetCurrentSubPhaseType() != USP_NONE)
	{
		I3PRINTLOG(I3LOG_NOTICE,"GameSubPhase : USP_NONE.");
		return;
	}

	if( arg < 0 )
	{
		m_bGameStart = false;
		GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(false);
		BattleSlotContext::i()->setSlotState( BattleSlotContext::i()->getMainSlotIdx(), SLOT_STATE_NORMAL);
		GetMainFrame()->GetTopMenu_V15()->SetInputDisable(false);

			//@eunil. �������� ����
		// �����̵� �ƴϵ� ���۹�ư �ؽ�Ʈ�� ����Ǿ�� �ؼ� ��ġ ���� �� ���� ���� (PBBUG-16420)
				LuaState * L = _CallLuaFunction("SetStartButtonText");
		if (L != nullptr)
				{
					i3Lua::PushBoolean(L, false);
					_EndLuaFunction(L, 1);
				}
			//

		// ���忡 ���� ���� ���� Count ���
		if(arg == -1)
		{			
			g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_COUNTDOWN_ABORTED_BY_ROOM_MASTER"));/*{col:255,120,0,255}������ ī��Ʈ �ٿ��� ��� �Ͽ����ϴ�.{/col}*/
		}
		// ����� ���� Ǯ�� ���� ���� Count ���
		else
		{
			g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_COUNTDOWN_ABORTED"));/*{col:255,120,0,255}�غ���°� ��� �����Ǿ� ī��Ʈ �ٿ��� ��ҵǾ����ϴ�.{/col}*/
		}

		StartCount(arg);
	}
	else
	{
		if( g_pFramework->GetClanContext()->InBattleTeam() && ClanSeasonContext::i()->IsPossibleSeasonMatch() )
		{
			_Process_ClanMatch_BattleCountDown(arg);
		}
		else
		{
			_Process_Normal_BattleCountDown(arg);
		}
	}
}

void UIPhaseReadyRoom::_Process_Normal_BattleCountDown(INT32 arg)
{
	// ���� ���� Count
	if(arg == COUNTDOWN_COUNT)
	{
		// ������ ���ӽ��� ��ư�� �������� ī���͸� �����ϱ� ���� ����Ѵ�.
		m_bGameStart = true;
		GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(true);
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_START_COUNTDOWN"));/*{col:255,120,0,255}ī��Ʈ �ٿ��� �����մϴ�.{/col}*/

		//@eunil. ���� ��ҷ� ����
		if (_IsHost())
		{
			LuaState * L = _CallLuaFunction("SetStartButtonText");
			if (L != nullptr)
			{
				i3Lua::PushBoolean(L, true);
				_EndLuaFunction(L, 1);
			}
		}
		//

		StartCount(arg);
	}
	
	if(0 < arg)
	{
		i3::rc_wstring wstr_countdownText;
		i3::sprintf(wstr_countdownText, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_PRE_GAME_START_ALARM"), arg);/*{col:255,120,0,255}���� ���� %d�� ��{/col}*/
		g_pFramework->getChatBox()->PutSystemChatting(wstr_countdownText);
	}
	
	if( arg == 0)
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_START_GAME"));/*{col:255,120,0,255}������ �����մϴ�.{/col}*/
	}

	if (0 < arg && arg <= COUNTDOWN_COUNT)
	{
		i3SoundContext* pSoundContext = g_pViewer->GetSoundContext();
		if( pSoundContext != nullptr)
			pSoundContext->Play(m_pCountdownSound[arg - 1]);
	}
}

void UIPhaseReadyRoom::_Process_ClanMatch_BattleCountDown(INT32 arg)
{
	if(arg == COUNTDOWN_COUNT)
	{
		m_bGameStart = true;
		GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(true);
		GameUI::PrintSystemChat( GAME_RCSTRING("STR_CLAN_HONORMATCH_EFFECT")); // Ŭ�� ������ ���� �մϴ�.
		GetMainFrame()->OpenPopup( UPW_CLANHONOR, nullptr);

		StartCount(arg);
	}

	if( arg == 0)
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_START_GAME"));/*{col:255,120,0,255}������ �����մϴ�.{/col}*/
	}
}

void UIPhaseReadyRoom::ReceiveEvent_GetLobbyUserInfo( INT32 result, const i3::user_data&)
{
	m_bInviteResult = EV_SUCCESSED( result);
	m_bInviteMessage = true;
}

void UIPhaseReadyRoom::ReceiveEvent_EnterLobby( INT32 result, const i3::user_data&)
{
	if( EV_SUCCESSED( result))
	{
		// keyword : Star Player
		MyRoomInfoContext::i()->setInReadyRoom(false);

		if (ChannelContext::i()->GetBattleChannel())
		{
			if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
			{
				ClanGameContext::i()->ResetMercRank();
			}

			if (ClanGameContext::i()->getClanBattleLobbyEnable() == true)
			{
				GetMainFrame()->MovePhase(UIPHASE_CLANLOBBY);
			}
			else
				GetMainFrame()->MovePhase(UIPHASE_LOBBY);
		}
		else
			GetMainFrame()->MovePhase(UIPHASE_LOBBY);

		ClanGameContext::i()->resetClanBattleTeamInfo();
	}
	else
	{
		CCommunityContext::i()->CompleteMovingToUser();
		m_bEnableUpdate = true;
		i3GuiRoot::setGuiDisableInput(false);

		GameUI::ErrorHandleStage( EVENT_ENTER_LOBBY, result);
	}

	m_bGameStart = false;
	GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(false);
	m_bKickingState = false;
	m_iRemainCount = 0;

	// [domi_room]
	m_curr_room->release_grade();
}

void UIPhaseReadyRoom::ReceiveEvent_KickUser( INT32 result, const i3::user_data&)
{
	if( EV_SUCCESSED( result))
	{
		// result�� 1(True)�� ������ ���� GM�� ���� ���� �� ��
		// GM������ ���� �Ǿ��� ���� 1�� �Ѱ��ݴϴ�. jinsik.kim(2015. 01. 14)
		bool isGM = result > 0 ? true : false;
		EhKicked(isGM);
		PlaySound( UISND_ERROR);
	}
}

void UIPhaseReadyRoom::ReceiveEvent_Unready_4vs4(INT32, const i3::user_data&)
{
	/*4vs4 �ο����ѿ� �ɷ�\n�غ�Ϸ� ���¸� �����մϴ�.*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_4VS4_LIMIT_COUNT"));
}

void UIPhaseReadyRoom::ReceiveEvent_GetRoomAddInfo( INT32, const i3::user_data&)
{
	// ������ �� ���� �����츦 ����.
	UIFloatReadyRoomInfo * pRoomInfo = static_cast<UIFloatReadyRoomInfo *>(GameUI::GetFloating( UFW_READYROOMINFO));
	if( pRoomInfo != nullptr)
		pRoomInfo->UpdateMapInfo();
}

void UIPhaseReadyRoom::ReceiveEvent_DestroyNetwork( INT32, const i3::user_data&)
{
	// ��Ʈ��ũ�� ��������
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DESTROYED_NETWORK_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseReadyRoom::ReceiveEvent_SearchHost( INT32 result, const i3::user_data&)
{
	i3::rc_wstring wstr_message;

	if (EV_SUCCESSED( result))
	{	
		/*���� ���� �˻� ���\n[%s]��\n������ ���� �Ͻðڽ��ϱ�?*/
		i3::sprintf( wstr_message, GAME_RCSTRING("STR_TBL_GUI_READY_MESSAGE_CHANGE_ROOM_MASTER"), BattleSlotContext::i()->getNickForSlot( result));
	}
	else
	{
		if( m_curr_room->is_alone())
		{
			/*������ �ѱ� ������ ����ڰ� �����ϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NO_EXIST_TO_CHANGE_ROOM_MASTER"));
			return;
		}

		if( NetworkContext::i()->isRealIp())
		{
			// ���常 RealIP��.
			/*���� ���� �˻� ���\n[%s]��\n���忡 ����ȭ�� �ٸ� ����ڰ� ������ ���� �Ͻðڽ��ϱ�?*/
			i3::sprintf( wstr_message, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_CHANGE_ROOM_MASTER_RESULT2"), BattleSlotContext::i()->getNickForSlot(BattleSlotContext::i()->getMySlotIdx()));
		}
		else
		{
			// �ƹ��� RealIP�� �ƴϴ�.
			/*���� ���� �˻� ���\n�����ϴ�.\n���忡 ����ȭ�� �ٸ� ����ڰ� ������ ���� �Ͻðڽ��ϱ�?*/
			i3::sprintf( wstr_message, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_CHANGE_ROOM_MASTER_RESULT1"));

		}
	}

	m_transferHostSlotIdx = result;
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstr_message, nullptr, MAKE_CALLBACK(CbSearchHost), this);
}

void UIPhaseReadyRoom::ReceiveEvent_Request_HostChange( INT32 result, const i3::user_data&)
{
	/*[%s]���� ����˻��� ��û�ϼ̽��ϴ�.*/
	i3::rc_wstring wstr_message;
	i3::sprintf( wstr_message, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_ROOM_MASTER_CHANGE_REQUEST"), BattleSlotContext::i()->getNickForSlot( result));
	GameUI::PrintSystemChat( wstr_message);
}

void UIPhaseReadyRoom::ReceiveEvent_Transfer_Host( INT32 result, const i3::user_data&)
{
	i3::rc_wstring wstr_message;
	// ���� ���� ����
	if (EV_SUCCESSED( result))
	{	
		/*[%s]���� ������ �Ǿ����ϴ�.*/
		i3::sprintf( wstr_message, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_CHANGED_ROOM_MASTER"), BattleSlotContext::i()->getNickForSlot( result));
		GameUI::PrintSystemChat( wstr_message);

		m_bIsHost = true;
		CheckBeHost();

	}
	else
	{
		/*������ �ѱ� ������ ����ڰ� �����ϴ�.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NO_EXIST_TO_CHANGE_ROOM_MASTER"));
	}
}

void UIPhaseReadyRoom::ReceiveEvent_Item_Auth( INT32 result, const i3::user_data&)
{
	if (EV_SUCCESSED( result))
	{ // ������ ������ ����
		OpenPopupUsingCashItem();
	}
	else
	{
		/*��Ʈ��ũ ������ ������ ������ �����߽��ϴ�.\n �ٽ� �õ��� �ּ���.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_ITEM_IDENTYFY_FAIL_BY_NETWORK_PROBLEM"));
	}
}

void UIPhaseReadyRoom::ReceiveEvent_Item_Auth_Data( INT32 result, const i3::user_data&)
{
	if (EV_FAILED( result))
	{
		// ������ ��� ����
		// �������� ����� �����Ͽ����ϴ�.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_3"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
	}
}

void UIPhaseReadyRoom::ReceiveEvent_User_Slot( INT32, const i3::user_data&)
{ 
	// ����, ��������� ���� Ȱ��ȭ�� ���¿����� ���ü� �ֽ��ϴ�.
	// --> ��������� ���� ��û��, EVENT_LEAVE_XXX ������ �̰��� ���´�..
	// ����, �Ұ����ϰ� ���ǹ��� �ɾ�� ��..(11.10.31.����)

	if (GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE )
		CheckBeHost();

	m_curr_room->grade_domi_room(false);
}

void UIPhaseReadyRoom::ReceiveEvent_Get_UserEquipmentInfo( INT32 result, const i3::user_data&)
{
	UIFloatCharacterView * pCharaWin = static_cast<UIFloatCharacterView *>(GameUI::GetFloating( UFW_CHARACTER));
	if( pCharaWin != nullptr)
		pCharaWin->UpdateCharacter( result);
}

void UIPhaseReadyRoom::ReceiveEvent_Get_UserEquipmentInfo_V10( INT32 result, const i3::user_data&)
{
	INT32 nTeam;

	if (0 == result % 2)
		nTeam = TEAM_RED;
	else
		nTeam = TEAM_BLUE;

	CharaViewMgr::i()->getView()->UpdateCharacter(result, true, WEAPON_SLOT_UNKNOWN, static_cast<TEAM_TYPE>(nTeam) );
}

// �� ������ �����
void UIPhaseReadyRoom::ReceiveEvent_Change_ReadyRoomInfo( INT32 result, const i3::user_data& UserData)
{
	//GetMainFrame()->GameEventTransferToPopup( EVENT_CHANGE_READYROOM_INFO, result, UserData);

	for (int i = 0; i < (int)COMBO_MAX; i++)
		UpdateCBBoxInfo((COMBO_TYPE)i);

	CheckBeHost();
	
	SetRoomTitle();
	SetLuaText();

	UIFloatCharacterView * pCharaView = static_cast<UIFloatCharacterView *>(GameUI::GetFloating(UFW_CHARACTER));
	if (pCharaView != nullptr) {
		pCharaView->DoUpdateCharacter();
}
}

void UIPhaseReadyRoom::ReceiveEvent_Enter_Domination( INT32 result, const i3::user_data&)
{
	GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_DOMI_GAME_CHANGE"), nullptr, MAKE_CALLBACK(CbDomiEnter), this);
}

void UIPhaseReadyRoom::ReceiveEvent_RoomScore( INT32, const i3::user_data&)
{
	if( GetMainFrame() != nullptr)
		GetMainFrame()->OpenPopup(UPW_BREAK_INTO_GAME, this);

	if( bing::is_break_into_game() == true )
	{
		bing::print_room_info();
	}
}

void UIPhaseReadyRoom::ReceiveEvent_Giveup_Battle(INT32, const i3::user_data&)
{
	//@eunil. ��ġ����ŷ ���� ����. Ŭ���� �ߵ� ���� ó����. �뺴�� ��� �ߵ� ��Ż�ڴ� �г�Ƽ�� �޴´�.
	if (ClanGameContext::i()->getMyMercenaryState())
		m_bGiveupBattle = true;

	// [domi_room]
	m_curr_room->grade_domi_room(true);
}

void UIPhaseReadyRoom::ReceiveEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

void UIPhaseReadyRoom::ReceiveEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

void UIPhaseReadyRoom::ReceiveEvent_PCCafeChangedStatus(INT32 arg, const i3::user_data&)
{
	
}

void UIPhaseReadyRoom::ReceiveEvent_ForceLeaveMatchroom(INT32 arg, const i3::user_data&)
{
	ClanGameContext::i()->setClanBattleLobbyEnable(true);
	GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
}