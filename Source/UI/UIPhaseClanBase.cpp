#include "pch.h"
#include "UIPhaseClanBase.h"

#include "UIMainFrame.h"
#include "UIUtil.h"
#include "UIFloatMiniProfile.h"
#include "UIMainFrame_V11.h"
#include "UI_V11/UITopMenu_V11.h"

#include "../CommunityContext.h"
#include "ClanGameContext.h"
#include "UserInfoContext.h"
#include "ChannelContext.h"
#include "UIFloatMercenaryList.h"
#include "UIPhaseClanLobby.h"

#include "i3Base/string/ext/mb_to_utf16.h"


void cbSuccessDissolveClan(void* pThis, INT32 nParam)
{
	UIMainFrame * pMainFrame = (UIMainFrame*)pThis;
	I3ASSERT(pMainFrame != nullptr);

	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32Clanidx = 0;
	MyUserInfoBasic.m_ui32ClanState = CLAN_MEMBER_LEVEL_UNKNOWN;
	MyUserInfoBasic.m_ui32ClanMark = CLAN_MARK_DEFAULT;
	i3mem::FillZero(MyUserInfoBasic.m_strClanName, sizeof(MyUserInfoBasic.m_strClanName));
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	CCommunityContext::i()->ClearClanMember();

	//Ŭ��ä�ο��� Ż���ϰ� �ٸ� ä�η� �����ϸ� Ŭ��â���� ������ ���� ����
	if(!ChannelContext::i()->GetBattleChannel())
		pMainFrame->ChangeClanPopup();

	// Ŭ�� ��ü�� ����, ��ü ���� �޼��� �� ���� ������â�� ���ŵȴ�.
	{
		UIFloatMiniProfile * pFrame = static_cast<UIFloatMiniProfile *>(GameUI::GetFloating(UFW_MINIPROFILE));
		if (pFrame != nullptr)
			pFrame->ProcessGameEvent(EVENT_CLAN_DISSOLUTION, nParam, i3::user_data());		// �ӽ÷� �̷��� ó���Ѵ�..
	}

	// Ŭ��ä�ο� �����ϴ� ���� Ż���ϴ� ��쿡 ���� ó��
	// ������ ����&ä�� ������� ���ư��� �ϱ� ������ ������ ���Ǹ� ���մϴ�.

	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		if (g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_CLAN_MEMBER) == true)		// ���� �Ǿ������� �ұ��ϰ� Ŭ������� â�� ���ִٸ� Closeó��
		{
			g_pFramework->GetUIMainframe()->ClosePopup(UPW_CLAN_MEMBER);
			g_pFramework->CloseGameMessageBox();
		}

		if (ChannelContext::i()->GetBattleChannel())
			GameEventSender::i()->SetEventOnce(EVENT_LEAVE_LOBBY);
	}
	else
	{
		if(ChannelContext::i()->GetBattleChannel())
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_LEAVE_TEMP);
	}


}

void cbSuccessSecessionClan(void* pThis, INT32 nParam)
{
	UIMainFrame * pMainFrame = (UIMainFrame*) pThis;
	I3ASSERT(pMainFrame != nullptr);

	if( nParam == MBR_OK)
	{
		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		MyUserInfoBasic.m_ui32Clanidx		= 0;
		MyUserInfoBasic.m_ui32ClanState		= CLAN_MEMBER_LEVEL_UNKNOWN;
		MyUserInfoBasic.m_ui32ClanMark		= CLAN_MARK_DEFAULT;
		i3mem::FillZero(MyUserInfoBasic.m_strClanName, sizeof(MyUserInfoBasic.m_strClanName));
		UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

		CCommunityContext::i()->ClearClanMember();
		pMainFrame->ChangeClanPopup();
	}

}

