#if !defined( __CUDPHOLEPUNCHERFORCLIENT_H )
#define __CUDPHOLEPUNCHERFORCLIENT_H

//////////////////////////////////////////////////////////////////////////
//								DEFINE									//
//////////////////////////////////////////////////////////////////////////
#define UDPHOLE_SENDCOUNT			3		// 10�� ���� �غ��ϴ�.
#define UDPHOLE_SENDTIME			5.0f	// 5�ʿ� �ѹ��� �����ش�.
#define UDPHOLE_SENDTIMEFORPEER		0.5f	// �ʴ� 2ȸ �����ϴ�.
#define UDPHOLE_RECVTIME			5.0f		
#define UDPHOLE_TIMEOUT				60.f	// 30��

#define CLIENT_UDP_TIME_OUT			10.0f
#define UDP_SEND_TIME				0.1f	//10��
#define RECVOK_SEND_TIME			0.05f	//20�� 
#define RECV_SEND_TIME_OUT			1.0f

#define CLIENT_RENDEZVOUS_TIME_OUT	5.0f
#define RENDEZVOUS_SEND_TIME		1.0f	//10��

#define CLIENT_RELAY_TIME_OUT		20.0f
#define RELAY_SEND_TIME				1.0f	//10��
#define RELAY_SEND_MASTER_TIME		0.5f
//////////////////////////////////////////////////////////////////////////
//								ENUM									//
//////////////////////////////////////////////////////////////////////////
enum UDPHOLE_STATE
{
	UDPHOLE_STATE_NOTHING = 0,	
	UDPHOLE_STATE_MAIN_START_WAIT,		//������ ó�� �����Ҷ� 
	UDPHOLE_STATE_MAIN_START_RECV,		//����.

	UDPHOLE_STATE_MAIN_ROOM_PRESTART,	//�濡�� ���� �����Ҷ� 
	UDPHOLE_STATE_MAIN_ROOM_START,		//����. 
	UDPHOLE_STATE_MAIN_ROOM,			//���� & NAT �϶�.
};

class CUdpHolePuncherForClient : public i3ElementBase
{
	I3_CLASS_DEFINE( CUdpHolePuncherForClient, i3ElementBase );
protected:
	UINT32				m_ServerState;					// Ȧ��Ī ���� (�����ϰ�)
	SERVER_UDP_STATE	m_ServerUDPState;				// Ȧ��Ī ��� ���� (�����ϰ�)

	REAL32				m_fJoinTimeOut;
	REAL32				m_fJoinSendTime;

	REAL32				m_UserCheckSendTime[ USER_SLOT_COUNT ];
	REAL32				m_UserCheckSendTimeOutTime[ USER_SLOT_COUNT ];

private: 
	void	_ClientHolePunch(REAL32 rDeltatime); 
//	void	_ClientHolePunchRendezvous( REAL32 rDeltaTime );
	void	_ClientHolePunchRelay( REAL32 rDeltaTime );
	bool	_ClientHolePunchEach(INT32 Num, REAL32 rDeltatime); 

public:	
	void	ClientStart(INT32 Num);					//Ŭ���̾�Ʈ Ȧ��Ī ����	
	void	ClientGameLeave(INT32 Num);				//Ŭ���̾�Ʈ�� ������ �ߴ�	
	void	setServerState( UINT32 nState )									{ m_ServerState		= nState; }
	void	setServerUDPState( SERVER_UDP_STATE eState )					{ m_ServerUDPState	= eState; }

public:
	CUdpHolePuncherForClient();
	virtual ~CUdpHolePuncherForClient();

	void	OnCreate( void );
	void	OnDestroy();
	void	OnUpdate( REAL32 rDeltaTime );

	// P2P ���� Ȧ��Ī�Դϴ�.
	void	P2PHolePunchSend( UINT32 IP, UINT16 Port );
	void	P2PHolePunchEndSend( UINT32 IP, UINT16 Port ); 
	bool	P2PHolePunchRecv( UINT32 Ip, UINT16 Port, char * pPacket );

	//void	RendezvousHolePunchSend( UINT32 IP, UINT16 Port );
	//void	RendezvousHolePunchRecv( char* pPacket );

	void	RelayHolePunchSend( UINT32 IP, UINT16 Port );
	void	RelayHolePunchRecv( char* pPacket );
	void	RelayHolePunchEndSend( UINT32 IP, UINT16 Port );
};

#endif	// __CUDPHOLEPUNCHERFORCLIENT_H
