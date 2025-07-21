#include "pch.h"
#include "UIFloatLobbyMapInfo_V10.h"

#include "../TextSet.h"
#include "../UI/UIMainFrame_V10.h"
#include "../UI/UIUtil.h"

#include "ClientStageInfoUtil.h"
#include "ClientStageInfoContext.h"

I3_CLASS_INSTANCE( UIFloatLobbyMapInfo_V10);

UIFloatLobbyMapInfo_V10::UIFloatLobbyMapInfo_V10() : m_pMapImage(nullptr), m_pGameModeImage(nullptr), 
													m_pNewIcon(nullptr), m_pHeadHunter(nullptr)
{
	vecStaticText.reserve(STATIC_TEXT_MAX);
}

UIFloatLobbyMapInfo_V10::~UIFloatLobbyMapInfo_V10()
{

}

void UIFloatLobbyMapInfo_V10::OnCreate( i3GameNode * pParent )
{
	UIFloatingBase::OnCreate(pParent);

	AddScene("Scene/Main/PointBlankRe_Lobby_MapInfo_ToolTip.i3UIs");
}

bool UIFloatLobbyMapInfo_V10::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr */ )
{
	if( UIFloatingBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	_SetMapInfo( static_cast<ROOM_INFO_BASIC*>(pArg1), static_cast<ROOM_INFO_ADD*>(pArg2));
	m_slide.ForceEnd();

	return true;
}

void UIFloatLobbyMapInfo_V10::_SetMapInfo(const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo )
{
	// Text ����
	_SetInfoText(pInfo, pAddInfo);

	// Image ����m_pImgRandMapEvent
	_SetInfoImage(pInfo);
}

void UIFloatLobbyMapInfo_V10::__GetTeamBalance( const ROOM_INFO_ADD * pAddInfo, i3::wstring& output )
{
	switch(pAddInfo->_LockTeamBalance)
	{
	case 0:		
		output = GAME_RCSTRING("STR_TBL_GUI_READY_TEAM_BALANCE_OFF");/*����*/
		break;
	case 1:
		output = GAME_RCSTRING("STR_TBL_GUI_READY_TEAM_BALANCE_PERSON");/*�ο�*/
		break;
	default:
		output = GAME_RCSTRING("STR_TBL_GUI_READY_TEAM_BALANCE_SKILLED"); /*�Ƿ�*/
		break;
	}
}

void UIFloatLobbyMapInfo_V10::__GetObserverText( const ROOM_INFO_ADD * pAddInfo, i3::wstring& output )
{
	if( TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_CHASE) &&
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_FREE) &&
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_ENEMY) &&
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_ENEMY_HP) )
	{
		output = GAME_RCSTRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER1"); // ��� ���
	}
	else if( TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_1PVONLY) ||
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_CHASE) ||
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_FREE) ||
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_ENEMY) ||
		TESTBIT(pAddInfo->_LockObserver, LOCK_OBSERVER_ENEMY_HP) )
	{
		output = GAME_RCSTRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER2"); // ���
	}
	else
	{
		output = GAME_RCSTRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER3"); // ������� ����
	}
}

void UIFloatLobbyMapInfo_V10::__GetWeaponLimit( const ROOM_INFO_BASIC * pInfo, i3::wstring& output )
{
	if (pInfo->_WeaponFlag == LOCK_BASIC)
	{
		output = GAME_RCSTRING("STR_TBL_GUI_LOBBY_NONE_TEXT");
	}
	else
	{
		output = GAME_RCSTRING("STR_TBL_GUI_LOBBY_LIMIT_TEXT");
	}
}

void UIFloatLobbyMapInfo_V10::__GetJoinLimit( const ROOM_INFO_ADD * pAddInfo, i3::wstring& output )
{
	if (0 == pAddInfo->_LockInterEnter)
	{
		output = GAME_RCSTRING("STR_TBL_GUI_LOBBY_NONE_TEXT");
	}
	else
	{
		output = GAME_RCSTRING("STR_TBL_GUI_LOBBY_LIMIT_TEXT");
	}
}

void UIFloatLobbyMapInfo_V10::__GetRoomStateText( const ROOM_INFO_BASIC * pInfo, i3::wstring& output )
{
	// ���� ���¿� ���� "������" or "�����" ǥ��
	if (ROOM_STATE_READY != pInfo->_State)
	{
		// �������϶� �������� or �� �ο��� �������� ���� �Ұ� ó��
		if( GET_ROOM_INFO_INTERENTER(pInfo->_InfoFlag) > 0 ||
			pInfo->_PersonNow == pInfo->_PersonPlayerMax)
		{
			output = GAME_RCSTRING("STR_UI_CANT_ENTER");
		}
		else
		{
			output = GAME_RCSTRING("STBL_IDX_ROOM_STATE_PLAY");
		}
	}
	else
	{
		// ������϶� �� �ο��� ���� ���� ���� �Ұ� ó��
		if( pInfo->_PersonNow == pInfo->_PersonPlayerMax)
		{
			output = GAME_RCSTRING("STR_UI_CANT_ENTER");
		}
		else
			output = GAME_RCSTRING("STBL_IDX_ROOM_STATE_WAIT");
	}
}

