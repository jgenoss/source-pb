#if !defined( __INTRUDE_CONTEXT_H)
#define __INTRUDE_CONTEXT_H

enum MY_INTRUDE_STATE
{
	MY_INTRUDE_STATE_NONE = 0,
	MY_INTRUDE_STATE_INTRUDE,
	MY_INTRUDE_STATE_PROCESSOK,
	MY_INTRUDE_STATE_PAUSED = MY_INTRUDE_STATE_PROCESSOK,	// GM Pause �� ���� �� m_ui8IinterEnter �� 2�� ���� �˴ϴ�.
};

// �ڽ��� ������ ��� üũ
class IntrudeContext : public i3::shared_ginst<IntrudeContext>
{
public:
	IntrudeContext();
	~IntrudeContext();

	void Clear() {  m_eState = MY_INTRUDE_STATE_NONE; m_ui8InterEnter = 0; }

	void Start() { m_eState = MY_INTRUDE_STATE_INTRUDE;}
	void End() { m_eState = MY_INTRUDE_STATE_PROCESSOK;}
	void ProcessIntrude();

	bool IsIntrudeStart() const { return (m_eState == MY_INTRUDE_STATE_INTRUDE);}
	bool IsIntrudeEnd() const { return (m_eState == MY_INTRUDE_STATE_PROCESSOK);}

	void InterEnter( UINT8 intrude);
	UINT8 GetInterEnter() const				{ return m_ui8InterEnter;}
	bool IsInterEnter(void) const			{ return m_ui8InterEnter != MY_INTRUDE_STATE_NONE; }
	
	void SetIntrusionMsgFlag(bool bEnable)	{  m_bIntrusionMsgFlag = bEnable; }
		/** \brief ���Ը޽��� �������*/
	bool IsIntrusionMessage(void) const		{ return m_bIntrusionMsgFlag; }

private:

	MY_INTRUDE_STATE	m_eState;				//���� ���� ~ ���� ��
	UINT8				m_ui8InterEnter;
	bool				m_bIntrusionMsgFlag;
};

#endif

