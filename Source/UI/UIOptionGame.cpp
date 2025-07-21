#include "pch.h"

#include "UIMainFrame.h"
#include "UITopMenu.h"
#include "UIOptionGame.h"
#include "UIClanFunction.h"
#include "CrosshairMgr.h"
#include "UIPopupOption.h"

#include "ConfigRenderBattle.h"
#include "BattleHud.h"

#include "EnvInfoContext.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( UIOptionGame);

namespace
{
	UIOptionGame* g_this = 0;

	enum	{ FRIEND = 0, BLOOD, HAND, GAME_INTERFACE, TEAM_BAND, MINIMAP, EFFECT, CROSSHAIR, LANGUAGE , TEXTEFFECT};
}

extern "C" 
{
	int option_game_btnGame( LuaState * L)
	{
		INT32 value1 = i3Lua::GetIntegerArg( L, 1);
		INT32 value2 = i3Lua::GetIntegerArg( L, 2);
		g_this->_SetPushedButton( value1, value2 );
		return 0;
	}
}

LuaFuncReg option_game_glue[] =
{
	{"btnPushed",	option_game_btnGame}, 
	{nullptr,				nullptr}
};

/************************//************************/
void UIOptionGame::clicked(int idx1, int idx2)
{

}

void UIOptionGame::_SetPushedButton(INT32 btnType, INT32 value)
{
	switch(btnType)
	{
		case FRIEND:
			m_bEnableFrindlyMark = value != 0;
			break;
		case BLOOD:
			m_bEnableBlood = value != 0;
			break;
		case HAND:
			m_nHandType = value;
			break;
		case GAME_INTERFACE:
			m_nGameInterface = value != 0; 
			break;
		case TEAM_BAND:
			m_bTeamBand = value != 0;
			break;
		case MINIMAP:
			m_bMiniMapMode = value != 0;
			break;
		case EFFECT:
			m_bCrossHairEffect = value != 0;
			break;
		case CROSSHAIR:
			m_nCrossHairType = value;
			break;
		case LANGUAGE:
			SelectLanguage( value );
		case TEXTEFFECT:
			m_bTextEffect = value ? true : false;
			break;
		default:
			break;
	}

	apply_lua();

}



/************************//************************/

UIOptionGame::UIOptionGame() : UIOptionBase()
{
	g_this = this;
}

UIOptionGame::~UIOptionGame()
{
}


/************************//************************/
/*virtual*/ void UIOptionGame::OnCreate( i3GameNode * pParent)
{
	UIOptionBase::OnCreate( pParent);

	AddScene("Scene/Option/PBRe_Pref_Games.i3UIs", option_game_glue);
}

/************************//************************/
void UIOptionGame::load_value()
{
	m_bEnableFrindlyMark =	g_pEnvSet->m_bEnableFrindlyMark;
	m_bEnableFrindlyName = TRUE;

	m_bEnableBlood = g_pEnvSet->m_bEnableBlood;

	m_nHandType = g_pEnvSet->m_nHandType;
	m_nCrossHairType = g_pEnvSet->m_nCrossHairType;
	m_bCrossHairEffect = g_pEnvSet->m_bCrossHairEffect;
	m_bMiniMapMode = g_pEnvSet->m_bMiniMapMode;

	m_nGameInterface = g_pEnvSet->m_nGameInterface;
	m_bTeamBand = g_pEnvSet->m_bTeamBand;

	m_nLanguage = g_pEnvSet->m_nLanguage;

	m_bTextEffect = g_pEnvSet->m_bTextEffect;
}

void UIOptionGame::save_value()
{
	if( is_modified() )
	{
		g_pEnvSet->m_bEnableFrindlyMark = m_bEnableFrindlyMark;	//�Ʊ� ��ũ ǥ��
		g_pEnvSet->m_bEnableFrindlyName = TRUE;	//�Ʊ� �г��� ǥ��
		g_pEnvSet->m_bEnableBlood = m_bEnableBlood;
		g_pEnvSet->m_nHandType = m_nHandType;
		g_pEnvSet->m_nCrossHairType = m_nCrossHairType;
		g_pEnvSet->m_bCrossHairEffect = m_bCrossHairEffect;
		g_pEnvSet->m_bMiniMapMode = m_bMiniMapMode;
		g_pEnvSet->m_nGameInterface = m_nGameInterface;
		g_pEnvSet->m_bTeamBand = m_bTeamBand;
		g_pEnvSet->m_nLanguage = m_nLanguage;
		g_pEnvSet->m_bTextEffect = m_bTextEffect;
		EnvInfoContext::i()->AddNetEnvSetBasicChange(BASIC);
	}
}

