/**
 * \file	UI\UIPhaseBase.h
 *
 * Declares the user interface phase base class.
 */


#if !defined( __UI_PHASE_BASE_H__)
#define __UI_PHASE_BASE_H__

#include "UIBase.h"

typedef void (*CbINVITEPOPUP)(void * pThis, INT32 nParam);

enum UIPHASE;
class UIMainFrame;
class UIFramework;
class CUserLocationInfo;


static bool isWindowsXP()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL bOsVersionInfoEx = ::GetVersionEx ((OSVERSIONINFO *) &osvi);
	if( ! bOsVersionInfoEx )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
			return true;
	}

	if ( osvi.dwMajorVersion < 6 )
	{
		return true;
	}

	return false;
}

typedef UINT (CALLBACK* LPFNDLLFUNC) (GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
static CString getWritableFolder(void)
{ 
	CString strPath = _T("");
	TCHAR szPath[ MAX_PATH] = {0,};

	if( !isWindowsXP() )
	{ 
		PWSTR       path = nullptr;
		HINSTANCE   hInst;
		LPFNDLLFUNC pGetKnownFldPathFnPtr;

		// Vista �̻󿡼� �����/AppData/Local ���� ��� {F1B32785-6FBA-4FCF-9D55-7B8E7F157091}, CSIDL_LOCAL_APPDATA 
		GUID folderid_localappdata = {0xF1B32785, 0x6FBA, 0x4FCF, {0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91}};

		hInst = LoadLibrary(_T("shell32.dll"));

		pGetKnownFldPathFnPtr = (LPFNDLLFUNC)GetProcAddress(hInst, "SHGetKnownFolderPath");

		if ( pGetKnownFldPathFnPtr && SUCCEEDED( pGetKnownFldPathFnPtr( folderid_localappdata, 0, nullptr, &path) ) )
		{

#ifndef _UNICODE
			i3String::WideCharToMultiByte( CP_ACP, 0, path, wcslen(path), szPath, MAX_PATH, nullptr, nullptr );
			strPath = szPath;
#else
			strPath = path;

#endif
			CoTaskMemFree( path );
		}

		FreeLibrary( hInst );
	}
	else
	{
		::SHGetSpecialFolderPath( NULL, szPath, CSIDL_APPDATA, TRUE );
		strPath = szPath;
	}

	return strPath;
}

class UIPhaseBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPhaseBase, UIBase);

protected:
	i3UIScene *		m_pScene;
public:
	bool			m_bMyPhase;	// Event�� ������ �������� �ڽ��� ��ġ�� �ƴ� ������ �����Ͽ� ������ Event�� ������ ���ϵ��� �Ѵ�.

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIPhaseBase();
	virtual ~UIPhaseBase();

	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	virtual void	OnUpdate_NoneRsc(REAL32 rDeltaSeconds) {}

	bool			isMyPhase(void)		{ return m_bMyPhase; }

	//******************************************************************************//
	//								Base Functions									//
	//******************************************************************************//
	virtual bool	OnKey_Escape( void) override;

	virtual void	OnSlideInOnce() override;
	virtual void	OnSlideOutOnce() override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnExitStart( void) override;

	//******************************************************************************//
	//								Phase Functions									//
	//******************************************************************************//
	/** \brief Phase���� Event�� GameContext�� ������.
		\desc ���� Event�� ������ Phase�� �������� �ʴ´�.
			 OnEnter�� ȣ��ɽ� Phase�� �����ϵ��� �Ѵ�.	*/
	virtual void	Enter( void) {}

	/** \brief Phase���� Event�� �޾Ҵ� */
	virtual void	OnEnter( void) {}

	/** \brief Phase���� Event�� GameContext�� ������*/
	virtual void	Leave( void) {}

	/** \brief Phase���� Event�� �޾Ҵ�.*/
	virtual void	OnLeave( void) {}
	
	virtual void	OnNotifyPrevPhase(INT32  phase_type) {}    // ���� ���°� �ʿ�������� �ֱ� ������, 
																// ���⼭�� ����ΰ�, �Ļ�Ŭ������ �˾Ƽ�.. (11.06.27.����)
	virtual void	OnGotoBackPhase( void) {}
	virtual void	OnModifyPhase_EndSubPhase(UIPHASE& inoutCurrentPhase) {}		// ����Ʈ�� �ƹ��͵� ����...
	/** \brief ������ ���� */
	virtual void	AuthStartSuccess(INT32 arg) {}

	/** \brief �����ư */
	virtual void			ClickExit(void) {}

	//------------------------------------------------------------------------------//
	bool			IsRun( void);

	/** \brief UIMainFrame �����͸� ��ȯ�Ѵ� */
	UIMainFrame *	GetMainFrame( void);

	/** \brief ��Ʋ������ ���ؼ� �����Լ� �����մϴ� */
	virtual void	JoinGame(void) {}

	//------------------------------------------------------------------------------//
	//									���� ����									//
	//------------------------------------------------------------------------------//
