#include "pch.h"
#include "UIPopupUserProfile.h"

#include "UIUtil.h"
#include "UIFloatUserList.h"
#include "UIFloatChat.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "UI_V15/UIPhaseReadyRoom_V15.h"

#include "../CommunityContext.h"
#include "../Record.h"
#include "../GameCharaWeaponContext.h"
#include "InvenList.h"

#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "ViewOtherUserItemContext.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "InviteContext.h"
#include "LobbyContext.h"
#include "BattleSlotContext.h"
#include "ChannelContext.h"
#include "GameContextUtil.h"

#include "GameGUI.h"
#include "ClanGameContext.h"

extern "C" {
	int UIPopupUserProfile_ClickButton( LuaState * L)		// �˾� �ϴ�(ã�ư���, �ʴ��ϱ�, Ŭ���ʴ�, ģ���߰�)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPopupUserProfile * pThis = static_cast<UIPopupUserProfile*>(GameUI::GetPopup( UPW_USERPROFILE));
		if( pThis != nullptr)
			pThis->ClickButton( idx);
		return 0;
	}

	int UIPopupUserProfile_ClickWhisper( LuaState * L)		// �ӼӸ� 
	{
		UIPopupUserProfile * pThis = static_cast<UIPopupUserProfile*>(GameUI::GetPopup( UPW_USERPROFILE));
		if( pThis != nullptr)
			pThis->ClickWhisper();
		return 0;
	}

	int UIPopupUserProfile_ClickDetail( LuaState * L)		// ������ ����
	{
		UIPopupUserProfile * pThis = static_cast<UIPopupUserProfile*>(GameUI::GetPopup( UPW_USERPROFILE));
		if( pThis != nullptr)
			pThis->ClickDetailInfo();
		return 0;
	}

	int UIPopupUserProfile_Close( LuaState * L)				// �ݱ�
	{
		UIPopupUserProfile * pThis = static_cast<UIPopupUserProfile*>(GameUI::GetPopup( UPW_USERPROFILE));
		if( pThis != nullptr)
			pThis->OnCloseButton();
		return 0;
	}

	int UIPopupUserProfile_ChangeHost( LuaState * L )
	{
		UIPopupUserProfile * pThis = static_cast<UIPopupUserProfile*>(GameUI::GetPopup( UPW_USERPROFILE));
		if( pThis != nullptr)
			pThis->ChangeHost();
		return 0;
	}

	int UIPopupUserProfile_ClickUserInfo( LuaState * L )
	{
		UIPopupUserProfile * pThis = static_cast<UIPopupUserProfile*>(GameUI::GetPopup( UPW_USERPROFILE));
		if( pThis != nullptr)
			pThis->ClickUserInfo();
		return 0;
	}

	int UIpopupUserProfile_ClickGMButton( LuaState * L )
	{
		UIPopupUserProfile * pThis = static_cast<UIPopupUserProfile*>(GameUI::GetPopup( UPW_USERPROFILE));
		if( pThis != nullptr)
			pThis->ClickGMButton();
		return 0;
	}
}

LuaFuncReg UIPopupUserProfile_Glue[] = {
	{"ClickButton",					UIPopupUserProfile_ClickButton		},
	{"ClickWhisper",				UIPopupUserProfile_ClickWhisper		},
	{"ClickDetail",					UIPopupUserProfile_ClickDetail		},
	{"ClickUserInfo",				UIPopupUserProfile_ClickUserInfo	},
	{"Close",						UIPopupUserProfile_Close			},
	{"ChangeHost",					UIPopupUserProfile_ChangeHost		},
	{"ClickGMButton",				UIpopupUserProfile_ClickGMButton	},
	{nullptr,							nullptr}
};

I3_CLASS_INSTANCE( UIPopupUserProfile);//, UIPopupBase);

UIPopupUserProfile::UIPopupUserProfile()
{
	m_UserTab = USERLIST_TAB_NONE;
	m_i32SelIdx = -1;

	for(INT32 idx = 0 ; idx < 3 ; idx++)
	{
		m_BtnState[idx] = UIP_BTN_STATE_NONE;
	}

	m_pClanMark = nullptr;
	m_nUTD = 0;

	fn_click_user_info = &UIPopupUserProfile::ClickUserInfo_in_none;
	m_bClanMercenary = false; 
}

UIPopupUserProfile::~UIPopupUserProfile()
{
	m_pClanMark = nullptr;
	m_nUTD = 0;
}

/*virtual*/ void UIPopupUserProfile::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	if (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
	{
		GameEventSender::i()->SetEvent(EVENT_GET_INVITE_LOBBY_USER);
	}

	m_pClanMark = (i3UIImageBoxEx *)pScene->FindChildByName("ImgBox_Clan");
}

/*virtual*/ void UIPopupUserProfile::_ClearAtUnload(i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload(pScene);
	m_pClanMark = nullptr;
}

void UIPopupUserProfile::_SetButtonState(void)
{
	_SetUserInfoButton();
	_SetCommunityButton();
	_SetDefaultButton();
}

// �ӼӸ�, ���� ������ ����, �� ���� ����
void UIPopupUserProfile::_SetUserInfoButton(void)
{
	bool isWisper = false; 
	bool isOtherView = false;
	bool isGM = false;

	// Ÿ ������ ���� �˾����� �ӼӸ�, ������ ���� Ȱ��ȭ
	if ( _IsUserState( TSU_TYPE_MYSELF ) == false )
	{
		isWisper = true;
		isOtherView = CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_VIEW_OTHER_USER_ITEM_INFO);
	}

	if ( UserInfoContext::i()->GetMyRank() == RANK_GAMEMASTER)
		isGM = true;

	LuaState * L = _CallLuaFunction("SetUserInfoButton");
	if(L != nullptr)
	{
		i3Lua::PushBoolean( L , isWisper);
		i3Lua::PushBoolean( L , isOtherView);
		i3Lua::PushBoolean( L , true);	// Detail Enable
		i3Lua::PushBoolean( L , isGM );
		_EndLuaFunction(L , 4);
	}
}

