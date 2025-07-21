#include "pch.h"
#include "UISideMenu.h"
#include "UISideFlyMenu.hpp"

#include "UICharaInfoMgr.h"
#include "UIMainFrame.h"

#include "UI/UIUtil.h"

#include "UISMenu_Ability.h"
#include "UISMenu_Equip.h"

#include "UIFloatCharacterView.h"

#include "UISignal.h"
#include "i3Base/itl/bind/bind.h"

I3_CLASS_INSTANCE( UISideMenu);//, UIBase);

namespace
{
	UISideMenu* g_this = nullptr;

	MultiSceneInfo g_scn_info, g_fly_info;
	//bool g_is_sub_opened = false;	//sub menu Ȱ��ȭ ����
	bool g_is_equip_sub_opened = false;  // ��������.

	void sm_btn_lock(int state);	//��ư disable �Ѵ�.
	void sm_btn_unlock(int state);	//��ư enable �Ѵ�.
	void sm_btn_unselected(int state);	//���õ� ���¸� ����Ѵ�.
	void sm_btn_enable(int state);	//�ش� ��ư�� Ȱ��ȭ �Ѵ�.
	void sm_btn_init();	//��ư ��Ȳ�� �°� �ʱ� ���·� �����Ѵ�.

	GameUI::AutoPosition g_btn_pos;

	class NullSMenu : public iSMenu
	{
	public:
		NullSMenu(UISideMenu* p) : iSMenu(p) {}
		virtual ~NullSMenu() {}

		virtual void OnCreate( i3GameNode * pParent) final {}
		virtual void OnLoadAllScenes() final {}
		virtual void OnUnloadAllScenes() final {}
		virtual void Entrance(bool arg1 = false) final {}
		virtual void Exit(void* arg1= nullptr, void* arg2 = nullptr) final {}
	};
	NullSMenu * g_pNullMenu = nullptr;
	iSMenu* g_curr_menu = nullptr;
	i3UIButton* curr_enable_btn = nullptr;
}

UISideMenu* UISideMenu::instance()
{
	//���� ������ ������ �ֱ� ������ �Ϲ����� �̱��� �������� �ϸ� �ȵȴ�.
	return g_this;
}

/***********************//***********************/
extern "C" 
{
	int sidemenu_click( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked( value );
		return 0;
	}
}

LuaFuncReg sidemenu_glue[] =
{
	{ "Click", sidemenu_click	},
	{ nullptr, nullptr}
};

/***********************//***********************/
void UISideMenu::clicked(int idx)
{
	switch( idx )
	{
	case SIDEMENU_BTN_EQUIP:
		if(g_is_equip_sub_opened == false)
		{
			sm_btn_enable(SIDEMENU_BTN_EQUIP);
			smenu_click_equip( true);
			g_is_equip_sub_opened = true;
		}
		else
		{
			//������ ��ȯ ��Ŷ ����
			send_change_equip();
			g_is_equip_sub_opened = false;
		}

		mSubList[SIDEMENU_BTN_ABILITY]->Exit();
		break;
	case SIDEMENU_BTN_ABILITY:	
		if( mSubList[SIDEMENU_BTN_ABILITY]->IsOpenedSubMenu() == false)
		{
			mSubList[SIDEMENU_BTN_ABILITY]->Entrance(true);
			setAbilityButton(true);
		}
		else
		{
			mSubList[SIDEMENU_BTN_ABILITY]->Exit();
			setAbilityButton(false);
		}
		sm_btn_enable(SIDEMENU_BTN_ABILITY);	// ��ư�� �׻� Ȱ��ȭ �Ǿ����� ��
		mSubList[SIDEMENU_BTN_EQUIP]->Exit();
		break;
	}
}

/***************************************/
void UISideMenu::smenu_click_equip( bool bOpen, INT32 arg)
{
	//arg1 : �ݵ�� open(true), ���� ���¸� ����, �� ���� ���¸� skip(false)
	//arg2 : ���� �ʱ�ȭ(true), �� �ʱ�ȭ(false), ����Ŭ���� main slot�� ���� ������ ����
	bool equip_open = true;

	if( bOpen == false && g_is_equip_sub_opened == false )
		equip_open = false;

	if( equip_open == true)
	{
		if( arg == 1 )
		{
			//�ش� ĳ���� ������ �����Ѵ�.
			m_pCharaInfoMgr->list_up( CCharaInfoContext::i()->GetMainCharaSlotIdx());
		}
		else if( arg == 2 )
		{
			CharaInfoMgr::CurrentState & state = m_pCharaInfoMgr->m_current_state;
			if( state.slot_idx != UINT_MAX )
			{
				state.info = m_pCharaInfoMgr->find( CCharaInfoContext::i()->GetMainCharaSlotIdx(), state.ctg, state.slot_idx );
			}
		}
		else if( arg == 3 ) //��ü ���� ����
		{
			m_pCharaInfoMgr->list_up();
		}

		if( g_curr_menu == mSubList[SIDEMENU_BTN_EQUIP])
		{
			mSubList[SIDEMENU_BTN_EQUIP]->Entrance(true);
		}
	}
}

