#include "pch.h"
#include "UIPopupCreateClan.h"
#include "UIUtil.h"
#include "UIMainFrame.h"

#include "GameGUI.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/utf8_to_utf16.h"


I3_CLASS_INSTANCE( UIPopupCreateClan);//, UIPopupClanBase);

extern "C" 
{
	int UIPopupCreateClan_CheckClanName( LuaState * L)
	{
		UIPopupCreateClan * p = static_cast<UIPopupCreateClan*>(GameUI::GetPopup( UPW_CLAN_CREATE));
		if( p != nullptr)
			p->CheckClanName();
		return 0;
	}
	
	int UIPopupCreateClan_CheckClanURL( LuaState * L)
	{
		UIPopupCreateClan * p = static_cast<UIPopupCreateClan*>(GameUI::GetPopup( UPW_CLAN_CREATE));
		if( p != nullptr)
			p->CheckClanURL();
		return 0;
	}
	
	int UIPopupCreateClan_CreateClan( LuaState * L)
	{
		UIPopupCreateClan * p = static_cast<UIPopupCreateClan*>(GameUI::GetPopup( UPW_CLAN_CREATE));
		if( p != nullptr)
			p->CreateClan();
		return 0;
	}

	int UIPopupCreateClan_Cancel( LuaState * L)
	{
		UIPopupCreateClan * p = static_cast<UIPopupCreateClan*>(GameUI::GetPopup( UPW_CLAN_CREATE));
		if( p != nullptr)
			p->Close();
		return 0;
	}
}

LuaFuncReg UIPBRe_PopUp_CreateClan_Glue[] =
{
	{ "CheckClanName",			UIPopupCreateClan_CheckClanName},
	{ "CheckClanURL",			UIPopupCreateClan_CheckClanURL},
	{ "CreateClan",				UIPopupCreateClan_CreateClan},
	{ "Cancel",					UIPopupCreateClan_Cancel},
	{nullptr, nullptr}
};

UIPopupCreateClan::UIPopupCreateClan()
{
	m_bWaitRequestCheck = false;

	m_bCompleteNameCheck = false;
	m_bCompleteURLCheck = false;

//	memset(m_szClanName, 0 , NET_CLAN_NAME_SIZE);
//	memset(m_szClanURL, 0, NET_CLAN_AZIT_URL_SIZE);
//	memset(m_szClanIntro, 0, NET_CLAN_INTRO_SIZE);

	RegisterGameEventFunc( EVENT_CLAN_DUPLICATE_NAME,				&UIPopupCreateClan::ReceiveGameEvent_DuplicateClanName);
	RegisterGameEventFunc( EVENT_CLAN_DUPLICATE_ADDRESS,			&UIPopupCreateClan::ReceiveGameEvent_DuplicateAddress);
	RegisterGameEventFunc( EVENT_CLAN_CREATE,						&UIPopupCreateClan::ReceiveGameEvent_Create_Clan);
}

UIPopupCreateClan::~UIPopupCreateClan()
{
}

void UIPopupCreateClan::_InitControl( void)
{
	UINT8 ui8ReuirementRank = 0;
	UINT32 ui32RequirementPoint = 0;

	if(g_pFramework->GetClanContext()->GetClanCreationRequirementInfo(&ui8ReuirementRank, &ui32RequirementPoint))
	{
		i3::stack_wstring wstrText;

		i3::sprintf(wstrText, GAME_RCSTRING("STR_CLAN_CREATE_WARNING"), GetCharaRank(ui8ReuirementRank), ui32RequirementPoint);

		LuaState * L = _CallLuaFunction("InitControl");

		if(L != nullptr)
		{
			i3Lua::PushStringUTF16To8(L , wstrText);
#ifdef CLAN_ADDRESS_ENABLE
			i3Lua::PushBoolean(L , true);
#else
			i3Lua::PushBoolean(L , false);
#endif	// CLAN_ADDRESS_ENABLE
			_EndLuaFunction(L , 2);
		}
	}
}

void UIPopupCreateClan::_SetCheckResultText(const i3::wliteral_range& wTextRng, const I3COLOR * pColor)
{
	LuaState * L = _CallLuaFunction("setCheckResult");

	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L , wTextRng);
		i3Lua::PushColor(L , pColor);
		_EndLuaFunction(L , 5);
	}
}

void UIPopupCreateClan::_SetCheckResultSuccess(const i3::wliteral_range& wTextRng)
{
	_SetCheckResultText(wTextRng, GameGUI::GetColor(GCT_LIGHT_GREEN));
}