protected:
	void			_ApplyMsgReadyBox( void);

	//------------------------------------------------------------------------------//
	//									�ʴ��ϱ�									//
	//------------------------------------------------------------------------------//
	
private:
	void		ReceiveGameEvent_InvitedMessage( INT32 arg, const i3::user_data& UserData);
	void		ReceiveGameEvent_InvitedMessageClan(INT32, const i3::user_data&);
	void		ReceiveGameEvent_InvitedMessageNormalUser(INT32, const i3::user_data&);

public:

	bool			IsEnableInviteMsg(INT64 i64UID);

	void			OpenInviteMessage(void);
	void			OpenInviteMessage_NormalUser(void);
	void			OpenInvitemessage_Friend(void);
	void			OpenInviteMessage_Clan(void);
	
	virtual bool	OnInviteMove( void)	{ return false; }

	//------------------------------------------------------------------------------//
	//								Callback Functions								//
	//------------------------------------------------------------------------------//
	// ģ�� �ʴ� �޽��� callback function
	static void		CbInviteMove( void * pThis, INT32 nParam);
	
	void			PopupInviteMessage( const CUserLocationInfo & UserLocInfo, const i3::rc_wstring& wstrNickName, CbINVITEPOPUP pCallBackFunc);

private:
	typedef void	(UIPhaseBase::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Change_Chara_Nick( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Error( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Disconnection( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_GameGuard_Error( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_HackToolDetected(INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Inventory( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Inventory( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Auth_Item( INT32 arg, const i3::user_data&);

	void			ReceiveGameEvent_Enter_Profile( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Profile(INT32 arg, const i3::user_data&);

	void			ReceiveGameEvent_Enter_Info( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Info( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Shop( INT32 arg, const i3::user_data& UserData);
	void			ReceiveGameEvent_Leave_Shop( INT32 arg, const i3::user_data& UserData);
	void			ReceiveGameEvent_Delete_NoteAsk( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Note( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Insert_Block( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Accept_Friend( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Delete_Friend( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Invite_Clan( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Gacha( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Gacha( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Clan( INT32 arg, const i3::user_data & userData);
	void			ReceiveGameEvent_Leave_Clan( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_View_Other_User_Item_Info( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_ShutDown_Time( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_GiftShop( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_GiftShop( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Fail_Enter_GiftShop(INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_GM_User_Exit( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Temp( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Temp( INT32 arg, const i3::user_data&);

	//Ŭ�� ä�γ��� ���� �̺�Ʈ ��� uiphase���� �۵��Ǿ�� �Ѵ�.
	void			ReceiveGameEvent_Clan_Deprotation(INT32 arg, const i3::user_data&);

protected:
	/** \brief �ڸ���� �˻� 
		\desc ���� �ð����� �Է��� ������ �����Ų��. */
	void			ReceiveGameEvent_DriveAway( INT32 arg, const i3::user_data&);
	
	
		

public:
	// GameEvent�� ó���Ѵ�.
	//GetMainFrame()->GameEventTransferToSubPhase(event,arg); �� ȣ�� ���� ����..
	//�� �Ʒ����� ȣ�� �Ѵ�.
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData);

};

#endif
