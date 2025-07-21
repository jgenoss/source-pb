#ifndef __UI_POPUP_DORMANT_EVENT_H__

#include "UIPopupBase.h"
#include "UIPopupAttendanceBase.h"

class UIImgSetCtrl;

/*
	�޸���� �⼮ �̺�Ʈ�� ������ Ŭ�����Դϴ�. (UPW_DORMANT_EVENT)
*/
class UIPopupDormantEvent : public UIPopupAttendanceBase
{
	I3_CLASS_DEFINE(UIPopupDormantEvent, UIPopupAttendanceBase);

public:
	UIPopupDormantEvent();
	virtual ~UIPopupDormantEvent();

public:
	virtual void				OnCreate(i3GameNode * pParent) override;
	virtual bool				OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	
	virtual void				OnUpdate(REAL32 rDeltaSeconds) override;
	virtual bool				OnEvent(UINT32 event, i3ElementBase* pObj, UINT32 param2, I3_EVT_CODE code) override;

protected:
	virtual void				_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void				_ClearAtUnload(i3UIScene * pScene) override;

private:
	/*		UI Animation	*/
	i3UIFrameWnd*				m_pFrmEffect;
	i3UIImageBox*				m_pSlideBack;				// �Ѿ��� �������鼭 TEXT�� ���� �� �ְ� �̸� �����δ� �̹���
	i3UIImageBox*				m_pSlideBullet;				// �Ѿ� �̹���

	bool						m_bAnimState;
	REAL32						m_rDelayTime_bullet;
	REAL32						m_rDelayTime_back;

	const REAL32				m_r32Bullet_PosX;
	const REAL32				m_r32BulletBack_PosX;
	const REAL32				m_r32BulletBack_MoveStart_PosX;
	const REAL32				m_r32BulletSpeed;
	const REAL32				m_r32AnimDuration;

	/*	���� ������ Control */
	UIImgSetCtrl *				m_pImgCtrl[2];
	i3UIStaticText *			m_pRewardText[2];
	i3UIButtonImageSet *		m_pRewardIcon[2];
	i3UIButtonComposed3*		m_pBtnOK;

	// 31�� ������ ����Ʈ
	i3::vector<i3UIButtonImageSet *> m_vMonthItemBG;

	/* ���� ������ �̺�Ʈ ���� ���� ���� ������ */
	ATTENDANCE_STATE			m_EventState;
	INT32						m_TodayAttendance;	// ������ ������ �⼮����
	INT32						m_ViewItemIdx;		// �ϴܿ� ������ �������� ������ �ϴ���
	INT32						m_RewardItemIdx;

	INT32						m_InputState;

	enum {
		INPUT_NONE = 0,			// �ʱ����
		INPUT_FIRST_PRESS,		// �ִϸ��̼��� ����Ǳ� ��, ���콺 �Է��� �ִ� ����
		INPUT_NON_RELATED,		// �Է°� ���̻� ���谡 ���� ����
	};

public:
	/*	UI LUA Input	*/
	void						Confirm();
	void						SelectReward(INT32 idx);
	void						StopAnim();

private:
	/*	UI Animation	 */
	void						ResetAnimation();
	void						PlayAnimation(REAL32 rDeltaSeconds);
	bool						DisableBtnConfirm();

	/* Set UI */
	void						SetEventData();
	void						SetCompletedDay(INT32 idx, INT32 shape_type);
	void						SetViewTodayItem();
	void						SetTodayItemIcon(ATTENDANCE_CHOICE_ITEM& item_, bool isViewOnly);

	/* About Game Event*/
	// ProcessGameEvent �Լ��� Base Ŭ������ �����ϰ�, EVENT_ATTENDANCE_CHECK �� �߻��� ��� �Ʒ� �Լ��� ȣ���
	virtual void				ReceiveGameEvent_Attendance_Check(INT32 arg, const i3::user_data &UserData);
	void						CompleteEvent();
};


#endif