ClanProcessGameEvent::ClanProcessGameEvent()
{
	RegisterFunc( EVENT_CLAN_JOINCHECK_AUTHORITY,			&ClanProcessGameEvent::ReceiveGameEvent_JoinCheck_Authority);
	RegisterFunc( EVENT_CLAN_DISSOLUTION,					&ClanProcessGameEvent::ReceiveGameEvent_Dissolution);
	RegisterFunc( EVENT_CLAN_SECESSION,						&ClanProcessGameEvent::ReceiveGameEvent_Secession);
	RegisterFunc( EVENT_CLAN_COMMISSIONMASTER,				&ClanProcessGameEvent::ReceiveGameEvent_CommisionMaster);
	RegisterFunc( EVENT_CLAN_CANCEL_REQUEST,				&ClanProcessGameEvent::ReceiveGameEvent_Request_Cancel);
	RegisterFunc( EVENT_CLAN_INVITE_ACCEPT_N,				&ClanProcessGameEvent::ReceiveGameEvent_Invite_Accept);
	RegisterFunc( EVENT_M_CLAN_DEPROTATION,					&ClanProcessGameEvent::ReceiveGameEvent_Deprotation);

}

void ClanProcessGameEvent::RegisterFunc( GAME_EVENT evt, ClanEventFunc f)
{
	m_eventMap.insert(i3::unordered_map< GAME_EVENT, ClanEventFunc >::value_type( evt, f));
}

/*virtual*/ void ClanProcessGameEvent::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	i3::unordered_map< GAME_EVENT, ClanEventFunc >::iterator it;
	it = m_eventMap.find( evt);

	if ( it != m_eventMap.end() )
	{
		ClanEventFunc & mf = it->second;
		(this->*mf)( g_pFramework->GetUIMainframe(), arg);
	}
}

void ClanProcessGameEvent::ReceiveGameEvent_JoinCheck_Authority( UIMainFrame* pFrame, INT32 arg)
{
	if( EV_SUCCESSED( arg ) )
	{
		INT32 nClanIdx = g_pFramework->GetClanContext()->GetJoinClanIdx();

		//	�����κ��� Ŭ�� ���� ������ �����Ǿ Ŭ�� ���� ��û���� ���ϴ�.
		// 2011-06-29 ������ : ���� UPW_CLAN_CREATE��  ���յ� �˾�â �и��� ���� ����
		// GetMainFrame()->TogglePopup( UPW_CLAN_CREATE, &nClanIdx);
		// 2011-06-29 ������ : ����Ȯ��â���� ����
		pFrame->OpenPopup( UPW_CLAN_JOIN, &nClanIdx);
	}
	else
	{
		switch( arg )
		{
		case EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL:
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_CLAN_JOIN_ABORTED_BY_AGE"),
				&GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST"));/*���� ��û ����*//*�ش� Ŭ���� ������ �� �ִ� ���ɰ� ���� �ʾ� Ŭ�� ���� ��û�� ��� �Ǿ����ϴ�.*/
			break;
		case EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL:
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_CLAN_JOIN_ABORTED_BY_GRADE"),
				&GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST"));/*���� ��û ����*//*�ش� Ŭ���� ������ �� �ִ� ��ް� ���� �ʾ� Ŭ�� ���� ��û�� ��� �Ǿ����ϴ�.*/
			break;
		default:
			{
				i3::rc_wstring wstrTemp;
				i3::sprintf( wstrTemp, GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST_BY_SYSTEM_PROBLEM"), arg );/*\n���� ��û�� �����Ͽ����ϴ�.\n �˼��մϴ�. �����ڿ��� �����Ͽ� �ֽʽÿ�.\n [Errcode : 0x%08x]*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_JOIN_REQUEST"));/*���� ��û ����*/
			}
			break;
		}
	}
}

void ClanProcessGameEvent::ReceiveGameEvent_Deprotation(UIMainFrame* pFrame, INT32 arg)
{
	if (EV_SUCCESSED(arg))
	{
		// Ŭ��ä�ο��� ���� ó���� UIPhaseBase�� ReceiveGameEvent_Clan_Deprotation()���� ó����
		if (ChannelContext::i()->GetBattleChannel())
			return;
		
		CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();
		i3::rc_wstring wstr_clanname;
		i3::sprintf(wstr_clanname, GAME_RCSTRING("STR_TBL_GUI_MESSAGER_CLAN_DEPORTATION"), pClanInfo->m_name);
		i3mem::FillZero(pClanInfo, sizeof(CLAN_DETAIL_INFO));

		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_clanname,	nullptr, MAKE_CALLBACK(cbSuccessDissolveClan), pFrame);
	}
}

