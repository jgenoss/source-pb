/**
 * \file	Network\GameNetworkManager.h
 *
 * Declares the game network manager class.
 */


#if !defined( __GAME_NETWORK_MANAGER_H__)
#define __GAME_NETWORK_MANAGER_H__

class ClientTCPSocket;
class ClientUDPSocket;
class CUdpHolePuncherForClient;
class CUdpRelay;
class CUDPRendezvous;
class CGameUDPHandler;

#include "ProfileDef.h"

#define HEARTBIT_SEND_TIME			60.f

#if defined( THREAD_TCP)
class cThreadTCP;
class cThreadTCP : public CGameThread
{
	I3_CLASS_DEFINE( cThreadTCP, CGameThread);
	i3Timer *				m_pTimer;
	REAL32			m_ElapsTime;

public:
	cThreadTCP();
	virtual ~cThreadTCP();

	virtual void	Run( void *pData) override;
};
#endif

class CGameNetworkManager : public i3::shared_ginst<CGameNetworkManager>
{
private:
	ClientTCPSocket				*	m_pTCPClientSocket;		// Server ��ſ� TCP handler
#if defined( THREAD_TCP)
	cThreadTCP					*	m_pTCPThread;
#endif

	ClientTCPSocket				*	m_pReserveSocket;

	ClientUDPSocket				*	m_pUDPClientSocket;		// UDPSocket
	
	CUdpHolePuncherForClient	*	m_pUDPHolePuncher;		// Ȧ ��Ī��
	//	CUdpRelay					*	m_pUdpRelay;			// �����̼��� �����

	CUDPRendezvous				*	m_pUDPRendezvous;		// ������ ���� �����
	bool							m_bRendezvousUpdate;	//Rendezvous Ȱ��ȭ��

#if !defined( I3_NO_PROFILE )
	NETPROFILE::info_t				m_ProfileData;
#endif

public:
	ClientTCPSocket *		GetTCPSocket( void)					{ return m_pTCPClientSocket; }
	ClientUDPSocket *		GetUDPSocket( void)					{ return m_pUDPClientSocket; }
	CUdpHolePuncherForClient	*	GetUDPHolePuncher( void)	{ return m_pUDPHolePuncher; }
	CUDPRendezvous *		GetUDPRendezvous( void)				{ return m_pUDPRendezvous; }

	ClientTCPSocket	*		GetReserveSocket(void)				{ return m_pReserveSocket;}

#if !defined( I3_NO_PROFILE)
	NETPROFILE::info_t*			GetProfileData(void)	{return &m_ProfileData; }
	NETPROFILE::info_t*			SetProfileData(void)	{return &m_ProfileData; } 
#endif

public:
	CGameNetworkManager();
	virtual ~CGameNetworkManager();

	// revision 40466
	/*		������ ������ ���� �� MacAddress*/
	bool		GetConnectedMacAddress( UINT64* pMacAddress);

	//----------------------------------------------------------------------------------//
	//										TCP functions								//
	void		CreateTCPSocket( void);
	bool		ConnectTCP( char * IP, UINT16 Port);
	void		DisconnectTCP( void);
	void		_CreateTcpThread( void);

	void		Update( REAL32 DeltaTime);
	void		UpdateHeartbeat( REAL32 AccTime);

	void		ChangeSocket( void);
	void		DestroyTCPSocket( void);

	void		SetEnteredServer( bool bEnter);
	bool		GetEnteredServer( void);

	bool		IsTCPConnected( void);
	bool		IsRealIP( void);

	INT32		GetSessionIndex( void);

	UINT16		GetGamePort( void);
	void		SetGamePort( UINT16 Port);

	const NET_UDP_INFO *	GetMyNetInfo( void)	const;
	NET_UDP_INFO *	SetMyNetInfo( void );

	bool		SendTCPMessage( i3NetworkPacket * pPacket);
	bool		SendTCPMessage( S2MOPacketBase* pS2MOPacket );

	//----------------------------------------------------------------------------------//
	//									Reserve Functions
	bool		ConnectReserveToGateway( char * IP, UINT16 Port);
	bool		ConnectReserveToGame( char * IP, UINT16 Port);
	void		DisconnectReserve( void);
	bool		SendReserveMessage( i3NetworkPacket * pPacket);

	bool		IsReserveConnected( void);

	//----------------------------------------------------------------------------------//
	//									UDP functions									//
	void		CreateUDPSocket( UINT16 Port);
	void		DestroyUDPSocket( void);

	//void		ConnectUDPRendezvous( bool bHost, UINT16 Port);
	//void		DisconnectUDPRendezvous( void);

	//void		SetRendezvousUpdate( bool bUpdate)		{ m_bRendezvousUpdate = bUpdate; }
	
	//----------------------------------------------------------------------------------//
	//								Hole puncher functions								//
	void		RelayHolePunchEndSend( UINT32 IP, UINT16 Port);

	//----------------------------------------------------------------------------------//
	//								Game Network Functions								//
	//	Desc:�����߿� ȣ��Ǿ� Network������ ������ �ϰų� State�� Update�ϴ� �Լ���	//
	//----------------------------------------------------------------------------------//
	/** \brief Round �ʱ�ȭ */
	void		InitRound( void);
	/** \brief User network info �ʱ�ȭ */
	void		InitUser( INT32 userIdx);
	/** \brief User Chara respawn */
	void		Respawn( INT32 userIdx);
	/** \brief Intrude Battle
		\desc Battle ���Խ� ȣ��˴ϴ�.*/
	void		IntrudeBattle( bool bHost);
	/** \brief ���� ������ �õ��մϴ�
		\desc Holepunching �õ� */
	void		PreStartBattleProcess( INT32 userIdx, bool bHost, bool bUseRelayServer);
	/** \brief user�� game ����
		\desc ������ ������ ȣ��˴ϴ�. */
	void		StartBattleProcess( INT32 userIdx, INT32 MyIndex, bool bHost);

	/** \brief for Only Host
		\desc Network ������ �ʱ�ȭ�մϴ�. */
	void		StartBattleUserProcess( INT32 userIdx);

	void		ChangeHostProcess( INT32 MainSlotIdx, bool bHost, bool bUseRelayServer);
	void		UserLeaveBattleProcess( INT32 userIdx, bool bUseRelayServer);

#if defined( I3_DEBUG)
	void		Dump( void);
	void		RoundEndDump( REAL32 GameTime);
#endif
};

#endif
