/**
 * \file	UI\UIBase.h
 *
 * Declares the user interface base class.
 */


#if !defined( __UI_BASE_H__)
#define __UI_BASE_H__

#include <unordered_map>

#include "UIFramework.h"
#include "UISlide.h"
#include "i3Base/itl/range/generic_empty.h"

typedef void	(*ErrMessageHandler)(void* pThis,INT32 nParam);

typedef UINT32 UI_STYLE;
#define UI_STYLE_NO_CLOSEBUTTON		0x00000001		// Close Button�� ���� Popup. Popup �̿��� ������ Click�ϸ� �������� �Ѵ�.
#define UI_STYLE_REOPENTYPE			0x00000002		// ���� �־ �ٽ� ���� �ִ� �˾� ����
#define UI_STYLE_INCLUDE_OKBUTTON	0x00000004		// OK Button�� ���� �ִ�. (�⺻������ Enter Key�� ������ ���۵�)

#define UI_SLIDEIN_SLOP	2.f
#define UI_SLIDEOUT_SLOP 3.f


enum UISOUND_TYPE
{
	UISND_SCROLLIN = 0,
	UISND_SCROLLOUT,
	UISND_POPUP_OPEN,
	UISND_POPUP_CLOSE,
	UISND_ITEM_SELECTING,
	UISND_ITEM_CLICK_OK,
	UISND_ITEM_CLICK_CANCEL,
	UISND_ITEM_EQUIP,

	// �߰��� Sound
	UISND_ERROR,
	UISND_CLICK,
	UISND_ENTER_MOUSE1,
	UISND_ENTER_MOUSE2,
	UISND_PAGE_SHIFT,
	UISND_POPUP_FADE_IN,
	UISND_SERVER_CONNECT,
	UISND_SERVER_ENTER,
	UISND_WARNING,
	UISND_LIST_OVER,
	UISND_MOUSE_OVER,

	UISND_USER_GUIDE_START,
	UISND_USER_GUIDE_COIN,
	UISND_USER_GUIDE_START4,

	UISND_MAX
};

enum	SLIDESTATE
{
	SS_IN_OK = 0,	// ���� �Ϸ�
	SS_OUT_OK,		// ���� �Ϸ�
	SS_IN_START,	// ���� ����
	SS_OUT_START,	// ���� ����
	SS_SLIDING_IN,	// ���� ��
	SS_SLIDING_OUT,	// ���� ��

	SS_MAX
};

struct UISCENEINFO
{
	char			_szPath[ MAX_PATH];
	i3UIScene *		_pScene;
	LuaFuncReg *	_GlueFunc;
	bool			_EnableScn;			//�߰�..
	bool			_LoadedStyle;		// Memory�� Loaded����
	
	UISCENEINFO();
	~UISCENEINFO();
	void Reset( void);
};


namespace UILua
{
	LuaState *		CallLuaFunction( i3UIScene* pScene, const char* pszFunction);
	void			EndLuaFunction( LuaState* L, INT32 nArg );
	bool			ReturnLuaBoolean( LuaState* L, INT32 nArg);
	REAL32			ReturnLuaNumber( LuaState * L, INT32 nArg);
	const char*		ReturnLuaString( LuaState* L, INT32 nArg);
	void *			ReturnLuaPointer( LuaState* L, INT32 nArg);
	INT32			ReturnLuaInteger( LuaState* L, INT32 nArg);
	void			RegisterLuaFunctions( i3UIScene* pScene, LuaFuncReg * GlueFunc);
	void			RegisterLuaFunctions(LuaState* L, LuaFuncReg * GlueFunc);
}

typedef void	(*GameEventFunc)(INT32, const i3::user_data&);

class UIBase : public i3GameNode
{
	I3_ABSTRACT_CLASS_DEFINE( UIBase, i3GameNode);

	friend class UIPhaseLogin;
	friend class UIPhaseLogin_V11;

	friend class UISubBase;
	friend class UIMainFrame;
	friend class UIFramework;

private:
	i3::vector<UISCENEINFO*> m_UISceneList;