void ClanProcessGameEvent::ReceiveGameEvent_Dissolution( UIMainFrame* pFrame, INT32 arg)
{
	if ( EV_SUCCESSED(arg) )
	{
		// Ŭ��ä�ο��� ����/Ż��/Ŭ����ü �� �߻��ϸ� Ŭ�� ���� update���� ���߰� ������ �κ񿡼� �������� �մϴ�
		if (ChannelContext::i()->GetBattleChannel())
		{
			if (ClanGameContext::i()->IsForceLeaveChannel())
				return;

			// ������ Ŭ��ä�ο��� ���������� ��. ����ä�μ��� ���������� �ٽ� �κ�� ������ �� �ʱ�ȭ�˴ϴ�.
			ClanGameContext::i()->setForceLeaveChannel(true);

			UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
			if (UIPHASE_READY_ROOM == pUIFrame->GetCurrentPhaseType())
			{
				GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
			}

			UIFloatMercenaryList*  pList = static_cast<UIFloatMercenaryList*>(GameUI::GetFloating(UFW_MERCENARY));
			pList->SetLeaveClanChannel(true);

			((UIPhaseClanLobby*)GameUI::GetPhase(UIPHASE_CLANLOBBY))->SetLeaveTeamFlag();
		}

		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_DISSOLUTION_SUCCESS"), 
			&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_DISSOLUTION_SUCCESS"), 
			MAKE_CALLBACK(cbSuccessDissolveClan) , pFrame);
	}
}

void ClanProcessGameEvent::ReceiveGameEvent_Secession( UIMainFrame* pFrame, INT32 arg)
{
	//	Ŭ�� ȸ�� Ż�� �����ϸ� �ٷ� Ŭ�� �̰����� �������� �Ѿ�ϴ�.
	if( EV_SUCCESSED(arg))
	{
		CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();
		i3mem::FillZero(pClanInfo,	sizeof(CLAN_DETAIL_INFO));

		// Ŭ��ä�ο��� ����/Ż��/Ŭ����ü �� �߻��ϸ� Ŭ�� ���� update���� ���߰� ������ �κ񿡼� �������� �մϴ�
		if (ChannelContext::i()->GetBattleChannel())
		{
			if (ClanGameContext::i()->IsForceLeaveChannel())
				return;

			// ������ Ŭ��ä�ο��� ���������� ��. ����ä�μ��� ���������� �ٽ� �κ�� ������ �� �ʱ�ȭ�˴ϴ�.
			ClanGameContext::i()->setForceLeaveChannel(true);

			UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
			if (UIPHASE_READY_ROOM == pUIFrame->GetCurrentPhaseType())
			{
				GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
			}

			UIFloatMercenaryList*  pList = static_cast<UIFloatMercenaryList*>(GameUI::GetFloating(UFW_MERCENARY));
			pList->SetLeaveClanChannel(true);

			((UIPhaseClanLobby*)GameUI::GetPhase(UIPHASE_CLANLOBBY))->SetLeaveTeamFlag();
		}

		//	"Ŭ�� Ż�� ���������� ó���Ǿ����ϴ�."
		//	"Ŭ�� Ż�� �Ϸ�"
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SECESSION_SUCCESS"),
				&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SECESSION_SUCCESS"),
				MAKE_CALLBACK(cbSuccessDissolveClan), pFrame);
		}
	}
}


void ClanProcessGameEvent::ReceiveGameEvent_CommisionMaster( UIMainFrame* pFrame, INT32 arg)
{
	//	������������ �����ϸ� �ٷ� Ŭ�� �̰����� �������� �Ѿ�ϴ�.
	if( EV_SUCCESSED(arg))
	{
		pFrame->ChangeClanPopup();
	}
}

void ClanProcessGameEvent::ReceiveGameEvent_Request_Cancel( UIMainFrame* pFrame, INT32 arg)
{
	if( EV_SUCCESSED(arg))
	{
		USER_INFO_BASIC		MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		MyUserInfoBasic.m_ui32Clanidx = 0;
		UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
	}
}

