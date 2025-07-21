#include "pch.h"
#include "UIPhaseReadyRoom_V11.h"

#include "UI/UIMainFrame.h"
#include "UI/UIFloatMiniProfile.h"
#include "UITopMenu_V11.h"
#include "UIFloatChat_V11.h"
#include "UIFloatReadyRoomInfo_V11.h"
#include "UIPhaseLobby_V11.h"
#include "../UI/UICharaView.h"

#include "../UI/UIUtil.h"
#include "../AwayChecker.h"
#include "i3Base/ColorString.h"

#include "../StageBattle.h"
#include "UI/BreakIntoGame.h"

#include "../CommunityContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "RandomMapContext.h"
#include "ClientStageInfoContext.h"
#include "ChannelContext.h"
#include "ClanGameContext.h"
#include "UserInfoContext.h"
#include "../StageBattle/NetworkContext.h"
#include "ViewOtherUserItemContext.h"
#include "InviteContext.h"
#include "GameContextUtil.h"
#include "../RoomMasterKickManager.h"
#include "BattleServerContext.h"
#include "ClanSeasonContext.h"
#include "LobbyContext.h"

#include "UIPopupUserProfile_V11.h"

#include "MissionCard/MCardSet.h"
#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCard.h"
#include "LatinAmericaNationInfo.h"

//#include "../ItemDataBase.h"
//#include "UIPopup.h"

#define SLOT_UPDATETIME	1.f
#define	READY_BUTTON_DEACTIVE_TIMEOUT		5.0f		// �����ư Ŭ���� �Է¸��� Ÿ�Ӿƿ� �ð�
#define	READY_BUTTON_ACTIVE_DELAY			0.15f		// �����ư Ȱ��ȭ �ȴ����� �ٽ� ��ǲ���������� �ð�

static UIPhaseReadyRoom_V11 * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	I3ASSERT( pFrame);

	UIPhaseBase * pPhase = pFrame->GetCurrentPhase();

	if( i3::same_of<UIPhaseReadyRoom_V11*>(pPhase))
		return (UIPhaseReadyRoom_V11*) pPhase;
	return nullptr;
}

//All User Ready ���¿��� Control Input ����
//��, Ready Btn �� Ȱ��ȭ
namespace
{
	UITopMenu* topMenu = nullptr;
	i3::vector<i3UIControl*> ctrlList;
	UIFloatMiniProfile * pMiniProfile = nullptr;
	UIFloatReadyRoomInfo_V11 * pReadyRoomInfo = nullptr;

	void initCtrl(i3UIScene* pScene)
	{
		topMenu = g_pFramework->GetUIMainframe()->GetTopMenu();
		pMiniProfile = (UIFloatMiniProfile*)g_pFramework->GetUIMainframe()->GetFloating(UFW_MINIPROFILE);
		pReadyRoomInfo = (UIFloatReadyRoomInfo_V11*)g_pFramework->GetUIMainframe()->GetFloating(UFW_READYROOMINFO);

		ctrlList.clear();

		//i3UIFrameWnd2 Frame�� ���� �ڽĸ� �˻��Ѵ�.
		i3UIControl* pFrame = (i3UIControl*)pScene->FindChildByName("i3UIFrameWnd2");
		i3GameNode * pTemp = pFrame->getFirstChild();
		while( pTemp != nullptr)
		{
			//Ready ��ư ����
			if( pTemp->hasName() &&  (i3::generic_is_iequal( pTemp->GetName(), "btnStart") != 0 ) )
			{
				ctrlList.push_back( (i3UIControl*)pTemp );
			}
			pTemp = pTemp->getNext();
		}
	}

	void msgReadyRoomCtrl(const i3::wstring & msg)
	{
		g_pFramework->getChatBox()->PutSystemChatting(L"  ");
		g_pFramework->getChatBox()->PutSystemChatting( msg );
		g_pFramework->getChatBox()->PutSystemChatting(L"  ");
	}

	void inputEnable(bool enable)
	{
		if(topMenu != nullptr)
			topMenu->SetInputDisable( !enable );
		if(pMiniProfile != nullptr)
			pMiniProfile->SetInputDisable( !enable );
		if(pReadyRoomInfo != nullptr)
			pReadyRoomInfo->SetInputDisable( !enable );

		for( size_t i = 0; i < ctrlList.size(); i++)
		{
			ctrlList[i]->setInputDisable( !enable );
		}
	}

	void stopReadyRoomCtrl()
	{
		inputEnable( false );

		i3::wstring wstr_msg = i3::format_string( L"%s", GAME_RCSTRING("STR_TBL_GUI_READYROOM_STOP_GET_AWAY") );
		::ColorString( wstr_msg, 255, 0, 0 );

		msgReadyRoomCtrl( wstr_msg );
	}

	void freeReadyRoomCtrl()
	{
		inputEnable( true );
	}
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

	int UIPhaseReadyRoom_V11_SelectTeam( LuaState * L)
	{
		INT32 team = i3Lua::GetIntegerArg( L, 1);
		if( team < TEAM_COUNT)
		{
			UIPhaseReadyRoom_V11 * pReadyRoom = GetThis();
			if( pReadyRoom != nullptr)
				pReadyRoom->OnSelectTeam( (TEAM_TYPE) team);
		}
		return 0;
	}

	int UIPhaseReadyRoom_V11_ClickSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseReadyRoom_V11 * pReadyRoom = GetThis();
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickSlot( idx);
		return 0;

	}

	int UIPhaseReadyRoom_V11_ClickRSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseReadyRoom_V11 * pReadyRoom = GetThis();
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickRSlot( idx);
		return 0;
	}

	int UIPhaseReadyRoom_V11_CloseSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseReadyRoom_V11 * pReadyRoom = GetThis();
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickCloseButton( idx);
		return 0;
	}

	int UIPhaseReadyRoom_V11_ClickButton( LuaState * L)
	{
		INT32 btn = i3Lua::GetIntegerArg( L, 1);

		UIPhaseReadyRoom_V11 * pReadyRoom = GetThis();
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickButton( btn);
		return 0;
	}


	int UIPhaseReadyRoom_V11_ChangeGMObserver( LuaState * L)
	{
		bool bChecked = i3Lua::GetBooleanArg(L , 1);

		GetThis()->ChangeGMObserver(bChecked);
		return 0;
	}

}

LuaFuncReg	PhaseReadyRoom_V11_Glue[] =
{
	{ "SelectTeam",			UIPhaseReadyRoom_V11_SelectTeam			},
	{ "ClickSlot",			UIPhaseReadyRoom_V11_ClickSlot			},
	{ "ClickRSlot",			UIPhaseReadyRoom_V11_ClickRSlot			},
	{ "CloseSlot",			UIPhaseReadyRoom_V11_CloseSlot			},
	{ "ClickButton",		UIPhaseReadyRoom_V11_ClickButton		},
	{ "ChangeGMObserver",	UIPhaseReadyRoom_V11_ChangeGMObserver	},
	{ nullptr,		nullptr},
};

I3_CLASS_INSTANCE( UIPhaseReadyRoom_V11);

UIPhaseReadyRoom_V11::UIPhaseReadyRoom_V11() : 
	m_SlotUpdateTime(0.f), m_nOldViewSlot(-1), m_bChagingTeam(false), m_bOnceChatType(false),
	m_bOnceChatTypeAll(false), m_fViewPosX(0.4f), m_fViewPosY(0.8f), m_fViewPosZ(3.0f),
	m_fViewAngleY(-30.0f), m_pWaitInviteMB(nullptr), m_pBeHostMessageBox(nullptr), m_bChangeOptionNotify(false),
	m_pCharaAttachRoot(nullptr), m_bKickingState(false), m_bRecordWindowSliding(false), m_bInRecordWindow(true),
	m_rRecordSlideTimeStamp(0.f), m_bInviteResult(false), m_bInviteMessage(false), m_transferHostSlotIdx(-1), 
	m_rSearchHostDelay(0.f), m_rTeamChangeDelay(0.f), m_bReadyPushed(false), m_bIsReady(false), m_rReadyDelay(0.f),
	m_circleTerm(0.f), m_circleIdx(0), m_playCircleAnimation(false), m_teamChangePosX(0.f),
	m_nSelectedSlotIdx(-1), m_IndexMap(0), m_CheckMap(0), m_OldHostIndex(-1), m_bBeHostMessageOK(false),
	m_bPauseKickMaster(false), m_bAllUserReady(false), m_bRoomMsterAwayCountDown(false), m_bGameStart(false),
	m_RoomMasterAwayCheckerTime(0), m_pRoomMasterAwayChecker(nullptr),
	m_iRemainCount(-1), m_rRemainTime(1.f), m_pHostImage(nullptr),
	m_bEnableBlockEscape(false), m_rBlockEscapeTime(0.f), m_bGiveupBattle(false),
	m_ListViewBox_RedTeam(nullptr), m_ListViewBox_BlueTeam(nullptr)
{

	i3mem::FillZero(m_pCountdownSound, sizeof(m_pCountdownSound));
	m_AiModeState = AI_MODE_STATE_ORIGINAL;

	m_UserUI_NationMarkData._Reset();

	for (int i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		m_UI_NationMark[i].Reset();
	}

	RegisterGameEventFunc( EVENT_READY_BATTLE,			&UIPhaseReadyRoom_V11::ReceiveEvent_Ready_Battle);
	RegisterGameEventFunc( EVENT_START_GAME,			&UIPhaseReadyRoom_V11::ReceiveEvent_Start_Game);
	RegisterGameEventFunc( EVENT_BATTLE_COUNTDOWN,		&UIPhaseReadyRoom_V11::_EventProcess_BattlCountDown);
	RegisterGameEventFunc( EVENT_GET_INVITE_LOBBY_USER,		&UIPhaseReadyRoom_V11::ReceiveEvent_GetLobbyUserInfo);
	RegisterGameEventFunc( EVENT_ENTER_LOBBY,			&UIPhaseReadyRoom_V11::ReceiveEvent_EnterLobby);
	RegisterGameEventFunc( EVENT_M_KICK_USER,			&UIPhaseReadyRoom_V11::ReceiveEvent_KickUser);
	RegisterGameEventFunc( EVENT_CHANGE_ROOM_OPTION_N,	&UIPhaseReadyRoom_V11::_ModifyRoomOption);
	RegisterGameEventFunc( EVENT_UNREADY_4VS4_N,		&UIPhaseReadyRoom_V11::ReceiveEvent_Unready_4vs4);
	RegisterGameEventFunc( EVENT_GET_ROOM_ADDINFO_N,	&UIPhaseReadyRoom_V11::ReceiveEvent_GetRoomAddInfo);
	RegisterGameEventFunc( EVENT_M_DESTROYED_NETWORK,	&UIPhaseReadyRoom_V11::ReceiveEvent_DestroyNetwork);
	RegisterGameEventFunc( EVENT_SEARCH_HOST,			&UIPhaseReadyRoom_V11::ReceiveEvent_SearchHost);
	RegisterGameEventFunc( EVENT_REQUEST_HOST_CHANGE,	&UIPhaseReadyRoom_V11::ReceiveEvent_Request_HostChange);
	RegisterGameEventFunc( EVENT_TRANSFER_HOST,			&UIPhaseReadyRoom_V11::ReceiveEvent_Transfer_Host);
	RegisterGameEventFunc( EVENT_ITEM_AUTH,				&UIPhaseReadyRoom_V11::ReceiveEvent_Item_Auth);
	RegisterGameEventFunc( EVENT_ITEM_AUTH_DATA,		&UIPhaseReadyRoom_V11::ReceiveEvent_Item_Auth_Data);
	RegisterGameEventFunc( EVENT_M_RECEIVE_USER_SLOT,	&UIPhaseReadyRoom_V11::ReceiveEvent_User_Slot);
	RegisterGameEventFunc( EVENT_GET_USER_EQUIPMENT_INFO, &UIPhaseReadyRoom_V11::ReceiveEvent_Get_UserEquipmentInfo);
	RegisterGameEventFunc( EVENT_CHANGE_READYROOM_INFO, &UIPhaseReadyRoom_V11::ReceiveEvent_Change_ReadyRoomInfo);
	RegisterGameEventFunc( EVENT_GET_ROOM_SCORE,		&UIPhaseReadyRoom_V11::ReceiveEvent_RoomScore);
	RegisterGameEventFunc( EVENT_GIVEUP_BATTLE,			&UIPhaseReadyRoom_V11::ReceiveEvent_Giveup_Battle);	
	RegisterGameEventFunc( EVENT_PLAYTIME_ALARM,			&UIPhaseReadyRoom_V11::ReceiveEvent_PlayTimeAlarm);
	RegisterGameEventFunc( EVENT_SHUTDOWNTIME_ALARM,		&UIPhaseReadyRoom_V11::ReceiveEvent_ShutDownTimeAlarm);
	RegisterGameEventFunc( EVENT_FORCE_LEAVE_MATCHROOM,		 &UIPhaseReadyRoom_V11::ReceiveEvent_ForceLeaveMatchroom);
}

