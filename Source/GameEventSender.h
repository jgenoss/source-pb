#pragma once

class CGameEventHandler;

class GameEventSender : public i3::shared_ginst<GameEventSender>
{
public:
	GameEventSender();
	~GameEventSender();
	
	void	OnCreate();

	//���ӿ��� �ʿ��� �̺�Ʈ�� ����մϴ�. ���������δ� ������ �޽����� ������.(Game -> GameContext)
	bool	SetEvent(INT32 Event, const void * pArg = nullptr, const void * pArg1 = nullptr, const void * pArg2 = nullptr, LPCSTR szFile = nullptr, int nLine = 0);
	// SetEvent�� ������ ����� �ϳ� �߻���Ų �̺�Ʈ�� ������ ���������� ������ �̺�Ʈ�� �߻��� ��� �����ϵ��� �ϴ� ����� �����ϴ�.
	bool	SetEventOnce(INT32 Event, const void * pArg = nullptr, const void * pArg1 = nullptr, const void * pArg2 = nullptr);
	
private:
	CGameEventHandler *	m_pEventHandler;

	INT32 m_BeforeEvent;			// ���� ��ġ�� �������� ����..
};
