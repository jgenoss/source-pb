#include "pch.h"

#include "UIPopupOption.h"

#include "UIMainFrame.h"
#include "UIBattleFrame.h"

#include "UIOptionBase.h"
#include "UIOptionVideo.h"
#include "UIOptionControl.h"
#include "UIOptionGame.h"
#include "UIOptionAudio.h"
#include "UIOptionCommunity.h"
#include "UIOptionGrade.h"
#include "UIUtil.h"
#include "BattleGui.h"
#include "GameGUI.h"

#include "CrosshairMgr.h"
#include "EnvInfoContext.h"
#include "BattleSlotContext.h"
#include "TwitchTVContext.h"

I3_CLASS_INSTANCE( UIPopupOption);//, UITabBase);

namespace
{
	UIPopupOption* g_this = 0;
	UIOptionBase * g_curr_option = 0;

	bool g_is_twitch_broadcasting = false;
	bool g_is_video_capture = false;
	bool g_delay_show = false;	//�ΰ����� �̸� �����Ŀ� Ư�� ������ Ȱ��ȭ �Ѵ�. �ΰ��� ���۽ÿ� ȭ�鿡 ������ �ʵ��� flag ����
	bool g_close_popup = false;
	bool g_delay_lua = false;	//lua ȣ��� ���¿��� lua�� �ٽ� ȣ���� ���

	enum	{ NON =-1, 
		GAME, VIDEO,	AUDIO,	CONTROL, COMMUNITY, GRADE, UI_POPUP_MAX,
		DEFAULT = UI_POPUP_MAX, OK = 7, CANCLE = 8, };

	void CbUserVerify( void * pThis, INT32 nParam)
	{
		if( nParam == MBR_OK )
		{
			UIOptionVideo* video_tab = (UIOptionVideo*)g_this->GetOption(VIDEO);
			if( video_tab->is_modified_screen() ) ((UIOptionBase*)video_tab)->save_value();	//������ ���� �����Ѵ�.
			g_this->apply_save_option();	//����� ������ ���� ���, �����ϸ鼭 �����Ѵ�.
			g_close_popup = true;
		}
		else if( nParam == MBR_CANCEL)
		{
			g_this->load_apply_option(true);	//��� ������ �ǵ�����.
			g_curr_option->apply_lua();
		}
	}

	void CbConfirm(void* pThis,INT32 nParam)
	{
		g_this->SetInputDisable(false);
		if( nParam == MBR_OK )
		{
			UIOptionVideo* video_tab = (UIOptionVideo*)g_this->GetOption(VIDEO);
			if( video_tab->is_modified_screen() )
			{
				((UIOptionBase*)video_tab)->apply_game();

				i3::rc_wstring wstr;

				if( video_tab->is_diff_default_RefreshRate() )
					wstr = GAME_RCSTRING("STR_POPUP_Q_MESSAGE_CONTINUE_TO_CONFIRM2");
				else
					wstr = GAME_RCSTRING("STR_POPUP_Q_MESSAGE_CONTINUE_TO_CONFIRM");

				/*���� ������ ��� ����Ͻ÷��� Ȯ�� ��ư�� �������ּ���.*/
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstr, nullptr, MAKE_CALLBACK(CbUserVerify), const_cast<UIPopupOption*>(g_this) );
			}
			else
			{
				g_this->apply_save_option();
				g_close_popup = true;
			}
		}
	}

	void adjust_video_tab(i3UIScene * scn, bool disable)
	{
		i3UIButtonComposed3* btn = static_cast<i3UIButtonComposed3*>(scn->FindChildByName("i3UIButton0"));

		COLOR color;

		if (g_pEnvSet->IsBaseOnUIVersion_1_0())
		{
#if defined( LOCALE_RUSSIA)
			// 1.2 UI���� ��Ʈ ������ �ٸ��� ������ �ٸ��� ó���������... �� UI 1.0������ disable�� ���¸� �Ⱦ�����(?)... UI 1.2�� �������� ���� ���������� ����� �ʿ� - sanghun.han
			if (!disable)
			{
				i3Color::Set(&color, (UINT8)100, 168, 239, 255);
			}
			else
#endif
			{
				i3Color::Set(&color, (UINT8)170, 170, 170, 255);			// ��...Ŀ����Ƽ�� �ڲ� ¡¡�뼭 �ϳ��� ���� �������� �ּ�ó��.
			}
//			if(disable)		
//				i3Color::Set(&color, (UINT8)170, 170, 170, 255);
//			else			
//				i3Color::Set(&color, (UINT8)170, 170, 170, 255);
		}
		else if(g_pEnvSet->m_nUIVersion == UFV_1_1)
		{
			if(disable)		
				i3Color::Set(&color, (UINT8)128, 128, 128, 255);
			else			
				i3Color::Set(&color, (UINT8)255, 255, 255, 255);
		}
		else if (g_pEnvSet->m_nUIVersion == UFV_1_5)
		{
			if (disable)
				i3Color::Set(&color, (UINT8)128, 128, 128, 255);
			else
				i3Color::Set(&color, (UINT8)91, 160, 222, 255);
		}

		btn->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE, color);
		btn->setModifyState();
		btn->setInputDisable(disable);
	}
}

