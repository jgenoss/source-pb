#ifndef _AwayChecker_h
#define _AwayChecker_h

enum AwayCheckerEvent
{
	ACE_START = I3_EVT_USER + 100000,			// Ȥ�� ���� �浹�� ���ϱ� ����

	ACE_AWAY = ACE_START,
	ACE_CAUTION,

	ACE_MAX
};

/*	Ư�� �ð��� listener �ϴ� Ŭ���� �̴�.
	AwayTime, CautionTime���� ������ �ð��� �Ǹ�, ��ϵ� Receiver�� ���ؼ� 
	ACE_AWAY, ACE_CAUTION �̺�Ʈ�� �߻��ȴ�.
	Receiver �� OnEvent(UINT32,i3ElementBase* ,UINT32, I3_EVT_CODE) �Լ��� ���ؼ� �޴´�.

	�̺�Ʈ �߻� �ð�
	��) AwayTime : 60.f, CautionTime : 5.f
	55�ʿ� ACE_CAUTION �̺�Ʈ �߻�, 60��(1��)�̸� ACE_AWAY �̺�Ʈ �߻�
*/
class CAwayChecker: public i3GameNode
{
	I3_CLASS_DEFINE(CAwayChecker, i3GameNode);
public:
	CAwayChecker();
	virtual ~CAwayChecker();

	virtual void OnUpdate(REAL32 rDeltaSeconds) override;

	void SetReceiver(i3EventReceiver* pObserver);
	void SetTime(REAL32 rAwayTime,REAL32 rCautionTime,bool bOneTimeNotify = true);
	void SetNotify(bool bOneTimeNotify);
	void Restart(void);
	void Reset(void);
	REAL32 GetCurrentTime() { return m_rCurrentTime; }
	
private:
	i3EventReceiver* m_pObserver;
	REAL32 m_rAwayTime;
	REAL32 m_rCautionTime;
	REAL32 m_rCurrentTime;
	bool m_bOneTimeNotify;
	bool m_bPause;
	bool m_bAwayNotify;
	bool m_bCautionNotify;
};

#endif //_AwayChecker_h
