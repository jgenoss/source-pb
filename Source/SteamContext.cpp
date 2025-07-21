#include "pch.h"
#include "SteamContext.h"
#include "UI/UIServerAndChannelBanner.h"
#include "UI/UIUtil.h"
#include "../Steam/include/steam_api.h"

SteamContext::SteamContext() : m_bRecvSteamCallback(false), m_pSteamUser(nullptr), m_bEnableSteam(false), m_CallbackGameOverlayActivated(this, &SteamContext::OnGameOverlayActivated)
{
	m_hSteamDll = LoadLibrary(_T("steam_api.dll"));

	if (m_hSteamDll)
	{
		m_CallbackGameOverlayActivated.SetDLL(m_hSteamDll);
		m_CallbackGameOverlayActivated.Register(this, &SteamContext::OnGameOverlayActivated);

		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Steam_API.dll okay");
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Steam_API.dll Load Fail");
	}
}

SteamContext::~SteamContext()
{
	if (m_hSteamDll)
	{
		m_CallbackGameOverlayActivated.Unregister();
		m_CallbackGameOverlayActivated.SetDLL(nullptr);
		FreeLibrary(m_hSteamDll);
	}
}

bool SteamContext::IsSteamUser()
{
	if (!m_hSteamDll)
		return false;

	m_pSteamUser = (ISteamUser*(*)())GetProcAddress(m_hSteamDll, "SteamUser");
	if (!m_pSteamUser)
		return false;

	return true;
}

bool SteamContext::SteamAPI_Init(LPTSTR lpCmdLine)
{
	int iLocaleCode = atoi(lpCmdLine);

	I3PRINTLOG(I3LOG_NOTICE, "lpCmdLine : %s", lpCmdLine);

	// Cmdline�� ��ġ���� �ʴ´ٸ� DLL�� �־ ��������� ó������ �ʽ��ϴ�.
	if (!(iLocaleCode > LOCALE_CODE_NONE && iLocaleCode < LOCALE_CODE_MAX))
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] LocaleCod Error");
		m_hSteamDll = false; //���� �ּ��� ������, �ڿ� �α��ν� ������ ������ üũ�ϱ� ������ dll�� ���ش�
		return true;
	}

	// DLL ���翩�� �˻�
	// �� ��쿡�� steam_api.dll �� ���ٸ� �������尡 �ƴϱ� ������
	// �Ϲ� ����� �����ϱ� ���ؼ� true�� ��ȯ�մϴ�.
	if (!m_hSteamDll)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] SteamDll did not Load");
		return true;
	}

	// CmdLine�� ��ġ. DLL�� �����Ѵٸ� �ݵ�� SteamBuild �̱� ������ ���� �������� ������ ������Ѿ��մϴ�.
	bool(*pSteam_Init)();
	pSteam_Init = (bool(*)())GetProcAddress(m_hSteamDll, "SteamAPI_Init");

	if (!pSteam_Init)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] pSteam_Init Fail");
		return false;
	}

	// SteamAPI_init() �Լ� ����.
	// ������ó �̱��� �� false �� �޽��ϴ�. 
	// Dev������ ��� steam_appid.txt �� �����ؾ� true�� ��ȯ�˴ϴ�.
	m_bEnableSteam = (*pSteam_Init)();

	I3PRINTLOG(I3LOG_NOTICE, "[STEAM] m_bEnableSteam %d", m_bEnableSteam);
	return m_bEnableSteam;
}

// Ư���� ū �ǹ̴� ������ RequestEncryptedAppTicket�� ���˴ϴ�.
uint32 k_unSecretData = 0x5444;

void SteamContext::SteamAPI_RunCallbacks()
{
	void(*pSteam_RunCallback)();

	pSteam_RunCallback = (void(*)())GetProcAddress(m_hSteamDll, "SteamAPI_RunCallbacks");
	if (pSteam_RunCallback)
	{
		(*pSteam_RunCallback)();
	}
}

