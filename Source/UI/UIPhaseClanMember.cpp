#include "pch.h"
#include "UIPhaseClanMember.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "UITabClanInfo.h"
#include "UITabClanMember.h"
#include "UITabClanMatchResult.h"
#include "UITabClanRanking.h"
#include "UserInfoContext.h"

//Ŭ�� �����������Դϴ�//
#include "UITabClanManagement.h"

/////////////////////////
#include "../ClanContext.h"
#include "../CommunityContext.h"

#include "ClanGameContext.h"

#include "../GameGUI.h"

extern "C"
{
	INT32 UIPhaseClanMember_SelectTab(LuaState* L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		UIPhaseClanMember * pThis = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
		if( pThis != nullptr)
			pThis->ChangeTab((UITabClanType) tab);
		return 0;
	}

	INT32 UIPhaseClanMember_Colse(LuaState * L)
	{
		UIPhaseClanMember * pThis = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
		if( pThis != nullptr)
			pThis->OnEscapeKey();
		return 0;
	}

	INT32 UIPhaseClanMember_ClickButton(LuaState * L)
	{
		UIPhaseClanMember * pThis = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
		if( pThis != nullptr)
			pThis->ClickButton();
		return 0;
	}

	INT32 UIPhaseClanMember_OnBackPhase(LuaState * L)
	{
		UIPhaseClanMember * pThis = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
		if( pThis != nullptr)
			pThis->OnBackPhase();
		return 0;
	}

	INT32 UIPhaseClanMember_ToggleFrame(LuaState * L)
	{
		UIPhaseClanMember * pThis = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
		if( pThis != nullptr)
			pThis->ToggleFrame();
		return 0;
	}
}


LuaFuncReg UIPhaseClanMember_Glue[] =
{
	{"SelectTab",		UIPhaseClanMember_SelectTab},
	{"OnClose",			UIPhaseClanMember_Colse},
	{"ClickButton",		UIPhaseClanMember_ClickButton},
	{"OnBackPhase",		UIPhaseClanMember_OnBackPhase},
	{"ToggleFrame",		UIPhaseClanMember_ToggleFrame},
	{nullptr,				nullptr}
};

I3_CLASS_INSTANCE( UIPhaseClanMember); //, UIPhaseClanBase);

UIPhaseClanMember::UIPhaseClanMember()
{
	m_pClanMark = nullptr;
	m_pSeasonInfo = nullptr;

	m_eCurrnetTab = UTC_INFO;
	m_eNextTab = UTC_NONE;

	for( INT32 i = 0; i < UTC_MAX; ++i)
		m_pClanTab[i] = nullptr;

	RegisterGameEventFunc( EVENT_M_CLAN_MEMBER_INFO,			&UIPhaseClanMember::ReceiveGameEvent_MemberInfo);
	RegisterGameEventFunc( EVENT_CLAN_DETAIL_INFO,				&UIPhaseClanMember::ReceiveGameEvent_DetailInfo);
	RegisterGameEventFunc( EVENT_CLAN_COMMISSIONMASTER,			&UIPhaseClanMember::ReceiveGameEvent_CommisionMaster);
	RegisterGameEventFunc( EVENT_M_CLAN_CHANGE_GRADE,			&UIPhaseClanMember::ReceiveGameEvent_ChangeGrade);
}

UIPhaseClanMember::~UIPhaseClanMember()
{
	m_pClanMark = nullptr;
	m_pSeasonInfo = nullptr;

	for( INT32 i = 0; i < UTC_MAX; ++i)
		if( m_pClanTab[i]) m_pClanTab[ i]->Destroy();
}

/////// protected Member Func //////////

/*virtual*/ void UIPhaseClanMember::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPhaseClanBase::_InitializeAtLoad(pScene);

	// Detail ClanMark
	m_pSeasonInfo = static_cast<i3UIFrameWnd*>(pScene->FindChildByName("FrameWnd_SesaonInfo"));
	m_pClanMark = static_cast<i3UIImageBoxEx*>(pScene->FindChildByName("ImgBox_ClanMark11"));
}

/*virtual*/ void UIPhaseClanMember::_ClearAtUnload(i3UIScene * pScene)
{
	m_pClanMark = nullptr;
	m_pSeasonInfo = nullptr;

	UIPhaseClanBase::_ClearAtUnload(pScene);
}

