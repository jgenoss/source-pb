#include "pch.h"
#include "LobbyContext.h"

#include "ChannelContext.h"
#include "MyRoomInfoContext.h"
#include "RandomMapContext.h"

#include "i3Base/RemoveColorString.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#ifdef max
#undef max
#endif


LobbyContext::~LobbyContext()
{

	i3::cu::for_each_SafeFree_clear(m_RoomListInfo); 
	i3::cu::for_each_SafeFree_clear(m_LobbyUserListInfo);

}

void	LobbyContext::InitLobbyData()
{
	InitLobbyUserData();

	if (MyRoomInfoContext::i()->IsQuickJoin() == false)
	{
		ROOM_INFO_BASIC	roomInfo;
		roomInfo.Reset();
		MyRoomInfoContext::i()->setMyRoomInfo(&roomInfo);
	}

	MyRoomInfoContext::i()->ClearMyRoomPW();

	m_nLobbyUserCount	= 0;
	m_LobbyRoomCount	= 0;
	m_LobbyRoomPage		= 0;
	m_LobbyRoomSortType	= ROOM_SORT_DEFAULT;

	size_t RoomMax = m_RoomListInfo.size();
	for( size_t i = 0; i < RoomMax; i++)
	{
		ROOM_INFO_BASIC * pRoomInfo = m_RoomListInfo[i];
		pRoomInfo->Reset();
	}

	INT32 LobbyUserMax = (INT32)m_LobbyUserListInfo.size();
	for( INT32 i = 0; i < LobbyUserMax; i++)
	{
		LOBBY_USER_INFO_LIST * pUserInfo = m_LobbyUserListInfo[i];
		pUserInfo->Reset();
	}
	
	//RandomMapContext::i()->ResetRandom();
	RandomMapContext::i()->ClearStageList();	
}

void	LobbyContext::InitLobbyUserData()
{
	INT32 ChannelMaxUserCount = ChannelContext::i()->GetChannelMaxUser();

	if (ChannelMaxUserCount == std::numeric_limits<INT32>::max())
		ChannelMaxUserCount = 200;

	if (ChannelMaxUserCount > (INT32)m_RoomListInfo.size())
	{
		INT32 CurCount = (INT32)m_RoomListInfo.size();
		m_RoomListInfo.resize(ChannelMaxUserCount);
		for (INT32 i = CurCount; i < ChannelMaxUserCount; ++i)
		{
			m_RoomListInfo[i] = (ROOM_INFO_BASIC*)i3MemAlloc(sizeof(ROOM_INFO_BASIC));
		}
	}

	if (ChannelMaxUserCount > (INT32)m_LobbyUserListInfo.size())
	{
		INT32 CurCount = (INT32)m_LobbyUserListInfo.size();

		//
		m_LobbyUserListInfo.resize(ChannelMaxUserCount);					// ���⼭ ����� �����ȴ�... 
		for (INT32 i = CurCount; i < ChannelMaxUserCount; ++i)
		{
			m_LobbyUserListInfo[i] = (LOBBY_USER_INFO_LIST*)i3MemAlloc(sizeof(LOBBY_USER_INFO_LIST));
		}
		//
	}
}

const LOBBY_USER_INFO_LIST*	LobbyContext::getLobbyUserInfo( INT32 idx ) const
{
	return m_LobbyUserListInfo[idx];
	
}

const LOBBY_USER_INFO_LIST * LobbyContext::getLobbyUserInfoByNick(const char *pName) const
{
	LOBBY_USER_INFO_LIST * t_UserInfo;
	for(size_t i=0; m_LobbyUserListInfo.size(); i++)
	{
		t_UserInfo = m_LobbyUserListInfo[i];
		if( i3::generic_is_iequal(t_UserInfo->_Nick, pName) )
			return t_UserInfo;
	}
	return nullptr;
}