UIPhaseReadyRoom_V11::~UIPhaseReadyRoom_V11()
{
	INT32 i=0;

	for( i = 0; i < COUNTDOWN_COUNT; i++)
	{
		I3_SAFE_RELEASE(m_pCountdownSound[i]);
	}

	I3_SAFE_RELEASE( m_pRoomMasterAwayChecker);
}

/*virtual*/ void UIPhaseReadyRoom_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	initCtrl( pScene );

	m_pClanBattleMarkRed = (i3UIImageBoxEx*) pScene->FindChildByName( "i3UIImageBoxEx");
	I3ASSERT( m_pClanBattleMarkRed);

	m_pClanBattleMarkBlue = (i3UIImageBoxEx*) pScene->FindChildByName( "i3UIImageBoxEx1");
	I3ASSERT( m_pClanBattleMarkBlue);

	m_pGMObserver = (i3UICheckBox*) pScene->FindChildByName( "CkGMObserver");
	I3ASSERT( m_pGMObserver);

	m_ListViewBox_RedTeam = (i3UIListView_Box*)pScene->FindChildByName("i3UIListView_RedTeam");
	I3ASSERT(m_ListViewBox_RedTeam);
	
	m_ListViewBox_BlueTeam = (i3UIListView_Box*)pScene->FindChildByName("i3UIListView_BlueTeam");
	I3ASSERT(m_ListViewBox_BlueTeam);	



	i3UIFrameWnd* pMainFmw = (i3UIFrameWnd*) pScene->FindChildByName("i3UIFrameWnd2");

	this->GetSlide()->AddFrameWnd(pMainFmw, UISlide::R_TO_L);
	_NationInitAtLoad(pScene);
}

/*virtual*/ void UIPhaseReadyRoom_V11::_ClearAtUnload( i3UIScene * pScene)
{
	UIPhaseBase::_ClearAtUnload( pScene);

	m_pClanBattleMarkRed = nullptr;
	m_pClanBattleMarkBlue = nullptr;
	m_pGMObserver = nullptr;
}

void UIPhaseReadyRoom_V11::_DoReadyGame(void)
{
	//EhKicked(false);
	//���� ���� ó��
	if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
	{//�Ʒ���
		if( _IsHost() )
		{
			if(false == _IsReady())
			{
				g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_TRAINIG_TEAM_S"));
				return;
			}
		}
		else
		{
			//1. ������ �ƴҶ��� ���� �Ҷ��� üũ �մϴ�.
			if( (BattleSlotContext::i()->getSlotInfo(BattleSlotContext::i()->getMySlotIdx())->_State) == SLOT_STATE_NORMAL)
			{
				if( (BattleSlotContext::i()->getMySlotIdx() % 2) != (BattleSlotContext::i()->getMainSlotIdx() % 2) )
				{
					g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_TRAINIG_TEAM_S"));
					return;
				}


			}
		}
	}
	else
	{
		//�����϶��� ���� ó�� �մϴ�.
		if( _IsHost() && (false == _IsReady()) )
		{
			g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_READY_TEAM_S"));
			return;
		}
	}
//#if defined (USE_RANDOMMAP_MODE)
	if(MyRoomInfoContext::i()->IsRandomMap()&& _IsHost())
		RunRandomMap();
//#endif
	ROOM_INFO_BASIC	roomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);

	if (roomInfo._State != ROOM_STATE_READY)
	{
		INT32 my_team_count = 0;

		INT32 slot_idx = BattleSlotContext::i()->getMySlotIdx() % 2;
		while (slot_idx < SLOT_MAX_COUNT)
		{
			const SLOT_INFO* pSlot = BattleSlotContext::i()->getSlotInfo(slot_idx);

			if (SLOT_STATE_READY <= pSlot->_State)
			{
				my_team_count++;
			}

			slot_idx += 2;
		}

		struct _tag_enter_limit
		{
			INT32		limit_count;
			i3::string	str_notice;
		};

		_tag_enter_limit limit[STAGE_SLOT_MODE_MAX] = {
			{ 8,	"STR_TBL_GUI_LOBBY_LIMIT_ENTER_ROOM" },			/*������ ���ѵ� ���Դϴ�..*/
			{ 4,	"STR_TBL_GUI_READY_NOTICE_LIMIT_ENTER_4VS4" },	/*4vs4 ��� �ο��� �������� ������ �� �����ϴ�.*/
			{ 5,	"STR_TBL_GUI_READY_NOTICE_LIMIT_ENTER_5VS5" }	/*5vs5 ��� �ο��� �������� ������ �� �����ϴ�.*/
		};

		STAGE_SLOT_MODE eMode = CStageInfoUtil::GetSlotMode(*MyRoomInfoContext::i()->getStage());

		if (limit[eMode].limit_count <= my_team_count)
		{
			g_pFramework->PutSystemChatting(GAME_RCSTRING(limit[eMode].str_notice));
			return;
		}
	}
	/*
	������ ���� ��� Ȧ��
	if( m_pSelectRandomMap->getCheckBoxState())
	g_pGameContext->AddGameFlag( GAMEFLAG_RANDMAPCHECKED);
	else
	g_pGameContext->RemoveGameFlag( GAMEFLAG_RANDMAPCHECKED);
	*/
	if( !m_bReadyPushed )
	{
		GameEventSender::i()->SetEvent( EVENT_READY_BATTLE );
		_ReadyButtonActive( false );
	}
}

void UIPhaseReadyRoom_V11::_DoSearchHost( void)
{
	// ���� �Է� ����
	if (0.0f < m_rSearchHostDelay)
		return;

	_SetButtonState();
	m_rSearchHostDelay = 3.0f;	// 3��

	// ���忩�ο� ���� ����� �޶�����.
	if( _IsHost())
	{
		// ������ �����ĺ��� ã�´�.
		GameEventSender::i()->SetEvent(EVENT_SEARCH_HOST);
	}
	else
	{
		// ���� �����û �޽����� �����Ѵ�.
		GameEventSender::i()->SetEvent(EVENT_REQUEST_HOST_CHANGE);
	}
}

void UIPhaseReadyRoom_V11::_DoInviteFromLobby(void)
{
	if( m_pWaitInviteMB == nullptr)
	{
		// ��� �޽��� -> Ack������ �ΰ�ó��
		/*���ǿ� ��ġ�� ����ڵ��� ����� �ް� *//*�ֽ��ϴ�. ��ø� ��ٷ� �ֽʽÿ�.*/
		m_pWaitInviteMB = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, GAME_RCSTRING("STR_TBL_GUI_READY_GET_USERLIST_FROM_LOBBY1"));
		m_rInviteLobbyUserTimer = 0.0f;
		// ������ ��û
		GameEventSender::i()->SetEvent(EVENT_GET_INVITE_LOBBY_USER);
	}
}

void UIPhaseReadyRoom_V11::_DoConfigRoom(void)
{
	if( _IsHost())
	{
		bool isLobby = false;

		GetMainFrame()->TogglePopup( UPW_CREATEROOM_EX, &isLobby );
	}
}

void UIPhaseReadyRoom_V11::OnSelectTeam( TEAM_TYPE team)
{
	// ��� �ִ� ������ ã��.
	INT32 i = 0;

	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const SLOT_INFO * pInfo = BattleSlotContext::i()->getSlotInfo( i);

		if( i % 2 != team)
			continue;

		if( SLOT_STATE_EMPTY == pInfo->_State)
		{	// ��� �ִ�.
			//	Ŭ������ �ƴ� ��츸 �����ϵ���
			if( !g_pFramework->GetClanContext()->InBattleTeam())
			{
				_ChangeTeam( team);
			}
		}
	}
}


bool UIPhaseReadyRoom_V11::_IsReady(void)
{
	INT32 i = 0;

	if ((MyRoomInfoContext::i()->getStage()->IsAiMode())) 
	{
		// �Ʒ����� ������� ������Ѵ�.
		for( i = 0; i < SLOT_MAX_COUNT; i++)
		{
			const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo(i);

			if (pInfo->_State != SLOT_STATE_CLOSE && pInfo->_State != SLOT_STATE_EMPTY)
			{
				INT32 myTeam = BattleSlotContext::i()->getMySlotIdx() % 2;
				INT32 slotTeam = i % 2;

				if (myTeam != slotTeam)
				{
					return false;
				}
			}
		}

		return true;
	}
	else
	{
		bool bBeRed = false;
		bool bBeBlue = false;

		for( i = 0; i < SLOT_MAX_COUNT; i++)
		{
			const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo(i);

			if( SLOT_STATE_EMPTY != pInfo->_State && SLOT_STATE_CLOSE != pInfo->_State)
			{
				if( 0 == i % 2)
				{
					bBeRed = true;
				}
				else
				{
					bBeBlue = true;
				}
			}
		}

#if defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE) //Ŭ���̾�Ʈ ȥ�� �����ϱ�
		return true;
#else
		return bBeRed && bBeBlue;
#endif
	}
}