	REAL32		m_rAwayInputTime;
	REAL32		m_rCautionTime;
	bool		m_bOffAwayInput;
	bool		m_bOffCaution;	

	// Sound
	i3SoundPlayInfo * m_pGuiSound[UISND_MAX];

	i3UIControl *	m_pLastMouseOverCtrl;
	bool m_pre_loaded;

protected:
	bool		m_bInputDisable;
	UIMessageBox*	m_pExitMessageBox;

private:
	void		__FindToolTipTemplate( void);

	/** \brief SceneFile Path�� ����մϴ�.
		\desc __LoadScene()�� ȣ���Ͽ� ��ϵ� Path�� Scene �ε��ϰ� �˴ϴ�. */
	void		__RegisterScene( const char * pszScenePath, const LuaFuncReg * GlueFunc = nullptr, bool bSceneEnable = true);

	/** \brief Register�� Scene�� Load�մϴ�. */
	void		__LoadScene( void);
	/** \brief pInfo�� ��ϵ� Path�� UIScene�� Load�մϴ� */
	void		__DirectlyLoadScene( UISCENEINFO * pInfo);

	void		__BindScene( i3UIScene * pScene, LuaFuncReg * GlueFunc, bool bEnale);
	void		__UnbindScene( i3UIScene * pScene);

	/** \brief Loading�� scene�� �޸𸮿��� �����մϴ�. */
	void		__UnloadScene( void);

	/** \brief �ش� Scene�� �����մϴ�. */
	void		__UnloadScene( i3UIScene * pScene);

	/** \brief ��� Scene�� �����ϰ� Register������ �����մϴ�. */
	void		__ClearScene( void);

protected:
	UI_STYLE	m_UIStyle;

	/** \brief CloseButton�� ���� ��� Ȯ��
		\desc Close Button�� ���� Popup. Popup �̿��� ������ Click�ϸ� �������� �Ѵ�. */
	bool		isNoCloseButton( void)			{ return (m_UIStyle & UI_STYLE_NO_CLOSEBUTTON) == UI_STYLE_NO_CLOSEBUTTON; }

	/** \brief ���� �־ �ٽ� ���� �ִ� �˾�(Floating) ���� */
	bool		isReopenStyle( void)			{ return (m_UIStyle & UI_STYLE_REOPENTYPE) == UI_STYLE_REOPENTYPE; }

	/** \brief OKButton�� ���� �ִ� ����
		\desc Enter Key�� ������ �����Ѵ�. */
	bool		isIncludeOKButton( void)		{ return (m_UIStyle & UI_STYLE_INCLUDE_OKBUTTON) == UI_STYLE_INCLUDE_OKBUTTON; }


	//--------------------------------------------------------------------------------------//
	//										Slide system									//
	//--------------------------------------------------------------------------------------//
	i3UIFrameWnd *			m_pFrameWnd;
	i3UIImageBoxEx *		m_pBG;

	SLIDESTATE	m_SlideState;

	/** \BG ������ �����մϴ�.  */
	virtual void			_Resize();

	void		_OnSlideInOnce();
	void		_OnSlidingIn(REAL32 rDeltaTime);
	void		_OnSlideInEnd();

	void		_OnSlideOutOnce();
	void		_OnSlidingOut(REAL32 rDeltaTime);
	void		_OnSlideOutEnd();
	
	virtual void OnSlideInOnce() {}
	virtual void OnSlideOutOnce() {}

	UISlide  m_slide;
	TUISlide_Callback< UIBase,	&UIBase::_OnSlideInOnce, &UIBase::_OnSlidingIn, &UIBase::_OnSlideInEnd, 
						&UIBase::_OnSlideOutOnce, &UIBase::_OnSlidingOut, &UIBase::_OnSlideOutEnd > m_slideCB;

	//--------------------------------------------------------------------------------------//
	//									LuaContext											//
	//--------------------------------------------------------------------------------------//
	i3::rc_string m_strLuaContextName;