extern "C" 
{
	int option_frame_click( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked( value );
		return 0;
	}
}

LuaFuncReg option_frame_glue[] =
{
	{"Click",		option_frame_click},
	{nullptr,				nullptr}
};

UIPopupOption* UIPopupOption::instance()
{
	return g_this;
}

/************************//************************/
void UIPopupOption::clicked(int idx) const
{
	switch( idx )
	{
	case GAME: case VIDEO: case AUDIO: case CONTROL: case COMMUNITY: case GRADE:
		change_option(idx);		
		break;
	case DEFAULT:
		{
			UIOptionVideo* video_tab = 0;
			if( g_curr_option == m_list[VIDEO] ) 
			{
				video_tab = static_cast<UIOptionVideo*>(g_curr_option);
				if( video_tab->clicked_default() )	return;
			}

			g_curr_option->default_value();
			g_curr_option->apply_game();
			g_curr_option->apply_lua();
		}
		break;
	case OK:
		if(g_this->isEmptyKeyItem())
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR2_OPTION_EXIST_NONE_KEY"));
			g_delay_lua = true;
			return;
		}
			
		if( g_curr_option == m_list[VIDEO] )
		{
			UIOptionVideo* video_tab = static_cast<UIOptionVideo*>(g_curr_option);
			if( video_tab->clicked_ok() )	return;
		}

		if( is_modified_option() )
		{
			/*����� �׸���� ���� �Ͻðڽ��ϱ�?*/
			g_this->SetInputDisable(true);
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_EP_OPTION_CONFIRM"), 
				nullptr, MAKE_CALLBACK(CbConfirm), const_cast<UIPopupOption*>(this) );
		}
		else
		{
			g_close_popup = true;
		}
		break;
	case CANCLE:	
		if( g_curr_option == m_list[VIDEO] )
		{
			UIOptionVideo* video_tab = static_cast<UIOptionVideo*>(g_curr_option);
			if( video_tab->clicked_cancel() )	return;
		}

		g_this->isLoadKeyItem();
		load_apply_option(true);	g_close_popup = true;	
		break;
	}
}

/************************//************************/
UIOptionBase* UIPopupOption::GetCurrentOption() const
{
	return g_curr_option;
}

void UIPopupOption::change_option(int idx) const
{
	if( idx == VIDEO && TwitchTVContext::i()->isBroadCasting() )
		return;

	if( g_curr_option == m_list[idx] )
		return;

	g_pFramework->m_EnableCapture = (idx != VIDEO); //Video �� �� ��쿣 ������ ��ȭ �����Ѵ�.

	if(g_curr_option)	
	{
		if(m_InGame)
			g_curr_option->hide_scn();
		else 
			g_curr_option->exit_scn();
	}
	
	g_curr_option = m_list[idx];

	if(m_InGame && !g_delay_show ) 
		g_curr_option->show_scn();
	else 
		g_curr_option->entrance_scn();
	
	g_delay_show = false;

	g_curr_option->apply_lua();
}

bool UIPopupOption::is_modified_option() const
{
	bool is_modified = false;
	size_t size = m_list.size();

	for(size_t i=0;i<size;++i)
	{
		if( m_list[i]->is_modified() )
			is_modified = true;
	}

	return is_modified;
}

