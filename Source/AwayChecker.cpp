#include "pch.h"
#include "AwayChecker.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE(CAwayChecker);//, i3GameNode);

CAwayChecker::CAwayChecker()
{
	m_pObserver = nullptr;
	m_rAwayTime = 0.0f;
	m_rCautionTime = 0.0f;
	m_rCurrentTime = 0.0f;
	m_bOneTimeNotify = true;
	m_bPause = true;
	m_bAwayNotify = false;
	m_bCautionNotify = false;
}

CAwayChecker::~CAwayChecker()
{
	// Empty
}

void CAwayChecker::OnUpdate(REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate(rDeltaSeconds);

	// ���� ������ ó������ �ʴ´�
	if (m_bPause) return;

	// �뺸�� class�� ���ٸ� ó������ �ʴ´�
	if (nullptr == m_pObserver) return;

	if( !UserInfoContext::i()->CheckAuthLevel() )	// revision 44121
	{
		// 0.0f ���� ����
		m_rCurrentTime = MAX(0.0f, m_rCurrentTime - rDeltaSeconds);
	}

	// Away �˻�
	if (m_rCurrentTime <= 0.0f)
	{
		if (m_bOneTimeNotify && m_bAwayNotify)
		{
			m_bPause = true;
		}
		else
		{
			m_pObserver->OnEvent(ACE_AWAY, this, 0, I3_EVT_CODE_ACTIVATE);
			m_bAwayNotify = true;
		}

		return;
	}

	
	// Caution �˻�	
	if (m_rCurrentTime <= m_rCautionTime)
	{
		if (false == m_bOneTimeNotify || false == m_bCautionNotify)
		{
			// ���� �ð��� �����Ѵ�
			m_pObserver->OnEvent(ACE_CAUTION, this, (UINT32)m_rCurrentTime, I3_EVT_CODE_ACTIVATE);
			m_bCautionNotify = true;
		}
	}
}

void CAwayChecker::SetReceiver(i3EventReceiver* pObserver)
{
	m_pObserver = pObserver;
}

void CAwayChecker::SetTime(REAL32 rAwayTime,REAL32 rCautionTime,bool bOneTimeNotify)
{
	// �ʱ�ȭ
	m_bOneTimeNotify = bOneTimeNotify;
	m_bPause = true;
	m_bAwayNotify = false;
	m_bCautionNotify = false;	

	// �� �� ����
	m_rAwayTime = MAX(0.0f, rAwayTime);
	m_rCautionTime = MAX(0.0f, rCautionTime);

	m_rCurrentTime = m_rAwayTime;

	m_bPause = false;
}

void CAwayChecker::SetNotify(bool bOneTimeNotify)
{
	m_bOneTimeNotify = bOneTimeNotify;
}

void CAwayChecker::Restart(void)
{
	m_rCurrentTime = m_rAwayTime;
	
	m_bAwayNotify = false;
	m_bCautionNotify = false;

	m_bPause = false;
}

void CAwayChecker::Reset(void)
{
	m_rAwayTime = 0.0f;
	m_rCautionTime = 0.0f;
	m_rCurrentTime = 0.0f;

	m_bOneTimeNotify = true;
	m_bPause = true;
	m_bAwayNotify = false;
	m_bCautionNotify = false;
}