	/** \brief Lua function�� ȣ���Ѵ�.
		\param[in] pszScene UI Scene Name
		\param[in] pszFunction Lua Function Name
		\return LuaState */
	LuaState *	_CallLuaFunction( const char * pszScene, const char * pszFunction);
	LuaState *	_CallLuaFunction( INT32 idxScene, const char* pszFunction);
	LuaState *	_CallLuaFunction( const char * pszFunction);

	/** \brief Lua function ȣ���� ������. */
	void		_EndLuaFunction( LuaState* L, INT32 nArg );

	/** \brief Lua Function ȣ��
		\desc call �Ŀ� return���� �޴´�.
		\return bool		*/
	bool		_ReturnLuaBoolean( LuaState* L, INT32 nArg);

	REAL32		_ReturnLuaNumber( LuaState * L, INT32 nArg);

	void		_ReturnLuaVec2D(VEC2D * pOut,  LuaState * L, INT32 nArg);

	const char *_ReturnLuaString( LuaState* L, INT32 nArg);

	void *		_ReturnLuaPointer( LuaState* L, INT32 nArg);
	
	INT32		_ReturnLuaInteger( LuaState* L, INT32 nArg);

	/** \brief Lua���� Global variable�� �����ɴϴ�.
		\param[in] pszScene Scene Name
		\param[in] pGlobalName Lua script�� ������
		\param[in] stackPos Stack�� ���� ��ġ��.
			������ �ݴ�� �������⶧���� ���� ����� ������ �� ���� ���� �����ϴ�.
			��) g_Width, �� g_Height �� �� ������ �����ϱ� ���ؼ��� g_Width�� -2�� g_Height�� -1�� ����Position�� �־�� �˴ϴ�.
			*/
	INT32		_GetLuaNumber( const char * pszScene, const char * pszGlobalName, INT32 StackPos);

	INT32		_GetLuaNumber( const char * pszGlobalName, INT32 StackPos = -1);

	/** \brief LuaGlue Function�� ����մϴ�. */
	void		_RegisterLuaFunctions( LuaState* L, LuaFuncReg * GlueFunc = nullptr);

	/** \brief �ش� Path�� SceneFile�� Load�մϴ� */
	virtual i3UIScene *	_LoadScene( const char * pszScenePath, LuaFuncReg * GlueFunc = nullptr, bool bSceneEnable = true);

	/** \brief SceneFile�� Load�ϰ� LuaContext�� �����Ǹ� ȣ��˴ϴ�.
		\desc Lua Function�� ȣ���Ͽ� �ʱ�ȭ�� ���� ���⿡ define�մϴ�. */
	virtual void	_InitializeAtLoad( i3UIScene * pScene);

	/** \brief SceneFile�� Unload�ϰ� �ʱ�ȭ�� �մϴ�.
		\desc Scene�� ������ ��� �������ֽʽÿ�. */
	virtual void	_ClearAtUnload( i3UIScene * pScene);

	/** \brief ��� Scene�� Load�ϰ� ȣ��˴ϴ�. */
	virtual void	OnLoadAllScenes() {}

	/** \brief ��� Scene�� Unload�ϱ� ���� ȣ��˴ϴ�. */
	virtual void	OnUnloadAllScenes() {}

	i3GameNode *	_FindListBoxFromCombo( i3GameNode * pNode);

	bool			_IsHost( void);

	void			_DefaultErrorPopup( const i3::wliteral_range& wMsgRng, INT32 event, INT32 arg);
	
	void			_AttachToTopScreen( void);


	//------------------------------------------------------------------------------//
	//								Inline Functions								//
	//------------------------------------------------------------------------------//
public:
	UISlide*		GetSlide(void) { return &m_slide; }
	SLIDESTATE		GetSlideState( void) const { return m_SlideState; }
	i3UIFrameWnd*	GetFirstFrameWnd() const { return m_pFrameWnd; }
	bool			IsReadyToUpdate( void)	{ if( m_pFrameWnd == nullptr || m_pFrameWnd->IsLoaded() == false) return false; return true;}