void UIPopupCreateClan::_SetCheckResultFail(const i3::wliteral_range& wTextRng)
{
	_SetCheckResultText(wTextRng, GameGUI::GetColor(GCT_RED));
}

void UIPopupCreateClan::_GetClanNameText( void)
{
	LuaState * L = _CallLuaFunction("getClanNameText");
	const char * pszClanName = _ReturnLuaString(L , 0);
	
	i3::wstring		wstrClanName;
	i3::utf8_to_utf16(pszClanName, wstrClanName);			// UTF8���� mb���� �����ؾ���.(2014.07.14.����)

	// ��Ʈ���� �ٸ��� �ߺ�üũ�� �ٽ� �Ǿ��մϴ�.
	if(i3::generic_is_equal(m_wstrClanName, wstrClanName) == false) // != 0)
		m_bCompleteNameCheck = false;

	i3::safe_string_copy(m_wstrClanName, wstrClanName, NET_CLAN_NAME_SIZE);			// �̰� �̷��Ը� �ϴ� ó���صд�. (2014.07.14.����)
}

void UIPopupCreateClan::_GetClanURLText( void)
{
	//LuaState * L = _CallLuaFunction("getClanURLText");
	//const char * pszClanURL = _ReturnLuaString(L , 0);

	// ��Ʈ���� �ٸ��� �ߺ�üũ�� �ٽ� �Ǿ��մϴ�.
	//if(i3::generic_is_equal(m_szClanURL, pszClanURL) == false) // != 0)
	//	m_bCompleteURLCheck = false;

	//i3::safe_string_copy(m_szClanURL, pszClanURL, NET_CLAN_AZIT_URL_SIZE);
}




/*virtual*/ void UIPopupCreateClan::OnCreate( i3GameNode * pParent)
{
	UIPopupClanBase::OnCreate( pParent);

	// Load Scene
//	char Path[MAX_PATH] = {0,};
//	i3::snprintf(Path, sizeof(Path), "Scene/Popup/PBRe_PopUp_ClanCreate.i3UIs", i3UI::getManager()->GetCommonLocaleUIFolder()->c_str());
	AddScene("Scene/Popup/PBRe_PopUp_ClanCreate.i3UIs", UIPBRe_PopUp_CreateClan_Glue);
}