// Ŀ�´�Ƽ ���� ��ư(ģ�� �߰�, ����, ���󰡱�, �ʴ��ϱ�, Ŭ�� �ʴ�, ���� ��..) 
void UIPopupUserProfile::_SetCommunityButton(void)
{
	for(INT32 idx = 0 ; idx < 3 ; idx++)
	{
		m_BtnState[idx] = UIP_BTN_STATE_NONE;
	}

	INT32 nBtnCount = 0;	
	bool bActiveBtn = true;				// ã�ư��� / �ʴ��ϱ� ��ư Activate 

	if(_IsUserState(TSU_TYPE_MYSELF) == false && !m_bClanMercenary)
	{
		if(m_UserTab == USERLIST_TAB_FRIEND && _IsUserState(TSU_TYPE_HOLD))
		{
			m_BtnState[0] = UIP_BTN_STATE_ACCEPTFRIEND;	// ����
			m_BtnState[1] = UIP_BTN_STATE_REFUSEFRIEND;	// ����
			nBtnCount = 2;	
		}
		else
		{
			if (_IsUserState(TSU_TYPE_HOLD))
			{
				m_BtnState[0] = UIP_BTN_STATE_ACCEPTFRIEND;	// ����
			}
			else
			{
				if (_IsUserState(TSU_TYPE_NORMAL))
					m_BtnState[0] = UIP_BTN_STATE_ADDFRIEND;	// ģ�� �߰�
				else
					m_BtnState[0] = UIP_BTN_STATE_REMOVEFRIEND;	// ģ�� ����
			}

			if (_IsUserState(TSL_TARGET_CLAN_CHENNEL))
			{
				if (_IsUserState(TSL_TARGET_CLAN_MATCH))
					m_BtnState[1] = UIP_BTN_STATE_TRACKING;		// ã�ư���
				else
					m_BtnState[1] = UIP_BTN_STATE_INVITE;		// �ʴ��ϱ�

			}
			else if( _IsUserState( TSL_USER_LOBBY ) )
				m_BtnState[1] = UIP_BTN_STATE_TRACKING;		// ã�ư���
			else
				m_BtnState[1] = UIP_BTN_STATE_INVITE;		// �ʴ��ϱ�

			nBtnCount = 2;

			if( _IsUserState(TSL_TARGET_OFFLINE) )
			{
				// ��� ������ ���������� ���, �ʴ��ϱ� / ���󰡱� ��ư�� ��ǲ�� �����ϴ�.
				bActiveBtn = false;
			}		
			else if( m_BtnState[1] == UIP_BTN_STATE_TRACKING )
			{
				// ���󰡱� ��ư�� ���

				if(_IsUserState(TSL_TARGET_CLAN_CHENNEL))
				{
					if(_IsUserState(TSL_TARGET_CLAN_MATCH) == false)
					{
						bActiveBtn = false;
					}
				}
				else if(_IsUserState(TSL_TARGET_READY_ROOM) == false)
				{
					//������ ����뿡 ��� ��ǲ�� ����
					bActiveBtn = false;
				}
			}
			else
			{
				if(g_pFramework->GetClanContext()->InBattleTeam() && _IsUserState(TSC_TYPE_SAMECLAN) == false)
				{
					bActiveBtn = false;
				}
			}

			if( _IsUserState( TSC_ENABLE_INVITE ) && _IsUserState( TSC_TYPE_NONE ) )
			{
				I3ASSERT(nBtnCount <= 2);

				m_BtnState[nBtnCount] = UIP_BTN_STATE_INVITECLAN;
				nBtnCount++;
			}
		}
	}

	// [�뺴] Ŭ�� �ʴ� ������, �뺴, �� ���嵵 ������־�� ��
	if( m_bClanMercenary )
	{
		bool bMyMercState = ClanGameContext::i()->getMyMercenaryState();
		bool bInviteRoom = ClanGameContext::i()->isMatchTeamLeader(); 

		nBtnCount = 3;

		if (_IsUserState(TSU_TYPE_NORMAL))
				m_BtnState[0] = UIP_BTN_STATE_ADDFRIEND;
		else
		{
			if(_IsUserState(TSU_TYPE_HOLD))
				m_BtnState[0] = UIP_BTN_STATE_ADDFRIEND;
			else
				m_BtnState[0] = UIP_BTN_STATE_REMOVEFRIEND;
		}

		if(!bMyMercState && bInviteRoom)
		{
			m_BtnState[1] = UIP_BTN_STATE_INVITEMERC;
			m_BtnState[2] = UIP_BTN_STATE_NONE;
		}
		else
		{
			m_BtnState[1] = UIP_BTN_STATE_NONE;
			m_BtnState[2] = UIP_BTN_STATE_NONE;
		}

		bActiveBtn = true;
		for (int i = 0; i < 3; ++i)
		{
			if(m_BtnState[i] == UIP_BTN_STATE_NONE)
				nBtnCount--;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	LuaState * L = _CallLuaFunction( "SetCommunityButton");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, m_BtnState[0]);
		i3Lua::PushInteger( L, m_BtnState[1]);
		i3Lua::PushInteger( L, m_BtnState[2]);
		i3Lua::PushInteger( L , nBtnCount);
		i3Lua::PushBoolean( L , bActiveBtn == true ? true : false );
		_EndLuaFunction( L, 5);
	}
}

