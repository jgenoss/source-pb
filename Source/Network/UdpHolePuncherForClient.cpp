#include "pch.h"
#include "UdpHolePuncherForClient.h"
#include "ClientTCPSocket.h"
#include "ClientUDPSocket.h"
#include "../GlobalVariables.h"

#include "../StageBattle/NetworkContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameNetworkManager.h"

I3_CLASS_INSTANCE( CUdpHolePuncherForClient);//, i3ElementBase );
/*
�ʵ� ������ ������ ������.
- Ȧ��Ī�� ���Ӽ������� ��Ŷ�� ������ �ִ°��� ������ Ŭ���̾�Ʈ ���Դϴ�.(��������)..(BATTLE START, CHECK HOLE )
- ������ �������� CHECK HOLE ��Ŷ�� ������ �ʽ��ϴ�. 
- ������ ó�������Ҷ�.. �ٸ� �κп��� �������� ���� ��Ŷ�� ������.
- 
*/
CUdpHolePuncherForClient::CUdpHolePuncherForClient()
{
	m_ServerUDPState = SERVER_UDP_STATE_NONE;
	m_ServerState = 0;
	i3mem::FillZero( m_UserCheckSendTimeOutTime, sizeof( m_UserCheckSendTimeOutTime));
	i3mem::FillZero( m_UserCheckSendTime, sizeof( m_UserCheckSendTimeOutTime));
	m_fJoinSendTime = 0.f;
	m_fJoinTimeOut = 0.f;
}

CUdpHolePuncherForClient::~CUdpHolePuncherForClient()
{
	OnDestroy();
}

void CUdpHolePuncherForClient::OnCreate( )
{
	m_ServerState		= UDPHOLE_STATE_NOTHING;

	for(INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		m_UserCheckSendTime[i] = 0.f;
		m_UserCheckSendTimeOutTime[i] = 0.f;
	}

	m_fJoinTimeOut		= 0.f;
	m_fJoinSendTime		= 0.f;
}

void CUdpHolePuncherForClient::OnDestroy()
{

}

void CUdpHolePuncherForClient::OnUpdate( REAL32 rDeltaTime )
{
	//�Ǽ� �ε��� Ÿ�� ������ ���� 
	if(rDeltaTime > 1.0f ) rDeltaTime = 1.0f; 

	switch( m_ServerUDPState )
	{
		//case SERVER_UDP_STATE_RENDEZVOUS:
		//	{
		//		// ������ ������ UDP �� ������.
		//		_ClientHolePunchRendezvous( rDeltaTime);
		//	}
		//	break;
		case SERVER_UDP_STATE_RELAY:
			{
				_ClientHolePunchRelay( rDeltaTime );
			}
			break;
		case SERVER_UDP_STATE_CLIENT:
			{
#ifndef JUST_RELAY_SERVER
				INT32 i;

				//P2P HOLEPUNCH
				if( BattleServerContext::i()->IsP2PHost() )
				{//�����϶�
					for( i = 0; i < USER_SLOT_COUNT; i++)
						_ClientHolePunchEach( i, rDeltaTime); 
				}else
				{//������ �ƴҶ�
					if( _ClientHolePunchEach( BattleSlotContext::i()->getMainSlotIdx() , rDeltaTime) )//Ȧ��Ī�� �Ϸ� �Ǿ���. 
					{
						GameEventSender::i()->SetEvent( EVENT_START_BATTLE ); I3TRACE("EVENT_START_BATTLE\n");
					}
				}
#endif
			}
			break;
	}
}


