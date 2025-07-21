#pragma once


struct PING
{
	// �� ���� �˻� �ð�
	REAL32			m_fPingRecvTime = 0.0f;
	// �� �˻縦 ���� �ð�
	REAL32			m_rPingSendTime = 0.0f;
	// �� ����� �� ��(���� ȭ�鿡 �Ѹ��� ���� �뵵)
	UINT8			m_Pings[SLOT_MAX_COUNT] = { 0 };
	// Ping �˻� �ð�
	REAL32			m_rPingCheckTime = 0.0f;
	// ���� Packet�� ������ ������ ���� �ð����κ��� Ÿ���� ����Ѵ�.
	REAL32			m_rPingList[SLOT_MAX_COUNT] = { 0.0f };		// m_PingList
	// ���� ���޿� ���� �� Ÿ�� ( 1�� = 1000 )
	UINT16			m_PingTimes[SLOT_MAX_COUNT] = { 0 };
	/** \brief ó�� �ʱ�ȭ �Լ� */
	void	Init( void);

	/** \brief Round ���۽� �ʱ�ȭ �Լ� */
	void	Reset( void);

	/** \brief Ping Packet�� ������ ó���Ѵ�. */
	void	RecvPing( INT32 userIdx, REAL32 sendTime, REAL32 elapsedTime);

	UINT8		GetPing(INT32 iUserIdx ) const
	{ 
		I3_BOUNDCHK( iUserIdx, SLOT_MAX_COUNT); return m_Pings[ iUserIdx ]; 
	}

	void		SetPings( UINT8 * pPings, UINT32 size)
	{
		I3ASSERT( sizeof( m_Pings) == size);
		i3mem::Copy( m_Pings, pPings, size);
	}
};


class BattleServerContext : public i3::shared_ginst<BattleServerContext>
{
public:

	BattleServerContext();
	~BattleServerContext();

	BATTLE_SERVER_TYPE	GetServerType( void) const				{ return m_ServerType; }
	void				SetServerType( BATTLE_SERVER_TYPE type)	{ m_ServerType = type; }

	
	void				SetP2PActivate()	{ m_bP2PActivate = true; }
	void				SetP2PDeactivate()	{ m_bP2PHost = false; m_bP2PActivate = false; }

	void				SetP2PHost()		{ m_bP2PHost = true; }
	void				SetP2PNonHost()		{ m_bP2PHost = false; }


	bool		IsP2PActivate() const	{	ISNETWORKRETURNV(false,true);	return m_bP2PActivate;	}
	bool		IsP2PHost() const		{	ISNETWORKRETURNV(false,true);	return IsUseDedicationMode()?false:m_bP2PHost;	}

	bool		IsUseRelayServer() const		{	return m_ServerType==BATTLE_SERVER_TYPE_RELAY; }
	bool		IsUseDedicatedServer() const	{	return m_ServerType==BATTLE_SERVER_TYPE_DEDICATION; }
	bool		IsUseBattleServer()const		{	return (IsUseDedicatedServer() | IsUseRelayServer()) ; 	}
	bool		IsUseDedicationMode()const ;
	
	const PING*	GetPingInfo( void) const				{ return &m_PingInfo; }
	PING*		SetPingInfo( void)						{ return &m_PingInfo; }

	// P2P�����Ȳ �Լ��� ��� ���⿡ �ű��..
		
	void	InitRoundData( void );				
	void	InitMulticastInfo( INT32 idx );

	void	InitP2PData(void);							// P2P�ʱ�ȭ(������ �����ϱ����� ȣ��Ǿ����)

	// Battle Load (Init P2P data, Create UDPSocket, create receiver)
	void	CreateBattle( bool bLoaded = false );

	// Intdrude Battle (Select Host, Client mode)
	void	P2PIntrudeBattle( void);

	// Battle ����
	void	P2PEndBattle( void );

	/** \brief ������ ���� ���
		\note ������ ���� �����մϴ�. */
	void	P2PHostChange( INT32 nNewHostIdx );

	/** \brief �ڽ��� ĳ���͸� ����ϰ� �ε��� �����Ѵ�. */
	void	P2PMyStartBattle( void);