void UIPhaseClanMember::_InitClanMemberPhase(UITabClanType eType)
{
	m_eCurrnetTab = eType;

	CLAN_MEMBER_LEVEL_TYPE eMemberType = g_pFramework->GetClanContext()->GetMyStaffLevel();

	i3::stack_wstring wstrTemplateName;

	if (g_pEnvSet->IsBaseOnUIVersion_1_0() )
	{
		wstrTemplateName += L"Composed3_24size";
	}
	else if(g_pEnvSet->m_nUIVersion == UFV_1_1 )
	{
		wstrTemplateName += L"BtC3_List_B01";
	}
	else
	{
		if (isVisitUser())
			wstrTemplateName += L"4T_3Bt_27size";
		else
			wstrTemplateName += L"4T_3Bt_clan01";
	}

	LuaState * L = _CallLuaFunction("InitClanMemberPhase");

	if(L != nullptr)
	{
		i3Lua::PushInteger(L, eMemberType);
		i3Lua::PushBoolean(L, isVisitUser());
		i3Lua::PushInteger(L, eType);
		i3Lua::PushStringUTF16To8(L, wstrTemplateName);
		_EndLuaFunction(L , 4);
	}
}

void UIPhaseClanMember::_UpdateClanDetailInfo(void)
{
	LuaState * L = _CallLuaFunction("ClanDetailInfo"); 

	if(L != nullptr)
	{
		// Ŭ���� ���õǾ� ������ ���� ǥ��
		CLAN_DETAIL_INFO * pInfo = nullptr;

		if(isVisitUser() == true)
			pInfo = ClanGameContext::i()->getClanDetailInfo();
		else
			pInfo = ClanGameContext::i()->getMyClanDetailInfo();

		ColorNickPalette::i()->GetColor(pInfo->m_masterNickColor);

		// �г��� �÷�
		const COLOR * pColor = GameGUI::GetColor(GCT_DEFAULT);

		if( pInfo->m_masterNickColor != 0)
		{
			pColor = GetColorCashItem(pInfo->m_masterNickColor);
		}
		///////// ǥ���� ���� �����մϴ�. /////////////////

		// Ŭ�� ���
		i3::wstring wstrClanLevel;
		GetClanFullName(wstrClanLevel, pInfo->m_exp, pInfo->m_currentPerson +1);

		// Ŭ�� ����ġ Rate
		i3::wstring wstrExpRate;
		INT32 i32ExpRate = 0;
		_SetClanExpRate(wstrExpRate, i32ExpRate ,pInfo->m_exp);

		// Ŭ�� ����Ʈ
		i3::stack_wstring wstrClanPoint;
		i3::sprintf( wstrClanPoint, L"%d",(INT32) (pInfo->GetClanPoint() ));

		// Ŭ�� ��� ��
		i3::stack_wstring wstrPeople;
		i3::sprintf( wstrPeople, L"%d/%d", pInfo->m_currentPerson, pInfo->m_maxPerson);

		i3::wstring wstrRecord;
		i3::wstring wstrRecordRate;
		_SetClanTotalRecord(wstrRecord, wstrRecordRate, pInfo);

		// Ŭ�� ��ũ ����
		_SetClanMark(m_pClanMark, pInfo->m_mark);

		// ���� ���� ���� (KEYWORD : SEASON_INFO_INDICATION)
		// ���� ����
		i3::rc_wstring wstr_pre_season_match;
		i3::rc_wstring wstr_pre_season_win;
		i3::rc_wstring wstr_pre_season_lose;
		_SetClanSeasonRecord(pInfo->m_SeasonRecord[SEASON_TYPE_PRE], wstr_pre_season_match, wstr_pre_season_win, wstr_pre_season_lose);

		// ���� ����
		i3::rc_wstring wstr_cur_season_match;
		i3::rc_wstring wstr_cur_season_win;
		i3::rc_wstring wstr_cur_season_lose;
		_SetClanSeasonRecord(pInfo->m_SeasonRecord[SEASON_TYPE_ONGOING], wstr_cur_season_match, wstr_cur_season_win, wstr_cur_season_lose);

		/////////////////////////////////////////////////////
		i3Lua::PushStringUTF16To8(L, pInfo->m_name);							// Ŭ����
		i3Lua::PushInteger( L,  pInfo->m_expRank);								// Ŭ�� ���
		i3Lua::PushStringUTF16To8( L, wstrClanLevel);							// Ŭ�� ��� ��Ī - ����
		i3Lua::PushNumber( L, i32ExpRate * 0.01f);								// Ŭ�� ����ġ Value
		i3Lua::PushStringUTF16To8( L, wstrExpRate);								// Ŭ�� ����ġ Rate
		i3Lua::PushStringUTF16To8( L, wstrClanPoint);							// Ŭ�� ����Ʈ

		i3Lua::PushStringUTF16To8(L, pInfo->m_masterNick);
		i3Lua::PushColor( L, pColor);											// Master NickColor

		i3Lua::PushStringUTF16To8( L, wstrPeople);								// Ŭ�� �ο�
		i3Lua::PushStringUTF16To8( L, wstrRecord);								// Ŭ�� ����
		i3Lua::PushStringUTF16To8( L, wstrRecordRate);							// Ŭ�� �·�

		i3Lua::PushStringUTF16To8( L, wstr_cur_season_match);					// ���� �� ���� ��
		i3Lua::PushStringUTF16To8( L, wstr_cur_season_win);						// ���� �¸� ��
		i3Lua::PushStringUTF16To8( L, wstr_cur_season_lose);					// ���� �й� ��
		i3Lua::PushStringUTF16To8( L, wstr_pre_season_match);					// ���� �� ���� ��
		i3Lua::PushStringUTF16To8( L, wstr_pre_season_win);						// ���� �¸� ��
		i3Lua::PushStringUTF16To8( L, wstr_pre_season_lose);					// ���� �й� ��
	}

	_EndLuaFunction(L , 20);
}