void UIPhaseReadyRoom_V11::RunRandomMap(void)
{
	// ������ ���� �ý����� ������� ������ ���� ������ ����� ����.
	if (true == ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		if (!_IsHost())
			return;

		ROOM_INFO_ALL info;
		MyRoomInfoContext::i()->getMyRoomInfo(&info._Basic);
		MyRoomInfoContext::i()->getMyRoomInfoAdd(&info._Add);

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

		ROOM_INFO_ALL info;
		MyRoomInfoContext::i()->getMyRoomInfo(&info._Basic);
		MyRoomInfoContext::i()->getMyRoomInfoAdd(&info._Add);

		INT32 RandomMapPrev = RandomMapContext::i()->getPrevIndex();
		INT32 RandomMapCount = RandomMapContext::i()->GetStageListSize();

		if (RandomMapCount <= 0)
			return;

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

void UIPhaseReadyRoom_V11::_UpdateMapInfo( ROOM_INFO_ALL * pInfo)
{
	ROOM_INFO_ALL Info;
	i3mem::Copy( &Info._Basic,	&pInfo->_Basic, sizeof( ROOM_INFO_BASIC ));
	i3mem::Copy( &Info._Add,	&pInfo->_Add,	sizeof( ROOM_INFO_ADD ));

	// ����� ��忡 ���� �⺻ ���� ����
	STAGE_MODE type = StageID::GetStageMode( Info._Basic._StageID);
		
	bool bProcess = true;
		
	// ������ ���� �ý����� ������� ������ ���� ������ ����� ����.
	if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		// V1.5 �ڵ�� �����մϴ�.
		if (!MyRoomInfoContext::i()->IsRandomMap() || RandomMapContext::i()->GetPrevStageType() != type && MyRoomInfoContext::i()->IsRandomMap())
			bProcess = true;
		else
			bProcess = false;
	}

	if (bProcess)
	{
		switch (type)
		{
		case STAGE_MODE_ANNIHILATION:
		case STAGE_MODE_BOMB:
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

	

	
	// RPG7 ��� ����
	if( CStageInfoUtil::GetSlotMode(*MyRoomInfoContext::i()->getStage()) != STAGE_SLOT_MODE_4VS4)
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

void UIPhaseReadyRoom_V11::CheckBeHost(void)
{
	if (!_IsHost()) m_bIsHost = false;

	if( m_OldHostIndex != BattleSlotContext::i()->getMainSlotIdx())
	{
		// ������ ����Ǹ� ��ư�� �ٽ� ����
		_SetButtonState();

		m_OldHostIndex = BattleSlotContext::i()->getMainSlotIdx();
	}

	// �̹� �����̸� ó������ �ʴ´�
	if (m_bIsHost)	return;

	// ������ �Ǿ��ٸ� �˾� ���
	if (_IsHost())
	{	
		if (!MyRoomInfoContext::i()->IsTeamChangeFlag())
		{
			i3::wstring wstrBuf;
			i3::sprintf(wstrBuf, GAME_RCSTRING("STBL_IDX_EP_ROOM_BE_HOST"), BattleSlotContext::i()->getNickForSlot(BattleSlotContext::i()->getMySlotIdx()));

			m_pBeHostMessageBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrBuf, nullptr, MAKE_CALLBACK(CbBeHost), this);

			m_bIsHost = true;
		}
	}
}

void UIPhaseReadyRoom_V11::_SetSlotInfo( INT32 slot)
{
	i3::rc_wstring szRankTooltip;

	const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo( slot);

	// ����
	UI_V11_READYSTATE state = UIRS_V11_NONE;

	if( slot == BattleSlotContext::i()->getMainSlotIdx())
		SetHostMark(slot,7);

	switch( pInfo->_State)
	{
	case SLOT_STATE_EMPTY :					break;
	case SLOT_STATE_CLOSE :					break;
	case SLOT_STATE_SHOP :		state = UIRS_V11_SHOP;	break;
	case SLOT_STATE_INFO :				// ��������
	case SLOT_STATE_CLAN :					break;			// Ŭ��
	case SLOT_STATE_INVENTORY :	state = UIRS_V11_INVEN;	break;
	case SLOT_STATE_NORMAL :	break;
	case SLOT_STATE_READY :		state = UIRS_V11_READY;	break;
	case SLOT_STATE_LOAD :				// (������) �ε����Դϴ�.			Time Out ( 2��)     ó������ ��������� (10��)
	case SLOT_STATE_RENDEZVOUS :		// (������) �����伭�� Ȧ��Ī		Time Out ( )
	case SLOT_STATE_PRESTART :			// (������) Ȧ��Ī					Time Out ( )
	case SLOT_STATE_BATTLE_READY :		// (������) Ȧ��Ī �Ϸ��� ���� ������ ��ٸ� 
	case SLOT_STATE_BATTLE :			// (������) �������Դϴ�.
		state = UIRS_V11_PLAY;
		break;
	case SLOT_STATE_GACHA :				// �̱���� ����
		state = UIRS_V11_RS;
		break;
	}
	// ����� ǥ���մϴ�.
	INT32 rank;
	if( ChannelContext::i()->IsBeginnerChannel1() )
	{	
		rank = (INT32)(RANK_NEWBIE);
	}
	else
	{	
		rank = (INT32)(pInfo->_Rank);
	}

	if (ChannelContext::i()->GetBattleChannel() &&
		UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
	{
		if (ClanGameContext::i()->GetMercRank(slot) != -1) // ���� ���� ���� �뺴�� �ƴ�
			rank = ClanGameContext::i()->GetMercRank(slot);
	}

	i3::generic_string_copy( szRankTooltip, GetCharaRank( rank));

	// �ڱ� ���� ����
	if (BattleSlotContext::i()->getMySlotIdx() == slot)
		_EffectSlot( slot);

	//	Ŭ�� ��ũ
	i3::stack_wstring wstrClanToolTip;

	if( pInfo->_clanidx > 0 && pInfo->_clanstate != CLAN_MEMBER_LEVEL_UNKNOWN)
	{	
		// Ŭ����ũ ���� ( ��ġ�� ���, �д������ ���� ����)
		if( g_pFramework->GetClanContext()->InBattleTeam() )
		{
			i3::wstring wstrTeamName;
			TEAM_TYPE team_type = CHARA::CharaTeamType2TeamType( BattleSlotContext::i()->getTeam(slot) );

			const CLAN_MATCH_BATTLE_TEAM * pTeamInfo = ClanGameContext::i()->getClanBattleTeamInfo(team_type);

			GetBattleTeamName( wstrTeamName, pTeamInfo->getMatchTeamID() );
			i3::sprintf( wstrClanToolTip, L"%s %s", pTeamInfo->getClanName(), wstrTeamName);		// TODO : �ڵ� ����ȯ�ǰ� ����.(2014.07.18.����)
		}
		else
		{
			// Ŭ����� ǥ��
			i3::rc_wstring wstrTemp; 
			GetClanRankPreName2( wstrTemp, pInfo->_clanExpRank);

			i3::sprintf( wstrClanToolTip, L"%s %s",wstrTemp, pInfo->_ClanName);		// TODO : �ڵ� ����ȯ�ǰ� ����.(2014.07.18.����)
		}
	}
	UI_V11_READYSTATE pcState = UIRS_V11_NONE;
	// PC�� ǥ��
	switch( pInfo->_pcCafe )
	{
	case PC_NOT_CAFE:	pcState = UIRS_V11_NONE; break;
	case PC_NORMAL_CAFE:			pcState = UIRS_V11_PCCAFE;		break;
	case PC_PREMIUM_CAFE:			pcState = UIRS_V11_PCCAFEPLUS;		break;
	case PC_VACATION_EVENT_ITEM:	pcState = UIRS_V11_NONE;		break;
	case PC_INDO_TELCOM_ITEM:	
		//I3ASSERT_0 ����.
		pcState = UIRS_V11_NONE;		
		break; 
	case PC_GARENA_GCA:
		pcState = UIRS_V11_PCCAFEPLUS;
		break;
	case 0xFF:						pcState = UIRS_V11_NONE;	break;
	default:
		pcState = UIRS_V11_NONE;
		//I3ASSERT_0;
		break;
	}

	//	���� E-Sport �±�
	INT32 esports_index = GameUI::GetEsportsTemplateIndex( (ESPORT_ID_LEVEL)pInfo->_eSportID );

	INT32 ping = -1;	// Pingǥ�ô� Room���� Ping�� ǥ���ϴ� ����� �ʿ�

	ping = BattleServerContext::i()->GetPingInfo()->GetPing( slot);
	if( ping > 4)
		ping = 4;

	const I3COLOR * pNickColor = GameUI::GetNickNameColor( BattleSlotContext::i()->getNickColorForSlot( slot));

	LuaState * L = _CallLuaFunction( "SetSlotInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);	// slot
		GameUI::SetClanMarkToLua( L, pInfo->_clanMark);
		i3Lua::PushStringUTF16To8( L, wstrClanToolTip);	// clanname tooltip
		i3Lua::PushInteger( L, rank);	// rank
		i3Lua::PushStringUTF16To8( L, szRankTooltip);	// rank tooltip
		i3Lua::PushStringUTF16To8( L, BattleSlotContext::i()->getNickForSlot( slot));	// nick
		i3Lua::PushColor( L, pNickColor);
		i3Lua::PushInteger( L, pcState);
		i3Lua::PushInteger( L, ping);
		i3Lua::PushInteger( L, state);

		bool bClosed = _IsHost()?false:true;
		if( BattleSlotContext::i()->getMainSlotIdx() == slot)
			bClosed = true;

		i3Lua::PushBoolean( L, bClosed);		// only Host. set closebutton state
		i3Lua::PushInteger( L, esports_index);		// Esports Mark
		_EndLuaFunction( L, 19);
	}

	// ���� ǥ��
	if (LocaleValue::Enable("EnableNationMark") == TRUE)
	{
		UINT8 nationCode = pInfo->_ui8NationalCode;
		UINT8 shapeIdx = LatinAmericaNationInfo::i()->FindNationShapeIdx(nationCode);

		m_UI_NationMark[slot].pNation->SetShapeIdx(shapeIdx);
}
}

void UIPhaseReadyRoom_V11::_EmptySlot( INT32 slot)
{
	LuaState * L = _CallLuaFunction( "SetSlotInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);	// slot
		GameUI::SetClanMarkToLua( L, 0xFFFFFFFF);
		i3Lua::PushA2UTF8String( L, "");	// clanname tooltip
		i3Lua::PushInteger( L, -1);	// rank
		i3Lua::PushA2UTF8String( L, "");	// rank tooltip
		i3Lua::PushA2UTF8String( L, "");	// nick
		i3Lua::PushColor( L, nullptr);
		i3Lua::PushInteger( L, -1);
		i3Lua::PushInteger( L, -1);
		i3Lua::PushInteger( L, -1);
		bool bClosed = _IsHost()?false:true;
		i3Lua::PushBoolean( L, bClosed);
		i3Lua::PushInteger( L, -1);
		_EndLuaFunction( L, 19);
	}

	if (LocaleValue::Enable("EnableNationMark") == TRUE)
	{
		if (m_UI_NationMark[slot].pNation != nullptr)
		{
			m_UI_NationMark[slot].pNation->EnableCtrl(false);
}
		if (m_UI_NationMark[slot].pEffect != nullptr)
		{
			m_UI_NationMark[slot].pEffect->EnableCtrl(false);
		}

	}

}

void UIPhaseReadyRoom_V11::_ModifyRoomOption(INT32, const i3::user_data&)
{
	/*���� ��޿ɼ��� ����Ǿ����ϴ�.*/
	GameUI::PrintSystemChat( GAME_RCSTRING("STR_TBL_GUI_READY_CHANGED_OPTION"));

	m_nOldViewSlot = -1;

	// ������ �� ���� �����츦 ����.
	UIFloatReadyRoomInfo_V11 * pRoomInfo = (UIFloatReadyRoomInfo_V11*) GetMainFrame()->GetFloating( UFW_READYROOMINFO);

	pRoomInfo->UpdateMapInfo();

	if (_IsHost())	return;		// ������ ó������ �ʴ´�

	/*���� ���������� ����Ǿ�\n�غ�Ϸ� ���¸� �����մϴ�.*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_CHANGED_WEAPON_LIMIT"));

	m_bChangeOptionNotify = true;
}

void UIPhaseReadyRoom_V11::_ChangeTeam( INT32 team)
{
	INT32 idxMySlot = BattleSlotContext::i()->getMySlotIdx();
	const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo(idxMySlot);

	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		return;

	if (pInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE,"MySlotIdx = nullptr : Index = %d",idxMySlot);
		return;
	}

	// ���� ���� ������ �̵��� �ʿ����
	if( team == idxMySlot % 2)
	{
		return;
	}

	// Ready �߿� �� ���� ����
	if( SLOT_STATE_READY == pInfo->_State)
	{
		((CGameFramework*)g_pFramework)->PutSystemChatting(GAME_STRING("STBL_IDX_EP_ROOM_NO_CHANGE_SLOT_S"));
		return;
	}

	if( SLOT_STATE_LOAD == pInfo->_State ) 
	{
		return;
	}

	m_bChagingTeam = true;
	m_nOldTeam = BattleSlotContext::i()->getMySlotIdx() % 2;

	GameEventSender::i()->SetEvent( EVENT_CHANGE_TEAM_N , &team);
}

void UIPhaseReadyRoom_V11::SetHostMark( INT32 slot, INT32 cellidx)
{
	i3UIListView_Box* t_box = nullptr;
	i3UIFrameWnd * frame = (i3UIFrameWnd *)GetScene(0)->FindChildByName("i3UIFrameWnd2");
	if(slot%2 == TEAM_RED) // Team Red
		t_box = (i3UIListView_Box*)GetScene(0)->FindChildByName("i3UIListView_RedTeam");
	else
		t_box = (i3UIListView_Box*)GetScene(0)->FindChildByName("i3UIListView_BlueTeam");
	i3UIControl * cell = t_box->getCellChild(slot/2,cellidx);
	VEC3D * cellPos = cell->getAccumulatedPos();
	VEC3D * FramePos = frame->getAccumulatedPos();

	if(m_pHostImage == nullptr)
		return;
	m_pHostImage->setPos(cellPos->x - FramePos->x, cellPos->y - FramePos->y, cellPos->z - FramePos->z);
	m_pHostImage->EnableCtrl(true);
}

void UIPhaseReadyRoom_V11::_AddAllRandomMap()
{
	ROOM_INFO_BASIC Info;
	MyRoomInfoContext::i()->getMyRoomInfo(&Info);
}

void UIPhaseReadyRoom_V11::_InitGMObserverEnable( void)
{
	LuaState* L = _CallLuaFunction("EnableGMObserver");

	if(L != nullptr)
	{	
		i3Lua::PushBoolean(L , UserInfoContext::i()->CheckAuthLevel() ); 
		i3Lua::PushInteger(L , UserInfoContext::i()->IsGM_Observer() ? 1 : 0); 
		_EndLuaFunction(L ,2);
	}
}

void UIPhaseReadyRoom_V11::_SetClanMarkFrame( void)
{
	LuaState * L = _CallLuaFunction("SetClanMarkFrame");

	if(L != nullptr)
	{
		// Ŭ����ġ = true , �׿� false
		i3Lua::PushBoolean(L, g_pFramework->GetClanContext()->InBattleTeam());
		_EndLuaFunction(L , 1);
	}
}

void UIPhaseReadyRoom_V11::_UpdateGMObserverControlState()
{
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

void UIPhaseReadyRoom_V11::_ProcessSlot( REAL32 rDeltaSeconds)
{
	INT32 i;

	// ���� ���� Ȯ��
	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(i);

		// ���Կ� �������� �ִ�.
		if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
		{	
			_CloseSlot( i, false);
			_SetSlotInfo( i);
		}
		else
		{
			// �����ִ� ���� ó��
			if (SLOT_STATE_CLOSE == pSlot->_State)
			{
				_CloseSlot(i, true);
				_EmptySlot( i);
			}
			else
			{
				_CloseSlot( i, false);
				_EmptySlot( i);
			}
		}
	}
}

void UIPhaseReadyRoom_V11::_ProcessClanBattle()
{
	//	Ŭ������ �ƴϸ� ó�����Ѵ�.
	if( !g_pFramework->GetClanContext()->InBattleTeam() &&
		!ChannelContext::i()->GetBattleChannel())
		return;


	// Ŭ����ġ����ŷ���� ������ ������ ���
	const CLAN_MATCH_BATTLE_TEAM* pRedInfo = ClanGameContext::i()->getClanBattleTeamInfo(TEAM_RED);
	const CLAN_MATCH_BATTLE_TEAM* pBlueInfo = ClanGameContext::i()->getClanBattleTeamInfo(TEAM_BLUE);
	if (pBlueInfo->getClanName() != L"" && pRedInfo->getClanName() != L"")
	{
		GameUI::SetClanTexture(m_pClanBattleMarkRed, CLAN_MARK_SMALL, pRedInfo->getClanMark());
		GameUI::SetClanTexture(m_pClanBattleMarkBlue, CLAN_MARK_SMALL, pBlueInfo->getClanMark());

		return;
	}

	// ���� Ŭ�������� ���� ������ ������ ���
	for (int i = 0; i < SLOT_MAX_COUNT; i+=2)
	{
		const SLOT_INFO* pSlot = BattleSlotContext::i()->getSlotInfo(i);
		if (pSlot != nullptr )
		{
			GameUI::SetClanTexture(m_pClanBattleMarkRed, CLAN_MARK_SMALL, pSlot->_clanMark);
			break;
		}
	}

	for (int i = 1; i < SLOT_MAX_COUNT; i+=2)
	{
		const SLOT_INFO* pSlot = BattleSlotContext::i()->getSlotInfo(i);
		if (pSlot != nullptr)
		{
			GameUI::SetClanTexture(m_pClanBattleMarkBlue, CLAN_MARK_SMALL, pSlot->_clanMark);
			break;
		}
	}
}

void UIPhaseReadyRoom_V11::_Process_Normal_BattleCountDown(INT32 arg)
{
	// ���� ���� Count
	if(arg == COUNTDOWN_COUNT)
	{
		// ������ ���ӽ��� ��ư�� �������� ī���͸� �����ϱ� ���� ����Ѵ�.
		m_bGameStart = true;
		GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(true);
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_START_COUNTDOWN"));/*{col:255,120,0,255}ī��Ʈ �ٿ��� �����մϴ�.{/col}*/

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

void UIPhaseReadyRoom_V11::_Process_ClanMatch_BattleCountDown(INT32 arg)
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

void UIPhaseReadyRoom_V11::_ProcessInviteLobbyUser( REAL32 rDeltaSeconds)
{
	if (m_pWaitInviteMB)
	{
		if (m_bInviteMessage && 1.0f <= m_rInviteLobbyUserTimer)
		{
			// �����̿� �� �ʱ�ȭ
			GameUI::CloseMsgBox( m_pWaitInviteMB);
			m_pWaitInviteMB = nullptr;

			m_rInviteLobbyUserTimer = 0.0f;
			m_bInviteMessage = false;

			// �ʴ��ϱ� ��� ó��
			if( m_bInviteResult)
			{
				GetMainFrame()->TogglePopup( UPW_INVITELOBBYUSER);
			}
			else
			{
				// ��Ȱ��ȭ Ǯ��
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_READY_NO_USER_AT_LOBBY"));/*���ǿ� ����ڰ� �����ϴ�.*/
			}
		}
		else
		{
			m_rInviteLobbyUserTimer += rDeltaSeconds;
		}
	}
}

void UIPhaseReadyRoom_V11::_CloseSlot(INT32 idxSlot, bool bClose, bool bLock)
{
	LuaState * L = _CallLuaFunction( "CloseSlot");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L,idxSlot);
		i3Lua::PushBoolean(L, bClose?true:false );
		i3Lua::PushBoolean(L, (bLock || !_IsHost())?true:false);
		_EndLuaFunction(L,3);
	}
}

void UIPhaseReadyRoom_V11::_EffectSlot(INT32 idxSlot)
{
	LuaState* L = _CallLuaFunction( "SetEffectSlot");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L, idxSlot);
		_EndLuaFunction(L,1);
	}
}

void UIPhaseReadyRoom_V11::_SetButtonState( void)
{
	if( _IsHost())
	{
		if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
			m_ButtonState[0] = RBS_V11_UNKNOWN;
		else
			m_ButtonState[0] = RBS_V11_TEAMCHANGE;

		if( m_rSearchHostDelay <= 0.f)
		{	// Activate
			m_ButtonState[1] = RBS_V11_SEARCH_HOST_ACTIVATE;
		}
		else
		{	// Deactivate
			m_ButtonState[1] = RBS_V11_SEARCH_HOST_DEACTIVATE;
		}

		if (m_bGameStart)
			m_ButtonState[3] = RBS_V11_START_CANCLE;
		else
			m_ButtonState[3] = RBS_V11_START;
	}
	else
	{
		m_ButtonState[0] = RBS_V11_UNKNOWN;
		if( m_rSearchHostDelay <= 0.f)
		{	// Activate
			m_ButtonState[1] = RBS_V11_SEARCH_HOST_ACTIVATE;
		}
		else
		{	// Deactivate
			m_ButtonState[1] = RBS_V11_SEARCH_HOST_DEACTIVATE;
		}
		m_ButtonState[3] = RBS_V11_UNKNOWN;

		for(INT32 i = 0; i < SLOT_ROOM_VIEW_COUNT; i++)
		{
			if( i < SLOT_MAX_COUNT)
			{
				const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(i);
				if ((SLOT_STATE_LOAD <= pSlot->_State) && (pSlot->_State<= SLOT_STATE_BATTLE))
				{
					m_ButtonState[3] = RBS_V11_START;
					break;
				}
				m_ButtonState[3] = RBS_V11_READY;
			}
		}
	}

	CClanContext* context = g_pFramework->GetClanContext();

	if( context && context->InBattleTeam() )
	{
		m_ButtonState[2] = RBS_V11_UNKNOWN;
	}
	else
	{
		m_ButtonState[2] = RBS_V11_INVITE;
	}


	LuaState * L = _CallLuaFunction( "SetButtonState");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, m_ButtonState[0]);
		i3Lua::PushInteger( L, m_ButtonState[1]);
		i3Lua::PushInteger( L, m_ButtonState[2]);
		i3Lua::PushInteger( L, m_ButtonState[3]);
		_EndLuaFunction( L, MAX_READYROOM_BUTTON);
	}
}