void UIFloatLobbyMapInfo_V10::__GetGameSettingText( const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo, i3::wstring& output)
{
	UINT8 GameSetPrim			= pAddInfo->_SubType & 0x0F;
	UINT8 GameSetSec			= pAddInfo->_SubType & 0xF0;

	TextSet::GetWinObjectText(output, CStageInfoUtil::GetStageData(pInfo->_StageID), GameSetPrim, GameSetSec);
}

void UIFloatLobbyMapInfo_V10::__GetMapName( const ROOM_INFO_BASIC * pInfo, i3::wstring& output)
{
	bool Random;
	Random = (pInfo->_InfoFlag & ROOM_INFO_FLAG_RANDOM_MODE) ? true : false;
	if( Random)
	{
		output = GAME_RCSTRING("STR_POPUP_RANDOMMAP");		
	}
	else
	{
		CSI_MAP* mapData = CStageInfoUtil::GetMapData(pInfo->_StageID);
		output = mapData != nullptr ? mapData->getDisplayName() : L"NotHaveData";
	}
}

bool UIFloatLobbyMapInfo_V10::OnExitStart( void )
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	if( isEnable() == true)
	{
		SetEnable(false);
	}

	m_slide.ForceEnd();
	return true;
}

void UIFloatLobbyMapInfo_V10::EnableToolTip( const ROOM_INFO_BASIC * pArg1 /*= nullptr*/, const ROOM_INFO_ADD * pArg2 /*= nullptr*/ )
{
	// Popup�� ���� ���� ��� Ű�� �ʴ´�.
	UIMainFrame* pMainframe = g_pFramework->GetUIMainframe();
	if(pMainframe->GetCurrentPopup() != nullptr) return;

	// ���� Phase�� Lobby�� �ƴϸ� return .
	if(pMainframe->GetCurrentPhaseType() != UIPHASE_LOBBY) return;

	OnEntranceStart( const_cast<ROOM_INFO_BASIC*>(pArg1), const_cast<ROOM_INFO_ADD*>(pArg2) );
}

void UIFloatLobbyMapInfo_V10::DisableToolTip( void )
{
	OnExitStart();
}

void UIFloatLobbyMapInfo_V10::ChangeToolTip( const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo )
{
	_SetMapInfo(pInfo, pAddInfo);
}

void UIFloatLobbyMapInfo_V10::_SetInfoText( const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo )
{
	if( nullptr == pInfo)
	{	// �ش� ���� ���������..
		return;
	}

	i3::wstring wstrString[STATIC_TEXT_MAX];

	// Stage Name
	i3::wstring wstrStageName;
	__GetMapName(pInfo, wstrString[STATIC_MAPINFO]);

	// ���� ��� 
	wstrString[STATIC_GAMEMODE] = CStageInfoUtil::GetRuleData((UINT32)(pInfo->_StageID))->getRuleName();

	// ���� ����
	__GetGameSettingText(pInfo, pAddInfo, wstrString[STATIC_GAMESETTING]);

	// �븶����
	i3::sprintf(wstrString[STATIC_ROOMMASTER], L"%s", pAddInfo->_Nick);

	// �� ����
	__GetRoomStateText(pInfo, wstrString[STATIC_ROOMSTATE]);

	// ���� ����
	__GetJoinLimit(pAddInfo, wstrString[STATIC_JOINLIMIT]);

	// ���� ����
	__GetWeaponLimit(pInfo, wstrString[STATIC_WEAPONLIMIT]);

	// ���� ���
	__GetObserverText(pAddInfo, wstrString[STATIC_OBSERVERTEXT]);

	// ���뷱��
	__GetTeamBalance(pAddInfo, wstrString[STATIC_TEAMBALANCE]);

	// SetText
	for(INT32 i = 0; i < STATIC_TEXT_MAX; i++)
	{
		if(vecStaticText[i] != nullptr)
			vecStaticText[i]->SetText(wstrString[i]);
	}
}