// �⺻ ��ư (�˾� �ݱ�, ���� �ѱ��, GM �ɼ�)
void UIPopupUserProfile::_SetDefaultButton(void)
{
	bool isChangeHost = false;		// ���� �ѱ�� ��ư

	if(_IsUserState(TSU_TYPE_MYSELF) == false)
	{
		// �����濡 �ִ� ���� / �ڽ��� �����϶� ���� ��ü��ư Ȱ��ȭ
		if( _IsHost() &&
			_IsUserState(TSL_USER_READY_ROOM) && _IsUserState(TSL_SAME_LOCATION) )
		{
			isChangeHost = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	LuaState * L = _CallLuaFunction( "SetDefaultButton");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L, isChangeHost);
		i3Lua::PushBoolean(L, UserInfoContext::i()->CheckAuthLevel(AUTH_LEVEL_GM) );
		_EndLuaFunction( L, 2);
	}
}

void UIPopupUserProfile::_ProcessClickButton( UIP_BTN_STATE state)
{
	const TTCHAR* pszNick = CCommunityContext::i()->GetUserSimpleInfo()->_UserInfo.m_nickName;

	i3::rc_wstring wstrNick = pszNick;

	if (m_bClanMercenary)
	{
		i3::generic_string_copy(wstrNick, ClanGameContext::i()->getClanMercenaryInfo(ClanGameContext::i()->GetSelectedMerID())->getNickName());
	}

	USERLIST_TAB eReleationTab = _FindRelationTab();
	
	switch (state)
	{
	case UIP_BTN_STATE_TRACKING:		GameUI::FindAwayUser(eReleationTab, m_i32SelIdx);	break;
	case UIP_BTN_STATE_INVITE:
	{
		if (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
		{
			InviteContext::i()->CheckInviteUserInfo(m_i32SelIdx);
			GameEventSender::i()->SetEvent(EVENT_INVITE_LOBBY_USER);
		}
		else
		{
			GameUI::InviteUser(eReleationTab, m_i32SelIdx);
		}

		break;
	}
	case UIP_BTN_STATE_ADDFRIEND:		GameUI::AddFriend(wstrNick);			break;
	case UIP_BTN_STATE_ACCEPTFRIEND:	GameUI::AcceptFriend(m_i32SelIdx);				break;
	case UIP_BTN_STATE_REFUSEFRIEND:	GameUI::RefuseFriend(m_i32SelIdx, MAKE_CALLBACK(CbDeleteFriend), this);		break;
	case UIP_BTN_STATE_REMOVEFRIEND:	GameUI::RemoveFriend(m_i32SelIdx);				break;
	case UIP_BTN_STATE_INVITECLAN: 	GameUI::InviteClan(wstrNick, MAKE_CALLBACK(CbInviteClan), this);	break;
	case UIP_BTN_STATE_INVITEMERC:
	{
		T_MerID mID = ClanGameContext::i()->GetSelectedMerID();
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_SEND_INVITE_MERCENARY, &mID);
	}
	break;
	case UIP_BTN_STATE_BLOCK:			break;
	default:
		I3PRINTLOG(I3LOG_FATAL, "not set button state function!!");
		break;
	}

	GetMainFrame()->TogglePopup(UPW_USERPROFILE);
}

void UIPopupUserProfile::_SetMerceInfo(void)
{
	const CLAN_MERCENARY_INFO * pInfo = ClanGameContext::i()->getClanMercenaryInfo( ClanGameContext::i()->GetSelectedMerID() );
	const CLAN_MERCENARY_RECORD* record = ClanGameContext::i()->getClanMercenaryRecord();

	i3::stack_wstring wstrNickAndRate;
	i3::stack_wstring wstrLocation;

	INT32 merce_rank = ClanGameContext::i()->GetMercIconIdx(pInfo);

	if(pInfo->_State == MER_STATE_NONE)
		return;

	const COLOR * pColor = GameGUI::GetColor(GCT_DEFAULT);

	if( pInfo->_ui8NickColor != 0)
	{
		pColor = GetColorCashItem(pInfo->_ui8NickColor);
	}

	INT32 KillRate = GameContextUtil::getKillRate(record->_i32KillCount, record->_i32DeathCount);
	i3::sprintf(wstrNickAndRate,  GAME_RCSTRING("STR_TBL_PROFILE_KILL_DEATH_RATE_MODE1"), KillRate);

	LuaState * L = _CallLuaFunction("SetUserInfo");

	if(L != nullptr)
	{
		i3Lua::PushInteger(L, (merce_rank+MERCENARY_RANK_STARTIDX));
		i3Lua::PushStringUTF16To8(L, pInfo->_wstrNickName);
		i3Lua::PushColor( L, pColor);
		i3Lua::PushStringUTF16To8(L, wstrNickAndRate);
		i3Lua::PushStringUTF16To8(L, wstrLocation);
		_EndLuaFunction(L , 8);
	}

}