void UIPopupOption::load_apply_option(bool restore) const
{
	//restore �� ��� video option �� apply�� ��� ���� ������ ���� ��츸 �����ϱ� ������, ���������� ȣ���ؾ� �ȴ�.
	UIOptionVideo* video_tab = (UIOptionVideo*)m_list[VIDEO];
	if( restore && video_tab->is_modified_screen() )
		VideoUtil::ApplyVideoEnv();
	
	struct Fn_la	{	void operator()(UIOptionBase* opt) { opt->load_value(); opt->apply_game(); }	};
	std::for_each(m_list.begin(), m_list.end(), Fn_la() );
}

void UIPopupOption::apply_save_option() const
{
	struct Fn_a{void operator()(UIOptionBase* opt) { opt->apply_game(); } };
	std::for_each(m_list.begin(), m_list.end(), Fn_a() );

	ISTEST_S
	{
		/*������ ����Ǵ� ������ �������, ������ ���� �� �����Ƿ� �������� �ʽ��ϴ�.\n���ӿ� �α����� ����� �ּ���.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_SAVE_INFO"));
		return;
	}
	ISTEST_E

	EnvInfoContext::i()->SetNetEnvSetBasicChange( 0 );

	struct Fn_s { 	void operator()(UIOptionBase* opt) { opt->save_value(); } };
	std::for_each(m_list.begin(), m_list.end(), Fn_s() );

	// �����Ǿ��� ���� �ɼ����Ͽ� �����մϴ�.
	g_pEnvSet->Save();
	g_pEnvSet->Net_Save();
	g_pEffectManager->InitAgeLimitEffect();

	if( EnvInfoContext::i()->GetEnvSetDefault() == 1)
	{
		// ���� ���� �����Դϴ�.
		UINT32 nChange = EnvInfoContext::i()->GetNetEnvSetBasicChange();

		nChange |= 0x00000001;
		nChange |= 0x00000002;
		nChange |= 0x00000004;

		EnvInfoContext::i()->SetNetEnvSetBasicChange( nChange);
		EnvInfoContext::i()->SetEnvSetDefault( 0);
	}

	GameEventSender::i()->SetEvent( EVENT_OPTION_SAVE_N );
	//GameEventSender::i()->SetEvent( EVENT_OPTION_SAVE_MACRO_N );
}

/************************//************************/
UIPopupOption::UIPopupOption()
{
	g_this = this;
	m_InGame = false;
	m_StartGame = false;
	m_pBGFrame = nullptr;
}

UIPopupOption::~UIPopupOption()
{
	//UI::remove_cb(this);

	struct Fn	{	void operator()(UITabBase* data) { data->Destroy(); }	};
	std::for_each(m_list.begin(), m_list.end(), Fn() );
	m_list.clear();

	g_curr_option = 0;
}


/************************//************************/
/*virtual*/ void UIPopupOption::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Option/PBRe_Pref_Frame.i3UIs", option_frame_glue);

	i3ClassMeta * pMeta = nullptr;
	m_list.resize(UI_POPUP_MAX);
	for (INT32 i = 0 ; i < UI_POPUP_MAX; ++i)
	{
		switch( i)
		{	
		case GAME: pMeta = UIOptionGame::static_meta();		break;
		case VIDEO: pMeta = UIOptionVideo::static_meta();	break;
		case AUDIO: pMeta = UIOptionAudio::static_meta();		break;
		case CONTROL: pMeta = UIOptionControl::static_meta(); break;
		case COMMUNITY: pMeta = UIOptionCommunity::static_meta();	break;
		case GRADE: pMeta = UIOptionGrade::static_meta(); break;
		}

		if( pMeta != nullptr)
		{
			m_list[i] = (UIOptionBase *) pMeta->create_instance();	//	CREATEINSTANCE(pMeta);
			m_list[i]->OnCreate(this);
			
			I3_MUST_ADDREF(m_list[i]);
			m_list[i]->setParent(this);
		}
	}

	m_InGame = g_pFramework->IsBattleStage();
	if(m_InGame)	
	{
		g_delay_show = true;
		bool bTrue = true;
		OnEntranceStart(static_cast<bool*>(&bTrue)); //�ΰ����� �̸�  loading �Ѵ�.
	}
	else
	{
		g_delay_show = false;
	}
}

void UIPopupOption::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);
}

