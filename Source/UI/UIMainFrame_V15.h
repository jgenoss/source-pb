#if !defined( __UI_MAINFRAME_V15_H__)
#define __UI_MAINFRAME_V15_H__
#include "UIMainFrame.h"

class UIMainFrame_V15 : public UIMainFrame
{
	I3_CLASS_DEFINE( UIMainFrame_V15, UIMainFrame);


	UIMainFrame_V15();
	virtual ~UIMainFrame_V15();

protected:
	virtual void			OnLoadEnd(i3Stage* pStage) override;
	virtual void			OnFinish( void) override;
	virtual void			StartPhase( UIPHASE MovePhase) override;
	virtual void			EndPhase( void) override;
	
public:

	/** \brief Chatting window�� ������Ʈ�Ѵ�. */
	virtual void	UpdateChatBuffer( void) override;
	/** \brief Chatting window�� MegaPhone �� ������Ʈ�Ѵ�. */
	virtual void	UpdateMegaPhoneBuffer(void) override;

	/* (1.0, 1.1), 1.5 ���� �ൿ�� �ٸ��Ƿ� virtual�� ���ڽ��ϴ� */
	virtual void	PreStartSubPhase( UISUBPHASE phase) override;
	virtual void	EndSubPhase( UISUBPHASE phase) override;
	virtual void	EntranceSubPhase( void) override;
	virtual void	ExitSubPhase( void) override;
	virtual void	UpdateLocation( void);
};



#endif