////////////////////////////////////////////////////////////////////////////////
/*
void	CUdpHolePuncherForClient::_ClientHolePunchRendezvous( REAL32 rDeltaTime )
{	
	switch( NetworkContext::i()->RendezvousState)
	{
	case UDP_STATE_DONE:
		break;	
	case UDP_STATE_SENDRECV			: // ��Ŷ�� �����鼭 ��ٸ�
		m_fJoinTimeOut		+= rDeltaTime; 
		m_fJoinSendTime		+= rDeltaTime; 

		if( m_fJoinSendTime > RENDEZVOUS_SEND_TIME )
		{
			m_fJoinSendTime =  0.0f;

			RendezvousHolePunchSend( NetworkContext::i()->RendezvousIP, NetworkContext::i()->RendezvousPort[0]);
		}

		if( m_fJoinTimeOut > CLIENT_RENDEZVOUS_TIME_OUT )
		{//Ÿ�� �ƿ� �Դϴ�. ���� ó���� �ؾ� �մϴ�. 
			I3TRACE("[CUdpHolePuncherForClient::_ClientHolePunchRendezvous] Time Out1 - idx\n"); 
			NetworkContext::i()->RendezvousState = UDP_STATE_NONE;
		}
		break;
	case UDP_STATE_RECV_OK			: //UDP_STATE_SENDRECV ���¿��� ��Ŷ�� ������ �̻��·� �´�. ��Ŷ�� ���� ����
		// ���� ������ ������ ������ ��� �����ٰ� �˷���
		GameEventSender::i()->SetEvent( EVENT_RENDEZVOUS_HOLEPUNCH );
		NetworkContext::i()->RendezvousState = UDP_STATE_DONE;

		I3TRACE("[CUdpHolePuncherForClient::_ClientHolePunchRendezvous] RECV_OK\n"); 
		break;
	default:
		{
			I3TRACE("[CUdpHolePuncherForClient::_ClientHolePunchRendezvous] HolePunching Error State : %d\n", NetworkContext::i()->RendezvousState);
		}
		break;
	}
}
*/