/*virtual*/ void UIPhaseClanMember::OnCreate( i3GameNode * pParent)
{
	UIPhaseClanBase::OnCreate( pParent);

	AddScene("Scene/Clan/PointBlankRe_Clan_Menu.i3UIs", UIPhaseClanMember_Glue, true, false);

	i3ClassMeta * pMeta = nullptr;

	for (INT32 i = 0 ; i < UTC_MAX ; ++i)
	{
		switch( i)
		{	
		case UTC_INFO :		pMeta = UITabClanInfo::static_meta();			break;
		case UTC_MEMBER :	pMeta = UITabClanMember::static_meta();		break;
		case UTC_RESULT :	pMeta = UITabClanMatchResult::static_meta();	break;
		case UTC_RANKING:	pMeta = UITabClanRanking::static_meta();		break;
		case UTC_MGT:		pMeta = UITabClanManagement::static_meta();	break;
			/*
		default :
			pMeta = nullptr;
			I3PRINTLOG(I3LOG_FATAL,  "Not Setted Subphase class...!!!");
			break;
			*/
		}

		if( pMeta != nullptr)
		{
			m_pClanTab[ i] = (UITabClan*) pMeta->create_instance();		//CREATEINSTANCE( pMeta);
			I3ASSERT( m_pClanTab[ i] != nullptr);
			I3ASSERT( m_pClanTab[ i]->same_of( pMeta));

			m_pClanTab[i]->OnCreate( this);
			I3_MUST_ADDREF(m_pClanTab[i]);
		}
	}
}

/*virtual*/ bool UIPhaseClanMember::OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if(UIPhaseClanBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	LuaState * L = _CallLuaFunction("__init");
	if( L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}

	_InitClanMemberPhase(UTC_INFO);
	_UpdateClanDetailInfo();

	GetCurrTab()->GetSlide()->SetTargetTime( true, 0.f );
	bool resTab = GetCurrTab()->OnEntranceStart();
	I3ASSERT(resTab);

	this->Enter();

	m_pSeasonInfo->EnableCtrl(false);

	L = _CallLuaFunction("SelectedTab");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, 0);
		_EndLuaFunction(L, 1);
	}

	return true;
}

/*virtual*/ bool UIPhaseClanMember::OnExitStart(void)
{
	m_eNextTab = UTC_NONE;

	COLOR color;

	GetCurrTab()->GetSlide()->SetTargetTime(false, 0.f);
	GetCurrTab()->OnExitStart();

	CClanContext* pClanContext = g_pFramework->GetClanContext();
	if( pClanContext != nullptr)
	{
		if(isVisitUser() == true)
		{
			pClanContext->SetClanVisit(false);
			pClanContext->ResetMatchResultContext();
		}
	}

	return UIPhaseClanBase::OnExitStart();
}

void UIPhaseClanMember::OnExitEnd()
{
	for (INT32 i=0; i<UTC_MAX; i++) // hansoft. 9728
	{
		if (m_pClanTab[i])
		{
			m_pClanTab[i]->GetSlide()->ForceEnd();
			m_pClanTab[i]->SetEnable(false);
		}
	}

	UIPhaseClanBase::OnExitEnd();
}

