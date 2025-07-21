#include "pch.h"
#include "UserLocationInfo.h"
#include "ServerInfo.h"



//////////////////////////////////////////////////////////////////////////

CUserLocationInfo::CUserLocationInfo()
{
	m_State = 0;
}

CUserLocationInfo::CUserLocationInfo(UINT64 FriendState)
{
	m_State = FriendState;
}

FRIEND_STATE CUserLocationInfo::GetState() const
{
	return static_cast<FRIEND_STATE>(GET_FRIEND_STATE(m_State));
}

UINT32 CUserLocationInfo::GetMatch() const
{
	return static_cast<UINT32>(GET_FRIEND_MATCH(m_State));
}

bool CUserLocationInfo::IsMatch() const
{
	return (GetMatch() != MATCH_TEAM_INVALID_IDX) ? true : false;
}

UINT32	CUserLocationInfo::GetServerNumber() const
{
	INT32	ServerIndex = static_cast<UINT32>(GET_FRIEND_SERVER(m_State));

	if ( g_pServerInfo->IsUseServerSelectPage() )
		ServerIndex -= 1; 

	// Ŭ���̾�Ʈ���� ����ϴ� ���Ӽ����ε����� 0base ����
	// �����ܿ����� ����Ʈ���� ������ ���� ����Ʈ���̰� 0�̰� ���Ӽ����� 1base�� �ϱ� ������
	// ����Ʈ���� ���ÿ� -1�� �Ѵ�.

	// I3ASSERT(ServerIndex >= 0);
	return ServerIndex;
}

UINT32	CUserLocationInfo::GetChannelNumber() const
{
	return static_cast<UINT32>(GET_FRIEND_CHANNEL(m_State));
}

UINT32	CUserLocationInfo::GetRoomNumber() const
{
	return static_cast<UINT32>(GET_FRIEND_ROOMIDX(m_State));
}

UINT32	CUserLocationInfo::GetDisplayServerNumber() const
{
	return GetServerNumber();
}

UINT32 CUserLocationInfo::GetDisplayChannelNumber() const
{
	return GetChannelNumber() + 1;
}

UINT32	CUserLocationInfo::GetDisplayRoomNumber() const
{
	return GetRoomNumber() + 1;
}

UINT32 CUserLocationInfo::GetDisplayServerChannelNumber() const
{
	return GetDisplayServerNumber() * 10 + GetDisplayChannelNumber();
}