////////////////////////////////////////////////////////////////////////////////
void	CUdpHolePuncherForClient::_ClientHolePunchRelay( REAL32 rDeltaTime )
{	
	switch( NetworkContext::i()->relayServerState)
	{	
	case UDP_STATE_NONE:
	case UDP_STATE_DONE:
		break;
	case UDP_STATE_SENDRECV			: // ��Ŷ�� �����鼭 ��ٸ�
		{
			m_fJoinTimeOut		+= rDeltaTime; 
			m_fJoinSendTime		+= rDeltaTime; 

			if( m_fJoinSendTime > RELAY_SEND_TIME )
			{
				m_fJoinSendTime =  0.0f; 

				RelayHolePunchSend( NetworkContext::i()->relayServerIp, NetworkContext::i()->relayServerPort);
			}

			if( m_fJoinTimeOut > CLIENT_RELAY_TIME_OUT )
			{//Ÿ�� �ƿ� �Դϴ�. ���� ó���� �ؾ� �մϴ�. 
				I3PRINTLOG(I3LOG_NOTICE, "HolePunching Time Out Over 20 Second \n");

				// UDP�� �ȵȴ�..
				// ������ ���ư���.
				//INT32 nArg = 0;
				//GameEventSender::i()->SetEvent(EVENT_GIVEUP_BATTLE, &nArg );
			}
		}
		break;
	case UDP_STATE_RECV_OK			: //UDP_STATE_SENDRECV ���¿��� ��Ŷ�� ������ �̻��·� �´�. ��Ŷ�� ���� ����

		NetworkContext::i()->UdpHoleState[BattleSlotContext::i()->getMySlotIdx()] = UDP_STATE_DONE;
		NetworkContext::i()->relayServerState = UDP_STATE_DONE;

		//GameEventSender::i()->SetEvent( EVENT_HOLEPUNCH_OK );	//���� �߰��� 
		GameEventSender::i()->SetEvent( EVENT_START_BATTLE );	//���� �߰��� 
		break;
	default:
		I3PRINTLOG(I3LOG_NOTICE,"HolePunching Error State : %d", NetworkContext::i()->relayServerState);
		break;
	}
}
////////////////////////////////////////////////////////////////////////////////
bool CUdpHolePuncherForClient::_ClientHolePunchEach(INT32 Num, REAL32 rDeltaTime)
{	
	bool Rv = false; 
	switch( NetworkContext::i()->UdpHoleState[Num] )
	{	
	case UDP_STATE_SENDRECV			: // ��Ŷ�� �����鼭 ��ٸ�
		m_UserCheckSendTimeOutTime[Num] += rDeltaTime; 
		m_UserCheckSendTime[Num]		+= rDeltaTime; 

		if( m_UserCheckSendTime[Num] > UDP_SEND_TIME)
		{
			m_UserCheckSendTime[Num] =  0.0f; 

			UINT32 Ip;
			UINT16 Port;
			if( NetworkContext::i()->UdpHoleType[Num] == UDP_TYPE_REAL)
			{
				Ip		= NetworkContext::i()->NetInfo[Num].m_nRealIP; 
				Port	= NetworkContext::i()->NetInfo[Num].m_nRealPort; 
			}
			else
			{
				Ip		= NetworkContext::i()->NetInfo[Num].m_nLocalIP; 
				Port	= NetworkContext::i()->NetInfo[Num].m_nLocalPort; 
			}
			
			P2PHolePunchSend( Ip, Port); 
		}

		if( m_UserCheckSendTimeOutTime[Num] > CLIENT_UDP_TIME_OUT)
		{//Ÿ�� �ƿ� �Դϴ�. ���� ó���� �ؾ� �մϴ�. 
			I3TRACE("[CUdpHolePuncherForClient::_ClientHolePunchEach] Time Out1 - idx %d\n", Num); 
			NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_NONE;
		}
		break; 
	case UDP_STATE_RECVSEND			: // ��Ŷ�� ��ٸ��� ������ ���� 
		m_UserCheckSendTimeOutTime[Num] += rDeltaTime; 
		if( m_UserCheckSendTimeOutTime[Num] > CLIENT_UDP_TIME_OUT)
		{//Ÿ�� �ƿ� �Դϴ�. ���� ó���� �ؾ� �մϴ�. +
			I3TRACE("[CUdpHolePuncherForClient::_ClientHolePunchEach] Time Out1 - idx %d\n", Num); 
			NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_NONE; 
		}
		break; 
	case UDP_STATE_RECV_OK			: //UDP_STATE_SENDRECV ���¿��� ��Ŷ�� ������ �̻��·� �´�. ��Ŷ�� ���� ���� 
		m_UserCheckSendTimeOutTime[Num] += rDeltaTime; 
		m_UserCheckSendTime[Num]		+= rDeltaTime; 

		if( m_UserCheckSendTime[Num] > RECVOK_SEND_TIME)
		{
			m_UserCheckSendTime[Num] =  0.0f; 

			UINT32 Ip; 
			UINT16 Port; 			
			if( NetworkContext::i()->UdpHoleType[Num] == UDP_TYPE_REAL)
			{
				Ip		= NetworkContext::i()->NetInfo[Num].m_nRealIP; 
				Port	= NetworkContext::i()->NetInfo[Num].m_nRealPort; 
			}
			else
			{
				Ip		= NetworkContext::i()->NetInfo[Num].m_nLocalIP; 
				Port	= NetworkContext::i()->NetInfo[Num].m_nLocalPort; 
			}

			P2PHolePunchEndSend( Ip, Port );
		}

		//1�ʵ��� ������ �����̷� �̵� 
		if( m_UserCheckSendTimeOutTime[Num] > RECV_SEND_TIME_OUT)
		{
			NetworkContext::i()->UdpHoleState[BattleSlotContext::i()->getMySlotIdx()] = UDP_STATE_DONE;
			NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_DONE;
			Rv = true; 
		}
		break; 
	case UDP_STATE_ERROR_DIFFNAT	: 
		//���� ���� 
		NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_NONE;
		break; 
	case UDP_STATE_NONE				: break; 
	case UDP_STATE_DONE				: break; 
	}

	return Rv; 
}