	/** \brief ������ */	
	void	ChangeTeam( void);  
	// chunjong.song 2014-09-03 : �������밡�Ǿ� ���ƹ�Ÿ�� �����ϴ°��� �ش�����̳��������� �ѹ��� �����մϴ�.
	// hansoft://115.91.29.169;zepetto;77b90e39/Task/1095487  (��: ���帶�� �������� �Ұ���)

	/** \brief �ڽ��� ĳ���� �ε��� ��ٸ���. */
	bool	WaitLoadingMyAvatar( void);

	/** \brief �����鿡 ���� ������ �޾Ҵ�.
			�ٸ� ������ ĳ���͸� ��������.
		\param[in] bVisible �����ϴ� ĳ������ visible ����.*/
	void	Battle_EntranceAnotherUser( bool bVisible = false, bool bForceCreateAvatar = true);

	// UDP NetInfo ���� �� Ȧ ��Ī ����
	void	P2PPreStartBattleUser( INT32 iSlotidx );

	// ������ ���ӿ� �����մϴ�. 
	bool	P2PStartBattleUser( INT32 iSlotidx, bool bStart = false);

	// ���ӿ��� ����
	void	P2PGiveupBattleUser( INT32 iSlotidx );

	/** \brief ĳ���� ��Ȱ ó�� */
	void	P2PRespawnChara( INT32 iSlotIdx);
	
	bool		IsMovingStage(void) const;	
	UINT32		GetPreStageID(void) const			{ return m_nPreStageID;		}
	void		MoveStageStart( UINT32 nStageID )	{ m_nPreStageID	= nStageID;	m_bMoveStage	= true;	}
	void		MoveStageEnd(void)					{ m_bMoveStage	= false;	}

	static void			sAfterLoadChara( void * pAvatar, INT32 idx = -1, bool bChangeParts = false, AI_BIND_DATA * pAIBind = nullptr);

private:
	

	void	_InitUserData( INT32 idx );					// ���� ������ �ʱ�ȭ(��Ʈ��ũ ������ �ʱ�ȭ��) in Battle

#if defined( DEF_OBSERVER_MODE)
	void	_InitUserDataForObserver( INT32 idx);		// Observer slot�� data �ʱ�ȭ
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Debug
	void	ProfileNetwork( void );


	BATTLE_SERVER_TYPE	m_ServerType;
	bool				m_bP2PHost;
	bool				m_bP2PActivate;

	PING				m_PingInfo;

	// �������� ���� - �ΰ��ӳ����� �ٸ� ����������
	bool		m_bMoveStage;
	UINT32		m_nPreStageID;

	bool		m_bWaitLoadForTeamChange;	// �� ���� �ý��ۿ� ���� �ڽ��� ĳ���� �ε� Ȯ�ο�

	// revision 31898 ������ ��û�� ������ 10���ı��� ������ ACK�� ���� ���Ұ�� ��������� �����ϴ�.
private:
	REAL32					m_fCheckRespawnaAck;
	bool					m_bCheckRespawnaReq;
public:
	REAL32	getRespawnAckTime(){return m_fCheckRespawnaAck;}
	bool	IsRespawnReq(){return m_bCheckRespawnaReq;}
	void	setRespawnReq(bool bTrue){m_bCheckRespawnaReq = bTrue;}
	void	setRespawnAckTime(REAL32 rTime){ m_fCheckRespawnaAck = rTime;}

	void	OnUpdate( REAL32 rDeltaTime);

	bool	LoadingAnotherCharacter(void);

	// Skin�� �ε� �Ǳ� ���� �ƹ�Ÿ ������ �Ǹ� �Ӹ��� �߸� �������� ���ͼ� Ÿ�̸Ӹ� �ξ�
	// ���� ����ϴٰ� Skin�� �ε� �ǵ��� �����մϴ�.
private:

	// 10�ʰ� ��� �ϸ鼭 ��� ������ �ε� �Ǵ��� Ȯ���� �ʿ�
	UINT32		m_TimeCheck;
	UINT32		m_BaseTime;

	bool	_AnotherCharaTimer();
};