void LobbyContext::SetLobbyUserInfo( INT32 PageNum, INT32 InfoCount, const LOBBY_USER_INFO_LIST * pUserList )
{
	INT32 StartIndex = PageNum * SEND_MAX_LOBBY_USER_COUNT;
	for ( INT32 i = 0; i < InfoCount ; ++StartIndex, ++i )
	{
		LOBBY_USER_INFO_LIST* pUserInfo = m_LobbyUserListInfo[StartIndex];
			
		if ( ! pUserInfo)				// �̰� �˻��ؾߵǴ� ������ ����δ�..�̰ͺ��ٴ�... StartIndex�� ��ȿ�� �˻簡 �ʿ��Ұ�ó�� ���δ�.
			break;						// �ϴ� ���д�..(2013.08.12.����..)

		*pUserInfo = pUserList[i];
	}

	// ��Ÿ�÷��̾ ������ �̵� (keyword : Star Player)
	{
		struct sort_by_star_player
		{
			bool operator()(const LOBBY_USER_INFO_LIST * lhs, const LOBBY_USER_INFO_LIST * rhs)
			{
				return (check_rank(lhs->_Rank) && !check_rank(rhs->_Rank));
			}

			bool check_rank(const UINT8 rank)
			{
				return (rank >= RANK_56 && rank <= RANK_60);
			}

		};

		i3::sort(m_LobbyUserListInfo.begin(), m_LobbyUserListInfo.end(), sort_by_star_player());
	}
}

const ROOM_INFO_BASIC *	LobbyContext::getRoomInfo( INT32 idx ) const
{
	return m_RoomListInfo[idx];
}

void			LobbyContext::SetRoomInfo( INT32 PageNum, INT32 InfoCount, const ROOM_INFO_BASIC * pRoomList)
{
	INT32 StartIndex = PageNum * SEND_MAX_ROOM_COUNT;

	i3::stack_wstring result;

	for ( INT32 i = 0; i < InfoCount ; ++StartIndex, ++i )
	{
		ROOM_INFO_BASIC * pRoomInfo = m_RoomListInfo[StartIndex];
		if ( !pRoomInfo)			// �̰� �˻��ؾߵǴ� ������ ����δ�..�̰ͺ��ٴ�... StartIndex�� ��ȿ�� �˻簡 �ʿ��Ұ�ó�� ���δ�.
			break;					// �ϴ� ���д�..(2013.08.12.����..)

		*pRoomInfo = pRoomList[i];

		result.clear();
		::RemoveColorString(pRoomInfo->_Title,result);
		i3::safe_string_copy(pRoomInfo->_Title,result,_countof(pRoomInfo->_Title));
	}
}

// Ư�� ������ �г������� ���� �ε��� ��������. 2011-03-07 ���ؿ�
INT32 	LobbyContext::getUserSessionIdxByUserNickName(const i3::wliteral_range&  wstrNickName ) const
{
	INT32 i32SessionIndex = -1;
	
	const i3::wliteral_range& strNick = wstrNickName;	

	for ( INT32 i = 0 ; i < m_nLobbyUserCount ; i++ )
	{
		LOBBY_USER_INFO_LIST * pUserInfo = m_LobbyUserListInfo[i];
			
		if( i3::generic_is_equal(strNick, pUserInfo->_Nick))  
		{	
			i32SessionIndex = pUserInfo->_Session;
		}
	}

	return i32SessionIndex;
}

/*
INT32 	LobbyContext::getUserSessionIdxByUserNickName(const char* szNickName ) const	// Ư�� ������ �г������� ���� �ε��� ��������. 2011-03-07 ���ؿ�
{
	INT32 i32SessionIndex = -1;

	for ( INT32 i = 0 ; i < m_nLobbyUserCount ; i++ )
	{
		LOBBY_USER_INFO_LIST * pUserInfo = m_LobbyUserListInfo[i];

		if( i3::generic_is_equal(szNickName, pUserInfo->_Nick))  
		{	
			i32SessionIndex = pUserInfo->_Session;
		}
	}

	return i32SessionIndex;
}
*/