void UIPhaseClanMember::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void	UIPhaseClanMember::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	UIPhaseClanBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}

	GetCurrTab()->ProcessGameEvent( evt, arg, UserData);
}

/*virtual*/ void UIPhaseClanMember::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPhaseClanBase::OnUpdate( rDeltaSeconds);

	// Ŭ�� ����Ʈ�� ������Ʈ�Ѵ�.
	g_pFramework->GetClanContext()->UpdateClanList(rDeltaSeconds);
}

/*virtual*/ bool UIPhaseClanMember::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */)
{
	// ���� ���� ����
	// KEYWORD : SEASON_INFO_INDICATION

	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pParam = (I3UI_CONTROL_NOTIFY*) param2;

		if(pParam->m_nEvent == I3UI_EVT_KILLFOCUS)
		{
			m_pSeasonInfo->EnableCtrl(false);
		}
	}

	return UIPhaseClanBase::OnEvent( event, pObj, param2, code);
}

bool UIPhaseClanMember::OnKey_Enter()
{
	for (INT32 i = 0 ; i < UTC_MAX ; ++i)
	{
		m_pClanTab[i]->OnKey_Enter( );
	}

	return true;
}

/*virtual*/ void UIPhaseClanMember::Enter( void)
{
	g_pFramework->GetClanContext()->EnterClanList(true);
}
void UIPhaseClanMember::OnLoadAllScenes()
{
	UIPhaseClanBase::OnLoadAllScenes();

	if (g_pEnvSet->m_nUIVersion != UFV_1_5)
	{
		m_slide.AddFrameWnds(GetScene(0), UISlide::R_TO_L);
		m_slide.SetTargetTime(false, 1.f / UI_SLIDEOUT_SLOP);
		m_slide.SetTargetTime(true, 1.f / UI_SLIDEOUT_SLOP);
	}
}
/*virtual*/ void UIPhaseClanMember::Leave( void)
{
	if (g_pEnvSet->IsBaseOnUIVersion_1_0() || g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		GetMainFrame()->ExclusedEnterSubPhase( USP_TEMP);		
	}
	else
	{
		CClanContext* pClanContext = g_pFramework->GetClanContext();

		// ��� ���� ���濡���� ���� ����߿� ó�� ����
		if(pClanContext->isWaitMemberInfoReq() == true)
			return;

		if( pClanContext->IsEnter())
		{
			pClanContext->LeaveServer();
		}
	}
}

/*vurtial*/ bool UIPhaseClanMember::OnPlayClickSound(i3UIControl * pCtrl)
{
	if( i3::same_of<i3UIButton*>(pCtrl) && 
		i3::generic_is_iequal(pCtrl->GetName() , "Btn_Close")  )
	{
		return PlaySound(UISND_ITEM_CLICK_CANCEL);
	}

	return UIPhaseClanBase::OnPlayClickSound(pCtrl);
}

void UIPhaseClanMember::ChangeTab( UITabClanType tab)
{
	if(m_eCurrnetTab == tab)
		return;

	CClanContext * ClanContext;
	ClanContext = g_pFramework->GetClanContext();

	if(tab == UTC_MGT && ClanContext->GetMyStaffLevel() == CLAN_MEMBER_LEVEL_REGULAR)
	{
		return;
	}

	m_eNextTab = tab;

	GetCurrTab()->GetSlide()->SetTargetTime(false, 0.f);
	GetCurrTab()->OnExitStart();	

	LuaState * L = _CallLuaFunction("SelectedTab");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, (INT32)tab);
		_EndLuaFunction(L, 1);
	}		
}

void UIPhaseClanMember::ProcessNextTab( void)
{
	if(m_eNextTab != UTC_NONE)
	{
		m_eCurrnetTab = m_eNextTab;
		m_eNextTab = UTC_NONE;

		// m_CurrentTab�� �����̵�ƿ��� �Ǹ�...m_NextTab�� �����̵� ��Ű��� �Ѵ�
		//... ��������� ��������� �ϹǷ� �Լ��� �ѷ� �ɰ���..
		if( GetCurrTab() )
		{
			GetCurrTab()->GetSlide()->SetTargetTime( true, 0.f );				
			int resTab = GetCurrTab()->OnEntranceStart();
			I3ASSERT(resTab);
		}
	}
}

