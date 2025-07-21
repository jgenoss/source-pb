#if !defined( __UI_POPUP_SPECIAL_CHAR_INPUT_H__)
#define __UI_POPUP_SEPCIAL_CHAR_INPUT_H__

#if legacy_gui_scene
#include "UIPopupBase.h"

class CSpecialCharInput;

class UIPopupSpecialCharInput : public UIPopupBase
{

	I3_CLASS_DEFINE( UIPopupSpecialCharInput, UIPopupBase);

private:
	CSpecialCharInput * m_pSpCharInput;	// Ư�� ���� �Է±� ���������� GUI�� ����մϴ�.

public:
	UIPopupSpecialCharInput();
	virtual ~UIPopupSpecialCharInput();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnWheel( i3UIControl * pControl, INT32 zDelta) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnExitStart( void) override;

	virtual void	SetInputDisable( bool bDisable) override;

	//--------------------------------------------------------------------------//
	//							Old GUI�� ����ϴ� Function�Դϴ�.				//
	virtual bool	OnLinkControl( const i3::vector_set<STRCINFO*>* pList, i3GameNode * pControl) override;
	virtual void	OnInitControl( void) override;
	virtual void	OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify) override;

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape() override;
};

#endif

#endif
