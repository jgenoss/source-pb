#include "pch.h"
#include "ConfigRenderBattle.h"
#include "MyRoomInfoContext.h"
#include "UserInfoContext.h"



config_render_battle::config_render_battle()
{
	i3mem::FillZero(m_pHudTexture, sizeof(m_pHudTexture));
}

void	config_render_battle::ProcessCreateStage()
{
	i3Color::Set( &m_DefaultDiffuse,	1.0f, 0.0f, 0.0f, 1.0f);
	i3Color::Set( &m_Ambient,			0.7f, 0.7f, 0.7f, 1.0f);

	config_render_battle::_InitHudTexture();
}

void	config_render_battle::ProcessDestroyStage()
{
	for(UINT32 i = 0; i < MAX_HUD_TEXTURE; i++)
	{
		I3_SAFE_RELEASE(m_pHudTexture[i]);
	}
}


void	config_render_battle::ProcessLoadProperties(i3RegKey* key)
{
	COLOR col;

	FIND_REG_DATA( key, "DefaultDiffuse", &col);
	i3Color::Set( &m_DefaultDiffuse, &col);

	FIND_REG_DATA( key, "Ambient", &col);
	i3Color::Set( &m_Ambient, &col);
	g_pFramework->getSgContext()->setAmbient( &m_Ambient);
}

void	config_render_battle::_InitHudTexture(void)
{
	char filename[MAX_PATH] = "";

#if defined (MULTYLANGUAGE)
	i3::string strLoadFilePath;
	const char* LocalePath  = GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage);

	i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Ingame/HUD_01.i3i", LocalePath );	
	m_pHudTexture[HUD_TEXTURE_HUD]	= g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD] != nullptr);

	i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Ingame/HUD_02.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
	m_pHudTexture[HUD_TEXTURE_HUD2]	= g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD2] != nullptr);

	// HUD3�� ������ ��ȭ ���� hud �������� ���� Hud3�� GameFramework ���� GameSupportSet::InitHud3Texture() �޼���(CGameFramework - 848 Line)�� �ε�� �ؽ�ó�� �����͸� �����Ѵ�.
	m_pHudTexture[HUD_TEXTURE_HUD3]					= g_pFramework->LoadHud3Texture();	
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD3] != nullptr);

	i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Ingame/HUD_04.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
	m_pHudTexture[HUD_TEXTURE_HUD4]	= g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD4] != nullptr);

	i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Ingame/kill_message_01.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
	m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE]	= g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE] != nullptr);

	i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Ingame/kill_message_02.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
	m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE1] = g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE1] != nullptr);

	i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Ingame/kill_message_03.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );
	m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE2] = g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE2] != nullptr);
#else
#if defined (USE_LOCALE_UI)
	m_pHudTexture[HUD_TEXTURE_HUD]					= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Hud_01.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD] != nullptr);

	m_pHudTexture[HUD_TEXTURE_HUD2]					= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Hud_02.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD2] != nullptr);

	// HUD3�� ������ ��ȭ ���� hud �������� ���� Hud3�� GameFramework ���� GameSupportSet::InitHud3Texture() �޼���(CGameFramework - 848 Line)�� �ε�� �ؽ�ó�� �����͸� �����Ѵ�.
	m_pHudTexture[HUD_TEXTURE_HUD3]					= g_pFramework->LoadHud3Texture();	
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD3] != nullptr);

	if(UserInfoContext::i()->IsGM_Observer())
	{
		m_pHudTexture[HUD_TEXTURE_HUD4]				= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/hud4.i3i");	
		I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD4] != nullptr);
	}
	else
		m_pHudTexture[HUD_TEXTURE_HUD4]	= nullptr;

	m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE]			= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/kill_message_01.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE] != nullptr);

	m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE1]			= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/kill_message_02.i3i");


#else
	m_pHudTexture[HUD_TEXTURE_HUD]					= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Hud.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD] != nullptr);

	m_pHudTexture[HUD_TEXTURE_HUD2]					= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/hud2.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD2] != nullptr);

	// HUD3�� ������ ��ȭ ���� hud �������� ���� Hud3�� GameFramework ���� GameSupportSet::InitHud3Texture() �޼���(CGameFramework - 848 Line)�� �ε�� �ؽ�ó�� �����͸� �����Ѵ�.
	m_pHudTexture[HUD_TEXTURE_HUD3]					= g_pFramework->LoadHud3Texture();	
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD3] != nullptr);

	if(UserInfoContext::i()->IsGM_Observer())
	{
		m_pHudTexture[HUD_TEXTURE_HUD4]				= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/hud4.i3i");	
		I3ASSERT( m_pHudTexture[HUD_TEXTURE_HUD4] != nullptr);
	}
	else
		m_pHudTexture[HUD_TEXTURE_HUD4]	= nullptr;

	m_pHudTexture[HUD_TEXTURE_CHARA_MARK2]			= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Chara_Mark1.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_CHARA_MARK2] != nullptr);

	m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE]			= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/kill_message.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE] != nullptr);

	m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE1]			= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/kill_message1.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_KILL_MESSAGE1] != nullptr);