void UIPopupUserProfile::_SetUserInfo( void)
{
	const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();

	// ������ ������ �����մϴ�.
	if(pInfo->_UserInfo.m_i64FindUID <= 0)
		return;

	i3::stack_wstring wstrNickAndRate;
	i3::stack_wstring wstrLocation;

	// �г��� �÷�
	const COLOR * pColor = GameGUI::GetColor(GCT_DEFAULT);
	if( pInfo->_UserInfo.m_nickColor != 0)
		pColor = GetColorCashItem( pInfo->_UserInfo.m_nickColor);

	m_wstrUserNickName = pInfo->_UserInfo.m_nickName;

	_SetNickAndKillDeathRate(wstrNickAndRate, pInfo->_UserInfo.m_killCount, pInfo->_UserInfo.m_deathCount);
	
	// Ŭ����ġ����ŷ �뺴 ���� �г��� , ų������ ����
	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) &&
		GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM )
	{
		// 13509 ��Ī�� Ŭ���� ����뿡�� �� ���� �̸� ���;� �մϴ�.
		if(ChannelContext::i()->GetBattleChannel() &&
			g_pFramework->GetClanContext()->InBattleTeam())
		{
			CLAN_PERSONAL_MATCH_RECORD* pRecord = ClanGameContext::i()->getMathRecord(m_i32SelSlotIdx);
			if(pRecord != nullptr)
				_SetNickAndKillDeathRate(wstrNickAndRate, pRecord->getKillCount(), pRecord->getDeathCount());
		}
	}
	
	
	// ���� ��ġ ����
	_SetUserLocation(wstrLocation, pInfo);

	// Ŭ����ũ ����
	_SetClanMark(pInfo->_UserInfo.m_clanMark);

	LuaState * L = _CallLuaFunction("SetUserInfo");

	if(L != nullptr)
	{
		if (pInfo->_UserInfo.m_AprilFoolRank != RANK_NONE)
			i3Lua::PushInteger(L, pInfo->_UserInfo.m_AprilFoolRank);
		else if(pInfo->_UserInfo.m_DisguiseRank != RANK_NONE)
			i3Lua::PushInteger(L, pInfo->_UserInfo.m_DisguiseRank);
		else
			i3Lua::PushInteger(L, pInfo->_UserInfo.m_rank);

		i3Lua::PushStringUTF16To8(L, m_wstrUserNickName);
		i3Lua::PushColor( L, pColor);
		i3Lua::PushStringUTF16To8(L, wstrNickAndRate);
		i3Lua::PushStringUTF16To8(L, wstrLocation);
		_EndLuaFunction(L , 8);
	}
}

void UIPopupUserProfile::_SetNickAndKillDeathRate(i3::stack_wstring& out, INT32 Kill, INT32 Death)
{
	i3::stack_wstring wstrKillDeathRate;
	INT32 KillRate = GameContextUtil::getKillRate(Kill, Death);

	i3::sprintf(out, GAME_RCSTRING("STR_TBL_PROFILE_KILL_DEATH_RATE_MODE1"), KillRate);

}

void UIPopupUserProfile::_SetUserLocation(i3::stack_wstring& out, const USER_SIMPLE_INFO * pInfo)
{
	// �¶��λ��°� �ƴ� ��� �������� ó��
	if( _IsUserState(TSL_TARGET_OFFLINE) )
	{
		out = GAME_RCSTRING("STR_POPUP_OFFLINE_STATE");	/*��������*/
		return;
	}
	else 	if (_IsUserState(TSL_TARGET_ONLINE))
	{
		out = GAME_RCSTRING("STR_POPUP_READY_STATE");	/*�����*/
		return;
	}

	

	// ����
	if(pInfo->_UserInfo.m_ServerIdx < INIT_UINT8)
	{
		if(pInfo->_UserInfo.m_ServerIdx == 0)
		{
			out = GAME_RCSTRING("STR_UI_SERVER_LIST");	/*�������*/
			return;
		}
		else
		{
			i3::sprintf(out, GAME_RCSTRING("STR_POPUP_WHAT_SERVER"), pInfo->_UserInfo.m_ServerIdx);
		}
	}

	// ä��

	if(pInfo->_UserInfo.m_ChannelNum < INIT_UINT8)
	{
		i3::stack_wstring wstrChannel;
		i3::generic_string_cat(out , L" ");
		i3::sprintf(wstrChannel, GAME_RCSTRING("STR_POPUP_WHAT_CHANNEL"), pInfo->_UserInfo.m_ChannelNum + 1);
		i3::generic_string_cat(out , wstrChannel);
	}

	// ���ȣ�� �κ� ���� ��� ������� ����
	if (pInfo->_UserInfo.m_State == FRIEND_STATE_ROOM)
	{
		if(pInfo->_UserInfo.m_RoomIdx > -1 && pInfo->_UserInfo.m_RoomIdx < INIT_0XFFF )
		{
			i3::stack_wstring wstrRoom;
			i3::generic_string_cat(out , L" ");
			i3::sprintf(wstrRoom, GAME_RCSTRING("STR_POPUP_WHAT_ROOM"), pInfo->_UserInfo.m_RoomIdx + 1);
			i3::generic_string_cat(out , wstrRoom);
		}
	}
}

void UIPopupUserProfile::_SetClanMark(UINT32 ui32ClanMark)
{
	GameUI::SetClanTexture(m_pClanMark, CLAN_MARK_SMALL, ui32ClanMark);
	m_pClanMark->SetShapeSize(30, 30);
}

USERLIST_TAB UIPopupUserProfile::_FindRelationTab()
{
	const USER_SIMPLE_INFO * pUserInfo = CCommunityContext::i()->GetUserSimpleInfo();

	if(m_UserTab == USERLIST_TAB_SEARCH || m_UserTab == USERLIST_TAB_CURRENT)
	{
		if(_IsUserState(TSC_TYPE_SAMECLAN))
		{
			m_i32SelIdx = CCommunityContext::i()->GetViewIndexOfUIDClanMember((INT64)pUserInfo->_UserInfo.m_i64FindUID);
			return USERLIST_TAB_CLAN;
		}
		else if(_IsUserState(TSU_TYPE_FRIEND))
		{
			m_i32SelIdx = CCommunityContext::i()->GetViewIndexOfUID((INT64)pUserInfo->_UserInfo.m_i64FindUID);
			return USERLIST_TAB_FRIEND;
		}
	}

	return m_UserTab;
}

