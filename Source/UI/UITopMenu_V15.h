/**
 * \file	UI\UITopMenu_V15.h
 *
 * Declares the user interface top menu class.
 */

#if !defined( __UI_STATIC_TOP_V15_H__)
#define __UI_STATIC_TOP_V15_H__

#include "UITopMenu.h"
#include "UIFlowEditBox.h"

enum	{ None = -1, Shop, Inven, Gacha, Clan, Myinfo, MailBox, Setting, Back, Exit, GiftShop, MaxCount };

class UITopMenu_V15 : public UITopMenu
{
	I3_CLASS_DEFINE( UITopMenu_V15, UITopMenu);

private:
	struct PushButton
	{
		INT32 i32Idx;
		i3UIButton * pNormalBtn;

		PushButton(void)
		{
			i32Idx = -1;
			pNormalBtn = nullptr;
		}

		void SetPushed()
		{
			pNormalBtn->UnSelectedNormalState();
		}

		bool GetPushed(void)
		{
			return pNormalBtn->isPushFlag();
		}
	};

private:
	REAL32			m_rElapsedBackButtonTime;

	i3UIImageBox *	m_pBGMenu;

	i3UIFrameWnd*			m_mainBtnsFrame;
	i3::vector<PushButton>	m_PushBtnList;

	// Background scene
	i3Node *			m_pCamNode;
	i3Node *			m_pCam[ CAS_MAX];
	CAM_ANIM_STATE		m_CurCameraState;
	CAM_LOOK_CMD		m_CurCameraLook;

	i3Node *			m_pBG;
	i3ReflectMapBindAttr *	m_pRefAttr;

	i3Light *		m_pLight;

public:
	void	cb_tmenu_gauge( bool lastestTarget);
	PBGaugeBar mPBGaugeBar;


protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	// Menu Button�� select Server������ �ٲ۴�.
	void			_SetServerMode( void);

	// Menu Button�� select Channel������ �ٲ۴�.
	void			_SetChannelMode( void);

	// Menu Button�� �Ϲ� Ready��(Lobby, readyroom)���� �ٲ۴�.
	void			_SetReadyMode( void);

	/** \brief ����� �ε� */
	void			_LoadBG( void);

	void			_AddLight( void);

	bool			_IsAvailablePush(INT32 value);
	void			_SetPushedButton(INT32 value);

	//------------------------------------------------------------------------------//
	//									inline functions							//
	//------------------------------------------------------------------------------//
public:
	UIMainFrame *	GetMainFrame( void)				{ return ((UIFramework*) m_pParent)->GetUIMainframe(); }

	i3Node *		GetCameraNode( void)			{ return m_pCam[ m_CurCameraState]; }

	i3UIFrameWnd*   GetMainBtnsFrame(void)			{ return m_mainBtnsFrame; }
	i3Texture*		GetUIReflectMap()				{ return m_pRefAttr ? m_pRefAttr->GetTexture() : nullptr; }

public:
	UITopMenu_V15();
	virtual ~UITopMenu_V15();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnClick( i3UIControl * pControl) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void 	OnLoadAllScenes() override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
	virtual bool	OnPlayClickSound(i3UIControl * pCtrl) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	virtual void	AllMenuBtnShapeToNormal() override;

	virtual void	SyncSubPhaseWithMenuBtnState() override;

	void SetOriginalCamera();
	void			Resize( void);

	void Clicked(INT32 value);
	void ClickPopupBackButton(bool close_popup = false);	//TopMenu���� control �ϴ� menu �� �� �Լ��� close ����.

	bool ClickBackButton( void);
	void ClickExit( void);

	void CloseEnterPopup( void);

	//chunjong.song 2014-07-15
	// UIPopupbase���� UIPopup_Close�Լ���� TogglePopup/ClosePopup�� �̿��ؼ� �˾��� ������
	// UITopMenu_V15�� ���� ��� �˾���(Mailbox, option)�� ������ �߰��۾�ó���� �����ʾ� ������ �����Ѵ�(������ �˾�����ġ ����)
	// UITopMenu_V15�� ���� �˾��鵵 TolgglePopup/ClosePopup�� �̿��ؼ� �ݾƵ� �������� �ϱ����ؼ� �Ʒ��Լ��߰�
	void			uglyOnMainFrameClosePopupAfter(INT32 popupType);

	/** \brief �������� �����̰� �Ѵ�. */
	void			StartHighlightMailbox( void);

	////////////////////////////////////////////////////////////////////////
	//						Camera

	void			SetUICharaCamera( CAM_LOOK_CMD state);
	void			UpdateCamera( void);

	virtual bool		ClickInventory( void) override;
protected:
	bool IsServerBasePopup();
	//------------------------------------------------------------------------------//
	//									Callback rutine								//
	//------------------------------------------------------------------------------//
	static void		CbExitNormal(void* pThis,INT32 nParam);
	static void		CbRemoveMercenary(void* pThis,INT32 nParam);
	static void		CbReRegistMercenary(void* pThis, INT32 nParam);


private:

	typedef void	(UITopMenu_V15::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_GET_USER_INFO( INT32, const i3::user_data&);
	void			ReceiveGameEvent_GET_INVEN_INFO( INT32, const i3::user_data&);
	void			ReceiveGameEvent_GET_GIFT_LIST( INT32, const i3::user_data& );
};

#endif
