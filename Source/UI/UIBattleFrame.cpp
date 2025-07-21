#include "pch.h"
#include "UIBattleFrame.h"

#include "UIBattle.h"

#include "UIPopupItemBase.h"
#include "UIShopFunction.h"
#include "UIUtil.h"

#include "../IGShopContext.h"
#include "../StageBattle.h"

#include "BattleGui.h"
#include "BattleHud.h"
#include "Minimap.h"

#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleServerContext.h"

#include "UIHudMultiWeapon.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"
#include "../ObserverBattle.h"

I3_CLASS_INSTANCE( UIBattleFrame); //, i3GameNode);

namespace
{
	class UIPopupFocusCallback_BattleFrame : public UIPopupFocusCallback
	{
	public:
		virtual void	ProcessInputDisableMainUI(bool bEnable)
		{
			UIBattleFrame::i()->InputDisableMainUI(bEnable);
		}
		virtual void	ProcessInputDisableFloatWindow(bool bEnable)
		{

		}
	} g_popup_focus_cb;
}


UIBattleFrame::UIBattleFrame() : m_shop_ctrl(nullptr), m_numRenderPopup(0)
{	
	m_popup_focus.SetCallback(&g_popup_focus_cb);

	m_shop_ctrl = new igshop_ctrl;
	MEMDUMP_NEW( m_shop_ctrl, sizeof( igshop_ctrl));

	i3mem::FillZero( m_pHUD, sizeof( m_pHUD));
	i3mem::FillZero( m_pPopup, sizeof( m_pPopup));

	m_InfoEquipWeapon.Reset();

	RegisterGameEventFunc( EVENT_OPEN_IGSHOP,				&UIBattleFrame::ReceiveGameEvent_Open_IGShop);
	RegisterGameEventFunc( EVENT_KICK_VOTE_SUGGEST_RESULT,	&UIBattleFrame::ReceiveGameEvent_KickVote_Suggest_Result);
	RegisterGameEventFunc( EVENT_KICK_VOTE_START,			&UIBattleFrame::ReceiveGameEvent_KickVote_Start);
	RegisterGameEventFunc( EVENT_KICK_VOTE,					&UIBattleFrame::ReceiveGameEvent_KickVote_Update);
}

UIBattleFrame::~UIBattleFrame()
{
	// ������� ���� ���������ش�..�κ�� ������ �����������Թ����� ���� ����...

	if (LocaleValue::Enable("FieldShop"))
	{

		if( BattleServerContext::i()->IsMovingStage() == false)
		{
			UserInfoContext::i()->SetMyUserInfoEquipmentWeapon(m_InfoEquipWeapon);
		}
	}

	INT32 i;

	for( i = 0; i < UIHUD_MAX; i++)
	{
		if(m_pHUD[ i])
			m_pHUD[ i]->Destroy();
	}

	for( i = 0; i < UBP_MAX; i++)
	{
		if(m_pPopup[ i])
			m_pPopup[ i]->Destroy();
	}

	I3_SAFE_DELETE( m_shop_ctrl);

	minimap_new::Minimap::Destroy();

	GameUI::ReleaseLoadingIcon();

	i3VirtualTexture* pVTex = i3UI::getVirtualTexture();
	pVTex->getCache()->TrashAllTexture();
}


void	UIBattleFrame::FocusPopup(UIPopupBase* popup, bool bFocus)
{
	if (bFocus)
		m_popup_focus.AddFocusPopup(popup);
	else
		m_popup_focus.RemoveFocusPopup(popup);
}

void	UIBattleFrame::InputDisableMainUI(bool bEnable)
{
	if( bEnable)
		g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);
	else
		g_pFramework->SetGameKeyMode( GAMEKEY_MODE_GAME);
}

void    UIBattleFrame::InputDisable(bool bEnable)
{
	INT32 i;

	for( i = 0; i < UIHUD_MAX; i++)
	{
		if(m_pHUD[ i])
			m_pHUD[ i]->SetInputDisable(bEnable);
	}
	for( i = 0; i < UBP_MAX; i++)
	{
		if(m_pPopup[ i])
			m_pPopup[ i]->SetInputDisable(bEnable);
	}
}

