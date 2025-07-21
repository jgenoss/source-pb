#include "pch.h"
#include "UITopMenu_V15.h"

#include "UIMainFrame.h"
#include "UIPhaseBase.h"
#include "UIPhaseSubBase.h"
#include "UIPhaseClanLobby.h"
#include "UIUtil.h"

#include "UIMath.h"
#include "UIFloatCharacterView.h"
#include "UISideMenu.h"
#include "Batmanager.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "MailBoxContext.h"
#include "UserInfoContext.h"
#include "../BattleSlotContext.h"
#include "ClanGameContext.h"
#include "ChannelContext.h"

#define BACKBUTTON_CLICK_DELAY 1.f


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
static UITopMenu_V15 *	GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	if( pFrame != nullptr)
		return (UITopMenu_V15*) pFrame->GetTopMenu();
	return nullptr;
}

extern "C" 
{
	int UITopMenu_Click( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		
		UITopMenu_V15 * pThis = GetThis();

		if( pThis != nullptr)
		{
			// ���� ��ư ���·�, ���� ��ư ���� ���� (Normal ���� <-> Push ����)
			pThis->Clicked(value);
		}

		return 0;
	}
}

LuaFuncReg UITopMenu_Glue[] =
{
	{"Click",				UITopMenu_Click },
	{nullptr,				nullptr}
};

/*************************************/
namespace
{

	INT32 g_last_phase_tab = None;
	INT32 g_last_popup_tab = None;

	MultiSceneInfo g_scn_info;

	INT32 get_tab_idx(UISUBPHASE phase);
	INT32 get_tab_idx(UIPOPUPWIN popup);

	UISUBPHASE get_sub_idx(INT32 tab_idx);
	UIPOPUPWIN get_popup_idx(INT32 tab_idx);

	void TabEnable(INT32 tab);

	void EnterSubPhase(UIMainFrame * pFrame, UISUBPHASE sub);
	void EnterPopup(UIMainFrame * pFrame, UIPOPUPWIN popup, bool delay = false);
}

//------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UITopMenu_V15); //, UIBase);

UITopMenu_V15::UITopMenu_V15() : m_pBG(nullptr), m_pRefAttr(nullptr)
	, m_rElapsedBackButtonTime(0.f)
	, m_pLight(nullptr) 
{
	m_mainBtnsFrame = nullptr;
	m_pAttrSet = nullptr;

	for( UINT32 i = 0; i < CAS_MAX; i++)
		m_pCam[i] = nullptr;

	RegisterGameEventFunc( EVENT_GET_USER_INFO,			&UITopMenu_V15::ReceiveGameEvent_GET_USER_INFO);
	RegisterGameEventFunc( EVENT_GET_INVEN_INFO,		&UITopMenu_V15::ReceiveGameEvent_GET_INVEN_INFO);
	RegisterGameEventFunc( EVENT_GET_GIFT_LIST,			&UITopMenu_V15::ReceiveGameEvent_GET_GIFT_LIST);
	//RegisterGameEventFunc( EVNT_CEC,		&UITopMenu_V15::ReceiveGameEvent_FAIL_ENTER_GIFTSHOP);
}

UITopMenu_V15::~UITopMenu_V15()
{
	// UI::remove_cb(this);

	for( UINT32 i = 0; i < CAS_MAX; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pCam[ i]);
	}

	I3_SAFE_NODE_RELEASE(m_pBG);
	I3_SAFE_RELEASE(m_pRefAttr);

	m_pAttrSet->RemoveAllChild();
	m_pAttrSet->RemoveAllAttrs();
	I3_SAFE_NODE_RELEASE(m_pAttrSet);

	if( g_pFramework != nullptr)
	{
		if( m_pLight != nullptr)
		{
			g_pFramework->removeLight( m_pLight);
			I3_SAFE_RELEASE( m_pLight);
		}
	}
}

//------------------------------------------------------------------------------//
//							Callback rutine										//
//------------------------------------------------------------------------------//

void UITopMenu_V15::CbExitNormal(void* pThis,INT32 nParam)
{
}

void UITopMenu_V15::CbRemoveMercenary(void* pThis,INT32 nParam)
{
	if(nParam == MB_OK)
	{
		UIMainFrame * pUIFrame = ((UITopMenu_V15*)pThis)->GetMainFrame();
		UIPhaseBase * pPhase = pUIFrame->GetCurrentPhase();
		if( pPhase != nullptr && pPhase->isSliding() == false)
		{
			pPhase->OnGotoBackPhase();
			pUIFrame->TogglePopup( UPW_NULL);
		}

		ClanGameContext::i()->setMyMercenaryState(false);
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
		g_last_phase_tab = None;
	}
}

