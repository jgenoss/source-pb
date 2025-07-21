#ifndef _STEAM_CONTEXT_H_
#define _STEAM_CONTEXT_H_

#include "../Steam/include/steam_api.h"

/* 
	LocaleCode �� �����ϰ� steam_api.dll�� �����ϸ� �̸� load�Ͽ� 
	Steam�� �α��� ó���� �����ϵ��� ���� Ŭ�����Դϴ�.
	
	dll�� ������ ���
	[ĳ������] ����� ������ SteamContext::ActivateWebPage() �� �����ϰ� �Ǹ�
	UserInfoContext::i()->GetBillingURL() �� Get������� BillingID/ExtUID �� �ٽ��ϴ�.
*/

// ����� ��ũ ������ ���� ��� ���ø�(CCallResult/CCallback)�� ����� �� ���
// base�� ��ӹ��� ��, PB���� ����� �� �ְ� �����Ͽ� ������.
template< class T, class P >
class CCallResult_PB : private CCallbackBase
{
public:
	typedef void (T::*func_t)(P*, bool);

	CCallResult_PB()
	{
		m_hAPICall = k_uAPICallInvalid;
		m_pObj = nullptr;
		m_Func = nullptr;
		m_hDLL = nullptr;
		m_iCallback = P::k_iCallback;
	}

	void Set(SteamAPICall_t hAPICall, T *p, func_t func, HINSTANCE hDLL)
	{
		if (!hDLL)
		{
			return;
		}

		m_hDLL = hDLL;

		if (m_hAPICall)
		{
			void(*pSteamAPI_UnregisterCallResult)(class CCallbackBase*, SteamAPICall_t);
			pSteamAPI_UnregisterCallResult = (void(*)(class CCallbackBase*, SteamAPICall_t))GetProcAddress(m_hDLL, "SteamAPI_UnregisterCallResult");
			(*pSteamAPI_UnregisterCallResult)(this, m_hAPICall);
		}

		m_hAPICall = hAPICall;
		m_pObj = p;
		m_Func = func;

		if (hAPICall)
		{
			void(*pSteamAPI_UnregisterCallResult)(class CCallbackBase*, SteamAPICall_t);
			pSteamAPI_UnregisterCallResult = (void(*)(class CCallbackBase*, SteamAPICall_t))GetProcAddress(m_hDLL, "SteamAPI_RegisterCallResult");
			(*pSteamAPI_UnregisterCallResult)(this, hAPICall);
		}
	}

	bool IsActive() const
	{
		return (m_hAPICall != k_uAPICallInvalid);
	}

	void Cancel()
	{
		if (!m_hDLL)
			return;

		if (m_hAPICall != k_uAPICallInvalid)
		{
			void(*pSteamAPI_UnregisterCallResult)(class CCallbackBase*, SteamAPICall_t);
			pSteamAPI_UnregisterCallResult = (void(*)(class CCallbackBase*, SteamAPICall_t))GetProcAddress(m_hDLL, "SteamAPI_RegisterCallResult");
			(*pSteamAPI_UnregisterCallResult)(this, m_hAPICall);
			m_hAPICall = k_uAPICallInvalid;
		}

	}

	~CCallResult_PB()
	{
		Cancel();
	}

	void SetGameserverFlag() { m_nCallbackFlags |= k_ECallbackFlagsGameServer; }
private:
	virtual void Run(void *pvParam) override
	{
		m_hAPICall = k_uAPICallInvalid;
		(m_pObj->*m_Func)((P *)pvParam, false);
	}
	void Run(void *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall)
	{
		if (hSteamAPICall == m_hAPICall)
		{
			m_hAPICall = k_uAPICallInvalid;
			(m_pObj->*m_Func)((P *)pvParam, bIOFailure);
		}
	}
	int GetCallbackSizeBytes()
	{
		return sizeof(P);
	}

	SteamAPICall_t m_hAPICall;
	T *m_pObj;
	func_t m_Func;
	HINSTANCE	m_hDLL;
};

template< class T, class P>
class CCallback_PB : protected CCallbackBase
{
public:
	typedef void (T::*func_t)(P*);
	HINSTANCE m_hDLL;

	CCallback_PB(T *pObj, func_t func) : m_pObj(pObj), m_Func(func), m_hDLL()
	{
		if (pObj && func)
			Register(pObj, func);
	}

	~CCallback_PB()
	{
		if (m_nCallbackFlags & k_ECallbackFlagsRegistered)
			Unregister();
	}