	const VEC3D*	GetMainFrameWndPos()	{ if (m_pFrameWnd == nullptr || m_pFrameWnd->IsLoaded() == false) return nullptr; return m_pFrameWnd->getPos(); };
	const VEC2D*	GetMainFrameWndSize()	{ if (m_pFrameWnd == nullptr || m_pFrameWnd->IsLoaded() == false) return nullptr; return m_pFrameWnd->getSize(); };

public:
	UIBase();
	virtual ~UIBase();

	/** \brief i3UIE ������ ����մϴ�.
		\param[in] pszScenePath Path
		\param[in] GlueFunc Lua Glue Function�� ����ϴ� Struture pointer		 */
	i3UIScene *		AddScene( const char * pszScenePath, const LuaFuncReg * GlueFunc = nullptr, bool bLoaded = false, bool bSceneEnable = true);
	
	/** \brief �⺻ ���� �ڵ��Դϴ�.
		\desc Scene file�� ����մϴ�. */
	virtual void	OnCreate( i3GameNode * pParent);

	/** \brief Scene�� �����ϰ� �ڽ��� �޸𸮿��� �����մϴ�. */
	void			Destroy( void);

	/** \brief Update
		\see i3GameNode::OnUpdate */
	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) {}

	virtual void	ReceiveGameEvent_Unknown( GAME_EVENT evt, INT32 arg) {}

#if defined(I3_DEBUG)
#pragma warning(push)
#pragma warning(disable : 4481)
	virtual void	ProcessGameEvent(GAME_EVENT evt, INT32 arg) sealed {}		// �Ǽ������� ���� �߰�..(2013.07.17.����)