void UIPhaseReadyRoom_V11::_ReadyButtonActive( bool bActive )
{
	if( m_bReadyPushed != bActive ) return;		// �ߺ��� �̺�Ʈ ����

	LuaState*	L	= _CallLuaFunction( "ReadyActive" );

	if( L != nullptr )
	{
		i3Lua::PushBoolean( L, bActive );
		_EndLuaFunction( L, 1 );

		if( !m_bReadyPushed ) 
		{
			INT32		nSlot	= BattleSlotContext::i()->getMySlotIdx();
			const SLOT_INFO*	pInfo	= BattleSlotContext::i()->getSlotInfo( nSlot);

			m_bReadyPushed	= true;
			m_bIsReady		= ( pInfo->_State == SLOT_STATE_READY );
			m_rReadyDelay	= READY_BUTTON_DEACTIVE_TIMEOUT;	// �����ư Ÿ�Ӿƿ� �ð�
		}
		else
		{
			m_rReadyDelay	= READY_BUTTON_ACTIVE_DELAY;
		}
	}
}


void UIPhaseReadyRoom_V11::_EventProcess_BattlCountDown( INT32 arg, const i3::user_data&)
{
	if ( GetMainFrame()->GetCurrentSubPhaseType() != USP_NONE)
	{
		I3PRINTLOG(I3LOG_NOTICE, "GameSubPhase : USP_NONE.\n");
		return;
	}
	if( arg < 0)
	{
		freeReadyRoomCtrl();
		m_bGameStart = false;
		GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(false);
		BattleSlotContext::i()->setSlotState( BattleSlotContext::i()->getMainSlotIdx(), SLOT_STATE_NORMAL);

		// ���忡 ���� ���� ���� Count ���
		if(arg == -1)
		{	
			g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_NOTICE_COUNTDOWN_ABORTED_BY_ROOM_MASTER"));/*{col:255,120,0,255}������ ī��Ʈ �ٿ��� ��� �Ͽ����ϴ�.{/col}*/
		}
		// ����� ���� Ǯ�� ���� ���� Count ���
		else
		{
			g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_NOTICE_COUNTDOWN_ABORTED"));/*{col:255,120,0,255}�غ���°� ��� �����Ǿ� ī��Ʈ �ٿ��� ��ҵǾ����ϴ�.{/col}*/
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

void UIPhaseReadyRoom_V11::_ProcessCommon( REAL32 rDeltaSeconds)
{
	if( m_bChagingTeam)
	{
		if(BattleSlotContext::i()->getMySlotIdx() % 2 == m_nOldTeam)
		{
			// �� ���� ���̸�, �� ������ �Ϸ�ɶ����� ���� ������Ʈ�� ���� �ʴ´�.
			return;
		}
		else
		{
			m_nOldViewSlot = -1;
			m_bChagingTeam = false;
		}
	}

	// ���� ����
	m_SlotUpdateTime += rDeltaSeconds;

	if( m_SlotUpdateTime > SLOT_UPDATETIME)
	{
		_ProcessSlot( rDeltaSeconds);
		m_SlotUpdateTime = 0.f;

		if( m_bReadyPushed )	// �����ư �������� �������� ���� �޾Ƽ� ���°��� �ٲ�� �������� ��Ȱ��ȭ ��Ű�� ����
		{
			INT32		nSlot		= BattleSlotContext::i()->getMySlotIdx();
			const SLOT_INFO*	pInfo		= BattleSlotContext::i()->getSlotInfo( nSlot);
			bool		bIsReady	= ( pInfo->_State == SLOT_STATE_READY );

			if( m_bIsReady != bIsReady || m_rReadyDelay <= READY_BUTTON_ACTIVE_DELAY )
			{
				_ReadyButtonActive( true );
			}
		}
	}
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//

void UIPhaseReadyRoom_V11::ProcessGameEvent(GAME_EVENT event,INT32 arg, const i3::user_data& UserData)
{
	i3::rc_wstring wstr_message;

	UIPhaseBase::ProcessGameEvent( event, arg, UserData);

	// �̺�Ʈ ó�� 1.5��Ÿ�Ϸ� ��ȯ
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( event);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}

	// �� ���� ������ ���Ͽ� �� ���� â�� Event�� ��.
	UIFloatReadyRoomInfo_V11 * pRoomInfo = (UIFloatReadyRoomInfo_V11*) GetMainFrame()->GetFloating(UFW_READYROOMINFO);
	if(pRoomInfo != nullptr)
	{
		pRoomInfo->ProcessGameEvent(event, arg);
	}

	// MiniProfile�� Ŭ�� ������ Update�Ѵ�.
	UIFloatMiniProfile * pFrame = (UIFloatMiniProfile*)GetMainFrame()->GetFloating(UFW_MINIPROFILE);
	if(pFrame != nullptr)
	{
		pFrame->ProcessGameEvent(event, arg, UserData);
	}

	//I3PRINTLOG(I3LOG_NOTICE,"UIPhaseReadyRoom_V11::ProcessGameEvent() Leave < event : %d, arg : %d >\n", event, arg);
}

void UIPhaseReadyRoom_V11::Set_bGameStart(bool bFlag)
{ 
	m_bGameStart = bFlag;
}

void UIPhaseReadyRoom_V11::StartCount(INT32 iCount)
{ 
	m_rRemainTime = 1.f;
	m_iRemainCount = iCount;
}

void UIPhaseReadyRoom_V11::SetRemainCount(INT32 iCount)
{ 
	m_iRemainCount = iCount; 
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_BATTLE_COUNTDOWN, m_iRemainCount);
}

//------------------------------------------------------------------------------//
//							PhaseReadyRoom OnUpdate								//
//------------------------------------------------------------------------------//

void UIPhaseReadyRoom_V11::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate( rDeltaSeconds);

	if(m_iRemainCount > 0)
	{
		m_rRemainTime -= rDeltaSeconds;
		if(m_rRemainTime <= 0)
		{
			m_rRemainTime = 1.f;
			DecreaseRemainCount();
		}
	}

	if( m_bEnableBlockEscape) m_rBlockEscapeTime += rDeltaSeconds;

	// ���뷱�� �޼��� ���
	if (MyRoomInfoContext::i()->IsTeamBalanceMessageFlag())
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_NOTICE_AUTO_BALANCED"));/*{col:255,120,0,255}�� �뷱�� �ɼǿ� ���� �� �� �ο����� ���߱� ���� �ڵ����� �����Ǿ����ϴ�.{/col}*/
		MyRoomInfoContext::i()->SetTeamBalanceMessageFlag(false);
		//g_pGameContext->RemoveGameFlag( GAMEFLAG_TEAMBALANCEMSG);
	}

	m_rSearchHostDelay = MAX(m_rSearchHostDelay - rDeltaSeconds, 0.f);
	m_rTeamChangeDelay = MAX(m_rTeamChangeDelay - rDeltaSeconds, 0.0f);
	if( m_bReadyPushed )
	{
		m_rReadyDelay	-= rDeltaSeconds;
		if( m_rReadyDelay <= 0.0f )
		{
			m_bReadyPushed	= false;
		}
	}

	_SetButtonState();

	_ProcessCommon( rDeltaSeconds);

	// �ʴ��ϱ� �˸��˾� �����ð� ����
	_ProcessInviteLobbyUser( rDeltaSeconds);

	// ��� ������ Ready�ߴ����� ���� ������ �Ǵ�.
	//__ProcessKickRoomMaster();

	_UpdateGMObserverControlState();

	if (LocaleValue::Enable("EnableNationMark") == TRUE)
	{
		GameUI::Update_RankAndNationMark_Swap(rDeltaSeconds, m_UserUI_NationMarkData);
		_Update_RankAndNaionMarkAlpha();
}

	//Ŭ��ä���� ����뿡 ������ ��������� ���
	if (ChannelContext::i()->GetBattleChannel() &&
		UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN &&
		!UserInfoContext::i()->CheckAuthLevel())								//MOD,GM�� Ŭ�� ������ ������� �������� �ʴ´�)
	{
		if (GetMainFrame()->getDeprotationClanChannel() == false &&
			ClanGameContext::i()->getMyMercenaryState() == false)
		{
			OnGotoBackPhase();
		}

	}
}