void UISideMenu::cb_smenu_ability_close()
{
	if( g_curr_menu == mSubList[SIDEMENU_BTN_ABILITY] )
	{
		mSubList[SIDEMENU_BTN_ABILITY]->Exit();
		sm_btn_enable(SIDEMENU_BTN_ABILITY);

		setAbilityButton(false);
	}
}

bool UISideMenu::clicked_escape_key()
{
	//���� �޴��� Ȱ��ȭ ���¸� esc Ű�� ��Ȱ��ȭ
	if( GameUI::IsKeyboardStroked(I3I_KEY_ESC) == true )
	{
		if( g_is_equip_sub_opened == true || mSubList[SIDEMENU_BTN_ABILITY]->IsOpenedSubMenu() == true)
		{
			if( g_curr_menu == mSubList[SIDEMENU_BTN_ABILITY] )
				cb_smenu_ability_close();
			else
			{
				//������ ��ȯ ��Ŷ ����
				send_change_equip();
			}

			return true;
		}
	}

	return false;
}

/***********************//***********************/
UISideMenu::UISideMenu() : mEntranced(false), m_bChangeEquipFromRepairAll(false)
{
	g_this = this;
	m_pCharaInfoMgr = CharaInfoMgr::instance();

	g_pNullMenu = new NullSMenu(this);
	MEMDUMP_NEW( g_pNullMenu, sizeof( NullSMenu));

	g_curr_menu = g_pNullMenu;

	mSubList.resize( SIDEMENU_BTN_ALL );
	mSubList[SIDEMENU_BTN_EQUIP] = new SMenuEquip(this);
	MEMDUMP_NEW( mSubList[SIDEMENU_BTN_EQUIP], sizeof( SMenuEquip));

	mSubList[SIDEMENU_BTN_ABILITY] = new SMenuAbility(this);
	MEMDUMP_NEW( mSubList[SIDEMENU_BTN_ABILITY], sizeof( SMenuAbility));

	RegisterGameEventFunc( EVENT_INVEN_CHANGE_CHARA_EQUIP,	&UISideMenu::ReceiveGameEvent_Change_CharaEquip);
}

UISideMenu::~UISideMenu()
{
	g_curr_menu = nullptr;
	g_is_equip_sub_opened = false;
	curr_enable_btn = nullptr;

	OnUnloadAllScenes();

	I3_SAFE_DELETE( g_pNullMenu);

	struct Fn {	void operator()(iSMenu*p) {	I3_SAFE_DELETE( p); } };
	i3::for_each(mSubList.begin(), mSubList.end(), Fn() );

	mSubList.clear();
}

/***********************//***********************/
/*virtual*/ void UISideMenu::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad(pScene);

	i3::string script_file_name( pScene->getScriptFile() );
	bool isctr1 = i3::generic_is_iequal(script_file_name, i3::string("script\\pointBlankre_sidemenu.lua"));
	bool isctr2 = i3::generic_is_iequal(script_file_name, i3::string("Script\\PointBlankRe_SideMenu3.lua"));
	if( isctr1 == true )
	{
		m_FrameWnd[SIDEMENU_BTN_EQUIP] = static_cast<i3UIFrameWnd*>( pScene->FindChildByName( "i3UIFrameWnd_Equip") );
		m_FrameWnd[SIDEMENU_BTN_ABILITY] = static_cast<i3UIFrameWnd*>( pScene->FindChildByName( "i3UIFrameWnd_Ability") );

		m_Btn[SIDEMENU_BTN_EQUIP] = static_cast<i3UIButton*>( pScene->FindChildByName( "button_equip") );
		m_Btn[SIDEMENU_BTN_ABILITY] = static_cast<i3UIButton*>( pScene->FindChildByName( "button_ability") );

		m_AbilityBtnPosY = m_FrameWnd[SIDEMENU_BTN_ABILITY]->getPos()->y;
	}
	else if( isctr2 == true)
	{
		fly::_InitializeAtLoad(pScene);
	}
}

