#if !defined( __UI_PHASE_SELECT_CHANNEL_H__)
#define __UI_PHASE_SELECT_CHANNEL_H__

#include "UIPhaseBase.h"
#include "UIDefine.h"


class UIPhaseSelectChannel : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseSelectChannel);

private:
	INT32			m_LastServerNumber;
	INT32			m_LastChannelNumber;

	INT32			m_nLastedSelectSlot;

	REAL32 			m_fRefreshClickElapseTime;
	REAL32 			m_fConnectClickElapseTime;
	REAL32 			m_fWaitDelayTime;

	bool		m_bInviteMoving;
	bool			m_bWaitConnect;
	UIMessageBox*	m_pMBConnectWait;

	UIMessageBox *	m_pMessageBox;

	// ���� ��ũ
	i3Texture*		m_pVerifiTexture;
	i3Sprite2D*		m_pVerifiSprite;

	// ä�� ����	
	REAL32			m_rChannelWinX;

	INT32			m_nServerChannelIdx;
	char			m_szServerText[ SS_MAINTYPE_MAX][ SS_SUBTYPE_COUNT][256];
	INT32			m_nServerCount[ SS_MAINTYPE_MAX];

	INT32			m_LogInConnectFailedCount;
#if(0)
	CHANNEL_GAUGE	__CalcChannelGauge( INT32 UserCount, INT32 MaxUserCount, const List * pPercentLevelList = NULL);

protected:
	// Channel Label
	void			_UpdateChannelLabel();

	void			_SetServerEnv( INT32 nServer, INT32 nChannel, INT32 nChannelState);

	bool			_CheckEnterChannel( INT32 channel);

	void			_LoadServerTextFromFile( void);

	/** \brief ���Ǹ�ũ ������ �մϴ�.
		\desc ������ ���� ����� ���ϴ� ��쿡 �̰��� �����Ͻʽÿ�. */
	void			_CreateVerifiMark( void);

	/** \brief ��ư ���¿� ���� ������ �մϴ�. */
	void			_SetButtonState( void);
#endif
public:
	UIPhaseSelectChannel();
	virtual ~UIPhaseSelectChannel();
#if(0)
	virtual void	OnCreate( i3GameNode * pParent);

	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArv2 = NULL);

	virtual void	OnEntranceEnd( void);

	virtual bool	OnExitStart( void);

	virtual void	OnExitEnd(void);

	virtual void	OnGotoBackPhase( void);

	virtual bool	OnEnterMouse( i3UIControl * pControl);

	virtual bool	OnClick( i3UIControl * pControl);

	bool			OnInviteMove( void);

	void			Refresh( void);
	void			RefreshByTime( void);

	void			SelectChannel( INT32 channel);

	void			TryEnterChannel( void);

	//--------------------------------------------------------------------------//
	//							Lua functions									//
	//--------------------------------------------------------------------------//
	void			SetChannelSlot( INT32 slot, const char * pszNumber, const char * pszName,
		INT32 event, INT32 Gauge, COLOR * pColor, const char * pszToolTip);


	//--------------------------------------------------------------------------//
	//							Error handler functions							//
	//--------------------------------------------------------------------------//
protected:
	void			_EhFailConnect( void);
	void			_EhStageError( INT32 Event,INT32 Arg);

	void			PrintMessageForChannelEnterError( ChannelType channelType );

	void			_CloseWaitMessageBox( void);


	//--------------------------------------------------------------------------//
	//								Callback rutine								//
public:
	static void		CbRefleshServerList( void* pThis,INT32 nParam);
#endif
	
};

#endif
