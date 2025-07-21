/**
 * \file	UI\UIFloatChat.h
 *
 * Declares the user interface float chat class.
 */


#if !defined( __UI_FLOAT_CHAT_V11_H__)
#define __UI_FLOAT_CHAT_V11_H__

#include "../UI/UIFloatingBase.h"
#include "../UI/UIFloatChat.h"

#include "../UserLocationInfo.h"
#include "../CommunityContext.h"



class UIFloatUserList_V11;
class UIResizer;
enum UI_SIZESTATE;

struct ALLCONTROLINFO
{
	i3::rc_string	strName;
	VEC3D			pos;
	VEC2D			size;
};

class UIFloatChat_V11 : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatChat_V11, UIFloatingBase);

private:
	UIResizer *		m_pFrameResizer;
	i3::vector<ALLCONTROLINFO>	m_pLargeCtl;
	i3::vector<ALLCONTROLINFO>	m_pSmallCtl;
	i3UIScene *					m_pMainScene;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIFloatChat_V11();
	virtual ~UIFloatChat_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;

	virtual	bool	OnKey_Enter( void) override;

	virtual void	OnCompleteLoad( void) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd() override;

	virtual bool	OnExitStart( void) override;

	virtual bool	OnEnterMouse( i3UIControl * pControl) override;

	virtual void	OnMovePhase( UIPHASE phase) override;
	virtual void	OnMoveSubPhase(UISUBPHASE phase) override;

	virtual bool	OnRevive(i3RenderContext * pCtx) override;

	bool			IsMinimizeDone( void);
	bool			IsMaximizeDone( void);
	UI_SIZESTATE	GetSizeState(void) const;
	/** \brief �ּ�ȭ ���� */
	void			OnMinimizeStart( void);
	/** \brief �ִ�ȭ ���� */
	void			OnMaximizeStart( void);
	/** \brief �ִ�ȭ �Ϸ� */
	void			OnMaximizeEnd( void);
	
	void			SetFocus_ChatEditBox();	// ä��â�� ��Ŀ���� �Ѵ� �ܺο��� �Ѵ� �Լ�..

	//--------------------------------------------------------------------------//
	//								Chatting									//
	//--------------------------------------------------------------------------//
	
	void			ClickMegaPhoneOpenButton();
private:
	i3UIFrameWnd *	m_pChatMainBox;
	i3UIFrameWnd *	m_pSubChatMain;
	
	i3UIEditBox *	m_pTextBox;
	i3UIControl *	m_pChatList;
	i3UIEditBox *	m_pChatBox;

	i3UIComboBox *	m_pChatCombo;

	i3UIButton *	m_pExpandBtn;
	i3UIButton *	m_pCollapseBtn;

	i3UIScrollBar * m_pChatScrollBar;

	INT32 			m_nChatType;
	INT32 			m_nOldChatType;
	INT32 			m_nMaxChatLineCount;
	CHAT_AREA		m_ChatArea;
	bool			m_bOnceChatType;
	bool			m_bOnceChatTypeAll;
	//UINT16			m_nCurrentChatBufferType;
	INT32			m_nCurrentSelectedChatType;

	REAL32			m_OriginalHeight;

	static bool		m_bUseHistory;

	i3::wstring		m_lastWhisperUserNick;

	CHAT_TYPE		m_Tab[CHAT_TYPE_MAX];
	CHAT_TYPE		m_CurrentTab;

	bool			m_bWhisperMode;	///<< �Ӹ���

	INT32			m_LanguageCount;
	INT32			m_CurrentLanguage;
	HKL				m_LanguageList[6];

	i3UIFrameWnd  *  m_pParentMegPhoneMain;
	i3UIButton	  *  m_pMegaPhone_Button;
	i3UIEditBox   *  m_pMegaPhoneMain_EditBox;
	i3UIEditBox   *  m_pMegaPhone_EditBox;

	bool			m_bMegaPhoneOpen;