/*virtual*/ void UISideMenu::_ClearAtUnload( i3UIScene * pScene)
{
	UIBase::_ClearAtUnload(pScene);
}

/*virtual*/ void UISideMenu::OnCreate( i3GameNode * pParent)
{
	UIBase::OnCreate( pParent);

	g_scn_info.idx = GetSceneCount();
	AddScene("Scene/Main/PointBlankRe_SideMenu.i3UIs", sidemenu_glue, true, false);

	g_fly_info.idx = GetSceneCount();
	//AddScene("Scene/Main/PointBlankRe_SideMenu3.i3UIs", 0, true, false);

	struct Fn	 
	{
		i3GameNode * parent;
		Fn(i3GameNode * p) : parent(p) {}

		void operator()(iSMenu* p) {	p->OnCreate(parent); 	}
	};
	i3::for_each(mSubList.begin(), mSubList.end(), Fn(pParent) );

	SetEnable( true );

	//fly::close_fly();
}

/*virtual*/ void UISideMenu::OnLoadAllScenes()
{
	UIBase::OnLoadAllScenes();

	//��� �ݹ� �Լ� ���
	m_sigConnection += UISignal::SMenuAbilityClose.connect(i3::bind(&UISideMenu::cb_smenu_ability_close, this));

	//Scene �� ���
	g_scn_info.scn = GetScene( g_scn_info.idx );
	LuaState * L = _CallLuaFunction(g_scn_info.idx, "LoadAllScenes");
	UILua::EndLuaFunction(L , 0);

	g_fly_info.scn = GetScene( g_fly_info.idx );

	struct Fn	 {  void operator()(iSMenu* p) {	p->OnLoadAllScenes(); } };
	i3::for_each(mSubList.begin(), mSubList.end(), Fn() );

	for(int i=0; i<SIDEMENU_BTN_ALL;++i)
	{
		i3UIFrameWnd* wnd = get_frm_wnd(i);
		wnd->EnableCtrl(false);
		wnd->setInputDisable(false);
	}
}

bool UISideMenu::OnExitStart( void)
{
	//OnExitStart != OnUnloadAllScenes �ٸ� ������ ������ OnUnloadAllScenes �� �Լ��� �ۼ��ߴµ�.
	//UIImgSetCtrl �� Ŭ�������� ���� ������ RestoreSizePos �� �Լ��� ȣ���ؾ� �ȴ�. (���� : ���ø��� �ٸ� ��� �̹����� �̻��ϰ� ����)
	//�ٵ�. RestoreSizePos �� �Լ��� ���ҽ��� ���������� ������ �Ǿ� �־ OnUnloadAllScenes �� ������ �ʹ� �ʴ�.(ũ���� �߻�)
	//�׷��� OnExitStart ������ �ڽ��� OnUnloadAllScenes �� �Լ��� ȣ���Ѵ�.

	//��� �ݹ� �Լ� �����ֱ�
	//UI::remove_cb(this);
	m_sigConnection.clear();

	struct Fn	 {  void operator()(iSMenu* p) {	p->OnUnloadAllScenes(); } };
	i3::for_each(mSubList.begin(), mSubList.end(), Fn() );

	//UIBase::OnExitStart(); //UIBase::OnExitStart ������� �ʴ� ������ OnSlideOut �Լ��� ȣ���� ������ ���� �����̴�.
	return true; 
}

void UISideMenu::OnUpdate( REAL32 rDeltaSeconds)
{
	UIBase::OnUpdate(rDeltaSeconds);

	if( g_curr_menu != nullptr)
		g_curr_menu->OnUpdate( rDeltaSeconds );

	//fly::update_fly(rDeltaSeconds);


}

bool UISideMenu::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	if( g_curr_menu != nullptr)
		g_curr_menu->OnEvent( event, pObj, param2, code );

	return UIBase::OnEvent( event, pObj, param2, code);
}

void UISideMenu::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & f = it->second;
		(this->*f)(arg, UserData);
	}

	if( g_curr_menu != nullptr)
		g_curr_menu->ProcessGameEvent( evt, arg, UserData);
}


