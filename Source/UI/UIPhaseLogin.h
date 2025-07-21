/**
 * \file	UI\UIPhaseLogin.h
 *
 * Declares the user interface phase login class.
 */


#if !defined( __UI_PHASE_LOGIN_H__)
#define __UI_PHASE_LOGIN_H__

#include "UIPhaseBase.h"

class UIPhaseLogin : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseLogin, UIPhaseBase);
protected:
	REAL32			m_rConnectWaitTimeStamp;
	bool			m_bConnectStart;

	UIMessageBox*	m_pmbLoginWait;
	bool			m_LoginError;

	INT32			m_LogInConnectFailedCount;

	REAL32			m_GameStartTime;	// ���� ���� �� �α���ȭ���� �������� �ð�.
	bool			m_ForceExit;		// �����ð� �̻� �α������� �ʾƼ� ������ ���������ϴ���.
	bool			m_RNFlag;

	i3UIEditBox *	m_pIDBox;
	i3UIEditBox *	m_pPwBox;

	i3UIImageBox *	m_pImage_IDbox;
	i3UIImageBox *	m_pImage_PWbox;

	i3UIButton* m_login_btn;
	i3UIButton* m_join_btn;
	bool open_exit_popup;

	INT32				m_iWaitCount;
	REAL32				m_rWaitTime;

	UIMessageBox *		m_pWaitText;

	void			__CheckTimeOut( void);
	void			__WaitConnectMessageBox( REAL32 rDeltaSeconds);
	void			__ResetWaitConnect( void);

private:
	void enable_sound();

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	static bool		m_bConnecting;

	static REAL32	m_rDelayWait;
	void			UpdateWaitTime(REAL32 rdeltaSeconds);
	static void		CbWaitOK(void* pThis,INT32 nParam);
	void			WaitMsgbox(INT32 count, REAL32 time);


	i3UIEditBox *	GetPwBox( void)			{ return m_pPwBox; }

private:
	void			_Init();
	bool			_GetEnvSessKey( UINT8 * pSessionKey);
	bool			_GetEnvMacAddr( INT64* pMacAddr);
	bool			_GetEnvServerAddr( char* strServerAddr, const size_t sizeServerAddrStrMax);

public:
	UIPhaseLogin();
	virtual ~UIPhaseLogin();

	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	//------------------------------------------------------------------------------//
	//								Game Event Functions							//
	//------------------------------------------------------------------------------//

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;

	virtual bool	OnKey_Enter( void) override;
	virtual bool	OnKey_Escape() override;

	virtual void	OnCompleteLoad( void) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnPlayClickSound(i3UIControl * pCtrl) override;

	void			TryConnectLoginServer( void);
	
	void			OnClickLoginBtn();
	
	void			OnClickLoginChina();


	// Login ����
	void			OnLoginOK( void);

	// Login ����
	void			OnLoginFailed( void);

	// ���� ���� ��ư
	virtual void	ClickExit(void) override;

	// ID ����
	static bool		SaveID( const char * pszPath, const char * pID, bool bChkSave);

	// ID �ҷ����� 
	void			LoadID( const char * pszPath);
	void			FocusToControl( void);

	// PB �α��� ��� Off��Ű��
	void			SetPBLoginModule(bool);

	// ID ã�� 
	void			ClickFindID( void);

	// Password ã��
	void			ClickFindPw( void);
	//------------------------------------------------------------------------------//
	//								Error Handler									//
	//------------------------------------------------------------------------------//
protected:
	ErrMessageHandler		GetErrorHandler();

	void					EhNonID(void);
	void					EhNonPW(void);
	void					EhInvalidAccount(void);
	void					EhInvalidID(void);
	void					EhInvalidPassword(void);
	void					EhInvalidName(void);

	// �Ϲ� Error Handler �߰� (2011.06.10 ���õ)
	void					EhInvalidAccount(const i3::rc_wstring& wstrMsg);
	void					EhInvalidCountry(void);
	
	void					EhLogouting(void);
	void					EhAlreadyLogin(void);
	void					EhTimeOut1(void);
	void					EhTimeOut2(void);
	void					EhBlockAccount(INT32 Arg = 0);
	void					EhUnknownLogin(INT32 Event, INT32 Arg);
	void					EhInventoryFail(void);
	void					EhWaitDisconnect(void);
	void					EhFailConnect(void);
	void					EhStageError(INT32 Event,INT32 Arg);
	void					EhBlockGame(INT32 Arg);
	void					EhBlockInner(INT32 Arg);
	void					EhBlockOuter(INT32 Arg);

	void					EhDBBufferFull(void);
	void					EhDeletedAccount(void);
	void					EhEmailAuthError(void);
	void					EhBlockIP(void);

public:
	static void				CbFailSaveID(void* pThis,INT32 nParam);
	static void				CbAlreadyLogin(void* pThis,INT32 nParam);
	static void				CbPopupOK(void* pThis,INT32 nParam);
	static void				CbNation(void* pThis, INT32 nParam);
	


private:
	typedef void		(UIPhaseLogin::*call_func)(INT32,const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void				RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	/** \brief Game Event�� ó��*/
	void				ProcessGameEvent( REAL32 rDeltaSeconds);

	void				ReceiveGameEvent_RealName( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_Connect_Server_Gateway( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_Login( INT32 arg, const i3::user_data& UserData);
	void				ReceiveGameEvent_Logout( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_Block_Account( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_ShutDown_Time_Login( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_Drive_Away( INT32 arg, const i3::user_data& UserData);
	void				ReceiveGameEvent_Error( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_GameGaurd_Error( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_Disconnection( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_Destroyed_Network( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_Login_Wait( INT32 arg, const i3::user_data& UserData);

	virtual void		ReceiveGameEvent_Unknown( GAME_EVENT evt, INT32 arg) override;

};

#endif