void UIPopupUserProfile::_SetMercenaryRecord()
{
	const CLAN_MERCENARY_RECORD* record = ClanGameContext::i()->getClanMercenaryRecord();

	if (record != nullptr)
	{
			LuaState* L = _CallLuaFunction("SetMercernaryInfo");  //SetMercernaryInfo
			if ( L == nullptr) 		return;

			i3::rc_wstring strRecord, temp; 

			sprintf(temp, L"%d", record->_i32MatchCount);
			if(record->_i32MatchCount >= 1000)
			{
				sprintf(temp, L"%d,%03d", record->_i32MatchCount/1000, record->_i32MatchCount%1000);
				if(record->_i32MatchCount >= 1000000)
					sprintf(temp, L"%d,%03d,%03d", record->_i32MatchCount/1000000, (record->_i32MatchCount%1000000)/1000, record->_i32MatchCount%1000);
			}	
			i3::generic_string_cat(strRecord, temp);

			i3::generic_string_cat(strRecord, GAME_RCSTRING("STR_TBL_PROFILE_BASICINFO_BATTLE2"));
			i3::generic_string_cat(strRecord, L"  ");

			sprintf(temp, L"%d", record->_i32WinMatchCnt);
			if(record->_i32WinMatchCnt >= 1000)
			{
				sprintf(temp, L"%d,%03d", record->_i32WinMatchCnt/1000, record->_i32WinMatchCnt%1000);
				if(record->_i32WinMatchCnt >= 1000000)
					sprintf(temp, L"%d,%03d,%03d", record->_i32WinMatchCnt/1000000, (record->_i32WinMatchCnt%1000000)/1000, record->_i32WinMatchCnt%1000);
			}
			i3::generic_string_cat(strRecord, temp);

			i3::generic_string_cat(strRecord, GAME_RCSTRING("STR_TBL_PROFILE_BASICINFO_WIN2")); 

			i3Lua::PushStringUTF16To8(L, strRecord.c_str());
			_EndLuaFunction(L, 1);

	}

}

/*virtual*/ void UIPopupUserProfile::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// AddScene
	AddScene( "Scene/Popup/PBRe_PopUp_UserInfo.i3UIs", UIPopupUserProfile_Glue);
}

/*virtual*/ bool UIPopupUserProfile::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if (pArg1 != nullptr)
	{
		i3::rc_wstring wstrBlockReason = *reinterpret_cast<i3::rc_wstring*>(pArg1);
		m_wstrBlockReason = wstrBlockReason;
	}
	else
	{
		m_wstrBlockReason.clear();
	}

	// ���� ���õ� ���� �����;��մϴ�.
	UIFloatUserList * pFrame = static_cast<UIFloatUserList *>(GameUI::GetFloating( UFW_USERLIST));
	if(pFrame != nullptr)
	{
		m_UserTab = pFrame->getCurrentUserTab();
		m_i32SelIdx = pFrame->getSelectIdx();
	}

	fn_click_user_info = &UIPopupUserProfile::ClickUserInfo_in_lobby;

	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
	{
		fn_click_user_info = &UIPopupUserProfile::ClickUserInfo_in_room;

		UIPhaseReadyRoom* ready_room = static_cast<UIPhaseReadyRoom*>( GetMainFrame()->GetCurrentPhase() );
		m_i32SelSlotIdx = ready_room->getSelectedSlot();
	}

	if( ClanGameContext::i()->IsExistMercDetailInfo())
	{
		m_bClanMercenary = true;
		ClanGameContext::i()->ResetMercDetailInfo(); // reset.
	}
	else
		m_bClanMercenary = false;

	return true;
}

/*virtual*/ void UIPopupUserProfile::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();

	_ResetUserState();

	if(m_bClanMercenary)
	{
		_UpdateMerceState();
		_SetMerceInfo();
		_SetMercenaryRecord();
	}
	else
	{
		_UpdateUserState();
		_SetUserInfo();
	}

	_SetButtonState();


	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}


/*virtual*/ void UIPopupUserProfile::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent(event,Arg, UserData);

	if( event == EVENT_GET_USER_DETAIL_INFO)
	{
		if(EV_SUCCESSED(Arg))
		{
			CCommunityContext::i()->SetCommunityUserInfo(true);

			GetMainFrame()->ClosePopup(UPW_USERPROFILE);
			// TopMenu�� Myinfo == 4�Դϴ�.
			GetMainFrame()->GetTopMenu()->Clicked(4);
		}
		else
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_MYINFO_LOADFAIL"));
		}

		SetInputDisable(false);
	}
}
/*virtual*/ bool UIPopupUserProfile::OnCloseButton( void)
{
	GetMainFrame()->TogglePopup(UPW_USERPROFILE);
	return UIPopupBase::OnCloseButton();
}

void UIPopupUserProfile::UpdateUserInfo( void)
{
	//���� ���� �����.. ������ �˸���
}

void UIPopupUserProfile::ClickButton( INT32 idx)
{
	_ProcessClickButton(m_BtnState[idx]);
}

