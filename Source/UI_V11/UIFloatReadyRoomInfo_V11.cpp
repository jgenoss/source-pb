#include "pch.h"
#include "UIFloatReadyRoomInfo_V11.h"

#include "UI/UIMainFrame.h"
#include "UI/UIUtil.h"

#include "UIFloatReadyRoomMissionInfo_V11.h"

#include "../TextSet.h"
#include "../GameGUI.h"

#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "StageBattle/AIModeContext.h"
#include "RandomMapContext.h"
#include "UserInfoContext.h"

#include "ClientStageInfoUtil.h"
#include "ClientStageInfoContext.h"


static UIFloatReadyRoomInfo_V11 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	if( pFrame)
	{
		return (UIFloatReadyRoomInfo_V11*) pFrame->GetFloating( UFW_READYROOMINFO);
	}

	return nullptr;
}

//------------------------------------------------------------------------------//
//									Lua Glue									//
//------------------------------------------------------------------------------//
extern "C" {
	int UIFloatReadyRoomInfo_ClickMapInfo( LuaState * L)
	{
		GetThis()->ChangeMapInfoTab();
		return 0;
	}

	int UIFloatReadyRoomInfo_ClickMissionInfo( LuaState * L)
	{
		GetThis()->ChangeMissionInfoTab();
		return 0;
	}

	int UIFloatReadyRoomInfo_ClickRoomSetting( LuaState * L)
	{
		GetThis()->ChangeRoomSetting();
		return 0;
	}
}

LuaFuncReg UIFloatReadyRoomInfo_Glue_V11[] = {
	{"ClickMapInfo",			UIFloatReadyRoomInfo_ClickMapInfo},
	{"ClickMissionInfo",		UIFloatReadyRoomInfo_ClickMissionInfo},
	{"ClickRoomSetting",		UIFloatReadyRoomInfo_ClickRoomSetting},
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIFloatReadyRoomInfo_V11);

UIFloatReadyRoomInfo_V11::UIFloatReadyRoomInfo_V11()
{
	// �ӹ�ī�� �۾� �� �־���� �մϴ�. 20150520 ������
	m_pMissionInfo = nullptr;
	m_CurrentTab = RIT_NONE;

	m_AlignedPos = UIFLOATING_POS_LEFTTOP;

#if defined( I3_DEBUG)
	SetName( "ReadyRoomInfo");
#endif
}

UIFloatReadyRoomInfo_V11::~UIFloatReadyRoomInfo_V11()
{
	// �ӹ�ī�� �۾� �� �־���� �մϴ�. 20150520 ������
	if( m_pMissionInfo)
	{
		m_pMissionInfo->SetEnable( false);
		RemoveChild( m_pMissionInfo);
		m_pMissionInfo->Release();
	}
}

bool UIFloatReadyRoomInfo_V11::_BeHost(void)
{
	return BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx();
}

/*virtual*/ void UIFloatReadyRoomInfo_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);

	m_pMapInfoFrame = (i3UIFrameWnd*) pScene->FindChildByName( "fwndMapInfo");
	I3ASSERT( m_pMapInfoFrame);

	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::L_TO_R);

	// �ӹ�ī�� �۾� �� �־���� �մϴ�. 20150520 ������
	m_pMissionInfo->BindScene( pScene);

	SetReadyRoomNeedUpdateInfo( true);

	// ������ �ν�Ʈ �̺�Ʈ ��Ʈ�� ����.
	LuaState * L = _CallLuaFunction("InitToolTip");
	if (L != NULL)
	{
		const i3::rc_wstring& wstrRMEvent = ClientStageInfoContext::i()->GetRandomMapEventString();
		i3Lua::PushStringUTF16To8(L, wstrRMEvent);	
		_EndLuaFunction(L, 1);
	}
}

/*virtual*/ void UIFloatReadyRoomInfo_V11::_ClearAtUnload( i3UIScene * pScene)
{
	UIFloatingBase::_ClearAtUnload( pScene);

	m_pMapInfoFrame = nullptr;
	// �ӹ�ī�� �۾� �� �־���� �մϴ�. 20150520 ������
	m_pMissionInfo->UnBindScene();

	SetReadyRoomNeedUpdateInfo( false);
}

