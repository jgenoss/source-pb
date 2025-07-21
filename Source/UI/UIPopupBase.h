/**
 * \file	UI\UIPopupBase.h
 *
 * Declares the user interface popup base class.
 */


#if !defined( __UI_POPUP_BASE_H__)
#define __UI_POPUP_BASE_H__

#include "UIBase.h"

class UIPopupBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPopupBase, UIBase);

protected:
	i3UIScene *		m_pScene;
	i3UIFrameWnd *	m_pPopupFrame;
	i3UIFrameWnd *	m_pBGFrame;

	VEC2D			m_vOriginalSize;
	REAL32			m_SlideScale;
	bool			m_bScaleAnimation;
	i3::vector<i3UIControl*> m_ChildList;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;
	void			_ApplyBGSize();
	void			_ReSizeBG();

	/** \brief �ڽ��� �ݴ´�. */
	virtual void	_OnClose( void);

public:
	UIPopupBase();
	virtual ~UIPopupBase();
	
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override {}

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual bool	OnKeyInput(i3InputDeviceManager * pInputMng) override;
	
	virtual bool	OnKey_Enter( void) override;

	virtual bool	OnKey_Escape( void) override;
	
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnExitStart( void) override;

	UIMainFrame *	GetMainFrame( void);

	/** \brief OK ��ư�� ���� ó��
		\desc Lua���� ��ư �Է¿� ���� ��� true ��ȯ�� �ڵ����� �ݽ��ϴ�. */
	virtual bool	OnOKButton( void) { return true;}

	/** \brief Close ��ư�� ���� ó��
		\desc Lua���� ��ư �Է��� ������ ȣ��Ǹ� true�� ��ȯ�ϸ� â�� �ݽ��ϴ�. */
	virtual bool	OnCloseButton( void) { return true;}

	//--------------------------------------------------------------------------//
	//							Old GUI�� ����ϴ� Function�Դϴ�.				//
	virtual bool	OnLinkControl( const i3::vector_set<struct STRCINFO*>* pList, i3GameNode * pControl) { return true;}
	virtual void	OnInitControl( void) {}
	virtual void	OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify) override {}


	//delay pop�� ����� ��쿡 ������ ����.
	virtual void	Enter( void) {}
	virtual void	OnEnter( void) {}
	virtual void	Leave( void) {}
	virtual void	OnLeave( void) {}

	virtual bool	OnRevive( i3RenderContext * pCtx) override;
};

#endif