void ClanProcessGameEvent::ReceiveGameEvent_Invite_Accept( UIMainFrame* pFrame, INT32 arg)
{
	switch( arg ) 
	{
	case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
		{
			//	"\nŬ�� ���� ó���� �����߽��ϴ�.\nŬ���� ã���� �����ϴ�."
			//	Ŭ�� ���� ���� ����
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_FINDING"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));	
		}
		break;
	case EVENT_ERROR_CLAN_NOMEMBER:
		{
			//	"\nŬ�� ���� ó���� �����߽��ϴ�.\nŬ������ �ƴմϴ�."
			//	Ŭ�� ���� ���� ����
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
		}
		break;
	case EVENT_ERROR_CLAN_LEVEL_NOMASTER:
		{
			//	"\nŬ�� ���� ó���� �����߽��ϴ�.\nŬ�� �����Ͱ� �ƴմϴ�."
			//	Ŭ�� ���� ���� ����
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_MASTER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
		}
		break;
	case EVENT_ERROR_CLAN_MEMBER:
		{
			//	"\nŬ�� ���� ó���� �����߽��ϴ�.\n�̹� Ŭ���� ���ԵǾ��ֽ��ϴ�."
			//	Ŭ�� ���� ���� ����
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ALREADY_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
		}
		break;
	case EVENT_ERROR_CLAN_MAXMEMBER:
		{
			//	"\nŬ�� ���� ó���� �����߽��ϴ�.\n���̻� ���� �� �� �����ϴ�."
			//	Ŭ�� ���� ���� ����
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_OVER_MEMBER"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
		}
		break;
	case EVENT_ERROR_CLAN_SEARCH_REQUESTIDX:
		{
			//	"\n���� ��û �����Ͽ����ϴ�.\n�̹� �ٸ� Ŭ���� ���ԵǾ� �ְų� ���� ��û�� �����Դϴ�."
			//	Ŭ�� ���� ���� ����
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ALREADY_SUBSCRIPTION"), &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
		}
		break;
	default:
		{
			i3::rc_wstring wstrTemp;
			//	""\nŬ�� ���� ó���� �����߽��ϴ�.\n �˼��մϴ�. �����ڿ��� �����Ͽ� �ֽʽÿ�."
			i3::sprintf( wstrTemp, L"%s\n [Errcode : 0x%08x]", 
				GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ASK_FOR_INFO"), arg );

			GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));	//	Ŭ�� ���� ���� ����
		}
		break;
	}
}

//I3_ABSTRACT_CLASS_INSTANCE( UIPhaseClanBase, UIPopupBase);
I3_CLASS_INSTANCE( UIPhaseClanBase);


UIPhaseClanBase::UIPhaseClanBase()
{
	
}

UIPhaseClanBase::~UIPhaseClanBase()
{
}


void UIPhaseClanBase::_SetClanExpRate(i3::wstring& wstrOut, INT32 & i32OutRate , UINT32 ui32Exp)
{
	if (ui32Exp > DF_CLAN_RANK_L5)
		ui32Exp = DF_CLAN_RANK_L5;

	UINT32 ui32CurRankExp = GetClanBeforeRankExp(ui32Exp);
	UINT32 ui32NextRankExp = GetClanNextRankExp(ui32Exp);

	UINT32 ui32CurExp = ui32Exp - ui32CurRankExp;			
	UINT32 ui32NeedExp = ui32NextRankExp - ui32CurRankExp;

	if( ui32NeedExp == 0)
		ui32NeedExp = 1;
	i32OutRate = (ui32CurExp * 100) / ui32NeedExp;
	i32OutRate = MINMAX(0 , i32OutRate , 100);

	i3::sprintf(wstrOut, L"%d%%", i32OutRate);
}

void UIPhaseClanBase::_SetClanTotalRecord(i3::wstring& OutRecord, i3::wstring& OutRate, CLAN_DETAIL_INFO * pInfo)
{
	i3::sprintf(OutRecord, L"%d %s , %d %s , %d %s", 
		pInfo->m_match,  GAME_RCSTRING("STBL_IDX_CLAN_MATCH"), 
		pInfo->m_win,	GAME_RCSTRING("STBL_IDX_CLAN_WIN"),
		pInfo->m_lose,	GAME_RCSTRING("STBL_IDX_CLAN_LOSE"));


	REAL32 rRecordRate = 0.0f;
	if(pInfo->m_match > 0)
	{
		rRecordRate = ((REAL32)pInfo->m_win / (REAL32)pInfo->m_match) * 100.0f;
	}

	i3::sprintf( OutRate, L"%.2f%%", rRecordRate);
}