#pragma warning(pop)
#endif
	/** \brief i3Engine Event�Դϴ�.
		\see i3EventReceiver::OnEvent */
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);

	/** \brief Key �Է� ó�� */
	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng);

	/** \brief Key�Է� Enter */
	virtual bool	OnKey_Enter()		{ return false; }

	/** \brief Key�Է� Escape */
	virtual bool	OnKey_Escape()		{ return false; }

	/** \brief Spirte Loading�� �Ϸ�Ǹ� ȣ��˴ϴ�. */
	virtual void	OnCompleteLoad( void) {}

	/** \brief Control�� Mouse Over�Ǹ� ȣ��˴ϴ�. */
	virtual bool	OnEnterMouse( i3UIControl * pControl);

	/** \brief Control�� Mouse leave�Ǹ� ȣ��˴ϴ�. */
	virtual void	OnLeaveMouse( i3UIControl * pControl);

	/** \brief UI Event Click*/
	virtual bool	OnClick( i3UIControl * pControl);

	/** \brief UI Event Rbutton Click */
	virtual void	OnRClick( i3UIControl * pControl);

	virtual bool	OnDblClick( i3UIControl * pControl);

	/** \brief UI Event Mouse Wheel */
	virtual void	OnWheel( i3UIControl * pControl, INT32 zDelta) {}

	virtual void	OnDragStart( i3UIControl * pControl, INT32 x, INT32 y) {}
	virtual void	OnDragging( i3UIControl * pControl, INT32 x, INT32 y)	{}
	virtual void	OnDragEnd( i3UIControl * pControl, INT32 x, INT32 y) {}

	/** \brief Enable�� �����մϴ�.
		\desc i3UIScene�� Enable�� ���� �����մϴ�.
		\note Scene�� Enable�� Load�� �մϴ�.
			���� Enable�� ȣ��� ��� ���� ���ϰ� �Ͼ �� �����Ƿ� EnableCtrl�� �̿��� ���(�Ǵ� SetVisible)�� ����Ͻʽÿ�. */
	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f);

	/** \brief Visible�� �����մϴ�.*/
	virtual void	SetVisible( bool bTrue = true, REAL32 tm = 0.f);
	
	i3UIScene *		GetScene( const char * pszSceneName) const;
	i3UIScene *		GetScene( INT32 idxScene) const;
	INT32			GetSceneCount() const { return (INT32)m_UISceneList.size(); }
	bool			IsEnableFromSceneInfo(INT32 idxScene) const;
	void			SetEnableToSceneInfo(INT32 idxScene, bool bEnable);

	virtual bool	OnLostDevice( bool bLostDevice);
	virtual bool	OnRevive( i3RenderContext * pCtx);


	/** \brief Disable Input
		\desc Scene�� i3UIControl���� Input�� �����ϴ�.
			Scene�� ������� �ʰ� ���� �����ϴ� Control�̳� UIBase�� �ִ� ��� ���� Class�� �����Ͻʽÿ�. */
	virtual void	SetInputDisable( bool bDisable);
	bool			IsInputDisable() const { return m_bInputDisable || i3UI::getDisableUIInput(); }

	/** \brief ȿ������ ����Ѵ�. */
	bool			PlaySound( UISOUND_TYPE type);

	// Phase / State�� ��� ���� ����.
	virtual bool	OnPlayClickSound(i3UIControl * pCtrl);
	virtual bool	OnPlayEnterMouseSound(i3UIControl * pCtrl);

	/** \brief LuaState�� ���� �ִ��� �˻��մϴ�.
		\note false��� �ٸ� Scene�� LuaState �Դϴ�. */
	bool			IsLuaContext( LuaState * L);

	/** \brief Scene�� �ش� Control�� �ִ��� Ȯ���Ѵ�. */
	bool			IsIncludeControl( i3UIControl * pControl);

	bool			_isEmptyLuaContext() const	{ return m_strLuaContextName.empty(); }

	//------------------------------------------------------------------------------//
	//								Game Event Functions							//
	//------------------------------------------------------------------------------//
	/** \brief �ʱ�ȭ �Լ�	*/
	virtual void	OnReset( void) {}

	/** \brief Sliding ����� �ݴϴ�.
		\return N/A */
	virtual void	OnSlideIn( void);

	/** \brief Sliding ����� �ݴϴ�.
		\return N/A */
	virtual void	OnSlideOut( void);

	bool			isSliding( void)				{ return (m_SlideState == SS_SLIDING_IN) || ( m_SlideState == SS_IN_START) ||	// ���� ����
															( m_SlideState == SS_OUT_START) || (m_SlideState == SS_SLIDING_OUT); }

	/** \brief ���� ����
		\desc �⺻������ Slide in�� �����մϴ�.
		\return false�̸� â�� ����� �ʴ´�.*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr);

	/** \brief ������ ó�� */
	virtual bool	OnEntrancing( REAL32 rDeltaSeconds);

	/** \brief ���� �Ϸ� ó�� */
	virtual void	OnEntranceEnd( void);

	/** \brief ���� ����
		\desc �⺻������ Slide Out�� �Ѵ�. */
	virtual bool	OnExitStart( void) { OnSlideOut(); return true;}

	/** \brief ������ */
	virtual bool	OnExiting( REAL32 rDeltaSeconds);

	/** \brief ���� �Ϸ� */
	virtual void	OnExitEnd( void);

	/** \brief ������ ��ư�� ������ ����
		\desc �Ϲ������� �����ϱ� Popup�� ����. */
	virtual void	OnEscapeKey( void);

	/** \brief Ű���� Enter �Է� ���� */
	bool			isInputKeyboardEnter( void);

	//------------------------------------------------------------------------------//
	//								Callback Functions								//
	//------------------------------------------------------------------------------//
	static void		CbExitNormal( void* pThis, INT32 nParam);
					
	LuaState *		__CallLuaFunction_Internal( i3UIScene* pScene, const char* pszFunction);


	//------------------------------------------------------------------------------//
	//								Old Gui Function								//
	//------------------------------------------------------------------------------//
	virtual void	OnPreGuiNotify(void)	{}
	virtual void	OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify){}
	virtual void	OnPostGuiNotify(void)	{}


	//------------------------------------------------------------------------------//
	//								ETC Function									//
	//------------------------------------------------------------------------------//

};

struct MultiSceneInfo
{
	MultiSceneInfo() : idx(-1), scn(0) {}

	INT32 idx;
	i3UIScene* scn;
};

#endif