void UIBattleFrame::OnCreate( void)
{
	m_numRenderPopup = 0;
	INT32 i;

	i3ClassMeta * pMeta = nullptr;

	if( g_pEnvSet->m_nUIVersion == UFV_1_5 || g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		// Create HUD
		for( i = 0; i < UIHUD_MAX; i++)
		{
			pMeta = nullptr;

			switch(i)
			{
			case UIHUD_FORCE_REMOVE:	pMeta = UIHUDForceRemove::static_meta(); break;
			case UIHUD_NOTICE_SHOP:	pMeta = UIHUDNoticeShop::static_meta();	 break;
			case UIHUD_RECORD_BOARD:	pMeta = UIHUDRecordBoard::static_meta();	break;
			case UIHUD_INGAMECHATING:	pMeta = UIHUDIngameChat::static_meta(); break;
			case UIHUD_MANAGER:			pMeta = UIHudManager::static_meta();	break;
			case UIHUD_PLAYTIME_ALARM:	pMeta = UIHUDPlayTimeAlarm::static_meta(); break;
			case UIHUD_TABMINIMAP:		pMeta = UIHUDTabMinimap::static_meta();	break;
			case UIHUD_OBSERVER:		pMeta = UIHUDObserver::static_meta(); break;
//			default :					pMeta = nullptr; break;
			}

			if( pMeta)
			{
				m_pHUD[ i] = (UIHUDBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
				I3_MUST_ADDREF(m_pHUD[ i]);
				m_pHUD[ i]->OnCreate( this);
			}
		}

		pMeta = nullptr;

		// Create Popup
		for( i = 0; i < UBP_MAX; i++)
		{
			switch(i)
			{
			case UBP_ACCUSE_CRIME :		pMeta = UIBattlePopupAccuseCrime::static_meta(); break;
#ifdef USE_REPORT
			case UBP_USER_REPORT :		pMeta = UIBattlePopupUserReport::static_meta(); break;
#endif
			//case UBP_INGAME_SHOP:			pMeta = UIBattlePopupShop::static_meta(); break;
			
			//case UBP_BUYITEM:				pMeta = UIBattlePopupBuyItem::static_meta(); break;
			//case UBP_DELETEITEM:			pMeta = UIBattlePopupDeleteItem::static_meta(); break;
			
			case UBP_SELECT_WEAPON:			pMeta = UIBattlePopupSelectWeapon::static_meta(); break;
			case UBP_SELECT_CHARA:			pMeta = UIBattlePopupSelectChara::static_meta(); break;
			case UBP_OPTION:				pMeta = UIPopupOption::static_meta(); break;
			case UBP_EXIT :					pMeta = UIBattlePopupExit::static_meta(); break;
			case UBP_HUD_USURPATION:		pMeta = UIHudUsurpation::static_meta(); break;
			case UBP_HUD_ESCAPE:			pMeta = UIHUDEscape::static_meta(); break;
			// Domination��
			case UBP_SKILL_SHOP:			break;

			case UBP_TWITCHOPTION :			pMeta = UIPopupTwitchOption::static_meta(); break;

			case UBP_HUD_SPACE:				pMeta = UIHudSpace::static_meta(); break;

			default :						pMeta = nullptr; break;
			}

			if( pMeta)
			{
				m_pPopup[ i] = (UIBattlePopupBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
				I3_MUST_ADDREF(m_pPopup[ i]);
				m_pPopup[ i]->OnCreate( this);
			}
		}
	}
	else
	{
		// Create HUD
		for( i = 0; i < UIHUD_MAX; i++)
		{
			switch(i)
			{
			case UIHUD_FORCE_REMOVE:	pMeta = UIHUDForceRemove::static_meta(); break;
				//case UIHUD_NOTICE_SHOP:	pMeta = UIHUDNoticeShop::static_meta();	 break;
			case UIHUD_RECORD_BOARD:	pMeta = UIHUDRecordBoard::static_meta();	break;
			case UIHUD_INGAMECHATING:	pMeta = UIHUDIngameChat::static_meta(); break;
			case UIHUD_MANAGER:			pMeta = UIHudManager::static_meta();	break;
			case UIHUD_PLAYTIME_ALARM:	pMeta = UIHUDPlayTimeAlarm::static_meta(); break;
			case UIHUD_TABMINIMAP:		pMeta = UIHUDTabMinimap::static_meta();	break;
			case UIHUD_OBSERVER:		pMeta = UIHUDObserver::static_meta(); break;
			default :					pMeta = nullptr; break;
			}

			if( pMeta)
			{
				m_pHUD[ i] = (UIHUDBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
				I3_MUST_ADDREF(m_pHUD[ i]);
				m_pHUD[ i]->OnCreate( this);
			}
		}

		pMeta = nullptr;

		// Create Popup
		for( i = 0; i < UBP_MAX; i++)
		{
			switch(i)
			{
			case UBP_ACCUSE_CRIME :		pMeta = UIBattlePopupAccuseCrime::static_meta(); break;
#ifdef USE_REPORT
			case UBP_USER_REPORT :		pMeta = UIBattlePopupUserReport::static_meta(); break;
#endif
				//		case UBP_INGAME_SHOP:			pMeta = UIBattlePopupShop::static_meta(); break;
				//
				//		case UBP_BUYITEM:				pMeta = UIBattlePopupBuyItem::static_meta(); break;
				//		case UBP_DELETEITEM:			pMeta = UIBattlePopupDeleteItem::static_meta(); break;
				//
			case UBP_SELECT_WEAPON:			pMeta = UIBattlePopupSelectWeapon::static_meta(); break;
			case UBP_SELECT_CHARA:			pMeta = UIBattlePopupSelectChara::static_meta(); break;
			case UBP_OPTION:				pMeta = UIPopupOption::static_meta(); break;
			case UBP_EXIT :					pMeta = UIBattlePopupExit::static_meta(); break;
				//		case UBP_HUD_USURPATION:		pMeta = UIHudUsurpation::static_meta(); break;
				//
				// Domination��
			//case UBP_SKILL_SHOP:
			//	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DOMINATION))
			//		pMeta = UIBattlePopupSkillShop::static_meta();
			//	break;

			case UBP_HUD_ESCAPE:			pMeta = UIHUDEscape::static_meta(); break;
			case UBP_TWITCHOPTION :			pMeta = UIPopupTwitchOption::static_meta(); break;

			case UBP_HUD_SPACE:				pMeta = UIHudSpace::static_meta(); break;

			default :						pMeta = nullptr; break;
			}

			if( pMeta)
			{
				m_pPopup[ i] = (UIBattlePopupBase*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
				I3_MUST_ADDREF(m_pPopup[ i]);
				m_pPopup[ i]->OnCreate( this);
			}
		}
	}


	if ( LocaleValue::Enable("FieldShop") )
	{
		UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(m_InfoEquipWeapon);
	}
}

/*virtual*/ void UIBattleFrame::OnUpdate( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	m_shop_ctrl->OnUpdate(rDeltaSeconds);
	i3GameNode::OnUpdate( rDeltaSeconds);
}

void	UIBattleFrame::SetGameShopValidTime(INT32 sec)
{
	m_shop_ctrl->SetGameShopValidTime(sec);
}


void UIBattleFrame::SetRespawn()
{
	m_shop_ctrl->SetRespawn();
}

void	UIBattleFrame::SetDeath()
{
	m_shop_ctrl->SetDeath();
} 

void	UIBattleFrame::SetDeathDelay()
{
	m_shop_ctrl->SetDeathDelay();
}


void	UIBattleFrame::SetRoundEnd()
{
	UIBattleFrame::TogglePopup(UBP_NONE);				  // ��Ʋ�������� ��� �˾��� �� �ݾƹ�����..
	m_shop_ctrl->SetRoundEnd();
}


void	UIBattleFrame::SetGameEnd()
{
	m_shop_ctrl->SetGameEnd();
}

void	UIBattleFrame::SetIntermissionStart()
{
	m_shop_ctrl->SetIntermissionStart();
}

void	UIBattleFrame::SetIntermissionEnd()
{
	m_shop_ctrl->SetIntermissionEnd();
}


/*virtual*/ bool UIBattleFrame::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	INT32 i;

	for( i = 0; i < UIHUD_MAX; i++)
	{
		if( m_pHUD[ i] && m_pHUD[ i]->isEnable())
		{
			m_pHUD[ i]->OnEvent( event, pObj, param2, code);
		}
	}

	for( i = 0; i < UBP_MAX; i++)
	{
		if(  m_pPopup[ i] && m_pPopup[ i]->isEnable())
		{
			if( m_pPopup[ i]->OnEvent( event, pObj, param2, code))
				return true;
		}
	}

	if( code == I3_EVT_CODE_ACTIVATE)
	{
		switch( event)
		{
		case I3_EVT_INPUT :
			{
				i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
				i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

				if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
				{
					if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon()
						&& HUD::instance()->IsEnableHud(HUD::MULTIWEAPON) == TRUE)
					{
						// ���߹��� HUD�� ���������� ����
						UIHudMultiWeapon* pHud = 0;
						pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
						pHud->ShowMultiWeaponSlot();
					}
					else
					{	// �ٸ� �˾��̳� HUD���� ó���Ѱ� ������ Exitâ�� ���� ó���� ����
						CBattleGui::i()->SetEnablePopup();
					}
				}
			}
			break;
		}
	}

	return false;
}

void UIBattleFrame::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
	
	/// ȣ�� ������ ��������... ////
	for(INT32 i = 0; i < UIHUD_MAX; i++)
	{
		if(i == UIHUD_INGAMECHATING)
			continue;

		if( m_pHUD[ i] && m_pHUD[ i]->isEnable())
			m_pHUD[ i]->ProcessGameEvent( evt, arg, UserData);
	}
}

void UIBattleFrame::EnableHUD( UIHUD hud, bool bEnable)
{
	if ( m_pHUD[hud] == nullptr || m_pHUD[hud]->isEnable() == static_cast<bool>(bEnable))	// ��ġ�ϸ� ƨ�ܳ���..
		return;
	
	I3_BOUNDCHK( hud, UIHUD_MAX);
	I3ASSERT( m_pHUD[ hud]);

	if (bEnable)
	{
		bool res = m_pHUD[hud]->OnEntranceStart();
		I3ASSERT(res);
	}
	else
	{
		bool res = m_pHUD[hud]->OnExitStart();
		I3ASSERT(res);
	}
}

void UIBattleFrame::UpdateChatBuffer( )
{
	I3ASSERT( m_pHUD[ UIHUD_INGAMECHATING]);

	if( m_pHUD[UIHUD_INGAMECHATING] != nullptr)
	{
		((UIHUDIngameChat*)m_pHUD[UIHUD_INGAMECHATING])->UpdateChatBuffer();
	}
}

INT32 UIBattleFrame::FindPopup( LuaState * L)
{
	INT32 i;
	i3UIScene * pScene;

	for( i = 0; i < UBP_MAX; i++)
	{
		if(!m_pPopup[i])//���ٰ� ũ���� ��������.
			continue;
		pScene = m_pPopup[ i]->GetScene( 0);

		if( pScene != nullptr)
		{
			if( pScene->getLuaContext() == L)
				return i;
		}	
	}

	return -1;
}

UIBATTLE_POPUP UIBattleFrame::FindPopupByPointer( UIBattlePopupBase * pPopup)
{
	INT32 i;

	for( i = 0; i < UBP_MAX; i++)
	{
		//I3ASSERT( m_pPopup[ i]);
		if( m_pPopup[ i] == pPopup)
			return (UIBATTLE_POPUP)i;
	}

	return UBP_NONE;
}

void UIBattleFrame::TogglePopup( UIBATTLE_POPUP type, void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	UINT32 i;

	if( UBP_NONE == type)
	{	// ��� �ݱ�
		for( i = 0; i < UBP_MAX; i++)
		{
			if( m_pPopup[i] &&m_pPopup[i]->isEnable())
			{
				m_pPopup[i]->OnExitStart();
				FocusPopup(m_pPopup[i], false);
			}
		}
	}
	else
	{
		I3_BOUNDCHK( type, UBP_MAX);

		if( m_pPopup[type] && m_pPopup[type]->isEnable())
		{	// �ش� â�� ���������� �ݱ�
			if( m_pPopup[type]->OnExitStart())
				FocusPopup(m_pPopup[type], false);
		}
		else
		{
			// �������� �ݰ�
			for( i = 0; i < UBP_MAX; i++)
			{
				if( m_pPopup[i] && m_pPopup[i]->isEnable())
				{
					m_pPopup[i]->OnExitStart();
					FocusPopup(m_pPopup[i], false);
				}
			}

			// �ش� â�� ����.
			if( m_pPopup[type] && m_pPopup[ type]->OnEntranceStart( pArg1, pArg2))
				FocusPopup(m_pPopup[type], true);
		}
	}
}

void	UIBattleFrame::OpenPopup(UIBATTLE_POPUP type, void * pArg1, void * pArg2)
{
	if( type != UBP_NONE)
	{
		if ( m_pPopup[type] && m_pPopup[type]->isEnable() == false)
		{
			if( m_pPopup[ type]->OnEntranceStart(pArg1, pArg2))
			{
				FocusPopup( m_pPopup[ type], true);
			}
		}
	}

}
 


void	UIBattleFrame::ClosePopup( UIBATTLE_POPUP type)
{
	if( type != UBP_NONE)
	{
		if ( m_pPopup[type] && m_pPopup[type]->isEnable() == true )
		{
			if( m_pPopup[ type]->OnExitStart())
				FocusPopup(m_pPopup[ type], false);
		}
	}
	
}

void	UIBattleFrame::CloseAll()
{
	// �˾�â�� ���������� ����..
	for( INT32 i = 0; i < UBP_MAX; ++i)
	{
		if( m_pPopup[i] && m_pPopup[i]->isEnable())
		{
			m_pPopup[i]->SetEnable(false);
			m_pPopup[i]->OnExitStart();
			FocusPopup(m_pPopup[i], false);
		}
	}
	
	//
	for( INT32 i = 0; i < UIHUD_MAX; ++i)
	{
		if ( m_pHUD[i] && i != UIHUD_MANAGER && m_pHUD[i]->isEnable() ) //UIHUD_MANAGER�� GameResult(���� ���â ������ �ʰ� ����.)
			m_pHUD[i]->SetEnable(false);
	}
}


void	UIBattleFrame::AddPopupCount(INT32 cnt)
{
	m_numRenderPopup += cnt;
}

bool	UIBattleFrame::IsOpenPopup( UIBATTLE_POPUP type )
{
	if( type != UBP_NONE )
		return (m_pPopup[type]!=nullptr && m_pPopup[type]->isEnable()) ? true : false;

	return false;
}

void UIBattleFrame::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIBattleFrame::ReceiveGameEvent_Open_IGShop( INT32 arg, const i3::user_data&)
{
	// ������ ������ Ȯ���� �����̹Ƿ� ��۸� ó���Ѵ�..
	UINT64 press  = g_pFramework->getKeyPress();

	if (press & GAME_KEY_CHANGEWEAPON )		// ���ⱳü Űó���� StageBattle�� ����..���߿� �ٸ� ������ ó���ϴ°� �������̴�.
		return;

	if (observer::IsObserverState()) return;

	if ( CBattleGui::i()->GetEnablePopup() == false )		// �� UI�� �˾��� �����ִٸ� ó���� ���� �ʵ��� ����..
	{
		this->TogglePopup(UBP_INGAME_SHOP);
	}
}

//��ǥ ����(�Ű��ڿ�)
void UIBattleFrame::ReceiveGameEvent_KickVote_Suggest_Result( INT32 arg, const i3::user_data&)
{
	i3::rc_wstring wstrMsg;
	switch(arg)
	{
	case	EVENT_ERROR_SUCCESS:
		/*���� ���� ��ǥ�� �³��Ǿ� ��ǥ �����մϴ�.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_OK");
		EnableHUD(UIHUD_FORCE_REMOVE, true); //��ǥ ui ȣ��
		break;

	case EVENT_ERROR_KICKVOTE_ALREADY_EXIST_OTHER_VOTE:
		/*�̹� �ٸ� ��ǥ�� ���� ���Դϴ�.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_ALREADY");
		break;

	case EVENT_ERROR_KICKVOTE_SAME_SUGGEST_IN_ONE_MINUTE:
		/*��ǥ ������ 1�� �̳��� ���� �Ű��� �Ǵ� ���� �ǽŰ��ڿ��� �Ű��Ҽ� �����ϴ�.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_TIMELIMIT");
		break;

	case EVENT_ERROR_KICKVOTE_SMALL_USER_IN_ROOM:
		/*��ȿ� ��ǥ �ο��� 2�� ���϶� ������ �� �����ϴ�.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_NUMPLAYERS");
		break;

	case EVENT_ERROR_KICKVOTE_NOT_ENOUGH_VOTE_TIME:
		/* ���� ���� �ܿ��ð��� �� ���� �ʾ� ��ǥ ������ �ǹ̰� �����ϴ�. */
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_REMAINTIME");
		break;
	case EVENT_ERROR_KICKVOTE_UNDER_RANK_LIMIT:
		/* �ּ� ��ũ�� �������� �ʾ� �Ű��� �� ���� */
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAILRANK");
		break;
	case EVENT_ERROR_KICKVOTE_SAME_SUGGEST_IN_GUARD_TIME:
		/* �ǽŰ��� ��ȣ�Ⱓ */
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_SAFETIME"); 
		break;
	case EVENT_ERROR_KICKVOTE_MAX_VOTE_COUNT_OVER:
		/* ���� �ִ� �Ű� Ƚ�� �ʰ� */
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_START_FAIL_VOTE_MAX"); 
		break;
	default: 
		/*�� �� ���� �����Դϴ�.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_UNKNOWN_ERROR");
		break;
	}

	g_pFramework->PutSystemChatting(wstrMsg);
}

void UIBattleFrame::ReceiveGameEvent_KickVote_Start( INT32 arg, const i3::user_data&)
{
	//��ǥ ����(�� �Ű��ڿ�)
	if ( GetPopup(UBP_ACCUSE_CRIME)->isEnable() == true )
		ClosePopup(UBP_ACCUSE_CRIME);

	EnableHUD(UIHUD_FORCE_REMOVE, true); //��ǥ ui ȣ��
}


// ��ǥ�� ����� �ް� �ǰ�, VOTE_COUNT��Ŷ�� ���� �����Ƿ� ���� ī��Ʈ�ؾߵ˴ϴ�. (�Ű��ڿ�)
void UIBattleFrame::ReceiveGameEvent_KickVote_Update( INT32 arg, const i3::user_data&)
{
	i3::rc_wstring wstrMsg;

	switch (arg)
	{
	case EVENT_ERROR_SUCCESS:	
		/*��ǥ�� �ݿ��Ǿ����ϴ�.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_VOTE_OK"); break;

	case EVENT_ERROR_KICKVOTE_NOTAPPLY_NONE_VOTE:
		// �������� ��ǥ�� �����ϴ�. ( ��ǥ�� ����Ǹ�, �������� ��ǥ���� ��Ŷ�� ������ Ŭ���̾�Ʈ�� ��ǥ ��Ŷ�� ������ ���� ���� ������ �߻� ���ɼ� �����ϴ�. )
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_VOTE_FAIL_NOVOTE"); break;

	case EVENT_ERROR_KICKVOTE_NOTAPPLY_ALREADY_VOTED:
		// �̹� ��ǥ�� �߽��ϴ�. ( ��ǥ�� ó���ϸ�, Ŭ���̾�Ʈ ��ü������ �� �̻� ��ǥ ��Ŷ�� ������ ���� �Ǳ� ������ �߻� ���ɼ� ���� �����ϴ�. �� ������ ���� �߻��Ѵٸ�, ���׳� ��ŷ �ǽ� )
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_VOTE_FAIL_ALREADY"); break;
	case 	EVENT_ERROR_KICKVOTE_NOTAPPLY_NO_RIGHT_TO_VOTE:		
		// ��ǥ���� �����ϴ�. ( ��ǥ���� ���� �Ű���, �ǽŰ���, �����ڴ� Ŭ���̾�Ʈ ��ü������ ��ǥ ��Ŷ�� ������ ���� ������ �߻� ���ɼ� �ظ� �����ϴ�. �� ������ ���� �߻��Ѵٸ�, ���׳� ��ŷ �ǽ� )
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_VOTE_FAIL_NORIGHT"); break;

	default:
		/*�� �� ���� �����Դϴ�.*/
		wstrMsg = GAME_RCSTRING("STR_ACK_FORCIBLYREMOVE_UNKNOWN_ERROR");
		break;
	}
	g_pFramework->PutSystemChatting(wstrMsg);
}

#include "UIHudManager.h"
namespace CrossCounter_Util
{
	UIBATTLE_POPUP GetStyle()
	{
		if( UIBattleFrame::i()->IsOpenPopup(UBP_OPTION) == true ) return UBP_NONE; //�ɼ�â�� �������� �۵� �� �ǰ� �Ѵ�.

		CGameCharaBase* pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->GetMySlotIdxEx());
		if( pMyChara == nullptr)
		{
			UIBattleFrame* pFrame = UIBattleFrame::i();
			if( pFrame != nullptr)
			{
				if( pFrame->IsOpenPopup( UBP_SELECT_WEAPON))		return UBP_SELECT_WEAPON;
				else if( pFrame->IsOpenPopup( UBP_SELECT_CHARA))	return UBP_SELECT_CHARA;
				else												return UBP_SELECT_WEAPON;
			}
			else return UBP_SELECT_WEAPON;
		}

		return (pMyChara->IsDino()) ? UBP_SELECT_CHARA : UBP_SELECT_WEAPON;
	}
};