/*virtual*/ void UIFloatReadyRoomInfo_V11::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// load scene
	AddScene( "Scene/Popup/PointBlankRe_ReadyRoom_MapInfo.i3UIs", UIFloatReadyRoomInfo_Glue_V11);

	// �ӹ�ī�� �۾� �� �־���� �մϴ�. 20150520 ������
	m_pMissionInfo = UIFloatReadyRoomMissionInfo_V11::new_object();
	I3ASSERT( m_pMissionInfo);
	m_pMissionInfo->OnCreate( this);
}

/*virtual*/ bool UIFloatReadyRoomInfo_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	
	OnSlideIn();

	// ó�� ����� ���� ǥ���� �� �ֵ��� �ʱ�ȭ
	m_CurrentTab = RIT_NONE;
	ChangeMapInfoTab();

	return true;
}

/*virtual*/ void UIFloatReadyRoomInfo_V11::SetInputDisable( bool bDisable )
{
	UIFloatingBase::SetInputDisable( bDisable );

	if( isEnable() && !bDisable )
	{
		LuaState*	L	= _CallLuaFunction( "InputDisable_MapOption" );
		if( L != nullptr ) _EndLuaFunction( L, 0 );
	}
}


/*virtual*/ bool UIFloatReadyRoomInfo_V11::OnExitStart( void)
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	OnSlideOut();

	return true;
}

/*virtual*/ void UIFloatReadyRoomInfo_V11::ProcessGameEvent( UINT32 event, INT32 arg)
{
	switch( event)
	{
	case EVENT_CHANGE_READYROOM_INFO:
		UpdateMapInfo();
		break;
	case EVENT_TRANSFER_HOST:
		// ���� ���� ����
		if (EV_SUCCESSED(arg))
		{	
			EnableRoomSet( arg == BattleSlotContext::i()->getMySlotIdx() );
		}
		break;
	case EVENT_M_RECEIVE_USER_SLOT:
		{
			if (GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE )
				EnableRoomSet(_IsHost());
		}		
		break;
	}
}

void UIFloatReadyRoomInfo_V11::ChangeRoomSetting()
{
	if( false == _IsHost() )
		return;

	bool isLobby = false;
	GetMainFrame()->TogglePopup( UPW_CREATEROOM_EX, (void*)&isLobby );
}


void UIFloatReadyRoomInfo_V11::ChangeMapInfoTab( void)
{
	if( m_CurrentTab != RIT_MAP)
	{
		m_CurrentTab = RIT_MAP;

		LuaState * L = _CallLuaFunction( "ChangeTab");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, static_cast<INT32>(m_CurrentTab));
			_EndLuaFunction( L, 1);
		}

		m_pMapInfoFrame->EnableCtrl( true, true);
		UpdateMapInfo();

		EnableRoomSet( _IsHost() );

		// �ӹ�ī�� �۾� �� �־���� �մϴ�. 20150520 ������
		m_pMissionInfo->OnExitStart();
	}
}

void UIFloatReadyRoomInfo_V11::EnableRoomSet( bool bHost )
{
	LuaState * L = _CallLuaFunction( "EnableRoomSet");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L, bHost);
		_EndLuaFunction( L, 1);
	}
}

void UIFloatReadyRoomInfo_V11::ChangeMissionInfoTab( void)
{
	if( m_CurrentTab != RIT_MISSION)
	{
		m_CurrentTab = RIT_MISSION;

		LuaState * L = _CallLuaFunction( "ChangeTab");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, static_cast<INT32>(m_CurrentTab));
			_EndLuaFunction( L, 1);
		}

		m_pMapInfoFrame->EnableCtrl( false, true);
		// �ӹ�ī�� �۾� �� �־���� �մϴ�. 20150520 ������
		m_pMissionInfo->OnEntranceStart();
	}
}

