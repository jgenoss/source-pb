#pragma once

#ifdef	DOMI_UTIL

enum	TIMER_STATE
{
	TIMER_STATE_PROCESS		= 0,	// Ÿ�̸� ������
	TIEMR_STATE_COUNTEND,			// Ÿ�̸��� ī��Ʈ�� ����
	TIMER_STATE_END,				// Ÿ�̸ӿ� ī��Ʈ�� ������ ����ɶ�
	TIMER_STATE_ENDED,				// �̹� ����� ����
};

class CMsgTimer
{
private:
	bool			m_bEnable;

	REAL32			m_rRemainTime;
	REAL32			m_rLimitTime;
	INT32			m_nCount;
	INT32			m_nLastCount;

	TIMER_STATE	(CMsgTimer::*m_pfUpdater)(REAL32);


private:
	TIMER_STATE	_UpdateCount( REAL32 rDeltaSec);
	TIMER_STATE	_UpdateOnce( REAL32 rDeltaSec);


public:
	CMsgTimer(void);
	virtual ~CMsgTimer(void);

	void		SetEnable(bool bEnable)			{ m_bEnable	= bEnable;	}
	bool		IsEnable(void) const			{ return m_bEnable;		}
	INT32		GetCount(void) const			{ return m_nCount;		}
	
	void		SetTimer( REAL32 rLimitTime, INT32 nCount = 0, bool bNonCountZero = true );
	TIMER_STATE	OnUpdate( REAL32 rDeltaSec);
};

#endif