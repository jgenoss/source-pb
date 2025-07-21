#pragma once


//
// ������ �ʴ� �� ������ �ϴ� �и���...
//

class MedalContext : public i3::shared_ginst<MedalContext>
{
public:
	UINT32	getMiniature(void) const	{ return m_iMiniature;		}	//����
	void	setMiniature(UINT32 val)	{  m_iMiniature = val; }

	UINT32	getInsignia(void) const		{ return m_iInsignia;		}	//����
	void	setInsignia(UINT32 val)		{  m_iInsignia = val;	}

	UINT32	getOrder(void)	const		{ return m_iMedal;			}	//����
	void	setOrder(UINT32 val)		{  m_iMedal = val;	}

	UINT32	getMaster(void) const		{ return m_iMedalMaster;	}	//������ ����
	void	setMaster(UINT32 val)		{  m_iMedalMaster = val;	}

	void	SetMedalInfo( UINT32 miniature, UINT32 insignia, UINT32 order, UINT32 master)
	{
		m_iMiniature = miniature; m_iInsignia = insignia; m_iMedal = order; m_iMedalMaster = master;
	}

	bool	GetReceivedMyMedal() const				{ return m_bReceivedMyMedal[0] && m_bReceivedMyMedal[1] && m_bReceivedMyMedal[2];	}
	void	SetReceivedMyMedal(INT32 idx, bool val)	{ m_bReceivedMyMedal[idx] = val; }

private:
	UINT32		m_iMiniature = 0;		// ����
	UINT32		m_iInsignia = 0;		// ����
	UINT32		m_iMedal = 0;			// ����
	UINT32		m_iMedalMaster = 0;		// ������ ����

	bool		m_bReceivedMyMedal[3] = { false, false, false };

};