/*virtual*/ void UIPhaseReadyRoom_V11::OnCreate( i3GameNode * pParent)
{
	INT32 i;

	UIPhaseBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_ReadyRoom_TeamSelect.i3UIs",  PhaseReadyRoom_V11_Glue, true, false);

	//Room Master GameStart Timer After All User Ready
	__CreateRoomMasterKick();

	const char* soundFileName[COUNTDOWN_COUNT] =
	{
		"UI_Count_A.wav",
		"UI_Count_B.wav",
		"UI_Count_C.wav",
		"UI_Count_D.wav",
		"UI_Count_E.wav"
	};

	for( i = 0; i < COUNTDOWN_COUNT; i++)
	{
		m_pCountdownSound[i] = i3SoundPlayInfo::new_object();

		i3GameResSound* pResSound = (i3GameResSound*)g_pFramework->QuaryResource(soundFileName[i]);
		I3ASSERT(pResSound != nullptr);

		i3SoundPlayInfo* pSoundInfo = pResSound->getSound();
		pSoundInfo->CopyTo(m_pCountdownSound[i], I3_COPY_INSTANCE);
	}
}

void UIPhaseReadyRoom_V11::OnLoadAllScenes()
{
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::R_TO_L);

	if (LocaleValue::Enable("EnableNationMark") == FALSE)
	{
		for (int i = 0; i < SLOT_MAX_COUNT; ++i)
		{
			if (m_UI_NationMark[i].pNation != nullptr) m_UI_NationMark[i].pNation->EnableCtrl(false);
			if (m_UI_NationMark[i].pEffect != nullptr) m_UI_NationMark[i].pEffect->EnableCtrl(false);
}
	}
}


/*virtual*/ bool UIPhaseReadyRoom_V11::OnKey_Escape( void)
{
	//	if( GetMainFrame()->IsOpenedPopup( UPW_RANK_UP))
	//	{
	//		GetMainFrame()->ClosePopup( UPW_RANK_UP);
	//		return true;
	//	}
	//  �� �˾����� �˾Ƽ� Escapeó�� �ǹǷ� �˾��ݱ�� ���⼭ ����..(11.10.22.����)

	//SetEventOnce ȣ�� �� �� 3�� ���� OnKey_Escape�� ó�� ���� �ʽ��ϴ�.
	if( m_bEnableBlockEscape && m_rBlockEscapeTime < 3.f) return false;

	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) &&
		ChannelContext::i()->GetBattleChannel() && m_bGiveupBattle)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_LEAVE_NOTICE_POPUP"), nullptr, MAKE_CALLBACK(UIPhaseReadyRoom_V11::cbQueryLeaveClanBattle), this);

		return true;
	}


	if( GameEventSender::i()->SetEventOnce( EVENT_ENTER_LOBBY))
	{
		m_bEnableBlockEscape = true;
		m_rBlockEscapeTime = 0.f;
	}
	m_bMyPhase = false;

	return true;
}

/*virtual*/ void UIPhaseReadyRoom_V11::OnGotoBackPhase( void)
{
	//	i3UI::setDisableUIInput( true);		
	// GameUI::ProcessInputDisableChangePhase ȣ���� ���� DisableInputȣ��ǹǷ� ���⼭ ����..
	// (11.10.22.����)

	//@eunil. Ŭ�� ��ġ����ŷ ��������
	//Ŭ���� �ߵ� ��Ż���� ��� �г�Ƽ �ȳ� �˾��� ���. 
	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) &&
		ChannelContext::i()->GetBattleChannel() && m_bGiveupBattle)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_LEAVE_NOTICE_POPUP"), nullptr, MAKE_CALLBACK(UIPhaseReadyRoom_V11::cbQueryLeaveClanBattle), this);

		return;
	}

	// �κ�� ������.
	GameEventSender::i()->SetEventOnce( EVENT_ENTER_LOBBY );
	m_bMyPhase = false;
}

/*virtual*/ bool UIPhaseReadyRoom_V11::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPhaseBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	// ����� ���� �� �� ���� ����
	if ( GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE )
		CCharaInfoContext::i()->SetTeamSlotBySlotIdx( BattleSlotContext::i()->GetMySlotIdxEx() );

	//freeReadyRoomCtrl();
	GetMainFrame()->GetRoomMasterAwayCheckerManager()->AddAwayChekcer();

	// �� ����� �ڽ��� �������� Ȯ���Ѵ�
	m_bIsHost = _IsHost()?true:false;
	m_OldHostIndex = BattleSlotContext::i()->getMainSlotIdx();

	// Button�� Ű�� ����
	_SetButtonState();

	// �ӽ� �̰��� ���׸� ���� ���� �մϴ�.
	// ������ ��� ���� ���¿��� �����ϸ� ������ �ʱ�ȭ���� �ʴ� ������ �־��.. ��..?�̷���..??
	_EmptySlot( 0);
	_EmptySlot( 1);

	InitHostIcon();

	_SetClanMarkFrame();

	_ProcessSlot( 0.f);

	//	Ŭ���� ������Ʈ -> ����
	_ProcessClanBattle();

	_InitGMObserverEnable();

	g_pFramework->getChatBox()->setWhisperEnable( true);

	m_bEnableBlockEscape = false;
	m_rBlockEscapeTime = 0.f;

	if (GetMainFrame()->IsPopupFocused() == false &&
		g_pFramework->GetMessageBoxManager()->m_is_open_message_box == false)
	{
		g_pFramework->SetFocus(nullptr);
		GetMainFrame()->InputDisableMainUI(false);
	}
	return true;
}

void UIPhaseReadyRoom_V11::OnEntranceEnd()
{
	UIPhaseBase::OnEntranceEnd();

	_ReadyButtonActive(true);

	// ������ üũ
	if( 0 < UserInfoContext::i()->GetRankUpRank())
	{
		GetMainFrame()->TogglePopup( UPW_RANK_UP);
		UserInfoContext::i()->ResetRankUp();
	}

	// ����, �н����� �ʱ� ǥ��
	//_UpdateTitlePassword(true);

	// ������ �ʱ�ȭ
	m_nOldViewSlot = -1;

	m_bChangeOptionNotify = false;

	// ������ ���� �ý����� ������� ������ ���� ������ ����� ����.
	if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		//#if defined (USE_RANDOMMAP_MODE)
		if (!_IsHost() && MyRoomInfoContext::i()->IsRandomMap())
		{
			if (RandomMapContext::i()->GetStageListSize() < 1)
			{
				_AddAllRandomMap();
			}
		}
		//#endif

		/*m_pOption->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		#if defined (USE_RANDOMMAP_MODE)
		m_pRandomMap->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		if(g_pGameContext->IsRandomMap() && g_pGameContext->GetRandomMapCount()<1)
		OnRandomMap();
		//������ READYROOM���� ���°��� �ƴ϶��(Lobby���� ���°��̶��)
		if(g_pFramework->GetState() != 7)//BattlePhase���� Readyroom���� �ö� STATE_FADEIN�� �̿�
		{
		//Lobby���� �������� üũ�Ѱ�� �ʱ�ȭ ���� �ʽ��ϴ�
		if(!g_pGameContext->IsRandomMap())
		InitRandomCheck();
		}

		#endif

		m_pAdvencedWin->SetEnable(FALSE);

		m_pRoomWin->setInputDisable(FALSE);*/
	}

	// �ӹ� �Ϸ� Ȯ��
	CheckBattleChallengeComplate();

	//���� ���� ä�� �޼����� ����.
	_ApplyMsgReadyBox();

	//�ð� ���� â�� ����.
	CheckTSReward();

	// ������ üũ
#if defined ( USE_REPAIR_WEAPON )
	if( UserInfoContext::i()->IsNewRepair() )
		_CheckEndurance();
#endif

	// Battle���� �����˾� �� ���� �� ó��
	if( g_pFramework->IsMessageBoxEnabled() )
	{
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
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_READY_NOTICE_LEAVE_SERVER_BY_KILL_RATE"), nullptr, MAKE_CALLBACK(CbMoveChannel), this);
			break;
		case CHANNEL_TYPE_BEGIN2:
			/*�ʺ� ������ ���� ������ ��ް� ���� �ʾ� ���� ���� ȭ������ �̵��մϴ�. (�� ��ޱ����� ���� ����)*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_READY_NOTICE_WRONG_GRADE_FOR_BEGINNER"), nullptr, MAKE_CALLBACK(CbMoveChannel), this);
			break;
		case CHANNEL_TYPE_EXPERT:
			/*��� ���� ���� ������ ������Ű�� ���Ͽ� �������� ȭ������ �̵��մϴ�.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_READY_NOTICE_WRONG_GRADE_FOR_VETERAN"), nullptr, MAKE_CALLBACK(CbMoveChannel), this);
			break;
		default:
			break;
		}
	}

	//if( g_pGameContext->IsMadnessRoom() && _IsHost() )
	//{
	//	for( int i = 0 ; i < SLOT_MAX_COUNT/2 ; i++)
	//	{
	//		if( i % 2 != 0 )
	//		{
	//			INT32 slotFlag = 0;
	//			const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo( i);
	//			slotFlag |= i;
	//			GameEventSender::i()->SetEvent(EVENT_CHANGE_SLOT_N, &slotFlag);
	//		}			
	//	}
	//}

	//UIFloatChat* float_chat = static_cast<UIFloatChat*>(GetMainFrame()->GetFloating(UFW_CHAT));
	//if (float_chat)
	//{
	//	float_chat->SetFocus_ChatEditBox();		// ��������� ��ȯ�Ǹ� ä����Ŀ���� �մϴ�..
	//}

	// keyword : Star Player
	if( !MyRoomInfoContext::i()->getInReadyRoom() )
	{
		INT32 i32MySlotIdx = BattleSlotContext::i()->getMySlotIdx();
		MyRoomInfoContext::i()->PrintStarPlayerEntranceMsg(i32MySlotIdx);
		MyRoomInfoContext::i()->setInReadyRoom(true);
	}		
	
	if( MyRoomInfoContext::i()->GetCountDown() > 0)
	{
		m_bGameStart = true;
		GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(true);
		StartCount( MyRoomInfoContext::i()->GetCountDown());
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_BATTLE_COUNTDOWN, MyRoomInfoContext::i()->GetCountDown() );
		MyRoomInfoContext::i()->SetCountDown( 0);
	}

	//@eunil. PBBUG-17128 ����� ���� �� �������� �ʱ�ȭ.
	ROOM_INFO_ALL info;
	MyRoomInfoContext::i()->getMyRoomInfo(&info._Basic);
	STAGE_MODE type = StageID::GetStageMode(info._Basic._StageID);
	RandomMapContext::i()->SetPrevStageType(type);

	UITopMenu_V11 * pTop = GetMainFrame()->GetTopMenu_V11();
	if (pTop != nullptr)
		pTop->cb_tmenu_gauge(true);
}

bool UIPhaseReadyRoom_V11::OnExitStart( void)
{
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

	//Room Master GameStart Timer After All User Ready		
	GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(false);
	__StopRoomMasterKick();		
	m_bGameStart = false;

	return true;
}

/*virtual*/ void UIPhaseReadyRoom_V11::OnExitEnd( void)
{
	UIPhaseBase::OnExitEnd();

	//g_pGameContext->RemoveGameFlag( GAMEFLAG_KICKBYHOST);

	//g_pFramework->getChatBox()->setWhisperEnable( false);

	InviteContext::i()->ClearInviteInfo();

	if(m_pHostImage != nullptr)
		m_pHostImage->EnableCtrl(false);
}

/*virtual*/ bool UIPhaseReadyRoom_V11::OnEnterMouse( i3UIControl * pControl)
{
	if( UIPhaseBase::OnEnterMouse( pControl) == false)
		return false;

	if( i3::contain_string( pControl->GetName(), "btnStart") == 0)
		PlaySound( UISND_ENTER_MOUSE1);

	return true;
}

/*virtual*/ bool UIPhaseReadyRoom_V11::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	switch(event)
	{
	case ACE_AWAY:			
		if(m_bGameStart) 
		{
			__StopRoomMasterKick();
			return true;
		}
		GetMainFrame()->setProcessLeaveRoom(true);			
		GetMainFrame()->CloseAwayMessageBox();
		return true;
	case ACE_CAUTION:
		{
#ifdef USE_ROOM_MASTER_KICK
			if(m_bRoomMsterAwayCountDown)
			{
				GetMainFrame()->setKickBtNoStartGame(true);
				/*���ѽð����� ������ �������� �ʾ� 5���� ���� ����˴ϴ�.*/
				GetMainFrame()->OpenAwayMessageBox(GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_ROOM_MASTER_GET_AWAY"));
				return true;
			}
			else
			{
				/*��ð� �������� �����Ƿ� 5�� �� �κ�� �̵��մϴ�.*/
				GetMainFrame()->OpenAwayMessageBox(GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_GET_AWAY"));
				return true;

			}
#else
			GetMainFrame()->OpenAwayMessageBox(GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_GET_AWAY"));/*��ð� �������� �����Ƿ� 5�� �� �κ�� �̵��մϴ�.*/
			return true;	
#endif
		}
		break;

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
							_DoReadyGame();
						}
						else
							GameEventSender::i()->SetEventOnce(EVENT_GET_ROOM_SCORE); //�� ���ھ� ������ ��û�Ѵ�.
					}
				}
			}
			
		}
		break;
	}

	return UIPhaseBase::OnEvent(event, pObj, param2, code);
}

