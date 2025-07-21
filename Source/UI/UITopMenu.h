/**
 * \file	UI\UITopMenu.h
 *
 * Declares the user interface top menu class.
 */

#pragma  once

#include "UIBase.h"
#include "UI/UIFlowEditBox.h"

enum CAM_ANIM_STATE
{
	CAS_IDLE = 0,
	CAS_IDLE_TO_LEFT,
	CAS_IDLE_TO_RIGHT,
	CAS_LEFT_TO_IDLE,
	CAS_LEFT_TO_RIGHT,
	CAS_RIGHT_TO_IDLE,
	CAS_RIGHT_TO_LEFT,
	
	CAS_MAX
};

enum TOPMENU_MODE
{
	TOPMENU_MODE_NONE = -1,
	TOPMENU_MODE_SERVER = 0,
	TOPMENU_MODE_CHANNEL,
	TOPMENU_MODE_READY,
	TOPMENU_MODE_READYROOM,
	TOPMENU_MODE_SHOP,
};

enum FLOW_STATE
{
	FLOW_STATE_INIT = 0,
	FLOW_STATE_READ_LETTER,
	FLOW_STATE_FLOW,

	FLOW_STATE_MAX
};

enum CAM_LOOK_CMD
{
	CL_IDLE = 0,	// �⺻
	CL_SHOP,		// ��
	CL_WEAPON,		// �����

	CL_MAX
};

class UITopMenu : public UIBase
{
	I3_CLASS_DEFINE( UITopMenu, UIBase);

protected:
	TOPMENU_MODE	m_Mode;
	REAL32			m_rElapsedBackButtonTime;
	i3AttrSet *		m_pAttrSet;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene)=0;


public:
	UITopMenu();
	virtual ~UITopMenu();

	void					SetMode( TOPMENU_MODE mode);
	TOPMENU_MODE			GetMode( void) const { return m_Mode; }

	virtual void			Clicked(INT32 value)=0;
	virtual	bool			ClickBackButton( void)=0;
	virtual void			SetUICharaCamera( CAM_LOOK_CMD state)=0;

	virtual void			ClickPopupBackButton(bool close_popup = false)=0;	//TopMenu���� control �ϴ� menu �� �� �Լ��� close ����.
	virtual void			StartHighlightMailbox( void)=0;

	virtual void			AllMenuBtnShapeToNormal() = 0;

	virtual void			SyncSubPhaseWithMenuBtnState() = 0;

	//chunjong.song 2014-07-15
	// UIPopupbase���� UIPopup_Close�Լ���� TogglePopup/ClosePopup�� �̿��ؼ� �˾��� ������
	// UITopMenu�� ���� ��� �˾���(Mailbox, option)�� ������ �߰��۾�ó���� �����ʾ� ������ �����Ѵ�(������ �˾�����ġ ����)
	// UITopMenu�� ���� �˾��鵵 TolgglePopup/ClosePopup�� �̿��ؼ� �ݾƵ� �������� �ϱ����ؼ� �Ʒ��Լ��߰�
	virtual void			uglyOnMainFrameClosePopupAfter(INT32 popupType)=0;
	virtual bool			IsServerBasePopup()=0;

	virtual bool			ClickInventory()=0;

	/** \brief new & hot ��ư ������ ��� */
	void					GotoShopFromNewAndHot();

	virtual i3AttrSet *		Get3DRoot(void) { return m_pAttrSet; }

protected:
	// Menu Button�� select Server������ �ٲ۴�.
	virtual void			_SetServerMode( void)=0;
	// Menu Button�� select Channel������ �ٲ۴�.
	virtual void			_SetChannelMode( void)=0;
	// Menu Button�� �Ϲ� Ready��(Lobby, readyroom)���� �ٲ۴�.
	virtual void			_SetReadyMode( void)=0;
	

	//--------------------------------------------------------------------------//
	//									�帣�� ����								//
	//--------------------------------------------------------------------------//
protected:
	UIFlowEditBox	m_FlowEditBox;

	bool			m_bTemporaryNotice;

	i3::rc_wstring	m_strPermanatNotice;
	i3::rc_wstring	m_strTemporaryNotice;

	bool			m_bAlarmCommunity;
	bool			m_bAlarmFlag;
	REAL32			m_rAlarmTerm;

protected:
	void			_UpdateFlowAnnounce( REAL32 rDeltaSeconds);

public:
	/* ������ ��ư�� �����̰��Ѵ�  - 2012.01.18 ���õ */
	void			_UpdateMailBoxAlaram(REAL32 rDeltaSeconds);
	void			_CheckUnReadMail();
	void			_EndHighlightMailbox();

public:
	void			SetSlideNotice_Permanant( const i3::rc_wstring& wstrStr);
	void			SetSlideNotice_Temporary( const i3::rc_wstring& wstrStr);
};

class PBGaugeBar
{
	enum { MAX_IMG_ANI = 16, };

private:
	void update_none(REAL32 rDeltaSeconds) {}
	void update_gauge(REAL32 rDeltaSeconds);

	void resize(REAL32 factor);

public:
	PBGaugeBar() : fn_update(&PBGaugeBar::update_none) {}

	void init(i3UIScene * pScene);

	void reset();
	void set_lastest_target_factor();
	void set_factor(REAL32 unit_factor);
	void update(REAL32 rDeltaSeconds);

public:
	void (PBGaugeBar::*fn_update)(REAL32 rDeltaSeconds);

	i3UIFrameWnd *logo_title;
	i3UIButtonImageSet* logo_imgset;
};