void UIPopupUserProfile::ClickWhisper( void)
{
	const TTCHAR* szNick = CCommunityContext::i()->GetUserSimpleInfo()->_UserInfo.m_nickName;

	i3::rc_wstring wstrNick = szNick;

	if(m_bClanMercenary )
	{
		i3::generic_string_copy (wstrNick, ClanGameContext::i()->getClanMercenaryInfo(ClanGameContext::i()->GetSelectedMerID())->_wstrNickName );
	}

	if(i3::generic_string_size(wstrNick) > 0)
	{
		if( !UserInfoContext::i()->IsMyNickName(wstrNick))
		{
			UIFloatChat * pChat = static_cast<UIFloatChat *>(GameUI::GetFloating( UFW_CHAT));
			if( pChat != nullptr)
				pChat->OnWhisper(wstrNick);
			GetMainFrame()->TogglePopup(UPW_USERPROFILE);
		}
	}
}

void UIPopupUserProfile::ClickDetailInfo( void)
{
	const USER_SIMPLE_INFO * pUserInfo = CCommunityContext::i()->GetUserSimpleInfo();

	i3::wliteral_range wstrNickName = pUserInfo->_UserInfo.m_nickName;

	if (m_bClanMercenary)
	{
		i3::generic_string_copy(wstrNickName, ClanGameContext::i()->getClanMercenaryInfo(ClanGameContext::i()->GetSelectedMerID())->getNickName());
	}

	bool is_other_user = UserInfoContext::i()->IsMyNickName( wstrNickName ) == false;

	CCommunityContext::i()->SetCommunityUserInfo( is_other_user ? true : false );
	GetMainFrame()->ClosePopup(UPW_USERPROFILE);
	// TopMenu�� Myinfo == 4�Դϴ�.
	GetMainFrame()->GetTopMenu()->Clicked(4);
}


void UIPopupUserProfile::ClickUserInfo_in_room( void)
{
	i3::rc_wstring wstr_selected_user_name = BattleSlotContext::i()->getNickForSlot(m_i32SelSlotIdx);
	
	ViewOtherUserItemContext::i()->GetData()->m_wstrOtherUserName = wstr_selected_user_name;

	INT32 ItemViewItemInfo = CASHITEM_ITEMID_VIEW_OTHER_USER_ITEM_INFO;
	
	T_ItemDBIdx i64UID = CInvenList::i()->FindWareDBIdx_ByItemID(ItemViewItemInfo);
//	UINT8 Size = (UINT8)strlen(selected_user_name); //strlen���� ���̸� ���ص� ����?		// TODO : ���׷� ��������.... ���� �������°� �������� ��Ȯ�� �ľ��ؾ��Ѵ�..(2014.07.16.����)
	UINT8 Size = sizeof(INT32);																// �ϴ� ������ ���� INT32�� ���ļ��Ѱܺ���..(m_i32SelSlotIdx)

	GameEventSender::i()->SetEvent(EVENT_VIEW_OTHER_USER_ITEM_INFO, &i64UID, &Size, &m_i32SelSlotIdx);
}

void UIPopupUserProfile::ClickUserInfo_in_lobby( void)
{
	UIFloatUserList * pFrame = static_cast<UIFloatUserList *>(GameUI::GetFloating( UFW_USERLIST));
	if( pFrame != nullptr)
	{
		i3::rc_wstring wstr_selected_user_name = pFrame->getUserInfo(m_i32SelIdx) ? pFrame->getUserInfo(m_i32SelIdx)->_wstrNick : i3::rc_wstring();

		ViewOtherUserItemContext::i()->GetData()->m_wstrOtherUserName = wstr_selected_user_name ;

		INT32 ItemViewItemInfo = CASHITEM_ITEMID_VIEW_OTHER_USER_ITEM_INFO;

		T_ItemDBIdx i64UID = CInvenList::i()->FindWareDBIdx_ByItemID(ItemViewItemInfo);

//		UINT8 Size = (UINT8)strlen(selected_user_name); //strlen���� ���̸� ���ص� ����?		// TODO : ���׷� ��������.... ���� �������°� �������� ��Ȯ�� �ľ��ؾ��Ѵ�..(2014.07.16.����)
		UINT8 Size = sizeof(INT32);																// �ϴ� ������ ���� INT32�� ���ļ��Ѱܺ���..(session)
		INT32 session = LobbyContext::i()->getUserSessionIdxByUserNickName(wstr_selected_user_name);
		if( session == -1 ) return;

		GameEventSender::i()->SetEvent(EVENT_VIEW_OTHER_USER_ITEM_INFO, &i64UID, &Size, &session);
	}
}

void UIPopupUserProfile::ClickUserInfo( void)
{
	(this->*fn_click_user_info)();
}