/*virtual*/ void UIPopupOption::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)pScene->FindChildByName( "i3UIButton0" );
	btn->enable_btn();
}

bool UIPopupOption::OnEntranceStart( void * pArg1, void * pArg2)
{
	g_delay_lua = false;
	g_close_popup = false;

	g_curr_option = 0;

	// ���� �ɼ� ���� �� ������ ���۵Ǹ� ������ �Ǳ⶧����, Ready ���¸� Ǭ��.
	if(!m_InGame)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
		if( pFrame->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
		{
			const SLOT_INFO* info = BattleSlotContext::i()->getMySlotInfo();
			if (SLOT_STATE_READY == info->_State)
			{
				GameEventSender::i()->SetEvent(EVENT_READY_BATTLE);
			}
		}
	}

	initialize_keyitem();
	isLoadKeyItem();

	struct Fn_l	{ 	void operator()(UIOptionBase* opt) { opt->load_value(); } };
	std::for_each(m_list.begin(), m_list.end(), Fn_l() );

	if( m_InGame )
	{
		if( pArg1 == nullptr )
		{
			SetVisible( true );
			_ForceEnable(true);
			GetScene(0)->Attach(false); //ui depth  ������ �� �ڷ� ������ �Ѵ�.
			OnLoadAllScenes();
			CrosshairMgr::i()->BackupEnable();

			LuaState* L = _CallLuaFunction("SetTab");	// ��� �� ���� �� �⺻ ����.
			i3Lua::PushInteger(L, GAME);
			UILua::EndLuaFunction(L , 1);
		}
		else
		{
			struct Fn	{ void operator()(UIOptionBase* opt) { opt->entrance_scn(); opt->hide_scn(); } };
			std::for_each(m_list.begin(), m_list.end(), Fn() );
			SetEnable( true );
			SetVisible( false );
			_ForceEnable(false);
		}
	}
	else
	{
		SetEnable( true );
	}

	

#if !defined( LOCALE_KOREA )
	LuaState * L = _CallLuaFunction( "SetOptionTabEnable");
	i3Lua::PushInteger(L, 6);
	i3Lua::PushBoolean(L, false);
	UILua::EndLuaFunction(L , 2);
#endif


	return true;
}

void UIPopupOption::OnLoadAllScenes()
{
	LuaState * L = _CallLuaFunction( "LoadAllScenes");
	UILua::EndLuaFunction(L , 0);

	g_is_video_capture = g_pFramework->IsVideoCapturing();
	g_is_twitch_broadcasting = TwitchTVContext::i()->isBroadCasting();
	adjust_video_tab(GetScene(0), m_InGame || g_is_video_capture || g_is_twitch_broadcasting);
}

bool UIPopupOption::OnExitStart()
{
	if( m_InGame )
	{
		struct Fn	{ void operator()(UIOptionBase* opt) { opt->hide_scn(); }	};
		std::for_each(m_list.begin(), m_list.end(), Fn() );
		SetVisible(false);
		_ForceEnable(false);
		CrosshairMgr::i()->RestoreEnable();
	}
	else
	{
		if( g_curr_option != nullptr)
			g_curr_option->exit_scn();

		SetEnable( false );
	}

	g_curr_option = 0;

	g_pFramework->m_EnableCapture = true;	//������ ��ȭ Ȱ��ȭ

	return true;
}

void	UIPopupOption::OnUnloadAllScenes()
{
}