void UIPhaseReadyRoom_V11::ClickSlot( INT32 idx)
{

	if( idx < 0 || idx >= SLOT_MAX_COUNT)
		return;

	const	SLOT_INFO* pSlotInfo = BattleSlotContext::i()->getSlotInfo( idx);

	if( SLOT_STATE_EMPTY == pSlotInfo->_State)
	{
		//	Ŭ������ �ƴ� ��츸 �����ϵ���
		if( !g_pFramework->GetClanContext()->InBattleTeam())
		{
			INT32 nTeam;

			if (0 == idx % 2)
				nTeam = TEAM_RED;
			else
				nTeam = TEAM_BLUE;

			_ChangeTeam(nTeam);
		}

	}
	else if( SLOT_STATE_CLOSE != pSlotInfo->_State)
	{
		// �� UI�� �ٷ� ĳ���� ������ ��û������..
		// Context �޴��� ����Ǿ ���� ��û�ϴ� ������� ����
		// Index�� ��������!!
		// �ϴ� �ӽ� �ּ�
		//g_pGameContext->SetCurrentInfoSlot( idx); 
	}

	if (SLOT_STATE_EMPTY != pSlotInfo->_State &&
		SLOT_STATE_CLOSE != pSlotInfo->_State)
	{
		//PlaySound( UISND_ITEM_SELECTING);
	}
}

void UIPhaseReadyRoom_V11::_GetUserInfo(INT32 idxGameSlot)
{
	if( BattleSlotContext::i()->getMySlotIdx() != idxGameSlot)
	{
		GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_N, &idxGameSlot);
	}
}

void UIPhaseReadyRoom_V11::_NationInitAtLoad(i3UIScene * pScene)
{
	char Nation[MAX_PATH];
	char Effect[MAX_PATH];
	for (int i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		int team = i % 2;
		int idx = i / 2;
		if (team == 0)		// ������
		{
			sprintf_s(Nation, "RED_NATION_%02d", idx + 1); // RED_NATION_01 ~ 08
			m_UI_NationMark[i].pNation = (i3UIButtonImageSet*)pScene->FindChildByName(Nation);
			I3ASSERT(m_UI_NationMark[i].pNation);

			sprintf_s(Effect, "RED_EFFECT_%02d", idx + 1); // RED_EFFECT_01 ~ 08
			m_UI_NationMark[i].pEffect = (i3UIButtonImageSet*)pScene->FindChildByName(Effect);
			I3ASSERT(m_UI_NationMark[i].pEffect);
		}
		else				// �����
		{
			sprintf_s(Nation, "BLUE_NATION_%02d", idx + 1); // BLUE_NATION_01 ~ 08
			m_UI_NationMark[i].pNation = (i3UIButtonImageSet*)pScene->FindChildByName(Nation);
			I3ASSERT(m_UI_NationMark[i].pNation);

			sprintf_s(Effect, "BLUE_EFFECT_%02d", idx + 1); // BLUE_EFFECT_01 ~ 08
			m_UI_NationMark[i].pEffect = (i3UIButtonImageSet*)pScene->FindChildByName(Effect);
			I3ASSERT(m_UI_NationMark[i].pEffect);
		}
		
		m_UI_NationMark[i].pNation->EnableCtrl(false);
		m_UI_NationMark[i].pEffect->EnableCtrl(false);
	}
}

void UIPhaseReadyRoom_V11::_Update_RankAndNaionMarkAlpha(void)
{
	UINT8 _NationMarkEffect_Idx			= m_UserUI_NationMarkData.ui8EffectIdx;
	REAL32 _NationMarkAlpha_AlphaValue	= m_UserUI_NationMarkData.rAlphaValue;

	for (int i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		const SLOT_INFO* pSlotInfo = BattleSlotContext::i()->getSlotInfo(i);
		if (pSlotInfo != nullptr)
		{
			if (m_UI_NationMark[i].pNation != nullptr)
				m_UI_NationMark[i].pNation->EnableCtrl(false);
			if (m_UI_NationMark[i].pEffect != nullptr)
				m_UI_NationMark[i].pEffect->EnableCtrl(false);

			if (pSlotInfo->_State == SLOT_STATE_CLOSE || pSlotInfo->_State == SLOT_STATE_EMPTY) continue;

			int team = i % 2;
			//int idx = i / 2;

			if (team == 0)	// ������
			{
				if (m_ListViewBox_RedTeam != nullptr)
				{
					GameUI::Update_RankMark_AlphaValue(m_ListViewBox_RedTeam, 1, 1 - _NationMarkAlpha_AlphaValue);
				}

				 // NationMark
				if (m_UI_NationMark[i].pNation != nullptr)
				{
					COLOR color;
					i3Color::Set(&color, (REAL32)1, (REAL32)1, (REAL32)1, _NationMarkAlpha_AlphaValue);
					m_UI_NationMark[i].pNation->EnableCtrl(true);
					m_UI_NationMark[i].pNation->setColor(&color);					
				}

				// NationMarkEffect
				if (m_UI_NationMark[i].pEffect != nullptr)
				{
					m_UI_NationMark[i].pEffect->EnableCtrl(true);
					m_UI_NationMark[i].pEffect->SetShapeIdx(_NationMarkEffect_Idx);
				}
			}
			else
			{
				if (m_ListViewBox_BlueTeam != nullptr)
				{
					GameUI::Update_RankMark_AlphaValue(m_ListViewBox_BlueTeam, 1, 1 - _NationMarkAlpha_AlphaValue);
				}
								
				// NationMark
				if (m_UI_NationMark[i].pNation != nullptr)
				{
					COLOR color;
					i3Color::Set(&color, (REAL32)1, (REAL32)1, (REAL32)1, _NationMarkAlpha_AlphaValue);
					m_UI_NationMark[i].pNation->EnableCtrl(true);
					m_UI_NationMark[i].pNation->setColor(&color);
				}

				// NationMarkEffect
				if (m_UI_NationMark[i].pEffect != nullptr)
				{
					m_UI_NationMark[i].pEffect->EnableCtrl(true);
					m_UI_NationMark[i].pEffect->SetShapeIdx(_NationMarkEffect_Idx);
				}
			}
		}// if (pSlotInfo != NULL) end
	}
}

void UIPhaseReadyRoom_V11::ClickRSlot( INT32 idx)
{
	UINT32 i;
	I3_BOUNDCHK( idx, SLOT_MAX_COUNT);

	m_nSelectedSlotIdx = idx;

	const SLOT_INFO* pSlotInfo = BattleSlotContext::i()->getSlotInfo( idx);
	if(pSlotInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE,"pSlotInfo Value is nullptr");
		return;
	}

	UIPopupUserProfile_V11 *pUserProfile = static_cast<UIPopupUserProfile_V11*>(GetMainFrame()->GetPopup(UPW_USERPROFILE));
	pUserProfile->SetReadyroomIdx(idx);

	if (SLOT_STATE_EMPTY != pSlotInfo->_State &&
		SLOT_STATE_CLOSE != pSlotInfo->_State)
	{
		ClickSlot( idx);

		GetMainFrame()->ResetUserActionPopup();

		// �ڽ��� ��� ����� ���ؽ�Ʈ�޴��� ����.
		if( BattleSlotContext::i()->getMySlotIdx() == idx)
		{
			GetMainFrame()->AddUserActionCommand( UCM_PROFILE);
		}
		else
		{
			bool bFriend = false;
			FRIEND_STATE friendState = FRIEND_STATE_OFFLINE;
			for( i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
			{
				const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( i);
				if(nullptr == node)	continue;
				if( i3::generic_is_equal( node->_wstr_nick, BattleSlotContext::i()->getNickForSlot( idx)))
				{
					// �̹� ģ���̴�.
					bFriend = true;
					friendState = (FRIEND_STATE) node->_state;
				}
			}

			GetMainFrame()->AddUserActionCommand( UCM_PROFILE);

			if( _IsHost())
			{
				GetMainFrame()->AddUserActionCommand( UCM_TRANSFER_HOST);
				GetMainFrame()->AddUserActionCommand( UCM_KICK);
			}

			if(!bFriend)
			{
				GetMainFrame()->AddUserActionCommand( UCM_ADD_FRIEND);
			}


			GetMainFrame()->AddUserActionCommand( UCM_WHISPER);

			// ���� ����뿡���� ģ���߰� ��� �����ϴ�.
			// �߰��� ���ؼ��� m_RequestInfo._tUserInfo._Session �� ���� �־�� �ϴµ�
			// �κ������� ���� �ְ� ���𿡼� ������ �ִ� �������� �����ϴ�.
			//if( bFriend == false)
			//	GetMainFrame()->AddUserActionCommand( UCM_ADD_FRIEND);		//	ģ���߰�

			if( CanClanInvite() )
			{
				//	�ڱⰡ Ŭ�� �ʴ� ����� ���� ��츸 ������
				if( BattleSlotContext::i()->getSlotInfo( idx )->_clanstate == CLAN_MEMBER_LEVEL_UNKNOWN &&
					g_pFramework->GetClanContext()->IsPossibleClanInvite() )
				{
					GetMainFrame()->AddUserActionCommand( UCM_CLAN_INVITE);
				}
			}

			if( bFriend && friendState > FRIEND_STATE_OFFLINE)
				GetMainFrame()->AddUserActionCommand( UCM_SEND_MAIL);			// ���� ������

			if( LocaleValue::Enable("EnableBlock") ) // �����ϱ� ��ư Ȱ��ȭ ����
			{
				//if( m_CurrentUserTab != USERLIST_TAB_BLOCK)
				GetMainFrame()->AddUserActionCommand( UCM_ADD_BLOCK);		//	�����ϱ�
				//else
				//	GetMainFrame()->AddUserActionCommand( UCM_REMOVE_BLOCK);
			}

			if( CanUse_ViewOtherUserItemInfo() )
				GetMainFrame()->AddUserActionCommand( UCM_VIEW_OTHER_USER_ITEM_INFO);
			// �Ű��ϱ�
			//GetMainFrame()->AddUserActionCommand( UCM_USER_REPORT);

			// GM �ɼ�â
			if ( UserInfoContext::i()->GetMyRank() == RANK_GAMEMASTER)
			{
				GetMainFrame()->AddUserActionCommand( UCM_GMOPTION);		// Ŭ���ʴ�(CurrentTab)
			}
		}

		GetMainFrame()->OpenUserActionPopup( this, idx);
		PlaySound( UISND_ITEM_SELECTING);
	}
}