void CUdpHolePuncherForClient::ClientStart(INT32 Num)
{
	//1. SetTime 
	m_UserCheckSendTime[ Num ]			= 0.0f; 
	m_UserCheckSendTimeOutTime[ Num ]	= 0.0f;

	if( BattleServerContext::i()->IsP2PHost() )
	{	//���� �����϶�.
		if( NetworkContext::i()->isRealIp() )
		{	//���� �����̸鼭 ��������� 
			NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_SENDRECV;
			NetworkContext::i()->UdpHoleType[Num] = UDP_TYPE_REAL;
		}
		else
		{	//���� �����̸鼭 NAT�ӿ� ���� 
			if( NetworkContext::i()->NetInfo[Num].m_nRealIP == NetworkContext::i()->NetInfo[Num].m_nLocalIP )
			{	//�ٸ����� ���������
				NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_SENDRECV;
				NetworkContext::i()->UdpHoleType[Num] = UDP_TYPE_REAL;
			}
			else if(  NetworkContext::i()->NetInfo[Num].m_nRealIP == NetworkContext::i()->getMyNetInfo()->m_nRealIP)
			{	//���� NAT
				NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_SENDRECV;
				NetworkContext::i()->UdpHoleType[Num] = UDP_TYPE_NAT;
			}
			else
			{	// ���� �� �� �ִ� ��� ����
				NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_ERROR_DIFFNAT;
				NetworkContext::i()->UdpHoleType[Num] =	UDP_TYPE_REAL;
			}
		}
	}
	else
	{	//���� ������ �ƴҶ�.
		INT32 MainIdx = BattleSlotContext::i()->getMainSlotIdx(); 

		if( NetworkContext::i()->NetInfo[MainIdx].m_nRealIP == NetworkContext::i()->NetInfo[MainIdx].m_nLocalIP )
		{	//������ ���������
			NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_SENDRECV;
			NetworkContext::i()->UdpHoleType[Num] =	UDP_TYPE_REAL;
		}		
		else
		{	//������ ��������� 
			if( NetworkContext::i()->isRealIp() )
			{	//���� ���������
				NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_SENDRECV;
				NetworkContext::i()->UdpHoleType[Num] =	UDP_TYPE_REAL;
			}
			else if( NetworkContext::i()->NetInfo[BattleSlotContext::i()->getMainSlotIdx()].m_nRealIP == NetworkContext::i()->getMyNetInfo()->m_nRealIP)
			{	//���� NAT
				NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_SENDRECV;
				NetworkContext::i()->UdpHoleType[Num] =	UDP_TYPE_NAT;
			}
			else
			{	// ���� �� �� �ִ� ��� ����
				NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_ERROR_DIFFNAT;
				NetworkContext::i()->UdpHoleType[Num] =	UDP_TYPE_REAL;
			}
		}
	}

	return; 
}

void CUdpHolePuncherForClient::ClientGameLeave(INT32 Num)
{
	NetworkContext::i()->UdpHoleState[Num] = UDP_STATE_NONE;
}

void CUdpHolePuncherForClient::P2PHolePunchSend( UINT32 IP, UINT16 Port )
{	
	N_GAME_HEADER head;
	char	SendBuffer[ 64 ];
	INT32	myRoomIndex	= MyRoomInfoContext::i()->getMyRoomIndex();

	ClientUDPSocket * pUDPSocket = CGameNetworkManager::i()->GetUDPSocket();
	
	::memset(&head, 0, sizeof(N_GAME_HEADER));

	head._ProtocolID	= UDPPROTOCOL_HOLEPUNCH;
	head._Index			= (UINT8)BattleSlotContext::i()->getMySlotIdx();
	head._GameTime		= 0.0f;
	head._RoundNum		= 0;
	head._Size			= sizeof(N_GAME_HEADER) + sizeof(INT32);
	
	head.CopyToPacket( SendBuffer );
	{
		INT32 * pTemp = (INT32*) &SendBuffer[ sizeof( N_GAME_HEADER ) ];
		*pTemp = myRoomIndex;
	}

	pUDPSocket->Send( SendBuffer, sizeof( N_GAME_HEADER ) + sizeof( INT32 ), IP, Port );

	return; 
}