/*virtual*/ void UIPopupOption::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);

	if(g_delay_lua)
	{
		LuaState* L = _CallLuaFunction("SetTab");
		if(g_curr_option == m_list[CONTROL])
			i3Lua::PushInteger( L, CONTROL);
		else if(g_curr_option == m_list[AUDIO])
			i3Lua::PushInteger( L, AUDIO);
		UILua::EndLuaFunction(L , 1);
		g_delay_lua = false;
	}

	if(g_close_popup)
	{
		if(m_InGame)
			UIBattleFrame::i()->ClosePopup(UBP_OPTION);
		else
			g_pFramework->GetUIMainframe()->ClosePopup(UPW_OPTION);
	}

	if( !m_InGame && g_is_video_capture != g_pFramework->IsVideoCapturing() )	//�ΰ����� �̹� ���� �ִ�., ĸ�� �� ���� ���´�.
	{
		g_is_video_capture = !g_is_video_capture;
		adjust_video_tab( GetScene(0), g_is_video_capture || g_is_twitch_broadcasting );
	}

	if( !m_InGame && g_is_twitch_broadcasting != TwitchTVContext::i()->isBroadCasting() )	//�ΰ����� �̹� ���� �ִ�., ĸ�� �� ���� ���´�.
	{
		g_is_twitch_broadcasting = !g_is_twitch_broadcasting;
		adjust_video_tab( GetScene(0), g_is_twitch_broadcasting || g_is_video_capture );
	}
}

bool UIPopupOption::OnKey_Escape()
{
	load_apply_option(true);

	if(m_InGame)
	{
		CBattleGui::i()->SetEnablePopup();
		return true;
	}

	return UIPopupBase::OnKey_Escape();
}

bool UIPopupOption::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	if( !g_curr_option ) return UIPopupBase::OnKeyInput(pInputMng);

	//editbox ��Ŀ���� �ִ� ���� option_tab���� OnKeyInput�� �Ѱ��ش�.
	if( i3UI::getGlobalIme()->isFocused())
	{
		i3UIEditBox * pCurEdit = reinterpret_cast<i3UIEditBox*>(i3UI::getGlobalIme()->getOwner());
		i3UIScene * pScene = g_curr_option->GetScene( 0);
		i3GameNode * pChild = pScene->getFirstChild();
		while( pChild != nullptr)
		{
			if( ((i3UIControl*)pChild)->IsChildControl( pCurEdit))
				return g_curr_option->OnKeyInput(pInputMng);

			pChild = pChild->getNext();
		}
	}

	//editbox �� �ܿ� ��Ŀ���� �ִ� ��쿡 option_tab���� �����ϵ��� �Ѵ�.
	//����, option_tab�� ���ϰ��� false�� ��쿣 base ���� �Ѱ��ش�.
	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
	if( pKeyboard->GetStrokeState( I3I_KEY_ESC) )
	{
		if( g_curr_option->key_escape() )	return true;

		if(m_InGame)	g_close_popup = true;	//�ΰ����� ��쿣 �ڵ� close�� �� �ǳ�
	}

	if( pKeyboard->GetStrokeState( I3I_KEY_ENTER) )
	{
		if( g_curr_option->key_enter() ) return true;
	}

	return UIPopupBase::OnKeyInput(pInputMng);
}

void UIPopupOption::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPopupBase::ProcessGameEvent( evt, arg, UserData);

	if( evt == EVENT_GET_MYOPTION_N)
	{
		m_InGame = false;
		m_StartGame = true;
		
		load_apply_option(false);

		m_StartGame = false;
	}
}


////////////////////////////////////////////////////////////////////////// 

KeyItem UIPopupOption::getKeyItem(UI_KEYCTRL_LIST_TYPE _type, int _idx)
{
	I3ASSERT(m_popupKeyItem != nullptr);
	return m_popupKeyItem[_type][_idx];
}

void UIPopupOption::add_key_item(UI_KEYCTRL_LIST_TYPE list_idx, GAME_KEY_MAP menu)
{
	KeyItem i;
	i.type = ITEM;	i.menu = menu;	i.wstr_name = GameUI::GetKeymenuString(menu);
	i.value._nDiviceType = -1;	i.value._nState = 0xFFFFFFFF; // I3I_KEY_UNKNOWN;
	m_popupKeyItem[list_idx].push_back( i );
}

void UIPopupOption::add_key_item(const i3::rc_wstring& wstr_text)
{
	KeyItem i; 
	i.menu = GAME_KEY_MAP(0);			// ������ �ʴµ� �ѵ�..�ʱ�ȭ�� �ʿ��ؼ� �̷��� ó��. (2015.04.06.����)
	i.type = TITLE;  i.wstr_name = wstr_text;
	i.value._nDiviceType = -1;	i.value._nState = 0xFFFFFFFF; // I3I_KEY_UNKNOWN;
	m_popupKeyItem[UI_KEYCTRL_TYPE_GAMEKEY_LEFT].push_back( i );

	i.type = DUMMY_TITLE; i.wstr_name = wstr_text;
	m_popupKeyItem[UI_KEYCTRL_TYPE_GAMEKEY_RIGHT].push_back( i );
}

