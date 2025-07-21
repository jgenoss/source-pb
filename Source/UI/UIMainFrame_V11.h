#if !defined( __UI_MAINFRAME_V11_H__)
#define __UI_MAINFRAME_V11_H__
#include "UIMainFrame.h"

class UIMainFrame_V11 : public UIMainFrame
{
	I3_CLASS_DEFINE( UIMainFrame_V11, UIMainFrame);


	UIMainFrame_V11();
	virtual ~UIMainFrame_V11();

protected:
	virtual void	OnLoadEnd(i3Stage* pStage) override;
	virtual void	OnFinish( void) override;
	virtual void	StartPhase( UIPHASE MovePhase) override;
	virtual void	EndPhase( void) override;

	virtual void	_EnableFloatingList() override;

public:
	static  void	InitItemImageMgr(void);

	//--------------------------------------------------------------------------//
	//								User Action									//
	//--------------------------------------------------------------------------//
	void			ResetUserActionPopup( void);
	void			AddUserActionCommand( USER_CONTEXT_MENU cmd);
	void			OpenUserActionPopup( void * pEventReceiver, INT32 slotIdx);

	/** \brief Chatting window�� ������Ʈ�Ѵ�. */
	virtual void	UpdateChatBuffer( void) override;
	/** \brief Chatting window�� MegaPhone �� ������Ʈ�Ѵ�. */
	virtual void	UpdateMegaPhoneBuffer(void) override;

	/* (1.0, 1.1), 1.5 ���� �ൿ�� �ٸ��Ƿ� virtual�� ���ڽ��ϴ� */
	virtual void	PreStartSubPhase( UISUBPHASE phase) override;
	virtual void	EndSubPhase( UISUBPHASE phase) override;
	virtual void	EntranceSubPhase( void) override;
	virtual void	ExitSubPhase( void) override;
};



#endif