void UIFloatLobbyMapInfo_V10::_SetInfoImage( const ROOM_INFO_BASIC * pInfo )
{
	INT32 mapshapeidx			= g_pFramework->GetMapShapeIdx(StageID::GetStageUID(pInfo->_StageID));
	UI_MODE_TYPE modeshapeidx	= GameUI::GetStageModeType(CStageInfoUtil::GetStageData(pInfo->_StageID), pInfo->_IsClanMatch);

	// �� �̹���
	bool bRandom = (pInfo->_InfoFlag & ROOM_INFO_FLAG_RANDOM_MODE) ? true : false;
	if (m_pMapImage != nullptr)
	{
		if (bRandom)
		{
			mapshapeidx = 26;
		}

		m_pMapImage->SetShapeIdx(mapshapeidx);
	}

	// ��� ��Ʈ��
	if(m_pGameModeImage != nullptr)
	{
		m_pGameModeImage->SetShapeIdx(modeshapeidx);
	}

	// New Icon
	if(m_pNewIcon != nullptr)
	{
		if ( CStageInfoUtil::GetStageData(pInfo->_StageID)->m_ui8Mark == GOODS_MARK_NEW )
		{
			m_pNewIcon->EnableCtrl(true);
		}
		else
		{
			m_pNewIcon->EnableCtrl(false);
		}
	}

	if (nullptr != m_pImgRandMapEvent)
	{		
		m_pImgRandMapEvent->EnableCtrl(true == bRandom && true == ClientStageInfoContext::i()->IsRandomMapEventOpened());
	}

	
	// �����������
	if(m_pHeadHunter != nullptr)
	{
		if (StageID::IsHeadShotOnly(pInfo->_StageID))
		{
			m_pHeadHunter->EnableCtrl(true);
			m_pHeadHunter->setShapeVisible(0, true);
		}
		else
			m_pHeadHunter->EnableCtrl(false);
	}
}

void UIFloatLobbyMapInfo_V10::_InitializeAtLoad( i3UIScene * pScene )
{
	UIFloatingBase::_InitializeAtLoad(pScene);

	// Text Control
	i3UIStaticText * temp = nullptr;

	temp = (i3UIStaticText*)pScene->FindChildByName("MapName");
	vecStaticText.push_back(temp);
	
	temp = (i3UIStaticText*)pScene->FindChildByName("GameModeName");
	vecStaticText.push_back(temp);

	temp = (i3UIStaticText*)pScene->FindChildByName("RoomSetting");
	vecStaticText.push_back(temp);

	temp = (i3UIStaticText*)pScene->FindChildByName("RoomMasterName");
	vecStaticText.push_back(temp);

	temp = (i3UIStaticText*)pScene->FindChildByName("RoomProcess");
	vecStaticText.push_back(temp);

	temp = (i3UIStaticText*)pScene->FindChildByName("BreakLimit");
	vecStaticText.push_back(temp);

	temp = (i3UIStaticText*)pScene->FindChildByName("WeaponLimit");
	vecStaticText.push_back(temp);

	temp = (i3UIStaticText*)pScene->FindChildByName("Observer");
	vecStaticText.push_back(temp);

	temp = (i3UIStaticText*)pScene->FindChildByName("TeamBalance");
	vecStaticText.push_back(temp);

	// Image Control
	m_pMapImage = (i3UIButtonImageSet *)pScene->FindChildByName("MapImage");
	m_pGameModeImage = (i3UIButtonImageSet *)pScene->FindChildByName("GameModeImage");

	m_pNewIcon = (i3UIImageBox *) pScene->FindChildByName("NewIcon");
	if(m_pNewIcon != nullptr)
	{
		m_pNewIcon->EnableCtrl(false);	
	}
	m_pHeadHunter = (i3UIImageBox *) pScene->FindChildByName("HeadHunter");
	if(m_pHeadHunter != nullptr)
	{
		m_pHeadHunter->EnableCtrl(false);
	}

	// ������ �ν�Ʈ �̺�Ʈ UI ����.
	m_pImgRandMapEvent = static_cast<i3UIImageBox*>(pScene->FindChildByName("i3UIImgRMEvent"));

	if (nullptr != m_pImgRandMapEvent)
	{
		const i3::rc_wstring& wstrRMEvent = ClientStageInfoContext::i()->GetRandomMapEventString();
		m_pImgRandMapEvent->SetToolTipMsg(wstrRMEvent.c_str());
	}
}

void UIFloatLobbyMapInfo_V10::_DefaultInfo()
{
	for(INT32 i = 0; i < STATIC_TEXT_MAX; i++)
	{
		vecStaticText[i]->SetText("");
	}

	if(m_pNewIcon != nullptr)
	{
		m_pNewIcon->EnableCtrl(false);
	}
	if(m_pHeadHunter != nullptr)
	{
		m_pHeadHunter->EnableCtrl(false);
	}
}

void UIFloatLobbyMapInfo_V10::_ClearAtUnload( i3UIScene * pScene )
{
	UIFloatingBase::_ClearAtUnload(pScene);

	vecStaticText.clear();
}