void UIPopupOption::initialize_keyitem()
{
	if(m_popupKeyItem[UI_KEYCTRL_TYPE_AUDIO].size() > 0)
		return; 

	add_key_item(UI_KEYCTRL_TYPE_AUDIO, GAME_KEY_MAP_RADIOMESSAGE1);
	add_key_item(UI_KEYCTRL_TYPE_AUDIO, GAME_KEY_MAP_RADIOMESSAGE2);
	add_key_item(UI_KEYCTRL_TYPE_AUDIO, GAME_KEY_MAP_RADIOMESSAGE3);

	if(m_popupKeyItem[UI_KEYCTRL_TYPE_GAMEKEY_LEFT].size() >0)
		return;
	
	add_key_item( GAME_RCSTRING("STR_OPT_TAP_CTR_KEY_NAME_MOVE") );	/*�̵�*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_FORWARD );	/*������ �̵�*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_BACKWARD );	/*�ڷ� �̵�*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_LEFT );	/*�·� �̵�*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_RIGHT );	/*��� �̵�*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_JUMP );	/*����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_FOOTSTEP );	/*�ȱ�*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_CROUCH );	/*�ɱ�*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_TURNBACK );	/*�ڵ��ƺ���*/

	add_key_item( GAME_RCSTRING("STR_OPT_TAP_CTR_KEY_NAME_ACTION") );	/*�ൿ*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_ATTACK );	/*������*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_SUBFUNC );	/*Extention 1*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_WEAPON_EXTENSION);	/*Extention 2(������ ��Ʈ / ��ź �߻� ��� ����)*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_RELOAD );	/*������*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_DROP );	/*���������*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_SPECIALACTION );	/*���߹� ��ü*/

	add_key_item( GAME_RCSTRING("STR_SHOP_MENU_NEWHOT_WEAPON") );	/*����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_CHANGEWEAPON );	/*�ֹ��� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_CHANGEWEAPON_1 );	/*�ֹ��� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_CHANGEWEAPON_2 );	/*�������� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_CHANGEWEAPON_3 );	/*�������� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_CHANGEWEAPON_4 );	/*��ô���� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_CHANGEWEAPON_5 );	/*Ư������ ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_CHANGEWEAPON_6 );	/*�̼Ǿ����� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_CHANGEWEAPON_NEXT );	/*��������� ��ü*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_CHANGEWEAPON_PREV );	/*��������� ��ü*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_CHANGEWEAPON_QUICK );	/*���� ���ⱳü*/

	add_key_item( GAME_RCSTRING("STR_POPUP_FORCIBLYREMOVE_REASON_ETC") );	/*��Ÿ*/
	//add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_TOGGLE_MAP );	/*��ü�� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_MINIMAP_ENLARGE );	/*�̴ϸ�Ȯ��*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_MINIMAP_REDUCTION );	/*�̴ϸ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_SHOWSCORE );	/*��Ȳ�� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_CHAT );	/*ä��*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_LEFT, GAME_KEY_MAP_CHAT_ALL );	/*��üä��*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_CHAT_TEAM );	/*��ä��*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_CHAT_CLAN );	/*Ŭ��ä��*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_CHAT_HISTORY );	/*���� ��ȭ ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_MOUSE_SENS_UP );	/*���콺���� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_MOUSE_SENS_DOWN );	/*���콺���� ����*/
	add_key_item( UI_KEYCTRL_TYPE_GAMEKEY_RIGHT, GAME_KEY_MAP_SCREENSHOT );	/*��ũ����*/

	int idx = 0;
	if( LocaleValue::Enable("EnableVideoCapture") )
	{
		add_key_item( (UI_KEYCTRL_LIST_TYPE)idx, GAME_KEY_MAP_VIDEOCAPTURE );	/*������ �Կ�*/
		++idx;
	}
	if( LocaleValue::Enable("FieldShop") )
	{
		add_key_item( (UI_KEYCTRL_LIST_TYPE)idx, GAME_KEY_MAP_INGAME_SHOP );	/*���� ����*/
	}
}