// �뺴���� ������
void UITopMenu_V15::CbReRegistMercenary(void* pThis,INT32 nParam)
{
	if(nParam == MB_OK)
	{
		UIMainFrame * pUIFrame = ((UITopMenu_V15*)pThis)->GetMainFrame();
		pUIFrame->MovePhase(UIPHASE_LOBBY);
		ClanGameContext::i()->setClanBattleLobbyEnable(false);
	}
	else
	{
		((UIPhaseClanLobby*)GameUI::GetPhase(UIPHASE_CLANLOBBY))->SetLeaveTeamFlag();
		ClanGameContext::i()->setClanBattleLobbyEnable(false);
		GameEventSender::i()->SetEvent(EVENT_LEAVE_LOBBY);
	}
}


/*virtual*/ void UITopMenu_V15::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad( pScene);

	I3ASSERT( pScene != nullptr);

	i3UIFrameWnd * pFrame = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd_Main");
	I3ASSERT( pFrame != nullptr);
	pFrame->setSize( (REAL32)g_pViewer->GetViewWidth(), (REAL32)g_pViewer->GetViewHeight());

	m_pBGMenu = static_cast<i3UIImageBox*>(pScene->FindChildByName( "bgMenu"));
	I3ASSERT( m_pBGMenu != nullptr);

	m_mainBtnsFrame = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd_MainBtn");

	m_FlowEditBox.SetFlowEditBox(static_cast<i3UIEditBox*>( pScene->FindChildByName( "i3UIEditBox")));

	std::string strPushBtnName[MaxCount] = 
	{
		"i3UIButton_Shop",		// Shop
		"i3UIButton_Inven",		// Inven
		"i3UIButton_Lottery",	// Gacha
		"",						// Clan		(not used push)
		"",						// Myinfo	(not used push)
		"",						// MailBox	(not used push)
		"",						// Setting	(not used push)
		"",						// Back		(not used push)
		"",						// Exit		(not used push)
		"i3UIButton_Gift"		// GiftShop
	};

	m_PushBtnList.clear();
	
	for(INT32 idx = 0 ; idx < MaxCount ; ++idx)
	{
		if(strPushBtnName[idx].length() == 0) 
			continue;

		PushButton BtnInfo;

		std::string strNormal = strPushBtnName[idx];
	
		BtnInfo.i32Idx = idx;
		BtnInfo.pNormalBtn = static_cast<i3UIButton*>( pScene->FindChildByName(strNormal.c_str() ));
	
		m_PushBtnList.push_back(BtnInfo);
	}

	{
		pFrame = (i3UIFrameWnd*) pScene->FindChildByName( "bgFrame");
		I3ASSERT( pFrame != nullptr);
		pFrame->setSize( (REAL32)g_pViewer->GetViewWidth(), (REAL32)g_pViewer->GetViewHeight());
		
		_AddLight();

		_LoadBG();

		g_pFramework->setShadowQuality( g_pEnvSet->m_nShadowQualityType, false);
	}

	Resize();
}

/*virtual*/ void UITopMenu_V15::_ClearAtUnload( i3UIScene * pScene)
{
	UIBase::_ClearAtUnload( pScene);
	m_PushBtnList.clear();

	m_FlowEditBox.Reset();
}

void UITopMenu_V15::_SetServerMode( void)
{
	LuaState * L = _CallLuaFunction( "SetServerMode");
	_EndLuaFunction( L, 0);
	_EndHighlightMailbox();
}

void UITopMenu_V15::_SetChannelMode( void)
{
	LuaState * L = _CallLuaFunction( "SetChannelMode");
	_EndLuaFunction( L, 0);
	_EndHighlightMailbox();
}

void UITopMenu_V15::_SetReadyMode(void)
{
	bool bGachaFlag = false;
	bool bClanFlag = false;
	bool bGiftFlag = false;

	// �̱������ Lobby������ ����մϴ�.(���� PhaseType�� UIPHASE_LOBBY�̰� TOPMENU_MODE_SHOP�� �ƴϾ�� �Ѵ�) - 2014. 12. 18 sanghun.han
	if (g_pFramework->GetUIMainframe()->GetCurrentPhaseType() == UIPHASE_LOBBY && GetMode() != TOPMENU_MODE_SHOP)
		bGachaFlag = UserInfoContext::i()->IsContentsEndabled(CONTENTS_GACHA);

	bClanFlag = UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN);
	bGiftFlag = UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFTSHOP);


	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
	{
		// 13553 �뺴 ����(Ŭ�� �̰��԰���)���� Ŭ��ä������� Ŭ�� ��ư�� ��Ȱ��ȭ �Ǿ�� ��
		if (ChannelContext::i()->GetBattleChannel() &&
			UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
		{
			bClanFlag = false;
		}
	}

	LuaState * L = _CallLuaFunction("SetReadyMode");
	i3Lua::PushBoolean(L, bGachaFlag);
	i3Lua::PushBoolean(L, bClanFlag);
	i3Lua::PushBoolean(L, bGiftFlag);

	_EndLuaFunction( L, 3);

}