void UIPhaseClanMember::ClickButton( void)
{
	CClanContext * pClanContext = g_pFramework->GetClanContext();

	if(pClanContext->IsEnter() == false)
		return;

	// ��Ŭ���� �湮���� ���� �����ϱ� �˾��� ���ϴ�.
	if(pClanContext->IsClanVisit() && pClanContext->GetMyStaffLevel() == CLAN_MEMBER_LEVEL_UNKNOWN)
	{
		if(_CheckClanRequsetEnable() == true)
		{
			// Ŭ���� ���õǾ� ������ ���� ǥ��
			CLAN_DETAIL_INFO * pInfo = ClanGameContext::i()->getClanDetailInfo();
			pClanContext->SendRequestJoining(pInfo->m_ui32CID);
		}
	}
	else
	{
		// Ŭ������ Ż�� / Ŭ�� ��ü ������ ó��
		if( pClanContext->GetMyStaffLevel() == CLAN_MEMBER_LEVEL_MASTER ) 
		{
			g_pFramework->GetClanContext()->ProcessDissolutionClan();
		}
		else
		{
			pClanContext->ProcessSecessionFromClan();
		}
	}
}

void UIPhaseClanMember::OnBackPhase( void)
{
	CClanContext * pClanCxt = g_pFramework->GetClanContext();

	if( pClanCxt->IsMember() == true )
	{
		g_pFramework->GetClanContext()->SetClanVisit(false);

		if(GetCurrTabType() != UTC_INFO)
			ChangeTab(UTC_INFO);
		else
			pClanCxt->SendClanDetailInfoNEvent(pClanCxt->GetMyClanDetailInfo()->m_ui32CID, 0);

		_InitClanMemberPhase(UTC_INFO);
		pClanCxt->ResetMatchResultContext();
	}
	else
	{
		GetMainFrame()->ChangeClanPopup();
	}
}

void UIPhaseClanMember::ToggleFrame( void )
{
	// ���� ���� ����
	// KEYWORD : SEASON_INFO_INDICATION

	bool bEnable = m_pSeasonInfo->isEnable()?true:false;

	m_pSeasonInfo->EnableCtrl(!bEnable);
}

void UIPhaseClanMember::ReceiveGameEvent_MemberInfo( INT32 arg, const i3::user_data&)
{
	if(ClanGameContext::i()->getClanDetailInfo()->m_ui32CID == ClanGameContext::i()->getMyClanDetailInfo()->m_ui32CID)
		g_pFramework->GetClanContext()->SetClanVisit(false);
	
	if(GetCurrTabType() != UTC_INFO)
	{
		GetCurrTab()->GetSlide()->SetTargetTime(false, 0.f);
		GetCurrTab()->OnExitStart();

		_InitClanMemberPhase(UTC_INFO);
		_UpdateClanDetailInfo();

		GetCurrTab()->GetSlide()->SetTargetTime( true, 0.f );
		bool res = GetCurrTab()->OnEntranceStart();
		I3ASSERT(res);
	}
	else
	{
		_InitClanMemberPhase(UTC_INFO);
		_UpdateClanDetailInfo();
	}
	GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_GUI_MESSAGER_CLAN_INVITE_ACCEPT")); // ���� ����
}

void UIPhaseClanMember::ReceiveGameEvent_DetailInfo( INT32 arg, const i3::user_data&)
{
	if(EV_SUCCESSED(arg))
	{
		if(isVisitUser() == true && m_eCurrnetTab == UTC_RANKING)
		{
			ChangeTab(UTC_INFO);
			_InitClanMemberPhase(UTC_INFO);
			g_pFramework->GetClanContext()->ResetMatchResultContext();
		}

		_UpdateClanDetailInfo();
	}
}

void UIPhaseClanMember::ReceiveGameEvent_CommisionMaster( INT32 arg, const i3::user_data&)
{
	_InitClanMemberPhase(UTC_MGT);
}

void UIPhaseClanMember::ReceiveGameEvent_ChangeGrade( INT32 arg, const i3::user_data&)
{
	CClanContext * pClanCxt = g_pFramework->GetClanContext();

	UITabClanType eType = m_eCurrnetTab;

	if(pClanCxt->GetMyStaffLevel() == CLAN_MEMBER_LEVEL_REGULAR &&
		m_eCurrnetTab == UTC_MGT)
	{
		eType = UTC_INFO;
		ChangeTab(eType);		
	}

	_InitClanMemberPhase(eType);

	GameEventSender::i()->SetEvent(EVENT_CLAN_MEMBER_CONTEXT);

	pClanCxt->SendClanDetailInfoNEvent(pClanCxt->GetMyClanDetailInfo()->m_ui32CID, 0);
}