#if !defined( __UI_POPUP_CREATEROOM_H__)
#define __UI_POPUP_CREATEROOM_H__

/*	

	==========================================================
	�游��� �˾�			-			MADE BY KDY 2016.05.25
	
	* �� UIPopupCreateRoomEx�� �̰����� ���׷��̵� �մϴ�.
	* ���� ������ ������ ���� �̹� ��ȸ�� �����Ͽ� �����մϴ�.
	==========================================================

	-�ٽ� ����-

		���� �����ʹ� ClientStageInfoContext ���� �����ϴ°� �⺻ ����.

*/

#include "UIDefine.h"
#include "UIMapSlot.h"
#include "UIPushButton.h"
#include "CreateRoomBase.h"

#include "CSI_TypeDef.h"

class UIPopupCreateRoom : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupCreateRoom, UIPopupBase);

	enum CreateRoomBase
	{
		CR_BASE_RULE = 0,
		CR_BASE_STAGE,
		CR_BASE_ROOMINFO,
		CR_BASE_MAIN_CD,
		CR_BASE_SUB_CD,
		CR_BASE_OBSERVER_CD,
		CR_BASE_WEAPON_CD,

		CR_BASE_MAX
	};

public:		//�ۺ� �Լ�
	
	UIPopupCreateRoom();
	~UIPopupCreateRoom();

public:

	virtual void	OnCreate(i3GameNode * pParent) override;

	/*
	param[in] pArg1 bool*	m_bCreateRoom
	*/
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual void	OnEntranceEnd() override;

	virtual bool	OnKeyInput(i3InputDeviceManager * pInputMng) override;

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape() override;

	virtual bool	OnPlayClickSound(i3UIControl * pCtrl) override;

protected:

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;

private:

	void			_InitializeCreateRoomInfo(void);						//�κ񿡼� �˾� ��.

private:

	CreateRoom_Base*	m_pCreateRoomBase[CR_BASE_MAX];

	bool				m_bCreateRoom;

public:

	//��� ���������� ����Ǹ� ������Ʈ �ϴ� ��ɵ�.
	void				UpdateforRule();
	void				UpdateforStage();

	//�� ��� ���� CreateRoom_Base�� �ҷ�����.
	CreateRoom_Base*	GetCreateRoomBase(CreateRoomBase base_enum);

	//UI EVENT �ӽ� �Լ�.
	void	ClickOk();
	void	ClickCancel();

};

#endif
