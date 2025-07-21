#if !defined( __PROFILE_DEF_H__)
#define __PROFILE_DEF_H__

#define MAX_TCP_MESSAGE_COUNT		100

#if !defined( I3_NO_PROFILE )

namespace NETPROFILE
{
	struct info_t
	{
		INT32			m_nRecvCount = 0;				// �ʴ� ���� Ƚ��
		INT32			m_nRecvBytes = 0;				// �ʴ� ���� ��Ŷ��
		INT32			m_nSendCount = 0;				// �ʴ� ���� Ƚ��
		INT32			m_nSendBytes = 0;				// �ʴ� ���� ��Ŷ��
		REAL32			m_fCountTimer = 0.0f;				//
		REAL32			m_PacketRecvTime = 0.0f;			// ��Ŷ�� �޴� Ÿ��( Debug ���Դϴ�. �� �ð��� ��Ŷ�� ó���� )
		INT32			m_nInvalidIndex = 0;			// �߸��� �ε����� ���� ��� Ƚ��
		INT32			m_nWrongPacket = 0;				// �߸��� ��Ŷ�������� ��� Ƚ��
		INT32			m_nInvalidAddr = 0;				// �߸��� �ּҷκ��� ���� ��� Ƚ��
		INT32			m_nLostTime = 0;				// ������ ��Ŷ�� ���� ��� Ƚ��
		
		REAL32			m_fProfileTime = 0.0f;				// �� �÷���Ÿ��
		UINT32			m_nTotalRecvCount = 0;			// �� RecvCount
		UINT32			m_nTotalSendCount = 0;			// �� SendCount
		UINT32			m_nTotalRecvBytes = 0;			// �� RecvBytes
		UINT32			m_nTotalSendBytes = 0;
		UINT32			m_nRecvCountPerSec = 0;			// ��� �ʴ� RecvCount
		UINT32			m_nSendCountPerSec = 0;
		UINT32			m_nRecvBytesPerSec = 0;			// ��� �ʴ� RecvBytes
		UINT32			m_nSendBytesPerSec = 0;			// ��� �ʴ� SendBytes
		UINT32			m_nRecvBytesPerOne = 0;			// ��� �� ��Ŷ�� RecvBytes
		UINT32			m_nSendBytesPerOne = 0;			// ��� �� ��Ŷ�� SendBytes
		INT32			m_nRecvBytesMax = 0;			// �ִ� RecvBytes
		INT32			m_nRecvBytesMin = 0;			// �ּ� RecvBytes
		INT32			m_nSendBytesMax = 0;			// �ִ� SendBytes
		INT32			m_nSendBytesMin = 0;			// �ּ� SendBytes

		//For SERVER TCP 	
		PROTOCOL		m_nTcpRecvMessage[MAX_TCP_MESSAGE_COUNT] = { 0 };
		INT32			m_nTcpRecvIdx = 0;
		PROTOCOL		m_nTcpSendMessage[MAX_TCP_MESSAGE_COUNT] = { 0 };
		INT32			m_nTcpSendIdx = 0;
	};

	void	UDPReset(NETPROFILE::info_t* out_info);
	void	TCPReset(NETPROFILE::info_t* out_info);	
	void	Reset(NETPROFILE::info_t* out_info);
	
	void	DumpTcpProtocol(const NETPROFILE::info_t* in_info);

}



#endif

#endif