#endif
#endif

	m_pHudTexture[HUD_TEXTURE_LEVEL_ICON]			= g_pFramework->GetResourceManager()->LoadTexture("Gui/Interface/level_icon.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_LEVEL_ICON] != nullptr);

#if defined (MULTYLANGUAGE)
	i3::stack_string strHelpImgFilePath;
	i3::sprintf( strHelpImgFilePath, "Locale/%s/GUI/Interface/help.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
	m_pHudTexture[HUD_TEXTURE_HELP]				= g_pFramework->GetResourceManager()->LoadTexture(strHelpImgFilePath.c_str());
#else
	m_pHudTexture[HUD_TEXTURE_HELP]					= g_pFramework->GetResourceManager()->LoadTexture("Gui/Interface/help.i3i");
#endif
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_HELP] != nullptr);

#if defined (MULTYLANGUAGE)
	i3::stack_string strGMHelpImgFilePath;
	i3::sprintf( strGMHelpImgFilePath, "Locale/%s/GUI/Interface/help_observer.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage));
	m_pHudTexture[HUD_TEXTURE_HELP_OBSERVER] = g_pFramework->GetResourceManager()->LoadTexture(strGMHelpImgFilePath.c_str());
#else
	m_pHudTexture[HUD_TEXTURE_HELP_OBSERVER] = g_pFramework->GetResourceManager()->LoadTexture("Gui/Interface/help_observer.i3i");
#endif
	I3ASSERT(m_pHudTexture[HUD_TEXTURE_HELP_OBSERVER] != nullptr);

	if (MyRoomInfoContext::i()->getStage()->IsAiMode())
	{
#if defined (MULTYLANGUAGE)
		i3::stack_string strTraingKillImgFilePath;
		i3::sprintf( strTraingKillImgFilePath, "Locale/%s/GUI/Ingame/Training_Kill.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
		m_pHudTexture[HUD_TEXTURE_TRAINING_KILL_MESSAGE] = g_pFramework->GetResourceManager()->LoadTexture(strTraingKillImgFilePath.c_str());
#else
		m_pHudTexture[HUD_TEXTURE_TRAINING_KILL_MESSAGE] = g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Training_Kill.i3i");
#endif
		I3ASSERT( m_pHudTexture[HUD_TEXTURE_TRAINING_KILL_MESSAGE] != nullptr);
	}

	m_pHudTexture[HUD_TEXTURE_KILLHISTORY_BG]		= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/kill_mark_HistoryBG.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_KILLHISTORY_BG] != nullptr);

#if defined (MULTYLANGUAGE)
	i3::stack_string strPauseImgFilePath;
	i3::sprintf( strPauseImgFilePath, "Locale/%s/GUI/Ingame/pause.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
	m_pHudTexture[HUD_TEXTURE_PAUSE]				= g_pFramework->GetResourceManager()->LoadTexture(strPauseImgFilePath.c_str());
#else
	m_pHudTexture[HUD_TEXTURE_PAUSE]				= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/pause.i3i");
#endif
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_PAUSE] != nullptr);

#if defined (THAILAND_SONGKRAN_EVENT)		//	�±� ������ �Ǹ� �ϴû�����

	i3::safe_string_copy( filename, "Gui/Ingame/blood_blue.i3i");

#else
	if (UserInfoContext::i()->IsAdultUser())
	{
		i3::safe_string_copy( filename, "Gui/Ingame/blood.i3i", MAX_PATH);
	}
	else
	{
		i3::safe_string_copy( filename, "Gui/Ingame/blood_green.i3i", MAX_PATH);
	}
#endif

	// �ǰ������� ��� ���ǿ��ɿ� ���� �޸� ����Ѵ�.
	m_pHudTexture[HUD_TEXTURE_DAMAGE_FILTER] = g_pFramework->GetResourceManager()->LoadTexture( filename);
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_DAMAGE_FILTER] != nullptr);

	m_pHudTexture[ HUD_TEXTURE_GRENADELAUNCHER_CROSSHAIR] = g_pFramework->GetResourceManager()->LoadTexture( "Gui/Hud/hud_K-201.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_GRENADELAUNCHER_CROSSHAIR] != nullptr);

#if !defined(USE_LOCALE_UI)
	m_pHudTexture[ HUD_TEXTURE_SKILL_DOMINATION] = g_pFramework->GetResourceManager()->LoadTexture( "Gui/Ingame/domination.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_SKILL_DOMINATION] != nullptr);
#endif

#if legacy_not_use_texture
	// �Ʒ���(Ÿ���� ������ �Ʒ���)�� ���� ������ �ʰ� �����Ƿ�, ���ҽ��� ���� �������� ���ҽ� �ε��� ���� �ʽ��ϴ�.
	//m_pHudTexture[HUD_TEXTURE_TRAININGSCORE]		= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/ui_firing range.i3i");

	m_pHudTexture[HUD_TEXTURE_CHARA_MARK]			= g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/Chara_Mark.i3i");
	I3ASSERT( m_pHudTexture[HUD_TEXTURE_CHARA_MARK] != nullptr);
#endif
}


//�Լ��� ȣ���ϸ� refcount�� ������ �����ϵ��� �Ǿ� �����Ƿ�
// refcount�� �����ؾ��Ѵ�.
i3Texture* config_render_battle::LoadHudTexture(HUD_TEXTURE_TYPE texType)
{
	if (m_pHudTexture[texType])
	{
		I3_MUST_ADDREF(m_pHudTexture[texType]);
		return m_pHudTexture[texType];
	}

	return nullptr;
}

void config_render_battle::ChangeHudTexture(HUD_TEXTURE_TYPE dstHudTextureType, const char* pszSrc)
{
	I3_SAFE_RELEASE( m_pHudTexture[dstHudTextureType] );

	m_pHudTexture[dstHudTextureType] = g_pFramework->GetResourceManager()->LoadTexture( pszSrc);
}