protected:
	/** \brief Input ����� ������ ����Ʈ�� �����Ѵ�. */
	void			_SearchLanguage( void);

	/** \brief Input Layout���� ù��° ���� �����Ѵ�. */
	void			_ResetLanguage( void);

	// Chatting
	void 			_UpdateMacroChat(void);
	void			_SendMacroChat(INT32 nIdx);

	void 			_UpdateLang(void);
	void			_UpdateWhisperBackupBuffer(void);
	void			_UpdateChatType( void);

	void 			_UpdateParsingType( EDIT_STATE state);
	void			_SetChatColorFromType(void);						// ä�� Ÿ�Ժ� ä�� Color ����
	void			_UpdateWhisperChat(const i3::rc_wstring& wstrTarget);
	CHAT_SYS_TYPE	_UpdateChatBox(const i3::wliteral_range& wImeBufRng);			// ChatBox Update (return Type CHAT_SYS_TYPE)

	void			_SendChattingMsg(i3::wstring& inout_wstrImeBuf, CHAT_SYS_TYPE eChatType);
	bool			_SetWhisperChatToSend(i3::stack_wstring& out_only_chat, i3::stack_wstring& out_colored_chat, const i3::wliteral_range& wImeBufRng);
	void			_SendWhisperChatting(const i3::rc_wstring& wstrTarget, const i3::wliteral_range& wMsgRng, const i3::wliteral_range& wImeBufRng, bool bReply);
	void			_SendMegaPhoneChatting(const i3::wliteral_range& wMsgRng);

	void			_SetChannel( CHAT_TYPE type);
	void			_SetChattingCombo( INT32 cur);

	void			_CopyChatBufferToBox( i3::wstring& inout_wst, INT32 bufferIdx);
	void			_CopyMegaPhoneBufferToBox(i3::wstring& inout_wstr, INT32 bufferIdx);

public:
	/** \brief Chatting View Channel�� �����մϴ�. */
	void			SelectChatChannel( INT32 idx);

	/** \brief ComboBox�� ���ÿ� ���� Chat �Է� �׷��� �����մϴ�. */
	void			SetChatTypeFromCombo( INT32 idxItem);

	/** \brief Chat �Է� �׷��� �����մϴ�. */
	void			SetChatType( INT32 type);

	void			InitChat( CHAT_AREA area, INT32 maxLineCount);

	void			OnWhisper( INT32 slotIdx);
	void			OnWhisper( const i3::rc_wstring& wstrNick);
	
	void			SetWhisperCommand(void);
	void			SetLastWhsiperNick(const i3::rc_wstring& wstrNick);

	/** \brief Chatting Buffer�� ������ ����Ѵ�. */
	void 			UpdateChatBuffer(void);
	void			UpdateMegaPhoneBuffer(void);

	/** \brief Language ���� */
	void			ToggleLang( void);

	void _ChatHistory(EDIT_STATE state);

	void			_ToWhisperChatMessage(i3::wstring& out, const i3::rc_wstring& target, const i3::wliteral_range& msg = L"");
	void			_GetOnlyChatMessage(i3::stack_wstring& out,	const i3::wliteral_range& wfullMsgRng);

	void			_SetMegaPhoneMainFrame(bool bEnable);
	//--------------------------------------------------------------------------//
	//								User List									//
	//--------------------------------------------------------------------------//


	void GetAllCtlInfo(i3GameNode *pScene, i3::vector<ALLCONTROLINFO>* AllInfo);
	void SetAllCtlInfo(i3::vector<ALLCONTROLINFO>* AllInfo);
	ALLCONTROLINFO* GetOrgCtlInfo(const char * pszName, i3::vector<ALLCONTROLINFO>* AllInfo);

private:
	UIFloatUserList_V11 *	m_pUserList;

public:
	UIFloatUserList_V11 *	GetUserList( void)		{ return m_pUserList; }

};

#endif