bool UIPopupOption::isInitKeyItem()
{
	if(m_popupKeyItem[UI_KEYCTRL_TYPE_AUDIO].size() <= 0) 
		return false;
	else
		return true;
}

bool UIPopupOption::isLoadKeyItem()
{
	for(size_t i=0; i<UI_KEYCTRL_TYPE_MAX; i++)
	{
		size_t _size = m_popupKeyItem[i].size();
		for(size_t j=0; j<_size; j++)
		{
			if( m_popupKeyItem[i][j].type == ITEM )
				m_popupKeyItem[i][j].load_value();
		}
	}
	return true;
}

void UIPopupOption::setKeyItem_value(UI_KEYCTRL_LIST_TYPE _type, int _idx, GAME_KEY_STRUCT _value)
{
	I3ASSERT(m_popupKeyItem[_type].size() >= (size_t)_idx); // �޾ƿ� idx ���� size���� �۾ƾ� ��
	m_popupKeyItem[_type][_idx].value = _value;

}

GAME_KEY_STRUCT UIPopupOption::getKeyItem_value(UI_KEYCTRL_LIST_TYPE _type, int _idx)
{
	I3ASSERT(m_popupKeyItem[_type].size() >= (size_t)_idx);
	return m_popupKeyItem[_type][_idx].value;
}

size_t UIPopupOption::getKeyListSize(UI_KEYCTRL_LIST_TYPE _type)
{
	return m_popupKeyItem[_type].size();
}

bool UIPopupOption::isDuplicateKey(UINT32 _key, INT8 _device_type, UI_KEYCTRL_LIST_TYPE _type, int _idx)
{
	I3ASSERT(m_popupKeyItem[_type].size() >= (size_t)_idx);

	if(m_popupKeyItem[_type][_idx].duplicate_keyItem(_key, _device_type))
		return true;
	else 
		return false;
}

bool UIPopupOption::isMatchValue( UI_KEYCTRL_LIST_TYPE _type, int _idx)
{
	I3ASSERT(m_popupKeyItem[_type].size() >= (size_t)_idx);

	KeyItem & item = m_popupKeyItem[_type][_idx];
	
	if( item.type != ITEM || item.match_value() )
		return true;
	else
		return false;
}

void UIPopupOption::setKeyItem_default()
{
	for(size_t i=0; i<UI_KEYCTRL_TYPE_MAX ; i++)
	{
		size_t _size = m_popupKeyItem[i].size();
		for(size_t j=0; j<_size; j++)
		{
			m_popupKeyItem[i][j].set_default();
		}
	}
}

void UIPopupOption::saveKeyItem( UI_KEYCTRL_LIST_TYPE _type, int _idx)
{
	I3ASSERT(m_popupKeyItem[_type].size() >= (size_t)_idx);
	m_popupKeyItem[_type][_idx].save_value();
}

SLOT_TYPE UIPopupOption::getKeyItem_slotType(UI_KEYCTRL_LIST_TYPE _type, int _idx)
{
	I3ASSERT(m_popupKeyItem[_type].size() >= (size_t)_idx);
	return m_popupKeyItem[_type][_idx].type;
}

bool UIPopupOption::isEmptyKeyItem()
{
	GAME_KEY_STRUCT _emptyKey;
	_emptyKey._nDiviceType = -1; 
	_emptyKey._nState = 0;

	for(size_t i=0; i<UI_KEYCTRL_TYPE_MAX ; i++)
	{
		size_t _size = m_popupKeyItem[i].size();
		for(size_t j=0; j<_size; j++)
		{
			if(m_popupKeyItem[i][j].duplicate_keyItem(_emptyKey._nState, _emptyKey._nDiviceType))
				return true;
		}
	}

	return false;
}

void UIPopupOption::SetInputDisable( bool bDisable)
{
	UIBase::SetInputDisable(bDisable);

	if( GetScene(0) != nullptr )
		adjust_video_tab(GetScene(0), m_InGame || g_is_video_capture );
}