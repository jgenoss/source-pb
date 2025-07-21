#pragma once

#define MAXIMUM_ROOM_COUNT		300		// 300�� �̤����̵Ǹ� �÷����Ѵ�.
#define MAXIMUM_USER_COUNT		5000	// �Ѽ����� 5õ�����

class LobbyContext : public i3::shared_ginst<LobbyContext>
{
public:
	~LobbyContext();

	void			InitLobbyData();
	void			InitLobbyUserData();

	void			SetLobbyUserCount( INT32 nCount )		{ m_nLobbyUserCount = nCount;	}
	INT32			GetLobbyUserCount( ) const				{ return m_nLobbyUserCount;		}

	void			setLobbyRoomCount( INT32 nValue )		{ m_LobbyRoomCount = nValue;	}
	INT32			getLobbyRoomCount(void)	const			{ return m_LobbyRoomCount;		}
	
	INT32			getLobbyRoomPage( void ) const			{ return m_LobbyRoomPage;		}
	void			setLobbyRoomPage( INT32 nValue )		{ m_LobbyRoomPage = nValue;		}

	const LOBBY_USER_INFO_LIST*		getLobbyUserInfo( INT32 idx ) const; 
	const LOBBY_USER_INFO_LIST*		getLobbyUserInfoByNick(const char *) const;

	void			SetLobbyUserInfo(INT32 PageNum, INT32 InfoCount, const LOBBY_USER_INFO_LIST * pUserList);

	const ROOM_INFO_BASIC *	getRoomInfo( INT32 idx ) const;		
	void			SetRoomInfo( INT32 PageNum, INT32 InfoCount, const ROOM_INFO_BASIC * pRoomList);

	INT32 				getUserSessionIdxByUserNickName(const i3::wliteral_range& wstrNickName ) const;	// Ư�� ������ �г������� ���� �ε��� ��������. 2011-03-07 ���ؿ�
//	INT32 				getUserSessionIdxByUserNickName(const char* szNickName ) const;	// Ư�� ������ �г������� ���� �ε��� ��������. 2011-03-07 ���ؿ�

	INT32				getSelectedRoomIdx(void) const			{ return m_SelectedRoomIdx; }
	void				setSelectedRoomIdx( INT32 v)			{ m_SelectedRoomIdx = v; }
	const ROOM_INFO_ADD *	getSelectedRoomInfoAdd(void) const	{ return &m_SelectedRoomInfoAdd; }
	ROOM_INFO_ADD *			setSelectedRoomInfoAdd(void)		{ return &m_SelectedRoomInfoAdd; }

	const COUPON_INFO*	getCouponInfo( void) const			{ return &m_couponInfo; }
	COUPON_INFO *		setCouponInfo( void)				{ return &m_couponInfo; }
	void				clearCouponInfo(void)				{ i3mem::FillZero(&m_couponInfo, sizeof(m_couponInfo));	}

private:
	
	//Lobby ���� ����Ʈ	(C->G)	
	i3::vector<LOBBY_USER_INFO_LIST*> m_LobbyUserListInfo;
	INT32			m_nLobbyUserCount = 0;

	//Lobby ������		(C->G)	
	i3::vector<ROOM_INFO_BASIC*> m_RoomListInfo;
	INT32			m_LobbyRoomCount = 0;		//Lobby				(C->G)
	INT32			m_LobbyRoomPage = 0;		//Lobby				(G->C)
	INT32			m_LobbyRoomSortType = ROOM_SORT_DEFAULT;	//Lobby				// �������� ���δ�..
	

	INT32			m_SelectedRoomIdx = 0;						//�ֹ߼� ���� 
	ROOM_INFO_ADD	m_SelectedRoomInfoAdd;					//Ŭ���� ����	(C->G)		// ��������ü�� �״�� ���°� ����Ҽ� �����Ƿ�..
																						// ���߿� ������ ����ü�� �����ؾ��Ѵ�...

	COUPON_INFO		m_couponInfo;			// ��������

};