void CUdpHolePuncherForClient::P2PHolePunchEndSend( UINT32 IP, UINT16 Port )
{
	N_GAME_HEADER head;
	char	SendBuffer[ 64 ];
	INT32	myRoomIdx = MyRoomInfoContext::i()->getMyRoomIndex();
	ClientUDPSocket * pUDPSocket = CGameNetworkManager::i()->GetUDPSocket();

	::memset(&head, 0, sizeof(N_GAME_HEADER));

	head._ProtocolID	= UDPPROTOCOL_HOLEPUNCH_END;
	head._Index			= (UINT8)BattleSlotContext::i()->getMySlotIdx();
	head._GameTime		= 0.0f;
	head._Size			= sizeof(N_GAME_HEADER) + sizeof(INT32);
	head.CopyToPacket( SendBuffer );
	{
		INT32 * pTemp = (INT32*) &SendBuffer[ sizeof( N_GAME_HEADER ) ];
		*pTemp = myRoomIdx;
	}

	pUDPSocket->Send( SendBuffer, sizeof( N_GAME_HEADER ) + sizeof( INT32 ), IP, Port );

	return; 
}

bool CUdpHolePuncherForClient::P2PHolePunchRecv( UINT32 Ip, UINT16 Port, char * pPacket )
{	
	N_GAME_HEADER	*pHead;
	UINT32			SlotIdx; 
	INT32			RoomIdx;
	
	pHead = (N_GAME_HEADER*) pPacket;
	i3mem::Copy( &RoomIdx,	&pPacket[sizeof(N_GAME_HEADER)],	sizeof(INT32) ); 
	SlotIdx = (UINT32) pHead->_Index;

	//1. Check Rooom Idx
	if( MyRoomInfoContext::i()->getMyRoomIndex() != RoomIdx )
	{
        return false; 
	}

	if(SlotIdx > (SLOT_MAX_COUNT - 1))
	{
		return false; 
	}
	//
	switch( NetworkContext::i()->UdpHoleState[SlotIdx] )
	{	
	case UDP_STATE_SENDRECV		:	
		{			
			NetworkContext::i()->UdpHoleState[SlotIdx] = UDP_STATE_RECV_OK; //
			m_UserCheckSendTimeOutTime[SlotIdx] = 0.0f; 
			m_UserCheckSendTime[SlotIdx]		= 0.0f; 

			NetworkContext::i()->NetInfo[SlotIdx].m_nRealIP		= Ip; 
			NetworkContext::i()->NetInfo[SlotIdx].m_nRealPort	= Port; 
			NetworkContext::i()->NetInfo[SlotIdx].m_nLocalIP	= Ip; 
			NetworkContext::i()->NetInfo[SlotIdx].m_nLocalPort	= Port; 
		}
		break; 
	case UDP_STATE_RECVSEND		:		
		//���� ���������� UDPPROTOCOL_HOLEPUNCH_END�̰Ÿ� �Ϸ� 
		{
			if( pHead->_ProtocolID == UDPPROTOCOL_HOLEPUNCH_END)
			{
				NetworkContext::i()->UdpHoleState[BattleSlotContext::i()->getMySlotIdx()] =	UDP_STATE_DONE;
				NetworkContext::i()->UdpHoleState[SlotIdx] = UDP_STATE_DONE;
				NetworkContext::i()->NetInfo[SlotIdx].m_nRealIP		= Ip; 
				NetworkContext::i()->NetInfo[SlotIdx].m_nRealPort	= Port; 
				NetworkContext::i()->NetInfo[SlotIdx].m_nLocalIP	= Ip; 
				NetworkContext::i()->NetInfo[SlotIdx].m_nLocalPort	= Port; 

				if( !BattleServerContext::i()->IsP2PHost() )//������ �ƴҶ��� ������..== �ڱ� �ڽ��϶���.
				{
					GameEventSender::i()->SetEvent( EVENT_HOLEPUNCH_OK );	//���� �߰��� 
				}
			}
			else if( pHead->_ProtocolID == UDPPROTOCOL_HOLEPUNCH )
			{
				//Send //UDPPROTOCOL_HOLEPUNCH_END �� ���� 
				P2PHolePunchEndSend( Ip, Port ); 
			}
		}
		break; 
	case UDP_STATE_DONE		: 
		break; 
	default: 
		break; 
	}

	return true;
}