void UITopMenu_V15::cb_tmenu_gauge( bool lastestTarget)
{
	if( UserInfoContext::i()->IsDailyGiftGauge() == false || UserInfoContext::i()->IsUserInfoDaily() == false)
	{
		mPBGaugeBar.reset(); return; 
	}

	if( UserInfoContext::i()->GetUserInfoDaily()->_ui8GetItem == DAILY_GIFT_TYPE_MAX ) 
	{
		mPBGaugeBar.reset(); return; 
	}

	INT32 idx = UserInfoContext::i()->GetUserInfoDaily()->_ui8GetItem;
	INT32 max_gauge = UserInfoContext::i()->GetDailyGiftGauge(idx);

	REAL32 factor = 0.f;
	if( max_gauge != 0 ) factor = (REAL32)(UserInfoContext::i()->GetUserInfoDaily()->_DGPlaytime / (REAL32)max_gauge);

	factor = uiMath::limited(factor, 0.f, 1.0f);

#if visible_on_current_value
	#ifndef NC_BUILD
		i3::stack_wstring	wstr_stack;
		i3::wstring wstr_msg = L"turn reward ->";
		wstr_msg += L" play ";
		i3::itoa(UserInfoContext::i()->GetUserInfoDaily()->_DGPlaytime, wstr_stack);
		wstr_msg += wstr_stack;

		wstr_msg += L", max ";
		i3::itoa(max_gauge, wstr_stack);
		wstr_msg += wstr_stack;

		wstr_msg += L", result ";
		i3::ftoa(factor, wstr_stack, 3);
		wstr_msg += wstr_stack;

		::ColorString(wstr_msg, 255,0,0);
		GameUI::PrintSystemChat(L"\n");
		GameUI::PrintSystemChat(wstr_msg);
		GameUI::PrintSystemChat(L"\n");
	#endif
#endif

	if( factor == 0.f )
	{
		mPBGaugeBar.reset();
	}
	else
	{
		if( lastestTarget == true)
		{
			mPBGaugeBar.set_lastest_target_factor();
		}
		mPBGaugeBar.set_factor(factor);
	}
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UITopMenu_V15::OnCreate( i3GameNode * pParent)
{
	UIBase::OnCreate( pParent);

	// Load Scene
	g_scn_info.idx = GetSceneCount();
	AddScene( "Scene/Main/PointBlankRe_Main.i3UIs", UITopMenu_Glue, true, false);

	mPBGaugeBar.init( GetScene(g_scn_info.idx) );

	// �׽� ���̹Ƿ� Enable�Ѵ�.
	SetEnable( true);
	switch( m_Mode)
	{
	case TOPMENU_MODE_SERVER :_SetServerMode();		break;
	case TOPMENU_MODE_CHANNEL :_SetChannelMode();	break;
	case TOPMENU_MODE_READY : _SetReadyMode();		break;
	}
}
bool UITopMenu_V15 :: OnClick( i3UIControl * pControl)
{

	//GetScene

	if(GetMainFrame()->GetCurrentSubPhaseType() == USP_GACHA)
		return GetMainFrame()->GetCurrentSubPhase()->OnClick(pControl);
	return UIBase::OnClick( pControl);
}
/*virtual*/ void UITopMenu_V15::OnUpdate( REAL32 rDeltaSeconds)
{
	UIBase::OnUpdate( rDeltaSeconds);
	BaTMaN::BatMan_Outgame_Update(rDeltaSeconds);

	_UpdateFlowAnnounce( rDeltaSeconds);
	if (m_Mode >= TOPMENU_MODE_READY)
	{
		_UpdateMailBoxAlaram(rDeltaSeconds);
	}

	m_rElapsedBackButtonTime -= rDeltaSeconds;

	mPBGaugeBar.update(rDeltaSeconds);

	UpdateCamera();
}

void UITopMenu_V15::OnLoadAllScenes()
{
	g_scn_info.scn = GetScene(g_scn_info.idx);
	g_last_popup_tab = g_last_phase_tab = None;
	_EndHighlightMailbox();

	mPBGaugeBar.reset();
}

bool UITopMenu_V15::OnRevive( i3RenderContext * pCtx)
{
	if( UIBase::OnRevive( pCtx) == false)
		return false;

	Resize();

	if( m_pCam[m_CurCameraState] != nullptr)
	{
		i3Camera * pCam = static_cast<i3Camera*>( i3Scene::FindNodeByType( m_pCam[m_CurCameraState], i3Camera::static_meta()));
		I3ASSERT( pCam != nullptr);
		g_pFramework->getSgContext()->setCurrentCamera( pCam);
		pCam->SetPerspective( pCam->GetFOV(), (REAL32) g_pEnvSet->m_nScreenWidth, (REAL32) g_pEnvSet->m_nScreenHeight, pCam->GetNear(), pCam->GetFar());
	}

	return true;
}

void UITopMenu_V15::SetOriginalCamera()
{
	i3Camera * pCam = static_cast<i3Camera*>( i3Scene::FindNodeByType( m_pCam[ m_CurCameraState], i3Camera::static_meta()));
	I3ASSERT( pCam != nullptr);
	g_pFramework->getSgContext()->setCurrentCamera( pCam);
}

/*virtual*/ bool UITopMenu_V15::OnPlayClickSound(i3UIControl * pCtrl)
{
	// �ڷΰ��� ��ư�� ��� Cancel ���� ���
	if(i3::same_of<i3UIButton*>(pCtrl) && 
		i3::generic_is_iequal(pCtrl->GetName(), "i3UIButton_Back") )
	{
		return PlaySound(UISND_ITEM_CLICK_CANCEL);
	}

	return UIBase::OnPlayClickSound(pCtrl);
}

void UITopMenu_V15::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

void UITopMenu_V15::Resize( void)
{
	// ��ư focus ������ �÷��� �Ѵ�.
	i3UIScene * pScene = GetScene( 0);
	I3ASSERT( pScene != nullptr);
	i3UIFrameWnd * pFrame = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd_Main");
	I3ASSERT( pFrame != nullptr);



	REAL32 flowNoticeScale = (REAL32)m_FlowEditBox.GetWidth() / (REAL32)pFrame->getWidth();

	pFrame->setSize( (REAL32)g_pViewer->GetViewWidth(), (REAL32)pFrame->getHeight());

	i3UIManager * pMng = i3UI::getManager();

	m_FlowEditBox.SetWidth((REAL32)(pFrame->getWidth() * flowNoticeScale));

	// ���

	REAL32 scale = (REAL32)g_pViewer->GetViewHeight() / (REAL32)pMng->getActualHeight();

	{
		REAL32 rateY = pMng->getScreenStretchRatioY();
		REAL32 offset = (REAL32) pMng->getDisplayOffsetX();

		m_pBGMenu->setSize( (REAL32)g_pViewer->GetViewWidth(), getY( m_pBGMenu->getSize()) * scale );
		m_pBGMenu->setPos( pFrame->getPos()->x - offset , -(REAL32)pMng->getDisplayOffsetY() * rateY);
	}

}

void UITopMenu_V15::ClickPopupBackButton(bool close_popup)
{
	if(close_popup)
	{
		GetMainFrame()->ClosePopup( get_popup_idx(g_last_popup_tab) );
	}

	if( g_last_phase_tab != None && IsServerBasePopup() )	//popup �� ��Ȱ��ȭ �� �� ������ �Ǵ� subphase�� �����Ѵ�.
		GetMainFrame()->EventEnterSubPhase( get_sub_idx(g_last_phase_tab) );

	if( g_last_phase_tab == None )
	{
		for(size_t idx = 0 ; idx < m_PushBtnList.size(); ++idx)
		{
			if (m_PushBtnList[idx].GetPushed())
				m_PushBtnList[idx].SetPushed();
		}
	}
	else
	{
		TabEnable(g_last_phase_tab);
	}


	g_last_popup_tab = None;
}

void UITopMenu_V15::Clicked(INT32 value)
{
	// Ŭ�� �̺�Ʈ�� �Ѿ� �� �� �ڽ��� �ΰ��� �ε����̸� 
	// �ٸ� ������������ �Ѿ�� �ȵǹǷ� ���ƵӴϴ�. - jinsik(2014. 11. 20)
	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM && 
		BattleSlotContext::i()->getMySlotInfo()->_State >= SLOT_STATE_LOAD) return;

	UIMainFrame * pFrame = GetMainFrame();

	if( _IsAvailablePush(value) )
		_SetPushedButton(value);

	switch (value)
	{
	case Shop:		
		{
			if( ! g_pConfigEx->GetShop().Enable )
			{
				/*���Ŀ� ������ �����Դϴ�.*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_ETC_MENUBAR_UNDER_CONSTRUCTION"));
			}

			EnterSubPhase(pFrame, USP_SHOP);
		}
		break;	
	case Inven:	
		{
			ClickInventory();			
		}
		break;
	case Gacha: EnterSubPhase(pFrame, USP_GACHA); break;

	case Clan:
		{
			UIPOPUPWIN popup = UPW_CLAN_MEMBER;
			if(pFrame->IsOpenedPopup(UPW_CLAN_NON_MEMBER) == true )
				popup = UPW_CLAN_NON_MEMBER;
			EnterPopup(pFrame, popup, true);
		}
		break;
	case Myinfo		: EnterPopup(pFrame, UPW_PROFILE);		break;	
	case MailBox	: EnterPopup(pFrame, UPW_MAILBOX);		break;
	case Setting	: EnterPopup(pFrame, UPW_OPTION);		break;
	case Back		: ClickBackButton();					break;
	case Exit		: ClickExit();							break;
	case GiftShop	: EnterSubPhase(pFrame, USP_GIFTSHOP);	break;
	}
}

/*virtual*/void UITopMenu_V15::AllMenuBtnShapeToNormal( void )
{
	for(size_t idx = 0 ; idx < m_PushBtnList.size(); ++idx)
	{
		if (m_PushBtnList[idx].GetPushed())
			m_PushBtnList[idx].SetPushed();
	}
}

/*virtual*/void UITopMenu_V15::SyncSubPhaseWithMenuBtnState()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UISUBPHASE currentSubPhaseType = pFrame->GetCurrentSubPhaseType();
	INT32 tabValue = -1;

	switch (currentSubPhaseType)
	{
	case USP_NONE:			tabValue = -1;			break;
	case USP_TEMP:			tabValue = Clan;		break;
	case USP_INVENTORY:		tabValue = Inven;		break;
	case USP_SHOP:			tabValue = Shop;		break;
	case USP_GIFTSHOP:		tabValue = GiftShop;	break;
	default:				tabValue = -1;			break;
	}

	if (tabValue == -1)
		AllMenuBtnShapeToNormal();
	else
	{
		if (_IsAvailablePush(tabValue))
			_SetPushedButton(tabValue);
	}
}


bool UITopMenu_V15::ClickBackButton( void)
{
	if( m_rElapsedBackButtonTime > 0.f) return false;

	bool bSucess = false;

	{
		UIMainFrame * pUIFrame = GetMainFrame();

		if( pUIFrame->GetCurrentSubPhaseType() != USP_NONE )
		{
			UIPhaseSubBase * pSubPhase = pUIFrame->GetCurrentSubPhase();
			if( pSubPhase != nullptr && pSubPhase->isSliding() == false )
			{
				pUIFrame->ExclusedEnterSubPhase( USP_NONE);
				bSucess = true;
			}
		}
		else
		{
			UIPhaseBase * pPhase = pUIFrame->GetCurrentPhase();
			if( pPhase != nullptr && pPhase->isSliding() == false)
			{
				// Ŭ��ä�ο��� �뺴�� �ڷΰ��� ��ư�� ������ �뺴 ��� Ȯ�� �˾��� ������
				if(ChannelContext::i()->GetBattleChannel())
				{
					if(ClanGameContext::i()->getMyMercenaryState() )
					{
						if( pUIFrame->GetCurrentPhaseType() == UIPHASE_LOBBY )
						{
							GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_CHANNEL_LIST_MOVE_CANCLE"), nullptr, MAKE_CALLBACK(CbRemoveMercenary), this);
							return false;
						}
						else if(pUIFrame->GetCurrentPhaseType() == UIPHASE_CLANLOBBY)
						{
							g_pFramework->GetClanContext()->MatchTeamLeave();
							if (UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
							{
								GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_REJOIN_POPUP"), nullptr, MAKE_CALLBACK(CbReRegistMercenary), this);
							}
							else
							{
								GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_CHANNEL_LIST_MOVE_CANCLE"), nullptr, MAKE_CALLBACK(CbRemoveMercenary), this);
							}
							return false;
						}


					}
				}

				pPhase->OnGotoBackPhase();
				bSucess = true;
			}
		}

		// �˾�â�� ��� �ݴ´�.
		pUIFrame->TogglePopup( UPW_NULL);
	}

	m_rElapsedBackButtonTime = BACKBUTTON_CLICK_DELAY;

	for(size_t idx = 0 ; idx < m_PushBtnList.size(); ++idx)
	{
		if (m_PushBtnList[idx].GetPushed())
			m_PushBtnList[idx].SetPushed();
	}

	g_last_phase_tab = None;

	return bSucess;
}

void UITopMenu_V15::StartHighlightMailbox( void)
{
	if(m_Mode < TOPMENU_MODE_READY)
		return;

	m_bAlarmCommunity = true;
	m_bAlarmFlag = true;
	m_rAlarmTerm = 0.0f;
}

//-----------------------------------------------------------------------------------//

void UITopMenu_V15::ClickExit( void)
{
	OnEscapeKey();
}

void UITopMenu_V15::CloseEnterPopup( void)
{	
	UIMainFrame * pFrame = GetMainFrame();
	
	switch(g_last_popup_tab)
	{
	case Clan:
		{
			UIPOPUPWIN popup = UPW_CLAN_MEMBER;
			if(pFrame->IsOpenedPopup(UPW_CLAN_NON_MEMBER) == true )
				popup = UPW_CLAN_NON_MEMBER;
			EnterPopup(pFrame, popup, true);
		}
		break;	
	case Myinfo:		EnterPopup(pFrame, UPW_PROFILE);	break;	
	case MailBox:		EnterPopup(pFrame, UPW_MAILBOX);	break;
	case Setting:		EnterPopup(pFrame, UPW_OPTION);		break;
	default:
		break;
	}
}



bool UITopMenu_V15::IsServerBasePopup()
{
	return (g_last_popup_tab == Clan);
}

void UITopMenu_V15::uglyOnMainFrameClosePopupAfter(INT32 popup)
{
	INT32 topMenuPopupIdx = get_tab_idx((UIPOPUPWIN)popup);

	if(topMenuPopupIdx == None)
		return;

	g_last_popup_tab = None;
}

namespace
{
	INT32 get_tab_idx(UISUBPHASE phase)
	{
		switch(phase)
		{
		case USP_SHOP		: return Shop;		break;
		case USP_INVENTORY	: return Inven;		break;
		case USP_GACHA		: return Gacha;		break;
		case USP_GIFTSHOP	: return GiftShop;	break;
		}
		return None;
	}
	INT32 get_tab_idx(UIPOPUPWIN popup)
	{
		switch(popup)
		{
		case UPW_CLAN_NON_MEMBER: return Clan; break;
		case UPW_CLAN_MEMBER: return Clan; break;
		case UPW_PROFILE: return Myinfo; break;
		case UPW_MAILBOX: return MailBox; break;
		case UPW_OPTION: return Setting; break;
		}
		return None;
	}
	UISUBPHASE get_sub_idx(INT32 tab_idx)
	{
		switch(tab_idx)
		{
		case Shop		: return USP_SHOP;		break;
		case Inven		: return USP_INVENTORY; break;
		case Gacha		: return USP_GACHA;		break;
		case GiftShop	: return USP_GIFTSHOP;	break;
		}

		return USP_NONE;
	}
	UIPOPUPWIN get_popup_idx(INT32 tab_idx)
	{
		switch(tab_idx)
		{
		case Myinfo: return UPW_PROFILE; break;
		case MailBox: return UPW_MAILBOX; break;
		case Setting: return UPW_OPTION; break;
		}

		return UPW_NULL;
	}


	void TabEnable(INT32 tab)
	{
		LuaState* L = UILua::CallLuaFunction(g_scn_info.scn, "TabEnable");
		i3Lua::PushInteger( L, tab );
		UILua::EndLuaFunction(L , 1);
	}

	void EnterSubPhase(UIMainFrame * pFrame, UISUBPHASE sub)
	{
		if( i3UI::getDisableUIInput() == true)	return;

		if( pFrame->IsEnteredSubPhase(sub) )
		{
			pFrame->EnableFloatWin(UFW_CHARACTER);
			pFrame->ExclusedEnterSubPhase( USP_NONE);
			g_last_phase_tab = None;
		}
		else
		{
			switch(sub)
			{
			case USP_GACHA		: pFrame->DisableFloatWin(UFW_CHARACTER);	break;
			default				: pFrame->EnableFloatWin(UFW_CHARACTER);	break;
			}

			//��� ���� ��ü �Ѵ�.
			UISideMenu::instance()->send_change_equip();
			pFrame->ExclusedEnterSubPhase( sub );
			g_last_phase_tab = get_tab_idx(sub);
		}
	}

	void EnterPopup(UIMainFrame * pFrame, UIPOPUPWIN popup, bool delay)
	{
		if( i3UI::getDisableUIInput() == true)	return;

		if( pFrame->IsOpenedPopup(popup) )
		{
			if(delay)
			{
				pFrame->ExclusedEnterPopup( UPW_NULL );
			}
			else 		
			{
				pFrame->ClosePopup(popup);
				//g_last_popup_tab = None; // OnMainFrameClosePopupAfter�� �Űܰ�
			}
		}
		else
		{
			g_last_popup_tab = get_tab_idx(popup);

			if(delay)
			{
				if( g_last_phase_tab != None )	//popup �� Ȱ��ȭ �� �� ������ �Ǵ� subphase�� ������.
				{
					if( pFrame->GetCurrentSubPhase() != nullptr)
						pFrame->GetCurrentSubPhase()->Leave();
				}

				pFrame->ExclusedEnterPopup( popup );
			}
			else 	pFrame->OpenPopup(popup);		
		}
	}
}

namespace CAM_ANIM
{
	char* szPath[CAS_MAX] = {
        "Gui/Showroom/UI_V15/Camera_Idle.I3S",
		"Gui/Showroom/UI_V15/Camera_IdleToLeft.I3S",
		"Gui/Showroom/UI_V15/Camera_IdleToRight.I3S",
		"Gui/Showroom/UI_V15/Camera_LeftToIdle.I3S",
		"Gui/Showroom/UI_V15/Camera_LeftToRight.I3S",
		"Gui/Showroom/UI_V15/Camera_RightToIdle.I3S",
		"Gui/Showroom/UI_V15/Camera_RightToLeft.I3S",
	};
}

void UITopMenu_V15::_LoadBG( void)
{
	I3ASSERT( m_pAttrSet == nullptr);
	m_pAttrSet = i3AttrSet::new_object();

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set( true);
		m_pAttrSet->AddAttr( pAttr);
	}

	// 3D ��� �б�
	if( m_pBG == nullptr)
	{
		i3SceneFile file;

		if( file.Load("Gui/Showroom/UI_V15/Gunshowcaseroom_Lighting.i3s") == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Couldn't find %s. Load Default Setting", "Gunshowcaseroom_Lighting.i3s");
			return;
		}

		m_pBG = file.GetSceneGraph();
		I3_MUST_ADDREF(m_pBG);

		i3BoundBox * pBox = i3BoundBox::new_object_ref();
		m_pBG->GetWrappingBound( pBox);
		m_pBG->SetBound( pBox);
	}

	m_pAttrSet->AddChild( m_pBG);

	m_pCamNode = i3Node::new_object_ref();
#if defined( I3_DEBUG)
	m_pCamNode->SetName( "CamNode");
#endif
	m_pAttrSet->AddChild( m_pCamNode);

	g_pFramework->Get3DRoot()->AddChild( m_pAttrSet);

	if( m_pRefAttr == nullptr)
	{	// ReflectionMap bind
		i3ImageFile refFile;
		i3TextureCube * pTex = (i3TextureCube*) refFile.Load( "Gui/Showroom/UI_V15/Gunshowcaseroom_Ref.dds");
		I3ASSERT( pTex != nullptr);

		{
			m_pRefAttr = i3ReflectMapBindAttr::new_object();
			m_pRefAttr->SetTexture( pTex);
			I3_MUST_RELEASE(pTex);

			m_pAttrSet->AddAttr( m_pRefAttr);
		}
	}

	if( m_pCam[0] == nullptr)
	{
		// load camera animation 
		for( UINT32 i = 0; i < CAS_MAX; i++)
		{
			i3SceneFile camFile;
			if( camFile.Load( CAM_ANIM::szPath[i]) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN,  "Invalid I3S file (%s)", CAM_ANIM::szPath[i] );
				return;
			}

			m_pCam[ i] = camFile.GetSceneGraph();
			I3_MUST_ADDREF( m_pCam[ i]);

			i3::vector<i3Node*> transList;
			i3Scene::FindNodeByExactType( m_pCam[ i], i3Transform::static_meta(), transList);
			for( UINT32 j = 0; j < transList.size(); j++)
			{
				i3Transform * pTrans = static_cast<i3Transform*>(transList.at(j));
				if( pTrans->IsAnimated())
					pTrans->SetPlayMode( I3_ANIM_PLAY_MODE_CLAMP);
			}

			i3Camera * pCam = static_cast<i3Camera*>( i3Scene::FindNodeByType( m_pCam[ i], i3Camera::static_meta()));
			I3ASSERT( pCam != nullptr);

			// MAX���� ������ Camera�� FOV�� ���� FOV�� �ƴ϶� ���� FOV�̴�.
			// �׷��� i3Export���� ���� FOV�� �׳� ���� FOV������ �����ϸ鼭, �����δ� ���� ū FOV�� �ٲ��
			// ���׸� ���ϱ� ����, ������ ������ ������ ������.
			pCam->SetFOV( I3_DEG2RAD( 34.5f));
			pCam->SetPerspective( pCam->GetFOV(), (REAL32) g_pEnvSet->m_nScreenWidth, (REAL32) g_pEnvSet->m_nScreenHeight, pCam->GetNear(), pCam->GetFar());
		}

		m_CurCameraState = CAS_IDLE;

		m_pCamNode->AddChild( m_pCam[ m_CurCameraState]);

		// select current camera animation
		i3Camera * pCam = static_cast<i3Camera*>( i3Scene::FindNodeByType( m_pCam[ m_CurCameraState], i3Camera::static_meta()));
		I3ASSERT( pCam != nullptr);
		g_pFramework->getSgContext()->setCurrentCamera( pCam);
	}
}

void UITopMenu_V15::_AddLight( void)
{
	if( m_pLight == nullptr)
	{	
		m_pLight = i3Light::new_object();
		m_pLight->SetLightType( I3G_LIGHT_DIRECTIONAL);

		i3RegKey * pRoot = Config::_GetRegRoot("Stage/Stage");
		I3ASSERT( pRoot != nullptr);

		COLOR col;
		COLORREAL d_diffuse, d_specular, ambient;
		VEC4D vd_diffuse, vd_specular;
		VEC3D d_rot;

		i3RegKey * pKey = Config::FindKey( "Lobby", pRoot);
		if( pKey != nullptr)
		{
			FIND_REG_DATA( pKey, "Ambient", &col);
			i3Color::Set( &ambient, i3Color::GetR(&col),i3Color::GetG(&col),i3Color::GetB(&col),i3Color::GetA(&col) );

			//directional light
			FIND_REG_DATA( pKey, "D_Light_Diffuse", &vd_diffuse);
			i3Color::Set( &d_diffuse, getX( &vd_diffuse), getY( &vd_diffuse), getZ( &vd_diffuse),getW( &vd_diffuse));

			FIND_REG_DATA( pKey, "D_Light_Specular", &vd_specular);
			i3Color::Set( &d_specular, getX( &vd_specular), getY( &vd_specular), getZ( &vd_specular),getW( &vd_specular));

			FIND_REG_DATA( pKey, "D_Light_Rotate",	&d_rot);
		}
		else
		{
			i3Color::Set( &ambient, 0.4f, 0.4f, 0.4f, 1.0f);
			i3Color::Set( &d_diffuse, 1.0f, 0.0f, 0.0f, 1.0f);
			i3Color::Set( &d_specular, 1.0f, 1.0f, 1.0f, 1.0f);

			i3Vector::Set( &d_rot, -12.0f, -34.0f, 4.0f);
		}

		m_pLight->SetDiffuse( &d_diffuse);
		m_pLight->SetSpecular( &d_specular);

		g_pFramework->getSgContext()->setAmbient( &ambient);

		MATRIX mtx;

		i3Matrix::SetTranslate( &mtx, 0.0f, 0.0f, getZ( &d_rot));
		i3Matrix::PostRotateX( &mtx, I3_DEG2RAD( getY( &d_rot)));
		i3Matrix::PostRotateY( &mtx, I3_DEG2RAD( getX( &d_rot)));

		m_pLight->SetWorldSpaceTransform( &mtx);

		g_pFramework->addLight( m_pLight);
	}
}

bool UITopMenu_V15::_IsAvailablePush(INT32 value)
{
	for each( PushButton _PushBtn in m_PushBtnList)
	{
		if(_PushBtn.i32Idx == value)
			return true;
	}

	return false;
}

void UITopMenu_V15::_SetPushedButton(INT32 value)
{
	// ���õ� ��ư�� Push ���� ����, �� �ܿ� ��ư���� Push ����.
	for each( PushButton _PushBtn in m_PushBtnList )
	{
		if((_PushBtn.i32Idx != value && _PushBtn.GetPushed()) || _PushBtn.i32Idx == value)
			_PushBtn.SetPushed();
	}
}

void UITopMenu_V15::SetUICharaCamera( CAM_LOOK_CMD state)
{
	m_CurCameraLook = state;
}

void UITopMenu_V15::UpdateCamera( void)
{
	CAM_ANIM_STATE newState = CAS_IDLE;

	REAL32 rPlayTime = ((i3Transform*)(m_pCam[m_CurCameraState]->GetChild(0)))->getPlayTime();
	REAL32 rDuration = ((i3Transform*)(m_pCam[m_CurCameraState]->GetChild(0)))->GetDuration();

	switch( m_CurCameraState)
	{
	case CAS_IDLE:				// fall throught
	case CAS_LEFT_TO_IDLE:		// fall throught
	case CAS_RIGHT_TO_IDLE:
		switch( m_CurCameraLook)
		{
		case CL_IDLE:	return;
		case CL_SHOP:	newState = CAS_IDLE_TO_LEFT;	break;
		case CL_WEAPON: newState = CAS_IDLE_TO_RIGHT;	break;
		}
		break;
	case CAS_IDLE_TO_LEFT:		// fall throught
	case CAS_RIGHT_TO_LEFT:
		switch( m_CurCameraLook)
		{
		case CL_IDLE:	newState = CAS_LEFT_TO_IDLE;	break;
		case CL_SHOP:	return;
		case CL_WEAPON: newState = CAS_LEFT_TO_RIGHT;	break;
		}
		break;
	case CAS_IDLE_TO_RIGHT:		// fall throught
	case CAS_LEFT_TO_RIGHT:
		switch( m_CurCameraLook)
		{
		case CL_IDLE:	newState = CAS_RIGHT_TO_IDLE;	break;
		case CL_SHOP:	newState = CAS_RIGHT_TO_LEFT;	break;
		case CL_WEAPON:	return;
		}
		break;
	default:
		I3PRINTLOG(I3LOG_FATAL, "invalid camera state!!!!");	// ī�޶� ������Ʈ�� �߸��Ǿ����ϴ�.
		break;
	}

	// �ִϸ��̼� ���̶�� �������� �ʴ´�.
	if( m_CurCameraState != newState && (rPlayTime > rDuration) )
	{
		// �ִϸ��̼��� ��ü�Ѵ�
		m_pCamNode->RemoveChild(m_pCam[m_CurCameraState]);

		m_CurCameraState = newState;

		m_pCamNode->AddChild(m_pCam[m_CurCameraState]);

		i3::vector<i3Node*> transList;
		i3Scene::FindNodeByExactType( m_pCam[ m_CurCameraState], i3Transform::static_meta(), transList);

		for( UINT32 i = 0; i < transList.size(); i++)
		{
			i3Transform * pTrans = static_cast<i3Transform*>( transList.at(i));
			if( pTrans->IsAnimated())
			{
				pTrans->SetPlayCount(0);
				pTrans->SetPlayTime(0.0f);
			}
		}

		{
			i3Camera * pCam = (i3Camera *) i3Scene::FindNodeByExactType( m_pCam[ m_CurCameraState], i3Camera::static_meta());
			I3ASSERT( pCam != nullptr);

			g_pFramework->getSgContext()->setCurrentCamera( pCam);

			// Aspect Ratio�� ����
			pCam->SetPerspective( pCam->GetFOV(), (REAL32) g_pViewer->GetViewWidth(), (REAL32) g_pViewer->GetViewHeight(), pCam->GetNear(), pCam->GetFar());
		}
	}
}

bool UITopMenu_V15::ClickInventory()
{
	UIMainFrame * pFrame = GetMainFrame();
	EnterSubPhase(pFrame, USP_INVENTORY);
	return true;
}

void UITopMenu_V15::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UITopMenu_V15::ReceiveGameEvent_GET_USER_INFO( INT32, const i3::user_data&)
{
}

void UITopMenu_V15::ReceiveGameEvent_GET_INVEN_INFO( INT32, const i3::user_data&)
{
}

void UITopMenu_V15::ReceiveGameEvent_GET_GIFT_LIST( INT32, const i3::user_data&)
{
}