void UIPhaseReadyRoom_V11::ClickCloseButton( INT32 idx)
{
	if( !_IsHost())
		return;

	// �ڱ� �ڽ��� �������� �ʵ��� �Ѵ�.
	if( BattleSlotContext::i()->getMainSlotIdx() == idx)
		return;

	// �Ʒ��� �� Ư�� �����̻��� ���� ���� �ʵ��� �մϴ�.
	if ((MyRoomInfoContext::i()->getStage()->IsAiMode()) )// || g_pGameContext->IsDiehardRoom())
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
	UINT32 StageID = g_pGameContext->GetMyRoomStageID();

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

void UIPhaseReadyRoom_V11::ClickButton( INT32 btn)
{
	I3_BOUNDCHK( btn, MAX_READYROOM_BUTTON);

	switch( m_ButtonState[ btn])
	{
	case RBS_V11_TEAMCHANGE :			_DoSwapTeam();			break;
	case RBS_V11_SEARCH_HOST_ACTIVATE :
	case RBS_V11_SEARCH_HOST_DEACTIVATE :
		_DoSearchHost();
		break;
	case RBS_V11_INVITE :				_DoInviteFromLobby();	break;
		//case RBS_ROOMSET :				_DoConfigRoom();		break;
	case RBS_V11_READY :				
	case RBS_V11_START :
	case RBS_V11_START_CANCLE:
		{	
			if( bing::is_break_into_game() == false)
			{	
				_DoReadyGame();
			}
			else
			{
				GameEventSender::i()->SetEventOnce( EVENT_GET_ROOM_SCORE ); //�� ���ھ� ������ ��û�Ѵ�.
			}
			break;
			
		}
	default :
		I3PRINTLOG(I3LOG_FATAL,"not yet setting this function!!!");
		break;
	}
}

void UIPhaseReadyRoom_V11::OpenPopupUsingCashItem(void)
{
	if( ! CInvenList::i()->IsUseBattleGiveupCashItem())
		return;

	i3::rc_wstring wstrRemainTeam;
	UINT32 itemTerm = 0;
	UINT8 itemAttr = 0;
	UINT32 itemArg = 0;


	//	���� '���� �̵�' �������� ����ϰ� ������ ������ ��� �˸� �˾�â�� ����.
	INT32 itemFreeMove = CASHITEM_ITEMID_FREE_MOVE;
	INT32 itemFreeMoveEV = CASHITEM_ITEMID_FREE_MOVE_EV;
	INT32 itemBattleMove = CASHITEM_ITEMID_BATTLEFIELD_MOVE;

	//	'���� �̵�' ������ �켱
	if( CInvenList::i()->IsUsingCashItem(itemFreeMove))
	{
		T_ItemID ItemID = GetCashItemByGroup(itemFreeMove);


		CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, ItemID, &itemAttr, &itemArg);

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

		if (1 < itemTerm)
		{
			//	���� �Ⱓ : %d��
			i3::sprintf(wstrRemainTeam, GAME_STRING("STR_TBL_GUI_READY_REMAINING_DAYS1"),
				itemTerm, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));/*���� �Ⱓ : {col:255,255,255,255}%d%s{/col}*/
		}
		else if( itemTerm == 0)
		{
			//	���� �Ⱓ : %d��
			i3::sprintf(wstrRemainTeam,GAME_STRING("STR_TBL_GUI_READY_REMAINING_DAYS2"),
				itemTerm, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));/*���� �Ⱓ : {col:255,255,255,255}%d%s{/col}*/
		}
		else
		{
			//	���� �Ⱓ : 1�� �̸�
			i3::sprintf(wstrRemainTeam, GAME_STRING("STR_TBL_GUI_READY_REMAINING_DAYS2"),
				GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"), GAME_STRING("STBL_IDX_ITEM_LESS_THEN"));/*���� �Ⱓ : {col:210,0,0,255}1%s %s{/col}*/
		}


		i3::rc_wstring wstrName; 
		g_pShopItemInfoDataBase->GetItemName(ItemID, wstrName);

		/*{col:255,153,51,255}%s{/col} �������� ����Ǿ����ϴ�.\n \n {col: 51,204,255,255}����ġ �� ����Ʈ�� ȹ���߽��ϴ�.\n ��Ż �г�Ƽ�� ������� �ʽ��ϴ�.\n ���� �� ų/������ ��ϵ��� �ʽ��ϴ�.{/col}\n \n %s*/
		i3::rc_wstring wstrMsg;
		i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_GUI_READY_ITEM_APPLYED_LEAVE_GAME"), wstrName, wstrRemainTeam);

		GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg);

		CInvenList::i()->SetUseBattleGiveupCashItem(false);
	}
	else if(CInvenList::i()->IsUsingCashItem(itemFreeMoveEV))
	{
		T_ItemID ItemID = GetCashItemByGroup(itemFreeMoveEV);


		CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, ItemID, &itemAttr, &itemArg);

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

		i3::rc_wstring wstrMsg;
		/*{col:255,153,51,255}%s{/col} �������� ����Ǿ����ϴ�.\n \n {col: 51,204,255,255}����ġ �� ����Ʈ�� ȹ���߽��ϴ�.\n ��Ż �г�Ƽ�� ������� �ʽ��ϴ�.\n ���� �� ų/������ ��ϵ��� �ʽ��ϴ�.{/col}\n \n %s*/
		i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_GUI_READY_ITEM_APPLYED_LEAVE_GAME"), wstrName, wstrRemainTeam);

		GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg);

		CInvenList::i()->SetUseBattleGiveupCashItem(false);
	}
	//	'���� �̵�' �������� ��� ����� �������
	//	�ΰ��ӿ��� '���� �̵�' �������� ����Ͽ� ������ ��� ����뿡 ������ ��� ����� �����κ��� �ް� �˸�â�� ����. komet
	else if( CInvenList::i()->GetLatestUsedCashItemFlag() == itemBattleMove)
	{
		T_ItemID ItemID = GetCashItemByGroup(itemBattleMove);
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

void UIPhaseReadyRoom_V11::CheckTSReward()
{
	//���� ���� �ڿ��.

	if( UserInfoContext::i()->IsTSRewardFlag())
	{
		GetMainFrame()->OpenPopup( UPW_TS_REWARD );
		UserInfoContext::i()->SetTSRewardFlag(false);
		return;
	}

	if (UserInfoContext::i()->IsDailyGiftGauge() == false || UserInfoContext::i()->IsUserInfoDaily() == false)
	{
		return;
	}

	if (UserInfoContext::i()->GetUserInfoDaily()->_ui8GetItem == DAILY_GIFT_TYPE_MAX)
	{
		return;
	}

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



void UIPhaseReadyRoom_V11::_CheckEndurance(void)
{
	if( UserInfoContext::i()->IsNewRepair() )
		GameUI::CheckWeaponEndurance();
}


void UIPhaseReadyRoom_V11::InitHostIcon()
{
	i3UIFrameWnd * frame = (i3UIFrameWnd *)GetScene(0)->FindChildByName("i3UIFrameWnd2");
	VEC3D tPos;
	VEC2D markSize;

	markSize.x = 20;
	markSize.y = 20;

	if(m_pHostImage != nullptr) // �̹� �����ϸ� �������� �ʽ��ϴ�.
		return;

	i3UITemplate * uitemplate = i3UI::getUILibrary()->FindByName("Ico_Host");
	if(uitemplate != nullptr)
		m_pHostImage = (i3UIImageBox*) i3UI::CreateControl( uitemplate, &tPos, &markSize, frame);
	if( m_pHostImage != nullptr)
	{
		m_pHostImage->EnableCtrl(false);
	}

}

void UIPhaseReadyRoom_V11::CheckBattleChallengeComplate(void)
{
	
	const MCardSet* pCardSet = MCardUtil::GetSelectedMCardSet();

	if (pCardSet == nullptr) // ���õ� �ӹ�ī����� ������ �׳� ��..
		return;

	#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		//�ӹ�ī�� ���� ���� �̸�����
		if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ) &&
			g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT) )
		{
			const MCard* pCard = pCardSet->GetSelectedMCard();

			if (pCard)
			{
				const MCardInfo* pInfo_Test = pCard->GetMCardInfo();
				bool bIsCardFinal = true;
				if ( pInfo_Test )
				{
					GetMainFrame()->OpenPopup( UPW_REWARDITEM, const_cast<MCardInfo*>(pInfo_Test), &bIsCardFinal);
					return;
				}
			}
		}
	#endif

	const MCardInfo* pInfo_Reward = nullptr;
	bool bIsAnyQuestCompleted = false;	bool bIsCardRewardExist = false;	bool bIsCardFinal = false;

	MCardUtil::PopRewardMark(pInfo_Reward, bIsAnyQuestCompleted, bIsCardRewardExist, bIsCardFinal);

	if (bIsAnyQuestCompleted == false)
		return;

	if (bIsCardRewardExist == false)
	{
		// �ӹ�ī���� ��� ����Ʈ�� �Ϸ��� ���� �ƴ� ��� �ӹ��� �Ϸ��ߴٸ� �˾��� ���ϴ�.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_EXIST_COMPLETE_MISSION"));//�Ϸ�� �ӹ��� �ֽ��ϴ�.
	}
	else
	{
		GetMainFrame()->OpenPopup( UPW_REWARDITEM , const_cast<MCardInfo*>(pInfo_Reward), &bIsCardFinal);
	}
}

bool UIPhaseReadyRoom_V11::IsRoomUser( i3::rc_wstring pNick )
{
	if( GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM) 
		return false;

	for( int i = 0; i < SLOT_ROOM_VIEW_COUNT; i++)
	{
		if( i < SLOT_MAX_COUNT)
		{
			const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(i);

			if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
			{
				i3::rc_wstring wstrUserNick = BattleSlotContext::i()->getNickForSlot(i);
				if( i3::generic_is_equal( wstrUserNick, pNick ) == true ) return true;
			}
		}
	}

	return false;
}

void UIPhaseReadyRoom_V11::ChangeGMObserver(bool bChecked)
{
	// üũ�� �ȵ��ִٸ� Observer ��� ���� �� Check ����
	// Check ���ִ� ���, Observer ��� ���� �� Check ����
	UserInfoContext::i()->SetEnable_GMObserver(bChecked != false);
}

void UIPhaseReadyRoom_V11::_DoSwapTeam( void)
{
	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		return;

	// TODO : ���ٲٱ�.
	if (0.0f < m_rTeamChangeDelay)
		return;

	m_rTeamChangeDelay = 2.0f;	// 2�ʰ� ��ٸ���.

	if( _IsHost())
	{
		GameEventSender::i()->SetEvent(EVENT_TEAM_CHANGE);
	}
}


//------------------------------------------------------------------------------//
//								Error Handler									//
//------------------------------------------------------------------------------//
void UIPhaseReadyRoom_V11::EhKicked(bool isGM)
{
	if( m_bKickingState == false)
	{	// Kick ���߽��ϴ�.
		// �κ�� �̵�
		m_bKickingState = true;

		//if (false == g_pGameContext->IsKicking())
		//{
		//	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_STRING("STBL_IDX_EP_ROOM_KICKED"));

		//	g_pGameContext->AddGameFlag( GAMEFLAG_KICKBYHOST);
		//}

		if( isGM)	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_GM_GAME_KICK_EXIT"));
		else		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_KICKED"));

		i3UI::setDisableUIInput(true);
		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
		m_bEnableUpdate = false;
		m_bMyPhase = false;
	}
}


void UIPhaseReadyRoom_V11::CbBeHost(void* pThis,INT32 nParam)
{
	if(pThis != nullptr)
		((UIPhaseReadyRoom_V11*)pThis)->m_pBeHostMessageBox = nullptr;
}

void UIPhaseReadyRoom_V11::CbSearchHost(void* pThis,INT32 nParam)
{
	if(pThis != nullptr && MBR_OK == nParam)
	{
		INT32 slotIdx = ((UIPhaseReadyRoom_V11*)pThis)->m_transferHostSlotIdx;
		GameEventSender::i()->SetEvent(EVENT_TRANSFER_HOST, &slotIdx);
	}
}

void UIPhaseReadyRoom_V11::cbQueryLeaveClanBattle(void * pThis, INT32 nParam)
{
	//	Ŭ�������� ������ ó��
	if( nParam == MB_OK)
	{
		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
		i3UI::setDisableUIInput( true);

		UIPhaseReadyRoom_V11 * pPhase = (UIPhaseReadyRoom_V11 *)pThis;
		pPhase->m_bMyPhase = false;
	}
}

void UIPhaseReadyRoom_V11::CbMoveChannel(void* pThis,INT32 nParam)
{
	// ���/�ʺ� ä�� ������ �������� ���Ͽ� ä�� �̵�
	if (nParam == MB_OK)
	{
		ChannelContext::i()->SetMoveChannel(); 

		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
		i3UI::setDisableUIInput( true);

		UIPhaseReadyRoom_V11 * pPhase = (UIPhaseReadyRoom_V11 *)pThis;
		pPhase->m_bMyPhase = false;
		//UIPhaseReadyRoom_V11* pReadyRoom = (UIPhaseReadyRoom_V11*)pThis;
		//pReadyRoom->SetUpdateEnable(false);
		//((CStageReady*)pReadyRoom->getParent())->SetMenuClanInputDisable(false);
	}
}

void UIPhaseReadyRoom_V11::CbBlockUser(void* pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
	}
}

void UIPhaseReadyRoom_V11::CbFinalComplateConfirm(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
	}
}

void UIPhaseReadyRoom_V11::CbMoveShop(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		UIPhaseReadyRoom_V11* pReadyRoom = (UIPhaseReadyRoom_V11*)pThis;
		pReadyRoom->GetMainFrame()->ChangeSubPhase( USP_SHOP);
	}
}


//--------------------------------------------------------------------------------------//
//									Kick RoomMaster										//
//--------------------------------------------------------------------------------------//
void UIPhaseReadyRoom_V11::__CreateRoomMasterKick()
{
	__ResetRoomMasterKick();			
	m_bGameStart = false;

	m_pRoomMasterAwayChecker = CAwayChecker::new_object();
	m_pRoomMasterAwayChecker->SetReceiver(this);
	m_pRoomMasterAwayChecker->Reset();

	I3_GAMENODE_ADDCHILD(this, m_pRoomMasterAwayChecker);
}

void UIPhaseReadyRoom_V11::__CheckRoomMasterKick()
{
	if( m_bGameStart)
	{
		__StopRoomMasterKick();
	}

	if( m_bPauseKickMaster)
	{
		__ResetRoomMasterKick();		

		// ���� ���� �˾� ������ ����.
		// �Ϲ����� ����.
		GetMainFrame()->setKickBtNoStartGame( false);

		m_pRoomMasterAwayChecker->SetTime(AWAY_INPUT_ROOM_MASTER_GAMESTART_TIME + AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION, AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION);
	}

	if( m_bAllUserReady)
	{
		if(0.0f == m_pRoomMasterAwayChecker->GetCurrentTime())
		{
			m_bPauseKickMaster = true;
			return;
		}

		m_RoomMasterAwayCheckerTime = 0;		
		m_bAllUserReady = false;
		m_bRoomMsterAwayCountDown = true;

		I3ASSERT( m_pRoomMasterAwayChecker != nullptr);
		m_pRoomMasterAwayChecker->Restart();

		GetMainFrame()->CloseAwayMessageBox();

		m_RoomMasterAwayCheckerTime = (INT32)(m_pRoomMasterAwayChecker->GetCurrentTime());
	}		

	if( m_bRoomMsterAwayCountDown)
	{
		// ä��â�� ��� �޽��� ǥ��
		if ( m_pRoomMasterAwayChecker->GetCurrentTime() < m_RoomMasterAwayCheckerTime)
		{
			m_RoomMasterAwayCheckerTime = INT32(m_pRoomMasterAwayChecker->GetCurrentTime());
			if( m_RoomMasterAwayCheckerTime == 14)
				return;
			if ((INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION - 1.0f) <= m_RoomMasterAwayCheckerTime)// && (INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_TIME + AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION) > m_RoomMasterAwayCheckerTime)
			{
				i3::rc_wstring countdownText;
				i3::sprintf(countdownText, 	GAME_STRING("STR_TBL_GUI_READYROOM_NOTICE_PRE_ROOM_MSTER_KICK_ALARM"), 
					m_RoomMasterAwayCheckerTime - (INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION - 2.0f));/*{col:255,120,0,255}���� ���� %d�� ��{/col}*/
				g_pFramework->getChatBox()->PutSystemChatting(countdownText);

				i3SoundContext* pSoundContext = g_pViewer->GetSoundContext();
				if( pSoundContext != nullptr)
				{
					pSoundContext->Play(m_pCountdownSound[0]);
				}
			}
		}
	}
}

