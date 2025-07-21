#include "pch.h"
#include "UIBlink.h"

IntervalTimeEvent::IntervalTimeEvent(REAL32 interval) : m_interval(interval), m_current(0.0f), m_isOn(false) {}
IntervalTimeEvent::~IntervalTimeEvent() {}

void IntervalTimeEvent::Start()
{
	m_isOn = true;
}

bool IntervalTimeEvent::OnUpdate(REAL32 tm)
{
	if (!m_isOn) return false;

	m_current += tm;
	if (m_current >= m_interval)
	{
		(*this)();
		m_current = 0.0f;
		return true;
	}

	return false;
}

void IntervalTimeEvent::Stop()
{
	m_isOn = false;
}

bool IntervalTimeEvent::IsOn() const
{
	return m_isOn;
}



UIBlink::UIBlink(i3UIControl* ctrl, REAL32 interval) : IntervalTimeEvent(interval), m_ctrl(ctrl), m_isBright(false) {}
UIBlink::~UIBlink() {}

void UIBlink::operator()()
{
	/*if (!m_ctrl->isState(I3UI_STATE_ONMOUSE))
		m_ctrl->addState(I3UI_STATE_ONMOUSE);
	else
		m_ctrl->removeState(I3UI_STATE_ONMOUSE);

	m_ctrl->setModifyState(true);*/

	// �����Ÿ� ���¸� I3UI_STATE_ONMOUSE�� �����ϰԵǸ�
	// OnEnterMouse/OnLeaveMouse �Լ��� ���� ���°��� ��߳� �� �����Ƿ�
	// ������ ���� ��.
	if (m_isBright)
		m_ctrl->removeState(I3UI_STATE_ONMOUSE);
	else
		m_ctrl->addState(I3UI_STATE_ONMOUSE);

	m_ctrl->setModifyState(true);
	m_isBright = !m_isBright;
}