void UIFloatReadyRoomInfo_V11::UpdateMapInfo(void)
{
	if (IsReadyToUpdate() == false)
		return;

	//Scene�ε� �Ŀ� Update�� �� �ֽ��ϴ�.
	//OnEntranceStart�� ChangeMapInfoTab()�� ȣ�� �Ǳ������� true�� �˴ϴ�.
	if (GetReadyRoomNeedUpdataInfo() == false)
		return;

	// �������� �ҷ��´�
	ROOM_INFO_BASIC Info;
	ROOM_INFO_ADD	InfoAdd;

	MyRoomInfoContext::i()->getMyRoomInfo(&Info);
	MyRoomInfoContext::i()->getMyRoomInfoAdd(&InfoAdd);

	//getStageDisplayName( (STAGE_ID) Info._StageID) ������ ���̵��� ���ü� �ֽ��ϴ�.
	if (Info._StageID == 0)
		return;

	// ���/�������� �̹��� ����
	{
		INT32 stage_shape_idx = 0;
		bool bRandomMapEventShow = false;

		//	 �� �̹���
		if (MyRoomInfoContext::i()->IsRandomMap())
		{
			stage_shape_idx = g_pFramework->GetRandomMapIdx();
			bRandomMapEventShow = ClientStageInfoContext::i()->IsRandomMapEventOpened();
		}
		else
			stage_shape_idx = g_pFramework->GetMapShapeIdx((STAGE_ID)Info._StageID);

		//  X-Mas Mark
		bool is_enable_xmas = false;
		if (TESTBIT(MyRoomInfoContext::i()->getStage()->m_ui8Mark, GOODS_MARK_XMAS))
			is_enable_xmas = true;

		//	���� ���� & ���� ���� ����
		INT32 mode_option = GameUI::GetModeOptionShapeIndex(Info);

		LuaState * L = _CallLuaFunction("SetStageImageInfo");
		if (L != nullptr)
		{
			i3Lua::PushInteger(L, stage_shape_idx);				// �������� �̹��� ShapeIdx
			i3Lua::PushInteger(L, mode_option);					// ���� ���� & ���� ���� ����
			i3Lua::PushBoolean(L, is_enable_xmas ? true : false);	// X-Mas Mark 
			i3Lua::PushBoolean(L, bRandomMapEventShow ? TRUE : FALSE);	// ������ �ν�Ʈ �̺�Ʈ
			_EndLuaFunction(L, 4);
		}
	}

	// �� ����
	{
		UINT32 gameType = InfoAdd._SubType & 0x0F;
		UINT32 gameTime = InfoAdd._SubType & 0xF0;

		i3::rc_wstring wstr_room_name = Info._Title;

		//getMyRoomPW() �Լ� ��ü���� NULL�� �ִ°ɷ� �ް� ������, �ٸ����� ���Ƿ�, �ϴ� ���⿡�� �̷��� ó���մϴ�.
		char str_room_pw[NET_ROOM_PW+1];
		for (int i = 0; i < NET_ROOM_PW; i++)
			str_room_pw[i] = MyRoomInfoContext::i()->getMyRoomPW()[i];
		str_room_pw[NET_ROOM_PW] = '\n';

		i3::rc_wstring wstr_room_pw;
		i3::mb_to_utf16(str_room_pw, wstr_room_pw);


		i3::rc_wstring wstr_mode_name = CStageInfoUtil::GetRuleData(MyRoomInfoContext::i()->getStage()->m_ui32StageID)->getRuleName();
		
		i3::rc_wstring wstr_stage_name;
		GameUI::GetStageMapName(MyRoomInfoContext::i()->IsRandomMap(), (STAGE_ID)Info._StageID, wstr_stage_name);

		i3::rc_wstring wstr_vic_condition_label;
		i3::rc_wstring wstr_vic_condition;
		i3::rc_wstring wstr_variable_str = L"";

		bool is_ai_mode = MyRoomInfoContext::i()->getStage()->IsAiMode();
		if (is_ai_mode == true)
		{
			// ���� �� / ���ѽð�
			i3::sprintf(wstr_vic_condition_label, L"%s/%s", GAME_STRING("STR_TBL_GUI_READY_ENEMY_COUNT"), GAME_STRING("STR_TBL_GUI_READY_LIMIT_TIME"));

			i3::rc_wstring wstr_temp;
			i3::sprintf(wstr_temp, GAME_RCSTRING("STBL_IDX_COUNT_PERSON"), AIModeContext::i()->NewEnemyCount);
			i3::sprintf(wstr_vic_condition, L"%s / %s", wstr_temp, TextSet::GameTime(gameTime));

			// ���̵�
			wstr_variable_str = TextSet::AIModeLevel(AIModeContext::i()->StartLevel);
		}
		else
		{
			// �¸� ���� / ���� �ð�
			i3::sprintf(wstr_vic_condition_label, L"%s/%s", GAME_STRING("STR_TBL_GUI_READY_VICTORY_CONDITION"), GAME_STRING("STR_TBL_GUI_READY_LIMIT_TIME"));
			i3::sprintf(wstr_vic_condition, L"%s / %s", TextSet::VictoryCondition(MyRoomInfoContext::i()->getStage()->GetStageMode(), gameType), TextSet::GameTime(gameTime));

			// �������� ��Ʈ�� ǥ�� (���� ����)
			// 1. ������ �������밡 �����־����.
			if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_TEAM_CHANGE))
			{
				// 2. �������� �������븦 �����ϴ� ��忩����.
				if (MyRoomInfoContext::i()->getStage()->IsAtkDefChangeStage())
				{
					// 3. ���� ���� ��Ʈ�� ǥ�� ����.
					if ((MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) == false) &&
						(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) == false))
					{
						// �������� On/Off ��Ʈ�� �Է�.
						if ((GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(Info._InfoFlag) == ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE))
						{
							// On
							wstr_variable_str = GAME_RCSTRING("STR_OPT_DEFAULT_ON");
						}
						else
						{
							// Off
							wstr_variable_str = GAME_RCSTRING("STR_OPT_DEFAULT_OFF");
						}
					}
				}
			}
		}

		LuaState * L = _CallLuaFunction("SetRoomBasicInfo");
		if (L != nullptr)
		{
			i3Lua::PushStringUTF16To8(L, wstr_room_name);			// �� �̸�
			i3Lua::PushStringUTF16To8(L, wstr_room_pw);				// �� ��й�ȣ
			i3Lua::PushStringUTF16To8(L, wstr_mode_name);			// ��� ��
			i3Lua::PushStringUTF16To8(L, wstr_stage_name);			// �������� ��
			i3Lua::PushStringUTF16To8(L, wstr_vic_condition_label);	// �¸� ���� �� (�Ϲ��� / AI���� ���� �ٸ�.)
			i3Lua::PushStringUTF16To8(L, wstr_vic_condition);		// �¸� ����
			i3Lua::PushStringUTF16To8(L, wstr_variable_str);		// ���� �����Ǵ� ��Ʈ�� (�������� on/off or AI Level)
			i3Lua::PushBoolean(L, is_ai_mode ? true : false);		// AI�� Ȯ�� Flag
			_EndLuaFunction(L, 8);
		}
	}

	// ���� ���� �ɼ� ����
	{
		bool is_lock_primary = TESTBIT(Info._WeaponFlag, LOCK_PRIMARY) ? true : false;
		bool is_lock_secondary = TESTBIT(Info._WeaponFlag, LOCK_SECONDARY) ? true : false;
		bool is_lock_knife = TESTBIT(Info._WeaponFlag, LOCK_MELEE) ? true : false;
		bool is_lock_throw = TESTBIT(Info._WeaponFlag, LOCK_THROW) ? true : false;
		bool is_lock_rpg7 = TESTBIT(Info._WeaponFlag, LOCK_RPG7) ? true : false;

		LuaState * L = _CallLuaFunction("SetRoomWeaponLimitInfo");
		if (L != nullptr)
		{
			i3Lua::PushInteger(L, is_lock_primary ? 0 : 1);
			i3Lua::PushInteger(L, is_lock_secondary ? 0 : 1);
			i3Lua::PushInteger(L, is_lock_knife ? 0 : 1);
			i3Lua::PushInteger(L, is_lock_throw ? 0 : 1);
			i3Lua::PushInteger(L, is_lock_rpg7 ? 0 : 1);
			_EndLuaFunction(L, 5);
		}
	}

	// �� �ΰ� �ɼ� ����
	{
		bool is_jump_in = (InfoAdd._LockInterEnter > 0) ? true : false;
		bool is_observer = (InfoAdd._LockObserver > 0) ? true : false;

		i3::rc_wstring wstr_team_balance;
		switch (InfoAdd._LockTeamBalance)
		{
		case 0:	i3::sprintf(wstr_team_balance, GAME_RCSTRING("STR_TBL_GUI_READY_TEAM_BALANCE_OFF"));		break;
		case 1:	i3::sprintf(wstr_team_balance, GAME_RCSTRING("STR_TBL_GUI_READY_TEAM_BALANCE_PERSON"));		break;
		default:	i3::sprintf(wstr_team_balance, GAME_RCSTRING("STR_TBL_GUI_READY_TEAM_BALANCE_SKILLED"));	break;
		}

		LuaState * L = _CallLuaFunction("SetRoomSubOptInfo");
		if (L != nullptr)
		{
			i3Lua::PushInteger(L, is_jump_in ? 1 : 0);
			i3Lua::PushInteger(L, is_observer ? 1 : 0);
			i3Lua::PushStringUTF16To8(L, wstr_team_balance);
			_EndLuaFunction(L, 3);
		}
	}
}