/*virtual*/ bool UIPopupCreateClan::OnEntranceStart(void * pArg1 /* = nullptr  */, void * pArg2 /* = nullptr */ )
{
	if(UIPopupClanBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;
	
	_InitControl();

	m_bWaitRequestCheck = false;

	m_bCompleteNameCheck = false;
	m_bCompleteURLCheck = false;

	return true;
}

void UIPopupCreateClan::OnEntranceEnd(void)
{
	UIPopupClanBase::OnEntranceEnd();


	i3UIEditBox* pEditBox = static_cast<i3UIEditBox*>(GetScene(0)->FindChildByName("Edit_ClanName"));
	pEditBox->OnSetFocus();
}

void UIPopupCreateClan::CheckClanName(void)
{
	// ó�����̸� return
	if(m_bWaitRequestCheck)
		return;

	_GetClanNameText();

	if(UIPopupClanBase::_CheckNickNameOverLap(m_wstrClanName) == false)
		return;

	m_bWaitRequestCheck = true;
	g_pFramework->GetClanContext()->SendOverlappingClanName(m_wstrClanName);
}

void UIPopupCreateClan::CheckClanURL(void)
{
#ifdef CLAN_ADDRESS_ENABLE
	// ó�����̸� return
	if(m_bWaitRequestCheck)
		return;

	_GetClanURLText();

	if(UIPopupClanBase::_CheckClanAddressOverLap(m_wstrClanURL) == false)
		return;

	m_bWaitRequestCheck = true;
	g_pFramework->GetClanContext()->SendOverlappingClanAddress(m_wstrClanURL);
#endif // CLAN_ADDRESS_ENABLE
}

void UIPopupCreateClan::CreateClan(void)
{
	// ó�����̸� return
	if(m_bWaitRequestCheck)
		return;
	
	_GetClanNameText();

	if( i3::generic_string_size(m_wstrClanName) == 0 )
	{
		// Ŭ������ �����ּ���
		_SetCheckResultFail(GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_NAME"));
		return;
	}

	if(m_bCompleteNameCheck == false)
	{
		// Ŭ���� �ߺ��˻縦 ���ֽʽÿ�.
		_SetCheckResultFail(GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_NAME"));
		return;
	}

#ifdef CLAN_ADDRESS_ENABLE
	_GetClanURLText();

	if( i3::generic_string_size(m_szClanURL) == 0)
	{
		// Ŭ�� ī�� �ּҸ� �����ּ���.
		_SetCheckResultFail(GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));
		return;
	}

	if(m_bCompleteURLCheck == false)
	{
		// Ŭ�� ī�� �ּ� �ߺ��˻縦 ���ֽʽÿ�.
		_SetCheckResultFail(GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_AZIT"));
		return;
	}
#endif // CLAN_ADDRESS_ENABLE

	m_bWaitRequestCheck = true;

	UINT32 nMarkFlag = 0;
	//	Ŭ�� ���� �̺�Ʈ�� ������.	

	g_pFramework->GetClanContext()->SendCreateClan(m_wstrClanName, m_wstrClanIntro, m_wstrClanURL, nMarkFlag);

}

void UIPopupCreateClan::Close(void)
{
	g_pFramework->GetClanContext()->ResetClanCreationInfo();
	GetMainFrame()->ClosePopup(UPW_CLAN_CREATE);
}

void UIPopupCreateClan::cbSuccessCreateClan(void* pThis, INT32 nParam)
{
	UIPopupCreateClan * pThisPopup = (UIPopupCreateClan*) pThis;
	I3ASSERT(pThisPopup != nullptr);

	pThisPopup->Close();
	pThisPopup->GetMainFrame()->ChangeClanPopup();
}

void UIPopupCreateClan::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPopupCreateClan::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPopupClanBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

void UIPopupCreateClan::ReceiveGameEvent_DuplicateClanName( INT32 arg, const i3::user_data&)
{
	if( m_bWaitRequestCheck)
	{
		if( arg == 0)
		{
			_SetCheckResultSuccess(GAME_RCSTRING("STR_CLAN_DUPLICATE_NAME_SUCCESS"));
			m_bCompleteNameCheck = true;
		}
		else
		{
			_SetCheckResultFail(GAME_RCSTRING("STR_CLAN_DUPLICATE_NAME_FAIL"));
		}

		m_bWaitRequestCheck = false;
	}
}

void UIPopupCreateClan::ReceiveGameEvent_DuplicateAddress( INT32 arg, const i3::user_data&)
{
	if( m_bWaitRequestCheck )
	{
		if( arg == 0)
		{
			_SetCheckResultSuccess(GAME_RCSTRING("STR_CLAN_DUPLICATE_URL_SUCCESS"));
			m_bCompleteURLCheck = true;
		}
		else
		{
			_SetCheckResultFail(GAME_RCSTRING("STR_CLAN_DUPLICATE_URL_FAIL"));
		}

		m_bWaitRequestCheck = false;
	}
}

void UIPopupCreateClan::ReceiveGameEvent_Create_Clan( INT32 arg, const i3::user_data&)
{
	if( m_bWaitRequestCheck)
	{
		if( EV_SUCCESSED( arg ) )
		{
			//	"\n�����մϴ�.\nŬ�� ������ �Ϸ�Ǿ����ϴ�."
			//	"Ŭ�� ���� ����"
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_CREATING"), 
				&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_COMPLETE_CREATING"), MAKE_CALLBACK(cbSuccessCreateClan), this);
		}
		else
		{
			switch( arg )
			{
			case EVENT_ERROR_CLAN_MAXCLAN:
				{
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�.\n ���̻� Ŭ���� ����� �����ϴ�."
					//	"Ŭ�� ���� ����"
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_OVERFLOW"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			case EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX:
				{
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�.\n �̹� �ٸ� Ŭ���� ���ԵǾ� �ְų� ���� ��û�� �����Դϴ�."
					//	"Ŭ�� ���� ����"
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_ALREADY"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			case EVENT_ERROR_CLAN_SEARCH_CLANIDX:
				{
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�.\n �̹� �ߺ��Ǵ� Ŭ���� �Ǵ� ����Ʈ �ּҰ� �����մϴ�."
					//	"Ŭ�� ���� ����"
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_OVERLAPPING"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			case EVENT_ERROR_CLAN_CREATE_FAIL:
				{
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�."
					//	"Ŭ�� ���� ����"
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			default:
				{
					i3::rc_wstring wstrTemp;
					//	"\n Ŭ�� ������ �����Ͽ����ϴ�.\n �˼��մϴ�. �����ڿ��� �����Ͽ� �ֽʽÿ�."
					i3::sprintf( wstrTemp, L"%s\n [Errcode : 0x%08x]", GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_FAIL_CREATING_ADMIN"), arg);

					//	"Ŭ�� ���� ����"
					GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_FAIL_CREATING"));
				}
				break;
			}
		}

		m_bWaitRequestCheck = false;
	}
}