void UIOptionGame::default_value()
{
	m_bEnableFrindlyMark = true;
	m_bEnableBlood = true;
	m_nHandType = 0;
	m_nCrossHairType = 0;
	m_bCrossHairEffect = false;
	m_bMiniMapMode = 0;
	m_bTextEffect = true;

	if(!g_pFramework->IsBattleStage())
	{
		//�ΰ��ӿ��� �ʱ�ȭ ��, UI���� InitializeAtLoad()�� �Ҹ��� �ʾ� ũ���ð� �߻���.
		if (g_pEnvSet->m_nUIVersion == UFV_1_5)
			m_nGameInterface = 0;
	}

	LuaState* L = _CallLuaFunction("EnableInterface");
	if (L != nullptr)
	{
		if (g_pEnvSet->m_nUIVersion != UFV_1_5)
			i3Lua::PushBoolean(L, false);
		UILua::EndLuaFunction(L, 1);
	}

#if defined(LOCALE_JAPAN) || defined(DISABLE_TEAMBAND)
	m_bTeamBand = false;
#else
	m_bTeamBand = (g_pEnvSet->m_nUIVersion == UFV_1_5 )? true : false;
#endif 

	m_nLanguage = GetLanguageType();

	// ����Ʈ�� UI 1.5�� �������� ���, �ٸ� UI���� ������ �� �� �ֽ��ϴ�.
	// g_pEnvSet->m_nUIVersion = GetUIVersionType();
}

void UIOptionGame::apply_game()
{
	if( ((UIPopupOption*)getParent())->m_InGame )
	{
		if( g_pEnvSet->m_nCrossHairType != m_nCrossHairType )
		{
			//ȯ�� ���� ������ �о �ϱ⶧���� �̸� �����ؾ� �ȴ�.
			g_pEnvSet->m_nCrossHairType = m_nCrossHairType;
			INT32 scopetype = m_nCrossHairType;

			EnvInfoContext::i()->SetNetEnvSetBasicChange(0x00000001);

			g_pEnvSet->Save();
			g_pEnvSet->Net_Save();

			CrosshairMgr::i()->ApplyEnvSetCrosshair();

			GameEventSender::i()->SetEventOnce(EVENT_OPTION_SAVE_N);
			GameEventSender::i()->SetEventOnce(EVENT_BATTLE_OPTION_SCOPETYPE, &scopetype);
		}

		if( g_pCharaManager != nullptr)
		{
			g_pEnvSet->m_bEnableFrindlyMark = m_bEnableFrindlyMark;
			g_pEnvSet->m_bEnableFrindlyName = TRUE;
			for( INT32 i=0; i<g_pCharaManager->getCharaCount(); i++)
			{
				g_pCharaManager->getChara(i)->Cmd_Enable_TeamTag( (m_bEnableFrindlyMark)  ? true : false );
			}
		}

		if( g_pEnvSet->m_bEnableBlood != m_bEnableBlood )
		{
			g_pEffectManager->NeetToInitAgeLimitEffect();

			char filename[MAX_PATH] = "";
			if (UserInfoContext::i()->IsAdultUser())
			{
				i3::safe_string_copy( filename, "Gui/Ingame/blood.i3i", MAX_PATH);
			}
			else
			{
				i3::safe_string_copy( filename, "Gui/Ingame/blood_green.i3i", MAX_PATH);
			}
			/*if( m_bEnableBlood )
				i3::safe_string_copy( filename, "Gui/Ingame/blood_green.i3i", MAX_PATH );
			else
				i3::safe_string_copy( filename, "Gui/Ingame/blood_HitEffect.i3i", MAX_PATH );*/

			crb::i()->ChangeHudTexture(HUD_TEXTURE_DAMAGE_FILTER, filename);
			i3Texture* pTex = crb::i()->LoadHudTexture(HUD_TEXTURE_DAMAGE_FILTER);
			if( CHud::i())
				CHud::i()->SetDamageFilterInGame( pTex );		
			I3_SAFE_RELEASE(pTex);
		}
	}else
	{
		/*
		-- 2015-01-15 PB_ENG ��뿵 ���뿡�� �ǽð����� ���� �Ǵ°��� �ҷ��� �Ͽ�����, �ǽð����� ������ �ǳ�
					  �̹� �����ִ� UI���� �������� ��� ����������ϴ� ����� ������ ������ ���� �۾� �Ҵ� �޵��� �����Ǿ����ϴ�.
					  ������ ��ȹ ������� �˾� â�� �� �� �ֵ��� �����մϴ�.
		i3UIManager* pUI = g_pFramework->getUIManager();
		if (pUI && g_pEnvSet->m_nLanguageID != m_nLanguageID)
		{
			g_pEnvSet->m_nLanguageID = m_nLanguageID;
			
			char fullPath[MAX_PATH];
			i3::snprintf(fullPath, sizeof(fullPath), "Locale/%s", GlobalFunc::GetLocaleFolderName());
			
			i3UI::getManager()->SetLocale(GlobalFunc::GetLocaleFolderName());
			i3UI::getManager()->SetLocaleUIFolder(fullPath);
			
			pUI->CloseVirtualTexture();
			pUI->CloseTemplateLibrary();
			char pszLocaleVTexPath[MAX_PATH];
			i3::snprintf(pszLocaleVTexPath, sizeof(pszLocaleVTexPath), "UI_V15/PointBlankRe.i3Uil");
			i3::snprintf(fullPath, sizeof(fullPath), "%s/%s", i3UI::getManager()->GetLocaleUIFolder()->c_str(), pszLocaleVTexPath);
			pUI->OpenTemplateLibrary(fullPath);
		}
		*/

		// ���߾�� ������ �ڵ��� �� �ѵ� ���� ������� �����Ƿ� Ŭ���̾�Ʈ �������� �ʵ��� �մϴ� gyungju.kim
		//if( g_pEnvSet->m_nLanguage != m_nLanguage )
		//{
		//	GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, L"���� ������ ������ �̹��� ������\n ���� ����� �� ���� �˴ϴ�.", nullptr, MAKE_CALLBACK(cb_ExitClient), this);
		//}
	}
}