/*virtual*/ bool UISideMenu::OnPlayClickSound(i3UIControl * pCtrl)
{
	// ���� ��� ��ü Ȯ�ν� OK ���� ���
	if(i3::same_of<i3UIButtonComposed3*>(pCtrl) && 
		i3::generic_is_iequal(pCtrl->GetName(), "i3UIButtonC") )
	{
		return PlaySound(UISND_ITEM_CLICK_OK);
	}

	return UIBase::OnPlayClickSound(pCtrl);
}

bool UISideMenu::OnRevive( i3RenderContext * pCtx)
{
	if( UIBase::OnRevive( pCtx) == false)
		return false;

	//struct Fn	 {  void operator()(iSMenu* p) {	p->Exit(); }	};
	//i3::for_each(mSubList.begin(), mSubList.end(), Fn() );

	sm_btn_init();
	//smenu_click_equip( false, 2 );

	//g_is_equip_sub_opened = false;

	if (g_is_equip_sub_opened)
		smenu_click_equip(true);
	else
		smenu_click_equip(false, 2);

	return true;
}

i3UIButton* UISideMenu::get_current_enable_btn()
{
	return curr_enable_btn;
}

void UISideMenu::open_fly_menu() { g_fly_info.scn->Attach(false); fly::open_fly(); }
void UISideMenu::close_fly_menu() { fly::close_fly(); }

void UISideMenu::Entrance()
{
	mEntranced = true;

	m_pCharaInfoMgr->list_up();
	m_pCharaInfoMgr->m_current_state.ctg = item_def::SCTG_WEAPON;
	m_pCharaInfoMgr->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

	sm_btn_init();

	g_scn_info.scn->Attach(false);

	struct Fn	 {  void operator()(iSMenu* p) {	p->MoveBackScene(); p->Entrance(); } };
	i3::for_each(mSubList.begin(), mSubList.end(), Fn() );
}

void UISideMenu::Exit()
{
	mEntranced = false;
	CharaInfoMgr::instance()->clear();

	sm_btn_enable( SIDEMENU_BTN_NA );

	struct Fn	 {  void operator()(iSMenu* p) {	p->Exit(); } };
	i3::for_each(mSubList.begin(), mSubList.end(), Fn() );

	g_curr_menu = g_pNullMenu;
	g_is_equip_sub_opened = false;
}

void UISideMenu::Notify(const char* msg, void * pArg)
{
	if( i3::generic_is_equal( msg, "menu_init") )
	{
		m_pCharaInfoMgr->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

		sm_btn_init();
	}
}

void UISideMenu::send_change_equip( void)
{
	if( false == m_pCharaInfoMgr->send_event_change_equip() )
	{
	//	INT32 arg = 0;
	//	UI::call_cb( "smenu_equip_click_close", &arg );

		UISignal::SMenuEquipClickClose(false);

		g_is_equip_sub_opened = false;
	}
}

void UISideMenu::setAbilityButton(bool _enable)
{
	if(_enable)
	{
		VEC3D* curPos = m_FrameWnd[SIDEMENU_BTN_ABILITY]->getPos();
		m_FrameWnd[SIDEMENU_BTN_ABILITY]->setPos(curPos->x, m_AbilityBtnPosY-200, curPos->z);
		curPos = m_FrameWnd[SIDEMENU_BTN_ABILITY]->getPos();
		m_Btn[SIDEMENU_BTN_ABILITY]->SetTemplateByName("Bt_Ability");
	}
	else if(!_enable)
	{
		if(mSubList[SIDEMENU_BTN_ABILITY]->IsOpenedSubMenu())
		{
			mSubList[SIDEMENU_BTN_ABILITY]->Exit();
		}

		VEC3D* curPos = m_FrameWnd[SIDEMENU_BTN_ABILITY]->getPos();
		m_FrameWnd[SIDEMENU_BTN_ABILITY]->setPos(curPos->x, m_AbilityBtnPosY, curPos->z);
		curPos = m_FrameWnd[SIDEMENU_BTN_ABILITY]->getPos();
		m_Btn[SIDEMENU_BTN_ABILITY]->SetTemplateByName("Bt_Ability_0");
	}
}

//------------------------------------------------------------------------------//

