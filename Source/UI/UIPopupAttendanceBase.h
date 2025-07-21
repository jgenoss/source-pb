#if !defined( _UI_POPUP_ATTENDANCE_BASE_H_)
#define _UI_POPUP_ATTENDANCE_BASE_H_

#include "UIPopupBase.h"

class UIImgSetCtrl;

class UIPopupAttendanceBase : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupAttendanceBase, UIPopupBase);
public:
	UIPopupAttendanceBase();
	virtual ~UIPopupAttendanceBase();

public:
	virtual void	ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

protected:
	// �̺�Ʈ �Ⱓ�� �Ľ��Ͽ� ������ ��Ʈ���� �°� ����.
	void				SetPeriod(i3::rc_wstring& out, UINT32 ui32StartDate, UINT32 ui32EndDate, bool IsNext = false);

	// i3UiStaticText ������ UI Control �� ã�� �ؽ�Ʈ�� ����
	void				SetStaticText(i3::string findname, i3::rc_wstring& text);
	void				SetErrorMsg(INT32 arg);

protected:
	virtual	void		ReceiveGameEvent_Attendance_Check(INT32 arg, const i3::user_data &UserData) = 0;

protected:
	enum { MONTH_MAX_COUNT = 31, };
	enum
	{
		CLEAR_ATTENDANCE = 0,
		TODAY_ATTENDANCE,
		NEXT_ATTENDANCE,

		MAX_ATTENDANCE_COUNT,
	};

};

#endif