void UIOptionGame::apply_lua()
{
	LuaState * L = 0;

	// Button ���� �ε�����...
	L = _CallLuaFunction( "SetValue");
	i3Lua::PushInteger( L, m_bEnableFrindlyMark);
	i3Lua::PushInteger( L, m_bEnableBlood);
	i3Lua::PushInteger( L, m_nHandType);
	i3Lua::PushInteger( L, m_nCrossHairType);		// ũ�ν���� Ÿ��
	i3Lua::PushInteger( L, m_bCrossHairEffect);		// ũ�ν���� Ÿ�� ȿ��
	i3Lua::PushInteger( L, m_nGameInterface);
	i3Lua::PushInteger( L, m_bTeamBand);
	i3Lua::PushInteger( L, m_bMiniMapMode);

	//UI 1.0, 1.1������ ����ϱ⶧���� ��������(lua ��ũ��Ʈ�� �ٸ�)
	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
		UILua::EndLuaFunction(L, 8);
	else
	{
		i3Lua::PushInteger(L, m_bTextEffect);
		UILua::EndLuaFunction(L, 9);
	}

	L = _CallLuaFunction( "EnableInterface");
	if(g_pEnvSet->m_nUIVersion == UFV_1_5)
		i3Lua::PushBoolean( L, !((UIPopupOption*)getParent())->m_InGame );
	else
		i3Lua::PushBoolean( L, false );
	UILua::EndLuaFunction(L , 1);

	//DEV������ ��� �ɼ� ���(PP-22682)
	L = _CallLuaFunction("EnableBand");
#if defined(DEV_BUILD)
	i3Lua::PushBoolean(L, true);
#else
	i3Lua::PushBoolean(L, (g_pEnvSet->m_nUIVersion == UFV_1_5));
#endif
	_EndLuaFunction(L, 1);

//	L = _CallLuaFunction( "EnableBloodFilter");
//#if defined(LOCALE_KOREA)
//	i3Lua::PushBoolean(L, true);
//#else
//	i3Lua::PushBoolean(L, false);
//#endif
//	UILua::EndLuaFunction(L,1);

	// ��� ��� �޺��ڽ�	
	ucf::ComboString language;
	if(InsertLanguage("MiddleEast", RESOURCE_LANGUAGE_MIDDLE_EAST))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_ARABIC");
	if(InsertLanguage("Taiwan", RESOURCE_LANGUAGE_TAIWAN))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_CHINESE");
	if(InsertLanguage("English", RESOURCE_LANGUAGE_ENGLISH))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_ENG");
	if(InsertLanguage("Philippines", RESOURCE_LANGUAGE_PHILIPPINES))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_ENG_PH");
	if(InsertLanguage("Singmal", RESOURCE_LANGUAGE_SINGMAL))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_ENG_SAM");
	if(InsertLanguage("NorthAmerica", RESOURCE_LANGUAGE_NORTHAMERICA))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_ENG_US");
	if(InsertLanguage("Indonesia", RESOURCE_LANGUAGE_INDONESIA))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_INDO");
	if(InsertLanguage("Italy", RESOURCE_LANGUAGE_ITALY))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_IT");
	if(InsertLanguage("Korea", RESOURCE_LANGUAGE_KOREA))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_KR");
	if(InsertLanguage("Brazil", RESOURCE_LANGUAGE_BRAZIL))
		language	<<GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_BR");
	if(InsertLanguage("Russia", RESOURCE_LANGUAGE_RUSSIA))
		language	<< GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_RU");
	if(InsertLanguage("Latin_America", RESOURCE_LANGUAGE_LATIN_AMERICA))
		language	<< GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_CH");
	if(InsertLanguage("Thai", RESOURCE_LANGUAGE_THAILAND))
		language	<< GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_TW");
	if(InsertLanguage("Turkey", RESOURCE_LANGUAGE_TURKEY))
		language	<< GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_TU");
	if(InsertLanguage("Vietnam", RESOURCE_LANGUAGE_VIETNAM))
		language	<< GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_VT");
	if (InsertLanguage("Japan", RESOURCE_LANGUAGE_JAPAN))
		language << GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_JP");
#ifdef DEV_BUILD
	if(InsertLanguage("Dev", RESOURCE_LANGUAGE_DEV))
		language	<< GAME_STRING("STR_OPT_TAP_GAME_LANGUAGE_DEV");
#endif

	L = _CallLuaFunction("SetLangaugeList");
	const i3::wstring& cs_wstr = language;
	i3Lua::PushStringUTF16To8(L, cs_wstr.c_str(), cs_wstr.size());
	i3Lua::PushInteger(L, FindLangListIndexToLangID(m_nLanguage));
	UILua::EndLuaFunction(L, 2);

	//TextEffect�� UI 1.0, 1.1������ ����ϱ⶧����(lua ��ũ��Ʈ�� �ٸ�)
	if (g_pEnvSet->m_nUIVersion != UFV_1_5)
	{
		L = _CallLuaFunction("EnableTextEffect");
		i3Lua::PushBoolean(L, true);
		_EndLuaFunction(L, 1);
	}
}