void UISideMenu::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UISideMenu::ReceiveGameEvent_Change_CharaEquip( INT32 arg, const i3::user_data&)
{
	m_pCharaInfoMgr->change_equip();

	bool result_changed_equip = true;

	if( EV_FAILED( arg ) )
	{
		smenu_click_equip( false, 1 );

		// ĳ���� ������ ������
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REVERT_CHARA, REVERT_TYPE_ALL);

		// �˾��� ����.
		switch( arg )
		{
		case EVENT_ERROR_INVENTORY_LEAVE_MAIN_SLOT:
			{
				/*��ǥ ĳ���� ������ �� �� �Ǿ����ϴ�.*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_INCORRECT_MAIN_CHARACTER"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"));
			}
			break;
		default: // �����ڵ尡 ���� Ȯ����� �𸣹Ƿ�. 2015.08.13 �������
		//case EVENT_ERROR_INVENTORY_LEAVE_EQUIPMENT:
		//case EVENT_ERROR_EQUIPMENT_DUPLICATE_PARTS:
			{
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GUI_BASE_EQUIP_FAIL_BY_NETWORK_PROBLEM"), &GAME_RCSTRING("STR_HEADER_COUPON_USE_FAIL") );
			}
			break;
		}

		result_changed_equip = false;
	}

	if( m_bChangeEquipFromRepairAll == true )
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REVERT_CHARA, REVERT_TYPE_ALL);
		m_bChangeEquipFromRepairAll = false;
	}
	else
	{
	//	INT32 AAA = 1;
	//	UI::call_cb( "smenu_equip_click_close", &AAA );
		UISignal::SMenuEquipClickClose(true);

		g_is_equip_sub_opened = false;
	}
}

/**********************************/
namespace
{
	void sm_btn_lock(int state)
	{
		g_this->get_frm_wnd(state)->setInputDisable(true);
	}

	void sm_btn_unlock(int state)
	{
		g_this->get_frm_wnd(state)->setInputDisable(false);
	}

	void sm_btn_unselected(int state)
	{
		g_this->get_btn(state)->RemoveOnMouse();
	}

	void _ready_btn_()
	{
		for(int i=0; i<SIDEMENU_BTN_ALL;++i)
		{
			i3UIFrameWnd* wnd = g_this->get_frm_wnd(i);
			wnd->EnableCtrl(true);
			wnd->setInputDisable(true);
		}
	}

	void _open_btn_(int idx)
	{
		iSMenu * smenu = g_this->getSubSidemenu( static_cast<SIDEMENU_TYPE>(idx) );
		g_this->get_frm_wnd(idx)->EnableCtrl( smenu->IsStatusEnable() );
	}

	void _clear_btn_()
	{
		for(int i=0; i<SIDEMENU_BTN_ALL;++i)
		{
			i3UIFrameWnd* wnd = g_this->get_frm_wnd(i);
			wnd->EnableCtrl(false);
			wnd->setInputDisable(false);
		}
	}


	void sm_btn_enable(int state)
	{
		_clear_btn_();

		switch( state )
		{
		case SIDEMENU_BTN_NA:
			g_curr_menu = g_pNullMenu;
			break;

		case SIDEMENU_BTN_ALL: 
			//_ready_btn_();

			for(size_t btn_idx = SIDEMENU_BTN_EQUIP; btn_idx<SIDEMENU_BTN_ALL;++btn_idx)
			{
				sm_btn_unlock(btn_idx);
				sm_btn_unselected(btn_idx);
			}

			g_curr_menu = g_pNullMenu;
			break;

		case SIDEMENU_BTN_EQUIP:	
			_open_btn_(state); 

			sm_btn_unselected(state);

			g_curr_menu = g_this->get_list_ptr(state);
			break;
		case SIDEMENU_BTN_ABILITY:
			_open_btn_(state);

			sm_btn_unselected(state);

			g_curr_menu = g_this->get_list_ptr(state);
			break;
		}
	}

	void sm_btn_init()
	{
		switch( g_this->GetMainFrame()->GetCurrentSubPhaseType() )
		{
		case USP_INVENTORY: sm_btn_enable(SIDEMENU_BTN_NA); break;

		case USP_GIFTSHOP:
		case USP_SHOP: sm_btn_enable(SIDEMENU_BTN_ABILITY); break;

		case USP_NONE:
			switch( g_this->GetMainFrame()->GetCurrentPhaseType() )
			{
			case UIPHASE_CLANLOBBY:	sm_btn_enable(SIDEMENU_BTN_NA); break;	//�ϴ� �� ��������.
			case UIPHASE_READY_ROOM:	case UIPHASE_LOBBY:
				sm_btn_enable(SIDEMENU_BTN_EQUIP);
				break;
			}
			break;
		}
	}
}
/**********************************/