void UIPopupUserProfile::ClickGMButton( void)
{
	INT32 _UserIndex = LobbyContext::i()->getUserSessionIdxByUserNickName(m_wstrUserNickName);
	UINT8 _IndexType = 1;

	// Search�� �̿��� �˻��� �κ� ������ ������ ���ŵ��� �ʱ� ������
	// ���ŵ� InviteContext�� ���� ��Ͽ��� SessionIndex �˻縦 �� �� �� �ؾ���
/*	if ( _UserIndex != -1)
		_UserIndex = InviteContext::i()->GetUserSessionIdxByUserNickName(m_strUserNickName);*/

	// SessionIndex�� ������ ������ �κ� ����
	if ( _UserIndex != -1)
	{
		_IndexType = GMCHAT_INDEX_LOBBY;
		I3TRACE("������ �κ� �ֽ��ϴ�.\n");
	}
	// SessionIndex�� -1�̸� �ش� ������ �κ� ����
	else
	{
		// ������ �ִ� ���� �ε��� ��ȣ
		const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();
		_UserIndex = pInfo->_UserInfo.m_RoomIdx + 1;
		_IndexType = GMCHAT_INDEX_ROOM;

		I3TRACE("������ �濡 �ֽ��ϴ�.\n");

		// ������ GM�� ���� �濡 �ִ��� Ȯ��
		if( g_pFramework->GetUIMainframe()->GetCurrentPhaseType() != UIPHASE_LOBBY)
		{

			I3TRACE("GM�� �濡 �ֽ��ϴ�.\n");
			if ( _UserIndex == MyRoomInfoContext::i()->getMyRoomIndex())
			{
				_UserIndex = BattleSlotContext::i()->GetUserIndex(m_wstrUserNickName);
				_IndexType = GMCHAT_INDEX_ROOMSLOT;

				I3TRACE("GM�� ������ ���� �濡 �ֽ��ϴ�.\n");
			}
			else
			{
				if (InviteContext::i()->GetUserSessionIdxByUserNickName(m_wstrUserNickName) != -1)
				{
					I3TRACE("������ �濡 �ֽ��ϴ�.\n");
					
					_IndexType = GMCHAT_INDEX_LOBBY;
					_UserIndex = InviteContext::i()->GetUserSessionIdxByUserNickName(m_wstrUserNickName);
				}
				else
				{
					I3TRACE("GM�� ������ �ٸ� �濡 �ֽ��ϴ�.\n");
				}
			}
		}
		else
		{
			I3TRACE("GM�� �κ� �ֽ��ϴ�.\n");
		}
	}


	//const USER_SIMPLE_INFO * Info = CCommunityContext::i()->GetUserSimpleInfo();

	I3TRACE("IndexType : %d\n", _IndexType);
	I3TRACE("UserIndex : %d\n", _UserIndex);
	
	GameEventSender::i()->SetEvent(EVENT_GM_AUTHORITY_REQ, &_IndexType, &_UserIndex, m_wstrUserNickName.c_str() );
}

void UIPopupUserProfile::AddFriend( void)
{
	if (CCommunityContext::i()->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_NO_MORE_FRIEND"));		//	"���̻� ģ���� �߰��� �� �����ϴ�."
		return;
	}

	const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();

	if( pInfo->_UserInfo.m_i64FindUID <= 0)		
		return;
	
	i3::rc_wstring wstrNick = pInfo->_UserInfo.m_nickName;

	CCommunityContext::i()->InsertFriend( wstrNick);
}

void UIPopupUserProfile::DoInviteClan( void)
{
//	const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();


/*
	i3::rc_wstring wstrNickName = pInfo->_UserInfo.m_nickName;
*/

	g_pFramework->GetClanContext()->SendClanInvite();
}

void UIPopupUserProfile::ChangeHost( void)
{
	// Ready Room ������ �����մϴ�.
	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
	{
		UIPhaseReadyRoom_V15 * pPhase = i3::same_cast<UIPhaseReadyRoom_V15*>(GetMainFrame()->GetCurrentPhase());
		I3ASSERT(pPhase != nullptr);

		INT32 i32SelectedSlot = pPhase->getSelectedSlot();
		GameEventSender::i()->SetEvent( EVENT_TRANSFER_HOST, &i32SelectedSlot); // ���� �ѱ��
	}

	GetMainFrame()->TogglePopup(UPW_USERPROFILE);
}

/*static*/ void	UIPopupUserProfile::CbInviteClan(void* pThis, INT32 nParam)
{
	if (MBR_OK == nParam)
	{
		((UIPopupUserProfile*)pThis)->DoInviteClan();
	}
}

void UIPopupUserProfile::CbDeleteFriend( void* pThis,INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		UIPopupUserProfile* pFrame = (UIPopupUserProfile*)pThis;

		if (0 > pFrame->m_i32SelIdx || false == CCommunityContext::i()->DeleteViewFriend(pFrame->m_i32SelIdx))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_DELETE"));/*������ ģ���� �����ϼž��մϴ�.*/
		}
		else
		{
			pFrame->m_i32SelIdx = -1;
			CCommunityContext::i()->SetCommunityUserInfo(false);
		}
	}
}

bool UIPopupUserProfile::_IsUserState( UINT32 _type )
{
 	if(m_nUTD&_type)
		return true;
	return false;
}

void UIPopupUserProfile::_AddUserState( UINT32 _type )
{
	m_nUTD |= _type;
}

void UIPopupUserProfile::_UpdateMerceState()
{
	const CLAN_MERCENARY_INFO * pInfo = ClanGameContext::i()->getClanMercenaryInfo( ClanGameContext::i()->GetSelectedMerID() );

	i3::stack_wstring wstrNickName;
	i3::generic_string_copy(wstrNickName, pInfo->_wstrNickName);

	if( UserInfoContext::i()->IsMyNickName( wstrNickName ) )
	{
		_AddUserState( TSU_TYPE_MYSELF );
	}
	// ģ���̴�.
	else if(GameUI::IsCommunityFriend(wstrNickName))
	{
		_AddUserState( TSU_TYPE_FRIEND );
		const FRIEND_NODE * pNode = CCommunityContext::i()->GetViewFriendByNick(wstrNickName);
		if( pNode )
		{
			// ģ�� ���� ������� ���
			if(GET_FRIEND_STATE(pNode->_state) == FRIEND_STATE_HOLD)
				_AddUserState(TSU_TYPE_HOLD);
		}
	}
	// ģ���� �ƴϴ�.
	else
	{
		_AddUserState( TSU_TYPE_NORMAL );
	}

	// ������ ��쿡�� �ʴ� ����
	if( ClanGameContext::i()->isMatchTeamLeader() )
		_AddUserState(TSC_ENABLE_INVITE);
	
}