bool SteamContext::RequestLoginTicket()
{
	if (!m_hSteamDll)
		return false;

	if (!m_bEnableSteam)
	{
		ShowWindow(g_hWnd, SW_HIDE);

		//Msg �ڽ� ���
		//Run STEAM client as administratorPlease run STEAM client as administrator
		::MessageBox(NULL, "Please run STEAM client as an administrator.", "Error", MB_OK);
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Login Error : Not Enable Steam");

		GameEventSender::i()->SetEventOnce(EVENT_GAME_END);
		g_pFramework->GotoNextStage("Exit");

		return true;
		//
	}

	// SteamUser ���� �˻�
	ISteamUser* (*pSteamUser)(void);
	pSteamUser = (ISteamUser*(*)())GetProcAddress(m_hSteamDll, "SteamUser");

	if (!pSteamUser)
		return false;

	//I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Login : %x", pSteamUser);

	//// AppTicekt ��û�ϴ� CallBack ���
	SteamAPICall_t cbRequestAppTicket = (*pSteamUser)()->RequestEncryptedAppTicket(&k_unSecretData, sizeof(k_unSecretData));
	m_EncryptedTicketResult.Set(cbRequestAppTicket, this, &SteamContext::OnRequestEncryptedAppTicket, m_hSteamDll);

	// �ݹ� Recv ����.
	m_bRecvSteamCallback = false;

	// �� 1�ʿ� �ѹ��� �� 10ȸ �õ��մϴ�. 
	for (int i = 0; i < 10; ++i)
	{
		// �ݵ�� �ణ�� ������ �� ȣ��Ǿ�� �մϴ�. 
		// SteamAPI_RunCallbacks() �� ȣ���� ��, Callback ������� �־�� OnRequestEncryptedAppTicket�� ����Ǳ� �����Դϴ�.
		Sleep(1000);
		// SteamAPI_RunCallbacks() ȣ��. 
		SteamAPI_RunCallbacks();

		// ���������� �ݹ��� �޾Ҵٸ� escape. 
		if (m_bRecvSteamCallback)
			break;
	}

	// �ݹ��� ���� ���ߴٸ� Ŭ���̾�Ʈ �����մϴ�
	// steam_api.dll�� �����Կ��� �ұ��ϰ� Steam �α��� ��ü�� �����ߴٴ� �ǹ��̱� �����Դϴ�.
	if (!m_bRecvSteamCallback)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Login Error : cb"); 
		GameEventSender::i()->SetEventOnce(EVENT_GAME_END);
		g_pFramework->GotoNextStage("Exit");
		return false;
	}

	return true;
}

void SteamContext::OnGameOverlayActivated(GameOverlayActivated_t *pOverlay)
{
	bool bOverlay = pOverlay->m_bActive ? true : false;

	UIServerAndChannelBanner * pBanner = static_cast<UIServerAndChannelBanner*>(GameUI::GetFloating(UFW_SCBANNER));
	if(pBanner != nullptr) 	pBanner->ShowHideWebAunnounce(!bOverlay);
}

void SteamContext::OnRequestEncryptedAppTicket(EncryptedAppTicketResponse_t *pEncryptedAppTicketResponse, bool bIOFailure)
{
	if (bIOFailure)
		return;

	// ����� ����Ǿ��� �ݹ��� �޾����� true�� �ٲ���
	m_bRecvSteamCallback = true;

	if (pEncryptedAppTicketResponse->m_eResult == k_EResultOK)
	{
		GameEventSender::i()->SetEvent(EVENT_LOG_IN_WEBKEY);
	}
	else 
	{
		// �ݹ��� ������� ������ �ƴ� ����Դϴ�.
		// �Ʒ� �ּ��� �����Ͽ� ������ �� �� �ֽ��ϴ�.
		//{
		//	if (!m_hSteamDll)
		//		return;

		//	ISteamUtils* (*pSteamUtils)(void);
		//	pSteamUtils = (ISteamUtils*(*)())GetProcAddress(m_hSteamDll, "SteamUtils");
		//	if (!pSteamUtils)
		//		return;

		//	ESteamAPICallFailure result = (*pSteamUtils)()->GetAPICallFailureReason(m_cbRequestAppTicket);
		//	
		//	if (result == k_ESteamAPICallFailureSteamGone)
		//	{
		//		// the local Steam process has gone away
		//		// . . . "isteamutils.h"  �� ��� Ȯ���� �� �ֽ��ϴ�.
		//	}
		//}

		I3TRACE("[STEAM] Enc. Login ERROR");
		GameEventSender::i()->SetEventOnce(EVENT_GAME_END);
		g_pFramework->GotoNextStage("Exit");
	}
}

ISteamUser* SteamContext::GetSteamUser()
{
	return (*m_pSteamUser)();
}

bool SteamContext::SetAchievement(const char* NameKey)
{
	if (!m_hSteamDll)
		return false;

	ISteamUserStats* (*pSteam_UserStats)(void); 

	pSteam_UserStats = (ISteamUserStats*(*)())GetProcAddress(m_hSteamDll, "SteamUserStats");
	if (!pSteam_UserStats)
		return false;

	return (*pSteam_UserStats)()->SetAchievement(NameKey);
}

bool SteamContext::ActivateWebPage(const char* strURL)
{
	if (!m_hSteamDll)
		return false;

	ISteamFriends* (*pSteam_ActivateWebPage)(void);

	pSteam_ActivateWebPage = (ISteamFriends*(*)())GetProcAddress(m_hSteamDll, "SteamFriends");
	if ( !pSteam_ActivateWebPage )
		return false;

	(*pSteam_ActivateWebPage)()->ActivateGameOverlayToWebPage(strURL);

	return true;
}