void UIPhaseReadyRoom_V11::__StopRoomMasterKick( void)
{
	I3ASSERT( m_pRoomMasterAwayChecker!= nullptr);

	m_pRoomMasterAwayChecker->Reset();		

	__ResetRoomMasterKick();
}

void UIPhaseReadyRoom_V11::__ResetRoomMasterKick( void)
{
	m_bPauseKickMaster = false;
	m_bAllUserReady = false;
	m_bRoomMsterAwayCountDown = false;
	m_RoomMasterAwayCheckerTime = 0;
}

void UIPhaseReadyRoom_V11::__ProcessKickRoomMaster( void)
{
	INT32 i;

	if( _IsHost())
	{
		INT32 OtherUserCount = 0;

		if( m_pBeHostMessageBox == nullptr)
			m_bBeHostMessageOK = false;

		if( m_bBeHostMessageOK)
			m_bPauseKickMaster = true;

		for( i = 0; i < SLOT_MAX_COUNT; i++)
		{	
			if( BattleSlotContext::i()->getMainSlotIdx() == i)		continue;

			const SLOT_INFO* pSlot = BattleSlotContext::i()->getSlotInfo(i);

			// ���Կ� �������� �ִ�.
			if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
			{
				if( SLOT_STATE_READY != pSlot->_State)
				{	// ���� Ready�� �ƴմϴ�.
					// ī��Ʈ�ٿ��� �����մϴ�.
					__StopRoomMasterKick();
					return;
				}			

				OtherUserCount++;
			}			
		}

		// ������� ������ �ϳ��� ������ �˻�
		if(0 < OtherUserCount)
		{
			if( m_bRoomMsterAwayCountDown == false)
			{
				// ��� ���������� ���� �ð� Ȱ��ȭ
				m_bAllUserReady = true;
			}
		}
		else
		{
			__StopRoomMasterKick();
		}
	}
	else
	{
		__StopRoomMasterKick();
	}

	// �������� ������ ���¿��� ������ ���� ���� ������ ���� ��Ų��.
	//Room Master GameStart Timer After All User Ready
	__CheckRoomMasterKick();
}

bool	UIPhaseReadyRoom_V11::OnInviteMove( void)
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

bool UIPhaseReadyRoom_V11::OnRevive( i3RenderContext * pCtx)
{
	if( UIPhaseBase::OnRevive( pCtx) == false)
		return false;

	return true;
}

void UIPhaseReadyRoom_V11::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPhaseReadyRoom_V11::ReceiveEvent_Ready_Battle( INT32 result, const i3::user_data&)
{
	if (EV_IS_ERROR(result, EVENT_ERROR_NO_REAL_IP))
		g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_REAL_IP_S"));
	else if (EV_IS_ERROR(result, EVENT_ERROR_NO_READY_TEAM))
		g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_READY_TEAM_S"));
	else if (EV_IS_ERROR(result, EVENT_ERROR_NO_START_FOR_UNDER_NAT))
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_UNDER_NAT"));
	else if ( EV_IS_ERROR( result, EVENT_ERROR_NO_CLAN_TEAM ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_NO_CLAN_TEAM"));
	else if ( EV_IS_ERROR( result, EVENT_ERROR_NO_CLAN_TEAM_NOTFULL ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_TEAM_NOTFULL"));
	else if ( EV_IS_ERROR( result, EVENT_ERROR_NO_CLAN_TEAM_NOT_ALL_READY ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_NOT_ALL_READY"));
	else if ( EV_IS_ERROR( result, EVENT_ERROR_EVENT_READY_WEAPON_EQUIP ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_ERROR_READY_WEAPON_EQUIP"));
	else if ( EV_IS_ERROR( result, EVENT_ERROR_NO_CLAN_DUPLE_CLAN_TEAM ) )
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_START_FOR_DUPLE_CLAN_TEAM"));
	else
		GameUI::ErrorHandleStage( EVENT_READY_BATTLE, result);

	if( result != 0 )	_ReadyButtonActive( true );	// ���۹�ư ��ҽõ� ����� ���ͼ� ����ó��
}

void UIPhaseReadyRoom_V11::ReceiveEvent_Start_Game( INT32 result, const i3::user_data&)
{

	// stage ���� ���� ���� ������ ĳ���� ���ҽ��� ŵ����!
	Avatar * pAvatar = (Avatar*) g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	if( pAvatar != nullptr)
		pAvatar->KeepResource();

	if(  CharaViewMgr::i()->isLoadWaiting() )
	{	
		SetInputDisable( true);
	}
	else		// �ش� stage�� ����
		g_pFramework->JumpToBattle();
}

void UIPhaseReadyRoom_V11::ReceiveEvent_GetLobbyUserInfo(INT32 result, const i3::user_data&)
{
	m_bInviteResult = EV_SUCCESSED(result);
	m_bInviteMessage = true;
}

void UIPhaseReadyRoom_V11::ReceiveEvent_EnterLobby(INT32 result, const i3::user_data&)
{
	if( EV_SUCCESSED(result))
	{
		// keyword : Star Player
		MyRoomInfoContext::i()->setInReadyRoom(false);

		if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) && 
			ChannelContext::i()->GetBattleChannel())
		{
			ClanGameContext::i()->ResetMercRank();
		}

		if( ChannelContext::i()->GetBattleChannel() && (ClanGameContext::i()->getClanBattleLobbyEnable() == true))
		{
			GetMainFrame()->MovePhase( UIPHASE_CLANLOBBY);
		}
		else
			GetMainFrame()->MovePhase( UIPHASE_LOBBY);

		ClanGameContext::i()->resetClanBattleTeamInfo();
	}
	else
	{
		if (CCommunityContext::i()->IsMovingToUser())
		{
			CCommunityContext::i()->CompleteMovingToUser();
		}

		m_bMyPhase = true;
		m_bEnableUpdate = true;
		i3GuiRoot::setGuiDisableInput(false);
		GameUI::ErrorHandleStage( EVENT_ENTER_LOBBY, result);
	}

	GetMainFrame()->GetRoomMasterAwayCheckerManager()->SetGameStart(false);
	m_bKickingState = false;
	m_bGameStart = false;
}

void UIPhaseReadyRoom_V11::ReceiveEvent_KickUser(INT32 result, const i3::user_data&)
{
	if( EV_SUCCESSED(result))
	{
		bool isGM = result > 0 ? true : false;
		EhKicked(isGM);
	}
}

void UIPhaseReadyRoom_V11::ReceiveEvent_Unready_4vs4(INT32 result, const i3::user_data&)
{
	/*4vs4 �ο����ѿ� �ɷ�\n�غ�Ϸ� ���¸� �����մϴ�.*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_4VS4_LIMIT_COUNT"));
}

void UIPhaseReadyRoom_V11::ReceiveEvent_GetRoomAddInfo(INT32 result, const i3::user_data&)
{
	// ������ �� ���� �����츦 ����.
	{
		UIFloatReadyRoomInfo_V11 * pRoomInfo = (UIFloatReadyRoomInfo_V11*) GetMainFrame()->GetFloating( UFW_READYROOMINFO);
		pRoomInfo->UpdateMapInfo();
	}
}

void UIPhaseReadyRoom_V11::ReceiveEvent_DestroyNetwork(INT32 result, const i3::user_data&)
{
	// ��Ʈ��ũ�� ��������
	GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_EP_GAME_DESTROYED_NETWORK_E"), nullptr, EXIT_CALLBACK(CbExitNormal, true), this);
}

void UIPhaseReadyRoom_V11::ReceiveEvent_SearchHost(INT32 result, const i3::user_data&)
{
	i3::stack_wstring wstr_message; 

	if (EV_SUCCESSED(result))
	{	
		/*���� ���� �˻� ���\n[%s]��\n������ ���� �Ͻðڽ��ϱ�?*/
		i3::sprintf( wstr_message, GAME_RCSTRING("STR_TBL_GUI_READY_MESSAGE_CHANGE_ROOM_MASTER"), BattleSlotContext::i()->getNickForSlot(result));
	}
	else
	{
		if ( NetworkContext::i()->isRealIp() )
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

void UIPhaseReadyRoom_V11::ReceiveEvent_Request_HostChange(INT32 result, const i3::user_data&)
{
	i3::stack_wstring wstr_message; 

	/*[%s]���� ����˻��� ��û�ϼ̽��ϴ�.*/
	i3::sprintf( wstr_message, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_ROOM_MASTER_CHANGE_REQUEST"), BattleSlotContext::i()->getNickForSlot(result));
	GameUI::PrintSystemChat( wstr_message);
}

void UIPhaseReadyRoom_V11::ReceiveEvent_Transfer_Host(INT32 result, const i3::user_data&)
{
	if (EV_SUCCESSED(result))
	{	
		i3::rc_wstring wstr_message; 

		/*[%s]���� ������ �Ǿ����ϴ�.*/
		i3::sprintf( wstr_message, GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_CHANGED_ROOM_MASTER"), BattleSlotContext::i()->getNickForSlot( result));
		GameUI::PrintSystemChat( wstr_message);

		CheckBeHost();
		m_bIsHost = true;
	}
	else
	{
		/*������ �ѱ� ������ ����ڰ� �����ϴ�.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_NO_EXIST_TO_CHANGE_ROOM_MASTER"));
	}
}

void UIPhaseReadyRoom_V11::ReceiveEvent_Item_Auth(INT32 result, const i3::user_data&)
{
	if (EV_FAILED(result))
	{
		/*��Ʈ��ũ ������ ������ ������ �����߽��ϴ�.\n �ٽ� �õ��� �ּ���.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_ITEM_IDENTYFY_FAIL_BY_NETWORK_PROBLEM"));
	}
}

void UIPhaseReadyRoom_V11::ReceiveEvent_Item_Auth_Data(INT32 result, const i3::user_data&)
{
	if (EV_FAILED(result))
	{
		// ������ ��� ����
		// �������� ����� �����Ͽ����ϴ�.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_3"), &GAME_RCSTRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
	}
}

void UIPhaseReadyRoom_V11::ReceiveEvent_User_Slot(INT32 result, const i3::user_data&)
{
	if (GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE )
		CheckBeHost();

	_ProcessClanBattle();
}


void UIPhaseReadyRoom_V11::ReceiveEvent_Get_UserEquipmentInfo( INT32 result, const i3::user_data&)
{
	//UIFloatCharacterView * pCharaWin = static_cast<UIFloatCharacterView *>(GameUI::GetFloating( UFW_CHARACTER));
	//if( pCharaWin != nullptr)
	//	pCharaWin->UpdateCharacter( result);
}

// �� ������ �����
void UIPhaseReadyRoom_V11::ReceiveEvent_Change_ReadyRoomInfo( INT32 result, const i3::user_data& UserData)
{
	//GetMainFrame()->GameEventTransferToPopup( EVENT_CHANGE_READYROOM_INFO, result, UserData);

	CheckBeHost();
}

void UIPhaseReadyRoom_V11::ReceiveEvent_RoomScore( INT32, const i3::user_data&)
{
	if( GetMainFrame() != nullptr)
		GetMainFrame()->OpenPopup(UPW_BREAK_INTO_GAME, this);
}

void UIPhaseReadyRoom_V11::ReceiveEvent_Giveup_Battle(INT32, const i3::user_data&)
{
	//@eunil. ��ġ����ŷ ���� ����. Ŭ���� �ߵ� ���� ó����. �뺴�� ��� �ߵ� ��Ż�ڴ� �г�Ƽ�� �޴´�.
	if(ClanGameContext::i()->getMyMercenaryState())
		m_bGiveupBattle = true;
}

void UIPhaseReadyRoom_V11::ReceiveEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

void UIPhaseReadyRoom_V11::ReceiveEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnableFloatWin( UFW_PLAYTIMEALARM);
}

void UIPhaseReadyRoom_V11::ReceiveEvent_ForceLeaveMatchroom(INT32 arg, const i3::user_data&)
{
	ClanGameContext::i()->setClanBattleLobbyEnable(true);
	GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
}