void UIPopupUserProfile::_UpdateUserState()
{
	const USER_SIMPLE_INFO * pUserInfo = CCommunityContext::i()->GetUserSimpleInfo();

	// �� �ڽ��̴�.
	i3::wliteral_range wstrNickName = pUserInfo->_UserInfo.m_nickName;

	INT64 uid = static_cast<INT64>(pUserInfo->_UserInfo.m_i64FindUID);

	if( UserInfoContext::i()->IsMyNickName( wstrNickName ) )
	{
		_AddUserState( TSU_TYPE_MYSELF );
	}
	// ģ���̴�.
	else if( GameUI::IsCommunityFriend(uid) )
	{
		_AddUserState( TSU_TYPE_FRIEND );
		const FRIEND_NODE * pNode = CCommunityContext::i()->GetViewFriendByUID(uid);
		if( pNode )
		{
			// ģ�� ���� ������� ���
			if(GET_FRIEND_STATE(pNode->_state) == FRIEND_STATE_HOLD)
				_AddUserState(TSU_TYPE_HOLD);
		}
	}
	// ģ���� �ƴϴ�.
	else
	{
		_AddUserState( TSU_TYPE_NORMAL );
	}

	// Ŭ�� �ʴ� ����
	if(g_pFramework->GetClanContext()->IsPossibleClanInvite())
		_AddUserState(TSC_ENABLE_INVITE);

	// ���õ� ������ Ŭ�� ���� �������� Ȯ��
	if(pUserInfo->_UserInfo.m_clanMark != CLAN_MARK_FLAG_NONE && 
		i3::generic_string_size(pUserInfo->_UserInfo.m_strClanName) > 0)
	{
		USER_INFO_BASIC MyInfo;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyInfo);
		// ���õ� ������ ���� Ŭ������ Ȯ��
		if(i3::generic_is_equal(pUserInfo->_UserInfo.m_strClanName, MyInfo.m_strClanName) )
			_AddUserState(TSC_TYPE_SAMECLAN);
		else
			_AddUserState(TSC_TYPE_OTHERCLAN);
	}
	else
		_AddUserState(TSC_TYPE_NONE);


	// ����ġ
	UIPHASE  CurPhaseType = GetMainFrame()->GetCurrentPhaseType();	
	switch( CurPhaseType )
	{
	case UIPHASE_LOBBY :	
		_AddUserState(TSL_USER_LOBBY); break;
	case UIPHASE_CLANLOBBY: 
		{
			if( g_pFramework->GetClanContext()->InBattleTeam())
				_AddUserState(TSL_USER_READY_ROOM);
			else
				_AddUserState(TSL_USER_LOBBY);
		}
		break;
	case UIPHASE_READY_ROOM :	
		_AddUserState(TSL_USER_READY_ROOM); break;
	}

	// ��� ��ġ
	if(pUserInfo->_UserInfo.m_State == FRIEND_STATE_ROOM)
	{
		_AddUserState( TSL_TARGET_READY_ROOM );
		ROOM_INFO_BASIC MyRoomInfo;
		MyRoomInfoContext::i()->getMyRoomInfo(&MyRoomInfo);

		INT32 i32MyServerIdx = g_pServerInfo->GetCurGameServerIndex() + 1; // ServerInfo �� ���� ServerIdx�� 0���� ���۵ǹǷ� + 1
		INT32 i32MyChannelIdx = g_pServerInfo->GetCurChannelNumber();

		// ���� / ä�� / ���ȣ�� ��ġ�ؾ� ���� ��ġ
		if(i32MyServerIdx == pUserInfo->_UserInfo.m_ServerIdx &&
			i32MyChannelIdx == pUserInfo->_UserInfo.m_ChannelNum && 
			MyRoomInfo._Idx == pUserInfo->_UserInfo.m_RoomIdx )
		{
			_AddUserState(TSL_SAME_LOCATION );
		}
	}
	else
	{
		INT32 i32TarServer = pUserInfo->_UserInfo.m_ServerIdx;
		INT32 i32TarChannel = pUserInfo->_UserInfo.m_ChannelNum;

		// 0���� ����Ʈ���̼��� 1���� ���ڼ����̱� ������ 1�ٿ��ݴϴ�
		ChannelType ui8TargetChannelType = ChannelContext::i()->GetChannelType(i32TarServer-1, i32TarChannel);

		if(ui8TargetChannelType == CHANNEL_TYPE_CLAN)
		{
			_AddUserState(TSL_TARGET_CLAN_CHENNEL);

			if( _IsUserState(TSC_TYPE_SAMECLAN) )
			{
				INT32 i32Memberidx = CCommunityContext::i()->GetViewIndexOfUIDClanMember(pUserInfo->_UserInfo.m_i64FindUID);
				const CLAN_MEMBER_NODE * pNode = CCommunityContext::i()->GetViewClanMember(i32Memberidx);
				
				if(pNode != nullptr)
				{
					if(GET_FRIEND_MATCH(pNode->_state) != MATCH_TEAM_INVALID_IDX)
						_AddUserState(TSL_TARGET_CLAN_MATCH);
				}
			}

		}
		else if(pUserInfo->_UserInfo.m_State == FRIEND_STATE_ONLINE )
		{
			_AddUserState(TSL_TARGET_ONLINE);
		}
		else if (pUserInfo->_UserInfo.m_State == FRIEND_STATE_LOBBY)
		{
			_AddUserState(TSL_TARGET_LOBBY);
		}
		else
		{
			_AddUserState(TSL_TARGET_OFFLINE);
		}
	}
	// ����ư..
	if( LocaleValue::GetBOOL("EnableBlock") )
		_AddUserState( TSB_STATE_ENABLE );
}

void UIPopupUserProfile::_ResetUserState()
{
	m_nUTD = 0;
}