bool UIOptionGame::is_modified()
{
	if( m_bEnableFrindlyMark != g_pEnvSet->m_bEnableFrindlyMark )		return true;
	if( m_bEnableBlood != g_pEnvSet->m_bEnableBlood )					return true;
	if( m_nHandType != g_pEnvSet->m_nHandType )							return true;
	if( m_nCrossHairType != g_pEnvSet->m_nCrossHairType )				return true;
	if( m_bCrossHairEffect != g_pEnvSet->m_bCrossHairEffect )			return true;
	if( m_bMiniMapMode != g_pEnvSet->m_bMiniMapMode )					return true;
	if( m_nGameInterface != g_pEnvSet->m_nGameInterface )				return true;
	if( m_bTeamBand != g_pEnvSet->m_bTeamBand )							return true;
	if( m_nLanguage != g_pEnvSet->m_nLanguage )							return true;
	if (m_bTextEffect != g_pEnvSet->m_bTextEffect)						return true;

	return false;
}

bool UIOptionGame::InsertLanguage(char* language, INT16 languageType)
{
	char strLanguage[MAX_STRING_COUNT];
	i3::snprintf( strLanguage, sizeof(strLanguage), "Language/%s", language);

	// LocaleValue.pef�� �ش� �������� language �� ����ϴ��� üũ
	if ( !LocaleValue::Enable(strLanguage))
		return false;

	// ������� ��ġ�Ǿ����� Locale ��ο� Virtual Texture ������ �����ؾ���
	// ���� �������� �Լ� ������ ����� ������ �����ؾ���
	//if(!CheckLanguagePack())
	//	return false;

	// language �� ����ϰ� ����ѵ� ��ġ �Ǿ����� ��Ͽ� ����Ѵ�.
	m_LanguageList.push_back(languageType);
	I3TRACE("%s languageID : %d", language, languageType);
	return true;
}

// ����� ��ġ ���θ� Ȯ���ϴ� �Լ�
// ���� ����� ��ο� PointBlankRe.i3RefTex ���� ���� ���θ� Ȯ�� - ���� Ȯ�� ��� ��� ���� �ʿ�
bool UIOptionGame::CheckLanguagePack()
{
	char conv[MAX_PATH];
	i3::snprintf(conv, sizeof(conv), "UI_V15/PointBlankRe.i3RefTex");
	const char * pLocalePath = GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage);

	if(GlobalFunc::IsFileInLocaleFolder( conv, pLocalePath))
		return true;

	return false;
}

void UIOptionGame::SelectLanguage(INT32 index)
{
	INT32 size = m_LanguageList.size();
	if( (0 > index && index >= size) )
		return;

	m_nLanguage = m_LanguageList[index];
}

INT32 UIOptionGame::FindLangListIndexToLangID(INT32 languageType)
{
	INT32 size = m_LanguageList.size();
	for(INT32 i = 0 ; i < size ; i++)
	{
		if(m_LanguageList[i] == languageType)
			return i;
	}
	return 0;
}

void UIOptionGame::cb_ExitClient(void* pThis,INT32 nParam)
{
	if(nParam == MB_OK)
	{
		GameEventSender::i()->SetEvent( EVENT_GAME_END );
		g_pFramework->GotoNextStage("Exit");
	}
}