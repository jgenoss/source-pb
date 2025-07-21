#if !defined (_ATTENDANCE_CONTEXT_H_)
#define _ATTENDANCE_CONTEXT_H_

// Key : �⼮ üũ �߰�
//----------------------------------------------------------------------//
//								�⼮üũ ����							//
//----------------------------------------------------------------------//

struct AttendanceData
{
	ATTENDANCE_USER		m_AttendanceUser;
	ATTENDANCE_INFO		m_AttendanceInfo;

	bool IsAttendanceEvent() const
	{
		if( m_AttendanceInfo.m_ui32Version	== 0 )							return false;

		return true;
	}

	bool IsEventTarget() const
	{
		if (m_AttendanceUser.m_ui8EventState == ATTENDANCE_STATE_NONE)		return false;
		
		return true; 
	}

};

// __alignof(AttendanceData)  //

class AttendanceContext : public i3::shared_ginst<AttendanceContext>
{
public:
	AttendanceContext();
	~AttendanceContext();

	// 16.03.14 Attendance type �� ������ ���� ������ Get/Set �Լ��� � Ÿ���� �̺�Ʈ���� �����ϵ��� ���ڸ� �߰���
public:
	AttendanceData*			SetData(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL); 
	const AttendanceData*	GetData(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL) const;

	const ATTENDANCE_USER *	GetAttendanceUser(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL) const;
	void SetAttendanceState(UINT8 eState, ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL);
	void SetAttendanceSelectItem(UINT8 uiItem, ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL);
	const ATTENDANCE_INFO * GetAttendanceInfo(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL) const;

	// ���� ������� �⼮ �̺�Ʈ Ÿ���� ��ȯ
	ATTENDANCE_TYPE			GetCurrrentType();
	// ���� ������� �⼮ �̺�Ʈ�� ��� ���μ����� �Ϸ�Ǿ� ���� ������ �⼮ �̺�Ʈ�� ���
	void					SetNextType();

	// ���ڰ��� ���ٸ� �����ϴ� ��� �⼮ �̺�Ʈ�� �˻��Ͽ� �ϳ��� �ִٸ� true �� ��ȯ
	// ATTENDANCE_TYPE_NON ������ ���� ������ �ε����ϰ� MAX �� default ���� �Ӵϴ�
	bool	IsAttendanceEvent(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_MAX) const;
	bool	IsEventTarget(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_MAX) const;

private:
	AttendanceData		m_AttendanceData[ATTENDANCE_TYPE_MAX];

	ATTENDANCE_TYPE		m_ActivateEventType;

};

#endif