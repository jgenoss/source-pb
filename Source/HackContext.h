#pragma once

class HackContext : public i3::shared_ginst<HackContext>
{
public:
	void	InitHackFlag();				// ���� ���� ������ ���� �Լ� ��ġ ���� �ʿ��Ұ����� �����

	void	resetHackUserSlot();			//��� ���� �ʱ�ȭ.
	void	resetHackUserSlot( INT32 slot )	{	m_HackUserSlot[slot] = false;	m_fPenaltyTime[slot] = PENALTY_TIME;}

	void	setHackUserSlot(INT32 slot,HACK_CHECK_TYPE Type, INT32 Number);

	//Hack ���� ��� ǥ��
	bool	isHackUserCaution(INT32 slot) const	
	{	
		if( m_EnableHackCaution )
		{
			return m_HackUserSlot[ slot ];
		}
		return false;
	}

	//Hack ���� Frezen ���
	bool	isHackUserFrezen(INT32 slot) const	
	{	
		if( m_EnableHackFrezen )
		{
			return m_HackUserSlot[ slot ];
		}
		return false;
	}

	HACK_CHECK_TYPE getHackUsertype(INT32 slot) const	
	{
		return m_eHackType[slot];
	};

	INT32	getHackNumber(INT32 slot) const	
	{
		return m_nHackNumber[slot];
	};

	INT32	getPenaltyTime(INT32 slot) const		{	return (INT32)m_fPenaltyTime[ slot ];	}

	void	CreatePenaltyMsg();
	void	penaltyUpdate( REAL32 fElapsedTime);

	void	setPenaltyLimit( INT32 slot, REAL32 fLimit) { m_fPenaltyTime[slot] = fLimit;}

	void	HackingDetected(REAL32 rMoveSpeed, UINT32 nType);
	void	GoodbyeCheatUser(CHEAT_CONTEXT CheatContext);

private:

	enum	{ PENALTY_TIME = 30 };

	bool					m_HackUserSlot[SLOT_MAX_COUNT] = { false };
	REAL32					m_fPenaltyTime[SLOT_MAX_COUNT] = { 0.0f };
	HACK_CHECK_TYPE			m_eHackType[SLOT_MAX_COUNT] = { HACK_CHECK_TYPE_NONE };
	INT32					m_nHackNumber[SLOT_MAX_COUNT] = { 0 };

	bool	m_EnableHackCaution = false;
	bool	m_EnableHackFrezen = false;

};