void UIPhaseClanBase::_SetClanSeasonRecord(const CLAN_RECORD & info, i3::rc_wstring & out_match, i3::rc_wstring & out_win, i3::rc_wstring & out_lose)
{
	i3::sprintf( out_match,	L"%d%s", info.m_i32MatchCount,	GAME_RCSTRING("STBL_IDX_BATTLE_COUNT"));
	i3::sprintf( out_win,	L"%d%s", info.m_i32WinMatch,	GAME_RCSTRING("STBL_IDX_BATTLE_WIN_COUNT"));
	i3::sprintf( out_lose,	L"%d%s", info.m_i32LoseMatch,	GAME_RCSTRING("STBL_IDX_BATTLE_LOSE_COUNT"));
}

void UIPhaseClanBase::_SetClanMark(i3UIImageBoxEx * pImgBox, UINT32 ui32ClanMark)
{
	GameUI::SetClanTexture(pImgBox, CLAN_MARK_MEDIUM, ui32ClanMark);

	// ���⼭ ���Ǵ� Ŭ����ũ ������� 68 x 68
	if( g_pEnvSet->m_nUIVersion == UFV_1_5 )
	{
		pImgBox->SetShapeSize(68, 68);
	}
	else
	{
		pImgBox->SetShapeSize(60, 60);
	}
}

// Ŭ�� ���� or ���� ��û�� ������ �ִ��� üũ (true = ����, flase = �Ұ���)
bool UIPhaseClanBase::_CheckClanRequsetEnable(void)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	if(( MyUserInfoBasic.m_ui32Clanidx != 0) && (MyUserInfoBasic.m_ui32ClanState == 0))
	{
		//	"�̹� Ŭ�������� ��û�� �����Դϴ�.\n���� ��û�� ����Ͻðڽ��ϱ�?"
		//	"���� ��û ���"
		GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANCEL_SUBSCRIPTION_Q"), 
			&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_CANCEL_SUBSCRIPTION"), MAKE_CALLBACK(cbCancelRequest), this);
		return false;
	}

	return true;
}

bool UIPhaseClanBase::OnExitStart()
{
	if( UIPopupBase::OnExitStart() == false)
		return false;

	return true;
}


/*virtual*/ void UIPhaseClanBase::OnExitEnd(void)
{
	if( g_pFramework->GetClanContext()->IsEnter() == false )
	{
		UISUBPHASE SubPhase = GetMainFrame()->GetCurrentSubPhaseType();
		GetMainFrame()->EventEnterSubPhase( SubPhase );
	}

	UIPopupBase::OnExitEnd();
}

/*virtual*/ void UIPhaseClanBase::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	// Clan Context
	g_pFramework->GetClanContext()->OnGameEvent( evt, arg, UserData);

	ClanProcessGameEvent::ProcessGameEvent( evt, arg, UserData);

	UIPopupBase::ProcessGameEvent( evt, arg, UserData);
}

bool UIPhaseClanBase::OnKey_Escape()
{
	Leave();

	
	if ( g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		UIMainFrame_V11* pParent = (UIMainFrame_V11*)g_pFramework->GetUIMainframe();

		I3ASSERT(pParent != nullptr);

		pParent->GetTopMenu_V11()->ClearTabSelection();
	}

	return true;
}

void UIPhaseClanBase::OnEscapeKey(void)
{
	Leave();
}

void UIPhaseClanBase::OnGotoBackPhase( void)
{
	Leave();
}

//--------------------------------------------------------------------------//
//--------------------------- CallBack Function ----------------------------// 
//--------------------------------------------------------------------------//
void UIPhaseClanBase::cbCancelRequest(void* pThis, INT32 nParam)
{
	if( nParam == MB_OK)
	{
		g_pFramework->GetClanContext()->SendCancelJoinFromClanMember();
	}
}

