#if !defined( __SCORE_CONTEXT_H)
#define __SCORE_CONTEXT_H



class ScoreContext : public i3::shared_ginst<ScoreContext>
{
private:
	SCORE_INFO IndividualScore[SLOT_MAX_COUNT];

	UINT8 CurRoundCount;
	UINT8 CurrentDediRound;					// �ӽ÷� �̰��� Dedi�� ��Ž� ����մϴ�.
											// GameServer�ʹ� m_uCurrentRound�� ���. ����, Dedi, Client ���� Round check�� �ٸ��ϴ�.
											// ���Ŀ� �ݵ�� �ٲ��� ��..... swoongoo
											// Dedicated Server���� ��ſ����� �̰��� ���. ���� GS - Dedi - Client �� Round�� �ٸ��� ���ǰ� �ִ�..
	UINT16	m_AccTrainingScore;
	UINT16	m_AddTrainingScore;

	//�������� ������ ���� �̱���尪�Դϴ�.(���������� �������뿩�οͻ������ �������ǰ��� �����ϴ�)
	UINT16 m_redWinRound;
	UINT16 m_blueWinRound;
	SCORE_INFO m_teamScore[TEAM_COUNT];

	bool m_autoTeamChangeExecuted;			// �ڵ��������� ���࿩��
	bool m_executeShowTeamAutoChangeHudEffect; // �ڵ��������� ����Ʈ �ѹ��� ����ϰ��ϱ�����

public:
	ScoreContext();

	UINT8			getCurrentRound( void);
	void			setCurrentRound( UINT8 Round);
	UINT8			getCurrentDediRound( void) const			{ return CurrentDediRound; }
	void			setCurrentDediRound( UINT8 round)			{ CurrentDediRound = round; }

	UINT8			getCurrentRound( bool bDedicated)		{ return bDedicated ? CurrentDediRound : getCurrentRound(); }


	//
	// �ڵ��������� ����
	//
	void			ExecuteAutoTeamChange(); // �ڵ��������� ����, �����������ص� �ѹ����ȴ�.

		// �ڵ��������밡 ����Ǿ����� �����Ǻ�
		// MyRoomContext�� �����ʾҴ������� ScoreContext�� Battle�� ������Բ� ������ ���µǾ.. ���߿� �����ؽ��� �ű�°͵�
	bool			IsAutoTeamChangeExecuted() const { return m_autoTeamChangeExecuted; }	
	void			ShowTeamAutoChangeHudEffect();

	//
	// ���������� (�ڵ��������밡 ����Ǹ� �ٲ� �������� �����ݴϴ�
	//
		// �������� ������ ���Դϴ�. ������������ �������
	void			InitTeamKillCount(void);

	void			SetSvrTeamWinRound(UINT16 red, UINT16 blue);
	UINT16			GetSvrTeamWinRound(TEAM_TYPE team) const;
	const SCORE_INFO* GetSvrTeamScore(TEAM_TYPE team) const;
	void			SetSvrTeamScore(const SCORE_INFO& redScore, const SCORE_INFO& blueScore);
	
								
		// �ڵ��������뿡���� �������� ���� �˴ϴ�.
	UINT16				GetRedTeamWinRound( void) const;
	UINT16				GetBlueTeamWinRound( void) const;
	UINT16				GetTeamWinRound(TEAM_TYPE team) const;
	const UINT16		GetDinoEscapeScore(TEAM_TYPE team) const;
	const SCORE_INFO*	GetTeamScore(TEAM_TYPE team) const;


	INT32			getTotalRoundCount( void) const				{ return static_cast<INT32>(m_redWinRound + m_blueWinRound); }

	const SCORE_INFO * getIndividualScore( INT32 userSlot)		{ I3_BOUNDCHK( userSlot, SLOT_MAX_COUNT); return &IndividualScore[ userSlot]; }
	void			setIndividualScores( const SCORE_INFO* pScores, UINT32 size)
	{
		I3ASSERT( sizeof(IndividualScore) == size);
		i3mem::Copy( IndividualScore, pScores, size);
	}

	void			ResetIndividualScore(INT32 userSlot)	{ I3_BOUNDCHK(userSlot, SLOT_MAX_COUNT); IndividualScore[userSlot].Reset();	}

	void			SetAddTrainingScore(UINT16 addScore)		{ m_AddTrainingScore = addScore;	}
	UINT16			GetAddTrainingScore(void) const				{ return m_AddTrainingScore;		}
	void			SetAccTrainingScore(UINT16 accScore)		{ m_AccTrainingScore = accScore;	}
	UINT16			GetAccTrainingScore(void) const				{ return m_AccTrainingScore;		}
	void			ResetTrainingScore()						{ m_AccTrainingScore = m_AddTrainingScore = 0; }

	void			CreateBattle( void);
	void			DestroyBattle(void);

};

#endif