// Relay 
void CUdpHolePuncherForClient::RelayHolePunchSend( UINT32 IP, UINT16 Port )
{
	N_GAME_HEADER head;
	char	SendBuffer[ 64 ];
	N_GAME_RELAY_HOLEPUNCH RelayHolePunch;
	ClientUDPSocket * pUDPSocket = CGameNetworkManager::i()->GetUDPSocket();

	::memset(&head, 0, sizeof(N_GAME_HEADER));

	head._ProtocolID	= UDPPROTOCOL_SERVER_JOIN_GROUP_REQ;
	head._Index			= (UINT8)BattleSlotContext::i()->getMySlotIdx();
	head._GameTime		= 0.0f;
	head._Size			= sizeof(N_GAME_HEADER) + sizeof( N_GAME_RELAY_HOLEPUNCH );
	head._ui8UID		= NetworkContext::i()->UID%256;

	head.CopyToPacket( SendBuffer );

	RelayHolePunch.wGroupIdx	= NetworkContext::i()->relayGroupId;
	RelayHolePunch.wEncryptKey	= NetworkContext::i()->RelayEncryptKey;
	RelayHolePunch.cSlotIdx		= (UINT8)BattleSlotContext::i()->getMySlotIdx();	

	RelayHolePunch.CopyToPacket( &(SendBuffer[ sizeof(N_GAME_HEADER) ]) );

	pUDPSocket->Send( SendBuffer, (sizeof( N_GAME_HEADER ) + sizeof( N_GAME_RELAY_HOLEPUNCH )), IP, Port );

	return;
}

void CUdpHolePuncherForClient::RelayHolePunchEndSend( UINT32 IP, UINT16 Port )
{
	N_GAME_HEADER head;
	char	SendBuffer[ 64 ];
	N_GAME_RELAY_HOLEPUNCH RelayHolePunch;
	ClientUDPSocket * pUDPSocket = CGameNetworkManager::i()->GetUDPSocket();

	::memset(&head, 0, sizeof(N_GAME_HEADER));

	head._ProtocolID	= UDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ;
	head._Index			= (UINT8)BattleSlotContext::i()->getMySlotIdx();
	head._GameTime		= 0.0f;
	head._Size			= sizeof(N_GAME_HEADER) + sizeof( N_GAME_RELAY_HOLEPUNCH );

	head.CopyToPacket( SendBuffer );

	RelayHolePunch.wGroupIdx	= NetworkContext::i()->relayGroupId;
	RelayHolePunch.wEncryptKey	= NetworkContext::i()->RelayEncryptKey;
	RelayHolePunch.cSlotIdx		= (UINT8)BattleSlotContext::i()->getMySlotIdx();	

	RelayHolePunch.CopyToPacket( &(SendBuffer[ sizeof(N_GAME_HEADER) ]) );

	pUDPSocket->Send( SendBuffer, (sizeof( N_GAME_HEADER ) + sizeof( N_GAME_RELAY_HOLEPUNCH )), IP, Port );

	return;
}

void	CUdpHolePuncherForClient::RelayHolePunchRecv( char* pPacket )
{
	switch( NetworkContext::i()->relayServerState)
	{	
	case UDP_STATE_SENDRECV	:
		NetworkContext::i()->relayServerState = UDP_STATE_RECV_OK;
		m_fJoinTimeOut = 0.f;
		break;
	}
}