	void SetDLL(HINSTANCE hDLL)
	{
		m_hDLL = hDLL;
	}
	// manual registration of the callback
	void Register(T *pObj, func_t func)
	{
		if (!pObj || !func)
			return;

		if (m_nCallbackFlags & k_ECallbackFlagsRegistered)
			Unregister();

		m_pObj = pObj;
		m_Func = func;

		if (m_hDLL)
		{
			void(*pSteamAPI_registerCall)(class CCallbackBase*, int);
			pSteamAPI_registerCall = (void(*)(class CCallbackBase*, int))GetProcAddress(m_hDLL, "SteamAPI_RegisterCallback");
			(*pSteamAPI_registerCall)(this, P::k_iCallback);
		}
	}

	void Unregister()
	{
		if (m_hDLL)
		{
			// SteamAPI_UnregisterCallback removes k_ECallbackFlagsRegistered
			void(*pSteamAPI_UnregisterCall)(class CCallbackBase*);
			pSteamAPI_UnregisterCall = (void(*)(class CCallbackBase*))GetProcAddress(m_hDLL, "SteamAPI_UnregisterCallback");
			(*pSteamAPI_UnregisterCall)(this);
		}
	}

	void SetGameserverFlag() { m_nCallbackFlags |= k_ECallbackFlagsGameServer; }
protected:
	virtual void Run(void *pvParam) override
	{
		(m_pObj->*m_Func)((P *)pvParam);
	}
	virtual void Run(void *pvParam, bool, SteamAPICall_t) override
	{
		(m_pObj->*m_Func)((P *)pvParam);
	}
	int GetCallbackSizeBytes()
	{
		return sizeof(P);
	}

	T *m_pObj;
	func_t m_Func;
};


class SteamContext : public i3::shared_ginst<SteamContext>
{
private: 
	HINSTANCE m_hSteamDll;
	ISteamUser* (*m_pSteamUser)(void);
	bool m_bEnableSteam;

public:
	SteamContext();
	virtual ~SteamContext(void);

public:
	// Client ����� ���� �����Ͽ� ���� ���� ����Ǿ�� �մϴ�.
	bool SteamAPI_Init(LPTSTR lpCmdLine);

	// ������������ üũ. steam_api.dll ���� ���ε� �� �Լ��� üũ�� �� �ֽ��ϴ�.
	bool IsSteamUser();

	// **����** IsSteamUser() �Լ��� ������� true �� ��쿡�� ȣ���ؾ��մϴ�. �ش� �Լ��� nullptr���� ���ϵǾ �ȵ˴ϴ�.
	ISteamUser* GetSteamUser();

	// ��������. �Ϸ��� ���������� StringKey���� ���ڷ� �Ѱ��ָ� Steam�� ������������ �Ϸ��� ������ ��µ˴ϴ�.
	bool SetAchievement(const char* NameKey);

	// ���������� ���� �� �������� ��� ���������� ������մϴ�. GET��ĸ� ����.
	bool ActivateWebPage(const char* strURL);

	// Steam�� ��ȣȭ �� �α����� ��û�ϴ� ������ ���ľ� GetEncryptedAppTicket ���� ��ȣȭ�� �α��� Ű ���� �޾ƿ� �� �ֽ��ϴ�.
	bool RequestLoginTicket();

	// Steam�ݹ� �Լ� ȣ��. ȣ�� ������� �ݹ��Լ��� ���ٸ� �ƹ� �ϵ� �Ͼ�� �ʽ��ϴ�.
	void SteamAPI_RunCallbacks();

public:
	bool				m_bRecvSteamCallback;

	CCallResult_PB< SteamContext, EncryptedAppTicketResponse_t > m_EncryptedTicketResult;
	void OnRequestEncryptedAppTicket(EncryptedAppTicketResponse_t *pEncryptedAppTicketResponse, bool bIOFailure);

	// �������� ���Ǵ� STEAM_CALLBACK �������� �Ʒ�ó�� ǥ���� �� �ֽ��ϴ�.
	// �ٸ� DLL ���� ������ ������ �Ҹ�� Register/Unregister callback �� ���� �������־�� �մϴ�.
	CCallback_PB<SteamContext, GameOverlayActivated_t> m_CallbackGameOverlayActivated;
	void OnGameOverlayActivated(GameOverlayActivated_t *pOverlay);

};

#endif